package kimascend.com.music.basic_audio.api;

import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

import kimascend.com.music.utils.ByteUtil;

public class WavFileWriter {

    private String mFilepath;
    private int mDataSize = 0;
    private DataOutputStream mDataOutputStream;

    public boolean openFile(String filepath, int sampleRateInHz, int channels, int bitsPerSample) throws IOException {
        if (mDataOutputStream != null) {
            closeFile();
        }
        mFilepath = filepath;
        mDataSize = 0;
        mDataOutputStream = new DataOutputStream(new FileOutputStream(filepath));
        return writeHeader(sampleRateInHz, bitsPerSample, channels);
    }

    public boolean closeFile() throws IOException {
        boolean ret = true;
        if (mDataOutputStream != null) {
            ret = writeDataSize();
            mDataOutputStream.close();
            mDataOutputStream = null;
        }
        return ret;
    }

    public boolean writeData(byte[] buffer, int offset, int count) {
        if (mDataOutputStream == null) {
            return false;
        }

        try {
            mDataOutputStream.write(buffer, offset, count);
            mDataSize += count;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    private boolean writeHeader(int sampleRateInHz, int channels, int bitsPerSample) {
        if (mDataOutputStream == null) {
            return false;
        }

        WavFileHeader header = new WavFileHeader(sampleRateInHz, channels, bitsPerSample);

        try {
            mDataOutputStream.writeBytes(header.chunkId);
            mDataOutputStream.write(ByteUtil.int2byte(header.chunkSize), 0, 4);
            mDataOutputStream.writeBytes(header.format);
            mDataOutputStream.writeBytes(header.subChunk1Id);
            mDataOutputStream.write(ByteUtil.int2byte(header.subChunk1Size), 0, 4);
            mDataOutputStream.write(ByteUtil.short2byte(header.audioFormat), 0, 2);
            mDataOutputStream.write(ByteUtil.short2byte(header.numChannels), 0, 2);
            mDataOutputStream.write(ByteUtil.int2byte(header.sampleRate), 0, 4);
            mDataOutputStream.write(ByteUtil.int2byte(header.byteRate), 0, 4);
            mDataOutputStream.write(ByteUtil.short2byte(header.blockAlign), 0, 2);
            mDataOutputStream.write(ByteUtil.short2byte(header.bitPerSample), 0, 2);
            mDataOutputStream.writeBytes(header.subChunk2Id);
            mDataOutputStream.write(ByteUtil.int2byte(header.subChunk2Size), 0, 4);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    private boolean writeDataSize() {
        if (mDataOutputStream == null) {
            return false;
        }

        try {
            RandomAccessFile wavFile = new RandomAccessFile(mFilepath, "rw");
            wavFile.seek(WavFileHeader.WAV_CHUNKSIZE_OFFSET);
            wavFile.write(ByteUtil.int2byte((mDataSize + WavFileHeader.WAV_CHUNKSIZE_EXCLUDE_DATA)), 0, 4);
            wavFile.seek(WavFileHeader.WAV_SUB_CHUNKSIZE2_OFFSET);
            wavFile.write(ByteUtil.int2byte((mDataSize)), 0, 4);
            wavFile.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }


}