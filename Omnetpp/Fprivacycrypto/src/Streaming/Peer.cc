#include "base.h"
#include "Peer.h"
#include "Source.h"
#include "IPAddressResolver.h"
#include "packets_m.h"
#include "Buffermap.h"

#include <algorithm>
#include <ctime>

Define_Module(Peer);

//#define CHECKS // TODO: uncomment to check code

void Peer::initialize() {

    selfId = NULL_NODE_ID;
    bindToPort(clientsPort);

    packetLen = Source::packetLen;
    fanout = Source::fanout;

    /*** Initialization of round parameters ***/

    roundId = 1;
    roundDuration = &par("roundDuration");
    roundEvent = new cMessage;
    startTime = &par("startTime");
    scheduleAt((simtime_t) *startTime + (simtime_t) *roundDuration, roundEvent); // Clients start one round after the source

    nbrRoundAfterEnd = 2*RTE;
    nbrRoundLeftAfterEnd = nbrRoundAfterEnd;

    /*** Statistics ***/

    lastMeasure = 0; // used for measuring periods of time, for collecting statistics per

    nbrReceivedUpdates = 0;
    uploadSize = 0;
    uploadWitnessingSize = 0;
}

void Peer::finish() {
    // TODO: activate
    //    delete [] selfWitnessedBM;
    // write percentage of received updates
    fprintf(Source::receivedUpdatesFile,
            "%li %f\n",
            (long) selfId,
            (float) nbrReceivedUpdates / ( Source::updNbPerRound * ((float) roundId - (float) nbrRoundAfterEnd - 1))
    );
    ++Source::writeReceivedUpdatesNbr;

    // the last node to write its results close the file
    if (Source::writeReceivedUpdatesNbr == Source::nodesNbr)
        fclose(Source::receivedUpdatesFile);
}

vector<int> getWitnesses(int nodeId) {
    vector<int> witnesses;
    for (int i = 0; i < Source::witnessNbr; i++) {
        witnesses.push_back((nodeId + i + 1) % Source::nodesNbr);
    }
    return witnesses;
}

vector<int> getWitnessed(int nodeId) {
    vector<int> witnessed;
    for (int i = 0; i < Source::witnessNbr; i++) {
        witnessed.push_back((nodeId - i - 1 + Source::nodesNbr) % Source::nodesNbr);
    }
    return witnessed;
}

