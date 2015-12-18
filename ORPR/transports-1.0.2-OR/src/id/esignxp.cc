#include "peerreview/transport/id/esignxp.h"

#define SUBSYSTEM "esign"
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)
#define plog(x, a...) do { transport->logText(SUBSYSTEM, x, a); } while (0)
#define warning(a...) do { transport->logText("warning", 0, "WARNING: " a); } while (0)

ESignTransport::ESignTransport(Transport *transport, int identifierSizeBits, const char *storeDir) : SimpleIdentityTransport(transport, identifierSizeBits, storeDir)
{
  this->ca_N = NULL;
  this->privateKey = NULL;
  this->nbits = -1;
  this->k = -1;
}

ESignTransport::~ESignTransport()
{
  if (ca_N)
    delete ca_N;
  if (privateKey)
    delete privateKey;

  while (head) {
    struct certificateRecord *cr = head;
    head = head->next;
    freeCertificate(cr->certificate);
    freePublicKey(cr->publicKey);
    delete cr->id;
    free(cr);
  }
}

SimpleIdentifier *ESignTransport::createIdentifier(const char *cn)
{
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

void ESignTransport::loadCACertificate()
{
  /* Load CA certificate */

  char namebuf[200];    
  sprintf(namebuf, "%s/ca_pubkey.esign", dirname);
  plog(2, "Loading CA public key from %s", namebuf);

  FILE *infile = fopen(namebuf, "r");
  if (!infile)
    panic("Cannot read CA public key from '%s'", namebuf);
  
  char linebuf[2000];
  fgets(linebuf, sizeof(linebuf), infile);
  nbits = atoi(linebuf);
  if (nbits < 20)
    panic("Invalid nBits setting in '%s' (%d<20)", namebuf, nbits);
  fgets(linebuf, sizeof(linebuf), infile);
  k = atoi(linebuf);
  if (k < 4)
    panic("Invalid K setting in '%s' (%d<4)", namebuf, k);
    
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  ca_N = new bigint(linebuf, 16);
  fclose(infile);
  
  /* Load private key */
  
  sprintf(namebuf, "%s/nodekey.esign", dirname);
  plog(2, "Loading node key from %s", namebuf);

  infile = fopen(namebuf, "r");
  if (!infile)
    panic("Cannot read private key from '%s'", namebuf);
    
  fgets(linebuf, sizeof(linebuf), infile);
  if (atoi(linebuf) != nbits)
    panic("Private key in '%s' does not match length of CA key", namebuf);
    
  fgets(linebuf, sizeof(linebuf), infile);
  if (atoi(linebuf) != k)
    panic("Private key in '%s' does not match K value of CA key", namebuf);
  
  char nbuf[2000];
  fgets(nbuf, sizeof(nbuf), infile);
  strtok(nbuf, "\r\n");
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  bigint p (linebuf, 16);
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  bigint q (linebuf, 16);
  privateKey = new esign_priv(p, q, k);
  fclose(infile);
  
  /* Load node certificate and verify it against CA certificate */

  sprintf(namebuf, "%s/nodecert.esign", dirname);
  plog(2, "Loading node certificate from %s", namebuf);

  infile = fopen(namebuf, "r");
  if (!infile)
    panic("Cannot read node certificate from '%s'", namebuf);
  
  fgets(linebuf, sizeof(linebuf), infile);
  if (atoi(linebuf) != nbits)
    panic("Node certificate in '%s' does not match length of node key", namebuf);
    
  fgets(linebuf, sizeof(linebuf), infile);
  if (atoi(linebuf) != k)
    panic("Node certificate in '%s' does not match K value of node key", namebuf);
  
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  if (strcmp(linebuf, nbuf))
    panic("Node certificate in '%s' has different N value than node key", namebuf);
  
  char idbuf[2000];
  fgets(idbuf, sizeof(idbuf), infile);
  strtok(idbuf, "\r\n");

  char sigbuf[2000];
  fgets(sigbuf, sizeof(sigbuf), infile);
  strtok(sigbuf, "\r\n");
  
  fseek(infile, 0, SEEK_SET);
  void *mycert = readCertificateFromStream(infile);
  if (!mycert)
    panic("Cannot decode my own certificate (in %s/nodecert.esign)", dirname);
  
  fclose(infile);

  char snbits[200], sk[200];
  sprintf(snbits, "%d", nbits);
  sprintf(sk, "%d", k);
  
  unsigned char hashbuf[21];
  hash(hashbuf, (const unsigned char*)snbits, strlen(snbits), (const unsigned char*)sk, strlen(sk), (const unsigned char*)nbuf, strlen(nbuf), (const unsigned char*)idbuf, strlen(idbuf));
  hashbuf[0] &= 0x7F;
  
  char shash[41];
  for (int i=0; i<20; i++)
    sprintf(&shash[2*i], "%02x", hashbuf[i]);

  esign_pub ep (*ca_N, k);
  bigint hashv (shash, 16);
  bigint sigv (sigbuf, 16);

  plog(2, "Verifying node certificate");
  if (!ep.raw_verify(hashv, sigv))
    panic("Cannot verify node certificate '%s'", namebuf);

  myHandle = new SimpleNodeHandle(createIdentifier(idbuf), transport->getLocalIP());
  plog(2, "OK; my node handle is %s", myHandle->render(linebuf));
  addCertificate(myHandle->getIdentifier(), mycert);

  signatureSizeBytes = (nbits+7)/8;
}

void *ESignTransport::readCertificateFromStream(FILE *file)
{
  const int maxlen = 4096;
  char *certbuf = (char*) malloc(maxlen);
  certbuf[0] = 0;
  for (int i=0; i<5; i++) {
    int len = strlen(certbuf);
    if (len >= (maxlen-1)) {
      free(certbuf);
      return NULL;
    }
      
    char *s = fgets(&certbuf[len], (maxlen-1)-len, file);
    if (!s) {
      free(certbuf);
      return NULL;
    }
  }

  return certbuf;
}

void ESignTransport::writeCertificateToStream(FILE *file, void *certificate)
{
  fprintf(file, "%s", certificate);
}

static void bin2str(const unsigned char *data, int dataLength, char *buf)
{
  for (int i=0; i<dataLength; i++) {
    *(buf++) = "0123456789abcdef"[(data[i]>>4)&0x0F];
    *(buf++) = "0123456789abcdef"[data[i]&0x0F];
  }
  
  *buf = 0;
}

static void str2bin(const char *ssig, int numBytes, unsigned char *outBuf)
{
  int slen = 0;
  while ((slen<(2*numBytes)) && (((ssig[slen]>='0') && (ssig[slen]<='9')) || ((ssig[slen]>='a') && (ssig[slen]<='f'))))
    slen++;
  
  int zeroesToAdd = 2*numBytes-slen;
  for (int i=0; i<numBytes; i++) {
    char c1 = ((2*i)<zeroesToAdd) ? '0' : ssig[2*i-zeroesToAdd];
    char c2 = ((2*i+1)<zeroesToAdd) ? '0' : ssig[2*i+1-zeroesToAdd];
    int i1, i2;
    
    if ((c1>='0') && (c1<='9'))
      i1 = c1-'0';
    else if ((c1>='a') && (c1<='f'))
      i1 = 10+(c1-'a');
    else panic("Format error: str2bin(%s) at %d", ssig, i);
    
    if ((c2>='0') && (c2<='9'))
      i2 = c2-'0';
    else if ((c2>='a') && (c2<='f'))
      i2 = 10+(c2-'a');
    else panic("Format error: str2bin(%s) at %d", ssig, i);  
    
    *(outBuf++) = (i1*0x10)+i2;
  }
}

bool ESignTransport::verifyInternal(void *pubkey, const unsigned char *data, const int dataLength, unsigned char *signature)
{
  assert(pubkey && data && signature && (dataLength==20));
  esign_pub *ep = (esign_pub*) pubkey;
  
  char sdata[41], ssig[signatureSizeBytes*2+1];
  bin2str(data, 20, sdata);
  bin2str(signature, signatureSizeBytes, ssig);
  
  bigint hashv (sdata, 16);
  bigint sigv (ssig, 16);

  return ep->raw_verify(hashv, sigv);
}

void ESignTransport::sign(const unsigned char *data, int dataLength, unsigned char *signatureBuffer)
{
  assert(privateKey && (dataLength == 20));
  
  char shash[41];
  bin2str(data, 20, shash);
  
  bigint hashv (shash, 16);
  bigint sv = privateKey->raw_sign(hashv);
  
  char ssig[100+signatureSizeBytes*2];
  ssig[100+signatureSizeBytes*2-1] = 0;
  mpz_get_str(ssig, 16, &sv);
  assert(ssig[100+signatureSizeBytes*2-1] == 0);

  str2bin(ssig, signatureSizeBytes, signatureBuffer);
}

void *ESignTransport::extractPublicKeyFromCertificate(void *certificate)
{
  char *ccert = (char*) certificate;
  int p1 = 0;
  while (ccert[p1]!='\n')
    p1++;
  int p2 = p1+1;
  while (ccert[p2]!='\n')
    p2++;
  int p3 = p2+1;
  while (ccert[p3]!='\n')
    p3++;
    
  char ck[p2-p1+1];
  for (int i=0; i<(p2-p1); i++)
    ck[i] = ccert[p1+i+1];
  ck[p2-p1] = 0;
  
  char cN[p3-p2+1];
  for (int i=0; i<(p3-p2); i++)
    cN[i] = ccert[p2+i+1];
  cN[p3-p2] = 0;
  
  int key_k = atoi(ck);
  bigint key_N (cN, 16);
  
  return new esign_pub(key_N, key_k);
}

bool ESignTransport::extractCertificateFromMessage(unsigned char *message, int msglen, void **certificate, SimpleIdentifier **id)
{
  char *cert = (char*) malloc(msglen);
  memcpy(cert, message, msglen);
  
  *certificate = cert;
  int pbeg = 0;
  for (int i=0; i<3; i++) {
    while (cert[pbeg]!='\n')
      pbeg++;
    pbeg ++;
  }
  
  unsigned char idbuf[21];
  str2bin(&cert[pbeg], 20, idbuf);

  *certificate = cert;
  *id = new SimpleIdentifier(160, idbuf);
  
  return true;
}
