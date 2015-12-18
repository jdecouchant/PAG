#ifndef __INET_VIDEOSTREAM_H
#define __INET_VIDEOSTREAM_H

#include <omnetpp.h>
#include "UDPAppBase.h"
#include "Buffermap.h"
#include "packets_m.h"

using namespace std;

class INET_API Peer: public UDPAppBase {

private:
    int selfId;
    Buffermap bm; // Buffermap of the node

    vector<int> selfWitnesses;
    vector<int> selfWitnessed;
    map<int, Buffermap *> selfWitnessedBM;

    vector<int> updatesToPropose;
    vector<int> updatesToAvoid;

protected:

    const static int serverPort = 1000;
    const static int clientsPort = 500;

    int packetLen;
    int fanout;

    // Round event and counter
    int roundId;
    cPar *startTime;
    cPar *roundDuration;
    cMessage *roundEvent;

    int nbrRoundLeftAfterEnd;
    int nbrRoundAfterEnd;

    // Statistics
    long nbrReceivedUpdates;

    simtime_t lastMeasure;

    long uploadSize;
    long uploadWitnessingSize;
    
    int nbEncryptions;
    int nbPrimes, nbPrimes512;

protected:

    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    /* Basic PRNG */
    void seedPRNG();
    int PRNG();

    void sendKEY_RQST();
    void receiveKEY_RQST(HistoryPacket *msg);
    
    void sendKEY_RESP(int toNodeId);
    void receiveKEY_RESP(HistoryPacket *msg);

    void sendSERVE(int toNodeId, vector<int> canReceive, unsigned nbNotSent);
    void receiveSERVE(BriefPacket *msg);

    void sendACK_SERVE(int toNodeId, vector<int> updList);
    void receiveACK_SERVE(HistoryPacket *msg);
    void receiveACK_NODE_WITN(HistoryPacket *msg);

    void sendACK_WITN_WITN(int nodeThatServed, int nodeServed, vector<int> proposedUpdates);
    void receiveACK_WITN_WITN(HistoryPacket *msg);
    
    void sendSHARE_CRYPTO(int nodeThatServed);
    void receiveSHARE_CRYPTO(HistoryPacket *msg);

};

#endif

