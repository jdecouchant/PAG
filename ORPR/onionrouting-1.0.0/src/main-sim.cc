#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <peerreview/transport/net/simulator.h>
#include "onionrouting.h"
#include "parameters.h"

#ifdef ESIGN
#include <peerreview/transport/id/esignxp.h>
#else
#include <peerreview/transport/id/x509.h>
#endif

int getfree() {
	int si = 2000000000;
	void *p;

	while (true) {
		p = malloc(si);
		if (p) {
			free(p);
			break;
		} else {
			si -= 4096;
		}
	}

	return si;
}

void runSim(Parameters *params) {
	static const long long startTime = EXP_START_TIME;
	static const long long endTime = EXP_END_TIME;
	int freeInit = getfree();

	int numPeers = params->numSections();
	NodeHandle **handles = (NodeHandle**) malloc(
			numPeers * sizeof(NodeHandle*));
	for (int i = 0; i < numPeers; i++) {
		char section[200];
		strcpy(section, params->getSectionByIndex(i));
		char *peerID = strtok(section, "/");
		char *peerIP = peerID ? strtok(NULL, "/") : NULL;
		handles[i] = new SimpleNodeHandle(
				SimpleIdentifier::readFromString(peerID, 160),
				inet_addr(peerIP));
		//printf("IP = %s\n",peerIP);
	}

	Simulator *simulator = new Simulator(startTime, 2000, endTime);
	simulator->enableSnapshots(200000, "snapshots.data");
	for (int i = 0; i < params->numSections(); i++) {
		const char *thisSection = params->getSectionByIndex(i);
		//std::cout<<"Section = "<<thisSection<<std::endl;
		params->selectSection(thisSection);
		if (!strchr(thisSection, '/'))
			panic("Section name is not a node handle: '%s'", thisSection);

		char dirname[200];
		__attribute__((unused)) char misbehaviorCommand[200];
		char *misbehavior = NULL;

		params->getAsString("directory", dirname, sizeof(dirname));
		//std::cout<<"dirname = "<<dirname<<std::endl;
		bool rational = false;
		if (params->existsSetting("rational"))
			rational = params->getAsBoolean("rational");
		//std::cout <<"Rational = "<<rational<<std::endl;

		int nbRelais = NB_RELAYS;
		if (params->existsSetting("nbRelais"))
			nbRelais = params->getAsInt("nbRelais");

		SimpleSecureHistoryFactory *factory = new SimpleSecureHistoryFactory();
		Transport *transport = simulator->createTransport(
				inet_addr(strchr(thisSection, '/') + 1));
#ifdef ESIGN
		ESignTransport *identity = new ESignTransport(transport, 160, dirname);
#else
		X509Transport *identity = new X509Transport(transport, 160, dirname);
#endif
		PeerReview *peerreview = new PeerReview(identity);
		OnionRouting *onionrouting = new OnionRouting(peerreview, params,
				nbRelais);
		OnionRoutingTest *testapp = new OnionRoutingTest(onionrouting, params, i);

		transport->setCallback(identity);
		identity->setCallback(peerreview);
		peerreview->setCallback(onionrouting);
		onionrouting->setCallback(testapp);

		identity->init();
		peerreview->init(dirname, factory, misbehavior, rational);
		onionrouting->bootstrap(handles, numPeers);
		testapp->init();

		peerreview->setLogDownloadTimeout(200000);
		peerreview->setAuthenticatorPushInterval(100000);
		if (params->existsSetting("pTransmit"))
			peerreview->enableProbabilisticChecking(
					params->getAsDouble("pTransmit"));

        // Jeremie
        // if (i == 0)
			testapp->startSending();
	}

	simulator->run();
	delete simulator;

	for (int i = 0; i < numPeers; i++)
		delete handles[i];
	free(handles);

	int freeFinal = getfree();
	printf("free=%d (%d)\n", freeFinal, freeFinal - freeInit);
}

int main(int argc, char *argv[]) {
	if (argc != 2)
		panic("Syntax: %s <parameterFile>", argv[0]);

	Parameters *params = new Parameters(argv[1]);
	runSim(params);
	delete params;

	return 0;
}
