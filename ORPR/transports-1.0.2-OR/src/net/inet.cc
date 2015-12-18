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
#include <netinet/tcp.h>
//#define log(a...) do { } while (0)
#define log(a...) do { logText("inet", 3, a); } while (0)
#define warning(a...) do { logText("inet", 1, "WARNING: " a); } while (0)
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); dumpState(); abort(); } while (0)

#define SUBSYSTEM "inet"

#ifdef __CYGWIN__
#define MSG_DONTWAIT 0
#endif

#define CHECK_PROGRESS

#include "peerreview/transport/net/inet.h"

static char *ipdisp(in_addr_t iaddr, char *buf)
{
  sprintf(buf, "%d.%d.%d.%d", 
    (int)(iaddr&0xFF), (int)((iaddr>>8)&0xFF), 
    (int)((iaddr>>16)&0xFF), (int)(iaddr>>24)
  );
  
  return buf;
}

//#define TRACE_PACKETS
#ifdef TRACE_PACKETS

static void dumpPacket(const unsigned char *payload, int len)
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
    fprintf(stderr, "%s\n", linebuf);
  }
}
#endif

InetTransport::InetTransport(in_addr_t localAddress, long long endTime)
{
  this->localAddress = localAddress;
  this->numTimers = 0;
  this->numConnections = 0;
  this->numPackets = 0;
  this->tcpServerPort = -1;
  this->udpServerPort = -1;
  this->now = getTimeMicros();
  this->endTime = endTime;  //par moi
  this->logfile = NULL;
  this->numDescriptors = 0;
  this->callback = NULL;
  this->logTimeVerbose = false;
  this->logLevel = 999;
  this->nextSendSequenceNo = 0;
  this->terminating = false;
}

InetTransport::~InetTransport()
{
  if (logfile) 
    fclose(logfile);
    
  if (callback)
    delete callback;
    
  for (int i=0; i<numConnections; i++)
    free(connection[i].buffer);
}

void InetTransport::openLog(const char *logname)
{
  logfile = fopen(logname, "w+");
}

void InetTransport::dump(const char *subsystem, int level, const unsigned char *payload, int len)
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
    logText(SUBSYSTEM, level, "%s", linebuf);
  }
}

void InetTransport::setLogFilter(const char *filterString)
{
  if (!filterString) {
    logFilter[0] = 0;
    return;
  }
  
  strncpy(logFilter, filterString, sizeof(logFilter));
}

void InetTransport::logText(const char *subsystem, int level, const char *format, ...)
{
  if (logFilter[0]) {
    if (!strstr(logFilter, subsystem) && (level>0))
      return;
  }
  
  if ((level>0) && (level>logLevel))
    return;

  char buffer[4096];
  va_list ap;
  va_start (ap, format);
  if (vsnprintf (buffer, sizeof(buffer), format, ap) > (int)sizeof(buffer))
    panic("Buffer overflow in InetTransport::logText()");
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
      
    char prefix[200];
    
    if (logTimeVerbose) {
      time_t ti = (int)(now/1000000);
      struct tm ttm;
      gmtime_r(&ti, &ttm);
      sprintf(prefix, "%02d/%02d/%04d %02d:%02d:%02d.%06d %s", 
        1+ttm.tm_mon, ttm.tm_mday, 1900+ttm.tm_year, ttm.tm_hour, ttm.tm_min, ttm.tm_sec, (int)(now%1000000), 
        indent
      );
    } else {
      sprintf(prefix, "%lld %s", now, indent);
    }
    
    char *ptr = buffer;
    while (true) {
      char *lf = strchr(ptr, '\n');
      if (!lf) {
        fprintf(logfile, "%s%s\n", prefix, ptr);
        break;
      }
      
      *lf = 0;
      fprintf(logfile, "%s%s\n", prefix, ptr);
      ptr = lf+1;
      if (!*ptr)
        break;
    }

    fflush(logfile);
  }
  va_end (ap);
}

long long InetTransport::getTimeMicros()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000LL + tv.tv_usec;
}

