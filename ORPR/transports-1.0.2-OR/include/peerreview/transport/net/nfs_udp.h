#ifndef __nfs_udp_h__
#define __nfs_udp_h__

#include <peerreview/transport.h>

class DescriptorCallback {
public:
  DescriptorCallback() {};
  virtual ~DescriptorCallback() {};
  virtual void descriptorReady(int desc, bool read, bool write, bool except) = 0;
};

class NfsUdpTransport : public Transport {
protected:
  const static int MAX_TIMERS = 10;
  const static int MAX_IDLE_MICROS = 30000000;
  const static int MAX_DESCRIPTORS = 10;
  const static int PORT_DATA = 3049;
  const static int PORT_CONTROL = 4049;

  struct timerStruct {
    long long time;
    int id;
    TimerCallback *callback;
  } timer[MAX_TIMERS];

  struct descriptorStruct {
    int desc;
    DescriptorCallback *callback;
    bool wantRead;
    bool wantWrite;
    bool wantExcept;
  } descriptor[MAX_DESCRIPTORS];

  FILE *logfile;
  TransportCallback *callback;
  in_addr_t localAddress;
  int dataSocket;
  int controlSocket;
  int numTimers;
  int numDescriptors;
  long long now;
  long long lastSendIdAssigned;
  long long lastSendIdCompleted;
  
  int openUdpServerSocket(in_addr_t addr, int port);
  
public:
  NfsUdpTransport(in_addr_t localAddress);
  virtual long long send(in_addr_t target, bool datagram, unsigned char *message, int msglen);
  virtual void scheduleTimer(TimerCallback *callback, int timerID, long long when);
  virtual void cancelTimer(TimerCallback *callback, int timerID);
  virtual int getMSS() { return 1440; };
  virtual void setCallback(TransportCallback *callback);
  virtual long long getTime() { return now; };
  virtual in_addr_t getLocalIP() { return localAddress; };
  virtual void logText(const char *subsystem, int level, const char *format, ...);
  virtual void incrementCounter(int code, int value) {};
  void dump(const char *subsystem, int level, const unsigned char *payload, int len);
  void openLog(const char *logname);
  void setDescriptorCallback(int desc, DescriptorCallback *callback, bool wantRead, bool wantWrite, bool wantExcept);
  void unsetDescriptorCallback(int desc);
  long long getTimeMicros();
  void run();
  void shutdown();
  int dumpMessage(const char *subsystem, int loglevel, bool isMountd, bool isResponse, int cookie, unsigned char *event, int size);
};

#endif /* defined(__nfs_udp_h__) */
