#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); exit(1); } while (0)

#define ALGO_RSA 0
#define ALGO_ESIGN 1

#define MAX_NUM 10000

struct nodeType {
  char filename[200];
  char subject[1000];
  char nodeid[200];
} node[MAX_NUM];

int num = -1;

char *getName(int index)
{
  assert((0<=index) && (index<num));
  return node[index].filename;
}

char *getSubject(int index)
{
  assert((0<=index) && (index<num));
  return node[index].subject;
}

int main(int argc, char *argv[])
{
  int algo = ALGO_RSA;
  int keybits = 1024;
  bool selfsign = false;
  int haveNum = 0;
  char idfile[200];
  idfile[0] = 0;
  
  for (int i=1; i<argc; i++) {
    if (!strncmp(argv[i], "-t", 2)) {
      if (!strcasecmp(&argv[i][2], "rsa"))
        algo = ALGO_RSA;
      else if (!strcasecmp(&argv[i][2], "esign"))
        algo = ALGO_ESIGN;
      else
        panic("Unknown crypto algorithm: '%s'", &argv[i][2]);
    } else if (!strncmp(argv[i], "-b", 2)) {
      keybits = atoi(&argv[i][2]);
    } else if (!strncmp(argv[i], "-selfsign", 9)) {
      selfsign = true;
    } else if (!strncmp(argv[i], "-f", 2)) {
      strncpy(idfile, &argv[i][2], sizeof(idfile));
    } else if (argv[i][0] == '-') {
      panic("Unknown command line option: '%s'", argv[i]);
    } else {
      num = atoi(argv[i]);
      haveNum ++;
    }
  }

  if (haveNum != 1)
    panic("Syntax: %s [-t{rsa,esign}][-b<keybits>][-selfsign][-f<idfile>] <#nodes>\n", argv[0]);

  srandom(time(0));
  
  /* Generate nodeIDs */

  if (idfile[0]) { 
    FILE *infile = fopen(idfile, "r");
    if (!infile)
      panic("Cannot read ID file '%s'", idfile);
      
    for (int i=0; i<num; i++) {
      char linebuf[2000];
      if (!fgets(linebuf, sizeof(linebuf), infile))
        panic("File '%s' does not have enough lines (need %d)", idfile, num);
      
      char *s0 = strtok(linebuf, " ");
      char *s1 = s0 ? strtok(NULL, " ") : NULL;
      char *s2 = s1 ? strtok(NULL, "\r\n") : NULL;
      if (!s2) 
        panic("File format error: '%s' needs at least three columns", idfile);
     
      strcpy(node[i].filename, s0);

      if (!strcmp(s1, "*")) {
        for (int j=0; j<40; j++)
          node[i].nodeid[j] = "0123456789abcdef"[(random()>>4)&0xF];
        node[i].nodeid[40] = 0;
      } else {
        strcpy(node[i].nodeid, s1);
      }

      node[i].subject[0] = 0;
      for (unsigned j=0; j<strlen(s2); j++) {
        if (s2[j]!='*')
          sprintf(&node[i].subject[strlen(node[i].subject)], "%c", s2[j]);
        else
          strcat(&node[i].subject[strlen(node[i].subject)], node[i].nodeid);
      }
    }
      
    fclose(infile);
  } else {
    for (int i=0; i<num; i++) {
      sprintf(node[i].filename, "node%04d", 1+i);
      for (int j=0; j<40; j++)
        node[i].nodeid[j] = "0123456789abcdef"[(random()>>4)&0xF];
      node[i].nodeid[40] = 0;

      sprintf(node[i].subject, "/C=DE/ST=Saarland/O=MPI-SWS/OU=Distributed Systems Group/CN=%s/emailAddress=node%04d@octarine.de", node[i].nodeid, 1+i);
    }
  }
  
  FILE *script = fopen("makecerts.sh", "w+");
  if (!script)
    panic("Cannot create script file");
    
  fprintf(script, "#!/bin/bash\n\n");

  if (algo == ALGO_RSA) {
    fprintf(script, "mkdir -p ca\n");
    fprintf(script, "cd ca\n");
    if (!selfsign) {
      fprintf(script, "Setting up CA configuration\n");
      fprintf(script, "mkdir -p demoCA\n");
      fprintf(script, "mkdir -p demoCA/newcerts\n");
      fprintf(script, "mkdir -p demoCA/private\n");
      fprintf(script, "touch demoCA/index.txt\n");
      fprintf(script, "echo [ ca ] >demoCA/demoCA.cnf\n");
      fprintf(script, "echo default_ca = demoCA >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo [ demoCA ] >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo dir = ./demoCA >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo database = \\$dir/index.txt >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo new_certs_dir = \\$dir/newcerts >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo certificate = cacert.pem >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo serial = \\$dir/serial >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo private_key = ./cakey.pem >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo RANDFILE = \\$dir/private/.rand >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo default_days = 365 >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo default_crl_days = 30 >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo default_md = md5 >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo policy = policy_any >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo [ policy_any ] >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo countryName = supplied >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo stateOrProvinceName = optional >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo organizationName = optional >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo organizationalUnitName = optional >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo commonName = supplied >>demoCA/demoCA.cnf\n");
      fprintf(script, "echo emailAddress = optional >>demoCA/demoCA.cnf\n");

      fprintf(script, "echo Making CA certificate\n");
      fprintf(script, "echo DE >foo\n");
      fprintf(script, "echo Saarland >>foo\n");
      fprintf(script, "echo Saarbruecken >>foo\n");
      fprintf(script, "echo MPI-SWS >>foo\n");
      fprintf(script, "echo Distributed Systems Group >>foo\n");
      fprintf(script, "echo Andreas Haeberlen >>foo\n");
      fprintf(script, "echo democa@octarine.de >>foo\n");
      fprintf(script, "echo >>foo\n");
      fprintf(script, "echo >>foo\n");
      fprintf(script, "openssl req -new -keyout cakey.pem -out careq.pem -passout pass:monkey -newkey rsa:%d <foo\n", keybits);
      fprintf(script, "openssl ca -config demoCA/demoCA.cnf -create_serial -passin pass:monkey -out cacert.pem -days 365 -batch -keyfile cakey.pem -selfsign -infiles careq.pem\n");
      fprintf(script, "rm -f careq.pem foo\n");
    }

    for (int i=0; i<num; i++) {
      if (i==0)
        fprintf(script, "rm -f temp.pem\n");

      if (!selfsign) {
        fprintf(script, "openssl req -new -keyout temp.pem -out newreq.pem -days 365 -passin pass:monkey -passout pass:monkey -newkey rsa:%d -subj \"%s\"\n", keybits, node[i].subject);
        fprintf(script, "openssl ca -config demoCA/demoCA.cnf -batch -policy policy_any -out %s_cert.pem -passin pass:monkey -infiles newreq.pem\n", getName(i));
        fprintf(script, "rm newreq.pem\n");
        fprintf(script, "openssl rsa -in temp.pem -out %s_key.pem -passin pass:monkey\n", getName(i));
        fprintf(script, "rm -f temp.pem\n");
      } else {
        fprintf(script, "openssl genrsa %d >%s_key.pem\n", keybits, getName(i));
        fprintf(script, "openssl req -new -x509 -nodes -sha1 -days 365 -key %s_key.pem >%s_cert.pem -subj \"%s\"\n", getName(i), getName(i), node[i].subject);
        fprintf(script, "openssl rsa -in %s_key.pem -inform pem -outform der -out %s_key.der\n", getName(i), getName(i));
        fprintf(script, "openssl x509 -in %s_cert.pem -inform pem -outform der -out %s_cert.der\n", getName(i), getName(i));
      }
    }

    if (!selfsign)
      fprintf(script, "rm -rf demoCA/\n");
  } else if (algo == ALGO_ESIGN) {
    fprintf(script, "mkdir -p ca\n");
    fprintf(script, "echo Making CA keypair\n");
    fprintf(script, "./esign-keygen %d >ca/ca_key.esign\n", keybits);
    fprintf(script, "head -3 ca/ca_key.esign >ca/ca_pubkey.esign\n");
    fprintf(script, "echo Making node certificates\n");
    for (int i=0; i<num; i++) {
      fprintf(script, "echo .. Node %d/%d\n", i+1, num);
      fprintf(script, "./esign-keygen %d >ca/%s_key.esign\n", keybits, getName(i));
      fprintf(script, "head -3 ca/%s_key.esign > temp\n", getName(i));
      fprintf(script, "./esign-signcert ca/ca_key.esign temp %s >ca/%s_cert.esign\n", node[i].nodeid, getName(i));
      fprintf(script, "rm -f temp\n");
    }
    fprintf(script, "echo Done\n");
  }
  
  fclose(script);
  chmod("makecerts.sh", 0755);
  
  printf("Now run the script 'makecerts.sh'!\n");
      
  return 0;
}
