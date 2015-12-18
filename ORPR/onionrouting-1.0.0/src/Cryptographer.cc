#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Cryptographer.h"

Cryptographer::Cryptographer(int nbNodes, int nodeId) {
	this->nbNodes = nbNodes;
	this->nodeId = nodeId;

	//printf("Node %d is creating a new Cryptographer for %d nodes\n", nodeId,
	//		nbNodes);

	if (init() != SUCCESS) {
		fprintf(stderr,
				"Error while initializing the Cryptographer for node %d. Abort!\n",
				nodeId);
		exit(-1);
	}
}

Cryptographer::~Cryptographer(void) {
	EVP_PKEY_free(localPrivKey);

	EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);
	EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);

	free(rsaEncryptCtx);
	free(rsaDecryptCtx);
}

int Cryptographer::init() {
	FILE *pemFile = fopen("rsa_onionrouting_private_1024.key", "r");
	if (pemFile) {
		localPrivKey = PEM_read_PrivateKey(pemFile, 0, 0, 0);
		fclose(pemFile);
	}

	// Initalize contexts
	rsaEncryptCtx = (EVP_CIPHER_CTX*) malloc(sizeof(EVP_CIPHER_CTX));
	rsaDecryptCtx = (EVP_CIPHER_CTX*) malloc(sizeof(EVP_CIPHER_CTX));

	// Always a good idea to check if malloc failed
	if (rsaEncryptCtx == NULL || rsaDecryptCtx == NULL) {
		return FAILURE;
	}

	// Init these here to make valgrind happy
	EVP_CIPHER_CTX_init(rsaEncryptCtx);
	EVP_CIPHER_CTX_init(rsaDecryptCtx);

	return SUCCESS;
}

int Cryptographer::rsaEncrypt(const unsigned char *msg, size_t msgLen,
		unsigned char **encMsg, unsigned char **ek, size_t *ekl,
		unsigned char **iv, size_t *ivl) {
	size_t encMsgLen = 0;
	size_t blockLen = 0;

	*ek = (unsigned char*) malloc(EVP_PKEY_size(localPrivKey));
	*iv = (unsigned char*) malloc(EVP_MAX_IV_LENGTH);
	if (*ek == NULL || *iv == NULL)
		return FAILURE;
	*ekl = EVP_PKEY_size(localPrivKey);
	*ivl = EVP_MAX_IV_LENGTH;

	*encMsg = (unsigned char*) malloc(msgLen + EVP_MAX_IV_LENGTH);
	if (encMsg == NULL)
		return FAILURE;

	if (!EVP_SealInit(rsaEncryptCtx, EVP_aes_256_cbc(), ek, (int*) ekl, *iv,
			&localPrivKey, 1)) {
		return FAILURE;
	}

	if (!EVP_SealUpdate(rsaEncryptCtx, *encMsg + encMsgLen, (int*)&blockLen, (const unsigned char*)msg, (int)msgLen)) {
		return FAILURE;
	}
	encMsgLen += blockLen;

	if (!EVP_SealFinal(rsaEncryptCtx, *encMsg + encMsgLen, (int*) &blockLen)) {
		return FAILURE;
	}
	encMsgLen += blockLen;

	EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);

	return (int) encMsgLen;
}

int Cryptographer::rsaDecrypt(unsigned char *encMsg, size_t encMsgLen,
		unsigned char *ek, size_t ekl, unsigned char *iv, size_t ivl,
		unsigned char **decMsg) {
	size_t decLen = 0;
	size_t blockLen = 0;
	EVP_PKEY *key;

	*decMsg = (unsigned char*) malloc(encMsgLen + ivl);
	if (decMsg == NULL)
		return FAILURE;

	key = localPrivKey;

	if (!EVP_OpenInit(rsaDecryptCtx, EVP_aes_256_cbc(), ek, ekl, iv, key)) {
		return FAILURE;
	}

	if (!EVP_OpenUpdate(rsaDecryptCtx, (unsigned char*)*decMsg + decLen, (int*)&blockLen, encMsg, (int)encMsgLen)) {
		return FAILURE;
	}
	decLen += blockLen;

	if (!EVP_OpenFinal(rsaDecryptCtx, (unsigned char*) *decMsg + decLen,
			(int*) &blockLen)) {
		return FAILURE;
	}
	decLen += blockLen;

	EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);

	return (int) decLen;
}

char* Cryptographer::encrypt(char* message, int len, int* encMsgLen) {
	unsigned char *encMsg = NULL;
	int ecl;
	unsigned char *ek;
	unsigned char *iv;
	size_t ekl;
	size_t ivl;

	// Encrypt the message with RSA
	if ((ecl = rsaEncrypt((const unsigned char*) message, len, &encMsg, &ek,
			&ekl, &iv, &ivl)) == -1) {
		fprintf(stderr, "Encryption failed\n");
		return NULL;
	}

	*encMsgLen = sizeof(struct encrypted_message_header) + ekl + ivl + ecl;

	char *result = (char*) malloc(*encMsgLen);
	struct encrypted_message_header *h =
			(struct encrypted_message_header*) result;
	h->ekl = ekl;
	h->ivl = ivl;
	h->encMsgLen = ecl;

	char *off = result + sizeof(struct encrypted_message_header);
	memcpy(off, ek, ekl);
	off += ekl;
	memcpy(off, iv, ivl);
	off += ivl;
	memcpy(off, encMsg, ecl);

	// No one likes memory leaks
	free(encMsg);
	free(ek);
	free(iv);

	return result;
}

char* Cryptographer::decrypt(char* message, int msgLen, int *decMsgLen) {

	struct encrypted_message_header *h =
			(struct encrypted_message_header*) message;

	char *off = message + sizeof(struct encrypted_message_header);
	unsigned char *ek = (unsigned char*) off;
	off += h->ekl;
	unsigned char *iv = (unsigned char*) off;
	off += h->ivl;
	unsigned char *encMsg = (unsigned char*) off;
	char *decMsg = NULL;

	// Decrypt the message
	if ((*decMsgLen = rsaDecrypt(encMsg, (size_t) h->encMsgLen, ek, h->ekl, iv,
			h->ivl, (unsigned char**) &decMsg)) == -1) {
		fprintf(stderr, "Decryption failed\n");
		return NULL;
	}

	return (char*) decMsg;
}

