package xeno.vm;

import xeno.Chr;
import xeno.PlayControl;
import xeno.util.Window;

public class System {
    public static native void arraycopy(Object var0, int var1, Object var2, int var3, int var4);

    public static native void exit(int var0);

    public static native void methodSignal(int var0);

    public static native void println(String var0);

    public static native void sleep(int var0);

    public static native void waitFor(Object var0);

    public static void waitSignal(Chr chr, int n) {
        int n2;
        while ((n2 = chr.getSignal()) != n) {
            System.sleep(1);
        }
    }

    public static void waitSignal(PlayControl playControl, int n) {
        int n2;
        while ((n2 = playControl.getSignal()) != n) {
            System.sleep(1);
        }
    }

    public static void waitSignal(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) != n) {
            System.sleep(1);
        }
    }
}

