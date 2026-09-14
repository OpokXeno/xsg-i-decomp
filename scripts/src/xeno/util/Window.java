package xeno.util;

public class Window {
    public native void clear();

    public native void close();

    public native void closeWaitKey();

    public static Window create() {
        return Window.create(0);
    }

    public static native Window create(int var0);

    public static Window create(int n, int n2, int n3, int n4) {
        Window window = Window.create(0);
        window.setLocation(n, n2);
        window.setSize(n3, n4);
        return window;
    }

    public native int getSignal();

    public native void print(String var1);

    public void print(String string, String string2) {
        this.setName(string);
        this.print(string2);
    }

    public native void print(String[] var1, int var2);

    public native void setLocation(int var1, int var2);

    public native void setName(String var1);

    public native void setSize(int var1, int var2);

    public native void signal(int var1);

    public native void wait(int var1);

    public native void waitkey(int var1);
}

