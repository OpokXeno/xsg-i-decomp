import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02012A
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02012A,
        JNT_Human,
        JNT_Accesories,
        FLSandrew_h {
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    FaceChr andrew = new FaceChr(333);
    FaceChr kebin = new FaceChr(267);
    Chrs CG = new Chrs(24577);
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    float[] shadowFilter = new float[4];
    int act_start;
    int act_end;
    Units CGa;
    Units CGk;
    Units target;
    Effect eft1;
    Effect eft2;
    Effect eft3;
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02012A() {
    }

    void MotionPack_srv() {
        this.MotionPack_srv(1000000);
    }

    void MotionPack_srv(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void PCextends_end(int n) {
        this.TotalCutTime = this.tool.Timechk_srv_totaltime - this.TotalCutTime;
        this.tool.Timechk_srv_totaltime = this.NextPCStart;
        this.pc.init(1, 0, this.NextPCStart, n, 1.0f);
        this.pc.start();
        System.println("------------ＭＰ速度変更は正常に終了しました。");
        Runtime.setRegister(0, this.TotalCutTime);
        Runtime.setRegister(1, this.tool.CutNo);
        Runtime.setRegister(2, this.BaseCutTime);
        System.println("------------カット/[$1]が（/[$2]→/[$0]）フレームに変更されました。");
        if (this.BaseCutTime <= this.TotalCutTime) {
            Runtime.setRegister(0, this.TotalCutTime - this.BaseCutTime);
            System.println("------------/[$0]フレームの増加です。");
        } else {
            Runtime.setRegister(0, this.BaseCutTime - this.TotalCutTime);
            System.println("------------/[$0]フレームの減少です。");
        }
    }

    void PCextends_start(int n, int n2, float f) {
        this.NextPCStart = n2;
        this.TotalCutTime = this.tool.Timechk_srv_totaltime;
        this.BaseCutTime = n2 - n;
        this.pc.init(1, 0, n, n2, f);
        this.pc.start();
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        Runtime.setRegister(2, f);
        System.println("------------ＭＰの速度を変更します(/[$0]-/[$1]) ×/[#2]");
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
        System.println("XEVEFLAG:EV02012A_F");
        Runtime.setFlags(119, 1, 1);
        System.println("XEVEJNAME:SCE02012B");
        Runtime.jumpEvent(2121);
    }

    public void cleanupOriginal() {
    }

    void init() {
        Runtime.setLocation(714);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.target = new Units(24577);
        this.eft1 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft2 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft3 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1.setCaster(this.target);
        this.eft2.setCaster(this.target);
        this.eft3.setCaster(this.target);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.tool.loadarc(this.andrew.face, "FLSandrew_h.fpk");
        this.tool.loadarc(this.kebin.face, "FLSkebin.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.CameraTool();
        this.tool.Timechk();
        Runtime.setDefocusQuick(0, 1, 174880, 1);
        Runtime.setDefocusQuick(1, 1, 166688, 1);
        Runtime.setDefocusQuick(2, 1, 158496, 1);
        Runtime.setDefocusQuick(3, 1, 150304, 1);
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x1000000;
        nArray[3] = 0x36000000;
        int[] nArray2 = nArray;
        Runtime.setDefocus(15, 1, nArray2);
        Sound.streamPlay(1290080, 48000);
        this.tool.Timechk_SceneStart();
        this.tool.SoundstreamDebug_init(999999);
        this.andrew.setVisible(14, false);
        this.act_start = 0;
        this.act_end = 221;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut1();
        this.target.setTranslate(-0.19f, 3.08f, 14.96f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(1.0f, 1.0f, 1.0f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 222;
        this.act_end = 311;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut2();
        this.target.setTranslate(-2.1f, 2.81f, 16.87f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(1.0f, 1.0f, 1.0f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 312;
        this.act_end = 443;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut3();
        this.target.setTranslate(-1.31f, 3.8f, 13.89f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 444;
        this.act_end = 623;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut4();
        this.target.setTranslate(-1.33f, 3.74f, 14.26f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        System.sleep(108);
        this.tool.SoundstreamPlay(212001);
        this.tool.MSG(30, this.andrew.face, 1, "So you're the...");
        System.sleep(41);
        this.act_start = 624;
        this.act_end = 695;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut5();
        this.target.setTranslate(-1.34f, 3.94f, 14.38f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 696;
        this.act_end = 791;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut6();
        this.target.setTranslate(-0.95f, 3.84f, 13.96f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 792;
        this.act_end = 857;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut7();
        this.target.setTranslate(-1.97f, 3.37f, 14.57f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.2f, 0.2f, 0.2f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 858;
        this.act_end = 959;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut8();
        this.target.setTranslate(-2.32f, 3.27f, 14.57f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.2f, 0.2f, 0.2f);
        System.sleep(60);
        this.tool.SoundstreamPlay(212002);
        this.tool._MSG(90, "アンドリュー", "I trust this will work correctly?");
        this.tool.FACE(this.andrew.face, 1);
        System.sleep(41);
        this.act_start = 960;
        this.act_end = 1049;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut9();
        this.target.setTranslate(-1.37f, 3.87f, 14.37f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        System.sleep(this.act_end - this.act_start);
        this.act_start = 1050;
        this.act_end = 1169;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut10();
        this.target.setTranslate(-1.14f, 3.77f, 14.38f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.1f, 0.1f, 0.1f);
        this.tool.SoundstreamPlay(212003);
        this.tool.MSG(50, this.andrew.face, 1, "But...why?");
        System.sleep(69);
        this.act_start = 1170;
        this.act_end = 1332;
        this.andrew.start(1, "act_a");
        this.kebin.start(1, "act_k");
        this.CG.start(1, "act_c");
        this.camerawork.cut11();
        this.target.setTranslate(-2.0f, 3.44f, 14.02f);
        this.target.setRotate(0.0f, 0.0f, 0.0f);
        this.target.setScale(0.5f, 0.5f, 0.5f);
        System.sleep(this.act_end - this.act_start - 30);
        this.tool.Timechk_SceneEnd();
    }

    class Units
            extends Unit {
        public Units(int n) {
            this.init(n);
        }
    }

    class Chrs
            extends Chr {
        public Chrs(int n) {
            this.init(n);
            this.setShadow(0, 0);
            this.setScale(0.25f, 0.25f, 0.25f);
        }

        void act_c() {
            this.mtn(257, SCE02012A.this.act_start, SCE02012A.this.act_end, 8, 8, 1.0f, true);
        }
    }

    class FaceChr
            extends Chr {
        Chr face;

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(4, 16);
            this.setScale(0.25f, 0.25f, 0.25f);
        }

        void act_a() {
            this.mtn(259, SCE02012A.this.act_start, SCE02012A.this.act_end, 8, 8, 1.0f, true);
        }

        void act_k() {
            this.mtn(262, SCE02012A.this.act_start, SCE02012A.this.act_end, 8, 8, 1.0f, true);
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
                this.setTranslate(1.0f + Math.sin(f) * 0.02f, 0.6f + Math.cos(f) * 0.02f, -2.23f);
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
            this.setScale(0.0f, 0.0f, 0.0f);
        }

        void on1() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on2() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on3() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on4() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
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

    class Camerawork {
        Camerawork() {
        }

        void cut1() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.52f, 0.6f, 0.64f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.955f, 0.245f, 0.166f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.9f, 0.0f, 0.436f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, -0.126f, -0.713f, -0.69f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, 2.99f, 4.78f, 22.6f, 221.0f, 3.71f, 4.78f, 22.26f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 8.36f;
            fArray2[2] = 25.52f;
            fArray2[4] = 221.0f;
            fArray2[5] = 8.36f;
            fArray2[6] = 25.52f;
            float[] fArray3 = fArray2;
            SCE02012A.this.BaseCam.transSPL(fArray, 0);
            SCE02012A.this.BaseCam.rotateSPL(fArray3, 0);
            SCE02012A.this.BaseCam.setFov(40.0f);
            SCE02012A.this.BaseCam.change();
        }

        void cut10() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.83f, 0.397f, 0.391f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.974f, 0.169f, -0.153f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -1.47f, 4.07f, 14.78f, 120.0f, -1.44f, 4.07f, 14.78f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 0.9f;
            fArray2[2] = 347.9f;
            fArray2[4] = 120.0f;
            fArray2[5] = 0.9f;
            fArray2[6] = 347.9f;
            float[] fArray3 = fArray2;
            SCE02012A.this.BaseCam.transSPL(fArray, 0, 1, 120);
            SCE02012A.this.BaseCam.rotateSPL(fArray3, 0, 1, 120);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut11() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.27f, 0.35f, 0.39f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.176f, 0.958f, 0.227f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.773f, 0.597f, -0.214f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -2.8f, 8.9f, 16.92f, 100.0f, -2.8f, 9.64f, 16.92f};
            float[] fArray2 = new float[]{1.0f, -56.94f, 334.96f, 0.64f, 100.0f, -60.5f, 334.96f, -8.3f};
            SCE02012A.this.BaseCam.transSPL(fArray, 0, 1, 162);
            SCE02012A.this.BaseCam.rotateSPL(fArray2, 0, 1, 162);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut2() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.22f, 0.3f, 0.34f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.925f, 0.336f, 0.176f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.9f, 0.01f, 0.436f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, -0.126f, -0.713f, -0.69f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -1.06f, 3.91f, 13.89f, 90.0f, -1.1f, 3.91f, 13.87f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 10.42f;
            fArray2[2] = 153.38f;
            fArray2[4] = 90.0f;
            fArray2[5] = 10.42f;
            fArray2[6] = 153.38f;
            float[] fArray3 = fArray2;
            SCE02012A.this.BaseCam.transSPL(fArray, 0);
            SCE02012A.this.BaseCam.rotateSPL(fArray3, 0);
            SCE02012A.this.BaseCam.setFov(40.0f);
        }

        void cut3() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.83f, 0.397f, 0.391f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.815f, 0.563f, -0.138f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -1.31f, 4.3f, 14.69f, 130.0f, -1.31f, 4.3f, 14.69f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -33.46f;
            fArray2[2] = 360.79f;
            fArray2[4] = 130.0f;
            fArray2[5] = -33.46f;
            fArray2[6] = 369.9f;
            float[] fArray3 = fArray2;
            SCE02012A.this.BaseCam.transSPL(fArray, 0, 1, 130);
            SCE02012A.this.BaseCam.rotateSPL(fArray3, 0, 1, 130);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut4() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.83f, 0.397f, 0.391f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.974f, 0.169f, -0.153f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02012A.this.BaseCam.setTranslate(-1.33f, 4.14f, 14.66f);
            SCE02012A.this.BaseCam.setRotate(4.64f, 364.28f, 0.0f);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut5() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.25f, 0.33f, 0.37f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.955f, 0.245f, 0.166f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.9f, 0.0f, 0.436f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, -0.126f, -0.713f, -0.69f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02012A.this.BaseCam.setTranslate(-1.34f, 4.14f, 14.28f);
            SCE02012A.this.BaseCam.setRotate(6.74f, 530.93f, 0.0f);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut6() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.83f, 0.397f, 0.391f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.974f, 0.169f, -0.153f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02012A.this.BaseCam.setTranslate(-1.35f, 4.14f, 14.66f);
            SCE02012A.this.BaseCam.setRotate(-0.92f, 356.61f, 0.0f);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut7() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.83f, 0.397f, 0.391f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.974f, 0.169f, -0.153f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -1.27f, 4.17f, 14.55f, 65.0f, -1.27f, 4.17f, 14.57f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -0.94f;
            fArray2[2] = 445.31f;
            fArray2[4] = 65.0f;
            fArray2[5] = -0.94f;
            fArray2[6] = 441.18f;
            float[] fArray3 = fArray2;
            SCE02012A.this.BaseCam.transSPL(fArray, 0, 1, 65);
            SCE02012A.this.BaseCam.rotateSPL(fArray3, 0, 1, 65);
            SCE02012A.this.BaseCam.setFov(38.4f);
            SCE02012A.this.tool.STCamera_on();
        }

        void cut8() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.tool.STCamera_off();
            SCE02012A.this.BaseCam.setTranslate(-1.12f, 4.07f, 14.57f);
            SCE02012A.this.BaseCam.setRotate(11.16f, 413.7f, 0.0f);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }

        void cut9() {
            SCE02012A.this.tool.Timechk_CutChange();
            SCE02012A.this.light.setColor(0, 0.24f, 0.32f, 0.36f);
            SCE02012A.this.light.setColor(1, 0.37f, 0.52f, 0.8f);
            SCE02012A.this.light.setDirection2(1, 0.659f, 0.695f, 0.288f);
            SCE02012A.this.light.setColor(2, 0.24f, 0.4f, 0.5f);
            SCE02012A.this.light.setDirection2(2, -0.847f, 0.517f, -0.122f);
            SCE02012A.this.light.setColor(3, 0.19f, 0.34f, 0.46f);
            SCE02012A.this.light.setDirection2(3, 0.624f, -0.496f, 0.604f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02012A.this.BaseCam.setTranslate(-1.27f, 4.17f, 14.47f);
            SCE02012A.this.BaseCam.setRotate(-60.24f, 548.58f, 0.0f);
            SCE02012A.this.BaseCam.setFov(38.4f);
        }
    }
}

