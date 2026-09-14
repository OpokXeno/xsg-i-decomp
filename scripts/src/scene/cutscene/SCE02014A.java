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
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02014A
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02014A,
        MC_ELS01_PRJ,
        FLSshion_h,
        FLSmatehws,
        FLSchaos,
        FLShammer {
    Light light = new Light(0);
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
    Thread _spl_thread_main;
    Input pad1;
    Input pad0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camerawork camerawork = new Camerawork();
    human hammr;
    human masyu;
    human chaos;
    human shion;
    Unit semotare;
    Unit zabuton;
    units dummy1;
    units dummy2;
    Unit isu1;
    Unit isu2;
    Unit isu3;
    MAPUnit isu4;
    Unit isu5;
    Unit isu6;
    MAPUnit isu7;
    Unit fm1;
    Unit fm3;
    Unit em1;
    Unit em3;
    float f_sabun = 0.0f;
    float pinvot_sabun = 0.0f;
    float f_isu5_py = 0.0f;
    float f_isu5_pz = 0.0f;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02014A() {
    }

    void FACE(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 8, f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 0, 120, 7, 9, f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
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

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02014_F");
        Runtime.setFlags(124, 1, 1);
        System.println("XEVEJNAME:CFJ2_110 XEVEJPOINT:POINT2_110");
        Runtime.jumpCF(510, 2);
    }

    void init() {
        Runtime.setLocation(46);
        this.fm3 = new Unit();
        this.fm3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm3.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm3.setArgs(1, 21007, 0, 128, 112);
        this.fm3.setArgs(2, 62, 0, 15, -1);
        this.fm3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm3.signal(1);
        this.fm3.setScale(0.23f, 0.23f, 0.23f);
        this.fm3.setTranslate(-3.83f, 0.52f, 1.54f);
        this.fm3.setRotate(0.02f, 89.37f, 0.0f);
        this.fm1 = new Unit();
        this.fm1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1.setArgs(1, 20053, 0, 128, 112);
        this.fm1.setArgs(2, 77, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm1.setScale(0.3f, 0.3f, 0.3f);
        this.fm1.setTranslate(-3.03f, 0.53f, 0.68f);
        this.fm1.setRotate(-11.17f, 3.15f, -0.0f);
        this.em1 = new Unit();
        this.em1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.em1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.em1.setArgs(1, 20053, 0, 128, 112);
        this.em1.setArgs(2, 77, 0, 15, -1);
        this.em1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.em1.signal(1);
        this.em1.setScale(0.3f, 0.3f, 0.3f);
        this.em1.setTranslate(3.03f, 0.53f, 0.68f);
        this.em1.setRotate(-11.17f, -3.15f, 0.0f);
        this.em3 = new Unit();
        this.em3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.em3.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.em3.setArgs(1, 21007, 0, 128, 112);
        this.em3.setArgs(2, 92, 0, 15, -1);
        this.em3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.em3.signal(1);
        this.em3.setScale(0.23f, 0.23f, 0.23f);
        this.em3.setTranslate(3.83f, 0.52f, 1.54f);
        this.em3.setRotate(0.02f, -89.37f, 0.0f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = -2147483391;
        this.fadeOut.args[1] = 60;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 60;
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
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.hammr = new human(0x1000116, 1.6f, 0.0f, 0.0f, 0.0f);
        this.hammr.setTranslate(-3.01f, -0.45f, 1.5f);
        this.hammr.setRotate(0.0f, 180.0f, 0.0f);
        this.masyu = new human(0x1000113, 0.4f, 0.0f, 0.0f, 0.0f);
        this.masyu.setTranslate(-0.79f, -1.3f, -3.41f);
        this.masyu.setRotate(-5.65f, -960.54f, 1.55f);
        this.chaos = new human(0x1000003, 0.4f, 0.0f, 1.0f, 0.0f);
        this.chaos.setTranslate(2.92f, -0.47f, 1.55f);
        this.chaos.setRotate(-5.55f, 180.35f, -1.75f);
        this.shion = new human(0x100001E, 0.6f, 0.0f, 0.0f, 0.0f);
        this.shion.setTranslate(-0.03f, 0.0f, 12.11f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.shion.setShadow(5, 16);
        this.semotare = new units();
        this.semotare.mapUnit(47);
        this.semotare.start(4, null);
        this.zabuton = new units();
        this.zabuton.mapUnit(48);
        this.zabuton.start(4, null);
        this.semotare.setTranslate(-3.1f, -0.03f, 1.59f);
        this.semotare.setRotate(0.0f, -26.0f, 0.0f);
        this.isu1 = new units();
        this.isu1.mapUnit(37);
        this.isu1.start(4, null);
        this.isu2 = new units();
        this.isu2.mapUnit(38);
        this.isu2.start(4, null);
        this.isu3 = new units();
        this.isu3.mapUnit(39);
        this.isu3.start(4, null);
        this.isu4 = new mirror_map();
        this.isu4.init(40);
        this.isu4.start(4, null);
        this.isu5 = new units();
        this.isu5.mapUnit(41);
        this.isu5.start(4, null);
        this.isu6 = new units();
        this.isu6.mapUnit(42);
        this.isu6.start(4, null);
        this.isu7 = new mirror_map();
        this.isu7.init(36);
        this.isu7.start(4, null);
        this.isu7.setPivot(-3.0f, 0.05f, 6.55f);
        this.isu7.setTranslate(0.0f, 0.0f, 0.0f);
        this.isu7.setRotate(0.0f, 0.0f, 0.0f);
        this.isu5.setTranslate(-3.0f, -0.1f, 3.63f);
        this.isu5.setRotate(0.0f, 0.0f, 0.0f);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        System.methodSignal(1);
    }

    void isu_move(float f) {
        this.f_sabun = 0.017920243f * f;
        this.f_isu5_py = 0.05f + Math.cos(Math.toRadians(f)) * 3.59f;
        this.f_isu5_pz = 3.626597f - this.f_sabun + (3.59f - Math.sin(Math.toRadians(f)) * 3.59f);
        this.isu1.setTranslate(this.isu5.px, this.f_isu5_py - 0.108f, this.f_isu5_pz - 0.76f);
        this.isu2.setTranslate(this.isu5.px, this.f_isu5_py - 0.01f, this.f_isu5_pz - 0.71f);
        this.isu3.setTranslate(this.isu5.px - 0.24f, this.f_isu5_py + 0.2022f, this.f_isu5_pz - 1.02f);
        this.isu4.setTranslate(this.isu5.px - 0.24f + 3.24f, this.f_isu5_py + 0.1977f - 2.42f, this.f_isu5_pz - 1.02f - 2.67f);
        this.isu5.setTranslate(this.isu5.px, this.f_isu5_py, this.f_isu5_pz);
        this.isu6.setRotate(-37.1891f + f, this.isu6.ry, this.isu6.rz);
        this.isu6.setTranslate(this.isu6.px, this.isu6.py, 7.21629f - this.f_sabun);
        this.isu7.setRotate(-37.1891f + f, this.isu7.ry, this.isu7.rz);
        this.pinvot_sabun = 1.24f - 0.031297326f * (f + 2.439f);
        this.f_isu5_py = this.pinvot_sabun * Math.cos(Math.toRadians(39.62f));
        this.f_isu5_pz = this.pinvot_sabun * Math.sin(Math.toRadians(39.62f));
        this.isu7.setPivot(-3.0f, 0.05f - this.f_isu5_py, 6.55f + this.f_isu5_pz);
        this.isu7.setTranslate(this.isu7.px, 0.0f + this.f_isu5_py, 0.66629f - this.f_sabun + 0.0f - this.f_isu5_pz);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        System.sleep(1);
        this.loadarc(this.shion.getChild(0x1000000), "FLSshion_h.fpk");
        this.loadarc(this.masyu.getChild(0x1000000), "FLSmatehws.fpk");
        this.loadarc(this.hammr.getChild(0x1000000), "FLShammer.fpk");
        this.loadarc(this.chaos.getChild(0x1000000), "FLSchaos.fpk");
        this.shion.face.mtn(4, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.masyu.face.mtn(2, 8, 1.0f, false);
        this.masyu.face.start(4, null);
        this.hammr.face.mtn(2, 8, 1.0f, false);
        this.hammr.face.start(4, null);
        this.chaos.face.mtn(4, 8, 1.0f, false);
        this.chaos.face.start(4, null);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.isu_move(-2.439f);
        System.sleep(1);
        Stage.setVisible(51, false);
        Stage.setVisible(52, false);
        Stage.setVisible(53, false);
        Stage.setVisible(54, false);
        Stage.setVisible(55, false);
        Stage.setVisible(56, false);
        Stage.setVisible(57, false);
        this.cam1.change();
        Sound.streamPlay(1290014, 48000);
        this.camerawork.cut02();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, 0.663f, 0.218f, -0.717f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.708f, 0.388f, -0.59f);
        this.light.setColor(3, 0.17f, 0.17f, 0.17f);
        this.light.setDirection2(3, 0.144f, -0.756f, -0.638f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.semotare.setTranslate(-3.012498f, -0.029998777f, 1.6649985f);
        this.semotare.setRotate(0.0f, 6.4999995f, 0.0f);
        this.hammr.setTranslate(-3.04f, -0.51f, 1.68f);
        this.hammr.setRotate(-5.6f, 176.22f, -0.27f);
        this.masyu.setTranslate(-1.99f, -0.52f, 0.78f);
        this.masyu.setRotate(37.65f, 284.42f, 37.82f);
        this.hammr.start(1, "mtn_001");
        this.masyu.start(1, "mtn_002");
        System.sleep(66);
        this.MSGW("What the...? There it is again.\nThat's so weird...");
        this.dummy1.start(1, "hammr_serifu_02_1");
        System.sleep(69);
        System.sleep(36);
        this.dummy2.start(1, "masyu_serifu_02");
        System.sleep(6);
        this.MSGW("What is it?");
        System.sleep(36);
        this.MSGW("It's the catapult deck.\nThere seems to be something\nwrong with the hatch.");
        this.dummy1.start(1, "hammr_serifu_02_2");
        System.sleep(143);
        System.sleep(1);
        this.__wait();
        this.camerawork.cut03();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.446f, 0.673f, 0.59f);
        this.light.setColor(2, 0.41f, 0.41f, 0.41f);
        this.light.setDirection2(2, -0.176f, 0.412f, -0.894f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.semotare.setTranslate(-3.0374937f, -0.029998777f, 1.6149977f);
        this.semotare.setRotate(0.0f, -2.2499998f, 0.0f);
        this.hammr.setTranslate(-3.02f, -0.52f, 1.48f);
        this.hammr.setRotate(0.0f, 183.1f, 0.0f);
        this.masyu.setTranslate(-2.08f, -0.52f, 0.82f);
        this.masyu.setRotate(0.0f, 284.89f, 0.0f);
        this.chaos.setTranslate(3.01f, -0.48f, 1.55f);
        this.chaos.setRotate(-3.05f, 171.6f, -1.75f);
        this.masyu.start(1, "mtn_003");
        this.hammr.start(1, "mtn_004");
        this.chaos.start(1, "mtn_005");
        System.sleep(6);
        this.dummy2.start(1, "masyu_serifu_03");
        this.MSGW("Air pressure?");
        System.sleep(33);
        this.MSGW("No leaks at the moment.");
        this.dummy1.start(1, "hammr_serifu_03_1");
        System.sleep(90);
        this.MSGW("I wonder if it's the electrical system...");
        this.dummy2.start(1, "hammr_serifu_03_2");
        System.sleep(66);
        this.MSGW("Shall I take a look at it?");
        this.dummy1.start(1, "shion_serifu_03");
        System.sleep(23);
        System.sleep(1);
        this.__wait();
        this.camerawork.cut04_2();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.446f, 0.673f, 0.59f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, 0.525f, 0.218f, -0.823f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.hammr.face.mtn(4, 8, 1.0f, false);
        this.hammr.face.start(4, null);
        this.shion.setTranslate(-1.75f, -0.53f, 3.95f);
        this.shion.setRotate(2.5f, 197.97f, 0.0f);
        this.masyu.setTranslate(-1.99f, -0.52f, 1.03f);
        this.masyu.setRotate(0.0f, 283.75f, 0.0f);
        this.fm1.setArgs(2, 52, 0, 15, -1);
        this.shion.start(1, "mtn_006");
        this.hammr.start(1, "mtn_007");
        this.masyu.start(1, "mtn_008");
        System.sleep(51);
        this.dummy2.start(1, "masyu_serifu_04");
        this.MSGW("Ah, that'd be great.");
        System.sleep(71);
        System.sleep(1);
        this.__wait();
        this.shion.look_speed(0.0f);
        this.shion.look_eye_speed(5.2f);
        this.shion.look_char(this.masyu);
        this.camerawork.cut05();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.73f, 0.73f, 0.73f);
        this.light.setDirection2(1, 0.202f, 0.605f, 0.77f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.519f, 0.306f, -0.798f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.642f, -0.765f, 0.046f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.semotare.setTranslate(-3.0249827f, -0.029998777f, 1.6149933f);
        this.semotare.setRotate(0.0f, -15.999998f, 0.0f);
        this.masyu.setTranslate(-2.09f, -0.52f, 1.08f);
        this.masyu.setRotate(0.0f, 731.45f, 0.0f);
        this.masyu.start(1, "mtn_009");
        this.hammr.start(1, "mtn_010");
        System.sleep(6);
        this.dummy1.start(1, "masyu_serifu_05");
        this.MSGW("Doesn't look like a leak,\nbut be careful, just in case.");
        System.sleep(120);
        this.__wait();
        this.shion.face.mtn(34, 8, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(1);
        this.camerawork.cut06();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(1, -0.598f, 0.18f, -0.781f);
        this.light.setColor(2, 0.52f, 0.52f, 0.52f);
        this.light.setDirection2(2, 0.944f, 0.223f, -0.243f);
        this.light.setColor(3, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(3, 0.366f, -0.587f, -0.722f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setTranslate(-1.88f, -0.5f, 3.08f);
        this.shion.setRotate(0.0f, 187.17f, 0.0f);
        this.masyu.start(1, "mtn_011");
        this.shion.start(1, "mtn_012");
        this.wait_clr(9);
        this.dummy2.start(1, "shion_serifu_06");
        this.MSGW("Got it.");
        this.wait_clr(36);
        System.sleep(21);
        this.__wait();
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class human
            extends Chr {
        Chr face;

        public human(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
        }

        public void mtn_001() {
            this.mtn(257, 0, 0.67f, true);
        }

        public void mtn_002() {
            this.mtn(258, 0, 0.67f, true);
        }

        public void mtn_003() {
            this.mtn(259, 0, 0.77f, true);
            this.signal(1);
        }

        public void mtn_004() {
            this.mtn(260, 0, 0.77f, true);
        }

        public void mtn_005() {
            this.mtn(261, 0, 0.77f, true);
        }

        public void mtn_006() {
            this.mtn(262, 0, 0.92f, true);
        }

        public void mtn_007() {
            this.mtn(263, 0, 0.92f, true);
        }

        public void mtn_008() {
            this.mtn(264, 0, 0.92f, true);
        }

        public void mtn_009() {
            this.mtn(265, 0, 0.85f, true);
        }

        public void mtn_010() {
            this.mtn(266, 0, 0.85f, true);
        }

        public void mtn_011() {
            this.mtn(267, 0, 1.0f, true);
        }

        public void mtn_012() {
            this.mtn(268, 0, 1.0f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }

        public void door_close() {
            float f = -0.76f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02014A.this.cam2.getRotateX() - f;
                this.setTranslate(this.px - f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        public void door_open() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02014A.this.cam2.getRotateX() - f;
                this.setTranslate(this.px - f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        void hammr_serifu_02_1() {
            SCE02014A.this.FACE(27, SCE02014A.this.hammr, 1, 0.72f);
            System.sleep(12);
            SCE02014A.this.FACE_SMOOTH(21, SCE02014A.this.hammr, 1, 1.12f);
            System.sleep(12);
            SCE02014A.this.FACE_SMOOTH(36, SCE02014A.this.hammr, 7, 0.62f);
        }

        void hammr_serifu_02_2() {
            SCE02014A.this.FACE_SMOOTH(45, SCE02014A.this.hammr, 1, 1.22f);
            System.sleep(15);
            SCE02014A.this.FACE(75, SCE02014A.this.hammr, 1, 1.22f);
        }

        void hammr_serifu_03_1() {
            SCE02014A.this.FACE_SMOOTH(22, SCE02014A.this.hammr, 1, 1.0f);
            System.sleep(12);
            SCE02014A.this.FACE_SMOOTH(33, SCE02014A.this.hammr, 1, 1.0f);
        }

        void hammr_serifu_03_2() {
            SCE02014A.this.FACE_SMOOTH(54, SCE02014A.this.hammr, 7, 0.72f);
        }

        void masyu_serifu_02() {
            SCE02014A.this.FACE(24, SCE02014A.this.masyu, 1, 0.72f);
        }

        void masyu_serifu_03() {
            SCE02014A.this.FACE(24, SCE02014A.this.masyu, 1, 0.72f);
        }

        void masyu_serifu_04() {
            SCE02014A.this.FACE(45, SCE02014A.this.masyu, 1, 0.72f);
        }

        void masyu_serifu_05() {
            SCE02014A.this.FACE(54, SCE02014A.this.masyu, 1, 0.87f);
            System.sleep(15);
            SCE02014A.this.FACE(36, SCE02014A.this.masyu, 1, 0.92f);
        }

        void shion_serifu_03() {
            SCE02014A.this.FACE(16, SCE02014A.this.shion, 3, 1.12f);
            System.sleep(12);
            SCE02014A.this.FACE(25, SCE02014A.this.shion, 3, 1.12f);
        }

        void shion_serifu_06() {
            SCE02014A.this.FACE(19, SCE02014A.this.shion, 33, 1.21f);
        }
    }

    class mirror_map
            extends MAPUnit {
        mirror_map() {
        }

        public void door_close_2() {
            float f = -0.76f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02014A.this.cam2.getRotateX() - f;
                this.setTranslate(this.px + f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        public void door_open_2() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02014A.this.cam2.getRotateX() - f;
                this.setTranslate(this.px + f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut01() {
            SCE02014A.this.cam1.setFov(32.038f);
            SCE02014A.this.cam0.setFov(32.038f);
            float[] fArray = new float[]{1.0f, 0.81f, 0.52f, -5.81f, 177.0f, 0.7f, 0.52f, -5.85f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -5.02f;
            fArray2[2] = 881.28f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 0);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut02() {
            SCE02014A.this.cam1.setFov(27.43f);
            SCE02014A.this.cam0.setFov(27.43f);
            float[] fArray = new float[]{1.0f, -1.84f, 0.81f, -0.99f, 204.0f, -1.87f, 0.82f, -0.91f, 407.0f, -1.91f, 0.82f, -0.83f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.54f;
            fArray2[2] = 876.47f;
            fArray2[4] = 204.0f;
            fArray2[5] = -3.09f;
            fArray2[6] = 876.47f;
            fArray2[8] = 407.0f;
            fArray2[9] = -2.41f;
            fArray2[10] = 876.47f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 1);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut02_2() {
            SCE02014A.this.cam1.setFov(27.427f);
            SCE02014A.this.cam0.setFov(27.427f);
            float[] fArray = new float[]{1.0f, -2.04f, 1.01f, -0.65f, 57.0f, -2.04f, 1.01f, -0.65f, 227.0f, -2.03f, 1.01f, -0.67f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -12.24f;
            fArray2[2] = 874.59f;
            fArray2[4] = 22.0f;
            fArray2[5] = -8.24f;
            fArray2[6] = 878.59f;
            fArray2[8] = 67.0f;
            fArray2[9] = -5.56f;
            fArray2[10] = 882.45f;
            fArray2[12] = 187.0f;
            fArray2[13] = -5.56f;
            fArray2[14] = 882.45f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 0, 2, 227);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1, 3, 247);
        }

        public void cut03() {
            SCE02014A.this.cam1.setFov(26.69f);
            SCE02014A.this.cam0.setFov(26.69f);
            float[] fArray = new float[]{1.0f, -4.85f, 1.21f, 1.82f, 278.0f, -4.79f, 1.16f, 1.71f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.85f;
            fArray2[2] = 1005.13f;
            fArray2[4] = 278.0f;
            fArray2[5] = -10.85f;
            fArray2[6] = 1006.18f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 1);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut04_2() {
            SCE02014A.this.cam1.setFov(27.89f);
            SCE02014A.this.cam0.setFov(27.89f);
            float[] fArray = new float[]{1.0f, -2.97f, 1.05f, -0.86f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.21f;
            fArray2[2] = 912.95f;
            fArray2[4] = 177.0f;
            fArray2[5] = -5.61f;
            fArray2[6] = 912.95f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 0, 2, 0);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1, 2, 177);
        }

        public void cut05() {
            SCE02014A.this.cam1.setFov(27.274f);
            SCE02014A.this.cam0.setFov(27.274f);
            float[] fArray = new float[]{1.0f, -0.39f, 0.96f, 2.18f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.42f;
            fArray2[2] = 1147.25f;
            fArray2[4] = 777.0f;
            fArray2[5] = -3.47f;
            fArray2[6] = 1152.25f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 1);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut06() {
            SCE02014A.this.cam1.setFov(27.519f);
            SCE02014A.this.cam0.setFov(27.519f);
            float[] fArray = new float[]{1.0f, -1.78f, 1.09f, 0.27f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.8f;
            fArray2[2] = 1248.05f;
            fArray2[4] = 207.0f;
            fArray2[5] = -2.8f;
            fArray2[6] = 1251.07f;
            float[] fArray3 = fArray2;
            SCE02014A.this.cam1.transSPL(fArray, 1);
            SCE02014A.this.cam1.rotateSPL(fArray3, 1);
        }
    }
}

