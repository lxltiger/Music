package kimascend.com.music.basic_audio.tester;

import android.os.Environment;

import java.io.IOException;

import kimascend.com.music.basic_audio.api.AudioPlayer;
import kimascend.com.music.basic_audio.api.WaveFileReader;

public class AudioPlayerTester implements Tester {

    private String path = Environment.getExternalStorageDirectory() + "/test.wav";
    private AudioPlayer audioPlayer;
    private WaveFileReader waveFileReader;

    private static final int SAMPLES_PER_FRAME = 1024;
    private volatile boolean exit = false;
    @Override
    public boolean startTesting() {
        audioPlayer = new AudioPlayer();
        waveFileReader = new WaveFileReader();
        try {
            waveFileReader.openFile(path);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        audioPlayer.startPlay();
        new Thread(runnable).start();
        return true;
    }

    private Runnable runnable=new Runnable() {
        @Override
        public void run() {
            byte[] buffer=new byte[SAMPLES_PER_FRAME*2];
            while (!exit && waveFileReader.readData(buffer, 0, buffer.length) > 0) {
                audioPlayer.play(buffer, 0, buffer.length);
            }
            audioPlayer.stopPlay();;
            try {
                waveFileReader.closeFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    };
    @Override
    public boolean stopTesting() {
        exit=true;
        return true;
    }
}
