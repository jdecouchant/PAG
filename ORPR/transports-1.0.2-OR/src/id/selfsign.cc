#include "peerreview/transport/id/selfsign.h"

#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER<=0x0090701fL
#define openssl_ptr_type unsigned char**
#else
#define openssl_ptr_type const unsigned char**
#endif

#define SUBSYSTEM "selfsign"
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)
#define plog(x, a...) do { transport->logText(SUBSYSTEM, x, a); } while (0)
#define warning(a...) do { transport->logText("warning", 0, "WARNING: " a); } while (0)

SelfSignTransport::SelfSignTransport(Transport *transport, int identifierSizeBits, const char *storeDir) : SimpleIdentityTransport(transport, identifierSizeBits, storeDir)
{
  this->privateKey = NULL;
  this->nodeCertificate = NULL;
  this->numRecentIPs = 0;
}

SelfSignTransport::~SelfSignTransport()
{
  if (privateKey)
    RSA_free(privateKey);

  if (nodeCertificate)
    free(nodeCertificate);

  while (head) {
    struct certificateRecord *cr = head;
    head = head->next;
    freeCertificate(cr->certificate);
    freePublicKey(cr->publicKey);
    delete cr->id;
    free(cr);
  }
}

static char *ipdisp(in_addr_t iaddr, char *buf)
{
  sprintf(buf, "%d.%d.%d.%d", 
    (int)(iaddr&0xFF), (int)((iaddr>>8)&0xFF), 
    (int)((iaddr>>16)&0xFF), (int)(iaddr>>24)
  );
  
  return buf;
}

void SelfSignTransport::loadCACertificate()
{
  OpenSSL_add_all_algorithms();

  /* Load node certificate */
   
  char namebuf[200];
  sprintf(namebuf, "%s/nodecert.der", dirname);
  FILE *fp = fopen(namebuf, "r");
  if (!fp)
    panic("Cannot load node certificate from '%s'", namebuf);

  X509 *cert;
  if (!(cert = d2i_X509_fp(fp, NULL)))
    panic("Cannot read DER-encoded certificate from '%s'", namebuf);
    
  signatureSizeBytes = RSA_size(EVP_PKEY_get1_RSA(X509_get_pubkey(cert)));

  char buf[200];  
  X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf));
  plog(2, "Loaded self-signed certificate (%s)", buf);  

  SimpleIdentifier *certId = createIdentifier(cert);
  myHandle = new SimpleNodeHandle(certId, transport->getLocalIP());

  char buf1[200];
  plog(2, "My local node handle is %s", myHandle->render(buf1));

  fseek(fp, 0, SEEK_SET);
  nodeCertificate = readCertificateFromStream(fp);
  if (!nodeCertificate)
    panic("Cannot decode certificate (invalid endorsements?)");
    
  int totalLen = sizeof(int)+(*(int*)nodeCertificate);
  unsigned char *certificateCopy = (unsigned char*) malloc(totalLen);
  memcpy(certificateCopy, nodeCertificate, totalLen);
    
  addCertificate(myHandle->getIdentifier(), certificateCopy);
  X509_free(cert);
  fclose(fp);
  
  /* Load private key */

  if (authenticateToRemote) {
    sprintf(namebuf, "%s/nodekey.der", dirname);
    fp = fopen(namebuf, "r");
    if (!fp)
      panic("Cannot open node key '%s'", namebuf);

    privateKey = d2i_RSAPrivateKey_fp(fp, NULL);
    if (!privateKey)
      panic("Cannot read private key from '%s'", namebuf);

    signatureSizeBytes = RSA_size(privateKey);
    plog(2, "Node key is a %d-bit RSA key", signatureSizeBytes*8);
    fclose(fp);
  }
}

void SelfSignTransport::statCertificate(void *cert, unsigned char **buf, int *len)
{
  *buf = &(((unsigned char*)cert)[sizeof(int)]);
  *len = *(int*)cert;
}

void *SelfSignTransport::readCertificateFromStream(FILE *file)
{
  assert(file);
  
  fseek(file, 0, SEEK_END);
  int len = ftell(file);
  fseek(file, 0, SEEK_SET);
  unsigned char *buf = (unsigned char*) malloc(len+sizeof(int));
  *(int*)buf = len;
  fread(&buf[sizeof(int)], len, 1, file);
  
  unsigned char *ptr = &buf[sizeof(int)];
  X509 *cert = d2i_X509(NULL, (openssl_ptr_type)&ptr, len);
  if (!cert) {
    free(buf);
    return NULL;
  }
  
  X509_free(cert);
  return buf;
}

