import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Movie;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_DYU01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02031
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02031,
        MC_DYU01_PRJ,
        FLSjr_h,
        FLSshelley,
        FLSmary,
        FLShyaku {
    Characters jr;
    Characters jr2;
    Characters mary;
    Characters shelly;
    Characters ob_l;
    Characters ob_r;
    Characters op_l1;
    Characters op_l2;
    Characters op_l3;
    Characters op_r1;
    Characters op_r2;
    Characters op_r3;
    Characters op_upl1;
    Characters op_upl2;
    Characters op_upr1;
    Characters op_upr2;
    Chr agws;
    Chr cock;
    Chr durandal;
    Chr_units space;
    Chr_units space2;
    Unit fm1;
    Unit fm2;
    Unit fm3;
    Unit fm4;
    Unit fm5;
    Unit fm6;
    Unit fm7;
    Unit fm8;
    Unit fm9;
    Unit fm10;
    Unit fm11;
    Unit fm12;
    Unit fm_shelly;
    Unit fm_momo;
    Unit agwsView1;
    Unit jrmoni1;
    Unit jrmoni2;
    Unit jrmoni3;
    Unit c_moni1;
    Unit c_moni2;
    Unit c_moni3;
    Unit c_moni4;
    Unit c_moni5;
    Unit c_moni6;
    Unit wreck_s1;
    Unit wreck_s2;
    Unit wreck_m1;
    Unit wreck_m2;
    Unit wreck_m3;
    Unit wreck_l;
    Unit wreck_h;
    MAPUnit chair;
    MAPUnit support;
    MAPUnit elv;
    MAPUnit ring_1a;
    MAPUnit ring_1b;
    MAPUnit ring_1c;
    MAPUnit ring_2a;
    MAPUnit ring_2b;
    MAPUnit ring_2c;
    Thread thread1;
    Effect FadeIn;
    Effect FadeOut;
    Effect vx7000thruster;
    Effect vx7000thruster2;
    Effect vx7000sensor;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int cut_length;
    Light light = new Light(0);
    int elv_up;
    float stars_spd;
    Spline spline = Spline.create();
    Movie movie;
    int cut = 0;
    static final int toSpace = 0;
    static final int toBridge = 1;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02031() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(23);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.051f, 0.51f, -0.859f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.772f, 0.475f, 0.422f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.222f, -0.63f, -0.744f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 74880, 1);
        this.cam1.setTranslate(1.57f, 2.39f, 6.5f);
        this.cam1.setRotate(1.5f, 509.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(24);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.731f, 0.341f, -0.591f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.512f, 0.489f, 0.706f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.43f, -0.681f, 0.593f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 47880, 1);
        this.cam1.setTranslate(-1.45f, 0.64f, 17.57f);
        this.cam1.setRotate(-2.0f, 314.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(90);
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[1] = -2.0f;
        fArray[2] = 314.0f;
        fArray[4] = 10.0f;
        fArray[5] = 2.0f;
        fArray[6] = 330.0f;
        float[] fArray2 = fArray;
        this.cam1.rotateSPL(fArray2, 0, 3, 60);
        this.waitCameraPlay(25);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.91f, 0.247f, -0.333f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.277f, 0.476f, 0.835f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.249f, -0.393f, 0.885f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 66880, 1);
        this.cam1.setTranslate(0.77f, 2.06f, 9.19f);
        this.cam1.setRotate(9.0f, 703.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(26);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.385f, 0.449f, -0.807f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.652f, 0.712f, 0.261f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.267f, -0.923f, -0.277f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.42f, 2.73f, 6.25f);
        this.cam1.setRotate(-17.5f, 549.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(27);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.731f, 0.565f, -0.382f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.56f, 0.522f, 0.644f);
        this.light.setColor(3, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(3, -0.391f, -0.599f, 0.699f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 41880, 1);
        this.cam1.setTranslate(-0.16f, 2.17f, 10.94f);
        this.cam1.setRotate(-2.22f, -13.78f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(28);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.152f, 0.265f, -0.952f);
        this.light.setColor(2, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(2, 0.35f, 0.389f, 0.852f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, -0.173f, -0.722f, 0.67f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 103880, 1);
        this.cam1.setTranslate(1.07f, 2.28f, 7.75f);
        this.cam1.setRotate(-9.22f, 27.22f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(29);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.101f, 0.198f, -0.975f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, -0.754f, 0.524f, 0.396f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.4f, -0.916f, -0.042f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 73880, 1);
        Runtime.setDefocusQuick(1, 2, 113880, 2);
        this.cam1.setTranslate(-0.58f, 2.2f, 6.57f);
        this.cam1.setRotate(3.78f, -117.28f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(30);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.307f, 0.543f, -0.782f);
        this.light.setColor(2, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(2, 0.541f, 0.312f, 0.781f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.173f, -0.722f, 0.67f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 55880, 1);
        Runtime.setDefocusQuick(1, 2, 89880, 2);
        this.cam1.setTranslate(1.51f, 2.59f, 8.48f);
        this.cam1.setRotate(-12.72f, 30.22f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(31);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.3f, 0.504f, -0.81f);
        this.light.setColor(2, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(2, -0.187f, 0.527f, 0.829f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, -0.856f, -0.513f, 0.059f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 56880, 1);
        this.cam1.setTranslate(-1.55f, 2.39f, 8.14f);
        this.cam1.setRotate(-8.22f, 302.72f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(32);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, -0.311f, 0.504f, -0.806f);
        this.light.setColor(2, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(2, -0.165f, 0.162f, 0.973f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, -0.856f, -0.513f, 0.059f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 87880, 1);
        this.cam1.setTranslate(-0.46f, 2.57f, 7.66f);
        this.cam1.setRotate(-22.72f, 317.72f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(132);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.745f, 0.313f, -0.589f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.902f, 0.338f, -0.267f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.194f, -0.654f, -0.731f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 104380, 1);
        this.cam1.setTranslate(-3.25f, -1.13f, 1.42f);
        this.cam1.setRotate(-10.72f, -148.78f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(33);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.3f, 0.607f, -0.736f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.883f, 0.469f, 0.009f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.64f, -0.663f, -0.389f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 105880, 1);
        this.cam1.setTranslate(0.19f, 2.44f, 6.94f);
        this.cam1.setRotate(-2.22f, 241.72f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(35);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.184f, 0.26f, -0.948f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, 0.547f, 0.522f, 0.655f);
        this.light.setColor(3, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(3, -0.385f, -0.393f, 0.835f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 106880, 1);
        this.cam1.setTranslate(0.71f, 2.54f, 7.73f);
        this.cam1.setRotate(-22.72f, 380.22f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(36);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.326f, 0.654f, -0.683f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.837f, 0.0f, 0.547f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.512f, -0.676f, -0.53f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 47380, 1);
        this.cam1.setTranslate(-2.02f, 2.21f, 7.32f);
        this.cam1.setRotate(-3.72f, 269.22f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(37);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.326f, 0.654f, -0.683f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.837f, 0.0f, 0.547f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.512f, -0.676f, -0.53f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 129880, 1);
        this.cam1.setTranslate(-0.3f, 1.89f, 6.28f);
        this.cam1.setRotate(16.28f, -131.78f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -0.3f, 1.98f, 6.28f);
        this.waitCameraPlay(38);
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, 0.966f, 0.0f, -0.257f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.817f, 0.576f, 0.027f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, -0.027f, -0.974f, -0.223f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.56f, -0.35f, 3.66f);
        this.cam1.setRotate(5.5f, 16.38f, 0.0f);
        this.cam1.setFov(50.0f);
        this.tSPL(this.cut_length, 0.61f, -0.35f, 3.03f);
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

    void changeLocation(int n) {
        if (n == 0) {
            this.space.start(1, "stars_inSpaceR");
            this.mary.setVisible(false);
            this.shelly.setVisible(false);
            this.op_upl1.setVisible(false);
            this.op_upl2.setVisible(false);
            this.op_upr1.setVisible(false);
            this.op_upr2.setVisible(false);
            this.op_l3.setVisible(false);
            Stage.setVisible(-1, false);
            Stage.setVisible(0, false);
            this.fm1.setArgs(2, 0, 0, 15, -1);
            this.fm2.setArgs(2, 0, 0, 15, -1);
            this.fm3.setArgs(2, 0, 0, 15, -1);
            this.fm4.setArgs(2, 0, 0, 15, -1);
            this.fm5.setArgs(2, 0, 0, 15, -1);
            this.fm6.setArgs(2, 0, 0, 15, -1);
            this.fm7.setArgs(2, 0, 0, 15, -1);
            this.fm8.setArgs(2, 0, 0, 15, -1);
            this.fm_momo.setArgs(2, 0, 0, 15, -1);
            this.fm_shelly.setArgs(2, 0, 0, 15, -1);
        }
        if (n == 1) {
            this.space.start(1, "stars_inBridge");
            this.mary.setVisible(true);
            this.shelly.setVisible(true);
            this.op_upl1.setVisible(true);
            this.op_upl2.setVisible(true);
            this.op_upr1.setVisible(true);
            this.op_upr2.setVisible(true);
            this.op_l3.setVisible(true);
            Stage.setVisible(-1, true);
            Stage.setVisible(0, false);
            Stage.setVisible(140, false);
            Stage.setVisible(139, false);
            Stage.setVisible(132, false);
            Stage.setVisible(137, false);
            Stage.setVisible(138, false);
            Stage.setVisible(139, false);
            Stage.setVisible(140, false);
            Stage.setVisible(126, false);
            Stage.setVisible(127, false);
            Stage.setVisible(128, false);
            Stage.setVisible(129, false);
            Stage.setVisible(130, false);
            Stage.setVisible(131, false);
            this.fm1.setArgs(2, 96, 0, 15, -1);
            this.fm2.setArgs(2, 96, 0, 15, -1);
            this.fm3.setArgs(2, 96, 0, 15, -1);
            this.fm4.setArgs(2, 96, 0, 15, -1);
            this.fm5.setArgs(2, 96, 0, 15, -1);
            this.fm6.setArgs(2, 96, 0, 15, -1);
            this.fm7.setArgs(2, 96, 0, 15, -1);
            this.fm8.setArgs(2, 96, 0, 15, -1);
            this.fm_momo.setArgs(2, 96, 0, 15, -1);
            this.fm_shelly.setArgs(2, 96, 0, 15, -1);
        }
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02031_F");
        Runtime.setFlags(144, 1, 1);
        System.println("XEVEJNAME:SCE02032");
        Runtime.jumpEvent(2320);
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
        Runtime.setLocation(800);
        this.jr2 = new Characters(0x1000022, 0.0f, 0.0f, 0.0f, 0.0f);
        this.mary = new Characters(0x1000120, 1.05f, 1.0f, 7.55f, 185.0f);
        this.shelly = new Characters(0x1000121, -0.3f, 1.05f, 7.55f, -90.0f);
        this.op_upl1 = new Characters(16778242, -5.25f, 0.1f, 13.75f, -135.0f);
        this.op_upl2 = new Characters(0x1000401, -5.25f, 0.1f, 16.75f, -135.0f);
        this.op_upr1 = new Characters(16778242, 5.25f, 0.1f, 13.75f, 135.0f);
        this.op_upr2 = new Characters(16778243, 5.25f, 0.1f, 16.75f, 135.0f);
        this.op_l3 = new Characters(0x1000404, -2.6f, -1.8f, 2.4f, -135.0f);
        this.space = new Chr_units(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.setScale(0.1f, 0.1f, 0.1f);
        this.space.setVisible(false);
        this.chair = new Mapunits(73);
        this.chair.setPivot(-0.493f, 1.453f, 7.739f);
        this.chair.setTranslate(0.0f, 0.0f, -0.2f);
        this.chair.setRotate(0.0f, 90.0f, 0.0f);
        this.support = new Mapunits(74);
        this.support.setTranslate(0.0f, 0.0f, -0.2f);
        this.elv = new Mapunits(89);
        this.loadarc(this.jr2.face, "FLSjr_h.fpk");
        this.loadarc(this.shelly.face, "FLSshelley.fpk");
        this.loadarc(this.mary.face, "FLSmary.fpk");
        this.loadarc(this.op_l3.face, "FLShyaku.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.op_upl1.start(1, "initMTN_upl1");
        this.op_upl2.start(1, "initMTN_upl2");
        this.op_upr1.start(1, "initMTN_upr1");
        this.op_upr2.start(1, "initMTN_upr2");
        this.op_l3.start(1, "initMTN_op_l3");
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 500.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.jr2.setVisible(false);
        Stage.setVisible(0, false);
        Stage.setVisible(137, false);
        Stage.setVisible(138, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(142, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        Stage.setVisible(128, false);
        Stage.setVisible(129, false);
        Stage.setVisible(130, false);
        Stage.setVisible(131, false);
        Stage.setVisible(132, false);
        this.fm1 = new Units(24613, -3.41f, -1.8f, -4.45f, 45.0f);
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm1.setArgs(1, 28060, 0, 0, 0);
        this.fm1.setArgs(2, 96, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm2 = new Units(24613, -3.41f, -1.8f, -1.45f, 45.0f);
        this.fm2.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm2.setArgs(1, 28060, 0, 0, 0);
        this.fm2.setArgs(2, 96, 0, 15, -1);
        this.fm2.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm3 = new Units(24613, -3.41f, -1.8f, 1.58f, 45.0f);
        this.fm3.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm3.setArgs(1, 28060, 0, 0, 0);
        this.fm3.setArgs(2, 96, 0, 15, -1);
        this.fm3.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm4 = new Units(24613, 3.41f, -1.8f, -4.45f, -45.0f);
        this.fm4.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm4.setArgs(1, 28060, 0, 0, 0);
        this.fm4.setArgs(2, 96, 0, 15, -1);
        this.fm4.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm5 = new Units(24613, 3.41f, -1.8f, -1.45f, -45.0f);
        this.fm5.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm5.setArgs(1, 28060, 0, 0, 0);
        this.fm5.setArgs(2, 96, 0, 15, -1);
        this.fm5.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm6 = new Units(24613, 3.41f, -1.8f, 1.58f, -45.0f);
        this.fm6.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm6.setArgs(1, 28060, 0, 0, 0);
        this.fm6.setArgs(2, 96, 0, 15, -1);
        this.fm6.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm7 = new Units(24613, -5.6f, -1.46f, 4.6f, 0.0f);
        this.fm7.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm7.setArgs(1, 28059, 0, 0, 0);
        this.fm7.setArgs(2, 96, 0, 15, -1);
        this.fm7.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm8 = new Units(24613, 5.6f, -1.46f, 4.6f, 0.0f);
        this.fm8.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm8.setArgs(1, 28059, 0, 0, 0);
        this.fm8.setArgs(2, 96, 0, 15, -1);
        this.fm8.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm9 = new Units(24613, -6.37f, 1.45f, 12.63f, 45.0f);
        this.fm9.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm9.setArgs(1, 28060, 0, 0, 0);
        this.fm9.setArgs(2, 96, 0, 15, -1);
        this.fm9.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm10 = new Units(24613, -6.37f, 1.45f, 15.63f, 45.0f);
        this.fm10.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm10.setArgs(1, 28060, 0, 0, 0);
        this.fm10.setArgs(2, 96, 0, 15, -1);
        this.fm10.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm11 = new Units(24613, 6.37f, 1.45f, 12.63f, -45.0f);
        this.fm11.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm11.setArgs(1, 28060, 0, 0, 0);
        this.fm11.setArgs(2, 96, 0, 15, -1);
        this.fm11.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm12 = new Units(24613, 6.37f, 1.45f, 15.63f, -45.0f);
        this.fm12.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm12.setArgs(1, 28060, 0, 0, 0);
        this.fm12.setArgs(2, 96, 0, 15, -1);
        this.fm12.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm_momo = new Units(24613, 0.0f, 1.3f, 1.3f, 0.0f);
        this.fm_momo.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm_momo.setArgs(1, 28055, 0, 0, 0);
        this.fm_momo.setArgs(2, 96, 0, 15, -1);
        this.fm_momo.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm_shelly = new Units(24613, -1.6f, 2.38f, 7.5f, 90.0f);
        this.fm_shelly.setArgs(0, 0.0f, 0.0f, 1.5f, 0.61f);
        this.fm_shelly.setArgs(1, 28061, 0, 0, 0);
        this.fm_shelly.setArgs(2, 96, 0, 15, -1);
        this.fm_shelly.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm2.signal(1);
        this.fm3.signal(1);
        this.fm4.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.fm7.signal(1);
        this.fm8.signal(1);
        this.fm9.signal(1);
        this.fm10.signal(1);
        this.fm11.signal(1);
        this.fm12.signal(1);
        this.fm_momo.signal(1);
        this.fm_shelly.signal(1);
        this.FadeIn = new Effect(0);
        this.FadeIn.args[0] = Integer.MIN_VALUE;
        this.FadeIn.args[1] = 30;
        this.FadeIn.args[2] = 1;
        this.FadeOut = new Effect(0);
        this.FadeOut.args[0] = Integer.MIN_VALUE;
        this.FadeOut.args[1] = 30;
        this.FadeOut.args[2] = 0;
        this.ring_1a = new Mapunits(120);
        this.ring_1b = new Mapunits(121);
        this.ring_1c = new Mapunits(122);
        this.ring_2a = new Mapunits(123);
        this.ring_2b = new Mapunits(124);
        this.ring_2c = new Mapunits(125);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Runtime.mpeg2("2031_1");
        System.sleep(1);
        Sound.streamPlay(1290100, 48000);
        this.space.setVisible(true);
        this.space.start(1, "stars_inBridge");
        this.op_upl1.setVisible(true);
        this.op_upl2.setVisible(true);
        this.op_upr1.setVisible(true);
        this.op_upr2.setVisible(true);
        this.op_l3.setVisible(false);
        this.CameraPlay = 23;
        this.mary.mtn(277, 0, 1.0f, true);
        this.shelly.mtn(266, 0, 0.6f, true);
        System.sleep(15);
        this.msg.print("Let's go! Keep up\nthe pace, everyone!");
        this.face(this.mary.face, 1, 0, 18, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.mary.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("We've still got work to do!");
        this.face(this.mary.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.jr2.setVisible(1, false);
        this.cut_length = 150;
        this.elv_up = 75;
        this.elv.setTranslate(0.0f, -1.5f, 0.0f);
        this.jr2.setVisible(true);
        this.CameraPlay = 24;
        this.elv.start(1, "e_act1");
        this.jr2.start(1, "mtn22");
        System.sleep(this.cut_length);
        this.jr2.setVisible(1, true);
        this.mary.setTranslate(0.9f, 1.0f, 7.55f);
        this.mary.setRotateY(210.0f);
        this.CameraPlay = 25;
        this.mary.mtn(279, 0, 1.0f, true);
        System.sleep(15);
        System.sleep(15);
        this.msg.print("Hey, welcome back,\nLittle Master.");
        this.face(this.mary.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(60);
        this.jr2.setShadow(0, 0);
        this.cut_length = 75;
        this.jr2.setTranslate(0.7f, 0.0f, 12.35f);
        this.jr2.setRotateY(180.0f);
        this.CameraPlay = 26;
        this.face(this.jr2.face, 2, 0, 45, 0, 0, 1.0f);
        this.jr2.mtn(280, 0, 73, 0, 0, 1.0f, true);
        this.face(this.jr2.face, 2, 0, 8, 0, 0, 0.5f);
        System.sleep(45);
        this.msg.print("Yo.\nAriadne's completely vanished.");
        this.face(this.jr2.face, 1, 0, 30, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.jr2.face, 1, 0, 0, 0, 0, 1.0f);
        this.cut_length = 120;
        this.mary.setRotateY(150.0f);
        this.jr2.setTranslate(0.4f, 0.7f, 9.75f);
        this.CameraPlay = 27;
        this.face(this.mary.face, 2);
        this.jr2.mtn(281, 0, 223, 0, 0, 1.0f, true);
        this.shelly.mtn(266, 0, 0.7f, true);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("But it doesn't seem like\nit was destroyed...");
        System.sleep(45);
        this.mary.setMotNoUpdate(2);
        this.mary.mtn(282, 0, 0, 0, 0, 1.0f, true);
        this.mary.setTranslate(0.9f, 1.0f, 7.55f);
        this.mary.setRotateY(135.0f);
        this.mary.setMotionFlags(0x800000, false);
        this.mary.setMotionFlags(0x2000000, true);
        this.mary.setVisible(false);
        this.CameraPlay = 28;
        this.face(this.jr2.face, 1, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("It's almost as if someone\nis hiding it somewhere.");
        this.face(this.jr2.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        this.op_l3.setVisible(false);
        this.mary.setVisible(true);
        this.jr2.setVisible(true);
        this.jr2.setTranslate(0.3f, 0.7f, 6.8f);
        this.jr2.setRotateY(180.0f);
        this.CameraPlay = 29;
        this.mary.mtn(282, 0, 88, 0, 0, 1.0f, true);
        this.jr2.mtn(283, 8, 1.0f, true);
        System.sleep(15);
        this.msg.print("Hiding it...?\nWhere would that be?");
        this.face(this.mary.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.mary.setMotionFlags(0x800000, false);
        this.mary.setMotionFlags(0x2000000, true);
        this.cut_length = 240;
        this.mary.setTranslate(1.3f, 1.0f, 7.55f);
        this.CameraPlay = 30;
        this.face(this.jr2.face, 18, 0, 0, 12, 1, 1.0f);
        this.mary.mtn(284, 8, 1.0f, true);
        this.jr2.mtn(285, 0, 223, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Who knows?");
        this.face(this.jr2.face, 17, 0, 35, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("The best way to find out would be\nto ask the person who's hiding it.");
        this.face(this.jr2.face, 1, 0, 89, 8, 1, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.msg.print("Shelley?");
        this.face(this.jr2.face, 1, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        this.mary.setTranslate(1.3f, 1.0f, 7.55f);
        this.mary.setRotateY(135.0f);
        this.jr2.setVisible(true);
        this.jr2.setTranslate(0.3f, 0.7f, 6.8f);
        this.jr2.setRotateY(180.0f);
        this.op_l3.setVisible(false);
        this.mary.setVisible(false);
        this.shelly.setTranslate(-0.55f, 1.05f, 7.7f);
        this.shelly.setRotateY(-135.0f);
        this.CameraPlay = 31;
        this.chair.start(1, "c_act1");
        this.shelly.start(1, "mtn30");
        this.jr2.mtn(287, 0, 340, 0, 0, 1.0f, true);
        this.face(this.jr2.face, 2);
        System.sleep(15);
        this.msg.print("It's very slight, but I'm getting\na reaction from the primer.");
        this.face(this.shelly.face, 1, 18, 119, 0, 0, 1.0f);
        this.waitclear(115);
        this.msg.print("No question about it.");
        this.waitclear(45);
        System.sleep(15);
        this.shelly.setMotionFlags(0x40000000, false);
        this.CameraPlay = 32;
        System.sleep(20);
        this.msg.print("Some kids playing\nwith matches, maybe?");
        this.face(this.jr2.face, 15, 8, 89, 12, 1, 1.0f);
        this.waitclear(70);
        System.sleep(15);
        this.msg.print("What happened to\nthat fleet, anyway?");
        this.face(this.jr2.face, 1, 15, 67, 12, 1, 1.0f);
        System.sleep(45);
        this.op_l3.setTranslate(-2.5f, -2.2f, 2.5f);
        this.op_l3.setVisible(true);
        this.CameraPlay = 132;
        this.op_l3.mtn(268, 0, 0.9f, true);
        this.waitclear(10);
        System.sleep(15);
        this.msg.print("We haven't heard anything\nsince we picked up that\ndistress signal six hours ago.");
        this.face(this.op_l3.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.op_l3.face, 1, 0, 18, 0, 0, 1.0f);
        this.waitclear(20);
        this.cut_length = 375;
        this.op_l3.setVisible(false);
        this.mary.setVisible(true);
        this.CameraPlay = 33;
        this.mary.mtn(288, 180, 375, 0, 0, 1.0f, true);
        this.face(this.jr2.face, 2);
        this.face(this.shelly.face, 2);
        System.sleep(15);
        this.msg.print("Gnosis, huh? Hmm...");
        this.face(this.mary.face, 1, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(10);
        this.msg.print("They must have been\ntotally wiped out.");
        this.face(this.mary.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        this.shelly.mtn(289, 150, 150, 0, 0, 1.0f, true);
        this.shelly.setMotionFlags(0x800000, false);
        this.shelly.setMotionFlags(0x2000000, true);
        this.CameraPlay = 35;
        this.face(this.jr2.face, 16, 0, 15, 0, 0, 1.0f);
        this.jr2.mtn(290, 0, 208, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Not necessarily.");
        this.face(this.jr2.face, 15, 0, 35, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("When the little ones play\nwith fire, you know their\nfolks can't be too far behind.");
        this.face(this.jr2.face, 15, 0, 120, 0, 0, 1.0f);
        this.waitclear(135);
        this.jr2.setVisible(true);
        this.jr2.setTranslate(0.3f, 0.7f, 6.8f);
        this.jr2.setRotateY(180.0f);
        this.mary.setTranslate(1.3f, 1.0f, 7.55f);
        this.shelly.setTranslate(-0.55f, 1.05f, 7.7f);
        this.shelly.setRotateY(180.0f);
        this.chair.setRotateY(0.0f);
        this.fm_shelly.start(1, "moni_off");
        this.mary.setRotateY(180.0f);
        this.CameraPlay = 36;
        this.mary.mtn(291, 0, 1.0f, true);
        this.jr2.mtn(292, 0, 1.0f, true);
        this.shelly.mtn(289, 150, 375, 0, 0, 1.0f, true);
        this.face(this.jr2.face, 2);
        this.face(this.shelly.face, 2);
        System.sleep(15);
        this.msg.print("Ah...I get it...");
        this.face(this.mary.face, 1, 0, 18, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("So you reckon that the momma's\ngonna show up soon?");
        this.face(this.mary.face, 1, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("Exactly.");
        this.face(this.jr2.face, 1, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.cut_length = 195;
        this.mary.setVisible(false);
        this.CameraPlay = 37;
        System.sleep(25);
        this.msg.print("Change course!");
        this.face(this.jr2.face, 5, 0, 35, 8, 1, 1.0f);
        this.waitclear(35);
        System.sleep(5);
        this.msg.print("Proceed to the last known\ncoordinates of the Federation fleet!");
        this.face(this.jr2.face, 5, 0, 89, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(40);
        Runtime.mpeg2("2031_2");
        this.space.setVisible(false);
        this.cam1.setTranslate(0.0f, 500.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
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
        this.space.stop();
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
        Runtime.setRegister(1, this.CameraPlay);
        System.println(">>>>>>> CUT /[$1]");
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

        void initMTN_ob_l() {
            this.mtn(293, 8, 1.0f, true);
        }

        void initMTN_ob_r() {
            this.mtn(294, 8, 1.0f, true);
        }

        void initMTN_op_l1() {
            this.mtn(295, 8, 1.0f, true);
        }

        void initMTN_op_l2() {
            this.mtn(296, 8, 1.0f, true);
        }

        void initMTN_op_l3() {
            this.mtn(297, 8, 1.0f, true);
        }

        void initMTN_op_r1() {
            this.mtn(296, 8, 0.9f, true);
        }

        void initMTN_op_r2() {
            this.mtn(295, 8, 0.9f, true);
        }

        void initMTN_op_r3() {
            this.mtn(297, 8, 0.9f, true);
        }

        void initMTN_upl1() {
            this.mtn(293, 8, 1.0f, true);
        }

        void initMTN_upl2() {
            this.mtn(294, 8, 0.9f, true);
        }

        void initMTN_upr1() {
            this.mtn(294, 8, 1.0f, true);
        }

        void initMTN_upr2() {
            this.mtn(293, 8, 0.9f, true);
        }

        void mtn22() {
            this.setTranslate(0.0f, -1.5f, 16.45f);
            this.setRotateY(180.0f);
            this.mtn(278, 0, 1.0f, false);
            int n = 0;
            while (n < SCE02031.this.elv_up) {
                SCE02031.this.elv.getTranslate();
                this.setTranslate(this.px, SCE02031.this.elv.py, this.pz);
                System.sleep(1);
                ++n;
            }
            System.sleep(15);
        }

        void mtn30() {
            this.mtn(286, 0, 1.0f, false);
            this.rotY(30, -180.0f, true);
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
        }

        void a_act() {
            this.setTranslate(0.0f, -1.0f, 0.01f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setScale(0.01f, 0.01f, 0.01f);
            this.mtn(270, 0, 0.5f, false);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 2.0f;
            fArray[6] = -0.95f;
            fArray[7] = -0.03f;
            float[] fArray2 = fArray;
            SCE02031.this.spline.setCtrlVertex(fArray2, 0, 2, SCE02031.this.cut_length);
            this.move(SCE02031.this.spline, 0, true);
        }

        void d_act1() {
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.getTranslate();
                this.setTranslate(this.px, this.py, this.pz + 0.008f);
                System.sleep(1);
                ++n;
            }
        }

        void d_act2() {
            this.setTranslate(0.0f, 0.0f, -0.2f);
            this.mtn(298, 8, 0.3f, false);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz + 0.25f / (float) SCE02031.this.cut_length);
                System.sleep(1);
                ++n;
            }
        }

        void d_act3() {
            this.mtn(298, 8, 0.3f, false);
            this.move(SCE02031.this.cut_length, this.px, this.pz + 1.0f, true);
        }

        void stars_inBridge() {
            int n = SCE02031.this.CameraPlay;
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                if (n == SCE02031.this.CameraPlay - 1) {
                    this.setTranslate(0.0f, 0.0f, -10.0f);
                    n = SCE02031.this.CameraPlay;
                } else {
                    this.setTranslate(this.px, this.py, this.pz + 0.1f);
                }
                System.sleep(1);
            }
        }

        void stars_inBridge_slow() {
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz - 0.1f);
                System.sleep(1);
            }
        }

        void stars_inSpaceL() {
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.05f);
                System.sleep(1);
            }
        }

        void stars_inSpaceR() {
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry - 0.05f);
                System.sleep(1);
            }
        }

        void stars_inSpaceRslow() {
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry - 0.01f);
                System.sleep(1);
            }
        }

        void stars_move() {
            this.setTranslate(0.0f, 0.0f, 10.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - SCE02031.this.stars_spd);
                System.sleep(1);
                ++n;
            }
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void moni_off() {
            this.setArgs(2, 0, 0, 15, -1);
        }

        void moni_on() {
            this.setArgs(2, 48, 0, 15, -1);
        }

        void wl_act1() {
            this.setVisible(true);
            this.setTranslate(4.0f, 4.5f, 6.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px + 0.01f, this.py, this.pz - 0.01f);
                this.setRotate(this.rx, this.ry - 0.4f, this.rz);
                System.sleep(1);
                ++n;
            }
        }

        void wm1_act1() {
            this.setVisible(true);
            this.setTranslate(3.0f, 2.0f, 4.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.02f);
                this.setRotate(this.rx, this.ry + 0.3f, this.rz + 0.3f);
                System.sleep(1);
                ++n;
            }
        }

        void wm2_act1() {
            this.setVisible(true);
            this.setTranslate(-5.0f, 4.0f, 5.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px + 0.01f, this.py, this.pz - 0.02f);
                this.setRotate(this.rx, this.ry - 0.3f, this.rz);
                System.sleep(1);
                ++n;
            }
        }

        void wm3_act1() {
            this.setVisible(true);
            this.setTranslate(5.0f, 10.0f, 10.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.02f);
                this.setRotate(this.rx - 0.05f, this.ry, this.rz);
                System.sleep(1);
                ++n;
            }
        }

        void ws1_act1() {
            this.setVisible(true);
            this.setTranslate(1.5f, 2.0f, 0.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.02f);
                this.setRotate(this.rx, this.ry + 0.6f, this.rz + 0.6f);
                System.sleep(1);
                ++n;
            }
        }

        void ws1_act2() {
            this.setVisible(true);
            this.setTranslate(-3.0f, 3.0f, 0.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.02f);
                this.setRotate(this.rx, this.ry + 0.6f, this.rz + 0.6f);
                System.sleep(1);
                ++n;
            }
        }

        void ws2_act1() {
            this.setVisible(true);
            this.setTranslate(0.0f, 0.0f, 5.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.02f);
                this.setRotate(this.rx, this.ry + 0.6f, this.rz + 0.6f);
                System.sleep(1);
                ++n;
            }
        }

        void ws2_act2() {
            this.setVisible(true);
            this.setTranslate(-8.0f, 5.0f, 1.0f);
            int n = 0;
            while (n < SCE02031.this.cut_length) {
                this.setTranslate(this.px, this.py, this.pz - 0.01f);
                this.setRotate(this.rx + 0.1f, this.ry + 0.6f, this.rz);
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
            this.setVisible(true);
            this.start(4, null);
        }

        void c_act1() {
            this.setRotateY(45.0f);
            this.rotY(30, 0.0f, true);
        }

        void e_act1() {
            this.setTranslate(0.0f, -1.5f, 0.0f);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -1.5f;
            fArray[4] = 10.0f;
            float[] fArray2 = fArray;
            SCE02031.this.spline.setCtrlVertex(fArray2, 0, 1, SCE02031.this.elv_up);
            this.move(SCE02031.this.spline, true);
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }

        void sonar_ring_L() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void sonar_ring_Large() {
            float f = 0.5f;
            this.setTranslate(0.0f, 0.0f, -9.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 9.0f, this.py, -9.0f + Math.sin(Math.toRadians(f)) * 9.0f);
                this.setRotateY(f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void sonar_ring_R() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(-Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, -f);
                f += 0.5f;
                System.sleep(1);
            }
        }
    }
}

