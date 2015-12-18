package colluders;

import java.io.ByteArrayInputStream;
import java.io.ObjectInputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import util.MessageType;
import util.Message;
import util.NetUtil;
import util.Node;
import colluders.SenderTh;

public class Source {

    // Attributes
    private short selfId; // The server id is equal to 0
    private int roundId;

    // Membership
    private ArrayList<Node> nodesList;

    // Protocol constants
    private final int FANOUT, UPDATES_NBR_PER_ROUND;
    public static final int UPDATES_NBR_PER_SERVE = 1;

    public static final int NB_SENDER = 5;
    public int senderId = 0;

    public SenderTh senders[];
    public Thread sendersTh[];
    public ArrayList<LinkedBlockingQueue<Message>> outMessages;

    public Thread receiverTh;
    public LinkedBlockingQueue<Message> inMessages;

    private int nbServeRounds; // Keep track of the number of rounds in which
    // the server sent updates
    private final Random generator;

    // Used to tell a node that it must terminate
    public boolean mustStop = false;

    private static final boolean DEBUG = false;

    // Constructor
    Source(int listeningPort, ArrayList<Node> nodesList, int FANOUT,
            int UPDATES_NBR_PER_ROUND, int updatesLen) {
        this.selfId = 0;
        this.roundId = 1;

        this.nodesList = nodesList;

        this.FANOUT = FANOUT;

        // Create receiver : receive messages, extract useful data and put it
        // into inMessages
        this.inMessages = new LinkedBlockingQueue<Message>();
        this.receiverTh = new Thread(new Receiver(inMessages, listeningPort));
        this.receiverTh.start();

        // Create sender : from useful data in outMessages, make a packet and
        // send it

        senders = new SenderTh[NB_SENDER];
        sendersTh = new Thread[NB_SENDER];
        outMessages = new ArrayList<LinkedBlockingQueue<Message>>();
        for (int i = 0; i < NB_SENDER; ++i) {
            outMessages.add(new LinkedBlockingQueue<Message>());
            senders[i] = new SenderTh(0, outMessages.get(i), updatesLen,
                    nodesList);
            sendersTh[i] = new Thread(senders[i]);
            sendersTh[i].start();
        }

        // Statistics
        this.UPDATES_NBR_PER_ROUND = UPDATES_NBR_PER_ROUND;
        nbServeRounds = 0;

        this.generator = new Random(System.currentTimeMillis());
    }

    public void stop() {
        this.mustStop = true;
        for (int i = 0; i < NB_SENDER; ++i)
            senders[i].mustStop = true;
    }

    public void newRound() {
        System.out.println("Server : round " + roundId);

        ArrayList<Short> selectedClients = new ArrayList<Short>();
        ArrayList<Short> availableClients = new ArrayList<Short>();

        for (int i = 0; i < nodesList.size(); i++) {
            availableClients.add(nodesList.get(i).nodeId);
        }

        int availableIndex;
        short toClientId;
        ArrayList<Integer> updates = new ArrayList<Integer>();
        Message sendMessage;

        if (availableClients.size() > 0) {
            int updateValue = UPDATES_NBR_PER_ROUND * (roundId - 1) + 1;
            for (int firstUpdateId = 0; firstUpdateId < UPDATES_NBR_PER_ROUND; firstUpdateId += UPDATES_NBR_PER_SERVE) {

                int firstUpdateValue = updateValue + firstUpdateId;
                for (int i = 0; i < UPDATES_NBR_PER_SERVE; ++i) {
                    updates.add(i, firstUpdateValue);
                    ++firstUpdateValue;
                }

                for (int clientIndex = 0; clientIndex < Math.min(FANOUT,
                        availableClients.size()); ++clientIndex) {
                    availableIndex = generator.nextInt(availableClients.size());
                    toClientId = availableClients.get(availableIndex);

                    if (DEBUG)
                        System.out.println("Sending updates to " + toClientId);

                    selectedClients.add(toClientId);
                    availableClients.remove(availableIndex);

                    sendMessage = new Message(roundId, MessageType.SERVE,
                            selfId, toClientId);

                    sendMessage.setUpdatesId(updates);

                    outMessages.get(senderId).add(sendMessage);
                    ++senderId;
                    senderId %= NB_SENDER;
                }
                availableClients.addAll(selectedClients);
                selectedClients.clear();
                updates.clear();
            }
        }

        ++nbServeRounds;
        ++this.roundId;
    }

    public int getNbServeRounds() {
        return nbServeRounds;
    }

    public void treatInMessage() {
        try {
            Message message = inMessages.poll(10, TimeUnit.MILLISECONDS);
            if (message != null) // Nodes can treat in messages if it joined the
                // session
            {
            }
        } catch (Exception e) {
        }
    }

    /***************************************/
    /** RECEIVER THREAD **/
    /***************************************/

    // Receiver shares inMessages with main program
    // Wait for incoming messages, extract data and place it in inMessages
    private class Receiver extends Thread implements Runnable {

        // Attributes
        public LinkedBlockingQueue<Message> inMessages;
        private int listeningPort;

        // Constructor
        public Receiver(LinkedBlockingQueue<Message> inMessages,
                int listeningPort) {
            this.inMessages = inMessages;
            this.listeningPort = listeningPort;
        }

        // Run : wait for incoming messages and place them in inMessages
        public void run() {
            try {
                ServerSocketChannel ssc = ServerSocketChannel.open();
                ssc.configureBlocking(false);
                ServerSocket server = ssc.socket();

                System.out.println("Node " + selfId + " listens on port "
                        + listeningPort);

                InetSocketAddress localAddr = new InetSocketAddress(
                        listeningPort);
                server.bind(localAddr);

                Selector mySelector = Selector.open();
                ssc.register(mySelector, SelectionKey.OP_ACCEPT);

                while (!mustStop) {
                    if (mySelector.select(1000) == 0) {
                        continue;
                    }

                    // Get the keys corresponding to the activity
                    // that have been detected and process them
                    // one by one.
                    Set<SelectionKey> keys = mySelector.selectedKeys();
                    Iterator<SelectionKey> it = keys.iterator();

                    while (it.hasNext()) {
                        SelectionKey key = (SelectionKey) it.next();
                        if (key.isAcceptable()) {
                            // Accept the incoming connection.
                            Socket peer = server.accept();
                            peer.setTcpNoDelay(true);

                            // Make sure to make it nonblocking, so you can
                            // use a Selector on it.
                            SocketChannel sc = peer.getChannel();
                            sc.configureBlocking(false);

                            sc.register(mySelector, SelectionKey.OP_READ);
                        } else if (key.isReadable()) {
                            // Read a message
                            SocketChannel sc = (SocketChannel) key.channel();
                            byte[] receiveData = NetUtil.recv(sc);

                            if (receiveData == null) {
                                System.out
                                .println("RECV ERROR: receiveData is null!!!");
                                System.exit(-1);
                            } else {
                                ByteArrayInputStream baos = new ByteArrayInputStream(
                                        receiveData);
                                ObjectInputStream oos = new ObjectInputStream(
                                        baos);
                                Message receiveMessage = (Message) oos
                                        .readObject();

                                inMessages.add(receiveMessage);
                            }
                        }

                        it.remove();
                    }
                    keys.clear();
                }

            } catch (Exception e) {
                e.printStackTrace();
                System.exit(-1);
            }
        }
    }
}
