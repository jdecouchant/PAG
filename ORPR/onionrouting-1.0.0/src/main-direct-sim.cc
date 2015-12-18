#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <peerreview/transport/net/simulator.h>
#include <peerreview/transport/id/x509.h>

#include "parameters.h"
#include "onionrouting.h"

void runSim(Parameters *params) {
	const long long startTime = EXP_START_TIME; // in usec; not 0 for warmup period
	const long long endTime = EXP_END_TIME; //100000000; // in usec
	int nbRelais = NB_RELAYS;

	//initialize the random number generator
	srand(time(NULL));

	int numPeers = params->numSections();
	NodeHandle **handles = (NodeHandle**) malloc(numPeers * sizeof(*handles));
	for (int i = 0; i < numPeers; i++) {
		char section[200];
		strcpy(section, params->getSectionByIndex(i));
		char *peerID = strtok(section, "/");
		char *peerIP = peerID ? strtok(NULL, "/") : NULL;
		handles[i]
				= new SimpleNodeHandle(
						SimpleIdentifier::readFromString(peerID, 160),
						inet_addr(peerIP));
	}

	Simulator *simulator = new Simulator(startTime, 120000, endTime);
	for (int i = 0; i < params->numSections(); i++) {
		const char *thisSection = params->getSectionByIndex(i);
		params->selectSection(thisSection);
		if (!strchr(thisSection, '/'))
			panic("Section name is not a node handle: '%s'", thisSection);

		char dirname[200];
		params->getAsString("directory", dirname, sizeof(dirname));

		if (params->existsSetting("nbRelais"))
			nbRelais = params->getAsInt("nbRelais");

		Transport *transport = simulator->createTransport(
				inet_addr(strchr(thisSection, '/') + 1));
		X509Transport *identity = new X509Transport(transport, 160, dirname);
		OnionRouting *onionrouting = new OnionRouting(identity, params,
				nbRelais);
		OnionRoutingTest *testapp = new OnionRoutingTest(onionrouting, params, i);

		transport->setCallback(identity);
		identity->setCallback(onionrouting);
		onionrouting->setCallback(testapp);

		identity->init();
		onionrouting->bootstrap(handles, numPeers);

		//if (params->existsSetting("sender") && params->getAsBoolean("sender"))
		//if (i == 0)
			testapp->startSending();
	}

	simulator->run();
	delete simulator;

	for (int i = 0; i < numPeers; i++)
		delete handles[i];
	free(handles);
}

int main(int argc, char *argv[]) {
	if (argc != 2)
		panic("Syntax: %s <parameterFile>", argv[0]);

	Parameters *params = new Parameters(argv[1]);
	runSim(params);
	delete params;

	return 0;
}
