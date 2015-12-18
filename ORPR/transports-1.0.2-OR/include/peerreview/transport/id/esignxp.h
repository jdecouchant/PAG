#ifndef __esign_h__
#define __esign_h__

#include <stdlib.h>
#include <stdio.h>

#include "esign/esign.h"
#include "simple.h"

class ESignTransport : public SimpleIdentityTransport {
protected:
  bigint *ca_N;
  esign_priv *privateKey;
  int nbits;
  int k;

  virtual void freeCertificate(void *certificate) { free(certificate); };
  virtual void freePublicKey(void *key) { esign_pub *ep = (esign_pub*) key; delete ep; };
  virtual void loadCACertificate();
  virtual void *readCertificateFromStream(FILE *file);
  virtual void writeCertificateToStream(FILE *file, void *certificate);
  virtual void *extractPublicKeyFromCertificate(void *certificate);
  virtual bool verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature);
  virtual bool extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id);
  SimpleIdentifier *createIdentifier(const char *cn);

public:
  ESignTransport(Transport *transport, int identifierSizeBits, const char *storeDir);
  virtual ~ESignTransport();
  virtual void sign(const unsigned char *data, int dataLength, unsigned char *signatureBuffer);
};

#endif /* defined(__esign_h__) */
