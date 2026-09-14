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
import xeno.map.MC_DYU01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE03041
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        JNT_Human,
        Pack03041,
        FLSjr_h,
        FLSshion_h,
        FLSmary,
        FLSshelley,
        FLSgaignun_h,
        FLSallen,
        MC_DYU01_PRJ {
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final float nc = 1.0E7f;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    Menu menu;
    Window win;
    Thread thread1;
    Thread _spl_thread_main;
    Input pad1;
    Input pad0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camerawork camerawork = new Camerawork();
    PlayControl pc;
    Effect kemuri1;
    mapunits pod_21;
    robo shion;
    robo allen;
    robo j___r;
    robo gaiun;
    robo shely;
    robo mary;
    robo guno_17;
    robo guno_18;
    robo guno_19a;
    robo guno_19b;
    robo guno_19e;
    units dummy1;
    units dummy2;
    units dummy3;
    units dummy4;
    units dummy5;
    units dummy6;
    mapunits isu;
    mapunits ring_1a;
    mapunits ring_1b;
    mapunits ring_1c;
    mapunits ring_2a;
    mapunits ring_2b;
    mapunits ring_2c;
    units fm1;
    units fm2;
    units star;
    int kubi_flag = 0;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE03041() {
    }

    void FACE(int n, robo robo2, int n2) {
        robo2.face.mtn(n2, 8, 1.0f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 8, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, robo robo2, int n2) {
        robo2.face.mtn(n2, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 0, 120, 7, 9, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void MSGW(String string) {
        this.msg.clear();
        this.msg.print(string);
    }

    void TP_FACE(robo robo2, int n, float f) {
        robo2.face.mtn(n, 8, f, false);
        robo2.face.start(4, null);
    }

    void TP_FACE_SMOOTH(robo robo2, int n, float f) {
        robo2.face.mtn(n, 0, 120, 7, 9, f, false);
        robo2.face.start(4, null);
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
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03041_F");
        Runtime.setFlags(367, 1, 1);
        System.println("XEVEJNAME:SCE03039C");
        Runtime.jumpEvent(3392);
    }

    void init() {
        Runtime.setLocation(800);
        this.pc = PlayControl.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.shion = new robo();
        this.shion.init(0x100001E, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shion.face = this.shion.getChild(0x1000000);
        this.allen = new robo();
        this.allen.init(0x1000107, 0.0f, 0.0f, 0.0f, 0.0f);
        this.allen.face = this.allen.getChild(0x1000000);
        this.j___r = new robo();
        this.j___r.init(0x1000022, 0.0f, 0.0f, 0.0f, 0.0f);
        this.j___r.face = this.j___r.getChild(0x1000000);
        this.gaiun = new robo();
        this.gaiun.init(16777555, 0.0f, 0.0f, 0.0f, 0.0f);
        this.gaiun.face = this.gaiun.getChild(0x1000000);
        this.shely = new robo();
        this.shely.init(0x1000121, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shely.face = this.shely.getChild(0x1000000);
        this.mary = new robo();
        this.mary.init(0x1000120, 0.0f, 0.0f, 0.0f, 0.0f);
        this.mary.face = this.mary.getChild(0x1000000);
        this.shion.setShadow(0, 0);
        this.allen.setShadow(0, 0);
        this.j___r.setShadow(0, 0);
        this.gaiun.setShadow(0, 0);
        this.shely.setShadow(0, 0);
        this.mary.setShadow(0, 0);
        this.guno_17 = new robo();
        this.guno_17.init(20498, 0.0f, 0.0f, 0.0f, 0.0f);
        this.guno_18 = new robo();
        this.guno_18.init(20499, 0.0f, 0.0f, 0.0f, 0.0f);
        this.guno_19a = new robo();
        this.guno_19a.init(20500, 0.0f, 0.0f, 0.0f, 0.0f);
        this.guno_19b = new robo();
        this.guno_19b.init(20500, 0.0f, 0.0f, 0.0f, 0.0f);
        this.guno_19e = new robo();
        this.guno_19e.init(20500, 0.0f, 0.0f, 0.0f, 0.0f);
        this.guno_17.setShadow(0, 0);
        this.guno_18.setShadow(0, 0);
        this.guno_19a.setShadow(0, 0);
        this.guno_19b.setShadow(0, 0);
        this.guno_19e.setShadow(0, 0);
        this.shion.setVisible(false);
        this.allen.setVisible(false);
        this.j___r.setVisible(false);
        this.gaiun.setVisible(false);
        this.shely.setVisible(false);
        this.mary.setVisible(false);
        this.guno_17.setVisible(false);
        this.guno_18.setVisible(false);
        this.guno_19a.setVisible(false);
        this.guno_19b.setVisible(false);
        this.guno_19e.setVisible(false);
        this.shion.setMotNoUpdate(2);
        this.j___r.setMotNoUpdate(2);
        this.shely.setMotNoUpdate(2);
        this.shion.setMotionFlags(0x2000000, true);
        this.j___r.setMotionFlags(0x2000000, true);
        this.shely.setMotionFlags(0x2000000, true);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy3 = new units();
        this.dummy3.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy4 = new units();
        this.dummy4.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this.dummy5 = new units();
        this.dummy5.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy5.setVisible(false);
        this.dummy6 = new units();
        this.dummy6.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy6.setVisible(false);
        this.star = new units();
        this.star.init(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.star.setScale(1.0f, 1.0f, 1.0f);
        this.isu = new mapunits();
        this.isu.init(73);
        this.isu.start(4, null);
        this.isu.setPivot(-0.5328f, 0.0f, 7.653f);
        this.isu.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1a = new mapunits();
        this.ring_1a.init(120);
        this.ring_1a.start(4, null);
        this.ring_1b = new mapunits();
        this.ring_1b.init(121);
        this.ring_1b.start(4, null);
        this.ring_1c = new mapunits();
        this.ring_1c.init(122);
        this.ring_1c.start(4, null);
        this.ring_2a = new mapunits();
        this.ring_2a.init(123);
        this.ring_2a.start(4, null);
        this.ring_2b = new mapunits();
        this.ring_2b.init(124);
        this.ring_2b.start(4, null);
        this.ring_2c = new mapunits();
        this.ring_2c.init(125);
        this.ring_2c.start(4, null);
        this.fm1 = new units();
        this.fm1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.setArgs(0, 0.0f, 0.0f, 1.6f, 0.0f);
        this.fm1.setArgs(1, 22044, 0, 128, 112);
        this.fm1.setArgs(2, 76, 0, 15, -4);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.setScale(3.27f, 3.27f, 3.27f);
        this.fm1.setTranslate(0.0f, 4.349987f, -0.15001117f);
        this.fm1.setRotate(29.999996f, 0.0f, 0.0f);
        this.fm2 = new units();
        this.fm2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm2.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.fm2.setArgs(1, 22037, 0, 128, 112);
        this.fm2.setArgs(2, 76, 0, 15, -5);
        this.fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm2.setScale(0.44f, 0.2f, 0.58f);
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
        Stage.setEventFade(30, 0.0f, 0.0f, 0.0f, 2, 0.0f, 0.0f, 0.0f);
        Stage.setVisible(0, false);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.isu.setTranslate(-0.13749988f, -0.037499975f, 0.13749991f);
        this.isu.setRotate(0.0f, 89.99999f, 0.0f);
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.j___r.face, "FLSjr_h.fpk");
        this.loadarc(this.gaiun.face, "FLSgaignun_h.fpk");
        this.loadarc(this.shely.face, "FLSshelley.fpk");
        this.loadarc(this.mary.face, "FLSmary.fpk");
        this.shion.face.mtn(26, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.allen.face.mtn(8, 8, 1.0f, false);
        this.allen.face.start(4, null);
        this.j___r.face.mtn(6, 8, 1.0f, false);
        this.j___r.face.start(4, null);
        this.gaiun.face.mtn(2, 8, 1.0f, false);
        this.gaiun.face.start(4, null);
        this.shely.face.mtn(2, 8, 1.0f, false);
        this.shely.face.start(4, null);
        this.mary.face.mtn(2, 8, 1.0f, false);
        this.mary.face.start(4, null);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        Stage.renderCommand(22);
        this.mary.renderCommand(22);
        this.allen.renderCommand(22);
        this.j___r.renderCommand(22);
        this.cam1.change();
        Sound.streamPlay(1390078, 48000);
        this.shion.setVisible(true);
        this.j___r.setVisible(true);
        this.allen.setVisible(true);
        this.mary.setVisible(true);
        this.allen.mtn(281, 8, 1.0f, false);
        this.allen.start(5, null);
        this.j___r.mtn(282, 8, 1.0f, false);
        this.j___r.start(5, null);
        this.mary.mtn(284, 8, 1.0f, false);
        this.mary.start(5, null);
        this.shion.mtn(257, 8, 1.0f, false);
        this.shion.start(5, null);
        this.pc.loadCamera("c03s41cut01.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, 0.438f, 0.438f, -0.785f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.88f, 0.0f, 0.475f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.583f, -0.332f, -0.742f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shely.setMotNoUpdate(2);
        this.shely.start(1, "c03s41cut01bshelley_mae");
        System.sleep(12);
        this.MSGW("The Federation fleet is\nsurrounding us again!");
        System.sleep(99);
        this.MSGW("Now what're they up to?!");
        this.dummy1.start(1, "shion_serifu_01");
        System.sleep(99);
        this.wait_clr(25);
        this.__wait();
        this.allen.renderCommand(0);
        this.j___r.renderCommand(0);
        this.gaiun.renderCommand(0);
        this.shely.renderCommand(0);
        this.mary.renderCommand(0);
        Stage.renderCommand(0);
        this.mary.setVisible(false);
        this.j___r.setVisible(false);
        this.allen.setVisible(false);
        this.shion.setVisible(false);
        this.shely.setVisible(true);
        this.shely.mtn(283, 8, 1.0f, false);
        this.shely.start(5, null);
        this.pc.loadCamera("c03s41cut01b.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.733f, 0.239f, -0.637f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.019f, 0.902f, 0.43f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.68f, -0.4f, 0.615f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(9);
        this.dummy2.start(1, "shely_serifu_01b");
        this.MSGW("I think I know.\nLook at the movements of the Gnosis.");
        System.sleep(81);
        this.wait_clr(24);
        this.shely.setMotNoUpdate(0);
        this.__wait();
        this.pc.loadCamera("c03s41cut02.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.dummy3.start(1, "main_moni_on");
        this.shely.setMotNoUpdate(2);
        this.shely.start(1, "C03S41CUT03ASHELLEY_mae");
        System.sleep(15);
        System.sleep(36);
        this.MSGW("The pattern of the Gnosis gate-outs\nare near the center of the Foundation.");
        System.sleep(99);
        this.wait_clr(18);
        this.__wait();
        this.isu.setTranslate(0.05000023f, -0.012496947f, 0.1749996f);
        this.isu.setRotate(0.0f, 42.499996f, 0.0f);
        this.gaiun.setShadow(5, 22);
        this.gaiun.setVisible(true);
        this.gaiun.mtn(258, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.shely.mtn(259, 8, 1.0f, false);
        this.shely.start(5, null);
        this.pc.loadCamera("c03s41cut03a.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, 0.114f, 0.564f, -0.818f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.811f, 0.0f, 0.586f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.841f, -0.475f, -0.259f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(9);
        this.dummy1.start(1, "gaiun_serifu_03a");
        this.MSGW("What about the Zohars in the hangar?!");
        System.sleep(51);
        this.wait_clr(11);
        this.shely.setMotNoUpdate(0);
        this.__wait();
        this.fm2.setTranslate(5.6499887f, 1.1124973f, 13.249963f);
        this.fm2.setRotate(0.0f, -44.999996f, 0.0f);
        this.fm2.signal(1);
        this.gaiun.setShadow(0, 0);
        this.gaiun.setVisible(false);
        this.shely.setVisible(false);
        this.mary.setVisible(true);
        this.mary.mtn(260, 8, 1.0f, false);
        this.mary.start(5, null);
        this.pc.loadCamera("c03s41cut03b.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.187f, 0.027f, -0.982f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.108f, 0.851f, -0.513f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.358f, -0.34f, 0.87f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.dummy2.start(1, "mary_serifu_03b");
        this.MSGW("They're in stasis!\nAttract Inhibitor output is normal!");
        System.sleep(90);
        this.wait_clr(21);
        this.__wait();
        this.fm2.signal(0);
        this.gaiun.setVisible(true);
        this.gaiun.mtn(261, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.mary.mtn(260, 8, 1.0f, false);
        this.mary.start(5, null);
        this.pc.loadCamera("c03s41cut04.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, 0.047f, 0.349f, -0.936f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.959f, 0.0f, 0.284f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, 0.358f, -0.891f, -0.278f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(6);
        this.MSGW("So if the Zohars in the Durandal\naren't calling the Gnosis, then...");
        this.dummy1.start(1, "gaiun_serifu_04");
        System.sleep(102);
        System.sleep(24);
        this.MSGW("Gaignun...It's that Song we've\nbeen hearing, isn't it...?");
        System.sleep(42);
        this.gaiun.look_eye_speed(2.72f);
        this.gaiun.look_eye_set(-22.0f, 3.0f);
        System.sleep(126);
        this.wait_clr(20);
        this.__wait();
        Stage.setVisible(140, false);
        this.mary.setVisible(false);
        this.gaiun.mtn(262, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.pc.loadCamera("c03s41cut05.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.398f, -0.243f, 0.885f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setMotNoUpdate(2);
        this.shion.start(1, "C03S41CUT06_07SHION_mae");
        this.shion.face.mtn(22, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.gaiun.look_eye_set(-24.75f, 0.0f);
        System.sleep(3);
        this.MSGW("Indeed. It's faint, but there's no\nmistaking it. No one else seems\nable to hear it, though...");
        System.sleep(177);
        this.wait_clr(12);
        this.__wait();
        Stage.setVisible(140, true);
        this.gaiun.setVisible(false);
        this.allen.setVisible(true);
        this.j___r.setVisible(true);
        this.shion.setVisible(true);
        this.allen.mtn(263, 8, 1.0f, false);
        this.allen.start(5, null);
        this.j___r.mtn(264, 8, 1.0f, false);
        this.j___r.start(5, null);
        this.shion.mtn(265, 8, 1.0f, false);
        this.shion.start(5, null);
        this.pc.loadCamera("c03s41cut06_07.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.dummy6.start(1, "light_change_cut06_07");
        this.allen.look_speed(0.0f);
        this.allen.look_eye_speed(5.2f);
        this.allen.look_char(this.shion);
        this.gaiun.look_eye_set(-3.5f, 9.5f);
        this.gaiun.face.mtn(10, 8, 1.0f, false);
        this.gaiun.face.start(4, null);
        System.sleep(18);
        this.dummy1.start(1, "shion_serifu_06_07");
        this.MSGW("Hey, do you guys hear something?\nIt sounds...like a...song?");
        System.sleep(225);
        System.sleep(12);
        this.MSGW("A...song?");
        this.dummy2.start(1, "allen_serifu_06_07");
        this.wait_clr(45);
        System.sleep(24);
        this.j___r.look_eye_speed(5.2f);
        this.j___r.look_eye_set(-0.75f, -4.5f);
        this.j___r.face.mtn(2, 8, 1.0f, false);
        this.j___r.face.start(4, null);
        this.wait_clr(6);
        this.j___r.face.mtn(6, 0, 120, 12, 9, 1.0f, false);
        this.j___r.face.start(4, null);
        this.wait_clr(24);
        this.shion.setMotNoUpdate(0);
        this.__wait();
        this.allen.setVisible(false);
        this.j___r.setVisible(false);
        this.shion.setVisible(false);
        this.gaiun.setVisible(true);
        this.gaiun.mtn(266, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.pc.loadCamera("c03s41cut08.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.766f, 0.014f, -0.643f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.994f, 0.009f, 0.109f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.236f, 0.0f, 0.972f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.j___r.setMotNoUpdate(2);
        this.j___r.start(1, "C03S41CUT09_10_10AJR_mae");
        this.j___r.look_eye_set(4.0f, -9.5f);
        System.sleep(30);
        this.__wait();
        this.gaiun.setVisible(false);
        this.allen.setVisible(true);
        this.j___r.setVisible(true);
        this.shion.setVisible(true);
        this.allen.mtn(267, 8, 1.0f, false);
        this.allen.start(5, null);
        this.j___r.mtn(268, 8, 1.0f, false);
        this.j___r.start(5, null);
        this.shion.mtn(269, 8, 1.0f, false);
        this.shion.start(5, null);
        this.pc.loadCamera("c03s41cut09_10_10a.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.dummy6.start(1, "light_change_cut09_10");
        this.shion.face.mtn(18, 8, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(6);
        this.MSGW("I don't hear anything.\nYou sure you aren't imagining things?");
        this.dummy1.start(1, "allen_serifu_09_10");
        System.sleep(132);
        this.MSGW("I don't know...\nI swear I can hear it...");
        this.dummy1.start(1, "shion_serifu_09_10");
        System.sleep(41);
        System.sleep(15);
        this.shion.look_eye_speed(1.0f);
        this.shion.look_eye_set(6.0f, -3.5f);
        System.sleep(90);
        System.sleep(12);
        this.j___r.look_eye_set(4.75f, -8.0f);
        this.MSGW("She hears it?\nHow's that possible?");
        System.sleep(90);
        this.wait_clr(24);
        this.j___r.setMotNoUpdate(0);
        this.__wait();
        this.allen.setVisible(false);
        this.j___r.setVisible(false);
        this.shion.setVisible(false);
        this.gaiun.setVisible(true);
        this.gaiun.setShadow(5, 22);
        this.shely.setVisible(true);
        this.isu.setTranslate(-0.03749975f, -0.012496947f, 0.124999516f);
        this.isu.setRotate(0.0f, 88.749985f, 0.0f);
        this.shely.mtn(283, 8, 0.27f, false);
        this.shely.start(5, null);
        this.gaiun.mtn(270, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.pc.loadCamera("c03s41cut11a.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.09f, 0.782f, -0.616f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.994f, 0.009f, 0.109f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.236f, 0.0f, 0.972f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.j___r.look_eye_set(3.75f, 6.05f);
        System.sleep(42);
        this.MSGW("Why...am I feeling so unsettled?");
        System.sleep(120);
        this.MSGW("This image...\njust won't leave my mind...");
        this.wait_clr(162);
        this.__wait();
        this.shely.setVisible(false);
        this.gaiun.setShadow(0, 0);
        this.gaiun.setVisible(false);
        this.j___r.setVisible(true);
        this.j___r.mtn(271, 8, 1.0f, false);
        this.j___r.start(5, null);
        this.pc.loadCamera("c03s41cut11b_12.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.dummy6.start(1, "light_change_cut11B_12");
        this.gaiun.look_eye_set(-7.5f, 8.7f);
        this.j___r.look_eye_speed(0.22f);
        this.j___r.look_eye_set(-1.0f, 4.6f);
        this.MSGW("You too? This darkness...\nI feel...like I'm being dragged\ndown into an abyss...");
        this.wait_clr(252);
        this.j___r.look_eye_speed(2.7f);
        this.j___r.look_eye_set(-1.0f, -5.8f);
        this.wait_clr(10);
        this.j___r.look_eye_set(-1.0f, -6.5f);
        this.wait_clr(15);
        this.__wait();
        this.gaiun.setVisible(true);
        this.j___r.setVisible(false);
        this.guno_17.setVisible(true);
        this.guno_18.setVisible(true);
        this.guno_18.setVisible(true);
        this.guno_19a.setVisible(true);
        this.guno_19b.setVisible(true);
        this.guno_19e.setVisible(true);
        this.gaiun.renderCommand(22);
        this.shely.renderCommand(22);
        Stage.renderCommand(22);
        this.guno_19a.renderCommand(22);
        this.guno_19b.renderCommand(22);
        this.shely.setMotNoUpdate(0);
        this.shely.setVisible(true);
        this.gaiun.mtn(272, 8, 1.0f, false);
        this.gaiun.start(5, null);
        this.guno_17.mtn(273, 8, 1.0f, false);
        this.guno_17.start(5, null);
        this.guno_18.mtn(274, 8, 1.0f, false);
        this.guno_18.start(5, null);
        this.guno_19a.mtn(275, 8, 1.0f, false);
        this.guno_19a.start(5, null);
        this.guno_19b.mtn(276, 8, 1.0f, false);
        this.guno_19b.start(5, null);
        this.guno_19e.mtn(279, 8, 1.0f, false);
        this.guno_19e.start(5, null);
        this.pc.loadCamera("c03s41cut13_14.cam");
        this.pc.init(1, 0);
        this.__wait();
        this.pc.start();
        this.star.start(1, "amanogawa");
        this.dummy6.start(1, "light_change_cut13_14");
        Stage.setVisible(58, false);
        Stage.setVisible(59, false);
        Stage.setVisible(60, false);
        Stage.setVisible(61, false);
        Stage.setVisible(62, false);
        Stage.setVisible(63, false);
        Stage.setVisible(64, false);
        Stage.setVisible(65, false);
        Stage.setVisible(70, false);
        Stage.setVisible(71, false);
        Stage.setVisible(72, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        Stage.setVisible(128, false);
        Stage.setVisible(129, false);
        Stage.setVisible(130, false);
        Stage.setVisible(131, false);
        Stage.setVisible(132, false);
        Stage.setVisible(137, false);
        Stage.setVisible(138, false);
        Stage.setVisible(141, false);
        Stage.setVisible(142, false);
        this.gaiun.look_eye_speed(5.2f);
        this.gaiun.look_eye_set(-4.2f, -8.1f);
        System.sleep(10);
        Runtime.setDefocusQuick(0, 4, 122880, 52);
        this.MSGW("It can't be...!");
        System.sleep(5);
        this.shely.face.setVisible(false);
        this.dummy5.start(1, "bura_down");
        this.wait_clr(51);
        this.wait_clr(9);
        this.__wait();
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class robo
            extends Chr {
        Chr face;

        robo() {
        }

        void C03S41CUT03ASHELLEY_mae() {
            SCE03041.this.shely.mtn(259, 0, 0, 0, 0, 1.0f, true);
        }

        void C03S41CUT06_07SHION_mae() {
            SCE03041.this.shion.mtn(265, 0, 0, 0, 0, 1.0f, true);
        }

        void C03S41CUT09_10_10AJR_mae() {
            SCE03041.this.j___r.mtn(268, 0, 0, 0, 0, 1.0f, true);
        }

        void c03s41cut01bshelley_mae() {
            SCE03041.this.shely.mtn(283, 0, 0, 0, 0, 1.0f, true);
        }

        void mobj_017() {
            this.mtn(285, 8, 1.0f, true);
        }

        void mobj_018() {
            this.mtn(286, 8, 1.0f, true);
        }

        void mobj_019() {
            this.mtn(287, 8, 1.0f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }

        void allen_serifu_06_07() {
            SCE03041.this.FACE_SMOOTH(18, SCE03041.this.allen, 1, 0.72f);
            System.sleep(12);
            SCE03041.this.FACE_SMOOTH(18, SCE03041.this.allen, 7, 0.87f);
        }

        void allen_serifu_09_10() {
            SCE03041.this.FACE_SMOOTH(48, SCE03041.this.allen, 1, 0.92f);
            System.sleep(22);
            SCE03041.this.FACE(36, SCE03041.this.allen, 1, 1.1f);
        }

        void amanogawa() {
            SCE03041.this.star.setTranslate(0.0f, 0.0f, 0.0f);
            SCE03041.this.star.setRotate(75.0f, -7.5f, 0.0f);
            while (true) {
                this.setTranslate(SCE03041.this.star.px, SCE03041.this.star.py, SCE03041.this.star.pz + 2.2f);
                System.sleep(1);
            }
        }

        void bura_down() {
            int n = 52;
            while (n > 0) {
                Runtime.setDefocusQuick(0, 4, 122880, n);
                System.sleep(1);
                --n;
            }
            Runtime.setDefocusQuick(0, 0, 0, 0);
        }

        void gaiun_serifu_03a() {
            SCE03041.this.FACE_SMOOTH(39, SCE03041.this.gaiun, 1, 1.0f);
        }

        void gaiun_serifu_04() {
            SCE03041.this.FACE(39, SCE03041.this.gaiun, 1, 1.0f);
            System.sleep(12);
            SCE03041.this.FACE(48, SCE03041.this.gaiun, 1, 1.0f);
        }

        void light_change_cut06_07() {
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, 0.348f, 0.031f, -0.937f);
            SCE03041.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE03041.this.light.setDirection2(2, 0.616f, 0.572f, 0.542f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.608f, -0.324f, 0.725f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            System.sleep(325);
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, -0.098f, 0.231f, -0.968f);
            SCE03041.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE03041.this.light.setDirection2(2, 0.422f, 0.336f, 0.842f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.979f, -0.203f, 0.018f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        void light_change_cut09_10() {
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, 0.071f, 0.341f, -0.937f);
            SCE03041.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE03041.this.light.setDirection2(2, 0.731f, -0.682f, 0.0f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.676f, -0.0f, 0.737f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            System.sleep(196);
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, 0.101f, 0.243f, -0.965f);
            SCE03041.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE03041.this.light.setDirection2(2, 0.348f, 0.572f, 0.743f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.866f, -0.472f, 0.167f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            System.sleep(102);
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, -0.098f, 0.231f, -0.968f);
            SCE03041.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE03041.this.light.setDirection2(2, 0.422f, 0.336f, 0.842f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.979f, -0.203f, 0.018f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        void light_change_cut11B_12() {
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.64f, 0.64f, 0.64f);
            SCE03041.this.light.setDirection2(1, -0.343f, 0.0f, -0.939f);
            SCE03041.this.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.light.setDirection2(2, 0.608f, 0.757f, 0.238f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.517f, -0.713f, -0.474f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            System.sleep(263);
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, -0.437f, 0.46f, -0.773f);
            SCE03041.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE03041.this.light.setDirection2(2, 0.376f, 0.128f, 0.918f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, 0.971f, -0.229f, 0.07f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        void light_change_cut13_14() {
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, 0.87f, 0.34f, -0.357f);
            SCE03041.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE03041.this.light.setDirection2(2, 0.232f, -0.335f, 0.913f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, -0.774f, -0.001f, 0.633f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE03041.this.guno_17.setLightMode(1);
            SCE03041.this.guno_17.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03041.this.guno_17.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03041.this.guno_17.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.guno_17.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_17.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.guno_17.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_17.light.setDirection2(3, -0.047f, -0.961f, 0.273f);
            SCE03041.this.guno_18.setLightMode(1);
            SCE03041.this.guno_18.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03041.this.guno_18.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03041.this.guno_18.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.guno_18.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_18.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.guno_18.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_18.light.setDirection2(3, -0.047f, -0.961f, 0.273f);
            SCE03041.this.guno_19a.setLightMode(1);
            SCE03041.this.guno_19a.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03041.this.guno_19a.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03041.this.guno_19a.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.guno_19a.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19a.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.guno_19a.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19a.light.setDirection2(3, -0.047f, -0.961f, 0.273f);
            SCE03041.this.guno_19b.setLightMode(1);
            SCE03041.this.guno_19b.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03041.this.guno_19b.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03041.this.guno_19b.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.guno_19b.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19b.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.guno_19b.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19b.light.setDirection2(3, -0.047f, -0.961f, 0.273f);
            SCE03041.this.guno_19e.setLightMode(1);
            SCE03041.this.guno_19e.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03041.this.guno_19e.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03041.this.guno_19e.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.guno_19e.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19e.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.guno_19e.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03041.this.guno_19e.light.setDirection2(3, -0.047f, -0.961f, 0.273f);
            System.sleep(16);
            SCE03041.this.light.setColor(0, 0.16f, 0.16f, 0.16f);
            SCE03041.this.light.setColor(1, 0.69f, 0.69f, 0.69f);
            SCE03041.this.light.setDirection2(1, -0.897f, 0.005f, -0.442f);
            SCE03041.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE03041.this.light.setDirection2(2, 0.784f, 0.567f, 0.252f);
            SCE03041.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03041.this.light.setDirection2(3, -0.398f, -0.243f, 0.885f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        void main_moni_on() {
            SCE03041.this.fm1.signal(1);
            int n = 0;
            float f = 0.0f;
            while (n < 9) {
                SCE03041.this.fm1.setArgs(0, 0.0f, 0.0f, 1.6f, f);
                ++n;
                f += 0.15555555f;
                System.sleep(1);
            }
            SCE03041.this.fm1.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        }

        void mary_serifu_03b() {
            SCE03041.this.FACE_SMOOTH(24, SCE03041.this.mary, 5, 1.0f);
            System.sleep(12);
            SCE03041.this.FACE_SMOOTH(75, SCE03041.this.mary, 5, 1.0f);
        }

        void shely_serifu_01b() {
            SCE03041.this.FACE_SMOOTH(39, SCE03041.this.shely, 1, 1.0f);
            System.sleep(24);
            SCE03041.this.FACE_SMOOTH(60, SCE03041.this.shely, 1, 1.0f);
        }

        void shion_serifu_01() {
            SCE03041.this.FACE_SMOOTH(24, SCE03041.this.shion, 25, 0.72f);
            System.sleep(15);
            SCE03041.this.FACE_SMOOTH(60, SCE03041.this.shion, 23, 1.12f);
        }

        void shion_serifu_06_07() {
            SCE03041.this.FACE_SMOOTH(24, SCE03041.this.shion, 11, 0.72f);
            System.sleep(27);
            SCE03041.this.FACE(48, SCE03041.this.shion, 11, 1.0f);
            System.sleep(48);
            SCE03041.this.FACE_SMOOTH(36, SCE03041.this.shion, 15, 0.72f);
            System.sleep(24);
            SCE03041.this.FACE_SMOOTH(21, SCE03041.this.shion, 7, 0.72f);
        }

        void shion_serifu_09_10() {
            SCE03041.this.FACE_SMOOTH(27, SCE03041.this.shion, 15, 0.82f);
            System.sleep(46);
            SCE03041.this.FACE_SMOOTH(63, SCE03041.this.shion, 7, 0.87f);
        }
    }

    class mapunits
            extends MAPUnit {
        mapunits() {
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut001() {
            SCE03041.this.cam1.setFov(29.54f);
            float[] fArray = new float[]{1.0f, -0.3532f, 1.3727778f, -1.9938f, 554.0f, -0.3532f, 1.1227778f, -1.993809f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.782f;
            fArray2[2] = -168.594f;
            float[] fArray3 = fArray2;
            SCE03041.this.cam1.transSPL(fArray, 0, 2, 554);
            SCE03041.this.cam1.rotateSPL(fArray3, 1, 2, 1);
        }
    }
}

