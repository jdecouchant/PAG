#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include "peerreview/transport/net/simulator.h"

//#define log(a...) do { fprintf(stdout, a); fprintf(stdout, "\n"); fflush(stdout); } while (0)
#define log(a...) do { } while (0)
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)

//#define CONDENSED_LOGGING

char *ipdisp(in_addr_t iaddr, char *buf)
{
  sprintf(buf, "%d.%d.%d.%d", 
    (int)(iaddr&0xFF), (int)((iaddr>>8)&0xFF), 
    (int)((iaddr>>16)&0xFF), (int)(iaddr>>24)
  );
  
  return buf;
}

Simulator::Simulator(long long now, long long delayMicros, long long endTime)
{
  this->numInstances = 0;
  this->numPackets = 0;
  this->numTimers = 0;
  this->delayMicros = delayMicros;
  this->now = now;
  this->endTime = endTime;
  this->snapshotFile = NULL;
  this->nextSendSequenceNo = 0;
  this->lastSnapshot = 0;
  this->snapshotInterval = -1;
  for (int i=0; i<maxCounterIndex; i++) 
    this->counter[i] = 0;
}

Transport *Simulator::createTransport(in_addr_t ip)
{
  char buf1[256];

  if (numInstances >= MAX_INSTANCES)
    panic("Too many instances in the simulator (increase MAX_INSTANCES)");

  for (int i=0; i<numInstances; i++)
    if (instance[i].address == ip)
      panic("Simulator transport registered twice: %s", ipdisp(ip, buf1));

  char namebuf[200];
  sprintf(namebuf, "sim-%s.log", ipdisp(ip, buf1));
#ifdef O_LARGEFILE
  int logfileHandle = open(namebuf, O_CREAT | O_APPEND | O_RDWR | O_LARGEFILE, 0644);
#else
  int logfileHandle = open(namebuf, O_CREAT | O_APPEND | O_RDWR, 0644);
#endif
    
  instance[numInstances].address = ip;
  instance[numInstances].transport = new SimulatedTransport(this, ip, logfileHandle);
  if (snapshotFile)
    instance[numInstances].transport->enableSnapshots();

  return instance[numInstances++].transport;
}

void Simulator::enableSnapshots(long long interval, const char *filename)
{
  snapshotFile = fopen(filename, "a");
  if (snapshotFile) {
    snapshotInterval = interval;
    lastSnapshot = 0;
    for (int i=0; i<numInstances; i++)
      instance[i].transport->enableSnapshots();
  }
}

void Simulator::SimulatedTransport::logText(const char *subsystem, int level, const char *format, ...)
{
#ifdef WARNINGSONLY
  if (level>0)
    return;
#endif

  char buffer[1024];
  va_list ap;
  va_start (ap, format);
  vsprintf (buffer, format, ap);
  if (logfileHandle>=0) {
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
 
    bool logIt = true;     
#ifdef CONDENSED_LOGGING
    if (!strcmp(subsystem, "peerreview"))
      logIt = false;
#endif
      
    if (logIt) {
      strcat(buffer, "\n");
      
      char prefix[200];
      sprintf(prefix, "%lld %s", simulator->getTime(), indent);
      write(logfileHandle, prefix, strlen(prefix));
      write(logfileHandle, buffer, strlen(buffer));
    }
  }
  va_end (ap);
}


void Simulator::scheduleTimer(in_addr_t source, TimerCallback *callback, int timerID, long long when)
{
  if (numTimers >= MAX_TIMERS)
    panic("Too many timers scheduled");
    
  char buf1[250];
  log("Scheduling timer #%d for %s at %lld (now=%lld)", timerID, ipdisp(source, buf1), when, now);
    
  timer[numTimers].time = when;
  timer[numTimers].addr = source;
  timer[numTimers].id = timerID;
  timer[numTimers].callback = callback;

  numTimers ++;
}

void Simulator::cancelTimer(in_addr_t source, TimerCallback *callback, int timerID)
{
  for (int i=0; i<numTimers; i++) {
    if ((timer[i].addr == source) && (timer[i].callback == callback) && (timer[i].id == timerID)) {
      timer[i] = timer[--numTimers];
      break;
    }
  }    
}

void Simulator::cancelAllTimers(in_addr_t source)
{
  for (int i=0; i<numTimers; ) {
    if (timer[i].addr == source)
      timer[i] = timer[--numTimers];
    else
      i++;
  }
}

int Simulator::findInstance(in_addr_t addr)
{
  for (int i=0; i<numInstances; i++) {
    if (instance[i].address == addr)
      return i;
  }
  
  return -1;
}

long long Simulator::send(in_addr_t source, in_addr_t target, bool datagram, unsigned char *message, int msglen)
{
  assert(message && (msglen > 0));
  if (numPackets >= MAX_PACKETS)
    panic("Simulator: Too many packets in flight (increase MAX_PACKETS)");

  int idx = findInstance(target);
  if (idx < 0)
    return -1;
    
  packet[numPackets].deliveryTime = now + delayMicros;
  packet[numPackets].source = source;
  packet[numPackets].targetInstance = idx;
  packet[numPackets].datagram = datagram;
  packet[numPackets].msgbuf = (unsigned char*) malloc(msglen);
  if (!packet[numPackets].msgbuf)
    panic("Simulator: Out of memory (%d/%d packets allocated)", numPackets, MAX_PACKETS);
  memcpy(packet[numPackets].msgbuf, message, msglen);
  packet[numPackets].msglen = msglen;
  numPackets ++;
  return nextSendSequenceNo++;
}