void InetTransport::shutdown()
{
/*  for (int i=0; i<numConnections; i++) 
    close(connection[i].fd);
  
  if (tcpServerPort >= 0) 
    close(tcpServerPort);
  if (udpServerPort >= 0)
    close(udpServerPort);
*/    
  terminating = true;
}

void InetTransport::dumpState()
{
  char namebuf[200], buf1[256];
  sprintf(namebuf, "inet-%s-dump-%lld", ipdisp(localAddress, buf1), getTime());
  FILE *outfile = fopen(namebuf, "w+");
  fprintf(outfile, "=== %d timer(s):\n", numTimers);
  for (int i=0; i<numTimers; i++)
    fprintf(outfile, " - T%d: ti=%lld id=%d callback=%p\n", i, timer[i].time, timer[i].id, timer[i].callback);
  fprintf(outfile, "=== %d descriptor(s):\n", numDescriptors);
  for (int i=0; i<numDescriptors; i++)
    fprintf(outfile, " - D%d: desc=%d callback=%p read=%s write=%s except=%s\n",
      i, descriptor[i].desc, descriptor[i].callback,
      descriptor[i].wantRead ? "yes" : "no",    
      descriptor[i].wantWrite ? "yes" : "no",    
      descriptor[i].wantExcept ? "yes" : "no"
    );
  fprintf(outfile, "=== %d connection(s):\n", numConnections);
  for (int i=0; i<numConnections; i++) 
    fprintf(outfile, " - C%d: target=%X fd=%d lastUsed=%lld buflen=%d\n",
      i, connection[i].target, connection[i].fd, connection[i].lastUsed, connection[i].len
    );
  fprintf(outfile, "=== %d packet(s):\n", numPackets);
  for (int i=0; i<numPackets; i++) {
    if (packet[i].inUse) {
      char data[200];
      data[0] = 0;
      for (int j=0; (j<16) && (j<packet[i].len); j++)
        sprintf(&data[strlen(data)], "%s%02X", (j==0) ? "" : " ", packet[i].msgbuf[j]);
      if (packet[i].len > 16)
        strcat(data, " ...");
      fprintf(outfile, " - P%d: target=%X sendTime=%lld datagram=%s len=%d data=[%s]\n",
        i, packet[i].target, packet[i].sendTime, packet[i].datagram ? "yes" : "no",
        packet[i].len, data
      );
    } else {
      fprintf(outfile, " - P%d: -/-\n", i);
    }
  }
  
  fclose(outfile);
}

