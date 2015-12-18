/*
 * onionrouting.cc
 *
 *  Created on: Feb 5, 2014
 *      Author: diarraa
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#include <peerreview/tools/parameters.h>
#include <peerreview/transport/id/simple.h>
#include "onionrouting.h"

#define SUBSYSTEM "onionrouting"

OnionRouting::OnionRouting(IdentityTransport *transport, Parameters *params,
        int nbRelais, bool logEvents) {
    this->transport = transport;
    this->app = NULL;
    this->logEvents = logEvents;
    this->numMembers = 0;
    this->memberInfo = NULL;
    this->commandTime = -1;
    this->params = params;
    this->command = (char*)malloc(200*sizeof(char));
    memset(&state, 0, sizeof(state));
    init();

    if (params && params->existsSetting("onionroutingMisbehavior")) {
        char valuebuf[200];
        params->getAsString("onionroutingMisbehavior", valuebuf, sizeof(valuebuf));
        char *sti = strtok(valuebuf, ",");
        char *scmd = sti ? strtok(NULL, "\r\n") : NULL;
        if (!scmd)
            panic("Syntax: onionroutingMisbehavior=timeMicros,command");

        commandTime = atoll(sti);
        strcpy(command, scmd);
    }
    //this->nodes = (char**)malloc(10*sizeof(char*));
    //this->nbFwd = 0;
    //this->nbRcvd = 0;
    this->nbRelais = nbRelais;

    this->nbEVT_BOOTSTRAP = 0;
    this->nbEVT_TRANSMIT = 0;
    this->nbEVT_SEND_O = 0;
    this->nbEVT_RECV_O = 0;
    this->nbEVT_STATUS = 0;


    /*Read parameters from the configuration file*/
    if (params)
        readMemberInfo(params);
}

OnionRouting::~OnionRouting() {
    init();

    if (memberInfo) {
        for (int i = 0; i < numMembers; i++)
            delete memberInfo[i].subject;
        free(memberInfo);
    }

    if (app)
        delete app;
}

void OnionRouting::init() {
    for (int i = 0; i < state.numPeers; i++)
        delete state.peer[i].handle;

    memset(&state, 0, sizeof(state));
    state.numPeers = 0;
    state.numExpectedMessages = 0;
    state.lastExpectedMessage = 5;
}

void OnionRouting::readMemberInfo(Parameters *params) {
    /* The mapping between nodes and their witnesses is static;
       each node reads it from the configuration file and
       stores it in memory. */

    numMembers = params->numSections();
    memberInfo = (struct memberInfo*) malloc(
            sizeof(struct memberInfo) * numMembers);
    nodes = (char**) malloc(sizeof(char*) * numMembers);
    if (!memberInfo)
        panic("OR: Out of memory when allocating memberInfo");

    for (int i = 0; i < numMembers; i++) {
        const char *name = params->getSectionByIndex(i);
        params->selectSection(name);
        memberInfo[i].subject = transport->readNodeHandleFromString(name);
        memberInfo[i].numWitnesses = params->getAsInt("witnesses");
    }

}

/*peeledOnion *OnionRouting::peelOnion(char *onion) {
  peeledOnion *po = (peeledOnion*) malloc(10 * sizeof(peeledOnion));
  char *tmp1 = strtok(onion, "|");
  char *tmp2 = strtok(NULL, "|");
  char *tmp3 = strtok(NULL, "");
  printf("**********\n");
  printf("tmp1 = %s\n",tmp1);
  printf("tmp2 = %s\n",tmp2);
  printf("tmp3 = %s\n",tmp3);
  if (tmp2) {
  char *peerID = strtok(tmp2, "/");
  char *peerIP = peerID ? strtok(NULL, "/") : NULL;
  printf("peerID = %s\n",peerID);
  printf("peerIP = %s\n",peerIP);
  printf("**********\n");
  po->node = new SimpleNodeHandle(
  SimpleIdentifier::readFromString(peerID, 160),
  inet_addr(peerIP));
  if (tmp3) {
  char *tmp = strcat(tmp1, "|");
  po->msg = strcat(tmp, tmp3);
  } else
  po->msg = tmp1;

  } else {
  if (tmp3) {
  char *peerID = strtok(tmp3, "/");
  char *peerIP = peerID ? strtok(NULL, "/") : NULL;
  po->node = new SimpleNodeHandle(
  SimpleIdentifier::readFromString(peerID, 160),
  inet_addr(peerIP));
  } else {
  po->node = NULL;
  }
  po->msg = tmp1;

  }

  return po;
  }*/

void OnionRouting::setCallback(OnionRoutingApp *app) {
    this->app = app;
}

