package kimascend.com.music.basic_audio.wav;

import android.util.Log;

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;

import kimascend.com.music.utils.ByteUtil;

public class WaveFileReader {
    private static final String TAG = WaveFileReader.class.getSimpleName();

    private DataInputStream mDataInputStream;
    private WavFileHeader mWavFileHeader;

    public boolean openFile(String filepath) throws IOException {
        if (mDataInputStream != null) {
            closeFile();
        }
        mDataInputStream = new DataInputStream(new FileInputStream(filepath));
        return readHeader();
    }

    public void closeFile() throws IOException {
        if (mDataInputStream != null) {
            mDataInputStream.close();
            mDataInputStream = null;
        }
    }

    public WavFileHeader getmWavFileHeader() {
        return mWavFileHeader;
    }

    public int readData(byte[] buffer, int offset, int count) {
        if (mDataInputStream == null || mWavFileHeader == null) {
            return -1;
        }

        try {
            int nbytes = mDataInputStream.read(buffer, offset, count);
            if (nbytes == -1) {
                return 0;
            }
            return nbytes;
        } catch (IOException e) {
            e.printStackTrace();
        }

        return -1;
    }

    private boolean readHeader() {
        if (mDataInputStream == null) {
            return false;
        }

        WavFileHeader header = new WavFileHeader();

        byte[] intValue = new byte[4];
        byte[] shortValue = new byte[2];

        try {
            header.chunkId = "" + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte();
            Log.d(TAG, "Read file chunkID:" + header.chunkId);

            mDataInputStream.read(intValue);
            header.chunkSize = ByteUtil.byteArray2int(intValue);
            Log.d(TAG, "Read file chunkSize:" + header.chunkSize);

            header.format = "" + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte();
            Log.d(TAG, "Read file format:" + header.format);

            header.subChunk1Id = "" + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte();
            Log.d(TAG, "Read fmt chunkID:" + header.subChunk1Id);

            mDataInputStream.read(intValue);
            header.subChunk1Size = ByteUtil.byteArray2int(intValue);
            Log.d(TAG, "Read fmt chunkSize:" + header.subChunk1Size);

            mDataInputStream.read(shortValue);
            header.audioFormat = ByteUtil.byteArray2short(shortValue);
            Log.d(TAG, "Read audioFormat:" + header.audioFormat);

            mDataInputStream.read(shortValue);
            header.numChannels = ByteUtil.byteArray2short(shortValue);
            Log.d(TAG, "Read channel number:" + header.numChannels);

            mDataInputStream.read(intValue);
            header.sampleRate = ByteUtil.byteArray2int(intValue);
            Log.d(TAG, "Read samplerate:" + header.sampleRate);

            mDataInputStream.read(intValue);
            header.byteRate = ByteUtil.byteArray2int(intValue);
            Log.d(TAG, "Read byterate:" + header.byteRate);

            mDataInputStream.read(shortValue);
            header.blockAlign = ByteUtil.byteArray2short(shortValue);
            Log.d(TAG, "Read blockalign:" + header.blockAlign);

            mDataInputStream.read(shortValue);
            header.bitPerSample = ByteUtil.byteArray2short(shortValue);
            Log.d(TAG, "Read bitspersample:" + header.bitPerSample);

            header.subChunk2Id = "" + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte() + (char) mDataInputStream.readByte();
            Log.d(TAG, "Read data chunkID:" + header.subChunk2Id);

            mDataInputStream.read(intValue);
            header.subChunk2Size = ByteUtil.byteArray2int(intValue);
            Log.d(TAG, "Read data chunkSize:" + header.subChunk2Size);

            Log.d(TAG, "Read wav file success !");
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        mWavFileHeader = header;

        return true;
    }
}
