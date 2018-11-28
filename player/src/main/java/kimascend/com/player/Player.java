package kimascend.com.player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import kimascend.com.listener.OnCompleteListener;
import kimascend.com.listener.OnErrorListener;
import kimascend.com.listener.OnLoadListener;
import kimascend.com.listener.OnPlayListener;
import kimascend.com.listener.OnPreparedListener;

public class Player {
    private static final String TAG = "Player";
    private String source;
    private OnPreparedListener preparedListener;
    private OnLoadListener loadListener;
    private OnPlayListener playListener;
    private OnErrorListener onErrorListener;
    private OnCompleteListener onCompleteListener;
    private int volume = 100;
    private boolean initMediaCodec = false;

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }


    public void setPreparedListener(OnPreparedListener preparedListener) {
        this.preparedListener = preparedListener;
    }

    public void setLoadListener(OnLoadListener loadListener) {
        this.loadListener = loadListener;
    }

    public void setPlayListener(OnPlayListener playListener) {
        this.playListener = playListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public Player() {
    }

    public void prepareAudio() {
        if (TextUtils.isEmpty(source)) {
            Log.e(TAG, "empty url");
            return;
        }

        new Thread() {
            @Override
            public void run() {
                prepare(source);
            }
        }.start();
    }

    public void startAudio() {
        if (TextUtils.isEmpty(source)) {
            Log.e(TAG, "empty url");
            return;
        }

        new Thread() {
            @Override
            public void run() {
                Player.this.start();
            }
        }.start();
    }

    public void stopAudio() {

        new Thread() {
            @Override
            public void run() {
                Player.this.stop();
            }
        }.start();
    }

    public void setVolume(int volume) {
        this.volume = volume;
        set_Volume(volume);
    }

    public int getVolume() {
        return volume;
    }

    public void startRecord(File file) {
            if (!initMediaCodec) {
                if (getSampleRate() > 0) {
                    initMediaCodec = true;
                    initMediaCodec(getSampleRate(),file);
                    startRecord(true);
                    Log.d(TAG, "startRecord: ");
                }
            }

    }

    public void pauseRecord() {
        Log.d(TAG, "pauseRcord: ");
        startRecord(false);
    }

    public void resumeRecord() {
        Log.d(TAG, "pauseRcord: ");
        startRecord(true);
    }

    public void stopRecord() {
        if (initMediaCodec) {
            startRecord(false);
            releaseMediaCodec();
        }
    }

    //----------C++调用
    public void onPrepared() {
        if (preparedListener != null) {
            preparedListener.onPrepared();
        }
    }

    public void onLoad(boolean isLoading) {
        if (loadListener != null) {
            loadListener.onLoad(isLoading);
        }
    }


    public void onPlaying(int current, int total) {
        Log.d(TAG, "onPlaying() called with: current = [" + current + "], total = [" + total + "]");
        if (playListener != null) {
            playListener.onTimeInfo(current, total);
        }
    }

    public void onError(int code, String msg) {
        if (onErrorListener != null) {
            stopAudio();
            onErrorListener.onError(code, msg);
        }
    }

    public void onComplete() {
        if (onCompleteListener != null) {
            stopAudio();
            onCompleteListener.onComplete();
        }
    }

    private native void prepare(String source);

    private native void start();

    public native void pause();

    public native void resume();

    private native void stop();

    private native void set_Volume(int volume);

    public native void seek(int seconds);

    public native void setMute(int mute);

    public native int getDuration();

    public native void setSpeed(float speed);

    public native void setPitch(float pitch);

    private native int getSampleRate() ;
    private native void  startRecord(boolean isRecording) ;


    private MediaCodec encode;
    private MediaFormat mediaFormat;
    private MediaCodec.BufferInfo bufferInfo;
    private FileOutputStream fileOutputStream;
    private int per_pcm_size = 0;
    private byte[] outByteBuffer;
    private int acc_sample_rate = 4;

    private void initMediaCodec(int sampleRate, File file) {
        try {
            acc_sample_rate = getADTSsamplerate(sampleRate);
            mediaFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, 2);
            mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);
            encode = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            if (encode == null) {
                Log.e(TAG, "initMediaCodec: fail");
                return;
            }
            bufferInfo = new MediaCodec.BufferInfo();
            encode.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            fileOutputStream = new FileOutputStream(file);
            encode.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //转码失败
    public void encodePcm2AAC(int size, byte[] buffer) {
        if (encode != null && buffer != null) {
            int inputBufferIndex = encode.dequeueInputBuffer(0);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = encode.getInputBuffers()[inputBufferIndex];
                inputBuffer.clear();
                inputBuffer.put(buffer);
                encode.queueInputBuffer(inputBufferIndex, 0, size, 0, 0);
            }
            int outputBufferIndex = encode.dequeueOutputBuffer(bufferInfo, 0);
            while (outputBufferIndex >= 0) {
                try {
                    per_pcm_size = bufferInfo.size + 7;
                    outByteBuffer = new byte[per_pcm_size];

                    ByteBuffer outputBuffer = encode.getOutputBuffers()[outputBufferIndex];
                    outputBuffer.position(bufferInfo.offset);
                    outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                    addADtsHeader(outByteBuffer, per_pcm_size, acc_sample_rate);

                    outputBuffer.get(outByteBuffer, 7, size);
                    outputBuffer.position(bufferInfo.offset);
                    fileOutputStream.write(outByteBuffer, 0, per_pcm_size);

                    encode.releaseOutputBuffer(outputBufferIndex, false);
                    outputBufferIndex = encode.dequeueOutputBuffer(bufferInfo, 0);
                    outByteBuffer = null;
                    Log.d(TAG, "encodePcm2AAC: ");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }


    private void addADtsHeader(byte[] packet, int packetLen, int samplerate) {
        int profile = 2; // AAC LC
        int freqIdx = samplerate; // samplerate
        int chanCfg = 2; // CPE

        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个t位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSsamplerate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }

    private void releaseMediaCodec() {

        if (encode != null) {
            encode.stop();
            encode.release();
            encode = null;
            mediaFormat = null;
            bufferInfo = null;
        }
        initMediaCodec = false;

        if (fileOutputStream != null) {
            try {
                fileOutputStream.close();
                fileOutputStream = null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        Log.d(TAG, "releaseMediaCodec: ");
    }

}
