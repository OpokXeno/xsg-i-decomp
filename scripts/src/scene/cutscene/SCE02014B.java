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
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02014B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        JNT_Human,
        Pack02014B,
        FLSshion_h {
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
    Effect eft1;
    Effect eft2;
    Effect eft3;
    Effect eft4;
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
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camera cam5;
    Camerawork camerawork = new Camerawork();
    human shion;
    units dummy;
    units dummy2;
    units konegi;
    units fm4;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
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
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02014B() {
    }

    void FACE(int n, Chr chr, int n2) {
        chr.mtn(n2, 8, 1.0f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        chr.start(4, null);
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
        while (true) {
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02014B_F");
        Runtime.setFlags(125, 1, 1);
        System.println("XEVEJNAME:CFJ2_120 XEVEJPOINT:POINT2_120");
        Runtime.jumpCF(560, 4);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(1340);
    }

    void init() {
        Runtime.setLocation(105);
        this.fm4 = new units();
        this.fm4.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm4.setArgs(0, 0.0f, 0.0f, 1.5343f, 1.04f);
        this.fm4.setArgs(1, 22025, 0, 128, 112);
        this.fm4.setArgs(2, 1, 0, 15, -1);
        this.fm4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
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
        this.shion = new human(0x100001E, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shion.setShadow(6, 22);
        this.shion.setMotNoUpdate(2);
        this.konegi = new units();
        this.konegi.init(24577, 1.17f, 110.5f, 3.36f, 4.88f);
        this.konegi.setScale(0.72f, 0.72f, 0.72f);
        this.dummy = new units();
        this.dummy.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.eft1 = new Effect(1520, -3.4999964f, 1.8999938f, -43.574867f, 0.0f);
        this.eft1.disp(false);
        this.eft1.setScale(1.0f, 1.0f, 1.0f);
        this.eft2 = new Effect(1520, -3.4249897f, 1.8249664f, -42.974113f, 0.0f);
        this.eft2.disp(false);
        this.eft2.setScale(1.27f, 1.27f, 1.27f);
        this.eft3 = new Effect(1416, -3.5749955f, 1.5999755f, -43.574867f, 0.0f);
        this.eft3.disp(false);
        this.eft3.setScale(1.0f, 1.0f, 1.0f);
        this.eft3.setForceLoop(true);
        this.eft4 = new Effect(1416, -3.5749955f, 1.5999755f, -43.574867f, 0.0f);
        this.eft4.disp(false);
        this.eft4.setScale(1.0f, 1.0f, 1.0f);
        this.eft4.setForceLoop(true);
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
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.shion.face.mtn(2, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.konegi.setParent(this.shion, 60);
        this.konegi.setTranslate(0.09699993f, -0.012749708f, -0.022999521f);
        this.konegi.setRotate(-191.79771f, 13.299985f, -84.44901f);
        this.fm4.setParent(this.shion, 60);
        this.fm4.setTranslate(0.23473537f, 0.02819491f, -0.15298927f);
        this.fm4.setRotate(-207.97408f, -38.217136f, -83.51375f);
        this.fm4.setArgs(0, 0.0f, 0.0f, 1.5343f, 1.04f);
        this.fm4.setArgs(2, 102, 0, 15, -1);
        this.fm4.setScale(0.23f, 0.23f, 0.23f);
        this.cam1.change();
        Sound.streamPlay(1290015, 48000);
        this.camerawork.cut02();
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.57f, 0.67f, 0.76f);
        this.light.setDirection2(1, -0.154f, 0.012f, -0.988f);
        this.light.setColor(2, 0.48f, 0.58f, 0.68f);
        this.light.setDirection2(2, 0.691f, 0.36f, 0.627f);
        this.light.setColor(3, 0.3f, 0.36f, 0.42f);
        this.light.setDirection2(3, -0.152f, -0.607f, 0.78f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fm4.signal(1);
        this.shion.setTranslate(0.08f, 1.0f, -34.42f);
        this.shion.setRotate(0.0f, -80.0f, 0.0f);
        this.shion.start(1, "mtn_001");
        System.sleep(36);
        this.MSGW("All right,");
        System.sleep(18);
        System.sleep(36);
        this.MSGW("if there's nothing there,\nI guess we're fine.");
        System.sleep(72);
        this.MSGW("Good work. Come on back.");
        this.wait_clr(60);
        this.wait_clr(15);
        this.dummy2.start(1, "horo_last_off");
        System.sleep(36);
        System.waitSignal(this.shion, 1);
        this.__wait();
        this.camerawork.cut03();
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.51f, 0.61f, 0.7f);
        this.light.setDirection2(1, 0.994f, 0.031f, 0.105f);
        this.light.setColor(2, 0.23f, 0.33f, 0.43f);
        this.light.setDirection2(2, -0.839f, 0.309f, 0.448f);
        this.light.setColor(3, 0.33f, 0.39f, 0.45f);
        this.light.setDirection2(3, 0.14f, -0.592f, 0.794f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setTranslate(0.07999999f, 0.99f, -34.42f);
        this.shion.setRotate(0.0f, -0.9999999f, 0.0f);
        this.shion.start(1, "mtn_003");
        System.sleep(151);
        System.sleep(1);
        this.camerawork.cut04();
        this.shion.start(1, "mtn_007");
        System.sleep(22);
        System.sleep(160);
        this.eft3.disp(true);
        this.wait_clr(22);
        this.eft4.disp(true);
        this.wait_clr(22);
        this.eft2.disp(true);
        this.wait_clr(12);
        this.eft1.disp(true);
        this.wait_clr(12);
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
        }

        void mtn_001() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(257, 0, 0.9f, true);
            this.signal(1);
        }

        void mtn_003() {
            this.mtn(259, 0, 1.0f, true);
        }

        void mtn_007() {
            this.mtn(259, 159, 371, 0, 0, 1.0f, true);
        }
    }

    class robo
            extends Chr {
        robo() {
        }
    }

    class units
            extends Unit {
        units() {
        }

        void horo_last_off() {
            float f = 1.04f;
            while (f > 0.1f) {
                SCE02014B.this.fm4.setArgs(0, 0.0f, 0.0f, 1.5343f, f);
                f -= 0.175f;
                System.sleep(1);
            }
            SCE02014B.this.fm4.setArgs(0, 0.0f, 0.0f, 1.5343f, 0.0f);
            SCE02014B.this.fm4.signal(0);
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

        public void cut01() {
            SCE02014B.this.cam1.setFov(25.95f);
            SCE02014B.this.cam0.setFov(25.95f);
            float[] fArray = new float[]{1.0f, 2.3f, 3.73f, -31.36f, 185.0f, 1.96f, 3.42f, -32.21f, 369.0f, 1.71f, 3.41f, -32.33f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -20.94f;
            fArray2[2] = -685.36f;
            fArray2[4] = 185.0f;
            fArray2[5] = -21.61f;
            fArray2[6] = -676.38f;
            fArray2[8] = 369.0f;
            fArray2[9] = -21.61f;
            fArray2[10] = -676.33f;
            float[] fArray3 = fArray2;
            SCE02014B.this.cam1.transSPL(fArray, 1);
            SCE02014B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut02() {
            SCE02014B.this.cam1.setFov(25.72f);
            SCE02014B.this.cam0.setFov(25.72f);
            float[] fArray = new float[]{1.0f, 0.71f, 2.98f, -33.65f, 327.0f, 0.66f, 2.91f, -33.68f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -29.76f;
            fArray2[2] = -672.96f;
            fArray2[4] = 327.0f;
            fArray2[5] = -29.3f;
            fArray2[6] = -674.31f;
            float[] fArray3 = fArray2;
            SCE02014B.this.cam1.transSPL(fArray, 1);
            SCE02014B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut03() {
            SCE02014B.this.cam1.setFov(27.74f);
            SCE02014B.this.cam0.setFov(27.74f);
            float[] fArray = new float[]{1.0f, 1.08f, 2.2f, -29.7f, 196.0f, 0.95f, 2.2f, -29.27f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.38f;
            fArray2[2] = 13.49f;
            fArray2[4] = 196.0f;
            fArray2[5] = -3.0f;
            fArray2[6] = 15.66f;
            float[] fArray3 = fArray2;
            SCE02014B.this.cam1.transSPL(fArray, 0);
            SCE02014B.this.cam1.rotateSPL(fArray3, 0);
        }

        public void cut04() {
            SCE02014B.this.cam1.setFov(24.1f);
            SCE02014B.this.cam0.setFov(24.1f);
            float[] fArray = new float[]{1.0f, 0.33600008f, 2.5079956f, -31.449999f, 54.0f, 0.33600008f, 2.5179956f, -31.449999f, 107.0f, 0.33600008f, 2.5679955f, -31.449999f, 160.0f, 0.33600008f, 2.6079957f, -31.449999f, 276.9f, 0.33700007f, 2.8679957f, -31.449999f, 345.8f, 0.33700007f, 2.9379957f, -31.449999f};
            float[] fArray2 = new float[24];
            fArray2[0] = 1.0f;
            fArray2[1] = -5.8f;
            fArray2[2] = 14.73f;
            fArray2[4] = 54.0f;
            fArray2[5] = -4.2400002f;
            fArray2[6] = 16.26f;
            fArray2[8] = 107.0f;
            fArray2[9] = -4.2400002f;
            fArray2[10] = 16.26f;
            fArray2[12] = 160.0f;
            fArray2[13] = -3.73f;
            fArray2[14] = 16.6f;
            fArray2[16] = 213.0f;
            fArray2[17] = -3.73f;
            fArray2[18] = 16.6f;
            fArray2[20] = 266.0f;
            fArray2[21] = -3.73f;
            fArray2[22] = 16.6f;
            float[] fArray3 = fArray2;
            SCE02014B.this.cam1.transSPL(fArray, 1);
            SCE02014B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut05() {
        }
    }
}

