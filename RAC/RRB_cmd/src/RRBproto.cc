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

Define_Module(RRBproto);

/////////////////////
/*fonction intermediaire renvoyant vraie si a appartient au vecteur v*/
bool isElt(int a, vector<int> v)
{
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == a) return true;
    return false;
}
////////////////////

//static int id = 1;
static bool build_rings = true;
static vector<Ring*> rings;
void RRBproto::initialize()
{
    numSessions = numBroken = packetsSent = packetsRcvd = bytesSent = bytesRcvd = 0;
    WATCH(numSessions);
    WATCH(numBroken);
    WATCH(packetsSent);
    WATCH(packetsRcvd);
    WATCH(bytesSent);
    WATCH(bytesRcvd);

    //Initialisation des paramètres du protocole
    nbNodes = par("nbNodes");
    t = par("t");
    init = true;

    //Ouverture d'une connection TCP
    TCPSocket socketIn;
    //socket = new TCPSocket();
    int portIn = par("connectPort");
    socketIn.bind(portIn);
    socketIn.setOutputGate(gate("tcpOut"));
    socketIn.listen();

    timer = new cMessage("timer");
    scheduleAt(1.0, timer);
}

void RRBproto::handleMessage(cMessage* msg)
{
    //	if(msg->arrivedOn("appIn"))
    //	{
    //		ev << "Bonjour le monde cruel\n";
    //		//send(msg,"tcpOut");
    //	}

    // à revoir!! trop d'appels de cette fonction
    //vector<Node> vecNodes = getNodes();

    /*On construit les anneaux une seule fois*/
    if(build_rings)
    {
        rings = buildRings();
        build_rings = false;
    }
    /*construction de la liste de predecesseurs et de successeurs pour chacun des
     * noeuds */
    if(init)
    {
        buildLists(rings);
        connectAll();
        init = false;
    }

    /*Mise en place du timer pour attendre le message RRBmsg des autres predecesseurs*/

    if(msg==timer) //le timer a expir�
    {

        //on decremente le compteur de chacun des elements du map
        for(map<int,Data>::iterator it=save_buffer.begin(); it!=save_buffer.end(); ++it)
        {
            it->second.counter--;
            if(it->second.counter == 0)
            {
                //on blackliste les noeuds qui n'ont pas envoy� leur message
                ev <<"======= Debut du blacklistage ======\n";
                for(int i=0;i<2*t+1;i++)
                {
                    if(!containNode(predecessors[i],it->second.buffer))
                    {
                        blacklist.push_back(predecessors[i]);
                    }
                }
                /*on nettoie les cases dont counter=0 apr�s le
                 * blacklistage*/
                save_buffer.erase(it);
            }
        }

        scheduleAt(simTime()+1.0,timer); //on réunitialise le timer
    }
    else
    {
        /*Le message est re�u pour la premi�re fois par le sous module RRB,
         * on le broadcaste. Ce message vient du module Test en dessus du RRB */
        if(msg->arrivedOn("appIn"))
        {
            RRBmsg *m = check_and_cast<RRBmsg*>(msg);
            /*on met l'id du message dans le buffer_msg_id
             * pour que le noeud qui a envoy� ne re�oive le m�me message */
            //buffer_msg_id.push_back(m->getRRBmsg_id());
            //buffer_msg_id.insert(m->getRRBmsg_id());
            //on broadcaste aux followers du noeud courant
            broadcast(m);
        }
        else
        {
            /*Reception d'un message RRBmsg*/
            if(strcmp(msg->getClassName(),"RRBmsg")==0)
            {
                RRBmsg *m = check_and_cast<RRBmsg*>(msg);

                /*On v�rifie si le noeud source appartient � la liste des predecesseurs
                 * du noeud courant*/
                if(!containNode(m->getSrc(),predecessors))
                {
                    ev <<"STOP: Le noeud source n'est pas dans predecessors\n";
                    return;
                }

                /*on v�rifie s'il(le noeud courant) reçoit le message pour la première fois */
                //if(!isElt(m->getRRBmsg_id(),buffer_msg_id))
                if((buffer_msg_id.insert(m->getRRBmsg_id())).second)
                {

                    //on delivre le message m
                    deliver(m->dup());
                    /*on enregistre l'Id du message dans le buffer buffer_msg_id*/
                    //buffer_msg_id.insert(m->getRRBmsg_id());
                    /*on enregistre le noeud source dans le buffer_pred*/
                    Data d;
                    d.counter = 3;
                    d.buffer.push_back(m->getSrc());
                    save_buffer[m->getRRBmsg_id()] = d;
                    /*on change la source contenue dans le message pour le broadcaster à nouveau
                     * aux followers du noeud courant*/
                    Node src;
                    src.node_id = getParentModule()->getIndex();
                    src.address = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);
                    m->setSrc(src);
                    if(src.node_id!=m->getOrigine().node_id)
                        broadcast(m);
                }
                else
                {
                    /*le noeud courant a d�j� re�u le message m, il enregistre donc le noeud source*/
                    //buffer_pred.push_back(m->getSrc());
                    save_buffer[m->getRRBmsg_id()].buffer.push_back(m->getSrc());
                }

            }
        }
    }

    //}

}

