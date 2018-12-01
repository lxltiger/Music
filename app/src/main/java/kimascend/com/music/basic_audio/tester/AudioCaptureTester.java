package kimascend.com.music.basic_audio.tester;

import android.media.AudioFormat;
import android.media.MediaRecorder;
import android.os.Environment;

import java.io.IOException;

import kimascend.com.music.basic_audio.api.AudioCapturer;
import kimascend.com.music.basic_audio.wav.WavFileWriter;

import static kimascend.com.music.utils.LogUtil.loge;

public class AudioCaptureTester implements Tester,AudioCapturer.AudioFrameListener {


    private AudioCapturer audioCapturer;
    private WavFileWriter wavFileWriter;
    private String path = Environment.getExternalStorageDirectory() + "/test.wav";
    @Override
    public void onFrameAvailable(byte[] buffer) {
        wavFileWriter.writeData(buffer, 0, buffer.length);
    }

    @Override
    public boolean startTesting() {
        audioCapturer=new AudioCapturer();
        wavFileWriter=new WavFileWriter();
        try {
            wavFileWriter.openFile(path, 44100, 1, 16);
        } catch (IOException e) {
            loge("fail to open file");
            return false;
        }
        audioCapturer.setAudioFrameListener(this);

        return audioCapturer.startCapture(MediaRecorder.AudioSource.MIC, 44100, AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
    }

    @Override
    public boolean stopTesting() {
        audioCapturer.stopCapture();
        try {
            wavFileWriter.closeFile();
        } catch (IOException e) {
            loge("fail to clsoe file");
            return false;
        }
        return true;
    }
}
