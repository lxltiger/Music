package kimascend.com.music.basic_audio.api;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.SystemClock;
import android.util.Log;

import static android.media.AudioRecord.ERROR_BAD_VALUE;
import static android.media.AudioRecord.ERROR_INVALID_OPERATION;
import static kimascend.com.music.utils.LogUtil.logd;
import static kimascend.com.music.utils.LogUtil.loge;

public class AudioCapturer {
    private static final String TAG = "AudioCapturer";
    private static final int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;

    private static final int SAMPLE_RATE = 44100;
    private static final int CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_STEREO;
    private static final int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;

    private boolean isCapturing = false;
    private int minBufferSize;
    private AudioRecord audioRecord;
    private volatile boolean exit_capture = false;
    private AudioFrameListener audioFrameListener;
    private Thread captureThread;



    public interface AudioFrameListener {
        void onFrameAvailable(byte[] buffer);
    }

    public void setAudioFrameListener(AudioFrameListener audioFrameListener) {
        this.audioFrameListener = audioFrameListener;
    }

    public boolean startCapture() {
        return startCapture(AUDIO_SOURCE, SAMPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT);
    }

    public boolean startCapture(int audioSource, int sampleRateInHz, int channelConfig, int audioFormat) {

        if (isCapturing) {
            loge("is capturing ");
            return false;
        }
        isCapturing = true;

        minBufferSize = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
        if (ERROR_BAD_VALUE == minBufferSize) {
            loge("Invalid parameter");
            isCapturing = false;
            return false;
        }
        if (AudioRecord.ERROR == minBufferSize) {
            loge("fail to get minBufferSize");
            isCapturing = false;
            return false;
        }

        audioRecord = new AudioRecord(audioSource, sampleRateInHz, channelConfig, audioFormat, minBufferSize);
        if (AudioRecord.STATE_UNINITIALIZED == audioRecord.getState()) {
            loge("fail to initialize");
            isCapturing = false;
            return false;
        }
        audioRecord.startRecording();

        exit_capture=false;
        captureThread = new Thread(runnable);
        captureThread.start();

        logd("start capture");

        return true;
    }


    public void stopCapture() {
        if (!isCapturing) {
            logd("is not capturing");
            return;
        }
        isCapturing=false;
        exit_capture=true;
        captureThread.interrupt();
        try {
            captureThread.join(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (AudioRecord.RECORDSTATE_RECORDING==audioRecord.getRecordingState()) {
            audioRecord.stop();
        }

        audioRecord.release();
        logd("success to stop capture");


    }
   private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            while (!exit_capture) {
                byte[] buffer = new byte[minBufferSize];
                int ret = audioRecord.read(buffer, 0, minBufferSize);
                if (ERROR_INVALID_OPERATION == ret) {
                    loge("ERROR_INVALID_OPERATION");
                } else if (ERROR_BAD_VALUE == ret) {
                    loge("ERROR_BAD_VALUE");
                } else {
                    if (audioFrameListener != null) {
                        audioFrameListener.onFrameAvailable(buffer);
                    }
                }
                SystemClock.sleep(10);
            }
        }
    };
}
