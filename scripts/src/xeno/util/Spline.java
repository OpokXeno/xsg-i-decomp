package xeno.util;

import xeno.util.Vector4f;

public class Spline {
    public static native Spline create();

    public static Spline create(float[] fArray, int n, int n2, int n3) {
        Spline spline = Spline.create();
        spline.setCtrlVertex(fArray, n, n2, n3);
        return spline;
    }

    public native Vector4f getValue(int var1);

    public native void setCtrlVertex(float[] var1, int var2, int var3, int var4);
}

