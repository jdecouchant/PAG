#ifndef __simple_h__
#define __simple_h__

#include <stdio.h>
#include <peerreview/identity.h>
#include <peerreview/tools.h>

#define MSG_ID_CHALLENGE 0
#define MSG_ID_RESPONSE 1
#define MSG_ID_REFUSAL 2
#define MSG_CERTIFICATE_REQUEST 3
#define MSG_CERTIFICATE_RESPONSE 4
#define MSG_CERTIFICATE_OFFER 5

class SimpleIdentifier : public Identifier {
protected:
  static const int MAX_BYTES = 20;
  unsigned char raw[MAX_BYTES];
  int sizeBits;
  
public:
  SimpleIdentifier(int sizeBits, const unsigned char *raw);
  virtual bool equals(Identifier *other);
  virtual SimpleIdentifier *clone();
  virtual char *render(char *buf);
  virtual bool write(unsigned char *buf, unsigned int *ptr, unsigned int maxlen);
  virtual int getSizeBytes();
  virtual bool isBiggerThan(Identifier *other);
  virtual bool isCloserThan(Identifier *basis, Identifier *other);
  virtual int getDigit(int digitNr, int bitsPerDigit);
  virtual int commonPrefixLength(Identifier *other, int bitsPerDigit);
  virtual bool isBetween(Identifier *from, Identifier *to);
  virtual SimpleIdentifier *makeIdHalfwayTo(Identifier *other);
  static SimpleIdentifier *read(const unsigned char *buf, unsigned int *ptr, unsigned int maxlen, int sizeBits);
  static SimpleIdentifier *readFromString(const char *str, int sizeBits);
  virtual int operator %(int modulo);
  virtual int size() { return getSizeBytes(); };
  unsigned char* get_raw_ptr(int *sz);
};

class SimpleNodeHandle : public NodeHandle {
protected:
  SimpleIdentifier *identifier;
  in_addr_t ip;
  
public:
  SimpleNodeHandle(SimpleIdentifier *identifier, in_addr_t ip) : NodeHandle()
    { this->identifier = identifier; this->ip = ip; };
  virtual ~SimpleNodeHandle()
    { delete identifier; };
  virtual Identifier *getIdentifier()
    { return identifier; };
  virtual SimpleNodeHandle *clone()
    { return new SimpleNodeHandle(identifier->clone(), ip); };
  virtual char *render(char *buf)
    { identifier->render(buf); sprintf(&buf[strlen(buf)], "/%d.%d.%d.%d", (int)(ip&0xFF), (int)((ip>>8)&0xFF), (int)((ip>>16)&0xFF), (int)(ip>>24)); return buf; };
  in_addr_t getIP()
    { return ip; };
  virtual bool write(unsigned char *buf, unsigned int *ptr, unsigned int maxlen)
    { if (!identifier->write(buf, ptr, maxlen)) return false; if (((*ptr)+sizeof(in_addr_t))>maxlen) return false; *(in_addr_t*)&buf[*ptr] = ip; (*ptr) += sizeof(ip); return true; };
  virtual bool equals(NodeHandle *other) 
    { SimpleNodeHandle *snh = (SimpleNodeHandle*)other; return snh->identifier->equals(identifier) && (snh->ip == ip); };
  virtual int size() 
    { return identifier->size() + sizeof(in_addr_t); };
};

class SimpleIdentityTransport : public IdentityTransport, public TransportCallback, public TimerCallback {
protected:
  static const long long VALIDATION_INTERVAL_MICROS = 3600000000ULL;
  static const long long PACKET_TIMEOUT_MICROS = 1000000;
  //static const long long CERT_REQUEST_TIMEOUT_MICROS = 1000000;
  static const long long CERT_REQUEST_TIMEOUT_MICROS = 10000000;
  static const int MAX_ACTIVE_VALIDATIONS = 250;
  static const int MAX_ACTIVE_CERT_REQUESTS = 250;
  static const int TI_CLEANUP = 0;
  static const int TI_CERT_REQUEST = 1;

  struct packetInfo {
    struct packetInfo *next;
    in_addr_t source;
    long long timeout;
    unsigned char *message;
    int msglen;
  };

  struct activeValidation {
    long long challenge;
    in_addr_t lastAddress;
    bool waitingForCertificate;
    SimpleIdentifier *certificateID;
    unsigned char *lastResponse;
  } validation[MAX_ACTIVE_VALIDATIONS];

  struct pendingCertificateRequest {
    in_addr_t source;
    Identifier *id;
    int attemptsSoFar;
  } certRequest[MAX_ACTIVE_CERT_REQUESTS];

  struct certificateRecord {
    struct certificateRecord *next;
    SimpleIdentifier *id;
    void *certificate;
    void *publicKey;
    in_addr_t currentIP;
    long long lastValidated;
  };

  struct anonymityRecord {
    struct anonymityRecord *next;
    in_addr_t ip;
    long long lastUsed;
  };