void Peer::handleMessage(cMessage* msg) {
    /* A node compute its location in array nodesAddr exactly once */
    if (selfId == NULL_NODE_ID)	{
        // Find selfId value, based on the position of the node in the nodesAddr array of the source
        IPvXAddress selfAddr = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);
        for (int nodeId = 0; nodeId < Source::nodesNbr; ++nodeId) {
            if (Source::nodesAddr[nodeId] == selfAddr) {
                selfId = nodeId;
                break;
            }
        }
        selfWitnesses = getWitnesses(selfId);
        selfWitnessed = getWitnessed(selfId);
        if (MAINTAIN_WITNESSED_BM) {
            for (unsigned i = 0; i < selfWitnessed.size(); i++) {
                int witnessedId = selfWitnessed[i];
                selfWitnessedBM.insert(pair<int, Buffermap *>(witnessedId, new Buffermap()));
            }
        }
    }

    /* Handle new round messages */
    if (msg->isSelfMessage()) {

        this->updatesToPropose = bm.getUpdatesUntil(roundId+1, 2);
        updatesToAvoid = bm.getUpdatesUntil(roundId+1, 4);

        if (roundId == 20) {
            if (Source::writeEncryptionsNbr == 0) {
                string extension;
                char str[20];
                extension.append("encryptions_FANOUT.");
                sprintf(str, "%d", fanout);
                extension.append(str);
                extension.append("_WITNESS.");
                sprintf(str, "%d", Source::witnessNbr);
                extension.append(str);
                extension.append("_UPDNB.");
                sprintf(str, "%d", Source::updNbPerRound);
                extension.append(str);
                extension.append("_UPDLEN.");
                sprintf(str, "%d", Source::packetLen);
                extension.append(str);
                extension.append(".dat");

                Source::encryptionsFile = fopen(extension.c_str(), "w");
            }
            Source::encryptionsSum += nbEncryptions;
            Source::primeSum += nbPrimes;
            Source::primeSum512 += nbPrimes512;
            Source::writeEncryptionsNbr++;

            if (Source::writeEncryptionsNbr == Source::nodesNbr) {
                fprintf(Source::encryptionsFile, "%lf\n%lf\n%lf\n",
                        Source::encryptionsSum / (double) Source::nodesNbr,
                        Source::primeSum / (double) Source::nodesNbr,
                        Source::primeSum512 / (double) Source::nodesNbr);
                fclose(Source::encryptionsFile);
            }
        }
        nbEncryptions = 0;
        nbPrimes = 0;
        nbPrimes512 = 0;

        // Collect statistics
        int mTime = 4; // collected data are smoothed every 4 seconds
        if (simTime() - lastMeasure >= mTime) {
            lastMeasure = simTime();

            Source::uploadBandwidthArray[selfId] = uploadSize / mTime;
            Source::uploadWitnessBandwidthArray[selfId] = uploadWitnessingSize / mTime;
            ++Source::writeUploadNbr;

            // last node must collect, and write the results
            if (Source::writeUploadNbr == Source::nodesNbr) {

                if (roundId >= 10 && roundId <= 280) {
                    Source::nbAvgMeasures++;
                    for (int nodeId = 0; nodeId < Source::nodesNbr; nodeId++) {
                        Source::avgUploadBandwidthArray[nodeId] += (double) Source::uploadBandwidthArray[nodeId] / (double) 1000.0;
                    }
                }

                float averageUpload = 0;
                float peakUpload = 0;
                float averageWitnessUpload = 0;
                float peakWitnessUpload = 0;
                for (int nodeId = 0; nodeId < Source::nodesNbr; nodeId++) {
                    averageUpload += (float) Source::uploadBandwidthArray[nodeId] / (float) Source::nodesNbr;
                    peakUpload = max(peakUpload, (float) Source::uploadBandwidthArray[nodeId]);

                    averageWitnessUpload += (float) Source::uploadWitnessBandwidthArray[nodeId] / (float) Source::nodesNbr;
                    peakWitnessUpload = max(peakWitnessUpload, (float) Source::uploadWitnessBandwidthArray[nodeId]);
                }

                // print average, and peak upload bandwidth for correct, and colluder nodes
                fprintf(Source::uploadFile, "%li\t%f\t%f\t%f\t%f\n", ((long) roundId) * ((long) *roundDuration),
                        averageUpload / 1000.0, peakUpload / 1000.0,
                        averageWitnessUpload / 1000.0, peakWitnessUpload / 1000.0);
                Source::writeUploadNbr = 0;
            }
            uploadSize = 0;
            uploadWitnessingSize = 0;
        }

        if (Source::bytesLeft || nbrRoundLeftAfterEnd) {

            // Actualisation des BMs des noeuds surveillés
            if (MAINTAIN_WITNESSED_BM) {
                for (int i = 0; i < Source::witnessNbr; i++) {
                    int witnessedId = selfWitnessed[i];
                    selfWitnessedBM.at(witnessedId)->actualize(roundId);
                }
            }

            nbrReceivedUpdates += bm.actualize(roundId);
            sendKEY_RQST();

            scheduleAt(simTime() + (simtime_t) *roundDuration, msg); // schedule next round event
            ++roundId;
            if (not Source::bytesLeft)
                --nbrRoundLeftAfterEnd;
        } else
            delete msg;

    } else if (strcmp(msg->getClassName(), "HistoryPacket") == 0) {
        HistoryPacket *histPacket = check_and_cast<HistoryPacket *> (msg);
        switch (histPacket->getMsgType()) {
        case KEY_RQST:
            receiveKEY_RQST(histPacket);
            break;
        case KEY_RESP:
            receiveKEY_RESP(histPacket);
            break;       
        case ACK_SERVE:
            receiveACK_SERVE(histPacket);
            break;
        case ACK_NODE_WITN:
            receiveACK_NODE_WITN(histPacket);
            break;
        case ACK_WITN_WITN:
            receiveACK_WITN_WITN(histPacket);
            break;       
        case SHARE_CRYPTO:
            receiveSHARE_CRYPTO(histPacket);
            break;
        default:
            error("Peer (handleMessage) unknown HistoryPacket message received");
            break;
        }

    } else if (strcmp(msg->getClassName(), "BriefPacket") == 0) {
        BriefPacket *briefPacket = check_and_cast<BriefPacket *> (msg);
        switch (briefPacket->getMsgType()) {
        case SERVE:
            receiveSERVE(briefPacket);
            break;
        default:
            error("Peer (handleMessage) unknown BriefPacket message received");
            break;
        }
    } else
        error("Peer (handleMessage) unknown message received");
}

