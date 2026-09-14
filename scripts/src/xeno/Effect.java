package xeno;

import xeno.Chr;
import xeno.Unit;
import xeno.util.Layout;
import xeno.util.Toolkit;
import xeno.util.Vector4f;

public class Effect {
    public int[] args;
    public Object peer;
    public int id;
    public float px;
    public float py;
    public float pz;
    public float rx;
    public float ry;
    public float rz;

    public Effect(int n) {
        this.id = n;
        this.args = new int[4];
    }

    public Effect(int n, float f, float f2, float f3, float f4) {
        this.id = n;
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.ry = f4;
        Toolkit.getPeer(this);
    }

    public Effect(int n, int n2) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setLocation(6, n2);
    }

    public Effect(int n, int n2, int n3, int n4, int n5) {
        this(n);
        this.args[1] = n2;
        this.args[2] = n3;
        this.args[3] = n4;
        this.args[4] = n5;
    }

    public native void call(int var1);

    public native void clearEffect();

    public native void disp(boolean var1);

    public native boolean getClip();

    public native boolean getForceLoop();

    public native void getRotate();

    public native Vector4f getScale();

    public native void getTranslate();

    public native void noAttach(boolean var1);

    public native void setCaster(Chr var1);

    public native void setCaster(Unit var1);

    public native boolean setClip(boolean var1);

    public native void setForceLoop(boolean var1);

    public void setLocation(int n) {
        Layout layout = Layout.getManager(0);
        layout.set(this, n);
    }

    public void setLocation(int n, int n2) {
        Layout layout = Layout.getManager(n);
        layout.set(this, n2);
    }

    public native void setMotion(boolean var1);

    public native void setRotate();

    public void setRotate(float f, float f2, float f3) {
        this.rx = f;
        this.ry = f2;
        this.rz = f3;
        this.setRotate();
    }

    public native void setScale(float var1, float var2, float var3);

    public native void setTarget(Chr var1);

    public native void setTarget(Unit var1);

    public native void setTransOffset(float var1, float var2, float var3);

    public native void setTranslate();

    public void setTranslate(float f, float f2, float f3) {
        this.px = f;
        this.py = f2;
        this.pz = f3;
        this.setTranslate();
    }
}

