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

#include "RRBproto.h"
#include "TCPCommand_m.h"
#include<fstream>
Define_Module(RRBproto);
ofstream fic("dist.txt",ios::out|ios::app);
static double distance_moyenne = 0;

/////////////////////
/*fonction intermediaire renvoyant vraie si a appartient au vecteur v*/
bool isElt(int a, vector<int> v)
{
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == a) return true;
    return false;
}
////////////////////


static bool build_rings = true;
//static vector<int> node_ids;
static int ind = 0;
static int node_ids[10];
static vector<Ring*> rings;

#define MSGKIND_CONNECT 0x2000
#define MSGKIND_WAIT_PREDECESSOR 0x2001
#define MSGKIND_FAIRNESS 0x2002

void RRBproto::initialize()
{

    numSessions = numBroken = packetsSent = packetsRcvd = bytesSent = bytesRcvd = 0;
    WATCH(numSessions);
    WATCH(numBroken);
    WATCH(packetsSent);
    WATCH(packetsRcvd);
    WATCH(bytesSent);
    WATCH(bytesRcvd);

    //Initialisation des param√®tres du protocole
    nbNodes = par("nbNodes");
    t = par("t");
    init = true;
    sent = 0;
//    ev <<"/////////////////////////\n";
//    ev <<"node_id = "<<getParentModule()->getId()<<"\n";
//
//    node_ids.push_back(getParentModule()->getId());
//    ev <<"SIZE = "<<node_ids.size()<<"\n";
//    ev <<"/////////////////////////\n";
    node_ids[ind] = getParentModule()->getId();
    ind++;
    //Ouverture d'une connection TCP
    TCPSocket socketIn;
    //socket = new TCPSocket();
    int portIn = par("connectPort");
    socketIn.bind(portIn);
    socketIn.setOutputGate(gate("tcpOut"));
    socketIn.listen();

    timeoutMsg = new cMessage("timerConnect");
    timeoutMsg->setKind(MSGKIND_CONNECT);
    scheduleAt((simtime_t)1, timeoutMsg);

}

void RRBproto::handleMessage(cMessage* msg)
{
    if(build_rings)
    {
        rings = buildRings();
        build_rings = false;
    }

    if(msg->isSelfMessage())
    {

        handleTimer(msg);
    }
    else
    {


        /*Le message est reçu pour la première fois par le sous module RRB,
         * on le broadcaste. Ce message vient du module Test en dessus du RRB */
        if(msg->arrivedOn("appIn"))
        {
            RRBmsg *m = check_and_cast<RRBmsg*>(msg);
            broadcast_queue.insert(m);
        }
        else
        {
            /*Reception d'un message RRBmsg*/
            if(strcmp(msg->getClassName(),"RRBmsg")==0)
            {
                RRBmsg *m = check_and_cast<RRBmsg*>(msg);

                /*On vérifie si le noeud source appartient à la liste des predecesseurs
                 * du noeud courant*/
                if(!containNode(m->getSrc(),predecessors))
                {
                    ev <<"STOP: Le noeud source n'est pas dans predecessors\n";
                    return;
                }

                /*on vérifie s'il(le noeud courant) reçoit le message pour la première fois */
                //if(!isElt(m->getRRBmsg_id(),buffer_msg_id))
                if((buffer_msg_id.insert(m->getRRBmsg_id())).second)
                {

                    Node cur;
                    cur.node_id = getParentModule()->getIndex();
                    //cur.address = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);
                    //on delivre le message

                    m->setRecepTime(simTime());
                    deliver(m->dup());

                    /*on enregistre l'Id du message dans le buffer buffer_msg_id*/
                    //buffer_msg_id.insert(m->getRRBmsg_id());
                    /*on enregistre le noeud source dans le buffer_pred*/
                    Data d;
                    d.counter = 3;
                    d.buffer.push_back(m->getSrc());
                    save_buffer[m->getRRBmsg_id()] = d;
                    /*on change la source contenue dans le message pour le broadcaster √† nouveau
                     * aux followers du noeud courant*/

                    if(cur.node_id!=m->getOrigine().node_id)
                    {    //broadcast(m);
                        m->setSrc(cur);
                        m->setHopCount(m->getHopCount()+1);
                        //forward(m);
                        forward_queue.insert(m);
                    }


                }
                else
                {
                    /*le noeud courant a déjà reçu le message m, il enregistre donc le noeud source*/
                    //buffer_pred.push_back(m->getSrc());
                    save_buffer[m->getRRBmsg_id()].buffer.push_back(m->getSrc());
                }


            }


        }

    }
}

