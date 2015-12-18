#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <openssl/sha.h>
#include <esign/esign.h>

#include "gmp.h"

#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); exit(1); } while (0)

int main (int argc, char **argv)
{
  if (argc != 4) {
    fprintf(stderr, "Syntax: esign-signcert <cakey> <nodekey> <nodeid>\n");
    return 1;
  }
  
  char linebuf[2000];
  FILE *infile;
  
  /* Read CA key */
  
  infile = fopen(argv[1], "r");
  if (!infile)
    panic("Cannot read CA key from '%s'", argv[1]);
    
  fgets(linebuf, sizeof(linebuf), infile);
  int ca_nbits = atoi(linebuf);
  if (ca_nbits < 10)
    panic("Cannot read bits from CA key file");
    
  fgets(linebuf, sizeof(linebuf), infile);
  int ca_k = atoi(linebuf);
  if (ca_k < 4)
    panic("Cannot read K value from CA key file");
    
  fgets(linebuf, sizeof(linebuf), infile);
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  bigint ca_p (linebuf, 16);
  fgets(linebuf, sizeof(linebuf), infile);
  strtok(linebuf, "\r\n");
  bigint ca_q (linebuf, 16);
  fclose(infile);

  /* Read node key */

  infile = fopen(argv[2], "r");
  if (!infile)
    panic("Cannot read node key from '%s'", argv[2]);
    
  fgets(linebuf, sizeof(linebuf), infile);
  int node_nbits = atoi(linebuf);
  if (node_nbits != ca_nbits)
    panic("Node and CA keys must have the same number of bits");
    
  fgets(linebuf, sizeof(linebuf), infile);
  int node_k = atoi(linebuf);
  if (node_k < 4)
    panic("Node and CA keys must use the same K value");
  
  char node_n[5000];  
  fgets(node_n, sizeof(node_n), infile);
  strtok(node_n, "\r\n");
  
  char snbits[200], sk[200];
  sprintf(snbits, "%d", ca_nbits);
  sprintf(sk, "%d", ca_k);

  /* Hash */
  
  unsigned char hash[21];
  SHA_CTX c;
  SHA1_Init(&c);
  SHA1_Update(&c, snbits, strlen(snbits));
  SHA1_Update(&c, sk, strlen(sk));
  SHA1_Update(&c, node_n, strlen(node_n));
  SHA1_Update(&c, argv[3], strlen(argv[3]));
  SHA1_Final(hash, &c);
  hash[0] &= 0x7F;
  
  char shash[41];
  for (int i=0; i<20; i++)
    sprintf(&shash[2*i], "%02x", hash[i]);

  /* Sign */
  
  esign_priv e (ca_p, ca_q, ca_k);
  bigint v (shash, 16);
  bigint sv = e.raw_sign(v);
  
  char ssig[2000];
  mpz_get_str(ssig, 16, &sv);
  
  /* Print certificate */
  
  printf("%d\n", node_nbits);
  printf("%d\n", node_k);
  printf("%s\n", node_n);
  printf("%s\n", argv[3]);
  printf("%s\n", ssig);
  return 0;
}
