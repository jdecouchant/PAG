#ifndef __dummy_transport_h__
#define __dummy_transport_h__

#include "x509.h"

class DummyIdentityTransport : public X509Transport {
protected:
  virtual bool verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature)
  {
    assert(dataLength==20);
    if (memcmp(data, signature, dataLength))
      return false;
      
    for (int i=dataLength; i<signatureSizeBytes; i++) {
      if (signature[i] != 0xEE)
        return false;
    }
    
    return true;
  }

public:
  DummyIdentityTransport(Transport *transport, int identifierSizeBits, const char *storeDir) : X509Transport(transport, identifierSizeBits, storeDir) {};
  virtual void sign(const unsigned char *data, int dataLength, unsigned char *signatureBuffer)
  { 
    assert(dataLength == 20); 
    assert(signatureSizeBytes>=dataLength); 
    memcpy(signatureBuffer, data, dataLength); 
    memset(&signatureBuffer[dataLength], 0xEE, signatureSizeBytes-dataLength);
  };
};

#endif /* defined(__dummy_transport_h__) */
