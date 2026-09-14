import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK01_PRJ;
import xeno.map.MC_VOK26_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01003B1
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01003,
        MC_VOK01_PRJ,
        MC_VOK26_PRJ,
        JNT_Human,
        FLSallen_h,
        FLSoff_m,
        FLSshion_h {
    public shion shion;
    public allen allen;
    public op_a op_a;
    public kosmos kosmos;
    public target_b1 target_b1;
    public tab tab;
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    static final float op_a_x = 4.78f;
    static final float op_a_y = 0.02f;
    static final float op_a_z = -9.1f;
    static final float op_b_x = 4.78f;
    static final float op_b_y = 0.02f;
    static final float op_b_z = -4.1f;
    static final float op_c_x = 4.78f;
    static final float op_c_y = 0.02f;
    static final float op_c_z = 0.9f;
    static final float op_d_x = -3.68f;
    static final float op_d_y = 0.02f;
    static final float op_d_z = -9.1f;
    static final float op_e_x = -3.68f;
    static final float op_e_y = 0.02f;
    static final float op_e_z = -4.1f;
    static final float op_f_x = -3.68f;
    static final float op_f_y = 0.02f;
    static final float op_f_z = 0.9f;
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    int BGinit = 0;
    Unit Box;
    Unit Elevator;
    Monitor fma1;
    Monitor fma2;
    Monitor fma3;
    Monitor fma1b;
    KB keyboard;
    KB CGMon;
    Unit CG;
    Unit headset;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01003B1() {
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
        System.println("XEVEFLAG:EV01003B1_F");
        Runtime.setFlags(4, 1, 1);
        System.println("XEVEJNAME:CFJ1_B3 XEVEJPOINT:POINT_B3");
        Runtime.jumpCF(9003, 0);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(1032);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(41);
        Runtime.setLocation(16);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.Box = new MapUnits();
        this.Box.mapUnit(12);
        this.Box.setTranslate(0.0f, 0.0f, -4.5f);
        this.Box.start(4, null);
        this.Elevator = new MapUnits();
        this.Elevator.mapUnit(4);
        this.Elevator.setTranslate(0.0f, 0.0f, 0.0f);
        this.Elevator.start(4, null);
        this.headset = new Unit();
        this.headset.init(24580);
        this.fma1 = new Monitor();
        this.fma1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fma1.setArgs(1, 20004, 0, 128, 112);
        this.fma1.setArgs(2, 92, 0, 15, -1);
        this.fma1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.signal(1);
        this.fma1.setTranslate(5.5800004f, 0.92f, -9.1f);
        this.fma1.setRotate(0.0f, 90.0f, 0.0f);
        this.fma1.setScale(0.37f, 0.37f, 0.37f);
        this.fma1b = new Monitor();
        this.fma1b.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1b.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fma1b.setArgs(1, 20050, 0, 128, 112);
        this.fma1b.setArgs(2, 92, 0, 15, -1);
        this.fma1b.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1b.setTranslate(5.5800004f, 0.92f, -9.1f);
        this.fma1b.setRotate(0.0f, 90.0f, 0.0f);
        this.fma1b.setScale(0.37f, 0.37f, 0.37f);
        this.fma2 = new Monitor();
        this.fma2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma2.setArgs(1, 20020, 0, 128, 112);
        this.fma2.setArgs(2, 92, 0, 15, -1);
        this.fma2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.signal(1);
        this.fma2.setTranslate(5.48f, 0.92f, -8.6f);
        this.fma2.setRotate(0.0f, 60.0f, 0.0f);
        this.fma2.setScale(0.37f, 0.37f, 0.37f);
        this.fma3 = new Monitor();
        this.fma3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma3.setArgs(1, 20001, 0, 128, 112);
        this.fma3.setArgs(2, 92, 0, 15, -1);
        this.fma3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.signal(1);
        this.fma3.setTranslate(5.48f, 0.92f, -9.6f);
        this.fma3.setRotate(0.0f, 120.0f, 0.0f);
        this.fma3.setScale(0.37f, 0.37f, 0.37f);
        this.keyboard = new KB();
        this.keyboard.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.keyboard.setArgs(0, 0.0f, 0.5f, 1.0f, 0.5f);
        this.keyboard.setArgs(1, 21003, 0, 0, 0);
        this.keyboard.setArgs(2, 96, 0, 0, -1);
        this.keyboard.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.keyboard.setParent(this.shion, 60);
        this.keyboard.start(1, "open");
        this.keyboard.signal(1);
        this.keyboard.setTranslate(0.06f, 0.1f, -0.1f);
        this.keyboard.setRotate(150.0f, 180.0f, 30.0f);
        this.keyboard.setScale(0.2f, 0.2f, 0.2f);
        this.CGMon = new KB();
        this.CGMon.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.CGMon.setArgs(0, 0.0f, 0.5f, 1.0f, 0.5f);
        this.CGMon.setArgs(1, 20001, 0, 0, 0);
        this.CGMon.setArgs(2, 96, 0, 0, -1);
        this.CGMon.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.CGMon.setParent(this.shion, 60);
        this.CGMon.setTranslate(0.1f, 0.0f, -0.1f);
        this.CGMon.setRotate(-10.0f, -310.0f, -130.0f);
        this.CGMon.setScale(0.4f, 0.4f, 0.4f);
        this.CGMon.signal(1);
        this.CGMon.setTranslate(0.21f, 0.05f, 0.0f);
        this.CGMon.setRotate(560.0f, -71.0f, -30.0f);
        this.CGMon.setScale(0.32f, 0.32f, 0.32f);
        this.CG = new Unit();
        this.CG.init(24596, 0.0f, 0.0f, 0.0f, 0.0f);
        this.CG.setParent(this.shion, 60);
        this.CG.setTranslate(0.08f, -0.0f, -0.01f);
        this.CG.setRotate(170.0f, -31.0f, -20.0f);
    }

    void initialize() {
    }

    static void main() {
    }

    void monitor_set() {
        Stage.setVisible(36, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(39, false);
        Stage.setVisible(40, false);
        Stage.setVisible(41, false);
        Stage.setVisible(42, false);
        Stage.setVisible(43, false);
        Stage.setVisible(44, false);
        Stage.setVisible(45, false);
        Stage.setVisible(46, false);
        Stage.setVisible(47, false);
    }

    void play() {
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        Sound.streamPlay(1191003, 48000);
        this.tool.SoundstreamDebug_init(999999);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.loadarc(this.allen.face, "FLSallen_h.fpk");
        this.tool.loadarc(this.op_a.face, "FLSoff_m.fpk");
        this.headset.setParent(this.allen, 48);
        this.headset.setTranslate(0.03f, -0.03f, -0.08f);
        this.headset.setRotate(94.0f, 92.2f, 87.0f);
        this.shion.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.allen.setMotionFlags(0x2000000, true);
        this.monitor_set();
        this.allen.start(1, "act1");
        this.camerawork.cut1();
        System.sleep(20);
        this.tool.SoundstreamPlay(103011);
        this.tool.sMSG(40, this.allen.face, 5, 30, "Here we go...");
        Runtime.setLocation(41);
        this.camerawork.cut2();
        this.target_b1.start(1, "act2");
        this.kosmos.start(1, "act2");
        this.shion.start(1, "act2");
        this.tab.setVisible(false);
        this.allen.setVisible(false);
        this.headset.setVisible(false);
        this.op_a.setVisible(false);
        this.target_b1.setVisible(false);
        this.Box.start(1, "down_box");
        this.Elevator.start(1, "down_elevator");
        System.sleep(120);
        this.camerawork.cut2_2();
        this.Elevator.start(1, "up_elevator");
        this.target_b1.start(1, "act2_2");
        System.sleep(90);
        this.camerawork.cut3();
        this.target_b1.setVisible(true);
        this.shion.start(1, "act3");
        System.sleep(90);
        this.camerawork.cut4();
        this.kosmos.start(1, "act4");
        this.shion.start(1, "act4");
        System.sleep(30);
        this.shion.look_eye_speed(3.0f);
        this.shion.look_eye_set(-1.2f, 7.5f);
        System.sleep(30);
        Runtime.setLocation(16);
        this.allen.start(1, "act5");
        this.camerawork.cut5();
        this.fma1b.start(1, "on_a");
        this.fma1.start(1, "off_s");
        this.fma1b.signal(1);
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.shion.look_eye_set(0.0f, 0.0f);
        Stage.setVisible(12, false);
        Stage.setVisible(4, false);
        this.tab.setVisible(true);
        this.allen.setVisible(true);
        this.headset.setVisible(true);
        this.op_a.setVisible(true);
        this.target_b1.setVisible(false);
        this.tool.MSG(40, "アレン", "　");
        this.tool.SoundstreamPlay(103021);
        this.tool.MSG(30, this.allen.face, 5, 20, "What's going on?!");
        this.camerawork.cut6();
        this.allen.start(1, "act6");
        this.allen.look_eye_speed(3.0f);
        this.allen.look_eye_set(10.8f, 0.0f);
        this.tool.SoundstreamPlay(103022);
        this.tool.MSG(50, this.op_a.face, 1, 40, "A brand-new network is ");
        this.allen.look_eye_set(0.0f, 0.0f);
        this.tool.MSG(60, this.op_a.face, 1, 50, "being created within\nthe KOS-MOS mainframe.");
        this.tool.SoundstreamPlay(103023);
        this.tool.MSG(65, this.op_a.face, 1, 50, "We've never had a reaction\nlike this before.");
        this.tool.SoundstreamPlay(103024);
        this.tool.MSG(55, this.op_a.face, 1, 40, "This is incredible...");
        this.tool.SoundstreamPlay(103025);
        this.tool.MSG(70, this.op_a.face, 1, 60, "I've never seen a Net grow so fast!");
        this.camerawork.cut7();
        this.allen.start(1, "act7");
        System.sleep(20);
        this.tool.SoundstreamPlay(103026);
        this.tool.MSG(40, this.op_a.face, 1, 30, "Look at it, sir.");
        this.tool.SoundstreamPlay(103027);
        this.tool.MSG(80, this.op_a.face, 1, 70, "Portions of the Encephalon map\nare evolving!");
        this.camerawork.cut8();
        this.allen.start(1, "act8");
        System.sleep(60);
        this.tool.SoundstreamPlay(103028);
        this.tool.MSG(35, this.allen.face, 5, 20, "What the...");
        Runtime.setLocation(41);
        this.camerawork.cut9();
        this.shion.start(1, "act9");
        this.kosmos.start(1, "act9");
        this.tool.FACE(this.shion.face, 18);
        this.shion.look_eye_speed(10.0f);
        this.shion.look_eye_set(3.6f, 0.0f);
        this.shion.setVisible(true);
        this.kosmos.setVisible(true);
        System.sleep(80);
        this.shion.look_eye_speed(2.0f);
        this.shion.look_eye_set(0.0f, 0.0f);
        System.sleep(90);
        this.shion.look_eye_speed(1.0f);
        this.shion.look_eye_set(0.0f, -8.1f);
        this.tool.SoundstreamPlay(103029);
        this.tool.sMSG(100, this.shion.face, 5, 90, "Allen, I'm going to engage the target.\nMake sure you capture");
        this.tool.SoundstreamPlay(103030);
        this.tool.MSG(30, this.shion.face, 5, 20, "all the data.");
        Runtime.setLocation(16);
        this.kosmos.stop();
        this.camerawork.cut10();
        this.allen.start(1, "act10");
        this.op_a.start(1, "act10");
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.allen.look_eye_set(0.0f, -6.0f);
        this.tool.SoundstreamPlay(103031);
        this.tool.MSG(30, this.allen.face, 5, "What?!");
        this.tool.SoundstreamPlay(103032);
        this.tool.MSG(80, this.allen.face, 5, "You cannot engage right now!");
        this.tool.SoundstreamPlay(103033);
        this.tool.MSG(100, this.allen.face, 5, "You're tapped into KOS-MOS'\nperception -- it's too unstable!");
        this.tool.SoundstreamPlay(103034);
        this.tool.MSG(30, "シオン", "Don't worry.");
        this.tool.SoundstreamPlay(103035);
        this.tool.MSG(60, "シオン", "We can pull it off.");
        this.tool.SoundstreamPlay(103036);
        this.tool.sMSG(55, this.allen.face, 7, "What do you mean, don't worry?!\nChief!");
        System.sleep(38);
        this.tool.Timechk_SceneEnd();
    }

    class Monitor
            extends Unit {
        float sz;
        int alpha;

        Monitor() {
        }

        void off_a() {
            this.alpha = 96;
            while (this.alpha <= 0) {
                this.alpha += 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 0, 0, 0, -1);
        }

        void off_s() {
            this.setScale(0.0f, 0.0f, 0.0f);
        }

        void on_a() {
            this.alpha = 0;
            while (this.alpha <= 96) {
                this.alpha += 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class KB
            extends Unit {
        float sz;
        int alpha;

        KB() {
        }

        void close() {
            this.setScale(0.3f, 0.3f, 0.3f);
            System.println("Keyboard Close.");
            this.sz = 0.3f;
            this.alpha = 96;
            while (this.sz >= 0.0f) {
                this.sz -= 0.02f;
                this.alpha -= 6;
                this.setScale(0.3f, this.sz, 0.3f);
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setScale(0.3f, 0.0f, 0.3f);
        }

        void open() {
            this.setScale(0.0f, 0.0f, 0.0f);
            System.println("Keyboard Open.");
            this.sz = 0.0f;
            this.alpha = 0;
            while (this.sz <= 0.3f) {
                this.sz += 0.02f;
                this.alpha += 6;
                this.setScale(0.3f, this.sz, 0.3f);
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setScale(0.3f, 0.3f, 0.3f);
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class MapUnits
            extends Unit {
        Spline posSPL = Spline.create();
        Spline posSPL2 = Spline.create();

        MapUnits() {
        }

        void down_box() {
            this.setTranslate(SCE01003B1.this.Box.px, 0.0f, SCE01003B1.this.Box.pz);
            System.sleep(30);
            int n = 0;
            float f = 0.0f;
            while (n != 120) {
                this.getTranslate();
                this.setTranslate(SCE01003B1.this.Box.px, f, SCE01003B1.this.Box.pz);
                System.sleep(1);
                f -= 0.02f;
                ++n;
            }
        }

        void down_elevator() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            System.sleep(30);
            int n = 0;
            float f = 0.0f;
            while (n != 120) {
                this.getTranslate();
                this.setTranslate(0.0f, f, 0.0f);
                System.sleep(1);
                f -= 0.02f;
                ++n;
            }
        }

        void up_elevator() {
            int n = 0;
            float f = -3.6f;
            while (n != 180) {
                this.getTranslate();
                this.setTranslate(0.0f, f, 0.0f);
                System.sleep(1);
                f += 0.02f;
                ++n;
            }
        }
    }

    class shion
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        shion() {
        }

        void act2() {
            this.setTranslate(-0.5f, 0.0f, 5.7f);
            this.setRotate(0.0f, 530.0f, 0.0f);
            this.mtn(263, 0, 119, 8, 8, 0.5f, true);
        }

        void act3() {
            this.mtn(266, 0, 60, 8, 8, 0.5f, true);
        }

        void act4() {
            this.setTranslate(-0.5f, 0.0f, 2.2f);
            this.setRotate(0.0f, 530.0f, 0.0f);
            this.mtn(268, 0, 60, 8, 8, 1.0f, true);
        }

        void act9() {
            this.setTranslate(-0.1f, 0.0f, 1.2f);
            this.setRotate(0.0f, 530.0f, 0.0f);
            this.mtn(273, 0, 280, 0, 8, 0.9f, true);
        }

        void init() {
            this.init(0x100001E, 0.0f, 0.0f, 0.0f, 180.0f);
            this.face = this.getChild(0x1000000);
            float f = 0.5f;
        }
    }

    class allen
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        allen() {
        }

        void act1() {
            this.setTranslate(4.74f, -0.2f, -8.62f);
            this.setRotate(0.0f, 125.0f, 0.0f);
            this.mtn(262, 0, 90, 0, 8, 1.0f, true);
        }

        void act10() {
            this.setTranslate(4.9f, 0.11f, -8.61f);
            this.setRotate(0.0f, 92.0f, 0.0f);
            this.mtn(274, 0, 320, 8, 0, 0.81f, true);
        }

        void act5() {
            this.setTranslate(4.44f, 0.0f, -8.22f);
            this.setRotate(0.0f, 125.0f, 0.0f);
            this.mtn(269, 0, 70, 0, 8, 1.0f, true);
        }

        void act6() {
            this.setTranslate(4.54f, 0.1f, -8.62f);
            this.setRotate(0.0f, 35.0f, 0.0f);
            this.mtn(270, 0, 300, 0, 8, 1.0f, true);
        }

        void act7() {
            this.setTranslate(4.54f, 0.1f, -8.62f);
            this.setRotate(0.0f, 105.0f, 0.0f);
            this.mtn(271, 0, 180, 0, 8, 1.0f, true);
        }

        void act8() {
            this.setTranslate(4.84f, 0.1f, -8.72f);
            this.setRotate(0.0f, 105.0f, 0.0f);
            this.mtn(272, 0, 95, 8, 8, 1.0f, true);
        }

        void init() {
            this.init(16777532, 0.0f, 0.0f, 0.0f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(4.74f, -0.2f, -8.72f);
            this.setRotate(0.0f, 125.0f, 0.0f);
        }
    }

    class op_a
            extends Chr {
        Chr face;

        op_a() {
        }

        void act10() {
            this.mtn(258, 0, 119, 8, 0, 1.0f, true);
            this.mtn(258, 0, 119, 8, 0, 1.0f, true);
            this.mtn(258, 0, 119, 8, 0, 1.0f, true);
            this.mtn(258, 0, 33, 8, 0, 1.0f, true);
        }

        void init() {
            this.init(0x1000202, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(4.78f, 0.02f, -9.1f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(258, 0, 119, 8, 8, 1.0f, true);
        }
    }

    class kosmos
            extends Chr {
        kosmos() {
        }

        void act2() {
            this.setTranslate(0.6f, 0.0f, 4.8f);
            this.setRotate(0.0f, 190.0f, 0.0f);
            this.mtn(264, 0, 119, 8, 8, 1.0f, true);
        }

        void act4() {
            this.setTranslate(0.6f, 0.0f, 1.1f);
            this.setRotate(0.0f, 190.0f, 0.0f);
            this.mtn(267, 0, 60, 0, 8, 1.0f, true);
        }

        void act9() {
            this.mtn(264, 0, 119, 8, 8, 1.0f, false);
            Runtime.setDefocusQuick(3, 4, 600000, 64);
            while (true) {
                Runtime.setDefocusQuick(0, 1, 622880, 1);
                Runtime.setDefocusQuick(1, 1, 614688, 1);
                Runtime.setDefocusQuick(2, 1, 606496, 1);
                System.sleep(1);
                Runtime.setDefocusQuick(0, 1, 622880, 2);
                Runtime.setDefocusQuick(1, 1, 614688, 2);
                Runtime.setDefocusQuick(2, 1, 606496, 2);
                System.sleep(1);
                Runtime.setDefocusQuick(0, 1, 622880, 1);
                Runtime.setDefocusQuick(1, 1, 614688, 1);
                Runtime.setDefocusQuick(2, 1, 606496, 1);
                System.sleep(1);
                Runtime.setDefocusQuick(0, 1, 622880, 0);
                Runtime.setDefocusQuick(1, 1, 614688, 0);
                Runtime.setDefocusQuick(2, 1, 606496, 0);
                System.sleep(3);
            }
        }

        void init() {
            this.init(14, 0.0f, 0.0f, 0.0f, 180.0f);
            float f = 0.5f;
        }
    }

    class target_b1
            extends Chr {
        target_b1() {
        }

        void act2() {
            this.setTranslate(-0.0f, 0.0f, -4.0f);
            this.setRotate(0.0f, 370.0f, 0.0f);
            this.mtn(265, 0, 80, 8, 8, 1.0f, true);
        }

        void act2_2() {
            int n = 0;
            float f = -0.5999999f;
            this.setVisible(true);
            while (n != 180) {
                this.getTranslate();
                this.setTranslate(0.0f, f - 3.0f, -4.0f);
                System.sleep(1);
                f += 0.02f;
                ++n;
            }
        }

        void init() {
            this.init(20231, 0.0f, 0.0f, 0.0f, 180.0f);
        }
    }

    class tab
            extends Unit {
        tab() {
        }

        void init() {
            this.init(20555, 0.0f, 0.0f, 0.0f, 180.0f);
            this.setTranslate(0.46f, 0.19f, -8.04f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE01003B1.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE01003B1.this.light.setDirection2(1, 0.921f, 0.311f, -0.233f);
            SCE01003B1.this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            SCE01003B1.this.light.setDirection2(2, 0.194f, 0.278f, 0.941f);
            SCE01003B1.this.light.setColor(3, 0.24f, 0.31f, 0.31f);
            SCE01003B1.this.light.setDirection2(3, 0.886f, -0.455f, 0.093f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 48880, 1);
            Runtime.setDefocusQuick(1, 1, 40688, 1);
            Runtime.setDefocusQuick(2, 1, 32496, 1);
            Runtime.setDefocusQuick(3, 0, 0, 0);
            float[] fArray = new float[]{1.0f, 6.26f, 1.54f, -8.74f, 60.0f, 6.25f, 1.54f, -8.7f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -15.02f;
            fArray2[2] = 438.84f;
            fArray2[4] = 60.0f;
            fArray2[5] = -15.02f;
            fArray2[6] = 438.84f;
            float[] fArray3 = fArray2;
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray3, 0);
            SCE01003B1.this.BaseCam.setFov(25.0f);
            SCE01003B1.this.BaseCam.change();
        }

        public void cut10() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE01003B1.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
            SCE01003B1.this.light.setColor(2, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(2, 0.795f, -0.6f, -0.089f);
            SCE01003B1.this.light.setColor(3, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
            Runtime.setDefocusQuick(0, 1, 84880, 1);
            Runtime.setDefocusQuick(1, 1, 76688, 1);
            Runtime.setDefocusQuick(2, 1, 68496, 1);
            Runtime.setDefocusQuick(3, 0, 0, 0);
            SCE01003B1.this.BaseCam.setTranslate(4.67f, 2.03f, -7.45f);
            SCE01003B1.this.BaseCam.setRotate(-22.72f, 347.47f, 0.01f);
            SCE01003B1.this.BaseCam.setFov(27.4f);
        }

        public void cut2() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01003B1.this.light.setColor(1, 0.72f, 0.72f, 0.72f);
            SCE01003B1.this.light.setDirection2(1, 0.671f, 0.372f, -0.641f);
            SCE01003B1.this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            SCE01003B1.this.light.setDirection2(2, -0.278f, 0.322f, 0.905f);
            SCE01003B1.this.light.setColor(3, 0.51f, 0.51f, 0.51f);
            SCE01003B1.this.light.setDirection2(3, -0.0f, -0.613f, -0.79f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -0.05f, 1.32f, 7.48f, 180.0f, -0.05f, 1.32f, 7.15f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 0.38f;
            fArray2[2] = 360.99f;
            fArray2[4] = 180.0f;
            fArray2[5] = 0.38f;
            fArray2[6] = 360.99f;
            float[] fArray3 = fArray2;
            SCE01003B1.this.BaseCam.setFov(32.0f);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut2_2() {
            SCE01003B1.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -2.15f, 1.29f, -5.33f, 100.0f, -2.08f, 1.29f, -6.38f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -5.16f;
            fArray2[2] = 204.6f;
            fArray2[4] = 100.0f;
            fArray2[5] = -5.16f;
            fArray2[6] = 204.6f;
            float[] fArray3 = fArray2;
            SCE01003B1.this.BaseCam.setFov(42.2f);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut3() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01003B1.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE01003B1.this.light.setDirection2(1, 0.944f, 0.0f, -0.329f);
            SCE01003B1.this.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE01003B1.this.light.setDirection2(2, -0.278f, 0.322f, 0.905f);
            SCE01003B1.this.light.setColor(3, 0.51f, 0.51f, 0.51f);
            SCE01003B1.this.light.setDirection2(3, 0.0f, -0.613f, -0.79f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -0.04f, 0.97f, -0.34f, 100.0f, -0.11f, 0.97f, 8.17f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 0.64f;
            fArray2[2] = 358.75f;
            fArray2[4] = 100.0f;
            fArray2[5] = 0.64f;
            fArray2[6] = 358.75f;
            float[] fArray3 = fArray2;
            SCE01003B1.this.BaseCam.transSPL(fArray, 0, 1, 90);
            SCE01003B1.this.BaseCam.rotateSPL(fArray3, 0, 1, 90);
            SCE01003B1.this.BaseCam.setFov(21.8f);
        }

        public void cut4() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01003B1.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE01003B1.this.light.setDirection2(1, -0.156f, 0.197f, -0.968f);
            SCE01003B1.this.light.setColor(2, 0.17f, 0.17f, 0.17f);
            SCE01003B1.this.light.setDirection2(2, 0.852f, 0.311f, 0.421f);
            SCE01003B1.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE01003B1.this.light.setDirection2(3, 0.267f, -0.503f, -0.822f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 48880, 1);
            Runtime.setDefocusQuick(1, 1, 40688, 1);
            Runtime.setDefocusQuick(2, 1, 32496, 1);
            float[] fArray = new float[]{1.0f, 0.94f, 1.58f, -0.46f, 60.0f, 0.94f, 1.58f, -0.46f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -9.66f;
            fArray2[2] = -196.54f;
            fArray2[4] = 60.0f;
            fArray2[5] = -9.66f;
            fArray2[6] = -196.54f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[]{1.0f, 28.2f, 60.0f, 26.6f};
            SCE01003B1.this.BaseCam.fovSPL(fArray4, 0);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut5() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01003B1.this.light.setColor(1, 0.67f, 0.67f, 0.67f);
            SCE01003B1.this.light.setDirection2(1, -0.313f, 0.238f, 0.919f);
            SCE01003B1.this.light.setColor(2, 0.54f, 0.54f, 0.54f);
            SCE01003B1.this.light.setDirection2(2, -0.954f, 0.138f, -0.268f);
            SCE01003B1.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE01003B1.this.light.setDirection2(3, -0.851f, -0.135f, 0.507f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 2, 52264, 2);
            Runtime.setDefocusQuick(1, 2, 60456, 2);
            Runtime.setDefocusQuick(2, 2, 68648, 2);
            float[] fArray = new float[]{1.0f, -0.19f, 0.71f, -5.78f, 70.0f, -0.15f, 0.71f, -5.74f};
            float[] fArray2 = new float[]{1.0f, -1.71f, 314.14f, 1.28f, 70.0f, -1.71f, 314.14f, 1.28f};
            SCE01003B1.this.BaseCam.setFov(40.0f);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut6() {
            SCE01003B1.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE01003B1.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
            SCE01003B1.this.light.setColor(2, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(2, 0.795f, -0.6f, -0.089f);
            SCE01003B1.this.light.setColor(3, 0.6f, 0.6f, 0.6f);
            SCE01003B1.this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
            SCE01003B1.this.tool.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 1, 84880, 1);
            Runtime.setDefocusQuick(1, 1, 76688, 1);
            Runtime.setDefocusQuick(2, 1, 68496, 1);
            float[] fArray = new float[]{1.0f, 5.22f, 1.25f, -7.1f, 300.0f, 5.43f, 1.25f, -7.17f};
            float[] fArray2 = new float[]{1.0f, 4.35f, 378.81f, 1.28f, 300.0f, 4.35f, 371.88f, 1.28f};
            SCE01003B1.this.BaseCam.setFov(25.8f);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut7() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.BaseCam.setTranslate(4.13f, 2.03f, -8.97f);
            SCE01003B1.this.BaseCam.setRotate(-34.98f, 270.04f, 1.28f);
            float[] fArray = new float[]{1.0f, 25.92f, 140.0f, 24.01f};
            SCE01003B1.this.BaseCam.fovSPL(fArray, 0);
        }

        public void cut8() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.BaseCam.setTranslate(5.15f, 1.39f, -7.69f);
            SCE01003B1.this.BaseCam.setRotate(-0.66f, 353.42f, 1.28f);
            SCE01003B1.this.BaseCam.setFov(30.8f);
        }

        public void cut9() {
            SCE01003B1.this.tool.Timechk_CutChange();
            SCE01003B1.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01003B1.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE01003B1.this.light.setDirection2(1, -0.092f, 0.243f, -0.966f);
            SCE01003B1.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE01003B1.this.light.setDirection2(2, 0.702f, 0.525f, 0.481f);
            SCE01003B1.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE01003B1.this.light.setDirection2(3, 0.533f, -0.418f, -0.736f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, 0.67f, 1.44f, -0.04f, 300.0f, 1.07f, 1.44f, 0.26f};
            float[] fArray2 = new float[]{1.0f, -0.05f, 143.32f, 0.01f, 300.0f, -0.12f, 136.12f, 0.01f};
            SCE01003B1.this.BaseCam.setFov(25.2f);
            SCE01003B1.this.BaseCam.transSPL(fArray, 0);
            SCE01003B1.this.BaseCam.rotateSPL(fArray2, 0);
        }
    }
}

