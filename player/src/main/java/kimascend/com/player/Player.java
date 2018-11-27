package kimascend.com.player;

import android.text.TextUtils;
import android.util.Log;

import kimascend.com.listener.OnCompleteListener;
import kimascend.com.listener.OnErrorListener;
import kimascend.com.listener.OnLoadListener;
import kimascend.com.listener.OnPlayListener;
import kimascend.com.listener.OnPreparedListener;

public class Player {
    private static final String TAG = "Player";
    private String source;
    private OnPreparedListener preparedListener;
    private OnLoadListener loadListener;
    private OnPlayListener playListener;
    private OnErrorListener onErrorListener;
    private OnCompleteListener onCompleteListener;
    private int volume=100;
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }


    public void setPreparedListener(OnPreparedListener preparedListener) {
        this.preparedListener = preparedListener;
    }

    public void setLoadListener(OnLoadListener loadListener) {
        this.loadListener = loadListener;
    }

    public void setPlayListener(OnPlayListener playListener) {
        this.playListener = playListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public Player() {
    }

    public void prepareAudio() {
        if (TextUtils.isEmpty(source)) {
            Log.e(TAG, "empty url");
            return;
        }

        new Thread() {
            @Override
            public void run() {
                prepare(source);
            }
        }.start();
    }

    public void startAudio() {
        if (TextUtils.isEmpty(source)) {
            Log.e(TAG, "empty url");
            return;
        }

        new Thread() {
            @Override
            public void run() {
                Player.this.start();
            }
        }.start();
    }

    public void stopAudio() {

        new Thread() {
            @Override
            public void run() {
                Player.this.stop();
            }
        }.start();
    }

    public void setVolume(int volume) {
        this.volume=volume;
        set_Volume(volume);
    }

    public int getVolume() {
        return volume;
    }

    //----------C++调用
    public void onPrepared() {
        if (preparedListener != null) {
            preparedListener.onPrepared();
        }
    }

    public void onLoad(boolean isLoading) {
        if (loadListener != null) {
            loadListener.onLoad(isLoading);
        }
    }


    public void onPlaying(int current, int total) {
        Log.d(TAG, "onPlaying() called with: current = [" + current + "], total = [" + total + "]");
        if (playListener != null) {
            playListener.onTimeInfo(current, total);
        }
    }

    public void onError(int code, String msg) {
        if (onErrorListener != null) {
            stopAudio();
            onErrorListener.onError(code, msg);
        }
    }

    public void onComplete() {
        if (onCompleteListener != null) {
            stopAudio();
            onCompleteListener.onComplete();
        }
    }

    private native void prepare(String source);

    private native void start();

    public native void pause();

    public native void resume();

    private native void stop();
    private native void set_Volume(int volume);
    public native void seek(int seconds);

    public native void setMute(int mute);

    public native int getDuration();

}
