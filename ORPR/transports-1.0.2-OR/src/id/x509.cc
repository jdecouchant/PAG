#include "peerreview/transport/id/x509.h"

#define SUBSYSTEM "x509"
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)
#define plog(x, a...) do { transport->logText(SUBSYSTEM, x, a); } while (0)
#define warning(a...) do { transport->logText("warning", 0, "WARNING: " a); } while (0)

X509Transport::X509Transport(Transport *transport, int identifierSizeBits, const char *storeDir) : SimpleIdentityTransport(transport, identifierSizeBits, storeDir)
{
  this->caCerts = NULL;
  this->privateKey = NULL;
}

X509Transport::~X509Transport()
{
  if (caCerts)
    X509_STORE_free(caCerts);
  if (privateKey)
    RSA_free(privateKey);

  while (head) {
    struct certificateRecord *cr = head;
    head = head->next;
    freeCertificate(cr->certificate);
    freePublicKey(cr->publicKey);
    delete cr->id;
    free(cr);
  }
}

void X509Transport::loadCACertificate()
{
  /* Load CA certificate */
  
  OpenSSL_add_all_algorithms();

  if (!(caCerts = X509_STORE_new())) 
    panic("Cannot create new X.509 store");

  char namebuf[200];    
  sprintf(namebuf, "%s/cacert.pem", dirname);
  if (X509_STORE_load_locations(caCerts, namebuf, NULL) != 1)
    panic("Cannot load CA certificate from '%s'", namebuf);

  if (X509_STORE_set_default_paths(caCerts) != 1)
    panic("Cannot set default paths in CA certificate");
  
  /* Load node certificate and verify it against CA certificate */
   
  sprintf(namebuf, "%s/nodecert.pem", dirname);
  FILE *fp = fopen(namebuf, "r");
  if (!fp)
    panic("Cannot load node certificate from '%s'", namebuf);

  X509 *cert;
  if (!(cert = PEM_read_X509(fp, NULL, NULL, NULL)))
    panic("Cannot read cert");
    
  fclose(fp);
  fp = NULL;
    
  if (!verifyCertificate(cert))
    panic("Node certificate '%s' cannot be verified", namebuf);
    
  char buf[200];  
  X509_NAME_oneline(X509_get_issuer_name(cert), buf, sizeof(buf));
  plog(2, "Loaded CA certificate (%s)", buf);
  X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf));
  plog(2, "Loaded node certificate (%s)", buf);  

  SimpleIdentifier *certId = createIdentifier(buf);
  myHandle = new SimpleNodeHandle(certId, transport->getLocalIP());

  char buf1[200];
  plog(2, "My local node handle is %s", myHandle->render(buf1));

  addCertificate(myHandle->getIdentifier(), cert);
  
  /* Load private key */

  sprintf(namebuf, "%s/nodekey.pem", dirname);
  fp = fopen(namebuf, "r");
  if (!fp)
    panic("Cannot open node key '%s'", namebuf);
    
  privateKey = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
  if (!privateKey)
    panic("Cannot read private key from '%s'", namebuf);
    
  signatureSizeBytes = RSA_size(privateKey);
  plog(2, "Node key is a %d-bit RSA key", signatureSizeBytes*8);
  fclose(fp);
}

void *X509Transport::readCertificateFromStream(FILE *file)
{
  assert(file);
  return PEM_read_X509(file, NULL, NULL, NULL);
}

void X509Transport::writeCertificateToStream(FILE *file, void *certificate)
{
  assert(file && certificate);
  PEM_write_X509(file, (X509*)certificate);
}

SimpleIdentifier *X509Transport::createIdentifier(const char *name)
{
  const char *cn = strstr(name, "CN=");
  if (!cn)
    return NULL;
    
  cn = cn+3;
  
  unsigned char material[200];
  memset(material, 0, sizeof(material));

  for (unsigned i=0; i<sizeof(material); i++) {
    if (!cn[2*i] || !cn[2*i+1] || (cn[2*i]=='/') || (cn[2*i+1]=='/'))
      break;
      
    char s[3] = { cn[2*i], cn[2*i+1], 0 };
    material[i] = strtol(s, 0, 16);
  }
  
  return new SimpleIdentifier(identifierSizeBytes * 8, material);
}

bool X509Transport::verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature)
{
  assert(dataLength == 20);

  return RSA_verify(NID_sha1, data, dataLength, signature, signatureSizeBytes, (RSA*)pubkey);
}

void *X509Transport::extractPublicKeyFromCertificate(void *certificate)
{
  EVP_PKEY *vkey = X509_get_pubkey((X509*)certificate);
  if (!vkey)
    panic("Cannot extract public key from certificate");
      
  RSA *publicKey = EVP_PKEY_get1_RSA(vkey);
  if (!publicKey)
    panic("Cannot decode public key in certificate");

  EVP_PKEY_free(vkey);
  
  return publicKey;
}

bool X509Transport::verifyCertificate(X509 *cert)
{
  X509_STORE_CTX ca_ctx;
  if (X509_STORE_CTX_init(&ca_ctx, caCerts, cert, NULL) != 1)
    panic("Cannot init store CTX");
  
  int result = X509_verify_cert(&ca_ctx);
  X509_STORE_CTX_cleanup(&ca_ctx);
  
  if (result != 1) {
    warning("Certificate verification failed (%s)", X509_verify_cert_error_string(ca_ctx.error));
    return false;
  }
  
  return true;
}

bool X509Transport::extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id)
{
  *certificate = NULL;
  *id = NULL;

  FILE *tmp = tmpfile();
  fwrite(message, msglen, 1, tmp);
  fseek(tmp, 0, SEEK_SET);
  X509 *cert = PEM_read_X509(tmp, NULL, NULL, NULL);
  fclose(tmp);
  
  if (!cert)
    return false;
  
  if (!verifyCertificate(cert)) {
    warning("Certificate is not signed by our CA; discarding");
    X509_free(cert);
    return false;
  }

  char buf1[256];  
  X509_NAME_oneline(X509_get_subject_name(cert), buf1, sizeof(buf1));
  plog(2, "Certificate verified OK [%s]", buf1);
  
  /* Great! The certificate is good */
  
  *certificate = cert;
  *id = createIdentifier(buf1);
  return true;
}
