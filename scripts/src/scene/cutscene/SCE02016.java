import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.PlayControl;
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
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02016
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02016,
        FLSmomo_h,
        FLSziggy_h {
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    public static final float nc = 1.0E7f;
    Effect ship2_efa;
    Effect ship2_efb;
    Effect ship2_efc;
    Effect u1_efa;
    Effect u1_efb;
    Effect u2_efa;
    Effect u2_efb;
    Effect u3_efa;
    Effect u3_efb;
    Effect rock1_ef;
    Effect rock2_ef;
    Effect rock3_ef;
    Effect rock11_ef;
    Effect rock22_ef;
    Effect rock33_ef;
    Effect fire1_ef;
    Effect fire2_ef;
    Effect fire3_ef;
    Effect rott2_ef;
    Effect rott3_ef;
    Effect marm2_ef;
    Effect marm3_ef;
    Effect syageki1_ef;
    Effect syageki2_ef;
    Effect syageki3_ef;
    Effect syageki4_ef;
    Effect u1sya_ef;
    Effect u2sya_ef;
    Effect u3sya_ef;
    Effect u4sya_ef;
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
    Camera cam6;
    Camerawork camerawork = new Camerawork();
    human ziggy;
    human momo;
    robo escp_ship;
    robo escp_ship2;
    robo u1;
    robo u2;
    robo u3;
    robo tube;
    float parcent = 1.0f;
    PlayControl pc;
    units dummy1;
    units dummy2;
    units dummy3;
    units dummy4;
    units dummy5;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    int rock_flag = 0;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02016() {
    }

    void FACE(int n, Chr chr, int n2, float f) {
        chr.mtn(n2, 8, f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        chr.start(4, null);
    }

    void FACE_SMOOTH(int n, Chr chr, int n2, float f) {
        chr.mtn(n2, 0, 120, 7, 9, 1.0f, false);
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
        while (true) {
            this.escp_ship2.setTranslate(this.escp_ship.px, this.escp_ship.py, this.escp_ship.pz);
            this.escp_ship2.setRotate(this.escp_ship.rx, this.escp_ship.ry, this.escp_ship.rz);
            if (this.rock_flag == 1) {
                this.rock1_ef.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY() + 1000.0f, this.cam4.getTranslateZ());
                this.rock11_ef.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY() + 1000.0f, this.cam4.getTranslateZ());
                this.rock2_ef.setTranslate(this.cam5.getTranslateX(), this.cam5.getTranslateY(), this.cam5.getTranslateZ());
                this.rock22_ef.setTranslate(this.cam5.getTranslateX(), this.cam5.getTranslateY(), this.cam5.getTranslateZ());
                this.rock3_ef.setTranslate(this.cam6.getTranslateX(), this.cam6.getTranslateY() + 1000.0f, this.cam6.getTranslateZ());
                this.rock33_ef.setTranslate(this.cam6.getTranslateX(), this.cam6.getTranslateY() + 1000.0f, this.cam6.getTranslateZ());
                this.rock_flag = 2;
            }
            System.sleep(1);
            if (this.rock_flag == 2) {
                this.rock1_ef.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY(), this.cam4.getTranslateZ());
                this.rock11_ef.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY(), this.cam4.getTranslateZ());
                this.rock2_ef.setTranslate(this.cam5.getTranslateX(), this.cam5.getTranslateY() + 1000.0f, this.cam5.getTranslateZ());
                this.rock22_ef.setTranslate(this.cam5.getTranslateX(), this.cam5.getTranslateY() + 1000.0f, this.cam5.getTranslateZ());
                this.rock3_ef.setTranslate(this.cam6.getTranslateX(), this.cam6.getTranslateY(), this.cam6.getTranslateZ());
                this.rock33_ef.setTranslate(this.cam6.getTranslateX(), this.cam6.getTranslateY(), this.cam6.getTranslateZ());
                this.rock_flag = 1;
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02016_F");
        Runtime.setFlags(127, 1, 1);
        System.println("XEVEJNAME:SCE02017");
        Runtime.jumpEvent(2170);
    }

    void init() {
        Stage.clrBackBuffer();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam4 = Camera.create(4);
        this.cam5 = Camera.create(5);
        this.cam6 = Camera.create(6);
        this.pc = PlayControl.create();
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.ziggy = new human(16777251, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ziggy.setShadow(0, 0);
        this.momo = new human(0x1000021, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo.setShadow(0, 0);
        this.escp_ship = new robo();
        this.escp_ship.init(20497, 0.0f, 0.0f, 0.0f, 0.0f);
        this.escp_ship.setShadow(0, 0);
        this.escp_ship2 = new robo();
        this.escp_ship2.init(20591, 0.0f, 0.0f, 0.0f, 0.0f);
        this.escp_ship2.setShadow(0, 0);
        this.u1 = new robo();
        this.u1.init(20554, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1.setShadow(0, 0);
        this.u2 = new robo();
        this.u2.init(20554, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u2.setShadow(0, 0);
        this.u3 = new robo();
        this.u3.init(20554, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u3.setShadow(0, 0);
        this.tube = new robo();
        this.tube.init(20490, 0.0f, 0.0f, 0.0f, 0.0f);
        this.tube.setShadow(0, 0);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy1.setRotate(-0.0f, 0.0f, -1.27f);
        this.dummy1.setParent(this.escp_ship2, 7);
        this.dummy1.setScale(1.0f, 1.0f, 1.0f);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy2.setParent(this.escp_ship2, 10);
        this.dummy2.setRotate(-0.0f, 0.0f, -1.27f);
        this.dummy2.setScale(1.0f, 1.0f, 1.0f);
        this.dummy3 = new units();
        this.dummy3.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy4 = new units();
        this.dummy4.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this.dummy5 = new units();
        this.dummy5.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy5.setVisible(false);
        this.ship2_efa = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ship2_efa.setCaster(this.dummy1);
        this.ship2_efa.setRotate(0.0f, -90.0f, 0.0f);
        this.ship2_efa.setScale(0.12f, 0.32f, -0.04f);
        this.ship2_efa.disp(false);
        this.ship2_efb = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ship2_efb.setCaster(this.dummy2);
        this.ship2_efb.setRotate(0.0f, -90.0f, 0.0f);
        this.ship2_efb.setScale(0.12f, 0.32f, -0.04f);
        this.ship2_efb.disp(false);
        this.ship2_efc = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ship2_efc.setCaster(this.escp_ship2);
        this.ship2_efc.setTranslate(0.0f, 3.9f, -5.02f);
        this.ship2_efc.setRotate(0.0f, 0.0f, 0.0f);
        this.ship2_efc.setScale(0.32f, 0.32f, -0.02f);
        this.ship2_efc.disp(false);
        this.u1_efa = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1_efa.setCaster(this.u1);
        this.u1_efa.setTranslate(-1.01f, 1.46f, -6.86f);
        this.u1_efa.setScale(-0.27f, 0.32f, -0.52f);
        this.u1_efa.disp(false);
        this.u1_efb = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1_efb.setCaster(this.u1);
        this.u1_efb.setTranslate(1.01f, 1.46f, -6.86f);
        this.u1_efb.setScale(-0.27f, 0.32f, -0.52f);
        this.u1_efb.disp(false);
        this.u2_efa = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u2_efa.setCaster(this.u2);
        this.u2_efa.setTranslate(-1.01f, 1.46f, -6.86f);
        this.u2_efa.setScale(-0.27f, 0.32f, -0.52f);
        this.u2_efa.disp(false);
        this.u2_efb = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u2_efb.setCaster(this.u2);
        this.u2_efb.setTranslate(1.01f, 1.46f, -6.86f);
        this.u2_efb.setScale(-0.27f, 0.32f, -0.52f);
        this.u2_efb.disp(false);
        this.u3_efa = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u3_efa.setCaster(this.u3);
        this.u3_efa.setTranslate(-1.01f, 1.46f, -6.86f);
        this.u3_efa.setScale(-0.27f, 0.32f, -0.52f);
        this.u3_efa.disp(false);
        this.u3_efb = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u3_efb.setCaster(this.u3);
        this.u3_efb.setTranslate(1.01f, 1.46f, -6.86f);
        this.u3_efb.setScale(-0.27f, 0.32f, -0.52f);
        this.u3_efb.disp(false);
        this.rock1_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock1_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock1_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock1_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock1_ef.disp(false);
        this.rock2_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock2_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock2_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock2_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock2_ef.disp(false);
        this.rock3_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock3_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock3_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock3_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock3_ef.disp(false);
        this.rock11_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock11_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock11_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock11_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock11_ef.disp(false);
        this.rock22_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock22_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock22_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock22_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock22_ef.disp(false);
        this.rock33_ef = new Effect(739, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rock33_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.rock33_ef.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.rock33_ef.setScale(0.017f, 0.017f, 0.017f);
        this.rock33_ef.disp(false);
        this.fire1_ef = new Effect(1403, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fire1_ef.setCaster(this.u1);
        this.fire1_ef.setMotion(true);
        this.fire1_ef.setTranslate(0.6f, 2.7f, -3.07f);
        this.fire1_ef.setScale(4.2f, 4.2f, 4.2f);
        this.fire1_ef.setForceLoop(false);
        this.fire1_ef.disp(false);
        this.fire2_ef = new Effect(1402, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fire2_ef.setCaster(this.u1);
        this.fire2_ef.setMotion(true);
        this.fire2_ef.setTranslate(-2.05f, -3.55f, 2.85f);
        this.fire2_ef.setScale(5.55f, 5.55f, 5.55f);
        this.fire2_ef.setForceLoop(false);
        this.fire2_ef.disp(false);
        this.fire3_ef = new Effect(1509, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fire3_ef.setCaster(this.u1);
        this.fire3_ef.setMotion(true);
        this.fire3_ef.setTranslate(0.0f, 1.6f, -4.8f);
        this.fire3_ef.setScale(2.92f, 2.92f, 2.92f);
        this.fire3_ef.setForceLoop(false);
        this.fire3_ef.noAttach(false);
        this.fire3_ef.disp(false);
        this.rott2_ef = new Effect(1402, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rott2_ef.setCaster(this.u2);
        this.rott2_ef.setMotion(true);
        this.rott2_ef.setTranslate(1.03f, 1.55f, 7.67f);
        this.rott2_ef.setScale(5.55f, 5.55f, 5.55f);
        this.rott2_ef.setForceLoop(false);
        this.rott2_ef.disp(false);
        this.rott3_ef = new Effect(1509, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rott3_ef.setCaster(this.u2);
        this.rott3_ef.setMotion(true);
        this.rott3_ef.setTranslate(0.0f, 1.6f, -4.8f);
        this.rott3_ef.setScale(2.92f, 2.92f, 2.92f);
        this.rott3_ef.noAttach(false);
        this.rott3_ef.setForceLoop(false);
        this.rott3_ef.disp(false);
        this.marm2_ef = new Effect(1402, 0.0f, 0.0f, 0.0f, 0.0f);
        this.marm2_ef.setCaster(this.u3);
        this.marm2_ef.setMotion(true);
        this.marm2_ef.setTranslate(2.35f, 0.65f, 8.05f);
        this.marm2_ef.setScale(5.55f, 5.55f, 5.55f);
        this.marm2_ef.setForceLoop(false);
        this.marm2_ef.disp(false);
        this.marm3_ef = new Effect(1509, 0.0f, 0.0f, 0.0f, 0.0f);
        this.marm3_ef.setCaster(this.u3);
        this.marm3_ef.setMotion(true);
        this.marm3_ef.setTranslate(0.0f, 1.6f, -4.8f);
        this.marm3_ef.setScale(2.92f, 2.92f, 2.92f);
        this.marm3_ef.setForceLoop(false);
        this.marm3_ef.noAttach(false);
        this.marm3_ef.disp(false);
        this.u1sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u2sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u3sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u4sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1sya_ef.disp(false);
        this.u2sya_ef.disp(false);
        this.u3sya_ef.disp(false);
        this.u4sya_ef.disp(false);
        this.syageki1_ef = new Effect(1577, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syageki2_ef = new Effect(1577, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syageki3_ef = new Effect(1577, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syageki4_ef = new Effect(1577, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syageki1_ef.disp(false);
        this.syageki2_ef.disp(false);
        this.syageki3_ef.disp(false);
        this.syageki4_ef.disp(false);
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
        this.loadarc(this.momo.getChild(0x1000000), "FLSmomo_h.fpk");
        this.loadarc(this.ziggy.getChild(0x1000000), "FLSziggy_h.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.escp_ship2.setVisible(false);
        this.u1.setVisible(false);
        this.u2.setVisible(false);
        this.u3.setVisible(false);
        this.ziggy.face.mtn(2, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.momo.face.mtn(30, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.escp_ship.setVisible(1, false);
        Sound.streamPlay(1290017, 48000);
        this.cam1.change();
        this.camerawork.cut001();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.55f, 0.8f, 0.73f);
        this.light.setDirection2(1, -0.7f, 0.014f, 0.714f);
        this.light.setColor(2, 0.37f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.884f, 0.245f, 0.398f);
        this.light.setColor(3, 0.13f, 0.23f, 0.23f);
        this.light.setDirection2(3, 0.162f, -0.768f, 0.62f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tube.setScale(1.0f, 1.0f, -3.74f);
        this.ziggy.setTranslate(-0.024999999f, 6.212498f, 7.3675f);
        this.ziggy.setRotate(1.6633312f, 0.0f, -0.8333334f);
        this.momo.setTranslate(-0.014999947f, 6.229994f, 6.2275033f);
        this.momo.setRotate(6.4966664f, 0.0f, -0.8333334f);
        this.ziggy.start(1, "mtn_001");
        this.wait_clr(42);
        this.dummy1.start(1, "ziggy_001_serifu");
        this.MSGW("I have to say,");
        System.sleep(75);
        this.MSGW("I'm very impressed.");
        this.wait_clr(45);
        this.momo.start(1, "mtn_002_mae");
        System.sleep(21);
        this.__wait();
        this.camerawork.cut002();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.55f, 0.8f, 0.73f);
        this.light.setDirection2(1, -0.7f, 0.014f, 0.714f);
        this.light.setColor(2, 0.37f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.884f, 0.245f, 0.398f);
        this.light.setColor(3, 0.13f, 0.23f, 0.23f);
        this.light.setDirection2(3, 0.162f, -0.768f, 0.62f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.dummy2.start(1, "momo_002_serifu");
        this.momo.start(1, "mtn_002");
        this.MSGW("I'll transfer the gun controls to you.");
        this.wait_clr(66);
        this.ziggy.start(1, "mtn_003");
        this.wait_clr(21);
        this.__wait();
        this.momo.hairStop(0, 1);
        this.camerawork.cut003();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.55f, 0.8f, 0.73f);
        this.light.setDirection2(1, -0.72f, 0.28f, 0.635f);
        this.light.setColor(2, 0.37f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.884f, 0.245f, 0.398f);
        this.light.setColor(3, 0.25f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.367f, -0.918f, 0.151f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.momo.setTranslate(-0.005999911f, 6.215487f, 6.2275033f);
        this.momo.setRotate(6.4966664f, 0.0f, -0.8333334f);
        this.ziggy.start(1, "mtn_003");
        this.momo.start(1, "mtn_004");
        this.wait_clr(6);
        this.dummy2.start(1, "momo_003_serifu");
        this.MSGW("Could you please take care\nof the return fire?");
        System.sleep(72);
        this.MSGW("Roger.");
        this.dummy1.start(1, "ziggy_003_serifu");
        System.sleep(48);
        this.MSGW("I wouldn't be much of a\nbodyguard if I didn't.");
        this.ziggy.look_eye_speed(0.72f);
        this.ziggy.look_eye_set(-12.0f, 1.72f);
        this.wait_clr(102);
        this.wait_clr(15);
        this.__wait();
        this.ziggy.setVisible(false);
        this.momo.setVisible(false);
        this.escp_ship2.setVisible(true);
        this.u1.setVisible(true);
        this.u2.setVisible(true);
        this.u3.setVisible(true);
        this.cam1.setRoll(0.0f);
        this.cam1.setFov(0.0f);
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.ziggy.setScale(0.25f, 0.25f, 0.25f);
        this.momo.setScale(0.25f, 0.25f, 0.25f);
        this.tube.setScale(0.52f, 0.52f, -1.23f);
        this.__wait();
        this.u3.setTranslate(1.05f, -0.0f, 0.0f);
        this.u3.setRotate(0.0f, 0.0f, 0.0f);
        this.escp_ship.mtn(263, 0, 1.0f, false);
        this.escp_ship2.mtn(266, 0, 1.0f, false);
        this.u1.mtn(269, 0, 1.0f, false);
        this.u2.mtn(272, 0, 1.0f, false);
        this.u3.mtn(275, 0, 1.0f, false);
        this.escp_ship.start(5, null);
        this.escp_ship2.start(5, null);
        this.u1.start(5, null);
        this.u2.start(5, null);
        this.u3.start(5, null);
        this.pc.loadCamera("c02s16_A.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.cam1.change();
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.96f, 0.96f, 0.96f);
        this.light.setDirection2(1, -0.755f, 0.482f, 0.445f);
        this.light.setColor(2, 0.63f, 0.63f, 0.63f);
        this.light.setDirection2(2, 0.939f, 0.025f, 0.343f);
        this.light.setColor(3, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(3, 0.236f, -0.801f, 0.55f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ship2_efa.disp(true);
        this.ship2_efb.disp(true);
        this.ship2_efc.disp(true);
        this.u1_efa.disp(true);
        this.u1_efb.disp(true);
        this.u2_efa.disp(true);
        this.u2_efb.disp(true);
        this.u3_efa.disp(true);
        this.u3_efb.disp(true);
        this.escp_ship.setVisible(1, true);
        this.momo.setVisible(false);
        this.ziggy.setVisible(false);
        System.sleep(88);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.light.setDirection2(1, -0.33f, 0.898f, -0.29f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.655f, 0.667f, 0.356f);
        this.light.setColor(3, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(3, 0.236f, -0.801f, 0.55f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.escp_ship.setTranslate(-0.0f, -0.15f, 0.67f);
        this.escp_ship.setRotate(0.0f, 0.0f, 0.0f);
        this.escp_ship2.setTranslate(-0.0f, -0.15f, 0.67f);
        this.escp_ship2.setRotate(0.0f, 0.0f, 0.0f);
        this.u3.setTranslate(0.0f, 0.0f, 0.0f);
        this.u3.setRotate(0.0f, 0.0f, 0.0f);
        System.sleep(76);
        this.cam2.change();
        this.__wait();
        this.camerawork.cut005_momo();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.55f, 0.8f, 0.73f);
        this.light.setDirection2(1, -0.72f, 0.28f, 0.635f);
        this.light.setColor(2, 0.37f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.884f, 0.245f, 0.398f);
        this.light.setColor(3, 0.21f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.063f, -0.865f, 0.497f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.escp_ship2.setVisible(false);
        this.u1.setVisible(false);
        this.u2.setVisible(false);
        this.u3.setVisible(false);
        this.escp_ship.setVisible(1, false);
        this.escp_ship.setTranslate(0.0f, 0.0f, 0.0f);
        this.escp_ship.setRotate(0.0f, 0.0f, 0.0f);
        this.dummy1.start(1, "momo_face_005");
        this.momo.setTranslate(-0.50750005f, 5.780002f, 73.37337f);
        this.momo.setRotate(6.4999995f, 0.0f, -0.8299999f);
        this.momo.setVisible(true);
        this.momo.start(1, "mtn_005");
        System.sleep(1);
        this.u3sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u3sya_ef.setScale(0.0f, 0.0f, 0.0f);
        this.u3sya_ef.setTranslate(0.07500655f, 2.1999817f, 4.0500016f);
        this.u3sya_ef.setRotate(0.8333365f, -13.333329f, 0.0f);
        this.u3sya_ef.setCaster(this.u3);
        this.u3sya_ef.setMotion(true);
        this.u3sya_ef.noAttach(false);
        this.u3sya_ef.setForceLoop(true);
        System.sleep(7);
        this.u1sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1sya_ef.setScale(0.0f, 0.0f, 0.0f);
        this.u1sya_ef.setTranslate(1.19000006E-7f, 2.2079923f, 1.8000005f);
        this.u1sya_ef.setRotate(-5.316616f, 7.274991f, -32.499996f);
        this.u1sya_ef.setCaster(this.u1);
        this.u1sya_ef.setMotion(true);
        this.u1sya_ef.noAttach(false);
        this.u1sya_ef.setForceLoop(true);
        System.sleep(7);
        this.u2sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u2sya_ef.setScale(0.0f, 0.0f, 0.0f);
        this.u2sya_ef.setTranslate(0.0f, 2.7556922f, 5.0884733f);
        this.u2sya_ef.setRotate(2.3001087f, -1.4749987f, 0.0f);
        this.u2sya_ef.setCaster(this.u2);
        this.u2sya_ef.setMotion(true);
        this.u2sya_ef.noAttach(false);
        this.u2sya_ef.setForceLoop(true);
        System.sleep(7);
        this.u4sya_ef = new Effect(1569, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u4sya_ef.setScale(0.0f, 0.0f, 0.0f);
        this.u4sya_ef.setTranslate(0.0f, 2.7556922f, 5.0884733f);
        this.u4sya_ef.setRotate(2.3001087f, -1.4749987f, 0.0f);
        this.u4sya_ef.setCaster(this.u2);
        this.u4sya_ef.setMotion(true);
        this.u4sya_ef.noAttach(false);
        this.u4sya_ef.setForceLoop(true);
        this.escp_ship.setVisible(1, true);
        this.momo.setVisible(false);
        this.escp_ship.setTranslate(0.0f, 0.0f, 0.0f);
        this.escp_ship.setRotate(0.0f, 0.0f, 0.0f);
        this.escp_ship2.setTranslate(0.0f, 0.0f, 0.0f);
        this.escp_ship2.setRotate(0.0f, 0.0f, 0.0f);
        this.escp_ship.mtn(264, 0, 1.0f, false);
        this.escp_ship2.mtn(267, 0, 1.0f, false);
        this.u1.mtn(270, 0, 1.0f, false);
        this.u2.mtn(273, 0, 1.0f, false);
        this.u3.mtn(276, 0, 1.0f, false);
        this.escp_ship.start(5, null);
        this.escp_ship2.start(5, null);
        this.u1.start(5, null);
        this.u2.start(5, null);
        this.u3.start(5, null);
        this.pc.loadCamera("c02s16_B.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.cam1.change();
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.light.setDirection2(1, -0.33f, 0.898f, -0.29f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.655f, 0.667f, 0.356f);
        this.light.setColor(3, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(3, 0.236f, -0.801f, 0.55f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.escp_ship2.setVisible(true);
        this.u1.setVisible(true);
        this.u2.setVisible(true);
        this.u3.setVisible(true);
        this.u1sya_ef.setScale(0.57f, 0.57f, 0.57f);
        this.u2sya_ef.setScale(0.35f, 0.35f, 0.35f);
        this.u3sya_ef.setScale(0.53f, 0.53f, 0.53f);
        this.u4sya_ef.setScale(0.35f, 0.35f, 0.35f);
        System.sleep(3);
        System.sleep(2);
        System.sleep(2);
        System.sleep(2);
        System.sleep(3);
        this.ship2_efa.setScale(0.2f, 0.11f, 0.27f);
        this.ship2_efb.setScale(0.2f, 0.11f, 0.27f);
        this.ship2_efc.setScale(0.32f, 0.11f, 0.32f);
        this.__wait();
        System.sleep(38);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.81f, 0.81f, 0.81f);
        this.light.setDirection2(1, -0.755f, 0.482f, 0.445f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.655f, 0.667f, 0.356f);
        this.light.setColor(3, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(3, 0.236f, -0.801f, 0.55f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tube.setScale(1.15f, 1.15f, -10.12f);
        this.u1.setTranslate(0.79999983f, 1.5999997f, 158.3992f);
        this.u2.setTranslate(0.0f, 0.0f, 147.2237f);
        this.u3.setTranslate(0.0f, 0.0f, 116.79973f);
        System.sleep(32);
        this.u1.setTranslate(0.0f, 0.0f, 0.0f);
        this.u2.setTranslate(0.0f, 0.0f, 0.0f);
        this.u3.setTranslate(0.0f, 0.0f, 0.0f);
        this.__wait();
        this.cam2.change();
        this.camerawork.cut009();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.55f, 0.8f, 0.73f);
        this.light.setDirection2(1, 0.754f, 0.562f, -0.341f);
        this.light.setColor(2, 0.37f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.17f, 0.945f, 0.28f);
        this.light.setColor(3, 0.13f, 0.23f, 0.23f);
        this.light.setDirection2(3, 0.958f, -0.0f, -0.287f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.escp_ship2.setVisible(false);
        this.tube.setTranslate(-0.011999963f, 0.0f, -0.011749985f);
        this.tube.setRotate(3.9999895f, 4.5249977f, 5.133317f);
        this.tube.setScale(1.15f, 1.15f, -7.15f);
        this.dummy1.start(1, "target_on");
        this.ziggy.setTranslate(-0.16649987f, 2.3535056f, -3.5204933f);
        this.ziggy.setRotate(4.399996f, 2.9999988f, 0.0f);
        this.ziggy.start(1, "mtn_006");
        this.ziggy.setVisible(true);
        this.escp_ship.setVisible(1, false);
        this.ship2_efa.setScale(0.0f, 0.0f, 0.0f);
        this.ship2_efb.setScale(0.0f, 0.0f, 0.0f);
        this.ship2_efc.setScale(0.0f, 0.0f, 0.0f);
        System.sleep(27);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.86f, 0.86f, 0.86f);
        this.light.setDirection2(1, -0.52f, 0.817f, -0.249f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.685f, 0.681f, 0.259f);
        this.light.setColor(3, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(3, 0.592f, -0.61f, 0.526f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.change();
        this.tube.setTranslate(0.0f, 0.0f, 0.0f);
        this.tube.setRotate(0.0f, 0.0f, 0.0f);
        this.ship2_efa.setScale(0.2f, 0.11f, 0.27f);
        this.ship2_efb.setScale(0.2f, 0.11f, 0.27f);
        this.ship2_efc.setScale(0.32f, 0.11f, 0.32f);
        this.u1sya_ef.disp(false);
        this.u2sya_ef.disp(false);
        this.u3sya_ef.disp(false);
        this.u4sya_ef.disp(false);
        this.escp_ship2.setVisible(true);
        this.tube.setScale(1.78f, 1.96f, -2.47f);
        this.ziggy.setVisible(false);
        this.escp_ship.setVisible(1, true);
        this.syageki1_ef.setCaster(this.escp_ship);
        this.syageki2_ef.setCaster(this.escp_ship);
        this.syageki3_ef.setCaster(this.escp_ship);
        this.syageki4_ef.setCaster(this.escp_ship);
        this.syageki1_ef.setMotion(true);
        this.syageki2_ef.setMotion(true);
        this.syageki3_ef.setMotion(true);
        this.syageki4_ef.setMotion(true);
        this.syageki1_ef.noAttach(false);
        this.syageki2_ef.noAttach(false);
        this.syageki3_ef.noAttach(false);
        this.syageki4_ef.noAttach(false);
        this.syageki1_ef.setScale(0.17f, 1.0f, 0.93f);
        this.syageki2_ef.setScale(0.17f, 1.0f, 0.93f);
        this.syageki3_ef.setScale(0.17f, 1.0f, 0.93f);
        this.syageki4_ef.setScale(0.17f, 1.0f, 0.93f);
        this.syageki4_ef.disp(true);
        this.syageki1_ef.setTranslate(-0.27048776f, 2.5324934f, -2.9374845f);
        this.syageki1_ef.setRotate(3.499998f, -20.166601f, -14.3333025f);
        this.syageki2_ef.setTranslate(0.002012056f, 2.5274935f, -2.9374845f);
        this.syageki2_ef.setRotate(-32.166664f, 15.499983f, 101.39992f);
        this.syageki3_ef.setTranslate(-0.0639879f, 2.5109885f, -2.9374845f);
        this.syageki3_ef.setRotate(-32.16664f, 17.699968f, 100.66647f);
        this.syageki4_ef.setTranslate(-0.2289876f, 2.510986f, -2.9594839f);
        this.syageki4_ef.setRotate(-35.099953f, 19.53329f, 100.66647f);
        System.sleep(1);
        System.sleep(1);
        this.syageki1_ef.disp(true);
        System.sleep(1);
        System.sleep(1);
        this.syageki3_ef.disp(true);
        System.sleep(1);
        this.syageki1_ef.setRotate(-34.43f, 19.83f, 5.93f);
        this.syageki2_ef.setRotate(-36.0f, 17.17f, 101.4f);
        this.syageki3_ef.setRotate(-32.17f, 17.7f, 100.67f);
        this.syageki4_ef.setRotate(-213.77f, -1977.94f, 135.67f);
        System.sleep(1);
        this.syageki2_ef.disp(true);
        System.sleep(1);
        System.sleep(1);
        System.sleep(1);
        System.sleep(1);
        this.syageki1_ef.setRotate(-22.7f, 21.97f, 20.87f);
        this.syageki2_ef.setRotate(-31.73f, 13.43f, 151.53f);
        this.syageki3_ef.setRotate(-33.77f, 13.43f, 115.07f);
        this.syageki4_ef.setRotate(-213.77f, -1999.8f, 178.33f);
        System.sleep(5);
        this.syageki1_ef.setRotate(-36.57f, 12.37f, 31.53f);
        this.syageki2_ef.setRotate(-31.73f, 20.37f, 151.53f);
        this.syageki3_ef.setRotate(-35.37f, 19.83f, 115.07f);
        this.syageki4_ef.setRotate(-216.43f, -1999.8f, 178.33f);
        System.sleep(5);
        this.syageki1_ef.setRotate(-34.43f, 19.83f, 5.93f);
        this.syageki2_ef.setRotate(-36.0f, 17.17f, 101.4f);
        this.syageki3_ef.setRotate(-32.17f, 17.7f, 100.67f);
        this.syageki4_ef.setRotate(-213.77f, -1977.94f, 135.67f);
        System.sleep(5);
        this.syageki1_ef.setRotate(-22.7f, 21.97f, 20.87f);
        this.syageki2_ef.setRotate(-31.73f, 13.43f, 151.53f);
        this.syageki3_ef.setRotate(-33.77f, 13.43f, 115.07f);
        this.syageki4_ef.setRotate(-213.77f, -1999.8f, 178.33f);
        System.sleep(5);
        this.syageki1_ef.setRotate(-25.37f, 13.43f, 5.93f);
        this.syageki2_ef.setRotate(-38.67f, 5.97f, 105.67f);
        this.syageki3_ef.setRotate(-31.1f, 8.63f, 106.0f);
        this.syageki4_ef.setRotate(-211.63f, -2001.4f, 135.67f);
        System.sleep(5);
        this.syageki1_ef.setRotate(-28.57f, 9.17f, 5.93f);
        this.syageki2_ef.setRotate(-29.6f, 18.77f, 121.67f);
        this.syageki3_ef.setRotate(-24.7f, 13.43f, 106.0f);
        this.syageki4_ef.setRotate(-215.9f, -1997.67f, 146.87f);
        System.sleep(5);
        this.syageki1_ef.setRotate(3.499998f, -20.166601f, -14.3333025f);
        this.syageki2_ef.setRotate(-32.166664f, 15.499983f, 101.39992f);
        this.syageki3_ef.setRotate(-32.16664f, 17.699968f, 100.66647f);
        this.syageki4_ef.setRotate(-35.099953f, 19.53329f, 100.66647f);
        System.sleep(2);
        this.__wait();
        this.syageki1_ef.setForceLoop(false);
        this.syageki2_ef.setForceLoop(false);
        this.syageki3_ef.setForceLoop(false);
        this.syageki4_ef.setForceLoop(false);
        this.syageki1_ef.disp(false);
        this.syageki2_ef.disp(false);
        this.syageki3_ef.disp(false);
        this.syageki4_ef.disp(false);
        this.escp_ship.mtn(265, 0, 1.0f, false);
        this.escp_ship2.mtn(268, 0, 1.0f, false);
        this.u1.mtn(271, 0, 1.0f, false);
        this.u2.mtn(274, 0, 1.0f, false);
        this.u3.mtn(277, 0, 1.0f, false);
        this.escp_ship.start(5, null);
        this.escp_ship2.start(5, null);
        this.u1.start(5, null);
        this.u2.start(5, null);
        this.u3.start(5, null);
        this.pc.loadCamera("c02s16_C.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.cam1.change();
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.light.setDirection2(1, -0.33f, 0.898f, -0.29f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.655f, 0.667f, 0.356f);
        this.light.setColor(3, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(3, 0.236f, -0.801f, 0.55f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tube.setScale(1.15f, 0.53f, -2.91f);
        this.ship2_efa.setScale(0.14f, 0.11f, 0.24f);
        this.ship2_efb.setScale(0.14f, 0.11f, 0.24f);
        this.ship2_efc.setScale(0.19f, 0.11f, 0.27f);
        this.u1_efa.setForceLoop(false);
        this.u2_efa.setForceLoop(false);
        this.u3_efa.setForceLoop(false);
        this.u1_efa.disp(false);
        this.u1_efb.disp(false);
        this.u2_efa.disp(false);
        this.u2_efb.disp(false);
        this.u3_efa.disp(false);
        this.u3_efb.disp(false);
        System.sleep(7);
        this.fire1_ef.disp(true);
        this.fire2_ef.disp(true);
        this.fire3_ef.disp(true);
        System.sleep(15);
        this.rott2_ef.disp(true);
        this.rott3_ef.disp(true);
        System.sleep(17);
        this.marm2_ef.disp(true);
        this.marm3_ef.disp(true);
        System.sleep(118);
        this.ship2_efa.setScale(0.16f, 0.11f, 0.15f);
        this.ship2_efb.setScale(0.16f, 0.11f, 0.15f);
        this.ship2_efc.setScale(0.25f, 0.11f, 0.22f);
        System.sleep(1);
        this.ship2_efa.setScale(0.22f, 0.11f, 0.15f);
        this.ship2_efb.setScale(0.22f, 0.11f, 0.15f);
        this.ship2_efc.setScale(0.28f, 0.11f, 0.22f);
        System.sleep(1);
        this.ship2_efa.setScale(0.25f, 0.11f, 0.15f);
        this.ship2_efb.setScale(0.25f, 0.11f, 0.15f);
        this.ship2_efc.setScale(0.3f, 0.11f, 0.22f);
        System.sleep(1);
        this.ship2_efa.setScale(0.27f, 0.11f, 0.15f);
        this.ship2_efb.setScale(0.27f, 0.11f, 0.15f);
        this.ship2_efc.setScale(0.34f, 0.11f, 0.22f);
        System.sleep(3);
        this.ship2_efa.setScale(0.31f, 0.11f, 0.17f);
        this.ship2_efb.setScale(0.31f, 0.11f, 0.17f);
        this.ship2_efc.setScale(0.37f, 0.11f, 0.25f);
        System.sleep(4);
        this.ship2_efa.setScale(0.27f, 0.11f, 0.17f);
        this.ship2_efb.setScale(0.27f, 0.11f, 0.17f);
        this.ship2_efc.setScale(0.3f, 0.11f, 0.22f);
        System.sleep(2);
        this.ship2_efa.setScale(0.25f, 0.11f, 0.15f);
        this.ship2_efb.setScale(0.25f, 0.11f, 0.15f);
        this.ship2_efc.setScale(0.3f, 0.11f, 0.17f);
        System.sleep(1);
        this.ship2_efa.setScale(0.23f, 0.11f, 0.13f);
        this.ship2_efb.setScale(0.23f, 0.11f, 0.13f);
        this.ship2_efc.setScale(0.3f, 0.11f, 0.17f);
        System.sleep(1);
        this.ship2_efa.setScale(0.24f, 0.11f, 0.12f);
        this.ship2_efb.setScale(0.24f, 0.11f, 0.12f);
        this.ship2_efc.setScale(0.32f, 0.11f, 0.17f);
        System.sleep(5);
        this.__wait();
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
        }

        void mtn_001() {
            this.mtn(257, 0, 0.82f, true);
        }

        void mtn_002() {
            this.mtn(258, 0, 0.82f, true);
        }

        void mtn_002_mae() {
            this.mtn(258, 0, 0, 0, 0, 0.82f, true);
        }

        void mtn_003() {
            this.mtn(259, 0, 1.0f, true);
        }

        void mtn_003_mae() {
            this.mtn(259, 0, 0, 0, 0, 1.0f, true);
        }

        void mtn_004() {
            this.mtn(260, 0, 1.0f, true);
        }

        void mtn_005() {
            this.mtn(261, 0, 1.0f, true);
        }

        void mtn_006() {
            this.mtn(262, 8, 1.0f, true);
        }
    }

    class robo
            extends Chr {
        robo() {
        }

        void mtn_016a() {
            this.mtn(263, 8, 1.0f, true);
        }

        void mtn_016b() {
            this.mtn(264, 8, 1.0f, true);
        }

        void mtn_016c() {
            this.mtn(265, 8, 1.0f, true);
        }

        void mtn_110a() {
            this.mtn(266, 8, 1.0f, true);
        }

        void mtn_110b() {
            this.mtn(267, 8, 1.0f, true);
        }

        void mtn_110c() {
            this.mtn(268, 8, 1.0f, true);
        }

        void mtn_73aa() {
            this.mtn(269, 8, 1.0f, true);
        }

        void mtn_73ab() {
            this.mtn(270, 8, 1.0f, true);
        }

        void mtn_73ac() {
            this.mtn(271, 8, 1.0f, true);
        }

        void mtn_73ba() {
            this.mtn(272, 8, 1.0f, true);
        }

        void mtn_73bb() {
            this.mtn(273, 8, 1.0f, true);
        }

        void mtn_73bc() {
            this.mtn(274, 8, 1.0f, true);
        }

        void mtn_73ca() {
            this.mtn(275, 8, 1.0f, true);
        }

        void mtn_73cb() {
            this.mtn(276, 8, 1.0f, true);
        }

        void mtn_73cc() {
            this.mtn(277, 8, 1.0f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }

        void momo_002_serifu() {
            SCE02016.this.FACE(54, SCE02016.this.momo.face, 29, 1.127f);
        }

        void momo_003_serifu() {
            SCE02016.this.FACE(57, SCE02016.this.momo.face, 29, 1.127f);
        }

        void momo_face_005() {
            SCE02016.this.momo.face.mtn(2, 95, 102, 0, 0, 0.527f, false);
            SCE02016.this.momo.face.start(4, null);
            System.sleep(9);
            SCE02016.this.momo.face.mtn(2, 92, 120, 0, 0, 1.27f, false);
            SCE02016.this.momo.face.start(4, null);
        }

        void target_on() {
            SCE02016.this.rock1_ef.disp(true);
            SCE02016.this.rock2_ef.disp(true);
            SCE02016.this.rock3_ef.disp(true);
            SCE02016.this.rock11_ef.disp(true);
            SCE02016.this.rock22_ef.disp(true);
            SCE02016.this.rock33_ef.disp(true);
            float[] fArray = new float[]{1.0f, -0.06249848f, 2.5764768f, -3.46547f};
            SCE02016.this.cam4.transSPL(fArray, 1);
            float[] fArray2 = new float[]{10.0f, -0.08199892f, 2.5854764f, -3.46547f};
            float[] fArray3 = new float[]{1.0f, -0.10599805f, 2.5719771f, -3.46547f};
            SCE02016.this.rock_flag = 1;
            System.sleep(2);
            SCE02016.this.cam5.transSPL(fArray2, 0);
            System.sleep(3);
            SCE02016.this.cam6.transSPL(fArray3, 0);
            System.sleep(22);
            SCE02016.this.rock_flag = 0;
            SCE02016.this.rock1_ef.disp(false);
            SCE02016.this.rock2_ef.disp(false);
            SCE02016.this.rock3_ef.disp(false);
            SCE02016.this.rock11_ef.disp(false);
            SCE02016.this.rock22_ef.disp(false);
            SCE02016.this.rock33_ef.disp(false);
        }

        void ziggy_001_serifu() {
            SCE02016.this.FACE(57, SCE02016.this.ziggy.face, 1, 1.12f);
            System.sleep(15);
            SCE02016.this.FACE(33, SCE02016.this.ziggy.face, 1, 1.12f);
        }

        void ziggy_003_serifu() {
            SCE02016.this.FACE_SMOOTH(33, SCE02016.this.ziggy.face, 5, 1.02f);
            System.sleep(12);
            SCE02016.this.FACE(34, SCE02016.this.ziggy.face, 5, 0.92f);
            System.sleep(12);
            SCE02016.this.FACE(33, SCE02016.this.ziggy.face, 5, 0.92f);
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

        public void cut001() {
            SCE02016.this.cam1.setFov(25.71f);
            SCE02016.this.cam0.setFov(25.71f);
            float[] fArray = new float[]{1.0f, 0.52f, 7.51f, 7.91f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -17.46f;
            fArray2[2] = 44.68f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut002() {
            SCE02016.this.cam1.setFov(22.05f);
            SCE02016.this.cam0.setFov(22.05f);
            float[] fArray = new float[]{1.0f, 0.51f, 7.02f, 7.17f, 744.0f, 0.51f, 7.02f, 7.17f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 1.36f;
            fArray2[2] = 37.63f;
            fArray2[4] = 744.0f;
            fArray2[5] = 1.36f;
            fArray2[6] = 37.63f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 22.05f, 744.0f, 23.23f};
            SCE02016.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut003() {
            SCE02016.this.cam1.setFov(27.96f);
            SCE02016.this.cam0.setFov(27.96f);
            float[] fArray = new float[]{1.0f, 1.08f, 6.94f, 8.04f, 418.0f, 1.08f, 6.94f, 8.04f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 8.73f;
            fArray2[2] = 44.36f;
            fArray2[4] = 418.0f;
            fArray2[5] = 8.73f;
            fArray2[6] = 47.8f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut004() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, -23.43f, 15.52f, 58.07f, 43.0f, 4.25f, 9.54f, 25.45f, 85.0f, 9.2f, 4.51f, -12.16f};
            float[] fArray2 = new float[]{1.0f, -5.0f, 458.23f, 7.01f, 43.0f, -5.04f, 470.03f, 7.01f, 85.0f, -3.75f, 490.25f, 7.01f};
            SCE02016.this.cam1.transSPL(fArray, 1, 1, 92);
            SCE02016.this.cam1.rotateSPL(fArray2, 1, 1, 92);
        }

        public void cut005() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, -34.06f, 18.11f, 68.1f, 92.0f, -33.2f, 18.11f, 62.96f};
            float[] fArray2 = new float[]{1.0f, -14.5f, 350.45f, 7.01f, 92.0f, -14.5f, 350.45f, 7.01f};
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut005_momo() {
            SCE02016.this.cam2.setFov(25.28f);
            float[] fArray = new float[]{0.9999999f, -0.42489254f, 5.856386f, 73.61913f, 226.99998f, -0.42977554f, 5.8640466f, 73.60626f};
            float[] fArray2 = new float[]{0.9999999f, 25.405743f, 19.98367f, -2.1299996f, 5.9999995f, 26.499805f, 20.632372f, -2.1299992f, 10.999999f, 27.02561f, 20.799711f, -2.129999f, 15.999998f, 27.16794f, 20.865414f, -2.129999f, 20.999998f, 27.211632f, 20.850916f, -2.129999f};
            float[] fArray3 = new float[]{0.9999999f, 25.28f, 226.99998f, 22.28f};
            SCE02016.this.cam2.fovSPL(fArray3, 1);
            SCE02016.this.cam2.transSPL(fArray, 1);
            SCE02016.this.cam2.rotateSPL(fArray2, 1);
        }

        public void cut006() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, 4.7f, -20.01f, -100.82f, 68.0f, 2.41f, -20.01f, -102.44f};
            float[] fArray2 = new float[]{1.0f, 6.65f, 510.87f, 7.01f, 68.0f, 6.02f, 511.7f, 7.01f};
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut007() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, -5.27f, -14.86f, -76.89f};
            float[] fArray2 = new float[]{1.0f, -19.34f, 371.5f, 7.01f};
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut007_ziggy() {
            SCE02016.this.cam2.setFov(40.8f);
            SCE02016.this.cam0.setFov(40.8f);
            float[] fArray = new float[]{1.0f, 0.57f, 6.96f, 6.36f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 5.93f;
            fArray2[2] = 167.53f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam2.transSPL(fArray, 1);
            SCE02016.this.cam2.rotateSPL(fArray3, 1);
        }

        public void cut008() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, 11.14f, 15.18f, 46.16f, 177.0f, 12.93f, 15.18f, 43.6f};
            float[] fArray2 = new float[]{1.0f, -14.99f, 415.61f, 7.01f, 177.0f, -14.26f, 413.48f, 7.01f};
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut009() {
            SCE02016.this.cam2.setFov(19.507978f);
            float[] fArray = new float[]{1.0f, -0.10966166f, 2.6118183f, -3.9496045f};
            float[] fArray2 = new float[]{1.0f, -4.0772777f, -181.70932f, 15.136129f};
            SCE02016.this.cam2.transSPL(fArray, 1);
            SCE02016.this.cam2.rotateSPL(fArray2, 1);
        }

        public void cut01() {
            SCE02016.this.cam1.setFov(42.12f);
            SCE02016.this.cam2.setFov(42.12f);
            SCE02016.this.cam0.setFov(42.12f);
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
            SCE02016.this.cam1.transSPL(fArray, 1, 3, 46);
            SCE02016.this.cam1.rotateSPL(fArray3, 1, 3, 46);
        }

        public void cut01_ato() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, 7.02f, 7.28f, -275.37f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -14.9f;
            fArray2[2] = -176.92f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam1.transSPL(fArray, 0);
            SCE02016.this.cam1.rotateSPL(fArray3, 0);
        }

        public void cut01_mae() {
            SCE02016.this.cam1.setFov(42.15f);
            SCE02016.this.cam0.setFov(42.15f);
            float[] fArray = new float[]{1.0f, -38.14f, 5.23f, 105.73f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.66f;
            fArray2[2] = -21.11f;
            float[] fArray3 = fArray2;
            SCE02016.this.cam1.transSPL(fArray, 1);
            SCE02016.this.cam1.rotateSPL(fArray3, 1);
        }
    }
}

