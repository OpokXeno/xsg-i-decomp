import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK01_PRJ;
import xeno.map.MC_VOK02_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01007B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        MC_VOK01_PRJ,
        MC_VOK02_PRJ,
        JNT_Human,
        Pack01007,
        FLSallen,
        FLSshion,
        FLSchaos {
    static final float op_a_x = 4.8f;
    static final float op_a_y = 0.0f;
    static final float op_a_z = -9.1f;
    static final float op_a_ry = 90.0f;
    static final float op_b_x = 4.8f;
    static final float op_b_y = 0.0f;
    static final float op_b_z = -4.1f;
    static final float op_b_ry = 90.0f;
    static final float op_c_x = 4.8f;
    static final float op_c_y = 0.0f;
    static final float op_c_z = 0.9f;
    static final float op_c_ry = 90.0f;
    static final float op_d_x = -3.8f;
    static final float op_d_y = 0.02f;
    static final float op_d_z = -9.1f;
    static final float op_d_ry = 270.0f;
    static final float op_e_x = -3.8f;
    static final float op_e_y = 0.02f;
    static final float op_e_z = -4.1f;
    static final float op_e_ry = 270.0f;
    static final float op_f_x = -3.8f;
    static final float op_f_y = 0.0f;
    static final float op_f_z = 0.9f;
    static final float op_f_ry = 270.0f;
    Unit fma1;
    Unit fma2;
    Unit fma3;
    Unit fmd1;
    Unit fmd2;
    Unit fmd3;
    Unit fme1;
    Unit fme2;
    Unit fme3;
    Light light = new Light(0);
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    public static final float nc = 1.0E7f;
    Effect tab_ef;
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
    Camerawork camerawork = new Camerawork();
    Tab tab;
    Tab seat;
    Tab dummy1;
    Tab dummy2;
    Tab dummy3;
    Tab dummy4;
    human allen;
    human togsi;
    human ken_s;
    human ken_m1;
    human ken_w1;
    human ken_m2;
    human ken_w2;
    mirror_map door;
    units u1;
    units u2;
    units u3;
    units u4;
    units u5;
    units u6;
    units u7;
    units u8;
    units isu1;
    units isu2;
    int allen_flag = 0;
    int ken_w1_flag = 0;
    int ken_s_flag = 0;
    int device_flag = 0;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01007B() {
    }

    void FACE(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 8, f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, human human2, int n2, float f) {
        human2.face.mtn(n2, 0, 120, 7, 9, f, false);
        human2.face.start(4, null);
        System.sleep(n);
        human2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        human2.face.start(4, null);
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
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        float f4 = 0.0f;
        float f5 = 0.0f;
        float f6 = 0.0f;
        float f7 = 0.0f;
        while (true) {
            if (this.device_flag == 1) {
                this.u1.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.u1.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.allen_flag == 1) {
                this.allen.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.allen.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.ken_w1_flag == 1) {
                this.ken_w1.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                this.ken_w1.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
            }
            if (this.ken_s_flag == 1) {
                this.ken_s.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY(), this.cam4.getTranslateZ());
                this.ken_s.setRotate(this.cam4.getRotateX(), this.cam4.getRotateY(), this.cam4.getRotateZ());
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01007B_F");
        Runtime.setFlags(13, 1, 1);
        System.println("XEVEJNAME:CFJ1_70 XEVEJPOINT:POINT_70");
        Runtime.jumpCF(30, 1);
    }

    void init() {
        Runtime.setLocation(17);
        Runtime.setLocation(16);
        this.fma1 = new Unit();
        this.fma1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fma1.setArgs(1, 20007, 0, 128, 112);
        this.fma1.setArgs(2, 82, 0, 15, -1);
        this.fma1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma1.setTranslate(5.6000004f, 0.92f, -9.1f);
        this.fma1.setRotate(0.0f, -90.0f, 0.0f);
        this.fma1.setScale(0.37f, 0.37f, 0.37f);
        this.fma2 = new Unit();
        this.fma2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma2.setArgs(1, 20016, 0, 128, 112);
        this.fma2.setArgs(2, 82, 0, 15, -1);
        this.fma2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma2.setTranslate(5.5f, 0.92f, -8.6f);
        this.fma2.setRotate(0.0f, 60.0f, 0.0f);
        this.fma2.setScale(0.37f, 0.37f, 0.37f);
        this.fma3 = new Unit();
        this.fma3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fma3.setArgs(1, 20004, 0, 128, 112);
        this.fma3.setArgs(2, 82, 0, 15, -1);
        this.fma3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fma3.setTranslate(5.5f, 0.92f, -9.6f);
        this.fma3.setRotate(0.0f, 120.0f, 0.0f);
        this.fma3.setScale(0.37f, 0.37f, 0.37f);
        this.fmd1 = new Unit();
        this.fmd1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fmd1.setArgs(1, 20007, 0, 128, 112);
        this.fmd1.setArgs(2, 82, 0, 15, -1);
        this.fmd1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd1.setTranslate(-4.6f, 0.92f, -9.1f);
        this.fmd1.setRotate(0.0f, 90.0f, 0.0f);
        this.fmd1.setScale(0.37f, 0.37f, 0.37f);
        this.fmd2 = new Unit();
        this.fmd2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmd2.setArgs(1, 20016, 0, 128, 112);
        this.fmd2.setArgs(2, 82, 0, 15, -1);
        this.fmd2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd2.setTranslate(-4.5f, 0.92f, -8.6f);
        this.fmd2.setRotate(0.0f, 120.0f, 0.0f);
        this.fmd2.setScale(0.37f, 0.37f, 0.37f);
        this.fmd3 = new Unit();
        this.fmd3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fmd3.setArgs(1, 20004, 0, 128, 112);
        this.fmd3.setArgs(2, 82, 0, 15, -1);
        this.fmd3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fmd3.setTranslate(-4.5f, 0.92f, -9.6f);
        this.fmd3.setRotate(0.0f, 60.0f, 0.0f);
        this.fmd3.setScale(0.37f, 0.37f, 0.37f);
        this.fme1 = new Unit();
        this.fme1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fme1.setArgs(1, 20007, 0, 128, 112);
        this.fme1.setArgs(2, 82, 0, 15, -1);
        this.fme1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme1.setTranslate(-4.6f, 0.92f, -4.1f);
        this.fme1.setRotate(0.0f, 90.0f, 0.0f);
        this.fme1.setScale(0.37f, 0.37f, 0.37f);
        this.fme2 = new Unit();
        this.fme2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme2.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fme2.setArgs(1, 20016, 0, 128, 112);
        this.fme2.setArgs(2, 82, 0, 15, -1);
        this.fme2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme2.setTranslate(-4.5f, 0.92f, -3.6f);
        this.fme2.setRotate(0.0f, 120.0f, 0.0f);
        this.fme2.setScale(0.37f, 0.37f, 0.37f);
        this.fme3 = new Unit();
        this.fme3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme3.setArgs(0, 0.0f, 0.5f, 0.77f, 1.4f);
        this.fme3.setArgs(1, 20004, 0, 128, 112);
        this.fme3.setArgs(2, 82, 0, 15, -1);
        this.fme3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fme3.setTranslate(-4.5f, 0.92f, -4.6f);
        this.fme3.setRotate(0.0f, 60.0f, 0.0f);
        this.fme3.setScale(0.37f, 0.37f, 0.37f);
        this.fmd1.signal(1);
        this.fmd2.signal(1);
        this.fmd3.signal(1);
        this.fme1.signal(1);
        this.fme2.signal(1);
        this.fme3.signal(1);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam4 = Camera.create(4);
        this.cam5 = Camera.create(5);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.allen = new human(0x1000107, 0.0f, 0.0f, 0.0f, 0.0f);
        this.togsi = new human(16777517, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_s = new human(16777732, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_m1 = new human(16777731, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_m1.setVisible(10, false);
        this.ken_m1.setVisible(11, false);
        this.ken_w1 = new human(16777734, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_w1.setVisible(11, false);
        this.ken_w1.setVisible(12, false);
        this.ken_m2 = new human(0x1000202, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_m2.setVisible(8, false);
        this.ken_m2.setVisible(9, false);
        this.ken_w2 = new human(16777733, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ken_w2.setVisible(11, false);
        this.ken_w2.setVisible(12, false);
        this.tab = new Tab();
        this.tab.init(20555, 1.0f, 1.0f, 0.0f, 0.0f);
        this.tab.start(4, null);
        this.tab.setTranslate(0.46f, 0.19f, -8.04f);
        this.tab.setRotate(0.0f, 0.0f, 0.0f);
        this.tab.setShadow(0, 0);
        this.tab_ef = new Effect(1553, 0.0f, 0.0f, 0.0f, 0.0f);
        this.tab_ef.setScale(0.1f, 0.1f, 0.1f);
        this.tab_ef.setCaster(this.tab);
        this.tab_ef.disp(true);
        this.tab_ef.setTranslate(0.0f, 0.0f, 0.0f);
        this.seat = new Tab();
        this.seat.init(20583, 0.0f, 0.0f, 0.0f, 0.0f);
        this.seat.start(4, null);
        this.seat.setTranslate(0.5f, 0.0f, 0.7f);
        this.seat.setRotate(0.0f, 180.0f, 0.0f);
        this.seat.setShadow(0, 0);
        this.dummy1 = new Tab();
        this.dummy1.init(24602, 1.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy1.setShadow(0, 0);
        this.dummy2 = new Tab();
        this.dummy2.init(24602, 1.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy2.setShadow(0, 0);
        this.dummy3 = new Tab();
        this.dummy3.init(24602, 1.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy3.setShadow(0, 0);
        this.dummy4 = new Tab();
        this.dummy4.init(24602, 1.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this.dummy4.setShadow(0, 0);
        this.door = new mirror_map();
        this.door.init(38);
        this.door.start(4, null);
        this.door.setTranslate(-22.16f, -1.9f, -0.9f);
        this.door.setRotate(0.0f, -90.0f, 0.0f);
        this.isu1 = new units();
        this.isu1.mapUnit(16);
        this.isu1.start(4, null);
        this.isu1.setTranslate(-3.7500005f, 0.37548828f, -4.0999994f);
        this.isu1.setRotate(0.0f, 0.0f, 0.0f);
        this.isu2 = new units();
        this.isu2.mapUnit(23);
        this.isu2.start(4, null);
        this.u1 = new units();
        this.u1.init(24579, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u1.setScale(7.62f, 7.62f, 7.62f);
        this.u1.setParent(this.allen, 72);
        this.u1.setTranslate(0.15749949f, -0.024507077f, 0.07374915f);
        this.u1.setRotate(24.28324f, -30.66659f, 51.181873f);
        this.u2 = new units();
        this.u2.init(24596, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u2.setTranslate(-4.5227337f, 0.7002156f, 0.991748f);
        this.u2.setRotate(-89.33285f, 7.733331f, 109.08287f);
        this.u3 = new units();
        this.u3.init(24584, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u3.setTranslate(-4.3775f, 0.6874847f, 0.8269951f);
        this.u3.setRotate(-0.5001232f, 150.3992f, 86.833046f);
        this.u4 = new units();
        this.u4.init(24582, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u4.setTranslate(-4.3930025f, 0.68648523f, 0.9159986f);
        this.u4.setRotate(278.59552f, 8.699984f, -3.799992f);
        this.u5 = new units();
        this.u5.init(24581, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u5.setTranslate(-4.4514823f, 0.6757355f, 0.7484962f);
        this.u5.setRotate(39.19981f, 135.1654f, 694.5827f);
        this.u6 = new units();
        this.u6.init(24580, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u6.setTranslate(-4.5074806f, 0.70748633f, 0.7767476f);
        this.u6.setRotate(-1.4000027f, 94.97417f, -98.48205f);
        this.u7 = new units();
        this.u7.init(24579, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u7.setScale(7.62f, 7.62f, 7.62f);
        this.u7.setParent(this.ken_s, 60);
        this.u7.setTranslate(0.15924944f, -0.0062573827f, -0.066999584f);
        this.u7.setRotate(-22.166574f, 24.524757f, 100.065834f);
        this.u8 = new units();
        this.u8.init(24579, -4.47f, 0.68f, 1.07f, 0.0f);
        this.u8.setScale(7.27f, 7.27f, 7.27f);
        this.u8.setTranslate(-4.464991f, 0.6884729f, 1.0069993f);
        this.u8.setRotate(-89.89977f, 7.7333302f, 61.133236f);
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
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(40, false);
        Stage.setVisible(42, false);
        Stage.setVisible(43, false);
        Stage.setVisible(44, false);
        Stage.setVisible(45, false);
        Stage.setVisible(46, false);
        Stage.setVisible(47, false);
        this.allen.setShadow(5, 12);
        this.togsi.setShadow(0, 0);
        this.togsi.look_speed(0.22f);
        this.ken_s.look_speed(0.22f);
        this.allen.look_speed(0.12f);
        this.ken_w1.look_speed(0.22f);
        this.ken_m1.look_speed(0.22f);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.loadarc(this.ken_m1.face, "FLSshion.fpk");
        this.loadarc(this.ken_m2.face, "FLSchaos.fpk");
        this.loadarc(this.ken_s.face, "FLSchaos.fpk");
        this.loadarc(this.ken_w1.face, "FLSshion.fpk");
        this.loadarc(this.ken_w2.face, "FLSshion.fpk");
        this.loadarc(this.togsi.face, "FLSchaos.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.togsi.face.mtn(4, 8, 1.0f, false);
        this.togsi.face.start(4, null);
        this.ken_m1.face.mtn(2, 8, 1.0f, false);
        this.ken_m1.face.start(4, null);
        this.ken_m2.face.mtn(2, 8, 1.0f, false);
        this.ken_m2.face.start(4, null);
        this.ken_s.face.mtn(2, 8, 1.0f, false);
        this.ken_s.face.start(4, null);
        this.ken_w1.face.mtn(2, 8, 1.0f, false);
        this.ken_w1.face.start(4, null);
        this.ken_w2.face.mtn(2, 8, 1.0f, false);
        this.ken_w2.face.start(4, null);
        this.allen.face.mtn(2, 8, 1.0f, false);
        this.allen.face.start(4, null);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.ken_s.setTranslate(-3.37f, 0.0f, 0.49f);
        this.ken_s.setRotate(0.0f, 138.75f, 0.0f);
        this.ken_s.setVisible(false);
        this.ken_m2.setVisible(false);
        this.ken_w2.setVisible(false);
        this.allen.setTranslate(-2.14f, 0.0f, -6.79f);
        this.allen.setRotate(0.0f, -3.75f, 0.0f);
        this.cam1.change();
        this.fme1.signal(0);
        this.fme2.signal(0);
        Sound.streamPlay(1190028, 48000);
        this.camerawork.cut001_1();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.945f, 0.24f, 0.222f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, -0.804f, 0.001f, 0.595f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.42f, -0.424f, 0.803f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.seat.setVisible(false);
        this.allen.setTranslate(-2.2512496f, 0.0f, -9.89902f);
        this.allen.setRotate(0.0f, -5.799988f, 0.0f);
        this.togsi.setTranslate(-3.57375f, 0.0f, -4.6029997f);
        this.togsi.setRotate(0.0f, -78.75f, 0.0f);
        this.ken_w1.setTranslate(-3.9397492f, 0.012999996f, -9.06425f);
        this.ken_w1.setRotate(0.0f, -90.24993f, 0.0f);
        this.ken_m1.setTranslate(-3.891476f, -0.018000057f, -4.105494f);
        this.ken_m1.setRotate(100.198524f, -89.399895f, 99.23182f);
        this.allen.start(1, "mtn_001");
        this.togsi.start(1, "mtn_002");
        this.ken_m1.start(1, "mtn_032");
        this.ken_w1.start(1, "mtn_033");
        this.ken_m2.start(1, "mtn_033");
        System.sleep(45);
        System.sleep(36);
        this.MSGW("She's really got you trained,\nhasn't she, sir.");
        this.dummy1.start(1, "togsi_serifu_001_1");
        System.sleep(30);
        System.sleep(57);
        System.sleep(33);
        this.cam3.setTranslate(0.15749949f, -0.024507077f, 0.07374915f);
        this.cam3.setRotate(24.28324f, -30.66659f, 51.181873f);
        this.MSGW("What?\nTrained?");
        this.dummy2.start(1, "allen_serifu_001_2_1");
        System.sleep(78);
        this.MSGW("What do you mean?");
        float[] fArray = new float[]{1.0f, 0.15749949f, -0.024507077f, 0.07374915f, 32.0f, 0.16099955f, -0.001003419f, 0.074499235f};
        float[] fArray2 = new float[]{1.0f, 24.28324f, -30.66659f, 51.181873f, 32.0f, 20.666576f, -25.499926f, 99.8829f};
        this.cam3.transSPL(fArray, 1);
        this.cam3.rotateSPL(fArray2, 1);
        this.device_flag = 1;
        System.sleep(72);
        this.MSGW("Hey, what are you smiling at?!\nHurry up and get back to work!");
        this.dummy2.start(1, "allen_serifu_001_2_2");
        System.sleep(72);
        this.dummy1.start(1, "togsi_serifu_001_2");
        this.MSGW("I'm on it.");
        System.sleep(51);
        this.__wait();
        this.camerawork.cut002();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(1, -0.412f, 0.756f, 0.509f);
        this.light.setColor(2, 0.52f, 0.52f, 0.52f);
        this.light.setDirection2(2, -0.62f, 0.0f, -0.784f);
        this.light.setColor(3, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(3, -0.581f, -0.765f, 0.278f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fme1.signal(1);
        this.allen.setShadow(0, 12);
        this.ken_s.setShadow(0, 12);
        this.togsi.setShadow(0, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.ken_m2.setShadow(0, 12);
        this.ken_w2.setShadow(0, 12);
        this.fma1.signal(1);
        this.fma2.signal(1);
        this.fma3.signal(1);
        this.fmd1.signal(0);
        this.fmd2.signal(0);
        this.fmd3.signal(0);
        this.u1.setParent(this.allen, 60);
        this.device_flag = 0;
        this.ken_m2.setVisible(true);
        this.ken_w2.setVisible(true);
        this.ken_m2.setTranslate(4.8949966f, -0.025000028f, -9.114986f);
        this.ken_m2.setRotate(0.0f, 89.99999f, 0.0f);
        this.ken_w2.setTranslate(4.599984f, 0.0f, -8.569985f);
        this.ken_w2.setRotate(0.0f, 220.99995f, 0.0f);
        this.allen.setTranslate(-2.7700028f, 0.0f, -6.113747f);
        this.allen.setRotate(0.0f, 338.69974f, 0.0f);
        this.togsi.setTranslate(-3.57375f, 0.0f, -4.6029997f);
        this.togsi.setRotate(0.0f, -78.75f, 0.0f);
        this.ken_w2.start(1, "mtn_040");
        this.allen.start(1, "mtn_003");
        this.togsi.start(1, "mtn_004");
        this.dummy2.start(1, "togsi_serifu_002_1");
        this.MSGW("See how he always\nchanges the subject...");
        System.sleep(1);
        this.u1.setTranslate(0.14599879f, -0.001507167f, -0.06750075f);
        this.u1.setRotate(-28.999914f, 21.633362f, 101.21651f);
        System.sleep(74);
        this.MSGW("What was that, Togashi?!");
        this.dummy1.start(1, "allen_serifu_002");
        System.sleep(66);
        this.dummy2.start(1, "togsi_serifu_002_2");
        this.MSGW("Nothing at all, sir.\nYou're just hearing things!");
        this.wait_clr(84);
        System.sleep(60);
        System.waitSignal(this.togsi, 1);
        this.__wait();
        this.camerawork.cut003();
        this.fme2.signal(1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.945f, 0.24f, 0.222f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, -0.804f, 0.001f, 0.595f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.42f, -0.424f, 0.803f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fma1.signal(0);
        this.fma2.signal(0);
        this.fma3.signal(0);
        this.fmd1.signal(1);
        this.fmd2.signal(1);
        this.fmd3.signal(1);
        this.ken_m2.setVisible(false);
        this.ken_w2.setVisible(false);
        this.seat.setVisible(false);
        this.tab.setVisible(false);
        this.ken_s.setVisible(true);
        this.ken_s.setTranslate(-3.3049986f, 0.0f, -0.13999996f);
        this.ken_s.setRotate(0.0f, 521.25f, 0.0f);
        this.allen.setTranslate(-2.9542468f, 0.0f, -3.5692468f);
        this.allen.setRotate(0.0f, 364.2245f, 0.0f);
        this.allen.start(1, "mtn_005");
        this.togsi.start(1, "mtn_006");
        this.ken_s.start(1, "mtn_007");
        this.MSGW("I swear...");
        this.dummy1.start(1, "allen_serifu_003_1");
        System.sleep(45);
        System.sleep(45);
        this.dummy2.start(1, "allen_serifu_003_2");
        this.MSGW("Hmm? What's up?\nStill not convinced?");
        System.sleep(36);
        System.sleep(51);
        this.wait_clr(6);
        this.__wait();
        this.camerawork.cut004();
        this.light.setColor(0, 0.12f, 0.12f, 0.12f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.91f, 0.196f, -0.365f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.798f, 0.001f, -0.603f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, 0.282f, -0.65f, -0.705f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ken_s.setTranslate(-3.2599883f, -0.02999999f, -0.16999976f);
        this.ken_s.setRotate(0.0f, 521.25f, 0.0f);
        this.ken_s.start(1, "mtn_008");
        this.dummy1.start(1, "ken_s_serifu_004");
        this.MSGW("Oh, no,\nit's not that. It's just...");
        System.sleep(78);
        System.sleep(42);
        this.wait_clr(30);
        this.__wait();
        this.camerawork.cut005();
        this.allen.setShadow(3, 12);
        this.ken_s.setShadow(3, 12);
        this.togsi.setShadow(3, 12);
        this.ken_m1.setShadow(3, 12);
        this.ken_w1.setShadow(3, 12);
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.063f, 0.48f, -0.875f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.833f, 0.467f, 0.296f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, 0.602f, -0.598f, -0.529f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ken_w1.setVisible(false);
        this.seat.setVisible(true);
        this.tab.setVisible(true);
        this.fmd1.signal(0);
        this.fmd2.signal(0);
        this.fmd3.signal(0);
        this.fma1.signal(1);
        this.fma2.signal(1);
        this.fma3.signal(1);
        this.ken_m2.setVisible(true);
        this.ken_w2.setVisible(true);
        this.ken_m2.setTranslate(4.8949966f, -0.025000028f, -9.114986f);
        this.ken_m2.setRotate(0.0f, 89.99999f, 0.0f);
        this.allen.setTranslate(-2.8142495f, 0.0f, -1.8492582f);
        this.allen.setRotate(0.0f, 357.2245f, 0.0f);
        this.allen.start(1, "mtn_009");
        this.ken_s.start(1, "mtn_010");
        this.ken_w2.start(1, "mtn_040");
        this.dummy2.start(1, "allen_serifu_005");
        this.MSGW("I know how you feel,\nbut you saw what happened just now.");
        System.sleep(120);
        this.MSGW("Even the Chief is cautious sometimes.");
        System.sleep(69);
        this.__wait();
        this.camerawork.cut006_2();
        this.allen.setShadow(0, 0);
        this.ken_s.setShadow(0, 0);
        this.ken_m2.setShadow(0, 0);
        this.ken_w2.setShadow(0, 0);
        this.togsi.setShadow(3, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.465f, 0.205f, 0.862f);
        this.light.setColor(2, 0.48f, 0.48f, 0.48f);
        this.light.setDirection2(2, -0.937f, 0.0f, 0.35f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, 0.004f, -0.834f, 0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.seat.setVisible(false);
        this.ken_w1.setVisible(true);
        this.fmd1.signal(1);
        this.fmd2.signal(1);
        this.fmd3.signal(1);
        this.fme1.signal(1);
        this.fme2.signal(1);
        this.fme3.signal(1);
        this.ken_m2.setVisible(false);
        this.ken_w2.setVisible(false);
        this.fma1.signal(0);
        this.fma2.signal(0);
        this.fma3.signal(0);
        this.isu1.setTranslate(-3.7500005f, 0.37248704f, -4.0999994f);
        this.ken_m1.setTranslate(-3.8914757f, -0.003000055f, -4.105494f);
        this.ken_m1.setRotate(100.198524f, -89.399895f, 99.23181f);
        this.allen.setTranslate(-3.29f, 0.0f, -0.77f);
        this.allen.setRotate(0.0f, 356.25f, 0.0f);
        this.ken_s.setTranslate(-3.32f, 0.0f, 0.42f);
        this.ken_s.setRotate(0.0f, 521.25f, 0.0f);
        this.allen.start(1, "mtn_011");
        this.dummy1.start(1, "allen_serifu_006_2");
        this.MSGW("Besides...");
        System.sleep(66);
        this.MSGW("Ohhh, that's right.\nYou just transferred in\na month ago, right?");
        System.sleep(54);
        System.sleep(90);
        this.MSGW("I guess you couldn't have known.");
        System.sleep(81);
        this.__wait();
        this.camerawork.cut007();
        this.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.light.setColor(1, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(1, -0.903f, 0.312f, 0.294f);
        this.light.setColor(2, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(2, -0.395f, 0.237f, -0.888f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, -0.726f, -0.471f, -0.501f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.seat.setVisible(true);
        this.tab.setVisible(false);
        this.allen.setShadow(0, 12);
        this.ken_s.setShadow(0, 12);
        this.ken_m2.setShadow(0, 12);
        this.ken_w2.setShadow(0, 12);
        this.togsi.setShadow(0, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.allen.setTranslate(-3.2899997f, 0.0f, -0.7699999f);
        this.allen.setRotate(1.0666666f, 356.24997f, 0.0f);
        this.ken_s.setTranslate(-3.3399947f, 0.0f, 0.52199817f);
        this.ken_s.setRotate(-6.2666626f, 529.5496f, -0.7999973f);
        this.allen.start(1, "mtn_012");
        this.ken_s.start(1, "mtn_013");
        this.dummy2.start(1, "ken_s_serifu_007");
        this.MSGW("You mean the accident\nfrom two years ago?");
        System.sleep(78);
        this.MSGW("I've heard about it,\nbut I don't know any details.");
        System.sleep(66);
        System.sleep(69);
        this.__wait();
        System.sleep(1);
        this.camerawork.cut008();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.44f, 0.44f, 0.44f);
        this.light.setDirection2(1, 0.465f, 0.202f, 0.862f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.979f, 0.0f, 0.205f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, 0.004f, -0.834f, 0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.seat.setVisible(false);
        this.tab.setVisible(true);
        this.ken_m2.setVisible(true);
        this.ken_w2.setVisible(true);
        this.fma1.signal(1);
        this.fma2.signal(1);
        this.fma3.signal(1);
        this.allen.setShadow(0, 12);
        this.ken_s.setShadow(0, 12);
        this.ken_m2.setShadow(0, 12);
        this.ken_w2.setShadow(0, 12);
        this.togsi.setShadow(0, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.allen.setTranslate(-2.939992f, 0.0f, -0.8949996f);
        this.allen.setRotate(4.999994f, 353.25f, 1.333333f);
        this.allen.start(1, "mtn_014");
        this.ken_w2.setTranslate(4.794976f, -0.005f, -8.569984f);
        this.ken_w2.setRotate(0.0f, 220.99994f, 0.0f);
        this.ken_w2.start(1, "mtn_040");
        this.dummy1.start(1, "allen_serifu_008");
        this.MSGW("Oh, right...");
        this.wait_clr(66);
        this.wait_clr(35);
        this.__wait();
        this.camerawork.cut009_1();
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.402f, 0.351f, -0.846f);
        this.light.setColor(2, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(2, 0.803f, 0.38f, 0.458f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, 0.543f, -0.554f, -0.631f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.seat.setVisible(false);
        this.tab.setVisible(false);
        this.ken_s.look_char(this.allen);
        this.u2.setVisible(true);
        this.u3.setVisible(true);
        this.u4.setVisible(true);
        this.u5.setVisible(true);
        this.u6.setVisible(true);
        this.u8.setVisible(true);
        this.allen.setTranslate(-2.777485f, 0.112499915f, -0.76249456f);
        this.allen.setRotate(-0.3333333f, 351.74997f, 0.0f);
        this.ken_s.setTranslate(-3.1199963f, 0.0f, 0.31999978f);
        this.ken_s.setRotate(-3.6666653f, 533.74994f, 0.9999998f);
        this.ken_s.start(1, "mtn_010");
        System.sleep(81);
        this.__wait();
        this.camerawork.cut009_2();
        this.light.setColor(0, 0.28f, 0.28f, 0.28f);
        this.light.setColor(1, 0.85f, 0.85f, 0.85f);
        this.light.setDirection2(1, -0.03f, 0.661f, -0.75f);
        this.light.setColor(2, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(2, 0.023f, 0.736f, 0.677f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, -0.02f, -0.986f, 0.163f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(96);
        this.__wait();
        System.sleep(1);
        this.camerawork.cut010();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.524f, 0.308f, 0.794f);
        this.light.setColor(2, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(2, -0.926f, 0.0f, 0.379f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.789f, -0.614f, 0.02f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.tab.setVisible(true);
        this.seat.setVisible(false);
        this.ken_w1.setVisible(false);
        this.ken_m1.setVisible(false);
        this.fmd1.signal(0);
        this.fmd2.signal(0);
        this.fmd3.signal(0);
        this.fme1.signal(0);
        this.fme2.signal(0);
        this.fme3.signal(0);
        this.fma1.signal(0);
        this.fma2.signal(0);
        this.fma3.signal(0);
        this.allen.setTranslate(-3.3599997f, 0.0f, -0.6999999f);
        this.allen.setRotate(0.0f, 352.24997f, 0.0f);
        this.ken_s.setTranslate(-3.069997f, 0.0f, 0.029999966f);
        this.ken_s.setRotate(0.0f, 557.49994f, 0.0f);
        this.togsi.setTranslate(-3.493745f, 0.0f, -4.6079974f);
        this.togsi.setRotate(0.0f, -78.75f, 0.0f);
        this.ken_s.start(1, "mtn_016");
        this.togsi.start(1, "mtn_017");
        this.allen.start(1, "mtn_015");
        this.dummy2.start(1, "allen_serifu_010");
        this.MSGW("Oh man, not again...\nOh well.");
        System.sleep(30);
        this.wait_clr(30);
        this.wait_clr(51);
        this.__wait();
        this.camerawork.cut011();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.73f, 0.73f, 0.73f);
        this.light.setDirection2(1, 0.551f, 0.414f, 0.724f);
        this.light.setColor(2, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(2, -0.809f, 0.0f, 0.588f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.479f, -0.518f, 0.709f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fme1.signal(1);
        this.fme2.signal(1);
        this.fme3.signal(1);
        this.ken_w1.setVisible(true);
        this.ken_m1.setVisible(true);
        this.u2.setVisible(false);
        this.u3.setVisible(false);
        this.u4.setVisible(false);
        this.u5.setVisible(false);
        this.u6.setVisible(false);
        this.u8.setVisible(false);
        this.ken_m1.setTranslate(-3.8584728f, -0.020500068f, -4.102494f);
        this.ken_m1.setRotate(181.23048f, -88.19955f, 178.49728f);
        this.ken_w1.setTranslate(-3.727975f, 0.019999998f, -6.3293676f);
        this.ken_w1.setRotate(0.0f, -12.199963f, -3.0000002E-8f);
        this.togsi.setTranslate(-3.418492f, 0.0f, -4.7234626f);
        this.togsi.setRotate(28.265738f, -78.53215f, 20.332848f);
        this.ken_w1.setTranslate(-3.7999876f, 0.019999998f, -6.289902f);
        this.ken_w1.setRotate(0.0f, -9.499999f, 0.0f);
        this.togsi.face.mtn(303, 8, 1.0f, false);
        this.togsi.face.start(4, null);
        this.togsi.start(1, "mtn_018");
        this.ken_w1.start(1, "mtn_019");
        this.ken_m1.start(1, "mtn_020");
        this.dummy1.start(1, "togsi_serifu_011");
        this.dummy2.start(1, "ken_w1_serifu_011");
        this.dummy4.start(1, "ken_m1_serifu_011");
        System.sleep(81);
        this.MSGW("Perfect!\nNow the two of you can be alone.");
        System.sleep(96);
        this.MSGW("Why don't you ask her out\nwhile you're at it?");
        this.wait_clr(48);
        this.__wait();
        this.ken_w1_flag = 0;
        System.sleep(1);
        this.camerawork.cut012();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.66f, 0.66f, 0.66f);
        this.light.setDirection2(1, 0.551f, 0.415f, 0.724f);
        this.light.setColor(2, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(2, -0.809f, 0.0f, 0.588f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.479f, -0.518f, 0.709f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ken_w2.setVisible(false);
        this.ken_m2.setVisible(false);
        this.fme1.signal(1);
        this.fme2.signal(1);
        this.fme3.signal(1);
        this.u2.setVisible(true);
        this.u3.setVisible(true);
        this.u4.setVisible(true);
        this.u5.setVisible(true);
        this.u6.setVisible(true);
        this.u8.setVisible(true);
        this.isu2.setTranslate(-3.7500005f, 0.41299438f, 1.1249996f);
        this.isu2.setRotate(0.0f, 0.0f, 0.0f);
        this.togsi.setTranslate(-3.354995f, 0.012999999f, -4.5884714f);
        this.togsi.setRotate(19.066582f, -78.09994f, 10.299956f);
        this.ken_s.setTranslate(-3.1449869f, -0.044999994f, 0.5199995f);
        this.ken_s.setRotate(-1.9999996f, 557.49994f, -7.999994f);
        this.ken_w1.setTranslate(-3.9899805f, 0.019999998f, -4.6653786f);
        this.ken_w1.setRotate(0.0f, -10.299996f, 0.0f);
        this.ken_m1.setTranslate(-3.7714844f, 0.005999941f, -4.105494f);
        this.ken_m1.setRotate(100.198524f, -89.399895f, 99.23181f);
        this.allen.setTranslate(-2.7839894f, 0.017999878f, -0.8399918f);
        this.allen.setRotate(0.0f, 322.04996f, 0.0f);
        this.u4.setTranslate(-4.352f, 0.6814804f, 0.9389969f);
        this.u4.setRotate(280.66223f, 9.499979f, -21.833303f);
        this.u3.setTranslate(-4.3775f, 0.6874847f, 0.8639922f);
        this.u3.setRotate(-0.5001232f, 150.3992f, 86.833046f);
        this.togsi.start(1, "mtn_022");
        this.ken_w1.start(1, "mtn_023");
        this.allen.start(1, "mtn_021");
        this.dummy3.start(1, "allen_get_disk");
        System.sleep(42);
        this.MSGW("We can handle the rest by ourselves.");
        System.sleep(72);
        this.dummy2.start(1, "ken_w1_serifu_012");
        this.MSGW("This is your big channnce!");
        System.sleep(54);
        System.sleep(6);
        this.MSGW("I told you guys,\nit's not like that!");
        this.dummy1.start(1, "allen_serifu_013");
        System.sleep(12);
        this.allen_flag = 0;
        this.__wait();
        this.camerawork.cut013();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(1, -0.788f, 0.001f, -0.616f);
        this.light.setColor(2, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(2, 0.793f, 0.465f, -0.393f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, 0.237f, -0.711f, -0.662f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.u4.setTranslate(0.10949983f, -0.005498779f, 0.044249736f);
        this.u4.setRotate(22.511583f, -23.767052f, -85.979416f);
        this.ken_s.setTranslate(-2.87f, 0.0f, 0.79f);
        this.ken_s.setRotate(0.0f, 588.75f, 0.0f);
        this.allen.setTranslate(-4.187997f, 0.0f, 0.5699992f);
        this.allen.setRotate(0.0f, 305.56903f, 0.0f);
        this.allen.start(1, "mtn_024");
        System.sleep(81);
        System.sleep(15);
        this.__wait();
        System.sleep(1);
        this.camerawork.cut014();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, 0.823f, 0.278f, 0.496f);
        this.light.setColor(2, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(2, -0.85f, 0.397f, 0.347f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, -0.202f, -0.808f, 0.553f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.u2.setVisible(false);
        this.u3.setVisible(false);
        this.u5.setVisible(false);
        this.u6.setVisible(false);
        this.u8.setVisible(false);
        this.ken_w1.look_char(this.allen);
        this.ken_m1.look_char(this.allen);
        this.togsi.look_char(this.allen);
        this.isu2.setTranslate(-3.7500005f, 0.41299438f, 0.9499992f);
        this.isu2.setRotate(0.0f, 0.0f, 0.0f);
        this.togsi.setTranslate(-3.4069924f, 0.012999999f, -4.614466f);
        this.togsi.setRotate(6.9332094f, -62.49985f, -0.10005295f);
        this.ken_w1.setTranslate(-4.0549736f, 0.019999998f, -4.7173758f);
        this.ken_w1.setRotate(0.0f, -6.399996f, 0.0f);
        this.ken_m1.setTranslate(-3.7194831f, 0.005999941f, -4.105494f);
        this.ken_m1.setRotate(100.198524f, -89.399895f, 99.23181f);
        this.ken_s.setTranslate(-2.418002f, 0.0f, 0.9739953f);
        this.ken_s.setRotate(0.0f, 583.64996f, 0.0f);
        this.allen.setTranslate(-3.780002f, -0.007999991f, 0.24999912f);
        this.allen.setRotate(-0.5333333f, 374.36798f, 0.0f);
        this.allen.start(1, "mtn_025");
        this.ken_s.start(1, "mtn_026");
        this.MSGW("For crying out loud...");
        this.dummy1.start(1, "allen_serifu_014");
        this.dummy2.start(1, "disk_off_device_on");
        this.wait_clr(51);
        System.waitSignal(this.ken_s, 3);
        this.__wait();
        this.camerawork.cut016();
        this.allen.setShadow(5, 12);
        this.ken_s.setShadow(0, 12);
        this.ken_m2.setShadow(0, 12);
        this.ken_w2.setShadow(0, 12);
        this.togsi.setShadow(0, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.71f, 0.71f, 0.71f);
        this.light.setDirection2(1, -0.808f, 0.409f, -0.423f);
        this.light.setColor(2, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(2, 0.854f, 0.0f, -0.52f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.323f, -0.752f, -0.575f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.setTranslate(-2.2179961f, -0.007999991f, 5.753434f);
        this.allen.setRotate(-0.5333333f, 353.06802f, 0.0f);
        this.allen.start(1, "mtn_027");
        System.sleep(81);
        this.dummy1.start(1, "allen_serifu_016");
        this.MSGW("A-anyway, I'd better get this to her.");
        this.cam3.setTranslate(-0.9f, 1.0f, 6.89f);
        this.cam3.setRotate(-0.07f, 85.93f, -2.0f);
        System.sleep(90);
        this.__wait();
        System.sleep(1);
        this.camerawork.cut017();
        this.allen.setShadow(0, 12);
        this.ken_s.setShadow(0, 12);
        this.ken_m2.setShadow(0, 12);
        this.ken_w2.setShadow(0, 12);
        this.togsi.setShadow(0, 12);
        this.ken_m1.setShadow(0, 12);
        this.ken_w1.setShadow(0, 12);
        this.togsi.setShadow(5, 12);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.808f, 0.409f, -0.423f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.335f, 0.428f, -0.839f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.323f, -0.752f, -0.575f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.look_default();
        this.ken_s.setTranslate(-2.418002f, 0.0f, 0.9739953f);
        this.ken_s.setRotate(0.0f, 646.14996f, 0.0f);
        float[] fArray3 = new float[]{1.0f, -0.9f, 1.0f, 6.89f, 236.0f, 5.25f, 1.0f, 6.89f};
        float[] fArray4 = new float[]{1.0f, -0.07f, 85.93f, -2.0f, 236.0f, -0.07f, 90.93f, -2.0f};
        this.cam3.transSPL(fArray3, 1);
        this.cam3.rotateSPL(fArray4, 1);
        this.allen_flag = 1;
        this.allen.setTranslate(-0.9f, 1.0f, 6.89f);
        this.allen.setRotate(-0.07f, 85.93f, -2.0f);
        this.allen.start(1, "mtn_037");
        this.togsi.start(1, "mtn_028");
        this.ken_w1.start(1, "mtn_029");
        this.ken_m1.start(1, "mtn_030");
        this.ken_w1.setTranslate(-3.84f, 0.02f, -5.11f);
        this.ken_w1.setRotate(0.0f, 20.0f, 0.0f);
        this.togsi.setTranslate(-3.02f, 0.0f, -4.54f);
        this.togsi.setRotate(0.0f, 5.0f, 0.0f);
        System.sleep(21);
        this.MSGW("Good luck!\n--Yeah!");
        System.sleep(33);
        this.MSGW("Go Tiger!");
        this.wait_clr(96);
        this.allen_flag = 0;
        this.__wait();
        Runtime.setLocation(17);
        this.camerawork.cut018();
        this.allen.setShadow(5, 12);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.638f, 0.417f, -0.648f);
        this.light.setColor(2, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(2, 0.534f, 0.556f, 0.637f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, -0.674f, 0.738f, -0.019f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.isu2.setTranslate(-16.25998f, -0.037005566f, 1.989992f);
        this.isu2.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.start(1, "mtn_037");
        System.sleep(9);
        this.cam3.setTranslate(-17.46f, -0.01f, 0.03f);
        this.cam3.setRotate(0.0f, 88.05f, 0.0f);
        this.door.start(1, "door_open");
        float[] fArray5 = new float[]{1.0f, -17.46f, -0.01f, 0.03f, 109.200005f, -13.89f, -0.01f, -0.18f, 217.20001f, -10.46f, -0.01f, -0.14f};
        float[] fArray6 = new float[4];
        fArray6[0] = 1.0f;
        fArray6[2] = 88.05f;
        float[] fArray7 = fArray6;
        this.cam3.transSPL(fArray5, 1);
        this.cam3.rotateSPL(fArray7, 1);
        this.allen_flag = 1;
        System.sleep(81);
        this.door.start(1, "door_close");
        System.sleep(6);
        this.__wait();
        this.allen_flag = 0;
        System.sleep(1);
        this.camerawork.cut019();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.461f, 0.228f, -0.858f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.595f, 0.001f, 0.804f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.161f, -0.866f, -0.474f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.start(1, "mtn_031");
        this.allen.setTranslate(-14.4000225f, -0.013999999f, -0.18299952f);
        this.allen.setRotate(-1.8666644f, 88.8499f, 1.3666658f);
        System.sleep(37);
        this.dummy1.start(1, "allen_serifu_019");
        this.MSGW("...Geez, they just don't know\nwhen to quit...");
        System.sleep(36);
        this.wait_clr(60);
        System.sleep(30);
        this.MSGW("It's not like I don't want to...");
        System.sleep(57);
        this.MSGW("I just...can't...");
        this.cam3.setTranslate(-12.68f, -0.01f, -0.21f);
        this.wait_clr(60);
        System.sleep(100);
        this.__wait();
        System.sleep(1);
        this.camerawork.cut020();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.76f, 0.76f, 0.76f);
        this.light.setDirection2(1, -0.452f, 0.31f, -0.836f);
        this.light.setColor(2, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(2, -0.569f, 0.31f, 0.762f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.738f, -0.607f, -0.294f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.togsi.setVisible(false);
        this.ken_s.setVisible(false);
        this.ken_m1.setVisible(false);
        this.ken_w1.setVisible(false);
        this.ken_m2.setVisible(false);
        this.ken_w2.setVisible(false);
        this.tab.setVisible(false);
        this.seat.setVisible(false);
        this.u1.setVisible(false);
        this.u2.setVisible(false);
        this.u3.setVisible(false);
        this.u5.setVisible(false);
        this.u6.setVisible(false);
        this.u7.setVisible(false);
        this.u8.setVisible(false);
        this.tab_ef.setTranslate(0.0f, 1000.0f, 0.0f);
        this.tab_ef.disp(false);
        this.tab_ef.clearEffect();
        this.fma1.signal(0);
        this.fma2.signal(0);
        this.fma3.signal(0);
        this.fmd1.signal(0);
        this.fmd2.signal(0);
        this.fmd3.signal(0);
        this.fme1.signal(0);
        this.fme2.signal(0);
        this.fme3.signal(0);
        this.allen.start(1, "mtn_037");
        float[] fArray8 = new float[]{1.0f, -12.68f, -0.01f, -0.21f, 67.0f, -11.12f, -0.01f, -0.2f, 133.0f, -9.27f, -0.01f, -0.19f};
        this.cam3.transSPL(fArray8, 1);
        this.allen_flag = 1;
        System.sleep(51);
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
            this.setShadow(3, 12);
        }

        void mtn_001() {
            this.mtn(257, 0, 1.0f, true);
        }

        void mtn_002() {
            this.mtn(258, 0, 1.0f, true);
        }

        void mtn_003() {
            this.mtn(259, 0, 1.0f, true);
        }

        void mtn_004() {
            this.mtn(260, 0, 252, 0, 0, 1.0f, true);
            this.mtn(260, 253, 347, 0, 0, 1.27f, true);
            this.signal(1);
        }

        void mtn_005() {
            this.mtn(261, 0, 1.0f, true);
        }

        void mtn_006() {
            this.mtn(262, 8, 1.0f, true);
        }

        void mtn_007() {
            this.mtn(263, 8, 1.0f, true);
        }

        void mtn_008() {
            this.mtn(264, 8, 1.0f, true);
        }

        void mtn_009() {
            this.mtn(265, 8, 1.0f, true);
        }

        void mtn_010() {
            this.mtn(266, 8, 1.0f, true);
        }

        void mtn_011() {
            this.mtn(267, 0, 1.0f, true);
        }

        void mtn_012() {
            this.mtn(268, 8, 1.0f, true);
        }

        void mtn_013() {
            this.mtn(269, 8, 1.0f, true);
        }

        void mtn_014() {
            this.mtn(270, 0, 1.0f, true);
            this.mtn(270, 102, 201, 0, 8, 1.0f, true);
        }

        void mtn_015() {
            this.mtn(271, 0, 1.0f, true);
        }

        void mtn_016() {
            this.mtn(272, 0, 1.0f, true);
        }

        void mtn_017() {
            this.mtn(273, 0, 1.0f, true);
        }

        void mtn_018() {
            this.mtn(274, 0, 1.0f, true);
        }

        void mtn_019() {
            this.mtn(275, 0, 1.0f, true);
        }

        void mtn_020() {
            this.mtn(276, 0, 1.0f, true);
            this.mtn(276, 225, 314, 0, 8, 1.0f, true);
        }

        void mtn_021() {
            this.mtn(277, 0, 1.0f, true);
        }

        void mtn_022() {
            this.mtn(278, 0, 1.0f, true);
        }

        void mtn_023() {
            this.mtn(279, 0, 1.0f, true);
        }

        void mtn_024() {
            this.mtn(280, 0, 1.0f, true);
            this.signal(2);
        }

        void mtn_025() {
            this.mtn(281, 0, 1.0f, true);
        }

        void mtn_026() {
            this.mtn(282, 0, 87, 0, 0, 1.0f, true);
            this.mtn(282, 87, 123, 0, 0, 0.82f, true);
            this.signal(3);
        }

        void mtn_027() {
            this.mtn(283, 0, 1.0f, true);
        }

        void mtn_028() {
            this.mtn(284, 8, 1.0f, true);
        }

        void mtn_029() {
            this.mtn(285, 8, 1.0f, true);
        }

        void mtn_030() {
            this.mtn(286, 8, 1.0f, true);
        }

        void mtn_031() {
            this.mtn(287, 72, 454, 0, 0, 1.0f, true);
            this.signal(4);
        }

        void mtn_032() {
            this.mtn(288, 30, 109, 0, 8, 1.0f, true);
        }

        void mtn_033() {
            this.mtn(289, 0, 79, 0, 8, 1.0f, true);
        }

        void mtn_034() {
            this.mtn(290, 8, 1.0f, true);
        }

        void mtn_035() {
            this.mtn(291, 8, 1.0f, true);
        }

        void mtn_036() {
            this.mtn(292, 8, 1.0f, true);
        }

        void mtn_037() {
            this.mtn(293, 8, 0.67f, true);
        }

        void mtn_038() {
            this.mtn(294, 8, 1.0f, true);
        }

        void mtn_039() {
            this.mtn(295, 8, 1.0f, true);
        }

        void mtn_040() {
            this.mtn(296, 220, 520, 0, 0, 0.52f, true);
        }

        void mtn_041() {
            this.mtn(297, 8, 1.0f, true);
        }

        void mtn_042() {
            this.mtn(298, 8, 1.0f, true);
        }

        void mtn_043() {
            this.mtn(299, 8, 1.0f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }
    }

    class mirror_map
            extends MAPUnit {
        mirror_map() {
        }

        public void door_close() {
            float[] fArray = new float[12];
            fArray[1] = -22.16f;
            fArray[2] = -1.9f;
            fArray[3] = -0.9f;
            fArray[4] = 27.0f;
            fArray[5] = -22.16f;
            fArray[6] = -1.9f;
            fArray[7] = 0.32f;
            fArray[8] = 47.0f;
            fArray[9] = -22.16f;
            fArray[10] = -1.9f;
            fArray[11] = 0.62f;
            float[] fArray2 = fArray;
            SCE01007B.this.cam2.rotateSPL(fArray2, 1, 1, 47);
            System.sleep(1);
            float f = -0.9f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE01007B.this.cam2.getRotateZ() - f;
                this.setTranslate(this.px, this.py, this.pz - f2);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 82);
        }

        public void door_open() {
            float[] fArray = new float[12];
            fArray[1] = -22.16f;
            fArray[2] = -1.9f;
            fArray[3] = -0.9f;
            fArray[4] = 27.0f;
            fArray[5] = -22.16f;
            fArray[6] = -1.9f;
            fArray[7] = 0.32f;
            fArray[8] = 47.0f;
            fArray[9] = -22.16f;
            fArray[10] = -1.9f;
            fArray[11] = 0.62f;
            float[] fArray2 = fArray;
            SCE01007B.this.cam2.rotateSPL(fArray2, 1, 1, 47);
            System.sleep(1);
            float f = -0.9f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE01007B.this.cam2.getRotateZ() - f;
                this.setTranslate(this.px, this.py, this.pz + f2);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 82);
        }
    }

    class Tab
            extends Chr {
        Tab() {
        }

        void allen_get_disk() {
            System.sleep(122);
            SCE01007B.this.u4.setParent(SCE01007B.this.allen, 72);
            SCE01007B.this.u4.setTranslate(0.10974973f, 0.0039919303f, 0.06224972f);
            SCE01007B.this.u4.setRotate(-12.4216f, -18.383741f, -100.76244f);
        }

        void allen_serifu_001_2_1() {
            SCE01007B.this.FACE(6, SCE01007B.this.allen, 9, 1.2f);
            System.sleep(12);
            SCE01007B.this.FACE(45, SCE01007B.this.allen, 9, 1.2f);
            System.sleep(15);
            SCE01007B.this.FACE(57, SCE01007B.this.allen, 9, 1.2f);
        }

        void allen_serifu_001_2_2() {
            SCE01007B.this.FACE(60, SCE01007B.this.allen, 9, 1.2f);
        }

        void allen_serifu_002() {
            System.sleep(3);
            SCE01007B.this.FACE(15, SCE01007B.this.allen, 9, 1.2f);
            System.sleep(12);
            SCE01007B.this.FACE(18, SCE01007B.this.allen, 9, 1.2f);
        }

        void allen_serifu_003() {
            System.sleep(3);
            SCE01007B.this.FACE(27, SCE01007B.this.allen, 5, 0.82f);
        }

        void allen_serifu_003_1() {
            System.sleep(3);
            SCE01007B.this.FACE(27, SCE01007B.this.allen, 5, 0.42f);
        }

        void allen_serifu_003_2() {
            System.sleep(3);
            SCE01007B.this.FACE_SMOOTH(24, SCE01007B.this.allen, 1, 0.92f);
            System.sleep(12);
            SCE01007B.this.FACE(30, SCE01007B.this.allen, 1, 1.12f);
        }

        void allen_serifu_005() {
            SCE01007B.this.FACE(54, SCE01007B.this.allen, 1, 1.1f);
            System.sleep(15);
            SCE01007B.this.FACE(39, SCE01007B.this.allen, 1, 1.2f);
            System.sleep(12);
            SCE01007B.this.FACE(51, SCE01007B.this.allen, 1, 1.2f);
        }

        void allen_serifu_006_2() {
            SCE01007B.this.FACE(18, SCE01007B.this.allen, 1, 1.2f);
            System.sleep(48);
            SCE01007B.this.FACE(42, SCE01007B.this.allen, 1, 0.72f);
            System.sleep(12);
            SCE01007B.this.FACE(75, SCE01007B.this.allen, 1, 1.1f);
            System.sleep(15);
            SCE01007B.this.FACE(60, SCE01007B.this.allen, 1, 1.1f);
        }

        void allen_serifu_008() {
            SCE01007B.this.FACE(27, SCE01007B.this.allen, 1, 1.0f);
        }

        void allen_serifu_010() {
            SCE01007B.this.FACE(48, SCE01007B.this.allen, 7, 0.52f);
            System.sleep(12);
            SCE01007B.this.FACE(33, SCE01007B.this.allen, 7, 1.0f);
        }

        void allen_serifu_013() {
            SCE01007B.this.allen.face.mtn(10, 8, 1.0f, false);
            SCE01007B.this.allen.face.start(4, null);
            System.sleep(21);
            SCE01007B.this.FACE(51, SCE01007B.this.allen, 9, 1.52f);
        }

        void allen_serifu_014() {
            SCE01007B.this.allen.face.mtn(6, 8, 1.0f, false);
            SCE01007B.this.allen.face.start(4, null);
            System.sleep(3);
            SCE01007B.this.FACE(24, SCE01007B.this.allen, 5, 1.2f);
        }

        void allen_serifu_016() {
            SCE01007B.this.allen.face.mtn(6, 8, 1.0f, false);
            SCE01007B.this.allen.face.start(4, null);
            System.sleep(6);
            SCE01007B.this.FACE(69, SCE01007B.this.allen, 5, 1.102f);
        }

        void allen_serifu_019() {
            SCE01007B.this.allen.face.mtn(10, 8, 1.0f, false);
            SCE01007B.this.allen.face.start(4, null);
            System.sleep(3);
            SCE01007B.this.FACE(18, SCE01007B.this.allen, 9, 1.47f);
            System.sleep(12);
            SCE01007B.this.FACE(42, SCE01007B.this.allen, 9, 1.47f);
            System.sleep(48);
            System.sleep(3);
            SCE01007B.this.FACE_SMOOTH(30, SCE01007B.this.allen, 1, 1.0f);
            System.sleep(27);
            SCE01007B.this.FACE(45, SCE01007B.this.allen, 1, 1.1f);
        }

        void disk_off_device_on() {
            System.sleep(42);
            SCE01007B.this.u1.setParent(SCE01007B.this.ken_s, 72);
            SCE01007B.this.u1.setTranslate(0.07499995f, -0.012496947f, 0.062499993f);
            SCE01007B.this.u1.setRotate(3.3333337f, 12.499999f, -1.6666688f);
        }

        void ken_m1_serifu_011() {
            SCE01007B.this.ken_m1.face.mtn(6, 0, 120, 7, 8, 1.0f, false);
            SCE01007B.this.ken_m1.face.start(4, null);
            System.sleep(156);
            SCE01007B.this.ken_m1.face.mtn(4, 0, 120, 7, 9, 1.0f, false);
            SCE01007B.this.ken_m1.face.start(4, null);
        }

        void ken_s_serifu_004() {
            SCE01007B.this.FACE_SMOOTH(90, SCE01007B.this.ken_s, 7, 1.0f);
        }

        void ken_s_serifu_007() {
            SCE01007B.this.FACE(60, SCE01007B.this.ken_s, 1, 1.0f);
            System.sleep(18);
            SCE01007B.this.FACE(42, SCE01007B.this.ken_s, 1, 1.0f);
            System.sleep(24);
            SCE01007B.this.FACE_SMOOTH(51, SCE01007B.this.ken_s, 7, 1.0f);
        }

        void ken_w1_serifu_011() {
            SCE01007B.this.ken_w1.face.mtn(2, 0, 120, 7, 8, 1.0f, false);
            SCE01007B.this.ken_w1.face.start(4, null);
            System.sleep(96);
            SCE01007B.this.ken_w1.face.mtn(4, 0, 120, 7, 9, 1.0f, false);
            SCE01007B.this.ken_w1.face.start(4, null);
        }

        void ken_w1_serifu_012() {
            SCE01007B.this.FACE(48, SCE01007B.this.ken_w1, 3, 1.0f);
        }

        void togsi_serifu_001_1() {
            SCE01007B.this.FACE(75, SCE01007B.this.togsi, 1, 1.1f);
            System.sleep(12);
            SCE01007B.this.FACE_SMOOTH(18, SCE01007B.this.togsi, 3, 1.2f);
        }

        void togsi_serifu_001_2() {
            SCE01007B.this.FACE_SMOOTH(36, SCE01007B.this.togsi, 5, 0.92f);
        }

        void togsi_serifu_002_1() {
            SCE01007B.this.FACE(78, SCE01007B.this.togsi, 7, 0.92f);
        }

        void togsi_serifu_002_2() {
            SCE01007B.this.FACE_SMOOTH(72, SCE01007B.this.togsi, 1, 1.1f);
        }

        void togsi_serifu_011() {
            SCE01007B.this.togsi.face.mtn(303, 0, 120, 7, 8, 1.0f, false);
            SCE01007B.this.togsi.face.start(4, null);
            System.sleep(36);
            SCE01007B.this.togsi.face.mtn(301, 0, 120, 7, 9, 1.0f, false);
            SCE01007B.this.togsi.face.start(4, null);
            System.sleep(45);
            SCE01007B.this.FACE(84, SCE01007B.this.togsi, 300, 1.0f);
            System.sleep(12);
            SCE01007B.this.FACE(66, SCE01007B.this.togsi, 300, 1.0f);
            System.sleep(24);
            SCE01007B.this.FACE(48, SCE01007B.this.togsi, 300, 1.0f);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut001_1() {
            SCE01007B.this.cam1.setFov(28.01f);
            SCE01007B.this.cam0.setFov(28.01f);
            float[] fArray = new float[]{1.0f, -2.46f, 0.9f, -1.24f, 124.41f, -2.32f, 0.89f, -1.23f, 247.95f, -2.04f, 0.86f, -1.29f, 371.49f, -1.93f, 0.86f, -1.34f, 458.49f, -1.93f, 0.86f, -1.34f, 632.49f, -1.93f, 0.86f, -1.34f};
            float[] fArray2 = new float[24];
            fArray2[0] = 1.0f;
            fArray2[1] = 2.1f;
            fArray2[2] = 10.08f;
            fArray2[4] = 124.41f;
            fArray2[5] = 2.19f;
            fArray2[6] = 13.7f;
            fArray2[8] = 247.95f;
            fArray2[9] = 2.1f;
            fArray2[10] = 18.84f;
            fArray2[12] = 371.49f;
            fArray2[13] = 2.36f;
            fArray2[14] = 21.93f;
            fArray2[16] = 458.49f;
            fArray2[17] = 2.36f;
            fArray2[18] = 21.93f;
            fArray2[20] = 632.49f;
            fArray2[21] = 2.36f;
            fArray2[22] = 21.93f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut001_2() {
            SCE01007B.this.cam1.setFov(26.81f);
            SCE01007B.this.cam0.setFov(26.81f);
            float[] fArray = new float[]{1.0f, -0.92f, 1.17f, -0.96f, 218.0f, -1.19f, 1.17f, -0.91f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -0.82f;
            fArray2[2] = 30.3f;
            fArray2[4] = 218.0f;
            fArray2[5] = -0.92f;
            fArray2[6] = 30.3f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 26.81f, 218.0f, 26.81f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut002() {
            SCE01007B.this.cam1.setFov(29.76f);
            SCE01007B.this.cam0.setFov(29.76f);
            float[] fArray = new float[]{1.0f, -5.79f, 1.7f, -4.28f, 277.0f, -5.73f, 1.7f, -4.29f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.32f;
            fArray2[2] = -81.76f;
            fArray2[4] = 277.0f;
            fArray2[5] = -6.57f;
            fArray2[6] = -81.76f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut003() {
            SCE01007B.this.cam1.setFov(28.8f);
            SCE01007B.this.cam0.setFov(28.8f);
            float[] fArray = new float[]{1.0f, -2.79f, 0.74f, -0.34f, 51.0f, -2.74f, 0.71f, 0.52f, 101.0f, -2.77f, 0.71f, 1.18f, 121.0f, -2.77f, 0.71f, 1.18f, 141.0f, -2.77f, 0.71f, 1.18f, 177.0f, -2.77f, 0.71f, 1.18f};
            float[] fArray2 = new float[20];
            fArray2[0] = 1.0f;
            fArray2[1] = 9.36f;
            fArray2[2] = 10.42f;
            fArray2[4] = 51.0f;
            fArray2[5] = 9.36f;
            fArray2[6] = 10.42f;
            fArray2[8] = 101.0f;
            fArray2[9] = 9.36f;
            fArray2[10] = 10.42f;
            fArray2[12] = 121.0f;
            fArray2[13] = 9.36f;
            fArray2[14] = 10.42f;
            fArray2[16] = 172.0f;
            fArray2[17] = 9.36f;
            fArray2[18] = 10.42f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut004() {
            SCE01007B.this.cam1.setFov(23.04f);
            SCE01007B.this.cam0.setFov(23.04f);
            float[] fArray = new float[]{1.0f, -2.58f, 1.51f, -0.88f, 128.0f, -2.58f, 1.55f, -0.88f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.25f;
            fArray2[2] = 494.87f;
            fArray2[4] = 128.0f;
            fArray2[5] = -2.25f;
            fArray2[6] = 494.87f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut005() {
            SCE01007B.this.cam1.setFov(32.69f);
            SCE01007B.this.cam0.setFov(32.69f);
            float[] fArray = new float[]{1.0f, 8.88f, 3.13f, -12.89f, 768.0f, 8.78f, 3.13f, -12.47f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -11.79f;
            fArray2[2] = 490.6f;
            fArray2[4] = 768.0f;
            fArray2[5] = -11.79f;
            fArray2[6] = 490.6f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut006_1() {
            SCE01007B.this.cam1.setFov(27.61f);
            SCE01007B.this.cam0.setFov(27.61f);
            float[] fArray = new float[]{1.0f, -3.36f, 1.59f, 0.26f, 278.0f, -3.37f, 1.59f, 0.28f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.68f;
            fArray2[2] = 352.78f;
            fArray2[4] = 278.0f;
            fArray2[5] = -4.68f;
            fArray2[6] = 352.78f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut006_2() {
            SCE01007B.this.cam1.setFov(27.61f);
            SCE01007B.this.cam0.setFov(27.61f);
            float[] fArray = new float[]{1.0f, -3.6f, 1.56f, 1.6f, 320.0f, -3.67f, 1.56f, 1.57f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.67f;
            fArray2[2] = 352.51f;
            fArray2[4] = 320.0f;
            fArray2[5] = -4.14f;
            fArray2[6] = 352.16f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut007() {
            SCE01007B.this.cam1.setFov(32.4f);
            SCE01007B.this.cam0.setFov(32.4f);
            float[] fArray = new float[]{1.0f, -6.66f, 2.12f, -1.95f, 231.0f, -6.64f, 2.15f, -1.94f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.58f;
            fArray2[2] = -115.23f;
            fArray2[4] = 231.0f;
            fArray2[5] = -13.58f;
            fArray2[6] = -115.23f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut008() {
            SCE01007B.this.cam1.setFov(23.05f);
            SCE01007B.this.cam0.setFov(23.05f);
            float[] fArray = new float[]{1.0f, -3.69f, 1.27f, 0.3f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = 9.92f;
            fArray2[2] = -33.51f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut009_1() {
            SCE01007B.this.cam1.setFov(24.69f);
            SCE01007B.this.cam0.setFov(24.69f);
            float[] fArray = new float[]{1.0f, -1.94f, 2.1f, -1.25f, 209.0f, -2.15f, 1.96f, -1.08f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -23.16f;
            fArray2[2] = 130.97f;
            fArray2[4] = 209.0f;
            fArray2[5] = -23.16f;
            fArray2[6] = 130.97f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 24.69f, 209.0f, 24.16f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut009_2() {
            SCE01007B.this.cam1.setFov(26.39f);
            SCE01007B.this.cam0.setFov(26.39f);
            float[] fArray = new float[]{1.0f, -3.71f, 1.07f, 0.57f, 128.0f, -3.76f, 1.05f, 0.6f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -25.84f;
            fArray2[2] = 114.31f;
            fArray2[4] = 128.0f;
            fArray2[5] = -25.84f;
            fArray2[6] = 114.31f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 26.39f, 128.0f, 25.67f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut010() {
            SCE01007B.this.cam1.setFov(32.24f);
            SCE01007B.this.cam0.setFov(32.24f);
            float[] fArray = new float[]{1.0f, -4.62f, 0.75f, 1.42f, 335.0f, -4.64f, 0.75f, 1.46f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.4f;
            fArray2[2] = -30.43f;
            fArray2[4] = 335.0f;
            fArray2[5] = 6.4f;
            fArray2[6] = -29.33f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut011() {
            SCE01007B.this.cam1.setFov(27.58f);
            SCE01007B.this.cam0.setFov(27.58f);
            float[] fArray = new float[]{1.0f, -4.44f, 0.94f, -1.96f, 328.0f, -4.42f, 0.97f, -2.05f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 5.19f;
            fArray2[2] = -15.6f;
            fArray2[4] = 328.0f;
            fArray2[5] = 5.57f;
            fArray2[6] = -15.6f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 27.58f, 328.0f, 27.26f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut012() {
            SCE01007B.this.cam1.setFov(30.11f);
            SCE01007B.this.cam0.setFov(30.11f);
            float[] fArray = new float[]{1.0f, -4.53f, 0.9f, 1.9f, 60.0f, -4.53f, 0.9f, 1.9f, 119.0f, -4.53f, 0.9f, 1.9f, 178.0f, -4.53f, 0.9f, 1.9f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = 2.92f;
            fArray2[2] = -22.32f;
            fArray2[4] = 60.0f;
            fArray2[5] = 2.92f;
            fArray2[6] = -19.11f;
            fArray2[8] = 119.0f;
            fArray2[9] = 2.92f;
            fArray2[10] = -15.46f;
            fArray2[12] = 178.0f;
            fArray2[13] = 2.92f;
            fArray2[14] = -14.25f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 30.11f, 60.0f, 30.11f, 119.0f, 30.11f, 178.0f, 30.11f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut013() {
            SCE01007B.this.cam1.setFov(23.82f);
            SCE01007B.this.cam0.setFov(23.82f);
            float[] fArray = new float[]{1.0f, -2.88f, 0.65f, -1.75f, 11.0f, -2.88f, 0.65f, -1.75f, 21.0f, -2.88f, 0.65f, -1.75f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = 12.19f;
            fArray2[2] = -567.71f;
            fArray2[4] = 11.0f;
            fArray2[5] = 12.97f;
            fArray2[6] = -563.45f;
            fArray2[8] = 21.0f;
            fArray2[9] = 13.0f;
            fArray2[10] = -562.16f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut014() {
            SCE01007B.this.cam1.setFov(31.66f);
            SCE01007B.this.cam0.setFov(31.66f);
            float[] fArray = new float[]{1.0f, -3.17f, 1.44f, 2.94f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.16f;
            fArray2[2] = -370.34f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut015() {
            SCE01007B.this.cam1.setFov(32.54f);
            SCE01007B.this.cam0.setFov(32.54f);
            float[] fArray = new float[]{1.0f, -1.68f, 1.36f, 7.77f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -4.87f;
            fArray2[2] = -353.92f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut016() {
            SCE01007B.this.cam1.setFov(29.01f);
            SCE01007B.this.cam0.setFov(29.01f);
            float[] fArray = new float[]{1.0f, -1.9f, 1.32f, 3.17f, 39.0f, -1.9f, 1.31f, 3.17f, 77.0f, -1.9f, 1.31f, 3.17f, 115.0f, -1.9f, 1.31f, 3.17f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = 7.45f;
            fArray2[2] = -539.07f;
            fArray2[4] = 39.0f;
            fArray2[5] = 8.96f;
            fArray2[6] = -535.51f;
            fArray2[8] = 77.0f;
            fArray2[9] = 9.23f;
            fArray2[10] = -532.69f;
            fArray2[12] = 115.0f;
            fArray2[13] = 9.23f;
            fArray2[14] = -532.38f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 29.01f, 39.0f, 29.01f, 77.0f, 28.4f, 115.0f, 28.09f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut017() {
            SCE01007B.this.cam1.setFov(32.54f);
            SCE01007B.this.cam0.setFov(32.54f);
            float[] fArray = new float[]{1.0f, -5.12f, 1.81f, -7.95f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -6.56f;
            fArray2[2] = -521.04f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut018() {
            SCE01007B.this.cam1.setFov(26.9f);
            SCE01007B.this.cam0.setFov(26.9f);
            float[] fArray = new float[]{1.0f, -12.52f, 5.02f, -1.27f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -47.04f;
            fArray2[2] = 109.19f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut019() {
            SCE01007B.this.cam1.setFov(24.78f);
            SCE01007B.this.cam0.setFov(24.78f);
            float[] fArray = new float[]{1.0f, -10.96f, 1.05f, -0.48f, 41.0f, -11.03f, 1.05f, -0.46f, 81.0f, -11.01f, 1.05f, -0.47f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = 12.22f;
            fArray2[2] = 100.1f;
            fArray2[4] = 41.0f;
            fArray2[5] = 13.38f;
            fArray2[6] = 101.74f;
            fArray2[8] = 81.0f;
            fArray2[9] = 13.65f;
            fArray2[10] = 102.71f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 24.78f, 41.0f, 24.63f, 81.0f, 24.41f};
            SCE01007B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut020() {
            SCE01007B.this.cam1.setFov(27.48f);
            SCE01007B.this.cam0.setFov(27.48f);
            float[] fArray = new float[]{1.0f, -16.16f, 0.19f, -1.98f, 128.0f, -16.16f, 0.19f, -1.98f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 9.43f;
            fArray2[2] = 244.57f;
            fArray2[4] = 128.0f;
            fArray2[5] = 9.43f;
            fArray2[6] = 250.66f;
            float[] fArray3 = fArray2;
            SCE01007B.this.cam1.transSPL(fArray, 1);
            SCE01007B.this.cam1.rotateSPL(fArray3, 1);
        }
    }
}

