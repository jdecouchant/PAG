#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <openssl/sha.h>
#include <limits.h>

#include "peerreview/transport/id/simple.h"

#define SUBSYSTEM "simpletransport"
#define HASH_SIZE_BYTES 20

//#define log(a...) do { } while (0)
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)

extern FILE *logfile;

#define plog(x, a...) do { transport->logText(SUBSYSTEM, x, a); } while (0)
#define warning(a...) do { transport->logText("warning", 0, "WARNING: " a); } while (0)

static char *ipdisp(in_addr_t iaddr, char *buf)
{
  sprintf(buf, "%d.%d.%d.%d", 
    (int)(iaddr&0xFF), (int)((iaddr>>8)&0xFF), 
    (int)((iaddr>>16)&0xFF), (int)(iaddr>>24)
  );
  
  return buf;
}

SimpleIdentityTransport::SimpleIdentityTransport(Transport *transport, int identifierSizeBits, const char *storeDir)
{
  this->callback = NULL;
  this->receiveQueue = NULL;
  this->anonList = NULL;
  this->head = NULL;
  this->myHandle = NULL;
  this->numActiveValidations = 0;
  this->signatureSizeBytes = 0;
  this->transport = transport;
  this->identifierSizeBytes = (identifierSizeBits+7)/8;
  this->numActiveCertRequests = 0;
  this->allowAnonymousConnections = false;
  this->authenticateToRemote = true;
  strncpy(dirname, storeDir, sizeof(dirname));
  transport->scheduleTimer(this, TI_CLEANUP, transport->getTime() + PACKET_TIMEOUT_MICROS);
  transport->scheduleTimer(this, TI_CERT_REQUEST, transport->getTime() + CERT_REQUEST_TIMEOUT_MICROS);
}

SimpleIdentityTransport::~SimpleIdentityTransport()
{
  while (receiveQueue) {
    struct packetInfo *pi = receiveQueue;
    receiveQueue = receiveQueue->next;
    free(pi->message);
    free(pi);
  }
 
  assert(!head); // derived classes must clean up certificates on their own  
  
  for (int i=0; i<numActiveValidations; i++) {
    delete validation[i].certificateID;
    if (validation[i].lastResponse)
      free(validation[i].lastResponse);
  }
  
  for (int i=0; i<numActiveCertRequests; i++)
    delete certRequest[i].id;
  
  delete myHandle;
  delete callback;
}

bool SimpleIdentityTransport::init()
{
  char buf1[200], buf2[200];
  assert(!myHandle);

  /* Read peer certificates */

  char dirnamebuf[256]; 
  sprintf(dirnamebuf, "%s/peers/", dirname);
  DIR *dir = opendir(dirnamebuf);
  if (!dir) {
    if (mkdir(dirnamebuf, 0755) < 0)
      return false;
    if ((dir = opendir(dirnamebuf)) == NULL)
      return false;
  }
    
  loadCACertificate();

  struct dirent *ent = readdir(dir);
  while (ent) {
    if (strstr(ent->d_name, ".cert")) {
      char namebuf[200];
      strncpy(namebuf, ent->d_name, sizeof(namebuf));
      strtok(namebuf, ".");
      SimpleIdentifier *id = SimpleIdentifier::readFromString(namebuf, identifierSizeBytes*8);
      
      if (!certificateExists(id)) {
        sprintf(namebuf, "%s/peers/%s.cert", dirname, id->render(buf1));
        FILE *infile = fopen(namebuf, "r");
        if (!infile)
          panic("Cannot read certificate file '%s'", namebuf);
        
        void *cert = readCertificateFromStream(infile);
        if (!cert)
          panic("Cannot parse certificate in '%s'", namebuf);
        
        fclose(infile);
        addCertificate(id, cert);
      }
      
      delete id;
    }
    
    ent = readdir(dir);
  }
  
  closedir(dir);
  
  return true;
}

void SimpleIdentityTransport::logText(const char *subsystem, int level, const char *format, ...)
{
  char buffer[4096];
  va_list ap;
  va_start (ap, format);
  if (vsnprintf (buffer, sizeof(buffer), format, ap) > (int)sizeof(buffer))
    panic("Buffer overflow in SimpleTransport::logText()");
  transport->logText(subsystem, level, "%s", buffer);
  va_end (ap);
}

