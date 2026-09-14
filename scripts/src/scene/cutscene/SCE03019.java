import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_KAS31_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE03019
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03019,
        MC_KAS31_PRJ,
        JNT_Human,
        JNT_Accesories,
        FLSshion_h,
        FLSelly,
        FLSallen,
        FLSchaos {
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    FaceChr shion = new FaceChr(30);
    FaceChr shion_m = new FaceChr(1);
    FaceChr chaos = new FaceChr(3);
    FaceChr allen = new FaceChr(263);
    FaceChr elly = new FaceChr(274);
    Chrs mobj116 = new Chrs(20597);
    Chrs mobj156 = new Chrs(20637);
    Chrs mobj157 = new Chrs(20638);
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    float[] shadowFilter = new float[4];
    float elly_facespeed = 0.5f;
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE03019() {
    }

    void MotionPack_srv01() {
        this.chr_alloff();
        int n = 222;
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.659f, 0.0f, 0.752f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.074f, 0.995f, 0.065f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.83f, -0.396f, 0.393f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    this.elly.setVisible(true);
                    this.elly.mtn(277, 0, 222, 8, 8, 1.0f, true);
                    this.elly.start(5, null);
                    this.shion.setVisible(true);
                    this.shion.mtn(278, 0, 222, 8, 8, 1.0f, true);
                    this.shion.start(5, null);
                    this.pc.start();
                    this.tool.FACE(this.shion.face, 16);
                    this.tool.FACE(this.elly.face, 2, this.elly_facespeed);
                    System.sleep(30);
                    this.tool.SoundstreamPlay(319001);
                    this.tool._MSG(140, "シオン", "You've been waiting...?\nFor us...?");
                    this.tool.FACE(40, this.shion.face, 15);
                    System.sleep(60);
                    this.tool.FACE(40, this.shion.face, 15);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv02() {
        this.chr_alloff();
        int n = 310;
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.856f, 0.0f, 0.517f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.176f, 0.939f, 0.294f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.239f, -0.841f, 0.486f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    this.shion.renderCommand(532);
                    this.elly.face.setVisible(false);
                    this.chaos.setVisible(0, false);
                    this.chaos.setVisible(2, false);
                    this.chaos.setVisible(3, false);
                    this.chaos.setVisible(4, false);
                    this.chaos.setVisible(9, false);
                    this.chaos.setVisible(10, false);
                    this.allen.setVisible(true);
                    this.allen.mtn(279, 0, 310, 8, 8, 1.0f, true);
                    this.allen.start(5, null);
                    this.chaos.setVisible(true);
                    this.chaos.mtn(280, 0, 310, 8, 8, 1.0f, true);
                    this.chaos.start(5, null);
                    this.elly.setVisible(true);
                    this.elly.mtn(281, 0, 310, 8, 8, 1.0f, true);
                    this.elly.start(5, null);
                    this.shion_m.setVisible(true);
                    this.shion_m.mtn(282, 0, 310, 8, 8, 1.0f, true);
                    this.shion_m.start(5, null);
                    this.pc.start();
                    this.tool.SoundstreamPlay(319002);
                    this.tool._MSG(110, "シオン", "chaos...Allen...?");
                    this.tool.FACE(30, this.shion.face, 1);
                    System.sleep(40);
                    this.tool.FACE(40, this.shion.face, 1);
                    this.tool.SoundstreamPlay(319003);
                    this.tool._MSG(140, "アレン", "Chief...?\nWhere are we?");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv03() {
        this.chr_alloff();
        int n = 90;
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.89f, 0.239f, 0.389f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.243f, 0.938f, 0.246f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, -0.502f, -0.8f, -0.328f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    this.shion.renderCommand(512);
                    this.elly.face.setVisible(true);
                    this.chaos.setVisible(10, true);
                    this.elly.setVisible(true);
                    this.elly.mtn(283, 0, 90, 8, 8, 1.0f, true);
                    this.elly.start(5, null);
                    this.shion.setVisible(true);
                    this.shion.mtn(284, 0, 90, 8, 8, 1.0f, true);
                    this.shion.start(5, null);
                    this.pc.start();
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv04() {
        this.chr_alloff();
        int n = 60;
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.878f, 0.0f, 0.479f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, 0.326f, 0.868f, 0.374f);
                    this.light.setColor(3, 0.19f, 0.2f, 0.21f);
                    this.light.setDirection2(3, 0.281f, -0.717f, 0.638f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    this.allen.setVisible(true);
                    this.allen.mtn(285, 0, 60, 8, 8, 1.0f, true);
                    this.allen.start(5, null);
                    this.chaos.setVisible(true);
                    this.chaos.mtn(286, 0, 60, 8, 8, 1.0f, true);
                    this.chaos.start(5, null);
                    this.shion.setVisible(true);
                    this.shion.mtn(287, 0, 60, 8, 0x2000008, 1.0f, true);
                    this.shion.start(5, null);
                    this.pc.start();
                    this.shion.look_speed(0.0f);
                    this.shion.look_char(this.elly);
                    this.tool.sFACE(this.shion.face, 18);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv05() {
        this.chr_alloff();
        int n = 384;
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.929f, 0.0f, 0.369f);
                    this.light.setColor(2, 0.21f, 0.19f, 0.15f);
                    this.light.setDirection2(2, 0.471f, 0.738f, -0.483f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.069f, -0.668f, -0.741f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    this.elly.setVisible(true);
                    this.elly.mtn(288, 0, 384, 8, 8, 1.0f, true);
                    this.elly.start(5, null);
                    this.shion.setVisible(true);
                    this.shion.mtn(289, 0, 384, 8, 8, 1.0f, true);
                    this.shion.start(5, null);
                    this.pc.start();
                    this.tool.SoundstreamPlay(319004);
                    this.tool.MSG(30, this.shion.face, 1, "Who are you?");
                    System.sleep(30);
                    this.tool.SoundstreamPlay(319005);
                    this.tool._MSG(160, "エリー", "I am...Nephilim. That's what\nI have been called.");
                    this.tool.FACE(50, this.elly.face, 1, this.elly_facespeed);
                    System.sleep(30);
                    this.tool.FACE(40, this.elly.face, 1, this.elly_facespeed);
                    System.sleep(40);
                    this.tool.MSG(90, this.elly.face, 1, this.elly_facespeed, "Ever since I existed in this form...");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void PCextends_end(int n) {
        this.TotalCutTime = this.tool.Timechk_srv_totaltime - this.TotalCutTime;
        this.tool.Timechk_srv_totaltime = this.NextPCStart;
        this.pc.init(1, 0, this.NextPCStart + 1, n, 1.0f);
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

    void PCstart(String string) {
        this.pc.loadCamera(string);
        this.pc.init(1, 0);
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

    void chr_alloff() {
        this.shion.setVisible(false);
        this.shion_m.setVisible(false);
        this.chaos.setVisible(false);
        this.allen.setVisible(false);
        this.elly.setVisible(false);
        this.mobj116.setVisible(false);
        this.mobj156.setVisible(false);
        this.mobj157.setVisible(false);
        this.shion.setMotionFlags(-1073741824, false);
        this.chaos.setMotionFlags(-1073741824, false);
        this.allen.setMotionFlags(-1073741824, false);
        this.elly.setMotionFlags(-1073741824, false);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03019_F");
        Runtime.setFlags(330, 1, 1);
        System.println("XEVEJNAME:CFJ3_70 XEVEJPOINT:POINT3_70");
        Runtime.jumpCF(2420, 1);
    }

    public void cleanupOriginal() {
    }

    void init() {
        Runtime.setLocation(719);
        Runtime.setLocation(700);
        Runtime.setLocation(1230);
        Stage.setVisible(32, false);
        Stage.setVisible(58, false);
        Stage.setVisible(59, false);
        Stage.setVisible(60, false);
        Stage.setVisible(61, false);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam0.change();
        this.pc = PlayControl.create();
    }

    void initialize() {
    }

    static void main() {
    }

    void map_set(int n, boolean bl) {
    }

    void play() {
        this.tool.loadarc(this.elly.face, "FLSelly.fpk");
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.tool.loadarc(this.allen.face, "FLSallen.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.SoundstreamDebug_init(999999);
        Sound.streamPlay(1390059, 48000);
        this.tool.FACE(this.allen.face, 6);
        this.tool.FACE(this.chaos.face, 6);
        this.shion.renderCommand(512);
        this.shion_m.renderCommand(512);
        this.chaos.renderCommand(532);
        this.elly.renderCommand(532);
        this.allen.renderCommand(532);
        this.PCstart("c03s19cut01.cam");
        this.MotionPack_srv01();
        this.PCstart("c03s19cut02.cam");
        this.MotionPack_srv02();
        this.PCstart("c03s19cut03.cam");
        this.MotionPack_srv03();
        this.PCstart("c03s19cut04.cam");
        this.MotionPack_srv04();
        this.PCstart("c03s19cut05.cam");
        this.MotionPack_srv05();
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
        }
    }

    class FaceChr
            extends Chr {
        Chr face;
        int Mtnno;
        int Mtn_start;

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(7, 40);
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
}

