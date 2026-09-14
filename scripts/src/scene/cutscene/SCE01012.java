import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK04_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01012
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_VOK04_PRJ,
        Pack01012,
        FLSshion_h,
        FLSallen,
        JNT_Human {
    public mobj049 mobj049;
    public shion shion;
    public allen allen;
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    static final int Chand_L = 60;
    Light light = new Light(0);
    Monitor CGMon;
    Unit CG;
    Mob_m vec_m = new Mob_m(514);
    Mob_w vec_w = new Mob_w(517);
    Mob_m pilot = new Mob_m(525);
    Mob_w breal_w = new Mob_w(543);
    Door door1r;
    Door door1l;
    Door door2r;
    Door door2l;
    Door door3r;
    Door door3l;
    Door door4r;
    Door door4l;
    Door door5r;
    Door door5l;
    Thread s_walk = Thread.create(this, "s_walk_main2");
    Thread a_walk = Thread.create(this, "a_walk_main2");
    float s_walk_dz = 0.01f;
    float a_walk_dz = 0.01f;
    boolean CameraStart_exec = false;
    Thread camera_thread = Thread.create();
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01012() {
    }

    void CameraStart(String string) {
        if (this.CameraStart_exec) {
            this.camera_thread.stop();
        }
        this.CameraStart_exec = true;
        this.camera_thread.setTarget(this.camerawork, string);
        this.camera_thread.start();
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

    void a_walk_main() {
        while (true) {
            this.allen.setTranslate(this.allen.px, this.allen.py, this.allen.pz - this.a_walk_dz);
            System.sleep(1);
        }
    }

    void a_walk_main2() {
        int n = 0;
        float f = 0.028f;
        while (n != 710) {
            this.allen.setTranslate(this.allen.px, this.allen.py, this.allen.pz - f);
            ++n;
            System.sleep(1);
        }
        while (f > 0.0f) {
            this.allen.setTranslate(this.allen.px, this.allen.py, this.allen.pz - f);
            f -= 5.6E-4f;
            System.sleep(1);
        }
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01012_F");
        Runtime.setFlags(19, 1, 1);
        System.println("XEVEJNAME:CFJ1_90 XEVEJPOINT:POINT_90");
        Runtime.jumpCF(40, 6);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(19);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.CGMon = new Monitor();
        this.CGMon.setArgs(0, 0.0f, 0.0f, 1.0f, 0.5f);
        this.CGMon.setArgs(1, 20001, 0, 0, 0);
        this.CGMon.setArgs(2, 96, 0, 0, -1);
        this.CGMon.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.CGMon.setParent(this.shion, 60);
        this.CGMon.signal(1);
        this.CGMon.setTranslate(0.25f, 0.08f, 0.02f);
        this.CGMon.setRotate(500.0f, -21.0f, -90.0f);
        this.CGMon.setScale(0.32f, 0.32f, 0.32f);
        this.CGMon.sz = 0.32f;
        this.CG = new Unit();
        this.CG.init(24577, 0.0f, 0.0f, 0.0f, 0.0f);
        this.CG.setParent(this.shion, 60);
        this.CG.setTranslate(0.08f, -0.02f, -0.01f);
        this.CG.setRotate(151.0f, -31.0f, -20.0f);
        this.CG.setScale(0.7f, 0.7f, 0.7f);
        this.CG.setVisible(false);
        this.door1r = new Door();
        this.door1r.mapUnit(107);
        this.door1r.start(4, null);
        this.door1l = new Door();
        this.door1l.mapUnit(108);
        this.door1l.start(4, null);
        this.door2r = new Door();
        this.door2r.mapUnit(109);
        this.door2r.start(4, null);
        this.door2l = new Door();
        this.door2l.mapUnit(110);
        this.door2l.start(4, null);
        this.door3r = new Door();
        this.door3r.mapUnit(111);
        this.door3r.start(4, null);
        this.door3l = new Door();
        this.door3l.mapUnit(112);
        this.door3l.start(4, null);
        this.door4r = new Door();
        this.door4r.mapUnit(113);
        this.door4r.start(4, null);
        this.door4l = new Door();
        this.door4l.mapUnit(114);
        this.door4l.start(4, null);
        this.door5r = new Door();
        this.door5r.mapUnit(115);
        this.door5r.start(4, null);
        this.door5l = new Door();
        this.door5l.mapUnit(116);
        this.door5l.start(4, null);
        this.door1l.start(1, "open_l");
        this.door1r.start(1, "open_r");
        this.door2l.start(1, "open_l");
        this.door2r.start(1, "open_r");
        this.door3l.start(1, "open_l");
        this.door3r.start(1, "open_r");
        this.door4l.start(1, "open_l");
        this.door4r.start(1, "open_r");
        this.door5l.start(1, "open_l");
        this.door5r.start(1, "open_r");
        this.door1r.setTranslate(this.door1r.px, this.door1r.py, this.door1r.pz - 0.01f);
        this.door2r.setTranslate(this.door2r.px, this.door2r.py, this.door2r.pz - 0.01f);
        this.door3r.setTranslate(this.door3r.px, this.door3r.py, this.door3r.pz - 0.01f);
        this.door4r.setTranslate(this.door4r.px, this.door4r.py, this.door4r.pz - 0.01f);
        this.door5r.setTranslate(this.door5r.px, this.door5r.py, this.door5r.pz - 0.01f);
        this.door1r.setRotate(0.0f, 180.0f, 0.0f);
        this.door2r.setRotate(0.0f, 180.0f, 0.0f);
        this.door3r.setRotate(0.0f, 180.0f, 0.0f);
        this.door4r.setRotate(0.0f, 180.0f, 0.0f);
        this.door5r.setRotate(0.0f, 180.0f, 0.0f);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.CGMon.setScale(0.0f, 0.0f, 0.0f);
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.loadarc(this.allen.face, "FLSallen.fpk");
        this.tool.SoundstreamDebug_init(999999);
        this.tool.Timechk_SceneStart();
        Sound.streamPlay(1190032, 48000);
        this.shion.setMotionFlags(0x2000000, true);
        this.allen.setMotionFlags(0x2000000, true);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.8f, 0.8f, 0.8f);
        this.light.setDirection2(1, 0.494f, 0.249f, 0.833f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.CameraStart("cut1");
        this.mobj049.start(1, "act1");
        this.shion.setShadow(5, 32);
        this.allen.setShadow(5, 32);
        System.sleep(60);
        this.CameraStart("cut2");
        this.shion.start(1, "act2");
        this.allen.start(1, "act2");
        this.mobj049.start(1, "act2");
        this.s_walk_dz = 0.028f;
        this.a_walk_dz = 0.028f;
        this.s_walk.setTarget(this, "s_walk_main");
        this.a_walk.setTarget(this, "a_walk_main");
        this.s_walk.start();
        this.a_walk.start();
        System.sleep(30);
        this.tool.SoundstreamPlay(112001);
        this.tool.MSG(30, this.shion.face, 1, "Allen...");
        System.sleep(10);
        this.tool.SoundstreamPlay(112002);
        this.tool.MSG(60, this.shion.face, 1, "Thanks for stepping in back there.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112003);
        this.tool.MSG(70, this.allen.face, 1, "Ah, no, I should've spoken\nto the new guy earlier.");
        System.sleep(10);
        this.tool.SoundstreamPlay(112004);
        this.tool.MSG(80, this.allen.face, 1, "So you don't need to thank me.");
        this.shion.start(1, "act3");
        this.allen.start(1, "act3");
        this.s_walk.stop();
        this.a_walk.stop();
        this.s_walk.setTarget(this, "s_walk_main2");
        this.a_walk.setTarget(this, "a_walk_main2");
        this.s_walk.start();
        this.a_walk.start();
        this.vec_m.setTranslate(9.9f, -1.0f, 46.9f);
        this.vec_m.setRotate(0.0f, 0.0f, 0.0f);
        this.vec_m.spd = 0.03f;
        this.vec_w.setTranslate(9.0f, -1.0f, 46.4f);
        this.vec_w.setRotate(0.0f, 0.0f, 0.0f);
        this.vec_w.spd = 0.028f;
        this.pilot.setTranslate(9.9f, -1.0f, 38.0f);
        this.pilot.setRotate(0.0f, 0.0f, 0.0f);
        this.pilot.spd = 0.028f;
        this.breal_w.setTranslate(9.0f, -1.0f, 30.0f);
        this.breal_w.setRotate(0.0f, 0.0f, 0.0f);
        this.breal_w.spd = 0.028f;
        this.vec_m.start(1, "walk");
        this.vec_w.start(1, "walk");
        this.pilot.start(1, "walk");
        this.breal_w.start(1, "walk");
        this.CameraStart("cut3");
        System.sleep(30);
        this.tool.SoundstreamPlay(112005);
        this.tool.MSG(90, this.allen.face, 1, "But it would really help if you would\ntry to understand their feelings more.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112006);
        this.tool.MSG(120, this.allen.face, 1, "I know everyone's still a little\nuneasy about KOS-MOS,");
        this.tool.MSG(60, this.allen.face, 1, "but they've been pouring their hearts\nand souls into this project.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112007);
        this.tool.MSG(70, this.allen.face, 1, "They all want to see with their own eyes\nthe end results of");
        this.tool.SoundstreamPlay(112008);
        this.tool.MSG(60, this.allen.face, 1, "what they've been working so hard\nto complete.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112009);
        this.tool.MSG(50, this.allen.face, 1, "I feel the same way myself, you know?");
        System.sleep(50);
        this.tool.SoundstreamPlay(112010);
        this.tool.MSG(30, this.shion.face, 7, "I know that.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112011);
        this.tool.MSG(70, this.shion.face, 7, "It's just that I...");
        System.sleep(10);
        this.s_walk.stop();
        this.a_walk.stop();
        this.vec_m.stop();
        this.vec_m.setVisible(false);
        this.tool.mtnStop((Chr) this.vec_m);
        this.vec_w.stop();
        this.pilot.stop();
        this.breal_w.stop();
        this.shion.start(1, "act4");
        this.allen.start(1, "act4");
        this.CameraStart("cut4");
        System.sleep(60);
        this.tool.SoundstreamPlay(112012);
        this.tool.MSG(40, this.allen.face, 5, 30, "Chief...");
        this.tool.SoundstreamPlay(112013);
        this.tool.MSG(60, this.allen.face, 5, 50, "The incident, right...?");
        this.CameraStart("cut5");
        this.tool.SoundstreamPlay(112014);
        this.tool.sMSG(30, this.shion.face, 15, "　");
        this.shion.look_eye_speed(1.0f);
        this.shion.look_eye_set(-2.4f, -3.3f);
        this.tool.SoundstreamPlay(112015);
        this.tool._MSG(150, "シオン", "Huh? Oh...I see...you're...");
        this.tool.sFACE(40, this.shion.face, 273);
        System.sleep(20);
        this.tool.sFACE(40, this.shion.face, 273);
        System.sleep(20);
        this.tool.FACE(30, this.shion.face, 273);
        this.shion.start(1, "act6");
        this.allen.start(1, "act6");
        this.CameraStart("cut6");
        this.shion.setShadow(0, 0);
        this.allen.setShadow(0, 0);
        this.tool.SoundstreamPlay(112017);
        this.tool.MSG(50, this.shion.face, 1, " ");
        this.tool.SoundstreamPlay(112018);
        this.tool.MSG(100, this.shion.face, 1, "Sorry.\nThat's...not...quite...it.");
        this.tool.sFACE(this.allen.face, 2);
        System.sleep(30);
        this.tool.SoundstreamPlay(112019);
        this.tool.MSG(70, this.shion.face, 3, "Forget about it. It's nothing.");
        System.sleep(30);
        this.tool.SoundstreamPlay(112020);
        this.tool.MSG(90, "シオン", "Let's just hurry up and get KOS-MOS completed, okay?");
        this.tool._MSG(60, "ダミー", "　");
        System.sleep(60);
        this.CG.setVisible(true);
        this.tool.SoundstreamPlay(112021);
        this.tool._MSG(60, "シオン", "Oh...who could that be?");
        System.sleep(30);
        this.CGMon.start(1, "on_s");
        System.sleep(30);
        this.shion.start(1, "act7");
        this.allen.start(1, "act7");
        this.mobj049.start(1, "act7");
        this.CameraStart("cut7");
        this.breal_w.start(1, "act7mob1");
        this.vec_w.setTranslate(7.4f, -1.0f, 48.18f);
        this.vec_w.setRotate(0.0f, 360.0f, 0.0f);
        this.vec_w.spd = 0.04f;
        this.vec_w.start(1, "walk");
        this.pilot.setTranslate(9.0f, -1.0f, 55.48f);
        this.pilot.setRotate(0.0f, 180.0f, 0.0f);
        this.pilot.spd = -0.04f;
        this.pilot.start(1, "walk");
        this.CGMon.stop();
        this.CGMon.setScale(0.0f, 0.0f, 0.0f);
        this.tool.mtnStop(this.allen.face);
        this.tool.mtnStop(this.shion.face);
        this.allen.face.setVisible(false);
        this.breal_w.face.setVisible(false);
        this.pilot.face.setVisible(false);
        this.shion.look_eye_speed(2.0f);
        this.shion.look_eye_set(0.0f, -2.4f);
        this.tool.SoundstreamPlay(112022);
        this.tool.MSG(60, "アレン", "More Realian maintenance?");
        this.tool.SoundstreamPlay(112023);
        this.tool.MSG(90, "アレン", "They really should stop calling you\nall the time.");
        this.tool.SoundstreamPlay(112024);
        this.tool.MSG(120, "アレン", "Technically, it's a violation of\nregulations for you to deal with\nother divisions.");
        this.tool.SoundstreamPlay(112025);
        this.tool.MSG(90, "アレン", "Besides, aren't they expecting\nyou on the bridge?");
        this.shion.start(1, "act8");
        this.allen.start(1, "act8");
        this.CameraStart("cut8");
        this.shion.setShadow(5, 32);
        this.allen.setShadow(5, 32);
        this.allen.face.setVisible(true);
        this.tool.SoundstreamPlay(112026);
        this.tool.MSG(50, this.shion.face, 1, "Oh, don't worry about it.");
        this.tool.SoundstreamPlay(112027);
        this.tool.MSG(150, this.shion.face, 1, "I don't want to brush them off.\nBesides, it's on the way, \nand I've got some time.");
        this.tool.FACE(this.shion.face, 1);
        this.tool.SoundstreamPlay(112031);
        this.tool.MSG(30, this.shion.face, 3, "See ya!");
        System.sleep(30);
        this.camerawork.cut9();
        this.allen.start(1, "act9");
        System.sleep(60);
        this.tool.SoundstreamPlay(112032);
        this.tool.MSG(30, this.allen.face, 7, 20, "Oh...?");
        System.sleep(60);
        this.tool.SoundstreamPlay(112033);
        this.tool.MSG(120, this.allen.face, 7, 100, "...Duh!\nI forgot to ask her out after work...");
        System.sleep(100);
        this.tool.Timechk_SceneEnd();
    }

    void s_walk_main() {
        while (true) {
            this.shion.setTranslate(this.shion.px, this.shion.py, this.shion.pz - this.s_walk_dz);
            System.sleep(1);
        }
    }

    void s_walk_main2() {
        int n = 0;
        float f = 0.028f;
        while (n != 660) {
            this.shion.setTranslate(this.shion.px, this.shion.py, this.shion.pz - f);
            ++n;
            System.sleep(1);
        }
        while (f > 0.0f) {
            this.shion.setTranslate(this.shion.px, this.shion.py, this.shion.pz - f);
            f -= 0.004f;
            System.sleep(1);
        }
    }

    class Mob_m
            extends Chr {
        float spd;
        Chr face;

        public Mob_m(int n) {
            this.init(n + 0x1000000);
            this.face = this.getChild(0x1000000);
            this.mtn(271, 0, 89, 8, 8, 1.0f, true);
        }

        void walk() {
            this.mtn(269, 0, 25, 8, 8, 1.0f, false);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz + this.spd);
                System.sleep(1);
            }
        }
    }

    class Mob_w
            extends Chr {
        float spd;
        Chr face;

        public Mob_w(int n) {
            this.init(n + 0x1000000);
            this.face = this.getChild(0x1000000);
            this.mtn(272, 0, 89, 8, 8, 1.0f, true);
        }

        void act7mob1() {
            this.setTranslate(6.6f, -1.0f, 55.2f);
            this.setRotate(0.0f, 140.0f, 0.0f);
            this.mtn(272, 0, 89, 8, 8, 1.0f, true);
        }

        void walk() {
            this.mtn(270, 0, 27, 8, 8, 1.0f, false);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz + this.spd);
                System.sleep(1);
            }
        }
    }

    class mobj049
            extends Chr {
        Spline posSPL = Spline.create();

        mobj049() {
        }

        void act1() {
            this.setTranslate(9.0f, -1.0f, 53.6f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(257, 0, 84, 8, 8, 1.0f, false);
            float[] fArray = new float[]{1.0f, 8.2f, -1.0f, 37.7f, 360.0f, 8.2f, -1.0f, 45.6f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 360);
            this.move(this.posSPL, 0, true);
        }

        void act2() {
            this.mtn(257, 0, 84, 8, 8, 1.0f, false);
            float[] fArray = new float[]{1.0f, 7.2f, -1.0f, 40.6f, 300.0f, 7.2f, -1.0f, 45.1f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 300);
            this.move(this.posSPL, 0, true);
        }

        void act7() {
            this.setTranslate(5.9f, -1.0f, 57.7f);
            this.setRotate(0.0f, 789.98f, 0.0f);
            this.mtn(Integer.MIN_VALUE, 0, 1.0f, false);
        }

        void init() {
            this.init(20530);
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
            this.setTranslate(12.3f, -1.0f, 46.18f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(258, 0, 372, 8, 8, 1.0f, true);
        }

        void act3() {
            this.setTranslate(11.6f, -1.0f, 52.1f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(260, 0, 760, 8, 8, 1.0f, true);
        }

        void act4() {
            this.setTranslate(11.6f, -1.0f, 33.56f);
            this.setRotate(0.0f, 200.0f, 0.0f);
            this.mtn(262, 0, 350, 8, 8, 1.0f, true);
        }

        void act6() {
            this.setTranslate(11.95f, -1.0f, 33.56f);
            this.setRotate(0.0f, 213.0f, 0.0f);
            this.mtn(264, 0, 492, 8, 0, 1.0f, true);
        }

        void act7() {
            this.setTranslate(11.35f, -1.0f, 33.56f);
            this.setRotate(0.0f, 183.0f, 0.0f);
            this.mtn(260, 550, 700, 0, 0, 1.0f, false);
            int n = 0;
            while (n != 120) {
                this.pz -= 0.025f;
                this.setTranslate();
                ++n;
                System.sleep(1);
            }
            n = 0;
            while (n != 30) {
                this.ry += 5.0f;
                this.setRotate();
                ++n;
                System.sleep(1);
            }
        }

        void act8() {
            this.setTranslate(11.01f, -1.0f, 31.86f);
            this.setRotate(0.0f, 722.99f, 0.0f);
            this.mtn(266, 0, 270, 0, 8, 0.9f, true);
        }

        void init() {
            this.init(0x100001E, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
        }
    }

    class allen
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        allen() {
        }

        void act2() {
            this.setTranslate(11.6f, -1.0f, 46.6f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(259, 0, 372, 8, 8, 1.0f, true);
        }

        void act3() {
            this.setTranslate(10.9f, -1.0f, 53.24f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(261, 0, 760, 8, 8, 1.0f, true);
        }

        void act4() {
            this.setTranslate(10.9f, -1.0f, 32.02f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(263, 0, 170, 8, 8, 1.0f, true);
        }

        void act6() {
            this.setTranslate(10.72f, -1.0f, 32.02f);
            this.setRotate(0.0f, 120.0f, 0.0f);
            this.mtn(265, 0, 492, 8, 8, 1.0f, true);
        }

        void act7() {
            this.mtn(265, 0, 1, 8, 8, 1.0f, false);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = 439.98f;
            fArray[4] = 120.0f;
            fArray[6] = 529.98f;
            float[] fArray2 = fArray;
            this.rotSPL.setCtrlVertex(fArray2, 0, 1, 120);
            this.rotate(this.rotSPL, true);
        }

        void act8() {
            this.setTranslate(10.95f, -1.0f, 32.82f);
            this.setRotate(0.0f, 540.0f, 0.0f);
            this.mtn(267, 0, 270, 8, 8, 0.9f, true);
        }

        void act9() {
            this.setTranslate(10.95f, -1.0f, 34.12f);
            this.setRotate(0.0f, 540.0f, 0.0f);
            this.mtn(268, 0, 390, 8, 0, 1.0f, true);
        }

        void init() {
            this.init(0x1000107, 11.2f, -1.0f, 72.0f, 180.0f);
            this.face = this.getChild(0x1000000);
        }
    }

    class Camerawork {
        Camerawork() {
        }

        void cut1() {
            SCE01012.this.tool.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 1, 24880, 1);
            Runtime.setDefocusQuick(1, 1, 16688, 1);
            Runtime.setDefocusQuick(2, 1, 8496, 1);
            SCE01012.this.BaseCam.setTranslate(8.81f, -0.13f, 42.08f);
            SCE01012.this.BaseCam.setRotate(14.12f, 16.3f, 0.0f);
            SCE01012.this.BaseCam.setFov(29.8f);
            SCE01012.this.BaseCam.change();
            System.sleep(37);
            SCE01012.this.tool.VIBCamera_on(0.01f);
            System.sleep(3);
            SCE01012.this.tool.VIBCamera_off();
        }

        public void cut10() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.BaseCam.setTranslate(10.980095f, 0.38245f, 39.356617f);
            SCE01012.this.BaseCam.setRotate(-1.244918f, 376.72516f, 0.0f);
            SCE01012.this.BaseCam.setFov(40.0f);
        }

        public void cut11() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.tool.STCameraPRO_on();
            float[] fArray = new float[]{1.0f, 9.01f, 0.27f, 38.49f, 210.0f, 8.94f, 0.27f, 36.39f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -0.76f;
            fArray2[2] = 234.08f;
            fArray2[4] = 210.0f;
            fArray2[5] = -0.7f;
            fArray2[6] = 237.43f;
            float[] fArray3 = fArray2;
            SCE01012.this.BaseCam.transSPL(fArray, 1, 3, 210);
            SCE01012.this.BaseCam.rotateSPL(fArray3, 1, 3, 210);
            SCE01012.this.BaseCam.setFov(24.6f);
        }

        public void cut11_2() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 8.94f, 0.27f, 36.39f, 60.0f, 8.94f, 0.27f, 36.39f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -0.7f;
            fArray2[2] = 237.43f;
            fArray2[4] = 60.0f;
            fArray2[5] = -0.7f;
            fArray2[6] = 244.1f;
            float[] fArray3 = fArray2;
            SCE01012.this.BaseCam.transSPL(fArray, 1, 3, 60);
            SCE01012.this.BaseCam.rotateSPL(fArray3, 1, 3, 60);
            SCE01012.this.BaseCam.setFov(24.6f);
        }

        public void cut12() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.BaseCam.setTranslate(10.45f, 0.18f, 37.65f);
            SCE01012.this.BaseCam.setRotate(0.4f, 198.79f, 0.0f);
            SCE01012.this.BaseCam.setFov(23.04f);
        }

        public void cut13() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.tool.STCamera_off();
            SCE01012.this.BaseCam.setTranslate(10.18f, 0.62f, 41.15f);
            SCE01012.this.BaseCam.setRotate(-10.16f, 319.26f, 0.0f);
            SCE01012.this.BaseCam.setFov(24.2f);
        }

        public void cut14() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.BaseCam.setTranslate(10.33f, 0.4f, 39.08f);
            SCE01012.this.BaseCam.setRotate(-1.08f, 206.35f, 0.0f);
            SCE01012.this.BaseCam.setFov(30.4f);
        }

        public void cut15() {
            SCE01012.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 12.151967f, 0.765741f, 38.206898f, 210.0f, 12.151967f, 1.191976f, 38.206898f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -18.044525f;
            fArray2[2] = -220.135f;
            fArray2[4] = 210.0f;
            fArray2[5] = -18.044521f;
            fArray2[6] = -208.90622f;
            float[] fArray3 = fArray2;
            SCE01012.this.BaseCam.transSPL(fArray, 1, 3, 210);
            SCE01012.this.BaseCam.rotateSPL(fArray3, 1, 3, 210);
        }

        void cut2() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE01012.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE01012.this.light.setDirection2(1, -0.448f, 0.283f, -0.848f);
            SCE01012.this.light.setColor(2, 0.38f, 0.38f, 0.38f);
            SCE01012.this.light.setDirection2(2, -0.447f, 0.573f, 0.686f);
            SCE01012.this.light.setColor(3, 0.22f, 0.22f, 0.22f);
            SCE01012.this.light.setDirection2(3, -0.814f, -0.454f, -0.363f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, 4.04f, -0.13f, 41.5f, 320.0f, 4.04f, -0.13f, 41.5f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -0.46f;
            fArray2[2] = -108.28f;
            fArray2[4] = 320.0f;
            fArray2[5] = -0.46f;
            fArray2[6] = -80.58f;
            float[] fArray3 = fArray2;
            SCE01012.this.BaseCam.transSPL(fArray, 0);
            SCE01012.this.BaseCam.rotateSPL(fArray3, 0);
            SCE01012.this.BaseCam.setFov(29.48f);
            System.sleep(37);
            SCE01012.this.tool.VIBCamera_on(0.01f);
            System.sleep(3);
            SCE01012.this.tool.VIBCamera_off();
            System.sleep(40);
            SCE01012.this.tool.VIBCamera_on(0.01f);
            System.sleep(4);
            SCE01012.this.tool.VIBCamera_off();
            System.sleep(37);
            SCE01012.this.tool.VIBCamera_on(0.01f);
            System.sleep(3);
            SCE01012.this.tool.VIBCamera_off();
            System.sleep(40);
            SCE01012.this.tool.VIBCamera_on(0.01f);
            System.sleep(4);
            SCE01012.this.tool.VIBCamera_off();
        }

        void cut3() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.22f, 0.22f, 0.22f);
            SCE01012.this.light.setColor(1, 0.61f, 0.61f, 0.61f);
            SCE01012.this.light.setDirection2(1, -0.398f, 0.538f, -0.743f);
            SCE01012.this.light.setColor(2, 0.42f, 0.42f, 0.42f);
            SCE01012.this.light.setDirection2(2, 0.05f, 0.557f, 0.829f);
            SCE01012.this.light.setColor(3, 0.38f, 0.38f, 0.38f);
            SCE01012.this.light.setDirection2(3, -0.88f, -0.424f, -0.215f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE01012.this.shion.getTranslate();
            SCE01012.this.allen.getTranslate();
            SCE01012.this.BaseCam.setTranslate(7.84f, -0.1f, 52.664f);
            SCE01012.this.BaseCam.setRotate(6.08f, -86.26f, 0.0f);
            SCE01012.this.BaseCam.setFov(29.48f);
            SCE01012.this.tool.STCamera_on();
            int n = 0;
            while (n != 760) {
                System.sleep(1);
                SCE01012.this.BaseCam.setTranslate(7.84f, -0.1f, (SCE01012.this.shion.pz + SCE01012.this.allen.pz) / 2.0f);
                ++n;
            }
        }

        void cut4() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE01012.this.light.setColor(1, 0.68f, 0.68f, 0.68f);
            SCE01012.this.light.setDirection2(1, -0.563f, 0.634f, -0.531f);
            SCE01012.this.light.setColor(2, 0.48f, 0.48f, 0.48f);
            SCE01012.this.light.setDirection2(2, 0.846f, 0.436f, -0.307f);
            SCE01012.this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            SCE01012.this.light.setDirection2(3, 0.002f, -0.735f, -0.678f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE01012.this.tool.STCamera_off();
            SCE01012.this.BaseCam.setTranslate(11.12f, 1.34f, 30.79f);
            SCE01012.this.BaseCam.setRotate(-27.74f, -177.22f, 0.0f);
            SCE01012.this.BaseCam.setFov(29.48f);
        }

        void cut5() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE01012.this.light.setDirection2(1, -0.825f, 0.242f, -0.511f);
            SCE01012.this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            SCE01012.this.light.setDirection2(2, 0.72f, 0.306f, -0.622f);
            SCE01012.this.light.setColor(3, 0.14f, 0.14f, 0.14f);
            SCE01012.this.light.setDirection2(3, 0.002f, -0.735f, -0.678f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE01012.this.BaseCam.setTranslate(10.98f, 0.47f, 32.44f);
            SCE01012.this.BaseCam.setRotate(-4.9f, -149.72f, 0.0f);
            SCE01012.this.BaseCam.setFov(23.72f);
        }

        void cut6() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE01012.this.light.setDirection2(1, 0.969f, 0.198f, 0.148f);
            SCE01012.this.light.setColor(2, 0.6f, 0.6f, 0.6f);
            SCE01012.this.light.setDirection2(2, -0.807f, 0.473f, 0.353f);
            SCE01012.this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            SCE01012.this.light.setDirection2(3, 0.047f, -0.767f, 0.64f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE01012.this.BaseCam.setTranslate(11.35f, 0.41f, 33.69f);
            SCE01012.this.BaseCam.setRotate(-0.7f, -346.23f, 0.0f);
            SCE01012.this.BaseCam.setFov(25.96f);
        }

        public void cut7() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE01012.this.light.setDirection2(1, 0.796f, 0.412f, 0.443f);
            SCE01012.this.light.setColor(2, 0.51f, 0.51f, 0.51f);
            SCE01012.this.light.setDirection2(2, -0.694f, 0.492f, 0.526f);
            SCE01012.this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            SCE01012.this.light.setDirection2(3, 0.047f, -0.767f, 0.64f);
            Stage.setColor(0.96f, 0.96f, 0.96f);
            SCE01012.this.BaseCam.setTranslate(6.34f, -0.26f, 61.97f);
            SCE01012.this.BaseCam.setRotate(5.7f, -366.01f, 0.0f);
            SCE01012.this.BaseCam.setFov(25.96f);
        }

        void cut8() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE01012.this.light.setDirection2(1, -0.845f, 0.365f, 0.391f);
            SCE01012.this.light.setColor(2, 0.47f, 0.47f, 0.47f);
            SCE01012.this.light.setDirection2(2, 0.784f, 0.195f, 0.59f);
            SCE01012.this.light.setColor(3, 0.23f, 0.23f, 0.23f);
            SCE01012.this.light.setDirection2(3, -0.674f, -0.538f, 0.506f);
            SCE01012.this.BaseCam.setTranslate(10.43f, 0.64f, 33.2f);
            SCE01012.this.BaseCam.setRotate(-10.46f, -390.36f, 0.0f);
            SCE01012.this.BaseCam.setFov(25.96f);
        }

        void cut9() {
            SCE01012.this.tool.Timechk_CutChange();
            SCE01012.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setColor(1, 0.67f, 0.67f, 0.67f);
            SCE01012.this.light.setDirection2(1, -0.769f, 0.361f, -0.528f);
            SCE01012.this.light.setColor(2, 0.54f, 0.54f, 0.54f);
            SCE01012.this.light.setDirection2(2, 0.659f, 0.168f, -0.734f);
            SCE01012.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE01012.this.light.setDirection2(3, -0.214f, -0.65f, -0.729f);
            Stage.setColor(0.96f, 0.96f, 0.96f);
            SCE01012.this.BaseCam.setTranslate(10.92f, 0.03f, 32.81f);
            SCE01012.this.BaseCam.setRotate(18.96f, -539.31f, 0.0f);
            SCE01012.this.BaseCam.setFov(25.96f);
        }
    }

    class Monitor
            extends Unit {
        float sz;
        int alpha;

        public Monitor() {
            this.init(24613);
        }

        public Monitor(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void moveloop() {
            float f = 0.0f;
            while (true) {
                this.setTranslate(1.0f + Math.sin(f) * 0.02f, 1.1f + Math.cos(f) * 0.02f, -2.23f);
                f += 0.1f;
                System.sleep(1);
            }
        }

        void off_a() {
            this.alpha = 96;
            while (this.alpha <= 0) {
                this.alpha -= 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 0, 0, 0, -1);
        }

        void off_s() {
            float f = this.sz;
            this.setArgs(2, 96, 0, 0, -1);
            this.setScale(f, f, f);
            while (f > 0.0f) {
                this.setScale(this.sz, f -= 0.1f, this.sz);
                System.sleep(1);
            }
            f = this.sz;
            while (f > 0.0f) {
                this.setScale(f -= 0.1f, 0.01f, f);
                System.sleep(1);
            }
            this.setScale(0.0f, 0.0f, 0.0f);
            this.setArgs(2, 0, 0, 0, -1);
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

        void on_s() {
            float f = 0.0f;
            this.setArgs(2, 96, 0, 0, -1);
            this.setScale(0.0f, 0.0f, 0.0f);
            while (f <= this.sz) {
                this.setScale(f += 0.1f, 0.01f, 0.01f);
                System.sleep(1);
            }
            f = 0.01f;
            while (f <= this.sz) {
                this.setScale(this.sz, f += 0.1f, f);
                System.sleep(1);
            }
            this.setScale(this.sz, this.sz, this.sz);
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class Door
            extends MAPUnit {
        Spline posSPL = Spline.create();

        Door() {
        }

        void close_l() {
            float[] fArray = new float[]{1.0f, 13.13f, this.py, this.pz, 120.0f, 10.83f, this.py, this.pz};
            this.posSPL.setCtrlVertex(fArray, 0, 1, 120);
            this.move(this.posSPL, 0, true);
        }

        void close_r() {
            float[] fArray = new float[]{1.0f, 6.92f, this.py, this.pz, 120.0f, 8.92f, this.py, this.pz};
            this.posSPL.setCtrlVertex(fArray, 0, 1, 120);
            this.move(this.posSPL, 0, true);
        }

        void open_l() {
            this.setTranslate(13.13f, this.py, this.pz);
        }

        void open_r() {
            this.setTranslate(6.92f, this.py, this.pz);
        }
    }
}

