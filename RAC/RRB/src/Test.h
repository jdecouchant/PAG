//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef TEST_H_
#define TEST_H_
#include <omnetpp.h>
#include "RRBmsg_m.h"
#include "IPAddressResolver.h"
#include "IPAddress.h"
#include "Util.h"
class Test : public cSimpleModule
{
protected:
    int nbSender;
    int msgSize;
    int nbMsgRcvd;
    unsigned long long nbBitsRcvd;
    cMessage* period;
    long double throughtput;

    simtime_t startTime; // debut
    unsigned int batchSize;   // nombre de packets par lot
    simtime_t maxInterval;

    //mesure du débit dans l'intervalle courant
    simtime_t intvlStartTime;
    simtime_t intvlLastPkTime;
    unsigned long intvlNumPackets;
    unsigned long intvlNumBits;

    // statistiques
    cOutVector Bit_secVector;
    cOutVector Pk_secVector;
protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 5;}
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual RRBmsg* createMessage();
    virtual void sendMessage(RRBmsg* msg);
    virtual void receiveMessage(RRBmsg* msg);

    virtual void updateStats(simtime_t now, unsigned long bits);
    virtual void beginNewInterval(simtime_t now);
};

#endif /* TEST_H_ */
