package xeno;

public class Sound {
    public static void effectPlay(int n) {
        Sound.effectPlay(n, 127, 64);
    }

    public static native void effectPlay(int var0, int var1, int var2);

    public static native void effectStop(int var0);

    public static native void sequencePlay(int var0);

    public static native void sequencePlay(int var0, int var1);

    public static native void sequenceStop(int var0);

    public static native void sequenceStop(int var0, int var1);

    public static void streamPlay(int n) {
        Sound.streamPlay(n, 44100, 127, 64);
    }

    public static void streamPlay(int n, int n2) {
        Sound.streamPlay(n, n2, 127, 64);
    }

    public static void streamPlay(int n, int n2, int n3) {
        Sound.streamPlay(n, n2, n3, 64);
    }

    public static native void streamPlay(int var0, int var1, int var2, int var3);
}