int InetTransport::openServerSocket(in_addr_t addr, int port, int type)
{
  char buf1[200];

  int sock = socket(AF_INET, type, 0);
  if (sock < 0)
    panic("Cannot open %s socket", (type == SOCK_DGRAM) ? "UDP" : "TCP");
    
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = addr;
  if (bind(sock, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
    panic("Cannot bind to port %s:%d (%s)", ipdisp(addr, buf1), port, (type == SOCK_DGRAM) ? "UDP" : "TCP");

  if (type == SOCK_STREAM) {
    if (listen(sock, 1)<0)
      panic("Cannot set socket to listen");
  }
  
  return sock;
}

long long InetTransport::send(in_addr_t target, bool datagram, unsigned char *message, int msglen)
{
  //dump(4, message, msglen);

  int idx = -1;
  for (int i=0; (idx<0) && (i<numPackets); i++)
    if (!packet[i].inUse)
      idx = i;

  if (idx < 0) {
    if (numPackets >= MAX_PACKETS)
      panic("Too many packets queued");
    idx = numPackets ++;
  }  

  char buf1[256];
  log("Accepted packet type %s for delivery to %s (%d bytes) idx=%d n=%d", datagram ? "DGRAM" : "MSG", ipdisp(target, buf1), msglen, idx, numPackets);

  packet[idx].inUse = true;    
  packet[idx].sendTime = getTimeMicros();
  packet[idx].target = target;
  packet[idx].datagram = datagram;
  packet[idx].ptr = 0;

  if (datagram) {
    assert(msglen < 65000);
    packet[idx].msgbuf = (unsigned char*) malloc(msglen);
    assert(packet[idx].msgbuf);
    memcpy(packet[idx].msgbuf, message, msglen);
    packet[idx].len = msglen;
  } else {
    packet[idx].msgbuf = (unsigned char*) malloc(msglen + 4);
    assert(packet[idx].msgbuf);
    *(int*)&packet[idx].msgbuf[0] = htonl(msglen);
    memcpy(&packet[idx].msgbuf[4], message, msglen);
    packet[idx].len = msglen + 4;
  }
  
  return nextSendSequenceNo ++;
}

void InetTransport::scheduleTimer(TimerCallback *callback, int timerID, long long when)
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

void InetTransport::cancelTimer(TimerCallback *callback, int timerID)
{
  for (int i=0; i<numTimers; ) {
    if ((timer[i].id == timerID) && (timer[i].callback == callback)) 
      timer[i] = timer[--numTimers];
    else
      i++;
  }
}

void InetTransport::setCallback(TransportCallback *callback)
{
  this->callback = callback;
}

void InetTransport::releasePacket(int idx)
{
  assert((idx>=0) && (idx<MAX_PACKETS) && packet[idx].inUse);
  packet[idx].inUse = false;
  free(packet[idx].msgbuf);
}

void InetTransport::deliverAnyPackets(int idx)
{
  char buf1[200];

  while (true) {
    if (connection[idx].len < 4)
      break;
      
    int len = ntohl(*(int*)&(connection[idx].buffer[0]));
    if (connection[idx].len < (4+len))
      break;
      
    log("Message from %s ready for delivery (%d bytes)", ipdisp(connection[idx].target, buf1), len);
    //dump(4, &connection[idx].buffer[2], len);
#ifdef TRACE_PACKETS
	inetTracePacketWithMagic(&(connection[idx].buffer[4]), len, "Recv_INET_after_tcp_read");
#endif
    callback->recv(connection[idx].target, false, &(connection[idx].buffer[4]), len);
    for (int i=(len + 4); i<connection[idx].len; i++)
      connection[idx].buffer[i-(len+4)] = connection[idx].buffer[i];
    
    connection[idx].len -= len + 4;
  }
}

void InetTransport::setDescriptorCallback(int desc, DescriptorCallback *callback, bool wantRead, bool wantWrite, bool wantExcept)
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

void InetTransport::unsetDescriptorCallback(int desc)
{
  for (int i=0; i<numDescriptors; i++) {
    if (descriptor[i].desc == desc) {
      descriptor[i] = descriptor[--numDescriptors];
      break;
    }
  }
}

void InetTransport::init()
{
  char buf1[200], buf2[200];

  tcpServerPort = openServerSocket(localAddress, PORT_TCP, SOCK_STREAM);
  udpServerPort = openServerSocket(localAddress, PORT_UDP, SOCK_DGRAM);
  log("Running service on %s:%d/tcp and %s:%d/udp", ipdisp(localAddress, buf1), PORT_TCP, ipdisp(localAddress, buf2), PORT_UDP);

  int yes = 1;
  if (setsockopt(tcpServerPort, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    panic("Cannot set socket option on TCP server port");
}

void InetTransport::getSelectArguments(int *maxFD, fd_set *fdr, fd_set *fdw, fd_set *fdx, long long *earliestTimeout)
{
  char buf1[256];

  /* Expire old packets */

  long long maxAge = getTimeMicros() - MAX_PACKET_BUFFERED_MICROS;
  for (int i=0; i<numPackets; i++) {
    if (packet[i].inUse && (packet[i].sendTime < maxAge)) {
      logText(SUBSYSTEM, 2, "Timing out queued %d-byte packet for %X", packet[i].len, packet[i].target);
      releasePacket(i);
    }
  }
  
  /* Send as much as we can via UDP */

  for (int i=0; i<numPackets; i++) {
    if (packet[i].inUse && packet[i].datagram) {
      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_port = htons(PORT_UDP);
      addr.sin_addr.s_addr = packet[i].target;
      log("Sending datagram to %s (%d bytes)", ipdisp(packet[i].target, buf1), packet[i].len);
      int ret = sendto(udpServerPort, packet[i].msgbuf, packet[i].len, MSG_DONTWAIT, (struct sockaddr*)&addr, sizeof(addr));
      if (ret < packet[i].len) {
        if ((ret < 0) && (errno != EINTR) && (errno != EAGAIN))
          panic("Cannot send via UDP (%s)", strerror(errno));
      } else {
        releasePacket(i);
      }
    }
  }

  /* Find out which connections need writing. We look at all the packets
     and mark the corresponding connection. */

  *maxFD = (tcpServerPort>udpServerPort) ? tcpServerPort : udpServerPort;
  FD_SET(tcpServerPort, fdr);
  FD_SET(udpServerPort, fdr);

  for (int i=0; i<numPackets; i++) {
    if (packet[i].inUse && !packet[i].datagram) {
      int idx = -1;
      for (int j=0; (idx<0) && (j<numConnections); j++) {
        if (connection[j].target == packet[i].target)
          idx = j;
      }

      if (idx < 0) {
        if (numConnections >= MAX_CONNECTIONS)
          panic("Too many open connections");

        log("Opening connection to %s", ipdisp(packet[i].target, buf1));

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
          panic("Cannot obtain socket");

        int flags = fcntl(sock, F_GETFL, 0);
        flags |= O_NONBLOCK | O_NDELAY;
        if (fcntl(sock, F_SETFL, flags) < 0)
          panic("Cannot set non-blocking options");

        struct sockaddr_in saddr;
        memset(&saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(0);
        saddr.sin_addr.s_addr = localAddress;
        if (bind(sock, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
          panic("Cannot bind to address %s (TCP)", ipdisp(localAddress, buf1));

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT_TCP);
        addr.sin_addr.s_addr = packet[i].target;

        int ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        if ((ret < 0) && (errno != EINPROGRESS))
          panic("Cannot connect to %s:%d (%s)", ipdisp(packet[i].target, buf1), PORT_TCP, strerror(errno));

        int yes = 1;
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)))
          panic("Cannot set nodelay option on outgoing connection");

        connection[numConnections].lastUsed = getTimeMicros();
        connection[numConnections].target = packet[i].target;
        connection[numConnections].fd = sock;
        connection[numConnections].len = 0;
        connection[numConnections].bufferSize = DEFAULT_BUFFER_SIZE_BYTES;
        connection[numConnections].buffer = (unsigned char*) malloc(connection[numConnections].bufferSize);
        idx = numConnections ++;
      }

      if (!FD_ISSET(connection[idx].fd, fdw)) {
        FD_SET(connection[idx].fd, fdw);
        if (connection[idx].fd > *maxFD)
          *maxFD = connection[idx].fd;
      }
    }
  }

  /* All connections need reading */

  for (int i=0; i<numConnections; i++) {
    FD_SET(connection[i].fd, fdr);
    if (connection[i].fd > *maxFD)
      *maxFD = connection[i].fd;
  }

  /* Mark the descriptors that have been specifically requested 
     by the application */

  for (int i=0; i<numDescriptors; i++) {
    if (descriptor[i].desc > *maxFD)
      *maxFD = descriptor[i].desc;

    if (descriptor[i].wantRead)
      FD_SET(descriptor[i].desc, fdr);
    if (descriptor[i].wantWrite)
      FD_SET(descriptor[i].desc, fdw);
    if (descriptor[i].wantExcept)
      FD_SET(descriptor[i].desc, fdx);
  }

  /* Find the earliest timeout, and create a timeval structure
     for use with select() */

  *earliestTimeout = -1;
  if (numTimers > 0) {
    *earliestTimeout = timer[0].time;
    for (int i=1; i<numTimers; i++) {
      if (timer[i].time < *earliestTimeout)
        *earliestTimeout = timer[i].time;
    }
  }
}

void InetTransport::run()
{
  long long twakeup = getTimeMicros();
  long long earliestTimeout;
  fd_set fdr, fdw, fdx;
  int maxFD;
  
  init();
  
  while (!terminating) {
    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_ZERO(&fdx);
    
    getSelectArguments(&maxFD, &fdr, &fdw, &fdx, &earliestTimeout);
    
    struct timeval tv;
    struct timeval *to = NULL;
    if (earliestTimeout >= 0) {
      long long realNow = getTimeMicros();
      long long wait = (earliestTimeout<realNow) ? 0 : (earliestTimeout - realNow);
      to = &tv;
      tv.tv_sec = (int)(wait/1000000);
      tv.tv_usec = (int)(wait%1000000);
    }
  
    /* Wait until something interesting happens */
    
#ifdef CHECK_PROGRESS
    long long tsleep = getTimeMicros(); 
    if ((tsleep-twakeup)>50000)
      warning("Long processing delay (dT=%lld)", tsleep-twakeup);
#endif

    int ret = select(maxFD + 1, &fdr, &fdw, NULL, to);
    if ((ret<0) && (errno != EINTR))
      panic("Select() failed; ret=%d (%s)", ret, strerror(errno));
      
    twakeup = getTimeMicros();

    if(now >= endTime)
        break;

    processSelectResults(&fdr, &fdw, &fdx);
  }
}

void InetTransport::processSelectResults(fd_set *fdr, fd_set *fdw, fd_set *fdx)
{
  /* Deliver any pending timeouts. Note that it is essential that we
     deliver the timeouts in a deterministic order; here we order them
     by ID, in ascending order */
  char buf1[200];
  long long realNow = getTimeMicros();

  bool madeProgress = true;
  while (madeProgress) {
    madeProgress = false;
    int best = -1;
    for (int i=0; i<numTimers; i++) {
      if ((timer[i].time <= realNow) && ((best<0) || (timer[i].time<timer[best].time) || ((timer[i].time==timer[best].time) && (timer[i].id<timer[best].id))))
        best = i;
    }
//printf("best = %d\n",best);
    if (best >= 0) {
     // printf("timer[best].time = %ld\n",timer[best].time);
     // printf("now = %ld\n",now);
      assert(timer[best].time >= now); 
      now = timer[best].time;
      int id = timer[best].id;
      TimerCallback *callback = timer[best].callback;
      timer[best] = timer[--numTimers];
      log("Timer %d", id);
      //printf("Timer %d\n", id);
      callback->timerExpired(id);
      madeProgress = true;
    }
  }

  now = realNow;

  /* Make progress on writing */

  for (int i=0; i<numConnections; ) {
    bool closed = false;
    if (FD_ISSET(connection[i].fd, fdw)) {
      int best = -1;
      long long bestTime = -1;
      for (int j=0; j<numPackets; j++) {
        if (packet[j].inUse && !packet[j].datagram && (packet[j].target == connection[i].target)) {
          if ((best < 0) || (packet[j].sendTime < bestTime)) {
            best = j;
            bestTime = packet[j].sendTime;
          }
        }
      }

      assert(best >= 0);
      assert(packet[best].ptr < packet[best].len);
      //printf("FD = %d\n",connection[i].fd);
      int w = write(connection[i].fd, &packet[best].msgbuf[packet[best].ptr], packet[best].len - packet[best].ptr);
      log("Wrote %d bytes to %s (idx=%d start=%d/%d)", w, ipdisp(connection[i].target, buf1), best, packet[best].ptr, packet[best].len);
      //printf("Wrote %d bytes\n",w);
      if (w <= 0) {
        log("Cannot write to %s (%s); closing connection", ipdisp(connection[i].target, buf1), strerror(errno));
        close(connection[i].fd);
        packet[best].ptr = 0; 
        closed = true;
      } else {
        packet[best].ptr += w;
        connection[i].lastUsed = getTimeMicros();
        if (packet[best].ptr >= packet[best].len) {
          log("Finished sending packet to %s [idx=%d]", ipdisp(connection[i].target, buf1), best);
          //printf("Finished sending packet\n");
          free(packet[best].msgbuf);
          packet[best].inUse = false;
          while ((numPackets>0) && !packet[numPackets-1].inUse)
            numPackets --;
          log("numPackets=%d", numPackets);
        }
      }
    }

    if (closed) {
      free(connection[i].buffer);
      connection[i] = connection[--numConnections];
    } else {
      i++;
    }
  }

  /* Make progress on reading */

  for (int i=0; i<numConnections; ) {
    bool closed = false;
    if (FD_ISSET(connection[i].fd, fdr)) {
      if (connection[i].len >= connection[i].bufferSize) {
        connection[i].bufferSize *= 2;
        warning("Extending message buffer to %d bytes", connection[i].bufferSize);

        unsigned char *newBuffer = (unsigned char*) malloc(connection[i].bufferSize);
        memcpy(newBuffer, connection[i].buffer, connection[i].len);
        free(connection[i].buffer);
        connection[i].buffer = newBuffer;
      }

      int r = read(connection[i].fd, &(connection[i].buffer[connection[i].len]), connection[i].bufferSize - connection[i].len);
      log("Read %d bytes from %s", r, ipdisp(connection[i].target, buf1));
      //printf("Read %d bytes\n",r);
      if (r <= 0) {
        log("Cannot read from %s (%s); closing connection", ipdisp(connection[i].target, buf1), strerror(errno));
        close(connection[i].fd);
        closed = true;
      } else {
        connection[i].len += r;
        connection[i].lastUsed = getTimeMicros();
        deliverAnyPackets(i);
      }
    }

    if (closed) {
      free(connection[i].buffer);
      connection[i] = connection[--numConnections];
    } else {
      i++;
    }
  }

  /* Accept incoming connections */

  if (FD_ISSET(tcpServerPort, fdr)) {
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    int fd = accept(tcpServerPort, (struct sockaddr*)&from, &fromlen);
    if ((fd >= 0) && (numConnections<MAX_CONNECTIONS)) {
      log("Incoming connection from %s", ipdisp(from.sin_addr.s_addr, buf1));
      //printf("Incomming connection\n");
      int yes = 1;
      if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)))
        panic("Cannot set nodelay option on incoming conncetion");

      int idx = -1;
      for (int i=0; i<numConnections; i++) {
        if (connection[i].target == from.sin_addr.s_addr)
          idx = i;
      }

      if (idx < 0) {
        connection[numConnections].lastUsed = getTimeMicros();
        connection[numConnections].target = from.sin_addr.s_addr;
        connection[numConnections].fd = fd;
        connection[numConnections].len = 0;
        connection[numConnections].bufferSize = DEFAULT_BUFFER_SIZE_BYTES;
        connection[numConnections].buffer = (unsigned char*) malloc(connection[numConnections].bufferSize);
        numConnections ++;
      } else {
        log("ERROR: Duplicate connection from %s; closing", ipdisp(from.sin_addr.s_addr, buf1));
        close(fd);
      }
    }
  }

  /* Receive incoming UDP packets */          

  if (FD_ISSET(udpServerPort, fdr)) {
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    unsigned char buffer[65536];

    int len = recvfrom(udpServerPort, &buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromlen);
    if (len < 0)
      panic("Cannot read from UDP socket (%s)", strerror(errno));

    log("Received datagram from %s (%d bytes)", ipdisp(from.sin_addr.s_addr, buf1), len);
    callback->recv(from.sin_addr.s_addr, true, buffer, len);
  }

  /* Handle callbacks, if any */

  for (int i=0; i<numDescriptors; i++) {
    bool reportRead = descriptor[i].wantRead && FD_ISSET(descriptor[i].desc, fdr);
    bool reportWrite = descriptor[i].wantWrite && FD_ISSET(descriptor[i].desc, fdw);
    bool reportExcept = descriptor[i].wantExcept && FD_ISSET(descriptor[i].desc, fdx);

    if (reportRead || reportWrite || reportExcept)
      descriptor[i].callback->descriptorReady(descriptor[i].desc, reportRead, reportWrite, reportExcept);
  }

  /* Clean up unused connections */  

  long long lastAcceptable = getTimeMicros() - MAX_IDLE_MICROS;  
  for (int i=0; i<numConnections; ) {
    if (connection[i].lastUsed < lastAcceptable) {
      log("Closing idle connection to %s", ipdisp(connection[i].target, buf1));
      close(connection[i].fd);
      for (int j=0; j<numPackets; j++) {
        if (packet[j].target == connection[i].target)
          packet[j].ptr = 0;
      }

      free(connection[i].buffer);
      connection[i] = connection[--numConnections];
    } else {
      i++;
    }
  }
}
