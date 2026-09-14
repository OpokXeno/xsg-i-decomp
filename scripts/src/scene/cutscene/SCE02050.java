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
import xeno.map.MC_GNU02_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02050
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_GNU02_PRJ,
        Pack02050,
        FLSchaos,
        FLSshion_h,
        FLSkosmos,
        FLSmomo,
        FLSziggy,
        FLSelly {
    Characters chaos;
    Characters shion;
    Characters kosmos;
    Characters momo;
    Characters ziggy;
    Characters elly;
    Characters shion_l;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    int cut_length = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    Effect FlashIn;
    Effect FlashOut;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02050() {
    }

    void AllChrSPECOFF() {
        this.shion.renderCommand(18);
        this.ziggy.renderCommand(18);
        this.chaos.renderCommand(18);
        this.momo.renderCommand(18);
        this.kosmos.renderCommand(18);
    }

    void AllChrSPECON() {
        this.shion.renderCommand(0);
        this.ziggy.renderCommand(0);
        this.chaos.renderCommand(0);
        this.momo.renderCommand(0);
        this.kosmos.renderCommand(0);
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.978f, 0.135f, 0.159f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, -0.57f, 0.715f, -0.406f);
        this.light.setColor(3, 0.25f, 0.25f, 0.23f);
        this.light.setDirection2(3, 0.667f, -0.611f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 31880, 1);
        Runtime.setDefocusQuick(0, 1, 1880, 1);
        this.cam1.setTranslate(31.13f, 9.76f, 21.02f);
        this.cam1.setRotate(-8.5f, 385.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 27.55f, 9.76f, 22.69f);
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(1, 0.978f, 0.135f, 0.159f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, -0.57f, 0.714f, -0.406f);
        this.light.setColor(3, 0.22f, 0.22f, 0.2f);
        this.light.setDirection2(3, 0.667f, -0.611f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(3.07f, 2.86f, -9.6f);
        this.cam1.setRotate(6.0f, 442.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(1, 0.781f, 0.406f, 0.475f);
        this.light.setColor(2, 0.11f, 0.12f, 0.11f);
        this.light.setDirection2(2, -0.919f, 0.014f, 0.394f);
        this.light.setColor(3, 0.18f, 0.18f, 0.16f);
        this.light.setDirection2(3, 0.47f, -0.778f, 0.417f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 11880, 2);
        this.cam1.setTranslate(-2.81f, 4.79f, -2.71f);
        this.cam1.setRotate(-10.0f, 333.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -2.34f, 4.79f, -2.47f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.27f, 0.27f, 0.27f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, 0.56f, 0.131f, 0.818f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.496f, 0.001f, -0.868f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.027f, -0.999f, -0.035f);
        this.cam1.setTranslate(1.72f, 4.44f, -9.0f);
        this.cam1.setRotate(-12.5f, 328.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 150, 2.51f, 4.44f, -8.52f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.926f, 0.063f, 0.373f);
        this.light.setColor(2, 0.14f, 0.15f, 0.14f);
        this.light.setDirection2(2, -0.164f, 0.371f, -0.914f);
        this.light.setColor(3, 0.14f, 0.14f, 0.12f);
        this.light.setDirection2(3, 0.567f, -0.792f, -0.225f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 99880, 1);
        this.cam1.setTranslate(0.59f, 3.02f, -8.4f);
        this.cam1.setRotate(7.0f, 438.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(1, 0.887f, 0.461f, -0.019f);
        this.light.setColor(2, 0.17f, 0.18f, 0.17f);
        this.light.setDirection2(2, -0.857f, 0.39f, -0.337f);
        this.light.setColor(3, 0.14f, 0.14f, 0.12f);
        this.light.setDirection2(3, 0.62f, -0.61f, -0.493f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-0.34f, 3.24f, -10.53f);
        this.cam1.setRotate(7.0f, 544.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.27f, 0.27f, 0.27f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, 0.56f, 0.131f, 0.818f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.496f, 0.001f, -0.868f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.027f, -0.999f, -0.035f);
        Runtime.setDefocusQuick(0, 1, 3880, 1);
        this.cam1.setTranslate(35.78f, 3.75f, 2.02f);
        this.cam1.setRotate(9.5f, 746.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 90, 36.92f, 3.75f, 1.46f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.978f, 0.136f, 0.159f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, -0.57f, 0.715f, -0.406f);
        this.light.setColor(3, 0.25f, 0.25f, 0.23f);
        this.light.setDirection2(3, 0.667f, -0.611f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.71f, 3.23f, -10.65f);
        this.cam1.setRotate(11.0f, 151.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.71f, 3.29f, -10.65f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.985f, 0.175f, -0.013f);
        this.light.setColor(2, 0.22f, 0.23f, 0.22f);
        this.light.setDirection2(2, 0.68f, 0.001f, 0.734f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, 0.459f, -0.42f, -0.783f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.88f, 3.39f, -10.22f);
        this.cam1.setRotate(5.5f, 109.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.978f, 0.03f, -0.206f);
        this.light.setColor(2, 0.12f, 0.13f, 0.12f);
        this.light.setDirection2(2, 0.04f, 0.156f, -0.987f);
        this.light.setColor(3, 0.19f, 0.19f, 0.17f);
        this.light.setDirection2(3, 0.469f, -0.44f, 0.766f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 72880, 1);
        this.cam1.setTranslate(1.13f, 3.27f, -9.75f);
        this.cam1.setRotate(11.0f, 63.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.966f, 0.193f, 0.169f);
        this.light.setColor(2, 0.15f, 0.16f, 0.15f);
        this.light.setDirection2(2, -0.741f, 0.001f, -0.672f);
        this.light.setColor(3, 0.23f, 0.23f, 0.21f);
        this.light.setDirection2(3, 0.678f, -0.729f, -0.094f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 130380, 1);
        this.cam1.setTranslate(0.83f, 3.52f, -10.11f);
        this.cam1.setRotate(-8.5f, 98.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.74f, 3.52f, -10.09f);
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
        System.println("XEVEFLAG:EV02050_F");
        Runtime.setFlags(171, 1, 1);
        System.println("XEVEJNAME:SCE02051");
        Runtime.jumpEvent(2510);
    }

    void face(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void face(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
        chr.mtn(n, n2, n3, n4, n5, f, false);
        chr.start(4, null);
    }

    void faceRotate() {
    }

    void init() {
        Runtime.setLocation(1701);
        this.chaos = new Characters(0x1000003, -1.9f, 2.0f, -8.41f, 120.0f);
        this.shion = new Characters(0x100001E, -0.97f, 2.0f, -9.33f, 120.0f);
        this.shion_l = new Characters(0x1000001, 0.0f, 0.0f, 0.0f, 120.0f);
        this.shion_l.setVisible(false);
        this.kosmos = new Characters(0x1000002, -1.55f, 2.0f, -9.42f, 120.0f);
        this.momo = new Characters(0x1000004, -0.65f, 2.05f, -8.47f, 120.0f);
        this.ziggy = new Characters(0x1000006, -1.6f, 2.0f, -9.93f, 120.0f);
        this.elly = new Characters(0x1000112, 0.0f, 1.75f, -2.35f, 90.0f);
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.elly.face, "FLSelly.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.shion.signal(0);
        this.chaos.signal(0);
        this.kosmos.signal(0);
        this.momo.signal(0);
        this.ziggy.signal(0);
        this.elly.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 100.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.FlashIn = new Effect(0);
        this.FlashIn.args[0] = -2130706433;
        this.FlashIn.args[1] = 15;
        this.FlashIn.args[2] = 1;
        this.FlashOut = new Effect(0);
        this.FlashOut.args[0] = -2130706433;
        this.FlashOut.args[1] = 15;
        this.FlashOut.args[2] = 0;
        Runtime.setLocation(721);
        Runtime.setLocation(1701);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290044, 48000);
        this.AllChrSPECOFF();
        this.face(this.shion.face, 53, 0, 0, 0, 0, 1.0f);
        this.face(this.chaos.face, 6);
        this.face(this.ziggy.face, 2);
        this.cut_length = 435;
        this.chaos.setVisible(false);
        this.shion.setVisible(false);
        this.ziggy.setVisible(false);
        this.momo.setVisible(false);
        this.kosmos.setVisible(false);
        this.CameraPlay = 101;
        System.sleep(30);
        this.msg.print("So this is a Cathedral Ship...");
        this.waitclear(75);
        this.msg.print("It does feel like we're inside a ship,");
        this.waitclear(90);
        this.msg.print("but it certainly doesn't look\nlike anything that belongs\nto the Federation...");
        System.sleep(90);
        this.chaos.setVisible(true);
        this.shion.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.kosmos.setVisible(false);
        this.chaos.setTranslate(-3.9f, 2.0f, -6.41f);
        this.shion.setTranslate(-2.97f, 2.0f, -7.33f);
        this.kosmos.setTranslate(-3.55f, 2.0f, -7.42f);
        this.momo.setTranslate(-2.65f, 2.05f, -6.4700003f);
        this.ziggy.setTranslate(-3.6f, 2.0f, -7.9300003f);
        this.chaos.mtn(272, 8, 0.5f, false);
        this.shion.mtn(275, 8, 0.5f, false);
        this.ziggy.mtn(276, 8, 0.5f, false);
        this.momo.mtn(274, 8, 0.5f, false);
        this.kosmos.mtn(273, 8, 0.5f, false);
        this.chaos.move(149, -0.9f, -9.41f, true);
        this.shion.move(149, 0.029999971f, -10.33f, true);
        this.ziggy.move(149, -0.6f, -10.93f, true);
        this.momo.move(149, 0.35000002f, -9.47f, true);
        this.kosmos.move(149, -0.54999995f, -10.42f, true);
        this.waitclear(30);
        System.sleep(15);
        this.msg.print("Could it be an alien ship?");
        this.waitclear(75);
        System.sleep(15);
        this.msg.print("I wonder...");
        System.sleep(15);
        this.AllChrSPECON();
        this.shion.setVisible(true);
        this.chaos.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(false);
        this.kosmos.setVisible(false);
        this.chaos.setTranslate(-1.9f, 2.0f, -8.41f);
        this.shion.setTranslate(-0.97f, 2.0f, -9.33f);
        this.kosmos.setTranslate(-1.55f, 2.0f, -9.42f);
        this.momo.setTranslate(-0.65f, 2.05f, -8.47f);
        this.ziggy.setTranslate(-1.6f, 2.0f, -9.93f);
        this.shion.setRotateY(120.0f);
        this.chaos.setRotateY(120.0f);
        this.ziggy.setRotateY(120.0f);
        this.kosmos.setRotateY(120.0f);
        this.momo.setRotateY(120.0f);
        this.CameraPlay = 1;
        this.face(this.shion.face, 53, 0, 17, 0, 0, 1.0f);
        this.face(this.chaos.face, 6);
        this.face(this.ziggy.face, 2);
        this.shion.mtn(257, 0, 118, 0, 0, 1.0f, true);
        this.ziggy.mtn(258, 0, 118, 0, 0, 1.0f, true);
        this.chaos.mtn(259, 0, 118, 0, 0, 1.0f, true);
        this.waitclear(45);
        this.msg.print("Look...over there.");
        this.face(this.shion.face, 53, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(30);
        this.cut_length = 225;
        this.shion.setVisible(false);
        this.shion_l.setVisible(true);
        this.momo.setVisible(true);
        this.kosmos.setVisible(true);
        this.shion_l.setTranslate(0.3f, 2.0f, -10.0f);
        this.ziggy.setTranslate(-0.2f, 2.0f, -10.65f);
        this.chaos.setTranslate(-0.35f, 2.0f, -9.2f);
        this.CameraPlay = 2;
        this.shion_l.mtn(267, 8, 1.0f, true);
        this.ziggy.mtn(271, 8, 1.0f, true);
        this.chaos.mtn(269, 8, 1.0f, true);
        this.kosmos.mtn(266, 8, 1.0f, true);
        this.momo.mtn(270, 8, 1.0f, true);
        System.sleep(45);
        this.msg.print("What is it? A sign or something?");
        System.sleep(30);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("Hey, it's written in English.");
        this.waitclear(75);
        System.sleep(15);
        this.cut_length = 300;
        this.shion_l.setVisible(false);
        this.CameraPlay = 3;
        System.sleep(15);
        this.msg.print("But this is odd...");
        this.waitclear(60);
        this.msg.print("It doesn't look like it's\nstuck on the wall...");
        this.waitclear(60);
        this.msg.print("Doesn't it look more like\na part of the wall itself?");
        this.waitclear(135);
        System.sleep(15);
        this.chaos.setVisible(false);
        this.CameraPlay = 4;
        this.momo.mtn(260, 0, 165, 0, 0, 0.92f, true);
        this.msg.print("Are you saying this sign\nis part of the Gnosis?");
        this.face(this.momo.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(135);
        this.msg.print("How could that be...");
        this.face(this.momo.face, 1, 0, 34, 0, 0, 1.0f);
        this.waitclear(45);
        this.momo.setVisible(false);
        this.kosmos.setVisible(false);
        this.shion.setVisible(false);
        this.ziggy.setVisible(false);
        this.chaos.setVisible(true);
        this.CameraPlay = 5;
        this.chaos.mtn(261, 15, 75, 0, 0, 1.0f, true);
        this.msg.print("Take a look around...");
        this.face(this.chaos.face, 1, 0, 54, 0, 0, 1.0f);
        this.waitclear(60);
        this.shion.setVisible(true);
        this.shion.mtn(262, 0, 0, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.cut_length = 165;
        this.momo.setVisible(true);
        this.CameraPlay = 6;
        System.sleep(15);
        this.msg.print("That's not the only one.");
        this.waitclear(60);
        this.msg.print("There's a lot of other stuff here, too.");
        this.waitclear(60);
        this.face(this.shion.face, 18, 0, 15, 0, 0, 1.0f);
        System.sleep(30);
        this.cut_length = 150;
        this.shion.setVisible(true);
        this.shion.setTranslate(0.3f, 2.0f, -10.0f);
        this.chaos.setVisible(false);
        this.momo.setVisible(false);
        this.ziggy.setVisible(false);
        this.kosmos.setVisible(false);
        this.CameraPlay = 7;
        this.shion.mtn(262, 0, 1.0f, true);
        this.msg.print("Are we really inside a Gnosis...?");
        this.face(this.shion.face, 17, 0, 17, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.shion.face, 17, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.FlashOut.call(0);
        System.sleep(15);
        Runtime.setLocation(721);
        this.cut_length = 120;
        this.shion.setVisible(false);
        this.chaos.setVisible(false);
        this.ziggy.setVisible(false);
        this.kosmos.setVisible(false);
        this.momo.setVisible(false);
        this.light.setColor(0, 0.27f, 0.27f, 0.27f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, 0.56f, 0.131f, 0.818f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.496f, 0.001f, -0.868f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.027f, -0.999f, -0.035f);
        Runtime.setDefocusQuick(0, 1, 32880, 1);
        this.cam1.setTranslate(1.3000001f, 2.58f, -2.33f);
        this.cam1.setRotate(5.5f, 90.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 10.3f, 2.23f, -2.33f, 0);
        int[] nArray = new int[]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        Runtime.setDefocus(12, 9, nArray);
        this.FlashIn.call(0);
        this.elly.mtn(263, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Hurry! He'll be gone soon!");
        this.face(this.elly.face, 1, 0, 100, 0, 0, 1.0f);
        System.sleep(30);
        this.waitclear(60);
        this.FlashOut.call(0);
        System.sleep(15);
        Runtime.setLocation(1701);
        this.elly.setVisible(false);
        this.shion.setVisible(true);
        this.CameraPlay = 9;
        this.shion.mtn(264, 0, 313, 0, 0, 1.0f, true);
        this.face(this.shion.face, 40, 0, 90, 0, 0, 1.0f);
        this.FlashIn.call(0);
        System.sleep(15);
        System.sleep(75);
        this.chaos.setVisible(true);
        this.ziggy.setVisible(true);
        this.kosmos.setVisible(true);
        this.momo.setVisible(true);
        this.CameraPlay = 10;
        this.ziggy.mtn(265, 0, 1.0f, true);
        this.face(this.shion.face, 40, 0, 90, 0, 0, 1.0f);
        this.ziggy.look_speed(0.0f);
        this.ziggy.look_char(this.shion);
        System.sleep(15);
        this.msg.print("What, did you find something?");
        this.face(this.ziggy.face, 1, 0, 44, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("Huh...Uh, no, no, it's...it's nothing...");
        this.face(this.shion.face, 3, 0, 120, 8, 1, 1.0f);
        this.waitclear(120);
        System.sleep(15);
        this.cut_length = 225;
        this.chaos.setVisible(false);
        this.ziggy.setVisible(false);
        this.kosmos.setVisible(false);
        this.momo.setVisible(false);
        this.CameraPlay = 11;
        this.face(this.shion.face, 22, 0, 120, 8, 1, 1.0f);
        this.shion.mtn(264, 315, 480, 0, 0, 0.74f, true);
        System.sleep(45);
        this.msg.print("That girl again...?");
        this.waitclear(75);
        this.msg.print("What does she have to\ndo with this place?");
        System.sleep(60);
        this.face(this.shion.face, 22);
        this.waitclear(30);
        System.sleep(15);
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
        Chr face;

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
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

