package xeno;

import xeno.Camera;

public class Stage {
    public Camera cam0;
    public Object peer;
    public int id;

    public static native void clrBackBuffer();

    public native void play(String var1);

    public static native void renderCommand(int var0);

    public static native void setBgClip(int var0);

    public static native void setBgColor(float var0, float var1, float var2);

    public static native void setCFBG(int var0, int var1, float[] var2);

    public static void setCFBG(int n, float[] fArray) {
        Stage.setCFBG(n, 0, fArray);
    }

    public static native void setColor(float var0, float var1, float var2);

    public static native void setEffectRender(int var0);

    public static native void setEventFade(int var0, float var1, float var2, float var3, int var4, float var5, float var6, float var7);

    public static native void setFade(int var0, int var1, float var2, float var3, float var4);

    public static native void setFadeCancel(int var0);

    public static native void setFrameRender(int var0, int var1);

    public static native void setPartsLast(int var0);

    public static native void setPartsLastReset();

    public static native void setVisible(int var0, boolean var1);

    public native void start(int var1, Object var2);

    public native void stop();
}