SimpleNodeHandle *SimpleIdentityTransport::readNodeHandle(unsigned char *buf, unsigned int *pos, unsigned int maxlen)
{
  SimpleIdentifier *identifier = readIdentifier(buf, pos, maxlen);
  in_addr_t ip = 0;
  if (((*pos)+sizeof(in_addr_t))<=maxlen)
    ip = *(in_addr_t*)&buf[*pos];
    
  (*pos) += sizeof(ip);
  return ((*pos)<=maxlen) ? new SimpleNodeHandle(identifier, ip) : NULL;
}

SimpleNodeHandle *SimpleIdentityTransport::readNodeHandleFromString(const char *str)
{
  char buf[200];
  char *ptr;
  strncpy(buf, str, sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  char *sid = strtok_r(buf, "/", &ptr);
  char *sip = sid ? strtok_r(NULL, "/", &ptr) : NULL;
  if (!sip)
    return NULL;
  
  return new SimpleNodeHandle(SimpleIdentifier::readFromString(sid, identifierSizeBytes*8), inet_addr(sip));
}

void SimpleIdentityTransport::timerExpired(int timerID)
{
  switch (timerID) {
    case TI_CLEANUP:
      cleanupReceiveQueue();
      transport->scheduleTimer(this, TI_CLEANUP, transport->getTime() + PACKET_TIMEOUT_MICROS);
      break;
    case TI_CERT_REQUEST:
      resendCertRequests();
      transport->scheduleTimer(this, TI_CERT_REQUEST, transport->getTime() + CERT_REQUEST_TIMEOUT_MICROS);
      break;
    default:
      panic("Unknown timer #%d expired in SimpleIdentityTransport", timerID);
  }
}

void *SimpleIdentityTransport::getCertificate(Identifier *id)
{
  struct certificateRecord *rec = find((SimpleIdentifier*)id);
  if (rec)
    return rec->certificate;
    
  return NULL;
}

bool SimpleIdentityTransport::certificateExists(Identifier *id)
{
  return (getCertificate(id) != NULL);
}

void SimpleIdentityTransport::deliverAnyQueuedMessages(in_addr_t ip, SimpleIdentifier *id)
{
  SimpleNodeHandle *handle = new SimpleNodeHandle(id->clone(), ip);
  
  while (receiveQueue && (receiveQueue->source == ip)) {
    struct packetInfo *pi = receiveQueue;
    receiveQueue = receiveQueue->next;
    callback->receive(handle, false, pi->message, pi->msglen);
    free(pi->message);
    free(pi);
  }
  
  struct packetInfo *iter = receiveQueue;
  while (iter && iter->next) {
    if (iter->next->source == ip) {
      struct packetInfo *pi = iter->next;
      iter->next = iter->next->next;
      callback->receive(handle, false, pi->message, pi->msglen);
      free(pi->message);
      free(pi);
    } else {
      iter = iter->next;
    }
  }
  
  delete handle;
}

void SimpleIdentityTransport::cleanupReceiveQueue()
{
  long long now = transport->getTime();
  char buf1[256];
  
  while (receiveQueue && (receiveQueue->timeout < now)) {
    struct packetInfo *pi = receiveQueue;
    warning("Timing out packet from %s", ipdisp(pi->source, buf1));
    receiveQueue = receiveQueue->next;
    free(pi->message);
    free(pi);
  }
  
  struct packetInfo *iter = receiveQueue;
  while (iter && iter->next) {
    if (iter->next->timeout < now) {
      struct packetInfo *pi = iter->next;
      warning("Timing out packet from %s", ipdisp(pi->source, buf1));
      iter->next = iter->next->next;
      free(pi->message);
      free(pi);
    } else {
      iter = iter->next;
    }
  }
}

void SimpleIdentityTransport::requestCertificate(NodeHandle *source, Identifier *id)
{
  requestCertificateInternal(((SimpleNodeHandle*)source)->getIP(), id);
}

void SimpleIdentityTransport::sendCertRequestMessage(in_addr_t source, Identifier *id)
{
  unsigned char request[1+id->getSizeBytes()];
  unsigned int size = 0;
  char buf1[256], buf2[256];

  plog(2, "Requesting certificate for %s from %s", id->render(buf1), ipdisp(source, buf2));
  request[size++] = MSG_CERTIFICATE_REQUEST;
  id->write(request, &size, sizeof(request));
  assert(size <= sizeof(request));
  
  transport->send(source, false, request, size);
}

void SimpleIdentityTransport::requestCertificateInternal(in_addr_t source, Identifier *id)
{
  char buf1[256], buf2[256];

  for (int i=0; i<numActiveCertRequests; i++) {
    if (/*(certRequest[i].source == source) && */id->equals(certRequest[i].id)) {
      plog(2, "Certificate request for %s from %s is already under way", id->render(buf1), ipdisp(source, buf2));
      return;
    }
  }
  
  if (numActiveCertRequests >= MAX_ACTIVE_CERT_REQUESTS)
    panic("Too many active certificate requests");
    
  certRequest[numActiveCertRequests].source = source;
  certRequest[numActiveCertRequests].id = id->clone();
  certRequest[numActiveCertRequests].attemptsSoFar = 0;
  numActiveCertRequests ++;
  
  sendCertRequestMessage(source, id);
}

void SimpleIdentityTransport::resendCertRequests()
{
  if (numActiveCertRequests == 0)
    return;
    
  plog(2, "Resending %d certificate request(s)", numActiveCertRequests);
  
  for (int i=0; i<numActiveCertRequests; i++) {
    sendCertRequestMessage(certRequest[i].source, certRequest[i].id);
    certRequest[i].attemptsSoFar ++;
  }
}

bool SimpleIdentityTransport::checkValidation(int idx, struct certificateRecord *rec)
{
  assert((0<=idx) && (idx<numActiveValidations) && validation[idx].lastResponse);
  assert(rec && rec->publicKey);
  
  unsigned char signedHash[HASH_SIZE_BYTES];
  hash(signedHash, (unsigned char*)&validation[idx].challenge, sizeof(validation[idx].challenge));

  if (verifyInternal(rec->publicKey, signedHash, HASH_SIZE_BYTES, validation[idx].lastResponse)) {
    rec->currentIP = validation[idx].lastAddress;
    rec->lastValidated = transport->getTime();
    free(validation[idx].lastResponse);
    delete validation[idx].certificateID;
    validation[idx] = validation[--numActiveValidations];
    
    char buf1[256], buf2[256];
    plog(2, "Identity of %s validated (now at %s)", rec->id->render(buf2), ipdisp(rec->currentIP, buf1));
    deliverAnyQueuedMessages(rec->currentIP, rec->id);
    return true;
  } 
  
  warning("Identity of %08X could not be validated", rec->id);
  return false;
}

void SimpleIdentityTransport::recv(in_addr_t source, bool datagram, unsigned char *message, int msglen)
{
  char buf1[256], buf2[256];
  
//  log(2, "Incoming %s from %s (%d bytes)", datagram ? "DATAGRAM" : "MESSAGE", ipdisp(source, buf1), msglen);

  if (msglen < 1) {
    warning("Truncated message received (%d bytes); discarding", msglen);
    return;
  }

  if (datagram) {
    SimpleNodeHandle *handle = new SimpleNodeHandle(new SimpleIdentifier(identifierSizeBytes*8, NULL), source);
    callback->receive(handle, true, message, msglen);
    delete handle;
    return;
  }
  
  switch (message[0]) {
    case MSG_CERTIFICATE_REQUEST :
    {
      handleCertificateRequest(source, message, msglen);
      return;
    }
    case MSG_CERTIFICATE_RESPONSE :
    {
      handleCertificateResponse(source, message, msglen);
      return;
    }
    case MSG_ID_CHALLENGE :
    {
      if (msglen != (1+sizeof(long long))) {
        warning("Truncated ID challenge (%d bytes); discarding", msglen);
        return;
      }

      if (authenticateToRemote) {
        plog(2, "Received ID challenge from %s [%lld]; responding", ipdisp(source, buf1), *(long long*)&message[1]);

        unsigned char response[1+MAX_ID_SIZE+signatureSizeBytes];
        unsigned int size = 0;

        response[size++] = MSG_ID_RESPONSE;
        myHandle->getIdentifier()->write(response, &size, sizeof(response));

        unsigned char signedHash[HASH_SIZE_BYTES];
        hash(signedHash, &message[1], sizeof(long long));

        sign(signedHash, HASH_SIZE_BYTES, &response[size]);
        size += signatureSizeBytes;

        transport->send(source, false, response, size);
      } else {
        plog(2, "Received ID challenge from %s; refusing", ipdisp(source, buf1));

        unsigned char response[1] = { MSG_ID_REFUSAL };
        transport->send(source, false, response, sizeof(response));
      }
      
      return;
    }
    case MSG_ID_RESPONSE :
    {
      unsigned int pos = 1;
      SimpleIdentifier *claimedID = readIdentifier(message, &pos, msglen);

      if (msglen != (int)(pos+signatureSizeBytes)) {
        warning("Truncated ID response (%d bytes); discarding", msglen);
        delete claimedID;
        return;
      }
      
      int idx = -1;
      for (int i=0; i<numActiveValidations; i++) {
        if (validation[i].lastAddress == source)
          idx = i;
      }
      
      if (idx < 0) {
        warning("Spurious ID response from %s; discarding", ipdisp(source, buf1));
        delete claimedID;
        return;
      }
      
      validation[idx].lastAddress = source;
      if (validation[idx].lastResponse)
        free(validation[idx].lastResponse);
      
      validation[idx].lastResponse = (unsigned char*) malloc(signatureSizeBytes);
      memcpy(validation[idx].lastResponse, &message[pos], signatureSizeBytes);
      
      struct certificateRecord *rec = find(claimedID, false);
      if (!rec || !rec->publicKey) {
        plog(2, "Received ID response from %s, but certificate is missing; sending request", ipdisp(source, buf1));
        validation[idx].waitingForCertificate = true;
        validation[idx].certificateID = claimedID->clone();
        requestCertificateInternal(source, claimedID);
        delete claimedID;
        return;
      }
      
      checkValidation(idx, rec);
      delete claimedID;
      return;
    }
    case MSG_ID_REFUSAL :
    {
      if (allowAnonymousConnections) {
        if (findByIP(source)) {
          warning("%s presented a certificate earlier, but is now refusing to authenticate -- ignoring", ipdisp(source, buf1));
          return;
        }
      
        struct anonymityRecord *item = anonList;
        while (item) {
          if (item->ip == source) {
            item->lastUsed = transport->getTime();
            return;
          }
          
          item = item->next;
        }

        item = (struct anonymityRecord*) malloc(sizeof(struct anonymityRecord));
        item->next = anonList;
        item->ip = source;
        item->lastUsed = transport->getTime();
        anonList = item;
        
        SimpleIdentifier *nullId = new SimpleIdentifier(identifierSizeBytes*8, NULL);
        deliverAnyQueuedMessages(source, nullId);
        delete nullId;
        return;
      }
      
      return;
    }
    default :
    {
      /* If we're allowing anonymous connections and the remote node refused to reveal its identity,
         we deliver the message with a zero-ID node handle */
    
      if (allowAnonymousConnections) {
        struct anonymityRecord *rec = findAnonByIP(source);
        if (rec) {
          rec->lastUsed = transport->getTime();
          SimpleNodeHandle *handle = new SimpleNodeHandle(new SimpleIdentifier(identifierSizeBytes*8, NULL), source);
          callback->receive(handle, false, message, msglen);
          delete handle;
          return;
        }
      }
    
      /* Otherwise... */
    
      struct certificateRecord *rec = findByIP(source);
      long long lastAcceptable = transport->getTime() - VALIDATION_INTERVAL_MICROS;
      if (!rec || (rec->lastValidated < lastAcceptable)) {
        struct packetInfo *pi = (struct packetInfo*) malloc(sizeof(struct packetInfo));
        pi->source = source;
        pi->timeout = transport->getTime() + PACKET_TIMEOUT_MICROS;
        pi->message = (unsigned char*) malloc(msglen);
        pi->msglen = msglen;
        pi->next = NULL;
        memcpy(pi->message, message, msglen);
        
        if (receiveQueue) {
          struct packetInfo *iter = receiveQueue;
          while (iter->next)
            iter = iter->next;
            
          iter->next = pi;
        } else {
          receiveQueue = pi;
        }
        
        int idx = -1;
        for (int i=0; i<numActiveValidations; i++) {
          if (validation[i].lastAddress == source)
            idx = i;
        }
        
        if (idx < 0) {
          if (numActiveValidations >= MAX_ACTIVE_VALIDATIONS) {
            warning("Validation buffer overflow; replacing random entry");            
            idx = random() % numActiveValidations;
            if (validation[idx].lastResponse) {
              free(validation[idx].lastResponse);
              validation[idx].lastResponse = NULL;
            }
          } else {
            idx = numActiveValidations ++;
          }
        
          validation[idx].challenge = random();
          validation[idx].waitingForCertificate = false;
          validation[idx].certificateID = 0;
          validation[idx].lastAddress = 0;
          validation[idx].lastResponse = NULL;
        }
        
        if (validation[idx].lastAddress != source) {
          plog(2, "Sending ID challenge to %s", ipdisp(source, buf1));
          validation[idx].lastAddress = source;
        
          unsigned char request[1+sizeof(long long)];
          request[0] = MSG_ID_CHALLENGE;
          memcpy(&request[1], (unsigned char*)&validation[idx].challenge, sizeof(long long));
          transport->send(source, false, request, sizeof(request));
        }
        
        return;
      }
      
      SimpleNodeHandle *handle = new SimpleNodeHandle(rec->id->clone(), source);
      callback->receive(handle, false, message, msglen);
      delete handle;
      return;
    }
  }
}

struct SimpleIdentityTransport::certificateRecord *SimpleIdentityTransport::find(SimpleIdentifier *id, bool create)
{
  struct certificateRecord *iter = head;

  while (iter != NULL) {
    if (iter->id->equals(id))
      return iter;
      
    iter = iter->next;
  }
  
  if (create) {
    struct certificateRecord *rec = (struct certificateRecord*) malloc(sizeof(struct certificateRecord));
    rec->next = head;
    rec->id = id->clone();
    rec->certificate = NULL;
    rec->publicKey = NULL;
    rec->currentIP = 0;
    rec->lastValidated = LONG_LONG_MIN;
    head = rec;
    return rec;
  } else {
    return NULL;
  }
}

struct SimpleIdentityTransport::certificateRecord *SimpleIdentityTransport::findByIP(in_addr_t ip)
{
  struct certificateRecord *iter = head;

  while (iter != NULL) {
    if (iter->currentIP == ip)
      return iter;
      
    iter = iter->next;
  }
  
  return NULL;
}

struct SimpleIdentityTransport::anonymityRecord *SimpleIdentityTransport::findAnonByIP(in_addr_t ip)
{
  struct anonymityRecord *iter = anonList;

  while (iter != NULL) {
    if (iter->ip == ip)
      return iter;
      
    iter = iter->next;
  }
  
  return NULL;
}

void *SimpleIdentityTransport::getPublicKey(Identifier *id)
{
  struct certificateRecord *rec = find((SimpleIdentifier*)id);
  if (rec)
    return rec->publicKey;
    
  return NULL;
}

int SimpleIdentityTransport::verify(Identifier *id, const unsigned char *data, const int dataLength, unsigned char *signature)
{
  assert(dataLength == HASH_SIZE_BYTES);

  void *pubkey = getPublicKey(id);
  if (!pubkey)
    return NO_CERTIFICATE;
    
  if (verifyInternal(pubkey, data, dataLength, signature))
    return SIGNATURE_OK;
    
  return SIGNATURE_BAD;
}

long long SimpleIdentityTransport::send(NodeHandle *target, bool datagram, unsigned char *message, int msglen, int relevantlen)
{
  char buf1[256];
  assert(datagram || ((message[0] != MSG_CERTIFICATE_REQUEST) && (message[0] != MSG_CERTIFICATE_RESPONSE)));
  assert(datagram || ((message[0] != MSG_ID_CHALLENGE) && (message[0] != MSG_ID_RESPONSE)));
  
//  logText(2, "Send %s to %s (%d/%d bytes)", datagram ? "DATAGRAM" : "MESSAGE", target->render(buf1), relevantlen, msglen);

  return transport->send(((SimpleNodeHandle*)target)->getIP(), datagram, message, msglen); 
};

void SimpleIdentityTransport::addCertificate(Identifier *id, void *certificate)
{
  char buf1[200];

  if (certificateExists(id)) {
    freeCertificate(certificate);
    return;
  }
  
  struct certificateRecord *rec = find((SimpleIdentifier*)id, true);
  rec->certificate = certificate;
  rec->publicKey = extractPublicKeyFromCertificate(certificate);

  if (dirname[0]) {
    char namebuf[200], buf1[200];
    sprintf(namebuf, "%s/peers/%s.cert", dirname, id->render(buf1));

    FILE *outfile = fopen(namebuf, "w+");
    if (!outfile)
      panic("Cannot create certificate file '%s' (%s)", namebuf, strerror(errno));

    writeCertificateToStream(outfile, certificate);
    fclose(outfile);
  }
}

void SimpleIdentityTransport::handleCertificateRequest(in_addr_t source, unsigned char *message, int msglen)
{
  char buf1[256], buf2[256];
  assert((msglen>0) && (message[0] == MSG_CERTIFICATE_REQUEST));
  
  unsigned int pos = 1;  
  Identifier *id = readIdentifier(message, &pos, msglen);
  plog(1, "Received certificate request for <%s> from %s", id->render(buf1), ipdisp(source, buf2));

  if (!authenticateToRemote) {
    warning("In anonymous mode - declining certificate request");
    return;
  }

  void *certificate = getCertificate(id);
  if (!certificate) {
    warning("We don't have that certificate; ignoring request");
    delete id;
    return;
  }    
  
  FILE *tmp = tmpfile();
  writeCertificateToStream(tmp, certificate);
  
  if (!id->equals(myHandle->getIdentifier()))
    writeEndorsementToStream(tmp, certificate);
  
  int len = ftell(tmp);
  unsigned char *buffer = (unsigned char*)malloc(len+1);
  buffer[0] = MSG_CERTIFICATE_RESPONSE;
  fseek(tmp, 0, SEEK_SET);
  if (fread(&buffer[1], len, 1, tmp) != 1)
    panic("Cannot read back X.509 certificate");
  fclose(tmp);
  plog(2, "Returning %d-byte certificate block", len);
  transport->send(source, false, buffer, len+1);

  free(buffer);
  delete id;
}

void SimpleIdentityTransport::handleCertificateResponse(in_addr_t source, unsigned char *message, int msglen)
{
  char buf1[256];

  /* Certificate responses: If we don't already have the certificate, we add it to our library
     and then try to check and deliver any waiting messages in the receive queue */

  plog(1, "Received a %d-byte certificate from %s", msglen-1, ipdisp(source, buf1));
  void *certificate = NULL;
  SimpleIdentifier *id = NULL;
  bool worked = extractCertificateFromMessage(&message[1], msglen-1, &certificate, &id);
  if (!worked) {
    assert(!certificate && !id);
    warning("Cannot parse certificate; discarding");
    return;
  }
  
  /* Great! The certificate is good */
  
  for (int i=0; i<numActiveCertRequests; /**/) {
    if (certRequest[i].id->equals(id)) {
      delete certRequest[i].id;
      certRequest[i] = certRequest[--numActiveCertRequests];
    } else {
      i++;
    }
  }
  
  if (certificateExists(id)) {
    plog(2, "We already have the certificate for <%s>", id->render(buf1));
    freeCertificate(certificate);
    delete id;
    return;
  }

  plog(2, "Adding certificate for <%s> to database", id->render(buf1));
  addCertificate(id, certificate);
  
  bool madeProgress = true;
  while (madeProgress) {
    madeProgress = false;
    for (int i=0; i<numActiveValidations; i++) {
      if (validation[i].waitingForCertificate && (id->equals(validation[i].certificateID))) {
        struct certificateRecord *rec = find(id);
        assert(rec && rec->publicKey);
        if (checkValidation(i, rec))
          madeProgress = true;
      }
    }
  }
  
  if (callback)
    callback->notifyCertificateAvailable(id);
    
  delete id;
}

void SimpleIdentityTransport::hash(unsigned char *outbuf, const unsigned char *inbuf1, const int insize1, const unsigned char *inbuf2, const int insize2, const unsigned char *inbuf3, const int insize3, const unsigned char *inbuf4, const int insize4)
{
  SHA_CTX c;
  SHA1_Init(&c);
  SHA1_Update(&c, inbuf1, insize1);
  if (inbuf2)
    SHA1_Update(&c, inbuf2, insize2);
  if (inbuf3)
    SHA1_Update(&c, inbuf3, insize3);
  if (inbuf4)
    SHA1_Update(&c, inbuf4, insize4);
  SHA1_Final((unsigned char*)outbuf, &c);
}

int SimpleIdentityTransport::getHashSizeBytes()
{
  return HASH_SIZE_BYTES;
}

void SimpleIdentityTransport::sendComplete(long long id)
{
}
