package xeno;

import xeno.Chr;
import xeno.util.Toolkit;

public class Enepc
        extends Chr {
    public Enepc() {
    }

    public Enepc(int n, float f, float f2, float f3, float f4) {
        this.id = n;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
    }

    public void disableDTKFlag(int n) {
        int n2 = this.getArgs(100, 4);
        this.setArgs(100, n2 &= ~n, 4);
    }

    public void enableDTKFlag(int n) {
        int n2 = this.getArgs(100, 4);
        this.setArgs(100, n2 |= n, 4);
    }

    public void init(int n, float f, float f2, float f3, float f4) {
        this.id = n;
        Toolkit.getPeer(this);
        Toolkit.loadResource(this, n);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
    }

    public void kickEnepc(int n, int n2) {
        int n3 = this.getArgs(96, 4);
        if (n3 >= 16) {
            return;
        }
        this.setArgs(128 + n3 * 4, n, 4);
        this.setArgs(192 + n3 * 4, n2, 4);
        this.setArgs(256 + n3 * 4, 10000, 4);
        this.setArgs(320 + n3 * 4, 10000, 4);
        this.setArgs(384 + n3 * 4, 10000, 4);
        this.setArgs(448 + n3 * 4, 10000, 4);
        this.setArgs(96, ++n3, 4);
    }

    public void kickEnepc(int n, int n2, int n3) {
        int n4 = this.getArgs(96, 4);
        if (n4 >= 16) {
            return;
        }
        this.setArgs(128 + n4 * 4, n, 4);
        this.setArgs(192 + n4 * 4, n2, 4);
        this.setArgs(256 + n4 * 4, n3, 4);
        this.setArgs(320 + n4 * 4, 10000, 4);
        this.setArgs(384 + n4 * 4, 10000, 4);
        this.setArgs(448 + n4 * 4, 10000, 4);
        this.setArgs(96, ++n4, 4);
    }

    public void kickEnepc(int n, int n2, int n3, int n4) {
        int n5 = this.getArgs(96, 4);
        if (n5 >= 16) {
            return;
        }
        this.setArgs(128 + n5 * 4, n, 4);
        this.setArgs(192 + n5 * 4, n2, 4);
        this.setArgs(256 + n5 * 4, n3, 4);
        this.setArgs(320 + n5 * 4, n4, 4);
        this.setArgs(384 + n5 * 4, 10000, 4);
        this.setArgs(448 + n5 * 4, 10000, 4);
        this.setArgs(96, ++n5, 4);
    }

    public void kickEnepc(int n, int n2, int n3, int n4, int n5) {
        int n6 = this.getArgs(96, 4);
        if (n6 >= 16) {
            return;
        }
        this.setArgs(128 + n6 * 4, n, 4);
        this.setArgs(192 + n6 * 4, n2, 4);
        this.setArgs(256 + n6 * 4, n3, 4);
        this.setArgs(320 + n6 * 4, n4, 4);
        this.setArgs(384 + n6 * 4, n5, 4);
        this.setArgs(448 + n6 * 4, 10000, 4);
        this.setArgs(96, ++n6, 4);
    }

    public void kickEnepc(int n, int n2, int n3, int n4, int n5, int n6) {
        int n7 = this.getArgs(96, 4);
        if (n7 >= 16) {
            return;
        }
        this.setArgs(128 + n7 * 4, n, 4);
        this.setArgs(192 + n7 * 4, n2, 4);
        this.setArgs(256 + n7 * 4, n3, 4);
        this.setArgs(320 + n7 * 4, n4, 4);
        this.setArgs(384 + n7 * 4, n5, 4);
        this.setArgs(448 + n7 * 4, n6, 4);
        this.setArgs(96, ++n7, 4);
    }

    public void moveEnepc(int n, float f, float f2, int n2) {
        int n3 = this.getArgs(96, 4);
        if (n3 >= 16) {
            return;
        }
        this.setArgs(128 + n3 * 4, n, 4);
        this.setArgs(192 + n3 * 4, f);
        this.setArgs(256 + n3 * 4, f2);
        this.setArgs(320 + n3 * 4, n2, 4);
        this.setArgs(384 + n3 * 4, 10000, 4);
        this.setArgs(448 + n3 * 4, 10000, 4);
        this.setArgs(96, ++n3, 4);
    }

    public void setBatEvent(int n) {
        this.setArgs(108, n, 2);
    }

    public void setDefault(int n) {
        this.setArgs(100, n, 4);
    }

    public void setES_cRange(float f, float f2, float f3) {
        this.setArgs(16, f);
        this.setArgs(20, f2);
        this.setArgs(104, f3);
    }

    public void setES_mGiveUp(float f) {
        this.setArgs(4, f);
    }

    public void setES_mGiveUp(float f, float f2) {
        this.setArgs(8, f);
        this.setArgs(12, f2);
    }

    public void setES_mLength(float f) {
        this.setArgs(0, f);
    }

    public void setGroup(int n) {
        this.setArgs(80, n, 2);
        this.setArgs(82, -1, 2);
        this.setArgs(84, -1, 2);
        this.setArgs(86, -1, 2);
        this.setArgs(88, -1, 2);
        this.setArgs(90, -1, 2);
        this.setArgs(92, -1, 2);
        this.setArgs(94, -1, 2);
    }

    public void setGroup(int n, int n2, int n3, int n4) {
        this.setArgs(80, n, 2);
        this.setArgs(82, n2, 2);
        this.setArgs(84, n3, 2);
        this.setArgs(86, n4, 2);
        this.setArgs(88, -1, 2);
        this.setArgs(90, -1, 2);
        this.setArgs(92, -1, 2);
        this.setArgs(94, -1, 2);
    }

    public void setGroup(int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8) {
        this.setArgs(80, n, 2);
        this.setArgs(82, n2, 2);
        this.setArgs(84, n3, 2);
        this.setArgs(86, n4, 2);
        this.setArgs(88, n5, 2);
        this.setArgs(90, n6, 2);
        this.setArgs(92, n7, 2);
        this.setArgs(94, n8, 2);
    }

    public void setInvalidID(int n) {
        this.setArgs(34, n, 2);
    }

    public void setMotion(int n, int n2) {
        this.setArgs(112 + n * 2, n2, 2);
    }

    public void setParams(int n, float f, float f2, float f3, float f4, float f5, float f6, int n2, int n3, int n4, float f7) {
        this.setArgs(76, n, 4);
        this.setArgs(0, f);
        this.setArgs(8, f2);
        this.setArgs(12, f3);
        this.setArgs(16, f4);
        this.setArgs(20, f5);
        this.setArgs(36, f6);
        this.setArgs(40, n2, 2);
        this.setArgs(42, n3, 2);
        this.setArgs(68, n4, 4);
        this.setArgs(64, f7);
    }

    public void setParams(int n, int n2, int n3, int n4) {
        this.setArgs(40, n, 2);
        this.setArgs(44, n2, 4);
        this.setArgs(76, n3, 4);
        this.setArgs(68, n4, 4);
        this.setArgs(56, 0, 4);
        this.setArgs(60, 0, 4);
    }

    public void setParams(int n, int n2, int n3, int n4, float[] fArray) {
        this.setArgs(40, n, 2);
        this.setArgs(44, n2, 4);
        this.setArgs(76, n3, 4);
        this.setArgs(68, n4, 4);
        this.setArgs(56, fArray, 0);
    }

    public void setParams(float[] fArray) {
        this.setArgs(48, fArray, 0);
    }

    public void setTP(int n) {
        this.setArgs(110, n, 2);
    }

    public void setTogetherWith(int n, int n2, int n3, int n4) {
        this.setArgs(544, n, 2);
        this.setArgs(546, n2, 2);
        this.setArgs(548, n3, 2);
        this.setArgs(550, n4, 2);
    }
}

