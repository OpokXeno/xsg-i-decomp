package xeno.vm;

public class Math {
    public static final float PI = (float) java.lang.Math.PI;

    public static native float atan2(float var0, float var1);

    public static native float cos(float var0);

    public static native int random();

    public static native float sin(float var0);

    public static float toDegrees(float f) {
        return f * 180.0f / (float) java.lang.Math.PI;
    }

    public static float toRadians(float f) {
        return f / 180.0f * (float) java.lang.Math.PI;
    }
}

