#ifndef __inet_h__
#define __inet_h__

#include <stdio.h>

#include <peerreview/transport.h>

class DescriptorCallback {
public:
  DescriptorCallback() {};
  virtual ~DescriptorCallback() {};
  virtual void descriptorReady(int desc, bool read, bool write, bool except) = 0;
};

class SelectCallback {
public:
  SelectCallback() {};
  virtual ~SelectCallback() {};
  virtual void init(fd_set *fdr, fd_set *fdw, fd_set *fdx) = 0;
};

class InetTransport : public Transport {
protected:
  static const int MAX_TIMERS = 100;
  static const int MAX_CONNECTIONS = 30;
  static const int MAX_PACKETS = 1000;
  static const int DEFAULT_BUFFER_SIZE_BYTES = 1000000;
  static const int MAX_IDLE_MICROS = 30000000;
  static const int MAX_DESCRIPTORS = 30;
  static const int MAX_PACKET_BUFFERED_MICROS = 30000000;
  static const int PORT_TCP = 4004;
  static const int PORT_UDP = 4004;

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

  struct connectionStruct {
    long long lastUsed;
    in_addr_t target;
    int fd;
    unsigned char *buffer;
    int bufferSize;
    int len;
  } connection[MAX_CONNECTIONS];

  struct packetBuffer {
    bool inUse;
    long long sendTime;
    in_addr_t target;
    bool datagram;
    unsigned char *msgbuf;
    int ptr;
    int len;
  } packet[MAX_PACKETS];

  FILE *logfile;
  TransportCallback *callback;
  in_addr_t localAddress;
  int tcpServerPort;
  int udpServerPort;
  int numConnections;
  int numDescriptors;
  int numPackets;
  int numTimers;
  long long now;
  long long endTime; //par moi
  long long nextSendSequenceNo;
  bool logTimeVerbose;
  char logFilter[200];
  int logLevel;
  bool terminating;
  
  int openServerSocket(in_addr_t addr, int port, int type);
  void deliverAnyPackets(int idx);
  void releasePacket(int idx);
  void dumpState();
  void dump(const char *subsystem, int level, const unsigned char *payload, int len);
  
public:
  InetTransport(in_addr_t localAddress, long long endTime);
  ~InetTransport();
  virtual long long send(in_addr_t target, bool datagram, unsigned char *message, int msglen);
  virtual void scheduleTimer(TimerCallback *callback, int timerID, long long when);
  virtual void cancelTimer(TimerCallback *callback, int timerID);
  virtual int getMSS() { return 1440; };
  virtual void setCallback(TransportCallback *callback);
  virtual long long getTime() { return now; };
  virtual in_addr_t getLocalIP() { return localAddress; };
  virtual void logText(const char *subsystem, int level, const char *format, ...);
  virtual void incrementCounter(int code, int value) {};
  void setDescriptorCallback(int desc, DescriptorCallback *callback, bool wantRead, bool wantWrite, bool wantExcept);
  void unsetDescriptorCallback(int desc);
  void openLog(const char *logname);
  void setLogFilter(const char *filterString);
  void setLogLevel(int logLevel) { this->logLevel = logLevel; };
  long long getTimeMicros();
  void run();
  void shutdown();

  void init();
  void getSelectArguments(int *maxFD, fd_set *fdr, fd_set *fdw, fd_set *fdx, long long *earliestTimeout);
  void processSelectResults(fd_set *fdr, fd_set *fdw, fd_set *fdx);
};

#endif /* defined(__inet_h__) */
