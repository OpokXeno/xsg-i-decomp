package xeno;

import xeno.util.Toolkit;

public class PlayControl {
    public static native PlayControl create();

    public native Object getParams();

    public native int getSignal();

    public native void init(int var1, int var2);

    public native void init(int var1, int var2, int var3, int var4, float var5);

    public native void loadCamera(Object var1);

    public void loadCamera(String string) {
        Object object = Toolkit.loadResource(string);
        this.loadCamera(object);
    }

    public native void loadTimeChart(Object var1);

    public void loadTimeChart(String string) {
        Object object = Toolkit.loadResource(string);
        this.loadTimeChart(object);
    }

    public native void setObserver(int var1, int var2, Object var3, String var4);

    public native void setObserver(int var1, String var2, Object var3, String var4);

    public native void signal(int var1);

    public native void start();

    public native void stop();
}