void OnionRouting::bootstrap(NodeHandle **initialPeers, int numInitialPeers) {
    assert(state.numPeers == 0);
    char buf1[256];

    /* This is called once after startup to initialize the node.
       We record an entry in the secure history, so we can repeat
       the call during replay. */

    unsigned char event[1 + MAX_HANDLE_SIZE * numInitialPeers];
    unsigned int eventLen = 0;

    Basics::writeByte(event, &eventLen, numInitialPeers);
    for (int i = 0; i < numInitialPeers; i++) {
            
            //Jeremie//plog(2, "Adding peer %s [%d]", initialPeers[i]->render(buf1),state.numPeers);
        initialPeers[i]->write(event, &eventLen, sizeof(event));
        assert(state.numPeers < MAX_PEERS);
        state.peer[state.numPeers].handle = initialPeers[i]->clone();
        state.peer[state.numPeers].active = true;
        state.numPeers++;
    }

    if (logEvents) {
        nbEVT_BOOTSTRAP++;
        
        //Jeremie//plog(3,"EVT_BOOTSTRAP : EntrySize = %lld nbEVT_BOOTSTRAP = %d", eventLen, nbEVT_BOOTSTRAP);
        transport->logEvent(EVT_BOOTSTRAP, event, eventLen);
    }
}

void OnionRouting::send(unsigned int seq, char *msg, int len, NodeHandle* dest) {
    if (logEvents) {
        unsigned char entry[sizeof(seq) + 20];
        *(unsigned int*)&entry[0] = seq;
        transport->hash(&entry[sizeof(seq)], (unsigned char*) msg, len);
        nbEVT_TRANSMIT++;
        
        //Jeremie//plog(3,"EVT_TRANSMIT : EntrySize = %lld nbEVT_TRANSMIT = %d", sizeof(entry), nbEVT_TRANSMIT);
        transport->logEvent(EVT_TRANSMIT, entry, sizeof(entry));
        nbEVT_SEND_O++;
        
        //Jeremie//plog(3,"EVT_SEND_O : EntrySize = %lld nbEVT_SEND_O = %d", len, nbEVT_SEND_O);
        transport->logEvent(EVT_SEND_O, msg, len);
    }
    
    //Jeremie//plog(3, "Transmitting message #%d (%d bytes)", seq, len);

    /* Next, we peel onion and create a message to send */
    unsigned char *message = (unsigned char*) malloc(6 + len);
    message[0] = MSG_DATA;
    message[1] = 50; /* TTL */
    *(unsigned int*) &message[2] = seq;
    memcpy(message + 6, msg, len);

    char buf[200];
    //printf("Sending onion to %s\n", dest->render(buf));
    transport->send(dest, false, message, len + 6, 6);
    //transport->send(dest, false, message, len + 6, -1);
    free(message);
}

void OnionRouting::receive(NodeHandle *source, bool datagram,
        unsigned char *msg, int msglen) {
    char buf1[256];

    /* Respond to incoming messages. The first byte is the message type. */
    switch (msg[0]) {
        case MSG_DATA: {
                           int seq = *(unsigned int*) &msg[2];
                           
                           //Jeremie//plog(3, "Received DATA #%d from %s (%d bytes, TTL %d)", seq, source->render(buf1), msglen - 6, msg[1]);

                           /* We got a data block. Give it to the application. */
                           if (app) {
                               app->receive(seq, (char*) &msg[6], msglen - 6);
                               if(logEvents)
                                   nbEVT_RECV_O++;
                               //Jeremie//plog(3,"EVT_RECV_O : EntrySize = %lld nbEVT_RECV_O = %d", msglen-6, nbEVT_RECV_O);
                               transport->logEvent(EVT_RECV_O,(char*) &msg[6],msglen-6);
                               //nbRcvd++;
                               ////Jeremie//plog(4, "nbRcvd = %d", nbRcvd);
                           }

                           /* If we're a bad node, eat the block */
                           /*if ((commandTime>=0) && (transport->getTime()>=commandTime) && !strcasecmp(command, "silent")) {
                             //Jeremie//plog(4, "Mischief: Silent");
                           //printf("totooo\n"); //par moi
                           msg[1] = 1;
                           }

                           if ((--msg[1]) > 0) {
                           //pour l'injection de fautes
                           } else {
                           warning("Message TTL has expired; dropping message");
                           }*/
                           break;
                       }
        case MSG_CHECK: {
                            
                            //Jeremie//plog(3, "Received CHECK from %s", source->render(buf1));
                            break;
                        }
        default: {
                     warning("Unknown message type %d; ignoring", msg[0]);
                     break;
                 }
    }

}

/////////////////////// PeerReview //////////////////////////
void OnionRouting::statusChange(Identifier *id, int status) {
    char buf1[256];
    
    //Jeremie//plog(2, "Update: %s becomes %d", id->render(buf1), status);

    /* This is called when the state of another node changes (that is,
       it becomes trusted, suspected or exposed). Again, we record
       an entry in the secure history so we can reproduce the call
       during replay. We also reconfigure the topology to exclude
       suspected or exposed nodes. */

    if (logEvents) {
        unsigned char event[1+MAX_ID_SIZE];
        unsigned int eventLen = 0;

        Basics::writeByteSafe(status, event, &eventLen, sizeof(event));
        id->write(event, &eventLen, sizeof(event));
        nbEVT_STATUS++;
        
        //Jeremie//plog(3,"EVT_STATUS : EntrySize = %lld nbEVT_STATUS = %d", eventLen, nbEVT_STATUS);
        transport->logEvent(EVT_STATUS, event, eventLen);
    }

    for (int i=0; i<state.numPeers; i++) {
        if (state.peer[i].handle->getIdentifier()->equals(id)) {
            state.peer[i].active = (status == STATUS_TRUSTED);
            if (status == STATUS_SUSPECTED) {
                unsigned char msg[1] = { MSG_CHECK };
                transport->send(state.peer[i].handle, false, msg, sizeof(msg));
            }
        }
    }
}

