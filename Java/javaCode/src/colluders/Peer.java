package colluders;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Random;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import util.BufferMap;
import util.Message;
import util.MessageType;
import util.Node;

public class Peer {
	// Attributes of peer
	public short selfId;
	private int roundId;

	// Membership
	private ArrayList<Node> nodesList;

	// Owned updates and logs
	private BufferMap selfBM; // Contains the ids of the updates the node received

	// Protocol parameters
	private int FANOUT; // Number of partnerships a node has to maintain

	private Random generator;

	// Thread and communication queue
	public SenderTh sender;
	public ReceiverTh receiver;
	public Thread senderTh, receiverTh;
	public LinkedBlockingQueue<Message> inMessages, outMessages;

	public BufferedWriter outLog;

	private static final boolean DEBUG = false;
	private static final boolean OUTPUT = true;

	private static final int KEY_SIZE = 512;
	private static final int MODULO_SIZE = 1024;
	private static final int SIGNATURE_SIZE = 1024;

	// Constructor
	Peer(short selfId, int listeningPort, ArrayList<Node> nodesList, int FANOUT, int RTE, int NBR_UPDATES_PER_ROUND,
			int updatesLen) {

		this.selfId = selfId;
		this.FANOUT = FANOUT;
		this.nodesList = nodesList;

		this.roundId = 1;

		this.selfBM = new BufferMap(RTE, NBR_UPDATES_PER_ROUND, DEBUG);

		this.generator = new Random(System.currentTimeMillis());

		// Create receiver : receive messages, extract useful data and put it into inMessages
		this.inMessages = new LinkedBlockingQueue<Message>();
		this.receiver = new ReceiverTh(listeningPort, inMessages, selfId);
		this.receiverTh = new Thread(receiver);
		this.receiverTh.start();

		// Create sender : from useful data in outMessages, make a packet and send it
		this.outMessages = new LinkedBlockingQueue<Message>();
		this.sender = new SenderTh(selfId, outMessages, updatesLen, nodesList, this);
		this.senderTh = new Thread(sender);
		this.senderTh.start();

		if (OUTPUT) {
			try {
				outLog = new BufferedWriter(new FileWriter(selfId + "_logSize.txt"));
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public int getRoundId() {
		return this.roundId;
	}

	public ArrayList<Short> getWitnesses(int nodeId) {
		ArrayList<Short> witnesses = new ArrayList<Short>();
		for (short i = 1; i <= (short) FANOUT; i++) {
			short witnessId = (short) ((selfId + i) % nodesList.size());
			if (witnessId==0) witnessId = (short) ((selfId + FANOUT + 1) % nodesList.size());
			witnesses.add(witnessId);
		}
		return witnesses;
	}

	// Methods
	// Used to collect final statistics

	public void newRound() {
		System.out.println("\nPeer " + selfId + ": round " + roundId);

		// Find new partners, and initiate associations with them
		ArrayList<Short> availableNodesId = new ArrayList<Short>();
		Iterator<Node> it = nodesList.iterator();
		while (it.hasNext()) {
			short nodeId = it.next().nodeId;
			if (nodeId != selfId && nodeId > 0)
				availableNodesId.add(nodeId);
		}
		for (int clientIndex = 0; clientIndex < Math.min(FANOUT, availableNodesId.size()); ++clientIndex) {
			int toPartnerIndex = generator.nextInt(availableNodesId.size());
			short toPartnerId = availableNodesId.get(toPartnerIndex);

			availableNodesId.remove(toPartnerIndex);
			sendKEY_REQUEST(toPartnerId);
			//           sendPROPOSE(toPartnerId);
		}
	}

	// Need an extra procedure for the end of the round (treat in messages in the meanwhile)
	public int endOfRound() {
		int deletedUpdNbr = selfBM.actualize(roundId);
		System.out.println("Number of available updates that expired : " + deletedUpdNbr);

		++this.roundId; 

		return deletedUpdNbr ;
	}

	public void treatInMessage() {
		try {
			Message message = inMessages.poll(10, TimeUnit.MILLISECONDS);
			if (message != null) { // Nodes can treat in messages if it joined the session
				switch (message.messageType) {
				case MessageType.KEY_REQUEST:
					receiveKEY_REQUEST(message);
					break;
				case MessageType.KEY_RESPONSE:
					receiveKEY_RESPONSE(message);
					break;
				case MessageType.SERVE:
					receiveSERVE(message);
					break;
				case MessageType.ACK:
					receiveACK(message);
					break;
				case MessageType.FORWARD_ACK:
					receiveFORWARD_ACK(message);
					break;
				case MessageType.FORWARD_ATTESTATION:
					receiveFORWARD_ATTESTATION(message);
					break;
				case MessageType.FORWARD_ACK_2:
					receiveFORWARD_ACK_2(message);
					break;
				case MessageType.BROADCAST_WIT:
					receiveBROADCAST_WIT(message);
					break;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void sendKEY_REQUEST(short toPartnerId) { // Ask key to the node to which updates will be sent
		Message sendMessage = new Message(roundId, MessageType.KEY_REQUEST, selfId, toPartnerId);
		sendMessage.setAdditionalSize(SIGNATURE_SIZE);
		outMessages.add(sendMessage);
	}

	public void receiveKEY_REQUEST(Message message) {
		sendKEY_RESPONSE(message.senderId);
	}

	public void sendKEY_RESPONSE(short toPartnerId) { // Answer with a prime number, the product of the other prime numbers, and the encryption of the updates already owned
		Message sendMessage = new Message(roundId, MessageType.KEY_RESPONSE, selfId, toPartnerId);
		ArrayList<Integer> updates = new ArrayList<Integer>(selfBM.getLastRoundUpdates(roundId, 3));//selfBM.getUpdates()); // TODO: send the encryption of only a subpart of the updates
		sendMessage.setUpdatesId(updates);
		sendMessage.setAdditionalSize(KEY_SIZE + FANOUT * KEY_SIZE + updates.size() * MODULO_SIZE + SIGNATURE_SIZE);
		outMessages.add(sendMessage);
	}

	// Rcv response from contacted node. Compute the updates that have to be sent, and the indexes of the others in the list sent by the node. 
	// Reply with updates, indexes of non sent updates, and the key (size FANOUT*KEY_SIZE) that the other node must use to ackowledge udpates.
	public void receiveKEY_RESPONSE(Message message) { 
		// TODO: send only the updates received during the last round
		Set<Integer> partnerUpdates = new HashSet<Integer>(); // Use a set for partner's updates to answer contains request in O(1) time
		for (int i = 0; i < message.getUpdatesIdSize(); i++) {
			partnerUpdates.add(message.updatesId[i]);
		}

		Set<Integer> ownUpdates = selfBM.getLastRoundUpdates(roundId, 2);//selfBM.getUpdates();
		ArrayList<Integer> canSend = new ArrayList<Integer>(); // Updates that self has, but the other node does not: those that will be sent

		int nbUpdatesNotSent = 0; // Number of updates that will only be pointed out, but not sent
		Iterator<Integer> it = ownUpdates.iterator();
		while (it.hasNext()) {
			int update = it.next();
			if (!partnerUpdates.contains(update)) {
				canSend.add(update);
			} else {
				nbUpdatesNotSent++;
			}
		}
		// The cost of updates is taken in account in SenderTh
		sendSERVE(message.senderId, canSend, FANOUT*KEY_SIZE + nbUpdatesNotSent * 16 + SIGNATURE_SIZE); // Product of the keys of last round, updates not sent, encryption for the attestation
	}

	/**********************************************************/
	/*** SERVE                                              ***/
	/**********************************************************/

	public void sendSERVE(short toPartnerId, ArrayList<Integer> updList, int additionalSize) {
		Message sendMessage = new Message(roundId, MessageType.SERVE, selfId, toPartnerId);
		sendMessage.setUpdatesId(updList);
		sendMessage.setAdditionalSize(additionalSize);

		outMessages.add(sendMessage);
	}

	public void receiveSERVE(Message message) {
		if (message.senderId != 0) {
			System.out.println("Receive serve at round " + roundId + " from node " + message.senderId);
			System.out.println(Arrays.toString(message.updatesId));
		}

		for (int i = 0; i < message.getUpdatesIdSize(); ++i)
			selfBM.insertUpdate(roundId, message.updatesId[i], message.senderId);

		sendACK(message.senderId);

		ArrayList<Short> selfWitnesses = getWitnesses(selfId);
		short rIndex = (short) new Random().nextInt(selfWitnesses.size());
		sendFORWARD_ACK(selfWitnesses.get(rIndex), message.senderId); // Forward to a random witness that will broadcast to the others
		sendFORWARD_ATTESTATION(selfWitnesses.get(rIndex)); // Forward directly to all the witnesses
	}

	public void sendACK(short toPartnerId) { // Ackowlegment given to the sender
		Message sendMessage = new Message(roundId, MessageType.ACK, selfId, toPartnerId);
		sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE);

		outMessages.add(sendMessage);
	}

	public void receiveACK(Message message) {}

	public void sendFORWARD_ACK(short toPartnerId, short fromNodeId) { // Forward to a random witness that will broadcast to the others
		Message sendMessage = new Message(roundId, MessageType.FORWARD_ACK, selfId, toPartnerId);
		sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE); // All these messages have the same size
		ArrayList<Integer> prevNodeId = new ArrayList<Integer>();
		prevNodeId.add((int) fromNodeId); // Id of the node to which witnesses it will be necessary to forward messages
		sendMessage.setUpdatesId(prevNodeId);
		outMessages.add(sendMessage);
	}

	public void receiveFORWARD_ACK(Message message) {
//		ArrayList<Short> witnesses = getWitnesses(message.senderId); // Find the other witnesses of the node that sent the forward_ack message
		// Send it to the other witnesses
//		for (int i = 0; i < witnesses.size(); i++) {
//			if (witnesses.get(i) != selfId) {
//				Message sendMessage = new Message(roundId, MessageType.BROADCAST_WIT, message.senderId, witnesses.get(i));
//				sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE);
//				outMessages.add(sendMessage);
//			}
//		}
		
		// Send the ACK to the witnesses of the previous node
		ArrayList<Short> prevNodeWit = getWitnesses(message.updatesId[0]);
		for (int i = 0; i < prevNodeWit.size(); i++) {
			sendFORWARD_ACK_2(prevNodeWit.get(i));
		}
	}

	public void sendFORWARD_ATTESTATION(short toPartnerId) { // From a receiver from its witnesses
		Message sendMessage = new Message(roundId, MessageType.FORWARD_ATTESTATION, selfId, toPartnerId);
		sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE + (FANOUT-1)*KEY_SIZE + SIGNATURE_SIZE); // All these messages have the same size
		outMessages.add(sendMessage);
	}
	
	public void receiveFORWARD_ATTESTATION(Message message) { // Compute the encryption and forward to the other witnesses of the sender node
		ArrayList<Short> witnesses = getWitnesses(message.senderId); // Forward the attestation to the other witnesses of the sender node (which is witnessed)
		for (int i = 0; i < witnesses.size(); i++) {
			if (witnesses.get(i) != selfId) {
				sendBROADCAST_WIT(witnesses.get(i));
			}
		}
	}

	public void sendBROADCAST_WIT(short toPartnerId) { // From a witness to the other witnesses of a node (broadcast the encrypted value of a reception from a predecessor).
		Message sendMessage = new Message(roundId, MessageType.BROADCAST_WIT, selfId, toPartnerId);
		sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE); // All these messages have the same size
		outMessages.add(sendMessage);
	}
	
	public void receiveBROADCAST_WIT(Message message) {}  

	public void sendFORWARD_ACK_2(short toPartnerId) { // From a witness to the witnesses of another node
		Message sendMessage = new Message(roundId, MessageType.FORWARD_ACK_2, selfId, toPartnerId);
		sendMessage.setAdditionalSize(MODULO_SIZE + SIGNATURE_SIZE); // All these messages have the same size
		outMessages.add(sendMessage);
	}
	
	public void receiveFORWARD_ACK_2(Message message) {}
}


