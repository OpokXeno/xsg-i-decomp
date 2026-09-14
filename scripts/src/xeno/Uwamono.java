package xeno;

import xeno.Effect;
import xeno.Enepc;
import xeno.Unit;
import xeno.plan.CfConstants;
import xeno.util.Toolkit;

public class Uwamono
        extends Unit
        implements CfConstants {
    public Uwamono() {
    }

    public Uwamono(int n, float f, float f2, float f3) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(0.0f);
        switch (n) {
            case 28678: {
                this.setArgs(0, 6, 2);
                Effect effect = new Effect(721, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28679: {
                this.setArgs(0, 7, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28733: {
                this.setArgs(0, 8, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28734: {
                this.setArgs(0, 9, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            default: {
                this.setArgs(0, 3, 2);
            }
        }
    }

    public Uwamono(int n, float f, float f2, float f3, float f4) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setArgs(0, 3, 2);
        switch (n) {
            case 28678: {
                this.setArgs(0, 6, 2);
                Effect effect = new Effect(721, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28679: {
                this.setArgs(0, 7, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28673: {
                Effect effect = new Effect(723, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28674: {
                Effect effect = new Effect(724, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28675: {
                Effect effect = new Effect(725, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28676: {
                Effect effect = new Effect(726, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
        }
        this.setArgs(36, 3.0f);
        this.setArgs(60, 90, 4);
    }

    public Uwamono(int n, float f, float f2, float f3, float f4, int n2) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        if (n == 28677 | n == 28680) {
            this.setArgs(0, 4, 2);
            Effect effect = new Effect(720, 0.0f, 0.0f, 0.0f, 0.0f);
            effect.setCaster(this);
            effect.noAttach(false);
        } else {
            this.setArgs(0, 5, 2);
        }
        this.setArgs(2, (short) n2, 2);
    }

    public Uwamono(int n, float f, float f2, float f3, float f4, Enepc enepc) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setArgs(0, 3, 2);
        int n2 = enepc.getSerial();
        this.setArgs(44, (char) n2, 1);
        this.setArgs(60, 90, 4);
        switch (n) {
            case 28673: {
                Effect effect = new Effect(723, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28674: {
                Effect effect = new Effect(724, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28675: {
                Effect effect = new Effect(725, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28676: {
                Effect effect = new Effect(726, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
        }
    }

    public Uwamono(int n, float f, float f2, float f3, float f4, Uwamono uwamono) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setArgs(0, 3, 2);
        int n2 = uwamono.getSerial();
        this.setArgs(5, (char) n2, 1);
    }

    public Uwamono(int n, float f, float f2, float f3, float f4, short s) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setTranslate(f, f2, f3);
        this.setRotateY(f4);
        this.setArgs(0, 3, 2);
        this.setArgs(2, s, 2);
        switch (n) {
            case 28678: {
                this.setArgs(0, 6, 2);
                Effect effect = new Effect(721, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28679: {
                this.setArgs(0, 7, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28733: {
                this.setArgs(0, 8, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28734: {
                this.setArgs(0, 9, 2);
                Effect effect = new Effect(722, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28673: {
                Effect effect = new Effect(723, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28674: {
                Effect effect = new Effect(724, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28675: {
                Effect effect = new Effect(725, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
            case 28676: {
                Effect effect = new Effect(726, 0.0f, 0.0f, 0.0f, 0.0f);
                effect.setCaster(this);
                effect.noAttach(false);
                break;
            }
        }
        this.setArgs(36, 3.0f);
        this.setArgs(60, 90, 4);
    }

    public Uwamono(int n, int n2, int n3, int n4) {
        int n5 = 2;
        if (n == 28691) {
            this.id = n4;
            Toolkit.getPeer(this);
            this.setArgs(0, 20, 2);
            this.setArgs(52, n2, 4);
            this.setArgs(56, n3, 4);
            this.setArgs(12, 0.1f);
            this.setArgs(16, 0.2f);
            this.setArgs(20, 0.1f);
            this.setArgs(24, 0.1f);
            this.setArgs(28, 0.2f);
            this.setArgs(32, 0.1f);
            this.setArgs(44, 5, 1);
            this.setArgs(5, 0, 1);
            this.setArgs(60, 0, 4);
            this.setArgs(51, 0, 1);
            this.setArgs(6, 30, 1);
            this.setArgs(7, 1, 1);
            this.setArgs(8, 0, 2);
            this.setArgs(50, 0, 1);
            this.setArgs(46, 0, 1);
            this.setArgs(36, 0.15f);
            this.setArgs(64, 0, 4);
            this.setArgs(10, 0, 1);
            this.setArgs(11, 0, 1);
            this.setArgs(80, -1, 4);
            this.setArgs(84, -1, 4);
            this.setArgs(92, 3600, 4);
            this.setArgs(88, n5, 4);
        }
    }

    public Uwamono(int n, short s) {
        this.id = n;
        Toolkit.getPeer(this);
        if (n < 4096) {
            this.setArgs(0, 2, 2);
        } else {
            this.setArgs(0, 3, 2);
        }
        this.setArgs(2, s, 2);
        this.setArgs(60, 8, 4);
    }

    public Uwamono(int n, short s, char c) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setArgs(0, 1, 2);
        this.setArgs(2, s, 2);
        this.setArgs(6, c, 1);
        this.setArgs(7, 1, 1);
        if (s == 40) {
            this.SetDoorSpd(16);
            this.setArgs(36, 1.5f);
            this.setArgs(52, 196709, 4);
            this.setArgs(56, 196710, 4);
        } else {
            this.SetDoorSpd(32);
            this.setArgs(36, 2.2f);
            this.setArgs(52, 196711, 4);
            this.setArgs(56, 196712, 4);
        }
    }

    public Uwamono(int n, short s, char c, Uwamono uwamono) {
        this.id = n;
        Toolkit.getPeer(this);
        this.setArgs(0, 1, 2);
        this.setArgs(2, s, 2);
        this.setArgs(6, c, 1);
        this.setArgs(7, 3, 1);
        if (s == 40 || s == 43) {
            uwamono.SetDoorSpd(8);
            uwamono.setArgs(2, 43, 2);
            this.SetDoorSpd(8);
            this.setArgs(2, 43, 2);
            this.setArgs(36, 1.5f);
            this.setArgs(52, 196709, 4);
            this.setArgs(56, 196710, 4);
        } else {
            uwamono.SetDoorSpd(16);
            uwamono.setArgs(2, 42, 2);
            this.SetDoorSpd(16);
            this.setArgs(2, 42, 2);
            this.setArgs(36, 2.2f);
            this.setArgs(52, 196711, 4);
            this.setArgs(56, 196712, 4);
        }
        int n2 = this.getSerial();
        uwamono.setArgs(5, (char) n2, 1);
    }

    public Uwamono(int n, short s, Enepc enepc) {
        this.id = n;
        Toolkit.getPeer(this);
        if (n < 4096) {
            this.setArgs(0, 2, 2);
        } else {
            this.setArgs(0, 3, 2);
        }
        this.setArgs(0, 2, 2);
        this.setArgs(2, s, 2);
        int n2 = enepc.getSerial();
        this.setArgs(44, (char) n2, 1);
        this.setArgs(60, 8, 4);
    }

    public Uwamono(int n, short s, Uwamono uwamono) {
        this.id = n;
        Toolkit.getPeer(this);
        if (n < 4096) {
            this.setArgs(0, 2, 2);
        } else {
            this.setArgs(0, 3, 2);
        }
        this.setArgs(0, 2, 2);
        this.setArgs(2, s, 2);
        int n2 = uwamono.getSerial();
        this.setArgs(5, (char) n2, 1);
        this.setArgs(60, 8, 4);
    }

    public void DoorClose() {
        this.setArgs(4, 0, 1);
    }

    public void DoorClose(int n) {
        this.setArgs(4, 0, 1);
        this.setArgs(8, (short) n, 2);
    }

    public void DoorOpen() {
        this.setArgs(4, 1, 1);
    }

    public void DoorOpen(int n) {
        this.setArgs(4, 1, 1);
        this.setArgs(8, (short) n, 2);
    }

    public int DrillCommand(int n) {
        switch (n) {
            case 1: {
                this.SendSignal();
                break;
            }
            case 2: {
                return this.getArgs(60, 4);
            }
            case 3: {
                this.setArgs(50, 1, 1);
                break;
            }
            case 4: {
                this.setArgs(46, 1, 1);
                break;
            }
            case 5: {
                this.setArgs(50, 0, 1);
                break;
            }
            case 6: {
                this.setArgs(50, 2, 1);
                break;
            }
            case 7: {
                return this.getArgs(64, 4);
            }
            case 8: {
                this.setArgs(10, 1, 1);
                break;
            }
            case 9: {
                this.setArgs(11, 1, 1);
                break;
            }
            case 10: {
                int n2 = this.getArgs(88, 4);
                this.setArgs(88, n2 |= 1, 4);
                break;
            }
            case 11: {
                int n3 = this.getArgs(88, 4);
                this.setArgs(88, n3 &= 0xFFFFFFFE, 4);
                break;
            }
            case 12: {
                int n4 = this.getArgs(88, 4);
                this.setArgs(88, n4 |= 2, 4);
                break;
            }
            case 13: {
                int n5 = this.getArgs(88, 4);
                this.setArgs(88, n5 &= 0xFFFFFFFD, 4);
                break;
            }
        }
        return 0;
    }

    public void DrillGameCount(int n) {
        this.setArgs(44, (char) n, 1);
    }

    public void DrillSetContainer(int n) {
        this.setArgs(6, (char) n, 1);
    }

    public void DrillSetContainerRandom(boolean bl) {
        if (bl) {
            this.setArgs(7, 1, 1);
        } else {
            this.setArgs(7, 0, 1);
        }
    }

    public void DrillSetDrillSize(float f) {
        this.setArgs(36, f);
    }

    public void DrillSetExParts(int n, int n2) {
        this.setArgs(80, n, 4);
        this.setArgs(84, n2, 4);
    }

    public void DrillSetReturnSpeed(float f, float f2, float f3) {
        this.setArgs(24, f);
        this.setArgs(28, f2);
        this.setArgs(32, f3);
    }

    public void DrillSetSpeed(float f, float f2, float f3) {
        this.setArgs(12, f);
        this.setArgs(16, f2);
        this.setArgs(20, f3);
    }

    public void DrillSetTimeLimit(int n) {
        this.setArgs(92, n, 4);
    }

    public void SendSignal() {
        this.setArgs(4, 1, 1);
    }

    public void SetBgm(int n) {
        this.setArgs(72, n, 4);
    }

    public void SetBgmType(char c) {
        this.setArgs(10, c, 1);
    }

    public void SetBroken(boolean bl) {
        int n = this.getArgs(68, 4);
        if (n == -1) {
            this.setArgs(68, 1, 4);
        }
        if (bl) {
            n = this.getArgs(68, 4);
            this.setArgs(68, n |= 1, 4);
        } else {
            n = this.getArgs(68, 4);
            this.setArgs(68, n &= 0xFFFFFFFE, 4);
        }
    }

    public void SetBrokenEnemy(boolean bl) {
        int n = this.getArgs(68, 4);
        if (n == -1) {
            this.setArgs(68, 1, 4);
        }
        if (bl) {
            n = this.getArgs(68, 4);
            this.setArgs(68, n |= 2, 4);
        } else {
            n = this.getArgs(68, 4);
            this.setArgs(68, n &= 0xFFFFFFFD, 4);
        }
    }

    public void SetCallNo(int n) {
        this.setArgs(48, (short) n, 2);
    }

    public void SetDiffSize(float f, float f2, float f3) {
        this.setArgs(24, f);
        this.setArgs(28, f2);
        this.setArgs(32, f3);
    }

    public void SetDoorRange(float f) {
        this.setArgs(40, f);
    }

    public void SetDoorScope(float f) {
        this.setArgs(36, f);
    }

    public void SetDoorSpd(int n) {
        this.setArgs(8, (short) n, 2);
    }

    public void SetDoorType(char c) {
        this.setArgs(0, 1, 2);
        this.setArgs(7, c, 1);
    }

    public void SetEffectTime(int n) {
        this.setArgs(60, n, 4);
    }

    public void SetEnemy(Enepc enepc) {
        int n = enepc.getSerial();
        this.setArgs(44, (char) n, 1);
    }

    public void SetGravity(boolean bl) {
        if (bl) {
            this.setArgs(46, 1, 1);
        } else {
            this.setArgs(46, 0, 1);
        }
    }

    public void SetHitKind(char c) {
        this.setArgs(45, c, 1);
    }

    public void SetItem(Uwamono uwamono) {
        int n = uwamono.getSerial();
        this.setArgs(5, (char) n, 1);
        if (uwamono.id == 28677 | uwamono.id == 28680) {
            uwamono.setArgs(0, 4, 2);
        } else {
            uwamono.setArgs(0, 5, 2);
        }
    }

    public void SetMaterial(char c) {
        this.setArgs(50, c, 1);
    }

    public void SetParticle(int n) {
        this.setArgs(64, n, 4);
    }

    public void SetSe0(int n) {
        this.setArgs(52, n, 4);
    }

    public void SetSe1(int n) {
        this.setArgs(56, n, 4);
    }

    public void SetShopNo(int n) {
        this.setArgs(2, n, 2);
    }

    public void SetSize(float f, float f2, float f3) {
        this.setArgs(12, f);
        this.setArgs(16, f2);
        this.setArgs(20, f3);
    }

    public void SetSymbol(int n) {
        Uwamono uwamono = new Uwamono();
        uwamono.id = n;
        uwamono.px = this.px;
        uwamono.py = this.py;
        uwamono.pz = this.pz;
        Toolkit.getPeer(uwamono);
        uwamono.setArgs(0, 10, 2);
        int n2 = uwamono.getSerial();
        this.setArgs(5, (char) n2, 1);
        int n3 = this.getArgs(2, 2);
        uwamono.setArgs(2, (short) n3, 2);
    }

    public void SetTrapScope(float f) {
        this.setArgs(36, f);
    }

    public void SetUwaType(short s) {
        this.setArgs(0, s, 2);
    }

    public void setElevatorMode(int n) {
        if (n == 0) {
            this.setArgs(47, 0, 1);
        } else {
            this.setArgs(47, 1, 1);
            this.setArgs(46, 1, 1);
        }
    }
}

