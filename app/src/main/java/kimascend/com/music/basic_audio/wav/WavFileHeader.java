package kimascend.com.music.basic_audio.wav;

public class WavFileHeader {

    public static final int WAV_FILE_HEADER_SIZE = 44;
    public static final int WAV_CHUNKSIZE_EXCLUDE_DATA = 36;

    public static final int WAV_CHUNKSIZE_OFFSET = 4;
    public static final int WAV_SUB_CHUNKSIZE1_OFFSET = 16;
    public static final int WAV_SUB_CHUNKSIZE2_OFFSET = 40;

    String chunkId = "RIFF";
    int chunkSize=0;
    String format = "WAVE";
//    注意空格 一共四个字节
    String subChunk1Id = "fmt ";
    int subChunk1Size=16;
    short audioFormat=1;
    short numChannels=1;
    int sampleRate=8000;
    int byteRate=0;
    short blockAlign=0;
    short bitPerSample=8;

    String subChunk2Id = "data";
    int subChunk2Size = 0;

    public WavFileHeader() {
    }

    public WavFileHeader(int numChannels, int sampleRate, int bitPerSample) {
        this.sampleRate = sampleRate;
        this.bitPerSample = (short) bitPerSample;
        this.numChannels = (short) numChannels;
        byteRate = sampleRate * numChannels * bitPerSample / 8;
        blockAlign = (short) (numChannels * bitPerSample / 8);
    }


    @Override
    public String toString() {
        return "WavFileHeader{" +
                "chunkId='" + chunkId + '\'' +
                ", chunkSize=" + chunkSize +
                ", format='" + format + '\'' +
                ", subChunk1Id='" + subChunk1Id + '\'' +
                ", subChunk1Size=" + subChunk1Size +
                ", audioFormat=" + audioFormat +
                ", numChannels=" + numChannels +
                ", sampleRate=" + sampleRate +
                ", byteRate=" + byteRate +
                ", blockAlign=" + blockAlign +
                ", bitPerSample=" + bitPerSample +
                ", subChunk2Id='" + subChunk2Id + '\'' +
                ", subChunk2Size=" + subChunk2Size +
                '}';
    }
}