void RRBproto::handleTimer(cMessage *msg)
{
    /*On construit les anneaux une seule fois*/

    switch(msg->getKind())
    {
    case MSGKIND_CONNECT:
        buildLists(rings);
        connect_to_followers();
        sendTime = new cMessage("sendTime");
        sendTime->setKind(MSGKIND_FAIRNESS);
        scheduleAt(simTime()+0.00009*(2*t+1),sendTime);

        timer = new cMessage("timer");
        timer->setKind(MSGKIND_WAIT_PREDECESSOR);
        scheduleAt(simTime()+(simtime_t)1, timer);

        break;
    case MSGKIND_FAIRNESS:
        if(!forward_queue.empty())
        {
            RRBmsg* msg = check_and_cast<RRBmsg*>(forward_queue.front());
            if(sent>=received[msg->getOrigine().node_id] || broadcast_queue.empty())
            {

                //broadcast(msg);
                forward(msg);
                received[msg->getOrigine().node_id]++;
                forward_queue.pop();
                delete msg;
            }
            else if(!broadcast_queue.empty())
            {
                RRBmsg* msg = check_and_cast<RRBmsg*>(broadcast_queue.front());
                //msg->setSendTime(simTime());
                broadcast(msg);
                broadcast_queue.pop();
                delete msg;
                sent++;
            }
        }else if(!broadcast_queue.empty())
        {
            RRBmsg* msg = check_and_cast<RRBmsg*>(broadcast_queue.front());
            broadcast(msg);
            broadcast_queue.pop();
            delete msg;
            sent++;
        }
        scheduleAt(simTime() + 0.00009*(2*t+1), sendTime);
        break;
    case MSGKIND_WAIT_PREDECESSOR:
        //on decremente le compteur de chacun des elements du map
        for(map<int,Data>::iterator it=save_buffer.begin(); it!=save_buffer.end(); ++it)
        {
            it->second.counter--;
            if(it->second.counter == 0)
            {
                //on blackliste les noeuds qui n'ont pas envoyé leur message
                ev <<"======= Debut du blacklistage ======\n";
                for(int i=0;i<2*t+1;i++)
                {
                    if(!containNode(predecessors[i],it->second.buffer))
                    {
                        blacklist.push_back(predecessors[i]);
                        //blacklist.push_back(*i);
                    }
                }
                /*on nettoie les cases dont counter=0 après le
                 * blacklistage*/
                save_buffer.erase(it);
            }
        }

        scheduleAt(simTime()+1,timer); //on r√©unitialise le timer
        break;
        //    default:
        //            ev << "Pas de selfMessage \n";
    }


}

vector<Node> RRBproto::getNodes()
{
    vector<Node> vecNodes;
    /*On recupère les adresses IP des noeuds et on le stcoke dans vecAdds
     * le vecteur 'simulation' contient tous les modules créés durant une simulation
     * le module de rang 1 correspond à la topologie réseau, donc les sous modules commencent
     * à partir de 2*/
    for(int i=2;i<nbNodes+2;i++)
    {
        cModule *mod = simulation.getModule(i);
        Node n;
        n.node_id = mod->getIndex();
        //n.address = IPAddressResolver().addressOf(mod,IPAddressResolver().ADDR_IPv4);
        vecNodes.push_back(n);
    }


//    for(int i;i<ind;i++)
//    {
//        Node n;
//        n.node_id = node_ids[i];
//        vecNodes.push_back(n);
//    }
   return vecNodes;
}

void RRBproto::connect(TCPSocket *sock, const char *connectAddress, const int connectPort)
//void RRBproto::connect(TCPSocket *sock, IPvXAddress connectAddress,const int connectPort)
{
    //nouveau connId si ce n'est pas la première connexion
    sock->renewSocket();
    //connection
    setStatusString("connecting");
    sock->connect(IPAddressResolver().resolve(connectAddress), connectPort);
    //sock->connect(connectAddress,connectPort);
    numSessions++;
}

void RRBproto::connect_to_followers()
{
    int size = (int)followers.size();
    socketsOut = new TCPSocket[size]();
    char name[15];
    for(int i=0;i<size;i++)
    {

        sprintf(name,"client[%d]",followers.at(i).node_id);
        socketsOut[i].setOutputGate(gate("tcpOut"));
        connect(&socketsOut[i],name,(int)par("connectPort"));
    }
}
void RRBproto::sendPacket(TCPSocket *sock, RRBmsg* msg)
{
    sock->send(msg);
    packetsSent+=1;
    bytesSent+=msg->getByteLength();
}

void RRBproto::deliver(RRBmsg* msg)
{

    send(msg,"appOut");

}

/*Renvoie un vecteur d'anneaux et sur chaque anneau on place tous les noeuds qui
 * participent au protocole de telle sorte qu'ils n'aient pas les mêmes positions*/
