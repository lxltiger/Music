package kimascend.com.music;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import kimascend.com.listener.OnPreparedListener;
import kimascend.com.player.Demo;
import kimascend.com.player.Player;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private Player player;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        player = new Player();
        player.setListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(TAG, "Prepared: ");
            }
        });
    }




    public void start(View view) {
        player.startAudio();
    }

    public void Prepare(View view) {
        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        player.prepareAudio();
    }
}
