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
import xeno.map.MC_GNU05_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02052
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_GNU05_PRJ,
        Pack02052,
        FLSshion,
        FLSziggy,
        FLSmomo,
        FLSkosmos,
        FLSchaos {
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    public static final float nc = 1.0E7f;
    Effect fadeIn;
    Effect fadeOut;
    Effect screenFade1;
    Effect screenFade2;
    Effect flash;
    public static final int Twohand = 0;
    public static final int Rhand = 16;
    public static final int Lhand = 32;
    public static final int Open = 0;
    public static final int Close = 1;
    Menu menu;
    Window win;
    Thread thread1;
    Thread thread2;
    Thread _spl_thread_main;
    Input pad1;
    Input pad0;
    int ziggy_flag = 0;
    int momo_flag = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camera cam5;
    Camerawork camerawork = new Camerawork();
    robo ziggy;
    robo momo;
    robo shion;
    robo kosms;
    robo chaos;
    MAPUnit haikei;
    MAPUnit bill1;
    MAPUnit bill2;
    units dummy1;
    units dummy2;
    units dummy3;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    float global_zensin_power = -0.009f;
    int cam_mize_a = 20;
    int cam_mize_b = 7;
    int cam_mize_c = 20;
    int cam_mize_e = 7;
    int cam_mize_f = 7;
    int cam_mize_g = 7;
    int ziggy_mize_a = 20;
    int ziggy_mize_b = 7;
    int ziggy_mize_c = 20;
    int ziggy_mize_e = 7;
    int ziggy_mize_f = 7;
    int ziggy_mize_g = 7;
    int cam_rnd_mize_flag = 0;
    int cam_flag = 0;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02052() {
    }

    void FACE(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 8, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 9, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void MSGW(String string) {
        this.msg.clear();
        this.msg.print(string);
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

    void __chracter_splne() {
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        float f4 = 0.0f;
        float f5 = 0.0f;
        float f6 = 0.0f;
        float f7 = 0.0f;
        while (true) {
            if (this.cam_flag == 1) {
                this.cam2.setTranslate(this.cam1.getTranslateX() + this.dummy1.px, this.cam1.getTranslateY() + this.dummy1.py, this.cam1.getTranslateZ() + this.dummy1.pz);
                this.cam2.setRotate(this.cam1.getRotateX() + this.dummy1.rx, this.cam1.getRotateY() + this.dummy1.ry, this.cam1.getRotateZ() + this.dummy1.rz);
                this.cam2.setFov(this.cam3.getRotateX());
            }
            if (this.momo_flag == 1) {
                this.momo.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.momo.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.ziggy_flag == 1) {
                this.ziggy.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY(), this.cam4.getTranslateZ());
                this.ziggy.setRotate(this.cam4.getRotateX(), this.cam4.getRotateY(), this.cam4.getRotateZ());
            }
            if (this.cam_rnd_mize_flag == 1) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = (f % (float) this.cam_mize_a + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f3 = (f % (float) this.cam_mize_b + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f4 = (f % (float) this.cam_mize_c + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f5 = (f % (float) this.cam_mize_e + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f6 = (f % (float) this.cam_mize_f + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f7 = (f % (float) this.cam_mize_g + (f - (float) ((int) f))) / 100.0f;
                this.cam2.setTranslate(this.cam1.getTranslateX() + f2, this.cam1.getTranslateY() + f3, this.cam1.getTranslateZ() + f4);
                this.cam2.setRotate(this.cam1.getRotateX() + f5, this.cam1.getRotateY() + f6, this.cam1.getRotateZ() + f7);
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02052_F");
        Runtime.setFlags(173, 1, 1);
        System.println("XEVEJNAME:SCE02053");
        Runtime.jumpEvent(2530);
    }

    void init() {
        Runtime.setLocation(1704);
        this.ziggy = new robo(0x1000006, -1.5f, 0.0f, 0.0f, 180.0f);
        this.momo = new robo(0x1000004, -1.0f, 0.0f, 0.0f, 180.0f);
        this.shion = new robo(0x1000001, 1.0f, 0.0f, 0.0f, 180.0f);
        this.kosms = new robo(0x1000002, 1.5f, 0.0f, 0.0f, 180.0f);
        this.chaos = new robo(0x1000003, 2.0f, 0.0f, 0.0f, 180.0f);
        this.ziggy.setMotNoUpdate(2);
        this.momo.setMotNoUpdate(2);
        this.shion.setMotNoUpdate(2);
        this.kosms.setMotNoUpdate(2);
        this.chaos.setMotNoUpdate(2);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = -2147483391;
        this.fadeOut.args[1] = 60;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 12;
        this.fadeIn.args[2] = 0;
        this.screenFade1 = new Effect(0);
        this.screenFade1.args[0] = -2130706433;
        this.screenFade1.args[1] = 45;
        this.screenFade1.args[2] = 1;
        this.screenFade2 = new Effect(0);
        this.screenFade2.args[0] = -2130706433;
        this.screenFade2.args[1] = 45;
        this.screenFade2.args[2] = 0;
        this.flash = new Effect(0);
        this.flash.args[0] = -2130706433;
        this.flash.args[1] = 3;
        this.flash.args[2] = 0;
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam4 = Camera.create(4);
        this.cam5 = Camera.create(5);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.bill1 = new mirror_map();
        this.bill1.init(11);
        this.bill1.start(4, null);
        this.bill1.setTranslate(-7.81f, -2.44f, 14.51f);
        this.bill1.setRotate(0.0f, -80.0f, 0.0f);
        this.bill1.setScale(1.0f, 1.27f, 1.0f);
        this.bill2 = new mirror_map();
        this.bill2.init(15);
        this.bill2.start(4, null);
        this.bill2.setTranslate(7.71f, 4.27f, -12.54f);
        this.bill2.setRotate(0.0f, 68.5f, 0.0f);
        this.bill2.setScale(1.0f, 2.32f, 1.0f);
        this.haikei = new mirror_map();
        this.haikei.init(49);
        this.haikei.start(4, null);
        this.haikei.setScale(1.76f, 4.03f, 1.0f);
        this.haikei.setTranslate(3.75f, 1.93f, 1.56f);
        this.haikei.setRotate(355.0f, 0.0f, 0.0f);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy3 = new units();
        this.dummy3.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this._spl_thread_main = Thread.create(this, "__chracter_splne");
        this._spl_thread_main.start();
        System.methodSignal(1);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        System.sleep(1);
        this.kosms.setShadow(4, 47);
        this.shion.setShadow(4, 47);
        this.chaos.setShadow(4, 47);
        this.momo.setShadow(4, 47);
        this.ziggy.setShadow(4, 47);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.loadarc(this.ziggy.getChild(0x1000000), "FLSziggy.fpk");
        this.loadarc(this.momo.getChild(0x1000000), "FLSmomo.fpk");
        this.loadarc(this.shion.getChild(0x1000000), "FLSshion.fpk");
        this.loadarc(this.kosms.getChild(0x1000000), "FLSkosmos.fpk");
        this.loadarc(this.chaos.getChild(0x1000000), "FLSchaos.fpk");
        this.momo.face.mtn(2, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.ziggy.face.mtn(2, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.shion.face.mtn(2, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.kosms.face.mtn(2, 8, 1.0f, false);
        this.kosms.face.start(4, null);
        this.chaos.face.mtn(2, 8, 1.0f, false);
        this.chaos.face.start(4, null);
        Stage.setVisible(18, false);
        Stage.setVisible(19, false);
        Stage.setVisible(20, false);
        Stage.setVisible(21, false);
        Stage.setVisible(22, false);
        this.kosms.setTranslate(2.59f, 4.0f, -31.09f);
        this.shion.setTranslate(2.39f, 4.0f, -29.77f);
        this.chaos.setTranslate(3.12f, 4.0f, -29.439999f);
        this.momo.setTranslate(1.72f, 4.0f, -28.02f);
        this.ziggy.setTranslate(1.35f, 4.0f, -27.32f);
        this.cam1.change();
        Sound.streamPlay(1290057, 48000);
        this.camerawork.cut0001();
        this.light.setColor(0, 0.07f, 0.08f, 0.08f);
        this.light.setColor(1, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(1, 0.304f, 0.001f, -0.953f);
        this.light.setColor(2, 0.16f, 0.19f, 0.16f);
        this.light.setDirection2(2, -0.161f, 0.634f, 0.756f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, 0.52f, -0.827f, 0.214f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.dummy1.start(1, "zensin");
        this.kosms.start(1, "zensin1");
        this.shion.start(1, "zensin2");
        this.chaos.start(1, "zensin3");
        this.momo.start(1, "zensin4");
        this.ziggy.start(1, "zensin5");
        System.sleep(60);
        this.dummy2.start(1, "shion_serifu_001");
        this.MSGW("It looks like...we've\narrived at the center...");
        System.sleep(36);
        this.wait_clr(90);
        this.__wait();
        this.camerawork.cut0002();
        this.light.setColor(0, 0.07f, 0.08f, 0.08f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.436f, 0.334f, -0.836f);
        this.light.setColor(2, 0.16f, 0.19f, 0.16f);
        this.light.setDirection2(2, -0.161f, 0.634f, 0.756f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, 0.52f, -0.827f, 0.214f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(0, 27);
        this.shion.setShadow(0, 27);
        this.chaos.setShadow(0, 27);
        this.momo.setShadow(0, 27);
        this.ziggy.setShadow(0, 27);
        this.haikei.setTranslate(3.75f, -15.92f, 1.56f);
        this.haikei.setRotate(355.0f, 0.0f, 0.0f);
        this.ziggy.setTranslate(0.9449967f, 3.9999995f, -26.984905f);
        this.ziggy.setRotate(-0.8333333f, 153.74998f, 4.1666646f);
        this.momo.setTranslate(1.2199897f, 4.1374974f, -27.302372f);
        this.momo.setRotate(-4.1666646f, 151.25f, 3.33333f);
        this.shion.setTranslate(1.5224872f, 3.9999995f, -28.619843f);
        this.shion.setRotate(0.0f, 139.99998f, 0.0f);
        this.chaos.setTranslate(2.5149977f, 3.9999995f, -28.109974f);
        this.chaos.setRotate(-2.4999995f, 149.16669f, 5.8333306f);
        this.kosms.setTranslate(2.5599415f, 4.0150013f, -28.97717f);
        this.kosms.setRotate(0.0f, 144.99998f, 4.9999995f);
        this.dummy1.start(1, "zensin_stop");
        this.kosms.start(1, "zensin_stop1");
        this.shion.start(1, "zensin_stop2");
        this.chaos.start(1, "zensin_stop3");
        this.momo.start(1, "zensin_stop4");
        this.ziggy.start(1, "zensin_stop5");
        this.wait_clr(21);
        System.sleep(51);
        this.dummy2.start(1, "shion_serifu_002_mae");
        this.MSGW("What is that?");
        System.sleep(30);
        this.wait_clr(30);
        this.dummy3.start(1, "chaos_serifu_002_mae");
        this.MSGW("A building?");
        System.sleep(30);
        this.wait_clr(30);
        this.__wait();
        this.cam2.change();
        this.dummy1.setTranslate(0.0f, -0.3f, 3.45f);
        this.dummy1.setRotate(11.27f, 0.0f, 0.0f);
        this.cam_flag = 1;
        this.bill1.setVisible(false);
        this.camerawork.cut0003();
        this.light.setColor(0, 0.02f, 0.05f, 0.05f);
        this.light.setColor(1, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(1, 0.677f, 0.736f, -0.031f);
        this.light.setColor(2, 0.17f, 0.18f, 0.17f);
        this.light.setDirection2(2, -1.0f, 0.0f, -0.029f);
        this.light.setColor(3, 0.15f, 0.15f, 0.12f);
        this.light.setDirection2(3, 0.521f, -0.453f, 0.724f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(4, 47);
        this.shion.setShadow(4, 47);
        this.chaos.setShadow(4, 47);
        this.momo.setShadow(4, 47);
        this.ziggy.setShadow(4, 47);
        this.haikei.setTranslate(25.55f, -20.5f, 0.36f);
        this.haikei.setRotate(357.33f, 0.0f, -1.33f);
        this.kosms.setTranslate(2.4850008f, 4.4949746f, -33.92502f);
        this.kosms.setRotate(0.0f, 176.99997f, 0.0f);
        this.shion.setTranslate(2.342476f, 3.9999995f, -30.9497f);
        this.shion.setRotate(0.0f, 186.33331f, -1.3333331f);
        this.chaos.setTranslate(3.1239853f, 3.9999976f, -30.85985f);
        this.chaos.setRotate(0.0f, 181.79993f, -1.4999979f);
        this.momo.setTranslate(1.749991f, 3.9999993f, -32.7624f);
        this.momo.setRotate(0.0f, 182.25f, 0.0f);
        this.ziggy.setTranslate(1.378495f, 3.9999993f, -31.858006f);
        this.ziggy.setRotate(0.0f, 185.3999f, 0.0f);
        Stage.setVisible(15, false);
        Stage.setVisible(51, false);
        Stage.setVisible(52, false);
        Stage.setVisible(53, false);
        Stage.setVisible(54, false);
        this.kosms.start(1, "kosms_idl");
        this.shion.start(1, "shion_lookupend");
        this.chaos.start(1, "chaos_lookupend");
        this.momo.start(1, "momo_idl");
        this.ziggy.start(1, "ziggy_idl");
        System.sleep(51);
        this.MSGW("Or maybe some\nsort of device...");
        System.sleep(90);
        this.wait_clr(102);
        this.shion.setTranslate(2.2529843f, 3.9999995f, -30.438364f);
        this.shion.setRotate(0.0f, 154.69987f, -0.40000015f);
        this.chaos.setTranslate(2.74f, 3.9999995f, -30.059998f);
        this.chaos.setRotate(-6.0666604f, 151.39975f, 8.533328f);
        this.shion.start(1, "mtn_003_mae");
        this.chaos.start(1, "mtn_004_mae");
        this.kosms.setTranslate(2.36f, 1004.0f, -30.94f);
        this.momo.setTranslate(1.766f, 3.9999995f, -29.93f);
        this.momo.setRotate(0.0f, 128.69969f, 0.0f);
        this.ziggy.setTranslate(1.2599998f, 3.9999995f, -29.034964f);
        this.ziggy.setRotate(0.0f, 155.99974f, 0.0f);
        System.sleep(51);
        Stage.setVisible(15, true);
        Stage.setVisible(51, true);
        Stage.setVisible(52, true);
        Stage.setVisible(53, true);
        Stage.setVisible(54, true);
        this.__wait();
        this.haikei.setTranslate(25.55f, -55.67494f, 28.984951f);
        this.haikei.setRotate(357.32996f, 0.0f, -1.33f);
        this.bill1.setVisible(true);
        this.cam1.change();
        this.dummy1.setTranslate(0.0f, 0.0f, 0.0f);
        this.dummy1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam_flag = 0;
        this.camerawork.cut0004();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.554f, 0.086f, -0.828f);
        this.light.setColor(2, 0.16f, 0.19f, 0.16f);
        this.light.setDirection2(2, -0.161f, 0.634f, 0.756f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, 0.52f, -0.827f, 0.214f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(0, 27);
        this.shion.setShadow(0, 27);
        this.chaos.setShadow(0, 27);
        this.momo.setShadow(0, 27);
        this.ziggy.setShadow(0, 27);
        this.shion.start(1, "mtn_003");
        this.chaos.start(1, "mtn_004");
        System.sleep(21);
        this.MSGW("What do you mean?");
        this.shion.look_eye_speed(1.2f);
        this.shion.look_eye_set(4.0f, 0.0f);
        this.dummy3.start(1, "shion_serifu_002_1");
        System.sleep(36);
        this.wait_clr(21);
        this.MSGW("I don't know, it's just...");
        this.dummy2.start(1, "chaos_serifu_002");
        this.chaos.look_eye_speed(1.2f);
        this.chaos.look_eye_set(-2.0f, 0.0f);
        this.wait_clr(72);
        this.chaos.look_eye_speed(1.2f);
        this.chaos.look_eye_set(-4.0f, 0.0f);
        System.sleep(21);
        this.MSGW("Yes...?");
        this.dummy3.start(1, "shion_serifu_002_2");
        this.wait_clr(42);
        this.__wait();
        this.camerawork.cut0005_mae();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(1, 0.734f, -0.357f, 0.578f);
        this.light.setColor(2, 0.12f, 0.12f, 0.12f);
        this.light.setDirection2(2, 0.809f, 0.312f, -0.498f);
        this.light.setColor(3, 0.3f, 0.3f, 0.27f);
        this.light.setDirection2(3, -0.108f, -0.612f, -0.783f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(4, 47);
        this.shion.setShadow(4, 47);
        this.chaos.setShadow(4, 47);
        this.momo.setShadow(4, 47);
        this.ziggy.setShadow(4, 47);
        this.kosms.setTranslate(2.74f, 4.0f, -33.31f);
        this.kosms.setRotate(0.0f, 180.0f, 0.0f);
        this.shion.setTranslate(2.16f, 4.0f, -32.42f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.chaos.setTranslate(3.11f, 4.0f, -32.21f);
        this.chaos.setRotate(0.0f, 180.0f, 0.0f);
        this.momo.setTranslate(1.72f, 4.0f, -31.560001f);
        this.momo.setRotate(0.0f, 180.0f, 0.0f);
        this.ziggy.setTranslate(1.35f, 4.0f, -31.54f);
        this.ziggy.setRotate(0.0f, 165.0f, 0.0f);
        this.bill1.setTranslate(-2.19f, -2.44f, 15.01f);
        this.bill1.setRotate(0.0f, -80.2f, 0.0f);
        this.bill1.setScale(1.0f, 1.36f, 1.52f);
        this.bill2.setTranslate(7.73f, 4.27f, -11.02f);
        this.bill2.setRotate(0.0f, 71.0f, 0.0f);
        this.bill2.setScale(1.0f, 1.78f, 1.46f);
        this.dummy1.start(1, "zensin_xx");
        this.kosms.start(1, "zensin1_xx");
        this.shion.start(1, "zensin2_xx");
        this.chaos.start(1, "zensin3_xx");
        this.momo.start(1, "zensin4_xx");
        this.ziggy.start(1, "zensin5_xx");
        System.sleep(51);
        this.MSGW("Well, if this is really the center,");
        System.sleep(96);
        this.MSGW("it's possible that it may be of\nsome importance to the Gnosis...");
        System.sleep(96);
        this.shion.look_default();
        this.chaos.look_default();
        this.MSGW("But this, of course,");
        this.wait_clr(60);
        this.haikei.setTranslate(25.55f, -20.5f, 0.36f);
        this.haikei.setRotate(357.33f, 0.0f, -1.33f);
        this.bill2.setScale(1.0f, 3.52f, 1.46f);
        this.camerawork.cut0005();
        System.sleep(15);
        this.global_zensin_power = -0.0045f;
        System.sleep(30);
        this.MSGW("is from our frame of reference,\nso we could be completely mistaken.");
        this.global_zensin_power = -0.00225f;
        System.sleep(30);
        this.global_zensin_power = -0.001125f;
        System.sleep(30);
        this.global_zensin_power = -5.625E-4f;
        System.sleep(3);
        this.global_zensin_power = -2.8125E-4f;
        System.sleep(3);
        this.global_zensin_power = -1.40625E-4f;
        System.sleep(3);
        this.global_zensin_power = -7.03125E-5f;
        System.sleep(3);
        this.global_zensin_power = -3.515625E-5f;
        System.sleep(3);
        this.global_zensin_power = 0.0f;
        this.dummy1.start(1, "zensin_stop");
        this.kosms.start(1, "kosms_idl_smooth");
        this.shion.start(1, "shion_idl_smooth");
        this.chaos.start(1, "chaos_idl_smooth");
        this.momo.start(1, "momo_idl_smooth");
        this.ziggy.start(1, "ziggy_idl_smooth");
        System.sleep(30);
        this.wait_clr(30);
        this.__wait();
        this.bill2.setScale(1.0f, 1.78f, 1.46f);
        this.camerawork.cut0006();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.909f, 0.241f, -0.34f);
        this.light.setColor(2, 0.16f, 0.17f, 0.16f);
        this.light.setDirection2(2, -0.997f, 0.0f, -0.08f);
        this.light.setColor(3, 0.13f, 0.13f, 0.12f);
        this.light.setDirection2(3, 0.472f, -0.537f, -0.699f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(0, 27);
        this.shion.setShadow(0, 27);
        this.chaos.setShadow(0, 27);
        this.momo.setShadow(0, 27);
        this.ziggy.setShadow(0, 27);
        this.momo.face.mtn(2, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.shion.setTranslate(1.63f, 4.0f, -40.15f);
        this.shion.setRotate(-1.85f, 225.0f, -1.85f);
        this.chaos.setTranslate(1.98f, 4.0f, -40.51f);
        this.chaos.setRotate(-4.03f, 223.05f, -2.4f);
        this.kosms.setTranslate(1.27f, 4.474996f, -40.77f);
        this.kosms.setRotate(0.0f, 175.0f, 0.0f);
        this.ziggy.setTranslate(0.4f, 4.0f, -39.89f);
        this.ziggy.setRotate(0.0f, 159.45f, 0.0f);
        this.momo.setTranslate(0.77f, 4.0f, -40.08f);
        this.momo.setRotate(-1.85f, 142.3f, 0.0f);
        this.ziggy.start(1, "mtn_005");
        this.momo.start(1, "mtn_006");
        this.kosms.start(1, "kosms_idl");
        this.shion.start(1, "shion_idl");
        this.chaos.start(1, "chaos_idl");
        System.sleep(21);
        this.dummy2.start(1, "ziggy_serifu_006");
        this.MSGW("In other words, if that\nis a man-made object,");
        System.sleep(75);
        this.MSGW("it could be a ship or some other\nwreckage that was swallowed\njust as we were...");
        System.sleep(150);
        this.momo.face.mtn(8, 0, 120, 7, 9, 1.0f, false);
        this.momo.face.start(4, null);
        this.wait_clr(27);
        this.MSGW("What about us...?");
        this.dummy3.start(1, "momo_serifu_006");
        System.sleep(36);
        this.MSGW("Are we gonna end up like that too?");
        System.sleep(60);
        this.shion.start(1, "mtn_007_mae");
        this.shion.setTranslate(1.6549996f, 3.9999995f, -40.062366f);
        this.shion.setRotate(-1.85f, 206.25f, -1.85f);
        this.wait_clr(30);
        this.__wait();
        this.ziggy.start(1, "ziggy_idl");
        this.camerawork.cut0007();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(1, 0.002f, 0.002f, -1.0f);
        this.light.setColor(2, 0.24f, 0.27f, 0.24f);
        this.light.setDirection2(2, 0.546f, 0.397f, 0.738f);
        this.light.setColor(3, 0.18f, 0.18f, 0.15f);
        this.light.setDirection2(3, -0.891f, -0.451f, 0.052f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.face.mtn(2, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.kosms.setTranslate(1.15f, 4.474996f, -40.69f);
        this.kosms.setRotate(0.1f, 180.0f, 0.0f);
        this.chaos.setTranslate(1.99f, 3.98f, -40.51f);
        this.chaos.setRotate(-4.03f, 222.05f, -2.4f);
        this.ziggy.setTranslate(0.42f, 4.0f, -39.99f);
        this.ziggy.setRotate(0.0f, 151.95f, 0.0f);
        this.kosms.setVisible(true);
        this.shion.start(1, "mtn_007");
        this.momo.start(1, "mtn_008");
        this.kosms.start(1, "kosms_idl");
        this.chaos.start(1, "chaos_idl");
        this.ziggy.start(1, "ziggy_idl");
        this.wait_clr(18);
        this.dummy2.start(1, "shion_serifu_007_1");
        this.MSGW("We'll be sure to get out of\nhere before that happens.");
        System.sleep(60);
        this.wait_clr(39);
        this.dummy2.start(1, "shion_serifu_007_2");
        this.MSGW("KOS-MOS, what's the\nElsa's current position?");
        System.sleep(39);
        this.wait_clr(51);
        this.__wait();
        this.camerawork.cut0008();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.634f, 0.263f, -0.728f);
        this.light.setColor(2, 0.13f, 0.14f, 0.13f);
        this.light.setDirection2(2, -0.694f, 0.62f, 0.365f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, -0.268f, -0.666f, -0.696f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setTranslate(1.5924965f, 3.9999995f, -37.136913f);
        this.shion.setRotate(-1.85f, 206.25f, -1.85f);
        this.ziggy.setTranslate(0.41999996f, 3.9999995f, -36.82728f);
        this.ziggy.setRotate(0.0f, 151.95f, 0.0f);
        this.chaos.setTranslate(1.9f, 3.98f, -37.55f);
        this.chaos.setRotate(-4.03f, 228.05f, -2.4f);
        this.momo.setTranslate(0.88f, 4.0f, -37.05f);
        this.momo.setRotate(-1.85f, 127.3f, 0.0f);
        this.kosms.setTranslate(1.14f, 4.48f, -37.6f);
        this.kosms.setRotate(-0.9f, 190.0f, 2.33f);
        this.kosms.start(1, "kosms_idl");
        this.shion.start(1, "mtn_009");
        this.chaos.start(1, "chaos_idl");
        this.momo.start(1, "momo_idl");
        this.ziggy.start(1, "ziggy_idl");
        System.sleep(9);
        this.dummy2.start(1, "kosms_serifu_008");
        this.MSGW("Location unknown.\nI have lost its signal.");
        System.sleep(30);
        System.sleep(39);
        this.MSGW("Lost...?");
        this.dummy3.start(1, "shion_serifu_008");
        this.wait_clr(51);
        this.__wait();
        this.bill2.setScale(1.0f, 3.83f, 1.0f);
        this.bill2.setTranslate(-0.27908844f, -1.2800064f, -0.12545086f);
        this.bill2.setRotate(-0.8333333f, 2.0998993f, 0.0f);
        this.camerawork.cut0009_mae();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(1, 0.734f, -0.357f, 0.578f);
        this.light.setColor(2, 0.12f, 0.12f, 0.12f);
        this.light.setDirection2(2, 0.809f, 0.312f, -0.498f);
        this.light.setColor(3, 0.3f, 0.3f, 0.27f);
        this.light.setDirection2(3, -0.108f, -0.612f, -0.783f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(4, 42);
        this.shion.setShadow(4, 42);
        this.chaos.setShadow(4, 42);
        this.momo.setShadow(4, 42);
        this.ziggy.setShadow(4, 42);
        this.haikei.setTranslate(0.0f, 0.0f, 0.0f);
        this.haikei.setRotate(0.0f, 0.0f, 0.0f);
        this.haikei.setScale(1.0f, 1.0f, 1.0f);
        this.bill1.setTranslate(0.0f, 0.0f, 0.0f);
        this.bill1.setRotate(0.0f, 0.0f, 0.0f);
        this.bill1.setScale(1.0f, 1.0f, 1.0f);
        this.kosms.start(1, "kosms_idl");
        this.shion.start(1, "shion_idl");
        this.chaos.start(1, "chaos_idl");
        this.momo.start(1, "momo_idl");
        this.ziggy.start(1, "ziggy_idl");
        this.MSGW("The Gnosis' magnetic and\ngravitational fields have\nbecome unstable near the center.");
        System.sleep(156);
        this.camerawork.cut0009();
        this.MSGW("The last known coordinates point to\na location on the lower level, 300\nmeters below our present position.");
        this.wait_clr(192);
        this.__wait();
        this.bill2.setTranslate(0.0f, 0.0f, 0.0f);
        this.bill2.setRotate(0.0f, 0.0f, 0.0f);
        this.bill2.setScale(1.0f, 1.0f, 1.0f);
        this.camerawork.cut0010();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(1, 0.503f, 0.327f, -0.8f);
        this.light.setColor(2, 0.23f, 0.26f, 0.23f);
        this.light.setDirection2(2, -0.824f, 0.168f, -0.541f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, 0.19f, -0.879f, 0.438f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(1, 27);
        this.shion.setShadow(1, 27);
        this.chaos.setShadow(1, 27);
        this.momo.setShadow(1, 27);
        this.ziggy.setShadow(1, 27);
        this.momo.face.mtn(2, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.shion.face.mtn(2, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.chaos.setTranslate(1.9299996f, 3.979998f, -40.48984f);
        this.chaos.setRotate(-4.03f, 228.74998f, -2.4f);
        this.shion.setTranslate(1.4824991f, 3.9999995f, -40.1775f);
        this.shion.setRotate(-1.85f, 214.25f, -1.85f);
        this.momo.setTranslate(1.0549998f, 3.9999995f, -40.064987f);
        this.momo.setRotate(-1.85f, 186.54999f, 0.0f);
        this.ziggy.setTranslate(0.41999996f, 3.9999995f, -39.80225f);
        this.ziggy.setRotate(0.0f, 151.95f, 0.0f);
        this.kosms.setTranslate(1.1399999f, 4.48f, -40.599792f);
        this.kosms.setRotate(-0.8999999f, 189.99998f, 2.3299997f);
        this.momo.start(1, "mtn_010");
        this.shion.start(1, "mtn_011");
        this.kosms.start(1, "kosms_idl");
        this.chaos.start(1, "chaos_idl");
        System.sleep(6);
        this.dummy3.start(1, "momo_serifu_010");
        this.MSGW("That's pretty close.\nShall we go?");
        System.sleep(45);
        System.sleep(36);
        this.dummy2.start(1, "shion_serifu_010");
        this.MSGW("Yes.");
        System.sleep(51);
        this.__wait();
        this.kosms.start(1, "kosms_idl");
        this.shion.start(1, "mtn_012");
        this.chaos.start(1, "chaos_muki");
        this.momo.start(1, "momo_muki");
        this.ziggy.start(1, "ziggy_muki");
        this.camerawork.cut0011();
        this.light.setColor(0, 0.06f, 0.07f, 0.07f);
        this.light.setColor(1, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(1, 0.634f, 0.263f, -0.728f);
        this.light.setColor(2, 0.22f, 0.23f, 0.22f);
        this.light.setDirection2(2, -0.694f, 0.621f, 0.365f);
        this.light.setColor(3, 0.13f, 0.13f, 0.1f);
        this.light.setDirection2(3, -0.268f, -0.666f, -0.696f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosms.setShadow(1, 17);
        this.shion.setShadow(1, 17);
        this.chaos.setShadow(1, 17);
        this.momo.setShadow(1, 17);
        this.ziggy.setShadow(1, 17);
        this.kosms.setTranslate(1.2019984f, 4.474996f, -40.753967f);
        this.kosms.setRotate(-7.1333265f, 173.39975f, 2.133332f);
        this.shion.setTranslate(1.641996f, 3.9999995f, -40.40477f);
        this.shion.setRotate(-3.4499907f, 223.99997f, -0.6499977f);
        this.chaos.setTranslate(2.0099947f, 4.0149918f, -40.289444f);
        this.chaos.setRotate(-0.9633139f, 251.04999f, 4.4666414f);
        this.momo.setTranslate(0.7759982f, 3.9999995f, -40.061012f);
        this.momo.setRotate(-1.85f, 120.49998f, 0.0f);
        this.ziggy.setTranslate(0.36f, 3.9999995f, -39.889996f);
        this.ziggy.setRotate(0.0f, 126.45f, 0.0f);
        this.wait_clr(6);
        this.MSGW("Let's just hope the Elsa\nisn't in the same condition\nas the wreckage around here.");
        this.dummy3.start(1, "shion_serifu_011");
        System.sleep(135);
        this.wait_clr(36);
        this.__wait();
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class robo
            extends Chr {
        Chr face;

        public robo(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
        }

        void chaos_idl() {
            this.mtn(283, 0, 1, 0, 0, 0.1f, true);
        }

        void chaos_idl_smooth() {
            this.mtn(283, 0, 1, 10, 1, 1.2f, true);
        }

        void chaos_lookupend() {
            this.mtn(258, 198, 198, 0, 0, 0.85f, true);
        }

        void chaos_muki() {
            this.setScale(-1.0f, 1.0f, 1.0f);
            this.mtn(272, 7, 120, 0, 0, 0.327f, true);
        }

        void kosms_idl() {
            this.mtn(278, 0, 0.27f, true);
        }

        void kosms_idl_smooth() {
            this.mtn(278, 0, 1, 9, 1, 1.2f, true);
        }

        void momo_idl() {
            this.mtn(281, 0, 1, 0, 0, 0.1f, true);
        }

        void momo_idl_smooth() {
            this.mtn(281, 0, 1, 10, 1, 1.2f, true);
        }

        void momo_muki() {
            this.mtn(271, 12, 120, 0, 0, 0.27f, true);
        }

        void mtn_003() {
            this.mtn(259, 8, 1.0f, true);
        }

        void mtn_003_mae() {
            this.mtn(259, 0, 0, 0, 0, 1.0f, true);
        }

        void mtn_004() {
            this.mtn(260, 8, 1.0f, true);
        }

        void mtn_004_mae() {
            this.mtn(260, 0, 0, 0, 0, 1.0f, true);
        }

        void mtn_005() {
            this.mtn(261, 8, 1.0f, true);
        }

        void mtn_006() {
            this.mtn(262, 137, 573, 0, 0, 1.0f, true);
        }

        void mtn_007() {
            this.mtn(263, 0, 1.17f, true);
        }

        void mtn_007_mae() {
            this.mtn(263, 0, 0, 0, 0, 1.17f, true);
        }

        void mtn_008() {
            this.mtn(264, 0, 1.0f, true);
        }

        void mtn_009() {
            this.mtn(265, 22, 527, 0, 0, 1.0f, true);
        }

        void mtn_010() {
            this.mtn(266, 0, 1.0f, true);
        }

        void mtn_011() {
            this.mtn(267, 52, 527, 0, 0, 1.0f, true);
        }

        void mtn_012() {
            this.mtn(268, 0, 1.0f, true);
        }

        void shion_idl() {
            this.mtn(282, 0, 1, 0, 0, 0.1f, true);
        }

        void shion_idl_smooth() {
            this.mtn(282, 0, 1, 10, 1, 1.2f, true);
        }

        void shion_lookupend() {
            this.mtn(257, 198, 198, 0, 0, 0.92f, true);
        }

        void zensin1() {
            this.mtn(277, 17, 27, 0, 0, 0.44f, true);
            this.mtn(277, 8, 0.44f, true);
        }

        void zensin1_xx() {
            this.mtn(277, 17, 27, 0, 0, 0.22f, true);
            this.mtn(277, 8, 0.22f, true);
        }

        void zensin2() {
            this.mtn(273, 7, 27, 0, 0, 0.47f, true);
            this.mtn(273, 8, 0.47f, true);
        }

        void zensin2_xx() {
            this.mtn(273, 7, 27, 0, 0, 0.235f, true);
            this.mtn(273, 8, 0.235f, true);
        }

        void zensin3() {
            this.mtn(274, 12, 26, 0, 0, 0.39f, true);
            this.mtn(274, 8, 0.39f, true);
        }

        void zensin3_xx() {
            this.mtn(274, 12, 26, 0, 0, 0.195f, true);
            this.mtn(274, 8, 0.195f, true);
        }

        void zensin4() {
            this.mtn(275, 17, 26, 0, 0, 0.5f, true);
            this.mtn(275, 8, 0.5f, true);
        }

        void zensin4_xx() {
            this.mtn(275, 17, 26, 0, 0, 0.25f, true);
            this.mtn(275, 8, 0.25f, true);
        }

        void zensin5() {
            this.mtn(276, 23, 26, 0, 0, 0.37f, true);
            this.mtn(276, 8, 0.37f, true);
        }

        void zensin5_xx() {
            this.mtn(276, 23, 26, 0, 0, 0.185f, true);
            this.mtn(276, 8, 0.185f, true);
        }

        void zensin_stop1() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(257, 32, 198, 0, 0, 0.92f, true);
        }

        void zensin_stop2() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(257, 8, 0.92f, true);
        }

        void zensin_stop3() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(258, 8, 0.85f, true);
        }

        void zensin_stop4() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(257, 8, 0.77f, true);
        }

        void zensin_stop5() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(258, 8, 0.57f, true);
        }

        void ziggy_idl() {
            this.mtn(280, 0, 1, 0, 0, 0.1f, true);
        }

        void ziggy_idl_smooth() {
            this.mtn(280, 0, 1, 9, 1, 1.2f, true);
        }

        void ziggy_muki() {
            this.mtn(272, 14, 120, 0, 0, 0.27f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }

        void chaos_serifu_002() {
            SCE02052.this.FACE(15, SCE02052.this.chaos, 1, 1.0f);
            System.sleep(45);
            SCE02052.this.FACE(12, SCE02052.this.chaos, 1, 1.0f);
        }

        void chaos_serifu_002_mae() {
            SCE02052.this.FACE(27, SCE02052.this.chaos, 1, 0.72f);
        }

        void kosms_serifu_008() {
            SCE02052.this.FACE(21, SCE02052.this.kosms, 1, 1.0f);
            System.sleep(12);
            SCE02052.this.FACE(27, SCE02052.this.kosms, 1, 1.0f);
        }

        void momo_serifu_006() {
            SCE02052.this.FACE(30, SCE02052.this.momo, 7, 1.0f);
            System.sleep(15);
            SCE02052.this.FACE(42, SCE02052.this.momo, 7, 1.0f);
        }

        void momo_serifu_010() {
            SCE02052.this.FACE_SMOOTH(39, SCE02052.this.momo, 3, 1.0f);
            System.sleep(12);
            SCE02052.this.FACE(21, SCE02052.this.momo, 3, 1.0f);
        }

        void shion_serifu_001() {
            SCE02052.this.FACE(30, SCE02052.this.shion, 1, 0.72f);
            System.sleep(15);
            SCE02052.this.FACE(45, SCE02052.this.shion, 1, 0.72f);
        }

        void shion_serifu_002_1() {
            SCE02052.this.FACE(36, SCE02052.this.shion, 1, 1.0f);
        }

        void shion_serifu_002_2() {
            SCE02052.this.FACE(9, SCE02052.this.shion, 1, 1.0f);
        }

        void shion_serifu_002_mae() {
            SCE02052.this.FACE(21, SCE02052.this.shion, 1, 1.0f);
        }

        void shion_serifu_007_1() {
            SCE02052.this.FACE_SMOOTH(54, SCE02052.this.shion, 3, 0.92f);
        }

        void shion_serifu_007_2() {
            SCE02052.this.FACE_SMOOTH(21, SCE02052.this.shion, 3, 1.0f);
            System.sleep(18);
            SCE02052.this.FACE_SMOOTH(33, SCE02052.this.shion, 1, 0.92f);
        }

        void shion_serifu_008() {
            SCE02052.this.FACE_SMOOTH(33, SCE02052.this.shion, 7, 0.92f);
        }

        void shion_serifu_010() {
            SCE02052.this.FACE_SMOOTH(9, SCE02052.this.shion, 3, 1.0f);
        }

        void shion_serifu_011() {
            SCE02052.this.FACE_SMOOTH(30, SCE02052.this.shion, 1, 0.82f);
            System.sleep(18);
            SCE02052.this.FACE(84, SCE02052.this.shion, 1, 0.82f);
        }

        void zensin() {
            while (true) {
                SCE02052.this.kosms.setTranslate(SCE02052.this.kosms.px, SCE02052.this.kosms.py, SCE02052.this.kosms.pz - 0.022f);
                SCE02052.this.shion.setTranslate(SCE02052.this.shion.px, SCE02052.this.shion.py, SCE02052.this.shion.pz - 0.022f);
                SCE02052.this.momo.setTranslate(SCE02052.this.momo.px, SCE02052.this.momo.py, SCE02052.this.momo.pz - 0.022f);
                SCE02052.this.chaos.setTranslate(SCE02052.this.chaos.px, SCE02052.this.chaos.py, SCE02052.this.chaos.pz - 0.022f);
                SCE02052.this.ziggy.setTranslate(SCE02052.this.ziggy.px, SCE02052.this.ziggy.py, SCE02052.this.ziggy.pz - 0.022f);
                System.sleep(1);
            }
        }

        void zensin_stop() {
        }

        void zensin_xx() {
            while (true) {
                SCE02052.this.kosms.setTranslate(SCE02052.this.kosms.px, SCE02052.this.kosms.py, SCE02052.this.kosms.pz + SCE02052.this.global_zensin_power);
                SCE02052.this.shion.setTranslate(SCE02052.this.shion.px, SCE02052.this.shion.py, SCE02052.this.shion.pz + SCE02052.this.global_zensin_power);
                SCE02052.this.momo.setTranslate(SCE02052.this.momo.px, SCE02052.this.momo.py, SCE02052.this.momo.pz + SCE02052.this.global_zensin_power);
                SCE02052.this.chaos.setTranslate(SCE02052.this.chaos.px, SCE02052.this.chaos.py, SCE02052.this.chaos.pz + SCE02052.this.global_zensin_power);
                SCE02052.this.ziggy.setTranslate(SCE02052.this.ziggy.px, SCE02052.this.ziggy.py, SCE02052.this.ziggy.pz + SCE02052.this.global_zensin_power);
                System.sleep(1);
            }
        }

        void ziggy_serifu_006() {
            SCE02052.this.FACE(60, SCE02052.this.ziggy, 1, 0.92f);
            System.sleep(21);
            SCE02052.this.FACE(60, SCE02052.this.ziggy, 1, 0.92f);
            System.sleep(15);
            SCE02052.this.FACE(66, SCE02052.this.ziggy, 1, 0.92f);
        }
    }

    class mirror_map
            extends MAPUnit {
        mirror_map() {
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0001() {
            SCE02052.this.cam1.setFov(32.27f);
            SCE02052.this.cam0.setFov(32.27f);
            float[] fArray = new float[]{1.0f, 11.465024f, 6.064997f, -45.419968f, 193.0f, 10.795024f, 5.9049973f, -44.31997f, 385.0f, 10.485024f, 5.9049973f, -44.129967f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -5.116651f;
            fArray2[2] = -210.78f;
            fArray2[4] = 193.0f;
            fArray2[5] = -4.946651f;
            fArray2[6] = -218.58f;
            fArray2[8] = 385.0f;
            fArray2[9] = -4.946651f;
            fArray2[10] = -219.11f;
            float[] fArray3 = fArray2;
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0002() {
            SCE02052.this.cam1.setFov(25.02f);
            SCE02052.this.cam0.setFov(25.02f);
            float[] fArray = new float[]{1.0f, 5.75f, 4.81f, -30.23f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 10.76f;
            fArray2[2] = 96.32f;
            float[] fArray3 = fArray2;
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0003() {
            SCE02052.this.cam1.setFov(42.6f);
            SCE02052.this.cam0.setFov(42.6f);
            float[] fArray = new float[]{1.0f, 2.3700001f, 5.9524984f, -28.88224f, 76.200005f, 2.38f, 5.9524984f, -28.86224f, 159.0f, 2.38f, 5.9524984f, -28.63224f, 238.20001f, 2.38f, 5.9524984f, -28.73224f, 268.2f, 2.38f, 5.9524984f, -28.73224f, 298.2f, 2.38f, 5.9524984f, -28.73224f, 328.2f, 2.38f, 5.9524984f, -28.73224f, 358.2f, 2.38f, 5.9524984f, -28.73224f};
            float[] fArray2 = new float[32];
            fArray2[0] = 1.0f;
            fArray2[1] = -9.89f;
            fArray2[2] = 3.21f;
            fArray2[4] = 76.200005f;
            fArray2[5] = 4.39f;
            fArray2[6] = 3.4699998f;
            fArray2[8] = 159.0f;
            fArray2[9] = 29.29f;
            fArray2[10] = 3.4699998f;
            fArray2[12] = 238.20001f;
            fArray2[13] = 40.43f;
            fArray2[14] = 3.4699998f;
            fArray2[16] = 268.2f;
            fArray2[17] = 40.43f;
            fArray2[18] = 3.4699998f;
            fArray2[20] = 298.2f;
            fArray2[21] = 40.43f;
            fArray2[22] = 3.4699998f;
            fArray2[24] = 328.2f;
            fArray2[25] = 40.43f;
            fArray2[26] = 3.4699998f;
            fArray2[28] = 358.2f;
            fArray2[29] = 40.43f;
            fArray2[30] = 3.4699998f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[8];
            fArray4[0] = 1.0f;
            fArray4[1] = 42.0f;
            fArray4[4] = 358.2f;
            fArray4[5] = 52.0f;
            float[] fArray5 = fArray4;
            SCE02052.this.cam0.setFov(42.6f);
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray3, 1);
            SCE02052.this.cam3.rotateSPL(fArray5, 1, 3, 314);
        }

        public void cut0004() {
            SCE02052.this.cam1.setFov(24.65f);
            SCE02052.this.cam0.setFov(24.65f);
            float[] fArray = new float[]{1.0f, 4.24f, 5.25f, -30.61f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 7.39f;
            fArray2[2] = 104.76f;
            float[] fArray3 = fArray2;
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0005() {
            SCE02052.this.cam1.setFov(32.7f);
            SCE02052.this.cam0.setFov(32.7f);
            float[] fArray = new float[]{1.0f, 12.5f, 82.56f, -50.41f, 398.0f, 12.2f, 80.55f, -50.22f};
            float[] fArray2 = new float[]{1.0f, -81.6f, 97.63f, 10.88f, 398.0f, -81.6f, 103.33f, 10.88f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
            float[] fArray3 = new float[]{1.0f, 32.7f, 398.0f, 32.36f};
            SCE02052.this.cam1.fovSPL(fArray3, 1);
        }

        public void cut0005_mae() {
            SCE02052.this.cam1.setFov(30.77f);
            SCE02052.this.cam0.setFov(30.77f);
            float[] fArray = new float[]{1.0f, 10.89f, 232.39f, 30.71f, 527.0f, 13.75f, 223.56f, 28.7f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -46.18f;
            fArray2[2] = 4.39f;
            fArray2[4] = 527.0f;
            fArray2[5] = -47.66f;
            fArray2[6] = 6.92f;
            float[] fArray3 = fArray2;
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0006() {
            SCE02052.this.cam1.setFov(27.73f);
            SCE02052.this.cam0.setFov(27.73f);
            float[] fArray = new float[]{1.0f, 1.28f, 4.72f, -41.88f};
            float[] fArray2 = new float[]{1.0f, 18.43f, 162.16f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0007() {
            SCE02052.this.cam1.setFov(25.15f);
            SCE02052.this.cam0.setFov(25.15f);
            float[] fArray = new float[]{1.0f, -0.41f, 5.88f, -40.57f};
            float[] fArray2 = new float[]{1.0f, -21.1f, 254.72f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0008() {
            SCE02052.this.cam1.setFov(26.83f);
            SCE02052.this.cam0.setFov(26.83f);
            float[] fArray = new float[]{1.0f, 1.0f, 5.95f, -38.5f, 84.0f, 1.0f, 5.95f, -38.5f, 167.0f, 1.0f, 5.95f, -38.5f};
            float[] fArray2 = new float[]{1.0f, -27.1f, 196.48f, 0.49f, 84.0f, -26.33f, 196.61f, 0.49f, 167.0f, -25.95f, 196.61f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0009() {
            SCE02052.this.cam1.setFov(51.28f);
            SCE02052.this.cam0.setFov(51.28f);
            float[] fArray = new float[]{1.0f, 0.64f, 11.88f, 10.14f, 428.0f, -0.14f, 11.88f, 10.13f};
            float[] fArray2 = new float[]{1.0f, 4.17f, 359.14f, 0.49f, 428.0f, 4.17f, 359.14f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0009_mae() {
            SCE02052.this.cam1.setFov(32.7f);
            SCE02052.this.cam0.setFov(32.7f);
            float[] fArray = new float[]{1.0f, 2.69f, 12.19f, 5.66f, 620.0f, 0.99f, 12.19f, 5.64f};
            float[] fArray2 = new float[]{1.0f, -4.47f, 359.26f, 0.49f, 620.0f, -4.47f, 359.14f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0010() {
            SCE02052.this.cam1.setFov(27.64f);
            SCE02052.this.cam0.setFov(27.64f);
            float[] fArray = new float[]{1.0f, 2.06f, 5.45f, -41.22f, 248.0f, 2.06f, 5.45f, -41.22f};
            float[] fArray2 = new float[]{1.0f, -6.35f, 505.25f, 0.49f, 248.0f, -2.85f, 505.25f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut0011() {
            SCE02052.this.cam1.setFov(30.0f);
            SCE02052.this.cam0.setFov(30.0f);
            float[] fArray = new float[]{1.0f, 1.26f, 5.26f, -44.34f, 408.0f, 1.29f, 5.13f, -45.22f};
            float[] fArray2 = new float[]{1.0f, -0.16f, 538.37f, 0.49f, 408.0f, -0.16f, 538.37f, 0.49f};
            SCE02052.this.cam1.transSPL(fArray, 1);
            SCE02052.this.cam1.rotateSPL(fArray2, 1);
        }
    }
}