vector<Ring*> RRBproto::buildRings()
{
    vector<Node> vecNodes = getNodes();
    vector<Ring*> res;
    res.resize(2*t+1);

    for(int i=0;i<2*t+1;i++)
        res[i] = new Ring();
    for(int i=0;i<2*t+1;i++)
    {
        vector<int> tmp;
        Ring *r = res[i];
        int k = intuniform(0,nbNodes-1);
        //int p = node_ids[3];
        //int k = intrand(nbNodes);
        r->set_of_nodes.push_back(vecNodes[k]);
        tmp.push_back(k);
        for(int j=1;j<nbNodes;j++)
        {
            //k = intrand(nbNodes);
            k = intuniform(0,nbNodes-1);
            while(isElt(k,tmp)) k = intuniform(0,nbNodes-1);
            r->set_of_nodes.push_back(vecNodes[k]);
            tmp.push_back(k);
        }
    }

    return res;

}


/*Construit les listes de predecesseurs et de successeurs d'un noeud*/
void RRBproto::buildLists(vector<Ring*> rings)
{

    //On parcourt ces anneaux pour construire les listes de predecesseurs et de successeurs
    //IPvXAddress add_cur = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);
    int node_cur = getParentModule()->getIndex();
    for(int i=0;i<2*t+1;i++)
    {
        //traitement du premier elt d'un anneau
        if(rings[i]->set_of_nodes[0].node_id==node_cur)
        {
            predecessors.push_back(rings[i]->set_of_nodes[nbNodes-1]);
            followers.push_back(rings[i]->set_of_nodes[1]);
        }

        //traitement du dernier elt d'un anneau
        if(rings[i]->set_of_nodes[nbNodes-1].node_id==node_cur)
        {
            predecessors.push_back(rings[i]->set_of_nodes[nbNodes-2]);
            followers.push_back(rings[i]->set_of_nodes[0]);

        }

        for(int j=1;j<nbNodes-1;j++)
        {
            if(rings[i]->set_of_nodes[j].node_id==node_cur)
            {
                predecessors.push_back(rings[i]->set_of_nodes[j-1]);
                followers.push_back(rings[i]->set_of_nodes[j+1]);
            }
        }

    }

}

void RRBproto::broadcast(RRBmsg* msg)
{

    for(int i=0;i<2*t+1;i++)
    {
        //int k = followers.at(i).node_id;
        RRBmsg* copy = msg->dup();
        copy->setSendTime(simTime());
        //sendPacket(&socketsOut[k],copy);
        sendPacket(&socketsOut[i],copy);
    }

}

void RRBproto::forward(RRBmsg* msg)
{

    for(int i=0;i<2*t+1;i++)
    {
        //int k = followers.at(i).node_id;
        sendPacket(&socketsOut[i],msg->dup());
    }

}

bool RRBproto::containNode(Node src, vector<Node> vecNodes)
{
    size_t i=0;
    while(i < vecNodes.size() && vecNodes[i].node_id!=src.node_id)
        i++;
    return i < vecNodes.size();
}

/////////////////////////////////////////////
int RRBproto::getNodeSuiv(int id,vector<Node> nodes)
{
    size_t i = 0;
    while(i<nodes.size() && nodes[i].node_id!=id )
        i++;
    if(i+1 == nodes.size())
    {
        return 0;
    }
    else
    {
        return i+1;
    }
}

void RRBproto::computeDistance(int id, int distance, map<int,int> &distTab, int origine)
{

    for(int i=0;i<2*t+1;i++)
    {
        int k = getNodeSuiv(id,rings[i]->set_of_nodes);

        if(rings[i]->set_of_nodes[k].node_id!=origine && (distTab[rings[i]->set_of_nodes[k].node_id]==0||
                distTab[rings[i]->set_of_nodes[k].node_id]>distance+1) )
        {
            distTab[rings[i]->set_of_nodes[k].node_id] = distance+1;

            computeDistance(rings[i]->set_of_nodes[k].node_id,distance+1,distTab,origine);
        }
    }


}

double RRBproto::getDistAvg()
{
    map<int,int> disTab;
    int myID = getParentModule()->getIndex();
    computeDistance(myID,0,disTab,myID);
    int somme = 0;
    for(map<int,int>::iterator it=disTab.begin();it!=disTab.end();++it)
    {
        somme = somme + it->second;
    }
    return (double)somme/disTab.size();
}


void RRBproto::finish()
{
//        distance_moyenne = distance_moyenne + getDistAvg();
//        if(getParentModule()->getIndex()==nbNodes-1)
//        {
//            distance_moyenne = distance_moyenne/nbNodes;
//            fic << nbNodes << " "<<distance_moyenne<<"\n";
//            fic.close();
//        }


}

///////////////////////////////////////////