void Simulator::shutdown(in_addr_t ip)
{
  int idx = findInstance(ip);
  if (idx >= 0)
    instance[idx] = instance[--numInstances];
}

Simulator::~Simulator()
{
  for (int i=0; i<numInstances; i++)
    delete instance[i].transport;
    
  for (int i=0; i<numPackets; i++)
    free(packet[i].msgbuf);
    
  if (snapshotFile)
    fclose(snapshotFile);
}

void Simulator::makeSnapshot()
{
  fprintf(snapshotFile, "%lld", now);
  for (int i=0; i<maxCounterIndex; i++)
    fprintf(snapshotFile, " %lld", counter[i]);
  fprintf(snapshotFile, "\n");
  fflush(snapshotFile);
  
  for (int i=0; i<numInstances; i++)
    instance[i].transport->makeSnapshot(now);
  
  lastSnapshot = now;
}

void Simulator::run()
{
  char buf1[256], buf2[256];

  while ((numPackets > 0) || (numTimers > 0)) {
    long long nextEvent = LONG_LONG_MAX;
    
    for (int i=0; i<numPackets; i++) {
      if (packet[i].deliveryTime < nextEvent) 
        nextEvent = packet[i].deliveryTime;
    }
    
    for (int i=0; i<numTimers; i++) {
      if (timer[i].time < nextEvent) 
        nextEvent = timer[i].time;
    }
    
    if (nextEvent > now) {
      printf("t=%d.%06d\n", (int)(nextEvent/1000000), (int)(nextEvent%1000000));
      now = nextEvent;
    }
    
    if ((snapshotInterval >= 0) && (now >= (lastSnapshot+snapshotInterval)))
      makeSnapshot();
    
    if (now >= endTime)
      break;
    
    /* Deliver any pending timeouts. Note that it is essential that we
       deliver the timeouts in a deterministic order; here we order them
       by ID, in ascending order */
      
    bool madeProgress = true;
    while (madeProgress) {
      int best = -1;
      madeProgress = false;
      for (int i=0; i<numTimers; i++) {
        if ((timer[i].time <= now) && ((best<0) || (timer[i].time<timer[best].time) || ((timer[i].time==timer[best].time) && (timer[i].id<timer[best].id))))
          best = i;
      }    
      
      if (best >= 0) {
        struct timerStruct ts = timer[best];
        timer[best] = timer[--numTimers];
        log("Invoking timer #%d on %s at %lld", ts.id, ipdisp(ts.addr, buf1), ts.time);
        ts.callback->timerExpired(ts.id);
        madeProgress = true;
      }
    }
    
    for (int i=0; i<numPackets; ) {
      if (packet[i].deliveryTime <= now) {
        struct packetBuffer pb = packet[i];
        packet[i] = packet[--numPackets];
        log("Delivering packet %s->%s (%d bytes)", ipdisp(pb.source, buf1), ipdisp(instance[pb.targetInstance].address, buf2), pb.msglen);
        instance[pb.targetInstance].transport->recv(pb.source, pb.datagram, pb.msgbuf, pb.msglen);
        free(pb.msgbuf);
      } else {
        i++;
      }
    }
  }
}

Simulator::SimulatedTransport::SimulatedTransport(Simulator *simulator, in_addr_t localAddr, int logfileHandle) : Transport()
{
  this->simulator = simulator; 
  this->localAddr = localAddr; 
  this->callback = NULL; 
  this->logfileHandle = logfileHandle;  
  this->snapshotFile = NULL;
  
  for (int i=0; i<maxCounterIndex; i++) 
    counter[i] = 0;
}

long long Simulator::SimulatedTransport::send(in_addr_t target, bool datagram, unsigned char *message, int msglen)
{
  incrementCounter(CTR_MSG_TOTAL, 1);
  incrementCounter(CTR_BYTES_TOTAL, msglen);
  if (message[0]<=24)
    incrementCounter(CTR_BYTES_MSG0+message[0], msglen);

  return simulator->send(localAddr, target, datagram, message, msglen); 
}

void Simulator::SimulatedTransport::enableSnapshots()
{
  char namebuf[200], buf1[200];
  sprintf(namebuf, "snapshots-%s.data", ipdisp(localAddr, buf1));
  snapshotFile = fopen(namebuf, "a");
}

void Simulator::SimulatedTransport::makeSnapshot(long long now)
{
  fprintf(snapshotFile, "%lld", now);
  for (int i=0; i<maxCounterIndex; i++)
    fprintf(snapshotFile, " %lld", counter[i]);
  fprintf(snapshotFile, "\n");
  fflush(snapshotFile);
}
