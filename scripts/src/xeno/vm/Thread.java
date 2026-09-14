package xeno.vm;

public class Thread {
    public static native Thread create();

    public static Thread create(Object object, String string) {
        Thread thread = Thread.create();
        thread.setTarget(object, string);
        return thread;
    }

    public native void setTarget(Object var1, String var2);

    public native void start();

    public native void stop();
}

