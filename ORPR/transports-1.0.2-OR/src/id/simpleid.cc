#include "peerreview/transport/id/x509.h"

SimpleIdentifier::SimpleIdentifier(int sizeBits, const unsigned char *material) 
{
  assert(sizeBits <= (MAX_BYTES*8));
  this->sizeBits = sizeBits;
  memset(raw, 0, sizeof(raw));
  if (material)
    memcpy(raw, material, (sizeBits+7)/8);
}

bool SimpleIdentifier::equals(Identifier *other)
{
  SimpleIdentifier *peer = (SimpleIdentifier*) other;
  if (peer->sizeBits != this->sizeBits)
    return false;
    
  int sizeBytes = getSizeBytes();
  for (int i=0; i<sizeBytes; i++) {
    if (peer->raw[i] != this->raw[i])
      return false;
  }
  
  return true;
}

SimpleIdentifier *SimpleIdentifier::clone()
{  
  return new SimpleIdentifier(sizeBits, raw);
}

char *SimpleIdentifier::render(char *buf)
{
  int sizeNibbles = getSizeBytes()*2;
  for (int i=0; i<sizeNibbles; i++) 
    buf[i] = "0123456789ABCDEF"[(((unsigned char*)&raw)[i/2] >> ((i%2) ? 0 : 4))&0xF];

  buf[sizeNibbles] = 0;
  return buf;
}

bool SimpleIdentifier::write(unsigned char *buf, unsigned int *ptr, unsigned int maxlen)
{
  int sizeBytes = getSizeBytes();
  if (((*ptr)+sizeBytes) > maxlen) 
    return false;
    
  for (int i=0; i<sizeBytes; i++)
    buf[(*ptr)+i] = raw[i];
  (*ptr) += sizeBytes;
  return true;
}

int SimpleIdentifier::getSizeBytes()
{
  return (sizeBits+7)/8;
}

SimpleIdentifier *SimpleIdentifier::read(const unsigned char *buf, unsigned int *ptr, unsigned int maxlen, int sizeBits)
{ 
  SimpleIdentifier *result = new SimpleIdentifier(sizeBits, &buf[*ptr]);
  (*ptr) += result->getSizeBytes();
  return result;
}

SimpleIdentifier *SimpleIdentifier::readFromString(const char *str, int sizeBits)
{
  unsigned char material[(sizeBits+7)/8];
  char sbuf[3];
  sbuf[2] = 0;
  int slen = strlen(str);
  for (int i=0; i<(int)sizeof(material); i++) {
    sbuf[0] = ((2*i)<slen) ? str[2*i] : '0';
    sbuf[1] = ((2*i+1)<slen) ? str[2*i+1] : '0';
    material[i] = strtoul(sbuf, NULL, 16);
  }
  
  return new SimpleIdentifier(sizeBits, material);
}

static void computeDiff(int sizeBytes, unsigned char *a, unsigned char *b, unsigned char *result) // a is bigger
{
  unsigned char carry = 0;
  for (int n=sizeBytes-1; n>=0; n--) {
    unsigned char u = a[n];
    unsigned char l = b[n] + carry;
    carry = (u<l) ? 1 : 0;
    result[n] = u-l;
  }
}

static void computeAbsDiff(int sizeBytes, const unsigned char *a, const unsigned char *b, unsigned char *result)
{
  unsigned char diff1[sizeBytes], diff2[sizeBytes];

  unsigned char carry1 = 0, carry2 = 0;
  for (int n=sizeBytes-1; n>=0; n--) {
    unsigned int u1 = a[n];
    unsigned int l1 = b[n] + carry1;
    carry1 = (u1<l1) ? 1 : 0;
    diff1[n] = u1-l1;

    unsigned int u2 = b[n];
    unsigned int l2 = a[n] + carry2;
    carry2 = (u2<l2) ? 1 : 0;
    diff2[n] = u2-l2;
  }

  int c = 0;
  while ((c<(sizeBytes - 1)) && (diff1[c]==diff2[c]))
    c++;

  const unsigned char *smaller = (diff1[c]>diff2[c]) ? diff2 : diff1;
  for (int i=0; i<sizeBytes; i++)
    *result++ = *smaller++;
}