  IdentityTransportCallback *callback;
  bool allowAnonymousConnections;
  bool authenticateToRemote;
  struct packetInfo *receiveQueue;
  struct certificateRecord *head;
  struct anonymityRecord *anonList;
  NodeHandle *myHandle;
  int numActiveValidations;
  int numActiveCertRequests;
  int identifierSizeBytes;
  int signatureSizeBytes;
  Transport *transport;
  char dirname[200];

  virtual void freeCertificate(void *certificate) { assert(false); };
  virtual void freePublicKey(void *key) { assert(false); };
  virtual void loadCACertificate() = 0;
  virtual void *readCertificateFromStream(FILE *file) = 0;
  virtual void writeCertificateToStream(FILE *file, void *certificate) = 0;
  virtual bool verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature) = 0;
  virtual void *extractPublicKeyFromCertificate(void *certificate) = 0;
  virtual bool extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id) = 0;
  virtual void writeEndorsementToStream(FILE *file, void *certificate) { };

  void handleCertificateRequest(in_addr_t source, unsigned char *message, int msglen);
  void handleCertificateResponse(in_addr_t source, unsigned char *message, int msglen);
  bool certificateExists(Identifier *id);
  void addCertificate(Identifier *id, void *certificate);
  void *getCertificate(Identifier *id);
  struct certificateRecord *find(SimpleIdentifier *id, bool create = false);
  struct certificateRecord *findByIP(in_addr_t ip);
  struct anonymityRecord *findAnonByIP(in_addr_t ip);
  bool checkValidation(int idx, struct certificateRecord *rec);
  void deliverAnyQueuedMessages(in_addr_t ip, SimpleIdentifier *id);
  void cleanupReceiveQueue();
  void requestCertificateInternal(in_addr_t source, Identifier *id);
  void sendCertRequestMessage(in_addr_t source, Identifier *id);
  void resendCertRequests();
  void *getPublicKey(Identifier *id);

public:
  SimpleIdentityTransport(Transport *transport, int identifierSizeBits, const char *storeDir);
  virtual ~SimpleIdentityTransport();
  virtual bool init();
  virtual void recv(in_addr_t source, bool datagram, unsigned char *message, int msglen);
  virtual long long getTime() 
    { return transport->getTime(); };
  virtual long long send(NodeHandle *target, bool datagram, unsigned char *message, int msglen, int relevantlen = -1);
  virtual void scheduleTimer(TimerCallback *callback, int timerID, long long when)
    { transport->scheduleTimer(callback, timerID, when); };
  virtual void cancelTimer(TimerCallback *callback, int timerID)
    { transport->cancelTimer(callback, timerID); };
  virtual NodeHandle *getLocalHandle() 
    { return myHandle; };
  virtual int getSignatureSizeBytes() 
    { assert(signatureSizeBytes > 0); return signatureSizeBytes; };
  virtual int getMSS() 
    { return transport->getMSS(); };
  virtual void logEvent(char type, const void *entry, int size)
    { };
  virtual SimpleIdentifier *readIdentifier(unsigned char *buf, unsigned int *pos, unsigned int maxlen)
    { return SimpleIdentifier::read(buf, pos, maxlen, identifierSizeBytes*8); };
  virtual SimpleIdentifier *readIdentifierFromString(const char *str)
    { return SimpleIdentifier::readFromString(str, identifierSizeBytes*8); };
  virtual int getIdentifierSizeBytes()
    { return identifierSizeBytes; };
  virtual bool haveCertificate(Identifier *id) 
    { return (getPublicKey(id) != NULL); };
  virtual SimpleNodeHandle *readNodeHandle(unsigned char *buf, unsigned int *pos, unsigned int maxlen);
  virtual SimpleNodeHandle *readNodeHandleFromString(const char *str);
  virtual void requestCertificate(NodeHandle *source, Identifier *id);
  virtual void logText(const char *subsystem, int level, const char *format, ...);
  virtual void timerExpired(int timerID);
  virtual void investigate(NodeHandle *target, long long since) { };
  virtual int verify(Identifier *id, const unsigned char *data, const int dataLength, unsigned char *signature);
  virtual void incrementCounter(int code, int value) 
    { transport->incrementCounter(code, value); };
  void setCallback(IdentityTransportCallback *callback)
    { this->callback = callback; };
  virtual int getHashSizeBytes();
  virtual int getRandom(int range) { if (range<1) return 0; return ::random()%range; };
  virtual void hash(unsigned char *outbuf, const unsigned char *inbuf1, const int insize1, const unsigned char *inbuf2 = NULL, const int insize2 = 0, const unsigned char *inbuf3 = NULL, const int insize3 = 0, const unsigned char *inbuf4 = NULL, const int insize4 = 0);
  virtual void sendComplete(long long id);
};

#endif /* defined(__simple_h__) */
