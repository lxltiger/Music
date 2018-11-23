package kimascend.com.listener;

/**
 * Created by yangw on 2018-2-28.
 */

public interface OnPlayListener {

    void onPause(boolean pause);

    void onTimeInfo(int current, int total);

}