bool SimpleIdentifier::isBiggerThan(Identifier *other)
{
  SimpleIdentifier *otherSimple = (SimpleIdentifier*) other;
  int sizeBytes = getSizeBytes();
  unsigned char absdiff1[sizeBytes], absdiff2[sizeBytes];

  computeDiff(sizeBytes, raw, otherSimple->raw, absdiff1);
  computeDiff(sizeBytes, otherSimple->raw, raw, absdiff2);
  
  int c = 0;
  while ((c<(sizeBytes - 1)) && (absdiff1[c]==absdiff2[c]))
    c++;

  return absdiff1[c]<absdiff2[c];
}

SimpleIdentifier *SimpleIdentifier::makeIdHalfwayTo(Identifier *other)
{
  SimpleIdentifier *otherSimple = (SimpleIdentifier*) other;
  int sizeBytes = getSizeBytes();
  unsigned char dist[sizeBytes];

  computeDiff(sizeBytes, otherSimple->raw, raw, dist);

  int sbit = 0;
  for (int n=0; n<sizeBytes; n++) {
    int newSbit = dist[n]&1;
    dist[n] = ((sbit*0x100+dist[n])>>1)&0xFF;
    sbit = newSbit;
  }
  
  unsigned char newMaterial[sizeBytes];
  int carry = 0;
  for (int n=sizeBytes-1; n>=0; n--) {
    int r = raw[n] + dist[n] + carry;
    newMaterial[n] = r&0xFF;
    carry = r>>8;
  }
  
  return new SimpleIdentifier(sizeBits, newMaterial);
}

bool SimpleIdentifier::isBetween(Identifier *from, Identifier *to) 
{
  bool tGf = to->isBiggerThan(from);
  bool xGf = this->isBiggerThan(from);
  bool tGx = to->isBiggerThan(this);
  return (tGf && xGf && tGx) || (!tGf && (tGx || xGf));
}

bool SimpleIdentifier::isCloserThan(Identifier *basis, Identifier *other)
{
  SimpleIdentifier *simpleBasis = (SimpleIdentifier*) basis;
  SimpleIdentifier *simpleOther = (SimpleIdentifier*) other;
  int sizeBytes = getSizeBytes();
  unsigned char absdiff1[sizeBytes], absdiff2[sizeBytes];

  computeAbsDiff(sizeBytes, raw, simpleBasis->raw, absdiff1);
  computeAbsDiff(sizeBytes, simpleOther->raw, simpleBasis->raw, absdiff2);

  int c = 0;
  while ((c<(sizeBytes - 1)) && (absdiff1[c]==absdiff2[c]))
    c++;

  return absdiff1[c]<absdiff2[c];
}

int SimpleIdentifier::getDigit(int digitNr, int bitsPerDigit)
{
  assert(bitsPerDigit == 4);
  assert((0<=digitNr) && (digitNr<(sizeBits/4)));
  
  if ((digitNr&1)==0)
    return (raw[digitNr>>1]>>4)&0x0F;
  else
    return raw[digitNr>>1]&0x0F;
}

int SimpleIdentifier::commonPrefixLength(Identifier *other, int bitsPerDigit)
{
  int commonLength = 0;
  while ((commonLength<(sizeBits/bitsPerDigit)) && (getDigit(commonLength, bitsPerDigit)==other->getDigit(commonLength, bitsPerDigit)))
    commonLength ++;
    
  return commonLength;
}

int SimpleIdentifier::operator %(int modulo) 
{
  int sizeBytes = getSizeBytes();
  int remainder = 0;
  for (int i=0; i<sizeBytes; i++)
    remainder = (0x100*remainder + raw[i]) % modulo;
    
  return remainder;
}

unsigned char* SimpleIdentifier::get_raw_ptr(int *sz) {
	*sz = sizeBits;
	return raw;
}
