package kimascend.com.music.basic_audio.api;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;

import java.io.IOException;
import java.nio.ByteBuffer;

import static kimascend.com.music.utils.LogUtil.logd;

public class AudioDecoder {
    private static final String MINE_TYPE = "audio/mp4a-latm";
    private static final int MAX_INPUT_SIZE = 1024 * 16;
    private static final int DEFAULT_CHANNEL_NUM = 1;
    private static final int DEFAULT_SAMPLE_RATE = 44100;
    private boolean isOpened = false;
    private MediaCodec mediaCodec;
    private boolean isFirstFrame = true;
    private MediaCodec.BufferInfo bufferInfo;
    private AudioDecoderListener audioDecoderListener;

    interface AudioDecoderListener{
        void onFrameDecode(byte[] buffer, long presentationTimeUs);
    }

    public void setAudioDecoderListener(AudioDecoderListener audioDecoderListener) {
        this.audioDecoderListener = audioDecoderListener;
    }


    public boolean open() {
        return open(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_NUM, MAX_INPUT_SIZE);
    }

    public boolean open(int sampleRate, int channels, int maxBufferSize) {
        if (isOpened) {
            return false;
        }

        isOpened = true;

        try {
            mediaCodec = MediaCodec.createDecoderByType(MINE_TYPE);
            MediaFormat mediaFormat = new MediaFormat();
            mediaFormat.setString(MediaFormat.KEY_MIME, MINE_TYPE);
            mediaFormat.setInteger(MediaFormat.KEY_CHANNEL_COUNT, channels);
            mediaFormat.setInteger(MediaFormat.KEY_SAMPLE_RATE, sampleRate);
            mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, maxBufferSize);
            mediaCodec.configure(mediaFormat, null, null, 0);
            bufferInfo = new MediaCodec.BufferInfo();
            mediaCodec.start();
        } catch (IOException e) {
            e.printStackTrace();
            isOpened = false;
            return false;
        }
        logd("open audio decode success");

        return true;
    }

    public void close() {
        if (!isOpened) {
            return;
        }
        isOpened = false;
        mediaCodec.stop();
        mediaCodec.release();
        mediaCodec = null;
        logd("close audio decoder");
    }

    public synchronized boolean decode(byte[] input, long presentationTimeUs) {
        if (!isOpened) {
            return false;
        }

        ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();

        int inputBufferIndex = mediaCodec.dequeueInputBuffer(presentationTimeUs);
        if (inputBufferIndex >= 0) {
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
            inputBuffer.clear();
            inputBuffer.put(input);
            if (isFirstFrame) {
                mediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, presentationTimeUs, MediaCodec.BUFFER_FLAG_CODEC_CONFIG);
                isFirstFrame = false;
            } else {
                mediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, presentationTimeUs, 0);
            }
        }
        return true;
    }

    public synchronized boolean retrieve(long presentationTimeUs) {
        if (!isOpened) {
            return false;
        }

        ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
        int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, presentationTimeUs);
        if (outputBufferIndex >= 0) {
            ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
            byte[] data = new byte[bufferInfo.size];
            outputBuffer.get(data);
            if (audioDecoderListener != null) {
                audioDecoderListener.onFrameDecode(data, presentationTimeUs);
            }
            mediaCodec.releaseOutputBuffer(outputBufferIndex,false);
        }
        return true;
    }


}
