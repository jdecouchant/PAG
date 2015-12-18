/*
 * onionrouting-sender.cc
 *
 *  Created on: Feb 5, 2014
 *      Author: diarraa
 */

#include <stdlib.h>
#include <string.h>
#include <vector>
#include "parameters.h"
#include "onionrouting.h"
#include "Cryptographer.h"

#define SUBSYSTEM "onionrouting"
#define mlog(a...) do { onionrouting->getTransport()->logText(SUBSYSTEM, a); } while (0)
static int onionId = 0;
OnionRoutingTest::OnionRoutingTest(OnionRouting *onionrouting,
		Parameters *params, int id) :
		OnionRoutingApp(onionrouting) {
	int numMembers = params->numSections();

	nodes = (NodeHandle**) malloc(sizeof(NodeHandle*) * numMembers);
	if (!nodes) {
		panic("ORTest: Out of memory when allocating nodes");
	}
	for (int i = 0; i < numMembers; i++) {
		const char *name = params->getSectionByIndex(i);
		params->selectSection(name);
		nodes[i] = onionrouting->getTransport()->readNodeHandleFromString(name);
	}

	this->numMembers = numMembers;
	this->id = id;
	this->cryptografer = new Cryptographer(numMembers, id);
	this->nextSeq = 0;
	this->nbSent = 0;
	this->nbFwd = 0;
	this->nbRcvd = 0;
}

void OnionRoutingTest::init() {
	/*char buf[200];
	 char *node = onionrouting->getTransport()->getLocalHandle()->render(buf);
	 char* id = strtok(node,"/");
	 char* ip = id ? strtok(NULL,"/") : NULL;
	 nodes.push_back(ip);*/
	//printf("node_ip = %s nodes-size = %d\n",ip,nodes.size());
}

bool isElt(int a, int* l, int n) {
	for (int i = 0; i < n; i++) {
		if (l[i] == a)
			return true;
	}
	return false;
}

char* OnionRoutingTest::padMessage(char* m, int s) {
	if (s > PADDED_MESSAGE_SIZE) {
		fprintf(stderr,
				"Padding the onion has failed: the message is greater than the maximal padded message size: %d > %d\n",
				s, PADDED_MESSAGE_SIZE);
		exit(-1);
	}

	char* padded_message = (char*) realloc(m, PADDED_MESSAGE_SIZE);
	bzero(padded_message + s, PADDED_MESSAGE_SIZE - s);
	return padded_message;
}

char* OnionRoutingTest::buildOnion(SimpleNodeHandle** dest) {
	char* message = (char*) malloc(sizeof(struct onion_header) + PAYLOAD_SIZE);
	if (!message) {
		fprintf(stderr, "[%s] Malloc(%lu) error. Abort!\n", __PRETTY_FUNCTION__,
				sizeof(struct onion_header) + PAYLOAD_SIZE);
		exit(-1);
	}
	char* payload = message + sizeof(struct onion_header);
	bzero(payload, PAYLOAD_SIZE);

	int nbRelais = onionrouting->getnbRelais();
	int onionSize = nbRelais + 1;
	int nodes_list[onionSize];
	for (int i = 0; i < onionSize; i++) {
		int k;
		do {
			k = rand() % numMembers;
		} while (k == id || isElt(k, nodes_list, i));
		nodes_list[i] = k;
	}

	//printf("[%s] Node %d builds a new onion: ", __PRETTY_FUNCTION__, id);
	//printf("Node %d builds a new onion: ", id);
	//for (int i = onionSize - 1; i > 0; i--) {
	//	printf("%d -> ", nodes_list[i]);
	//}
	//printf("%d\n", nodes_list[0]);

	struct onion_header* h = (struct onion_header*) message;
	h->magic_number = ONION_MAGIC_NUMBER;
	h->len = PAYLOAD_SIZE;
	h->dest = true;
	h->raw_size = 0;
	h->ip = 0;

	// encrypt the message for the destination
	char* message_to_encrypt = message;
	int message_to_encrypt_size = sizeof(struct onion_header) + PAYLOAD_SIZE;
	int encMsgLen;
	char* encrypted_message = cryptografer->encrypt(message_to_encrypt,
			message_to_encrypt_size, &encMsgLen);

	NodeHandle* n = nodes[nodes_list[onionSize - 1]];
	*dest = new SimpleNodeHandle((SimpleIdentifier*) n->getIdentifier(),
			((SimpleNodeHandle*) n)->getIP());

	// encrypt the message for the relays
	for (int i = 1; i < onionSize; i++) {
		free(message_to_encrypt);
		message_to_encrypt_size = encMsgLen + sizeof(struct onion_header);
		message_to_encrypt = (char*) malloc(message_to_encrypt_size);
		if (!message_to_encrypt) {
			fprintf(stderr, "[%s] Malloc(%d) error. Abort!\n",
					__PRETTY_FUNCTION__, message_to_encrypt_size);
			exit(-1);
		}

		NodeHandle* n = nodes[nodes_list[i - 1]];
		struct onion_header* h = (struct onion_header*) message_to_encrypt;
		h->magic_number = ONION_MAGIC_NUMBER;
		h->len = encMsgLen;
		h->dest = false;
		memcpy(h->raw,
				((SimpleIdentifier*) n->getIdentifier())->get_raw_ptr(
						&h->raw_size), MAX_BYTES);
		h->ip = ((SimpleNodeHandle*) n)->getIP();

		memcpy((char*) (h + 1), encrypted_message, h->len);
		free(encrypted_message);

		encrypted_message = cryptografer->encrypt(message_to_encrypt,
				message_to_encrypt_size, &encMsgLen);
	}

	free(message_to_encrypt);
	message_to_encrypt = encrypted_message;
	message_to_encrypt_size = encMsgLen;

	char* padded_message = padMessage(message_to_encrypt,
			message_to_encrypt_size);

	return padded_message;
}