/* Basic PRNG */
void Peer::seedPRNG() {
    // this is a bijection from (roundId, selfId, BalOrPush) to the integer range
    srand(roundId + 1000*selfId + time(0)*1000);
}

int Peer::PRNG () {
    return rand() % Source::nodesNbr;
}

void Peer::sendKEY_RQST() {
    seedPRNG();

    int clientsId[fanout];
    for (int clientIndex = 0; clientIndex < fanout; ++clientIndex) {

        // Select new nodes to send Propose to
        int toPartnerId;
        bool yetPresent = true;
        while (yetPresent)  {
            toPartnerId = rand() % Source::nodesNbr;
            while (toPartnerId == selfId)
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

        // Send Propose message
        HistoryPacket *histPacket = new HistoryPacket("KEY_RQST");
        histPacket->setMsgType(KEY_RQST);
        histPacket->setSenderId(selfId);

        // stats
        uploadSize += MSG_TYPE_SIZE + ROUND_ID_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + SIGN_SIZE;

        sendToUDP(histPacket, clientsPort, Source::nodesAddr[toPartnerId], clientsPort);
        nbEncryptions++;
    }
}

void Peer::receiveKEY_RQST(HistoryPacket *msg) {
    sendKEY_RESP(msg->getSenderId());
    delete msg;
}

void Peer::sendKEY_RESP(int toNodeId) {
    // Send Propose message
    HistoryPacket *histPacket = new HistoryPacket("KEY_RESP");
    histPacket->setMsgType(KEY_RESP);
    histPacket->setSenderId(selfId);

    // Send the encryption of the updates that are already possessed from the N last rounds
    // Use the primary number that has been received to compute this encryption
//    vector<int> updates = bm.getUpdatesUntil(roundId, 2);
    histPacket->setUpdatesIdArraySize(updatesToAvoid.size());
    for (unsigned updatePos = 0; updatePos < updatesToAvoid.size(); updatePos++)
        histPacket->setUpdatesId(updatePos, updatesToAvoid[updatePos]);

    // stats
    uploadSize += MSG_TYPE_SIZE + ROUND_ID_SIZE + RECEIVER_ID_SIZE + PRIME_NUMBER_SIZE*(fanout-1) + SIGN_SIZE + ENCRYPTION_SIZE * updatesToAvoid.size();

    sendToUDP(histPacket, clientsPort, Source::nodesAddr[toNodeId], clientsPort);
    nbEncryptions += 1;
    nbPrimes512 += updatesToAvoid.size(); // The encryption of the updates can be transmitted using a small modulo to save bandwidth
}

void Peer::receiveKEY_RESP(HistoryPacket *msg) {
    //mostRecentUpdates = bm.getUpdatesUntil(roundId, 2);
    vector<int> canReceive;
    for (unsigned i = 0; i < this->updatesToPropose.size(); i++) { // For all updates that we can send
        bool msgContainsUpd = false;
        for (unsigned j = 0; j < msg->getUpdatesIdArraySize(); j++) { // Do the partner node has it?
            if (msg->getUpdatesId(j) == updatesToPropose[i]) {// TODO: impact of last condition? {
                msgContainsUpd = true;
                break;
            }
        }
        if (!msgContainsUpd)
            canReceive.push_back(updatesToPropose[i]);
    }

    sendSERVE(msg->getSenderId(), canReceive, (msg->getUpdatesIdArraySize() - canReceive.size()) );

    delete msg;
}

void Peer::sendSERVE(int toNodeId, vector<int> canReceive, unsigned nbNotSent) {
    //vector<int> updates = bm.getUpdatesUntil(roundId, 2);

    if (canReceive.size() > 0) {
        BriefPacket *briefPacket = new BriefPacket("SERVE");
        briefPacket->setMsgType(SERVE);
        briefPacket->setSenderId(selfId);

        //        briefPacket->setUpdatesIdArraySize(updates.size());
        //        for (unsigned updatePos = 0; updatePos < updates.size(); updatePos++)
        //            briefPacket->setUpdatesId(updatePos, updates[updatePos]);

        briefPacket->setUpdatesIdArraySize(canReceive.size());
        for (unsigned updatePos = 0; updatePos < canReceive.size(); updatePos++)
            briefPacket->setUpdatesId(updatePos, canReceive[updatePos]);

        uploadSize += MSG_TYPE_SIZE + ROUND_ID_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + canReceive.size() * (8 * packetLen + COUNT_SIZE) + KEY_SIZE + SIGN_SIZE + ENCRYPTION_SIZE + nbNotSent*(UPDATES_ID_SIZE + COUNT_SIZE);

        sendToUDP(briefPacket, clientsPort, Source::nodesAddr[toNodeId], clientsPort);
        nbEncryptions += 2;
    }
}

void Peer::receiveSERVE(BriefPacket *msg) {
    bool fromServer = (msg->getSenderId() == SERVER_ID);
    vector<int> receivedUpdates;
    for (unsigned i = 0; i < msg->getUpdatesIdArraySize(); ++i) {
        receivedUpdates.push_back(msg->getUpdatesId(i));
    }
    bm.insertUpdates(roundId, receivedUpdates);
    nbEncryptions++;

    int senderId = msg->getSenderId();
    delete msg;

    if (!fromServer)
        sendACK_SERVE(senderId, receivedUpdates);
}

void Peer::sendACK_SERVE(int nodeThatProposed, vector<int> updList) {

    //    HistoryPacket *histPacket = new HistoryPacket("ACK_SERVE"); // TODO: to uncomment to get more realistic
    //    histPacket->setMsgType(ACK_SERVE);
    //    histPacket->setSenderId(selfId); // fromNodeId sent Propose to SelfId
    //    histPacket->setReceiverId(nodeThatProposed);
    //    sendToUDP(histPacket, clientsPort, Source::nodesAddr[nodeThatProposed], clientsPort);
    uploadSize += MSG_TYPE_SIZE + ROUND_ID_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + SIGN_SIZE + ENCRYPTION_SIZE + SIGN_SIZE;
    nbEncryptions += 1;
    nbPrimes += 1;

    int posId = rand() % selfWitnesses.size();
    //     for (unsigned i = 0; i < selfWitnesses.size(); i++) {
    int witnessId = selfWitnesses[posId];

    // Send ack_propose message to witnesses
    HistoryPacket *histPacket2 = new HistoryPacket("ACK_NODE_WITN");
    histPacket2->setMsgType(ACK_NODE_WITN);
    histPacket2->setSenderId(nodeThatProposed); // fromNodeId sent Propose to SelfId
    histPacket2->setReceiverId(selfId);

    histPacket2->setUpdatesIdArraySize(updList.size());
    for (unsigned updatePos = 0; updatePos < updList.size(); updatePos++)
        histPacket2->setUpdatesId(updatePos, updList[updatePos]);

    sendToUDP(histPacket2, clientsPort, Source::nodesAddr[witnessId], clientsPort);
    uploadSize += MSG_TYPE_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + ROUND_ID_SIZE + ENCRYPTION_SIZE + ENCRYPTION_SIZE + PRIME_NUMBER_SIZE*(fanout-1) + SIGN_SIZE;
    nbEncryptions += 1;
    //     }
}

void Peer::receiveACK_SERVE(HistoryPacket *msg) {
    delete msg;
}

void Peer::receiveACK_NODE_WITN(HistoryPacket *msg) {   
    vector<int> proposedUpdates;
    for (unsigned i = 0; i < msg->getUpdatesIdArraySize(); i++) {
        proposedUpdates.push_back(msg->getUpdatesId(i));
    }

#ifdef CHECKS
    // Check that the node that sent the message is monitored by us
    bool isAWitness = false;
    for (int i = 0; i < Source::witnessNbr; i++) {
        if ((msg->getReceiverId() + i + 1) % Source::nodesNbr == selfId) {
            isAWitness = true;
            if (MAINTAIN_WITNESSED_BM) {
                selfWitnessedBM.at(msg->getReceiverId())->insertUpdates(roundId, proposedUpdates);
            }
            break;
        }
    }
    if (not isAWitness)
        throw cException("receiveACK_NODE_WITN: %d has been proposed, but is not monitored by %d", msg->getReceiverId(), selfId);
#endif
    sendACK_WITN_WITN(msg->getSenderId(), msg->getReceiverId(), proposedUpdates);

    delete msg;
}

void Peer::sendACK_WITN_WITN(int nodeThatProposed, int nodeProposedTo, vector<int> proposedUpdates) {
    vector<int> witnessesOfNode = getWitnesses(nodeThatProposed);

    for (unsigned i = 0; i < witnessesOfNode.size(); i++) {
        int witnessId = witnessesOfNode[i];

        // Send ack message to witnesses
        //         HistoryPacket *histPacket = new HistoryPacket("ACK_WITN_WITN"); // TODO: uncomment to get more realistic
        //         histPacket->setMsgType(ACK_WITN_WITN);
        //         histPacket->setSenderId(nodeThatProposed);
        //         histPacket->setReceiverId(nodeProposedTo);
        //
        //         histPacket->setUpdatesIdArraySize(proposedUpdates.size());
        //         for (unsigned updatePos = 0; updatePos < proposedUpdates.size(); updatePos++)
        //             histPacket->setUpdatesId(updatePos, proposedUpdates[updatePos]);

        //sendToUDP(histPacket, clientsPort, Source::nodesAddr[witnessId], clientsPort);
        uploadSize += MSG_TYPE_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + ROUND_ID_SIZE + ENCRYPTION_SIZE + ENCRYPTION_SIZE + SIGN_SIZE;
        uploadWitnessingSize += MSG_TYPE_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + ROUND_ID_SIZE + ENCRYPTION_SIZE + ENCRYPTION_SIZE + SIGN_SIZE;
    }


    vector<int> otherWitnesses = getWitnesses(nodeProposedTo);
    for (unsigned i = 0; i < witnessesOfNode.size(); i++) {
        int witnessId = witnessesOfNode[i];

        if (witnessId != selfId) {
            // Send ack_propose message to witnesses
            //             HistoryPacket *histPacket = new HistoryPacket("SHARE_CRYPTO"); // TODO: uncomment to get more realistic
            //             histPacket->setMsgType(SHARE_CRYPTO);
            //             histPacket->setSenderId(selfId);
            //             histPacket->setReceiverId(witnessId);

            //sendToUDP(histPacket, clientsPort, Source::nodesAddr[witnessId], clientsPort); // TODO: to improve simulation time
            uploadSize += MSG_TYPE_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + ROUND_ID_SIZE + ENCRYPTION_SIZE + SIGN_SIZE;
            uploadWitnessingSize += MSG_TYPE_SIZE + SENDER_ID_SIZE + RECEIVER_ID_SIZE + ROUND_ID_SIZE + ENCRYPTION_SIZE + SIGN_SIZE;
        }
    }

}

void Peer::receiveACK_WITN_WITN(HistoryPacket *msg) {
#ifdef CHECKS
    bool isWitnessed = false;
    for (int i = 0; i < Source::witnessNbr; i++) {
        if ((msg->getSenderId() + i + 1) % Source::nodesNbr == selfId) {
            isWitnessed = true;
            break;
        }
    }
    if (!isWitnessed) {
        throw cException("receiveHAS_PROPOSED: %d proposed, but is not monitored by %d", msg->getSenderId(), selfId);
    }
#endif

    delete msg;
}

void Peer::receiveSHARE_CRYPTO(HistoryPacket *msg) {
    delete msg;
}
