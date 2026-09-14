import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01042
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01042,
        MC_ELS01_PRJ,
        JNT_Human,
        JNT_Accesories,
        FLSkosmos_h,
        FLShammer,
        FLStonny,
        FLSmatehws {
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    FaceChr hammer = new FaceChr(278);
    Chr kosmos = new FaceChr(2);
    FaceChr mathews = new FaceChr(275);
    Chrs mobj064a = new Chrs(20545);
    Chrs mobj064b = new Chrs(20545);
    Chrs mobj064c = new Chrs(20545);
    Chrs mobj065a = new Chrs(20546);
    Chrs mobj065b = new Chrs(20546);
    Chrs mobj065c = new Chrs(20546);
    Chrs mobj066a = new Chrs(20547);
    Chrs mobj066b = new Chrs(20547);
    Chrs mobj066c = new Chrs(20547);
    Chrs mobj067a = new Chrs(20548);
    Chrs mobj067b = new Chrs(20548);
    Chrs mobj067c = new Chrs(20548);
    Chrs mobj001 = new Chrs(20482);
    FaceChr tonny = new FaceChr(277);
    Monitor fm1;
    Monitor fm2;
    Monitor fm3;
    Monitor fm4;
    Monitor fm5;
    Monitor fm6;
    Monitor fm7;
    Monitor em1;
    Monitor em2;
    Monitor em3;
    Monitor em4;
    Monitor em5;
    Monitor em6;
    Monitor em7;
    Monitor tonny_fm;
    Monitor masyu_fm;
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    float[] shadowFilter = new float[4];
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01042() {
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
                    System.sleep(50);
                    this.tool.SoundstreamPlay(142001);
                    this.tool.MSG(30, this.hammer.face, 1, "Huh?");
                    break;
                }
                case 120: {
                    this.tool.Timechk_CutChange();
                    this.PCextends_start(120, 254, 1.2f);
                    this.light.setColor(0, 0.2f, 0.2f, 0.2f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    this.tool.SoundstreamPlay(142002);
                    this.tool.MSG(50, this.hammer.face, 1, "Hey, Captain...");
                    this.tool.SoundstreamPlay(142003);
                    this.tool.MSG(60, this.hammer.face, 1, "I've got someone on the open channel.");
                    this.PCextends_end(n);
                    break;
                }
                case 255: {
                    this.tool.Timechk_CutChange();
                    this.PCextends_start(255, 374, 1.4f);
                    this.light.setColor(0, 0.14f, 0.14f, 0.14f);
                    this.light.setColor(1, 0.31f, 0.31f, 0.31f);
                    this.light.setDirection2(1, -0.915f, 0.402f, 0.044f);
                    this.light.setColor(2, 0.51f, 0.51f, 0.51f);
                    this.light.setDirection2(2, 0.919f, -0.0f, 0.394f);
                    this.light.setColor(3, 0.61f, 0.61f, 0.61f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    this.tool.SoundstreamPlay(142004);
                    this.tool.MSG(40, this.mathews.face, 1, "Who could it be?");
                    this.tool.SoundstreamPlay(142005);
                    this.tool.MSG(40, this.mathews.face, 1, "Survivors?");
                    this.PCextends_end(n);
                    break;
                }
                case 375: {
                    this.tool.Timechk_CutChange();
                    this.tool.SoundstreamPlay(142006);
                    this.tool.MSG(50, this.hammer.face, 1, "I dunno.");
                    this.tool.SoundstreamPlay(142007);
                    this.tool.MSG(50, this.hammer.face, 1, "Hold on, I'll put it on the speakers.");
                    break;
                }
                case 510: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.14f, 0.14f, 0.14f);
                    this.light.setColor(1, 0.31f, 0.31f, 0.31f);
                    this.light.setDirection2(1, -0.849f, 0.379f, -0.368f);
                    this.light.setColor(2, 0.51f, 0.51f, 0.51f);
                    this.light.setDirection2(2, 0.919f, -0.0f, 0.394f);
                    this.light.setColor(3, 0.61f, 0.61f, 0.61f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    this.tool.SoundstreamPlay(142008);
                    this.tool.MSG(120, "アレン", "...Hello!\nIs anybody out there?!");
                    break;
                }
                case 690: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.17f, 0.17f, 0.17f);
                    this.light.setColor(1, 0.44f, 0.44f, 0.44f);
                    this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
                    this.light.setColor(2, 0.48f, 0.48f, 0.48f);
                    this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
                    this.light.setColor(3, 0.44f, 0.44f, 0.44f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    this.tool.FACE(this.mathews.face, 8);
                    break;
                }
                case 810: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.22f, 0.22f, 0.22f);
                    this.light.setColor(1, 0.42f, 0.42f, 0.42f);
                    this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
                    this.light.setColor(2, 0.36f, 0.36f, 0.36f);
                    this.light.setDirection2(2, 0.945f, -0.127f, 0.302f);
                    this.light.setColor(3, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(3, -0.728f, -0.658f, -0.194f);
                    System.sleep(30);
                    break;
                }
                case 870: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.408f, 0.899f, -0.16f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, -0.157f, -0.0f, -0.988f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.281f, -0.384f, -0.879f);
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

    void chair(boolean bl) {
        Stage.setVisible(37, bl);
        Stage.setVisible(38, bl);
        Stage.setVisible(39, bl);
        Stage.setVisible(40, bl);
        Stage.setVisible(41, bl);
        Stage.setVisible(42, bl);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01042_F");
        Runtime.setFlags(55, 1, 1);
        System.println("XEVEJNAME:SCE01043");
        Runtime.jumpEvent(1430);
    }

    public void cleanupOriginal() {
    }

    void init() {
        Runtime.setLocation(46);
        this.monitor(false);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.pc = PlayControl.create();
        this.pc.loadCamera("c01s42.cam");
        this.pc.init(1, 4);
        this.fm1 = new Monitor();
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1.setArgs(1, 20053, 0, 128, 112);
        this.fm1.setArgs(2, 77, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm1.setScale(0.3f, 0.3f, 0.3f);
        this.fm1.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1.setRotate(0.0f, 3.15f, 0.0f);
        this.hammer.mtn(257, 0, 1050, 8, 8, 1.0f, true);
        this.kosmos.mtn(258, 0, 1050, 8, 0x40000008, 1.0f, true);
        this.mathews.mtn(259, 0, 1050, 8, 8, 1.0f, true);
        this.mobj064a.mtn(260, 0, 1050, 8, 8, 1.0f, true);
        this.mobj064b.mtn(261, 0, 1050, 8, 8, 1.0f, true);
        this.mobj064c.mtn(262, 0, 1050, 8, 8, 1.0f, true);
        this.mobj065a.mtn(263, 0, 1050, 8, 8, 1.0f, true);
        this.mobj065b.mtn(264, 0, 1050, 8, 8, 1.0f, true);
        this.mobj065c.mtn(265, 0, 1050, 8, 8, 1.0f, true);
        this.mobj066a.mtn(266, 0, 1050, 8, 8, 1.0f, true);
        this.mobj066b.mtn(267, 0, 1050, 8, 8, 1.0f, true);
        this.mobj066c.mtn(268, 0, 1050, 8, 8, 1.0f, true);
        this.mobj067a.mtn(269, 0, 1050, 8, 8, 1.0f, true);
        this.mobj067b.mtn(270, 0, 1050, 8, 8, 1.0f, true);
        this.mobj067c.mtn(271, 0, 1050, 8, 8, 1.0f, true);
        this.mobj001.mtn(272, 0, 1050, 8, 8, 1.0f, true);
        this.tonny.mtn(273, 0, 1050, 8, 8, 1.0f, true);
        this.hammer.start(5, null);
        this.kosmos.start(5, null);
        this.mathews.start(5, null);
        this.mobj064a.start(5, null);
        this.mobj064b.start(5, null);
        this.mobj064c.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj065c.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj066c.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj067b.start(5, null);
        this.mobj067c.start(5, null);
        this.mobj001.start(5, null);
        this.tonny.start(5, null);
    }

    void initialize() {
    }

    static void main() {
    }

    void monitor(boolean bl) {
        Stage.setVisible(52, bl);
        Stage.setVisible(53, bl);
        Stage.setVisible(54, bl);
        Stage.setVisible(55, bl);
        Stage.setVisible(56, bl);
        Stage.setVisible(57, bl);
        Stage.setVisible(51, bl);
    }

    void play() {
        this.tool.loadarc(this.mathews.face, "FLSmatehws.fpk");
        this.tool.loadarc(this.tonny.face, "FLStonny.fpk");
        this.tool.loadarc(this.hammer.face, "FLShammer.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.SoundstreamDebug_init(999999);
        Sound.streamPlay(1190053, 48000);
        this.mathews.renderCommand(18);
        this.tonny.renderCommand(18);
        this.hammer.renderCommand(18);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        this.pc.start();
        this.MotionPack_srv(690);
        this.tool.Timechk_SceneEnd();
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

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
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

