#include <openssl/x509.h>
#include <openssl/err.h>

#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); exit(1); } while (0)

int main(int argc, char *argv[])
{
  OpenSSL_add_all_algorithms();
  if (argc != 2)
    panic("Syntax: readcert <certchain>");
    
  FILE *infile = fopen(argv[1], "r");
  if (!infile)
    panic("Cannot open '%s'", argv[1]);
    
  X509 *orig = d2i_X509_fp(infile, NULL);
  if (!orig)
    panic("Cannot read original X.509 certificate");
    
  char buf1[256];
  printf("--- Certificate\n");
  X509_NAME_oneline(X509_get_issuer_name(orig), buf1, sizeof(buf1));
  printf("  Issuer: %s\n", buf1);
  X509_NAME_oneline(X509_get_subject_name(orig), buf1, sizeof(buf1));
  printf("  Subject: %s\n", buf1);
  
  int bytesSoFar = ftell(infile);
  fseek(infile, 0, SEEK_END);
  int bytesTotal = ftell(infile);
  fseek(infile, bytesSoFar, SEEK_SET);
  
  int numEndorsements = 0;
  while (bytesSoFar < bytesTotal) {
    char *soFarBuf = (char*) malloc(bytesSoFar);
    fseek(infile, 0, SEEK_SET);
    fread(soFarBuf, bytesSoFar, 1, infile);
    unsigned char hash[20];
    SHA_CTX c;
    SHA1_Init(&c);
    SHA1_Update(&c, soFarBuf, bytesSoFar);
    SHA1_Final(hash, &c);
    
    printf("  Hash so far: [");
    for (int i=0; i<20; i++)
      printf("%s%02X", (i==0) ? "" : ":", hash[i]);
    printf("] @%d\n", bytesSoFar);

    for (int i=0; i<20; i++)
      printf("%s%02X", (i==0) ? "" : ":", hash[i]);
    printf("] @%d\n", bytesSoFar);
    
    
    printf("--- Endorsement #%d\n", ++numEndorsements);
    X509 *endorser = d2i_X509_fp(infile, NULL);
    if (!endorser)
      panic("Cannot read endorsement certificate");
      
    X509_NAME_oneline(X509_get_issuer_name(endorser), buf1, sizeof(buf1));
    printf("  Issuer: %s\n", buf1);
    X509_NAME_oneline(X509_get_subject_name(endorser), buf1, sizeof(buf1));
    printf("  Subject: %s\n", buf1);
    
    EVP_PKEY *vkey = X509_get_pubkey(endorser);
    if (!vkey)
      panic("Cannot extract public key");

    RSA *pubkey = EVP_PKEY_get1_RSA(vkey);
    if (!pubkey)
      panic("Cannot convert public key to RSA");
      
    unsigned char sig[RSA_size(pubkey)];
    fread(sig, sizeof(sig), 1, infile);
    printf("  Signature: [");
    for (int i=0; i<(int)sizeof(sig); i++)
      printf("%s%02X", (i==0) ? "" : ":", sig[i]);
    printf("]\n");
    
    if (RSA_verify(NID_sha1, hash, sizeof(hash), sig, sizeof(sig), pubkey))
      printf("  Verified OK\n");
    else
      printf("  *** BAD SIGNATURE *** (%s)\n", ERR_error_string(ERR_get_error(), buf1));
    
    free(soFarBuf);
    bytesSoFar = ftell(infile);
  }
  
  fclose(infile);
  return 0;
}
