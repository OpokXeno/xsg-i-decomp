import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK05_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01018
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_VOK05_PRJ,
        JNT_Human,
        Pack01018A,
        FLSshion_h {
    public shion2 shion2;
    public shion shion;
    public con con;
    public hunger_on_hand hunger_on_hand;
    public hunger hunger;
    public costume costume;
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    float Location_Z = 60.0f;
    float Location_Y = -1.0f;
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final int BGOBJ_monitor = 41;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    Light light = new Light(0);
    Monitor fm1;
    Monitor fm2;
    Space space = new Space(20615);
    Thread shitan_talk_thread = Thread.create(this, "shitan_talk_main");
    Thread shitan_talk_thread2 = Thread.create(this, "shitan_talkmonitor_main1");
    int shitan_talk_count = 0;
    boolean shitan_talk_flag;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01018() {
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
        System.println("XEVEFLAG:EV01018_F");
        Runtime.setFlags(25, 1, 1);
        System.println("XEVEJNAME:CFJ1_130 XEVEJPOINT:POINT_130");
        Runtime.jumpCF(50, 2);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(1190);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(20);
        Stage.setVisible(29, false);
        Stage.setVisible(44, false);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.fm1 = new Monitor();
        this.fm1.setArgs(0, 0.0f, 0.0f, 1.0f, 0.75f);
        this.fm1.setArgs(1, 20078, 0, 0, 0);
        this.fm1.setArgs(2, 96, 30, 0, -2);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm1.setTranslate(1.0f, 0.5f, -2.23f);
        this.fm1.setRotate(0.0f, 0.0f, 0.0f);
        this.fm1.sz = 1.0f;
        this.fm2 = new Monitor();
        this.fm2.setArgs(0, 0.0f, 0.0f, 1.0f, 0.75f);
        this.fm2.setArgs(1, 20111, 0, 0, 0);
        this.fm2.setArgs(2, 96, 30, 0, -2);
        this.fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm2.signal(1);
        this.fm2.setTranslate(1.0f, 0.5f, -2.23f);
        this.fm2.setRotate(0.0f, 0.0f, 0.0f);
        this.fm2.sz = 1.0f;
        Stage.setVisible(41, false);
        this.fm1.setArgs(2, 96, 30, 0, -2);
        this.fm2.setArgs(2, 0, 30, 0, -2);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.loadarc(this.shion2.face, "FLSshion_h.fpk");
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.fm1.start(1, "moveloop");
        this.fm2.start(1, "moveloop");
        Sound.streamPlay(1190062, 48000);
        this.tool.SoundstreamDebug_init(999999);
        this.tool.Timechk_SceneStart();
        this.shion.setMotionFlags(0x2000000, true);
        this.shion2.setMotionFlags(0x2000000, true);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, 0.897f, 0.03f, 0.441f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, -0.95f, 0.0f, 0.313f);
        this.light.setColor(3, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(3, 0.242f, -0.001f, -0.97f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shion.start(1, "act1");
        this.space.start(1, "move");
        this.camerawork.cut1();
        this.tool.SoundstreamPlay(118001);
        System.sleep(20);
        this.tool.MSG(160, this.shion.face, 45, "Like I told you before, I can't go\nanywhere until my project stabilizes.");
        this.shion.start(1, "act2");
        this.hunger.start(1, "act2");
        this.space.start(1, "move");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, 0.406f, 0.525f, -0.748f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, -0.819f, 0.526f, 0.229f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.324f, -0.648f, -0.689f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.camerawork.cut2();
        this.tool.SoundstreamPlay(118002);
        this.tool.MSG(59, this.shion.face, 45, "Don't you remember?");
        this.tool.SoundstreamPlay(118003);
        this.tool.MSG(180, "シタン", "You know how long you've been\nsaying that? I haven't seen you for\ntwo years now.");
        this.tool.SoundstreamPlay(118004);
        this.tool._MSG(100, "シタン", "You could at least come home for\nour parents' memorial.");
        System.sleep(80);
        this.shion.start(1, "act3");
        this.con.start(1, "act3");
        this.space.start(1, "move");
        this.camerawork.cut3();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.396f, 0.776f, -0.492f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.999f, 0.024f, -0.024f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.561f, -0.508f, -0.654f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        System.sleep(30);
        this.tool.SoundstreamPlay(118005);
        this.tool.MSG(69, "シタン", "Where's your sense of filial duty?");
        this.tool.SoundstreamPlay(118006);
        this.tool._MSG(90, "シオン", "Memorial? Ahh...Come on!");
        this.tool.FACE(this.shion.face, 274);
        System.sleep(40);
        this.shion.start(1, "act4");
        this.hunger.start(1, "act4");
        this.space.start(1, "move");
        this.camerawork.cut4();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.179f, 0.343f, -0.922f);
        this.light.setColor(2, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(2, -0.87f, 0.474f, 0.133f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.799f, -0.572f, -0.187f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        System.sleep(20);
        System.sleep(10);
        System.sleep(20);
        this.tool._MSG(109, "シオン", "Why are you trying to resurrect\nobscure ancient rituals?");
        System.sleep(89);
        this.shion.start(1, "act5");
        this.hunger.setVisible(false);
        this.hunger_on_hand.setVisible(true);
        this.hunger_on_hand.start(1, "parent");
        this.space.start(1, "move");
        this.camerawork.cut5();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.289f, 0.52f, -0.804f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, 0.18f, 0.34f, 0.923f);
        this.light.setColor(3, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(3, -0.525f, -0.352f, 0.775f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        System.sleep(20);
        System.sleep(20);
        this.tool.SoundstreamPlay(118008);
        this.tool._MSG(100, "シオン", "Wait a minute, you've been reading\nthose weird old books again,");
        System.sleep(80);
        this.camerawork.cut6();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.367f, 0.824f, -0.432f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.905f, 0.022f, -0.426f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.067f, -0.65f, -0.757f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shitan_talk(true);
        this.shion.look_eye_set(5.4f, -4.8f);
        this.shion.start(1, "act6");
        this.hunger_on_hand.setTranslate(0.15f, -0.01f, 0.02f);
        this.hunger_on_hand.setRotate(30.0f, -1.0f, 90.0f);
        this.space.start(1, "move");
        System.sleep(20);
        this.tool.MSG(160, this.shion.face, 274, "haven't you?\nI swear, you're so obsessed with\nthose precious books of yours!");
        this.tool.SoundstreamPlay(118010);
        this.tool.MSG(80, "シタン", "That is none of your business,\nthank you very much.");
        this.tool.SoundstreamPlay(118011);
        this.tool.MSG(130, "シタン", "How many times must I tell you");
        this.tool.MSG(60, "シタン", "not to quibble about my way of life.");
        this.tool.SoundstreamPlay(118012);
        this.tool.MSG(40, this.shion.face, 274, "What do you mean, way of life?");
        this.shion.start(1, "act7");
        this.hunger_on_hand.setTranslate(0.11f, -0.01f, 0.02f);
        this.hunger_on_hand.setRotate(30.0f, -1.0f, 90.0f);
        this.space.start(1, "move");
        this.camerawork.cut7();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.57f, 0.632f, -0.525f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, -0.884f, 0.445f, -0.147f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, -0.63f, -0.766f, -0.126f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shitan_talk(false);
        System.sleep(20);
        this.tool.SoundstreamPlay(118013);
        this.tool.MSG(70, this.shion.face, 1, "All that stuff's just a stupid\nold hobby for you.");
        this.shion.setTranslate(-0.17f, 0.01f, 2.48f);
        this.shion.setRotate(0.0f, 25.0f, 0.0f);
        this.hunger.setTranslate(-0.17f, 0.01f, 2.48f);
        this.hunger.setRotate(0.0f, 25.0f, 0.0f);
        this.shion.start(1, "act8");
        this.hunger.start(1, "act8");
        this.hunger.setVisible(true);
        this.hunger_on_hand.setVisible(false);
        this.space.start(1, "move");
        this.camerawork.cut8();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.767f, 0.334f, -0.548f);
        this.light.setColor(2, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(2, 0.17f, 0.465f, 0.869f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, -0.724f, -0.52f, 0.454f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        System.sleep(20);
        this.tool.SoundstreamPlay(118014);
        this.tool.MSG(40, "シオン", "Just remember,");
        this.tool.MSG(120, this.shion.face, 274, "don't expect me to take you in when\nyou're old, senile, and all alone.");
        this.shion2.start(1, "act9");
        this.shion.setVisible(false);
        this.hunger.setVisible(false);
        this.space.start(1, "move");
        this.camerawork.cut9();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.053f, 0.326f, -0.944f);
        this.light.setColor(2, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(2, 0.671f, 0.4f, -0.624f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, 0.654f, -0.325f, -0.683f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shitan_talk(true);
        System.sleep(20);
        this.tool.SoundstreamPlay(118016);
        this.tool.MSG(90, "シタン", "That's terribly rude of you, Shion.");
        this.tool.SoundstreamPlay(118017);
        this.tool.MSG(70, "シタン", "Don't worry about me,\njust promise me");
        this.tool.SoundstreamPlay(118018);
        this.tool.MSG(120, "シタン", "you'll come home this year, okay?\nIf you don't...");
        this.tool.SoundstreamPlay(118020);
        this.tool._MSG(120, "シオン", "All right, all right,\nwhen I get some time off.");
        this.tool.FACE(this.shion2.face, 274);
        this.shion2.start(1, "act10");
        this.costume.start(1, "act10");
        this.space.start(1, "move");
        this.camerawork.cut10();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.854f, 0.48f, -0.2f);
        this.light.setColor(2, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(2, -0.482f, 0.017f, -0.876f);
        this.light.setColor(3, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(3, 0.325f, -0.898f, 0.298f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shion2.look_eye_set(-6.9f, 3.6f);
        this.tool.FACE(60, this.shion2.face, 274);
        this.tool.SoundstreamPlay(118021);
        this.tool._MSG(40, "シオン", "Look, gotta run -- see ya!");
        this.tool.FACE(this.shion2.face, 274);
        System.sleep(40);
        this.tool.SoundstreamPlay(118022);
        this.tool._MSG(35, "シタン", "Hey, wait!");
        System.sleep(15);
        this.tool.sFACE(this.shion2.face, 275);
        System.sleep(20);
        this.tool.MSG(95, "シタン", "I'm not going to let you\ndodge the question again...");
        this.shion2.start(1, "act11");
        this.space.start(1, "move");
        this.camerawork.cut11();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.632f, 0.496f, -0.595f);
        this.light.setColor(2, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(2, -0.92f, 0.0f, -0.392f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.519f, -0.001f, -0.855f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shion2.look_default();
        this.shitan_talk(true);
        this.tool.SoundstreamPlay(118023);
        this.tool.MSG(60, "シタン", "Hello?");
        this.fm1.start(1, "off_s");
        this.fm2.start(1, "off_s");
        this.tool.SoundstreamPlay(118024);
        this.tool.MSG(60, "シタン", "Hellooo...");
        this.shion2.start(1, "act12");
        this.space.start(1, "move");
        this.camerawork.cut12();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.727f, 0.554f, -0.405f);
        this.light.setColor(2, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(2, 0.009f, 0.341f, 0.94f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.574f, -0.627f, 0.527f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        System.sleep(80);
        this.tool.SoundstreamPlay(118025);
        this.tool.MSG(50, this.shion2.face, 7, 20, "Honestly...");
        this.shion2.start(1, "act13");
        this.space.start(1, "move");
        this.camerawork.cut13();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.589f, 0.522f, -0.617f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.224f, 0.242f, 0.944f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.517f, -0.643f, 0.565f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.tool.MSG(90, this.shion2.face, 7, "I wish he'd consider my feelings\nfor a change...");
        System.sleep(30);
        this.tool.Timechk_SceneEnd();
    }

    void shitan_talk(int n) {
        this.shitan_talk_count = n;
        this.shitan_talk_thread.start();
    }

    void shitan_talk(boolean bl) {
        if (bl) {
            this.fm1.setArgs(2, 0, 30, 0, -2);
            this.fm2.setArgs(2, 96, 30, 0, -2);
        } else {
            this.fm1.setArgs(2, 96, 30, 0, -2);
            this.fm2.setArgs(2, 0, 30, 0, -2);
        }
    }

    void shitan_talk_main() {
        this.shitan_talkmonitor(true);
        while (this.shitan_talk_count > 0) {
            System.sleep(1);
            --this.shitan_talk_count;
        }
        this.shitan_talkmonitor(false);
    }

    void shitan_talkmonitor(boolean bl) {
        if (bl) {
            System.println("talk on");
            this.shitan_talk_thread2.setTarget(this, "shitan_talkmonitor_main1");
            this.shitan_talk_thread2.start();
        } else {
            System.println("talk off");
            this.shitan_talk_thread2.setTarget(this, "shitan_talkmonitor_main2");
            this.shitan_talk_thread2.start();
        }
    }

    void shitan_talkmonitor_main1() {
        this.fm1.setArgs(2, 96, 30, 0, -2);
        this.fm2.setArgs(2, 0, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 80, 30, 0, -2);
        this.fm2.setArgs(2, 16, 30, 0, -2);
        this.fm1.setArgs(2, 64, 30, 0, -2);
        this.fm2.setArgs(2, 32, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 48, 30, 0, -2);
        this.fm2.setArgs(2, 48, 30, 0, -2);
        this.fm1.setArgs(2, 32, 30, 0, -2);
        this.fm2.setArgs(2, 64, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 16, 30, 0, -2);
        this.fm2.setArgs(2, 80, 30, 0, -2);
        this.fm1.setArgs(2, 0, 30, 0, -2);
        this.fm2.setArgs(2, 96, 30, 0, -2);
    }

    void shitan_talkmonitor_main2() {
        this.fm1.setArgs(2, 0, 30, 0, -2);
        this.fm2.setArgs(2, 96, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 16, 30, 0, -2);
        this.fm2.setArgs(2, 80, 30, 0, -2);
        this.fm1.setArgs(2, 32, 30, 0, -2);
        this.fm2.setArgs(2, 64, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 48, 30, 0, -2);
        this.fm2.setArgs(2, 48, 30, 0, -2);
        this.fm1.setArgs(2, 64, 30, 0, -2);
        this.fm2.setArgs(2, 32, 30, 0, -2);
        System.sleep(1);
        this.fm1.setArgs(2, 80, 30, 0, -2);
        this.fm2.setArgs(2, 16, 30, 0, -2);
        this.fm1.setArgs(2, 96, 30, 0, -2);
        this.fm2.setArgs(2, 0, 30, 0, -2);
    }

    class shion2
            extends Chr {
        Chr face;

        shion2() {
        }

        void act10() {
            this.setTranslate(0.07f, 0.0f, 3.48f);
            this.setRotate(0.0f, -97.0f, 0.0f);
            this.mtn(267, 0, 240, 8, 0, 1.0f, true);
        }

        void act11() {
            this.setTranslate(-0.1f, 0.0f, 3.66f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.mtn(269, 0, 120, 8, 8, 1.0f, true);
        }

        void act12() {
            this.setTranslate(0.596408f, 0.0f, 3.560342f);
            this.setRotate(0.0f, 172.0f, 0.0f);
            this.mtn(270, 50, 180, 0, 8, 1.0f, true);
        }

        void act13() {
            this.setTranslate(1.296404f, 0.1f, 2.060343f);
            this.setRotate(0.0f, 272.0f, 0.0f);
            this.mtn(271, 0, 240, 8, 8, 1.0f, true);
        }

        void act9() {
            this.setTranslate(-0.403595f, 0.0f, 3.760342f);
            this.setRotate(0.0f, -40.0f, 0.0f);
            this.mtn(266, 0, 230, 8, 8, 1.0f, true);
        }

        void init() {
            this.init(16777259, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
        }
    }

    class shion
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        shion() {
        }

        public void act1() {
            this.setTranslate(0.199892f, 0.007343f, -0.899991f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            float[] fArray = new float[]{1.0f, 0.199892f, 0.007343f, -1.899991f, 300.0f, 0.199892f, 0.007343f, 2.100009f};
            this.posSPL.setCtrlVertex(fArray, 0, 18, 300);
            this.move(this.posSPL, 0, false);
            this.mtn(257, 0, 64, 8, 8, 1.0f, true);
        }

        public void act2() {
            this.setTranslate(0.199892f, 0.007343f, 9.0E-6f);
            float[] fArray = new float[]{1.0f, 0.199892f, 0.007343f, 9.0E-6f, 350.0f, 0.199892f, 0.007343f, 3.500006f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 350);
            this.move(this.posSPL, 0, false);
            this.mtn(258, 0, 450, 0, 8, 1.2f, true);
        }

        public void act3() {
            this.setTranslate(0.33f, 0.01f, 3.3f);
            this.setRotate(0.0f, 50.0f, 0.0f);
            this.mtn(259, 0, 150, 8, 8, 1.0f, true);
        }

        public void act4() {
            this.setTranslate(0.87f, -0.03f, 3.33f);
            this.setRotate(0.0f, 65.0f, 0.0f);
            this.mtn(260, 0, 150, 0, 0, 1.0f, true);
        }

        public void act5() {
            this.setTranslate(0.17f, -0.03f, 3.33f);
            this.setRotate(0.0f, 365.0f, 0.0f);
            this.mtn(262, 0, 120, 8, 0, 1.0f, true);
        }

        public void act6() {
            this.setTranslate(0.399893f, 0.007343f, -1.199993f);
            this.setRotate(0.0f, 210.0f, 0.0f);
            this.mtn(263, 0, 522, 8, 8, 1.0f, true);
        }

        void act7() {
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setTranslate(0.21f, 0.01f, -1.47f);
            float[] fArray = new float[]{1.0f, 0.21f, 0.01f, -1.47f, 96.0f, 0.21f, 0.01f, -0.77f};
            this.posSPL.setCtrlVertex(fArray, 0, 0, 96);
            this.move(this.posSPL, 0, false);
            this.mtn(264, 0, 96, 0, 8, 1.0f, true);
        }

        public void act8() {
            this.mtn(265, 0, 180, 0, 8, 1.0f, true);
        }

        void init() {
            this.init(0x100001E, 10.5f, -1.0f, 71.98f, 180.0f);
            this.face = this.getChild(0x1000000);
            this.setTranslate(0.199892f, 0.007343f, -0.899991f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setShadow(5, 32);
        }
    }

    class con
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        con() {
        }

        void act3() {
            this.setVisible(true);
            this.setTranslate(0.33f, 0.01f, 3.3f);
            this.setRotate(0.0f, 50.0f, 0.0f);
            this.mtn(272, 0, 150, 8, 0, 1.0f, false);
        }

        void init() {
            this.init(24577, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setVisible(false);
            this.setShadow(0, 0);
        }

        void init2() {
        }
    }

    class hunger_on_hand
            extends Unit {
        hunger_on_hand() {
        }

        void init() {
            this.init(24591, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }

        void parent() {
            this.setTranslate(0.15f, -0.01f, 0.02f);
            this.setRotate(0.0f, -1.0f, 90.0f);
            this.setParent(SCE01018.this.shion, 72);
        }
    }

    class hunger
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        hunger() {
        }

        void act2() {
            this.setVisible(true);
            this.setTranslate(-0.0f, 1.4f, 3.99f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void act4() {
            this.setTranslate(0.87f, -0.03f, 3.33f);
            this.setRotate(0.0f, 65.0f, 0.0f);
            this.mtn(261, 0, 150, 0, 8, 1.0f, true);
            SCE01018.this.tool.mtnStop((Chr) this);
        }

        void act8() {
            this.setVisible(true);
            this.mtn(273, 0, 150, 8, 0, 1.0f, true);
        }

        void init() {
            this.init(24591, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setVisible(false);
            this.setShadow(0, 0);
        }
    }

    class costume
            extends Chr {
        costume() {
        }

        void act10() {
            this.setVisible(true);
            this.setTranslate(0.07f, 0.0f, 3.48f);
            this.setRotate(0.0f, -97.0f, 0.0f);
            this.mtn(268, 0, 240, 0, 0, 1.0f, true);
        }

        void init() {
            this.init(24590, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setVisible(false);
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

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE01018.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.720952f, 0.422554f, 0.479797f, 180.0f, -0.720952f, 0.422554f, 0.479797f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 5.023775f;
            fArray2[2] = -48.13594f;
            fArray2[4] = 180.0f;
            fArray2[5] = 5.023775f;
            fArray2[6] = -69.45553f;
            float[] fArray3 = fArray2;
            SCE01018.this.BaseCam.transSPL(fArray, 1);
            SCE01018.this.BaseCam.rotateSPL(fArray3, 1);
            SCE01018.this.BaseCam.setFov(40.0f);
            SCE01018.this.BaseCam.change();
            SCE01018.this.tool.STCameraPRO_on();
        }

        public void cut10() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(0.85f, 1.34f, 3.11f);
            SCE01018.this.BaseCam.setRotate(4.53f, -229.85f, 0.0f);
            SCE01018.this.BaseCam.setFov(28.16f);
        }

        public void cut11() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-0.269611f, 1.241466f, -3.741816f);
            SCE01018.this.BaseCam.setRotate(-13.169538f, -156.71541f, 0.0f);
        }

        public void cut12() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(0.326689f, 1.273401f, 2.963955f);
            SCE01018.this.BaseCam.setRotate(-16.029053f, -18.63173f, 0.0f);
        }

        public void cut13() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(0.2f, 1.27f, 2.72f);
            SCE01018.this.BaseCam.setRotate(-10.15f, 305.39f, 0.0f);
            SCE01018.this.BaseCam.setFov(28.16f);
        }

        public void cut14() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(1.869686f, 1.529393f, 1.727828f);
            SCE01018.this.BaseCam.setRotate(-33.188774f, 105.04716f, 0.0f);
            float[] fArray = new float[]{1.0f, 40.0f, 240.0f, 30.0f};
            SCE01018.this.BaseCam.fovSPL(fArray, 1);
        }

        public void cut15() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setFov(40.0f);
            SCE01018.this.BaseCam.setTranslate(0.689787f, 1.017374f, 1.689546f);
            SCE01018.this.BaseCam.setRotate(9.711442f, 239.14363f, 0.0f);
        }

        public void cut16() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(0.705397f, 0.953369f, 3.676118f);
            SCE01018.this.BaseCam.setRotate(-21.54833f, 319.83698f, 0.0f);
        }

        public void cut17() {
            SCE01018.this.tool.Timechk_CutChange();
            System.sleep(10);
            float[] fArray = new float[]{1.0f, 1.414088f, 1.721323f, 2.694211f, 300.0f, 1.414088f, 1.721323f, 2.694211f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -82.186386f;
            fArray2[2] = 216.34628f;
            fArray2[4] = 300.0f;
            fArray2[5] = -82.186386f;
            fArray2[6] = 203.41077f;
            float[] fArray3 = fArray2;
            SCE01018.this.BaseCam.transSPL(fArray, 0);
            SCE01018.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut2() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-0.153919f, 1.15854f, -0.283619f);
            SCE01018.this.BaseCam.setRotate(-4.776113f, -170.65414f, 0.0f);
            SCE01018.this.tool.STCamera_on();
            System.sleep(30);
            float[] fArray = new float[]{1.0f, -0.153919f, 1.15854f, -0.283619f, 320.0f, 0.145029f, 1.446535f, 2.277081f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.776113f;
            fArray2[2] = -170.65414f;
            fArray2[4] = 320.0f;
            fArray2[5] = -3.537875f;
            fArray2[6] = -179.1933f;
            float[] fArray3 = fArray2;
            SCE01018.this.BaseCam.transSPL(fArray, 1);
            SCE01018.this.BaseCam.rotateSPL(fArray3, 1);
        }

        public void cut3() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.tool.STCamera_off();
            SCE01018.this.BaseCam.setTranslate(1.302765f, 0.734076f, 3.004682f);
            SCE01018.this.BaseCam.setRotate(-22.85165f, -213.61183f, 0.0f);
        }

        public void cut4() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-0.76f, 1.31f, 2.85f);
            SCE01018.this.BaseCam.setRotate(2.73f, -126.07f, 0.0f);
            SCE01018.this.BaseCam.setFov(24.64f);
        }

        public void cut5() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-0.33f, 1.45f, 4.06f);
            SCE01018.this.BaseCam.setRotate(-9.41f, -24.07f, 0.0f);
            SCE01018.this.BaseCam.setFov(34.56f);
        }

        public void cut6() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-0.131542f, 0.97021f, -3.25947f);
            SCE01018.this.BaseCam.setRotate(-5.409885f, -153.60968f, 0.0f);
            SCE01018.this.BaseCam.setFov(34.56f);
        }

        public void cut7() {
            SCE01018.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -2.620146f, 1.322109f, -4.061334f, 300.0f, -2.432779f, 1.322109f, -3.905404f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.909079f;
            fArray2[2] = -134.23495f;
            fArray2[4] = 300.0f;
            fArray2[5] = -7.909077f;
            fArray2[6] = -134.23492f;
            float[] fArray3 = fArray2;
            SCE01018.this.BaseCam.transSPL(fArray, 1);
            SCE01018.this.BaseCam.rotateSPL(fArray3, 1);
            SCE01018.this.BaseCam.setFov(34.56f);
        }

        public void cut8() {
            SCE01018.this.tool.Timechk_CutChange();
            SCE01018.this.BaseCam.setTranslate(-2.02f, 1.36f, 3.71f);
            SCE01018.this.BaseCam.setRotate(-4.87f, -79.86f, 0.0f);
            SCE01018.this.BaseCam.setFov(24.0f);
        }

        public void cut9() {
            SCE01018.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 2.271735f, 0.313496f, -4.142205f, 600.0f, 2.874538f, 0.313496f, -3.886714f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.950413f;
            fArray2[2] = -211.93736f;
            fArray2[4] = 600.0f;
            fArray2[5] = 6.950413f;
            fArray2[6] = -211.93736f;
            float[] fArray3 = fArray2;
            SCE01018.this.BaseCam.transSPL(fArray, 0);
            SCE01018.this.BaseCam.rotateSPL(fArray3, 0);
            SCE01018.this.BaseCam.setFov(34.56f);
        }
    }

    class Space
            extends Unit {
        public Space(int n) {
            this.init(n);
        }

        void move() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz + 0.05f);
                System.sleep(1);
            }
        }

        void move2() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz + 0.05f);
                this.setRotate(this.rx, this.ry - 0.02f, this.rz);
                System.sleep(1);
            }
        }
    }
}

