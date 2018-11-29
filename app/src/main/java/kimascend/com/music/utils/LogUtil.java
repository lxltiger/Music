package kimascend.com.music.utils;

import android.util.Log;

public class LogUtil {

    private LogUtil() {
    }

    private static final String TAG = "LogUtil";

    public static void logd(String msg) {
        Log.d(TAG, msg);
    }

    public static void loge(String msg) {
        Log.e(TAG, msg);
    }

}
