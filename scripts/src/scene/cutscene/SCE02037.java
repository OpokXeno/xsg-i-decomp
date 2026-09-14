import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_DYU08_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02037
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_DYU08_PRJ,
        Pack02037,
        FLSjr_h,
        FLSmary,
        JNT_Human,
        JNT_Accesories {
    Characters jr;
    Characters mary;
    Characters sold1;
    Characters sold2;
    Characters sold3;
    Characters sold4;
    Characters sold5;
    Characters sold6;
    Unit rifle1;
    Unit rifle2;
    Unit rifle3;
    Unit rifle4;
    Unit rifle5;
    Unit rifle6;
    Unit headset;
    MAPUnit airlock_L;
    MAPUnit airlock_R;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    int cut_length = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02037() {
    }

    void CameraThread() {
        this.light.setColor(0, 0.57f, 0.57f, 0.57f);
        this.light.setColor(1, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(1, 0.793f, 0.6f, 0.111f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, -0.963f, 0.172f, 0.209f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.501f, -0.47f, -0.727f);
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        Runtime.setDefocusQuick(0, 1, 12880, 1);
        this.cam1.setTranslate(10.84f, -6.35f, -3.9f);
        this.cam1.setRotate(52.0f, 78.0f, 12.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 60, 10.93f, -6.72f, -5.52f);
        this.waitCameraPlay(2);
        Runtime.setDefocusQuick(0, 1, 13880, 1);
        this.cam1.setTranslate(-6.3f, 5.07f, -6.23f);
        this.cam1.setRotate(-15.0f, -84.0f, 12.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length + 120, -6.11f, 5.07f, -4.39f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.64f, 0.64f, 0.64f);
        this.light.setDirection2(1, -0.982f, 0.018f, 0.188f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.039f, 0.548f, -0.836f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.124f, -0.874f, -0.469f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 87880, 1);
        this.cam1.setTranslate(-3.21f, 1.22f, -0.88f);
        this.cam1.setRotate(14.0f, -159.03f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, -0.992f, 0.0f, -0.128f);
        this.light.setColor(2, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(2, 0.776f, 0.001f, 0.631f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.31f, -0.931f, -0.194f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(-0.31f, 0.77f, -5.26f);
        this.cam1.setRotate(14.5f, -421.5f, 10.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -0.77f, 0.77f, -6.1f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.03f, 0.03f, 0.03f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.317f, 0.322f, 0.892f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.656f, 0.001f, -0.755f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, -0.346f, -0.883f, 0.317f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 19880, 1);
        this.cam1.setTranslate(6.02f, -2.92f, -7.13f);
        this.cam1.setRotate(51.5f, -274.5f, 16.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 6.1f, -2.92f, -8.2f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.64f, 0.64f, 0.64f);
        this.light.setDirection2(1, -0.982f, 0.0f, 0.188f);
        this.light.setColor(2, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(2, -0.12f, 0.651f, -0.749f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, 0.124f, -0.874f, -0.469f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 92880, 1);
        this.cam1.setTranslate(-3.54f, 1.6f, -1.04f);
        this.cam1.setRotate(-9.5f, -148.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.59f, 0.59f, 0.59f);
        this.light.setDirection2(1, -0.646f, 0.023f, 0.763f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, 0.789f, 0.566f, 0.238f);
        this.light.setColor(3, 0.12f, 0.12f, 0.12f);
        this.light.setDirection2(3, -0.147f, -0.767f, 0.625f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 50880, 1);
        Runtime.setDefocusQuick(0, 2, 124264, 2);
        this.cam1.setTranslate(-3.66f, 0.86f, 0.53f);
        this.cam1.setRotate(8.0f, -389.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.18f, 0.18f, 0.18f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.646f, 0.04f, 0.763f);
        this.light.setColor(2, 0.41f, 0.41f, 0.41f);
        this.light.setDirection2(2, 0.818f, 0.001f, 0.575f);
        this.light.setColor(3, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(3, -0.051f, -0.994f, -0.101f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 14880, 1);
        this.cam1.setTranslate(-6.66f, 2.57f, 0.61f);
        this.cam1.setRotate(-4.0f, -405.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -6.66f, 1.9f, 0.61f);
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
        System.println("XEVEFLAG:EV02037_F");
        Runtime.setFlags(151, 1, 1);
        System.println("XEVEJNAME:CFJ2_150 XEVEJPOINT:POINT2_150");
        Runtime.jumpCF(1780, 3);
    }

    void face(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void face(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
        chr.mtn(n, n2, n3, n4, n5, f, false);
        chr.start(4, null);
    }

    void faceRotate() {
    }

    void init() {
        Runtime.setLocation(807);
        this.jr = new Characters(0x1000022, -2.9f, 0.0f, -1.1f, -20.0f);
        this.mary = new Characters(0x1000120, -2.95f, 0.0f, 0.0f, 160.0f);
        this.sold1 = new Characters(0x1000401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold2 = new Characters(16778242, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold3 = new Characters(16778243, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold4 = new Characters(0x1000401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold5 = new Characters(16778242, 0.0f, 0.0f, 0.0f, 0.0f);
        this.sold6 = new Characters(16778243, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle1 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle1.setParent(this.sold1, 72);
        this.rifle1.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle1.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle1.setVisible(false);
        this.rifle2 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle2.setParent(this.sold2, 72);
        this.rifle2.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle2.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle2.setVisible(false);
        this.rifle3 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle3.setParent(this.sold3, 72);
        this.rifle3.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle3.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle3.setVisible(false);
        this.rifle4 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle4.setParent(this.sold4, 72);
        this.rifle4.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle4.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle4.setVisible(false);
        this.rifle5 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle5.setParent(this.sold5, 72);
        this.rifle5.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle5.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle5.setVisible(false);
        this.rifle6 = new Units(24594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle6.setParent(this.sold6, 72);
        this.rifle6.setTranslate(0.1f, -0.03f, 0.02f);
        this.rifle6.setRotate(46.0f, 13.0f, -55.0f);
        this.rifle6.setVisible(false);
        this.headset = new Units(24580, 0.0f, 0.0f, 0.0f, 0.0f);
        this.headset.setParent(this.mary, 48);
        this.headset.setTranslate(0.03f, -0.03f, -0.08f);
        this.headset.setRotate(94.0f, 92.2f, 87.0f);
        this.airlock_L = new Mapunits(42);
        this.airlock_L.setTranslate(0.0f, 0.0f, -3.5f);
        this.loadarc(this.jr.face, "FLSjr_h.fpk");
        this.loadarc(this.mary.face, "FLSmary.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.jr.setVisible(false);
        this.mary.setVisible(false);
        this.sold1.setVisible(false);
        this.sold2.setVisible(false);
        this.sold3.setVisible(false);
        this.sold4.setVisible(false);
        this.sold5.setVisible(false);
        this.sold6.setVisible(false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290033, 48000);
        this.cut_length = 150;
        this.CameraPlay = 1;
        this.airlock_L.start(1, "L_open");
        System.sleep(60);
        this.msg.print("Our objective is\nthe enemy mainframe.");
        this.waitclear(90);
        this.mary.setMotNoUpdate(2);
        this.mary.mtn(257, 15, 15, 0, 0, 1.0f, true);
        this.mary.setMotionFlags(0x800000, false);
        this.mary.setMotionFlags(0x2000000, true);
        this.face(this.mary.face, 6, 0, 0, 0, 0, 1.0f);
        this.cut_length = 150;
        this.CameraPlay = 2;
        System.sleep(15);
        this.msg.print("Utilize all available terminals and\ncounter any protective measures.");
        this.waitclear(120);
        System.sleep(30);
        this.cut_length = 180;
        this.jr.setVisible(true);
        this.mary.setVisible(true);
        this.sold1.setTranslate(-1.55f, 0.0f, 1.9f);
        this.sold1.setRotate(0.0f, -19.0f, 0.0f);
        this.sold1.setVisible(true);
        this.sold2.setTranslate(-0.95f, 0.0f, 4.6f);
        this.sold2.setRotate(0.0f, -76.0f, 0.0f);
        this.sold2.setVisible(true);
        this.sold3.setTranslate(-0.05f, 0.0f, 5.45f);
        this.sold3.setRotate(0.0f, -80.0f, 0.0f);
        this.sold3.setVisible(true);
        this.CameraPlay = 3;
        this.face(this.mary.face, 6, 0, 15, 0, 0, 1.0f);
        this.sold1.mtn(268, 0, 163, 0, 0, 1.0f, true);
        this.sold2.start(1, "sold2_c3");
        this.sold3.mtn(257, 75, 180, 0, 0, -0.65f, true);
        this.mary.mtn(257, 15, 180, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Once inside, engage all hostiles.");
        this.face(this.mary.face, 5, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("All hands, move out.");
        this.face(this.mary.face, 5, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        this.sold1.setVisible(true);
        this.sold2.setVisible(true);
        this.sold3.setVisible(true);
        this.sold1.setShadow(0, 0);
        this.sold2.setShadow(0, 0);
        this.sold3.setShadow(0, 0);
        this.sold4.setShadow(0, 0);
        this.sold5.setShadow(0, 0);
        this.sold6.setShadow(0, 0);
        this.rifle1.setVisible(true);
        this.rifle2.setVisible(true);
        this.rifle3.setVisible(true);
        this.sold1.setTranslate(-2.0f, 0.0f, -8.4f);
        this.sold2.setTranslate(-2.0f, 0.0f, -7.4f);
        this.sold3.setTranslate(-2.0f, 0.0f, -6.7f);
        this.sold4.setTranslate(-2.0f, 0.0f, -8.0f);
        this.sold5.setTranslate(-2.0f, 0.0f, -7.2f);
        this.sold6.setTranslate(-2.0f, 0.0f, -6.5f);
        this.sold1.setRotateY(90.0f);
        this.sold2.setRotateY(90.0f);
        this.sold3.setRotateY(90.0f);
        this.sold4.setRotateY(90.0f);
        this.sold5.setRotateY(90.0f);
        this.sold6.setRotateY(90.0f);
        this.cut_length = 75;
        this.CameraPlay = 4;
        this.sold1.mtn(258, 0, 1.0f, true);
        this.sold2.mtn(259, 0, 1.0f, true);
        this.sold3.mtn(260, 0, 1.0f, true);
        System.sleep(30);
        this.sold4.setVisible(true);
        this.rifle4.setVisible(true);
        this.sold4.mtn(258, 0, 1.0f, true);
        System.sleep(15);
        this.sold5.setVisible(true);
        this.rifle5.setVisible(true);
        this.sold5.mtn(259, 0, 1.0f, true);
        System.sleep(15);
        this.sold6.setVisible(true);
        this.rifle6.setVisible(true);
        this.sold6.mtn(260, 0, 1.0f, true);
        System.sleep(15);
        this.cut_length = 90;
        this.CameraPlay = 5;
        System.sleep(15);
        this.sold1.mtn(258, 0, 73, 0, 0, 1.0f, true);
        this.sold3.mtn(260, 0, 73, 0, 0, 1.0f, true);
        System.sleep(15);
        this.sold2.mtn(259, 0, 58, 0, 0, 1.0f, true);
        System.sleep(60);
        this.jr.setVisible(true);
        this.jr.mtn(262, 0, 0, 0, 0, 1.0f, true);
        this.jr.setMotionFlags(0x2000000, true);
        this.cut_length = 180;
        this.sold1.setShadow(7, 32);
        this.sold2.setShadow(7, 32);
        this.sold3.setShadow(7, 32);
        this.sold4.setVisible(false);
        this.sold5.setVisible(false);
        this.sold6.setVisible(false);
        this.rifle1.setVisible(false);
        this.rifle2.setVisible(false);
        this.rifle3.setVisible(false);
        this.rifle4.setVisible(false);
        this.rifle5.setVisible(false);
        this.rifle6.setVisible(false);
        this.sold1.setTranslate(-1.0f, 0.0f, 1.1f);
        this.sold1.setRotate(0.0f, -208.0f, 0.0f);
        this.sold1.setVisible(true);
        this.sold2.setTranslate(-0.95f, 0.0f, 4.6f);
        this.sold2.setRotate(0.0f, -76.0f, 0.0f);
        this.sold2.setVisible(true);
        this.rifle2.setVisible(true);
        this.sold3.setTranslate(-0.85f, 0.0f, 3.5f);
        this.sold3.setRotate(0.0f, -24.0f, 0.0f);
        this.sold3.setVisible(true);
        this.jr.setVisible(true);
        this.mary.setVisible(true);
        this.CameraPlay = 6;
        this.face(this.mary.face, 2, 0, 45, 0, 0, 1.0f);
        this.sold1.mtn(268, 0, 163, 0, 0, 0.7f, true);
        this.sold2.mtn(259, 0, 165, 0, 0, 1.2f, true);
        this.sold3.mtn(257, 75, 180, 0, 0, -0.65f, true);
        this.mary.mtn(261, 0, 1.0f, true);
        System.sleep(45);
        this.msg.print("Well, shall we head for the bridge,\nLittle Master? Ready?");
        this.face(this.mary.face, 1, 0, 70, 0, 0, 1.0f);
        System.sleep(80);
        this.face(this.mary.face, 1, 0, 18, 0, 0, 1.0f);
        this.waitclear(40);
        this.CameraPlay = 7;
        this.jr.mtn(262, 0, 1.0f, true);
        this.msg.print("All right.");
        this.face(this.jr.face, 15, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("Let's get this over with and go home.");
        this.face(this.jr.face, 15, 0, 51, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.jr.setTranslate(-4.85f, -0.05f, -1.3f);
        this.mary.setTranslate(-4.95f, 0.0f, 0.0f);
        this.cut_length = 75;
        this.CameraPlay = 8;
        this.jr.start(1, "j_c8");
        this.mary.start(1, "m_c8");
        System.sleep(this.cut_length - 15);
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
            this.setShadow(7, 32);
        }

        void j_c8() {
            this.setTranslate(-4.85f, -0.05f, -1.3f);
            this.mtn(263, 8, 1.0f, true);
            this.move(SCE02037.this.cut_length, -0.8f, -5.8f, true);
        }

        void m_c8() {
            this.setTranslate(-4.95f, 0.0f, 0.0f);
            this.mtn(265, 8, 1.0f, true);
            this.move(SCE02037.this.cut_length, -0.85f, -4.1f, true);
        }

        void sold2_c3() {
            this.mtn(267, 8, 1.0f, false);
            this.move(165, this.px - 8.0f, this.pz, true);
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.start(4, null);
        }

        void L_open() {
            int n = 1;
            while (n <= 300) {
                this.setTranslate(0.0f, 0.0f, -3.5f + 0.013666666f * (float) n);
                System.sleep(1);
                ++n;
            }
        }
    }
}