void RRBproto::handleTimer(cMessage *msg)
{

}

vector<Node> RRBproto::getNodes()
{
    vector<Node> vecNodes;
    /*On recup�re les adresses IP des noeuds et on le stcoke dans vecAdds
     * le vecteur 'simulation' contient tous les modules cr��s durant une simulation
     * le module de rang 1 correspond � la topologie r�seau, donc les sous modules commencent
     * � partir de 2*/
    for(int i=2;i<nbNodes+2;i++)
    {
        cModule *mod = simulation.getModule(i);
        Node n;
        n.node_id = mod->getIndex();
        n.address = IPAddressResolver().addressOf(mod,IPAddressResolver().ADDR_IPv4);
        vecNodes.push_back(n);
    }
    return vecNodes;
}

void RRBproto::connect(TCPSocket *sock, IPvXAddress connectAddress, const int connectPort)
{
    //nouveau connId si ce n'est pas la premi�re connexion
    sock->renewSocket();
    //connection
    setStatusString("connecting");
    sock->connect(connectAddress, connectPort);
    numSessions++;
}

void RRBproto::connectAll()
{
    socketsOut = new TCPSocket[nbNodes]();
    vector<Node> vecNodes = getNodes();
    for(int i=0;i<nbNodes;i++)
    {
        if(vecNodes.at(i).address!=IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4))
        {
            socketsOut[i].setOutputGate(gate("tcpOut"));
            connect(&socketsOut[i],vecNodes.at(i).address,(int)par("connectPort"));
        }
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
 * participent au protocole de telle sorte qu'ils n'aient pas les m�mes positions*/
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
        r->set_of_nodes.push_back(vecNodes[k]);
        tmp.push_back(k);
        for(int j=1;j<nbNodes;j++)
        {
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
    //    for(int i=0;i<2*t+1;i++)
    //    {
    //        ev <<"RING "<< i <<"\n";
    //        for(int j=0;j<nbNodes;j++)
    //        {
    //            ev<<rings[i]->set_of_nodes[j].node_id<<"\n";
    //            //ev<<rings[i]->set_of_nodes[j].address<<"\n";
    //        }
    //        ev <<"::::::::::::::\n";
    //    }
    //    ev <<"#############\n";

    //On parcourt ces anneaux pour construire les listes de predecesseurs et de successeurs
    IPvXAddress add_cur = IPAddressResolver().addressOf(getParentModule(),IPAddressResolver().ADDR_IPv4);

    for(int i=0;i<2*t+1;i++)
    {
        //traitement du premier elt d'un anneau
        if(rings[i]->set_of_nodes[0].address==add_cur)
        {
            predecessors.push_back(rings[i]->set_of_nodes[nbNodes-1]);
            followers.push_back(rings[i]->set_of_nodes[1]);
        }

        //traitement du dernier elt d'un anneau
        if(rings[i]->set_of_nodes[nbNodes-1].address==add_cur)
        {
            predecessors.push_back(rings[i]->set_of_nodes[nbNodes-2]);
            followers.push_back(rings[i]->set_of_nodes[0]);

        }

        for(int j=1;j<nbNodes-1;j++)
        {
            if(rings[i]->set_of_nodes[j].address==add_cur)
            {
                predecessors.push_back(rings[i]->set_of_nodes[j-1]);
                followers.push_back(rings[i]->set_of_nodes[j+1]);
            }
        }

    }


    //Juste une petite verification pour savoir si exactement la taille de chacune des listes vaut 2t+1
    //    ev<<"+++++++++++++\n";
    //    ev <<"SIZE_P = "<<predecessors.size()<<"\n";
    //    ev <<"SIZE_F = "<<followers.size()<<"\n";
    //    ev <<"ID = "<<getParentModule()->getIndex()<<"\n";
    //    for(int i=0;i<predecessors.size();i++)
    //    {
    //        ev << predecessors.at(i).node_id <<"\n";
    //        ev << followers.at(i).node_id <<"\n";
    //        ev <<"*********************\n";
    //    }
    //    ev<<"+++++++++++++\n";
}

void RRBproto::broadcast(RRBmsg* msg)
{
    for(int i=0;i<2*t+1;i++)
    {
        sendPacket(&socketsOut[followers.at(i).node_id],msg->dup());
    }
}

bool RRBproto::containNode(Node src, vector<Node> vecNodes)
{
    size_t i=0;
    while(i < vecNodes.size() && vecNodes[i].address!=src.address)
        i++;
    return i < vecNodes.size();
}


