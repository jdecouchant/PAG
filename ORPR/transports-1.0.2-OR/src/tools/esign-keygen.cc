#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "gmp.h"

/* This code generates BAD RANDOM NUMBERS (only 2^32 different seeds)!
 * Do NOT use this for production code! 
 *
 * Output format: <bits,k,n,p,q>
 */

int main (int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "Syntax: esign-keygen <nbits>\n");
    return 1;
  }
  
  int nbits = atoi(argv[1]);
  int k = 8;

  /* Init PRNG */
 
  struct timeval tv;
  gettimeofday(&tv, NULL);
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, tv.tv_usec);

  /* Generate p and q */

  int pqbits = (nbits + 1) / 3;
  int cls;

  mpz_t p;
  mpz_init2 (p, pqbits); 
  mpz_urandomb(p, state, pqbits);
  mpz_setbit (p, pqbits-1); 
  mpz_setbit (p, 0);     
  cls = mpz_probab_prime_p (p, 50);
  while (cls == 0) {
    mpz_add_ui (p, p, 2);
    cls = mpz_probab_prime_p (p, 50);
  };

  mpz_t q;
  mpz_init2 (q, pqbits); 
  mpz_urandomb(q, state, pqbits);
  mpz_setbit (q, pqbits-1); 
  mpz_setbit (q, 0);     
  cls = mpz_probab_prime_p (q, 50);
  while (cls == 0) {
    mpz_add_ui (q, q, 2);
    cls = mpz_probab_prime_p (q, 50);
  };

  /* p must be bigger than q */
  
  if (mpz_cmp(p, q) < 0) {
    mpz_t h;
    mpz_init2(h, pqbits);
    mpz_set(h, p);
    mpz_set(p, q);
    mpz_set(q, h);
  }

  /* Calculate n = p^2 * q */
  
  mpz_t p2;
  mpz_init2 (p2, nbits);
  mpz_mul(p2, p, p);
  
  mpz_t n;
  mpz_init2 (n, nbits);
  mpz_mul(n, p2, q);

  /* Print */

  fprintf(stdout, "%d\n%d\n", nbits, k);
  mpz_out_str(stdout, 16, n);
  fprintf(stdout, "\n");
  mpz_out_str(stdout, 16, p);
  fprintf(stdout, "\n");
  mpz_out_str(stdout, 16, q);
  fprintf(stdout, "\n");
  return 0;
}
