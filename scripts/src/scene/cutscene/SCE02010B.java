import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_PRO08_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02010B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02010B,
        MC_PRO08_PRJ,
        JNT_Human,
        FLSyuri,
        FLScommi,
        FLScommi1,
        FLScommi2,
        FLScommi3,
        FLScommi4,
        FLScommi5 {
    STool tool = new STool();
    static final float yuri_x = -2.82f;
    static final float yuri_y = 0.0f;
    static final float yuri_z = -2.82f;
    static final float com_a_x = 2.82f;
    static final float com_a_y = 0.0f;
    static final float com_a_z = -2.82f;
    static final float com_b_x = 2.82f;
    static final float com_b_y = 0.0f;
    static final float com_b_z = 2.82f;
    static final float com_c_x = -2.82f;
    static final float com_c_y = 0.0f;
    static final float com_c_z = 2.82f;
    static final float com_d_x = 0.0f;
    static final float com_d_y = 0.0f;
    static final float com_d_z = 4.0f;
    static final float com_e_x = -4.0f;
    static final float com_e_y = 0.0f;
    static final float com_e_z = 0.0f;
    static final float com_f_x = 4.0f;
    static final float com_f_y = 0.0f;
    static final float com_f_z = 0.0f;
    Camera cam0;
    Camera cam1;
    Camera chrcam1;
    Camera chrcam2;
    Camera chrcam3;
    Camera BaseCam;
    Window win;
    boolean snd_chk = false;
    Light light = new Light(0);
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    static final int Chand_R = 72;
    static final int Chand_L = 60;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    int BGinit = 0;
    Unit Stick;
    Monitor Mon;
    Monitor Mon_A;
    Monitor Mon_B;
    Monitor Mon_C;
    Monitor Mon_D;
    Monitor Mon_E;
    Monitor Mon_F;
    Monitor Mon_Y;
    Monitor Mon2;
    Monitor Mon_A2;
    Monitor Mon_B2;
    Monitor Mon_C2;
    Monitor Mon_D2;
    Monitor Mon_E2;
    Monitor Mon_F2;
    Monitor Mon_Y2;
    Yuri yuri;
    Com_a com_a;
    Com_b com_b;
    Com_c com_c;
    Com_d com_d;
    Com_e com_e;
    Com_f com_f;
    Units mobj001 = new Units(20482);
    Units mobj134 = new Units(20615);
    Effect whiteOut;
    Thread CSEffect = Thread.create();
    int FadeTime;
    int[] ColorScreenParam;
    Input Xpad1P;
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag;

    SCE02010B() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 1614815232;
        this.ColorScreenParam = nArray;
        this.Xpad1P = Input.create(0);
        this.Xenvmainthreadendflag = false;
    }

    void CSFadeIn() {
        this.CSFadeIn(60);
    }

    void CSFadeIn(int n) {
        this.FadeTime = n;
        this.CSEffect.setTarget(this, "CSFadeIn_srv");
        this.CSEffect.start();
    }

    void CSFadeIn_srv() {
        int n = 255;
        int n2 = 0;
        int n3 = 0;
        int n4 = 0;
        int n5 = 255 / this.FadeTime;
        int n6 = 0;
        while (n6 != this.FadeTime) {
            if ((n -= n5) < 0) {
                n = 0;
            }
            this.ColorScreenParam[2] = 0x100000;
            this.ColorScreenParam[3] = n * 0x1000000 + n2 * 65536 + n3 * 256 + n4;
            Runtime.setDefocus(15, 1, this.ColorScreenParam);
            ++n6;
            System.sleep(1);
        }
        this.ColorScreenParam[2] = 0x100000;
        this.ColorScreenParam[3] = n2 * 65536 + n3 * 256 + n4;
        Runtime.setDefocus(15, 1, this.ColorScreenParam);
    }

    void CSFadeOut() {
        this.CSFadeOut(60);
    }

    void CSFadeOut(int n) {
        this.FadeTime = n;
        this.CSEffect.setTarget(this, "CSFadeOut_srv");
        this.CSEffect.start();
    }

    void CSFadeOut_srv() {
        int n = 0;
        int n2 = 0;
        int n3 = 0;
        int n4 = 0;
        int n5 = 255 / this.FadeTime;
        int n6 = 0;
        while (n6 != this.FadeTime) {
            if ((n += n5) >= 255) {
                n = 255;
            }
            this.ColorScreenParam[2] = 0x100000;
            this.ColorScreenParam[3] = n * 0x1000000 + n2 * 65536 + n3 * 256 + n4;
            Runtime.setDefocus(15, 1, this.ColorScreenParam);
            ++n6;
            System.sleep(1);
        }
        this.ColorScreenParam[2] = 0x100000;
        this.ColorScreenParam[3] = -16777216 + n2 * 65536 + n3 * 256 + n4;
        Runtime.setDefocus(15, 1, this.ColorScreenParam);
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
        System.println("XEVEFLAG:EV02010B_F");
        Runtime.setFlags(117, 1, 1);
        System.println("XEVEJNAME:CFJ2_100 XEVEJPOINT:POINT2_100");
        Runtime.jumpCF(570, 2);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(540, 2);
    }

    void init() {
        System.methodSignal(1);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.BaseCam = Camera.create(1);
        this.yuri = new Yuri();
        this.yuri.init(0x100011D, 0.0f, 0.0f, 0.0f, 180.0f);
        this.yuri.face = this.yuri.getChild(0x1000000);
        this.yuri.setTranslate(-2.82f, 0.0f, -2.82f);
        this.yuri.setRotate(0.0f, 45.0f, 0.0f);
        this.yuri.setShadow(0, 0);
        this.yuri.setVisible(true);
        this.com_a = new Com_a();
        this.com_a.init(0x1000303, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_a.face = this.com_a.getChild(0x1000000);
        this.com_a.setTranslate(2.82f, 0.0f, -2.82f);
        this.com_a.setRotate(0.0f, -45.0f, 0.0f);
        this.com_a.setShadow(0, 0);
        this.com_a.setVisible(true);
        this.com_b = new Com_b();
        this.com_b.init(16777989, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_b.face = this.com_b.getChild(0x1000000);
        this.com_b.setTranslate(2.82f, 0.0f, 2.82f);
        this.com_b.setRotate(0.0f, -135.0f, 0.0f);
        this.com_b.setShadow(0, 0);
        this.com_b.setVisible(true);
        this.com_c = new Com_c();
        this.com_c.init(16777990, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_c.face = this.com_c.getChild(0x1000000);
        this.com_c.setTranslate(-2.82f, 0.0f, 2.82f);
        this.com_c.setRotate(0.0f, 135.0f, 0.0f);
        this.com_c.setShadow(0, 0);
        this.com_c.setVisible(true);
        this.com_d = new Com_d();
        this.com_d.init(0x1000301, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_d.face = this.com_d.getChild(0x1000000);
        this.com_d.setTranslate(0.0f, 0.0f, 4.0f);
        this.com_d.setRotate(0.0f, 180.0f, 0.0f);
        this.com_d.setShadow(0, 0);
        this.com_d.setVisible(true);
        this.com_e = new Com_e();
        this.com_e.init(16777986, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_e.face = this.com_e.getChild(0x1000000);
        this.com_e.setTranslate(-4.0f, 0.0f, 0.0f);
        this.com_e.setRotate(0.0f, 90.0f, 0.0f);
        this.com_e.setShadow(0, 0);
        this.com_e.setVisible(true);
        this.com_f = new Com_f();
        this.com_f.init(16777988, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_f.face = this.com_f.getChild(0x1000000);
        this.com_f.setTranslate(4.0f, 0.0f, 0.0f);
        this.com_f.setRotate(0.0f, -90.0f, 0.0f);
        this.com_f.setShadow(0, 0);
        this.com_f.setVisible(true);
        this.Mon = new Monitor();
        this.Mon.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon.setArgs(1, 20067, 0, 0, 0);
        this.Mon.setArgs(2, 100, 0, 0, -1);
        this.Mon.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon.setTranslate(0.0f, 3.37f, -8.38f);
        this.Mon.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon_A = new Monitor();
        this.Mon_A.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_A.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_A.setArgs(1, 20040, 0, 0, 0);
        this.Mon_A.setArgs(2, 96, 0, 0, -1);
        this.Mon_A.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_A.setTranslate(2.4099998f, 0.9f, -2.37f);
        this.Mon_A.setRotate(0.0f, -45.0f, 0.0f);
        this.Mon_B = new Monitor();
        this.Mon_B.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_B.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_B.setArgs(1, 20040, 0, 0, 0);
        this.Mon_B.setArgs(2, 96, 0, 0, -1);
        this.Mon_B.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_B.setTranslate(2.32f, 0.9f, 2.37f);
        this.Mon_B.setRotate(0.0f, -135.0f, 0.0f);
        this.Mon_C = new Monitor();
        this.Mon_C.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_C.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_C.setArgs(1, 20040, 0, 0, 0);
        this.Mon_C.setArgs(2, 96, 0, 0, -1);
        this.Mon_C.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_C.setTranslate(-2.4199998f, 0.9f, 2.37f);
        this.Mon_C.setRotate(0.0f, 135.0f, 0.0f);
        this.Mon_D = new Monitor();
        this.Mon_D.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_D.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_D.setArgs(1, 20040, 0, 0, 0);
        this.Mon_D.setArgs(2, 96, 0, 0, -1);
        this.Mon_D.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_D.setTranslate(0.0f, 0.9f, 3.55f);
        this.Mon_D.setRotate(0.0f, 180.0f, 0.0f);
        this.Mon_E = new Monitor();
        this.Mon_E.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_E.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_E.setArgs(1, 20040, 0, 0, 0);
        this.Mon_E.setArgs(2, 96, 0, 0, -1);
        this.Mon_E.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_E.setTranslate(-3.4f, 0.9f, 0.0f);
        this.Mon_E.setRotate(0.0f, 90.0f, 0.0f);
        this.Mon_F = new Monitor();
        this.Mon_F.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_F.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_F.setArgs(1, 20040, 0, 0, 0);
        this.Mon_F.setArgs(2, 96, 0, 0, -1);
        this.Mon_F.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_F.setTranslate(3.4f, 0.9f, 0.0f);
        this.Mon_F.setRotate(0.0f, -90.0f, 0.0f);
        this.Mon_Y = new Monitor();
        this.Mon_Y.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_Y.setArgs(1, 20040, 0, 0, 0);
        this.Mon_Y.setArgs(2, 96, 0, 0, -1);
        this.Mon_Y.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y.setTranslate(-2.54f, 0.9f, -2.55f);
        this.Mon_Y.setRotate(0.0f, 45.0f, 0.0f);
        this.Mon2 = new Monitor();
        this.Mon2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon2.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon2.setArgs(1, 20110, 0, 0, 0);
        this.Mon2.setArgs(2, 100, 0, 0, -1);
        this.Mon2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon2.setTranslate(0.0f, 3.37f, -8.38f);
        this.Mon2.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon_A2 = new Monitor();
        this.Mon_A2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_A2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_A2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_A2.setArgs(2, 96, 0, 0, -1);
        this.Mon_A2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_A2.setTranslate(2.4099998f, 0.9f, -2.37f);
        this.Mon_A2.setRotate(0.0f, -45.0f, 0.0f);
        this.Mon_B2 = new Monitor();
        this.Mon_B2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_B2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_B2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_B2.setArgs(2, 96, 0, 0, -1);
        this.Mon_B2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_B2.setTranslate(2.32f, 0.9f, 2.37f);
        this.Mon_B2.setRotate(0.0f, -135.0f, 0.0f);
        this.Mon_C2 = new Monitor();
        this.Mon_C2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_C2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_C2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_C2.setArgs(2, 96, 0, 0, -1);
        this.Mon_C2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_C2.setTranslate(-2.4199998f, 0.9f, 2.37f);
        this.Mon_C2.setRotate(0.0f, 135.0f, 0.0f);
        this.Mon_D2 = new Monitor();
        this.Mon_D2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_D2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_D2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_D2.setArgs(2, 96, 0, 0, -1);
        this.Mon_D2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_D2.setTranslate(0.0f, 0.9f, 3.55f);
        this.Mon_D2.setRotate(0.0f, 180.0f, 0.0f);
        this.Mon_E2 = new Monitor();
        this.Mon_E2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_E2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_E2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_E2.setArgs(2, 96, 0, 0, -1);
        this.Mon_E2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_E2.setTranslate(-3.4f, 0.9f, 0.0f);
        this.Mon_E2.setRotate(0.0f, 90.0f, 0.0f);
        this.Mon_F2 = new Monitor();
        this.Mon_F2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_F2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_F2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_F2.setArgs(2, 96, 0, 0, -1);
        this.Mon_F2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_F2.setTranslate(3.4f, 0.9f, 0.0f);
        this.Mon_F2.setRotate(0.0f, -90.0f, 0.0f);
        this.Mon_Y2 = new Monitor();
        this.Mon_Y2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y2.setArgs(0, 0.0f, 0.0f, 0.5f, 0.33f);
        this.Mon_Y2.setArgs(1, 27003, 0, 0, 0);
        this.Mon_Y2.setArgs(2, 96, 0, 0, -1);
        this.Mon_Y2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y2.setTranslate(-2.54f, 0.9f, -2.55f);
        this.Mon_Y2.setRotate(0.0f, 45.0f, 0.0f);
        this.Stick = new Unit();
        this.Stick.init(24634, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Stick.setTranslate(0.09f, 0.01f, 0.03f);
        this.Stick.setRotate(269.16f, -30.87f, 265.96f);
        this.Stick.setScale(1.0f, 1.0f, 1.0f);
        this.Stick.setVisible(true);
        this.Stick.setParent(this.com_b, 72);
        this.mobj001.setVisible(false);
        this.mobj134.setVisible(false);
        this.mobj001.setScale(10.0f, 10.0f, 10.0f);
    }

    void initialize() {
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void map_set(boolean bl) {
        int n = 0;
        while (n != 100) {
            Stage.setVisible(n, bl);
            ++n;
        }
    }

    void monitor_set() {
    }

    void play() {
        this.loadarc(this.yuri.face, "FLSyuri.fpk");
        this.loadarc(this.com_a.face, "FLScommi2.fpk");
        this.loadarc(this.com_b.face, "FLScommi4.fpk");
        this.loadarc(this.com_c.face, "FLScommi5.fpk");
        this.loadarc(this.com_d.face, "FLScommi.fpk");
        this.loadarc(this.com_e.face, "FLScommi1.fpk");
        this.loadarc(this.com_f.face, "FLScommi3.fpk");
        Runtime.setLocation(1107);
        Stage.setVisible(26, false);
        Stage.setVisible(84, false);
        Stage.setVisible(66, false);
        this.Mon.signal(1);
        this.Mon_A.signal(1);
        this.Mon_B.signal(1);
        this.Mon_C.signal(1);
        this.Mon_D.signal(1);
        this.Mon_E.signal(1);
        this.Mon_F.signal(1);
        this.Mon_Y.signal(1);
        this.BaseCam.setTranslate(-3.59f, 1.55f, 2.68f);
        this.BaseCam.setRotate(-9.92f, 309.61f, 0.0f);
        this.BaseCam.setFov(25.0f);
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.0f, 0.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 1.0f, 0.0f, -0.363f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -1.0f, 0.0f, -0.341f);
        Sound.streamPlay(1290055, 48000);
        System.println("CUT1 START");
        this.yuri.start(1, "sit");
        this.com_a.start(1, "sit");
        this.com_b.start(1, "sit");
        this.com_c.start(1, "sit");
        this.com_d.start(1, "sit");
        this.com_e.start(1, "sit");
        this.com_f.start(1, "sit");
        this.camerawork.cut1();
        System.sleep(150);
        if (this.snd_chk) {
            Sound.streamPlay(210101);
        }
        this.tool.MSG(53, this.com_a.face, 1, 53, "I've received word that");
        System.println("CUT1 END");
        System.println("CUT2 START");
        this.camerawork.cut2();
        this.com_a.start(1, "act2");
        if (this.snd_chk) {
            Sound.streamPlay(210901);
        }
        this.tool.MSG(90, this.com_a.face, 1, 89, "the 100-Series is onboard a ship\n headed for Second Miltia.");
        System.sleep(15);
        System.println("CUT2 END");
        this.com_b.start(1, "act3");
        this.com_d.start(1, "act1");
        this.com_f.start(1, "act1");
        this.camerawork.cut3();
        System.sleep(30);
        if (this.snd_chk) {
            Sound.streamPlay(210102);
        }
        this.tool.MSG(80, this.com_b.face, 1, 79, "Well, that was fast.");
        System.sleep(10);
        this.tool._MSG(159, "委員Ｂ", "Once she's safely transferred\nto the U.M.N. Control Center\non Second Miltia,");
        if (this.snd_chk) {
            Sound.streamPlay(210103);
        }
        this.tool.FACE(159, this.com_b.face, 1);
        this.tool._MSG(72, "委員Ｂ", "we'll finally have a little peace of mind.");
        this.tool.FACE(72, this.com_b.face, 1);
        System.sleep(6);
        System.println("CUT5 START");
        this.com_c.start(1, "act5");
        this.camerawork.cut5();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210104);
        }
        this.tool.MSG(60, this.com_c.face, 1, 52, "It's all such a bother, though.");
        if (this.snd_chk) {
            Sound.streamPlay(210105);
        }
        this.tool.MSG(105, this.com_c.face, 1, 95, "It would've been so much easier\nto perform the analysis here...");
        System.println("CUT5 END");
        System.println("CUT6 START");
        this.com_c.start(1, "act6");
        this.com_d.start(1, "act6");
        this.camerawork.cut6();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210106);
        }
        this.tool._MSG(98, "委員Ｄ", "We can't decode the protection\nin that Realian here.");
        this.tool.FACE(98, this.com_d.face, 1);
        this.tool._MSG(97, "委員Ｄ", "We have to send her to the\nU.M.N. Control Center,");
        this.tool.FACE(97, this.com_d.face, 1);
        this.tool.MSG(105, this.com_d.face, 1, 104, "located at the former Vector\ntransfer gate control facility.");
        System.sleep(15);
        System.println("CUT6 END");
        System.println("CUT7 START");
        this.com_c.start(1, "act7");
        this.com_e.start(1, "act7");
        this.camerawork.cut7();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210108);
        }
        this.tool.MSG(30, this.com_e.face, 1, 30, "That's right.");
        if (this.snd_chk) {
            Sound.streamPlay(210109);
        }
        this.tool._MSG(98, "委員Ｅ", "Besides, the entire area within a\nfew dozen light years of Old Miltia");
        this.tool.FACE(98, this.com_e.face, 1);
        this.tool.MSG(89, this.com_e.face, 1, 89, "has been sealed off ever since\nthat incident 14 years ago.");
        System.sleep(8);
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210110);
        }
        this.tool.MSG(120, this.com_e.face, 1, 114, "So this certainly beats traveling\nthrough regular space.");
        System.sleep(15);
        System.println("CUT7 END");
        System.println("CUT8 START");
        this.light.setDirection2(1, -0.178f, 0.885f, 0.43f);
        this.com_a.start(1, "act8");
        this.com_f.start(1, "act8");
        this.yuri.look_point(-2.54f, 0.9f, -2.55f);
        this.camerawork.cut8();
        System.sleep(30);
        if (this.snd_chk) {
            Sound.streamPlay(210111);
        }
        this.tool.MSG(58, this.com_f.face, 1, 58, "Joachim Mizrahi...");
        if (this.snd_chk) {
            Sound.streamPlay(210112);
        }
        this.tool.iMSG(75, this.com_f.face, 1, 75, "You really outdid yourself this time...");
        System.sleep(30);
        System.println("CUT8 END");
        System.println("CUT9 START");
        this.yuri.start(1, "sit");
        this.camerawork.cut9();
        System.sleep(60);
        System.println("CUT9 END");
        System.println("CUT10 START");
        System.sleep(120);
        System.println("CUT10 END");
        System.println("CUT11 START");
        this.yuri.look_default();
        this.yuri.start(1, "act11");
        this.camerawork.cut11();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210114);
        }
        this.tool._MSG(53, "ユリ", "Oh, don't worry about it.");
        this.tool.FACE(53, this.yuri.face, 1);
        this.tool.iMSG(30, this.yuri.face, 1, 29, "You're absolutely right.");
        System.sleep(22);
        System.println("CUT11 END");
        System.println("CUT12 START");
        this.light.setDirection2(1, -0.259f, 0.947f, -0.19f);
        this.com_b.start(1, "act12");
        this.com_f.start(1, "act1");
        this.camerawork.cut12();
        System.sleep(10);
        if (this.snd_chk) {
            Sound.streamPlay(210116);
        }
        this.tool._MSG(81, "委員Ｂ", "By the way, do you really\nthink the Y Data is");
        this.tool.FACE(81, this.com_b.face, 1);
        this.tool.iMSG(90, this.com_b.face, 1, 68, "hidden within that Realian?");
        System.sleep(14);
        System.println("CUT12 END");
        System.println("CUT13 START");
        this.light.setDirection2(1, 0.537f, 0.826f, 0.17f);
        this.yuri.start(1, "act13");
        this.com_b.start(1, "act12");
        this.com_e.start(1, "act1");
        this.camerawork.cut13();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210119);
        }
        this.tool.MSG(210, this.yuri.face, 1, 206, "We have conclusive evidence that she\nholds the code to unlock the U.M.N. transfer\ngate leading to the sealed-off sector.");
        if (this.snd_chk) {
            Sound.streamPlay(210120);
        }
        if (this.snd_chk) {
            Sound.streamPlay(210121);
        }
        this.tool.MSG(90, this.yuri.face, 1, 75, "Aside from that, she's pretty\nmuch a mystery.");
        if (this.snd_chk) {
            Sound.streamPlay(210121);
        }
        this.tool.MSG(60, this.yuri.face, 1, 41, "We'll just have to open her up and see...");
        System.sleep(30);
        System.println("CUT13 END");
        System.println("CUT14 START");
        this.light.setDirection2(1, 0.384f, 0.891f, -0.244f);
        this.com_d.start(1, "act14");
        this.com_c.start(1, "act7");
        this.camerawork.cut14();
        if (this.snd_chk) {
            Sound.streamPlay(210122);
        }
        this.tool.MSG(112, this.com_d.face, 1, 111, "If she doesn't have it, we'll be required\nto delay the Zohar Project.");
        if (this.snd_chk) {
            Sound.streamPlay(210123);
        }
        this.tool.MSG(68, this.com_d.face, 1, 67, "How will we explain that\nto the council...");
        System.sleep(30);
        System.println("CUT14 END");
        System.println("CUT15 START");
        this.light.setDirection2(1, -0.48f, 0.639f, 0.6f);
        this.yuri.start(1, "act15");
        this.camerawork.cut15();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210124);
        }
        this.tool.MSG(120, this.yuri.face, 1, 118, "Even if it's lost,\nwe still have one other lead.");
        if (this.snd_chk) {
            Sound.streamPlay(210125);
        }
        this.tool.MSG(45, this.yuri.face, 1, 44, "Let me pull up my documents.");
        System.sleep(30);
        System.println("CUT15 END");
        this.camerawork.cut16();
        System.println("CUT16 START");
        System.sleep(50);
        this.Mon_Y.signal(0);
        this.Mon_Y2.signal(1);
        System.sleep(40);
        System.println("CUT16 END");
        System.println("CUT17 START");
        this.Mon_Y.setTranslate(-2.3f, 0.9f, -2.2f);
        this.Mon_Y.setRotate(0.0f, 45.0f, 0.0f);
        this.Mon_Y2.setTranslate(-2.3f, 0.9f, -2.2f);
        this.Mon_Y2.setRotate(0.0f, 45.0f, 0.0f);
        this.yuri.start(1, "act17");
        this.com_a.start(1, "act1");
        this.com_b.start(1, "act1");
        this.com_c.start(1, "act1");
        this.com_d.start(1, "act1");
        this.com_e.start(1, "act1");
        this.com_f.start(1, "act1");
        this.Mon.signal(0);
        this.Mon_A.signal(0);
        this.Mon_B.signal(0);
        this.Mon_C.signal(0);
        this.Mon_D.signal(0);
        this.Mon_E.signal(0);
        this.Mon_F.signal(0);
        this.Mon_Y.signal(0);
        this.Mon.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_A.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_B.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_C.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_D.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_E.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_F.setScale(0.0f, 0.0f, 0.0f);
        this.Mon_Y.setScale(0.0f, 0.0f, 0.0f);
        this.Mon2.signal(1);
        this.Mon_A2.signal(1);
        this.Mon_B2.signal(1);
        this.Mon_C2.signal(1);
        this.Mon_D2.signal(1);
        this.Mon_E2.signal(1);
        this.Mon_F2.signal(1);
        this.Mon_Y2.signal(1);
        this.Mon2.setScale(0.75f, 1.0f, 1.0f);
        this.yuri.setShadow(5, 16);
        this.camerawork.cut17();
        System.sleep(105);
        if (this.snd_chk) {
            Sound.streamPlay(210126);
        }
        this.tool.MSG(142, this.yuri.face, 1, 141, "This image was processed from the\nonly remaining surveillance camera.");
        System.sleep(23);
        System.println("CUT17 END");
        System.println("CUT18 START");
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210127);
        }
        this.tool.MSG(120, this.yuri.face, 1, 119, "The picture's unclear, but that\nboy served as Joachim's assistant.");
        if (this.snd_chk) {
            Sound.streamPlay(210128);
        }
        this.tool.MSG(172, this.yuri.face, 1, 171, "Judging from the time frame,\nwe believe he was somehow\ninvolved with the Y Data.");
        System.sleep(23);
        System.println("CUT18 END");
        System.println("CUT19 START");
        this.light.setDirection2(1, -0.014f, 0.934f, -0.356f);
        this.yuri.start(1, "act19");
        this.com_c.start(1, "act19");
        this.com_d.start(1, "act19");
        this.camerawork.cut19();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210129);
        }
        this.tool.MSG(51, this.com_c.face, 1, 51, "Where did the boy come from?");
        System.sleep(11);
        if (this.snd_chk) {
            Sound.streamPlay(210130);
        }
        this.tool.MSG(64, this.yuri.face, 1, 63, "The records are missing.");
        System.println("CUT19 END");
        System.println("CUT20 START");
        this.light.setDirection2(1, -0.212f, 0.869f, 0.448f);
        this.yuri.start(1, "act20");
        this.com_a.start(1, "act20");
        this.camerawork.cut20();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210131);
        }
        this.tool.MSG(150, this.yuri.face, 1, 140, "The only information we have is\nthat he was 14 and studying at\nthe University of Bormeo.");
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210132);
        }
        this.tool.MSG(75, this.yuri.face, 1, 58, "But even that's just second-hand\ninformation from Mizrahi...");
        System.sleep(15);
        System.println("CUT20 END");
        System.println("CUT21 START");
        this.light.setDirection2(1, 0.487f, 0.829f, -0.274f);
        this.com_e.start(1, "act21");
        this.com_c.start(1, "act21");
        this.camerawork.cut21();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210133);
        }
        this.tool.MSG(112, this.com_e.face, 1, 111, "He got into Bormeo at 14?\nMust've been pretty smart.");
        System.sleep(8);
        if (this.snd_chk) {
            Sound.streamPlay(210134);
        }
        this.tool.MSG(51, this.com_c.face, 1, 50, "You don't know what\nhappened to him?");
        System.sleep(9);
        System.println("CUT21 END");
        System.println("CUT22 START");
        this.light.setDirection2(1, -0.379f, 0.679f, 0.628f);
        this.yuri.start(1, "act22");
        this.camerawork.cut22();
        System.sleep(15);
        this.tool._MSG(60, "ユリ", "Correct. It's currently\nunder investigation.");
        if (this.snd_chk) {
            Sound.streamPlay(210135);
        }
        this.tool.FACE(18, this.yuri.face, 1);
        if (this.snd_chk) {
            Sound.streamPlay(210136);
        }
        this.tool.FACE(45, this.yuri.face, 1);
        System.sleep(12);
        System.println("CUT22 END");
        System.println("CUT23 START");
        this.light.setDirection2(1, -0.463f, 0.872f, -0.157f);
        this.com_a.start(1, "act23");
        this.com_f.start(1, "act23");
        this.camerawork.cut23();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210137);
        }
        this.tool.MSG(83, this.com_f.face, 1, 82, "If he's alive, he'd be 28 years old now...");
        System.sleep(7);
        if (this.snd_chk) {
            Sound.streamPlay(210138);
        }
        this.tool.MSG(150, this.com_a.face, 1, 115, "This research paper is on\nthe secondary effects\nof the Hilbert wave...");
        System.println("CUT23 END");
        System.println("CUT24 START");
        this.com_a.start(1, "act1");
        this.com_b.start(1, "act1");
        this.com_c.start(1, "act24");
        this.com_d.start(1, "act1");
        this.com_e.start(1, "act1");
        this.com_f.start(1, "act1");
        this.camerawork.cut24();
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210139);
        }
        this.tool.MSG(45, this.com_c.face, 1, 36, "The Hilbert Effect...");
        this.tool.iMSG(105, this.com_c.face, 1, 102, "Do you think there's a chance\nhe slipped into Vector?");
        System.println("CUT24 END");
        System.println("CUT25 START");
        this.light.setDirection2(1, -0.183f, 0.598f, 0.781f);
        this.yuri.start(1, "act25");
        this.com_a.start(1, "act25");
        this.camerawork.cut25();
        System.sleep(30);
        if (this.snd_chk) {
            Sound.streamPlay(210141);
        }
        this.tool.MSG(60, this.yuri.face, 1, 60, "Back when U-TIC was still");
        this.tool.MSG(135, this.yuri.face, 1, 135, "the Mizrahi Cerebral Sciences\nResearch Center, Mizrahi's\nmain patron was");
        this.tool.MSG(60, this.yuri.face, 1, 58, "none other than Vector.");
        System.println("CUT25 END");
        System.println("CUT26 START");
        this.yuri.start(1, "act26");
        if (this.snd_chk) {
            Sound.streamPlay(210142);
        }
        this.tool.MSG(60, this.yuri.face, 1, 57, "It's certainly a possibility.");
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210143);
        }
        this.tool.MSG(75, this.yuri.face, 1, 46, "There's no concrete evidence,\nof course...");
        System.sleep(15);
        System.println("CUT26 END");
        System.println("CUT27 START");
        this.com_a.start(1, "act27");
        this.camerawork.cut27();
        if (this.snd_chk) {
            Sound.streamPlay(210144);
        }
        this.tool.MSG(38, this.com_a.face, 1, 38, "Dr. Mizrahi...");
        if (this.snd_chk) {
            Sound.streamPlay(210145);
        }
        this.tool.MSG(78, this.com_a.face, 1, 78, "Please continue your\ninvestigation of this issue.");
        System.sleep(15);
        if (this.snd_chk) {
            Sound.streamPlay(210146);
        }
        this.tool.MSG(45, this.yuri.face, 1, 26, "Yes, of course.");
        System.sleep(15);
        System.println("CUT27 END");
        System.println("CUT28 START");
        this.light.setDirection2(1, 0.326f, 0.0f, 0.946f);
        this.com_b.start(1, "act28");
        this.camerawork.cut28();
        if (this.snd_chk) {
            Sound.streamPlay(210147);
        }
        this.tool.MSG(190, this.com_b.face, 1, 189, "In any case, our first priority is to\nget access to the Old Miltia sector.");
        System.println("CUT28 END");
        System.println("CUT29 START");
        this.com_a.start(1, "act1");
        this.com_b.start(1, "act1");
        this.com_c.start(1, "act1");
        this.com_d.start(1, "act1");
        this.com_e.start(1, "act1");
        this.com_f.start(1, "act1");
        this.camerawork.cut29();
        if (this.snd_chk) {
            Sound.streamPlay(210148);
        }
        this.tool.MSG(192, this.com_b.face, 1, 191, "Our hands are tied until we recover\nthe Original Zohar located there.");
        System.sleep(18);
        System.println("CUT29 END");
        this.CSFadeOut();
        System.sleep(30);
        this.map_set(false);
        this.yuri.setVisible(false);
        this.com_a.setVisible(false);
        this.com_b.setVisible(false);
        this.com_c.setVisible(false);
        this.com_d.setVisible(false);
        this.com_e.setVisible(false);
        this.com_f.setVisible(false);
        this.Stick.setVisible(false);
        this.Mon.signal(0);
        this.Mon_A.signal(0);
        this.Mon_B.signal(0);
        this.Mon_C.signal(0);
        this.Mon_D.signal(0);
        this.Mon_E.signal(0);
        this.Mon_F.signal(0);
        this.Mon_Y.signal(0);
        this.Mon2.signal(0);
        this.Mon_A2.signal(0);
        this.Mon_B2.signal(0);
        this.Mon_C2.signal(0);
        this.Mon_D2.signal(0);
        this.Mon_E2.signal(0);
        this.Mon_F2.signal(0);
        this.Mon_Y2.signal(0);
        this.mobj001.setVisible(true);
        this.mobj134.setVisible(true);
        this.CSFadeIn(30);
        this.camerawork.cut30();
        this.mobj134.start(1, "spacemove");
        System.sleep(60);
        this.tool.MSG(150, "ダミー", "#86#86#86 The Elsa #86#86#86");
        this.tool.Timechk_SceneEnd();
    }

    void whiteout() {
        this.whiteOut = new Effect(0);
        this.whiteOut.args[0] = -2130706433;
        this.whiteOut.args[1] = 30;
        this.whiteOut.args[2] = 0;
        this.whiteOut.call(0);
    }

    class Monitor
            extends Unit {
        int alpha;

        Monitor() {
        }

        void act17() {
            float f = 1.0f;
            while (f >= 0.75f) {
                this.setScale(f, 1.0f, 1.0f);
                f -= 0.02f;
                System.sleep(1);
            }
        }

        void off_s() {
            float f = 0.32f;
            int n = 0;
            while (n < 32) {
                this.setScale(0.32f, f -= 0.01f, 0.32f);
                System.sleep(1);
                ++n;
            }
            this.signal(0);
            this.signal(0);
            this.signal(0);
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

    class Yuri
            extends Chr {
        Chr face;

        Yuri() {
        }

        void act11() {
            this.mtn(271, 0, 120, 8, 0, 1.0f, true);
        }

        void act13() {
            this.mtn(273, 0, 405, 8, 0, 1.0f, true);
        }

        void act15() {
            this.mtn(275, 0, 300, 8, 0, 1.0f, true);
        }

        void act17() {
            this.mtn(276, 0, 270, 8, 0, 1.0f, true);
        }

        void act19() {
            this.setTranslate(0.16f, 0.01f, -2.76f);
            this.setRotate(0.0f, 9.0f, 0.0f);
            this.mtn(278, 0, 150, 8, 0, 1.06f, true);
        }

        void act20() {
            this.mtn(281, 0, 270, 8, 0, 1.0f, true);
        }

        void act22() {
            this.mtn(284, 0, 90, 8, 0, 1.0f, true);
        }

        void act25() {
            this.mtn(289, 0, 285, 8, 0, 1.0f, true);
        }

        void act26() {
            this.mtn(290, 0, 375, 8, 0, 1.0f, true);
        }

        void act9() {
            this.mtn(270, 0, 60, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(260, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_a
            extends Chr {
        Chr face;

        Com_a() {
        }

        void act1() {
            this.mtn(257, 0, 90, 8, 8, 1.0f, true);
        }

        void act2() {
            this.mtn(262, 0, 105, 8, 8, 1.0f, true);
        }

        void act20() {
            this.mtn(280, 0, 270, 8, 0, 1.0f, true);
        }

        void act23() {
            this.mtn(286, 0, 255, 8, 0, 1.0f, true);
        }

        void act25() {
            this.mtn(288, 0, 285, 8, 0, 1.0f, true);
        }

        void act27() {
            this.mtn(291, 0, 210, 8, 0, 1.1f, true);
        }

        void act8() {
            this.mtn(268, 0, 195, 8, 8, 1.01f, true);
        }

        void sit() {
            this.mtn(257, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_b
            extends Chr {
        Chr face;

        Com_b() {
        }

        void act1() {
            this.mtn(258, 0, 90, 8, 8, 1.0f, true);
        }

        void act12() {
            this.mtn(272, 0, 195, 8, 8, 1.0f, true);
        }

        void act28() {
            this.mtn(292, 0, 180, 8, 0, 1.0f, true);
        }

        void act3() {
            this.mtn(263, 0, 360, 8, 8, 1.0f, true);
        }

        void sit() {
            this.mtn(258, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_c
            extends Chr {
        Chr face;

        Com_c() {
        }

        void act1() {
            this.mtn(259, 0, 90, 8, 8, 1.0f, true);
        }

        void act19() {
            this.mtn(277, 0, 150, 8, 0, 1.06f, true);
        }

        void act21() {
            this.mtn(283, 0, 195, 8, 0, 1.0f, true);
        }

        void act24() {
            this.mtn(287, 0, 165, 8, 0, 1.0f, true);
        }

        void act5() {
            this.mtn(264, 0, 180, 8, 0, 1.0f, true);
        }

        void act6() {
            System.sleep(80);
            this.mtn(264, 0, 180, 8, 0, 1.0f, true);
        }

        void act7() {
            this.mtn(267, 0, 390, 8, 8, 1.0f, true);
        }

        void sit() {
            this.mtn(259, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_d
            extends Chr {
        Chr face;

        Com_d() {
        }

        void act1() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }

        void act14() {
            this.mtn(274, 0, 210, 8, 0, 1.0f, true);
        }

        void act19() {
            this.mtn(279, 0, 150, 8, 0, 1.06f, true);
        }

        void act6() {
            this.mtn(265, 0, 345, 8, 0, 1.05f, true);
        }

        void sit() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_e
            extends Chr {
        Chr face;

        Com_e() {
        }

        void act1() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }

        void act21() {
            this.mtn(282, 0, 195, 8, 0, 1.0f, true);
        }

        void act7() {
            this.mtn(266, 0, 390, 8, 8, 1.0f, true);
        }

        void sit() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_f
            extends Chr {
        Chr face;

        Com_f() {
        }

        void act1() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }

        void act23() {
            this.mtn(285, 0, 255, 8, 0, 1.0f, true);
        }

        void act8() {
            this.mtn(269, 0, 375, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.93f, 1.71f, 6.24f, 225.0f, -1.93f, 1.32f, 6.24f};
            SCE02010B.this.BaseCam.change();
            SCE02010B.this.BaseCam.setFov(50.0f);
            SCE02010B.this.BaseCam.setRotate(-0.06f, 347.42f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut11() {
            SCE02010B.this.tool.Timechk_CutChange();
            SCE02010B.this.BaseCam.setTranslate(-1.71f, 0.99f, 0.04f);
            SCE02010B.this.BaseCam.setRotate(-2.71f, 22.66f, 0.0f);
        }

        public void cut12() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.3f, 1.43f, 3.6f, 195.0f, 0.35f, 1.43f, 3.46f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -12.97f;
            fArray2[2] = 301.09f;
            fArray2[4] = 195.0f;
            fArray2[5] = -12.97f;
            fArray2[6] = 296.5f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut13() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 3.73f, 1.31f, 2.97f, 405.0f, 3.43f, 1.31f, 2.77f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -5.37f;
            fArray2[2] = 421.43f;
            fArray2[4] = 405.0f;
            fArray2[5] = -5.66f;
            fArray2[6] = 414.43f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut14() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -2.51f, 1.5f, -4.03f, 210.0f, -2.49f, 1.4f, -3.92f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.48f;
            fArray2[2] = 184.52f;
            fArray2[4] = 210.0f;
            fArray2[5] = -4.48f;
            fArray2[6] = 185.56f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut15() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.73f, 1.25f, 4.69f, 210.0f, 0.6f, 1.25f, 4.4f};
            SCE02010B.this.BaseCam.setRotate(-5.08f, 24.0f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut16() {
            SCE02010B.this.tool.Timechk_CutChange();
            SCE02010B.this.BaseCam.setTranslate(-3.99f, 1.76f, -2.48f);
            SCE02010B.this.BaseCam.setRotate(-29.82f, 271.06f, 0.0f);
        }

        public void cut17() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.33f, 1.59f, 4.81f, 600.0f, -0.48f, 2.01f, 3.3f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 2.44f;
            fArray2[2] = 355.77f;
            fArray2[4] = 600.0f;
            fArray2[5] = 4.92f;
            fArray2[6] = 357.75f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut19() {
            SCE02010B.this.tool.Timechk_CutChange();
            SCE02010B.this.BaseCam.setTranslate(1.07f, 2.72f, -6.13f);
            SCE02010B.this.BaseCam.setRotate(-16.65f, 164.98f, 0.0f);
        }

        public void cut2() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.82f, 1.26f, -0.75f, 105.0f, 0.7f, 1.26f, -0.6f};
            SCE02010B.this.BaseCam.setFov(25.0f);
            SCE02010B.this.BaseCam.setRotate(-8.53f, 316.22f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut20() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -13.58f;
            fArray[2] = 301.18f;
            fArray[4] = 270.0f;
            fArray[5] = -13.58f;
            fArray[6] = 304.18f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(-2.78f, 1.89f, -0.49f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut21() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -6.82f;
            fArray[2] = 49.65f;
            fArray[4] = 195.0f;
            fArray[5] = -7.82f;
            fArray[6] = 54.53f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(-0.54f, 1.31f, 3.81f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut22() {
            SCE02010B.this.tool.Timechk_CutChange();
            SCE02010B.this.BaseCam.setTranslate(-0.57f, 1.38f, -0.3f);
            SCE02010B.this.BaseCam.setRotate(-2.24f, -15.01f, 0.0f);
        }

        public void cut23() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.33f, 1.77f, -4.01f, 255.0f, 0.33f, 1.7f, -4.01f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -15.74f;
            fArray2[2] = 232.29f;
            fArray2[4] = 255.0f;
            fArray2[5] = -15.74f;
            fArray2[6] = 238.0f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut24() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -18.52f;
            fArray[2] = 203.77f;
            fArray[4] = 270.0f;
            fArray[5] = -19.0f;
            fArray[6] = 202.0f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(-3.51f, 1.93f, -0.34f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut25() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.56f, 2.09f, 3.73f, 450.0f, -0.9f, 2.09f, 2.5f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -12.57f;
            fArray2[2] = 337.47f;
            fArray2[4] = 450.0f;
            fArray2[5] = -14.72f;
            fArray2[6] = 348.4f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }

        public void cut27() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -2.02f, 1.5f, -1.71f, 210.0f, -1.99f, 1.4f, -1.63f};
            SCE02010B.this.BaseCam.setRotate(-8.51f, 291.8f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut28() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.42f, 1.34f, 1.44f, 180.0f, 1.5f, 1.32f, 1.5f};
            SCE02010B.this.BaseCam.setRotate(-9.65f, 226.93f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut29() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -39.89f;
            fArray[2] = 255.48f;
            fArray[4] = 240.0f;
            fArray[5] = -39.89f;
            fArray[6] = 250.0f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(-4.0f, 5.74f, -2.53f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut3() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -5.89f;
            fArray[2] = 188.89f;
            fArray[4] = 360.0f;
            fArray[5] = -5.89f;
            fArray[6] = 185.89f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(2.76f, 1.19f, -2.7f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut30() {
            SCE02010B.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE02010B.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02010B.this.light.setDirection2(1, 0.463f, -0.0f, 0.886f);
            SCE02010B.this.light.setColor(2, 0.11f, 0.11f, 0.11f);
            SCE02010B.this.light.setDirection2(2, 0.012f, -0.968f, 0.249f);
            SCE02010B.this.light.setColor(3, 0.08f, 0.08f, 0.08f);
            SCE02010B.this.light.setDirection2(3, -0.9f, -0.001f, -0.435f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, 2.17f, -0.54f, 6.06f, 240.0f, 2.19f, -0.54f, 5.82f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.41f;
            fArray2[2] = 393.34f;
            fArray2[4] = 240.0f;
            fArray2[5] = 6.41f;
            fArray2[6] = 393.34f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.setFov(61.82f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
            SCE02010B.this.BaseCam.change();
        }

        public void cut5() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 2.75f, 1.27f, 2.77f, 180.0f, 2.39f, 1.27f, 2.76f};
            SCE02010B.this.BaseCam.setRotate(-8.24f, 88.69f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut6() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -3.36f, 1.33f, 2.04f, 345.0f, -3.44f, 1.33f, 2.16f};
            SCE02010B.this.BaseCam.setRotate(-9.02f, 237.31f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut7() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.47f, 1.43f, 3.17f, 390.0f, 1.64f, 1.43f, 2.82f};
            SCE02010B.this.BaseCam.setRotate(-9.78f, 70.27f, 0.0f);
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut8() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -9.29f;
            fArray[2] = 350.43f;
            fArray[4] = 360.0f;
            fArray[5] = -9.29f;
            fArray[6] = 347.0f;
            float[] fArray2 = fArray;
            SCE02010B.this.BaseCam.setTranslate(3.15f, 1.33f, 2.11f);
            SCE02010B.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut9() {
            SCE02010B.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -3.58f, 1.19f, -2.75f, 180.0f, -3.57f, 1.19f, -2.74f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.89f;
            fArray2[2] = 258.8f;
            fArray2[4] = 180.0f;
            fArray2[5] = -6.44f;
            fArray2[6] = 254.5f;
            float[] fArray3 = fArray2;
            SCE02010B.this.BaseCam.transSPL(fArray, 0);
            SCE02010B.this.BaseCam.rotateSPL(fArray3, 0);
        }
    }

    class Units
            extends Unit {
        public Units(int n) {
            this.init(n);
        }

        void spacemove() {
            while (true) {
                this.setTranslate(this.px + 0.05f, this.py, this.pz - 0.05f);
                System.sleep(1);
            }
        }
    }
}

