#include "base.h"
#include "Source.h"
#include "Peer.h"
#include "packets_m.h"
#include "Buffermap.h"

#include <IPAddressResolver.h>
#include <stdlib.h>

Define_Module(Source);


/*** Global variables used by all nodes ***/
int Source::nodesNbr;
std::vector<IPvXAddress> Source::nodesAddr;
long Source::bytesLeft;

int Source::fanout;
int Source::packetLen;
int Source::witnessNbr;

/*** Variables used to print statistics ***/

int Source::writeReceivedUpdatesNbr;
FILE *Source::receivedUpdatesFile;

int Source::nbSessions;

int Source::writeUploadNbr;
long *Source::uploadBandwidthArray;
long *Source::uploadWitnessBandwidthArray;
FILE *Source::uploadFile;
FILE *Source::uploadSnapshotFile;

long Source::nbAvgMeasures;
double *Source::avgUploadBandwidthArray;

int Source::updNbPerRound;

int Source::writeEncryptionsNbr;
double Source::encryptionsSum;
double Source::primeSum;
double Source::primeSum512;
FILE  *Source::encryptionsFile;

Source::Source() {}

Source::~Source() {}

void Source::initialize() {
	// The list of all IP addresses must be computed once
	membershipNeverComputed = true;

	// initialization of parameters
	packetLenPar = &par("packetLenPar");
	videoSize = &par("videoSize");
	bytesLeft = (*videoSize);
	fanoutPar = &par("fanoutPar");
	witnessNbrPar = &par("witnessNbrPar");
	updNbPerRoundPar = &par("updNbPerRoundPar");

	fanout = (int) *fanoutPar;
	packetLen = (int) *packetLenPar;
	witnessNbr = (int) *witnessNbrPar;
	updNbPerRound = (int) *updNbPerRoundPar;

	encryptionsSum = 0.0;
	writeEncryptionsNbr = 0;

	bindToPort(serverPort);

	// initialization of round informations and event
	roundId = 0;
	roundDuration = &par("roundDuration");
	roundEvent = new cMessage;
	startTime = &par("startTime");
	scheduleAt((simtime_t) (*startTime), roundEvent);

	// Prepare result files
	writeUploadNbr = 0;

	string extension;
	char str[20];
	extension.append("_FANOUT.");
	sprintf(str, "%d", fanout);
	extension.append(str);
	extension.append("_WITNESS.");
	sprintf(str, "%d", witnessNbr);
	extension.append(str);
    extension.append("_UPDNB.");
    sprintf(str, "%d", updNbPerRound);
    extension.append(str);
    extension.append("_UPDLEN.");
    sprintf(str, "%d", packetLen);
    extension.append(str);
	extension.append(".dat");

	string uploadFileName = "uploadBandwidth";
	uploadFileName.append(extension);

	uploadFile = fopen(uploadFileName.c_str(), "w");
	if (uploadFile == NULL)
		error("uploadBandwidth.dat could not be opened");

	string uploadSnapshotFileName = "uploadBdwSnapshot";
	uploadSnapshotFileName.append(extension);

    uploadSnapshotFile = fopen(uploadSnapshotFileName.c_str(), "w");
    if (uploadSnapshotFile == NULL)
        error("uploadBdwSnapshot.dat could not be opened");

	writeReceivedUpdatesNbr = 0;

	string receivedUpdatesFileName = "receivedUpdates";
	receivedUpdatesFileName.append(extension);

	receivedUpdatesFile = fopen(receivedUpdatesFileName.c_str(), "w");
	if (receivedUpdatesFile == NULL)
		error("receivedUpdates.dat could not be opened");

}


void Source::finish() {
    for (int nodeId = 0; nodeId < Source::nodesNbr; nodeId++) {
        fprintf(Source::uploadSnapshotFile, "%lf\n", Source::avgUploadBandwidthArray[nodeId] / (double) nbAvgMeasures);
    }
    fclose(Source::uploadSnapshotFile);
	fclose(uploadFile);

	delete[] avgUploadBandwidthArray;
    delete[] uploadBandwidthArray;
    delete[] uploadWitnessBandwidthArray;
}


void Source::handleMessage(cMessage *msg) {

	/* Compute the IP addresses of all clients, and store them in nodesAddr */
	if (membershipNeverComputed) {
		for (int id = 1; id < simulation.getLastModuleId(); id++) {
			cModule *mod = simulation.getModule(id);
			if (strcmp(mod->getComponentType()->getName(), "ClientNode") == 0) {
				IPvXAddress nodeAddr = IPAddressResolver().addressOf(mod,IPAddressResolver().ADDR_IPv4);
				ev << "IP addr found : " << nodeAddr.get4() << "\n";
				nodesAddr.push_back(nodeAddr);
			}
		}
		nodesNbr = nodesAddr.size();
		uploadBandwidthArray = new long[nodesNbr];
		uploadWitnessBandwidthArray = new long[nodesNbr];
		membershipNeverComputed = false;
		nbAvgMeasures = 0;
		avgUploadBandwidthArray = new double[nodesNbr];
	}

	/* Handle new round messages */
	if (msg->isSelfMessage()) {
		if (packetLen * updNbPerRound > bytesLeft)
			bytesLeft = 0;

		/* Reschedule roundEvent if there are bytes left to send */
		if (bytesLeft != 0) {
			/* Find fanout new clients, all different the one from the others */
			srand(roundId);
			int clientsId[fanout];
			for (int clientIndex = 0; clientIndex < fanout; ++clientIndex) {
				int toPartnerId;
				bool yetPresent = true;

				while (yetPresent)	{
					toPartnerId = rand() % Source::nodesNbr;
					yetPresent = false;
					for (int prevClient = 0; prevClient < clientIndex; ++prevClient) {
						if (clientsId[prevClient] == toPartnerId) {
							yetPresent = true;
							break;
						}
					}
				}
				clientsId[clientIndex] = toPartnerId;
				injectUpdates(toPartnerId);
			}
			bytesLeft -= packetLen * updNbPerRound;
			scheduleAt(simTime() + (*roundDuration), roundEvent);
		} else
			delete roundEvent;

		++roundId;
	}
}

void Source::injectUpdates(int destNodeId) {
	BriefPacket *briefPacket = new BriefPacket("SERVE");
	briefPacket->setSenderId(SERVER_ID);
	briefPacket->setMsgType(SERVE);

	briefPacket->setUpdatesIdArraySize(updNbPerRound);
	for (int updatePos = 0; updatePos < updNbPerRound; ++updatePos)
		briefPacket->setUpdatesId(updatePos, (updNbPerRound * roundId) + 1 + updatePos);

	sendToUDP(briefPacket, serverPort, Source::nodesAddr[destNodeId], clientsPort);
}
