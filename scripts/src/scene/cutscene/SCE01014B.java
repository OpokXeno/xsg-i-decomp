import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.map.MC_VOK06_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01014B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01014B,
        MC_VOK06_PRJ,
        FLSshion_h,
        FLScommi,
        FLSreal_m,
        FLSreal_w {
    Thread thread1;
    Characters shion;
    Characters tech;
    Real real_m1;
    Real real_m2;
    Real real_m3;
    Real real_w1;
    Real real_w3;
    Real real_w5;
    MAPUnit doorL;
    MAPUnit doorR;
    MAPUnit doorL2;
    MAPUnit doorR2;
    MAPUnit sora;
    Effect light1;
    Effect light2;
    Effect light3;
    Effect light4;
    Effect light5;
    Effect light6;
    Effect light7;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int cut_length;
    Light light = new Light(0);
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01014B() {
    }

    void CameraThread() {
        this.cam1.change();
        this.DefocusClear();
        this.light.setColor(0, 0.66f, 0.66f, 0.66f);
        this.light.setColor(1, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(1, -0.386f, 0.847f, -0.366f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.482f, 0.753f, 0.448f);
        this.light.setColor(3, 0.0f, 0.0f, 0.0f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.76f, 0.76f, 0.76f);
        this.light.setDirection2(1, -0.573f, 0.415f, 0.707f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, 0.837f, 0.372f, 0.402f);
        this.light.setColor(3, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(3, 0.783f, -0.622f, -0.018f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 72880, 1);
        this.cam1.setTranslate(0.32f, 1.67f, -8.86f);
        this.cam1.setRotate(-2.57f, 370.03f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.81f, 0.81f, 0.81f);
        this.light.setDirection2(1, 0.766f, 0.187f, 0.615f);
        this.light.setColor(2, 0.86f, 0.86f, 0.86f);
        this.light.setDirection2(2, -0.27f, 0.694f, -0.668f);
        this.light.setColor(3, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(3, 0.488f, -0.655f, -0.577f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 15880, 1);
        this.cam1.setTranslate(1.01f, 1.57f, -12.04f);
        this.cam1.setRotate(-2.57f, 518.53f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(150, -2.57f, 511.53f, 0.0f, 3);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, 0.377f, 0.468f, 0.799f);
        this.light.setColor(2, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(2, 0.515f, 0.473f, -0.715f);
        this.light.setColor(3, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(3, 0.547f, -0.749f, 0.373f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 30880, 0);
        this.cam1.setTranslate(3.55f, 1.42f, -9.49f);
        this.cam1.setRotate(1.93f, 804.53f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.459f, 0.241f, 0.855f);
        this.light.setColor(2, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(2, 0.336f, 0.28f, -0.899f);
        this.light.setColor(3, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(3, 0.73f, -0.643f, 0.232f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 131880, 1);
        this.cam1.setTranslate(1.04f, 1.54f, -10.51f);
        this.cam1.setRotate(3.03f, 97.56f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.12f, 0.12f, 0.12f);
        this.light.setColor(1, 0.83f, 0.83f, 0.83f);
        this.light.setDirection2(1, -0.823f, 0.566f, 0.051f);
        this.light.setColor(2, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(2, 0.104f, 0.453f, -0.886f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, -0.503f, -0.731f, -0.46f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 113880, 1);
        this.cam1.setTranslate(0.9f, 1.64f, -10.02f);
        this.cam1.setRotate(-0.57f, 941.53f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(1, 0.459f, 0.241f, 0.855f);
        this.light.setColor(2, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(2, 0.336f, 0.281f, -0.899f);
        this.light.setColor(3, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(3, 0.73f, -0.643f, 0.232f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 46880, 1);
        this.cam1.setTranslate(1.03f, 1.57f, -10.61f);
        this.cam1.setRotate(2.53f, 103.56f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(255);
        this.tSPL(150, 0.5f, 1.52f, -9.94f, 3);
        this.rSPL(150, 1.53f, 161.06f, 0.0f, 3);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.12f, 0.12f, 0.12f);
        this.light.setColor(1, 0.83f, 0.83f, 0.83f);
        this.light.setDirection2(1, -0.823f, 0.566f, 0.051f);
        this.light.setColor(2, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(2, 0.104f, 0.453f, -0.886f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, -0.503f, -0.731f, -0.46f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 89880, 1);
        this.cam1.setTranslate(0.56f, 1.67f, -9.52f);
        this.cam1.setRotate(-1.07f, 964.03f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.12f, 0.12f, 0.12f);
        this.light.setColor(1, 0.78f, 0.78f, 0.78f);
        this.light.setDirection2(1, 0.752f, 0.276f, 0.599f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.032f, 0.337f, -0.941f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, 0.322f, -0.538f, -0.779f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 23880, 1);
        this.cam1.setTranslate(1.04f, 1.57f, -9.2f);
        this.cam1.setRotate(1.93f, 837.03f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.458f, 0.477f, 0.75f);
        this.light.setColor(2, 0.59f, 0.59f, 0.59f);
        this.light.setDirection2(2, 0.125f, 0.485f, -0.865f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.762f, -0.608f, -0.223f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 18880, 1);
        this.cam1.setTranslate(0.68f, 1.73f, -9.17f);
        this.cam1.setRotate(-6.57f, 857.03f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.68f, 1.81f, -9.17f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, -0.412f, 0.163f, 0.897f);
        this.light.setColor(2, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(2, 0.895f, 0.253f, 0.367f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, -0.004f, -0.538f, 0.843f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 2);
        this.cam1.setTranslate(0.13f, 1.57f, -8.21f);
        this.cam1.setRotate(-15.5f, 6.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, -0.409f, 0.41f, 0.815f);
        this.light.setColor(2, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(2, 0.95f, 0.312f, 0.006f);
        this.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(3, 0.344f, -0.504f, 0.792f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 117880, 1);
        this.cam1.setTranslate(0.31f, 1.87f, -7.88f);
        this.cam1.setRotate(-16.5f, 17.5f, 0.0f);
        this.cam1.setFov(25.0f);
        this.waitCameraPlay(12);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(1, 0.835f, 0.542f, 0.089f);
        this.light.setColor(2, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(2, -0.418f, 0.391f, -0.82f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, 0.195f, -0.742f, -0.642f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 9880, 1);
        this.cam1.setTranslate(2.62f, 1.23f, -11.18f);
        this.cam1.setRotate(1.5f, 139.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, 0.493f, 0.28f, 0.824f);
        this.light.setColor(2, 0.64f, 0.64f, 0.64f);
        this.light.setDirection2(2, -0.802f, 0.327f, 0.5f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, -0.193f, -0.472f, 0.86f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 15880, 1);
        this.cam1.setTranslate(-3.5f, 1.11f, -1.66f);
        this.cam1.setRotate(4.0f, -19.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -3.5f, 1.33f, -1.66f);
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
        System.println("XEVEFLAG:EV01014B_F");
        Runtime.setFlags(22, 1, 1);
        System.println("XEVEJNAME:CFJ1_120 XEVEJPOINT:POINT_120");
        Runtime.jumpCF(140, 2);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(140, 2);
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
        Runtime.setLocation(21);
        this.shion = new Characters(0x100001E, 0.0f, 0.2f, -9.47f, 180.0f);
        this.tech = new Characters(0x1000210, 0.0f, 0.2f, -5.9000006f, 180.0f);
        this.real_m1 = new Real(16777753, -3.2f, 0.2f, -4.0f, 90.0f);
        this.real_m1.setShadow(5, 16);
        this.real_m2 = new Real(16777753, -5.4f, 0.6f, -1.33f, 0.0f);
        this.real_m2.setRotate(-95.0f, 53.99f, 101.5f);
        this.real_m3 = new Real(16777996, -2.17f, 1.1f, -8.65f, 0.0f);
        this.real_m3.setRotate(-89.0f, 35.0f, 94.99f);
        this.real_w1 = new Real(16777758, 5.25f, 1.27f, 0.8f, 0.0f);
        this.real_w1.setRotate(-61.0f, 20.5f, 28.0f);
        this.real_w3 = new Real(16777999, 5.5f, 0.77f, -1.72f, 0.0f);
        this.real_w3.setRotate(56.0f, 131.5f, -46.0f);
        this.real_w5 = new Real(16777758, 5.92f, 0.77f, -8.1f, 0.0f);
        this.real_w5.setRotate(-46.5f, -64.0f, -44.5f);
        this.doorL = new Mapunits(12);
        this.doorR = new Mapunits(9);
        this.doorL2 = new Mapunits(11);
        this.doorR2 = new Mapunits(10);
        this.sora = new Mapunits(21);
        this.sora.start(1, "rotate_sora");
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.tech.face, "FLScommi.fpk");
        this.loadarc(this.real_m1.face, "FLSreal_m.fpk");
        this.loadarc(this.real_m2.face, "FLSreal_m.fpk");
        this.loadarc(this.real_m3.face, "FLSreal_m.fpk");
        this.loadarc(this.real_w1.face, "FLSreal_w.fpk");
        this.loadarc(this.real_w3.face, "FLSreal_w.fpk");
        this.loadarc(this.real_w5.face, "FLSreal_w.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.real_m2.start(1, "init_idle_m");
        this.real_m3.start(1, "init_idle_m");
        this.real_w1.start(1, "init_idle_w");
        this.real_w3.start(1, "init_idle_w");
        this.real_w5.start(1, "init_idle_w");
        this.shion.signal(0);
        this.tech.signal(0);
        this.real_m1.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.real_m1.setVisible(false);
        this.face(this.real_m1.face, 2, 47, 47, 0, 0, 1.0f);
        this.face(this.real_m2.face, 2, 47, 47, 0, 0, 1.0f);
        this.face(this.real_m3.face, 2, 47, 47, 0, 0, 1.0f);
        this.face(this.real_w1.face, 2, 69, 69, 0, 0, 1.0f);
        this.face(this.real_w3.face, 2, 69, 69, 0, 0, 1.0f);
        this.face(this.real_w5.face, 2, 69, 69, 0, 0, 1.0f);
        this.light1 = new Effect(1007, -4.12f, 2.84f, -7.9f, 0.0f);
        this.light1.setRotate(0.0f, 8.5f, 0.0f);
        this.light1.disp(false);
        this.light2 = new Effect(1007, -3.7f, 2.84f, -8.47f, 0.0f);
        this.light2.setRotate(0.0f, 99.0f, 0.0f);
        this.light2.disp(true);
        this.light3 = new Effect(1007, -6.55f, 2.84f, -1.07f, 0.0f);
        this.light3.setRotate(0.0f, 103.0f, 0.0f);
        this.light3.disp(true);
        this.light4 = new Effect(1007, 6.52f, 2.84f, -8.27f, 0.0f);
        this.light4.setRotate(0.0f, -74.5f, 0.0f);
        this.light4.disp(true);
        this.light5 = new Effect(1007, 3.9f, 2.84f, -1.32f, 0.0f);
        this.light5.setRotate(0.0f, 29.5f, 0.0f);
        this.light5.disp(true);
        this.light6 = new Effect(1007, 4.57f, 2.84f, -1.17f, 0.0f);
        this.light6.setRotate(0.0f, -57.5f, 0.0f);
        this.light6.disp(true);
        this.light7 = new Effect(1007, 4.4f, 2.84f, -0.5f, 0.0f);
        this.light7.setRotate(0.0f, 32.5f, 0.0f);
        this.light7.disp(true);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1190035, 48000);
        this.shion.setMotionFlags(0x40000000, true);
        this.shion.setMotionFlags(Integer.MIN_VALUE, true);
        this.CameraPlay = 1;
        this.face(this.shion.face, 2, 0, 120, 0, 0, 1.0f);
        this.shion.mtn(257, 0, 493, 0, 0, 1.0f, true);
        System.sleep(25);
        this.msg.print("My apologies, Ms. Uzuki.");
        this.face(this.tech.face, 1, 0, 75, 0, 0, 1.0f);
        this.waitclear(65);
        System.sleep(30);
        this.CameraPlay = 2;
        this.tech.mtn(258, 0, 1.0f, true);
        this.face(this.shion.face, 2);
        System.sleep(105);
        this.msg.print("He wasn't always like that,");
        this.face(this.tech.face, 1, 0, 75, 0, 0, 1.0f);
        this.waitclear(65);
        this.msg.print("but then...something...happened.");
        this.face(this.tech.face, 1, 0, 42, 0, 0, 1.0f);
        System.sleep(50);
        this.face(this.tech.face, 1, 0, 12, 0, 0, 1.0f);
        this.waitclear(40);
        System.sleep(20);
        this.msg.print("Is he an old...acquaintance?");
        this.face(this.shion.face, 21, 0, 90, 0, 0, 1.0f);
        this.waitclear(95);
        this.shion.setMotionFlags(Integer.MIN_VALUE, false);
        this.shion.setMotionFlags(0x40000000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.shion.setTranslate(0.0f, this.shion.py, -10.47f);
        this.CameraPlay = 3;
        this.face(this.shion.face, 2);
        this.shion.mtn(259, 0, 238, 0, 0, 1.0f, true);
        this.msg.print("We were classmates \nat the military academy.");
        this.face(this.tech.face, 1, 0, 75, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("And I've been stuck \nwith him since then.");
        this.face(this.tech.face, 1, 0, 52, 0, 0, 1.0f);
        System.sleep(52);
        this.face(this.tech.face, 2, 52, 90, 0, 0, 1.0f);
        this.waitclear(23);
        System.sleep(15);
        this.msg.print("...It was Miltia.");
        this.face(this.tech.face, 1, 0, 29, 0, 0, 1.0f);
        this.waitclear(60);
        this.CameraPlay = 4;
        this.shion.setMotionFlags(0x40000000, true);
        this.shion.mtn(260, 0, 150, 0, 0, 0.83f, true);
        this.msg.print("I see...\nSo...");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        System.sleep(50);
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(70);
        this.msg.print("...that's why.");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(60);
        this.shion.setTranslate(0.0f, this.shion.py, -10.47f);
        this.shion.setRotateY(150.0f);
        this.tech.setTranslate(2.2f, this.tech.py, -9.07f);
        this.tech.setRotateY(0.0f);
        this.CameraPlay = 5;
        this.tech.mtn(262, 0, 1.0f, true);
        this.msg.print("You knew about that, huh?");
        this.face(this.tech.face, 1, 0, 29, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Then again,");
        this.face(this.tech.face, 1, 0, 29, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("anyone planning to join \nthe Third Division would know.");
        this.face(this.tech.face, 1, 0, 75, 0, 0, 1.0f);
        this.waitclear(90);
        this.shion.setMotionFlags(Integer.MIN_VALUE, true);
        this.shion.setMotionFlags(0x40000000, true);
        this.shion.setRotateY(0.0f);
        this.CameraPlay = 6;
        this.shion.mtn(263, 0, 600, 0, 0, 1.0f, true);
        this.msg.print("Well, I...");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("I'm from Miltia.");
        this.face(this.shion.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Huh?");
        this.waitclear(45);
        this.msg.print("Of course, no one's allowed\nto go there anymore...");
        this.face(this.shion.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(120);
        this.msg.print("My family moved to Second Miltia\nwhen they transferred the capital\nafter the war...");
        this.face(this.shion.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.shion.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("And my brother still lives there, alone.");
        this.face(this.shion.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(90);
        this.shion.setMotionFlags(Integer.MIN_VALUE, false);
        this.shion.setMotionFlags(0x40000000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.shion.setTranslate(0.05f, 0.2f, -8.74f);
        this.shion.setRotateY(30.0f);
        this.tech.setTranslate(1.7f, this.tech.py, -9.07f);
        this.tech.setRotateY(-90.0f);
        this.CameraPlay = 7;
        this.tech.mtn(264, 0, 1.0f, true);
        this.msg.print("Oh, I see...");
        this.face(this.tech.face, 1, 0, 29, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("I'm sorry, I didn't mean to bring back\npainful memories.");
        this.face(this.tech.face, 1, 0, 29, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.tech.face, 1, 0, 42, 0, 0, 1.0f);
        this.waitclear(60);
        this.tech.setVisible(false);
        this.shion.setRotateY(-40.0f);
        this.CameraPlay = 8;
        this.shion.mtn(266, 0, 148, 0, 0, 0.83f, true);
        this.msg.print("Oh, it's all right.");
        this.face(this.shion.face, 1, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("After all, it's something we must\nnever forget about.");
        this.face(this.shion.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(120);
        this.cut_length = 150;
        this.shion.setRotateY(30.0f);
        this.real_m1.setVisible(true);
        this.CameraPlay = 9;
        this.shion.mtn(266, 150, 298, 0, 0, 1.0f, true);
        this.real_m1.mtn(267, 0, 1.0f, true);
        System.sleep(60);
        this.msg.print("For our sake, and theirs as well.");
        this.face(this.shion.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.shion.setRotateY(-15.0f);
        this.CameraPlay = 10;
        this.shion.mtn(268, 0, 88, 0, 0, 1.0f, true);
        System.sleep(90);
        this.CameraPlay = 11;
        this.shion.mtn(269, 0, 133, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Oh, no!\nI forgot I had to report to the bridge!");
        this.face(this.shion.face, 15, 0, 90, 0, 0, 1.0f);
        this.waitclear(120);
        this.tech.setVisible(true);
        this.real_m1.setRotateY(150.0f);
        this.CameraPlay = 12;
        this.shion.mtn(270, 0, 195, 0, 0, 1.0f, true);
        this.tech.mtn(273, 8, 1.0f, true);
        this.real_m1.mtn(272, 0, 1.0f, true);
        this.msg.print("Sorry,\nI'll come back to check on them later!");
        this.face(this.shion.face, 37, 0, 90, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("Bye!");
        System.sleep(30);
        this.doorL.start(1, "openLcloseR");
        this.doorL2.start(1, "openLcloseR");
        this.doorR.start(1, "closeLopenR");
        this.doorR2.start(1, "closeLopenR");
        this.waitclear(15);
        System.sleep(45);
        this.tech.look_speed(1000.0f);
        this.tech.look_point(0.0f, 2.0f, -14.0f);
        this.cut_length = 90;
        this.shion.setVisible(false);
        this.tech.setTranslate(1.0f, this.tech.py, -9.07f);
        this.tech.setRotateY(-120.0f);
        this.real_m1.setRotateY(120.0f);
        this.CameraPlay = 13;
        this.doorL.start(1, "closeLopenR");
        this.doorL2.start(1, "closeLopenR");
        this.doorR.start(1, "openLcloseR");
        this.doorR2.start(1, "openLcloseR");
        System.sleep(90);
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
        System.println(">>>>>>>>> CUT /[$1]");
        this.DefocusClear();
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Characters
            extends Chr {
        Chr face;

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(9, 32);
        }
    }

    class Real
            extends Chr {
        Chr face;

        public Real(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
        }

        void init_idle_m() {
            this.mtn(273, 8, 0.5f, true);
        }

        void init_idle_w() {
            this.mtn(274, 8, 0.5f, true);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.start(4, null);
        }

        void closeLopenR() {
            this.setRotate(0.0f, 0.0f, 0.0f);
            int n = 0;
            while (n < 60) {
                this.getTranslate();
                this.setTranslate(this.px + 0.033333335f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void openLcloseR() {
            this.setRotate(0.0f, 0.0f, 0.0f);
            int n = 0;
            while (n < 60) {
                this.getTranslate();
                this.setTranslate(this.px - 0.033333335f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void rotate_sora() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.01f);
                System.sleep(1);
            }
        }
    }
}

