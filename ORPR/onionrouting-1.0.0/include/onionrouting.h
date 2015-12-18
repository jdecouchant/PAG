/*
 * onionrouting.h
 *
 *  Created on: Feb 5, 2014
 *      Author: diarraa
 */

#ifndef ONIONROUTING_H_
#define ONIONROUTING_H_

#include <peerreview.h>
#include <peerreview/tools/parameters.h>
#include <peerreview/transport/id/simple.h>
#include <map>
using namespace std;
static const int ONION_MAGIC_NUMBER = 0xDEADBEEF;

class Cryptographer;
class OnionRouting;

struct peeledOnion {
	NodeHandle* node;
	char *msg;
};

struct Time {
	long long tsend;
	long long trcvd;
};

//This value must be consistent with MAX_BYTES in simple.h
static const int MAX_BYTES = 20;

struct onion_header {
	int magic_number; // to identify a valid onion
	bool dest; // true iff this is the message for the final destination, false otherwise
	int len; // len of the payload

	// identifier of the destination
	unsigned char raw[MAX_BYTES];
	int raw_size;
	in_addr_t ip;
};
// followed by len bytes of payload

class OnionRoutingApp: public TimerCallback {
protected:
	OnionRouting *onionrouting;

public:
	OnionRoutingApp(OnionRouting *onionrouting) {
		this->onionrouting = onionrouting;
	}

	virtual void timerExpired(int timerID) {
	}

	virtual void receive(unsigned int seq, char *data, int datalen) {
	}

	virtual ~OnionRoutingApp() {
	}

};

class OnionRouting: public PeerReviewCallback, public TimerCallback {
protected:
	static const int SEND_INTERVAL_US = 26666; //26666; //50000; //96385; //153846; //266666; //500000;
	static const int SEND_TIMEOUT_US = 1000000;
	static const int STAGGERED_TIMEOUT_US = 800000;
	static const int MAX_WITNESSES_PER_NODE = 500;
	static const int MAX_PEERS = 1200;
	static const int MAX_EXPECTED_MESSAGES = 20;

	static const int MSG_HELLO = 32;
	static const int MSG_NOTIFY = 33;
	static const int MSG_REQUEST = 34;
	static const int MSG_DATA = 35;
	static const int MSG_CHECK = 36;

	static const int EVT_BOOTSTRAP = 10;
	static const int EVT_TRANSMIT = 12;
	static const int EVT_STATUS = 13;

	static const int TI_EXPECTED = 20;
	static const int TI_COMMAND = 21;

	int nbRelais;
	//int nbFwd;
	//int nbRcvd;
	char **nodes;

	int nbEVT_BOOTSTRAP;
	int nbEVT_TRANSMIT;
	int nbEVT_SEND_O;
	int nbEVT_RECV_O;
	int nbEVT_STATUS;

	class OnionRoutingReplayHelper: public EventCallback {
	protected:
		OnionRouting *onionrouting;
		ReplayWrapper *transport;
	public:
		OnionRoutingReplayHelper(OnionRouting *onionrouting,
				ReplayWrapper *transport) :
				EventCallback() {
			this->onionrouting = onionrouting;
			this->transport = transport;
		}

		virtual void replayEvent(unsigned char type, unsigned char *entry,
				int entrySize);
	};

	struct memberInfo {
		NodeHandle *subject;
		int numWitnesses;
	};

	struct memberInfo *memberInfo;
	int numMembers;
	IdentityTransport *transport;
	bool logEvents;
	OnionRoutingApp *app;
	//char command[200];
	char *command;
	long long commandTime;
	Parameters *params;

	struct state {
		struct {
			bool active;
			NodeHandle *handle;
		} peer[MAX_PEERS];

		struct {
			long long arrivalTime;
			int seq;
		} expectedMessage[MAX_EXPECTED_MESSAGES];

		int numExpectedMessages;
		int lastExpectedMessage;
		int numPeers;
	} state;

	void readMemberInfo(Parameters *params);

public:
	OnionRouting(IdentityTransport *transport, Parameters *params, int nbRelais,
			bool logEvents = true);
	virtual ~OnionRouting();
	virtual void init();
	virtual void receive(NodeHandle *source, bool datagram, unsigned char *msg, int msglen);
	char* getCommand(){return command;};
	long long getCommandTime(){return commandTime;};
	//////////////  For PeerReview //////////////
	virtual void statusChange(Identifier *id, int status);
	virtual void timerExpired(int timerID);
	virtual int storeCheckpoint(unsigned char *buffer, unsigned int maxlen);
	virtual bool loadCheckpoint(unsigned char *buffer, unsigned int len);
	virtual void getWitnesses(Identifier *subject, WitnessListener *callback);
	virtual int getMyWitnessedNodes(NodeHandle **nodes, int maxResults);
	virtual PeerReviewCallback *getReplayInstance(ReplayWrapper *replayWrapper);
	virtual void notifyCertificateAvailable(Identifier *id) {
	}

	virtual void setTransport(IdentityTransport *transport) {
		this->transport = transport;
	}

	//////////////////////////////////////////////
	virtual void sendComplete(long long id) {
	}

	IdentityTransport *getTransport() {
		return transport;
	}

	void scheduleTimer(TimerCallback *callback, int timerID, long long when) {
		transport->scheduleTimer(callback, timerID, when);
	}

	long long getTime() {
		return transport->getTime();
	}

	void bootstrap(NodeHandle **initialPeers, int numInitialPeers);
	void setCallback(OnionRoutingApp *app);
	void send(unsigned int seq, char *data, int len, NodeHandle *dest);
	int getnbRelais() {
		return nbRelais;
	}

	//int getnbFwd() { return nbFwd; };

	struct memberInfo *getMemberInfo() {
		return memberInfo;
	}

	char **getNodes() {
		return nodes;
	}

	//peeledOnion *peelOnion(char *onion);

};

class OnionRoutingTest: public OnionRoutingApp {
protected:
	static const int SEND_INTERVAL_US = 26666; //26666; //50000; //96385; //153846; //266666; //500000;
	static const int UPDATE_INTERVAL_US = 200000;
	static const int MAX_DELAY = 10000000;
	static const int TI_SEND = 99;
	static const int TI_CHECK = 11;
	NodeHandle **nodes;
	unsigned int nextSeq;
	map<int,Time> srTime;

	Cryptographer *cryptografer; // for onion encryption/decryption
	int nbSent; // number of sent onions
	int nbFwd; // number of forwarded onions
	int id; // node id
	int numMembers; // number of nodes
    int nbRcvd; // number of received onions (final destination)

	char *buildOnion(SimpleNodeHandle** dest);
	char* padMessage(char* m, int s);

public:
	OnionRoutingTest(OnionRouting *onionrouting, Parameters *params, int id);
	virtual void timerExpired(int timerID);
	virtual void receive(unsigned int seq, char *data, int datalen);
	void startSending();
	void init();
};

#endif /* ONIONROUTING_H_ */
