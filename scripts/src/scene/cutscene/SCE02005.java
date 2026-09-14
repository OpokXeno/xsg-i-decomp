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
import xeno.map.MC_PRO07_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02005
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_PRO07_PRJ,
        Pack02005,
        FLSmarglis_h,
        FLSpelegri {
    Light light = new Light(0);
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    Unit unit0;
    Unit unit1;
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
    public static final float nc = 1.0E7f;
    Menu menu;
    Window win;
    Thread thread1;
    Thread thread2;
    Thread _spl_thread_main;
    Input pad1;
    Input pad0;
    int rnd_mize_flag = 0;
    int isu1_flag = 1;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camerawork camerawork = new Camerawork();
    Unit gold_unit;
    Chr gold;
    robo dummy1;
    robo dummy2;
    robo magls;
    robo pereg;
    robo ope_1;
    robo ope_2;
    robo ope_3;
    robo ope_4;
    Unit utic_fm;
    Unit utic_fm2;
    Unit utic_fm3;
    MAPUnit isu1;
    MAPUnit isu2;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    int mize_a = 20;
    int mize_b = 7;
    int mize_c = 20;
    int mize_e = 7;
    int mize_f = 7;
    int mize_g = 7;
    int mize_count = 0;
    float masyu_a = 22.0f;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02005() {
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
        float f7 = 0.0f;
        while (true) {
            if (this.isu1_flag == 1) {
                this.unit0.getRotate();
                this.isu1.setRotate(this.isu1.rx, this.unit0.ry, this.isu1.rz);
            }
            if (this.rnd_mize_flag == 1) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = (f % (float) this.mize_a + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f3 = (f % (float) this.mize_b + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f4 = (f % (float) this.mize_c + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f5 = (f % (float) this.mize_e + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f6 = (f % (float) this.mize_f + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f7 = (f % (float) this.mize_g + (f - (float) ((int) f))) / 100.0f;
                this.cam2.setTranslate(this.cam1.getTranslateX() + f2, this.cam1.getTranslateY() + f3, this.cam1.getTranslateZ() + f4);
                this.cam2.setRotate(this.cam1.getRotateX() + f5, this.cam1.getRotateY() + f6, this.cam1.getRotateZ() + f7);
                if (this.mize_count == 0) {
                    this.rnd_mize_flag = 0;
                }
                --this.mize_count;
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02005_F");
        Runtime.setFlags(109, 1, 1);
        System.println("XEVEJNAME:CFJ2_60 XEVEJPOINT:POINT2_60");
        Runtime.jumpCF(852, 3);
    }

    void init() {
        Runtime.setLocation(1106);
        this.utic_fm = new Unit();
        this.utic_fm.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm.setArgs(1, 20056, 0, 128, 112);
        this.utic_fm.setArgs(2, 52, 0, 15, -1);
        this.utic_fm.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm.signal(1);
        this.utic_fm.setScale(1.32f, 1.32f, 1.32f);
        this.utic_fm.setTranslate(0.53f, 2.03f, -3.03f);
        this.utic_fm.setRotate(0.0f, -90.0f, 0.0f);
        this.utic_fm2 = new Unit();
        this.utic_fm2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm2.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm2.setArgs(1, 20047, 0, 128, 112);
        this.utic_fm2.setArgs(2, 52, 0, 15, -1);
        this.utic_fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm2.signal(1);
        this.utic_fm2.setScale(0.72f, 0.72f, 0.72f);
        this.utic_fm2.setTranslate(0.5f, 1.94f, 0.81f);
        this.utic_fm2.setRotate(-0.05f, 0.0f, 0.0f);
        this.utic_fm3 = new Unit();
        this.utic_fm3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm3.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm3.setArgs(1, 20051, 0, 128, 112);
        this.utic_fm3.setArgs(2, 52, 0, 15, -1);
        this.utic_fm3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm3.signal(1);
        this.utic_fm3.setScale(0.72f, 0.72f, 0.72f);
        this.utic_fm3.setTranslate(0.5f, 1.94f, -6.79f);
        this.utic_fm3.setRotate(-0.05f, 0.0f, 0.0f);
        this.unit0 = new units();
        this.unit0.init(24582, 0.0f, 110.0f, 0.0f, 0.0f);
        this.unit0.setVisible(false);
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
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.isu1 = new mirror_map();
        this.isu1.init(130);
        this.isu1.start(4, null);
        this.isu1.setPivot(-5.84f, 3.76f, -4.143f);
        this.isu2 = new mirror_map();
        this.isu2.init(131);
        this.isu2.start(4, null);
        this.isu2.setPivot(-5.84f, 3.76f, -4.143f);
        this.magls = new robo();
        this.magls.init(16777530, 0.0f, 0.0f, 0.0f, 0.0f);
        this.magls.face = this.magls.getChild(0x1000000);
        this.pereg = new robo();
        this.pereg.init(16777516, 0.0f, 0.0f, 0.0f, 0.0f);
        this.pereg.face = this.pereg.getChild(0x1000000);
        this.ope_1 = new robo();
        this.ope_1.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ope_2 = new robo();
        this.ope_2.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ope_3 = new robo();
        this.ope_3.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ope_4 = new robo();
        this.ope_4.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1 = new robo();
        this.dummy1.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2 = new robo();
        this.dummy2.init(778, 0.0f, 0.0f, 0.0f, 0.0f);
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
        this.magls.setShadow(5, 22);
        this.pereg.setShadow(5, 22);
        this.ope_1.setShadow(0, 0);
        this.ope_2.setShadow(0, 0);
        this.ope_3.setShadow(0, 0);
        this.ope_4.setShadow(0, 0);
        this.loadarc(this.magls.getChild(0x1000000), "FLSmarglis_h.fpk");
        this.loadarc(this.pereg.getChild(0x1000000), "FLSpelegri.fpk");
        this.magls.face.mtn(1, 8, 1.0f, false);
        this.magls.face.start(4, null);
        this.pereg.face.mtn(4, 8, 1.0f, false);
        this.pereg.face.start(4, null);
        this.unit0.transCNS(this.ope_1, 13, 0.0f, 0.0f, 0.0f);
        this.unit0.rotYCNS(this.ope_1, 13, 0.0f, -27.0f, 1.0f);
        Stage.setVisible(125, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        this.magls.setTranslate(-0.7f, 0.5f, -3.0f);
        this.magls.setRotate(0.0f, -91.7f, 1.68f);
        this.ope_1.setTranslate(-5.9099793f, 0.69499964f, -4.3599834f);
        this.ope_1.setRotate(18.333323f, -95.2f, 0.0f);
        this.ope_2.setTranslate(-5.2299967f, 0.58499956f, -6.3699985f);
        this.ope_2.setRotate(0.0f, -123.19999f, 0.0f);
        this.ope_3.setTranslate(-5.059998f, 0.5799996f, 0.6649999f);
        this.ope_3.setRotate(0.0f, -54.099995f, 0.0f);
        this.ope_4.setTranslate(-5.8499994f, 0.6049996f, -1.74f);
        this.ope_4.setRotate(0.0f, -88.99999f, 0.0f);
        this.pereg.setTranslate(0.11f, 0.49f, -3.57f);
        this.pereg.setRotate(0.0f, -77.9f, 0.0f);
        this.ope_1.start(1, "mtn_001_1");
        this.ope_2.start(1, "mtn_001_2");
        this.ope_3.start(1, "mtn_001_3");
        this.ope_4.start(1, "mtn_001_4");
        this.magls.start(1, "mtn_002");
        this.pereg.start(1, "mtn_003");
        this.cam1.change();
        Sound.streamPlay(1290009, 48000);
        this.camerawork.cut01();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(111);
        this.__wait();
        this.camerawork.cut02();
        this.ope_1.start(1, "mtn_004");
        this.MSGW("Commander!\nThe intruder is after the 100-Series!");
        this.pereg.start(1, "mtn_006_mae");
        this.wait_clr(84);
        this.wait_clr(9);
        this.__wait();
        this.camerawork.cut03();
        this.magls.start(1, "mtn_005");
        this.pereg.start(1, "mtn_006");
        this.ope_1.setTranslate(-5.91f, 0.11f, -4.06f);
        this.wait_clr(9);
        this.MSGW("Really...Probably one of\nthe government's dogs.");
        this.dummy1.start(1, "magls_serifu_03");
        System.sleep(36);
        System.sleep(69);
        this.MSGW("The intruder disappeared from\nour sensors near D Block.");
        this.isu1_flag = 0;
        System.sleep(59);
        System.sleep(1);
        this.__wait();
        this.camerawork.cut04();
        Stage.setVisible(114, false);
        this.isu1.setRotate(0.0f, 57.78f, 0.0f);
        this.ope_1.setTranslate(-5.7949786f, 0.5149996f, -4.144979f);
        this.ope_1.setRotate(0.0f, -98.4f, -6.666667f);
        this.ope_1.start(1, "mtn_007");
        this.ope_2.start(1, "mtn_007");
        this.magls.start(1, "mtn_002_b");
        this.pereg.start(1, "mtn_003_b");
        this.wait_clr(30);
        System.sleep(9);
        this.MSGW("All units are currently\npatrolling the area.");
        System.sleep(84);
        this.__wait();
        this.camerawork.cut05();
        this.magls.start(1, "mtn_008");
        this.pereg.start(1, "mtn_009");
        this.MSGW("Surround D Block. We'll fence them\nin from both sides.");
        this.dummy2.start(1, "magls_serifu_05");
        System.sleep(54);
        System.sleep(54);
        this.__wait();
        this.MSGW("Yes, Sir.");
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

        robo() {
        }

        void magls_serifu_03() {
            SCE02005.this.FACE(24, SCE02005.this.magls.face, 1);
            System.sleep(12);
            SCE02005.this.FACE(51, SCE02005.this.magls.face, 1);
        }

        void magls_serifu_05() {
            SCE02005.this.FACE(43, SCE02005.this.magls.face, 3);
            System.sleep(12);
            SCE02005.this.FACE(46, SCE02005.this.magls.face, 3);
        }

        public void mtn_001_1() {
            this.mtn(257, 8, 1.0f, true);
        }

        public void mtn_001_2() {
            this.setScale(-1.0f, 1.0f, 1.0f);
            this.mtn(257, 25, 100, 8, 0, 1.0f, true);
            this.mtn(257, 8, 1.0f, true);
        }

        public void mtn_001_3() {
            this.setScale(-1.0f, 1.0f, 1.0f);
            this.mtn(257, 50, 100, 8, 0, 1.0f, true);
            this.mtn(257, 8, 1.0f, true);
        }

        public void mtn_001_4() {
            this.mtn(257, 75, 100, 8, 0, 1.0f, true);
            this.mtn(257, 8, 1.0f, true);
        }

        public void mtn_002() {
            this.mtn(258, 8, 1.0f, true);
        }

        public void mtn_002_b() {
            this.mtn(258, 8, 0.82f, true);
        }

        public void mtn_003() {
            this.mtn(259, 8, 1.0f, true);
        }

        public void mtn_003_b() {
            this.mtn(259, 8, 0.82f, true);
        }

        public void mtn_004() {
            this.mtn(260, 0, 1.0f, true);
        }

        public void mtn_005() {
            this.mtn(261, 0, 0.85f, true);
        }

        public void mtn_006() {
            this.mtn(262, 0, 0.85f, true);
        }

        public void mtn_006_mae() {
            this.mtn(262, 0, 0, 0, 0, 0.85f, true);
        }

        public void mtn_007() {
            this.mtn(263, 0, 1.0f, true);
        }

        public void mtn_008() {
            this.mtn(264, 0, 1.0f, true);
        }

        public void mtn_009() {
            this.mtn(265, 0, 1.0f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }
    }

    class mirror_map
            extends MAPUnit {
        mirror_map() {
        }
    }

    class Camerawork
            extends Camera {
        Chr face;

        Camerawork() {
        }

        public void cut01() {
            SCE02005.this.cam1.setFov(30.34f);
            SCE02005.this.cam0.setFov(30.34f);
            float[] fArray = new float[]{1.0f, -4.51f, 4.16f, 9.03f, 192.0f, -4.48f, 4.12f, 8.76f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.96f;
            fArray2[2] = -5.92f;
            fArray2[4] = 192.0f;
            fArray2[5] = -13.96f;
            fArray2[6] = -4.59f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[]{1.0f, 30.34f, 192.0f, 29.27f};
            SCE02005.this.cam1.transSPL(fArray, 1);
            SCE02005.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut02() {
            SCE02005.this.cam1.setFov(27.45f);
            SCE02005.this.cam0.setFov(27.45f);
            float[] fArray = new float[]{1.0f, -5.56f, 1.57f, -2.35f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.43f;
            fArray2[2] = 2.73f;
            float[] fArray3 = fArray2;
            SCE02005.this.cam1.transSPL(fArray, 1);
            SCE02005.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut03() {
            SCE02005.this.cam1.setFov(23.684f);
            SCE02005.this.cam0.setFov(23.684f);
            float[] fArray = new float[]{1.0f, -1.95f, 2.45f, -2.69f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -15.71f;
            fArray2[2] = -73.51f;
            float[] fArray3 = fArray2;
            SCE02005.this.cam1.transSPL(fArray, 1);
            SCE02005.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut04() {
            SCE02005.this.cam1.setFov(29.75f);
            SCE02005.this.cam0.setFov(29.75f);
            float[] fArray = new float[]{1.0f, -7.69f, 1.31f, -3.98f, 403.0f, -7.7f, 1.31f, -3.82f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -1.65f;
            fArray2[2] = -91.35f;
            fArray2[4] = 403.0f;
            fArray2[5] = -1.65f;
            fArray2[6] = -91.35f;
            float[] fArray3 = fArray2;
            SCE02005.this.cam1.transSPL(fArray, 1);
            SCE02005.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut05() {
            SCE02005.this.cam1.setFov(29.76f);
            SCE02005.this.cam0.setFov(29.76f);
            float[] fArray = new float[]{1.0f, -4.17f, 1.67f, -3.35f, 176.0f, -3.43f, 1.67f, -3.3f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 0.05f;
            fArray2[2] = -93.83f;
            fArray2[4] = 176.0f;
            fArray2[5] = 1.85f;
            fArray2[6] = -93.83f;
            float[] fArray3 = fArray2;
            SCE02005.this.cam1.transSPL(fArray, 1);
            SCE02005.this.cam1.rotateSPL(fArray3, 1);
        }
    }
}

