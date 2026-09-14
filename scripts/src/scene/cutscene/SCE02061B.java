import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02061B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02061B,
        MC_ELS01_PRJ,
        FLSshion,
        FLSallen,
        FLSmomo,
        FLSziggy,
        FLSchaos,
        FLSmatehws,
        FLShammer,
        FLStonny {
    Light light = new Light(0);
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    public static final float nc = 1.0E7f;
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
    int shion_flag = 0;
    int allen_flag = 0;
    int masyu_flag = 0;
    int hammr_flag = 0;
    int rnd_mize_flag = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camerawork camerawork = new Camerawork();
    human hammr;
    human masyu;
    human chaos;
    human shion;
    human allen;
    human ziggy;
    human momo;
    Unit isu1;
    Unit isu2;
    Unit isu3;
    MAPUnit isu4;
    Unit isu5;
    Unit isu6;
    MAPUnit isu7;
    MAPUnit semotare;
    Unit semotare2;
    Unit zabuton;
    units dummy1;
    units dummy2;
    units dummy3;
    units dummy4;
    Unit fm1;
    Unit fm1_2;
    Unit fm1_3;
    Unit fm1_4;
    Unit fm1_5;
    Unit fm3;
    Unit masyu_fm;
    Unit masyu_fm2;
    float f_sabun = 0.0f;
    float pinvot_sabun = 0.0f;
    float f_isu5_py = 0.0f;
    float f_isu5_pz = 0.0f;
    float masyu_zure_x = 0.0f;
    float masyu_zure_y = 0.0f;
    float masyu_zure_z = 0.0f;
    float moni_zure_x = 0.0f;
    float moni_zure_y = 0.0f;
    float moni_zure_z = 0.0f;
    int mize_a = 20;
    int mize_b = 7;
    int mize_c = 20;
    int mize_e = 7;
    int mize_f = 7;
    int mize_g = 7;
    int mize_count = 0;
    float masyu_a = 22.0f;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02061B() {
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
            if (this.masyu_flag == 4) {
                this.isu_move(this.cam3.getRotateX());
            }
            if (this.masyu_flag == 3) {
                this.isu_move(this.cam3.getRotateX());
            }
            if (this.shion_flag == 1) {
                this.shion.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.shion.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.shion_flag == 2) {
                this.shion.setTranslate(this.cam4.getTranslateX(), this.cam4.getTranslateY(), this.cam4.getTranslateZ());
                this.shion.setRotate(this.cam4.getRotateX(), this.cam4.getRotateY(), this.cam4.getRotateZ());
            }
            if (this.allen_flag == 1) {
                this.allen.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
            }
            if (this.allen_flag == 2) {
                this.allen.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.allen.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.hammr_flag == 1) {
                this.hammr.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                this.hammr.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
            }
            if (this.hammr_flag == 2) {
                this.hammr.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                this.hammr.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
            }
            if (this.rnd_mize_flag == 1) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = (f % (float) this.mize_a + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f3 = (f % (float) this.mize_b + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f4 = (f % (float) this.mize_c + (f - (float) ((int) f))) / 1000.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f5 = (f % (float) this.mize_e + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f6 = (f % (float) this.mize_f + (f - (float) ((int) f))) / 100.0f;
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f7 = (f % (float) this.mize_g + (f - (float) ((int) f))) / 100.0f;
                this.cam2.setTranslate(this.cam1.getTranslateX() + f2, this.cam1.getTranslateY() + f3, this.cam1.getTranslateZ() + f4);
                this.cam2.setRotate(this.cam1.getRotateX() + f5, this.cam1.getRotateY() + f6, this.cam1.getRotateZ() + f7);
                if (this.mize_count == 0) {
                    this.rnd_mize_flag = 0;
                }
                --this.mize_count;
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    void _asi_yawaraka_off() {
        this.momo.setVisible(0, false);
        this.momo.setVisible(15, false);
        this.momo.setVisible(16, false);
        this.shion.setVisible(0, false);
        this.shion.setVisible(10, false);
        this.shion.setVisible(15, false);
        this.shion.setVisible(16, false);
        this.shion.setVisible(17, false);
        this.shion.setVisible(18, false);
        this.allen.setVisible(14, false);
        this.allen.setVisible(6, false);
        this.allen.setVisible(7, false);
        this.allen.setVisible(8, false);
        this.allen.setVisible(9, false);
        this.allen.setVisible(10, false);
        this.hammr.setVisible(9, false);
        this.hammr.setVisible(12, false);
        this.hammr.setVisible(13, false);
        this.hammr.setVisible(14, false);
        this.ziggy.setVisible(6, false);
        this.ziggy.setVisible(9, false);
        this.ziggy.setVisible(10, false);
        this.ziggy.setVisible(11, false);
        this.ziggy.setVisible(13, false);
        this.ziggy.setVisible(14, false);
        this.ziggy.setVisible(15, false);
        this.ziggy.setVisible(16, false);
        this.ziggy.setVisible(17, false);
        this.ziggy.setVisible(18, false);
        this.ziggy.setVisible(19, false);
        this.ziggy.setVisible(20, false);
        this.ziggy.setVisible(21, false);
        this.ziggy.setVisible(22, false);
        this.ziggy.setVisible(23, false);
        this.ziggy.setVisible(24, false);
    }

    void _asi_yawaraka_on() {
        this.momo.setVisible(0, true);
        this.momo.setVisible(15, true);
        this.momo.setVisible(16, true);
        this.shion.setVisible(0, true);
        this.shion.setVisible(10, true);
        this.shion.setVisible(15, true);
        this.shion.setVisible(16, true);
        this.allen.setVisible(14, true);
        this.allen.setVisible(6, true);
        this.allen.setVisible(7, true);
        this.allen.setVisible(8, true);
        this.hammr.setVisible(9, true);
        this.hammr.setVisible(12, true);
        this.hammr.setVisible(13, true);
        this.hammr.setVisible(14, true);
        this.ziggy.setVisible(6, true);
        this.ziggy.setVisible(9, true);
        this.ziggy.setVisible(10, true);
        this.ziggy.setVisible(11, true);
        this.ziggy.setVisible(13, true);
        this.ziggy.setVisible(14, true);
        this.ziggy.setVisible(15, true);
        this.ziggy.setVisible(16, true);
        this.ziggy.setVisible(17, true);
        this.ziggy.setVisible(18, true);
        this.ziggy.setVisible(19, true);
        this.ziggy.setVisible(20, true);
        this.ziggy.setVisible(21, true);
        this.ziggy.setVisible(24, true);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02061B_F");
        Runtime.setFlags(194, 1, 1);
        System.println("XEVEJNAME:SCE02062");
        Runtime.jumpEvent(2620);
    }

    void init() {
        Runtime.setLocation(46);
        Stage.renderCommand(22);
        this.fm3 = new Unit();
        this.fm3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm3.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm3.setArgs(1, 21007, 0, 128, 112);
        this.fm3.setArgs(2, 82, 0, 15, -1);
        this.fm3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm3.setScale(0.23f, 0.23f, 0.23f);
        this.fm3.setTranslate(-3.83f, 0.52f, 1.54f);
        this.fm3.setRotate(0.02f, 89.37f, 0.0f);
        this.fm1 = new Unit();
        this.fm1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1.setArgs(1, 22035, 0, 128, 112);
        this.fm1.setArgs(2, 82, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm1.setScale(0.3f, 0.3f, 0.3f);
        this.fm1.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1.setRotate(0.0f, 3.15f, 0.0f);
        this.fm1_2 = new Unit();
        this.fm1_2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_2.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1_2.setArgs(1, 22007, 0, 128, 112);
        this.fm1_2.setArgs(2, 72, 0, 15, -1);
        this.fm1_2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_2.setScale(0.3f, 0.3f, 0.3f);
        this.fm1_2.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1_2.setRotate(0.0f, 3.15f, 0.0f);
        this.fm1_3 = new Unit();
        this.fm1_3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_3.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1_3.setArgs(1, 22035, 0, 128, 112);
        this.fm1_3.setArgs(2, 0, 0, 15, -1);
        this.fm1_3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_3.setScale(0.3f, 0.3f, 0.3f);
        this.fm1_3.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1_3.setRotate(0.0f, 3.15f, 0.0f);
        this.fm1_4 = new Unit();
        this.fm1_4.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_4.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1_4.setArgs(1, 22030, 0, 128, 112);
        this.fm1_4.setArgs(2, 82, 0, 15, -1);
        this.fm1_4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_4.setScale(0.3f, 0.3f, 0.3f);
        this.fm1_4.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1_4.setRotate(0.0f, 3.15f, 0.0f);
        this.fm1_5 = new Unit();
        this.fm1_5.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_5.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1_5.setArgs(1, 22034, 0, 128, 112);
        this.fm1_5.setArgs(2, 0, 0, 15, -1);
        this.fm1_5.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1_5.setScale(0.3f, 0.3f, 0.3f);
        this.fm1_5.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1_5.setRotate(0.0f, 3.15f, 0.0f);
        this.masyu_fm = new Unit();
        this.masyu_fm.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.masyu_fm.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.masyu_fm.setArgs(1, 22030, 0, 128, 112);
        this.masyu_fm.setArgs(2, 82, 0, 15, -1);
        this.masyu_fm.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.masyu_fm.signal(1);
        this.masyu_fm.setScale(0.17f, 0.17f, 0.17f);
        this.masyu_fm.setRotate(-23.0f, 0.0f, 0.0f);
        this.masyu_fm2 = new Unit();
        this.masyu_fm2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.masyu_fm2.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.masyu_fm2.setArgs(1, 22031, 0, 128, 112);
        this.masyu_fm2.setArgs(2, 0, 0, 15, -1);
        this.masyu_fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.masyu_fm2.setScale(0.17f, 0.17f, 0.17f);
        this.masyu_fm2.setRotate(-23.0f, 0.0f, 0.0f);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam4 = Camera.create(4);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.hammr = new human(0x1000116, 1.6f, 0.0f, 0.0f, 0.0f);
        this.hammr.renderCommand(22);
        this.hammr.setVisible(false);
        this.masyu = new human(0x1000113, 0.0f, 0.0f, 0.0f, 180.0f);
        this.masyu.renderCommand(22);
        this.masyu.setVisible(false);
        this.chaos = new human(0x1000003, 0.4f, 0.0f, 1.0f, 0.0f);
        this.chaos.renderCommand(22);
        this.chaos.setVisible(false);
        this.shion = new human(0x1000001, 0.6f, 0.0f, 0.0f, 0.0f);
        this.shion.renderCommand(22);
        this.shion.setVisible(false);
        this.allen = new human(0x1000107, 0.6f, 0.0f, 0.0f, 0.0f);
        this.allen.renderCommand(22);
        this.allen.setVisible(false);
        this.ziggy = new human(0x1000006, 0.6f, 0.0f, 0.0f, 0.0f);
        this.ziggy.renderCommand(22);
        this.ziggy.setVisible(false);
        this.momo = new human(0x1000004, 0.6f, 0.0f, 0.0f, 0.0f);
        this.momo.renderCommand(22);
        this.momo.setVisible(false);
        this.masyu.setMotNoUpdate(2);
        this.hammr.setMotNoUpdate(2);
        this.momo.setMotNoUpdate(2);
        this.ziggy.setMotNoUpdate(2);
        this.shion.setMotNoUpdate(2);
        this.allen.setMotNoUpdate(2);
        this.semotare = new mirror_map();
        this.semotare.init(47);
        this.semotare.start(4, null);
        this.semotare.setPivot(-2.98f, -0.03f, 1.59f);
        this.semotare2 = new units();
        this.semotare2.mapUnit(44);
        this.semotare2.start(4, null);
        this.zabuton = new units();
        this.zabuton.mapUnit(48);
        this.zabuton.start(4, null);
        this.isu1 = new units();
        this.isu1.mapUnit(37);
        this.isu1.start(4, null);
        this.isu2 = new units();
        this.isu2.mapUnit(38);
        this.isu2.start(4, null);
        this.isu3 = new units();
        this.isu3.mapUnit(39);
        this.isu3.start(4, null);
        this.isu4 = new mirror_map();
        this.isu4.init(40);
        this.isu4.start(4, null);
        this.isu5 = new units();
        this.isu5.mapUnit(41);
        this.isu5.start(4, null);
        this.isu6 = new units();
        this.isu6.mapUnit(42);
        this.isu6.start(4, null);
        this.isu7 = new mirror_map();
        this.isu7.init(36);
        this.isu7.start(4, null);
        this.isu7.setPivot(-3.0f, 0.05f, 6.55f);
        this.isu7.setTranslate(0.0f, 0.0f, 0.0f);
        this.isu7.setRotate(0.0f, 0.0f, 0.0f);
        this.isu5.setTranslate(-3.0f, -0.1f, 3.63f);
        this.isu5.setRotate(0.0f, 0.0f, 0.0f);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 0.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy3 = new units();
        this.dummy3.init(24602, 0.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy4 = new units();
        this.dummy4.init(24602, 0.0f, 1000.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this._spl_thread_main = Thread.create(this, "__chracter_splne");
        this._spl_thread_main.start();
        System.methodSignal(1);
    }

    void isu_move(float f) {
        this.f_sabun = 0.017920243f * f;
        this.f_isu5_py = 0.05f + Math.cos(Math.toRadians(f)) * 3.59f;
        this.f_isu5_pz = 3.626597f - this.f_sabun + (3.59f - Math.sin(Math.toRadians(f)) * 3.59f);
        this.isu1.setTranslate(this.isu5.px, this.f_isu5_py - 0.108f, this.f_isu5_pz - 0.76f);
        this.isu2.setTranslate(this.isu5.px, this.f_isu5_py - 0.01f, this.f_isu5_pz - 0.71f);
        this.isu3.setTranslate(this.isu5.px - 0.24f, this.f_isu5_py + 0.2022f, this.f_isu5_pz - 1.02f);
        this.isu4.setTranslate(this.isu5.px - 0.24f + 3.24f, this.f_isu5_py + 0.1977f - 2.42f, this.f_isu5_pz - 1.02f - 2.67f);
        this.isu5.setTranslate(this.isu5.px, this.f_isu5_py, this.f_isu5_pz);
        this.isu6.setRotate(-37.1891f + f, this.isu6.ry, this.isu6.rz);
        this.isu6.setTranslate(this.isu6.px, this.isu6.py, 7.21629f - this.f_sabun);
        this.masyu.setTranslate(this.isu1.px + this.masyu_zure_x, this.isu1.py + this.masyu_zure_y, this.isu1.pz + this.masyu_zure_z);
        this.masyu_fm.setTranslate(this.isu1.px + this.moni_zure_x, this.isu1.py + this.moni_zure_y, this.isu1.pz + this.moni_zure_z);
        this.masyu_fm2.setTranslate(this.isu1.px + this.moni_zure_x, this.isu1.py + this.moni_zure_y, this.isu1.pz + this.moni_zure_z);
        this.isu7.setRotate(-37.1891f + f, this.isu7.ry, this.isu7.rz);
        this.pinvot_sabun = 1.24f - 0.031297326f * (f + 2.439f);
        this.f_isu5_py = this.pinvot_sabun * Math.cos(Math.toRadians(39.62f));
        this.f_isu5_pz = this.pinvot_sabun * Math.sin(Math.toRadians(39.62f));
        this.isu7.setPivot(-3.0f, 0.05f - this.f_isu5_py, 6.55f + this.f_isu5_pz);
        this.isu7.setTranslate(this.isu7.px, 0.0f + this.f_isu5_py, 0.66629f - this.f_sabun + 0.0f - this.f_isu5_pz);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        this.ziggy.look_speed(10000.0f);
        this.ziggy.look_point(-3.03f, 0.53f, 0.68f);
        System.sleep(1);
        this.semotare.rotYCNS(this.hammr, 13, 1.1300097f, -0.49999994f, -0.27250415f);
        this.zabuton.rotYCNS(this.hammr, 13, 1.1300097f, -0.49999994f, -0.27250415f);
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.shion.face, "FLSshion.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.masyu.face, "FLSmatehws.fpk");
        this.loadarc(this.hammr.face, "FLShammer.fpk");
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.ziggy.face.mtn(4, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.momo.face.mtn(2, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.shion.face.mtn(8, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.allen.face.mtn(2, 8, 1.0f, false);
        this.allen.face.start(4, null);
        this.masyu.face.mtn(2, 8, 1.0f, false);
        this.masyu.face.start(4, null);
        this.hammr.face.mtn(2, 8, 1.0f, false);
        this.hammr.face.start(4, null);
        this.chaos.face.mtn(2, 8, 1.0f, false);
        this.chaos.face.start(4, null);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.__wait();
        this.isu4.setPivot(-3.24f, 2.42f, 2.67f);
        this.isu4.setTranslate(0.0f, 0.0f, 0.0f);
        this.isu4.setRotate(0.0f, 0.0f, 0.0f);
        Stage.setVisible(51, false);
        Stage.setVisible(52, false);
        Stage.setVisible(53, false);
        Stage.setVisible(54, false);
        Stage.setVisible(55, false);
        Stage.setVisible(56, false);
        Stage.setVisible(57, false);
        this.cam1.change();
        this.moni_zure_x = -0.09f;
        this.moni_zure_y = 0.58f;
        this.moni_zure_z = -0.58f;
        this.masyu_zure_x = 0.0f;
        this.masyu_zure_y = 0.17f;
        this.masyu_zure_z = -0.1f;
        this.isu_move(37.1891f);
        this.semotare2.setTranslate(3.0f, -0.03f, 1.59f);
        this.semotare2.setRotate(8.83f, 12.75f, 0.0f);
        this.shion.setTranslate(-2.3f, -0.53f, 1.49f);
        this.shion.setRotate(0.0f, 231.7f, 2.17f);
        this.ziggy.setTranslate(-2.01f, -0.5f, 1.84f);
        this.ziggy.setRotate(0.0f, 225.77f, 0.83f);
        this.allen.setTranslate(-2.47f, -0.49f, 1.86f);
        this.allen.setRotate(-5.53f, 217.62f, -2.83f);
        this.chaos.setTranslate(2.97f, -0.49f, 1.55f);
        this.chaos.setRotate(-3.17f, 180.35f, 0.0f);
        this.momo.setTranslate(-2.05f, -0.51f, 1.25f);
        this.momo.setRotate(0.0f, -112.35f, 0.0f);
        this.hammr.setTranslate(-3.01f, -0.45f, 1.5f);
        this.hammr.setRotate(0.0f, 180.0f, 0.0f);
        this.masyu.start(1, "mtn_001");
        this.hammr.start(1, "mtn_002");
        this.shion.start(1, "mtn_003");
        this.allen.start(1, "mtn_004");
        this.chaos.start(1, "mtn_005");
        this.ziggy.start(1, "mtn_006");
        this.momo.start(1, "mtn_007");
        Runtime.setDefocusQuick(0, 1, 9772, 1);
        Runtime.setDefocusQuick(1, 1, 7688, 1);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        Sound.streamPlay(1290058, 48000);
        this.camerawork.cut0001();
        this.hammr.setVisible(true);
        this.masyu.setVisible(true);
        this.shion.setVisible(true);
        this.allen.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(1, -0.451f, 0.031f, -0.892f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.795f, 0.607f, 0.002f);
        this.light.setColor(3, 0.25f, 0.25f, 0.24f);
        this.light.setDirection2(3, 0.318f, -0.694f, -0.646f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.chaos.setVisible(false);
        this.masyu.face.mtn(1, 0, 7, 0, 0, 1.0f, false);
        this.masyu.face.start(4, null);
        System.sleep(186);
        this.__wait();
        this.camerawork.cut0002();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.101f, 0.015f, -0.995f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.656f, 0.752f, 0.066f);
        this.light.setColor(3, 0.25f, 0.25f, 0.22f);
        this.light.setDirection2(3, 0.655f, -0.667f, -0.356f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fm1.signal(0);
        this.dummy4.start(1, "masyu_fm_irekae");
        this.masyu.renderCommand(0);
        this.chaos.renderCommand(0);
        this.allen.renderCommand(0);
        this.hammr.renderCommand(0);
        this.ziggy.renderCommand(0);
        Stage.renderCommand(0);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.masyu_zure_x = 0.0f;
        this.masyu_zure_y = 0.18f;
        this.masyu_zure_z = -0.11f;
        this.masyu.setRotate(8.0f, 180.0f, 2.0f);
        this.masyu.setTranslate(this.isu1.px + this.masyu_zure_x, this.isu1.py + this.masyu_zure_y, this.isu1.pz + this.masyu_zure_z);
        this.masyu.start(1, "mtn_008");
        System.sleep(12);
        this.MSGW("Heck, that wasn't so bad.");
        this.dummy1.start(1, "masyu_serifu_01_2");
        this.chaos.setVisible(true);
        this.hammr.setTranslate(-3.01f, -0.45f, 1.5f);
        this.hammr.setRotate(0.0f, 180.0f, 0.0f);
        this.chaos.setTranslate(2.97f, -0.49f, 1.55f);
        this.chaos.setRotate(-3.17f, 180.35f, 0.0f);
        this.shion.setTranslate(-2.3099997f, -0.53f, 1.4024976f);
        this.shion.setRotate(0.1333346f, 231.44995f, 1.866664f);
        this.allen.setTranslate(-2.5784905f, -0.49699986f, 1.858989f);
        this.allen.setRotate(1.2f, 223.0033f, 2.2666621f);
        this.ziggy.setTranslate(-2.1624677f, -0.49249986f, 1.7849855f);
        this.ziggy.setRotate(0.0f, 215.52f, 0.0f);
        this.momo.setTranslate(-1.8749982f, -0.50999993f, 1.1374995f);
        this.momo.setRotate(0.0f, -101.09999f, 0.0f);
        this.allen.start(1, "mtn_009_mae");
        this.shion.start(1, "mtn_010_mae");
        this.wait_clr(141);
        this.__wait();
        this.camerawork.cut0003();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.006f, 0.0f, -1.0f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, -0.288f, 0.027f, 0.957f);
        this.light.setColor(3, 0.17f, 0.17f, 0.16f);
        this.light.setDirection2(3, -0.782f, -0.483f, -0.394f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.momo.renderCommand(0);
        this.shion.renderCommand(0);
        this.masyu.renderCommand(22);
        this.chaos.renderCommand(22);
        this.allen.renderCommand(22);
        this.hammr.renderCommand(22);
        this.ziggy.renderCommand(22);
        Stage.renderCommand(22);
        this.masyu_fm2.signal(0);
        this.masyu.setMotNoUpdate(1);
        this._asi_yawaraka_off();
        this.masyu_fm.signal(0);
        this.masyu.setVisible(false);
        Runtime.setDefocusQuick(0, 2, 116072, 1);
        Runtime.setDefocusQuick(1, 2, 116072, 1);
        Runtime.setDefocusQuick(2, 1, 78572, 1);
        Runtime.setDefocusQuick(3, 1, 37072, 1);
        this.allen.start(1, "mtn_009");
        this.shion.start(1, "mtn_010");
        this.isu_move(33.0f);
        System.sleep(81);
        this.dummy2.start(1, "allen_serifu_01_3");
        this.allen.look_eye_speed(1.2f);
        this.allen.look_eye_set(4.0f, -2.0f);
        this.MSGW("Chief...");
        System.sleep(51);
        this.chaos.start(1, "mtn_005_2");
        this.dummy3.start(1, "shion_serifu_01_3");
        this.MSGW("Did...");
        this.shion.look_eye_speed(1.2f);
        this.shion.look_eye_set(-4.0f, 0.0f);
        System.sleep(36);
        this.allen_flag = 0;
        this.__wait();
        this.shion.start(1, "mtn_011");
        this.allen.start(1, "mtn_012");
        this.momo.start(1, "mtn_013");
        this.hammr.start(1, "mtn_014");
        this.camerawork.cut0004();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.006f, 0.0f, -1.0f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, -0.288f, 0.027f, 0.957f);
        this.light.setColor(3, 0.17f, 0.17f, 0.16f);
        this.light.setDirection2(3, -0.782f, -0.483f, -0.394f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fm1_2.signal(1);
        this.dummy4.start(1, "hammr_fm_irekae1");
        this._asi_yawaraka_on();
        this.masyu.renderCommand(0);
        this.chaos.renderCommand(0);
        this.allen.renderCommand(0);
        this.hammr.renderCommand(0);
        this.ziggy.renderCommand(0);
        Stage.renderCommand(0);
        this.masyu.setVisible(false);
        this.chaos.setVisible(false);
        Runtime.setDefocusQuick(0, 1, 53072, 1);
        Runtime.setDefocusQuick(1, 1, 36072, 1);
        Runtime.setDefocusQuick(2, 1, 19072, 1);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.ziggy.setTranslate(-2.14f, -0.5f, 1.81f);
        this.ziggy.setRotate(0.0f, 227.77f, 0.0f);
        this.shion.setTranslate(-2.31f, -0.52f, 1.49f);
        this.shion.setRotate(-0.17f, 224.67f, -0.17f);
        this.allen.setTranslate(-2.5599997f, -0.54f, 1.8619984f);
        this.allen.setRotate(-4.67f, 211.34f, -2.4999998f);
        this.momo.setTranslate(-2.05f, -0.51f, 1.27f);
        this.momo.setRotate(0.0f, -112.35f, 0.0f);
        this.hammr.setTranslate(-3.02f, -0.53f, 1.56f);
        this.hammr.setRotate(4.83f, 182.75f, 0.0f);
        this.semotare.setTranslate(-0.01f, 0.0f, 0.06f);
        this.semotare.setRotate(8.33f, 0.0f, 0.0f);
        System.sleep(15);
        this.shion.look_eye_set(0.0f, 0.0f);
        this.MSGW("...did that really just happen?");
        System.sleep(111);
        this.MSGW("Yeah...");
        this.dummy1.start(1, "allen_serifu_02_1");
        System.sleep(36);
        this.allen.look_eye_set(0.0f, 0.0f);
        this.MSGW("It appeared as if she\nabsorbed the Gnosis.");
        System.sleep(99);
        this.dummy2.start(1, "shion_serifu_02_1");
        this.MSGW("How could she have weaponry\nwe don't even know about...?");
        this.allen.look_eye_set(0.0f, 3.2f);
        this.wait_clr(96);
        System.sleep(15);
        this.__wait();
        this.shion.start(1, "mtn_015");
        this.allen.start(1, "mtn_016");
        this.hammr.start(1, "mtn_017");
        this.camerawork.cut0005();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -0.451f, 0.031f, -0.892f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, 0.811f, 0.585f, -0.025f);
        this.light.setColor(3, 0.25f, 0.25f, 0.24f);
        this.light.setDirection2(3, 0.318f, -0.694f, -0.646f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.masyu.setMotNoUpdate(0);
        Runtime.setDefocusQuick(0, 1, 79072, 1);
        Runtime.setDefocusQuick(1, 1, 51072, 1);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.ziggy.setTranslate(-2.0999875f, -0.49999994f, 1.9199998f);
        this.ziggy.setRotate(-3.47f, 214.66998f, 0.3f);
        this.allen.setTranslate(-2.67f, -0.52f, 2.01f);
        this.allen.setRotate(-6.17f, 181.49f, 0.5f);
        this.shion.setTranslate(-2.42f, -0.39f, 1.75f);
        this.shion.setRotate(-4.94f, 216.22f, -0.27f);
        this.shion.setScale(0.92f, 0.92f, 0.92f);
        this.hammr.setTranslate(-2.96f, -0.53f, 1.41f);
        this.hammr.setRotate(-2.12f, 170.92f, 7.09f);
        this.momo.setTranslate(-2.18f, -0.51f, 1.5f);
        this.momo.setRotate(-6.4f, -115.15f, -3.47f);
        this.semotare.setTranslate(0.0f, 0.0f, 0.0f);
        this.semotare.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.look_default();
        this.allen.look_speed(0.0f);
        this.allen.look_eye_speed(2.2f);
        this.allen.look_char(this.shion);
        System.sleep(18);
        this.dummy3.start(1, "shion_serifu_02_2");
        this.MSGW("No, that wasn't a weapon,");
        System.sleep(96);
        this.shion.look_eye_set(0.7f, 4.48f);
        this.MSGW("that was...");
        System.sleep(60);
        this.dummy1.start(1, "allen_serifu_02_2");
        this.MSGW("...something impossible.");
        this.cam3.setRotate(33.0f, 0.0f, 0.0f);
        System.sleep(60);
        this.masyu.setVisible(true);
        float[] fArray = new float[16];
        fArray[0] = 1.0f;
        fArray[1] = 33.0f;
        fArray[4] = 67.0f;
        fArray[5] = 19.5f;
        fArray[8] = 77.0f;
        fArray[9] = 19.5f;
        fArray[12] = 127.0f;
        fArray[13] = 19.5f;
        float[] fArray2 = fArray;
        this.cam3.rotateSPL(fArray2, 1);
        this.masyu_flag = 3;
        this.allen.look_eye_speed(0.92f);
        this.allen.look_char(this.masyu);
        this.MSGW("Who cares if it's possible or not?");
        this.dummy3.start(1, "masyu_serifu_02_2");
        System.sleep(30);
        this.__wait();
        this.ziggy.setTranslate(-2.17f, -0.5f, 1.74f);
        this.ziggy.setRotate(-3.47f, 216.17f, 0.3f);
        this.masyu_zure_x = 0.0f;
        this.masyu_zure_y = 0.17f;
        this.masyu_zure_z = -0.11f;
        this.masyu.setTranslate(this.isu1.px + this.masyu_zure_x, this.isu1.py + this.masyu_zure_y, this.isu1.pz + this.masyu_zure_z);
        this.camerawork.cut0006();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.941f, 0.0f, -0.339f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.919f, 0.387f, -0.079f);
        this.light.setColor(3, 0.25f, 0.25f, 0.23f);
        this.light.setDirection2(3, 0.037f, -0.586f, -0.809f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.fm1_3.signal(0);
        this.fm1_3.setTranslate(0.0f, 1000.0f, 0.0f);
        this.masyu_fm2.signal(1);
        this.dummy4.start(1, "masyu_fm2_off");
        this.shion.look_eye_set(0.0f, 0.0f);
        Runtime.setDefocusQuick(0, 1, 122572, 1);
        Runtime.setDefocusQuick(1, 1, 108072, 1);
        Runtime.setDefocusQuick(2, 1, 93572, 1);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.masyu.setRotate(4.57f, 180.0f, 1.72f);
        this.hammr.setTranslate(-3.12f, -0.49f, 1.48f);
        this.hammr.setRotate(-12.62f, 240.67f, -14.63f);
        this.allen.setVisible(false);
        this.shion.setVisible(false);
        this.masyu.start(1, "mtn_018");
        System.sleep(54);
        this.MSGW("She got rid of the Gnosis, right?");
        System.sleep(69);
        this.dummy1.start(1, "hammr_serifu_03_1");
        this.MSGW("Uhh...Captain?");
        System.sleep(36);
        this.__wait();
        this.allen.look_default();
        this.hammr.start(1, "mtn_019");
        this.masyu.start(1, "mtn_020");
        this.allen.start(1, "mtn_021");
        this.shion.start(1, "mtn_022");
        this.momo.start(1, "mtn_023");
        this.semotare.setPivot(-3.0f, -0.03f, 1.59f);
        this.masyu_zure_x = -0.0f;
        this.masyu_zure_y = 0.17f;
        this.masyu_zure_z = -0.09f;
        this.masyu.setTranslate(this.isu1.px + this.masyu_zure_x, this.isu1.py + this.masyu_zure_y, this.isu1.pz + this.masyu_zure_z);
        this.masyu.setRotate(10.52f, 180.0f, 1.72f);
        this.masyu_flag = 0;
        this.camerawork.cut0007();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(1, -0.703f, 0.229f, -0.673f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.328f, 0.807f, 0.491f);
        this.light.setColor(3, 0.28f, 0.28f, 0.27f);
        this.light.setDirection2(3, -0.931f, -0.341f, 0.132f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.setMotNoUpdate(1);
        this.chaos.setMotNoUpdate(1);
        this.shion.setVisible(true);
        this.chaos.setVisible(false);
        this.allen.setVisible(false);
        this.masyu.renderCommand(22);
        this.chaos.renderCommand(22);
        this.allen.renderCommand(22);
        this.hammr.renderCommand(22);
        this.ziggy.renderCommand(22);
        this.momo.renderCommand(22);
        this.shion.renderCommand(22);
        Stage.renderCommand(22);
        this.fm1_4.signal(1);
        this.fm3.signal(1);
        this.semotare.rotYCNS(this.hammr, 13, -1.8799994f, -0.49999994f, 1.9299952f);
        this.zabuton.rotYCNS(this.hammr, 13, -1.8799994f, -0.49999994f, 1.9299952f);
        this.isu_move(17.0f);
        Runtime.setDefocusQuick(0, 2, 83264, 1);
        Runtime.setDefocusQuick(1, 2, 83264, 1);
        Runtime.setDefocusQuick(2, 1, 46572, 1);
        Runtime.setDefocusQuick(3, 1, 27572, 1);
        this.shion.setTranslate(-2.54f, -0.61f, 1.43f);
        this.shion.setRotate(-0.95f, 209.63f, -0.35f);
        this.shion.setScale(1.0f, 1.0f, 1.0f);
        this.hammr.setTranslate(-3.15f, -0.37f, 1.57f);
        this.hammr.setRotate(0.0f, 245.64f, 0.0f);
        this.momo.setTranslate(-2.18f, -0.51f, 1.32f);
        this.momo.setRotate(4.55f, -89.2f, 1.75f);
        this.ziggy.setTranslate(-2.03f, -0.5f, 1.78f);
        this.ziggy.setRotate(-0.7f, 247.14f, -1.4f);
        System.sleep(36);
        this.dummy3.start(1, "hammr_serifu_03_3");
        this.MSGW("Little Master's been\nbuzzing us for awhile...");
        System.sleep(111);
        this.MSGW("Huh? Oh.\nPatch him in.");
        System.sleep(21);
        this.wait_clr(75);
        Stage.setVisible(3, true);
        Stage.setVisible(59, true);
        Stage.setVisible(61, true);
        Stage.setVisible(63, true);
        this.__wait();
        this.hammr.start(1, "mtn_024");
        this.shion.start(1, "mtn_025");
        this.allen.start(1, "mtn_026");
        this.momo.start(1, "mtn_027");
        this.masyu_zure_x = 0.03f;
        this.masyu_zure_y = 0.16f;
        this.masyu_zure_z = -0.09f;
        this.masyu.setTranslate(this.isu1.px + this.masyu_zure_x, this.isu1.py + this.masyu_zure_y, this.isu1.pz + this.masyu_zure_z);
        this.masyu.setRotate(-0.5999999f, 184.34892f, -2.2333317f);
        this.camerawork.cut0008();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.279f, 0.0f, -0.96f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, 0.811f, 0.585f, -0.025f);
        this.light.setColor(3, 0.25f, 0.25f, 0.24f);
        this.light.setDirection2(3, 0.318f, -0.694f, -0.646f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.masyu.start(1, "mtn_001");
        this.masyu.face.setVisible(false);
        this.masyu.renderCommand(22);
        this.chaos.renderCommand(22);
        this.allen.renderCommand(22);
        this.hammr.renderCommand(22);
        this.ziggy.renderCommand(22);
        Stage.renderCommand(22);
        this.allen.face.mtn(2, 8, 1.0f, false);
        this.allen.face.start(4, null);
        this.momo.face.mtn(8, 8, 1.0f, false);
        this.momo.face.start(4, null);
        this.dummy4.start(1, "hammr_fm_irekae3");
        this.masyu.setMotNoUpdate(1);
        this.allen.setMotNoUpdate(0);
        this.allen.setVisible(true);
        Runtime.setDefocusQuick(0, 1, 62880, 1);
        Runtime.setDefocusQuick(1, 1, 29688, 1);
        Runtime.setDefocusQuick(2, 1, 11880, 1);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.ziggy.look_speed(10000.0f);
        this.ziggy.look_point(-2.57f, 0.73f, 0.68f);
        this.momo.setTranslate(-2.22f, -0.51f, 1.29f);
        this.momo.setRotate(0.0f, -82.4f, 0.0f);
        this.ziggy.setTranslate(-1.86f, -0.51f, 1.52f);
        this.ziggy.setRotate(0.0f, 221.45f, 0.0f);
        this.hammr.setTranslate(-3.2f, -0.49f, 1.76f);
        this.hammr.setRotate(0.0f, 274.02f, 0.0f);
        this.allen.setTranslate(-2.12f, -0.51f, 2.18f);
        this.allen.setRotate(0.0f, 239.2f, 0.0f);
        this.shion.setTranslate(-2.48f, -0.51f, 1.51f);
        this.shion.setRotate(-4.56f, 225.96f, -0.01f);
        this.isu_move(20.0f);
        this.hammr.start(1, "mtn_024");
        this.shion.start(1, "mtn_025");
        this.allen.start(1, "mtn_026");
        this.momo.start(1, "mtn_027");
        this.cam3.setRotate(20.0f, 0.0f, 0.0f);
        System.sleep(9);
        float[] fArray3 = new float[12];
        fArray3[0] = 1.0f;
        fArray3[1] = 20.0f;
        fArray3[4] = 127.0f;
        fArray3[5] = 27.7f;
        fArray3[8] = 127.0f;
        fArray3[9] = 27.7f;
        float[] fArray4 = fArray3;
        this.cam3.rotateSPL(fArray4, 1, 3, 147);
        this.masyu_flag = 4;
        this.MSGW(" ");
        this.dummy1.start(1, "allen_serifu_04_3");
        System.sleep(279);
        this.MSGW("This isn't the KOS-MOS that I built...");
        System.sleep(132);
        this.__wait();
        this.shion.start(1, "mtn_028");
        this.hammr.start(1, "mtn_029");
        this.allen.start(1, "mtn_030");
        this.momo.start(1, "mtn_007");
        this.camerawork.cut0009();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.279f, 0.0f, -0.96f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, 0.811f, 0.585f, -0.025f);
        this.light.setColor(3, 0.25f, 0.25f, 0.24f);
        this.light.setDirection2(3, 0.318f, -0.694f, -0.646f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.hammr.setTranslate(-2.924991f, -0.5149999f, 1.6849988f);
        this.hammr.setRotate(0.0f, 426.52f, 0.0f);
        this.semotare.setTranslate(-0.025000062f, 0.0f, 0.025000121f);
        this.semotare.setRotate(-3.333333f, -4.9999995f, 0.0f);
        this.semotare.rotYCNS(this.hammr, 13, -2.0099945f, -0.50999993f, -0.3550021f);
        this.zabuton.rotYCNS(this.hammr, 13, 0.4400067f, -0.50999993f, -14.854822f);
        Runtime.setDefocusQuick(0, 1, 240072, 1);
        Runtime.setDefocusQuick(1, 1, 240072, 1);
        Runtime.setDefocusQuick(2, 1, 87072, 1);
        Runtime.setDefocusQuick(3, 1, 56072, 1);
        this.shion.setTranslate(-2.54f, -0.51f, 1.42f);
        this.shion.setRotate(-4.56f, -164.49f, -0.78f);
        this.dummy1.start(1, "allen_serifu_04_4");
        System.sleep(6);
        this.MSGW("Kevin...");
        this.wait_clr(66);
        System.sleep(30);
        this.MSGW("Is this your hand at work...?");
        this.wait_clr(201);
        this.__wait();
        this.camerawork.cut0010();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.279f, 0.0f, -0.96f);
        this.light.setColor(2, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(2, 0.811f, 0.585f, -0.025f);
        this.light.setColor(3, 0.25f, 0.25f, 0.24f);
        this.light.setDirection2(3, 0.318f, -0.694f, -0.646f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.masyu.setVisible(false);
        this.ziggy.setVisible(false);
        this.chaos.setVisible(false);
        this.masyu.renderCommand(0);
        this.chaos.renderCommand(0);
        this.allen.renderCommand(0);
        this.hammr.renderCommand(0);
        this.ziggy.renderCommand(0);
        this.shion.renderCommand(0);
        this.momo.renderCommand(0);
        Stage.renderCommand(0);
        Runtime.setDefocusQuick(0, 1, 252880, 1);
        Runtime.setDefocusQuick(1, 1, 213688, 1);
        Runtime.setDefocusQuick(2, 1, 174496, 1);
        Runtime.setDefocusQuick(3, 1, 135304, 1);
        this.zabuton.setTranslate(-3.04f, -0.05f, 1.71f);
        this.semotare.setTranslate(-0.04f, 0.0f, 0.12f);
        this.allen.setTranslate(-2.03f, -0.51f, 2.21f);
        this.allen.setRotate(0.0f, 235.45f, 0.0f);
        this.hammr.setTranslate(-2.9199986f, -0.49f, 1.7324984f);
        this.hammr.setRotate(0.0f, 61.519993f, 0.0f);
        this.shion.setTranslate(-2.5f, -0.51f, 1.54f);
        this.shion.setRotate(-9.73f, -170.49f, -0.01f);
        this.momo.setTranslate(-2.25f, -0.51f, 1.25f);
        this.momo.setRotate(-4.33f, -73.9f, 0.17f);
        this.shion.start(1, "mtn_031");
        this.hammr.start(1, "mtn_029_2");
        System.sleep(36);
        this.MSGW("Is this the real KOS-MOS\nthat you wished to create...?");
        this.wait_clr(156);
        this.masyu_flag = 0;
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
            this.setShadow(0, 0);
        }

        public void mtn_001() {
            this.mtn(257, 8, 1.0f, true);
        }

        public void mtn_002() {
            this.mtn(258, 8, 1.0f, true);
        }

        public void mtn_003() {
            this.mtn(259, 8, 1.0f, true);
        }

        public void mtn_004() {
            this.mtn(260, 8, 1.0f, true);
        }

        public void mtn_005() {
            this.mtn(261, 82, 270, 0, 0, 1.0f, true);
        }

        public void mtn_005_2() {
            this.mtn(261, 0, 270, 0, 0, -0.72f, true);
        }

        public void mtn_006() {
            this.mtn(262, 8, 1.0f, true);
        }

        public void mtn_006_STOP() {
            this.mtn(262, 0, 0, 0, 0, 1.0f, true);
        }

        public void mtn_007() {
            this.mtn(263, 8, 1.0f, true);
        }

        public void mtn_008() {
            this.mtn(264, 0, 0.72f, true);
        }

        public void mtn_009() {
            this.mtn(265, 0, 1.0f, true);
        }

        public void mtn_009_mae() {
            this.mtn(265, 0, 0, 0, 0, 1.0f, true);
        }

        public void mtn_010() {
            this.mtn(266, 0, 0.98f, true);
        }

        public void mtn_010_mae() {
            this.mtn(266, 0, 0, 0, 0, 0.98f, true);
        }

        public void mtn_011() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(267, 0, 0.86f, true);
        }

        public void mtn_012() {
            this.setMotionFlags(0x2000000, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.mtn(268, 0, 0.86f, true);
        }

        public void mtn_013() {
            this.mtn(269, 8, 0.86f, true);
        }

        public void mtn_014() {
            this.mtn(270, 0, 0.86f, true);
        }

        public void mtn_015() {
            this.mtn(271, 0, 0.84f, true);
        }

        public void mtn_016() {
            this.mtn(272, 0, 0.84f, true);
        }

        public void mtn_017() {
            this.mtn(273, 8, 0.84f, true);
        }

        public void mtn_018() {
            this.mtn(274, 0, 0.69f, true);
        }

        public void mtn_019() {
            this.mtn(275, 8, 0.66f, true);
        }

        public void mtn_020() {
            this.mtn(276, 0, 0.66f, true);
        }

        public void mtn_021() {
            this.mtn(277, 8, 1.0f, true);
        }

        public void mtn_022() {
            this.mtn(278, 8, 1.0f, true);
        }

        public void mtn_023() {
            this.mtn(279, 8, 1.0f, true);
        }

        public void mtn_024() {
            this.mtn(280, 0, 0.7f, true);
        }

        public void mtn_025() {
            this.mtn(281, 0, 0.7f, true);
        }

        public void mtn_026() {
            this.mtn(282, 0, 0.7f, true);
        }

        public void mtn_027() {
            this.mtn(283, 0, 0.7f, true);
        }

        public void mtn_028() {
            this.mtn(284, 0, 0.66f, true);
        }

        public void mtn_029() {
            this.mtn(285, 8, 0.66f, true);
        }

        public void mtn_029_2() {
            this.mtn(285, 54, 202, 0, 0, 0.62f, true);
        }

        public void mtn_030() {
            this.mtn(286, 0, 0.66f, true);
        }

        public void mtn_031() {
            this.mtn(287, 0, 0.67f, true);
            this.mtn(287, 0, -0.67f, true);
        }
    }

    class units
            extends Unit {
        units() {
        }

        void allen_serifu_01_3() {
            SCE02061B.this.FACE(21, SCE02061B.this.allen, 1, 0.52f);
        }

        void allen_serifu_02_1() {
            SCE02061B.this.FACE(12, SCE02061B.this.allen, 1, 0.72f);
            System.sleep(24);
            SCE02061B.this.FACE(69, SCE02061B.this.allen, 1, 0.92f);
        }

        void allen_serifu_02_2() {
            SCE02061B.this.FACE(42, SCE02061B.this.allen, 1, 1.12f);
        }

        void allen_serifu_04_3() {
            System.sleep(126);
            SCE02061B.this.FACE(18, SCE02061B.this.allen, 1, 0.72f);
            System.sleep(42);
            SCE02061B.this.FACE(30, SCE02061B.this.allen, 1, 0.92f);
            System.sleep(12);
            SCE02061B.this.FACE(36, SCE02061B.this.hammr, 1, 1.27f);
            SCE02061B.this.FACE_SMOOTH(12, SCE02061B.this.allen, 3, 0.92f);
            SCE02061B.this.FACE_SMOOTH(126, SCE02061B.this.hammr, 3, 0.92f);
        }

        void allen_serifu_04_4() {
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(66, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(66, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
            SCE02061B.this.FACE(27, SCE02061B.this.hammr, 3, 1.27f);
            SCE02061B.this.FACE(36, SCE02061B.this.allen, 3, 0.72f);
        }

        public void door_close() {
            float f = -0.76f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02061B.this.cam2.getRotateX() - f;
                this.setTranslate(this.px - f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        public void door_open() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02061B.this.cam2.getRotateX() - f;
                this.setTranslate(this.px - f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        void hammr_fm_irekae1() {
            System.sleep(72);
            SCE02061B.this.fm1_3.signal(1);
            float f = 82.0f;
            float f2 = 0.0f;
            int n = 0;
            while (n < 22) {
                SCE02061B.this.fm1_2.setArgs(2, (int) f, 0, 15, -1);
                SCE02061B.this.fm1_3.setArgs(2, (int) f2, 0, 15, -1);
                System.sleep(1);
                f -= 3.7272727f;
                f2 += 3.7272727f;
                ++n;
            }
            SCE02061B.this.fm1_3.setArgs(2, 72, 0, 15, -1);
            SCE02061B.this.fm1_2.signal(0);
        }

        void hammr_fm_irekae3() {
            System.sleep(102);
            SCE02061B.this.fm1_5.signal(1);
            float f = 82.0f;
            float f2 = 0.0f;
            int n = 0;
            while (n < 22) {
                SCE02061B.this.fm1_4.setArgs(2, (int) f, 0, 15, -1);
                SCE02061B.this.fm1_5.setArgs(2, (int) f2, 0, 15, -1);
                System.sleep(1);
                f -= 3.7272727f;
                f2 += 3.7272727f;
                ++n;
            }
            SCE02061B.this.fm1_5.setArgs(2, 82, 0, 15, -1);
            SCE02061B.this.fm1_4.signal(0);
        }

        void hammr_serifu_03_1() {
            System.sleep(36);
            SCE02061B.this.FACE(18, SCE02061B.this.hammr, 1, 1.0f);
        }

        void hammr_serifu_03_3() {
            SCE02061B.this.FACE(24, SCE02061B.this.hammr, 1, 1.27f);
            System.sleep(12);
            SCE02061B.this.FACE_SMOOTH(60, SCE02061B.this.hammr, 3, 1.127f);
        }

        void masyu_fm2_off() {
            System.sleep(42);
            float f = 1.4f;
            int n = 1;
            while (n < 9) {
                SCE02061B.this.masyu_fm2.setArgs(0, 0.0f, 0.0f, 1.6f, f -= 0.15555555f);
                System.sleep(1);
                ++n;
            }
            SCE02061B.this.masyu_fm2.setArgs(0, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02061B.this.masyu_fm2.signal(0);
        }

        void masyu_fm_irekae() {
            System.sleep(32);
            SCE02061B.this.masyu_fm2.signal(1);
            float f = 82.0f;
            float f2 = 0.0f;
            int n = 0;
            while (n < 22) {
                SCE02061B.this.masyu_fm.setArgs(2, (int) f, 0, 15, -1);
                SCE02061B.this.masyu_fm2.setArgs(2, (int) f2, 0, 15, -1);
                System.sleep(1);
                f -= 3.7272727f;
                f2 += 3.7272727f;
                ++n;
            }
            SCE02061B.this.masyu_fm2.setArgs(2, 82, 0, 15, -1);
            SCE02061B.this.masyu_fm.signal(0);
        }

        void masyu_serifu_01_2() {
            SCE02061B.this.FACE_SMOOTH(39, SCE02061B.this.masyu, 1, 0.42f);
            System.sleep(15);
            SCE02061B.this.FACE_SMOOTH(39, SCE02061B.this.masyu, 3, 0.92f);
        }

        void masyu_serifu_02_2() {
            SCE02061B.this.FACE(72, SCE02061B.this.masyu, 1, 0.92f);
            System.sleep(12);
            SCE02061B.this.FACE(57, SCE02061B.this.masyu, 1, 1.12f);
        }

        void shion_serifu_01_3() {
            SCE02061B.this.FACE_SMOOTH(15, SCE02061B.this.shion, 7, 0.72f);
            System.sleep(75);
            SCE02061B.this.FACE_SMOOTH(60, SCE02061B.this.shion, 7, 0.72f);
        }

        void shion_serifu_02_1() {
            SCE02061B.this.FACE(72, SCE02061B.this.shion, 7, 0.82f);
        }

        void shion_serifu_02_2() {
            SCE02061B.this.FACE(21, SCE02061B.this.shion, 7, 0.12f);
            System.sleep(30);
            SCE02061B.this.FACE(39, SCE02061B.this.shion, 7, 0.92f);
            System.sleep(36);
            SCE02061B.this.FACE(15, SCE02061B.this.shion, 7, 0.92f);
        }
    }

    class mirror_map
            extends MAPUnit {
        mirror_map() {
        }

        public void door_close_2() {
            float f = -0.76f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02061B.this.cam2.getRotateX() - f;
                this.setTranslate(this.px + f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }

        public void door_open_2() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 0;
            do {
                f2 = SCE02061B.this.cam2.getRotateX() - f;
                this.setTranslate(this.px + f2, this.py, this.pz);
                System.sleep(1);
                f = f2 + f;
            } while (++n != 50);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0001() {
            SCE02061B.this.cam1.setFov(31.02f);
            SCE02061B.this.cam0.setFov(31.02f);
            float[] fArray = new float[]{1.0f, -5.61f, 3.35f, -5.31f, 172.0f, -5.5f, 3.31f, -5.1f, 343.0f, -5.48f, 3.3f, -5.03f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -15.03f;
            fArray2[2] = -157.96f;
            fArray2[4] = 172.0f;
            fArray2[5] = -14.96f;
            fArray2[6] = -158.56f;
            fArray2[8] = 343.0f;
            fArray2[9] = -14.95f;
            fArray2[10] = -158.92f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0002() {
            SCE02061B.this.cam1.setFov(26.17f);
            SCE02061B.this.cam0.setFov(26.17f);
            float[] fArray = new float[]{1.0f, -4.08f, 3.12f, 1.26f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.98f;
            fArray2[2] = -143.68f;
            fArray2[4] = 37.0f;
            fArray2[5] = -10.98f;
            fArray2[6] = -144.40001f;
            fArray2[8] = 82.0f;
            fArray2[9] = -10.98f;
            fArray2[10] = -144.58f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0003() {
            SCE02061B.this.cam1.setFov(22.27f);
            SCE02061B.this.cam2.setFov(22.27f);
            float[] fArray = new float[]{1.0f, -4.25f, 1.23f, 1.64f, 527.0f, -4.25f, 1.2f, 1.55f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -8.78f;
            fArray2[2] = -89.77f;
            fArray2[4] = 527.0f;
            fArray2[5] = -8.78f;
            fArray2[6] = -89.77f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0004() {
            SCE02061B.this.cam1.setFov(33.94f);
            SCE02061B.this.cam2.setFov(33.94f);
            float[] fArray = new float[]{1.0f, -4.12f, 0.81f, 0.32f, 677.0f, -3.75f, 0.81f, 0.15f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.45f;
            fArray2[2] = 237.82f;
            fArray2[4] = 677.0f;
            fArray2[5] = -3.45f;
            fArray2[6] = 219.11f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0005() {
            SCE02061B.this.cam1.setFov(23.75f);
            SCE02061B.this.cam0.setFov(23.75f);
            float[] fArray = new float[]{1.0f, -2.37f, 0.86f, 0.06f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -1.12f;
            fArray2[2] = -188.46f;
            fArray2[4] = 327.0f;
            fArray2[5] = 4.27f;
            fArray2[6] = -188.46f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 0);
            SCE02061B.this.cam1.rotateSPL(fArray3, 0);
        }

        public void cut0006() {
            SCE02061B.this.cam1.setFov(30.68f);
            SCE02061B.this.cam0.setFov(30.68f);
            float[] fArray = new float[]{1.0f, -2.63f, 0.97f, 1.39f, 177.0f, -2.63f, 0.97f, 1.39f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 34.06f;
            fArray2[2] = 158.91f;
            fArray2[4] = 177.0f;
            fArray2[5] = 34.97f;
            fArray2[6] = 158.34f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[]{1.0f, 30.67f, 177.0f, 30.77f};
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
            SCE02061B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut0007() {
            SCE02061B.this.cam1.setFov(35.517f);
            SCE02061B.this.cam2.setFov(35.517f);
            float[] fArray = new float[]{1.0f, -3.71f, 2.76f, 2.78f, 277.0f, -3.71f, 2.81f, 2.79f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -56.1f;
            fArray2[2] = -43.12f;
            fArray2[4] = 277.0f;
            fArray2[5] = -56.1f;
            fArray2[6] = -41.6f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0008() {
            SCE02061B.this.cam1.setFov(24.07f);
            SCE02061B.this.cam0.setFov(24.07f);
            float[] fArray = new float[]{1.0f, -2.23f, 1.16f, -2.48f, 211.5f, -2.26f, 1.13f, -2.22f, 423.0f, -2.29f, 1.09f, -2.01f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.88f;
            fArray2[2] = 173.65f;
            fArray2[4] = 211.5f;
            fArray2[5] = -3.83f;
            fArray2[6] = 173.65f;
            fArray2[8] = 423.0f;
            fArray2[9] = -3.46f;
            fArray2[10] = 173.65f;
            float[] fArray3 = fArray2;
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut0009() {
            SCE02061B.this.cam1.setFov(23.02f);
            SCE02061B.this.cam0.setFov(23.02f);
            float[] fArray = new float[]{1.0f, -2.43f, 0.96f, -0.17f, 327.0f, -2.46f, 0.96f, 0.11f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.06f;
            fArray2[2] = 174.34f;
            fArray2[4] = 327.0f;
            fArray2[5] = -2.86f;
            fArray2[6] = 174.27f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[]{1.0f, 23.02f, 327.0f, 21.99f};
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
            SCE02061B.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut0010() {
            SCE02061B.this.cam1.setFov(24.07f);
            SCE02061B.this.cam0.setFov(24.07f);
            float[] fArray = new float[]{1.0f, -2.29f, 1.33f, 0.69f, 240.0f, -2.48f, 1.29f, 0.65f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -46.57f;
            fArray2[2] = 160.51f;
            fArray2[4] = 240.0f;
            fArray2[5] = -46.57f;
            fArray2[6] = 178.75f;
            float[] fArray3 = fArray2;
            float[] fArray4 = new float[]{1.0f, 24.07f, 240.0f, 22.77f};
            SCE02061B.this.cam1.transSPL(fArray, 1);
            SCE02061B.this.cam1.rotateSPL(fArray3, 1);
            SCE02061B.this.cam1.fovSPL(fArray4, 1);
        }
    }
}

