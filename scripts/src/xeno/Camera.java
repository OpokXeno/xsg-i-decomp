package xeno;

public class Camera {
    public native void change();

    public void changeID(int n) {
        this.changeID(n, 0, 0);
    }

    public native void changeID(int var1, int var2, int var3);

    public static native Camera create(int var0);

    public native void fovSPL(float[] var1, int var2);

    public native float getFov();

    public native int getMode();

    public native float getRotateX();

    public native float getRotateY();

    public native float getRotateZ();

    public native float getTranslateX();

    public native float getTranslateY();

    public native float getTranslateZ();

    public native void resetFog(int var1);

    public native void rollSPL(float[] var1, int var2);

    public native void rotateSPL(float[] var1, int var2);

    public native void rotateSPL(float[] var1, int var2, int var3, int var4);

    public native void setActive(boolean var1);

    public void setCFAngle(float f, float f2, float f3, float f4) {
        this.setCFAngle(-1, f, f2, f3, f4);
    }

    public void setCFAngle(float f, float f2, float f3, float f4, float f5) {
        this.setCFAnglePers(f, f2, f3, f4, f5);
    }

    public native void setCFAngle(int var1, float var2, float var3, float var4, float var5);

    public void setCFAngle(int n, float f, float f2, float f3, float f4, float f5) {
        this.setCFAnglePers(n, f, f2, f3, f4, f5);
    }

    public void setCFAnglePers(float f, float f2, float f3, float f4, float f5) {
        this.setCFAnglePers(-1, f, f2, f3, f4, f5);
    }

    public native void setCFAnglePers(int var1, float var2, float var3, float var4, float var5, float var6);

    public void setCFHokan(float f, float f2) {
        this.setCFHokan(-1, f, f2);
    }

    public native void setCFHokan(int var1, float var2, float var3);

    public native void setCFLock(int var1, int var2, float var3, float var4, float var5);

    public void setCFLockX(int n, float f) {
        this.setCFLock(n, 1, f, 0.0f, 0.0f);
    }

    public void setCFLockZ(int n, float f) {
        this.setCFLock(n, 4, 0.0f, 0.0f, f);
    }

    public void setCFOffset(float f, float f2, float f3) {
        this.setCFOffset(-1, f, f2, f3);
    }

    public void setCFOffset(int n, float f, float f2, float f3) {
        this.setCFOffset(n, f, f2, f3, 15, 0.05f);
    }

    public void setCFOffset(int n, float f, float f2, float f3, int n2, float f4) {
        this.setCFOffset(n, f, f2, f3, n2, f4, 60, 0.05f);
    }

    public native void setCFOffset(int var1, float var2, float var3, float var4, int var5, float var6, int var7, float var8);

    public void setCFPedestal(int n, float f, float f2, float f3, float f4, float f5, float f6) {
        this.setCFPedestal(n + 0x10000000, f, f2, f3, 0.0f, f4, f5, f6, 0.0f);
    }

    public native void setCFPedestal(int var1, float var2, float var3, float var4, float var5, float var6, float var7, float var8, float var9);

    public void setCFPedestalAngle(int n, float f, float f2, float f3, float f4, float f5, float f6) {
        this.setCFPedestal(n + 0x20000000, f, f2, f3, 0.0f, f4, f5, f6, 2.0f);
    }

    public native void setCFPedestalHokan(int var1, int var2);

    public void setCFPedestalPers(int n, float f) {
        this.setCFPedestal(n + 0x40000000, 0.0f, 0.0f, 0.0f, f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    public void setCFPedestalToPlayer(int n, float f, float f2, float f3) {
        this.setCFPedestal(n + 0x10000000, f, f2, f3, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    public void setCFUnlock(int n) {
        this.setCFLock(n, 0, 0.0f, 0.0f, 0.0f);
    }

    public native void setClipRange(float var1, float var2);

    public native void setFog(int var1, float var2, float var3, float var4, float var5, int var6, int var7, int var8, int var9);

    public native void setFov(float var1);

    public native void setMode(int var1);

    public native void setRoll(float var1);

    public native void setRotate(float var1, float var2, float var3);

    public native void setTranslate(float var1, float var2, float var3);

    public native void setView(float var1, float var2, float var3);

    public native void start(int var1, Object var2);

    public native void transCNS(Object var1, float var2, float var3, float var4);

    public native void transSPL(float[] var1, int var2);

    public native void transSPL(float[] var1, int var2, int var3, int var4);

    public native void viewCNS(Object var1, float var2, float var3, float var4);

    public native void viewSPL(float[] var1, int var2);

    public native void viewSPL(float[] var1, int var2, int var3, int var4);
}

