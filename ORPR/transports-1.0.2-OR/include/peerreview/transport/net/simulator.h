#ifndef __simulator_h__
#define __simulator_h__

#include <assert.h>
#include <unistd.h>
#include <peerreview/transport.h>

class Simulator {

  static const int maxCounterIndex = 32;

  class SimulatedTransport : public Transport {
  protected:
    TransportCallback *callback;
    Simulator *simulator;
    in_addr_t localAddr;
    int logfileHandle;
    
    long long counter[maxCounterIndex];
    FILE *snapshotFile;

  public:
    SimulatedTransport(Simulator *simulator, in_addr_t localAddr, int logfileHandle);
    virtual ~SimulatedTransport()
      { simulator->shutdown(localAddr); delete callback; close(logfileHandle); };
    virtual long long send(in_addr_t target, bool datagram, unsigned char *message, int msglen);
    virtual void scheduleTimer(TimerCallback *callback, int timerID, long long when) 
      { simulator->scheduleTimer(localAddr, callback, timerID, when); };
    virtual void cancelTimer(TimerCallback *callback, int timerID)
      { simulator->cancelTimer(localAddr, callback, timerID); };
    virtual int getMSS() 
      { return simulator->getMSS(); };
    virtual void setCallback(TransportCallback *callback)
      { this->callback = callback; };
    virtual long long getTime() 
      { return simulator->getTime(); };
    virtual in_addr_t getLocalIP()
      { return localAddr; };
    virtual void logText(const char *subsystem, int level, const char *format, ...);
    void enableSnapshots();
    void makeSnapshot(long long now);
    virtual void incrementCounter(int code, int value)
      { assert((0<=code) && (code<maxCounterIndex)); counter[code] += value; simulator->incrementCounter(code, value); };
    void recv(in_addr_t source, char type, unsigned char *message, int msglen)
      { callback->recv(source, type, message, msglen); };
  };

  static const int MAX_TIMERS = 25000;
  static const int MAX_PACKETS = 1000000;
  static const int MAX_INSTANCES = 1000;

  struct timerStruct {
    long long time;
    in_addr_t addr;
    int id;
    TimerCallback *callback;
  } timer[MAX_TIMERS];

  struct packetBuffer {
    long long deliveryTime;
    in_addr_t source;
    int targetInstance;
    bool datagram;
    unsigned char *msgbuf;
    int msglen;
  } packet[MAX_PACKETS];

  struct instanceInfo {
    in_addr_t address;
    SimulatedTransport *transport;
  } instance[MAX_INSTANCES];

  long long counter[maxCounterIndex];
  FILE *snapshotFile;
  long long lastSnapshot;
  long long snapshotInterval;
  long long nextSendSequenceNo;
  long long delayMicros;
  long long endTime;
  long long now;
  int numInstances;
  int numPackets;
  int numTimers;

  int findInstance(in_addr_t addr);
  void makeSnapshot();
  
public:
  Simulator(long long now, long long delayMicros, long long endTime = -1);
  ~Simulator();
  long long send(in_addr_t source, in_addr_t target, bool datagram, unsigned char *message, int msglen);
  void scheduleTimer(in_addr_t source, TimerCallback *callback, int timerID, long long when);
  void cancelTimer(in_addr_t source, TimerCallback *callback, int timerID);
  void cancelAllTimers(in_addr_t source);
  int getMSS() { return 1440; };
  Transport *createTransport(in_addr_t ip);
  long long getTime() { return now; };
  void enableSnapshots(long long interval, const char *filename);
  void run();
  void incrementCounter(int code, int value)
    { assert((0<=code) && (code<maxCounterIndex)); counter[code] += value; };
  void shutdown(in_addr_t ip);
};

#endif /* defined(__simulator_h__) */
