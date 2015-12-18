#ifndef __x509_h__
#define __x509_h__

#include <stdio.h>
#include <openssl/pem.h>

#include "simple.h"

class X509Transport : public SimpleIdentityTransport {
protected:
  X509_STORE *caCerts;
  RSA *privateKey;

  virtual void freeCertificate(void *certificate) { X509_free((X509*)certificate); };
  virtual void freePublicKey(void *key) { RSA_free((RSA*)key); };
  virtual void loadCACertificate();
  virtual void *readCertificateFromStream(FILE *file);
  virtual void writeCertificateToStream(FILE *file, void *certificate);
  virtual void *extractPublicKeyFromCertificate(void *certificate);
  virtual bool verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature);
  virtual bool extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id);

  SimpleIdentifier *createIdentifier(const char *name);
  bool verifyCertificate(X509 *cert);

public:
  X509Transport(Transport *transport, int identifierSizeBits, const char *storeDir);
  virtual ~X509Transport();
  virtual void sign(const unsigned char *data, int dataLength, unsigned char *signatureBuffer)
    { assert(dataLength == 20); unsigned int siglen = signatureSizeBytes; RSA_sign(NID_sha1, data, dataLength, signatureBuffer, &siglen, privateKey); };
};

#endif /* defined(__x509_h__) */
