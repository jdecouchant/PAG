package util;

import java.io.Externalizable;
import java.io.IOException;
import java.io.ObjectInput;
import java.io.ObjectOutput;
import java.util.ArrayList;

public class Message implements Externalizable {

    // Header
    public int roundId;
    public byte messageType;
    public short senderId;
    public short receiverId;

    // Additional attributes
    public int[] updatesId;

    public int additionalSize; // Used for authenticators, and log entries

    public Message(int roundId, byte messageType, short senderId,
            short receiverId) {
        this.roundId = roundId;
        this.messageType = messageType;
        this.senderId = senderId;
        this.receiverId = receiverId;

        this.additionalSize = 0;
    }

    public Message() {}

    public void setUpdatesId(ArrayList<Integer> updatesId) {
        if (updatesId != null) {
            this.updatesId = new int[updatesId.size()];
            for (int i = 0; i < updatesId.size(); ++i)
                this.updatesId[i] = updatesId.get(i);
        }
    }

    public void setAdditionalSize(int additionalSize) {
        this.additionalSize = additionalSize;
    }

    /**
     * Getters
     */

    public int getUpdatesIdSize() {
        if (updatesId == null)
            return 0;
        return updatesId.length;
    }

    public int[] getUpdatesId() {
        return updatesId;
    }

    public void writeExternal(ObjectOutput out) throws IOException {
        out.writeByte(messageType);
        out.writeInt(roundId);
        out.writeShort(senderId);
        out.writeShort(receiverId);

        short updatesLen = (updatesId == null) ? 0 : (short) updatesId.length;
        out.writeShort(updatesLen);
        for (int i = 0; i < updatesLen; ++i)
            out.writeInt(updatesId[i]);

        if (updatesLen < 0) {
            System.out.println("OVERFLOW: (" + updatesLen + ") -> ("
                    + ((updatesId == null) ? 0 : updatesId.length) + ")");
        }
    }

    public void readExternal(ObjectInput in) throws IOException,
    ClassNotFoundException {
        messageType = in.readByte();
        roundId = in.readInt();
        senderId = in.readShort();
        receiverId = in.readShort();

        short updatesLen = in.readShort();
        updatesId = new int[updatesLen];
        for (int i = 0; i < updatesLen; ++i)
            updatesId[i] = in.readInt();
    }

    public String toString() {
        String s = "<";
        s += messageType + ", " + roundId + ", " + senderId + ", " + receiverId;

        byte updatesLen = (updatesId == null) ? 0 : (byte) updatesId.length;
        s += updatesLen + "[";
        for (int i = 0; i < updatesLen; ++i)
            s += updatesId[i] + ", ";
        s += "]";

        s += ">";
        return s;
    }
}
