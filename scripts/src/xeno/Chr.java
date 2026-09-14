package xeno;

import xeno.Light;
import xeno.util.Format;
import xeno.util.Layout;
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.util.Vector4f;

public class Chr {
    public Object peer;
    public int algorithm;
    public Object model;
    private Chr[] child;
    public Light light;
    public int id;
    public float px;
    public float py;
    public float pz;
    public float rx;
    public float ry;
    public float rz;

    protected native Object childGetPeer(int var1, int var2);

    public native void dispRadar(boolean var1);

    public native int getArgs(int var1, int var2);

    public Chr getChild(int n) {
        Chr chr = null;
        if (n == 0x1000000 && this.child != null) {
            chr = this.child[0];
        }
        return chr;
    }

    protected native int getFlags();

    public native void getPlayer();

    public native void getRotate();

    public native Vector4f getScale();

    public native int getSerial();

    public native int getSignal();

    public native int getState();

    public native void getTranslate();

    public native void hairStop(int var1, int var2);

    public native void ignoreShape(int var1);

    public void init(float f, float f2, float f3) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        Toolkit.getPeer(this);
    }

    public void init(int n) {
        this.id = n & 0xFFFFFF;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
        if ((n & 0x1000000) != 0) {
            Chr chr = new Chr();
            chr.id = (n & 0xFFFFFF) + 65536 & 0xFFFFFF;
            chr.setPeer(this.childGetPeer(0x1000000, 0));
            if (this.child == null) {
                this.child = new Chr[4];
                this.child[0] = chr;
            }
        }
    }

    public void init(int n, float f, float f2, float f3, float f4) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        this.init(n);
    }

    public void init(int n, int n2) {
        this.id = n;
        Toolkit.getPeer(this);
        Toolkit.loadResource((Object) this, n, n2);
    }

    public void init(int n, int n2, float f, float f2, float f3, float f4) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        this.init(n, n2);
    }

    public native void invalidate();

    public native void look_camera();

    public native void look_char(Object var1);

    public native void look_default();

    public native void look_eye_control(int var1);

    public native void look_eye_set(float var1, float var2);

    public native void look_eye_speed(float var1);

    public native void look_point(float var1, float var2, float var3);

    public native void look_speed(float var1);

    public native void look_unit(Object var1);

    public native void move(float var1, float var2, float var3, boolean var4);

    public native void move(int var1, float var2, float var3, boolean var4);

    public native void move(int var1, Object var2, boolean var3);

    public native void move(Object var1, float var2, boolean var3);

    public native void move(Spline var1, int var2, boolean var3);

    public void move(Spline spline, boolean bl) {
        this.move(spline, 16, bl);
    }

    public native void mtn(int var1, int var2, float var3, boolean var4);

    public native void mtn(int var1, int var2, int var3, int var4, int var5, float var6, boolean var7);

    public native void mtnGetRoot(int var1, Vector4f var2);

    public void npc(int n, float f, float f2, float f3, float f4) {
        this.setAlgorithm(1);
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        this.init(n);
    }

    public void npc(int n, int n2, float f, float f2, float f3, float f4) {
        this.setAlgorithm(1);
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        this.init(n, n2);
    }

    public native void pixelAlpha(int var1);

    public native void pixelAlphaParts(int var1, int var2);

    public native void pixelAlphaPartsReset();

    public native void relax(int var1, int var2);

    public native void renderCommand(int var1);

    public native void resetEnv();

    public native void resetHand();

    public native void resetWeaponR(Chr var1);

    public native void rotCNS(int var1, float var2, float var3, float var4);

    public native void rotX(float var1, float var2, boolean var3);

    public native void rotX(int var1, float var2, boolean var3);

    public native void rotX(int var1, Object var2, boolean var3);

    public native void rotX(Object var1, float var2, boolean var3);

    public native void rotY(float var1, float var2, boolean var3);

    public native void rotY(int var1, float var2, boolean var3);

    public native void rotY(int var1, Object var2, boolean var3);

    public native void rotY(Object var1, float var2, boolean var3);

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
        this.algorithm &= 0xFFFFF0FF;
        this.algorithm |= n << 8 & 0xF00;
    }

    public void setArgs(int n, float f) {
        this.setArgs(n, Format.floatToIntBits(f), 4);
    }

    public native void setArgs(int var1, int var2, int var3);

    public native void setArgs(int var1, Object var2, int var3);

    public native void setClip(int var1);

    public native void setCollision(boolean var1);

    public native void setEdgeFall(int var1);

    public native void setElevatorMode(int var1);

    public native int setFilter(int var1);

    public native void setFilterParam(float[] var1);

    protected native void setFlags(int var1);

    public native void setHand(int var1);

    public native void setID(int var1);

    public void setLightMode(int n) {
        int n2 = this.getFlags();
        switch (n) {
            case 0: {
                this.setFlags(n2 & 0xFFFF7FFF);
                break;
            }
            case 1: {
                this.setFlags(n2 | 0x8000);
                break;
            }
        }
    }

    public void setLocation(int n) {
        Layout layout = Layout.getManager(0);
        layout.set(this, n);
    }

    public void setLocation(int n, int n2) {
        Layout layout = Layout.getManager(n);
        layout.set(this, n2);
    }

    public native void setMotNoUpdate(int var1);

    public native void setMotionFlags(int var1, boolean var2);

    public void setOrientation(int n) {
        this.algorithm &= 0xFFFFFFFE;
        this.algorithm |= n;
    }

    public native void setParent(Chr var1, int var2, int var3, int var4);

    protected native void setPeer(Object var1);

    public native void setPlayer();

    public native void setPointLightCol(int var1, float var2, float var3, float var4);

    public native void setPointLightPos(int var1, float var2, float var3, float var4);

    public native void setPointLightReset();

    public native void setRotCNSParam(int var1, float var2, float var3, float var4, float var5, float var6);

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

    public native void setShadow(byte[] var1);

    public native void setSortOffset(float var1);

    public native void setSymmetryY(int var1);

    public native void setTranslate();

    public void setTranslate(float f, float f2, float f3) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.setTranslate();
    }

    public native void setVisible(int var1, boolean var2);

    public native void setVisible(boolean var1);

    public native void setWeaponR(Chr var1);

    public native void shadow_clip_scale(float var1);

    public native void shadow_map_id(int var1);

    public native void shadow_map_reset();

    public native void signal(int var1);

    public native void start(int var1, Object var2);

    public native void stop();

    public native void talkto(String var1);

    public native void touchto(String var1);

    public native void validate();
}

