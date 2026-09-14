import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_GNU12_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02054
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_GNU12_PRJ,
        Pack02054,
        FLSchaos,
        FLSshion_h,
        FLSkosmos,
        FLSmomo,
        FLSziggy,
        FLSandrew {
    Characters chaos;
    Characters shion;
    Characters kosmos;
    Characters momo;
    Characters ziggy;
    Characters andrew;
    Characters shion_m;
    Chr zohal;
    MAPUnit doorL;
    MAPUnit doorR;
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

    SCE02054() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.505f, 0.001f, -0.863f);
        this.light.setColor(2, 0.09f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.914f, 0.0f, 0.407f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.906f, -0.413f, -0.093f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 14380, 1);
        this.cam1.setTranslate(-2.03f, 5.01f, 8.93f);
        this.cam1.setRotate(-24.5f, -162.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -2.81f, 5.01f, 9.18f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.165f, 0.131f, -0.977f);
        this.light.setColor(2, 0.12f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.864f, -0.419f, -0.278f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.19f, -0.461f, -0.867f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(-1.84f, 1.41f, 15.16f);
        this.cam1.setRotate(1.0f, -108.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.5f);
        this.light.setDirection2(1, 0.01f, 0.544f, -0.839f);
        this.light.setColor(2, 0.46f, 0.37f, 0.25f);
        this.light.setDirection2(2, 0.011f, 0.811f, -0.585f);
        this.light.setColor(3, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(3, -0.01f, -0.772f, -0.636f);
        this.zohal.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.zohal.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.zohal.light.setDirection2(1, -0.072f, 0.774f, 0.629f);
        this.zohal.light.setColor(2, 0.46f, 0.37f, 0.25f);
        this.zohal.light.setDirection2(2, -0.593f, 0.156f, -0.79f);
        this.zohal.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.zohal.light.setDirection2(3, -0.525f, -0.712f, 0.467f);
        Runtime.setDefocusQuick(0, 1, 5880, 2);
        this.cam1.setTranslate(-2.32f, 1.43f, 17.02f);
        this.cam1.setRotate(19.5f, -11.5f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(15);
        this.rSPL(195, 39.0f, -11.5f, 0.0f, 3);
        this.waitCameraPlay(4);
        this.zohal.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zohal.light.setColor(1, 0.38f, 0.38f, 0.38f);
        this.zohal.light.setDirection2(1, -0.019f, 0.784f, 0.62f);
        this.zohal.light.setColor(2, 0.7f, 0.7f, 0.7f);
        this.zohal.light.setDirection2(2, -0.026f, 0.039f, 0.999f);
        this.zohal.light.setColor(3, 0.35f, 0.34f, 0.44f);
        this.zohal.light.setDirection2(3, -0.016f, -0.931f, -0.365f);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.349f, 0.276f, -0.896f);
        this.light.setColor(2, 0.12f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.864f, -0.419f, -0.278f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.19f, -0.461f, -0.867f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-1.72f, 1.36f, 15.21f);
        this.cam1.setRotate(4.0f, -109.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.718f, 0.196f, -0.668f);
        this.light.setColor(2, 0.14f, 0.14f, 0.18f);
        this.light.setDirection2(2, -0.926f, -0.366f, -0.092f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.19f, -0.461f, -0.867f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 50880, 1);
        this.cam1.setTranslate(-0.2f, 1.84f, 12.88f);
        this.cam1.setRotate(-22.0f, -173.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(45, -29.5f, -161.5f, 0.0f, 3);
        this.waitCameraPlay(105);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.718f, 0.196f, -0.668f);
        this.light.setColor(2, 0.14f, 0.14f, 0.18f);
        this.light.setDirection2(2, -0.926f, -0.366f, -0.092f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.19f, -0.461f, -0.867f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 117880, 2);
        this.cam1.setTranslate(-1.12f, 1.38f, 15.12f);
        this.cam1.setRotate(4.0f, -160.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(205);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.429f, 0.367f, -0.825f);
        this.light.setColor(2, 0.14f, 0.14f, 0.18f);
        this.light.setDirection2(2, -0.926f, -0.366f, -0.092f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.19f, -0.461f, -0.867f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 2);
        this.cam1.setTranslate(-0.57f, 1.38f, 12.88f);
        this.cam1.setRotate(4.0f, -138.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -0.49f, 1.4f, 12.97f);
        this.waitCameraPlay(305);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, -0.682f, 0.502f, -0.532f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.862f, 0.01f, -0.507f);
        this.light.setColor(3, 0.05f, 0.08f, 0.12f);
        this.light.setDirection2(3, -0.091f, -0.687f, 0.721f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.zohal.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.zohal.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.zohal.light.setDirection2(1, -0.072f, 0.774f, 0.629f);
        this.zohal.light.setColor(2, 0.46f, 0.37f, 0.25f);
        this.zohal.light.setDirection2(2, -0.593f, 0.156f, -0.79f);
        this.zohal.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.zohal.light.setDirection2(3, -0.525f, -0.712f, 0.467f);
        Runtime.setDefocusQuick(0, 1, 91880, 2);
        this.cam1.setTranslate(-0.37f, 0.93f, 15.54f);
        this.cam1.setRotate(33.5f, -9.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 15, -0.37f, 0.83f, 15.54f);
        this.rSPL(this.cut_length + 15, 36.5f, -9.5f, 0.0f);
        this.waitCameraPlay(6);
        this.zohal.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zohal.light.setColor(1, 0.38f, 0.38f, 0.38f);
        this.zohal.light.setDirection2(1, -0.019f, 0.784f, 0.62f);
        this.zohal.light.setColor(2, 0.7f, 0.7f, 0.7f);
        this.zohal.light.setDirection2(2, -0.026f, 0.039f, 0.999f);
        this.zohal.light.setColor(3, 0.35f, 0.34f, 0.44f);
        this.zohal.light.setDirection2(3, -0.016f, -0.931f, -0.365f);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.505f, 0.014f, -0.863f);
        this.light.setColor(2, 0.09f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.913f, 0.032f, 0.407f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, -0.906f, -0.413f, -0.093f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 27880, 1);
        this.cam1.setTranslate(-2.85f, 0.22f, 13.63f);
        this.cam1.setRotate(13.5f, -140.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -2.74f, 0.22f, 13.54f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.335f, 0.141f, -0.932f);
        this.light.setColor(2, 0.09f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.914f, 0.0f, 0.407f);
        this.light.setColor(3, 0.09f, 0.09f, 0.09f);
        this.light.setDirection2(3, -0.906f, -0.413f, -0.093f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 2);
        this.cam1.setTranslate(-0.75f, 1.43f, 14.01f);
        this.cam1.setRotate(2.0f, 245.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.324f, 0.001f, -0.946f);
        this.light.setColor(2, 0.09f, 0.12f, 0.16f);
        this.light.setDirection2(2, -0.913f, 0.032f, 0.407f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, -0.906f, -0.413f, -0.093f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 41880, 1);
        this.cam1.setTranslate(-2.97f, 0.35f, 14.31f);
        this.cam1.setRotate(19.5f, 249.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 120, -2.76f, 0.35f, 13.76f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.6f, 0.6f, 0.43f);
        this.light.setDirection2(1, 0.416f, 0.339f, -0.844f);
        this.light.setColor(2, 0.31f, 0.31f, 0.35f);
        this.light.setDirection2(2, -0.66f, -0.477f, -0.58f);
        this.light.setColor(3, 0.12f, 0.12f, 0.12f);
        this.light.setDirection2(3, 0.666f, -0.34f, 0.664f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 109880, 1);
        this.cam1.setTranslate(-0.29f, 1.29f, 13.99f);
        this.cam1.setRotate(6.0f, 151.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.614f, 0.36f, -0.703f);
        this.light.setColor(2, 0.09f, 0.12f, 0.16f);
        this.light.setDirection2(2, 0.098f, 0.327f, 0.94f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 0.762f, -0.648f, 0.013f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 18880, 2);
        this.cam1.setTranslate(0.87f, 0.49f, 18.44f);
        this.cam1.setRotate(4.5f, 401.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.42f, 0.49f, 17.93f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.7f, 0.7f, 0.5f);
        this.light.setDirection2(1, 0.771f, 0.198f, -0.605f);
        this.light.setColor(2, 0.13f, 0.16f, 0.2f);
        this.light.setDirection2(2, 0.561f, 0.136f, 0.816f);
        this.light.setColor(3, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(3, -0.707f, -0.671f, -0.223f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 62880, 2);
        this.cam1.setTranslate(-3.21f, 1.22f, 14.01f);
        this.cam1.setRotate(-18.5f, 372.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(this.cut_length, -21.0f, 372.0f, 0.0f);
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
        System.println("XEVEFLAG:EV02054_F");
        Runtime.setFlags(175, 1, 1);
        System.println("XEVEJNAME:SCE02055");
        Runtime.jumpEvent(2550);
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
        Runtime.setLocation(1711);
        this.chaos = new Characters(0x1000003, 0.9f, 0.0f, 19.2f, 180.0f);
        this.shion = new Characters(0x100001E, -0.9f, 0.0f, 19.2f, 180.0f);
        this.shion_m = new Characters(0x1000001, -0.9f, 0.0f, 19.2f, 180.0f);
        this.shion.setVisible(false);
        this.kosmos = new Characters(0x1000002, 0.0f, 0.0f, 20.0f, 180.0f);
        this.momo = new Characters(0x1000004, -0.7f, 0.0f, 20.5f, 180.0f);
        this.ziggy = new Characters(0x1000006, -1.4f, 0.0f, 20.3f, 180.0f);
        this.andrew = new Characters(0x1000117, -3.5f, -0.0f, 12.0f, 0.0f);
        this.zohal = new Characters(20541, 0.0f, 13.94f, -1.74f, 0.0f);
        this.zohal.start(1, "zohal_light");
        this.zohal.setShadow(0, 0);
        this.doorL = new Mapunits(0);
        this.doorR = new Mapunits(1);
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.andrew.face, "FLSandrew.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.shion.signal(0);
        this.chaos.signal(0);
        this.kosmos.signal(0);
        this.momo.signal(0);
        this.ziggy.signal(0);
        this.andrew.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.andrew.setVisible(false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290046, 48000);
        this.cut_length = 120;
        this.doorL.setTranslate(-1.75f, this.doorL.py, this.doorL.pz);
        this.doorR.setTranslate(1.75f, this.doorL.py, this.doorL.pz);
        this.CameraPlay = 1;
        this.face(this.shion.face, 50);
        this.face(this.chaos.face, 2);
        this.face(this.kosmos.face, 2);
        this.face(this.momo.face, 2);
        this.face(this.ziggy.face, 2);
        this.shion.mtn(257, 15, 255, 0, 0, 1.0f, true);
        this.shion_m.mtn(257, 15, 255, 0, 0, 1.0f, true);
        this.chaos.start(1, "c_c1");
        this.kosmos.start(1, "k_c1");
        this.momo.start(1, "m_c1");
        this.ziggy.start(1, "z_c1");
        System.sleep(30);
        this.doorL.start(1, "closeL");
        this.doorR.start(1, "closeR");
        System.sleep(this.cut_length - 30);
        this.shion.setShadow(9, 32);
        this.kosmos.setShadow(9, 32);
        this.chaos.setShadow(9, 32);
        this.ziggy.setShadow(9, 32);
        this.momo.setShadow(9, 32);
        this.andrew.setShadow(9, 32);
        this.shion_m.setVisible(false);
        this.shion.setVisible(true);
        this.shion.setTranslate(-0.9f, 0.0f, 19.7f);
        this.chaos.setTranslate(0.9f, 0.0f, 16.2f);
        this.CameraPlay = 2;
        this.chaos.start(1, "c_c2");
        this.face(this.shion.face, 50);
        this.face(this.chaos.face, 6);
        System.sleep(45);
        this.msg.print("Shion!");
        this.face(this.chaos.face, 5, 0, 16, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("Look at that...");
        this.face(this.chaos.face, 5, 0, 16, 0, 0, 1.0f);
        this.waitclear(30);
        System.sleep(15);
        this.shion.setTranslate(-0.9f, 0.0f, 15.7f);
        this.shion.mtn(260, 90, 90, 0, 0, 1.0f, true);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.cut_length = 240;
        this.CameraPlay = 3;
        System.sleep(150);
        this.msg.print("...The Zohar?");
        this.waitclear(45);
        System.sleep(45);
        this.chaos.setTranslate(0.9f, 0.0f, 15.7f);
        this.kosmos.setTranslate(0.0f, 0.0f, 17.0f);
        this.ziggy.setTranslate(-0.7f, 0.0f, 17.5f);
        this.momo.setTranslate(-1.4f, 0.0f, 17.3f);
        this.CameraPlay = 4;
        this.chaos.mtn(259, 0, 1.0f, true);
        this.kosmos.mtn(261, 45, 165, 0, 0, 1.0f, true);
        this.ziggy.mtn(273, 8, 1.0f, true);
        this.momo.mtn(275, 8, 1.0f, true);
        this.face(this.chaos.face, 6);
        this.face(this.shion.face, 50);
        this.face(this.kosmos.face, 2);
        this.shion.start(1, "c4_4");
        System.sleep(15);
        this.msg.print("We must go.");
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("Hey, KOS-MOS, wait!");
        this.face(this.shion.face, 5, 0, 46, 16, 1, 1.0f);
        this.waitclear(60);
        this.shion.renderCommand(2);
        this.cut_length = 105;
        this.shion.setTranslate(-0.9f, 0.0f, 15.9f);
        this.chaos.setTranslate(0.9f, 0.0f, 15.9f);
        this.CameraPlay = 5;
        this.shion.mtn(260, 0, 105, 0, 0, 1.0f, true);
        this.kosmos.mtn(261, 165, 268, 0, 0, 1.0f, true);
        System.sleep(30);
        this.msg.print("Don't you go off on your own...");
        this.waitclear(45);
        System.sleep(30);
        this.shion.renderCommand(0);
        this.kosmos.mtn(261, 240, 240, 0, 0, 1.0f, true);
        this.kosmos.setVisible(true);
        this.shion.setTranslate(-0.4f, 0.0f, 16.67f);
        this.shion.setRotate(0.0f, -141.5f, 0.0f);
        this.CameraPlay = 105;
        this.face(this.shion.face, 6, 0, 0, 0, 0, 1.0f);
        this.shion.mtn(265, 225, 360, 0, 0, 1.0f, true);
        this.face(this.shion.face, 5, 0, 17, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("I don't recall\nprogramming her that way.");
        this.face(this.shion.face, 5, 0, 70, 0, 0, 1.0f);
        this.waitclear(90);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.cut_length = 120;
        this.CameraPlay = 205;
        this.kosmos.mtn(261, 240, 268, 0, 0, 0.25f, true);
        System.sleep(this.cut_length);
        this.kosmos.setMotionFlags(0x40000000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.cut_length = 120;
        this.kosmos.setTranslate(0.0f, 0.0f, 14.5f);
        this.CameraPlay = 305;
        this.kosmos.mtn(259, 0, 90, 0, 0, 0.7f, true);
        System.sleep(this.cut_length);
        this.kosmos.mtn(264, 0, 0, 0, 0, 1.0f, true);
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.cut_length = 150;
        this.shion.setTranslate(-0.9f, 0.0f, 15.7f);
        this.shion.setRotateY(180.0f);
        this.CameraPlay = 6;
        this.face(this.shion.face, 12, 0, 60, 0, 0, 1.0f);
        this.face(this.chaos.face, 6);
        this.face(this.momo.face, 2);
        this.face(this.ziggy.face, 2);
        this.chaos.mtn(259, 0, 150, 0, 0, 1.0f, true);
        this.shion.mtn(259, 0, 150, 0, 0, 1.0f, true);
        this.ziggy.mtn(262, 0, 1.0f, true);
        this.momo.mtn(263, 0, 1.0f, true);
        this.msg.print("But what's the Zohar doing here...?");
        this.face(this.shion.face, 11, 0, 90, 0, 0, 1.0f);
        this.waitclear(105);
        this.cut_length = 345;
        this.CameraPlay = 7;
        this.kosmos.mtn(264, 0, 568, 0, 0, 1.0f, true);
        this.msg.print("I have confirmed that this\nobject can be identified\nwith a 99.99998% probability");
        this.face(this.kosmos.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.kosmos.face, 1, 0, 60, 0, 0, 1.0f);
        this.waitclear(70);
        this.msg.print("to be the Zohar Emulator that\nwas stored onboard the Woglinde.");
        this.face(this.kosmos.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(125);
        System.sleep(15);
        this.cut_length = 240;
        this.ziggy.setVisible(true);
        this.momo.setVisible(false);
        this.CameraPlay = 8;
        this.chaos.mtn(259, 0, 165, 0, 0, 0.75f, true);
        this.shion.mtn(265, 0, 238, 0, 0, 1.0f, true);
        this.face(this.kosmos.face, 2, 0, 105, 0, 0, 1.0f);
        this.msg.print("An emulator?");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Are you saying a real one\nexists somewhere else?");
        this.face(this.shion.face, 1, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Affirmative.");
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("Affirmative...?");
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.shion.face, 1, 0, 17, 0, 0, 1.0f);
        System.sleep(20);
        this.face(this.shion.face, 23, 17, 17, 0, 0, 1.0f);
        this.waitclear(10);
        this.chaos.setVisible(false);
        this.CameraPlay = 9;
        this.shion.mtn(265, 240, 645, 0, 0, 1.0f, true);
        this.kosmos.mtn(269, 8, 1.0f, true);
        this.ziggy.mtn(273, 8, 1.0f, true);
        this.msg.print("KOS-MOS, how do\nyou know all of this?");
        this.face(this.shion.face, 23, 17, 120, 0, 0, 1.0f);
        this.waitclear(120);
        this.msg.print("I never stored any type of information\nlike that in your main databank...");
        this.face(this.shion.face, 23, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.face(this.shion.face, 24, 0, 120, 0, 0, 1.0f);
        this.msg.print("You...you're the creator\nof that thing...");
        System.sleep(45);
        this.face(this.andrew.face, 3);
        this.cut_length = 150;
        this.ziggy.setRotateY(210.0f);
        this.CameraPlay = 10;
        this.waitclear(75);
        System.sleep(15);
        this.msg.print("And yet...");
        this.waitclear(60);
        this.andrew.setVisible(true);
        this.CameraPlay = 11;
        this.andrew.mtn(266, 0, 1.0f, true);
        this.msg.print("you didn't even know about that...?");
        this.face(this.andrew.face, 3, 0, 76, 0, 0, 1.0f);
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
            this.setShadow(4, 32);
        }

        void c4_4() {
            this.mtn(260, 90, 165, 0, 0, 1.0f, true);
            this.mtn(260, 90, 165, 0, 0, -1.0f, true);
        }

        void c_c1() {
            this.getTranslate();
            this.mtn(272, 8, 0.6f, false);
            this.move(SCE02054.this.cut_length - 1, this.px, this.pz - 3.0f, true);
        }

        void c_c2() {
            this.mtn(258, 15, 135, 0, 0, 1.0f, true);
        }

        void k_c1() {
            this.getTranslate();
            this.mtn(277, 8, 0.6f, false);
            this.move(SCE02054.this.cut_length, this.px, this.pz - 3.0f, true);
        }

        void m_c1() {
            this.getTranslate();
            this.mtn(276, 8, 0.6f, false);
            this.move(SCE02054.this.cut_length, this.px, this.pz - 3.0f, true);
        }

        void z_c1() {
            this.getTranslate();
            this.mtn(274, 8, 0.6f, false);
            this.move(SCE02054.this.cut_length, this.px, this.pz - 3.0f, true);
        }

        void zohal_light() {
            this.setLightMode(1);
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
            this.start(4, null);
        }

        void closeL() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(-1.75f + 0.03888889f * (float) n, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void closeR() {
            int n = 1;
            while (n <= 45) {
                this.setTranslate(1.75f - 0.03888889f * (float) n, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void openL() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(0.0f - 0.03888889f * (float) n, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void openR() {
            int n = 1;
            while (n <= 45) {
                this.setTranslate(0.0f + 0.03888889f * (float) n, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }
    }
}

