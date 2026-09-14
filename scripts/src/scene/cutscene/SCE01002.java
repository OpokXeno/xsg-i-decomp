import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01002
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01002,
        FLSshion_h,
        FLSkosmos_h,
        FLSallen_h,
        FLSvec_m,
        FLSvec_w,
        JNT_Human,
        MC_VOK01_PRJ {
    Thread thread1;
    Characters shion;
    Characters kosmos;
    Characters allen;
    Characters rm1;
    Characters rw1;
    Chr rm2;
    Chr rm3;
    Chr rw2;
    Chr togashi;
    Monitors fm1;
    Monitors fm2;
    Monitors fm3;
    Monitors fm4;
    Monitors fm5;
    Monitors fm6;
    Units headset;
    Units moniDev1;
    Units moniDev2;
    Unit kosmos_tub;
    Unit vtl_seat;
    int cut_length = 0;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    static final int Chand_L = 26;
    static final int Chand_R = 20;
    static final int toRes = 0;
    static final int toEns = 1;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01002() {
    }

    void CameraThread() {
        this.defaultLight();
        this.cam1.change();
        this.waitCameraPlay(1);
        Runtime.setDefocusQuick(0, 1, 36880, 1);
        this.cam1.setTranslate(5.71f, 1.19f, -7.14f);
        this.cam1.setRotate(-2.5f, -329.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 5.8f, 1.19f, -7.19f);
        this.waitCameraPlay(2);
        Runtime.setDefocusQuick(0, 1, 71880, 1);
        Runtime.setDefocusQuick(1, 1, 14880, 1);
        this.cam1.setTranslate(5.7f, 1.43f, -4.4f);
        this.cam1.setRotate(-1.5f, -258.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, 0.584f, 0.187f, -0.79f);
        this.light.setColor(2, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(2, -0.927f, 0.138f, -0.348f);
        this.light.setColor(3, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(3, -0.186f, -0.358f, -0.915f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 94880, 1);
        this.cam1.setTranslate(-0.01f, 1.35f, 6.14f);
        this.cam1.setRotate(0.5f, -160.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.84f, 0.84f, 0.84f);
        this.light.setDirection2(1, 0.256f, 0.234f, -0.938f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, -0.687f, 0.195f, 0.7f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, -0.919f, -0.382f, -0.096f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 63880, 1);
        Runtime.setDefocusQuick(1, 2, 149264, 2);
        this.cam1.setTranslate(-1.39f, 1.34f, 7.58f);
        this.cam1.setRotate(4.38f, -92.87f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -1.39f, 1.41f, 7.58f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, 0.584f, 0.187f, -0.79f);
        this.light.setColor(2, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(2, -0.927f, 0.138f, -0.348f);
        this.light.setColor(3, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(3, -0.186f, -0.358f, -0.915f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 94880, 1);
        this.cam1.setTranslate(-0.01f, 1.35f, 6.14f);
        this.cam1.setRotate(0.5f, -160.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(1, -0.031f, 0.152f, 0.988f);
        this.light.setColor(2, 0.76f, 0.76f, 0.76f);
        this.light.setDirection2(2, 0.718f, 0.197f, -0.667f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, 0.789f, -0.461f, 0.407f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 103880, 1);
        Runtime.setDefocusQuick(1, 1, 13880, 1);
        this.cam1.setTranslate(5.13f, 1.47f, -3.49f);
        this.cam1.setRotate(-1.5f, -668.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.49f, 0.49f, 0.49f);
        this.light.setColor(1, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(1, -0.708f, 0.0f, 0.706f);
        this.light.setColor(2, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(2, -0.72f, 0.549f, -0.424f);
        this.light.setColor(3, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(3, 0.687f, -0.001f, -0.726f);
        Runtime.setDefocusQuick(0, 1, 27880, 1);
        this.cam1.setTranslate(2.24f, 1.3f, -2.38f);
        this.cam1.setRotate(5.0f, -49.5f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(15);
        this.rSPL(90, 5.0f, -35.5f, 0.0f, 3);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.8f, 0.8f, 0.8f);
        this.light.setDirection2(1, 0.576f, 0.139f, -0.805f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.966f, 0.194f, 0.17f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.306f, -0.921f, -0.24f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 78880, 1);
        Runtime.setDefocusQuick(0, 1, 51880, 1);
        this.cam1.setTranslate(1.17f, 0.95f, 5.79f);
        this.cam1.setRotate(10.0f, -206.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 60, 0.99f, 0.95f, 5.7f);
        this.waitCameraPlay(9);
        Runtime.setDefocusQuick(0, 1, 55880, 1);
        Runtime.setDefocusQuick(1, 1, 22880, 1);
        this.cam1.setTranslate(1.62f, 0.75f, -1.55f);
        this.cam1.setRotate(12.5f, -39.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.5f, 0.75f, -1.64f);
        this.waitCameraPlay(109);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(2.14f, 1.35f, -2.63f);
        this.cam1.setRotate(6.5f, -46.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, 0.584f, 0.187f, -0.79f);
        this.light.setColor(2, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(2, -0.927f, 0.138f, -0.348f);
        this.light.setColor(3, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(3, -0.186f, -0.358f, -0.915f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 36572, 1);
        Runtime.setDefocusQuick(0, 1, 9572, 1);
        this.cam1.setTranslate(0.09f, 1.42f, 6.22f);
        this.cam1.setRotate(-1.5f, -165.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        Runtime.setDefocusQuick(0, 1, 97880, 1);
        Runtime.setDefocusQuick(1, 1, 34880, 1);
        this.cam1.setTranslate(2.91f, 1.46f, -2.83f);
        this.cam1.setRotate(-1.5f, -48.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(12);
        Runtime.setDefocusQuick(0, 1, 10880, 1);
        this.cam1.setTranslate(3.24f, 1.34f, -1.84f);
        this.cam1.setRotate(-1.0f, -15.0f, 0.0f);
        this.cam1.setFov(24.0f);
        this.waitCameraPlay(112);
        Runtime.setDefocusQuick(0, 1, 54880, 1);
        this.cam1.setTranslate(3.12f, 1.2f, -7.83f);
        this.cam1.setRotate(-0.5f, -45.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(212);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        Runtime.setDefocusQuick(1, 1, 19880, 1);
        this.cam1.setTranslate(3.05f, 1.34f, -4.2f);
        this.cam1.setRotate(4.0f, -114.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        Runtime.setDefocusQuick(0, 1, 54880, 1);
        this.cam1.setTranslate(3.12f, 1.2f, -7.83f);
        this.cam1.setRotate(-0.5f, -45.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        Runtime.setDefocusQuick(0, 1, 31880, 1);
        Runtime.setDefocusQuick(0, 1, 14380, 1);
        this.cam1.setTranslate(0.54f, 1.02f, -3.32f);
        this.cam1.setRotate(9.0f, -441.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(60);
        this.rSPL(60, 12.0f, -437.5f, 0.0f, 3);
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
        if (n == 1) {
            this.allen.setVisible(false);
            this.rm1.setVisible(false);
            this.rm2.setVisible(false);
            this.rm3.setVisible(false);
            this.rw1.setVisible(false);
            this.rw2.setVisible(false);
            this.shion.setVisible(true);
            this.kosmos.setVisible(true);
            this.moniDev1.setVisible(false);
            this.moniDev2.setVisible(false);
            this.headset.setVisible(false);
            this.kosmos_tub.setVisible(false);
            this.light.setColor(0, 0.42f, 0.42f, 0.42f);
            this.light.setColor(1, 0.22f, 0.22f, 0.22f);
            this.light.setDirection2(1, 0.163f, 0.596f, -0.786f);
            this.light.setColor(2, 0.38f, 0.38f, 0.38f);
            this.light.setDirection2(2, 0.665f, 0.0f, -0.747f);
            this.light.setColor(3, 0.0f, 0.0f, 0.0f);
            this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        }
        if (n == 0) {
            this.allen.setVisible(true);
            this.rm1.setVisible(true);
            this.rm2.setVisible(true);
            this.rm3.setVisible(true);
            this.rw1.setVisible(true);
            this.rw2.setVisible(true);
            this.kosmos.setVisible(false);
            this.moniDev1.setVisible(true);
            this.headset.setVisible(true);
            this.kosmos_tub.setVisible(true);
            this.light.setColor(0, 0.49f, 0.49f, 0.49f);
            this.light.setColor(1, 0.44f, 0.44f, 0.44f);
            this.light.setDirection2(1, -0.708f, 0.0f, 0.706f);
            this.light.setColor(2, 0.29f, 0.29f, 0.29f);
            this.light.setDirection2(2, -0.72f, 0.549f, -0.424f);
            this.light.setColor(3, 0.47f, 0.47f, 0.47f);
            this.light.setDirection2(3, 0.687f, -0.001f, -0.726f);
        }
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01002_F");
        Runtime.setFlags(2, 1, 1);
        System.println("XEVEJNAME:CFJ1_20 XEVEJPOINT:POINT_20");
        Runtime.jumpCF(230, 3);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(230, 3);
    }

    void defaultLight() {
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, -0.792f, 0.001f, 0.61f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.685f, 0.001f, -0.728f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.772f, 0.001f, 0.635f);
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
        Runtime.setLocation(38);
        Runtime.setLocation(16);
        this.shion = new Characters(0x100001E, 0.45f, 0.0f, 7.3f, -150.0f);
        this.kosmos = new Characters(14, -0.55f, 0.0f, 7.8f, 180.0f);
        this.allen = new Characters(16777532, 4.25f, 0.0f, -4.1f, 120.0f);
        this.rm1 = new Characters(0x1000202, 4.8f, 0.0f, -9.1f, 90.0f);
        this.rm2 = new Chr_faceless(515, -3.8f, 0.0f, -4.1f, -90.0f);
        this.rm3 = new Chr_faceless(516, 0.0f, 1.0f, -18.15f, -90.0f);
        this.rw1 = new Characters(16777733, 4.15f, 0.0f, -9.65f, 60.0f);
        this.rw2 = new Chr_faceless(518, -3.8f, 0.0f, -9.1f, -90.0f);
        this.headset = new Units(24580, 0.0f, 0.0f, 0.0f, 0.0f);
        this.headset.setParent(this.allen, 48);
        this.headset.setTranslate(0.03f, -0.03f, -0.08f);
        this.headset.setRotate(94.0f, 92.2f, 87.0f);
        this.moniDev1 = new Units(24579, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniDev1.setParent(this.rw1, 72);
        this.moniDev1.setTranslate(0.14f, -0.08f, 0.05f);
        this.moniDev1.setRotate(-9.5f, -27.0f, -7.0f);
        this.moniDev1.setScale(10.0f, 10.0f, 10.0f);
        this.moniDev2 = new Units(24579, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniDev2.setParent(this.allen, 60);
        this.moniDev2.setTranslate(0.18f, -0.05f, -0.09f);
        this.moniDev2.setRotate(-26.3f, 35.3f, -114.6f);
        this.moniDev2.setScale(10.0f, 10.0f, 10.0f);
        this.kosmos_tub = new Units(20555, 0.46f, 0.18f, -8.04f, 0.0f);
        Stage.setVisible(46, false);
        Stage.setVisible(45, false);
        Stage.setVisible(47, false);
        Stage.setVisible(43, false);
        Stage.setVisible(42, false);
        Stage.setVisible(44, false);
        this.fm1 = new Monitors(24613, 5.6f, 1.16f, -9.1f, -90.0f);
        this.fm1.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm1.setArgs(1, 20042, 0, 0, 0);
        this.fm1.setArgs(2, 96, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm2 = new Monitors(24613, 5.6f, 1.16f, -4.1f, -90.0f);
        this.fm2.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm2.setArgs(1, 20042, 0, 0, 0);
        this.fm2.setArgs(2, 96, 0, 15, -1);
        this.fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm3 = new Monitors(24613, 5.6f, 1.16f, 0.9f, -90.0f);
        this.fm3.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm3.setArgs(1, 20042, 0, 0, 0);
        this.fm3.setArgs(2, 96, 0, 15, -1);
        this.fm3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm4 = new Monitors(24613, -4.6f, 1.16f, -9.1f, 90.0f);
        this.fm4.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm4.setArgs(1, 20042, 0, 0, 0);
        this.fm4.setArgs(2, 96, 0, 15, -1);
        this.fm4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm5 = new Monitors(24613, -4.6f, 1.16f, -4.1f, 90.0f);
        this.fm5.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm5.setArgs(1, 20042, 0, 0, 0);
        this.fm5.setArgs(2, 96, 0, 15, -1);
        this.fm5.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm6 = new Monitors(24613, -4.6f, 1.16f, 0.9f, 90.0f);
        this.fm6.setArgs(0, 0.0f, 0.0f, 0.9f, 0.5f);
        this.fm6.setArgs(1, 20042, 0, 0, 0);
        this.fm6.setArgs(2, 96, 0, 15, -1);
        this.fm6.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm2.signal(1);
        this.fm3.signal(1);
        this.fm4.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.allen.face, "FLSallen_h.fpk");
        this.loadarc(this.rm1.face, "FLSvec_m.fpk");
        this.loadarc(this.rw1.face, "FLSvec_w.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.kosmos.start(1, "initMTN_kosmos");
        this.rm2.start(1, "initMTN_r");
        this.rm3.start(1, "initMTN_r");
        this.rw2.start(1, "initMTN_r");
        this.allen.signal(0);
        this.rm1.signal(0);
        this.rw1.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.setTranslate(0.0f, 300.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.cam1.change();
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.rm3.setVisible(false);
        this.kosmos_tub.setVisible(false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        System.sleep(1);
        Sound.streamPlay(1191001, 48000);
        this.cut_length = 105;
        this.CameraPlay = 1;
        this.face(this.rm1.face, 2);
        this.face(this.rw1.face, 2);
        this.rm1.start(1, "c1_1");
        this.rw1.start(1, "c1_2");
        System.sleep(60);
        this.msg.print("All processes to 300 cleared.");
        this.face(this.rm1.face, 1, 0, 60, 0, 0, 1.0f);
        this.waitclear(45);
        this.face(this.shion.face, 50, 0, 0, 0, 0, 1.0f);
        this.CameraPlay = 2;
        this.rm1.start(1, "initMTN_r");
        this.allen.start(1, "c2_3");
        this.face(this.allen.face, 2, 0, 45, 0, 0, 1.0f);
        System.sleep(45);
        this.msg.print("Chief, the data transfer\nis going smoothly.");
        this.face(this.allen.face, 1, 0, 18, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.allen.face, 1, 0, 52, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("The A-LINE protocol's\nlooking good.");
        this.face(this.allen.face, 1, 52, 120, 0, 0, 1.0f);
        System.sleep(68);
        this.face(this.allen.face, 2);
        this.waitclear(7);
        this.msg.print("It sure is. The response levels\nare really good.");
        System.sleep(30);
        this.waitclear(75);
        this.msg.print("If we can maintain these levels, \nwe'll have some great results soon.");
        this.face(this.allen.face, 1, 0, 45, 0, 0, 1.0f);
        System.sleep(30);
        this.shion.setVisible(true);
        this.shion.start(1, "c3_5_4");
        System.sleep(15);
        Runtime.setLocation(38);
        this.changeLocation(1);
        this.CameraPlay = 3;
        this.face(this.shion.face, 50, 15, 120, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(30);
        this.msg.print("Yeah...");
        this.face(this.shion.face, 49, 17, 37, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.cut_length = 180;
        this.CameraPlay = 4;
        this.face(this.shion.face, 50);
        System.sleep(105);
        this.msg.print("Chief?");
        this.waitclear(45);
        System.sleep(30);
        this.allen.setMotNoUpdate(2);
        this.allen.mtn(262, 0, 0, 0, 0, 1.0f, true);
        this.allen.setMotionFlags(0x800000, false);
        this.allen.setMotionFlags(0x2000000, true);
        this.CameraPlay = 5;
        System.sleep(45);
        this.msg.print("Hey, Allen...");
        this.face(this.shion.face, 49, 0, 45, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("Let's skip all 300 level processes");
        this.face(this.shion.face, 49, 0, 60, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("and start from 400.");
        this.face(this.shion.face, 49, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        Runtime.setLocation(16);
        this.changeLocation(0);
        this.CameraPlay = 6;
        this.allen.start(1, "c6_7_6");
        this.msg.print("Huh...400? Chief, but that's...");
        this.face(this.allen.face, 5, 0, 90, 8, 1, 1.0f);
        this.waitclear(105);
        this.face(this.allen.face, 6, 0, 75, 0, 0, 1.0f);
        this.msg.print("I feel pretty confident this time.");
        this.waitclear(75);
        this.msg.print("But, Chief!");
        this.face(this.allen.face, 17, 0, 35, 8, 1, 1.0f);
        System.sleep(15);
        this.CameraPlay = 7;
        this.rm1.start(1, "c7_16");
        System.sleep(15);
        this.waitclear(45);
        this.face(this.allen.face, 30, 0, 0, 29, 1, 1.0f);
        System.sleep(30);
        this.msg.print("Remember what happened\nlast time...?");
        this.face(this.allen.face, 29, 0, 52, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Had we waited ten more seconds\nto intervene,");
        this.face(this.allen.face, 29, 0, 120, 0, 0, 1.4f);
        this.waitclear(90);
        this.msg.print("we wouldn't have been able to\n get you back at all.");
        this.face(this.allen.face, 29, 0, 99, 0, 0, 1.3f);
        this.waitclear(90);
        Runtime.setLocation(38);
        this.changeLocation(1);
        this.cut_length = 345;
        this.CameraPlay = 8;
        this.shion.start(1, "c8_7");
        this.face(this.shion.face, 50, 0, 90, 0, 0, 1.0f);
        this.msg.print("Can't we at least \ntest it out in Objective Mode?");
        this.waitclear(90);
        this.msg.print("You know we can't get\nprecise data that way!");
        this.face(this.shion.face, 49, 0, 70, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.msg.print("I'll be fine.\nIf something happens,\nI'll get myself out.");
        this.face(this.shion.face, 49, 0, 17, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.shion.face, 49, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        this.face(this.shion.face, 50, 0, 15, 0, 0, 1.0f);
        System.sleep(15);
        this.msg.print("Besides, you want to test it out\nas well, don't you?");
        this.face(this.shion.face, 49, 0, 16, 0, 0, 1.0f);
        System.sleep(30);
        Runtime.setLocation(16);
        this.changeLocation(0);
        this.cut_length = 345;
        this.moniDev2.setVisible(false);
        this.CameraPlay = 9;
        this.allen.start(1, "c9_8");
        this.face(this.allen.face, 30);
        this.waitclear(60);
        this.msg.print("We spent all night building the\nnative A-LINE protocol...");
        this.waitclear(120);
        this.msg.print("Well, yeah, but...");
        this.face(this.allen.face, 29, 0, 52, 0, 0, 1.0f);
        this.waitclear(60);
        this.face(this.allen.face, 30, 0, 65, 0, 0, 1.0f);
        this.msg.print("That settles it!\nLet's get started.");
        System.sleep(35);
        this.waitclear(30);
        this.face(this.allen.face, 30, 0, 70, 0, 0, 1.0f);
        System.sleep(40);
        this.shion.mtn(265, 30, 30, 0, 0, 1.0f, true);
        this.CameraPlay = 109;
        System.sleep(15);
        this.msg.print("All right then...");
        this.face(this.allen.face, 5, 0, 34, 12, 1, 1.0f);
        this.waitclear(45);
        this.msg.print("But if anything happens, I'm going\nto shut it down from here,");
        this.face(this.allen.face, 5, 0, 99, 0, 0, 1.0f);
        this.waitclear(105);
        Runtime.setLocation(38);
        this.allen.setMotNoUpdate(2);
        this.allen.mtn(266, 0, 0, 0, 0, 1.0f, true);
        this.allen.setMotionFlags(0x800000, false);
        this.allen.setMotionFlags(0x2000000, true);
        this.changeLocation(1);
        this.CameraPlay = 10;
        this.shion.setMotionFlags(0x800000, true);
        this.shion.mtn(265, 30, 270, 0, 0, 1.0f, true);
        this.face(this.shion.face, 4, 0, 60, 0, 0, 1.0f);
        this.msg.print("so don't press your luck!");
        this.waitclear(60);
        this.msg.print("All right, all right...");
        this.face(this.shion.face, 3, 0, 37, 0, 0, 1.0f);
        this.waitclear(45);
        this.face(this.shion.face, 4, 0, 90, 0, 0, 1.0f);
        this.msg.print("And don't deviate from the program...!");
        this.waitclear(90);
        this.msg.print("Okay...Mom.");
        this.face(this.shion.face, 3, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        Runtime.setLocation(16);
        this.changeLocation(0);
        this.CameraPlay = 11;
        this.allen.start(1, "c11_12_10");
        this.face(this.allen.face, 11, 0, 0, 29, 1, 1.0f);
        System.sleep(30);
        this.msg.print("...Jeez. This is so typical\nof her.");
        this.face(this.allen.face, 11, 0, 73, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(15);
        this.CameraPlay = 12;
        this.face(this.rm1.face, 2, 0, 120, 0, 0, 1.0f);
        this.face(this.rw1.face, 2, 0, 120, 0, 0, 1.0f);
        this.moniDev1.setTranslate(0.19f, -0.01f, 0.02f);
        this.moniDev1.setRotate(-1.7f, -7.6f, -100.0f);
        this.rm1.start(1, "c12_11");
        this.rw1.start(1, "c12_12");
        this.msg.print("Will she ever stop to think about \nthe hell I go through for her?");
        this.face(this.allen.face, 11, 18, 120, 0, 0, 1.0f);
        this.waitclear(135);
        this.rw1.setTranslate(4.15f, 0.0f, -9.15f);
        this.CameraPlay = 112;
        this.msg.print("You know, sir, the way you worry\nabout Chief Uzuki all the time,");
        this.face(this.rw1.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.rw1.face, 1, 0, 27, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("I'm surprised you haven't\nkeeled over already.");
        this.face(this.rw1.face, 1, 0, 75, 0, 0, 1.0f);
        this.waitclear(90);
        this.CameraPlay = 212;
        this.allen.start(1, "c12_100_10");
        this.msg.print("H...hey!");
        this.face(this.allen.face, 5, 0, 34, 8, 1, 1.0f);
        this.waitclear(45);
        this.msg.print("That's enough!");
        this.face(this.allen.face, 5, 0, 45, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Is the 400 level program\nready to go?");
        this.face(this.allen.face, 5, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.CameraPlay = 13;
        this.rm1.start(1, "c13_11");
        this.rw1.start(1, "c13_12");
        this.msg.print("We're ready to go,");
        this.face(this.rm1.face, 1, 0, 45, 0, 0, 1.0f);
        this.waitclear(35);
        this.msg.print("anytime.");
        this.face(this.rm1.face, 1, 0, 45, 0, 0, 1.0f);
        this.waitclear(45);
        this.CameraPlay = 14;
        this.allen.start(1, "c14_15");
        this.msg.print("Then start up the program!");
        this.face(this.allen.face, 5, 0, 60, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("I want detailed reports from\nevery monitoring station.");
        this.face(this.allen.face, 5, 0, 100, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("Any abnormalities, and I'm shutting\ndown immediately.");
        this.face(this.allen.face, 5, 0, 90, 0, 0, 1.0f);
        this.waitclear(105);
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
        Runtime.setRegister(1, this.CameraPlay);
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

        void c11_12_10() {
            this.setTranslate(3.9f, 0.0f, -3.6f);
            this.setRotateY(-70.0f);
            this.mtn(266, 0, 285, 0, 0, 1.0f, true);
        }

        void c12_100_10() {
            this.mtn(266, 480, 660, 0, 0, 1.0f, true);
        }

        void c12_11() {
            this.mtn(267, 0, 390, 0, 0, 1.0f, true);
        }

        void c12_12() {
            this.setRotateY(90.0f);
            this.mtn(268, 165, 555, 0, 0, 1.0f, true);
        }

        void c13_11() {
            this.mtn(267, 510, 630, 0, 0, 1.0f, true);
        }

        void c13_12() {
            this.mtn(268, 390, 510, 0, 0, 1.0f, true);
        }

        void c14_15() {
            this.setRotateY(-90.0f);
            this.mtn(271, 0, 255, 0, 0, 0.94f, true);
        }

        void c1_1() {
            this.mtn(257, 45, 150, 0, 0, 1.0f, true);
        }

        void c1_2() {
            this.mtn(258, 0, 1.0f, true);
        }

        void c2_3() {
            this.mtn(259, 0, 210, 0, 0, 0.89f, true);
            this.mtn(259, 210, 450, 0, 0, 1.0f, true);
        }

        void c3_5_4() {
            this.mtn(260, 0, 390, 0, 0, 1.0f, true);
            this.mtn(260, 390, 615, 0, 0, 1.0f, true);
            this.mtn(260, 585, 615, 0, 0, -1.0f, true);
        }

        void c6_7_6() {
            this.mtn(262, 0, 1.0f, true);
        }

        void c7_16() {
            this.mtn(272, 0, 119, 0, 0, 1.0f, true);
            this.mtn(272, 0, 119, 0, 0, 1.0f, true);
            this.mtn(272, 0, 119, 0, 0, 1.0f, true);
        }

        void c8_7() {
            this.mtn(263, 0, 0, 0, 0, 1.0f, false);
            this.setMotionFlags(0x2000000, true);
            System.sleep(15);
            this.setMotionFlags(0x800000, true);
            this.mtn(263, 0, 345, 0, 0, 1.0f, true);
        }

        void c9_8() {
            this.setTranslate(2.97f, 0.0f, -3.37f);
            this.setRotateY(-70.0f);
            this.mtn(264, 0, 1.0f, true);
        }

        void initMTN_kosmos() {
            this.mtn(261, 8, 1.0f, true);
        }

        void initMTN_r() {
            this.mtn(272, 8, 1.0f, false);
        }
    }

    class Chr_faceless
            extends Chr {
        public Chr_faceless(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void initMTN_r() {
            this.mtn(272, 8, 1.0f, true);
        }

        void rm3_act() {
            this.setVisible(false);
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.start(4, null);
        }
    }

    class Monitors
            extends Unit {
        public Monitors(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }
    }
}

