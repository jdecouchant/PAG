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

#include "Test.h"
#include<fstream>
using namespace std;
///*fonction intermediaire renvoyant vraie si a appartient au vecteur v*/
bool App(int a, vector<int> v)
{
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == a) return true;
    return false;
}

static int id = 1;
static vector<int> senders_id;
bool open = true;
static int nbNodes;
ofstream fichier("resultat.txt",ios::out|ios::trunc);
static long double somme = 0;
Define_Module(Test);

void Test::initialize(int stage)
{
    if(stage!=4)
        return;
    nbMsgRcvd = 0;
    nbBitsRcvd = 0;
    nbSender = par("nbSender");
    msgSize = par("msgSize");

    startTime = par("startTime");

    long _batchSize = par("batchSize");
    if((_batchSize < 0) || (((long)(unsigned int)_batchSize) != _batchSize))
        throw cRuntimeError("invalid 'batchSize=%ld' parameter at '%s' module",_batchSize, getFullPath().c_str());
    batchSize = (unsigned int)_batchSize;
    maxInterval = par("maxInterval");

    intvlStartTime = intvlLastPkTime = 0;
    intvlNumPackets = intvlNumBits = 0;

    period = new cMessage("period");
    scheduleAt((simtime_t)startTime,period);
    //counter = 0;
    if((int)senders_id.size()!=nbSender)
        senders_id.push_back(getParentModule()->getIndex());

    Bit_secVector.setName("throughput (bit/sec)");
    Pk_secVector.setName("packet/sec");
}


void Test::handleMessage(cMessage* msg)
{
    if(msg->isSelfMessage())
    {
        if(App(getParentModule()->getIndex(),senders_id))
        {
            RRBmsg* m = createMessage();
            sendMessage(m);
        }
        scheduleAt(simTime()+(simtime_t)startTime,msg);
    }
    else
    {

        RRBmsg* m = check_and_cast<RRBmsg*>(msg);
        receiveMessage(m);
    }
}

void Test::finish()
{
    double duration = (simTime() - simulation.getWarmupPeriod()).dbl();
    throughtput = nbBitsRcvd/duration;
    ev <<"=================\n";
    ev << "Node = "<<getParentModule()->getIndex()<<"\n";
    ev <<"nbMsgRcvd = "<<nbMsgRcvd<<"\n";
    ev <<"nbBitsRcvd = "<<nbBitsRcvd<<"\n";
    ev <<"Throughput = "<<throughtput<<"\n";

    //ev <<"Elapsed_time = "<<lastReceptionTime.dbl() - simulation.getWarmupPeriod().dbl()<<"\n";
    ev <<"=================\n";
    recordScalar("#nbMsgRcvd",nbMsgRcvd);
    recordScalar("#AvgThroughput",throughtput);
    somme = somme + throughtput;

    if(open)
    {
       //nbNodes = getParentModule()->getParentModule()->par("n");
       nbNodes = getParentModule()->getSubmodule("RRB")->par("nbNodes");
       open = false;
    }
    if(getParentModule()->getIndex()==nbNodes-1)
    {
        long double moyenne = somme/nbNodes;
        fichier<<nbSender<<"\t"<<moyenne<<"\n";
        fichier.close();
    }

}

RRBmsg* Test::createMessage()
{
    RRBmsg* m = new RRBmsg("MessageTest");
    Node n;
    n.node_id = getParentModule()->getIndex();
    //n.address = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);
    m->setOrigine(n);
    m->setSrc(n);
    m->setRRBmsg_id(id);
    m->setByteLength(msgSize);
    id++;
    return m;
}

void Test::sendMessage(RRBmsg* msg)
{
    ev <<"===============\n";
    ev <<"Start to send message  with ID = "<<msg->getRRBmsg_id()<<" ......\n";
    ev <<"===============\n";
    send(msg,"appOut");
}

void Test::receiveMessage(RRBmsg* msg)
{
    if(simTime() >= simulation.getWarmupPeriod())
        updateStats(simTime(), msg->getBitLength());
    //ev <<"======================\n";
    //ev <<"Je suis le noeud : "<<getParentModule()->getIndex()<<" et j'ai re�u le message d'ID : "<<msg->getRRBmsg_id()<<"\n";
    //ev <<"IP_source = "<<msg->getSrc().address<<"\n";
    //ev <<"source_id = "<<msg->getSrc().node_id<<"\n";
    //ev <<"======================\n";
    delete msg;
}

void Test::updateStats(simtime_t now, unsigned long bits)
{
    nbMsgRcvd++;
    nbBitsRcvd+=bits;

    //prise en compte des packets dans le nouveau intervalle
    if (intvlNumPackets >= batchSize || now-intvlStartTime >= maxInterval)
        beginNewInterval(now);

    intvlNumPackets++;
    intvlNumBits += bits;
    intvlLastPkTime = now;
}

void Test::beginNewInterval(simtime_t now)
{
    simtime_t duration = now - intvlStartTime;

    //On enregistre les mesures
    double bitpersec = intvlNumBits/duration.dbl();
    double pkpersec = intvlNumPackets/duration.dbl();

    Bit_secVector.recordWithTimestamp(intvlStartTime, bitpersec);
    Pk_secVector.recordWithTimestamp(intvlStartTime, pkpersec);

    //on r�initialise les compteurs
    intvlStartTime = now;
    intvlNumPackets = intvlNumBits = 0;
}
