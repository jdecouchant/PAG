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

#ifndef RRBPROTO_H_
#define RRBPROTO_H_
#include "TCPGenericCliAppBase.h"
#include "GenericAppMsg_m.h"
//#include "TCPSocket.h"
#include "IPAddressResolver.h"
#include "IPAddress.h"
#include "Util.h"
#include "RRBmsg_m.h"
#include <set>


class  RRBproto: public TCPGenericCliAppBase {

protected:
	vector<Node> predecessors; // la liste des predecesseurs
	vector<Node> followers; // la liste des successeurs


	TCPSocket *socketsOut;
	int nbNodes; // nombre de noeuds
	int t;   // nombres de noeuds Byzantins
	bool init; // sert dans la construction des anneaux
	//vector<int> buffer_msg_id;  //pour enregistrer l'ID des messages reçus
	set<int> buffer_msg_id;
	map<int,Data> save_buffer; //pour enregistrer les predecesseurs qui ont envoyé le message d'id i
	//vector<IPvXAddress> buffer_pred; //pour enregistrer les predecesseurs qui ont envoyé leur message
	cMessage* timer; //pour le timer;
	cMessage* sendTime; // pour le mecanisme de fairness
	cMessage* timeoutMsg;
	vector<Node> blacklist; // la liste des noeuds blacklistés
	long sent;
	map<int,long> received;
	cPacketQueue broadcast_queue;
	cPacketQueue forward_queue;

//protected:
public:
	/*rédéfinition de la méthode initialize() de la classe mère*/
	virtual void initialize();
	/*connecte le noeud courant au noeud d'adresse connectAddress*/
	//virtual void connect(TCPSocket *sock, IPvXAddress connectAddress,const int connectPort);
	virtual void connect(TCPSocket *sock, const char *connectAddress,const int connectPort);

	/*connecte tous les noeuds entre eux*/
	virtual void connect_to_followers();

	virtual void handleTimer(cMessage* msg);

	virtual void handleMessage(cMessage* msg);

	/*recupère tous les noeuds participants au protocole*/
	virtual vector<Node> getNodes();

	virtual void sendPacket(TCPSocket *sock, RRBmsg* msg);

	virtual void deliver(RRBmsg* msg);

	/*construit les différents anneaux de communication*/
	virtual vector<Ring*> buildRings();
	/*construit la liste de predecesseurs et de successeurs du noeud courant */
	virtual void buildLists(vector<Ring*> rings);
	/*Diffuse un message à tous les followers du noeud courant*/
	virtual void broadcast(RRBmsg* msg);

	virtual void forward(RRBmsg* msg);
	/*Vérifie si l'adresse passée en paramètre appartient au vecteur d'IP vecAdd*/
	virtual bool containNode(Node src, vector<Node> vecNodes);

	//virtual bool isPredecessor(Node src);
	/*renvoie la position du noeud qui suit le noeud d'id  id*/
	virtual int getNodeSuiv(int id,vector<Node> nodes);
	virtual void computeDistance(int id, int distance, map<int,int> &distTab, int origine);
	virtual double getDistAvg();
	virtual void finish();

};



#endif /* RRBPROTO_H_ */
