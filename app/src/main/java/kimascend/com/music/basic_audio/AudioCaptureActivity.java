package kimascend.com.music.basic_audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import kimascend.com.music.R;

/**
 * 使用AudioRecord录制视频
 */
public class AudioCaptureActivity extends AppCompatActivity {

    public static final int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;

    public static final int SAMPLE_RATE = 44100;
    public static final int CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_STEREO;
    public static final int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_capture);

    }
}
