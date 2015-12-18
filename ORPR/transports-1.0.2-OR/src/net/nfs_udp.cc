#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/socket.h>

#define log(a...) do { } while (0)
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)

#include "peerreview/transport/net/nfs_udp.h"

static char *ipdisp(in_addr_t iaddr, char *buf)
{
  sprintf(buf, "%d.%d.%d.%d", 
    (int)(iaddr&0xFF), (int)((iaddr>>8)&0xFF), 
    (int)((iaddr>>16)&0xFF), (int)(iaddr>>24)
  );
  
  return buf;
}

NfsUdpTransport::NfsUdpTransport(in_addr_t localAddress)
{
  this->localAddress = localAddress;
  this->numTimers = 0;
  this->dataSocket = -1;
  this->controlSocket = -1;
  this->now = getTimeMicros();
  this->logfile = NULL;
  this->numDescriptors = 0;
  this->lastSendIdAssigned = 0;
  this->lastSendIdCompleted = 0;
}

void NfsUdpTransport::openLog(const char *logname)
{
  logfile = fopen(logname, "w+");
}

void NfsUdpTransport::setDescriptorCallback(int desc, DescriptorCallback *callback, bool wantRead, bool wantWrite, bool wantExcept)
{
  unsetDescriptorCallback(desc);
  
  if (numDescriptors >= MAX_DESCRIPTORS)
    panic("Too many descriptors");
  
  descriptor[numDescriptors].desc = desc;
  descriptor[numDescriptors].callback = callback;
  descriptor[numDescriptors].wantRead = wantRead;
  descriptor[numDescriptors].wantWrite = wantWrite;
  descriptor[numDescriptors].wantExcept = wantExcept;
  numDescriptors ++;
}

void NfsUdpTransport::unsetDescriptorCallback(int desc)
{
  for (int i=0; i<numDescriptors; i++) {
    if (descriptor[i].desc == desc) {
      descriptor[i] = descriptor[--numDescriptors];
      break;
    }
  }
}

void NfsUdpTransport::dump(const char *subsystem, int level, const unsigned char *payload, int len)
{
  int i, off = 0;
  char linebuf[400];
  
  while (off < len) {
    sprintf(linebuf, "%04X   ", off);
    for (i=0; i<16; i++) {
      if ((i+off) < len)
        sprintf(&linebuf[strlen(linebuf)], "%02X ", payload[i+off]);
      else
        strcat(linebuf, "   ");
    }
    
    strcat(linebuf, "   ");
    
    for (i=0; i<16; i++) {
      if ((i+off) < len)
        sprintf(&linebuf[strlen(linebuf)], "%c", isprint(payload[i+off]) ? payload[i+off] : '.');
    }
    
    off += 16;
    logText(subsystem, level, "%s", linebuf);
  }
}

void NfsUdpTransport::logText(const char *subsystem, int level, const char *format, ...)
{
  char buffer[1024];
  va_list ap;
  va_start (ap, format);
  vsprintf (buffer, format, ap);
  if (logfile) {
    char indent[100];
    if (level == 0) 
      strcpy(indent, "");
    else if (level == 1)
      strcpy(indent, "=== ");
    else if (level == 2)
      strcpy(indent, "  ");
    else if (level == 3)
      strcpy(indent, "    ");
    else if (level == 4)
      strcpy(indent, "      ");
    else
      panic("Level=%d", level);
      
    fprintf(logfile, "%lld %s%s\n", now, indent, buffer);
    fflush(logfile);
  }
  va_end (ap);
}

long long NfsUdpTransport::getTimeMicros()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000LL + tv.tv_usec;
}

void NfsUdpTransport::shutdown()
{
  if (dataSocket >= 0) {
    close(dataSocket);
    dataSocket = 0;
  }

  if (controlSocket >= 0) {
    close(controlSocket);
    controlSocket = 0;
  }
}

