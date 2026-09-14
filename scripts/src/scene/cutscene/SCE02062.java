import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_KAS39_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02062
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_KAS39_PRJ,
        Pack02062 {
    Chr durandal;
    Unit zohal;
    Unit pod1;
    Unit pod2;
    Characters space;
    Effect durfx;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    int cut_length = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02062() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.909f, 0.311f, -0.277f);
        this.light.setColor(2, 0.4f, 0.4f, 0.42f);
        this.light.setDirection2(2, -0.391f, 0.0f, -0.92f);
        this.light.setColor(3, 0.47f, 0.47f, 0.5f);
        this.light.setDirection2(3, 0.813f, -0.558f, 0.168f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-10.59f, -4.41f, 1.41f);
        this.cam1.setRotate(6.5f, -36.5f, 12.0f);
        this.cam1.setFov(55.0f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
    }

    void Xenvmainthreadmain() {
        while (true) {
            int n;
            if ((n = this.Xpad1P.getButton()) == 79) {
                this.Xenvmainthreadendflag = true;
            }
            System.sleep(1);
        }
    }

    void Xenvplaymain() {
        this.Xenvmainthread = Thread.create(this, "Xenvmainthreadmain");
        this.Xenvmainthread.start();
        this.Xenvplaythread = Thread.create(this, "Xenvplaythread");
        this.Xenvplaythread.start();
        while (!this.Xenvmainthreadendflag) {
            System.sleep(1);
        }
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02062_F");
        Runtime.setFlags(195, 1, 1);
        System.println("XEVEJNAME:SCE02063");
        Runtime.jumpEvent(2630);
    }

    void face(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void face(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
        chr.mtn(n, n2, n3, n4, n5, f, false);
        chr.start(4, null);
    }

    void init() {
        this.zohal = new Units(20541, -10.0f, -5.0f, 0.0f, 75.0f);
        this.zohal.setScale(0.1f, 0.1f, 0.1f);
        this.zohal.setRotate(-52.0f, -57.0f, -57.0f);
        this.durandal = new Characters(20484, 0.0f, 0.0f, -25.0f, 0.0f);
        this.durandal.setScale(10.0f, 10.0f, 10.0f);
        this.durandal.setRotate(0.0f, 0.0f, 15.0f);
        this.space = new Characters(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.setScale(0.1f, 0.1f, 0.1f);
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.durfx = new Effect(1459, 0.0f, 0.0f, 0.0f, 0.0f);
        this.durfx.setCaster(this.durandal);
        this.durfx.setMotion(true);
        this.durfx.noAttach(false);
        this.durfx.setScale(0.1f, 0.1f, 0.1f);
        this.durfx.disp(true);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290050, 48000);
        this.cut_length = 240;
        this.CameraPlay = 1;
        this.space.start(1, "rotate");
        this.zohal.start(1, "z_c1");
        this.durandal.start(1, "d_c1");
        System.sleep(this.cut_length);
    }

    void rSPL(int n, float f, float f2, float f3) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0);
    }

    void rSPL(int n, float f, float f2, float f3, int n2) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0, n2, n);
    }

    void s() {
        System.waitSignal(this.msg, 255);
    }

    void tSPL(int n, float f, float f2, float f3) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0);
    }

    void tSPL(int n, float f, float f2, float f3, int n2) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0, n2, n);
    }

    void waitCameraEnd(int n) {
        while (n != this.CameraEnd) {
            System.sleep(1);
        }
    }

    void waitCameraPlay(int n) {
        while (true) {
            if (n == this.CameraPlay) {
                ++this.cut;
                break;
            }
            System.sleep(1);
        }
        Runtime.setRegister(1, this.cut);
        System.println(">>>>>>>>>>> CUT /[$1]");
        this.DefocusClear();
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Characters
            extends Chr {
        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void d_c1() {
            this.mtn(278, 8, 0.3f, false);
            int n = 0;
            while (n < SCE02062.this.cut_length + 30) {
                this.setTranslate(this.px, this.py, this.pz + 15.0f / (float) SCE02062.this.cut_length);
                System.sleep(1);
                ++n;
            }
        }

        void rotate() {
            while (true) {
                this.getRotate();
                this.setRotate(this.rx, this.ry - 0.02f, this.rz);
                System.sleep(1);
            }
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void z_c1() {
            int n = 0;
            while (n < SCE02062.this.cut_length + 30) {
                this.setTranslate(this.px + 0.001f, this.py + 0.001f, this.pz);
                this.setRotate(this.rx + 0.02f, this.ry + 0.05f, this.rz);
                System.sleep(1);
                ++n;
            }
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }
    }
}

