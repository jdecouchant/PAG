/*
 * Code inspired (and largely copied) from https://shanetully.com/2012/06/openssl-rsa-aes-and-c/ and https://github.com/shanet/Crypto-Example
 *
 * Note: for simplicity, we assume that every node has the same public/private key pair.
 */

#ifndef CRYPTOGRAPHER_H_
#define CRYPTOGRAPHER_H_

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define RSA_KEYLEN 2048
#define AES_KEYLEN 256
#define AES_ROUNDS 6

#define SUCCESS 0
#define FAILURE -1

#define KEY_SERVER_PRI 0
#define KEY_SERVER_PUB 1
#define KEY_CLIENT_PUB 2
#define KEY_AES 3
#define KEY_AES_IV 4

struct encrypted_message_header {
	size_t ekl;
	size_t ivl;
	int encMsgLen;
};

class Cryptographer {
public:
	Cryptographer(int nbNodes, int nodeId);
	~Cryptographer(void);

	char* encrypt(char* message, int msgLen, int *encMsgLen);
	char* decrypt(char* message, int msgLen, int *decMsgLen);

private:
	int init();

	int rsaEncrypt(const unsigned char *msg, size_t msgLen,
			unsigned char **encMsg, unsigned char **ek, size_t *ekl,
			unsigned char **iv, size_t *ivl);
	int rsaDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char *ek,
			size_t ekl, unsigned char *iv, size_t ivl, unsigned char **decMsg);

	EVP_PKEY *localPrivKey;
	EVP_CIPHER_CTX *rsaEncryptCtx;
	EVP_CIPHER_CTX *rsaDecryptCtx;

	int nbNodes;
	int nodeId;
};

#endif
