package kimascend.com.music.basic_audio.api;

public class WavFileHeader {
    public static final int WAV_FILE_HEADER_SIZE = 44;
    public static final int WAV_CHUNKSIZE_EXCLUDE_DATA = 36;

    public static final int WAV_CHUNKSIZE_OFFSET = 4;
    public static final int WAV_SUB_CHUNKSIZE1_OFFSET = 16;
    public static final int WAV_SUB_CHUNKSIZE2_OFFSET = 40;

    String chunkId = "RIFF";
    int chunkSize=0;
    String format = "WAVE";

    String subChunk1Id = "fmt";
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
}
