package kimascend.com.music.basic_audio.api;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;

import static android.media.AudioRecord.ERROR_BAD_VALUE;
import static kimascend.com.music.utils.LogUtil.logd;
import static kimascend.com.music.utils.LogUtil.loge;

public class AudioPlayer {
    private static final int STREAMTYPE = AudioManager.STREAM_MUSIC;
    private static final int SAMPLERATEINHZ = 44100;
    private static final int CHANNELCONFIG = AudioFormat.CHANNEL_IN_STEREO;
    private static final int AUDIOFORMAT = AudioFormat.ENCODING_PCM_16BIT;
    private static final int MODE = AudioTrack.MODE_STREAM;
    private int minBufferSize;
    private boolean isPlaying = false;
    private AudioTrack audioTrack;


    public boolean isPlaying() {
        return isPlaying;
    }

    public boolean startPlay() {
        return startPlay(STREAMTYPE, SAMPLERATEINHZ, CHANNELCONFIG, AUDIOFORMAT, MODE);
    }

    private boolean startPlay(int streamType, int sampleRateInHz, int channelConfig, int audioFormat, int mode) {
        if (isPlaying) {
            loge("is isPlaying ");
            return false;
        }
        isPlaying = true;

        minBufferSize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
        if (ERROR_BAD_VALUE == minBufferSize) {
            loge("Invalid parameter");
            isPlaying = false;
            return false;
        }
        if (AudioRecord.ERROR == minBufferSize) {
            loge("fail to get minBufferSize");
            isPlaying = false;
            return false;
        }
        audioTrack = new AudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, minBufferSize, mode);
        if (AudioTrack.STATE_UNINITIALIZED == audioTrack.getState()) {
            isPlaying = false;
            return false;
        }
        logd("start play");
        return true;
    }


    public void stopPlay() {
        if (!isPlaying) {
            return;
        }
        isPlaying = false;
        if (AudioTrack.PLAYSTATE_PLAYING == audioTrack.getState()) {
            audioTrack.stop();
        }

        audioTrack.release();

        logd("stop play");
    }

    public boolean play(byte[] buffer, int offset, int size) {
        if (!isPlaying) {
            loge("is not playing");
            return false;
        }

        if (size < minBufferSize) {
            loge("byte is not enough");
            return false;
        }

        if (audioTrack.write(buffer, offset, size) != size) {
            loge("could not write all buffer ");
            return false;
        }

        audioTrack.play();

        return true;
    }

}