void OnionRoutingTest::startSending() {
	onionrouting->scheduleTimer(this, TI_SEND, onionrouting->getTime() + SEND_INTERVAL_US);
}

void OnionRoutingTest::timerExpired(int timerID) {
	switch (timerID) {
	case TI_SEND: {
		SimpleNodeHandle *dest;
		char *onion = buildOnion(&dest);
		//printf("Time to send an onion = %lld size = %d\n",onionrouting->getTime(), sizeof(onion));
		//onionrouting->send(nextSeq++, onion, PADDED_MESSAGE_SIZE, dest);
		int id = ++onionId;
		srTime[id].tsend = onionrouting->getTime();
		mlog(4,"srTime : (%d, %lld, %lld)", id, srTime[id].tsend, srTime[id].trcvd);
		onionrouting->send(id, onion, PADDED_MESSAGE_SIZE, dest);
		//delete dest;
		nbSent++;
		mlog(4, "nbSent = %d", nbSent);

		//send another onion
		onionrouting->scheduleTimer(this, TI_SEND, onionrouting->getTime() + SEND_INTERVAL_US);
		break;
	}
	default: {
		panic("Unknown timer #%d expired in OnionRoutingTest", timerID);
		break;
	}
	}
}

void OnionRoutingTest::receive(unsigned int seq, char *data, int datalen) {
	char buf[200];
	NodeHandle *myHandle = onionrouting->getTransport()->getLocalHandle();
	//printf("Node %s received onion #%d\n", myHandle->render(buf), seq);
	//printf("Time to receive an onion = %lld\n",onionrouting->getTime());

	int decMsgLen;
	char* decrypted_message = cryptografer->decrypt(data, datalen, &decMsgLen);
	if (!decrypted_message) {
		//printf("Node %d: Unable to decrypt message\n", id);
		return;
	}

	struct onion_header* h = (struct onion_header*) decrypted_message;
	if (h->magic_number != ONION_MAGIC_NUMBER) {
		printf("Node %d: I have received garbage: %d != %d\n", id,
				h->magic_number, ONION_MAGIC_NUMBER);
		return;
	}

	// msg is the payload (i.e., data without the onion_header)
	int msg_size = decMsgLen - sizeof(struct onion_header);
	char* msg = (char*) malloc(msg_size);
	memcpy(msg, decrypted_message + sizeof(struct onion_header), msg_size);
	mlog(3,"EVT_FWD_O : EntrySize = %lld", msg_size);
	onionrouting->getTransport()->logEvent(EVT_FWD_O, msg, msg_size);

	if (h->dest) {
		//printf("Time to receive the final msg = %lld\n",onionrouting->getTime());
		//printf("I have received a onion-ized message!!!\n");
		srTime[seq].trcvd = onionrouting->getTime();
		mlog(4,"srTime : (%d, %lld, %lld)", seq, srTime[seq].tsend, srTime[seq].trcvd);
		nbRcvd++;
		mlog(4,"nbRcvd = %d", nbRcvd);

		//the application may be interested in the message msg of size msg_size
	} else {
		//printf("I am not the final destination\n");
		// forward the message to the next relay
		msg = padMessage(msg, msg_size);
		SimpleNodeHandle *dest = new SimpleNodeHandle(
				new SimpleIdentifier(h->raw_size, h->raw), h->ip);
		//onionrouting->send(nextSeq++, msg, PADDED_MESSAGE_SIZE, dest);
		//onionrouting->getTransport()->send(dest, false, (unsigned char*)msg, PADDED_MESSAGE_SIZE, 6);
        //printf("tTime = %ld  cTime = %ld command = %s\n",onionrouting->getTransport()->getTime(), onionrouting->getCommandTime(), onionrouting->getCommand());
		if ((onionrouting->getCommandTime()>=0) && (onionrouting->getTransport()->getTime()>=onionrouting->getCommandTime()) && !strcasecmp(onionrouting->getCommand(), "silent"))
			printf("Rational Node :  No forwarding\n");
		else{
			  onionrouting->send(seq, msg, PADDED_MESSAGE_SIZE, dest);
		      delete dest;
		      nbFwd++;
		      mlog(4, "nbFwd = %d", nbFwd);
		}
	}

	free(msg);
}
