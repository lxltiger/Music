package kimascend.com.music;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.icu.text.TimeZoneFormat;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.text.format.DateFormat;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Locale;
import java.util.TimeZone;

import kimascend.com.listener.OnCompleteListener;
import kimascend.com.listener.OnErrorListener;
import kimascend.com.listener.OnLoadListener;
import kimascend.com.listener.OnPlayListener;
import kimascend.com.listener.OnPreparedListener;
import kimascend.com.player.Player;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss", Locale.CHINA);

    private Player player;

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};
    private TextView textView;


    public void verifyStoragePermissions(Activity activity) {

        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(activity,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    String timeInfo = String.format("%s/%s", dateFormat.format(msg.arg1 * 1000), dateFormat.format(msg.arg2 * 1000));
                    textView.setText(timeInfo);
                    break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //去除8小时时差
        dateFormat.setTimeZone(TimeZone.getTimeZone("GMT"));

        textView = findViewById(R.id.play_time_info);
        verifyStoragePermissions(this);
        player = new Player();
        player.setPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(TAG, "准备中.. ");
                player.startAudio();
            }
        });

        player.setLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean isLoading) {
                if (isLoading) {
                    Log.d(TAG, "加载中...");
                } else {
                    Log.d(TAG, "播放中...");
                }
            }
        });

        player.setPlayListener(new OnPlayListener() {
            @Override
            public void onPause(boolean pause) {

            }

            @Override
            public void onTimeInfo(int current, int total) {
                Message.obtain(handler,0,current,total).sendToTarget();
            }
        });

        player.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int code, String msg) {
                Log.d(TAG, "onError() called with: code = [" + code + "], msg = [" + msg + "]");
            }
        });

        player.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                Log.d(TAG, "onComplete() called");
            }
        });
    }


    public void start(View view) {
//        File musicDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC);
//        Log.d(TAG, musicDirectory.getAbsolutePath());
//        String url = musicDirectory + "/Akon.mp3";
//        player.setSource(url);
        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        player.prepareAudio();

    }

    public void pause(View view) {
        Log.d(TAG, "pause: ");
        player.pause();
    }

    public void resume(View view) {
        Log.d(TAG, "resume: ");
        player.resume();
    }

    public void stop(View view) {
        player.stopAudio();
    }

    public void seek(View view) {
        player.seek(210);
    }
}
