import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_PRO07_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02000
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02000,
        MC_PRO07_PRJ,
        FLSmarglis_h,
        FLSpelegri {
    static final int MovieNum = 9019;
    static final int MovieWaitFull = 870;
    static final int MovieWait = 630;
    Characters marglis;
    Characters pelegri;
    Characters op1;
    Characters op2;
    Characters op3;
    Characters op4;
    Unit monitor0;
    Unit opmoni1;
    Unit opmoni2;
    Unit opmoni3;
    Unit opmoni4;
    Unit sidemoni1;
    Unit sidemoni2;
    Unit bmoni;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int cut_length;
    Light light = new Light(0);
    Effect FadeIn;
    Effect FadeOut;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02000() {
    }

    void CameraThread() {
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        this.cam1.change();
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(2);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(1.16f, 2.07f, -2.76f);
        this.cam1.setRotate(-9.5f, -31.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.16f, 2.11f, -2.76f);
        this.waitCameraPlay(3);
        Runtime.setDefocusQuick(0, 1, 34880, 1);
        this.cam1.setTranslate(2.86f, 1.12f, -0.23f);
        this.cam1.setRotate(10.88f, 26.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        Runtime.setDefocusQuick(0, 1, 96880, 1);
        this.cam1.setTranslate(1.86f, 2.06f, -2.35f);
        this.cam1.setRotate(-9.62f, 13.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        Runtime.setDefocusQuick(0, 1, 98880, 1);
        this.cam1.setTranslate(1.96f, 1.8f, -2.9f);
        this.cam1.setRotate(9.38f, 163.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.96f, 1.93f, -2.9f);
        this.waitCameraPlay(6);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(-5.99f, 2.9f, -8.0f);
        this.cam1.setRotate(-22.62f, 182.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -6.03f, 2.9f, -8.97f);
        this.waitCameraPlay(7);
        Runtime.setDefocusQuick(0, 1, 29880, 1);
        this.cam1.setTranslate(5.9f, 1.35f, -0.08f);
        this.cam1.setRotate(2.38f, 62.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 5.98f, 1.35f, -0.29f);
        this.waitCameraPlay(8);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(2.22f, 1.95f, -2.54f);
        this.cam1.setRotate(-1.12f, 35.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(9);
        Runtime.setDefocusQuick(0, 1, 45880, 1);
        Runtime.setDefocusQuick(1, 2, 102264, 2);
        this.cam1.setTranslate(2.21f, 1.75f, -4.38f);
        this.cam1.setRotate(4.88f, 162.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 2.21f, 1.8f, -4.38f);
        this.waitCameraPlay(10);
        Runtime.setDefocusQuick(0, 1, 168880, 1);
        this.cam1.setTranslate(1.87f, 1.76f, -2.43f);
        this.cam1.setRotate(14.88f, 18.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        Runtime.setDefocusQuick(0, 1, 77880, 1);
        this.cam1.setTranslate(2.99f, 1.22f, -3.02f);
        this.cam1.setRotate(22.88f, 133.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 2.93f, 1.22f, -3.1f);
        this.waitCameraPlay(12);
        Runtime.setDefocusQuick(0, 1, 48880, 1);
        Runtime.setDefocusQuick(1, 2, 89264, 2);
        this.cam1.setTranslate(2.63f, 1.44f, -0.87f);
        this.cam1.setRotate(6.38f, 28.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(1.97f, 1.91f, -2.03f);
        this.cam1.setRotate(-1.12f, 17.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(2.88f, 2.36f, -2.78f);
        this.cam1.setRotate(-12.62f, 113.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(15);
        Runtime.setDefocusQuick(0, 1, 56880, 1);
        this.cam1.setTranslate(-1.13f, 1.9f, -2.14f);
        this.cam1.setRotate(-16.12f, 123.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(16);
        Runtime.setDefocusQuick(0, 1, 25880, 1);
        this.cam1.setTranslate(-4.13f, 1.7f, -1.67f);
        this.cam1.setRotate(0.38f, 275.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(17);
        Runtime.setDefocusQuick(0, 1, 120380, 1);
        this.cam1.setTranslate(0.96f, 1.8f, -2.79f);
        this.cam1.setRotate(14.88f, 222.11f, 0.0f);
        this.cam1.setFov(30.0f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
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
        System.println("XEVEJNAME:SCE02001A");
        Runtime.jumpEvent(2010);
    }

    void face(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void face(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
        chr.mtn(n, n2, n3, n4, n5, f, false);
        chr.start(4, null);
    }

    void init() {
        Runtime.setLocation(1106);
        this.marglis = new Characters(16777530, 1.7f, 0.5f, -0.7f, -180.0f);
        this.marglis.setShadow(5, 32);
        this.pelegri = new Characters(16777516, 1.65f, 0.5f, -3.45f, -56.0f);
        this.pelegri.setShadow(5, 32);
        this.op1 = new Characters(778, -5.13f, 0.0f, 0.63f, -55.0f);
        this.op2 = new Characters(778, -5.95f, 0.0f, -1.8f, -90.0f);
        this.op3 = new Characters(778, -5.95f, 0.0f, -4.15f, -90.0f);
        this.op4 = new Characters(778, -5.35f, 0.0f, -6.58f, -125.0f);
        this.loadarc(this.marglis.face, "FLSmarglis_h.fpk");
        this.loadarc(this.pelegri.face, "FLSpelegri.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.op1.mtn(261, 8, 1.0f, true);
        this.op2.mtn(262, 8, 1.0f, true);
        this.op3.mtn(261, 8, 0.9f, true);
        this.op4.mtn(262, 8, 0.9f, true);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 500.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.FadeIn = new Effect(0);
        this.FadeIn.args[0] = Integer.MIN_VALUE;
        this.FadeIn.args[1] = 30;
        this.FadeIn.args[2] = 1;
        this.FadeOut = new Effect(0);
        this.FadeOut.args[0] = Integer.MIN_VALUE;
        this.FadeOut.args[1] = 30;
        this.FadeOut.args[2] = 0;
        Stage.setVisible(125, false);
        Stage.setVisible(127, false);
        this.sidemoni1 = new Units(24613, 0.5f, 1.9f, 0.85f, 180.0f);
        this.sidemoni1.setArgs(0, 0.0f, 0.0f, 1.2f, 0.9f);
        this.sidemoni1.setArgs(1, 20051, 0, 0, 0);
        this.sidemoni1.setArgs(2, 80, 0, 0, -1);
        this.sidemoni1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        Stage.setVisible(126, false);
        this.sidemoni2 = new Units(24613, 0.5f, 1.9f, -6.7f, 0.0f);
        this.sidemoni2.setArgs(0, 0.0f, 0.0f, 1.2f, 0.9f);
        this.sidemoni2.setArgs(1, 20051, 0, 0, 0);
        this.sidemoni2.setArgs(2, 80, 0, 0, -1);
        this.sidemoni2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sidemoni1.signal(1);
        this.sidemoni2.signal(1);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        this.face(this.pelegri.face, 4, 0, 0, 0, 0, 1.0f);
        this.pelegri.setRotateY(-15.0f);
        this.pelegri.mtn(257, 0, 0, 0, 0, 1.0f, true);
        this.pelegri.setMotionFlags(0x800000, false);
        this.pelegri.setMotionFlags(0x2000000, true);
        System.sleep(5);
        Runtime.mpeg2("2000_1");
        Sound.streamPlay(1290094, 48000);
        this.cut_length = 120;
        this.CameraPlay = 2;
        this.face(this.pelegri.face, 4);
        this.pelegri.mtn(257, 0, 118, 0, 0, 1.0f, true);
        System.sleep(105);
        this.marglis.start(1, "c3_2");
        System.sleep(15);
        this.pelegri.setRotateY(-45.0f);
        this.cut_length = 150;
        this.CameraPlay = 3;
        this.pelegri.start(1, "c3_4_3");
        this.msg.print("What's one and a half");
        this.waitclear(60);
        this.msg.print("billion people to us...");
        this.waitclear(60);
        System.sleep(15);
        this.CameraPlay = 4;
        this.msg.print("They're innocents!");
        this.face(this.pelegri.face, 3, 0, 28, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Surely they deserve more respect.");
        this.face(this.pelegri.face, 3, 0, 74, 0, 0, 1.0f);
        this.waitclear(75);
        this.cut_length = 255;
        this.marglis.setTranslate(1.7f, 0.5f, -1.7f);
        this.pelegri.setRotateY(-40.0f);
        this.CameraPlay = 5;
        this.marglis.start(1, "c5_4");
        System.sleep(30);
        this.msg.print("Respect? For what?");
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Save your philanthropy \nfor someone that cares.");
        this.face(this.marglis.face, 1, 0, 57, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(30);
        this.cut_length = 300;
        this.CameraPlay = 6;
        System.sleep(15);
        this.msg.print("The experiment may have been\na failure, but I've taken steps\nto recover the Emulator.");
        this.waitclear(150);
        this.msg.print("If the need arises, I'm not against\nusing the Original, either.");
        this.waitclear(90);
        System.sleep(15);
        this.cut_length = 165;
        this.CameraPlay = 7;
        this.face(this.pelegri.face, 4);
        this.marglis.start(1, "c7_7");
        this.pelegri.mtn(264, 0, 493, 0, 0, 1.0f, true);
        this.msg.print("All we have to do is repeat\nthe process until we get results.");
        this.face(this.marglis.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(120);
        System.sleep(30);
        this.CameraPlay = 8;
        this.waitclear(75);
        this.cut_length = 270;
        this.CameraPlay = 9;
        this.marglis.start(1, "c9_9");
        this.msg.print("Is this all too much for your \nconscience to bear, Pellegri?");
        this.face(this.marglis.face, 1, 0, 95, 0, 0, 1.0f);
        this.waitclear(120);
        this.msg.print("In that case,\nfeel free to reveal everything\nand wait for your death sentence.");
        this.face(this.marglis.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(30);
        this.CameraPlay = 10;
        this.pelegri.start(1, "c10_10");
        this.msg.print("What about you?!");
        this.face(this.pelegri.face, 3, 0, 19, 0, 0, 1.0f);
        this.waitclear(30);
        this.pelegri.mtn(268, 0, 0, 0, 0, 1.0f, true);
        this.pelegri.setMotionFlags(0x800000, false);
        this.pelegri.setMotionFlags(0x2000000, true);
        this.cut_length = 315;
        this.CameraPlay = 11;
        this.marglis.start(1, "c11_12_11");
        System.sleep(15);
        this.msg.print("Me, tried in a court\nof ignoble commoners?");
        this.face(this.marglis.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("Don't make me laugh.");
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("We are the ones who\ndetermine life or death.");
        this.face(this.marglis.face, 1, 0, 95, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(15);
        this.msg.print("But, wait...");
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(45);
        this.CameraPlay = 12;
        this.pelegri.start(1, "c12_12");
        System.sleep(15);
        this.msg.print("Pellegri, are you telling me that you've forgotten everything we've been\nstriving for these past 14 years?");
        this.waitclear(195);
        System.sleep(15);
        this.CameraPlay = 13;
        this.face(this.pelegri.face, 4);
        System.sleep(105);
        this.msg.print("Commander Margulis.");
        this.waitclear(45);
        System.sleep(15);
        this.op2.setTranslate(-3.0f, 0.0f, -1.45f);
        this.op2.setRotateY(0.0f);
        this.CameraPlay = 14;
        this.marglis.start(1, "c14_13");
        this.op2.start(1, "c14_16_14");
        this.msg.print("What?");
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(45);
        this.CameraPlay = 15;
        this.msg.print("A message from\nLieutenant Commander Vanderkam,\nsole survivor of the vanguard force.");
        this.waitclear(105);
        this.msg.print("The unit was decimated, the Zohar\nEmulator, unsecured.");
        System.sleep(30);
        this.waitclear(75);
        System.sleep(15);
        this.pelegri.setRotateY(-45.0f);
        this.CameraPlay = 16;
        this.marglis.start(1, "c16_15");
        this.pelegri.start(1, "c16_16");
        System.sleep(15);
        this.msg.print("Where's the Zohar now?");
        this.face(this.marglis.face, 1, 0, 39, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("Location unknown. \nPossibly seized by the Gnosis.");
        this.waitclear(90);
        System.sleep(15);
        this.CameraPlay = 17;
        this.marglis.start(1, "c17_17");
        System.sleep(15);
        this.msg.print("I see.");
        this.face(this.marglis.face, 1, 0, 19, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("Notify the 474th Spec Ops Fleet.");
        this.face(this.marglis.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("We may need to implement Plan 31.");
        this.face(this.marglis.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Have them stand by in the specified coordinates for further orders.");
        this.face(this.marglis.face, 1, 0, 85, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.msg.print("Yes, Sir!");
        System.sleep(15);
        this.waitclear(28);
        System.sleep(2);
    }

    void rSPL(int n, float f, float f2, float f3) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0);
    }

    void rSPL(int n, float f, float f2, float f3, int n2) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0, n2, n);
    }

    void s() {
        System.waitSignal(this.msg, 255);
    }

    void tSPL(int n, float f, float f2, float f3) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0);
    }

    void tSPL(int n, float f, float f2, float f3, int n2) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0, n2, n);
    }

    void waitCameraEnd(int n) {
        while (n != this.CameraEnd) {
            System.sleep(1);
        }
    }

    void waitCameraPlay(int n) {
        while (true) {
            if (n == this.CameraPlay) {
                ++this.cut;
                break;
            }
            System.sleep(1);
        }
        Runtime.setRegister(1, this.cut);
        System.println(">>>>>>> CUT /[$1]");
        this.DefocusClear();
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Characters
            extends Chr {
        Chr face;

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
        }

        void c10_10() {
            this.mtn(266, 0, 28, 0, 0, 1.0f, true);
        }

        void c11_12_11() {
            this.mtn(267, 0, 1.0f, true);
            this.mtn(267, 510, 524, 0, 0, -1.0f, true);
        }

        void c12_12() {
            this.mtn(268, 0, 1.0f, true);
        }

        void c14_13() {
            this.mtn(269, 0, 1.0f, true);
        }

        void c14_16_14() {
            this.mtn(270, 0, 1.0f, true);
        }

        void c16_15() {
            this.mtn(271, 0, 178, 0, 0, 1.0f, true);
        }

        void c16_16() {
            this.mtn(272, 0, 1.0f, true);
        }

        void c17_17() {
            this.mtn(273, 0, 1.0f, true);
        }

        void c2_1() {
            this.mtn(257, 0, 1.0f, true);
        }

        void c3_2() {
            this.setTranslate(1.7f, 0.5f, 0.8f);
            this.mtn(258, 0, 0.9f, true);
        }

        void c3_4_3() {
            this.mtn(259, 0, 1.0f, true);
        }

        void c5_4() {
            this.mtn(260, 0, 0.9f, true);
        }

        void c7_7() {
            this.mtn(263, 0, 1.0f, true);
        }

        void c9_9() {
            this.mtn(265, 0, 1.0f, true);
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void monitor_off() {
            int n = 0;
            while (n < 8) {
                this.setArgs(0, 0.0f, 0.0f, 1.5f, 1.125f - 0.139375f * (float) n);
                this.setArgs(2, 112, 255, 255, 1);
                System.sleep(1);
                ++n;
            }
            int n2 = 0;
            while (n2 < 12) {
                this.setArgs(0, 0.0f, 0.0f, 1.5f - 0.125f * (float) n2, 0.01f);
                this.setArgs(2, 112, 255, 255, 1);
                System.sleep(1);
                ++n2;
            }
            this.setArgs(0, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setArgs(2, 0, 255, 255, 1);
        }
    }
}