int NfsUdpTransport::openUdpServerSocket(in_addr_t addr, int port)
{
  char buf1[200];

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    panic("Cannot open UDP socket");
    
  sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = addr;
  if (bind(sock, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
    panic("Cannot bind to UDP port %s:%d", ipdisp(addr, buf1), port);

  return sock;
}

long long NfsUdpTransport::send(in_addr_t target, bool datagram, unsigned char *message, int msglen)
{
  char buf1[200];
  assert(msglen < 65536);

  log("Accepted packet type %s for delivery to %s (%d bytes)", datagram ? "DGRAM" : "MSG", ipdisp(target, buf1), msglen);
  //dump(4, message, msglen);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(datagram ? PORT_CONTROL : PORT_DATA);
  addr.sin_addr.s_addr = target;
  int ret = sendto(datagram ? controlSocket : dataSocket, message, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, sizeof(addr));
  
  if (ret < msglen) 
    panic("Cannot send via UDP (%s)", strerror(errno));
    
  return ++lastSendIdAssigned;
}

void NfsUdpTransport::scheduleTimer(TimerCallback *callback, int timerID, long long when)
{
  for (int i=0; i<numTimers; i++) {
    if ((timer[i].id == timerID) && (timer[i].callback == callback)) {
      timer[i].time = when;
      return;
    }
  }
  
  if (numTimers >= MAX_TIMERS)
    panic("Too many timers scheduled");
    
  timer[numTimers].time = when;
  timer[numTimers].id = timerID;
  timer[numTimers].callback = callback;
  numTimers ++;
}

void NfsUdpTransport::cancelTimer(TimerCallback *callback, int timerID)
{
  for (int i=0; i<numTimers; ) {
    if ((timer[i].id == timerID) && (timer[i].callback == callback)) 
      timer[i] = timer[--numTimers];
    else
      i++;
  }
}

void NfsUdpTransport::setCallback(TransportCallback *callback)
{
  this->callback = callback;
}

void NfsUdpTransport::run()
{
  char buf1[200], buf2[200];
  
  dataSocket = openUdpServerSocket(localAddress, PORT_DATA);
  controlSocket = openUdpServerSocket(localAddress, PORT_CONTROL);
  log("Running service on %s:%d/udp and %s:%d/udp", ipdisp(localAddress, buf1), PORT_DATA, ipdisp(localAddress, buf2), PORT_CONTROL);

  while (1) {
  
    /* Find out which connections need writing. We look at all the packets
       and mark the corresponding connection. */
    
    int maxFD = (dataSocket>controlSocket) ? dataSocket : controlSocket;
    fd_set fdr;
    fd_set fdw;
    fd_set fdx;
    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_ZERO(&fdx);
    FD_SET(dataSocket, &fdr);
    FD_SET(controlSocket, &fdr);
    
    for (int i=0; i<numDescriptors; i++) {
      if (descriptor[i].desc > maxFD)
        maxFD = descriptor[i].desc;
        
      if (descriptor[i].wantRead)
        FD_SET(descriptor[i].desc, &fdr);
      if (descriptor[i].wantWrite)
        FD_SET(descriptor[i].desc, &fdw);
      if (descriptor[i].wantExcept)
        FD_SET(descriptor[i].desc, &fdx);
    }
        
    /* Find the earliest timeout, and create a timeval structure
       for use with select() */
    
    struct timeval tv;
    struct timeval *to = NULL;
    if (numTimers > 0) {
      long long earliestTimeout = timer[0].time;
      for (int i=1; i<numTimers; i++) {
        if (timer[i].time < earliestTimeout)
          earliestTimeout = timer[i].time;
      }
      
      long long realNow = getTimeMicros();
      long long wait = (earliestTimeout<realNow) ? 0 : (earliestTimeout - realNow);
      to = &tv;
      tv.tv_sec = (int)(wait/1000000);
      tv.tv_usec = (int)(wait%1000000);
    }
    
    /* Wait until something interesting happens */
    
    int ret = select(maxFD + 1, &fdr, &fdw, &fdx, to);
    if ((ret<0) && (errno != EINTR))
      panic("Select() failed; ret=%d (%s)", ret, strerror(errno));
      
    /* Deliver any pending timeouts. Note that it is essential that we
       deliver the timeouts in a deterministic order; here we order them
       by ID, in ascending order */
    
    long long realNow = getTimeMicros();
    bool madeProgress = true;
    while (madeProgress) {
      madeProgress = false;
      int best = -1;
      for (int i=0; i<numTimers; i++) {
        if ((timer[i].time <= realNow) && ((best<0) || (timer[i].time<timer[best].time) || ((timer[i].time==timer[best].time) && (timer[i].id<timer[best].id))))
          best = i;
      }
      
      if (best >= 0) {
        assert(timer[best].time >= now);
        now = timer[best].time;
        int id = timer[best].id;
        TimerCallback *callback = timer[best].callback;
        timer[best] = timer[--numTimers];
        log("Timer %d", id);
        callback->timerExpired(id);
        madeProgress = true;
      }
    }
    
    now = realNow;
    
    /* Receive incoming UDP packets */          
        
    if (FD_ISSET(controlSocket, &fdr)) {
      struct sockaddr_in from;
      socklen_t fromlen = sizeof(from);
      unsigned char buffer[65536];
      
      int len = recvfrom(controlSocket, &buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromlen);
      if (len < 0)
        panic("Cannot read from UDP control socket (%s)", strerror(errno));
      
      log("Received control message from %s (%d bytes)", ipdisp(from.sin_addr.s_addr, buf1), len);
      callback->recv(from.sin_addr.s_addr, true, buffer, len);
    }

    if (FD_ISSET(dataSocket, &fdr)) {
      struct sockaddr_in from;
      socklen_t fromlen = sizeof(from);
      unsigned char buffer[65536];
      
      int len = recvfrom(dataSocket, &buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromlen);
      if (len < 0)
        panic("Cannot read from UDP data socket (%s)", strerror(errno));
      
      log("Received data message from %s (%d bytes)", ipdisp(from.sin_addr.s_addr, buf1), len);
      callback->recv(from.sin_addr.s_addr, false, buffer, len);
    }
    
    /* Handle callbacks, if any */
    
    for (int i=0; i<numDescriptors; i++) {
      bool reportRead = descriptor[i].wantRead && FD_ISSET(descriptor[i].desc, &fdr);
      bool reportWrite = descriptor[i].wantWrite && FD_ISSET(descriptor[i].desc, &fdw);
      bool reportExcept = descriptor[i].wantExcept && FD_ISSET(descriptor[i].desc, &fdx);
      
      if (reportRead || reportWrite || reportExcept)
        descriptor[i].callback->descriptorReady(descriptor[i].desc, reportRead, reportWrite, reportExcept);
    }
    
    /* UDP transmissions complete immediately (we don't guarantee that they have reached the other side) */

    while (lastSendIdCompleted < lastSendIdAssigned)
      callback->sendComplete(++lastSendIdCompleted);
  }
}

char *dumpFhandle(unsigned char *fhandle, char *buf)
{
  if ((fhandle[0] == 1) && (fhandle[1] == 0)) {
    switch (fhandle[2]) {
      case 0 :
        sprintf(buf, "<dev%d.%d ino%d file%d>", ntohs(*(short*)&fhandle[4]),
          ntohs(*(short*)&fhandle[6]), *(int*)&fhandle[8], *(int*)&fhandle[12]
        );
        break;
      default :
        sprintf(buf, "<?fhandle_type%d?>", fhandle[2]);
        break;
    }
  } else {
    sprintf(buf, "<ino%d.%08X>", *(unsigned int*)&fhandle[0], *(unsigned int*)&fhandle[4]);
  }

  return buf;
}

char *dumpName(unsigned char *msg, char *buf)
{
  int len = ntohl(*(int*)msg);
  assert(0<=len);
  buf[0] = '[';
  strncpy(&buf[1], (const char*)&msg[4], len);
  buf[len+1] = ']';
  buf[len+2] = 0;
  return buf;
}

char *dumpStat(int code, char *buf)
{
  switch (code) {
    case 0 :
      sprintf(buf, "NFS_OK");
      break;
    case 1 :
      sprintf(buf, "NFSERR_PERM");
      break;
    case 2 :
      sprintf(buf, "NFSERR_NOENT");
      break;
    case 5 :
      sprintf(buf, "NFSERR_IO");
      break;
    case 6 :
      sprintf(buf, "NFSERR_NXIO");
      break;
    case 13 :
      sprintf(buf, "NFSERR_ACCES");
      break;
    case 17 :
      sprintf(buf, "NFSERR_EXIST");
      break;
    case 19 :
      sprintf(buf, "NFSERR_NODEV");
      break;
    case 20 :
      sprintf(buf, "NFSERR_NOTDIR");
      break;
    case 21 :
      sprintf(buf, "NFSERR_ISDIR");
      break;
    case 27 :
      sprintf(buf, "NFSERR_FBIG");
      break;
    case 28 :
      sprintf(buf, "NFSERR_NOSPC");
      break;
    case 30 :
      sprintf(buf, "NFSERR_ROFS");
      break;
    case 63 :
      sprintf(buf, "NFSERR_NAMETOOLONG");
      break;
    case 66 :
      sprintf(buf, "NFSERR_NOTEMPTY");
      break;
    case 69 :
      sprintf(buf, "NFSERR_DQUOT");
      break;
    case 70 :
      sprintf(buf, "NFSERR_STALE");
      break;
    case 99 :
      sprintf(buf, "NFSERR_WFLUSH");
      break;
    default :
      sprintf(buf, "#UNKNOWN_ERR:%d#", code);
      break;
  }
  
  return buf;
     
}

char *dumpStatfsres(unsigned char *msg, char *buf)
{
  if ((*(int*)msg) != 0) {
    char buf2[100];
    sprintf(buf, "<FAIL %s>", dumpStat(ntohl(*(int*)msg), buf2));
    return buf;
  }
  
  int tsize = ntohl(*(int*)&msg[4]);
  int bsize = ntohl(*(int*)&msg[8]);
  int blocks = ntohl(*(int*)&msg[12]);
  int bfree = ntohl(*(int*)&msg[16]);
  int bavail = ntohl(*(int*)&msg[20]);
  
  sprintf(buf, "<tsize=%d bsize=%d blocks=%d bfree=%d bavail=%d>",
    tsize, bsize, blocks, bfree, bavail
  );
  
  return buf;
}

char *dumpFattr(unsigned char *msg, char *buf)
{
  const char *stype[] = { "non", "reg", "dir", "blk", "chr", "lnk" };
  int type = ntohl(*(int*)&msg[0]);
  assert((0<=type) && (type<=5));
  int mode = ntohl(*(int*)&msg[4]);
  int nlink = ntohl(*(int*)&msg[8]);
  int uid = ntohl(*(int*)&msg[12]);
  int gid = ntohl(*(int*)&msg[16]);
  int size = ntohl(*(int*)&msg[20]);
  int blocksize = ntohl(*(int*)&msg[24]);
  int rdev = ntohl(*(int*)&msg[28]);
  int blocks = ntohl(*(int*)&msg[32]);
  int fsid = *(int*)&msg[36];
  int fileid = ntohl(*(int*)&msg[40]);
  int atime = ntohl(*(int*)&msg[44]);
  int mtime = ntohl(*(int*)&msg[48]);
  int ctime = ntohl(*(int*)&msg[52]);
  
  sprintf(buf, "<%s %4o %dlnk %d:%d %d/%dx%d rdev=%d fsid=%Xh fileid=%d atime=%d mtime=%d ctime=%d>",
    stype[type], mode, nlink, uid, gid, size, blocks, blocksize, rdev,
    fsid, fileid, atime, mtime, ctime
  );

  return buf;
}

char *dumpSattr(unsigned char *msg, char *buf)
{
  int mode = ntohl(*(int*)&msg[0]);
  int uid = ntohl(*(int*)&msg[4]);
  int gid = ntohl(*(int*)&msg[8]);
  int size = ntohl(*(int*)&msg[12]);
  int atime = ntohl(*(int*)&msg[16]);
  int mtime = ntohl(*(int*)&msg[24]);
  
  sprintf(buf, "<%4o, %d:%d, %d bytes, atime=%d, mtime=%d>",
    mode, uid, gid, size, atime, mtime
  );

  return buf;
}

char *dumpDiropres(unsigned char *msg, char *buf)
{
  if ((*(int*)msg) != 0) {
    char buf2[100];
    sprintf(buf, "<FAIL %s>", dumpStat(ntohl(*(int*)msg), buf2));
    return buf;
  }
  
  char buf1[200], buf2[200];
  sprintf(buf, "<fhandle=%s fattr=%s>", dumpFhandle(&msg[4], buf1), dumpFattr(&msg[36], buf2));
  return buf;
}

char *dumpCreateargs(unsigned char *msg, char *buf)
{
  char buf1[200], buf2[200], buf3[200];
  int nelLen = ntohl(*(int*)&msg[32]);
  while (nelLen%4) 
    nelLen ++;
  sprintf(buf, "<fhandle=%s name=%s sattr=%s>", dumpFhandle(&msg[0], buf1), dumpName(&msg[32], buf2), dumpSattr(&msg[36+nelLen], buf3));
  return buf;
}

char *dumpAttrstat(unsigned char *msg, char *buf)
{
  if ((*(int*)msg) != 0) {
    char buf2[100];
    sprintf(buf, "<FAIL %s>", dumpStat(ntohl(*(int*)msg), buf2));
    return buf;
  }
  
  dumpFattr(&msg[4], buf);
  return buf;
}

int NfsUdpTransport::dumpMessage(const char *subsystem, int loglevel, bool isMountd, bool isResponse, int cookie, unsigned char *event, int size)
{
  char buf1[200], buf2[200];

  if (isMountd && !isResponse) {
    logText(subsystem, loglevel, "     MOUNTD.REQ #%d", ntohl(*(int*)&event[20]));
    return ntohl(*(int*)&event[20]);
  }
  
  if (isMountd && isResponse) {
    logText(subsystem, loglevel, "     MOUNTD.RESP #%d", cookie);
    return 0;
  }

  char obuf[200];
  if (!isMountd && isResponse) {
    logText(subsystem, loglevel, "     RPC[id=%08X]", *(int*)&event[0]);
    strcpy(obuf, "     NFSD.RSP[");
    switch (cookie) {
      case 0 :
        sprintf(&obuf[strlen(obuf)], "NULL()");
        break;
      case 1 :
        sprintf(&obuf[strlen(obuf)], "GETATTR(attrstat=%s)", dumpAttrstat(&event[24], buf1));
        break;
      case 2 :
        sprintf(&obuf[strlen(obuf)], "SETATTR(attrstat=%s)", dumpAttrstat(&event[24], buf1));
        break;
      case 3 :
        sprintf(&obuf[strlen(obuf)], "ROOT()");
        break;
      case 4 :
        sprintf(&obuf[strlen(obuf)], "LOOKUP(diropres=%s)", dumpDiropres(&event[24], buf1));
        break;
      case 5 :
        sprintf(&obuf[strlen(obuf)], "READLINK(xxx)");
        break;
      case 6 :
        sprintf(&obuf[strlen(obuf)], "READ(xxx)");
        break;
      case 7 :
        sprintf(&obuf[strlen(obuf)], "WRITECACHE(xxx)");
        break;
      case 8 :
        sprintf(&obuf[strlen(obuf)], "WRITE(attrstat=%s)", dumpAttrstat(&event[24], buf1));
        break;
      case 9 :
        sprintf(&obuf[strlen(obuf)], "CREATE(diropres=%s)", dumpDiropres(&event[24], buf1));
        break;
      case 10 :
        sprintf(&obuf[strlen(obuf)], "REMOVE(xxx)");
        break;
      case 11 :
        sprintf(&obuf[strlen(obuf)], "RENAME(xxx)");
        break;
      case 12 :
        sprintf(&obuf[strlen(obuf)], "LINK(xxx)");
        break;
      case 13 :
        sprintf(&obuf[strlen(obuf)], "SYMLINK(xxx)");
        break;
      case 14 :
        sprintf(&obuf[strlen(obuf)], "MKDIR(diropres=%s)", dumpDiropres(&event[24], buf1));
        break;
      case 15 :
        sprintf(&obuf[strlen(obuf)], "RMDIR(xxx)");
        break;
      case 16 :
        sprintf(&obuf[strlen(obuf)], "READDIR(xxx)");
        break;
      case 17 :
        sprintf(&obuf[strlen(obuf)], "STATFS(statfsres=%s)", dumpStatfsres(&event[24], buf1));
        break;
      default :
        sprintf(&obuf[strlen(obuf)], "UNKNOWN#%d(xxx)", ntohl(*(int*)&event[20]));
    }
    strcat(obuf, "]");
    logText(subsystem, loglevel, obuf);
    return 0;
  }
  
  logText(subsystem, loglevel, "     RPC[id=%08X rpcv%d prog=%dv%d]", *(int*)&event[0], ntohl(*(int*)&event[8]), ntohl(*(int*)&event[12]), ntohl(*(int*)&event[16]));
  assert(!cookie);
  strcpy(obuf, "     AUTH[");
  int msgstart = 28;
  if (ntohl(*(int*)&event[24]) == 1) {
    msgstart = 32 + ntohl(*(int*)&event[28]);  
    sprintf(&obuf[strlen(obuf)], "Unix: id=%d machine=[", ntohl(*(int*)&event[32]));
    for (int i=0; i<(int)ntohl(*(int*)&event[36]); i++)
      sprintf(&obuf[strlen(obuf)], "%c", event[40+i]);
    int pos = 40+ntohl(*(int*)&event[36]);
    while (pos%4) pos++;
    sprintf(&obuf[strlen(obuf)], "] uid=%d gid=%d gids=[", ntohl(*(int*)&event[pos]), ntohl(*(int*)&event[pos+4]));
    pos += 8;
    for (int i=0; i<(int)ntohl(*(int*)&event[pos]); i++)
      sprintf(&obuf[strlen(obuf)], "%s%d", (i==0) ? "" : " ", ntohl(*(int*)&event[pos+4+4*i]));
    strcat(obuf, "]]");
  } else if (ntohl(*(int*)&event[24]) == 0) {
    strcat(obuf, "NULL]");
  } else {
    panic("Unknown auth: %d", ntohl(*(int*)&event[24]));
  }
  
  logText(subsystem, loglevel, obuf);
msgstart += 8;
//  if (msgstart%16)
//    msgstart+=16-(msgstart%16);
  
  strcpy(obuf, "     NFSD.REQ[");
  int code = ntohl(*(int*)&event[20]);
  switch (code) {
    case 0 :
      sprintf(&obuf[strlen(obuf)], "NULL()");
      break;
    case 1 :
      sprintf(&obuf[strlen(obuf)], "GETATTR(fhandle=%s)", dumpFhandle(&event[msgstart], buf1));
      break;
    case 2 :
      sprintf(&obuf[strlen(obuf)], "SETATTR(fhandle=%s, sattr=%s)", dumpFhandle(&event[msgstart], buf1), dumpSattr(&event[msgstart+32], buf2));
      break;
    case 3 :
      sprintf(&obuf[strlen(obuf)], "ROOT()");
      break;
    case 4 :
      sprintf(&obuf[strlen(obuf)], "LOOKUP(fhandle=%s, name=%s)", dumpFhandle(&event[msgstart], buf1), dumpName(&event[msgstart+32], buf2));
      break;
    case 5 :
      sprintf(&obuf[strlen(obuf)], "READLINK(fhandle=%s)", dumpFhandle(&event[msgstart], buf1));
      break;
    case 6 :
      sprintf(&obuf[strlen(obuf)], "READ(fhandle=%s, begin=%d, offset=%d, total=%d)", dumpFhandle(&event[msgstart], buf1), ntohl(*(int*)&event[msgstart+32]), ntohl(*(int*)&event[msgstart+36]), ntohl(*(int*)&event[msgstart+40]));
      break;
    case 7 :
      sprintf(&obuf[strlen(obuf)], "WRITECACHE()");
      break;
    case 8 :
      sprintf(&obuf[strlen(obuf)], "WRITE(fhandle=%s, begin=%d, offset=%d, total=%d)", dumpFhandle(&event[msgstart], buf1), ntohl(*(int*)&event[msgstart+32]), ntohl(*(int*)&event[msgstart+36]), ntohl(*(int*)&event[msgstart+40]));
      break;
    case 9 :
      sprintf(&obuf[strlen(obuf)], "CREATE(%s)", dumpCreateargs(&event[msgstart], buf1));
      break;
    case 10 :
      sprintf(&obuf[strlen(obuf)], "REMOVE(xxx)");
      break;
    case 11 :
      sprintf(&obuf[strlen(obuf)], "RENAME(xxx)");
      break;
    case 12 :
      sprintf(&obuf[strlen(obuf)], "LINK(xxx)");
      break;
    case 13 :
      sprintf(&obuf[strlen(obuf)], "SYMLINK(xxx)");
      break;
    case 14 :
      sprintf(&obuf[strlen(obuf)], "MKDIR(%s)", dumpCreateargs(&event[msgstart], buf1));
      break;
    case 15 :
      sprintf(&obuf[strlen(obuf)], "RMDIR(xxx)");
      break;
    case 16 :
      sprintf(&obuf[strlen(obuf)], "READDIR(xxx)");
      break;
    case 17 :
      sprintf(&obuf[strlen(obuf)], "STATFS(fhandle=%s)", dumpFhandle(&event[msgstart], buf1));
      break;
    default :
      sprintf(&obuf[strlen(obuf)], "UNKNOWN#%d(xxx)", ntohl(*(int*)&event[20]));
  }
  strcat(obuf, "]");
  logText(subsystem, loglevel, obuf);
  
  return ntohl(*(int*)&event[20]);
}
