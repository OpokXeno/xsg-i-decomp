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
import xeno.map.MC_VOK03_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01010
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_VOK03_PRJ,
        Pack01010,
        FLSshion_h,
        FLSvan {
    Characters shion;
    Characters van;
    Characters sold1;
    Characters sold2;
    Characters eng;
    Chr_units zohal;
    MAPUnit ring1;
    MAPUnit ring2;
    MAPUnit ring3;
    MAPUnit ring4;
    MAPUnit ring5;
    MAPUnit ring6;
    MAPUnit ring7;
    MAPUnit ring8;
    MAPUnit ring9;
    MAPUnit ringA;
    MAPUnit ringB;
    MAPUnit ringC;
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

    SCE01010() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(29);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.132f, 0.357f, -0.925f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, 0.938f, 0.016f, 0.347f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, -0.392f, -0.784f, -0.482f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam1.setTranslate(0.04f, 1.25f, 5.13f);
        this.cam1.setRotate(3.5f, 179.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.04f, 1.32f, 5.13f);
        this.waitCameraPlay(30);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(0.04f, 1.32f, 5.13f);
        this.cam1.setRotate(3.5f, 179.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(31);
        Runtime.setDefocusQuick(0, 1, 14880, 1);
        this.cam1.setTranslate(1.21f, 1.74f, 6.32f);
        this.cam1.setRotate(3.5f, -48.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(this.cut_length, 3.5f, -66.5f, 0.0f);
        this.waitCameraPlay(32);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.781f, 0.471f, -0.411f);
        this.light.setColor(2, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(2, -0.566f, 0.379f, 0.732f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, 0.36f, -0.6f, 0.714f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 19880, 1);
        this.cam1.setTranslate(-0.27f, 0.41f, 8.29f);
        this.cam1.setRotate(31.0f, -9.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(33);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.802f, 0.243f, -0.545f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.634f, 0.001f, -0.773f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.374f, -0.712f, -0.594f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 21880, 1);
        this.cam1.setTranslate(-0.42f, 0.17f, 6.09f);
        this.cam1.setRotate(9.0f, -150.5f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(70);
        this.rSPL(this.cut_length - 70, 9.0f, -170.5f, 0.0f, 3);
        this.waitCameraPlay(34);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.319f, 0.648f, -0.692f);
        this.light.setColor(2, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(2, 0.271f, 0.753f, -0.599f);
        this.light.setColor(3, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(3, -0.081f, -0.0f, 0.997f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 16880, 1);
        this.cam1.setTranslate(-3.66f, 7.07f, 4.96f);
        this.cam1.setRotate(-57.0f, -116.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(35);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, -0.799f, 0.001f, -0.601f);
        this.light.setColor(2, 0.41f, 0.41f, 0.41f);
        this.light.setDirection2(2, 0.183f, -0.457f, 0.871f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.138f, 0.813f, 0.565f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 25880, 1);
        this.cam1.setTranslate(-0.67f, 1.5f, 9.87f);
        this.cam1.setRotate(-3.0f, -22.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(165);
        this.rSPL(60, -3.0f, -43.5f, 0.0f, 3);
        this.waitCameraPlay(36);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(1, -0.406f, 0.015f, 0.914f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.664f, 0.706f, 0.247f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.276f, -0.613f, 0.74f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 11880, 1);
        this.cam1.setTranslate(0.41f, 0.58f, 20.39f);
        this.cam1.setRotate(3.0f, -2.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(37);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.8f, 0.8f, 0.8f);
        this.light.setDirection2(1, 0.494f, 0.249f, 0.833f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 29880, 1);
        this.cam1.setTranslate(1.79f, 1.08f, 11.96f);
        this.cam1.setRotate(0.0f, -189.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(38);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(1, 0.489f, 0.0f, 0.872f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, 0.738f, 0.41f, -0.537f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, 0.701f, -0.713f, -0.025f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 16880, 1);
        this.cam1.setTranslate(13.17f, 0.68f, 17.31f);
        this.cam1.setRotate(1.5f, -273.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(39);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.63f, 0.63f, 0.63f);
        this.light.setDirection2(1, 0.455f, 0.608f, 0.65f);
        this.light.setColor(2, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(2, -0.107f, 0.85f, -0.516f);
        this.light.setColor(3, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(3, 0.025f, -0.816f, -0.578f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(11.34f, 2.27f, 16.74f);
        this.cam1.setRotate(-59.5f, -145.5f, 0.0f);
        this.cam1.setFov(24.0f);
        this.tSPL(this.cut_length, 11.38f, 2.15f, 16.79f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
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

    void allRingsSlowdown() {
        this.ring1.start(1, "ringL_sd");
        this.ring2.start(1, "ringL_sd");
        this.ring3.start(1, "ringL_sd");
        this.ring4.start(1, "ringR_sd");
        this.ring5.start(1, "ringR_sd");
        this.ring6.start(1, "ringR_sd");
        this.ring7.start(1, "ringR_sd");
        this.ring8.start(1, "ringR_sd");
        this.ring9.start(1, "ringR_sd");
        this.ringA.start(1, "ringR_sd");
        this.ringB.start(1, "ringL_sd");
        this.ringC.start(1, "ringR_sd");
        this.zohal.mtn(282, 0, 45, 0, 0, 0.2f, true);
    }

    void allRingsStop() {
        this.ring1.start(1, "ringL_st");
        this.ring2.start(1, "ringL_st");
        this.ring3.start(1, "ringL_st");
        this.ring4.start(1, "ringR_st");
        this.ring5.start(1, "ringR_st");
        this.ring6.start(1, "ringR_st");
        this.ring7.start(1, "ringR_st");
        this.ring8.start(1, "ringR_st");
        this.ring9.start(1, "ringR_st");
        this.ringA.start(1, "ringR_st");
        this.ringB.start(1, "ringL_st");
        this.ringC.start(1, "ringR_st");
        this.zohal.mtn(282, 0, 0, 0, 0, 1.0f, true);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01010_F");
        Runtime.setFlags(17, 1, 1);
        System.println("XEVEJNAME:CFJ1_80 XEVEJPOINT:POINT_80");
        Runtime.jumpCF(30, 4);
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
        Runtime.setLocation(18);
        this.shion = new Characters(0x100001E, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shion.setShadow(5, 24);
        this.van = new Characters(16777525, 0.0f, 0.0f, 0.0f, 0.0f);
        this.van.setShadow(5, 24);
        this.sold1 = new Characters(526, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold2 = new Characters(526, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eng = new Characters(527, 0.0f, 0.0f, 0.0f, 0.0f);
        this.zohal = new Chr_units(20541, 0.0f, 2.84f, -1.61f, 0.0f);
        this.ring1 = new Mapunits(99);
        this.ring2 = new Mapunits(100);
        this.ring3 = new Mapunits(101);
        this.ring4 = new Mapunits(102);
        this.ring5 = new Mapunits(103);
        this.ring6 = new Mapunits(104);
        this.ring7 = new Mapunits(105);
        this.ring8 = new Mapunits(106);
        this.ring9 = new Mapunits(107);
        this.ringA = new Mapunits(115);
        this.ringB = new Mapunits(114);
        this.ringC = new Mapunits(113);
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.van.face, "FLSvan.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 100.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.van.setVisible(false);
        this.sold1.setVisible(false);
        this.sold2.setVisible(false);
        this.eng.setVisible(false);
        this.FlashIn = new Effect(0);
        this.FlashIn.args[0] = -2130706433;
        this.FlashIn.args[1] = 90;
        this.FlashIn.args[2] = 1;
        this.FlashOut = new Effect(0);
        this.FlashOut.args[0] = -2130706433;
        this.FlashOut.args[1] = 90;
        this.FlashOut.args[2] = 0;
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        this.shion.setMotNoUpdate(2);
        this.shion.mtn(269, 0, 0, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        System.sleep(5);
        Runtime.mpeg2("1010_1");
        Sound.streamPlay(1190088, 48000);
        this.cut_length = 120;
        Stage.setVisible(-1, false);
        this.shion.setTranslate(0.0f, 0.0f, 6.05f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.CameraPlay = 29;
        this.face(this.shion.face, 16);
        this.shion.mtn(269, 0, 1.0f, true);
        System.sleep(this.cut_length);
        this.cut_length = 42;
        Stage.setVisible(-1, true);
        this.zohal.setVisible(true);
        this.CameraPlay = 30;
        this.msg.print("Look out!!");
        this.waitclear(30);
        this.face(this.shion.face, 40, 0, 12, 8, 1, 1.0f);
        System.sleep(12);
        this.cut_length = 42;
        this.ring1.start(1, "c31");
        this.ring2.start(1, "c31");
        this.ring3.start(1, "c31");
        this.ring4.start(1, "ringR");
        this.ring5.start(1, "ringR");
        this.ring6.start(1, "ringR");
        this.ring7.start(1, "ringR");
        this.ring8.start(1, "ringR");
        this.ring9.start(1, "ringR");
        this.ringA.start(1, "ringR");
        this.ringB.start(1, "c31");
        this.ringC.start(1, "ringR");
        this.CameraPlay = 31;
        System.sleep(42);
        this.shion.setTranslate(0.0f, 0.0f, 6.3f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.ring1.start(1, "c32");
        this.ring2.start(1, "c32");
        this.ring3.start(1, "c32");
        this.ringB.start(1, "c32");
        this.CameraPlay = 32;
        this.shion.mtn(270, 0, 1.0f, true);
        System.sleep(30);
        this.cut_length = 164;
        this.ring1.start(1, "ringL");
        this.ring2.start(1, "ringL");
        this.ring3.start(1, "ringL");
        this.ringB.start(1, "ringL");
        this.van.setVisible(true);
        this.van.setTranslate(-3.6f, 0.0f, 15.4f);
        this.van.setRotate(0.0f, 0.0f, 0.0f);
        this.CameraPlay = 33;
        this.face(this.shion.face, 26);
        System.sleep(10);
        System.sleep(10);
        System.sleep(37);
        this.van.mtn(271, 0, 290, 0, 0, 1.0f, true);
        this.msg.print("Watch it, you slacker!!");
        this.face(this.van.face, 1, 0, 46, 0, 0, 1.0f);
        this.waitclear(62);
        System.sleep(45);
        this.shion.setTranslate(0.0f, 0.0f, 7.3f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.CameraPlay = 34;
        this.shion.mtn(272, 0, 183, 0, 0, 1.0f, true);
        this.msg.print("Only authorized personnel\nare allowed up here!");
        this.face(this.van.face, 1, 0, 91, 0, 0, 1.0f);
        this.waitclear(110);
        this.msg.print("I've had three people vanish\non me already!");
        this.face(this.van.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        this.van.setTranslate(0.3f, 0.0f, 8.6f);
        this.van.setRotate(0.0f, 180.0f, 0.0f);
        this.shion.setTranslate(0.0f, 0.0f, 7.3f);
        this.shion.setRotate(0.0f, 0.0f, 0.0f);
        this.CameraPlay = 35;
        this.face(this.van.face, 2);
        this.face(this.shion.face, 38);
        this.van.mtn(273, 0, 238, 0, 0, 1.0f, true);
        this.shion.mtn(274, 0, 238, 0, 0, 1.0f, true);
        this.msg.print("Get the hell out of here!");
        this.face(this.van.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("My men'll start slacking off if they\nsee bimbos like you around!");
        this.face(this.van.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(120);
        System.sleep(15);
        this.msg.print("I'm so sorry!!");
        this.face(this.shion.face, 37, 0, 30, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(30);
        this.eng.setVisible(true);
        this.eng.setTranslate(1.75f, 0.0f, 14.0f);
        this.eng.setRotate(0.0f, 0.0f, 0.0f);
        this.shion.setTranslate(0.35f, 0.0f, 13.65f);
        this.shion.setRotate(0.0f, 0.0f, 0.0f);
        this.van.setTranslate(0.25f, 0.0f, 9.45f);
        this.van.setRotate(0.0f, 0.0f, 0.0f);
        this.CameraPlay = 36;
        this.shion.mtn(275, 0, 136, 0, 0, 1.0f, true);
        this.eng.mtn(276, 0, 136, 0, 0, 1.0f, true);
        this.van.mtn(277, 0, 136, 0, 0, 1.0f, true);
        this.msg.print(" ");
        System.sleep(30);
        this.face(this.van.face, 1, 0, 46, 0, 0, 1.0f);
        this.waitclear(40);
        this.msg.print("Who's the idiot\nthat let civilians on this ship?!");
        this.face(this.van.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(68);
        this.sold1.setVisible(true);
        this.sold1.setTranslate(0.0f, 0.0f, 18.5f);
        this.sold1.setRotate(0.0f, -96.0f, 0.0f);
        this.eng.setTranslate(0.0f, 0.0f, 12.3f);
        this.eng.setRotate(0.0f, 0.0f, 0.0f);
        this.van.setTranslate(0.0f, 0.0f, 12.3f);
        this.van.setRotate(0.0f, 0.0f, 0.0f);
        this.van.setVisible(5, false);
        this.van.setVisible(7, true);
        this.CameraPlay = 37;
        this.van.mtn(278, 0, 1.0f, true);
        this.eng.mtn(279, 0, 1.0f, true);
        this.sold1.mtn(281, 8, 1.0f, false);
        this.sold1.move(120, -4.0f, 18.5f, true);
        System.sleep(15);
        this.msg.print("What the hell \nare you staring at?!");
        this.waitclear(104);
        System.sleep(15);
        this.shion.setTranslate(7.05f, 0.0f, 17.3f);
        this.shion.setRotate(0.0f, 90.0f, 0.0f);
        this.sold1.setTranslate(3.0f, 0.0f, 18.5f);
        this.sold1.setRotate(0.0f, -96.0f, 0.0f);
        this.CameraPlay = 38;
        this.shion.mtn(280, 0, 1.0f, true);
        this.sold1.mtn(281, 8, 1.0f, false);
        this.sold1.move(180, -3.0f, 18.5f, true);
        System.sleep(80);
        this.msg.print("Damn slacker!!");
        this.waitclear(60);
        System.sleep(40);
        this.cut_length = 240;
        this.CameraPlay = 39;
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
        System.println(">>>>>>>>>> CUT /[$1]");
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

        void c21_disappear() {
            float[] fArray = new float[4];
            this.setFilter(2);
            int n = 0;
            while (n < 60) {
                fArray[0] = 1.0f - 0.016666668f * (float) n;
                fArray[1] = 0.0f + 1.6666666f * (float) n;
                fArray[2] = 20.0f - 0.016666668f * (float) n;
                fArray[3] = 1.0f - 0.016666668f * (float) n;
                this.setFilter(2);
                this.setFilterParam(fArray);
                System.sleep(1);
                ++n;
            }
            this.setFilter(0);
            this.setVisible(false);
        }

        void shion_c22() {
            this.mtn(267, 0, 148, 0, 0, 1.0f, false);
            System.sleep(60);
            int n = 1;
            while (n <= 90) {
                this.getTranslate();
                this.setTranslate(this.px, this.py - 0.003f, this.pz);
                System.sleep(1);
                ++n;
            }
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
        }
    }

    class Units
            extends Unit {
        public Units(int n) {
            this.mapUnit(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(true);
        }

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
            this.setVisible(true);
        }

        void c31() {
            this.setRotateY(-10.0f);
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry - 1.0f);
                System.sleep(1);
            }
        }

        void c31r() {
            this.setRotate(0.0f, -10.0f, 0.0f);
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry - 1.0f);
                System.sleep(1);
            }
        }

        void c32() {
            this.setRotateY(-65.0f);
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry - 1.0f);
                System.sleep(1);
            }
        }

        void c32r() {
            this.setRotate(0.0f, -65.0f, 0.0f);
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry - 1.0f);
                System.sleep(1);
            }
        }

        void fogLoop() {
            int n = SCE01010.this.CameraPlay;
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                if (n == SCE01010.this.CameraPlay - 1) {
                    this.setRotate(0.0f, 0.0f, 0.0f);
                    n = SCE01010.this.CameraPlay;
                } else {
                    this.setRotate(this.rx, this.ry + 0.03f, this.rz);
                }
                System.sleep(1);
            }
        }

        void ringL() {
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry - 0.12f);
                System.sleep(1);
            }
        }

        void ringL_sd() {
            int n = 1;
            while (n <= SCE01010.this.cut_length) {
                this.getRotate();
                this.setRotateY(this.ry - (0.12f - 0.001f * (float) n));
                System.sleep(1);
                ++n;
            }
        }

        void ringL_st() {
            int n = 1;
            while (n <= SCE01010.this.cut_length) {
                this.getRotate();
                this.setRotateY(this.ry - (0.06f - 5.0E-4f * (float) n));
                if (0.06f - 5.0E-4f * (float) n <= 0.0f) break;
                System.sleep(1);
                ++n;
            }
        }

        void ringR() {
            while (true) {
                this.getRotate();
                this.setRotateY(this.ry + 0.12f);
                System.sleep(1);
            }
        }

        void ringR_sd() {
            int n = 1;
            while (n <= SCE01010.this.cut_length) {
                this.getRotate();
                this.setRotateY(this.ry + (0.12f - 0.001f * (float) n));
                System.sleep(1);
                ++n;
            }
        }

        void ringR_st() {
            int n = 1;
            while (n <= SCE01010.this.cut_length) {
                this.getRotate();
                this.setRotateY(this.ry + (0.06f - 5.0E-4f * (float) n));
                if (0.06f - 5.0E-4f * (float) n <= 0.0f) break;
                System.sleep(1);
                ++n;
            }
        }

        void ring_reset() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }
    }
}

