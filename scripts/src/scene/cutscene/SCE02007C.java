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
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02007C
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02007C,
        FLSmomo_h,
        FLSziggy_h {
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    public static final float nc = 1.0E7f;
    Effect flash;
    Effect eft0;
    Effect ship2_efa;
    Effect ship2_efb;
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
    human ziggy;
    human momo;
    robo escp_ship;
    robo escp_ship2;
    units dummy1;
    units dummy2;
    units star;
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
    int camera_flag = 0;
    int scale_flag = 0;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02007C() {
    }

    void FACE(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 8, 1.0f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
    }

    void FACE2(int n, human human2, int n2) {
        human2.face.mtn(n2, 8, 1.32f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 9, f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
    }

    void FACE_SMOOTH2(int n, human human2, int n2) {
        human2.face.mtn(n2, 9, 1.32f, false);
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

    void __chracter_splne() {
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        float f4 = 0.0f;
        float f5 = 0.0f;
        float f6 = 0.0f;
        float f7 = 0.0f;
        while (true) {
            this.escp_ship2.setTranslate(this.escp_ship.px, this.escp_ship.py, this.escp_ship.pz);
            this.escp_ship2.setRotate(this.escp_ship.rx, this.escp_ship.ry, this.escp_ship.rz);
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

    void __over_wrap__on() {
        int[] nArray = new int[5];
        nArray[0] = 0x3000000;
        int[] nArray2 = nArray;
        Runtime.setDefocus(0, 10, nArray2);
        System.sleep(1);
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02007C_F");
        Runtime.setFlags(113, 1, 1);
        System.println("XEVEJNAME:SCE02008");
        Runtime.jumpEvent(2080);
    }

    void init() {
        Stage.clrBackBuffer();
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
        this.ziggy = new human(16777251, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo = new human(0x1000021, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo.setMotNoUpdate(2);
        this.momo.setMotionFlags(0x1000000, false);
        this.momo.setMotionFlags(0x800000, false);
        this.momo.setMotionFlags(0x2000000, true);
        this.escp_ship = new robo();
        this.escp_ship.init(20497, 0.0f, 0.0f, 0.0f, 0.0f);
        this.escp_ship2 = new robo();
        this.escp_ship2.init(20591, 0.0f, 0.0f, 0.0f, 0.0f);
        this.escp_ship2.setVisible(2, false);
        this.star = new units();
        this.star.init(20615, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy1.setRotate(-0.0f, 0.0f, -1.27f);
        this.dummy1.setParent(this.escp_ship2, 7);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy2.setParent(this.escp_ship2, 10);
        this.dummy2.setRotate(-0.0f, 0.0f, -1.27f);
        this.eft0 = new Effect(1466, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.eft0.setCaster(this.escp_ship);
        this.eft0.disp(true);
        this.eft0.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.ship2_efa = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ship2_efa.setCaster(this.dummy1);
        this.ship2_efa.setRotate(0.0f, -90.0f, 0.0f);
        this.ship2_efa.setScale(0.27f, 0.32f, 0.27f);
        this.ship2_efa.disp(true);
        this.ship2_efb = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ship2_efb.setCaster(this.dummy2);
        this.ship2_efb.setRotate(0.0f, -90.0f, 0.0f);
        this.ship2_efb.setScale(0.27f, 0.32f, 0.27f);
        this.ship2_efb.disp(true);
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
        Stage.setEventFade(30, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f);
        this.loadarc(this.momo.face, "FLSmomo_h.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy_h.fpk");
        this.momo.face.mtn(2, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.ziggy.face.mtn(4, 81, 81, 0, 0, 1.0f, false);
        this.ziggy.face.start(4, null);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.escp_ship.setVisible(1, false);
        this.momo.setTranslate(0.044999786f, 6.209988f, 6.5549755f);
        this.momo.setRotate(0.0f, -5.9999995f, 0.0f);
        this.ziggy.setTranslate(-0.008000008f, 6.174963f, 7.3139896f);
        this.ziggy.setRotate(4.8666663f, 0.0f, 0.0f);
        this.momo.start(1, "mtn_001_momo");
        this.ziggy.start(1, "mtn_001");
        this.star.start(1, "amanogawa_stat");
        this.escp_ship2.setLightMode(1);
        this.escp_ship2.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.escp_ship2.light.setColor(1, 0.52f, 0.52f, 0.52f);
        this.escp_ship2.light.setDirection2(1, -0.961f, 0.221f, 0.168f);
        this.escp_ship2.light.setColor(2, 0.9f, 0.9f, 0.9f);
        this.escp_ship2.light.setDirection2(2, 0.695f, -0.254f, 0.673f);
        this.escp_ship2.light.setColor(3, 0.06f, 0.09f, 0.11f);
        this.escp_ship2.light.setDirection2(3, -0.826f, 0.561f, -0.062f);
        this.cam1.change();
        Sound.streamPlay(1290070, 48000);
        this.camerawork.cut001_1();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.42f, 0.67f, 0.6f);
        this.light.setDirection2(1, 0.343f, 0.0f, 0.939f);
        this.light.setColor(2, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(2, -0.738f, 0.24f, -0.631f);
        this.light.setColor(3, 0.2f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.078f, -0.566f, 0.821f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(142);
        this.__wait();
        Runtime.mpeg2("2007C_1");
        Sound.streamPlay(1290072, 48000);
        this.camerawork.cut001_2();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.42f, 0.67f, 0.6f);
        this.light.setDirection2(1, 0.343f, 0.0f, 0.939f);
        this.light.setColor(2, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(2, -0.738f, 0.24f, -0.631f);
        this.light.setColor(3, 0.2f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.078f, -0.566f, 0.821f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.escp_ship.setVisible(true);
        this.escp_ship2.setVisible(true);
        System.sleep(66);
        this.MSGW("Is...something wrong?");
        System.sleep(18);
        this.__wait();
        this.camerawork.cut003();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.42f, 0.67f, 0.6f);
        this.light.setDirection2(1, 0.416f, 0.571f, 0.708f);
        this.light.setColor(2, 0.28f, 0.32f, 0.32f);
        this.light.setDirection2(2, -0.982f, 0.143f, 0.123f);
        this.light.setColor(3, 0.2f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.447f, -0.475f, 0.758f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ziggy.start(1, "mtn_002");
        this.momo.start(1, "mtn_003");
        this.dummy1.start(1, "momo_serifu_003");
        this.wait_clr(36);
        this.dummy2.start(1, "ziggy_serifu_003_1");
        this.wait_clr(36);
        System.sleep(30);
        this.dummy1.start(1, "ziggy_serifu_003_2");
        this.MSGW("No, it's nothing.");
        this.wait_clr(60);
        this.MSGW("MOMO.");
        this.dummy1.start(1, "ziggy_serifu_003_3");
        System.sleep(30);
        this.MSGW("Find a trade column and\nshort-jump us out of here.");
        System.sleep(114);
        this.__wait();
        this.momo.start(1, "mtn_005");
        this.camerawork.cut002();
        this.star.start(1, "amanogawa_stat2");
        this.ziggy.setTranslate(-0.00500004f, 6.174963f, 7.384992f);
        this.ziggy.setRotate(0.0f, 0.0f, 0.5333333f);
        this.ziggy.start(1, "mtn_004");
        System.sleep(9);
        this.dummy1.start(1, "ziggy_serifu_002");
        this.MSGW("I want to get back on our return \npath, once we shake off the pursuit.");
        System.sleep(72);
        this.momo.face.mtn(16, 0, 120, 7, 9, 1.0f, false);
        this.momo.face.start(4, null);
        System.sleep(27);
        this.MSGW("Okay!");
        this.dummy2.start(1, "momo_serifu_002");
        this.wait_clr(51);
        System.waitSignal(this.momo, 2);
        this.camerawork.cut_black();
        this.__wait();
        Runtime.mpeg2("2007C_2");
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

        void mtn_001() {
            this.mtn(257, 8, 1.0f, true);
        }

        void mtn_001_momo() {
            this.mtn(259, 0, 0, 0, 0, 1.0f, true);
        }

        void mtn_002() {
            this.mtn(258, 0, 1.2f, true);
        }

        void mtn_003() {
            this.mtn(259, 0, 1.2f, true);
        }

        void mtn_004() {
            this.mtn(260, 0, 1.0f, true);
        }

        void mtn_005() {
            this.mtn(261, 52, 239, 0, 0, 1.0f, true);
            this.signal(2);
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

        void amanogawa() {
            this.setTranslate(0.0f, 0.0f, -22.87f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setScale(1.77f, 1.77f, 1.77f);
            while (true) {
                this.setTranslate(SCE02007C.this.star.px, SCE02007C.this.star.py, SCE02007C.this.star.pz - 0.1f);
                System.sleep(1);
            }
        }

        void amanogawa_spin() {
            this.setScale(1.77f, 1.77f, 1.77f);
            while (true) {
                this.setRotate(SCE02007C.this.star.rx, SCE02007C.this.star.ry + 0.027f, SCE02007C.this.star.rz);
                System.sleep(1);
            }
        }

        void amanogawa_stat() {
            this.setTranslate(345.28818f, -0.1f, -155.81477f);
            this.setRotate(775.06665f, -2939.7927f, -1377.7327f);
            this.setScale(1.77f, 1.77f, 1.77f);
            while (true) {
                this.setTranslate(SCE02007C.this.star.px, SCE02007C.this.star.py, SCE02007C.this.star.pz - 0.072f);
                System.sleep(1);
            }
        }

        void amanogawa_stat2() {
            this.setTranslate(62.4f, 8.0f, -108.93f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setScale(1.77f, 1.77f, 1.77f);
            while (true) {
                this.setTranslate(SCE02007C.this.star.px, SCE02007C.this.star.py, SCE02007C.this.star.pz - 0.072f);
                System.sleep(1);
            }
        }

        void momo_serifu_002() {
            SCE02007C.this.FACE(7, SCE02007C.this.momo, 15, 0.92f);
        }

        void momo_serifu_003() {
            SCE02007C.this.FACE(36, SCE02007C.this.momo, 1, 1.0f);
        }

        void ziggy_serifu_002() {
            System.sleep(6);
            SCE02007C.this.FACE(30, SCE02007C.this.ziggy, 1, 1.0f);
            System.sleep(12);
            SCE02007C.this.FACE(45, SCE02007C.this.ziggy, 1, 1.0f);
        }

        void ziggy_serifu_003_1() {
            SCE02007C.this.ziggy.face.mtn(4, 0, 104, 0, 0, -0.27f, false);
            SCE02007C.this.ziggy.face.start(4, null);
            System.sleep(30);
            SCE02007C.this.ziggy.face.mtn(4, 8, 1.0f, false);
            SCE02007C.this.ziggy.face.start(4, null);
        }

        void ziggy_serifu_003_2() {
            SCE02007C.this.FACE_SMOOTH(21, SCE02007C.this.ziggy, 1, 0.92f);
        }

        void ziggy_serifu_003_3() {
            SCE02007C.this.FACE(12, SCE02007C.this.ziggy, 1, 1.2f);
            System.sleep(15);
            SCE02007C.this.FACE(81, SCE02007C.this.ziggy, 1, 0.92f);
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

        public void cut000() {
            SCE02007C.this.cam1.setFov(28.9f);
            SCE02007C.this.cam0.setFov(28.9f);
            float[] fArray = new float[]{1.0f, -0.97f, 7.0f, 8.01f, 45.0f, -0.97f, 7.0f, 8.01f, 89.0f, -0.97f, 7.0f, 8.01f, 133.0f, -0.96f, 7.0f, 8.0f, 177.0f, -0.95f, 7.0f, 7.99f};
            float[] fArray2 = new float[20];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.2f;
            fArray2[2] = -65.27f;
            fArray2[4] = 45.0f;
            fArray2[5] = -3.61f;
            fArray2[6] = -61.85f;
            fArray2[8] = 89.0f;
            fArray2[9] = 3.33f;
            fArray2[10] = -58.4f;
            fArray2[12] = 133.0f;
            fArray2[13] = 8.08f;
            fArray2[14] = -56.88f;
            fArray2[16] = 177.0f;
            fArray2[17] = 8.59f;
            fArray2[18] = -56.6f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut001_1() {
            SCE02007C.this.cam1.setFov(24.81f);
            SCE02007C.this.cam0.setFov(24.81f);
            float[] fArray = new float[]{1.0f, -0.81f, 7.08f, 7.9f, 178.0f, -0.76f, 7.11f, 7.87f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 9.52f;
            fArray2[2] = -55.62f;
            fArray2[4] = 178.0f;
            fArray2[5] = 9.52f;
            fArray2[6] = -55.62f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut001_2() {
            SCE02007C.this.cam1.setFov(24.81f);
            SCE02007C.this.cam0.setFov(24.81f);
            float[] fArray = new float[]{100.0f, -0.76f, 7.11f, 7.87f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 9.52f;
            fArray2[2] = -55.62f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut002() {
            SCE02007C.this.cam1.setFov(37.16f);
            SCE02007C.this.cam0.setFov(37.16f);
            float[] fArray = new float[]{1.0f, -0.51f, 7.33f, 9.07f, 368.0f, -0.6f, 7.32f, 8.99f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.72f;
            fArray2[2] = -15.55f;
            fArray2[4] = 368.0f;
            fArray2[5] = -7.99f;
            fArray2[6] = -15.55f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut003() {
            SCE02007C.this.cam1.setFov(33.51f);
            SCE02007C.this.cam0.setFov(33.51f);
            float[] fArray = new float[]{1.0f, -0.29f, 7.44f, 8.49f, 328.0f, -0.38f, 7.44f, 8.48f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.7f;
            fArray2[2] = -10.62f;
            fArray2[4] = 328.0f;
            fArray2[5] = -13.7f;
            fArray2[6] = -12.06f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut004() {
            SCE02007C.this.cam1.setFov(40.8f);
            SCE02007C.this.cam0.setFov(40.8f);
            float[] fArray = new float[]{1.0f, -1.0f, 6.95f, 7.7f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 10.28f;
            fArray2[2] = -53.36f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut007() {
            SCE02007C.this.cam1.setFov(32.27f);
            SCE02007C.this.cam0.setFov(32.27f);
            float[] fArray = new float[]{1.0f, -14.04f, 1.22f, -7.17f};
            float[] fArray2 = new float[28];
            fArray2[0] = 1.0f;
            fArray2[1] = 5.32f;
            fArray2[2] = 258.59f;
            fArray2[4] = 19.0f;
            fArray2[5] = 4.2f;
            fArray2[6] = 244.28f;
            fArray2[8] = 37.0f;
            fArray2[9] = 6.16f;
            fArray2[10] = 238.85f;
            fArray2[12] = 55.0f;
            fArray2[13] = 4.2f;
            fArray2[14] = 244.28f;
            fArray2[16] = 177.0f;
            fArray2[17] = 4.2f;
            fArray2[18] = 244.28f;
            fArray2[20] = 277.0f;
            fArray2[21] = 4.2f;
            fArray2[22] = 244.28f;
            fArray2[24] = 377.0f;
            fArray2[25] = 4.2f;
            fArray2[26] = 244.28f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut01() {
            SCE02007C.this.cam1.setFov(42.12f);
            SCE02007C.this.cam2.setFov(42.12f);
            SCE02007C.this.cam0.setFov(42.12f);
            float[] fArray = new float[]{1.0f, -35.89f, 0.56f, -51.51f, 10.0f, -35.89f, 0.56f, -51.51f, 19.0f, -35.89f, 0.56f, -51.51f, 28.0f, -35.89f, 0.56f, -51.51f, 37.0f, -35.89f, 0.56f, -51.51f, 47.0f, -35.89f, 0.56f, -51.51f};
            float[] fArray2 = new float[24];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.83f;
            fArray2[2] = -29.09f;
            fArray2[4] = 10.0f;
            fArray2[5] = -2.83f;
            fArray2[6] = -38.79f;
            fArray2[8] = 19.0f;
            fArray2[9] = -2.83f;
            fArray2[10] = -99.39f;
            fArray2[12] = 28.0f;
            fArray2[13] = -2.83f;
            fArray2[14] = -153.23f;
            fArray2[16] = 37.0f;
            fArray2[17] = -2.83f;
            fArray2[18] = -166.55f;
            fArray2[20] = 47.0f;
            fArray2[21] = -2.83f;
            fArray2[22] = -167.26f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1, 3, 46);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1, 3, 46);
        }

        public void cut01_ato() {
            SCE02007C.this.cam1.setFov(42.15f);
            SCE02007C.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, 7.02f, 7.28f, -275.37f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -14.9f;
            fArray2[2] = -176.92f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 0);
            SCE02007C.this.cam1.rotateSPL(fArray3, 0);
        }

        public void cut01_mae() {
            SCE02007C.this.cam1.setFov(42.15f);
            SCE02007C.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, -38.14f, 5.23f, 105.73f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.66f;
            fArray2[2] = -21.11f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut_black() {
            SCE02007C.this.cam1.setFov(0.0f);
            float[] fArray = new float[]{1.0f, -35.89f, 1000.56f, -51.51f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.83f;
            fArray2[2] = -29.09f;
            float[] fArray3 = fArray2;
            SCE02007C.this.cam1.transSPL(fArray, 1, 3, 46);
            SCE02007C.this.cam1.rotateSPL(fArray3, 1, 3, 46);
        }
    }
}

