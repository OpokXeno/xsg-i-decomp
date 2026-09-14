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
import xeno.map.MC_VOK06_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01013
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01013,
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
    Unit fm1;
    MAPUnit doorL;
    MAPUnit doorR;
    MAPUnit doorL2;
    MAPUnit doorR2;
    MAPUnit futa;
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
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01013() {
    }

    void CameraThread() {
        this.cam1.change();
        this.DefocusClear();
        this.light.setColor(0, 0.54f, 0.54f, 0.54f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, -0.177f, 0.984f, -0.008f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, -0.923f, 0.0f, 0.384f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.431f, -0.86f, -0.273f);
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.94f, 0.34f, -0.04f);
        this.light.setColor(2, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(2, 0.031f, 0.411f, 0.911f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, -0.451f, -0.823f, -0.345f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 20880, 1);
        this.cam1.setTranslate(-10.72f, 3.12f, 2.04f);
        this.cam1.setRotate(-11.0f, 313.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -10.16f, 3.12f, 2.64f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.302f, 0.303f, 0.904f);
        this.light.setColor(2, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(2, -0.665f, 0.322f, -0.674f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.724f, -0.649f, 0.235f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(6.4f, 0.75f, 4.9f);
        this.cam1.setRotate(-1.0f, -84.05f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(75, -1.0f, -80.05f, 0.0f, 3);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(1, 0.179f, 0.239f, 0.954f);
        this.light.setColor(2, 0.48f, 0.48f, 0.48f);
        this.light.setDirection2(2, 0.34f, 0.319f, -0.885f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.269f, -0.514f, 0.815f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(7.7f, 0.61f, 6.74f);
        this.cam1.setRotate(4.5f, 38.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(90, 4.5f, 48.0f, 0.0f, 3);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, 0.8f, 0.526f, -0.29f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, -0.391f, 0.411f, 0.824f);
        this.light.setColor(3, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(3, 0.532f, -0.742f, 0.408f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 70880, 1);
        this.cam1.setTranslate(-4.51f, 1.56f, -1.58f);
        this.cam1.setRotate(3.3f, 73.77f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.538f, 0.334f, 0.774f);
        this.light.setColor(2, 0.62f, 0.62f, 0.62f);
        this.light.setDirection2(2, -0.42f, 0.339f, -0.842f);
        this.light.setColor(3, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(3, -0.495f, -0.813f, 0.306f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 12880, 1);
        this.cam1.setTranslate(-5.87f, 1.59f, -1.88f);
        this.cam1.setRotate(-4.2f, -429.31f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(135, -4.2f, -427.81f, 0.0f, 3);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.26f, 0.26f, 0.26f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.77f, 0.61f, 0.188f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, -0.438f, 0.583f, 0.684f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.175f, -0.648f, 0.741f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-0.23f, 3.33f, 5.14f);
        this.cam1.setRotate(-13.7f, -327.31f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.47f, 3.33f, 4.7f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.54f, 0.54f, 0.54f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, -0.177f, 0.984f, -0.008f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, -0.923f, 0.0f, 0.384f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.431f, -0.86f, -0.273f);
        this.cam1.setTranslate(-3.46f, 1.63f, -1.5f);
        this.cam1.setRotate(-10.7f, -197.31f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.26f, 0.26f, 0.26f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.632f, 0.415f, 0.655f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.028f, 0.611f, -0.791f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.747f, -0.641f, -0.176f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 38880, 1);
        this.cam1.setTranslate(-1.42f, 1.71f, -3.28f);
        this.cam1.setRotate(-4.2f, -604.31f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(25);
        this.rSPL(75, -4.2f, -583.81f, 0.0f, 3);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.26f, 0.26f, 0.26f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.238f, 0.778f, -0.581f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, -0.448f, 0.191f, 0.873f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, -0.828f, -0.52f, 0.21f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 58880, 1);
        Runtime.setDefocusQuick(1, 1, 14880, 1);
        this.cam1.setTranslate(-5.04f, 1.49f, -0.79f);
        this.cam1.setRotate(-4.7f, -466.81f, 0.0f);
        this.cam1.setFov(30.0f);
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
        System.println("XEVEFLAG:EV01013_F");
        Runtime.setFlags(20, 1, 1);
        System.println("XEVEJNAME:CFJ1_100 XEVEJPOINT:POINT_100");
        Runtime.jumpCF(60, 3);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(60, 3);
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
        this.shion = new Characters(0x100001E, 12.7f, -0.63f, 4.65f, -90.0f);
        this.tech = new Characters(0x1000210, -4.5f, 0.2f, -0.8f, 25.0f);
        this.real_m1 = new Real(16777753, -3.97f, 0.5f, -6.95f, 0.0f);
        this.real_m1.setRotate(-32.0f, 10.0f, 6.0f);
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
        this.futa = new Mapunits(134);
        this.futa.setTranslate(-1.56f, 1.18f, -1.06f);
        this.futa.setRotate(0.0f, 0.0f, 76.2f);
        this.doorL = new Mapunits(8);
        this.doorR = new Mapunits(6);
        this.doorL2 = new Mapunits(7);
        this.doorR2 = new Mapunits(5);
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
        this.real_m1.start(1, "init_idle_m");
        this.real_m2.start(1, "init_idle_m");
        this.real_m3.start(1, "init_idle_m");
        this.real_w1.start(1, "init_idle_w");
        this.real_w3.start(1, "init_idle_w");
        this.real_w5.start(1, "init_idle_w");
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.fm1 = new Units(24613, -3.9f, 1.4f, -0.15f, 180.0f);
        this.fm1.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm1.setArgs(1, 20092, 0, 0, 0);
        this.fm1.setArgs(2, 96, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        Stage.setVisible(154, false);
        Stage.setVisible(155, false);
        Stage.setVisible(156, false);
        Stage.setVisible(157, false);
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
        System.sleep(1);
        Sound.streamPlay(1190033, 48000);
        this.cut_length = 300;
        this.CameraPlay = 1;
        this.face(this.tech.face, 2);
        this.tech.mtn(257, 0, 1.0f, true);
        System.sleep(210);
        this.doorL.start(1, "openLcloseR");
        this.doorR.start(1, "closeLopenR");
        this.doorL2.start(1, "openLcloseR");
        this.doorR2.start(1, "closeLopenR");
        System.sleep(30);
        this.CameraPlay = 2;
        this.face(this.shion.face, 4, 0, 30, 0, 0, 1.0f);
        this.shion.start(1, "c2_3_2");
        System.sleep(30);
        this.msg.print("Hello! You rang?");
        this.face(this.shion.face, 3, 0, 21, 0, 0, 1.0f);
        System.sleep(30);
        this.doorL.start(1, "closeLopenR");
        this.doorR.start(1, "openLcloseR");
        this.face(this.shion.face, 3, 0, 17, 0, 0, 1.0f);
        this.waitclear(30);
        this.tech.setVisible(false);
        this.CameraPlay = 3;
        System.sleep(45);
        this.tech.setVisible(true);
        this.tech.setTranslate(-5.75f, 0.2f, -2.0f);
        this.tech.setRotateY(-20.0f);
        this.CameraPlay = 4;
        this.tech.mtn(259, 15, 133, 0, 0, 1.0f, true);
        this.msg.print("My apologies, Ms. Uzuki!");
        this.face(this.tech.face, 1, 0, 42, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("It seems we're constantly \nin need of your help.");
        this.face(this.tech.face, 1, 70, 120, 0, 0, 1.0f);
        this.waitclear(60);
        this.shion.setTranslate(0.7f, 0.2f, -2.69f);
        this.tech.setTranslate(-4.85f, 0.2f, -2.75f);
        this.tech.setRotateY(95.0f);
        this.CameraPlay = 5;
        this.tech.mtn(260, 0, 178, 0, 0, 1.0f, true);
        this.shion.mtn(261, 0, 178, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("No problem, Lieutenant.");
        this.face(this.shion.face, 3, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("I want everyone to be healthy\nand happy too, after all.");
        this.face(this.shion.face, 3, 0, 70, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.cut_length = 495;
        this.shion.setTranslate(-3.36f, 0.2f, -2.46f);
        this.tech.setTranslate(-5.05f, 0.2f, -2.14f);
        this.CameraPlay = 6;
        this.tech.mtn(262, 0, 435, 0, 0, 0.98f, true);
        this.shion.start(1, "c6_7");
        this.msg.print("So...what's the problem today?");
        this.face(this.shion.face, 3, 0, 70, 0, 0, 1.0f);
        this.waitclear(85);
        this.msg.print("Well...\nI'm trying to teach them some new");
        this.face(this.tech.face, 1, 0, 60, 0, 0, 1.0f);
        this.waitclear(65);
        this.msg.print("battle algorithms to\nbetter reflect the unit's reorg,");
        this.face(this.tech.face, 1, 0, 90, 0, 0, 1.0f);
        this.waitclear(110);
        this.msg.print("but the integration is not going\nvery smoothly.");
        this.face(this.tech.face, 1, 0, 60, 0, 0, 1.0f);
        this.waitclear(80);
        System.sleep(15);
        this.CameraPlay = 7;
        System.sleep(15);
        this.msg.print("You're right.\nHe's rejecting the data.");
        this.waitclear(105);
        System.sleep(15);
        this.cut_length = 100;
        this.CameraPlay = 8;
        this.tech.look_speed(100.0f);
        this.tech.look_char(this.shion);
        this.face(this.tech.face, 2);
        this.tech.mtn(262, 0, 435, 0, 0, 0.98f, true);
        this.shion.mtn(263, 315, 413, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Let's see...");
        this.face(this.shion.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(50);
        System.sleep(35);
        this.shion.setTranslate(-3.6000001f, 0.2f, -0.9f);
        this.shion.setRotate(0.0f, -22.0f, 0.0f);
        this.CameraPlay = 9;
        this.shion.mtn(264, 0, 1.0f, true);
        System.sleep(120);
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
            if (n == this.CameraPlay) break;
            System.sleep(1);
        }
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
            this.setShadow(9, 24);
        }

        void c2_3_2() {
            this.setTranslate(10.5f, -0.63f, 4.65f);
            this.mtn(258, 0, 1.0f, false);
            System.sleep(30);
            this.move(120, 6.5f, 4.65f, false);
            System.sleep(45);
            int n = 0;
            while (n < 75) {
                this.getTranslate();
                this.py += 0.0085f;
                this.setTranslate();
                System.sleep(1);
                ++n;
            }
            this.move(30, 5.5f, 4.65f, false);
        }

        void c6_7() {
            this.mtn(263, 0, 105, 0, 0, -1.0f, true);
            this.mtn(263, 0, 313, 0, 0, 1.0f, true);
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
            this.mtn(265, 8, 1.0f, true);
        }

        void init_idle_w() {
            this.mtn(266, 8, 1.0f, true);
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void monitor_on() {
            this.signal(1);
            int n = 0;
            while (n < 12) {
                this.setArgs(0, 0.0f, 0.0f, 0.0f + 0.074999996f * (float) n, 0.01f);
                System.sleep(1);
                ++n;
            }
            int n2 = 0;
            while (n2 < 8) {
                this.setArgs(0, 0.0f, 0.0f, 0.9f, 0.01f + 0.0625f * (float) n2);
                this.setArgs(2, 32 + 8 * n2, 0, 0, -1);
                System.sleep(1);
                ++n2;
            }
            this.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void closeLopenR() {
            this.getTranslate();
            int n = 0;
            while (n < 60) {
                this.setTranslate(this.px, this.py, this.pz + 0.033333335f);
                System.sleep(1);
                ++n;
            }
        }

        void openLcloseR() {
            this.getTranslate();
            int n = 0;
            while (n < 60) {
                this.setTranslate(this.px, this.py, this.pz - 0.033333335f);
                System.sleep(1);
                ++n;
            }
        }

        void rotate_sora() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.015f);
                System.sleep(1);
            }
        }
    }
}

