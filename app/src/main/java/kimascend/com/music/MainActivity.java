package kimascend.com.music;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import java.io.File;

import kimascend.com.listener.OnPreparedListener;
import kimascend.com.player.Demo;
import kimascend.com.player.Player;

import static android.os.Environment.DIRECTORY_MUSIC;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private Player player;

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE" };


    public  void verifyStoragePermissions(Activity activity) {

        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(activity,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE,REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
            verifyStoragePermissions(this);
        player = new Player();
        player.setListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(TAG, "Prepared: ");
                player.startAudio();
            }
        });
    }




    public void start(View view) {
//        player.startAudio();
        File musicDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC);
        Log.d(TAG, musicDirectory.getAbsolutePath());
        /*if (musicDirectory.isDirectory()) {
            File[] files = musicDirectory.listFiles();
            for (File file : files) {
                Log.d(TAG, "start: "+file.getAbsolutePath());/mnt/sdcard/Music/Akon.mp3
            }
        }*/
        String url = musicDirectory+"/Akon.pcm";
        player.play(url);

    }

    public void Prepare(View view) {
        File musicDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC);
        Log.d(TAG, musicDirectory.getAbsolutePath());
        String url = musicDirectory+"/Akon.mp3";

//        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        player.setSource(url);
        player.prepareAudio();
    }
}
