package kimascend.com.player;

public class Demo {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    public native void testFfempg();

    public native void createThread();


    public native void callBackFromC();



    private OnErrerListener onErrerListener;

    public void setOnErrerListener(OnErrerListener onErrerListener) {
        this.onErrerListener = onErrerListener;
    }

    public void onError(int code, String msg)
    {
        if(onErrerListener != null)
        {
            onErrerListener.onError(code, msg);
        }
    }

    public interface OnErrerListener
    {
        void onError(int code, String msg);
    }
}