void SelfSignTransport::writeCertificateToStream(FILE *file, void *certificate)
{
  assert(file && certificate);
  unsigned char *buf = &(((unsigned char*)certificate)[sizeof(int)]);
  int len = *(int*)certificate;
  fwrite(buf, len, 1, file);
}

void SelfSignTransport::writeEndorsementToStream(FILE *file, void *certificate)
{
  assert(file && certificate && nodeCertificate && authenticateToRemote);
  
  unsigned char *cbuf, *ncbuf;
  int clen, nclen;
  statCertificate(nodeCertificate, &ncbuf, &nclen);
  statCertificate(certificate, &cbuf, &clen);

  unsigned char hashbuf[20];
  hash(hashbuf, cbuf, clen);

  unsigned char signature[signatureSizeBytes];
  sign(hashbuf, sizeof(hashbuf), signature);
  
  fwrite(ncbuf, nclen, 1, file);
  fwrite(signature, signatureSizeBytes, 1, file);
}

SimpleIdentifier *SelfSignTransport::createIdentifier(X509 *cert)
{
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digestSize = sizeof(digest);
  
  memset(digest, 0, sizeof(digest));
  if (!X509_digest(cert, EVP_sha1(), digest, &digestSize))
    panic("Cannot create digest of certificate");
  
  return new SimpleIdentifier(identifierSizeBytes * 8, digest);
}

void *SelfSignTransport::extractPublicKeyFromCertificate(void *certificate)
{
  unsigned char *buf = &(((unsigned char*)certificate)[sizeof(int)]);
  int len = *(int*)certificate;
  unsigned char *ptr = buf;
  
  X509 *x509 = d2i_X509(NULL, (openssl_ptr_type)&ptr, len);
  if (!x509)
    return NULL;

  EVP_PKEY *vkey = X509_get_pubkey(x509);
  if (!vkey)
    panic("Cannot extract public key from certificate");
      
  RSA *publicKey = EVP_PKEY_get1_RSA(vkey);
  if (!publicKey)
    panic("Cannot decode public key in certificate");

  EVP_PKEY_free(vkey);
  
  return publicKey;
}

bool SelfSignTransport::verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature)
{
  assert(dataLength == 20);

  return RSA_verify(NID_sha1, data, dataLength, signature, signatureSizeBytes, (RSA*)pubkey);
}

bool SelfSignTransport::extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id)
{
  *certificate = NULL;
  *id = NULL;
  
  unsigned char *ptr = message;
  X509 *x509 = d2i_X509(NULL, (openssl_ptr_type)&ptr, msglen);
  if (!x509) {
    warning("Cannot extract X.509 certificate from certificate response");
    return false;
  }
  
  char buf1[256];
  X509_NAME_oneline(X509_get_subject_name(x509), buf1, sizeof(buf1));
  plog(2, "Extracting certificate: %s", buf1);
    
  unsigned char *certbuf = (unsigned char*)malloc(msglen + sizeof(int));
  *(int*)certbuf = msglen;
  memcpy(&certbuf[sizeof(int)], message, msglen);

  int endorserIndex = 0;
  while (true) {
    int readSoFar = ptr - message;
    if (readSoFar >= msglen)
      break;
      
    bool validationOK = false;
    X509 *endorser = d2i_X509(NULL, (openssl_ptr_type)&ptr, msglen - readSoFar);
    if (endorser) {
      X509_NAME_oneline(X509_get_issuer_name(endorser), buf1, sizeof(buf1));
      plog(2, "Endorser #%d: %s", endorserIndex+1, buf1);
    
      EVP_PKEY *vkey = X509_get_pubkey(endorser);
      RSA *pubkey = vkey ? EVP_PKEY_get1_RSA(vkey) : NULL;
      if (pubkey) {
        int keylen = RSA_size(pubkey);
        unsigned char hashvalue[20];
        hash(hashvalue, message, readSoFar);
        
        unsigned char *ptr2 = &message[ptr-message];
        ptr += keylen;
        
        if (((readSoFar+keylen)<=msglen) && RSA_verify(NID_sha1, hashvalue, sizeof(hashvalue), ptr2, keylen, pubkey)) {
          validationOK = true;
        } else {
          warning("Cannot verify endorser #%d's signature on certificate", endorserIndex + 1);
        }
      } else {
        warning("Cannot extract public key from endorser #%d's certificate", endorserIndex + 1);
      }
    
      X509_free(endorser);
      endorserIndex ++;
    } else {
      warning("Cannot extract endorser #%d's certificate", endorserIndex + 1);
    }
    
    if (!validationOK) {
      free(certbuf);
      X509_free(x509);
      return false;
    }
  }

  plog(2, "Certificate verified OK");
  
  *id = createIdentifier(x509);
  *certificate = certbuf;
  X509_free(x509);
  return true;
}

