package kimascend.com.music.utils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ByteUtil {

    private ByteUtil() {

    }


    public static byte[] int2byte(int data) {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(data).array();
    }

    public static byte[] short2byte(short data) {
        return ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN).putShort(data).array();
    }

    public static short byteArray2short(byte[] data) {
        return ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN).getShort();
    }

    public static int byteArray2int(byte[] data) {
        return ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN).getInt();
    }
}
