#ifndef Source_H
#define Source_H

#include <vector>
#include <omnetpp.h>
#include "UDPAppBase.h"
#include "packets_m.h"
#include "Buffermap.h"

using namespace std;

class INET_API Source : public UDPAppBase {

public:

    static int nodesNbr;
    static vector<IPvXAddress> nodesAddr; // store the IP addresses of all clients*
    static long bytesLeft;

    static int packetLen;
    static int fanout;
    static int witnessNbr;

    static int writeUploadNbr; // count the number of clients that registered their upload figures
    static long *uploadBandwidthArray;
    static long *uploadWitnessBandwidthArray;
    static FILE *uploadFile;

    static long nbAvgMeasures;
    static double *avgUploadBandwidthArray;
    static FILE *uploadSnapshotFile;

    static int writeReceivedUpdatesNbr; // count the number of clients that registered the percentage of updates they received
    static FILE *receivedUpdatesFile;

    static double encryptionsSum;
    static double primeSum;
    static double primeSum512;
    static int writeEncryptionsNbr;
    static FILE  *encryptionsFile;

    static int nbSessions;

    static int updNbPerRound;

    Source();
    virtual ~Source();

protected:

    bool membershipNeverComputed; // used to compute only once nodesAddr and nodesNbr, when the source receives the first message

    // module parameters
    const static int serverPort = 1000;
    const static int clientsPort = 500;

    cPar *videoSize; // Size of the video to transmit in bytes
    cPar *packetLenPar; // Size of video chunks in bytes
    cPar *fanoutPar;    // Number of clients the source directly sends updates to
    cPar *witnessNbrPar;
    cPar *nbSessionsPar;
    cPar *updNbPerRoundPar;

    // Round events and counter
    cPar *startTime;
    cPar *roundDuration;
    int roundId;
    cMessage *roundEvent;

    void injectUpdates(int destNodeId);

    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage* msg);

};


#endif