void SelfSignTransport::gossip()
{
  if (numRecentIPs < 1) {
    plog(2, "Cannot gossip; no messages have been sent yet");
    return;
  }
  
  if (!head) {
    plog(2, "Cannot gossip; no certificates loaded");
    return;
  }

  struct certificateRecord *ptr = head;
  int numCertificates = 0;

  while (ptr) {
    numCertificates ++;
    ptr = ptr->next;
  }
  
  int whichCertificate = random() % numCertificates;
  ptr = head;
  while (whichCertificate > 0) {
    ptr = ptr->next;
    assert(ptr != NULL);
    whichCertificate --;
  }

  in_addr_t target = recentIPs[random() % numRecentIPs];
  Identifier *id = ptr->id;
  unsigned char offer[1+id->getSizeBytes()];
  unsigned int size = 0;
  char buf1[256], buf2[256];

  plog(2, "Offering certificate for %s to %s", id->render(buf1), ipdisp(target, buf2));
  offer[size++] = MSG_CERTIFICATE_OFFER;
  id->write(offer, &size, sizeof(offer));
  assert(size <= sizeof(offer));
  
  transport->send(target, false, offer, size);
}

void SelfSignTransport::timerExpired(int timerID)
{
  switch (timerID) {
    case TI_GOSSIP :
      if (authenticateToRemote)
        gossip();
        
      transport->scheduleTimer(this, TI_GOSSIP, transport->getTime() + ((GOSSIP_INTERVAL_MICROS/2) + (random()%GOSSIP_INTERVAL_MICROS)));
      break;
    default :
      SimpleIdentityTransport::timerExpired(timerID);
      break;
  }
}

long long SelfSignTransport::send(NodeHandle *target, bool datagram, unsigned char *message, int msglen, int relevantlen)
{
  in_addr_t targetIP = ((SimpleNodeHandle*)target)->getIP();

  bool haveIt = false;
  assert(numRecentIPs <= MAX_RECENT_IPS);
  for (int i=0; i<numRecentIPs; i++) {
    if (recentIPs[i] == targetIP)
      haveIt = true;
  }

  if (!haveIt) {
    if (numRecentIPs < MAX_RECENT_IPS) 
      recentIPs[numRecentIPs++] = targetIP;
    else
      recentIPs[random() % MAX_RECENT_IPS] = targetIP;
  }

  return SimpleIdentityTransport::send(target, datagram, message, msglen, relevantlen);
}

void SelfSignTransport::recv(in_addr_t source, bool datagram, unsigned char *message, int msglen)
{
  if (datagram || (msglen < 1) || (message[0] != MSG_CERTIFICATE_OFFER)) {
    SimpleIdentityTransport::recv(source, datagram, message, msglen);
    return;
  }

  char buf1[256], buf2[256];  
  unsigned int pos = 1;  
  Identifier *id = readIdentifier(message, &pos, msglen);
  plog(1, "Received certificate offer for <%s> from %s", id->render(buf1), ipdisp(source, buf2));

  if (!certificateExists(id)) {
    requestCertificateInternal(source, id);
  } else {
    plog(2, "We already have that certificate; declining...");
  }
  
  delete id;    
}

bool SelfSignTransport::init()
{
  bool result = SimpleIdentityTransport::init();
  transport->scheduleTimer(this, TI_GOSSIP, transport->getTime() + GOSSIP_INTERVAL_MICROS);
  return result;
}
