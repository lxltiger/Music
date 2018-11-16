package kimascend.com.music;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import kimascend.com.player.Demo;

public class MainActivity extends AppCompatActivity {

    private Demo demo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        demo = new Demo();
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(demo.stringFromJNI());
    }


    public void create_thread(View view) {
        demo.createThread();

    }
}
