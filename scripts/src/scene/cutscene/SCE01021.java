import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK11_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01021
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01021,
        MC_VOK11_PRJ,
        FLSmoriyama,
        FLSchaos,
        FLSoff_w,
        FLSoff_w1 {
    Thread thread1;
    Characters captain;
    Characters op_upl;
    Characters op_upr1;
    Characters op_upr2;
    Characters ob_l;
    Characters ob_r;
    Characters op_l1;
    Characters op_l2;
    Characters op_l3;
    Characters op_r1;
    Characters op_r2;
    Characters op_r3;
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
    Unit fm_cap1;
    Unit fm_cap2;
    Unit fm_momo;
    Unit kos_moni;
    Unit km1;
    Unit km2;
    Unit km3;
    Unit km4;
    Unit km5;
    Unit km6;
    Unit km7;
    Unit km8;
    Unit km9;
    Unit km10;
    Chr_units space;
    MAPUnit ring_1a;
    MAPUnit ring_1b;
    MAPUnit ring_1c;
    MAPUnit ring_2a;
    MAPUnit ring_2b;
    MAPUnit ring_2c;
    MAPUnit sonar_ring_0;
    MAPUnit sonar_ring_1;
    MAPUnit sonar_ring_2;
    MAPUnit sonar_ring_3;
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

    SCE01021() {
    }

    void AllChrSPECOFF() {
        this.captain.renderCommand(530);
        this.op_upl.renderCommand(530);
        this.op_upr1.renderCommand(530);
        this.op_upr2.renderCommand(530);
        this.ob_l.renderCommand(530);
        this.ob_r.renderCommand(530);
        this.op_r1.renderCommand(530);
        this.op_r2.renderCommand(530);
        this.op_r3.renderCommand(530);
        this.op_l1.renderCommand(530);
        this.op_l2.renderCommand(530);
        this.op_l3.renderCommand(530);
    }

    void AllChrSPECON() {
        this.captain.renderCommand(0);
        this.op_upl.renderCommand(0);
        this.op_upr1.renderCommand(0);
        this.op_upr2.renderCommand(0);
        this.ob_l.renderCommand(0);
        this.ob_r.renderCommand(0);
        this.op_r1.renderCommand(0);
        this.op_r2.renderCommand(0);
        this.op_r3.renderCommand(0);
        this.op_l1.renderCommand(0);
        this.op_l2.renderCommand(0);
        this.op_l3.renderCommand(0);
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.light.setColor(0, 0.32f, 0.32f, 0.32f);
        this.light.setColor(1, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(1, -0.786f, 0.036f, 0.617f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.656f, 0.711f, -0.251f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.waitCameraPlay(1);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-7.67f, 0.24f, -9.06f);
        this.cam1.setRotate(-2.05f, 220.36f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 120, -8.87f, 0.24f, -8.04f);
        this.waitCameraPlay(2);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(4.66f, 1.14f, 12.66f);
        this.cam1.setRotate(-3.55f, 213.86f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 4.73f, 1.14f, 12.61f);
        this.waitCameraPlay(3);
        Runtime.setDefocusQuick(0, 1, 65880, 1);
        this.cam1.setTranslate(-1.1f, 2.01f, 6.57f);
        this.cam1.setRotate(5.45f, 205.86f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        Runtime.setDefocusQuick(0, 1, 53880, 1);
        this.cam1.setTranslate(5.63f, 1.36f, 15.41f);
        this.cam1.setRotate(-15.68f, -195.72f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 5.76f, 1.36f, 15.45f);
        this.waitCameraPlay(5);
        Runtime.setDefocusQuick(0, 1, 61880, 1);
        this.cam1.setTranslate(5.63f, 1.14f, 12.57f);
        this.cam1.setRotate(-1.05f, 525.86f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 5.75f, 1.14f, 12.6f);
        this.waitCameraPlay(6);
        Runtime.setDefocusQuick(0, 1, 29880, 1);
        this.cam1.setTranslate(-6.73f, 1.22f, 16.74f);
        this.cam1.setRotate(-3.68f, -85.72f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -6.74f, 1.22f, 16.62f);
        this.waitCameraPlay(7);
        Runtime.setDefocusQuick(0, 1, 12880, 1);
        Runtime.setDefocusQuick(1, 2, 110264, 1);
        this.cam1.setTranslate(-6.21f, 1.2f, 17.74f);
        this.cam1.setRotate(2.96f, -27.6f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        Runtime.setDefocusQuick(0, 1, 17880, 1);
        this.cam1.setTranslate(-0.86f, 2.0f, 9.56f);
        this.cam1.setRotate(9.92f, -2.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(9);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.32f, 2.77f, 5.84f);
        this.cam1.setRotate(-12.0f, -199.0f, 0.0f);
        this.cam1.setFov(20.0f);
        this.waitCameraPlay(10);
        Runtime.setDefocusQuick(0, 1, 97880, 1);
        this.cam1.setTranslate(-5.41f, 1.15f, 15.4f);
        this.cam1.setRotate(-3.94f, -182.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(120);
        this.rSPL(30, 7.06f, -181.5f, 0.0f, 3);
        this.waitCameraPlay(11);
        Runtime.setDefocusQuick(0, 1, 74880, 1);
        this.cam1.setTranslate(4.68f, 0.97f, 12.32f);
        this.cam1.setRotate(2.05f, -160.07f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(12);
        Runtime.setDefocusQuick(0, 1, 98880, 1);
        this.cam1.setTranslate(-5.67f, 1.24f, 15.34f);
        this.cam1.setRotate(-6.45f, -170.07f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        Runtime.setDefocusQuick(0, 1, 37880, 1);
        this.cam1.setTranslate(-6.6f, 1.22f, 16.9f);
        this.cam1.setRotate(0.54f, -68.36f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        this.cam1.setTranslate(-5.39f, 3.69f, 12.19f);
        this.cam1.setRotate(-32.96f, -172.86f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -5.79f, 3.69f, 12.24f);
        this.waitCameraPlay(15);
        Runtime.setDefocusQuick(0, 1, 64880, 1);
        this.cam1.setTranslate(-6.38f, 1.0f, 16.82f);
        this.cam1.setRotate(11.54f, -71.36f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(16);
        this.cam1.setTranslate(4.18f, 2.52f, 14.45f);
        this.cam1.setRotate(17.64f, -34.65f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(17);
        this.cam1.setTranslate(-6.79f, 1.25f, 17.63f);
        this.cam1.setRotate(9.64f, -55.15f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -5.41f, 1.25f, 18.47f);
        this.rSPL(this.cut_length, 9.64f, -6.65f, 0.0f);
        this.waitCameraPlay(18);
        Runtime.setDefocusQuick(0, 1, 136880, 1);
        this.cam1.setTranslate(-6.34f, 0.83f, 16.58f);
        this.cam1.setRotate(29.39f, -85.16f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(19);
        this.cam1.setTranslate(-5.08f, 1.42f, 17.03f);
        this.cam1.setRotate(2.57f, 43.02f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -5.43f, 1.42f, 16.66f);
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
        System.println("XEVEFLAG:EV01021_F");
        Runtime.setFlags(29, 1, 1);
        System.println("XEVEJNAME:SCE01022");
        Runtime.jumpEvent(1220);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(1220);
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
        Runtime.setLocation(26);
        this.captain = new Characters(16777518, -0.52f, 1.05f, 7.7f, 180.0f);
        this.op_upl = new Characters(16777735, -5.25f, 0.05f, 16.75f, -135.0f);
        this.op_upr1 = new Characters(16777739, 5.25f, 0.125f, 13.75f, 135.0f);
        this.op_upr2 = new Characters(16777740, 5.25f, 0.125f, 16.75f, 135.0f);
        this.ob_l = new Characters(16777740, -5.65f, -1.95f, 5.2f, 180.0f);
        this.ob_r = new Characters(522, 5.6f, -2.0f, 5.3f, 180.0f);
        this.op_r1 = new Characters(519, 2.55f, -2.25f, -3.55f, 135.0f);
        this.op_r2 = new Characters(521, 2.55f, -2.25f, -0.6f, 135.0f);
        this.op_r3 = new Characters(524, 2.6f, -2.2f, 2.4f, 135.0f);
        this.op_l1 = new Characters(523, -2.6f, -2.2f, -3.6f, -135.0f);
        this.op_l2 = new Characters(520, -2.55f, -2.25f, -0.6f, -135.0f);
        this.op_l3 = new Characters(522, -2.55f, -2.25f, 2.45f, -135.0f);
        this.space = new Chr_units(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.setVisible(true);
        this.space.start(1, "stars_inBridge");
        this.loadarc(this.captain.face, "FLSmoriyama.fpk");
        this.loadarc(this.op_upl.face, "FLSchaos.fpk");
        this.loadarc(this.op_upr1.face, "FLSoff_w.fpk");
        this.loadarc(this.op_upr2.face, "FLSoff_w1.fpk");
        this.loadarc(this.ob_l.face, "FLSoff_w1.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.captain.start(1, "initMTN_cap");
        this.op_upl.start(1, "initMTN_op_upl");
        this.op_upr1.start(1, "initMTN_op_upr1");
        this.op_upr2.start(1, "initMTN_op_upr2");
        this.op_l1.start(1, "initMTN_op_l1");
        this.op_l2.start(1, "initMTN_op_l2");
        this.op_l3.start(1, "initMTN_op_l3");
        this.ob_l.start(1, "initMTN_ob_l");
        this.ob_r.start(1, "initMTN_ob_r");
        this.op_r1.start(1, "initMTN_op_r1");
        this.op_r2.start(1, "initMTN_op_r2");
        this.op_r3.start(1, "initMTN_op_r3");
        this.captain.signal(0);
        this.op_upl.signal(0);
        this.op_upr1.signal(0);
        this.op_upr2.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        Stage.setVisible(0, false);
        Stage.setVisible(137, false);
        Stage.setVisible(138, false);
        Stage.setVisible(151, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(142, false);
        Stage.setVisible(143, false);
        Stage.setVisible(144, false);
        Stage.setVisible(145, false);
        Stage.setVisible(146, false);
        Stage.setVisible(147, false);
        Stage.setVisible(148, false);
        this.fm1 = new Units(24613, -3.41f, -1.8f, -4.45f, 45.0f);
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm1.setArgs(1, 15001, 0, 128, 112);
        this.fm1.setArgs(2, 96, 0, 0, -1);
        this.fm1.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm2 = new Units(24613, -3.41f, -1.8f, -1.45f, 45.0f);
        this.fm2.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm2.setArgs(1, 15001, 0, 128, 112);
        this.fm2.setArgs(2, 96, 0, 0, -1);
        this.fm2.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm3 = new Units(24613, -3.41f, -1.8f, 1.58f, 45.0f);
        this.fm3.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm3.setArgs(1, 15001, 0, 128, 112);
        this.fm3.setArgs(2, 96, 0, 0, -1);
        this.fm3.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm4 = new Units(24613, 3.41f, -1.8f, -4.45f, -45.0f);
        this.fm4.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm4.setArgs(1, 15001, 0, 128, 112);
        this.fm4.setArgs(2, 96, 0, 0, -1);
        this.fm4.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm5 = new Units(24613, 3.41f, -1.8f, -1.45f, -45.0f);
        this.fm5.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm5.setArgs(1, 15001, 0, 128, 112);
        this.fm5.setArgs(2, 96, 0, 0, -1);
        this.fm5.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm6 = new Units(24613, 3.41f, -1.8f, 1.58f, -45.0f);
        this.fm6.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm6.setArgs(1, 15001, 0, 128, 112);
        this.fm6.setArgs(2, 96, 0, 0, -1);
        this.fm6.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm7 = new Units(24613, -5.6f, -1.46f, 4.6f, 0.0f);
        this.fm7.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm7.setArgs(1, 15002, 0, 128, 114);
        this.fm7.setArgs(2, 96, 0, 0, -1);
        this.fm7.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm8 = new Units(24613, 5.6f, -1.46f, 4.6f, 0.0f);
        this.fm8.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm8.setArgs(1, 15002, 0, 128, 114);
        this.fm8.setArgs(2, 96, 0, 0, -1);
        this.fm8.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm9 = new Units(24613, -6.37f, 1.45f, 12.63f, 45.0f);
        this.fm9.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm9.setArgs(1, 15001, 0, 128, 112);
        this.fm9.setArgs(2, 96, 0, 0, -1);
        this.fm9.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm10 = new Units(24613, -6.37f, 1.45f, 15.63f, 45.0f);
        this.fm10.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm10.setArgs(1, 15001, 0, 128, 112);
        this.fm10.setArgs(2, 96, 0, 0, -1);
        this.fm10.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm11 = new Units(24613, 6.37f, 1.45f, 12.63f, -45.0f);
        this.fm11.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm11.setArgs(1, 15001, 0, 128, 112);
        this.fm11.setArgs(2, 96, 0, 0, -1);
        this.fm11.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm12 = new Units(24613, 6.37f, 1.45f, 15.63f, -45.0f);
        this.fm12.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm12.setArgs(1, 15001, 0, 128, 112);
        this.fm12.setArgs(2, 96, 0, 0, -1);
        this.fm12.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm_momo = new Units(24613, 0.0f, 1.3f, 1.3f, 0.0f);
        this.fm_momo.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm_momo.setArgs(1, 15003, 0, 128, 114);
        this.fm_momo.setArgs(2, 96, 0, 0, -1);
        this.fm_momo.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm_cap1 = new Units(24613, -0.38f, 2.3799999f, 6.0f, 0.0f);
        this.fm_cap1.setArgs(0, 0.0f, 0.0f, 1.5f, 0.61f);
        this.fm_cap1.setArgs(1, 20012, 0, 0, 0);
        this.fm_cap1.setArgs(2, 96, 0, 15, -1);
        this.fm_cap1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm_cap2 = new Units(24613, -1.6f, 2.3799999f, 7.06f, 90.0f);
        this.fm_cap2.setArgs(0, 0.0f, 0.0f, 1.5f, 0.61f);
        this.fm_cap2.setArgs(1, 20036, 0, 0, 0);
        this.fm_cap2.setArgs(2, 96, 0, 15, -1);
        this.fm_cap2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
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
        this.fm_cap1.signal(1);
        this.fm_cap2.signal(1);
        this.kos_moni = new Units(24613, -6.37f, 1.5f, 15.63f, 45.0f);
        this.kos_moni.setArgs(0, 0.0f, 0.0f, 1.2f, 0.9f);
        this.kos_moni.setArgs(1, 20074, 0, 0, 0);
        this.kos_moni.setArgs(2, 96, 0, 0, 1);
        this.kos_moni.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km1 = new Units(24613, 6.95f, 3.65f, 11.45f, 0.0f);
        this.km1.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km1.setArgs(1, 22024, 0, 0, 0);
        this.km1.setArgs(2, 96, 0, 0, -1);
        this.km1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km2 = new Units(24613, 7.45f, 4.75f, 6.7f, 0.0f);
        this.km2.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km2.setArgs(1, 22024, 0, 0, 0);
        this.km2.setArgs(2, 96, 0, 0, -1);
        this.km2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km3 = new Units(24613, 7.45f, 3.65f, 2.7f, 0.0f);
        this.km3.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km3.setArgs(1, 22024, 0, 0, 0);
        this.km3.setArgs(2, 96, 0, 0, -1);
        this.km3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km4 = new Units(24613, 6.05f, 3.9f, -2.85f, 0.0f);
        this.km4.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km4.setArgs(1, 22024, 0, 0, 0);
        this.km4.setArgs(2, 96, 0, 0, -1);
        this.km4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km5 = new Units(24613, 4.5f, 5.0f, -3.5f, 0.0f);
        this.km5.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km5.setArgs(1, 22024, 0, 0, 0);
        this.km5.setArgs(2, 96, 0, 0, -1);
        this.km5.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km6 = new Units(24613, 2.0f, 4.5f, -4.5f, 0.0f);
        this.km6.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km6.setArgs(1, 22024, 0, 0, 0);
        this.km6.setArgs(2, 96, 0, 0, -1);
        this.km6.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km7 = new Units(24613, 0.0f, 6.0f, -5.0f, 0.0f);
        this.km7.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km7.setArgs(1, 22024, 0, 0, 0);
        this.km7.setArgs(2, 96, 0, 0, -1);
        this.km7.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km8 = new Units(24613, -3.0f, 5.5f, -2.5f, 0.0f);
        this.km8.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km8.setArgs(1, 22024, 0, 0, 0);
        this.km8.setArgs(2, 96, 0, 0, -1);
        this.km8.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km9 = new Units(24613, -5.0f, 4.5f, 5.0f, 0.0f);
        this.km9.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km9.setArgs(1, 22024, 0, 0, 0);
        this.km9.setArgs(2, 96, 0, 0, -1);
        this.km9.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.km10 = new Units(24613, -6.0f, 5.0f, 8.0f, 0.0f);
        this.km10.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
        this.km10.setArgs(1, 22024, 0, 0, 0);
        this.km10.setArgs(2, 96, 0, 0, -1);
        this.km10.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ring_1a = new Mapunits(126);
        this.ring_1b = new Mapunits(127);
        this.ring_1c = new Mapunits(128);
        this.ring_2a = new Mapunits(129);
        this.ring_2b = new Mapunits(130);
        this.ring_2c = new Mapunits(131);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.sonar_ring_0 = new Mapunits(133);
        this.sonar_ring_1 = new Mapunits(134);
        this.sonar_ring_2 = new Mapunits(135);
        this.sonar_ring_3 = new Mapunits(136);
        this.sonar_ring_0.start(1, "sonar_ring_Large");
        this.sonar_ring_1.start(1, "sonar_ring_L");
        this.sonar_ring_2.start(1, "sonar_ring_R");
        this.sonar_ring_3.start(1, "sonar_ring_L");
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        System.sleep(1);
        Sound.streamPlay(1190039, 48000);
        this.AllChrSPECOFF();
        this.cut_length = 180;
        this.captain.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upl.setVisible(false);
        this.ob_l.setVisible(false);
        this.CameraPlay = 1;
        System.sleep(this.cut_length);
        this.AllChrSPECON();
        this.cut_length = 150;
        this.captain.setVisible(true);
        this.op_upr1.setVisible(true);
        this.op_upl.setVisible(true);
        this.ob_l.setVisible(true);
        this.CameraPlay = 2;
        this.op_upr1.mtn(257, 0, 1.0f, true);
        this.op_upr2.mtn(257, 0, 75, 0, 0, 0.5f, true);
        System.sleep(75);
        this.msg.print("Now exiting the asteroid field.");
        this.face(this.op_upr1.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.CameraPlay = 3;
        this.ob_l.start(1, "c3_idle_ob_l");
        this.captain.mtn(258, 15, 120, 0, 0, 1.0f, true);
        this.msg.print("That's excellent!");
        this.face(this.captain.face, 1, 0, 18, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("Prepare to gate-jump.");
        this.face(this.captain.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.cut_length = 105;
        this.CameraPlay = 4;
        this.op_upr1.start(1, "ready_c5_4");
        this.op_upr2.start(1, "c4_3");
        this.msg.print("Aye-aye, Captain.\nAll ships entering approach.");
        this.face(this.op_upr2.face, 1, 0, 18, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.op_upr2.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(60);
        this.cut_length = 225;
        this.CameraPlay = 5;
        this.face(this.op_upr2.face, 2, 0, 105, 0, 0, 1.0f);
        this.op_upr1.mtn(260, 30, 193, 0, 0, 1.0f, true);
        this.op_upr2.mtn(261, 30, 193, 0, 0, 1.0f, true);
        this.msg.print("19 minutes, 30 seconds\nto column area.");
        this.face(this.op_upr1.face, 1, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.face(this.op_upr1.face, 2, 0, 90, 0, 0, 1.0f);
        this.msg.print("U.M.N. pulse received.");
        this.face(this.op_upr2.face, 1, 0, 90, 0, 0, 1.0f);
        this.waitclear(75);
        this.cut_length = 225;
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.CameraPlay = 6;
        this.op_upr1.start(1, "c6_idle_r1");
        this.op_upr2.start(1, "c6_idle_r2");
        this.op_upl.start(1, "c6_6");
        this.msg.print("Current coordinates locked.\nTransfer vector correction to 103.");
        this.face(this.op_upl.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(45);
        this.waitclear(75);
        this.msg.print("Target: Athens Column.");
        this.face(this.op_upl.face, 1, 0, 39, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(30);
        this.face(this.op_upl.face, 6, 0, 0, 12, 1, 1.0f);
        System.sleep(15);
        this.CameraPlay = 7;
        this.captain.start(1, "c7_7");
        System.sleep(45);
        this.face(this.captain.face, 4, 0, 0, 0, 0, 1.0f);
        this.CameraPlay = 8;
        System.sleep(60);
        this.op_upl.setTranslate(-5.28f, 0.1f, 16.73f);
        this.CameraPlay = 9;
        this.face(this.captain.face, 4, 0, 90, 0, 0, 1.0f);
        this.captain.start(1, "c9_8");
        this.op_upl.mtn(265, 15, 148, 0, 0, 1.0f, true);
        this.msg.print("Captain!\nA warning signal!");
        this.face(this.op_upl.face, 5, 0, 15, 0, 0, 1.0f);
        System.sleep(20);
        this.face(this.op_upl.face, 5, 0, 27, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("It can't be...\nIs it them?!");
        this.face(this.captain.face, 3, 0, 59, 0, 0, 1.0f);
        this.waitclear(60);
        this.CameraPlay = 10;
        this.op_upl.start(1, "c10_9");
        System.sleep(30);
        this.msg.print("No, Sir. The detection\nsystem is silent.");
        this.face(this.op_upl.face, 5, 0, 79, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("How's it look on your side?");
        this.face(this.op_upl.face, 5, 0, 55, 0, 0, 1.0f);
        System.sleep(30);
        this.op_upr1.setTranslate(5.32f, 0.105f, 13.68f);
        this.CameraPlay = 11;
        this.op_upr1.mtn(266, 0, 1.0f, true);
        this.op_upr2.start(1, "c11_bgmtn");
        this.face(this.op_upr1.face, 2, 0, 30, 0, 0, 1.0f);
        this.waitclear(20);
        this.msg.print("Nothing over here, either.");
        this.face(this.op_upr1.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(50);
        this.msg.print("You sure it's not an error?");
        this.face(this.op_upr1.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        this.CameraPlay = 12;
        this.op_upr1.start(1, "c12_idle_r1");
        this.op_upr2.start(1, "c12_idle_r2");
        this.op_upl.start(1, "c12_13_11");
        System.sleep(45);
        this.msg.print("No, no it's not.");
        this.face(this.op_upl.face, 5, 0, 39, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("What...is this...?");
        this.face(this.op_upl.face, 5, 0, 15, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("What's going on?");
        System.sleep(30);
        this.captain.setTranslate(-3.35f, 0.0f, 15.75f);
        this.captain.setRotateY(-80.0f);
        this.CameraPlay = 13;
        this.captain.mtn(268, 0, 268, 0, 0, 1.0f, true);
        this.waitclear(15);
        this.msg.print("Sir, I don't think an external source\nis causing this warning signal.");
        this.face(this.op_upl.face, 5, 0, 120, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("Then what's causing it?");
        this.face(this.captain.face, 3, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("I'll run a search.");
        this.face(this.op_upl.face, 5, 0, 15, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.captain.setTranslate(-4.51f, 0.0f, 15.94f);
        this.captain.setRotateY(-80.0f);
        this.op_upl.setTranslate(-5.22f, 0.12f, 16.78f);
        this.CameraPlay = 14;
        this.captain.mtn(269, 0, 88, 0, 0, 1.0f, true);
        this.op_upl.mtn(270, 0, 88, 0, 0, 1.0f, true);
        System.sleep(90);
        this.CameraPlay = 15;
        this.op_upl.start(1, "c15_15");
        this.captain.start(1, "c15_16");
        System.sleep(45);
        this.msg.print("...I've pinpointed the anomaly.");
        this.face(this.op_upl.face, 5, 0, 55, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        System.sleep(10);
        this.msg.print("It's inside the ship,\nSector Three...?!");
        this.face(this.op_upl.face, 5, 0, 55, 0, 0, 1.0f);
        this.waitclear(80);
        this.ob_l.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.CameraPlay = 16;
        System.sleep(30);
        this.km1.start(1, "moni_on");
        System.sleep(30);
        this.km2.start(1, "moni_on");
        System.sleep(30);
        this.cut_length = 300;
        this.CameraPlay = 17;
        this.captain.start(1, "c17_17");
        this.op_upl.start(1, "c17_18");
        System.sleep(30);
        this.km4.start(1, "moni_on");
        System.sleep(30);
        this.km5.start(1, "moni_on");
        System.sleep(15);
        this.km3.start(1, "moni_on");
        System.sleep(15);
        this.km6.start(1, "moni_on");
        System.sleep(30);
        this.km7.start(1, "moni_on");
        System.sleep(15);
        this.km8.start(1, "moni_on");
        System.sleep(45);
        this.km9.start(1, "moni_on");
        System.sleep(45);
        this.km10.start(1, "moni_on");
        System.sleep(75);
        this.captain.setVisible(false);
        this.CameraPlay = 18;
        System.sleep(45);
        this.cut_length = 270;
        this.op_upl.setVisible(false);
        this.fm10.setTranslate(0.0f, 100.0f, 0.0f);
        this.CameraPlay = 19;
        this.kos_moni.signal(1);
        System.sleep(120);
        this.msg.print("It's...");
        this.face(this.op_upl.face, 5, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("KOS-MOS!!");
        this.face(this.op_upl.face, 5, 0, 60, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(45);
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

        void c10_9() {
            this.mtn(265, 165, 315, 0, 0, 1.0f, true);
            this.mtn(265, 300, 315, 0, 0, -1.0f, true);
        }

        void c11_bgmtn() {
            this.setTranslate(5.25f, 0.125f, 16.75f);
            this.mtn(261, 0, 1.0f, true);
        }

        void c12_13_11() {
            this.setTranslate(-5.28f, 0.1f, 16.73f);
            this.mtn(267, 30, 210, 0, 0, 0.93f, true);
            this.mtn(267, 210, 478, 0, 0, 1.0f, true);
        }

        void c12_idle_r1() {
            this.setTranslate(this.px, 0.15f, this.pz);
            this.mtn(276, 8, 0.8f, true);
        }

        void c12_idle_r2() {
            this.setTranslate(this.px, 0.15f, this.pz);
            this.mtn(277, 8, 0.9f, true);
        }

        void c15_15() {
            this.setTranslate(-5.25f, 0.1f, 16.75f);
            this.mtn(271, 0, 1.0f, true);
        }

        void c15_16() {
            this.mtn(272, 0, 1.0f, true);
        }

        void c17_17() {
            this.setTranslate(-4.51f, 0.0f, 15.94f);
            this.setRotateY(180.0f);
            this.mtn(273, 0, 1.0f, true);
        }

        void c17_18() {
            this.setTranslate(this.px, this.py - 0.05f, this.pz);
            this.mtn(274, 0, 1.0f, true);
        }

        void c3_idle_ob_l() {
            this.mtn(275, 8, 1.0f, true);
        }

        void c4_3() {
            this.mtn(259, 15, 103, 0, 0, 1.0f, true);
        }

        void c6_6() {
            this.mtn(262, 15, 270, 0, 0, 0.9f, true);
        }

        void c6_idle_r1() {
            this.setTranslate(this.px, 0.15f, this.pz);
            this.mtn(260, 0, 0.6f, true);
        }

        void c6_idle_r2() {
            this.setTranslate(this.px, 0.15f, this.pz);
            this.mtn(277, 8, 0.9f, true);
        }

        void c7_7() {
            this.mtn(263, 0, 103, 0, 0, 1.0f, true);
        }

        void c9_8() {
            this.mtn(264, 0, 1.0f, true);
        }

        void initMTN_cap() {
            this.mtn(278, 8, 1.0f, true);
        }

        void initMTN_ob_l() {
            this.mtn(275, 8, 1.0f, true);
        }

        void initMTN_ob_r() {
            this.mtn(257, 8, 0.3f, true);
        }

        void initMTN_op_l1() {
            this.mtn(257, 8, 0.3f, true);
        }

        void initMTN_op_l2() {
            SCE01021.this.op_l2.mtn(275, 8, 0.8f, true);
        }

        void initMTN_op_l3() {
            SCE01021.this.op_l3.mtn(276, 8, 1.2f, true);
        }

        void initMTN_op_r1() {
            this.mtn(257, 8, 0.5f, false);
        }

        void initMTN_op_r2() {
            this.mtn(257, 8, 0.4f, false);
        }

        void initMTN_op_r3() {
            this.mtn(257, 8, 0.3f, false);
        }

        void initMTN_op_upl() {
            this.mtn(275, 8, 0.7f, true);
        }

        void initMTN_op_upr1() {
            this.mtn(276, 8, 0.8f, true);
        }

        void initMTN_op_upr2() {
            this.mtn(277, 8, 0.9f, true);
        }

        void ready_c5_4() {
            this.mtn(260, 0, 1, 0, 0, 1.0f, true);
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
            this.setVisible(false);
        }

        void rotL() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.01f);
                System.sleep(1);
            }
        }

        void rotR() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotateY(this.ry - 0.01f);
                System.sleep(1);
            }
        }

        void stars_inBridge() {
            int n = SCE01021.this.CameraPlay;
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                if (n == SCE01021.this.CameraPlay - 1) {
                    this.setTranslate(0.0f, 0.0f, -10.0f);
                    n = SCE01021.this.CameraPlay;
                } else {
                    this.setTranslate(this.px, this.py, this.pz + 1.0f);
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
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void moni_on() {
            this.setArgs(2, 32, 0, 0, -1);
            this.signal(1);
            int n = 0;
            while (n < 12) {
                this.setArgs(0, 0.0f, 0.0f, 0.0f + 0.13333334f * (float) n, 0.01f);
                System.sleep(1);
                ++n;
            }
            int n2 = 0;
            while (n2 < 8) {
                this.setArgs(0, 0.0f, 0.0f, 1.6f, 0.01f + 0.14875f * (float) n2);
                this.setArgs(2, 32 + 8 * n2, 0, 0, -1);
                System.sleep(1);
                ++n2;
            }
            this.setArgs(0, 0.0f, 0.0f, 1.6f, 1.2f);
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(true);
            this.setPivot(0.0f, 0.0f, 0.0f);
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

        void stars_inBridge() {
            int n = SCE01021.this.CameraPlay;
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                if (n == SCE01021.this.CameraPlay - 1) {
                    this.setTranslate(0.0f, 0.0f, -10.0f);
                    n = SCE01021.this.CameraPlay;
                } else {
                    this.setTranslate(this.px, this.py, this.pz + 0.07f);
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
    }
}

