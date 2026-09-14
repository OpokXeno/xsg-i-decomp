import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_GNU01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02047
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_GNU01_PRJ,
        Pack02047,
        FLSchaos,
        FLSshion_h,
        FLSkosmos,
        FLSmomo,
        FLSziggy {
    Characters chaos;
    Characters shion;
    Characters kosmos;
    Characters momo;
    Characters ziggy;
    Characters momo_h;
    Characters kosmos2;
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

    SCE02047() {
    }

    void AllShadowOff() {
        this.shion.setShadow(0, 0);
        this.chaos.setShadow(0, 0);
        this.ziggy.setShadow(0, 0);
        this.momo.setShadow(0, 0);
        this.kosmos.setShadow(0, 0);
    }

    void AllShadowOn() {
        this.shion.setShadow(7, 48);
        this.chaos.setShadow(7, 48);
        this.ziggy.setShadow(7, 48);
        this.momo.setShadow(7, 48);
        this.kosmos.setShadow(7, 48);
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        this.shion.setShadow(5, 64);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(1, 0.918f, 0.364f, 0.159f);
        this.light.setColor(2, 0.2f, 0.21f, 0.2f);
        this.light.setDirection2(2, -0.256f, -0.71f, -0.656f);
        this.light.setColor(3, 0.19f, 0.19f, 0.16f);
        this.light.setDirection2(3, 0.528f, 0.705f, 0.473f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 85880, 3);
        this.cam1.setTranslate(1.04f, 0.43f, 7.54f);
        this.cam1.setRotate(-20.5f, 56.5f, 0.0f);
        this.cam1.setFov(15.0f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.08f, 0.09f, 0.09f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.607f, 0.444f, -0.659f);
        this.light.setColor(2, 0.21f, 0.22f, 0.21f);
        this.light.setDirection2(2, 0.34f, 0.0f, 0.94f);
        this.light.setColor(3, 0.18f, 0.18f, 0.15f);
        this.light.setDirection2(3, 0.672f, -0.718f, 0.181f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.97f, 0.27f, 6.88f);
        this.cam1.setRotate(24.0f, 124.87f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.08f, 0.09f, 0.09f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.71f, 0.333f, -0.62f);
        this.light.setColor(2, 0.19f, 0.2f, 0.19f);
        this.light.setDirection2(2, -0.936f, 0.119f, 0.331f);
        this.light.setColor(3, 0.14f, 0.14f, 0.11f);
        this.light.setDirection2(3, 0.328f, -0.487f, -0.809f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 34880, 1);
        this.cam1.setTranslate(0.76f, 0.68f, 5.88f);
        this.cam1.setRotate(5.5f, 169.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.298f, 0.848f, -0.438f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, 0.998f, 0.0f, 0.057f);
        this.light.setColor(3, 0.29f, 0.29f, 0.26f);
        this.light.setDirection2(3, 0.37f, -0.76f, -0.535f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 2);
        this.cam1.setTranslate(1.56f, 0.7f, 10.96f);
        this.cam1.setRotate(-22.5f, 146.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.08f, 0.09f, 0.09f);
        this.light.setColor(1, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(1, 0.71f, 0.334f, -0.62f);
        this.light.setColor(2, 0.19f, 0.2f, 0.19f);
        this.light.setDirection2(2, -0.936f, 0.12f, 0.331f);
        this.light.setColor(3, 0.14f, 0.14f, 0.11f);
        this.light.setDirection2(3, 0.328f, -0.488f, -0.809f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 23880, 1);
        this.cam1.setTranslate(1.54f, 0.76f, 5.79f);
        this.cam1.setRotate(9.0f, 146.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.74f, 0.76f, 5.93f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.866f, 0.494f, -0.077f);
        this.light.setColor(2, 0.25f, 0.26f, 0.25f);
        this.light.setDirection2(2, -0.767f, 0.607f, 0.208f);
        this.light.setColor(3, 0.21f, 0.21f, 0.19f);
        this.light.setDirection2(3, 0.355f, -0.721f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(1.94f, 0.95f, 8.68f);
        this.cam1.setRotate(7.5f, 162.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.72f, 0.95f, 8.61f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.866f, 0.494f, -0.077f);
        this.light.setColor(2, 0.25f, 0.26f, 0.25f);
        this.light.setDirection2(2, -0.767f, 0.607f, 0.208f);
        this.light.setColor(3, 0.21f, 0.21f, 0.19f);
        this.light.setDirection2(3, 0.355f, -0.721f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 11880, 1);
        this.cam1.setTranslate(7.51f, 15.05f, 6.9f);
        this.cam1.setRotate(-60.0f, 109.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 7.51f, 20.0f, 6.9f);
        this.rSPL(this.cut_length, -69.0f, 109.5f, 10.0f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.79f, 0.011f, 0.613f);
        this.light.setColor(2, 0.14f, 0.15f, 0.14f);
        this.light.setDirection2(2, -0.049f, 0.414f, 0.909f);
        this.light.setColor(3, 0.14f, 0.14f, 0.12f);
        this.light.setDirection2(3, -0.737f, -0.596f, -0.319f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 88880, 1);
        this.cam1.setTranslate(0.24f, 1.2f, 8.63f);
        this.cam1.setRotate(13.5f, 18.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.866f, 0.493f, -0.077f);
        this.light.setColor(2, 0.13f, 0.14f, 0.13f);
        this.light.setDirection2(2, -0.904f, 0.414f, 0.109f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, 0.355f, -0.721f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 139880, 1);
        this.cam1.setTranslate(-0.85f, 1.53f, 12.26f);
        this.cam1.setRotate(-1.0f, 143.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.27f, 0.27f, 0.27f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, 0.56f, 0.131f, 0.818f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.496f, 0.001f, -0.868f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.027f, -0.999f, -0.035f);
        Runtime.setDefocusQuick(0, 1, 7880, 1);
        this.cam1.setTranslate(6.28f, 5.87f, 7.09f);
        this.cam1.setRotate(-14.5f, 24.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 3.82f, 5.87f, 8.19f);
        this.waitCameraPlay(12);
        this.cam1.setTranslate(-0.22f, 0.48f, -11.35f);
        this.cam1.setRotate(4.5f, -106.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -0.55f, 0.48f, -10.2f);
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(1, 0.86f, 0.243f, 0.448f);
        this.light.setColor(2, 0.13f, 0.14f, 0.13f);
        this.light.setDirection2(2, -0.919f, 0.014f, 0.394f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, 0.47f, -0.778f, 0.417f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 43880, 1);
        this.cam1.setTranslate(0.54f, 1.52f, 10.37f);
        this.cam1.setRotate(-2.0f, 18.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.823f, 0.0f, -0.568f);
        this.light.setColor(2, 0.2f, 0.21f, 0.2f);
        this.light.setDirection2(2, 0.924f, 0.0f, 0.381f);
        this.light.setColor(3, 0.18f, 0.18f, 0.16f);
        this.light.setDirection2(3, -0.788f, -0.544f, -0.288f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 114880, 1);
        this.cam1.setTranslate(1.76f, 1.15f, 8.88f);
        this.cam1.setRotate(3.0f, 153.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(15);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(1, 0.887f, 0.09f, -0.452f);
        this.light.setColor(2, 0.2f, 0.21f, 0.2f);
        this.light.setDirection2(2, 0.979f, 0.0f, 0.203f);
        this.light.setColor(3, 0.16f, 0.16f, 0.13f);
        this.light.setDirection2(3, -0.871f, -0.292f, -0.395f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 40880, 1);
        this.cam1.setTranslate(1.45f, 1.62f, 10.49f);
        this.cam1.setRotate(0.0f, 135.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(165);
        this.rSPL(75, -5.0f, 119.0f, 0.0f, 3);
        this.waitCameraPlay(16);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(1, 0.056f, 0.216f, 0.975f);
        this.light.setColor(2, 0.14f, 0.15f, 0.14f);
        this.light.setDirection2(2, 0.718f, 0.001f, 0.696f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, -0.663f, -0.742f, -0.098f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 98880, 1);
        this.cam1.setTranslate(-0.25f, 1.39f, 9.25f);
        this.cam1.setRotate(-0.5f, -4.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(17);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.866f, 0.494f, -0.077f);
        this.light.setColor(2, 0.13f, 0.14f, 0.13f);
        this.light.setDirection2(2, -0.904f, 0.413f, 0.109f);
        this.light.setColor(3, 0.18f, 0.18f, 0.16f);
        this.light.setDirection2(3, 0.355f, -0.721f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 103880, 1);
        this.cam1.setTranslate(-1.08f, 1.44f, 10.38f);
        this.cam1.setRotate(0.5f, 164.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(18);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(1, 0.056f, 0.216f, 0.975f);
        this.light.setColor(2, 0.14f, 0.15f, 0.14f);
        this.light.setDirection2(2, 0.718f, 0.001f, 0.696f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, -0.663f, -0.742f, -0.098f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 98880, 1);
        this.cam1.setTranslate(-0.25f, 1.39f, 9.25f);
        this.cam1.setRotate(-0.5f, -4.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(19);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(1, 0.887f, 0.09f, -0.452f);
        this.light.setColor(2, 0.2f, 0.21f, 0.2f);
        this.light.setDirection2(2, 0.979f, 0.0f, 0.203f);
        this.light.setColor(3, 0.16f, 0.16f, 0.13f);
        this.light.setDirection2(3, -0.871f, -0.292f, -0.395f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 25880, 1);
        this.cam1.setTranslate(1.92f, 1.67f, 10.49f);
        this.cam1.setRotate(-3.0f, 118.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(20);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.758f, 0.001f, 0.652f);
        this.light.setColor(2, 0.19f, 0.2f, 0.19f);
        this.light.setDirection2(2, -0.304f, 0.523f, 0.796f);
        this.light.setColor(3, 0.16f, 0.16f, 0.14f);
        this.light.setDirection2(3, 0.563f, -0.814f, 0.142f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.84f, 1.16f, 8.66f);
        this.cam1.setRotate(13.0f, 57.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.84f, 1.11f, 8.66f);
        this.waitCameraPlay(21);
        this.light.setColor(0, 0.05f, 0.06f, 0.06f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.81f, 0.022f, -0.586f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, -0.983f, 0.024f, -0.182f);
        this.light.setColor(3, 0.14f, 0.16f, 0.14f);
        this.light.setDirection2(3, 0.752f, -0.633f, -0.183f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(0.32f, 1.27f, 8.56f);
        this.cam1.setRotate(12.5f, 158.0f, 0.0f);
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
        System.println("XEVEFLAG:EV02047_F");
        Runtime.setFlags(168, 1, 1);
        System.println("XEVEJNAME:SCE02048");
        Runtime.jumpEvent(2480);
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
        Runtime.setLocation(1700);
        this.chaos = new Characters(0x1000003, 0.0f, 0.0f, 11.45f, 180.0f);
        this.shion = new Characters(0x100001E, 0.0f, 0.0f, 7.6f, 180.0f);
        this.kosmos = new Characters(0x1000002, -1.35f, 0.0f, 13.0f, 180.0f);
        this.kosmos2 = new Characters(0x1000002, -1.35f, 0.0f, 13.0f, 180.0f);
        this.momo = new Characters(0x1000004, 1.3f, 0.0f, 11.4f, 180.0f);
        this.momo_h = new Characters(0x1000021, 1.3f, 0.0f, 11.4f, 180.0f);
        this.momo_h.setVisible(false);
        this.ziggy = new Characters(0x1000006, 0.8f, 0.0f, 11.4f, 180.0f);
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.kosmos2.face, "FLSkosmos.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.shion.signal(0);
        this.chaos.signal(0);
        this.kosmos.signal(0);
        this.momo.signal(0);
        this.ziggy.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        this.shion.setVisible(true);
        Sound.streamPlay(1290042, 48000);
        this.shion.setTranslate(0.3f, 0.05f, 7.6f);
        this.CameraPlay = 1;
        this.face(this.shion.face, 18, 53, 53, 0, 0, 1.0f);
        this.shion.mtn(257, 0, 253, 0, 0, 1.0f, true);
        System.sleep(90);
        System.sleep(30);
        this.face(this.shion.face, 18, 53, 60, 0, 0, 0.5f);
        System.sleep(30);
        this.face(this.shion.face, 18, 53, 83, 0, 0, 1.0f);
        this.waitclear(30);
        this.face(this.shion.face, 18, 0, 75, 0, 0, 1.0f);
        System.sleep(75);
        this.AllShadowOn();
        this.shion.setShadow(7, 48);
        this.cut_length = 270;
        this.shion.setMotionFlags(0x40000000, false);
        this.shion.setTranslate(0.0f, 0.0f, 7.6f);
        this.CameraPlay = 2;
        this.face(this.shion.face, 18);
        this.shion.start(1, "c2_1");
        System.sleep(150);
        this.msg.print("Where...where am I...?");
        this.face(this.shion.face, 17, 0, 17, 0, 0, 1.0f);
        System.sleep(20);
        this.face(this.shion.face, 17, 0, 17, 0, 0, 1.0f);
        this.waitclear(55);
        this.face(this.shion.face, 18);
        System.sleep(30);
        this.face(this.shion.face, 49, 0, 10, 8, 1, 1.0f);
        System.sleep(30);
        this.AllShadowOff();
        this.face(this.momo_h.face, 280, 0, 0, 0, 0, 1.0f);
        this.chaos.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.kosmos.setVisible(true);
        this.cut_length = 165;
        this.ziggy.setRotateY(90.0f);
        this.CameraPlay = 3;
        this.face(this.momo.face, 1, 95, 95, 0, 0, 1.0f);
        this.chaos.mtn(258, 0, 0.9f, true);
        this.ziggy.mtn(259, 0, 300, 0, 0, 1.0f, true);
        this.momo.mtn(260, 0, 300, 0, 0, 1.0f, true);
        this.momo_h.mtn(260, 60, 300, 0, 0, 0.8f, true);
        this.kosmos.mtn(279, 8, 1.0f, true);
        this.face(this.chaos.face, 2);
        System.sleep(60);
        this.msg.print("You're all...");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(30);
        this.momo.setVisible(false);
        this.momo_h.setVisible(true);
        this.CameraPlay = 4;
        this.face(this.momo_h.face, 280, 0, 120, 0, 0, 1.0f);
        System.sleep(60);
        this.waitclear(30);
        System.sleep(45);
        this.AllShadowOff();
        this.momo.setVisible(true);
        this.momo_h.setVisible(false);
        this.cut_length = 280;
        this.ziggy.setRotateY(90.0f);
        this.chaos.setTranslate(0.0f, 0.0f, 9.45f);
        this.shion.setTranslate(0.0f, 0.0f, 7.6f);
        this.shion.setRotateY(0.0f);
        this.CameraPlay = 5;
        this.face(this.shion.face, 2, 0, 90, 0, 0, 1.0f);
        this.chaos.mtn(262, 0, 270, 0, 0, 0.97f, true);
        this.shion.start(1, "c5_5");
        this.msg.print("I'm glad we're all okay.");
        this.face(this.chaos.face, 3, 0, 39, 0, 0, 1.0f);
        this.waitclear(40);
        this.face(this.chaos.face, 2, 0, 75, 0, 0, 1.0f);
        this.msg.print("chaos, where are we?");
        this.face(this.shion.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("I'm not sure.");
        this.face(this.chaos.face, 1, 0, 16, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("I remember we were all engulfed\nby this light, and then...");
        this.face(this.chaos.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(135);
        this.AllShadowOn();
        this.cut_length = 210;
        this.ziggy.setRotateY(180.0f);
        this.kosmos.setVisible(false);
        this.CameraPlay = 7;
        this.momo.mtn(263, 30, 210, 0, 0, 1.0f, true);
        this.ziggy.mtn(264, 0, 180, 0, 0, 1.0f, true);
        this.msg.print("Wait a second...");
        this.face(this.momo.face, 5, 0, 15, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("This feeling...");
        this.face(this.momo.face, 5, 0, 36, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(45);
        this.waitclear(30);
        this.AllShadowOff();
        this.cut_length = 210;
        this.kosmos.setTranslate(-1.35f, 0.0f, 12.0f);
        this.kosmos.setVisible(true);
        this.CameraPlay = 8;
        System.sleep(15);
        this.msg.print("Yes, I'm sure of it.");
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("We're inside a Gnosis!");
        this.waitclear(90);
        System.sleep(15);
        this.AllShadowOn();
        this.kosmos.mtn(266, 0, 0, 0, 0, 1.0f, true);
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.kosmos.setVisible(true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.kosmos.setTranslate(-1.35f, 0.0f, 13.0f);
        this.CameraPlay = 9;
        this.face(this.shion.face, 50, 0, 15, 0, 0, 1.0f);
        this.shion.mtn(265, 15, 135, 0, 0, 1.0f, true);
        this.msg.print("Inside a Gnosis...");
        this.face(this.shion.face, 49, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Huh?!");
        this.face(this.shion.face, 41, 0, 45, 0, 0, 1.0f);
        this.waitclear(45);
        this.CameraPlay = 10;
        this.kosmos.mtn(266, 0, 1.0f, true);
        this.msg.print("To be more specific, we are\nlocated at the approximate\ncenter of a giant Gnosis.");
        this.face(this.kosmos.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(15);
        this.cut_length = 240;
        this.CameraPlay = 11;
        System.sleep(15);
        this.msg.print("This Gnosis is nearly\n16,000 kilometers in diameter.");
        this.waitclear(150);
        System.sleep(15);
        this.msg.print("To date, the only Gnosis of\nthis size on record is");
        System.sleep(60);
        this.face(this.shion.face, 18, 0, 0, 0, 0, 1.0f);
        this.shion.setVisible(true);
        this.shion.mtn(267, 0, 0, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.cut_length = 210;
        this.CameraPlay = 12;
        this.waitclear(45);
        this.msg.print("the one named Cathedral Ship.");
        this.waitclear(105);
        System.sleep(45);
        this.kosmos2.setMotNoUpdate(2);
        this.kosmos2.mtn(279, 0, 0, 0, 0, 1.0f, true);
        this.kosmos2.setMotionFlags(0x800000, false);
        this.kosmos2.setMotionFlags(0x2000000, true);
        this.momo.mtn(269, 0, 0, 0, 0, 1.0f, true);
        this.momo.setVisible(true);
        this.momo.setMotionFlags(0x800000, false);
        this.momo.setMotionFlags(0x2000000, true);
        this.face(this.momo.face, 11, 0, 0, 0, 0, 1.0f);
        this.ziggy.setVisible(false);
        this.kosmos.setVisible(false);
        this.shion.setTranslate(0.0f, 0.0f, 8.0f);
        this.chaos.setRotateY(-15.0f);
        this.CameraPlay = 13;
        this.face(this.shion.face, 18, 0, 15, 0, 0, 1.0f);
        this.face(this.chaos.face, 6, 0, 90, 0, 0, 1.0f);
        this.shion.mtn(267, 0, 1.0f, true);
        this.chaos.mtn(268, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Does that mean...we were eaten?");
        this.face(this.shion.face, 17, 0, 17, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.shion.face, 17, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("That may not be far from the truth...");
        this.face(this.chaos.face, 5, 0, 62, 0, 0, 1.0f);
        this.waitclear(90);
        this.kosmos2.setVisible(true);
        this.kosmos.setVisible(false);
        this.kosmos.setMotNoUpdate(2);
        this.kosmos.mtn(271, 15, 15, 0, 0, 1.0f, true);
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.momo.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setTranslate(1.3f, 0.0f, 9.9f);
        this.CameraPlay = 14;
        this.kosmos2.mtn(279, 8, 1.0f, true);
        this.ziggy.mtn(270, 8, 1.0f, true);
        this.momo.mtn(269, 0, 1.0f, true);
        this.msg.print("I don't sense any activity\nfrom the Gnosis outside.");
        this.face(this.momo.face, 11, 0, 60, 0, 0, 1.0f);
        System.sleep(60);
        this.face(this.momo.face, 11, 0, 16, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("It's almost as if they're sleeping...");
        this.face(this.momo.face, 11, 0, 51, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.msg.print("Hey! What about the Elsa?");
        this.face(this.momo.face, 11, 0, 16, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.momo.face, 11, 0, 16, 0, 0, 1.0f);
        this.waitclear(25);
        this.msg.print("Where's the Captain\nand everyone else?");
        this.face(this.momo.face, 11, 0, 51, 0, 0, 1.0f);
        this.waitclear(65);
        this.kosmos.setVisible(true);
        this.kosmos2.setVisible(false);
        this.face(this.shion.face, 49, 0, 0, 0, 0, 1.0f);
        this.shion.setVisible(true);
        this.shion.mtn(272, 15, 15, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.CameraPlay = 15;
        this.kosmos.mtn(271, 15, 270, 0, 0, 1.0f, true);
        this.face(this.kosmos.face, 2);
        this.msg.print("Hopefully, they too are alive\nand in here somewhere...");
        this.face(this.ziggy.face, 1, 0, 98, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(90);
        this.ziggy.look_speed(0.1f);
        this.ziggy.look_point(-1.46f, 1.62f, 10.41f);
        System.sleep(60);
        this.chaos.setTranslate(0.0f, 0.0f, 9.45f);
        this.shion.setTranslate(0.0f, 0.0f, 8.0f);
        this.shion.setRotateY(0.0f);
        this.momo.setTranslate(1.3f, 0.0f, 9.9f);
        this.AllShadowOff();
        this.ziggy.look_default();
        this.CameraPlay = 16;
        this.shion.mtn(272, 15, 103, 0, 0, 1.0f, true);
        this.msg.print("...What is it, KOS-MOS?");
        this.face(this.shion.face, 49, 0, 17, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.shion.face, 49, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        this.shion.mtn(274, 0, 0, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.kosmos.setTranslate(-1.35f, 0.0f, 11.5f);
        this.CameraPlay = 17;
        this.kosmos.mtn(273, 15, 420, 0, 0, 1.0f, true);
        this.msg.print("My sensors have picked up a signal...");
        this.face(this.kosmos.face, 1, 0, 43, 0, 0, 1.0f);
        this.waitclear(60);
        this.face(this.kosmos.face, 2, 0, 105, 0, 0, 1.0f);
        this.msg.print("A signal...is it the Elsa?!");
        this.waitclear(90);
        this.msg.print("Yes. And something else near it.");
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.kosmos.face, 1, 0, 43, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("Although it is faint, I am picking up\na waveform similar to the Zohar\nthat was taken from the Woglinde.");
        this.face(this.kosmos.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.kosmos.face, 1, 0, 29, 0, 0, 1.0f);
        this.waitclear(45);
        this.kosmos.setMotionFlags(0x40000000, false);
        this.CameraPlay = 18;
        this.shion.mtn(274, 0, 180, 0, 0, 0.92f, true);
        this.msg.print("The Zohar? You mean that gold-\ncolored object that was in the hangar?");
        this.face(this.shion.face, 49, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.shion.face, 49, 0, 17, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("That thing's here?");
        this.face(this.shion.face, 49, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        this.AllShadowOn();
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.kosmos.setRotateY(165.0f);
        this.CameraPlay = 19;
        this.ziggy.mtn(275, 15, 195, 0, 0, 1.0f, true);
        this.kosmos.mtn(276, 15, 195, 0, 0, 1.0f, true);
        this.face(this.kosmos.face, 2, 0, 60, 0, 0, 1.0f);
        this.msg.print("How far is the Elsa from here?");
        this.face(this.ziggy.face, 1, 0, 44, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Approximately 10 kilometers\nfrom our current position.");
        this.face(this.kosmos.face, 1, 0, 68, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("Not too far...");
        this.face(this.ziggy.face, 1, 0, 25, 0, 0, 1.0f);
        this.waitclear(45);
        this.face(this.chaos.face, 5, 0, 0, 0, 0, 1.0f);
        this.cut_length = 120;
        this.CameraPlay = 20;
        this.shion.mtn(277, 0, 1.0f, true);
        this.face(this.shion.face, 50, 0, 15, 0, 0, 1.0f);
        this.msg.print("Stay here or find the Elsa.");
        this.face(this.shion.face, 49, 0, 17, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("I'm worried about Allen\nand the others...");
        this.face(this.shion.face, 49, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.kosmos.setVisible(false);
        this.chaos.setRotateY(180.0f);
        this.CameraPlay = 21;
        this.chaos.mtn(278, 0, 1.0f, true);
        System.sleep(15);
        System.sleep(20);
        this.msg.print("Looks like we have no choice...");
        this.face(this.chaos.face, 5, 0, 55, 0, 0, 1.0f);
        this.waitclear(70);
        System.sleep(30);
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
        Runtime.setRegister(1, this.CameraPlay);
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
            this.setVisible(false);
        }

        void c2_1() {
            this.mtn(257, 255, 510, 0, 0, 1.0f, true);
            this.mtn(257, 510, 570, 0, 0, 1.5f, true);
            this.mtn(257, 570, 707, 0, 0, 1.0f, true);
        }

        void c5_5() {
            this.mtn(261, 0, 170, 0, 0, 1.0f, true);
            this.mtn(261, 115, 170, 0, 0, -0.5f, true);
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

