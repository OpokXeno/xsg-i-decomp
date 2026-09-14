package xeno;

import xeno.Unit;
import xeno.util.Toolkit;

public class Monitor
        extends Unit {
    public Monitor(float f, float f2, float f3, float f4) {
        this.id = 24579;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, this.id);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setParams(-0.5f, 1.0f, 1.0f, 0.5f, 0.0f, 0.1f, 64);
    }

    public void init(float f, float f2, float f3, float f4) {
        this.id = 24579;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, this.id);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setParams(-0.5f, 1.0f, 1.0f, 0.5f, 0.0f, 0.1f, 64);
    }

    public void setParams(float f, float f2, float f3, float f4, float f5, float f6, int n) {
        this.setArgs(0, f);
        this.setArgs(4, f2);
        this.setArgs(8, f3);
        this.setArgs(12, f4);
        this.setArgs(16, f5);
        this.setArgs(20, f6);
        this.setArgs(32, n, 1);
    }

    public void setSize(float f, float f2) {
        this.setArgs(8, f);
        this.setArgs(12, f2);
    }
}

