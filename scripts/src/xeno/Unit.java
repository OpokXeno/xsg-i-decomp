package xeno;

import xeno.Effect;
import xeno.util.Format;
import xeno.util.Layout;
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.util.Vector4f;

public class Unit {
    public Object peer;
    public int algorithm;
    public int id;
    public float px;
    public float py;
    public float pz;
    public float rx;
    public float ry;
    public float rz;

    public native int getArgs(int var1, int var2);

    public native void getAxis(Vector4f var1);

    public native void getPivot(Vector4f var1);

    public native void getRotate();

    public native Vector4f getScale();

    public native int getSerial();

    public native int getSignal();

    public native int getState();

    public native void getTranslate();

    public void getXAxis(Vector4f vector4f) {
        this.getAxis(vector4f);
    }

    public void getYAxis(Vector4f vector4f) {
        this.getAxis(vector4f);
    }

    public void getZAxis(Vector4f vector4f) {
        this.getAxis(vector4f);
    }

    public static void group(int n) {
        Toolkit.peerSetGroup(1, n);
    }

    public void init(float f, float f2, float f3) {
        this.algorithm = 0;
        this.px = f;
        this.py = f2;
        this.pz = f3;
        Toolkit.getPeer(this);
    }

    public void init(int n) {
        this.algorithm = 0;
        this.id = n;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
    }

    public void init(int n, float f, float f2, float f3, float f4) {
        this.algorithm = 0;
        this.id = n;
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
    }

    public void init(int n, int n2, float f, float f2, float f3, float f4) {
        this.algorithm = 0;
        this.id = n;
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        Toolkit.getPeer(this);
        Toolkit.loadResource((Object) this, n, n2);
    }

    public void initElevator(int n, float f, float f2) {
        this.algorithm = 0;
        this.id = n;
        Toolkit.getPeer(this);
        this.setArgs(0, n, 1);
        this.setArgs(4, f);
        this.setArgs(8, f2);
        this.initElevatorFunc();
    }

    public native void initElevatorFunc();

    public native void invalidate();

    public void mapUnit(int n) {
        this.id = n;
        this.setAlgorithm(1);
        Toolkit.getPeer(this);
    }

    public native void map_shadow(int var1);

    public native void move(float var1, float var2, float var3, boolean var4);

    public native void move(int var1, float var2, float var3, boolean var4);

    public native void move(int var1, Object var2, boolean var3);

    public native void move(Spline var1, int var2, boolean var3);

    public void move(Spline spline, boolean bl) {
        this.move(spline, 16, bl);
    }

    public native void mtn(int var1, int var2, float var3, boolean var4);

    public native void mtn(int var1, int var2, int var3, int var4, int var5, float var6, boolean var7);

    public native void mtnGetRoot(int var1, Vector4f var2);

    public native void mtnSetMask(int var1);

    public native void renderCommand(int var1);

    public static native void resume(int var0);

    public native void rotX(float var1, float var2, boolean var3);

    public native void rotX(int var1, float var2, boolean var3);

    public native void rotX(int var1, Object var2, boolean var3);

    public native void rotX(Object var1, float var2, boolean var3);

    public native void rotY(float var1, float var2, boolean var3);

    public native void rotY(int var1, float var2, boolean var3);

    public native void rotY(int var1, Object var2, boolean var3);

    public native void rotY(Object var1, float var2, boolean var3);

    public void rotYCNS(Object object, int n) {
        this.rotYCNS(object, n, 0.0f, 0.0f, 1.0f);
    }

    public native void rotYCNS(Object var1, int var2, float var3, float var4, float var5);

    public native void rotZ(float var1, float var2, boolean var3);

    public native void rotZ(int var1, float var2, boolean var3);

    public native void rotZ(int var1, Object var2, boolean var3);

    public native void rotZ(Object var1, float var2, boolean var3);

    public native void rotate(Spline var1, int var2, boolean var3);

    public void rotate(Spline spline, boolean bl) {
        this.rotate(spline, 56, bl);
    }

    public native void scale(Spline var1, int var2, boolean var3);

    public void scale(Spline spline, boolean bl) {
        this.scale(spline, 7, bl);
    }

    public native void sclX(float var1, float var2, boolean var3);

    public native void sclX(int var1, float var2, boolean var3);

    public native void sclY(float var1, float var2, boolean var3);

    public native void sclY(int var1, float var2, boolean var3);

    public native void sclZ(float var1, float var2, boolean var3);

    public native void sclZ(int var1, float var2, boolean var3);

    public void setAlgorithm(int n) {
        this.algorithm &= 0xFFFF00FF;
        this.algorithm |= n << 8 & 0xFF00;
    }

    public void setArgs(int n, float f) {
        this.setArgs(n, Format.floatToIntBits(f), 4);
    }

    public void setArgs(int n, float f, float f2, float f3, float f4) {
        this.setArgs(n, Format.floatToIntBits(f), Format.floatToIntBits(f2), Format.floatToIntBits(f3), Format.floatToIntBits(f4));
    }

    public native void setArgs(int var1, int var2, int var3);

    public native void setArgs(int var1, int var2, int var3, int var4, int var5);

    public native void setArgs(int var1, Object var2, int var3);

    public native void setAxis(float var1, float var2, float var3, float var4);

    public native void setClip(int var1);

    public native void setCollision(boolean var1);

    public void setEffect(Effect effect) {
        effect.setCaster(this);
    }

    public native int setFilter(int var1);

    public native void setFilterParam(float[] var1);

    public void setLocation(int n) {
        Layout layout = Layout.getManager(0);
        layout.set(this, n);
    }

    public void setLocation(int n, int n2) {
        Layout layout = Layout.getManager(n);
        layout.set(this, n2);
    }

    public native void setMonitorPrio(int var1);

    public void setOrientation(int n) {
        this.algorithm &= 0xFFFFFFFE;
        this.algorithm |= n & 1;
    }

    public native void setParent(Object var1, int var2);

    public native void setPivot(float var1, float var2, float var3);

    public native void setRotate();

    public void setRotate(float f, float f2, float f3) {
        this.rx = f;
        this.ry = f2;
        this.rz = f3;
        this.setRotate();
    }

    public void setRotateY(float f) {
        this.getRotate();
        this.ry = f;
        this.setRotate();
    }

    public native void setScale(float var1, float var2, float var3);

    public native void setShadow(int var1, int var2);

    public native void setSortOffset(float var1);

    public native void setTranslate();

    public void setTranslate(float f, float f2, float f3) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.setTranslate();
    }

    public native void setVisible(int var1, boolean var2);

    public native void setVisible(boolean var1);

    public void setXAxis(float f, float f2, float f3) {
        this.setAxis(f, f2, f3, 1.0f);
    }

    public void setYAxis(float f, float f2, float f3) {
        this.setAxis(f, f2, f3, 2.0f);
    }

    public void setZAxis(float f, float f2, float f3) {
        this.setAxis(f, f2, f3, 3.0f);
    }

    public native void shadow_clip_scale(float var1);

    public native void shadow_map_id(int var1);

    public native void shadow_map_reset();

    public native void signal(int var1);

    public native void start(int var1, Object var2);

    public native void stop();

    public static native void suspend(int var0);

    public native void transCNS(Object var1, int var2, float var3, float var4, float var5);

    public void unit(int n) {
        this.id = n;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
    }

    public void unit(Object object, int n) {
        this.id = n;
        this.setAlgorithm(2);
        Toolkit.getPeer(this);
    }

    public native void validate();
}

