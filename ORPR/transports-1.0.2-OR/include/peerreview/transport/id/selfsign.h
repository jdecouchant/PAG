#ifndef __selfsign_h__
#define __selfsign_h__

#include <stdio.h>
#include <openssl/pem.h>

#include "simple.h"

class SelfSignTransport : public SimpleIdentityTransport {
protected:
  static const int GOSSIP_INTERVAL_MICROS = 5000000;
  static const int MAX_RECENT_IPS = 10;
  static const int TI_GOSSIP = 2;

  void *nodeCertificate;
  RSA *privateKey;
  in_addr_t recentIPs[MAX_RECENT_IPS];
  int numRecentIPs;

  virtual void freeCertificate(void *certificate) { free(certificate); };
  virtual void freePublicKey(void *key) { RSA_free((RSA*)key); };
  virtual void loadCACertificate();
  virtual void *readCertificateFromStream(FILE *file);
  virtual void writeCertificateToStream(FILE *file, void *certificate);
  virtual void *extractPublicKeyFromCertificate(void *certificate);
  virtual bool verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature);
  virtual bool extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id);
  virtual void writeEndorsementToStream(FILE *file, void *certificate);

  SimpleIdentifier *createIdentifier(X509 *cert);
  void statCertificate(void *cert, unsigned char **buf, int *len);
  void gossip();

public:
  SelfSignTransport(Transport *transport, int identifierSizeBits, const char *storeDir);
  virtual ~SelfSignTransport();
  virtual bool init();
  virtual void sign(const unsigned char *data, int dataLength, unsigned char *signatureBuffer)
    { assert(dataLength == 20); assert(privateKey); unsigned int siglen = signatureSizeBytes; RSA_sign(NID_sha1, data, dataLength, signatureBuffer, &siglen, privateKey); };
  virtual void timerExpired(int timerID);
  virtual long long send(NodeHandle *target, bool datagram, unsigned char *message, int msglen, int relevantlen = -1);
  virtual void recv(in_addr_t source, bool datagram, unsigned char *message, int msglen);
  void acceptAnonymousConnections() { this->allowAnonymousConnections = true; };
  void remainAnonymous() { this->authenticateToRemote = false; };
};

#endif /* defined(__selfsign_h__) */
