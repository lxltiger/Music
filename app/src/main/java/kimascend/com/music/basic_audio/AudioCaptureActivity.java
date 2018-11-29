package kimascend.com.music.basic_audio;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import kimascend.com.music.R;
import kimascend.com.music.basic_audio.tester.AudioCaptureTester;
import kimascend.com.music.basic_audio.tester.AudioPlayerTester;
import kimascend.com.music.basic_audio.tester.Tester;

/**
 * 使用AudioRecord录制视频
 * 需要动态申请相关权限
 */
public class AudioCaptureActivity extends AppCompatActivity {
    private Tester recorder;
    private Tester player;
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE",
            "android.permission.RECORD_AUDIO"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_capture);
        verifyStoragePermissions(this);
        recorder = new AudioCaptureTester();
        player = new AudioPlayerTester();
    }

    public void verifyStoragePermissions(Activity activity) {

        try {
            //检测是否有写的权限
//            int permission = ActivityCompat.checkSelfPermission(activity,"android.permission.WRITE_EXTERNAL_STORAGE");
            int permission = ActivityCompat.checkSelfPermission(activity,"android.permission.RECORD_AUDIO");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS, REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    public void start_record(View view) {
        recorder.startTesting();
    }


    public void stop_record(View view) {
        recorder.stopTesting();

    }

    public void stop_play(View view) {
        player.stopTesting();
    }

    public void start_play(View view) {
        player.startTesting();
    }
}
