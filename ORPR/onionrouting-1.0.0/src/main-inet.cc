#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <peerreview/transport/net/inet.h>
#include <peerreview/transport/id/x509.h>
#include "onionrouting.h"

/*void runInet(Parameters *params, const char *addr, const char *logname)
 {
 int numPeers = params->numSections();
 int mySectionIndex = -1;
 NodeHandle **handles = (NodeHandle**) malloc(numPeers * sizeof(NodeHandle*));
 char dirname[200];
 dirname[0] = 0;

 for (int i=0; i<numPeers; i++) {
 char section[200];
 strcpy(section, params->getSectionByIndex(i));
 char *peerID = strtok(section, "/");
 char *peerIP = peerID ? strtok(NULL, "/") : NULL;
 handles[i] = new SimpleNodeHandle(SimpleIdentifier::readFromString(peerID, 32), inet_addr(peerIP));
 if (!strcmp(peerIP, addr))
 params->getAsString("directory", dirname, sizeof(dirname));
 }

 if (!dirname[0])
 panic("Cannot find section in parameter file for IP '%s'", addr);

 InetTransport *transport = new InetTransport(inet_addr(addr));
 X509Transport *identity = new X509Transport(transport, 32, dirname);
 PeerReview *peerreview = new PeerReview(identity);
 Multicast *multicast = new Multicast(peerreview, params);
 MulticastTest *testapp = new MulticastTest(multicast);

 transport->setCallback(identity);
 identity->setCallback(peerreview);
 peerreview->setCallback(multicast);
 multicast->setCallback(testapp);

 identity->init();
 peerreview->init(dirname);

 if (logname)
 transport->openLog(logname);

 if (params->existsSetting("sender") && params->getAsBoolean("sender"))
 testapp->startSending();

 transport->run();

 multicast->bootstrap(handles, numPeers);
 for (int i=0; i<numPeers; i++)
 delete handles[i];

 free(handles);
 }*/

int main(int argc, char *argv[]) {
	/*if ((argc != 3) && (argc != 4))
	 panic("Syntax: %s <parameterFile> <localIP> [logfile]", argv[0]);

	 Parameters *params = new Parameters(argv[1]);
	 runInet(params, argv[2], (argc==4) ? argv[3] : NULL);
	 delete params;*/

	return 0;
}
