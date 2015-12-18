#ifndef BASE_H
#define BASE_H

#include "Source.h"

using namespace std;

const bool MAINTAIN_WITNESSED_BM = false;

/*** Message types ***/
const int KEY_RQST  = 1;
const int KEY_RESP = 2;
const int SERVE     = 3;
const int ACK_SERVE     = 4;
const int ACK_NODE_WITN    = 5;
const int ACK_WITN_WITN = 6;
const int SHARE_CRYPTO = 7;

/*** Constants of the streaming protocol ***/

const int RTE = 8; //10 // Number of rounds between the release of an update and its expiration
//const int UPD_NBR_PER_ROUND = 40; // Updates number released by the source at each round
// Rounds duration, and the size of video chunks are specified in the .ini file


/*** Size of log components in bits ***/

const int ROUND_ID_SIZE     = 13; // 2^13 rounds = more than 2 hours
const int SENDER_ID_SIZE    = 18; // 2^18 = up to 262000 peers simultaneously
const int RECEIVER_ID_SIZE  = 18;
const int MSG_TYPE_SIZE     = 3; // up to 8 types of messages
const int UPDATES_ID_SIZE   = 18;
const int ENCRYPTION_SIZE   = 1024; //1024;
const int SIGN_SIZE         = ENCRYPTION_SIZE; // Size of a signature with RSA (1024 bits)
const int HASH_SIZE         = 160; // Size of a hash in bits with SHA-1 (as in PeerReview)
const int KEY_SIZE          = 512; //512;
const int COUNT_SIZE        = 32;
const int PRIME_NUMBER_SIZE = KEY_SIZE;


/*** Diverse constants ***/

const int NULL_NODE_ID = -1;
const int SERVER_ID = -1;


#endif
