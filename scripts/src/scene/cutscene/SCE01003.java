import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
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
import xeno.util.Spline;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01003
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01003,
        MC_VOK01_PRJ,
        JNT_Human,
        FLSallen_h,
        FLStogashi,
        FLSvirgil,
        FLSvec_m {
    public allen allen;
    public op_a op_a;
    public op_b op_b;
    public op_d op_d;
    public target_a1 target_a1;
    public target_a2 target_a2;
    public target_b1 target_b1;
    public target_b2 target_b2;
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
    Light light = new Light(0);
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    Thread camera_thread;
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    int BGinit = 0;
    Unit fma1;
    Unit fma2;
    Unit fma3;
    Unit fmb1;
    Unit fmb2;
    Unit fmb3;
    Unit fmd1;
    Unit fmd2;
    Unit fmd3;
    float[] gnoFilter = new float[4];
    float[] gnoFilter2 = new float[4];
    Effect eft1;
    Effect eft2;
    Unit headset;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01003() {
    }

    void CutWin(String string) {
        Window window = Window.create();
        window.setSize(2, 16);
        window.setLocation(10, 10);
        window.print(string);
        window.wait(30);
        window.close();
    }

    void SubWin(int n, String string) {
        this.msg.print(string);
        this.msg.wait(n);
        this.msg.clear();
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
        System.println("XEVEFLAG:EV01003_F");
        Runtime.setFlags(3, 1, 1);
        System.println("XEVEJNAME:CFJ1_30 XEVEJPOINT:POINT_30");
        Runtime.jumpCF(251, 4);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(251, 4);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(38);
        Runtime.setLocation(16);
        this.cam0 = Camera.create(0);
        this.headset = new Unit();
        this.headset.init(24580);
        this.fma1 = new Unit();
        this.fma1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fma1.setArgs(1, 20004, 0, 128, 112);
        this.fma1.setArgs(2, 92, 0, 15, -1);
        this.fma1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.signal(1);
        this.fma1.setTranslate(5.5800004f, 0.92f, -9.1f);
        this.fma1.setRotate(0.0f, 90.0f, 0.0f);
        this.fma1.setScale(0.37f, 0.37f, 0.37f);
        this.fma2 = new Unit();
        this.fma2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma2.setArgs(1, 20020, 0, 128, 112);
        this.fma2.setArgs(2, 92, 0, 15, -1);
        this.fma2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.signal(1);
        this.fma2.setTranslate(5.48f, 0.92f, -8.6f);
        this.fma2.setRotate(0.0f, 60.0f, 0.0f);
        this.fma2.setScale(0.37f, 0.37f, 0.37f);
        this.fma3 = new Unit();
        this.fma3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma3.setArgs(1, 20021, 0, 128, 112);
        this.fma3.setArgs(2, 92, 0, 15, -1);
        this.fma3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.signal(1);
        this.fma3.setTranslate(5.48f, 0.92f, -9.6f);
        this.fma3.setRotate(0.0f, 120.0f, 0.0f);
        this.fma3.setScale(0.37f, 0.37f, 0.37f);
        this.fmb1 = new Unit();
        this.fmb1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fmb1.setArgs(1, 20004, 0, 128, 112);
        this.fmb1.setArgs(2, 92, 0, 15, -1);
        this.fmb1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb1.signal(1);
        this.fmb1.setTranslate(5.5800004f, 0.92f, -4.1f);
        this.fmb1.setRotate(0.0f, 90.0f, 0.0f);
        this.fmb1.setScale(0.37f, 0.37f, 0.37f);
        this.fmb2 = new Unit();
        this.fmb2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmb2.setArgs(1, 20020, 0, 128, 112);
        this.fmb2.setArgs(2, 92, 0, 15, -1);
        this.fmb2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb2.signal(1);
        this.fmb2.setTranslate(5.48f, 0.92f, -3.6f);
        this.fmb2.setRotate(0.0f, 60.0f, 0.0f);
        this.fmb2.setScale(0.37f, 0.37f, 0.37f);
        this.fmb3 = new Unit();
        this.fmb3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmb3.setArgs(1, 20021, 0, 128, 112);
        this.fmb3.setArgs(2, 92, 0, 15, -1);
        this.fmb3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmb3.signal(1);
        this.fmb3.setTranslate(5.48f, 0.92f, -4.6f);
        this.fmb3.setRotate(0.0f, 120.0f, 0.0f);
        this.fmb3.setScale(0.37f, 0.37f, 0.37f);
        this.fmd1 = new Unit();
        this.fmd1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fmd1.setArgs(1, 20004, 0, 128, 112);
        this.fmd1.setArgs(2, 92, 0, 15, -1);
        this.fmd1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd1.signal(1);
        this.fmd1.setTranslate(-4.48f, 0.92f, -9.1f);
        this.fmd1.setRotate(0.0f, 90.0f, 0.0f);
        this.fmd1.setScale(0.37f, 0.37f, 0.37f);
        this.fmd2 = new Unit();
        this.fmd2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmd2.setArgs(1, 20020, 0, 128, 112);
        this.fmd2.setArgs(2, 92, 0, 15, -1);
        this.fmd2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd2.signal(1);
        this.fmd2.setTranslate(-4.38f, 0.92f, -8.6f);
        this.fmd2.setRotate(0.0f, 120.0f, 0.0f);
        this.fmd2.setScale(0.37f, 0.37f, 0.37f);
        this.fmd3 = new Unit();
        this.fmd3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmd3.setArgs(1, 20021, 0, 128, 112);
        this.fmd3.setArgs(2, 92, 0, 15, -1);
        this.fmd3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd3.signal(1);
        this.fmd3.setTranslate(-4.38f, 0.92f, -9.6f);
        this.fmd3.setRotate(0.0f, 60.0f, 0.0f);
        this.fmd3.setScale(0.37f, 0.37f, 0.37f);
        this.eft1 = new Effect(1521, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft2 = new Effect(1521, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1.disp(false);
        this.eft2.disp(false);
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
        this.tool.loadarc(this.allen.face, "FLSallen_h.fpk");
        this.tool.loadarc(this.op_a.face, "FLSvec_m.fpk");
        this.tool.loadarc(this.op_b.face, "FLStogashi.fpk");
        this.tool.loadarc(this.op_d.face, "FLSvirgil.fpk");
        this.tool.loadarc((Chr) this.target_b1, "enemy.fpk");
        this.tool.loadarc((Chr) this.target_b2, "enemy.fpk");
        this.tool.loadarc((Chr) this.target_a1, "enemy.fpk");
        this.tool.loadarc((Chr) this.target_a2, "enemy.fpk");
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        Sound.streamPlay(1191002, 48000);
        this.tool.SoundstreamDebug_init(999999);
        this.tool.Timechk_SceneStart();
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.headset.setParent(this.allen, 48);
        this.headset.setTranslate(0.03f, -0.03f, -0.08f);
        this.headset.setRotate(94.0f, 92.2f, 87.0f);
        this.monitor_set();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.914f, 0.189f, 0.359f);
        this.light.setColor(2, 0.62f, 0.62f, 0.62f);
        this.light.setDirection2(2, -0.815f, 0.133f, 0.564f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, 0.364f, -0.518f, 0.774f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.start(1, "act1");
        this.camerawork.cut1();
        System.sleep(60);
        this.tool.SoundstreamPlay(103001);
        this.tool.MSG(30, this.allen.face, 1, 20, "Status check.");
        this.op_a.start(1, "act2");
        this.camerawork.cut2();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.6f, 0.7f, 0.8f);
        this.light.setDirection2(1, 0.778f, 0.279f, 0.563f);
        this.light.setColor(2, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(2, -0.822f, 0.436f, 0.366f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, 0.156f, -0.81f, 0.566f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.op_a.look_eye_set(9.0f, 6.3f);
        this.tool.SoundstreamPlay(103002);
        this.tool.MSG(60, this.op_a.face, 1, 50, "I don't see anything unusual.");
        this.tool.SoundstreamPlay(103003);
        this.tool.MSG(50, this.op_a.face, 1, 40, "All systems are stable.");
        this.allen.start(1, "act3");
        this.op_a.start(1, "act3");
        this.camera_thread = Thread.create(this.camerawork, "cut3");
        this.camera_thread.start();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.6f, 0.7f, 0.8f);
        this.light.setDirection2(1, 0.778f, 0.279f, 0.563f);
        this.light.setColor(2, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(2, -0.822f, 0.436f, 0.366f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, 0.156f, -0.81f, 0.566f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tool.SoundstreamPlay(103004);
        this.tool.MSG(40, this.op_a.face, 1, 30, "It's looking good.");
        this.tool.SoundstreamPlay(103005);
        this.tool.MSG(50, this.op_a.face, 1, 40, "This ought to make up for last month.");
        this.tool.SoundstreamPlay(103006);
        this.tool.MSG(50, this.allen.face, 1, 40, "...I sure hope so.");
        this.op_d.start(1, "act4");
        this.camerawork.cut4();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.6f, 0.7f, 0.8f);
        this.light.setDirection2(1, -0.873f, 0.0f, 0.487f);
        this.light.setColor(2, 0.63f, 0.63f, 0.63f);
        this.light.setDirection2(2, 0.893f, 0.198f, 0.405f);
        this.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(3, 0.033f, -0.571f, 0.82f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tool.SoundstreamPlay(103007);
        this.tool.MSG(60, this.op_d.face, 1, 50, "KOS-MOS has reached the checkpoint.");
        this.tool.SoundstreamPlay(103008);
        this.tool.MSG(60, this.op_d.face, 1, 50, "Entering process 431.");
        this.allen.start(1, "act5");
        this.op_a.start(1, "act5");
        this.op_b.start(1, "act5");
        this.camerawork.cut5();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.67f, 0.67f, 0.67f);
        this.light.setDirection2(1, 0.998f, 0.0f, 0.061f);
        this.light.setColor(2, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(2, -0.822f, 0.364f, 0.438f);
        this.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(3, 0.148f, -0.412f, 0.899f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tool.SoundstreamPlay(103009);
        this.tool.MSG(55, this.op_b.face, 1, 50, "Replacing targets with Type-G Drones.");
        this.tool.SoundstreamPlay(103010);
        this.tool.MSG(40, this.op_b.face, 1, 30, "Commencing display.");
        Runtime.setLocation(38);
        this.camerawork.cut6();
        this.gnoFilter[0] = 1.0f;
        this.gnoFilter[1] = 0.0f;
        this.gnoFilter[2] = 0.0f;
        this.gnoFilter[3] = 1.0f;
        this.gnoFilter2[0] = 0.0f;
        this.gnoFilter2[1] = 0.0f;
        this.gnoFilter2[2] = 0.0f;
        this.gnoFilter2[3] = 0.0f;
        this.target_a1.start(1, "act6");
        this.target_a2.start(1, "act6");
        this.target_b1.start(1, "act6");
        this.target_b2.start(1, "act6");
        this.allen.setVisible(false);
        this.op_a.setVisible(false);
        this.op_b.setVisible(false);
        this.op_d.setVisible(false);
        this.tab.setVisible(false);
        float f = 1.0f;
        System.sleep(30);
        this.eft1.setTranslate(this.target_a1.px, this.target_a1.py, this.target_a1.pz);
        this.eft2.setTranslate(this.target_a2.px, this.target_a2.py, this.target_a2.pz);
        this.eft1.disp(true);
        this.eft2.disp(true);
        while (f >= 0.0f) {
            this.gnoFilter[0] = f -= 0.01f;
            this.gnoFilter2[0] = 1.0f - f;
            System.sleep(1);
        }
        f = 1.0f;
        while (f >= 0.0f) {
            this.eft1.setScale(f -= 0.05f, f, f);
            this.eft2.setScale(f, f, f);
            System.sleep(1);
        }
        this.eft1.disp(false);
        this.eft2.disp(false);
        System.sleep(50);
        this.tool.Timechk_SceneEnd();
    }

    class allen
            extends Chr {
        Spline SPL = Spline.create();
        Chr face;

        allen() {
        }

        void act1() {
            this.setTranslate(2.6f, 0.0f, -3.7f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(257, 0, 90, 0, 8, 1.0f, true);
        }

        void act3() {
            this.setTranslate(4.1f, 0.0f, -8.4f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(260, 0, 170, 8, 8, 1.0f, true);
        }

        void act5() {
            this.setTranslate(4.27f, 0.0f, -9.04f);
            this.setRotate(10.0f, 90.0f, -10.0f);
            this.mtn(260, 70, 170, 8, 0, 1.0f, true);
        }

        void init() {
            this.init(16777532, 0.0f, 0.0f, 0.0f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(2.6f, 0.0f, -3.7f);
            this.setRotate(0.0f, 180.0f, 0.0f);
        }
    }

    class op_a
            extends Chr {
        Chr face;

        op_a() {
        }

        void act2() {
            this.mtn(259, 0, 110, 0, 8, 1.0f, true);
        }

        void act3() {
            this.mtn(261, 0, 170, 8, 8, 1.0f, true);
        }

        void act5() {
            this.mtn(258, 0, 119, 8, 8, 1.0f, true);
        }

        void init() {
            this.init(0x1000202, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(4.78f, 0.0f, -9.1f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(258, 0, 119, 8, 8, 1.0f, true);
        }
    }

    class op_b
            extends Chr {
        Chr face;

        op_b() {
        }

        void act5() {
            this.mtn(302, 0, 119, 0, 8, 1.0f, true);
        }

        void init() {
            this.init(16777517, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(4.78f, 0.0f, 0.9f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(258, 0, 119, 8, 8, 1.0f, true);
        }
    }

    class op_d
            extends Chr {
        Chr face;

        op_d() {
        }

        void act4() {
            this.setRotCNSParam(0, 40.0f, -70.0f, 70.0f, -30.0f, 30.0f);
            this.rotCNS(0, SCE01003.this.fmd3.px, SCE01003.this.fmd3.py, SCE01003.this.fmd3.pz);
        }

        void init() {
            this.init(16777731, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(-3.68f, 0.0f, -9.1f);
            this.setRotate(0.0f, -90.0f, 0.0f);
            this.mtn(258, 0, 119, 8, 8, 1.0f, true);
        }
    }

    class target_a1
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        target_a1() {
        }

        void act6() {
            this.mtn(1, 8, 0.3f, false);
            this.setVisible(true);
            while (true) {
                this.setFilter(2);
                this.setFilterParam(SCE01003.this.gnoFilter);
                System.sleep(1);
            }
        }

        void init() {
            this.init(17665, 0.0f, 0.0f, 0.0f, 180.0f);
            this.setTranslate(1.39f, 0.0f, -13.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }
    }

    class target_a2
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        target_a2() {
        }

        void act6() {
            this.mtn(2, 8, 0.3f, false);
            this.setVisible(true);
            while (true) {
                this.setFilter(2);
                this.setFilterParam(SCE01003.this.gnoFilter);
                System.sleep(1);
            }
        }

        void init() {
            this.init(17665, 0.0f, 0.0f, 0.0f, 180.0f);
            this.setTranslate(-3.12f, 0.0f, 8.0f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(false);
        }
    }

    class target_b1
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        target_b1() {
        }

        void act6() {
            this.mtn(2, 8, 0.5f, false);
            this.setVisible(true);
            while (true) {
                this.setFilter(2);
                this.setFilterParam(SCE01003.this.gnoFilter2);
                System.sleep(1);
            }
        }

        void init() {
            this.init(16409, 0.0f, 0.0f, 0.0f, 180.0f);
            this.setTranslate(1.99f, 0.0f, -14.0f);
            this.setVisible(false);
        }
    }

    class target_b2
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        target_b2() {
        }

        void act6() {
            this.mtn(2, 8, 0.7f, false);
            this.setVisible(true);
            while (true) {
                this.setFilter(2);
                this.setFilterParam(SCE01003.this.gnoFilter2);
                System.sleep(1);
            }
        }

        void init() {
            this.init(16409, 0.0f, 0.0f, 0.0f, 180.0f);
            this.setTranslate(-3.12f, 0.0f, 8.0f);
            this.setVisible(false);
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
            SCE01003.this.tool.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 1, 71880, 1);
            Runtime.setDefocusQuick(1, 1, 63688, 1);
            Runtime.setDefocusQuick(2, 1, 55496, 1);
            SCE01003.this.BaseCam.setTranslate(2.65f, 1.57f, -2.21f);
            SCE01003.this.BaseCam.setRotate(-5.86f, -7.94f, 0.64f);
            SCE01003.this.BaseCam.setFov(23.36f);
            SCE01003.this.BaseCam.change();
        }

        public void cut2() {
            SCE01003.this.tool.Timechk_CutChange();
            SCE01003.this.BaseCam.setTranslate(4.03f, 1.52f, -7.59f);
            SCE01003.this.BaseCam.setRotate(-14.92f, 323.48f, 0.64f);
            SCE01003.this.BaseCam.setFov(24.4f);
        }

        public void cut3() {
            SCE01003.this.tool.Timechk_CutChange();
            SCE01003.this.BaseCam.setTranslate(4.04f, 1.45f, -7.37f);
            SCE01003.this.BaseCam.setRotate(-7.0f, 331.43f, 0.0f);
            SCE01003.this.BaseCam.setFov(24.2f);
        }

        public void cut4() {
            SCE01003.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -3.26f, 0.81f, -7.9f, 150.0f, -3.17f, 0.81f, -7.97f};
            float[] fArray2 = new float[]{1.0f, 13.16f, 401.68f, 0.64f, 150.0f, 13.16f, 401.68f, 0.64f};
            SCE01003.this.BaseCam.transSPL(fArray, 0);
            SCE01003.this.BaseCam.rotateSPL(fArray2, 0);
            SCE01003.this.BaseCam.setFov(28.8f);
        }

        public void cut5() {
            SCE01003.this.tool.Timechk_CutChange();
            SCE01003.this.BaseCam.setTranslate(5.11f, 1.14f, 2.4f);
            SCE01003.this.BaseCam.setRotate(-1.24f, 364.18f, 0.64f);
            SCE01003.this.BaseCam.setFov(22.08f);
        }

        public void cut6() {
            SCE01003.this.tool.Timechk_CutChange();
            SCE01003.this.BaseCam.setTranslate(5.87f, 1.71f, -17.84f);
            SCE01003.this.BaseCam.setRotate(-2.99f, 146.09f, -0.32f);
            SCE01003.this.BaseCam.setFov(30.12f);
        }
    }
}