void OnionRouting::timerExpired(int timerID) {

}

int OnionRouting::storeCheckpoint(unsigned char *buffer, unsigned int maxlen) {
    return 0;
}

bool OnionRouting::loadCheckpoint(unsigned char *buffer, unsigned int len) {
    return false;
}

void OnionRouting::getWitnesses(Identifier *subject, WitnessListener *callback){
    assert(memberInfo);
    char buf1[256];

    /* This determines a list of witnesses for a given node. Since the mapping
       in this application is static, we just have to look up this information
       in memory (it was read from the config file earlier) */

    int idx = -1;
    for (int i=0; i<numMembers; i++) {
        if (subject->equals(memberInfo[i].subject->getIdentifier()))
            idx = i;
    }

    if (idx<0)
        panic("Cannot answer getWitnesses(%s) request", subject->render(buf1));

    NodeHandle **witnesses = (NodeHandle**) malloc(memberInfo[idx].numWitnesses * sizeof(NodeHandle*));
    for (int i=0; i<memberInfo[idx].numWitnesses; i++)
        witnesses[i] = memberInfo[(idx+numMembers-(1+i))%numMembers].subject;

    callback->notifyWitnessSet(subject, memberInfo[idx].numWitnesses, witnesses);

    free(witnesses);

}

int OnionRouting::getMyWitnessedNodes(NodeHandle **nodes, int maxResults){
    assert(memberInfo);

    /* Find out which nodes are witnessed by the local node. Again, the
       mapping is static, so we just have to look it up in memory. */

    NodeHandle *myself = transport->getLocalHandle();
    int myIndex = -1;
    for (int i=0; (myIndex<0) && (i<numMembers); i++)
        if (memberInfo[i].subject->equals(myself))
            myIndex = i;

    assert(myIndex >= 0);

    int numResults = 0;
    for (int i=1; (i<numMembers) && (numResults<maxResults); i++) {
        if (memberInfo[(myIndex+i)%numMembers].numWitnesses >= i)
            nodes[numResults++] = memberInfo[(myIndex+i)%numMembers].subject;
    }

    return numResults;
}

PeerReviewCallback *OnionRouting::getReplayInstance(ReplayWrapper *replayWrapper){
    /* Get another instance of the state machine for replay... */

    //OnionRouting *instance = new OnionRouting(replayWrapper, NULL, false);
    //OnionRoutingReplayHelper *helper = new OnionRoutingReplayHelper(instance, replayWrapper);

    /* ... and register all the event types we've been writing to the
       secure history, so we can reproduce the corresponding events */
    /*replayWrapper->registerCallback(helper);
      replayWrapper->registerEvent(helper, EVT_BOOTSTRAP);
      replayWrapper->registerEvent(helper, EVT_TRANSMIT);
      replayWrapper->registerEvent(helper, EVT_STATUS);
      return instance;*/
    return NULL;
}

void OnionRouting::OnionRoutingReplayHelper::replayEvent(unsigned char type, unsigned char *entry, int entrySize)
{
    /*switch (type) {
      case EVT_BOOTSTRAP:
      {
      Replay a bootstrap event

      unsigned int pos = 0;
      int numHandles = Basics::readByte(entry, &pos);
      NodeHandle **handles = (NodeHandle**) malloc(numHandles * sizeof(NodeHandle*));
      for (int i=0; i<numHandles; i++)
      handles[i] = transport->readNodeHandle(entry, &pos, entrySize);
      onionrouting->bootstrap(handles, numHandles);
      for (int i=0; i<numHandles; i++)
      delete handles[i];
      free(handles);
      break;

      }
      case EVT_STATUS:
      {
      Replay a status change event

      unsigned int pos = 0;
      int status = Basics::readByte(entry, &pos);
      Identifier *id = transport->readIdentifier(entry, &pos, entrySize);
      onionrouting->statusChange(id, status);
      delete id;
      break;

      }
      case EVT_TRANSMIT:
      Replay a transmission event

      transport->logText(SUBSYSTEM, 2, "Transmitting %d bytes, seq #%d", entrySize-4, *(unsigned int*)&entry[0]);
    //onionrouting->send(*(unsigned int*)&entry[0], (char*)&entry[4], entrySize-4);
    NodeHandle *dest;  //à donner une valeur
    onionrouting->send(*(unsigned int*)&entry[0], (char*)&entry[4], entrySize-4,dest);


    break;
    default:
    panic("Replay helper cannot replay events of type #%d", type);
    }*/

}
