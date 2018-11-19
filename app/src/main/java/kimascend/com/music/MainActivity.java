package kimascend.com.music;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import kimascend.com.player.Demo;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private Demo demo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        demo = new Demo();
        demo.setOnErrerListener(new Demo.OnErrerListener() {
            @Override
            public void onError(int code, String msg) {
                Log.d(TAG, "onError() called with: code = [" + code + "], msg = [" + msg + "]");
            }
        });
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(demo.stringFromJNI());
    }


    public void producer_customer(View view) {
        demo.createThread();

    }

    public void callJava(View view) {
        demo.callBackFromC();
    }
}
