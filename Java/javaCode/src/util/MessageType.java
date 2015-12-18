package util;

public class MessageType {
    public static final byte EMPTY_UPDATES = 0; // used only to simulate bandwidth consumption (size of updates)
    public static final byte EMPTY_LOG = 1;
    
    public static final byte KEY_REQUEST = 2;
    public static final byte KEY_RESPONSE = 3;
    
    public static final byte SERVE = 8;
    public static final byte ACK = 9;
    
    public static final byte FORWARD_ACK = 10;
    public static final byte FORWARD_ATTESTATION = 11;
    public static final byte FORWARD_ACK_2 = 12;
    public static final byte BROADCAST_WIT = 13;
}

// Remark: log_svrEnd, log_svrBegin, peer_end : do not exist in the real protocol