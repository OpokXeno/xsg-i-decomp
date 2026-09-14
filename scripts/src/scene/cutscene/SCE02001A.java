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
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.util.Vector4f;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02001A
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02001A,
        MC_PRO08_PRJ,
        JNT_Human,
        FLSyuri,
        FLScommi,
        FLScommi1,
        FLScommi2,
        FLScommi3,
        FLScommi4,
        FLScommi5,
        FLSziggy {
    static final int hat_time = 60;
    static final float MON_H = 0.9f;
    static final float yuri_isu_x = -2.7f;
    static final float yuri_isu_z = -2.7f;
    static final float yuri_ry = 45.0f;
    static final float yuri_x = -2.82f;
    static final float yuri_y = 0.0f;
    static final float yuri_z = -2.82f;
    static final float mon_y_px = -2.54f;
    static final float mon_y_py = 0.9f;
    static final float mon_y_pz = -2.55f;
    static final float yuri_a = -0.119999886f / Math.cos(Math.toRadians(45.0f));
    static final float mon_y_a = 0.16000009f / Math.cos(Math.toRadians(45.0f));
    static final float com2_isu_x = 2.7f;
    static final float com2_isu_z = -2.7f;
    static final float com_2_ry = 315.0f;
    static final float com_2_x = 2.82f;
    static final float com_2_y = 0.0f;
    static final float com_2_z = -2.82f;
    static final float mon_2_px = 2.4099998f;
    static final float mon_2_py = 0.9f;
    static final float mon_2_pz = -2.37f;
    static final float com_2_a = 0.119999886f / Math.cos(Math.toRadians(315.0f));
    static final float mon_2_a = -0.2900002f / Math.cos(Math.toRadians(315.0f));
    static final float com4_isu_x = 2.7f;
    static final float com4_isu_z = 2.7f;
    static final float com_4_ry = 225.0f;
    static final float com_4_x = 2.82f;
    static final float com_4_y = 0.0f;
    static final float com_4_z = 2.82f;
    static final float mon_4_px = 2.32f;
    static final float mon_4_py = 0.9f;
    static final float mon_4_pz = 2.37f;
    static final float com_4_a = 0.119999886f / Math.cos(Math.toRadians(225.0f));
    static final float mon_4_a = -0.3800001f / Math.cos(Math.toRadians(225.0f));
    static final float com5_isu_x = -2.7f;
    static final float com5_isu_z = 2.7f;
    static final float com_5_ry = 135.0f;
    static final float com_5_x = -2.82f;
    static final float com_5_y = 0.0f;
    static final float com_5_z = 2.82f;
    static final float mon_5_px = -2.4199998f;
    static final float mon_5_py = 0.9f;
    static final float mon_5_pz = 2.37f;
    static final float com_5_a = -0.119999886f / Math.cos(Math.toRadians(135.0f));
    static final float mon_5_a = 0.2800002f / Math.cos(Math.toRadians(135.0f));
    static final float com0_isu_x = 0.0f;
    static final float com0_isu_z = 3.8f;
    static final float com_0_ry = 180.0f;
    static final float com_0_x = 0.0f;
    static final float com_0_y = 0.0f;
    static final float com_0_z = 4.0f;
    static final float mon_0_px = 0.0f;
    static final float mon_0_py = 0.9f;
    static final float mon_0_pz = 3.55f;
    static final float com_0_a = 0.0f / Math.cos(Math.toRadians(180.0f));
    static final float mon_0_a = 0.0f / Math.cos(Math.toRadians(180.0f));
    static final float com1_isu_x = -3.8f;
    static final float com1_isu_z = 0.0f;
    static final float com_1_ry = 90.0f;
    static final float com_1_x = -4.0f;
    static final float com_1_y = 0.0f;
    static final float com_1_z = 0.0f;
    static final float mon_1_px = -3.4f;
    static final float mon_1_py = 0.9f;
    static final float mon_1_pz = 0.0f;
    static final float com_1_a = -0.20000005f / Math.cos(Math.toRadians(90.0f));
    static final float mon_1_a = 0.39999986f / Math.cos(Math.toRadians(90.0f));
    static final float com3_isu_x = 3.8f;
    static final float com3_isu_z = 0.0f;
    static final float com_3_ry = 270.0f;
    static final float com_3_x = 4.0f;
    static final float com_3_y = 0.0f;
    static final float com_3_z = 0.0f;
    static final float mon_3_px = 3.4f;
    static final float mon_3_py = 0.9f;
    static final float mon_3_pz = 0.0f;
    static final float com_3_a = 0.20000005f / Math.cos(Math.toRadians(270.0f));
    static final float mon_3_a = -0.39999986f / Math.cos(Math.toRadians(270.0f));
    static final int win_y = 30;
    static final int ANM_HAIR_NOFLOOR = 0x400000;
    boolean syaku = false;
    boolean snd_chk = true;
    boolean snd_chk2 = false;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    Thread thread1;
    Thread _spl_thread_main;
    Unit gold_unit;
    Chr gold;
    Input pad1;
    Input pad0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera BaseCam;
    Window win;
    Light light = new Light(0);
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    Thread camera_thread;
    static final int Chand_R = 72;
    static final int Chand_L = 60;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    int BGinit = 0;
    Monitor Mon;
    Monitor Mon2;
    Monitor Mon3;
    Monitor Mon4;
    Monitor Mon_2;
    Monitor Mon_22;
    Monitor Mon_4;
    Monitor Mon_42;
    Monitor Mon_5;
    Monitor Mon_52;
    Monitor Mon_0;
    Monitor Mon_02;
    Monitor Mon_1;
    Monitor Mon_12;
    Monitor Mon_3;
    Monitor Mon_32;
    Monitor Mon_Y;
    Monitor Mon_Y2;
    Unit elv;
    Unit hat1;
    Unit hat2;
    Unit hat3;
    Unit hat4;
    Unit hat5;
    Unit hat6;
    Unit t_isu;
    Unit yuri_isu;
    Unit com1_isu;
    Unit com5_isu;
    Unit com0_isu;
    Unit com4_isu;
    Unit com3_isu;
    Unit com2_isu;
    Unit t_h1;
    Unit yuri_h1;
    Unit com1_h1;
    Unit com5_h1;
    Unit com0_h1;
    Unit com4_h1;
    Unit com3_h1;
    Unit com2_h1;
    Unit t_h2;
    Unit yuri_h2;
    Unit com1_h2;
    Unit com5_h2;
    Unit com0_h2;
    Unit com4_h2;
    Unit com3_h2;
    Unit com2_h2;
    Unit Stick;
    Unit StickL;
    Unit Mon_13;
    Unit Tobacco;
    Effect eft0;
    Effect eft1;
    Ziggy ziggy;
    Woman woman;
    Yuri yuri;
    Com_2 com_2;
    Com_4 com_4;
    Com_5 com_5;
    Com_0 com_0;
    Com_1 com_1;
    Com_3 com_3;
    Effect whiteOut;
    int kind_c0_u1 = 0;
    float rb_x = 0.0f;
    float rb_y = 0.0f;
    float rb_z = 0.0f;
    float rb_rx = 0.0f;
    float rb_ry = 0.0f;
    float rb_rz = 0.0f;
    float px = 0.0f;
    float py = 0.0f;
    float pz = 0.0f;
    float rx = 0.0f;
    float ry = 0.0f;
    float rz = 0.0f;
    float fov_new = 0.0f;
    int ColorScreen_flag = 0;
    int camtool0_objtool1_flag = 1;
    Vector4f obj_scale;
    public static final int CASE_MAX = 37;
    boolean FocusSet_exec;
    int Focus_z = 131072;
    int Focus_zwide = 8192;
    int Focus_back = 1;
    int Focus_before = 1;
    int Focus_before_layer = 1;
    int Focus_back_layer = 3;
    static final int Layer_num = 4;
    int Focus_layer = 3;
    boolean Focus_flag;
    int alpha_percent = 64;
    int alpha_flag = 0;
    int set_norm2 = 2;
    int set_norm1 = 1;
    int dfq4_param = 0;
    int focus_flag = 0;
    int Focus_work0 = 0;
    int Focus_work1 = 0;
    int _PADR3 = 1024;
    int _PADL3 = 512;
    int btn1;
    int btn2;
    int edge1;
    int edge2;
    int rep1;
    int rep2;
    int msgsignal = 128;
    int cutwait = 0;
    boolean msgflag = true;
    Thread msg_clear = Thread.create(this, "msg_clear_thread");
    int msg_clearwait;
    boolean msg_clear_exec;
    Thread timer_thread;
    Thread timer_thread_srv;
    int CutNo = 0;
    boolean Timechk_srv_exec;
    int Timechk_srv_time;
    int Timechk_srv_totaltime;
    int Timechk_NextCut;
    Thread Capture_thread;
    Thread Capture_thread_srv;
    boolean Capture_thread_srv_exec = false;
    Menu Capturemenu;
    int Capture_mode;
    int CaptureFolder = 0;
    String FolderName0 = "eventmovie/";
    String FolderName;
    Menu menu;
    boolean ColorScreenSet_exec;
    int Color_R = 0;
    int Color_G = 0;
    int Color_B = 0;
    int Color_A = 96;
    int Color_Z = 147456;
    int Color_Z_2 = 5000;
    int Color_Zwide = 32768;
    int Color_layer = 1;
    int Color_work = 0;
    int Color_work2 = 0;
    int[] ColorScreenParam;
    int[] test_param0;
    int[] test_param1;
    int CamHistory_max;
    int CamHistory_ptr;
    int CamHistorywork;
    int CamHistorywork2;
    int CamHistorywork3;
    int work0;
    int work1;
    float CamHistoryposX;
    float CamHistoryposY;
    float CamHistoryposZ;
    float CamHistoryrotX;
    float CamHistoryrotY;
    float CamHistoryrotZ;
    float CamHistoryFov;
    float[] ManualCamRecord;
    float[] ManualCampos;
    float[] ManualCamrot;
    float[] ManualCamfov;
    int Manualcamrecord_dt;
    int Manualcamrecord_max;
    boolean Manualcamrecord_exec;
    int Manualcamrecord_time;
    int Manualcamrecord_ptr;
    float Manualcamrec_posX;
    float Manualcamrec_posY;
    float Manualcamrec_posZ;
    float Manualcamrec_rotX;
    float Manualcamrec_rotY;
    float Manualcamrec_rotZ;
    float Manualcamrec_fov;
    float Objchk_dx;
    float Objchk_dy;
    float Objchk_dz;
    float Objchk_drx;
    float Objchk_dry;
    float Objchk_drz;
    float Objchk_dsx;
    float Objchk_dsy;
    float Objchk_dsz;
    float Objchk_scalex;
    float Objchk_scaley;
    float Objchk_scalez;
    float Objchk_scale;
    Vector4f ObjchkScale;
    boolean Objchk_exec;
    int Objchk_key1;
    int mtnframe;
    Input Xpad1P;
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag;

    SCE02001A() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 1614815232;
        this.ColorScreenParam = nArray;
        int[] nArray2 = new int[2];
        nArray2[1] = this.Color_Z_2;
        this.test_param0 = nArray2;
        int[] nArray3 = new int[6];
        nArray3[0] = 1;
        nArray3[1] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
        this.test_param1 = nArray3;
        this.CamHistory_max = 10;
        this.CamHistory_ptr = 0;
        this.ManualCamRecord = new float[140];
        this.ManualCampos = new float[84];
        this.ManualCamrot = new float[84];
        this.ManualCamfov = new float[42];
        this.Manualcamrecord_dt = 15;
        this.Manualcamrecord_max = 20;
        this.Manualcamrecord_exec = false;
        this.Manualcamrecord_time = 0;
        this.Objchk_dx = 0.0f;
        this.Objchk_dy = 0.0f;
        this.Objchk_dz = 0.0f;
        this.Objchk_drx = 0.0f;
        this.Objchk_dry = 0.0f;
        this.Objchk_drz = 0.0f;
        this.Objchk_dsx = 0.0f;
        this.Objchk_dsy = 0.0f;
        this.Objchk_dsz = 0.0f;
        this.Objchk_scalex = 1.0f;
        this.Objchk_scaley = 1.0f;
        this.Objchk_scalez = 1.0f;
        this.Objchk_scale = 1.0f;
        this.Objchk_key1 = 2048;
        this.mtnframe = 0;
        this.Xpad1P = Input.create(0);
        this.Xenvmainthreadendflag = false;
    }

    float Abs(float f) {
        if (f < 0.0f) {
            return f * -1.0f;
        }
        return f;
    }

    int Abs(int n) {
        if (n < 0) {
            return n * -1;
        }
        return n;
    }

    void CaptureEnd_CaptureTool() {
        Runtime.CaptureEnd();
        this.msgflag = true;
    }

    void CaptureStart_CaptureTool() {
        if (this.CaptureFolder >= 10) {
            this.msgflag = true;
        } else if (this.Capture_mode != 36864) {
            this.msgflag = false;
            this.msg.clear();
        }
        System.println("*********Capture Start!!!*****************");
        Runtime.setRegister(0, this.Capture_mode);
        System.println("CaptureMode = /[$0]");
        Runtime.CaptureStart(this.FolderName, this.Capture_mode);
    }

    void CaptureTool() {
    }

    void CaptureTool_main() {
        Input input = Input.create(0);
        Input input2 = Input.create(1);
        System.println("*********CaptureTool Standby*****************");
        while (true) {
            int n = input.getButton();
            int n2 = input.getEdge();
            int n3 = input.getRepeat();
            if ((n2 & 0x20) != 0 && !this.Capture_thread_srv_exec) {
                this.Capture_thread_srv = Thread.create(this, "Capture_wait");
                this.Capture_thread_srv.start();
            }
            if ((n2 & 0x40) != 0 && this.Capture_thread_srv_exec) {
                System.println("Capture Cancel.");
                this.CaptureEnd_CaptureTool();
                this.Capture_thread_srv.stop();
                this.Capture_thread_srv_exec = false;
            }
            if ((n & 4) != 0 && (n2 & 8) != 0 && !this.Capture_thread_srv_exec) {
                this.Capture_thread_srv = Thread.create(this, "Capture_abrupt");
                this.Capture_thread_srv.start();
            }
            System.sleep(1);
        }
    }

    void Capture_SelectChr() {
        boolean bl = true;
        int n = 0;
        while (bl) {
            switch (n) {
                case 0: {
                    this.Menucreate();
                    this.menu.addQuery("キャプチャモード");
                    this.menu.addItem("全画面");
                    this.menu.addItem("ＢＧのみ（ＺＰＩＣなし）");
                    this.menu.addItem("ＢＧのみ（ＺＰＩＣあり）");
                    this.menu.addItem("キャラ単体（ＺＰＩＣなし）");
                    this.menu.addItem("キャラ単体（ＺＰＩＣあり）");
                    this.menu.addItem("オブジェ単体（ＺＰＩＣなし）");
                    this.menu.addItem("オブジェ単体（ＺＰＩＣあり）");
                    this.menu.addItem("テキスト（ＺＰＩＣなし）");
                    this.menu.addItem("パーティクル（ＺＰＩＣなし）");
                    this.menu.addItem("パーティクル（ＺＰＩＣあり）");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 == 0) {
                        System.println("CaptureMODE ---All Screen.");
                        this.Capture_mode = 0;
                        bl = false;
                    }
                    if (n2 == 1) {
                        System.println("CaptureMODE ---SoftImage BG(pic)");
                        this.Capture_mode = 33024;
                        bl = false;
                    }
                    if (n2 == 2) {
                        System.println("CaptureMODE ---SoftImage BG(pic+zpic)");
                        this.Capture_mode = 256;
                        bl = false;
                    }
                    if (n2 == 3) {
                        System.println("CaptureMODE ---Chr(pic)");
                        this.Capture_mode = 33280;
                        n = 1;
                    }
                    if (n2 == 4) {
                        System.println("CaptureMODE ---Chr(pic+zpic)");
                        this.Capture_mode = 512;
                        n = 1;
                    }
                    if (n2 == 5) {
                        System.println("CaptureMODE ---MObj(pic)");
                        this.Capture_mode = 33536;
                        n = 1;
                    }
                    if (n2 == 6) {
                        System.println("CaptureMODE ---MObj(pic+zpic)");
                        this.Capture_mode = 768;
                        n = 1;
                    }
                    if (n2 == 7) {
                        System.println("CaptureMODE ---TXT(pic)");
                        this.Capture_mode = 36864;
                        bl = false;
                    }
                    if (n2 == 8) {
                        System.println("CaptureMODE ---Particle(pic)");
                        this.Capture_mode = 34816;
                        bl = false;
                    }
                    if (n2 != 9) break;
                    System.println("CaptureMODE ---Particle(pic+zpic)");
                    this.Capture_mode = 2048;
                    bl = false;
                    break;
                }
                case 1: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ０");
                    this.menu.addItem("キャラ１");
                    this.menu.addItem("キャラ２");
                    this.menu.addItem("キャラ３");
                    this.menu.addItem("キャラ４");
                    this.menu.addItem("キャラ５");
                    this.menu.addItem("キャラ６");
                    this.menu.addItem("キャラ７");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1;
                        bl = false;
                        break;
                    }
                    n = 2;
                    break;
                }
                case 2: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ８");
                    this.menu.addItem("キャラ９");
                    this.menu.addItem("キャラ０Ａ");
                    this.menu.addItem("キャラ０Ｂ");
                    this.menu.addItem("キャラ０Ｃ");
                    this.menu.addItem("キャラ０Ｄ");
                    this.menu.addItem("キャラ０Ｅ");
                    this.menu.addItem("キャラ０Ｆ");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 8;
                        bl = false;
                        break;
                    }
                    n = 3;
                    break;
                }
                case 3: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ１０");
                    this.menu.addItem("キャラ１１");
                    this.menu.addItem("キャラ１２");
                    this.menu.addItem("キャラ１３");
                    this.menu.addItem("キャラ１４");
                    this.menu.addItem("キャラ１５");
                    this.menu.addItem("キャラ１６");
                    this.menu.addItem("キャラ１７");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 16;
                        bl = false;
                        break;
                    }
                    n = 4;
                    break;
                }
                case 4: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ１８");
                    this.menu.addItem("キャラ１９");
                    this.menu.addItem("キャラ１Ａ");
                    this.menu.addItem("キャラ１Ｂ");
                    this.menu.addItem("キャラ１Ｃ");
                    this.menu.addItem("キャラ１Ｄ");
                    this.menu.addItem("キャラ１Ｅ");
                    this.menu.addItem("キャラ１Ｆ");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 24;
                        bl = false;
                        break;
                    }
                    n = 5;
                    break;
                }
                case 5: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ２０");
                    this.menu.addItem("キャラ２１");
                    this.menu.addItem("キャラ２２");
                    this.menu.addItem("キャラ２３");
                    this.menu.addItem("キャラ２４");
                    this.menu.addItem("キャラ２５");
                    this.menu.addItem("キャラ２６");
                    this.menu.addItem("キャラ２７");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 32;
                        bl = false;
                        break;
                    }
                    n = 6;
                    break;
                }
                case 6: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ２８");
                    this.menu.addItem("キャラ２９");
                    this.menu.addItem("キャラ２Ａ");
                    this.menu.addItem("キャラ２Ｂ");
                    this.menu.addItem("キャラ２Ｃ");
                    this.menu.addItem("キャラ２Ｄ");
                    this.menu.addItem("キャラ２Ｅ");
                    this.menu.addItem("キャラ２Ｆ");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 40;
                        bl = false;
                        break;
                    }
                    n = 7;
                    break;
                }
                case 7: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ３０");
                    this.menu.addItem("キャラ３１");
                    this.menu.addItem("キャラ３２");
                    this.menu.addItem("キャラ３３");
                    this.menu.addItem("キャラ３４");
                    this.menu.addItem("キャラ３５");
                    this.menu.addItem("キャラ３６");
                    this.menu.addItem("キャラ３７");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 48;
                        bl = false;
                        break;
                    }
                    n = 8;
                    break;
                }
                case 8: {
                    this.Capture_SelectChr_MenuTitle();
                    this.menu.addItem("ページ切り替え");
                    this.menu.addItem("キャラ３８");
                    this.menu.addItem("キャラ３９");
                    this.menu.addItem("キャラ３Ａ");
                    this.menu.addItem("キャラ３Ｂ");
                    this.menu.addItem("キャラ３Ｃ");
                    this.menu.addItem("キャラ３Ｄ");
                    this.menu.addItem("キャラ３Ｅ");
                    this.menu.addItem("キャラ３Ｆ");
                    System.waitFor(this.menu);
                    int n2 = this.menu.getSelected();
                    if (n2 != 0) {
                        this.Capture_mode += n2 - 1 + 56;
                        bl = false;
                        break;
                    }
                    n = 1;
                    break;
                }
            }
        }
        Runtime.setRegister(0, this.Capture_mode);
        System.println("CaptureMode = /[$0]");
    }

    void Capture_SelectChr_MenuTitle() {
        int n = this.Capture_mode & 0xF00;
        this.Menucreate();
        if (n == 512) {
            this.menu.addQuery("キャラのみキャプチャ");
        }
        if (n == 768) {
            this.menu.addQuery("オブジェのみキャプチャ");
        }
    }

    void Capture_SelectFolder() {
        int n = 0;
        boolean bl = true;
        while (bl) {
            block31:
            while (true) {
                switch (n) {
                    case 0: {
                        int n2;
                        this.Menucreate();
                        this.menu.addQuery("フォルダ選択");
                        this.menu.addItem("企画サイドのフォルダを使用する");
                        this.menu.addItem("ｍｏｖｉｅ００（まつやま）");
                        this.menu.addItem("ｍｏｖｉｅ０１（たいぞう）");
                        this.menu.addItem("ｍｏｖｉｅ０２（はましま）");
                        this.menu.addItem("ｍｏｖｉｅ０３（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０４（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０５（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０６（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０７（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０８（？？？？）");
                        this.menu.addItem("ｍｏｖｉｅ０９（？？？？）");
                        System.waitFor(this.menu);
                        this.CaptureFolder = n2 = this.menu.getSelected();
                        switch (n2) {
                            case 0: {
                                n = 1;
                                break block31;
                            }
                            case 1: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie00";
                                bl = false;
                                break block31;
                            }
                            case 2: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie01";
                                bl = false;
                                break block31;
                            }
                            case 3: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie02";
                                bl = false;
                                break block31;
                            }
                            case 4: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie03";
                                bl = false;
                                break block31;
                            }
                            case 5: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie04";
                                bl = false;
                                break block31;
                            }
                            case 6: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie05";
                                bl = false;
                                break block31;
                            }
                            case 7: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie06";
                                bl = false;
                                break block31;
                            }
                            case 8: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie07";
                                bl = false;
                                break block31;
                            }
                            case 9: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie08";
                                bl = false;
                                break block31;
                            }
                            case 10: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie09";
                                bl = false;
                                break block31;
                            }
                            default: {
                                if (bl) continue block31;
                            }
                        }
                    }
                    case 1: {
                        this.Menucreate();
                        this.menu.addQuery("フォルダ選択");
                        this.menu.addItem("エフェクトサイドのフォルダを使用する");
                        this.menu.addItem("ｍｏｖｉｅ１０（おの）");
                        this.menu.addItem("ｍｏｖｉｅ１１（くらもと）");
                        this.menu.addItem("ｍｏｖｉｅ１２（すぎさわ）");
                        this.menu.addItem("ｍｏｖｉｅ１３（ふじ）");
                        this.menu.addItem("ｍｏｖｉｅ１４（やぎ）");
                        this.menu.addItem("ｍｏｖｉｅ１５（こじま）");
                        this.menu.addItem("ｍｏｖｉｅ１６（ひみつ）");
                        this.menu.addItem("ｍｏｖｉｅ１７（ひみつ）");
                        this.menu.addItem("ｍｏｖｉｅ１８（さとせい）");
                        this.menu.addItem("ｍｏｖｉｅ１９（さとせい）");
                        System.waitFor(this.menu);
                        int n2 = this.menu.getSelected();
                        this.CaptureFolder = n2 + 10;
                        switch (n2) {
                            case 0: {
                                n = 0;
                                break block31;
                            }
                            case 1: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie10";
                                bl = false;
                                break block31;
                            }
                            case 2: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie11";
                                bl = false;
                                break block31;
                            }
                            case 3: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie12";
                                bl = false;
                                break block31;
                            }
                            case 4: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie13";
                                bl = false;
                                break block31;
                            }
                            case 5: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie14";
                                bl = false;
                                break block31;
                            }
                            case 6: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie15";
                                bl = false;
                                break block31;
                            }
                            case 7: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie16";
                                bl = false;
                                break block31;
                            }
                            case 8: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie17";
                                bl = false;
                                break block31;
                            }
                            case 9: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie18";
                                bl = false;
                                break block31;
                            }
                            case 10: {
                                this.FolderName = String.valueOf(this.FolderName0) + "movie19";
                                bl = false;
                                break block31;
                            }
                        }
                    }
                }
                break;
            }
        }
        System.println("Capture Folder Setting...");
    }

    void Capture_abrupt() {
        this.Capture_thread_srv_exec = true;
        this.CaptureStart_CaptureTool();
        System.sleep(2);
        this.NextCutWait();
        this.CaptureEnd_CaptureTool();
        System.println("*********Capture End.*****************");
        this.Capture_thread_srv_exec = false;
    }

    void Capture_wait() {
        this.Capture_thread_srv_exec = true;
        System.println("*********Capture Waiting...*****************");
        if (this.Capture_mode != 36864) {
            this.msgflag = false;
        }
        if (this.CaptureFolder >= 10) {
            this.msgflag = true;
        }
        this.NextCutWait();
        this.CaptureStart_CaptureTool();
        System.sleep(2);
        this.NextCutWait();
        this.CaptureEnd_CaptureTool();
        System.println("*********Capture End.*****************");
        this.Capture_thread_srv_exec = false;
    }

    void ColorScreenSet() {
        if (this.ColorScreen_flag == 1) {
            Runtime.setDefocus(0, 2, this.test_param0);
        }
        this.ColorScreenSet_exec = true;
        System.println("ColorScreen  SetMode");
        while (this.ColorScreenSet_exec) {
            this.btn2 = this.pad1.getButton();
            this.edge2 = this.pad1.getEdge();
            if ((this.edge2 & 0x100) != 0) {
                this.ColorScreen_printout();
            }
            if ((this.edge2 & 0x800) != 0) {
                this.ColorScreenSet_exec = false;
            }
            if ((this.edge2 & this._PADL3) != 0) {
                ++this.Color_layer;
                this.Color_layer %= 5;
                Runtime.setRegister(0, this.Color_layer);
                System.println("Layer = /[$0]");
            }
            if ((this.btn2 & 0xF0) == 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    this.Color_Z -= 1000;
                    this.Color_Z_2 -= 1000;
                }
                if ((this.btn2 & 0x4000) != 0) {
                    this.Color_Z += 1000;
                    this.Color_Z_2 += 1000;
                }
                if ((this.btn2 & 0x2000) != 0) {
                    this.Color_Zwide -= 500;
                }
                if ((this.btn2 & 0x8000) != 0) {
                    this.Color_Zwide += 500;
                }
            }
            if ((this.btn2 & 0x80) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_R < 254) {
                        this.Color_R += 2;
                    } else {
                        System.println("Red Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_R != 0) {
                        this.Color_R -= 2;
                    } else {
                        System.println("Red Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x10) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_G < 254) {
                        this.Color_G += 2;
                    } else {
                        System.println("Green Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_G != 0) {
                        this.Color_G -= 2;
                    } else {
                        System.println("Green Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x20) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_B < 254) {
                        this.Color_B += 2;
                    } else {
                        System.println("Blue Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_B != 0) {
                        this.Color_B -= 2;
                    } else {
                        System.println("Blue Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x40) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_A < 254) {
                        this.Color_A += 2;
                    } else {
                        System.println("Alpha Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_A != 0) {
                        this.Color_A -= 2;
                    } else {
                        System.println("Alpha Min!!");
                    }
                }
            }
            if (this.ColorScreen_flag == 0) {
                this.ColorScreenParam[2] = this.Color_Z;
                this.ColorScreenParam[3] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
                this.Color_work = 15;
                this.Color_work2 = 0;
                Runtime.setDefocus(12, 0, null);
                Runtime.setDefocus(13, 0, null);
                Runtime.setDefocus(14, 0, null);
                Runtime.setDefocus(15, 0, null);
                while (this.Color_work2 != this.Color_layer) {
                    Runtime.setDefocus(this.Color_work, 1, this.ColorScreenParam);
                    this.ColorScreenParam[2] = this.ColorScreenParam[2] - this.Color_Zwide;
                    --this.Color_work;
                    ++this.Color_work2;
                }
            } else if (this.ColorScreen_flag == 1) {
                this.test_param1[1] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
                this.test_param0[1] = this.Color_Z_2;
                Runtime.setDefocus(15, 0, null);
                Runtime.setDefocus(0, 2, this.test_param0);
                Runtime.setDefocus(12, 3, this.test_param1);
                Runtime.setDefocus(13, 3, this.test_param1);
                Runtime.setDefocus(14, 3, this.test_param1);
                Runtime.setDefocus(15, 3, this.test_param1);
            }
            System.sleep(1);
        }
        System.println("ScreenColor SetMode-------end");
    }

    void ColorScreen_printout() {
        if (this.ColorScreen_flag == 0) {
            this.ColorScreenParam[2] = this.Color_Z;
            this.ColorScreenParam[3] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
            Runtime.setRegister(0, this.ColorScreenParam[3]);
            Runtime.setRegister(1, this.ColorScreenParam[2]);
            if (this.Color_layer != 0) {
                System.println("*************ColorScreen Printout*************");
                System.println("int ColorScreenParam[] = {");
                System.println("0,");
                System.println("1,");
                System.println("/[$1],");
                System.println("/[$0],");
                System.println("0,");
                System.println("0,");
                System.println("0,");
                System.println("0,");
                System.println("};");
                System.println("Runtime.setDefocus(15, 1, ColorScreenParam);");
                this.Color_work = 15;
                this.Color_work2 = 1;
                while (this.Color_work2 != this.Color_layer) {
                    this.ColorScreenParam[2] = this.ColorScreenParam[2] - this.Color_Zwide;
                    Runtime.setRegister(1, this.ColorScreenParam[2]);
                    Runtime.setRegister(2, this.Color_work);
                    System.println("ColorScreenParam[2] = /[$1]");
                    System.println("Runtime.setDefocus(/[$2], 1, ColorScreenParam);");
                    --this.Color_work;
                    ++this.Color_work2;
                }
            } else {
                System.println("Layer not exist.");
            }
        } else if (this.ColorScreen_flag == 1) {
            Runtime.setRegister(0, this.Color_Z_2);
            Runtime.setRegister(1, this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R);
            System.println("*************ColorScreen Printout_temae*************");
            System.println("int[] test_param0 = {");
            System.println("0, ");
            System.println("/[$0],");
            System.println("};");
            System.println("Runtime.setDefocus(15, 2, test_param0);");
            System.println("int[] test_param1 = {");
            System.println("1,");
            System.println("/[$1],");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("};");
            System.println("Runtime.setDefocus(15, 3, test_param1);");
        }
    }

    void CutWin(String string) {
        Window window = Window.create();
        window.setSize(2, 16);
        window.setLocation(10, 10);
        window.print(string);
        window.wait(30);
        window.close();
    }

    void Debugprint5() {
        System.println("*********ManualCamera Replay*********");
        System.println("pos[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 4) {
            Runtime.setRegister(0, this.ManualCampos[this.work0]);
            Runtime.setRegister(1, this.ManualCampos[this.work0 + 1]);
            Runtime.setRegister(2, this.ManualCampos[this.work0 + 2]);
            Runtime.setRegister(3, this.ManualCampos[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("rot[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 4) {
            Runtime.setRegister(0, this.ManualCamrot[this.work0]);
            Runtime.setRegister(1, this.ManualCamrot[this.work0 + 1]);
            Runtime.setRegister(2, this.ManualCamrot[this.work0 + 2]);
            Runtime.setRegister(3, this.ManualCamrot[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("fov[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 2) {
            Runtime.setRegister(0, this.ManualCamfov[this.work0]);
            Runtime.setRegister(1, this.ManualCamfov[this.work0 + 1]);
            System.println("/[#0]f,/[#1]f,");
            this.work0 += 2;
        }
        System.println("};");
    }

    void FACE(int n, Chr chr, int n2) {
        chr.mtn(n2, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 5, 9, 1.0f, false);
        chr.start(4, null);
    }

    void FACE(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void FocusSet() {
        this.FocusSet_exec = true;
        System.println("Focus SetMode");
        int n = 0;
        while (n == 4) {
            Runtime.setDefocusQuick(n, 0, 0, 0);
            ++n;
        }
        while (this.FocusSet_exec) {
            this.Pad_get();
            if ((this.btn2 & 0x100) != 0) {
                if ((this.btn2 & 0x20) != 0) {
                    ++this.alpha_percent;
                    Runtime.setRegister(0, (float) this.alpha_percent);
                    System.println("alpha_percent-/[#0]");
                }
                if ((this.btn2 & 0x80) != 0) {
                    --this.alpha_percent;
                    if (this.alpha_percent < 0) {
                        this.alpha_percent = 0;
                    }
                    Runtime.setRegister(0, (float) this.alpha_percent);
                    System.println("alpha_percent-/[#0]");
                }
                if ((this.btn2 & 0x40) != 0) {
                    if (this.alpha_flag == 0) {
                        this.alpha_flag = 1;
                    } else if (this.alpha_flag == 1) {
                        this.alpha_flag = 0;
                    }
                    System.sleep(3);
                    Runtime.setRegister(0, (float) this.alpha_flag);
                    System.println("alpha_set-/[#0]");
                }
            }
            if ((this.btn2 & 0x100) != 0 && (this.btn2 & 0x800) != 0) {
                this.Focus_printout();
                this.focus_flag = 1;
            }
            if (this.focus_flag == 0 && (this.btn2 & 0x800) != 0) {
                this.FocusSet_exec = false;
            }
            if ((this.edge2 & this._PADR3) != 0) {
                ++this.Focus_layer;
                if (this.Focus_layer > 4) {
                    this.Focus_layer = 0;
                }
                Runtime.setRegister(0, this.Focus_layer);
                System.println("Layer = /[$0]");
            }
            if ((this.edge2 & this._PADL3) != 0) {
                if (this.Focus_flag) {
                    this.Focus_flag = false;
                    System.println("Layer = Back");
                } else {
                    this.Focus_flag = true;
                    System.println("Layer = Before");
                }
            }
            if ((this.btn2 & 0x1000) != 0) {
                this.Focus_z -= 1000;
            }
            if ((this.btn2 & 0x4000) != 0) {
                this.Focus_z += 1000;
            }
            if ((this.btn2 & 0x8000) != 0) {
                this.Focus_zwide -= 500;
                if (this.Focus_zwide <= 0) {
                    this.Focus_zwide = 0;
                }
            }
            if ((this.btn2 & 0x2000) != 0) {
                this.Focus_zwide += 500;
            }
            if ((this.edge2 & 8) != 0) {
                --this.Focus_back;
                --this.Focus_before;
                Runtime.setRegister(0, this.Focus_back);
                System.println("Focus delta = /[$0](back)");
            }
            if ((this.edge2 & 2) != 0) {
                ++this.Focus_back;
                ++this.Focus_before;
                Runtime.setRegister(0, this.Focus_back);
                System.println("Focus delta = /[$0](back)");
            }
            this.Focus_callRuntime();
            System.sleep(1);
            this.focus_flag = 0;
        }
        System.println("Focus SetMode-------end");
    }

    void Focus_callRuntime() {
        this.Focus_work0 = 0;
        while (this.Focus_work0 != 4) {
            Runtime.setDefocusQuick(this.Focus_work0, 0, 0, 0);
            ++this.Focus_work0;
        }
        this.Focus_work0 = 0;
        this.Focus_work1 = 0;
        if (this.Focus_flag) {
            this.Focus_before_layer = this.Focus_layer;
            this.Focus_back_layer = 0;
        } else {
            this.Focus_before_layer = 0;
            this.Focus_back_layer = this.Focus_layer;
        }
        if (this.Focus_before_layer != 0) {
            this.Focus_work1 = 0;
            while (this.Focus_work1 != this.Focus_before_layer) {
                ++this.Focus_work1;
                this.set_norm2 = this.alpha_flag == 1 ? 4 : 2;
                this.dfq4_param = this.alpha_flag == 1 ? this.alpha_percent : this.Focus_before;
                if (this.Focus_z + this.Focus_zwide * this.Focus_work1 > 0) {
                    Runtime.setDefocusQuick(this.Focus_work0, 0, 0, 0);
                    Runtime.setDefocusQuick(this.Focus_work0, this.set_norm2, this.Focus_z + this.Focus_zwide * this.Focus_work1, this.dfq4_param);
                }
                ++this.Focus_work0;
            }
        }
        if (this.Focus_back_layer != 0) {
            this.Focus_work1 = 0;
            while (this.Focus_work1 != this.Focus_back_layer) {
                ++this.Focus_work1;
                this.set_norm1 = this.alpha_flag == 1 ? 4 : 1;
                this.dfq4_param = this.alpha_flag == 1 ? this.alpha_percent : this.Focus_back;
                if (this.Focus_z - this.Focus_zwide * this.Focus_work1 > 0) {
                    Runtime.setDefocusQuick(this.Focus_work0, 0, 0, 0);
                    Runtime.setDefocusQuick(this.Focus_work0, this.set_norm1, this.Focus_z - this.Focus_zwide * this.Focus_work1, this.dfq4_param);
                }
                ++this.Focus_work0;
            }
        }
    }

    void Focus_printout() {
        System.println("*************Focus Printout*************");
        this.Focus_work0 = 0;
        this.Focus_work1 = 0;
        int n = 0;
        if (this.Focus_flag) {
            this.Focus_before_layer = this.Focus_layer;
            this.Focus_back_layer = 0;
        } else {
            this.Focus_before_layer = 0;
            this.Focus_back_layer = this.Focus_layer;
        }
        if (this.Focus_before_layer != 0) {
            this.Focus_work1 = 0;
            while (this.Focus_work1 != this.Focus_before_layer) {
                ++this.Focus_work1;
                Runtime.setRegister(0, this.Focus_work0);
                Runtime.setRegister(1, this.Focus_z + this.Focus_zwide * this.Focus_work1);
                Runtime.setRegister(2, this.Focus_before);
                if (this.Focus_z + this.Focus_zwide * this.Focus_work1 > 0) {
                    System.println("Runtime.setDefocusQuick(/[$0], 2, /[$1], /[$2]);");
                    ++n;
                }
                ++this.Focus_work0;
            }
        }
        if (this.Focus_back_layer != 0) {
            this.Focus_work1 = 0;
            while (this.Focus_work1 != this.Focus_back_layer) {
                ++this.Focus_work1;
                Runtime.setRegister(0, this.Focus_work0);
                Runtime.setRegister(1, this.Focus_z - this.Focus_zwide * this.Focus_work1);
                Runtime.setRegister(2, this.Focus_back);
                if (this.Focus_z - this.Focus_zwide * this.Focus_work1 > 0) {
                    System.println("Runtime.setDefocusQuick(/[$0], 1, /[$1], /[$2]);");
                    ++n;
                }
                ++this.Focus_work0;
            }
        }
        if (n == 0) {
            System.println("Layer Not insight.");
        }
    }

    void MSG(int n, String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        this.msg.print(string);
        System.println(string);
    }

    void MSG(int n, String string, String string2) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        if (this.msgflag) {
            System.println("MSGprint.");
            this.msg.print(string2);
            System.println(string2);
        }
        System.sleep(n);
        this.msg.clear();
    }

    void MSG(int n, Chr chr, int n2, int n3, String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        chr.mtn(n2, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
        if (this.msgflag) {
            System.println("MSGprint(with Face).");
            System.println(string);
            this.msg.print(string);
        }
        System.sleep(n3);
        chr.mtn(n2 + 1, 0, 120, 5, 9, 1.0f, false);
        chr.start(4, null);
        System.println("Face End.");
        if (n > n3) {
            System.sleep(n - n3);
        }
        this.msg.clear();
    }

    void MSG(int n, Chr chr, int n2, String string) {
        this.MSG(n, chr, n2, n - 10, string);
    }

    void MSG(String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        this.msg.print(string);
        System.println(string);
    }

    void MSGW(String string) {
        this.msg.clear();
        this.msg.print(string);
    }

    void MSGr(int n, Chr chr, int n2, int n3, String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        if (this.msgflag) {
            System.println("MSGprint(with Face).");
            System.println(string);
            this.msg.print(string);
        }
        if (n3 > 120) {
            chr.mtn(n2, 0, 120, 5, 8, 1.0f, false);
            chr.start(4, null);
        } else {
            chr.mtn(n2, 0, n3, 0, 0, -1.0f, false);
            chr.start(4, null);
        }
        System.sleep(n3);
        if (n3 > 120) {
            chr.mtn(n2 + 1, 0, 120, 5, 9, 1.0f, false);
            chr.start(4, null);
        }
        System.println("Face End.");
        if (n > n3) {
            System.sleep(n - n3);
        }
        this.msg.clear();
    }

    void MSGr(int n, Chr chr, int n2, String string) {
        this.MSGr(n, chr, n2, n - 10, string);
    }

    int ManualCam_play() {
        if (this.Manualcamrecord_time == 0) {
            System.println("ManualCamera not Recorded.");
            return -1;
        }
        this.CamHistorywork = 1;
        this.CamHistorywork2 = 0;
        while (this.CamHistorywork2 <= this.Manualcamrecord_ptr - 1) {
            this.ManualCampos[this.CamHistorywork2 * 4] = this.CamHistorywork;
            this.ManualCampos[this.CamHistorywork2 * 4 + 1] = this.ManualCamRecord[this.CamHistorywork2 * 7];
            this.ManualCampos[this.CamHistorywork2 * 4 + 2] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 1];
            this.ManualCampos[this.CamHistorywork2 * 4 + 3] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 2];
            this.ManualCamrot[this.CamHistorywork2 * 4] = this.CamHistorywork;
            this.ManualCamrot[this.CamHistorywork2 * 4 + 1] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 3];
            this.ManualCamrot[this.CamHistorywork2 * 4 + 2] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 4];
            this.ManualCamrot[this.CamHistorywork2 * 4 + 3] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 5];
            this.ManualCamfov[this.CamHistorywork2 * 2] = this.CamHistorywork;
            this.ManualCamfov[this.CamHistorywork2 * 2 + 1] = this.ManualCamRecord[this.CamHistorywork2 * 7 + 6];
            this.CamHistorywork += this.Manualcamrecord_dt;
            ++this.CamHistorywork2;
        }
        while (this.CamHistorywork2 < this.Manualcamrecord_max + 1) {
            this.ManualCampos[this.CamHistorywork2 * 4] = this.Manualcamrecord_time;
            this.ManualCampos[this.CamHistorywork2 * 4 + 1] = this.Manualcamrec_posX;
            this.ManualCampos[this.CamHistorywork2 * 4 + 2] = this.Manualcamrec_posY;
            this.ManualCampos[this.CamHistorywork2 * 4 + 3] = this.Manualcamrec_posZ;
            this.ManualCamrot[this.CamHistorywork2 * 4] = this.Manualcamrecord_time;
            this.ManualCamrot[this.CamHistorywork2 * 4 + 1] = this.Manualcamrec_rotX;
            this.ManualCamrot[this.CamHistorywork2 * 4 + 2] = this.Manualcamrec_rotY;
            this.ManualCamrot[this.CamHistorywork2 * 4 + 3] = this.Manualcamrec_rotZ;
            this.ManualCamfov[this.CamHistorywork2 * 2] = this.Manualcamrecord_time;
            this.ManualCamfov[this.CamHistorywork2 * 2 + 1] = this.Manualcamrec_fov;
            ++this.CamHistorywork2;
        }
        return 0;
    }

    void ManualCam_play_sub() {
        this.cam1.transSPL(this.ManualCampos, 1);
        this.cam1.rotateSPL(this.ManualCamrot, 1);
        this.cam1.fovSPL(this.ManualCamfov, 1);
        this.cam1.change();
        this.Systemsleep2(this.Manualcamrecord_time);
        this.cam0.change();
    }

    int ManualCam_record() {
        System.println("ManualCam_record.");
        System.println("ManualCamera MOVE to start.");
        this.Manualcamrecord_exec = true;
        System.sleep(5);
        this.CamHistoryposX = this.cam0.getTranslateX();
        this.CamHistoryposY = this.cam0.getTranslateY();
        this.CamHistoryposZ = this.cam0.getTranslateZ();
        this.CamHistoryrotX = this.cam0.getRotateX();
        this.CamHistoryrotY = this.cam0.getRotateY();
        this.CamHistoryrotZ = this.cam0.getRotateZ();
        this.CamHistoryFov = this.cam0.getFov();
        while (this.Manualcamrecord_exec) {
            if (this.CamHistoryposX != this.cam0.getTranslateX()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryposY != this.cam0.getTranslateY()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryposZ != this.cam0.getTranslateZ()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotX != this.cam0.getRotateX()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotY != this.cam0.getRotateY()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotZ != this.cam0.getRotateZ()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryFov != this.cam0.getFov()) {
                this.Manualcamrecord_exec = false;
            }
            this.edge1 = this.pad1.getEdge();
            if ((this.edge1 & 0x400) != 0) {
                System.println("Cancel!!");
                return -1;
            }
            System.sleep(1);
        }
        System.println("ManualCameraRecord START!!!");
        this.Manualcamrecord_exec = true;
        this.Manualcamrecord_time = 1;
        this.Manualcamrecord_ptr = 0;
        while (this.Manualcamrecord_exec) {
            if (this.Manualcamrecord_time % this.Manualcamrecord_dt == 0) {
                Runtime.setRegister(0, this.Manualcamrecord_ptr);
                System.println("Record /[$0]");
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7] = this.cam0.getTranslateX();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 1] = this.cam0.getTranslateY();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 2] = this.cam0.getTranslateZ();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 3] = this.cam0.getRotateX();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 4] = this.cam0.getRotateY();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 5] = this.cam0.getRotateZ();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 6] = this.cam0.getFov();
                ++this.Manualcamrecord_ptr;
                if (this.Manualcamrecord_ptr >= this.Manualcamrecord_max) {
                    System.println("End(TimeOver).");
                    this.Manualcamrecord_exec = false;
                }
            }
            this.edge1 = this.pad1.getEdge();
            if ((this.edge1 & this._PADL3) != 0) {
                System.println("End(Button).");
                Runtime.setRegister(0, this.Manualcamrecord_time + 1);
                System.println("Time = /[$0]");
                this.Manualcamrecord_exec = false;
            }
            if ((this.edge1 & this._PADR3) != 0) {
                System.println("End(Button).");
                Runtime.setRegister(0, this.Manualcamrecord_time + 1);
                System.println("Time = /[$0]");
                this.Manualcamrecord_exec = false;
            }
            ++this.Manualcamrecord_time;
            System.sleep(1);
        }
        this.Manualcamrec_posX = this.cam0.getTranslateX();
        this.Manualcamrec_posY = this.cam0.getTranslateY();
        this.Manualcamrec_posZ = this.cam0.getTranslateZ();
        this.Manualcamrec_rotX = this.cam0.getRotateX();
        this.Manualcamrec_rotY = this.cam0.getRotateY();
        this.Manualcamrec_rotZ = this.cam0.getRotateZ();
        this.Manualcamrec_fov = this.cam0.getFov();
        return this.Manualcamrecord_time;
    }

    void Menucreate() {
        System.sleep(15);
        this.menu = Menu.create();
    }

    void NextCutWait() {
        while (this.CutNo < this.Timechk_NextCut) {
            System.sleep(1);
        }
    }

    void Objchk(Chr chr) {
        System.println("Objchk......(Chr)");
        this.Objchk_exec = true;
        this.ObjchkScale = chr.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(chr);
            this.Unit_put(chr);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk(Chr chr, int n) {
        System.println("Objchk(+Mtn)......");
        this.Objchk_exec = true;
        this.ObjchkScale = chr.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv_mtn(chr, n);
            this.Unit_mov(chr);
            this.Unit_put(chr);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk(Effect effect) {
        System.println("Objchk......(Effect)");
        this.Objchk_exec = true;
        this.ObjchkScale = effect.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(effect);
            this.Unit_put(effect);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk(Unit unit) {
        System.println("Objchk......(Unit)");
        this.Objchk_exec = true;
        this.ObjchkScale = unit.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(unit);
            this.Unit_put(unit);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk(Unit unit, Chr chr, int n) {
        System.println("Objchk......");
        this.Objchk_exec = true;
        this.ObjchkScale = unit.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        unit.setParent(chr, n);
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(unit);
            this.Unit_put(unit);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk_Keychk_srv() {
        if ((this.edge1 & this._PADL3) != 0) {
            this.Objchk_exec = false;
        }
        if ((this.edge1 & this._PADR3) != 0) {
            this.Objchk_exec = false;
        }
        if ((this.btn1 & 0x10) != 0) {
            if ((this.btn1 & 0x40) != 0) {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dsx = 0.01f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dsx = -0.01f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dsz = -0.01f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dsz = 0.01f;
                }
                if ((this.btn1 & 0x20) != 0) {
                    this.Objchk_dsy = -0.01f;
                }
                if ((this.btn1 & 0x80) != 0) {
                    this.Objchk_dsy = 0.01f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dsx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dsx = -0.1f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dsz = -0.1f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dsz = 0.1f;
                }
                if ((this.btn1 & 0x20) != 0) {
                    this.Objchk_dsy = -0.1f;
                }
                if ((this.btn1 & 0x80) != 0) {
                    this.Objchk_dsy = 0.1f;
                }
            }
        } else if ((this.btn1 & 0x40) != 0) {
            if ((this.btn1 & this.Objchk_key1) != 0) {
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_drz = 1.0f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_drz = -1.0f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_drx = 1.0f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_drx = -1.0f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.01f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.01f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dz = -0.01f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dz = 0.01f;
                }
            }
            if ((this.btn1 & 4) != 0) {
                this.Objchk_dry = 1.0f;
            }
            if ((this.btn1 & 1) != 0) {
                this.Objchk_dry = -1.0f;
            }
            if ((this.btn1 & 8) != 0) {
                this.Objchk_dy = 0.01f;
            }
            if ((this.btn1 & 2) != 0) {
                this.Objchk_dy = -0.01f;
            }
        } else {
            if ((this.btn1 & this.Objchk_key1) != 0) {
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_drz = 10.0f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_drz = -10.0f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_drx = 10.0f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_drx = -10.0f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.1f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.1f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dz = -0.1f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dz = 0.1f;
                }
            }
            if ((this.btn1 & 4) != 0) {
                this.Objchk_dry = 10.0f;
            }
            if ((this.btn1 & 1) != 0) {
                this.Objchk_dry = -10.0f;
            }
            if ((this.btn1 & 8) != 0) {
                this.Objchk_dy = 0.1f;
            }
            if ((this.btn1 & 2) != 0) {
                this.Objchk_dy = -0.1f;
            }
        }
    }

    void Objchk_Keychk_srv_mtn(Chr chr, int n) {
        if ((this.edge1 & this._PADL3) != 0) {
            this.Objchk_exec = false;
        }
        if ((this.edge1 & this._PADR3) != 0) {
            this.Objchk_exec = false;
        }
        if ((this.btn1 & 0x10) != 0) {
            if ((this.edge1 & 8) != 0) {
                ++this.mtnframe;
                chr.mtn(n, this.mtnframe, this.mtnframe, 0, 0, 1.0f, true);
                Runtime.setRegister(0, this.mtnframe);
                System.println("Mtn Frame = /[$0]");
            }
            if ((this.edge1 & 4) != 0 && this.mtnframe != 0) {
                --this.mtnframe;
                chr.mtn(n, this.mtnframe, this.mtnframe, 0, 0, 1.0f, true);
                Runtime.setRegister(0, this.mtnframe);
                System.println("Mtn Frame = /[$0]");
            }
            if ((this.edge1 & 2) != 0) {
                this.mtnframe += 10;
                chr.mtn(n, this.mtnframe, this.mtnframe, 0, 0, 1.0f, true);
                Runtime.setRegister(0, this.mtnframe);
                System.println("Mtn Frame = /[$0]");
            }
            if ((this.edge1 & 1) != 0 && this.mtnframe >= 10) {
                this.mtnframe -= 10;
                chr.mtn(n, this.mtnframe, this.mtnframe, 0, 0, 1.0f, true);
                Runtime.setRegister(0, this.mtnframe);
                System.println("Mtn Frame = /[$0]");
            }
            if ((this.btn1 & 0x40) != 0) {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dsx = 0.01f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dsx = -0.01f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dsz = -0.01f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dsz = 0.01f;
                }
                if ((this.btn1 & 0x20) != 0) {
                    this.Objchk_dsy = -0.01f;
                }
                if ((this.btn1 & 0x80) != 0) {
                    this.Objchk_dsy = 0.01f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dsx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dsx = -0.1f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dsz = -0.1f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dsz = 0.1f;
                }
                if ((this.btn1 & 0x20) != 0) {
                    this.Objchk_dsy = -0.1f;
                }
                if ((this.btn1 & 0x80) != 0) {
                    this.Objchk_dsy = 0.1f;
                }
            }
        } else if ((this.btn1 & 0x40) != 0) {
            if ((this.btn1 & this.Objchk_key1) != 0) {
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_drz = 1.0f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_drz = -1.0f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_drx = 1.0f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_drx = -1.0f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.01f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.01f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dz = -0.01f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dz = 0.01f;
                }
            }
            if ((this.btn1 & 4) != 0) {
                this.Objchk_dry = 1.0f;
            }
            if ((this.btn1 & 1) != 0) {
                this.Objchk_dry = -1.0f;
            }
            if ((this.btn1 & 8) != 0) {
                this.Objchk_dy = 0.01f;
            }
            if ((this.btn1 & 2) != 0) {
                this.Objchk_dy = -0.01f;
            }
        } else {
            if ((this.btn1 & this.Objchk_key1) != 0) {
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_drz = 10.0f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_drz = -10.0f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_drx = 10.0f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_drx = -10.0f;
                }
            } else {
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.1f;
                }
                if ((this.btn1 & 0x2000) != 0) {
                    this.Objchk_dx = 0.1f;
                }
                if ((this.btn1 & 0x8000) != 0) {
                    this.Objchk_dx = -0.1f;
                }
                if ((this.btn1 & 0x1000) != 0) {
                    this.Objchk_dz = -0.1f;
                }
                if ((this.btn1 & 0x4000) != 0) {
                    this.Objchk_dz = 0.1f;
                }
            }
            if ((this.btn1 & 4) != 0) {
                this.Objchk_dry = 10.0f;
            }
            if ((this.btn1 & 1) != 0) {
                this.Objchk_dry = -10.0f;
            }
            if ((this.btn1 & 8) != 0) {
                this.Objchk_dy = 0.1f;
            }
            if ((this.btn1 & 2) != 0) {
                this.Objchk_dy = -0.1f;
            }
        }
    }

    void Objchk_mtnplay(Chr chr, int n, int n2) {
        chr.mtn(n, n2, n2, 0, 0, 1.0f, true);
    }

    void Pad_get() {
        this.btn1 = this.pad0.getButton() | this.pad1.getButton();
        this.edge1 = this.pad0.getEdge() | this.pad1.getEdge();
        this.rep1 = this.pad0.getRepeat() | this.pad1.getRepeat();
        this.btn1 = this.pad0.getButton();
        this.edge1 = this.pad0.getEdge();
        this.rep1 = this.pad0.getRepeat();
        this.btn2 = this.pad1.getButton();
        this.edge2 = this.pad1.getEdge();
        this.rep2 = this.pad1.getRepeat();
    }

    void SubWin(int n, String string) {
        this.msg.print(string);
        this.msg.wait(n);
        this.msg.clear();
    }

    void Systemsleep2(int n) {
        int n2 = 0;
        while (n2 <= n) {
            this.cam0.setTranslate(this.cam1.getTranslateX(), this.cam1.getTranslateY(), this.cam1.getTranslateZ());
            this.cam0.setRotate(this.cam1.getRotateX(), this.cam1.getRotateY(), this.cam1.getRotateZ());
            this.cam0.setFov(this.cam1.getFov());
            System.sleep(1);
            this.edge1 = this.pad1.getEdge();
            if ((this.edge1 & 0x400) != 0) {
                n2 = n;
                System.println("Cancel!!");
            }
            ++n2;
        }
        System.sleep(1);
    }

    void Timechk() {
        System.println("*********Timechk Standby*****************");
        this.timer_thread = Thread.create(this, "Timechk_main");
        this.timer_thread.start();
    }

    void Timechk_CutChange() {
        ++this.CutNo;
    }

    void Timechk_SceneEnd() {
        ++this.CutNo;
        this.CaptureEnd_CaptureTool();
        Runtime.setRegister(0, this.Timechk_srv_totaltime);
        System.println("----------シーンは終了しました。(Total = /[$0])");
    }

    void Timechk_main() {
        this.CutNo = 0;
        this.Timechk_NextCut = this.CutNo + 1;
        this.timer_thread_srv = Thread.create(this, "Timerchk_srv_main");
        this.Timechk_NextCut = this.CutNo + 1;
        this.Timechk_srv_time = 0;
        this.Timechk_srv_exec = true;
        this.Timerchk_srv();
        while (true) {
            this.NextCutWait();
            int n = this.Timerchk_srv_end();
            Runtime.setRegister(0, this.Timechk_srv_totaltime);
            Runtime.setRegister(1, this.CutNo);
            Runtime.setRegister(2, n);
            System.println("-----Cut = /[$1] CutTime = /[$2](Total = /[$0])");
            this.Timechk_srv_time = 0;
            this.Timechk_srv_exec = true;
            this.Timerchk_srv();
            System.sleep(1);
            this.Timechk_NextCut = this.CutNo + 1;
        }
    }

    void Timerchk_srv() {
        this.timer_thread_srv.start();
    }

    int Timerchk_srv_end() {
        this.Timechk_srv_exec = false;
        this.timer_thread_srv.stop();
        return this.Timechk_srv_time;
    }

    void Timerchk_srv_main() {
        while (this.Timechk_srv_exec) {
            ++this.Timechk_srv_time;
            ++this.Timechk_srv_totaltime;
            System.sleep(1);
        }
    }

    void Unit_mov(Chr chr) {
        chr.getTranslate();
        chr.getRotate();
        chr.px += this.Objchk_dx;
        chr.py += this.Objchk_dy;
        chr.pz += this.Objchk_dz;
        chr.rx += this.Objchk_drx;
        chr.ry += this.Objchk_dry;
        chr.rz += this.Objchk_drz;
        this.Objchk_scalex += this.Objchk_dsx;
        this.Objchk_scaley += this.Objchk_dsy;
        this.Objchk_scalez += this.Objchk_dsz;
        this.Objchk_dx = 0.0f;
        this.Objchk_dy = 0.0f;
        this.Objchk_dz = 0.0f;
        this.Objchk_drx = 0.0f;
        this.Objchk_dry = 0.0f;
        this.Objchk_drz = 0.0f;
        this.Objchk_dsx = 0.0f;
        this.Objchk_dsy = 0.0f;
        this.Objchk_dsz = 0.0f;
        if ((this.edge1 & this.Objchk_key1) != 0) {
            this.Unit_print(chr);
        }
    }

    void Unit_mov(Effect effect) {
        effect.getTranslate();
        effect.getRotate();
        effect.px += this.Objchk_dx;
        effect.py += this.Objchk_dy;
        effect.pz += this.Objchk_dz;
        effect.rx += this.Objchk_drx;
        effect.ry += this.Objchk_dry;
        effect.rz += this.Objchk_drz;
        this.Objchk_scalex += this.Objchk_dsx;
        this.Objchk_scaley += this.Objchk_dsy;
        this.Objchk_scalez += this.Objchk_dsz;
        this.Objchk_dx = 0.0f;
        this.Objchk_dy = 0.0f;
        this.Objchk_dz = 0.0f;
        this.Objchk_drx = 0.0f;
        this.Objchk_dry = 0.0f;
        this.Objchk_drz = 0.0f;
        this.Objchk_dsx = 0.0f;
        this.Objchk_dsy = 0.0f;
        this.Objchk_dsz = 0.0f;
        if ((this.edge1 & this.Objchk_key1) != 0) {
            this.Unit_print(effect);
        }
    }

    void Unit_mov(Unit unit) {
        unit.getTranslate();
        unit.getRotate();
        unit.px += this.Objchk_dx;
        unit.py += this.Objchk_dy;
        unit.pz += this.Objchk_dz;
        unit.rx += this.Objchk_drx;
        unit.ry += this.Objchk_dry;
        unit.rz += this.Objchk_drz;
        this.Objchk_scalex += this.Objchk_dsx;
        this.Objchk_scaley += this.Objchk_dsy;
        this.Objchk_scalez += this.Objchk_dsz;
        this.Objchk_dx = 0.0f;
        this.Objchk_dy = 0.0f;
        this.Objchk_dz = 0.0f;
        this.Objchk_drx = 0.0f;
        this.Objchk_dry = 0.0f;
        this.Objchk_drz = 0.0f;
        this.Objchk_dsx = 0.0f;
        this.Objchk_dsy = 0.0f;
        this.Objchk_dsz = 0.0f;
        if ((this.edge1 & this.Objchk_key1) != 0) {
            this.Unit_print(unit);
        }
    }

    void Unit_print(Chr chr) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(chr.px, chr.py, chr.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(chr.rx, chr.ry, chr.rz);
        System.println("setRotate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
        System.println("setScale(/[#0]f,/[#1]f,/[#2]f);");
    }

    void Unit_print(Effect effect) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(effect.px, effect.py, effect.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(effect.rx, effect.ry, effect.rz);
        System.println("setRotate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
        System.println("setScale(/[#0]f,/[#1]f,/[#2]f);");
    }

    void Unit_print(Unit unit) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(unit.px, unit.py, unit.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(unit.rx, unit.ry, unit.rz);
        System.println("setRotate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
        System.println("setScale(/[#0]f,/[#1]f,/[#2]f);");
    }

    void Unit_put(Chr chr) {
        chr.setTranslate();
        chr.setRotate();
        this.Objchk_scale = this.Objchk_scalex;
        chr.setScale(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
    }

    void Unit_put(Effect effect) {
        effect.setTranslate();
        effect.setRotate();
        this.Objchk_scale = this.Objchk_scalex;
        effect.setScale(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
    }

    void Unit_put(Unit unit) {
        unit.setTranslate();
        unit.setRotate();
        this.Objchk_scale = this.Objchk_scalex;
        unit.setScale(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
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

    void _MSG(int n, String string, String string2) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        if (this.msgflag) {
            this.msg.print(string2);
        }
        this.msg_clearwait = n;
        this.msg_clear.start();
    }

    void __set_position() {
        float[] fArray = new float[72];
        float[] fArray2 = new float[72];
        float[] fArray3 = new float[36];
        this.gold = this.ziggy;
        int n = 0;
        int n2 = 0;
        int n3 = 0;
        int n4 = 0;
        float f = 1.6f;
        float f2 = 1.4f;
        float f3 = 1.0f;
        int n5 = 101;
        float f4 = 1.0f;
        float f5 = 1.0f;
        float f6 = 1.0f;
        int n6 = 0;
        int n7 = 0;
        int n8 = -1;
        int n9 = 300;
        float f7 = 1.0f;
        int n10 = 0;
        int n11 = 0;
        int n12 = -1;
        int n13 = 0;
        int n14 = 0;
        int n15 = 17;
        float f8 = 30.0f;
        int n16 = 0;
        boolean bl = false;
        int n17 = 1;
        int n18 = 1;
        int n19 = 127;
        int n20 = 1;
        int n21 = 10;
        int n22 = 0;
        int n23 = 0;
        boolean bl2 = false;
        while (true) {
            System.sleep(1);
            this.gold_get_iti();
            n2 = this.pad0.getButton();
            n3 = this.pad0.getEdge();
            n = this.pad1.getButton();
            n4 = this.pad1.getEdge();
            n6 = 0;
            if ((n & 0x100) != 0) {
                if ((n2 & 0x40) != 0) {
                    System.println("-------------* ManualCam_record()start *-----------");
                    System.sleep(2);
                    this.ManualCam_record();
                    this.ManualCam_play();
                    System.println("-------------* ManualCam_record()end *-----------");
                }
                if ((n2 & 8) != 0) {
                    this.Debugprint5();
                    System.println("-------------* Debugprint5() *-----------");
                }
                if ((n2 & 2) != 0) {
                    System.println("-------------* ManualCam_play_sub()start *-----------");
                    this.ManualCam_play_sub();
                    System.println("-------------* ManualCam_play_sub()end *-----------");
                }
                if ((n2 & 0x80) != 0) {
                    System.println("-------------* Gnochk(gold) *-----------");
                    System.sleep(7);
                }
                if ((n2 & 0x20) != 0) {
                    System.println("-------------* ColorScreenSet() *-----------");
                    this.ColorScreen_flag = 0;
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    System.sleep(7);
                    this.ColorScreenSet();
                }
                if ((n2 & 4) != 0) {
                    System.println("-------------* ColorScreenSet()_temae *-----------");
                    this.ColorScreen_flag = 1;
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    System.sleep(7);
                    this.ColorScreenSet();
                }
                if ((n2 & 0x10) != 0) {
                    System.println("-------------* map_look_start *-----------");
                    System.sleep(7);
                    this.map_look();
                }
                if ((n2 & 8) != 0) {
                    System.println("-------------* chr_look_start *-----------");
                    this.ColorScreenSet_exec = true;
                    System.sleep(7);
                    this.chr_look();
                    this.ColorScreenSet_exec = false;
                }
                if ((n2 & 1) != 0) {
                    System.println("-------------* point_light()start *-----------");
                    this.ColorScreenSet_exec = true;
                    System.sleep(7);
                    this.point_light();
                    System.println("-------------* point_light()end *-----------");
                    this.ColorScreenSet_exec = false;
                }
            } else {
                if ((n2 & 0x10) != 0) {
                    this.FocusSet();
                    n6 = 1;
                }
                if (n23 != 0) {
                    n23 = 0;
                    n22 = 0;
                    this.camtool0_objtool1_flag = 1;
                }
                if ((n2 & 0x4000) != 0) {
                    n22 = 1;
                    n23 = 2;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 0x1000) != 0) {
                    n22 = 1;
                    n23 = 1;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 0x2000) != 0) {
                    n22 = 1;
                    n23 = 4;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 1) != 0) {
                    n22 = 1;
                    n23 = 5;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 2) != 0) {
                    n22 = 1;
                    n23 = 6;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 4) != 0) {
                    if ((n2 & 0x8000) != 0) {
                        n22 = 1;
                        n23 = 7;
                        this.camtool0_objtool1_flag = 0;
                    }
                } else if ((n2 & 0x8000) != 0) {
                    n22 = 1;
                    n23 = 3;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 0x400) != 0 || (n2 & 0x200) != 0) {
                    n22 = 1;
                    n23 = 8;
                    this.camtool0_objtool1_flag = 0;
                }
                if ((n2 & 0x100) != 0 && (n2 & 0x40) != 0) {
                    if (n22 == 0) {
                        n22 = 2;
                        this.camtool0_objtool1_flag = 2;
                        System.println("objtool---------------------------scl");
                        if (this.kind_c0_u1 == 0) {
                            this.obj_scale = this.gold.getScale();
                            f4 = this.obj_scale.x;
                            f5 = this.obj_scale.y;
                            f6 = this.obj_scale.z;
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.obj_scale = this.gold_unit.getScale();
                            f4 = this.obj_scale.x;
                            f5 = this.obj_scale.y;
                            f6 = this.obj_scale.z;
                        }
                    } else if (n22 == 2) {
                        n22 = 0;
                        this.camtool0_objtool1_flag = 1;
                        System.println("objtool---------------------------ido");
                    }
                    System.sleep(3);
                }
            }
            if (n22 == 0 && (n & 0x100) != 0 && (n & 0x40) != 0 && (n & 2) != 0 && (n & 8) != 0) {
                if (this.camtool0_objtool1_flag == 1) {
                    this.camtool0_objtool1_flag = 0;
                    System.println("camtool---------------------------cam");
                } else if (this.camtool0_objtool1_flag == 0) {
                    this.camtool0_objtool1_flag = 1;
                    System.println("objtool---------------------------ido");
                }
                System.sleep(3);
                n6 = 1;
            }
            if (n7 < 27) {
                if ((n7 & 1) == 1) {
                    this.gold_Visible_false();
                }
                if ((n7 & 1) == 0) {
                    this.gold_Visible_true();
                }
                ++n7;
            }
            if (this.camtool0_objtool1_flag == 2) {
                if ((n & 0x1000) != 0) {
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setScale(f3 += f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit.setScale(f3 += f7 / 100.0f, f3, f3);
                    }
                    Runtime.setRegister(0, f3);
                    System.println("one_scale-----------------/[#0]");
                }
                if ((n & 0x4000) != 0) {
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setScale(f3 -= f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit.setScale(f3 -= f7 / 100.0f, f3, f3);
                    }
                    Runtime.setRegister(0, f3);
                    System.println("one_scale-----------------/[#0]");
                }
                if ((n & 0x100) != 0) {
                    if ((n & 0x20) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4, f5 += f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4, f5 += f7 / 100.0f, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 0x40) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4, f5 -= f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4, f5 -= f7 / 100.0f, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 1) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4 += f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4 += f7 / 100.0f, f5, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 4) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4 -= f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4 -= f7 / 100.0f, f5, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 2) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4, f5, f6 += f7 / 100.0f);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4, f5, f6 += f7 / 100.0f);
                        }
                        n6 = 27;
                    }
                    if ((n & 8) != 0) {
                        if (this.kind_c0_u1 == 0) {
                            this.gold.setScale(f4, f5, f6 -= f7 / 100.0f);
                        }
                        if (this.kind_c0_u1 == 1) {
                            this.gold_unit.setScale(f4, f5, f6 -= f7 / 100.0f);
                        }
                        n6 = 27;
                    }
                    if (n6 == 27) {
                        Runtime.setRegister(0, f4);
                        Runtime.setRegister(1, f5);
                        Runtime.setRegister(2, f6);
                        System.println("robo.setScale(/[#0]f,/[#1]f,/[#2]f);");
                    }
                }
                if ((n2 & 0x100) != 0 && this.kind_c0_u1 == 1) {
                    if ((n & 8) != 0) {
                        this.gold_unit.setArgs(0, 0.0f, 0.0f, f -= f7 / 100.0f, f2);
                    }
                    if ((n & 2) != 0) {
                        this.gold_unit.setArgs(0, 0.0f, 0.0f, f += f7 / 100.0f, f2);
                    }
                    if ((n & 4) != 0) {
                        this.gold_unit.setArgs(0, 0.0f, 0.0f, f, f2 -= f7 / 100.0f);
                    }
                    if ((n & 1) != 0) {
                        this.gold_unit.setArgs(0, 0.0f, 0.0f, f, f2 += f7 / 100.0f);
                    }
                    if ((n & 0x80) != 0) {
                        this.gold_unit.setArgs(2, --n5, 0, 0, -1);
                    }
                    if ((n & 0x20) != 0) {
                        this.gold_unit.setArgs(2, ++n5, 0, 0, -1);
                    }
                }
                if ((n & 0x800) != 0) {
                    Runtime.setRegister(0, (float) n8);
                    System.println("gold_number-------------------(/[#0])");
                    Runtime.setRegister(0, f);
                    Runtime.setRegister(1, f2);
                    Runtime.setRegister(2, (float) n5);
                    Runtime.setRegister(3, f3);
                    Runtime.setRegister(4, f4);
                    Runtime.setRegister(5, f5);
                    Runtime.setRegister(6, f6);
                    System.println("haba -----------------/[#0]");
                    System.println("takasa ---------------/[#1]");
                    System.println("noudo ----------------/[#2]");
                    System.println("one_scale ------------/[#3]");
                    System.println("robo.setScale(/[#4]f,/[#5]f,/[#6]f);");
                    System.sleep(7);
                }
                if ((n & 0x400) != 0) {
                    if (f7 <= 10.0f) {
                        f7 -= 2.0f;
                    }
                    if (f7 > 10.0f) {
                        f7 -= 10.0f;
                    }
                    if (f7 >= 50.0f) {
                        f7 -= 30.0f;
                    }
                    if (f7 <= 0.0f) {
                        f7 = 1.0f;
                    }
                    Runtime.setRegister(0, f7);
                    System.println("f_speed--------------------------------------------------(/[#0])");
                    System.sleep(2);
                }
                if ((n & 0x200) != 0) {
                    if (f7 <= 10.0f) {
                        f7 += 2.0f;
                    }
                    if (f7 > 10.0f) {
                        f7 += 10.0f;
                    }
                    if (f7 >= 50.0f) {
                        f7 += 30.0f;
                    }
                    if (f7 >= 200.0f) {
                        f7 = 400.0f;
                    }
                    Runtime.setRegister(0, f7);
                    System.println("f_speed--------------------------------------------------(/[#0])");
                    System.sleep(2);
                }
            }
            if (this.camtool0_objtool1_flag == 0) {
                if (n23 != 0) {
                    if (n23 == 1) {
                        if (--n11 < 0) {
                            n11 = 0;
                        }
                        n6 = (n2 & 4) != 0 ? 127 : 177;
                    }
                    if (n23 == 2) {
                        if (++n11 > n15) {
                            n11 = n15;
                        }
                        if (n11 > n12) {
                            n11 = n12;
                        }
                        if (n11 < 0) {
                            n11 = 0;
                        }
                        n6 = (n2 & 4) != 0 ? 127 : 177;
                    }
                    if (n6 == 127) {
                        Runtime.setRegister(1, (float) n11);
                        Runtime.setRegister(2, (float) n12);
                        System.println("non_move_view-/[#1]|cap-/[#2]");
                        System.sleep(5);
                    }
                    if (n6 == 177) {
                        if (n22 == 0) {
                            this.cam0.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                            this.cam0.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                            this.cam0.setFov(fArray3[n11 * 2 + 1]);
                            f8 = fArray3[n11 * 2 + 1];
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("view-/[#1]|cap-/[#2]");
                        } else if (n22 == 1) {
                            if (this.kind_c0_u1 == 0) {
                                this.gold.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.gold.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                Runtime.setRegister(0, (float) n8);
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("gold-no-/[#0]|view-/[#1]|cap-/[#2]");
                            } else if (this.kind_c0_u1 == 1) {
                                this.gold_unit.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.gold_unit.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                Runtime.setRegister(0, (float) n8);
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("gold-unit-no-/[#0]|view-/[#1]|cap-/[#2]");
                            }
                        }
                        System.sleep(5);
                    }
                    if (n23 == 3 && n11 < n15) {
                        if ((n2 & 0x100) != 0 && n11 == n12) {
                            ++n11;
                            System.println("--------plus_one_write--------");
                        }
                        if (n22 == 0) {
                            fArray[n11 * 4 + 1] = this.cam0.getTranslateX();
                            fArray[n11 * 4 + 2] = this.cam0.getTranslateY();
                            fArray[n11 * 4 + 3] = this.cam0.getTranslateZ();
                            fArray2[n11 * 4 + 1] = this.cam0.getRotateX();
                            fArray2[n11 * 4 + 2] = this.cam0.getRotateY();
                            fArray2[n11 * 4 + 3] = this.cam0.getRotateZ();
                            fArray3[n11 * 2 + 1] = this.cam0.getFov();
                        } else if (n22 == 1) {
                            if (this.kind_c0_u1 == 0) {
                                fArray[n11 * 4 + 1] = this.gold.px;
                                fArray[n11 * 4 + 2] = this.gold.py;
                                fArray[n11 * 4 + 3] = this.gold.pz;
                                fArray2[n11 * 4 + 1] = this.gold.rx;
                                fArray2[n11 * 4 + 2] = this.gold.ry;
                                fArray2[n11 * 4 + 3] = this.gold.rz;
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (this.kind_c0_u1 == 1) {
                                fArray[n11 * 4 + 1] = this.gold_unit.px;
                                fArray[n11 * 4 + 2] = this.gold_unit.py;
                                fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            }
                        }
                        if (n11 == n12 + 1) {
                            if (++n12 > 0) {
                                n16 = n19 / n12;
                            }
                            if (n12 == 0) {
                                n16 = 1;
                            }
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            bl = true;
                        }
                        Runtime.setRegister(0, (float) (++n11) - 1.0f);
                        Runtime.setRegister(1, (float) n11);
                        Runtime.setRegister(2, (float) n12);
                        System.println("write-/[#0]|now-/[#1]|cap-/[#2]");
                        System.sleep(5);
                    }
                    if (n23 == 4) {
                        if (--n12 < -1) {
                            n12 = -1;
                        }
                        if (n11 > n12 + 1) {
                            n11 = n12 + 1;
                        }
                        if (n12 > 0) {
                            n16 = n19 / n12;
                        }
                        if (n12 == 0) {
                            n16 = 1;
                        }
                        n13 = 0;
                        while (n13 <= n12) {
                            fArray[n13 * 4] = n16 * n13 + 1;
                            fArray2[n13 * 4] = n16 * n13 + 1;
                            fArray3[n13 * 2] = n16 * n13 + 1;
                            ++n13;
                        }
                        n13 = n12;
                        while (n13 < n15) {
                            fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                            fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                            fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                            fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                            fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                            fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                            fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                            fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                            fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                            fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                            ++n13;
                        }
                        Runtime.setRegister(0, (float) n12 + 1.0f);
                        Runtime.setRegister(1, (float) n11);
                        Runtime.setRegister(2, (float) n12);
                        System.println("erase-/[#1]|now-/[#1]|cap-/[#2]");
                        System.sleep(5);
                    }
                    if (n23 == 5) {
                        if (n22 == 0) {
                            this.cam1.change();
                            this.cam1.transSPL(fArray, 1);
                            this.cam1.rotateSPL(fArray2, 1);
                            this.cam1.fovSPL(fArray3, n17);
                            System.println("live-normal-cam");
                        } else if (n22 == 1) {
                            this.cam2.transSPL(fArray, 1);
                            this.cam2.rotateSPL(fArray2, 1);
                            this.cam2.fovSPL(fArray3, n17);
                            System.println("live-normal-human");
                        }
                        Runtime.setRegister(0, (float) n19);
                        System.println("time-/[#0]");
                        System.sleep(7);
                        while (true) {
                            n2 = this.pad0.getButton();
                            n = this.pad1.getButton();
                            if ((n2 & 0x40) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x800) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x100) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x1000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x4000) != 0) {
                                this.cam0.change();
                                System.sleep(5);
                                break;
                            }
                            if ((n2 & 1) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 2) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x800) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x100) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x1000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x4000) != 0) {
                                this.cam0.change();
                                System.sleep(5);
                                break;
                            }
                            if ((n2 & 0x400) != 0) {
                                Runtime.setRegister(0, (float) (++n21));
                                System.println("cam_time_tani++(/[#0])");
                            }
                            if ((n2 & 0x200) != 0) {
                                if (--n21 < 1) {
                                    n21 = 1;
                                }
                                Runtime.setRegister(0, (float) n21);
                                System.println("cam_time_tani--(/[#0])");
                            }
                            if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    this.gold.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                    this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1 == 1) {
                                    this.gold_unit.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                    this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                }
                            }
                            System.sleep(1);
                        }
                    }
                    if (n23 == 6) {
                        if (n22 == 0) {
                            System.sleep(4);
                            this.cam1.change();
                            this.cam2.rotateSPL(fArray, n17, n18, n19);
                            this.cam1.rotateSPL(fArray2, n17, n18, n19);
                            this.cam1.fovSPL(fArray3, n17);
                            System.println("live-pretty-cam");
                        } else if (n22 == 1) {
                            System.sleep(4);
                            this.cam3.rotateSPL(fArray, n17, n18, n19);
                            this.cam2.rotateSPL(fArray2, n17, n18, n19);
                            this.cam2.fovSPL(fArray3, n17);
                            System.println("live-human-pretty-cam");
                        }
                        if (n17 == 1) {
                            System.println("--SPL_CARDINAL");
                        }
                        if (n17 == 0) {
                            System.println("----SPL_LINEAR");
                        }
                        if (n18 == 2) {
                            System.println("---PATH_LINEAR");
                        }
                        if (n18 == 1) {
                            System.println("-----PATH_DOWN");
                        }
                        if (n18 == 0) {
                            System.println("-------PATH_UP");
                        }
                        if (n18 == 3) {
                            System.println("---PATH_SPLINE");
                        }
                        Runtime.setRegister(0, (float) n19);
                        System.println("time-/[#0]");
                        while (true) {
                            n2 = this.pad0.getButton();
                            n = this.pad1.getButton();
                            if ((n2 & 0x40) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x800) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x100) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x1000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 0x4000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 1) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n2 & 2) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x800) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x100) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x1000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if ((n & 0x4000) != 0) {
                                this.cam0.change();
                                System.sleep(3);
                                break;
                            }
                            if (n22 == 0) {
                                this.cam1.setTranslate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                            }
                            if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    this.gold.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                    this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1 == 1) {
                                    this.gold_unit.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                    this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                }
                            }
                            if ((n2 & 0x400) != 0) {
                                Runtime.setRegister(0, (float) (++n21));
                                System.println("cam_time_tani++(/[#0])");
                            }
                            if ((n2 & 0x200) != 0) {
                                if (--n21 < 1) {
                                    n21 = 1;
                                }
                                Runtime.setRegister(0, (float) n21);
                                System.println("cam_time_tani--(/[#0])");
                            }
                            System.sleep(1);
                        }
                    }
                    if (n23 == 7) {
                        if (n11 <= n12) {
                            if (n22 == 0) {
                                fArray[n11 * 4 + 1] = this.cam0.getTranslateX();
                                fArray[n11 * 4 + 2] = this.cam0.getTranslateY();
                                fArray[n11 * 4 + 3] = this.cam0.getTranslateZ();
                                fArray2[n11 * 4 + 1] = this.cam0.getRotateX();
                                fArray2[n11 * 4 + 2] = this.cam0.getRotateY();
                                fArray2[n11 * 4 + 3] = this.cam0.getRotateZ();
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                }
                            }
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("change-/[#1]|now-/[#1]|cap-/[#2]");
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            System.sleep(3);
                        }
                        if ((n & 0x400) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 += n21));
                            System.println("time-/[#0]");
                        }
                        if ((n & 0x200) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 -= n21));
                            System.println("time-/[#0]");
                        }
                        if (n6 == 7) {
                            if (n12 > 0) {
                                n16 = n19 / n12;
                            }
                            if (n12 == 0) {
                                n16 = 1;
                            }
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            System.sleep(2);
                        }
                    }
                    if (n23 == 8) {
                        if ((n2 & 0x400) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 += n21));
                            System.println("time-/[#0]");
                        }
                        if ((n2 & 0x200) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 -= n21));
                            System.println("time-/[#0]");
                        }
                        if (n6 == 7) {
                            if (n12 > 0) {
                                n16 = n19 / n12;
                            }
                            if (n12 == 0) {
                                n16 = 1;
                            }
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            System.sleep(2);
                        }
                    }
                } else {
                    if ((n & 0x80) != 0) {
                        if ((n & 0x40) != 0) {
                            if (--n12 < -1) {
                                n12 = -1;
                            }
                            if (n11 > n12 + 1) {
                                n11 = n12 + 1;
                            }
                            if (n12 > 0) {
                                n16 = n19 / n12;
                            }
                            if (n12 == 0) {
                                n16 = 1;
                            }
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            Runtime.setRegister(0, (float) n12 + 1.0f);
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("erase-/[#1]|now-/[#1]|cap-/[#2]");
                        }
                        System.sleep(3);
                    }
                    if ((n & 0x800) != 0) {
                        if ((n & 0x40) != 0) {
                            Runtime.setRegister(0, fArray3[1]);
                            System.println("STCam.setFov(/[#0]f);");
                            System.println("cam1.setFov(/[#0]f);");
                            System.println("cam0.setFov(/[#0]f);");
                            System.println("float pos00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray[n13 * 4] * 1.0E7f);
                                Runtime.setRegister(1, fArray[n13 * 4 + 1] * 1.0E7f);
                                Runtime.setRegister(2, fArray[n13 * 4 + 2] * 1.0E7f);
                                Runtime.setRegister(3, fArray[n13 * 4 + 3] * 1.0E7f);
                                System.println("  /[#0]fnc,/[#1]fnc,/[#2]fnc,/[#3]fnc,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("float rot00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray2[n13 * 4] * 1.0E7f);
                                Runtime.setRegister(1, fArray2[n13 * 4 + 1] * 1.0E7f);
                                Runtime.setRegister(2, fArray2[n13 * 4 + 2] * 1.0E7f);
                                Runtime.setRegister(3, fArray2[n13 * 4 + 3] * 1.0E7f);
                                System.println("  /[#0]fnc,/[#1]fnc,/[#2]fnc,/[#3]fnc,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("cam1.transSPL(pos00, SPL_CARDINAL);");
                            System.println("cam1.rotateSPL(rot00, SPL_CARDINAL);");
                            System.println("float fov00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray3[n13 * 2] * 1.0E7f);
                                Runtime.setRegister(1, fArray3[n13 * 2 + 1] * 1.0E7f);
                                System.println("  /[#0]fnc,/[#1]fnc,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("cam1.fovSPL(fov00, SPL_CARDINAL);");
                            System.sleep(3);
                        } else {
                            Runtime.setRegister(0, fArray3[1]);
                            System.println("STCam.setFov(/[#0]f);");
                            System.println("cam1.setFov(/[#0]f);");
                            System.println("cam0.setFov(/[#0]f);");
                            System.println("float pos00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray[n13 * 4]);
                                Runtime.setRegister(1, fArray[n13 * 4 + 1]);
                                Runtime.setRegister(2, fArray[n13 * 4 + 2]);
                                Runtime.setRegister(3, fArray[n13 * 4 + 3]);
                                System.println("  /[#0]f,/[#1]f,/[#2]f,/[#3]f,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("float rot00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray2[n13 * 4]);
                                Runtime.setRegister(1, fArray2[n13 * 4 + 1]);
                                Runtime.setRegister(2, fArray2[n13 * 4 + 2]);
                                Runtime.setRegister(3, fArray2[n13 * 4 + 3]);
                                System.println("  /[#0]f,/[#1]f,/[#2]f,/[#3]f,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("cam1.transSPL(pos00, SPL_CARDINAL);");
                            System.println("cam1.rotateSPL(rot00, SPL_CARDINAL);");
                            System.println("float fov00[] = {");
                            n13 = 0;
                            while (n13 <= n12) {
                                Runtime.setRegister(0, fArray3[n13 * 2]);
                                Runtime.setRegister(1, fArray3[n13 * 2 + 1]);
                                System.println("  /[#0]f,/[#1]f,");
                                ++n13;
                            }
                            System.println("};");
                            System.println("cam1.fovSPL(fov00, SPL_CARDINAL);");
                            System.sleep(3);
                        }
                    }
                    if (n6 == 0) {
                        if ((n & 0x1000) != 0) {
                            if (--n11 < 0) {
                                n11 = 0;
                            }
                            n6 = (n & 0x40) != 0 ? 27 : 77;
                        }
                        if ((n & 0x4000) != 0) {
                            if (++n11 > n15) {
                                n11 = n15;
                            }
                            if (n11 > n12) {
                                n11 = n12;
                            }
                            if (n11 < 0) {
                                n11 = 0;
                            }
                            n6 = (n & 0x40) != 0 ? 27 : 77;
                        }
                        if (n6 == 27) {
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("non_move_view-/[#1]|cap-/[#2]");
                        }
                        if (n6 == 77) {
                            if (n22 == 0) {
                                this.cam0.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.cam0.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                this.cam0.setFov(fArray3[n11 * 2 + 1]);
                                f8 = fArray3[n11 * 2 + 1];
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("view-/[#1]|cap-/[#2]");
                            } else if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    this.gold.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                    this.gold.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                    Runtime.setRegister(0, (float) n8);
                                    Runtime.setRegister(1, (float) n11);
                                    Runtime.setRegister(2, (float) n12);
                                    System.println("gold-no-/[#0]|view-/[#1]|cap-/[#2]");
                                } else if (this.kind_c0_u1 == 1) {
                                    this.gold_unit.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                    this.gold_unit.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                    Runtime.setRegister(0, (float) n8);
                                    Runtime.setRegister(1, (float) n11);
                                    Runtime.setRegister(2, (float) n12);
                                    System.println("gold-unit-no-/[#0]|view-/[#1]|cap-/[#2]");
                                }
                            }
                        }
                        System.sleep(3);
                    }
                    if ((n & 1) == 0 && (n & 0x80) == 0 && (n & 0x40) == 0 && (n & 0x100) != 0 && (n & 0x20) != 0) {
                        if (n11 <= n12) {
                            if (n22 == 0) {
                                fArray[n11 * 4 + 1] = this.cam0.getTranslateX();
                                fArray[n11 * 4 + 2] = this.cam0.getTranslateY();
                                fArray[n11 * 4 + 3] = this.cam0.getTranslateZ();
                                fArray2[n11 * 4 + 1] = this.cam0.getRotateX();
                                fArray2[n11 * 4 + 2] = this.cam0.getRotateY();
                                fArray2[n11 * 4 + 3] = this.cam0.getRotateZ();
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                }
                            }
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("change-/[#1]|now-/[#1]|cap-/[#2]");
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            System.sleep(3);
                        }
                        n6 = 1;
                    }
                    if (n6 == 0) {
                        if ((n & 0x40) != 0) {
                            n6 = 12;
                        }
                        if ((n & 0x100) != 0) {
                            n6 = 18;
                        }
                        if ((n & 1) == 0 && (n6 == 12 || n6 == 18) && (n & 0x20) != 0 && n11 < n15) {
                            if (n6 == 18 && (n & 0x80) != 0 && n11 == n12) {
                                ++n11;
                                System.println("--------plus_one_write--------");
                            }
                            if (n22 == 0) {
                                fArray[n11 * 4 + 1] = this.cam0.getTranslateX();
                                fArray[n11 * 4 + 2] = this.cam0.getTranslateY();
                                fArray[n11 * 4 + 3] = this.cam0.getTranslateZ();
                                fArray2[n11 * 4 + 1] = this.cam0.getRotateX();
                                fArray2[n11 * 4 + 2] = this.cam0.getRotateY();
                                fArray2[n11 * 4 + 3] = this.cam0.getRotateZ();
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (n22 == 1) {
                                if (this.kind_c0_u1 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                }
                            }
                            if (n11 == n12 + 1) {
                                if (++n12 > 0) {
                                    n16 = n19 / n12;
                                }
                                if (n12 == 0) {
                                    n16 = 1;
                                }
                                n13 = 0;
                                while (n13 <= n12) {
                                    fArray[n13 * 4] = n16 * n13 + 1;
                                    fArray2[n13 * 4] = n16 * n13 + 1;
                                    fArray3[n13 * 2] = n16 * n13 + 1;
                                    ++n13;
                                }
                                n13 = n12;
                                while (n13 < n15) {
                                    fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                    fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                    fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                    fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                    fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                    fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                    fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                    fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                    fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                    fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                    ++n13;
                                }
                                bl = true;
                            }
                            Runtime.setRegister(0, (float) (++n11) - 1.0f);
                            Runtime.setRegister(1, (float) n11);
                            Runtime.setRegister(2, (float) n12);
                            System.println("write-/[#0]|now-/[#1]|cap-/[#2]");
                            System.sleep(5);
                            n6 = 1;
                        }
                    }
                    if ((n & 0x100) != 0) {
                        if ((n & 0x800) != 0) {
                            n13 = 0;
                            System.println("--------trans--------");
                            while (n13 <= n15) {
                                Runtime.setRegister(0, fArray[n13 * 4]);
                                Runtime.setRegister(1, fArray[n13 * 4 + 1]);
                                Runtime.setRegister(2, fArray[n13 * 4 + 2]);
                                Runtime.setRegister(3, fArray[n13 * 4 + 3]);
                                System.println("  /[#0]f,/[#1]f,/[#2]f,/[#3]f");
                                ++n13;
                            }
                            n13 = 0;
                            System.println("--------rotate--------");
                            while (n13 <= n15) {
                                Runtime.setRegister(0, fArray2[n13 * 4]);
                                Runtime.setRegister(1, fArray2[n13 * 4 + 1]);
                                Runtime.setRegister(2, fArray2[n13 * 4 + 2]);
                                Runtime.setRegister(3, fArray2[n13 * 4 + 3]);
                                System.println("  /[#0]f,/[#1]f,/[#2]f,/[#3]f");
                                ++n13;
                            }
                            n13 = 0;
                            System.println("--------fov--------");
                            while (n13 <= n15) {
                                Runtime.setRegister(0, fArray3[n13 * 2]);
                                Runtime.setRegister(1, fArray3[n13 * 2 + 1]);
                                System.println("  /[#0]f,/[#1]f");
                                ++n13;
                            }
                            System.sleep(3);
                        }
                        if ((n & 4) != 0 && (n & 0x40) != 0) {
                            if (n22 == 0) {
                                this.cam1.change();
                                this.cam1.transSPL(fArray, 1);
                                this.cam1.rotateSPL(fArray2, 1);
                                this.cam1.fovSPL(fArray3, n17);
                                System.println("live-normal-cam");
                            } else if (n22 == 1) {
                                this.cam2.transSPL(fArray, 1);
                                this.cam2.rotateSPL(fArray2, 1);
                                this.cam2.fovSPL(fArray3, n17);
                                System.println("live-normal-human");
                            }
                            Runtime.setRegister(0, (float) n19);
                            System.println("time-/[#0]");
                            System.sleep(7);
                            while (true) {
                                if (((n = this.pad1.getButton()) & 0x40) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x800) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x100) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x1000) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x4000) != 0) {
                                    this.cam0.change();
                                    System.sleep(5);
                                    break;
                                }
                                if ((n & 0x400) != 0) {
                                    Runtime.setRegister(0, (float) (++n21));
                                    System.println("cam_time_tani++(/[#0])");
                                }
                                if ((n & 0x200) != 0) {
                                    if (--n21 < 1) {
                                        n21 = 1;
                                    }
                                    Runtime.setRegister(0, (float) n21);
                                    System.println("cam_time_tani--(/[#0])");
                                }
                                if (n22 == 1) {
                                    if (this.kind_c0_u1 == 0) {
                                        this.gold.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                        this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1 == 1) {
                                        this.gold_unit.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                        this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    }
                                }
                                System.sleep(1);
                            }
                        }
                        if ((n & 4) != 0) {
                            if (n22 == 0) {
                                System.sleep(4);
                                this.cam1.change();
                                this.cam2.rotateSPL(fArray, n17, n18, n19);
                                this.cam1.rotateSPL(fArray2, n17, n18, n19);
                                this.cam1.fovSPL(fArray3, n17);
                                System.println("live-pretty-cam");
                            } else if (n22 == 1) {
                                System.sleep(4);
                                this.cam3.rotateSPL(fArray, n17, n18, n19);
                                this.cam2.rotateSPL(fArray2, n17, n18, n19);
                                this.cam2.fovSPL(fArray3, n17);
                                System.println("live-human-pretty-cam");
                            }
                            if (n17 == 1) {
                                System.println("--SPL_CARDINAL");
                            }
                            if (n17 == 0) {
                                System.println("----SPL_LINEAR");
                            }
                            if (n18 == 2) {
                                System.println("---PATH_LINEAR");
                            }
                            if (n18 == 1) {
                                System.println("-----PATH_DOWN");
                            }
                            if (n18 == 0) {
                                System.println("-------PATH_UP");
                            }
                            if (n18 == 3) {
                                System.println("---PATH_SPLINE");
                            }
                            Runtime.setRegister(0, (float) n19);
                            System.println("time-/[#0]");
                            while (true) {
                                if (((n = this.pad1.getButton()) & 0x40) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x800) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x100) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x1000) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if ((n & 0x4000) != 0) {
                                    this.cam0.change();
                                    System.sleep(3);
                                    break;
                                }
                                if (n22 == 0) {
                                    this.cam1.setTranslate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                }
                                if (n22 == 1) {
                                    if (this.kind_c0_u1 == 0) {
                                        this.gold.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                        this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1 == 1) {
                                        this.gold_unit.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                        this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    }
                                }
                                if ((n & 0x400) != 0) {
                                    Runtime.setRegister(0, (float) (++n21));
                                    System.println("cam_time_tani++(/[#0])");
                                }
                                if ((n & 0x200) != 0) {
                                    if (--n21 < 1) {
                                        n21 = 1;
                                    }
                                    Runtime.setRegister(0, (float) n21);
                                    System.println("cam_time_tani--(/[#0])");
                                }
                                System.sleep(1);
                            }
                        }
                        if ((n & 1) != 0) {
                            if ((n & 0x200) != 0) {
                                if (--n20 < 1) {
                                    n20 = 1;
                                }
                                n6 = 7;
                                System.sleep(3);
                            }
                            if ((n & 0x400) != 0) {
                                if (++n20 > n15) {
                                    n20 = n15;
                                }
                                n6 = 7;
                                System.sleep(3);
                            }
                            if (n6 == 7) {
                                Runtime.setRegister(0, (float) n20);
                                Runtime.setRegister(1, fArray[n20 * 4]);
                                Runtime.setRegister(2, (float) n20 + 1.0f);
                                Runtime.setRegister(3, fArray[(n20 + 1) * 4]);
                                System.println("t_no_change-/[#0]-/[#1]/[#2]-/[#3]");
                                n6 = 0;
                            }
                            n13 = (int) fArray[n20 * 4];
                            n14 = (int) fArray[(n20 + 1) * 4];
                            if ((n & 0x10) != 0) {
                                n13 = (int) fArray[n20 * 4];
                                if ((n13 += n21) < 1) {
                                    n13 = 1;
                                }
                                n6 = 6;
                            }
                            if ((n & 0x80) != 0) {
                                n13 = (int) fArray[n20 * 4];
                                if ((n13 -= n21) < 1) {
                                    n13 = 1;
                                }
                                n6 = 6;
                            }
                            if ((n & 0x20) != 0) {
                                n14 = (int) fArray[(n20 + 1) * 4];
                                if ((n14 += n21) < 1) {
                                    n14 = 1;
                                }
                                n6 = 6;
                            }
                            if ((n & 0x40) != 0) {
                                n14 = (int) fArray[(n20 + 1) * 4];
                                if ((n14 -= n21) < 1) {
                                    n14 = 1;
                                }
                                n6 = 6;
                            }
                            if (n6 == 6) {
                                Runtime.setRegister(0, (float) n20);
                                Runtime.setRegister(1, (float) n13);
                                Runtime.setRegister(2, (float) n20 + 1.0f);
                                Runtime.setRegister(3, (float) n14);
                                System.println("time_change-/[#0]-/[#1]/[#2]-/[#3]");
                                fArray[n20 * 4] = n13;
                                fArray2[n20 * 4] = n13;
                                fArray3[n20 * 2] = n13;
                                fArray[(n20 + 1) * 4] = n14;
                                fArray2[(n20 + 1) * 4] = n14;
                                fArray3[(n20 + 1) * 2] = n14;
                                n6 = 0;
                            }
                            n6 = 1;
                        }
                        if (n6 == 18) {
                            if ((n & 0x200) != 0) {
                                if (n17 == 0) {
                                    n17 = 1;
                                    System.println("---SPL_CARDINAL---");
                                } else {
                                    n17 = 0;
                                    System.println("---SPL_LINEAR---");
                                }
                                System.sleep(3);
                            }
                            if ((n & 0x400) != 0) {
                                if (n18 == 1) {
                                    n18 = 0;
                                    System.println("---PATH_UP---");
                                } else if (n18 == 0) {
                                    n18 = 2;
                                    System.println("---PATH_LINEAR---");
                                } else if (n18 == 2) {
                                    n18 = 3;
                                    System.println("---PATH_SPLINE---");
                                } else if (n18 == 3) {
                                    n18 = 1;
                                    System.println("---PATH_DOWN---");
                                }
                                System.sleep(3);
                            }
                        }
                        n6 = 1;
                    }
                    if (n6 == 0) {
                        if ((n & 0x400) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 += n21));
                            System.println("time-/[#0]");
                        }
                        if ((n & 0x200) != 0) {
                            n6 = 7;
                            Runtime.setRegister(0, (float) (n19 -= n21));
                            System.println("time-/[#0]");
                        }
                        if (n6 == 7) {
                            if (n12 > 0) {
                                n16 = n19 / n12;
                            }
                            if (n12 == 0) {
                                n16 = 1;
                            }
                            n13 = 0;
                            while (n13 <= n12) {
                                fArray[n13 * 4] = n16 * n13 + 1;
                                fArray2[n13 * 4] = n16 * n13 + 1;
                                fArray3[n13 * 2] = n16 * n13 + 1;
                                ++n13;
                            }
                            n13 = n12;
                            while (n13 < n15) {
                                fArray[(n13 + 1) * 4] = fArray[n13 * 4];
                                fArray[(n13 + 1) * 4 + 1] = fArray[n13 * 4 + 1];
                                fArray[(n13 + 1) * 4 + 2] = fArray[n13 * 4 + 2];
                                fArray[(n13 + 1) * 4 + 3] = fArray[n13 * 4 + 3];
                                fArray2[(n13 + 1) * 4] = fArray2[n13 * 4];
                                fArray2[(n13 + 1) * 4 + 1] = fArray2[n13 * 4 + 1];
                                fArray2[(n13 + 1) * 4 + 2] = fArray2[n13 * 4 + 2];
                                fArray2[(n13 + 1) * 4 + 3] = fArray2[n13 * 4 + 3];
                                fArray3[(n13 + 1) * 2] = fArray3[n13 * 2];
                                fArray3[(n13 + 1) * 2 + 1] = fArray3[n13 * 2 + 1];
                                ++n13;
                            }
                            System.sleep(2);
                        }
                    }
                    n6 = 0;
                }
            }
            if (this.camtool0_objtool1_flag != 1) continue;
            if (n6 == 0 && (n & 0x100) != 0 && (n & 0x40) != 0) {
                if ((n & 8) != 0) {
                    this.rb_rx += 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 2) != 0) {
                    this.rb_rx -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 4) != 0) {
                    this.rb_rz += 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 1) != 0) {
                    this.rb_rz -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 0x80) != 0) {
                    this.rb_ry += 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 0x20) != 0) {
                    this.rb_ry -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                n6 = 1;
            }
            if (n6 == 0 && (n & 0x100) != 0) {
                if ((n & 0x200) != 0) {
                    this.rb_y += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 0x400) != 0) {
                    this.rb_y -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 8) != 0) {
                    this.rb_x += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 2) != 0) {
                    this.rb_x -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 4) != 0) {
                    this.rb_z += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 1) != 0) {
                    this.rb_z -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 0x80) != 0) {
                    this.rb_ry += 2.5f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                if ((n & 0x20) != 0) {
                    this.rb_ry -= 2.5f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1 == 1) {
                        this.gold_unit_set_rotate();
                    }
                }
                n6 = 2;
            }
            if (n6 == 0) {
                if ((n & 0x800) != 0) {
                    this.gold_get_iti();
                    Runtime.setRegister(0, (float) n8);
                    System.println("gold_number-------------------(/[#0])");
                    if ((n & 0x40) != 0) {
                        Runtime.setRegister(0, this.rb_x * 1.0E7f);
                        Runtime.setRegister(1, this.rb_y * 1.0E7f);
                        Runtime.setRegister(2, this.rb_z * 1.0E7f);
                        Runtime.setRegister(3, this.rb_rx * 1.0E7f);
                        Runtime.setRegister(4, this.rb_ry * 1.0E7f);
                        Runtime.setRegister(5, this.rb_rz * 1.0E7f);
                        System.println("shion.setTranslate(/[#0]fnc,/[#1]fnc,/[#2]fnc);");
                        System.println("shion.setRotate(/[#3]fnc,/[#4]fnc,/[#5]fnc);");
                        this.px = this.cam0.getTranslateX();
                        this.py = this.cam0.getTranslateY();
                        this.pz = this.cam0.getTranslateZ();
                        this.rx = this.cam0.getRotateX();
                        this.ry = this.cam0.getRotateY();
                        this.rz = this.cam0.getRotateZ();
                        this.fov_new = this.cam0.getFov();
                        Runtime.setRegister(0, this.px * 1.0E7f);
                        Runtime.setRegister(1, this.py * 1.0E7f);
                        Runtime.setRegister(2, this.pz * 1.0E7f);
                        Runtime.setRegister(3, this.rx * 1.0E7f);
                        Runtime.setRegister(4, this.ry * 1.0E7f);
                        Runtime.setRegister(5, this.rz * 1.0E7f);
                        Runtime.setRegister(6, this.fov_new * 1.0E7f);
                        System.println("trans----/[#0]fnc,/[#1]fnc,/[#2]fnc");
                        System.println("rotate---/[#3]fnc,/[#4]fnc,/[#5]fnc");
                        System.println("fov------/[#6]fnc");
                    } else {
                        Runtime.setRegister(0, this.rb_x);
                        Runtime.setRegister(1, this.rb_y);
                        Runtime.setRegister(2, this.rb_z);
                        Runtime.setRegister(3, this.rb_rx);
                        Runtime.setRegister(4, this.rb_ry);
                        Runtime.setRegister(5, this.rb_rz);
                        System.println("robo.setTranslate(/[#0]f,/[#1]f,/[#2]f);");
                        System.println("robo.setRotate(/[#3]f,/[#4]f,/[#5]f);");
                        this.px = this.cam0.getTranslateX();
                        this.py = this.cam0.getTranslateY();
                        this.pz = this.cam0.getTranslateZ();
                        this.rx = this.cam0.getRotateX();
                        this.ry = this.cam0.getRotateY();
                        this.rz = this.cam0.getRotateZ();
                        this.fov_new = this.cam0.getFov();
                        Runtime.setRegister(0, this.px);
                        Runtime.setRegister(1, this.py);
                        Runtime.setRegister(2, this.pz);
                        Runtime.setRegister(3, this.rx);
                        Runtime.setRegister(4, this.ry);
                        Runtime.setRegister(5, this.rz);
                        Runtime.setRegister(6, this.fov_new);
                        System.println("trans----/[#0]f,/[#1]f,/[#2]f");
                        System.println("rotate---/[#3]f,/[#4]f,/[#5]f");
                        System.println("fov------/[#6]f");
                    }
                    n7 = 0;
                    this.gold_Visible_true();
                    System.sleep(7);
                }
                if ((n & 0x400) != 0) {
                    if (n9 >= 3200) {
                        n9 -= 800;
                    } else if (n9 >= 1600) {
                        n9 -= 400;
                    } else if (n9 >= 800) {
                        n9 -= 200;
                    } else if (n9 >= 300) {
                        n9 -= 100;
                    } else if (n9 >= 100) {
                        n9 -= 50;
                    } else if (n9 >= 50) {
                        n9 -= 30;
                    } else if (n9 > 10) {
                        n9 -= 10;
                    } else if (n9 <= 10) {
                        n9 -= 2;
                    } else if (n9 <= 0) {
                        n9 = 1;
                    }
                    if (n9 <= 0) {
                        n9 = 1;
                    }
                    Runtime.setRegister(0, (float) n9);
                    System.println("speed--------------------------------------------------(/[#0])");
                    System.sleep(2);
                    n6 = 1;
                }
                if ((n & 0x200) != 0) {
                    if (n9 >= 3200) {
                        n9 = 6400;
                    } else if (n9 >= 1600) {
                        n9 = 3200;
                    } else if (n9 >= 800) {
                        n9 = 1600;
                    } else if (n9 >= 400) {
                        n9 = 800;
                    } else if (n9 >= 200) {
                        n9 = 400;
                    } else if (n9 >= 50) {
                        n9 += 30;
                    } else if (n9 > 10) {
                        n9 += 10;
                    } else if (n9 <= 10) {
                        n9 += 2;
                    }
                    Runtime.setRegister(0, (float) n9);
                    System.println("speed--------------------------------------------------(/[#0])");
                    System.sleep(2);
                    n6 = 1;
                }
            }
            if ((n & 0x1000) != 0) {
                if (--n10 == -1) {
                    n10 = 0;
                }
                this.hand_cam0_shift(n10);
            }
            if ((n & 0x4000) != 0) {
                if (++n10 == 3) {
                    n10 = 2;
                }
                this.hand_cam0_shift(n10);
            }
            if (n7 <= 3 || n6 != 0 || (n & 0x40) == 0) continue;
            if ((n & 0x20) != 0) {
                if (++n8 > 37) {
                    n8 = 0;
                }
                n6 = 1;
            }
            if ((n & 0x80) != 0) {
                if (--n8 < 0) {
                    n8 = 37;
                }
                n6 = 1;
            }
            if (n6 != 1) continue;
            n7 = 0;
            this.gold_Visible_true();
            switch (n8) {
                case 0: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.ziggy;
                    break;
                }
                case 1: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.woman;
                    break;
                }
                case 2: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.yuri;
                    break;
                }
                case 3: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_0;
                    break;
                }
                case 4: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_1;
                    break;
                }
                case 5: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_2;
                    break;
                }
                case 6: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_3;
                    break;
                }
                case 7: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_4;
                    break;
                }
                case 8: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.com_5;
                    break;
                }
                case 9: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.elv;
                    break;
                }
                case 10: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_Y;
                    break;
                }
                case 11: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com0_isu;
                    break;
                }
                case 12: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com1_isu;
                    break;
                }
                case 13: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com2_isu;
                    break;
                }
                case 14: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com3_isu;
                    break;
                }
                case 15: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com4_isu;
                    break;
                }
                case 16: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.com5_isu;
                    break;
                }
                case 17: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_1;
                    break;
                }
                case 18: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_12;
                    break;
                }
                case 19: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_2;
                    break;
                }
                case 20: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_22;
                    break;
                }
                case 21: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_3;
                    break;
                }
                case 22: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_32;
                    break;
                }
                case 23: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_4;
                    break;
                }
                case 24: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_42;
                    break;
                }
                case 25: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_5;
                    break;
                }
                case 26: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_52;
                    break;
                }
                case 27: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_Y;
                    break;
                }
                case 28: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_Y2;
                    break;
                }
                case 29: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon;
                    break;
                }
                case 30: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon2;
                    break;
                }
                case 31: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon3;
                    break;
                }
                case 32: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Tobacco;
                    break;
                }
                case 33: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Stick;
                    break;
                }
                case 34: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_13;
                    break;
                }
                case 35: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_0;
                    break;
                }
                case 36: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.Mon_02;
                    break;
                }
                case 37: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.StickL;
                    break;
                }
            }
            Runtime.setRegister(0, (float) n8);
            System.println("gold_number-------------------(/[#0])");
            this.gold_get_iti();
        }
    }

    void __wait() {
        int n = 0;
        int n2 = 0;
        do {
            System.sleep(1);
            n = this.pad0.getButton();
            n2 = this.pad1.getButton();
        } while (this.ColorScreenSet_exec || this.camtool0_objtool1_flag == 2 || (n2 & 0x100) != 0 || (n2 & 0x10) == 0);
        System.sleep(17);
    }

    void cam_copy(Camera camera, Camera camera2) {
        camera.setTranslate(camera2.getTranslateX(), camera2.getTranslateY(), camera2.getTranslateZ());
        camera.setRotate(camera2.getRotateX(), camera2.getRotateY(), camera2.getRotateZ());
        camera.setFov(camera2.getFov());
    }

    void chr_look() {
        int n = 0;
        int n2 = 0;
        boolean bl = false;
        int n3 = 0;
        do {
            n2 = this.pad1.getButton();
            n = this.pad0.getButton();
            if ((n2 & 0x20) != 0) {
                Runtime.setRegister(0, (float) (++n3));
                System.println("parts_nomber---------------(/[#0])");
                System.sleep(3);
            }
            if ((n2 & 0x80) != 0) {
                if (--n3 < 0) {
                    n3 = 0;
                }
                Runtime.setRegister(0, (float) n3);
                System.println("parts_nomber---------------(/[#0])");
                System.sleep(3);
            }
            System.sleep(1);
            if ((n2 & 0x100) != 0 & (n & 8) != 0) {
                System.sleep(7);
                bl = true;
                System.println("-------------* chr_look_end *-----------");
            }
            if (this.kind_c0_u1 == 1) {
                this.gold_unit.setVisible(n3, false);
            } else {
                this.gold.setVisible(n3, false);
            }
            if ((n2 & 0x10) != 0) {
                ++n3;
            }
            System.sleep(1);
            if (this.kind_c0_u1 == 1) {
                this.gold_unit.setVisible(n3, true);
                continue;
            }
            this.gold.setVisible(n3, true);
        } while (!bl);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02001A_F");
        Runtime.setFlags(101, 1, 1);
        System.println("XEVEJNAME:SCE02001B");
        Runtime.jumpEvent(2011);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(2011);
    }

    int floatDowner(float f) {
        float f2 = this.Abs(f * 100000.0f);
        return (int) f2;
    }

    int floatUpper(float f) {
        return (int) f;
    }

    void gold_Visible_false() {
        if (this.kind_c0_u1 == 0) {
            this.gold.setVisible(false);
        }
        if (this.kind_c0_u1 == 1) {
            this.gold_unit.py += 100.0f;
            this.gold_unit.setTranslate();
            this.gold_unit.py -= 100.0f;
        }
    }

    void gold_Visible_true() {
        if (this.kind_c0_u1 == 0) {
            this.gold.setVisible(true);
        }
        if (this.kind_c0_u1 == 1) {
            this.gold_unit.setTranslate();
        }
    }

    void gold_get_iti() {
        if (this.kind_c0_u1 == 0) {
            this.rb_x = this.gold.px;
            this.rb_y = this.gold.py;
            this.rb_z = this.gold.pz;
            this.rb_rx = this.gold.rx;
            this.rb_ry = this.gold.ry;
            this.rb_rz = this.gold.rz;
        }
        if (this.kind_c0_u1 == 1) {
            this.rb_x = this.gold_unit.px;
            this.rb_y = this.gold_unit.py;
            this.rb_z = this.gold_unit.pz;
            this.rb_rx = this.gold_unit.rx;
            this.rb_ry = this.gold_unit.ry;
            this.rb_rz = this.gold_unit.rz;
        }
    }

    void gold_unit_set_rotate() {
        this.gold_unit.rx = this.rb_rx;
        this.gold_unit.ry = this.rb_ry;
        this.gold_unit.rz = this.rb_rz;
        this.gold_unit.setRotate();
    }

    void gold_unit_set_translate() {
        this.gold_unit.px = this.rb_x;
        this.gold_unit.py = this.rb_y;
        this.gold_unit.pz = this.rb_z;
        this.gold_unit.setTranslate();
    }

    void hand_cam0_shift(int n) {
        if (n == 2) {
            this.cam1.change();
        }
        if (n == 1) {
            System.sleep(1);
            this.cam_copy(this.cam0, this.cam1);
            this.cam0.change();
        }
        if (n == 0) {
            System.sleep(1);
            this.cam_copy(this.cam0, this.cam2);
            this.cam0.change();
        }
    }

    void iFACE(int n, Chr chr, int n2) {
        chr.mtn(n2, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
    }

    void iMSG(int n, Chr chr, int n2, int n3, String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        chr.mtn(n2, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
        if (this.msgflag) {
            System.println("MSGprint(with Face).");
            System.println(string);
            this.msg.print(string);
        }
        System.sleep(n3);
        chr.mtn(n2 + 1, 0, 120, 5, 8, 1.0f, false);
        chr.start(4, null);
        System.println("Face End.");
        if (n > n3) {
            System.sleep(n - n3);
        }
        this.msg.clear();
    }

    void iMSG(int n, Chr chr, int n2, String string) {
        this.iMSG(n, chr, n2, n - 10, string);
    }

    void init() {
        System.methodSignal(1);
        float[] fArray = new float[4];
        Runtime.setLocation(1107);
        Stage.renderCommand(0x400004);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.BaseCam = Camera.create(1);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.ziggy = new Ziggy();
        this.ziggy.init(0x1000006, 0.0f, 0.0f, 0.0f, 180.0f);
        this.ziggy.face = this.ziggy.getChild(0x1000000);
        this.ziggy.setTranslate(0.0f, -6.29f, 0.0f);
        this.ziggy.setRotate(0.0f, 0.0f, 0.0f);
        this.ziggy.setVisible(true);
        this.ziggy.renderCommand(534);
        this.ziggy.setMotionFlags(0x400000, false);
        this.woman = new Woman();
        this.woman.init(783, 0.0f, 0.0f, 0.0f, 180.0f);
        this.woman.setTranslate(-0.8f, -6.29f, 0.0f);
        this.woman.setRotate(0.0f, 0.0f, 0.0f);
        this.woman.setShadow(0, 0);
        this.woman.setLightMode(1);
        this.woman.light.setColor(0, 0.23f, 0.28f, 0.28f);
        this.woman.light.setColor(1, 0.83f, 1.0f, 0.37f);
        this.woman.light.setDirection2(1, -0.026f, 0.16f, 0.987f);
        this.woman.light.setColor(2, 0.21f, 0.35f, 0.21f);
        this.woman.light.setDirection2(2, 0.18f, 0.981f, -0.074f);
        this.woman.light.setColor(3, 0.39f, 0.51f, 0.51f);
        this.woman.light.setDirection2(3, -0.093f, -0.93f, 0.357f);
        fArray[0] = 0.55f;
        fArray[1] = 60.5f;
        fArray[2] = 1.0f;
        fArray[3] = 0.93f;
        this.woman.setFilter(2);
        this.woman.setFilterParam(fArray);
        this.woman.setVisible(true);
        this.woman.renderCommand(534);
        this.woman.setMotionFlags(0x400000, false);
        this.yuri = new Yuri();
        this.yuri.init(0x100011D, 0.0f, 0.0f, 0.0f, 180.0f);
        this.yuri.face = this.yuri.getChild(0x1000000);
        this.yuri.setTranslate(-2.82f, 0.0f, -2.82f);
        this.yuri.setRotate(0.0f, 45.0f, 0.0f);
        this.yuri.setShadow(0, 0);
        this.yuri.setVisible(true);
        this.yuri.renderCommand(534);
        this.yuri.setMotionFlags(0x400000, false);
        this.com_2 = new Com_2();
        this.com_2.init(0x1000303, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_2.face = this.com_2.getChild(0x1000000);
        this.com_2.setTranslate(2.82f, 0.0f, -2.82f);
        this.com_2.setRotate(0.0f, 315.0f, 0.0f);
        this.com_2.setShadow(0, 0);
        this.com_2.setVisible(true);
        this.com_2.renderCommand(534);
        this.com_2.setMotionFlags(0x400000, false);
        this.com_4 = new Com_4();
        this.com_4.init(16777989, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_4.face = this.com_4.getChild(0x1000000);
        this.com_4.setTranslate(2.82f, 0.0f, 2.82f);
        this.com_4.setRotate(0.0f, 225.0f, 0.0f);
        this.com_4.setShadow(0, 0);
        this.com_4.setVisible(true);
        this.com_4.renderCommand(534);
        this.com_4.setMotionFlags(0x400000, false);
        this.com_5 = new Com_5();
        this.com_5.init(16777990, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_5.face = this.com_5.getChild(0x1000000);
        this.com_5.setTranslate(-2.82f, 0.0f, 2.82f);
        this.com_5.setRotate(0.0f, 135.0f, 0.0f);
        this.com_5.setShadow(0, 0);
        this.com_5.setVisible(true);
        this.com_5.renderCommand(534);
        this.com_5.setMotionFlags(0x400000, false);
        this.com_0 = new Com_0();
        this.com_0.init(0x1000301, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_0.face = this.com_0.getChild(0x1000000);
        this.com_0.setTranslate(0.0f, 0.0f, 4.0f);
        this.com_0.setRotate(0.0f, 180.0f, 0.0f);
        this.com_0.setShadow(0, 0);
        this.com_0.setVisible(true);
        this.com_0.renderCommand(534);
        this.com_0.setMotionFlags(0x400000, false);
        this.com_1 = new Com_1();
        this.com_1.init(16777986, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_1.face = this.com_1.getChild(0x1000000);
        this.com_1.setTranslate(-4.0f, 0.0f, 0.0f);
        this.com_1.setRotate(0.0f, 90.0f, 0.0f);
        this.com_1.setShadow(0, 0);
        this.com_1.setVisible(true);
        this.com_1.renderCommand(534);
        this.com_1.setMotionFlags(0x400000, false);
        this.com_3 = new Com_3();
        this.com_3.init(16777988, 0.0f, 0.0f, 0.0f, 180.0f);
        this.com_3.face = this.com_3.getChild(0x1000000);
        this.com_3.setTranslate(4.0f, 0.0f, 0.0f);
        this.com_3.setRotate(0.0f, 270.0f, 0.0f);
        this.com_3.setShadow(0, 0);
        this.com_3.setVisible(true);
        this.com_3.renderCommand(534);
        this.com_3.setMotionFlags(0x400000, false);
        this.elv = new Mapunits();
        this.elv.mapUnit(66);
        this.elv.start(4, null);
        this.elv.setTranslate(0.0f, -6.19f, 0.0f);
        this.elv.setRotate(0.0f, 0.0f, 0.0f);
        this.hat1 = new Mapunits();
        this.hat1.mapUnit(0);
        this.hat1.start(4, null);
        this.hat1.setTranslate(-0.96f, -0.15f, -0.56f);
        this.hat1.setRotate(0.0f, 150.0f, 0.0f);
        this.hat2 = new Mapunits();
        this.hat2.mapUnit(1);
        this.hat2.start(4, null);
        this.hat2.setTranslate(0.96f, -0.15f, -0.56f);
        this.hat2.setRotate(0.0f, 30.0f, 0.0f);
        this.hat3 = new Mapunits();
        this.hat3.mapUnit(2);
        this.hat3.start(4, null);
        this.hat3.setTranslate(0.0f, -0.15f, 1.11f);
        this.hat3.setRotate(0.0f, -90.0f, 0.0f);
        this.hat4 = new Mapunits();
        this.hat4.mapUnit(3);
        this.hat4.start(4, null);
        this.hat4.setTranslate(0.0f, -0.15f, -0.83f);
        this.hat4.setRotate(0.0f, 90.0f, 0.0f);
        this.hat5 = new Mapunits();
        this.hat5.mapUnit(4);
        this.hat5.start(4, null);
        this.hat5.setTranslate(-0.72f, -0.15f, 0.42f);
        this.hat5.setRotate(0.0f, -150.0f, 0.0f);
        this.hat6 = new Mapunits();
        this.hat6.mapUnit(5);
        this.hat6.start(4, null);
        this.hat6.setTranslate(0.72f, -0.15f, 0.42f);
        this.hat6.setRotate(0.0f, -30.0f, 0.0f);
        this.yuri_isu = new Mapunits();
        this.yuri_isu.mapUnit(59);
        this.yuri_isu.start(4, null);
        this.yuri_isu.setTranslate(-2.7f, 0.0f, -2.7f);
        this.yuri_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com1_isu = new Mapunits();
        this.com1_isu.mapUnit(60);
        this.com1_isu.start(4, null);
        this.com1_isu.setTranslate(-3.8f, 0.0f, 0.0f);
        this.com1_isu.setRotate(0.0f, 356.0f, 0.0f);
        this.com5_isu = new Mapunits();
        this.com5_isu.mapUnit(61);
        this.com5_isu.start(4, null);
        this.com5_isu.setTranslate(-2.7f, 0.0f, 2.7f);
        this.com5_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com0_isu = new Mapunits();
        this.com0_isu.mapUnit(62);
        this.com0_isu.start(4, null);
        this.com0_isu.setTranslate(0.0f, 0.0f, 3.8f);
        this.com0_isu.setRotate(0.0f, 356.0f, 0.0f);
        this.com4_isu = new Mapunits();
        this.com4_isu.mapUnit(63);
        this.com4_isu.start(4, null);
        this.com4_isu.setTranslate(2.7f, 0.0f, 2.7f);
        this.com4_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com3_isu = new Mapunits();
        this.com3_isu.mapUnit(64);
        this.com3_isu.start(4, null);
        this.com3_isu.setTranslate(3.8f, 0.0f, 0.0f);
        this.com3_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com2_isu = new Mapunits();
        this.com2_isu.mapUnit(65);
        this.com2_isu.start(4, null);
        this.com2_isu.setTranslate(2.7f, 0.0f, -2.7f);
        this.com2_isu.setRotate(0.0f, 4.0f, 0.0f);
        this.Mon = new Monitor();
        this.Mon.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon.setArgs(1, 20067, 0, 0, 0);
        this.Mon.setArgs(2, 100, 0, 0, -1);
        this.Mon.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon.setTranslate(0.0f, 3.37f, -8.38f);
        this.Mon.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon2 = new Monitor();
        this.Mon2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon2.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon2.setArgs(1, 22026, 0, 0, 0);
        this.Mon2.setArgs(2, 82, 0, 0, -1);
        this.Mon2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon2.setTranslate(0.01f, 3.37f, -8.38f);
        this.Mon2.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon3 = new Monitor();
        this.Mon3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon3.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon3.setArgs(1, 22027, 0, 0, 0);
        this.Mon3.setArgs(2, 0, 0, 0, -1);
        this.Mon3.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon3.setTranslate(0.0f, 3.36f, -8.38f);
        this.Mon3.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon4 = new Monitor();
        this.Mon4.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon4.setArgs(0, 0.0f, 0.0f, 8.24f, 3.16f);
        this.Mon4.setArgs(1, 20066, 0, 0, 0);
        this.Mon4.setArgs(2, 82, 0, 0, -1);
        this.Mon4.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon4.setTranslate(0.0f, 3.37f, -8.38f);
        this.Mon4.setRotate(0.0f, 0.08f, 0.0f);
        this.Mon_2 = new Monitor();
        this.Mon_2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_2.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_2.setArgs(1, 20045, 0, 0, 0);
        this.Mon_2.setArgs(2, 96, 0, 0, -1);
        this.Mon_2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_2.setTranslate(2.4099998f, 0.9f, -2.37f);
        this.Mon_2.setRotate(0.0f, 315.0f, 0.0f);
        this.Mon_2.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_22 = new Monitor();
        this.Mon_22.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_22.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_22.setArgs(1, 20065, 0, 0, 0);
        this.Mon_22.setArgs(2, 96, 0, 0, -1);
        this.Mon_22.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_22.setTranslate(this.Mon_2.px, this.Mon_2.py, this.Mon_2.pz - 0.01f);
        this.Mon_22.setRotate(this.Mon_2.rx, this.Mon_2.ry, this.Mon_2.rz);
        this.Mon_22.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_4 = new Monitor();
        this.Mon_4.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_4.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_4.setArgs(1, 20045, 0, 0, 0);
        this.Mon_4.setArgs(2, 96, 0, 0, -1);
        this.Mon_4.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_4.setTranslate(2.32f, 0.9f, 2.37f);
        this.Mon_4.setRotate(0.0f, 225.0f, 0.0f);
        this.Mon_4.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_5 = new Monitor();
        this.Mon_5.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_5.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_5.setArgs(1, 20045, 0, 0, 0);
        this.Mon_5.setArgs(2, 96, 0, 0, -1);
        this.Mon_5.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_5.setTranslate(-2.4199998f, 0.9f, 2.37f);
        this.Mon_5.setRotate(0.0f, 135.0f, 0.0f);
        this.Mon_5.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_0 = new Monitor();
        this.Mon_0.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_0.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_0.setArgs(1, 20045, 0, 0, 0);
        this.Mon_0.setArgs(2, 96, 0, 0, -1);
        this.Mon_0.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_0.setTranslate(0.0f, 0.9f, 3.55f);
        this.Mon_0.setRotate(0.0f, 180.0f, 0.0f);
        this.Mon_0.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_02 = new Monitor();
        this.Mon_02.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_02.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_02.setArgs(1, 20065, 0, 0, 0);
        this.Mon_02.setArgs(2, 96, 0, 0, -1);
        this.Mon_02.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_02.setTranslate(this.Mon_0.px + 0.01f, this.Mon_0.py, this.Mon_0.pz);
        this.Mon_02.setRotate(this.Mon_0.rx, this.Mon_0.ry, this.Mon_0.rz);
        this.Mon_02.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_1 = new Monitor();
        this.Mon_1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_1.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_1.setArgs(1, 20045, 0, 0, 0);
        this.Mon_1.setArgs(2, 96, 0, 0, -1);
        this.Mon_1.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_1.setTranslate(-3.4f, 0.9f, 0.0f);
        this.Mon_1.setRotate(0.0f, 90.0f, 0.0f);
        this.Mon_1.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_12 = new Monitor();
        this.Mon_12.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_12.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_12.setArgs(1, 20065, 0, 0, 0);
        this.Mon_12.setArgs(2, 96, 0, 0, -1);
        this.Mon_12.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_12.setTranslate(this.Mon_1.px, this.Mon_1.py, this.Mon_1.pz);
        this.Mon_12.setRotate(this.Mon_1.rx, this.Mon_1.ry, this.Mon_1.rz);
        this.Mon_12.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_3 = new Monitor();
        this.Mon_3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_3.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_3.setArgs(1, 20045, 0, 0, 0);
        this.Mon_3.setArgs(2, 96, 0, 0, -1);
        this.Mon_3.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_3.setTranslate(3.4f, 0.9f, 0.0f);
        this.Mon_3.setRotate(0.0f, 270.0f, 0.0f);
        this.Mon_3.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_32 = new Monitor();
        this.Mon_32.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_32.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_32.setArgs(1, 20065, 0, 0, 0);
        this.Mon_32.setArgs(2, 96, 0, 0, -1);
        this.Mon_32.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_32.setTranslate(this.Mon_3.px - 0.01f, this.Mon_3.py, this.Mon_3.pz);
        this.Mon_32.setRotate(this.Mon_3.rx, this.Mon_3.ry, this.Mon_3.rz);
        this.Mon_32.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_Y = new Monitor();
        this.Mon_Y.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_Y.setArgs(1, 20045, 0, 0, 0);
        this.Mon_Y.setArgs(2, 100, 0, 0, -1);
        this.Mon_Y.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y.setTranslate(-2.54f, 0.9f, -2.55f);
        this.Mon_Y.setRotate(0.0f, 45.0f, 0.0f);
        this.Mon_Y.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_Y2 = new Monitor();
        this.Mon_Y2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y2.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_Y2.setArgs(1, 20065, 0, 0, 0);
        this.Mon_Y2.setArgs(2, 96, 0, 0, -1);
        this.Mon_Y2.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_Y2.setTranslate(this.Mon_Y.px, this.Mon_Y.py, this.Mon_Y.pz);
        this.Mon_Y2.setRotate(this.Mon_Y.rx, this.Mon_Y.ry, this.Mon_Y.rz);
        this.Mon_Y2.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_42 = new Monitor();
        this.Mon_42.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_42.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_42.setArgs(1, 20065, 0, 0, 0);
        this.Mon_42.setArgs(2, 96, 0, 0, -1);
        this.Mon_42.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_42.setTranslate(this.Mon_4.px, this.Mon_4.py, this.Mon_4.pz);
        this.Mon_42.setRotate(this.Mon_4.rx, this.Mon_4.ry, this.Mon_4.rz);
        this.Mon_42.setScale(0.31f, 0.27f, 0.37f);
        this.Mon_52 = new Monitor();
        this.Mon_52.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_52.setArgs(0, 0.0f, 0.0f, 0.8f, 0.7f);
        this.Mon_52.setArgs(1, 20065, 0, 0, 0);
        this.Mon_52.setArgs(2, 96, 0, 0, -1);
        this.Mon_52.setArgs(3, 0.2f, 0.0f, 0.0f, 0.0f);
        this.Mon_52.setTranslate(this.Mon_5.px, this.Mon_5.py, this.Mon_5.pz);
        this.Mon_52.setRotate(this.Mon_5.rx, this.Mon_5.ry, this.Mon_5.rz);
        this.Mon_52.setScale(0.31f, 0.27f, 0.37f);
        this.Tobacco = new Unit();
        this.Tobacco.init(24614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Tobacco.setTranslate(0.14f, -0.01f, 0.02f);
        this.Tobacco.setRotate(178.33f, -39.67f, 84.33f);
        this.Tobacco.setScale(8.3f, 7.5f, 8.5f);
        this.Tobacco.setVisible(true);
        this.Tobacco.setParent(this.com_3, 72);
        this.Stick = new Unit();
        this.Stick.init(24634, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Stick.setTranslate(0.09f, 0.01f, 0.03f);
        this.Stick.setRotate(269.16f, -30.87f, 265.96f);
        this.Stick.setScale(1.0f, 1.0f, 1.0f);
        this.Stick.setVisible(true);
        this.Stick.setParent(this.com_4, 72);
        this.StickL = new Unit();
        this.StickL.init(24634, 0.0f, 0.0f, 0.0f, 0.0f);
        this.StickL.setTranslate(0.1f, -0.01f, -0.06f);
        this.StickL.setRotate(100.66f, 36.64f, 273.96f);
        this.StickL.setScale(1.0f, 1.0f, 1.0f);
        this.StickL.setVisible(true);
        this.StickL.setParent(this.com_4, 60);
        this.Mon_13 = new Unit();
        this.Mon_13.init(24579, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Mon_13.setTranslate(0.13f, -0.01f, -0.07f);
        this.Mon_13.setRotate(-20.53f, 39.1f, 95.6f);
        this.Mon_13.setScale(6.0f, 6.5f, 1.0f);
        this.Mon_13.setVisible(true);
        this.Mon_13.setParent(this.com_1, 60);
        this.eft0 = new Effect(1534, 1000.0f, 1000.0f, 1000.0f, 0.0f);
        this.eft0.setScale(0.0f, 0.0f, 0.0f);
        this.eft0.disp(false);
        this.eft0.setForceLoop(false);
        this.eft1 = new Effect(1533, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1.setScale(1.0f, 1.0f, 1.0f);
        this.eft1.setTranslate(0.02f, -0.05f, 0.01f);
        this.eft1.setCaster(this.com_3);
        this.eft1.disp(true);
        this.eft1.setMotion(true);
    }

    void initialize() {
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void map_look() {
        int n = 0;
        int n2 = 0;
        boolean bl = false;
        int n3 = 0;
        do {
            n2 = this.pad1.getButton();
            n = this.pad0.getButton();
            if ((n2 & 0x20) != 0) {
                Runtime.setRegister(0, (float) (++n3));
                System.println("map_nomber---------------(/[#0])");
                System.sleep(3);
            }
            if ((n2 & 0x80) != 0) {
                if (--n3 < 0) {
                    n3 = 0;
                }
                Runtime.setRegister(0, (float) n3);
                System.println("map_nomber---------------(/[#0])");
                System.sleep(3);
            }
            System.sleep(1);
            if ((n2 & 0x100) != 0 & (n & 0x10) != 0) {
                System.sleep(7);
                bl = true;
                System.println("-------------* map_look_end *-----------");
            }
            Stage.setVisible(n3, false);
            if ((n2 & 0x10) != 0) {
                ++n3;
            }
            System.sleep(1);
            Stage.setVisible(n3, true);
        } while (!bl);
    }

    void msg_clear_thread() {
        this.msg_clear_exec = true;
        System.sleep(this.msg_clearwait);
        this.msg.clear();
        this.msg_clear_exec = false;
    }

    void play() {
        this.loadarc(this.yuri.face, "FLSyuri.fpk");
        this.loadarc(this.com_2.face, "FLScommi2.fpk");
        this.loadarc(this.com_4.face, "FLScommi4.fpk");
        this.loadarc(this.com_5.face, "FLScommi5.fpk");
        this.loadarc(this.com_0.face, "FLScommi.fpk");
        this.loadarc(this.com_1.face, "FLScommi1.fpk");
        this.loadarc(this.com_3.face, "FLScommi3.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        Stage.setVisible(26, false);
        Stage.setVisible(84, false);
        this.BaseCam.setTranslate(-3.59f, 1.55f, 2.68f);
        this.BaseCam.setRotate(-9.92f, 309.61f, 0.0f);
        this.BaseCam.setFov(25.0f);
        this.BaseCam.change();
        this.cam0.setClipRange(0.01f, 3000.0f);
        this.CaptureTool();
        this.Timechk();
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
        boolean bl = false;
        int n = 0;
        this.Mon_Y.setTranslate(-2.54f, 0.9f, -2.55f);
        this.Mon_Y.setRotate(0.0f, 45.0f, 0.0f);
        this.Mon_Y2.setTranslate(-2.54f, 0.9f, -2.55f);
        this.Mon_Y2.setRotate(0.0f, 45.0f, 0.0f);
        this.Mon_2.signal(1);
        this.Mon_4.signal(1);
        this.Mon_5.signal(1);
        this.Mon_0.signal(1);
        this.Mon_1.signal(1);
        this.Mon_3.signal(1);
        this.Mon_Y.signal(1);
        this.Mon.signal(1);
        this.Mon2.signal(0);
        this.Mon3.signal(0);
        this.elv.setTranslate(0.0f, -6.19f, 0.0f);
        this.elv.setRotate(0.0f, 0.0f, 0.0f);
        this.yuri_isu.setTranslate(-2.7f, 0.0f, -2.7f);
        this.yuri_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com1_isu.setTranslate(-3.8f, 0.0f, 0.0f);
        this.com1_isu.setRotate(0.0f, 356.0f, 0.0f);
        this.com5_isu.setTranslate(-2.7f, 0.0f, 2.7f);
        this.com5_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com0_isu.setTranslate(0.0f, 0.0f, 3.8f);
        this.com0_isu.setRotate(0.0f, 356.0f, 0.0f);
        this.com4_isu.setTranslate(2.7f, 0.0f, 2.7f);
        this.com4_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com3_isu.setTranslate(3.8f, 0.0f, 0.0f);
        this.com3_isu.setRotate(0.0f, 0.0f, 0.0f);
        this.com2_isu.setTranslate(2.7f, 1.38f, -2.7f);
        this.com2_isu.setRotate(0.0f, 4.0f, 0.0f);
        this.hat1.setTranslate(-0.96f, -0.15f, -0.56f);
        this.hat2.setTranslate(0.96f, -0.15f, -0.56f);
        this.hat3.setTranslate(0.0f, -0.15f, 1.11f);
        this.hat4.setTranslate(0.0f, -0.15f, -0.83f);
        this.hat5.setTranslate(-0.72f, -0.15f, 0.42f);
        this.hat6.setTranslate(0.72f, -0.15f, 0.42f);
        this.StickL.setTranslate(0.1f, -0.01f, -0.06f);
        this.StickL.setRotate(100.66f, 36.64f, 273.96f);
        this.yuri.setTranslate(-2.82f, 0.0f, -2.82f);
        this.yuri.setRotate(0.0f, 45.0f, 0.0f);
        Runtime.mpeg2AfterCrossFade(90);
        Runtime.mpeg2("2001A_1");
        this.ziggy.setShadow(7, 32);
        Sound.streamPlay(1290052, 48000);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.151f, 0.869f, 0.472f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.556f, -0.001f, -0.831f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.786f, -0.001f, -0.619f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.woman.setVisible(true);
        this.Mon_13.setVisible(true);
        this.Stick.setVisible(true);
        this.StickL.setVisible(false);
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.ziggy.start(1, "off");
        this.woman.start(1, "off");
        this.yuri.start(1, "sit");
        this.com_2.start(1, "sit");
        this.com_4.start(1, "sit");
        this.com_5.start(1, "sit");
        this.com_0.start(1, "sit");
        this.com_1.start(1, "sit");
        this.com_3.start(1, "sit");
        this.com4_isu.start(1, "com4_isu_reset");
        this.camerawork.cut1();
        System.sleep(135);
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(true);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.Mon.signal(1);
        this.com_4.start(1, "act2");
        this.com_0.start(1, "act2");
        this.camerawork.cut2();
        System.sleep(135);
        this.com_0.start(1, "act3");
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.camerawork.cut3();
        System.sleep(30);
        this.com_0.look_speed(0.12f);
        this.com_0.look_char(this.com_2);
        this._MSG(91, "委員０", "So, what can you tell\nus about this man?");
        if (!this.snd_chk) {
            Sound.streamPlay(201001);
        }
        this.FACE(62, this.com_0.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201002);
        }
        this.FACE(29, this.com_0.face, 1);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.151f, 0.869f, 0.472f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.974f, -0.0f, -0.228f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.683f, -0.688f, 0.247f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(true);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.com_0.look_speed(0.22f);
        this.com_2.start(1, "act4");
        this.camerawork.cut4();
        this._MSG(145, "委員２", "He was a Special Forces agent\nwith the Federation Police.\nA counterterrorism specialist.");
        if (!this.snd_chk) {
            Sound.streamPlay(201003);
        }
        this.FACE(88, this.com_2.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201004, 44100);
        }
        this.FACE(57, this.com_2.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201005);
        }
        this.iMSG(83, this.com_2.face, 1, 83, "Of course, that was over\na hundred years ago.");
        this.com_0.look_default();
        this.com_0.start(1, "act5");
        this.com_2.start(1, "act5");
        this.Mon_0.start(1, "f0");
        this.Mon_02.start(1, "f1");
        this.Mon_2.start(1, "f0");
        this.Mon_22.start(1, "f1");
        this.Mon_3.start(1, "f0");
        this.Mon_32.start(1, "f1");
        this.Mon_4.start(1, "f0");
        this.Mon_42.start(1, "f1");
        this.Mon_5.start(1, "f0");
        this.Mon_52.start(1, "f1");
        this.Mon_1.start(1, "f0");
        this.Mon_12.start(1, "f1");
        this.Mon_Y.start(1, "f0");
        this.Mon_Y2.start(1, "f1");
        this.camerawork.cut5();
        if (!this.snd_chk) {
            Sound.streamPlay(201006);
        }
        this.iMSG(102, this.com_2.face, 1, 101, "Now he's a cyborg working\nfreelance missions.");
        if (!this.snd_chk) {
            Sound.streamPlay(201007);
        }
        this.MSG(61, this.com_0.face, 1, 60, "A cyborg? How anachronistic.");
        if (!this.snd_chk) {
            Sound.streamPlay(201008);
        }
        this.MSG(58, this.com_5.face, 1, 58, "He's a relic from the days");
        System.println("CUT5 END");
        System.println("CUT6 START");
        if (this.syaku) {
            this.com_5.setVisible(true);
        }
        this.com_5.start(1, "act6");
        this.camerawork.cut6();
        if (!this.snd_chk) {
            Sound.streamPlay(201908);
        }
        this.iMSG(68, this.com_5.face, 1, 68, "when they reanimated the dead.");
        System.println("CUT6 END");
        System.println("CUT7 START");
        this.com_5.start(1, "act7");
        if (!this.snd_chk) {
            Sound.streamPlay(201009);
        }
        this.iMSG(162, this.com_5.face, 1, 99, "They didn't have disposable\nRealians like we do now.");
        System.println("CUT7 END");
        System.println("CUT8 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.11f, 0.993f, 0.035f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.585f, -0.593f, -0.553f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.919f, -0.266f, -0.292f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_1.start(1, "act8");
        this.com_5.start(1, "act8");
        this.camerawork.cut8();
        System.sleep(26);
        if (!this.snd_chk) {
            Sound.streamPlay(201010);
        }
        this.MSG(132, this.com_1.face, 1, 71, "Well, he's certainly got\nquite a resum#2b...");
        System.println("CUT8 END");
        System.println("CUT9 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.225f, 0.912f, 0.343f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.972f, -0.0f, -0.234f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.278f, -0.815f, 0.508f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_2.setVisible(true);
        }
        this.com_2.start(1, "act9");
        this.com_3.start(1, "act9");
        this.camerawork.cut9();
        if (!this.snd_chk) {
            Sound.streamPlay(201011);
        }
        this.MSG(150, this.com_2.face, 1, 125, "Rumor has it he deliberately seeks out\nmissions with low odds of success.");
        System.println("CUT9 END");
        System.println("CUT10 START");
        if (this.syaku) {
            this.com_3.setVisible(true);
        }
        this.com_3.start(1, "act10");
        this.elv.start(1, "kemuri");
        this.camerawork.cut10();
        System.sleep(20);
        System.sleep(51);
        this._MSG(103, "委員３", "Is he insane?\nOr just fascinated with death?");
        if (!this.snd_chk) {
            Sound.streamPlay(201012);
        }
        this.iFACE(36, this.com_3.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201013);
        }
        this.iFACE(67, this.com_3.face, 1);
        System.println("CUT10 END");
        System.println("CUT11 START");
        this.com_2.start(1, "act11");
        this.com_3.start(1, "act11");
        this.com_4.start(1, "act12");
        this.camerawork.cut11();
        if (!this.snd_chk) {
            Sound.streamPlay(201014);
        }
        this.MSG(85, this.com_2.face, 1, 76, "Well, he's been given a clean bill\nof mental health.");
        if (!this.snd_chk) {
            Sound.streamPlay(201015);
        }
        this.iMSG(85, this.com_2.face, 1, 57, "It doesn't look like he does it\nfor the killing.");
        this._MSG(83, "委員４", "Hmph. Advances in science");
        if (!this.snd_chk) {
            Sound.streamPlay(201016);
        }
        this.FACE(23, this.com_4.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201017);
        }
        this.FACE(60, this.com_4.face, 1);
        System.println("CUT11 END");
        System.println("CUT12 START");
        if (!this.snd_chk) {
            Sound.streamPlay(201917);
        }
        this._MSG(162, "委員４", "have placed thousands of drugs\non our shelves, and yet");
        this.FACE(30, this.com_4.face, 1);
        this.FACE(110, this.com_4.face, 1);
        System.println("CUT12 END");
        System.println("CUT13 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.775f, 0.607f, 0.176f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.078f, -0.947f, -0.313f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.963f, -0.0f, -0.271f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_2.look_default();
        this.com_3.look_default();
        this.com_4.start(1, "act12");
        this.camerawork.cut13();
        if (!this.snd_chk) {
            Sound.streamPlay(201018);
        }
        this.MSG(150, this.com_4.face, 1, 114, "we still let psychiatrists wield\ntheir influence over us.\nThey use these \"assessments\"");
        System.println("CUT13 END");
        System.println("CUT14 START");
        this.com_4.start(1, "act14");
        if (!this.snd_chk) {
            Sound.streamPlay(201019);
        }
        this.MSG(156, this.com_4.face, 1, 120, "to validate their existence.\nI don't believe them one bit.");
        System.sleep(15);
        System.println("CUT14 END");
        System.println("CUT15 START");
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.868f, 0.292f, -0.403f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.101f, -0.001f, -0.995f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.846f, -0.0f, -0.532f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_0.start(1, "act15");
        this.camerawork.cut15();
        System.sleep(23);
        this._MSG(111, "委員０", "Well, he's got an extremely high\nsuccess rate...I think he'll be fine.");
        if (!this.snd_chk) {
            Sound.streamPlay(201020);
        }
        this.iFACE(73, this.com_0.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201021);
        }
        this.iFACE(38, this.com_0.face, 1);
        System.sleep(60);
        if (this.snd_chk2) {
            this._MSG(30, "ポン", "”ポン”――インターホン――");
        }
        System.sleep(30);
        System.println("CUT15 END");
        System.println("CUT16 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.393f, 0.872f, -0.291f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.146f, -0.454f, 0.879f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.745f, -0.283f, 0.604f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_0.start(1, "act15");
        this.ziggy.start(1, "act16");
        this.woman.start(1, "act16");
        this.camerawork.cut16();
        if (!this.snd_chk) {
            Sound.streamPlay(201022);
        }
        this.MSG(45, "女", "Sir, he's here.");
        System.sleep(15);
        if (!this.snd_chk) {
            Sound.streamPlay(201023);
        }
        this.MSG(57, this.com_0.face, 1, 23, "Show him in.");
        System.sleep(18);
        System.println("CUT16 END");
        System.println("CUT17 START");
        this.ziggy.start(1, "up");
        this.woman.start(1, "up");
        this.elv.start(1, "el_up");
        this.hat1.start(1, "tume1_o");
        this.hat2.start(1, "tume2_o");
        this.hat3.start(1, "tume3_o");
        this.hat4.start(1, "door1_o");
        this.hat5.start(1, "door2_o");
        this.hat6.start(1, "door3_o");
        this.camerawork.cut17();
        System.sleep(17);
        System.sleep(85);
        System.println("CUT17 END");
        System.println("CUT18 START");
        this.camerawork.cut18();
        System.sleep(45);
        System.println("CUT18 END");
        System.println("CUT19 START");
        this.camerawork.cut19();
        System.sleep(45);
        System.println("CUT19 END");
        System.println("CUT20-21 START");
        this.ziggy.start(1, "act20");
        this.woman.start(1, "act20");
        this.com_0.start(1, "sit");
        this.camerawork.cut20();
        System.sleep(7);
        System.sleep(97);
        this.camerawork.cut21();
        if (!this.snd_chk) {
            Sound.streamPlay(201024);
        }
        this.MSG(50, this.com_0.face, 1, 26, "That'll be all.");
        if (!this.snd_chk) {
            Sound.streamPlay(201025);
        }
        this.MSG(72, "レアリエン女", "Yes, sir.");
        System.println("CUT20-21 END");
        System.println("CUT22 START");
        this.ziggy.start(1, "act22");
        this.woman.start(1, "act22");
        this.camerawork.cut22();
        System.sleep(90);
        System.println("CUT22 END");
        System.println("CUT23 START");
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(true);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_2.look_char(this.ziggy);
        this.com_1.start(1, "act23");
        this.ziggy.start(1, "act23");
        this.camerawork.cut23();
        System.sleep(5);
        if (!this.snd_chk) {
            Sound.streamPlay(201026);
        }
        this.MSG(77, this.com_1.face, 1, 40, "Ziggurat 8, isn't it?");
        System.println("CUT23 END");
        System.println("CUT24 START");
        if (!this.snd_chk) {
            Sound.streamPlay(201027);
        }
        this.iMSG(134, this.com_1.face, 1, 134, "Upon your death in T.C. 4667,\nyou donated your body and\nchose to become a product");
        if (!this.snd_chk) {
            Sound.streamPlay(201028);
        }
        this.iMSG(41, this.com_1.face, 1, 41, "of Ziggurat Industries.");
        if (!this.snd_chk) {
            Sound.streamPlay(201029);
        }
        this.MSG(70, this.ziggy.face, 1, 32, "That's correct.");
        System.println("CUT24 END");
        System.println("CUT25 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.16f, 0.987f, 0.019f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.055f, -0.859f, -0.509f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.41f, -0.887f, -0.213f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_5.look_char(this.ziggy);
        this.com_0.start(1, "act25");
        this.com_3.start(1, "act25");
        this.camerawork.cut25();
        if (!this.snd_chk) {
            Sound.streamPlay(201030);
        }
        this.MSG(82, this.com_0.face, 1, 64, "Have you heard about the mission?");
        if (!this.snd_chk) {
            Sound.streamPlay(201031);
        }
        this.MSG(45, this.ziggy.face, 1, 15, "No.");
        System.println("CUT25 END");
        System.println("CUT26 START");
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(true);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_3.start(1, "act26");
        this.elv.start(1, "kemuri");
        this.camerawork.cut26();
        if (!this.snd_chk) {
            Sound.streamPlay(201032);
        }
        this.MSG(197, this.com_3.face, 1, 196, "Recently, we've received information\nthat a certain armed group has shown\nsigns of resurgence.");
        if (!this.snd_chk) {
            Sound.streamPlay(201033);
        }
        this.iMSG(126, this.com_3.face, 1, 125, "This same group was responsible for the events that took place 14 years ago.");
        System.println("CUT26 END");
        System.println("CUT27-28 START");
        this.com_2.look_default();
        this.camera_thread = Thread.create(this.camerawork, "cut27");
        this.camera_thread.start();
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(true);
            this.com_5.setVisible(true);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_4.start(1, "act27");
        this.com4_isu.start(1, "com4_isu_r1");
        this.ziggy.start(1, "act27");
        if (!this.snd_chk) {
            Sound.streamPlay(201034);
        }
        this.MSG(49, this.ziggy.face, 1, 43, "The U-TIC Organization...");
        this._MSG(137, "委員４", "That's right.\nThe information we've received from\nour independent sources");
        if (!this.snd_chk) {
            Sound.streamPlay(201035);
        }
        this.FACE(20, this.com_4.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201036);
        }
        this.FACE(117, this.com_4.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201936);
        }
        this.iMSG(135, this.com_4.face, 1, 128, "has helped to identify the location of the U-TIC Organization's hideout.");
        System.println("CUT27-28 END");
        System.println("CUT29 START");
        this.ziggy.start(1, "act29");
        this.Mon.start(1, "act29");
        this.yuri_isu.start(1, "yuri_isu_r1");
        this.com2_isu.start(1, "com2_isu_r1");
        this.com5_isu.start(1, "com5_isu_r1");
        this.com1_isu.start(1, "com1_isu_r1");
        this.com3_isu.start(1, "com3_isu_r1");
        this.Mon2.signal(1);
        this.Mon3.signal(1);
        this.Mon.signal(0);
        System.sleep(32);
        System.println("CUT29 END");
        System.println("CUT30 START");
        this.camerawork.cut30();
        System.sleep(50);
        if (!this.snd_chk) {
            Sound.streamPlay(201037);
        }
        this.MSG(62, this.com_1.face, 1, 121, "This structure was originally a shrine\nfor some long-lost ancient religion.");
        if (!this.snd_chk) {
            Sound.streamPlay(201038);
        }
        this.iMSG(150, this.com_1.face, 1, 95, "It was eventually abandoned\nand remained uninhabited for centuries.");
        System.sleep(10);
        System.println("CUT30 END");
        System.println("CUT31 START");
        this.camerawork.cut31();
        System.sleep(5);
        if (!this.snd_chk) {
            Sound.streamPlay(201039);
        }
        this.MSG(150, this.ziggy.face, 1, 112, "So you want me to infiltrate\nand collect information on the group?");
        this.yuri_isu.start(1, "yuri_isu_r2");
        this.com2_isu.start(1, "com2_isu_r2");
        this.com4_isu.start(1, "com4_isu_r2");
        this.com5_isu.start(1, "com5_isu_r2");
        this.com1_isu.start(1, "com1_isu_r2");
        this.com3_isu.start(1, "com3_isu_r2");
        if (!this.snd_chk) {
            Sound.streamPlay(201040);
        }
        this.MSG(140, this.com_3.face, 1, 137, "If that was all we needed,\nwe would have left it to our\nintelligence agency.");
        if (!this.snd_chk) {
            Sound.streamPlay(201041);
        }
        this.MSG(30, this.com_0.face, 1, 30, "Actually...");
        System.println("CUT31 END");
        System.println("CUT32 START");
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_2.look_char(this.ziggy);
        this.ziggy.start(1, "act32");
        this.com_0.start(1, "act32");
        this.camerawork.cut32();
        System.sleep(10);
        if (!this.snd_chk) {
            Sound.streamPlay(201941);
        }
        this.iMSG(60, this.com_0.face, 1, 49, "To put it simply,");
        if (!this.snd_chk) {
            Sound.streamPlay(201042);
        }
        this.iMSG(190, this.com_0.face, 1, 155, "we need you to rescue someone\nand return her to her rightful place.");
        if (!this.snd_chk) {
            Sound.streamPlay(201043);
        }
        this.iMSG(95, this.com_0.face, 1, 83, "And technically, she's not human...");
        if (!this.snd_chk) {
            Sound.streamPlay(201044);
        }
        this.iMSG(50, this.ziggy.face, 1, 43, "Just tell me what I need to know.");
        if (!this.snd_chk) {
            Sound.streamPlay(201045);
        }
        this.ziggy.look_speed(0.22f);
        this.com_0.look_speed(0.22f);
        this.FACE(23, this.yuri.face, 1);
        this.ziggy.look_char(this.yuri);
        this.com_0.look_char(this.yuri);
        System.sleep(20);
        System.println("CUT32 END");
        System.println("CUT33 START");
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(true);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.com_0.look_default();
        this.camerawork.cut33();
        this.yuri.start(1, "act33");
        System.sleep(82);
        System.println("CUT33 END");
        System.println("CUT34 START");
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.com_0.start(1, "act34");
        this.camerawork.cut34();
        System.sleep(72);
        System.println("CUT34 END");
        System.println("CUT35 START");
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(true);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.yuri.start(1, "act35");
        this.camerawork.cut35();
        System.sleep(107);
        this.ziggy.look_speed(0.12f);
        this.ziggy.look_point(0.0f, 3.0f, -8.38f);
        System.sleep(22);
        System.println("CUT35 END");
        System.println("CUT36 START");
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.ziggy.look_point(0.0f, 5.0f, -8.38f);
        this.yuri.start(1, "sit");
        this.com_3.start(1, "act36");
        this.elv.start(1, "kemuri");
        this.Mon4.signal(1);
        this.Mon2.signal(0);
        this.camerawork.cut36();
        if (!this.snd_chk) {
            Sound.streamPlay(201046);
        }
        this.MSG(45, this.ziggy.face, 1, 30, "A child... ");
        if (!this.snd_chk) {
            Sound.streamPlay(201946);
        }
        this.MSG(46, this.ziggy.face, 1, 35, "Is she a civilian?");
        this._MSG(87, "委員３", "She's a Realian, a 100-Series Realian.");
        if (!this.snd_chk) {
            Sound.streamPlay(201047);
        }
        this.FACE(48, this.com_3.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201048);
        }
        this.FACE(39, this.com_3.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201948);
        }
        this.iMSG(47, this.com_3.face, 1, 46, "You heard of them?");
        System.println("CUT36 END");
        System.println("CUT37 START");
        this.camerawork.cut37();
        this.ziggy.look_speed(0.22f);
        this.com_1.look_char(this.ziggy);
        System.sleep(32);
        if (!this.snd_chk) {
            Sound.streamPlay(201049);
        }
        this.MSG(50, this.ziggy.face, 1, 47, "The 100-Series Observational Realians...");
        if (!this.snd_chk) {
            Sound.streamPlay(201949);
        }
        this.MSG(190, this.ziggy.face, 1, 169, "built specifically to combat the Gnosis.\nI've heard rumors of them,");
        if (!this.snd_chk) {
            Sound.streamPlay(201050);
        }
        this.MSG(150, this.ziggy.face, 1, 122, "but I didn't know they were disguised\nas children.");
        System.println("CUT37 END");
        System.println("CUT38 START");
        this.Mon_13.setVisible(false);
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(true);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(true);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.16f, 0.987f, 0.019f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.11f, -0.75f, -0.652f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.563f, -0.691f, 0.452f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ziggy.look_default();
        this.ziggy.start(1, "act38");
        this.com_1.start(1, "act38");
        this.camerawork.cut38();
        if (!this.snd_chk) {
            Sound.streamPlay(201051);
        }
        this.iMSG(90, this.com_1.face, 1, 65, "Is this the first time \nyou've seen the real thing?");
        if (!this.snd_chk) {
            Sound.streamPlay(201052);
        }
        this.MSG(47, this.ziggy.face, 1, 14, "Yes.");
        if (!this.snd_chk) {
            Sound.streamPlay(201053);
        }
        this.MSG(160, this.ziggy.face, 1, 111, "But isn't Vector already\nmass-producing them?");
        if (!this.snd_chk) {
            Sound.streamPlay(201054);
        }
        this._MSG(80, "委員４", "This is a prototype.");
        System.sleep(20);
        this.ziggy.look_speed(0.12f);
        this.ziggy.look_char(this.com_5);
        System.sleep(66);
        this.com_0.look_char(this.ziggy);
        System.println("CUT38 END");
        System.println("CUT39 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.341f, 0.939f, -0.038f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.244f, -0.798f, -0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.609f, -0.713f, -0.347f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(true);
            this.com_5.setVisible(false);
            this.yuri.setVisible(false);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.com_1.look_default();
        this.ziggy.look_default();
        this.ziggy.look_speed(0.12f);
        this.ziggy.start(1, "act39");
        this.com_4.start(1, "act39");
        this.camerawork.cut39();
        if (!this.snd_chk) {
            Sound.streamPlay(201055);
        }
        this.MSG(210, this.com_4.face, 1, 153, "It will serve as the model for all\nfuture 100-Series Observational Units.");
        if (!this.snd_chk) {
            Sound.streamPlay(201056);
        }
        this.MSG(80, this.ziggy.face, 1, 75, "So, what you're telling me is that");
        if (!this.snd_chk) {
            Sound.streamPlay(201956);
        }
        this.iMSG(130, this.ziggy.face, 1, 113, "securing this \"little girl\" takes\npriority over all else, right?");
        System.println("39 END");
        System.println("CUT40 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.341f, 0.939f, -0.038f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.317f, -0.767f, -0.558f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.27f, -0.929f, 0.253f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ziggy.look_speed(0.22f);
        this.Stick.setVisible(false);
        this.StickL.setVisible(true);
        this.com_4.start(1, "act40");
        this.com_0.start(1, "sit");
        this.camerawork.cut40();
        if (!this.snd_chk) {
            Sound.streamPlay(201057);
        }
        this.MSG(65, this.com_4.face, 1, 63, "It helps that you're a quick study.");
        if (!this.snd_chk) {
            Sound.streamPlay(201058);
        }
        this.MSG(76, this.com_0.face, 1, 76, "So, will you take the mission?");
        System.println("CUT40 END");
        System.println("CUT41 START");
        this.camerawork.cut41();
        this.ziggy.look_char(this.com_0);
        this._MSG(143, "ジギー", "A cyborg has no rights.\nI cannot refuse your request.");
        if (!this.snd_chk) {
            Sound.streamPlay(201059);
        }
        this.FACE(63, this.ziggy.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201060);
        }
        this.FACE(45, this.ziggy.face, 1);
        System.sleep(42);
        if (!this.snd_chk) {
            Sound.streamPlay(201061);
        }
        this.iMSG(150, this.ziggy.face, 1, 115, "I will analyze the situation  and\nlet you know what weapons and\nresources I need.");
        System.println("CUT41 END");
        System.println("CUT42 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.341f, 0.939f, -0.038f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.819f, -0.541f, -0.189f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.238f, -0.862f, 0.448f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ziggy.look_default();
        this.ziggy.start(1, "act42");
        this.camerawork.cut42();
        System.sleep(59);
        if (!this.snd_chk) {
            Sound.streamPlay(201062);
        }
        this.MSG(168, this.ziggy.face, 1, 167, "I don't mean to demand any\ncompensation, but...may I\nmake one request?");
        System.println("CUT42 END");
        System.println("CUT43 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.528f, 0.844f, -0.093f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.626f, -0.714f, -0.315f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.783f, -0.522f, 0.337f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_0.look_default();
        this.com_4.start(1, "act43");
        this.camerawork.cut43();
        if (!this.snd_chk) {
            Sound.streamPlay(201063);
        }
        this.MSG(75, this.com_4.face, 1, 40, "Go ahead.");
        System.println("CUT43 END");
        System.println("CUT44 START");
        this.ziggy.start(1, "act44");
        this.com_0.start(1, "sit");
        this.ziggy.look_speed(0.22f);
        this.ziggy.look_char(this.com_4);
        this.camerawork.cut44();
        if (!this.snd_chk) {
            Sound.streamPlay(201064);
        }
        this.MSG(96, this.ziggy.face, 1, 96, "After my return, I ask that you\neliminate the neural memories");
        this.iMSG(83, this.ziggy.face, 1, 83, "residing in my brain from\nmy previous life.");
        if (!this.snd_chk) {
            Sound.streamPlay(201065);
        }
        this.MSG(225, this.com_4.face, 1, 225, "That's not a problem, but it\ncould be a difficult procedure with\na body as outdated as yours.");
        if (!this.snd_chk) {
            Sound.streamPlay(201066);
        }
        this.MSG(92, this.ziggy.face, 1, 92, "Add as many synthetic parts\nas necessary.");
        System.sleep(12);
        if (!this.snd_chk) {
            Sound.streamPlay(201067);
        }
        this._MSG(47, "委員０", "What an odd request.");
        this.FACE(27, this.com_0.face, 1);
        this.ziggy.look_char(this.com_0);
        System.sleep(20);
        System.println("CUT44 END");
        System.println("CUT45 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.382f, 0.874f, -0.3f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.829f, -0.384f, 0.408f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.363f, -0.533f, 0.764f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_0.setVisible(false);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(false);
            this.com_5.setVisible(false);
            this.yuri.setVisible(true);
            this.ziggy.setVisible(true);
            this.woman.setVisible(false);
        }
        this.ziggy.look_char(this.com_0);
        this.yuri.start(1, "act45");
        this.camerawork.cut45();
        if (!this.snd_chk) {
            Sound.streamPlay(201068);
        }
        this.iMSG(170, this.com_0.face, 1, 146, "These days, Realians are clamoring for\nhuman rights, but here's a man\nwho wants to become a machine.");
        if (!this.snd_chk) {
            Sound.streamPlay(201069);
        }
        this.iMSG(70, this.com_0.face, 1, 57, "All right. We'll make the arrangements.");
        if (!this.snd_chk) {
            Sound.streamPlay(201070);
        }
        this.camerawork.cut42();
        this.MSG(70, this.ziggy.face, 1, 35, "Thank you.");
        System.sleep(20);
        System.println("CUT45 END");
        System.println("CUT46 START");
        this.com_0.look_default();
        this.ziggy.look_default();
        this.com_5.look_default();
        this.com_0.start(1, "act46");
        this.com_5.start(1, "act46");
        this.camerawork.cut46();
        System.sleep(35);
        if (!this.snd_chk) {
            Sound.streamPlay(201071);
        }
        this.MSG(150, this.com_5.face, 1, 149, "You can get the specifics from her,\nDr. Juli Mizrahi...later.");
        System.println("CUT46 END");
        System.println("CUT47 START");
        this.com_2.look_default();
        this.ziggy.setTranslate(0.0f, 0.07f, 0.0f);
        this.ziggy.start(1, "act47");
        this.camerawork.cut47();
        System.sleep(180);
        System.println("CUT47 END");
        System.println("CUT48 START");
        this.light.setDirection2(1, -0.08f, 0.862f, 0.501f);
        this.yuri.start(1, "act48");
        this.elv.start(1, "el_down");
        this.camerawork.cut48();
        System.sleep(70);
        this.hat1.start(1, "tume1_c");
        this.hat2.start(1, "tume2_c");
        this.hat3.start(1, "tume3_c");
        this.hat4.start(1, "door1_c");
        this.hat5.start(1, "door2_c");
        this.hat6.start(1, "door3_c");
        System.sleep(143);
        System.println("CUT48 END");
        System.println("CUT49 START");
        System.sleep(75);
        if (!this.snd_chk) {
            Sound.streamPlay(201072);
        }
        this.MSG(122, this.yuri.face, 1, 121, "Well, well...\nHe didn't die in the line of duty.");
        System.println("CUT49 END");
        System.println("CUT50 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.035f, 0.264f, 0.964f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.855f, -0.507f, -0.106f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.7f, -0.692f, -0.174f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        if (this.syaku) {
            this.com_0.setVisible(true);
            this.com_1.setVisible(false);
            this.com_2.setVisible(false);
            this.com_3.setVisible(false);
            this.com_4.setVisible(true);
            this.com_5.setVisible(false);
            this.yuri.setVisible(true);
            this.ziggy.setVisible(false);
            this.woman.setVisible(false);
        }
        this.com_4.look_default();
        this.yuri.start(1, "act50");
        this.camerawork.cut50();
        this._MSG(128, "ユリ", "It says here he committed suicide.\nOne shot in the head from\nhis beloved pistol.");
        if (!this.snd_chk) {
            Sound.streamPlay(201073);
        }
        this.FACE(59, this.yuri.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201074);
        }
        this.FACE(69, this.yuri.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201075);
        }
        this.MSG(45, this.com_4.face, 1, 45, "Did you say suicide?");
        System.println("CUT50 END");
        System.println("CUT51 START");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.281f, 0.834f, 0.475f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -0.431f, -0.0f, -0.902f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.116f, -0.817f, -0.564f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.com_4.start(1, "act51");
        this.camerawork.cut51();
        this._MSG(183, "委員４", "Can we entrust the 100-Series to\nthat man? What if his self-destructive\ntendencies arise?");
        if (!this.snd_chk) {
            Sound.streamPlay(201076);
        }
        this.FACE(97, this.com_4.face, 1);
        if (!this.snd_chk) {
            Sound.streamPlay(201077);
        }
        this.FACE(86, this.com_4.face, 1);
        System.println("CUT51 END");
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.384f, 0.814f, 0.437f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.299f, -0.857f, 0.421f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.208f, -0.863f, 0.459f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.Mon_Y2.start(1, "f0_52");
        this.yuri.start(1, "act52");
        this.yuri_isu.start(1, "yuri_isu_r3");
        this.camerawork.cut52();
        if (!this.snd_chk) {
            Sound.streamPlay(201078);
        }
        this.MSG(45, this.yuri.face, 1, 34, "Not to worry.");
        if (!this.snd_chk) {
            Sound.streamPlay(201079);
        }
        this.iMSG(140, this.yuri.face, 1, 134, "He's been equipped with a safety\nmechanism, which prevents him from\nhurting himself or abandoning his mission.");
        System.sleep(168);
        System.println("52 END");
        System.println("CUT52-2 START");
        this.yuri.start(1, "act52_2");
        this.camerawork.cut52_2();
        System.sleep(140);
        System.println("CUT52-2 END");
        if (!this.snd_chk) {
            Sound.streamPlay(201080);
        }
        this.MSG(105, this.yuri.face, 1, 90, "He was brought back to life\nagainst his will.");
        System.println("CUT52-2 END");
        System.println("CUT56 START");
        this.yuri.setShadow(0, 0);
        this.yuri.start(1, "act55");
        this.camerawork.cut56();
        if (!this.snd_chk) {
            Sound.streamPlay(201081);
        }
        this.iMSG(210, this.yuri.face, 1, 163, "Once he replaces\nthe rest of his brain with synthetics,\nhe'll be a complete machine.");
        System.println("CUT56 END");
        System.println("CUT57 START");
        this.yuri.setShadow(0, 0);
        this.camerawork.cut57();
        System.sleep(15);
        if (!this.snd_chk) {
            Sound.streamPlay(201082);
        }
        this.iMSG(165, this.yuri.face, 1, 156, "Only then...will he be legally dead.");
        System.println("CUT57 END");
        ++n;
        this.Timechk_SceneEnd();
    }

    void point_light() {
        int n = 1000;
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        boolean bl = false;
        while (true) {
            this.btn1 = this.pad0.getButton();
            this.edge1 = this.pad0.getEdge();
            this.btn2 = this.pad1.getButton();
            this.edge2 = this.pad1.getEdge();
            if ((this.edge2 & 0x800) != 0) break;
            if ((this.btn2 & 0x400) != 0) {
                if (n >= 3200) {
                    n -= 800;
                } else if (n >= 1600) {
                    n -= 400;
                } else if (n >= 800) {
                    n -= 200;
                } else if (n >= 300) {
                    n -= 100;
                } else if (n >= 100) {
                    n -= 50;
                } else if (n >= 50) {
                    n -= 30;
                } else if (n > 10) {
                    n -= 10;
                } else if (n <= 10) {
                    n -= 2;
                } else if (n <= 0) {
                    n = 1;
                }
                if (n <= 0) {
                    n = 1;
                }
                Runtime.setRegister(0, (float) n);
                System.println("speed--------------------------------------------------(/[#0])");
                System.sleep(2);
            }
            if ((this.btn2 & 0x200) != 0) {
                if (n >= 3200) {
                    n = 6400;
                } else if (n >= 1600) {
                    n = 3200;
                } else if (n >= 800) {
                    n = 1600;
                } else if (n >= 400) {
                    n = 800;
                } else if (n >= 200) {
                    n = 400;
                } else if (n >= 50) {
                    n += 30;
                } else if (n > 10) {
                    n += 10;
                } else if (n <= 10) {
                    n += 2;
                }
                Runtime.setRegister(0, (float) n);
                System.println("speed--------------------------------------------------(/[#0])");
                System.sleep(2);
            }
            if ((this.btn2 & 0x80) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (f < 10.0f) {
                        f += 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Red Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (f > 0.0f) {
                        f -= 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Red Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x10) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (f2 < 10.0f) {
                        f2 += 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Green Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (f2 > 0.0f) {
                        f2 -= 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Green Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x20) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (f3 < 10.0f) {
                        f3 += 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Blue Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (f3 > 0.0f) {
                        f3 -= 2.0f / (float) n;
                        bl = true;
                    } else {
                        System.println("Blue Min!!");
                    }
                }
            }
            if (bl) {
                Runtime.setRegister(0, f);
                Runtime.setRegister(1, f2);
                Runtime.setRegister(2, f3);
                System.println("light.setGlobalPointLightCol( 0, /[#0]f,/[#1]f,/[#2]f);");
            }
            this.light.setGlobalPointLightCol(0, f, f2, f3);
            bl = false;
            System.sleep(1);
        }
    }

    void sMSG(int n, Chr chr, int n2, int n3, String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        chr.mtn(n2, 0, 120, 10, 9, 1.0f, false);
        chr.start(4, null);
        if (this.msgflag) {
            System.println("MSGprint(with Face).");
            System.println(string);
            this.msg.print(string);
        }
        System.sleep(n3);
        chr.mtn(n2 + 1, 0, 120, 5, 9, 1.0f, false);
        chr.start(4, null);
        System.println("Face End.");
        if (n > n3) {
            System.sleep(n - n3);
        }
        this.msg.clear();
    }

    void sMSG(int n, Chr chr, int n2, String string) {
        this.sMSG(n, chr, n2, n - 10, string);
    }

    void setRegTF(float f, float f2) {
        Runtime.setRegister(0, f);
        Runtime.setRegister(1, f2);
    }

    void setRegTXYZ(float f, float f2, float f3, float f4) {
        Runtime.setRegister(0, f);
        Runtime.setRegister(1, f2);
        Runtime.setRegister(2, f3);
        Runtime.setRegister(3, f4);
    }

    void setRegXYZ(float f, float f2, float f3) {
        Runtime.setRegister(0, f);
        Runtime.setRegister(1, f2);
        Runtime.setRegister(2, f3);
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
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

        void act29() {
            System.sleep(170);
            int n = 0;
            while (n < 83) {
                SCE02001A.this.Mon2.setArgs(2, 82 - n, 0, 0, -1);
                SCE02001A.this.Mon3.setArgs(2, n, 0, 0, -1);
                System.sleep(1);
                n += 2;
            }
            SCE02001A.this.Mon2.signal(0);
        }

        void f0() {
            int n = 100;
            while (n > -1) {
                this.setArgs(2, n, 0, 0, -1);
                System.sleep(1);
                n -= 2;
            }
            this.setArgs(2, 0, 0, 0, -1);
            this.signal(0);
        }

        void f0_52() {
            System.sleep(30);
            int n = 100;
            while (n > -1) {
                this.setArgs(2, n, 0, 0, -1);
                System.sleep(1);
                n -= 2;
            }
            this.setArgs(2, 0, 0, 0, -1);
            this.signal(0);
        }

        void f1() {
            this.signal(1);
            int n = 0;
            while (n < 101) {
                this.setArgs(2, n, 0, 0, -1);
                System.sleep(1);
                n += 2;
            }
            this.setArgs(2, 100, 0, 0, -1);
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

    class Mapunits
            extends Unit {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        Mapunits() {
        }

        void com0_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, -1.3f, -0.1f, 3.8f, 90.0f, -0.34f, -0.1f, 3.8f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com0_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 1.35f, -0.1f, 3.8f, 90.0f, 0.34f, -0.1f, 3.8f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com0_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(0.0f, f, 3.8f);
                SCE02001A.this.com_0.setTranslate(SCE02001A.this.com_0.px, f, SCE02001A.this.com_0.pz);
                SCE02001A.this.Mon_0.setTranslate(SCE02001A.this.Mon_0.px, 0.9f + f, SCE02001A.this.Mon_0.pz);
                SCE02001A.this.Mon_02.setTranslate(SCE02001A.this.Mon_0.px, SCE02001A.this.Mon_0.py, SCE02001A.this.Mon_0.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com1_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, -3.8f, -0.1f, -1.25f, 90.0f, -3.8f, -0.1f, -0.35f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com1_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, -3.81f, -0.1f, 1.32f, 90.0f, -3.81f, -0.1f, 0.33f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com1_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(-3.8f, f, 0.0f);
                SCE02001A.this.com_1.setTranslate(SCE02001A.this.com_1.px, f, SCE02001A.this.com_1.pz);
                SCE02001A.this.Mon_1.setTranslate(SCE02001A.this.Mon_1.px, 0.9f + f, SCE02001A.this.Mon_1.pz);
                SCE02001A.this.Mon_12.setTranslate(SCE02001A.this.Mon_1.px, SCE02001A.this.Mon_1.py, SCE02001A.this.Mon_1.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com1_isu_r1() {
            float f = 356.0f;
            while (f < 434.0f) {
                float f2 = Math.cos(Math.toRadians(f + 90.0f));
                float f3 = Math.sin(Math.toRadians(f + 90.0f));
                float f4 = f2 * com_1_a + -3.8f;
                float f5 = f3 * com_1_a + 0.0f;
                float f6 = f2 * mon_1_a + -3.8f;
                float f7 = f3 * mon_1_a + 0.0f;
                SCE02001A.this.com_1.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_1.setRotate(0.0f, f + 90.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_1.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_1.setRotate(0.0f, f + 90.0f, 0.0f);
                SCE02001A.this.Mon_12.setTranslate(SCE02001A.this.Mon_1.px, SCE02001A.this.Mon_1.py, SCE02001A.this.Mon_1.pz);
                SCE02001A.this.Mon_12.setRotate(SCE02001A.this.Mon_1.rx, SCE02001A.this.Mon_1.ry, SCE02001A.this.Mon_1.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void com1_isu_r2() {
            float f = 433.0f;
            while (f > 356.0f) {
                float f2 = Math.cos(Math.toRadians(f + 90.0f));
                float f3 = Math.sin(Math.toRadians(f + 90.0f));
                float f4 = f2 * com_1_a + -3.8f;
                float f5 = f3 * com_1_a + 0.0f;
                float f6 = f2 * mon_1_a + -3.8f;
                float f7 = f3 * mon_1_a + 0.0f;
                SCE02001A.this.com_1.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_1.setRotate(0.0f, f + 90.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_1.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_1.setRotate(0.0f, f + 90.0f, 0.0f);
                SCE02001A.this.Mon_12.setTranslate(SCE02001A.this.Mon_1.px, SCE02001A.this.Mon_1.py, SCE02001A.this.Mon_1.pz);
                SCE02001A.this.Mon_12.setRotate(SCE02001A.this.Mon_1.rx, SCE02001A.this.Mon_1.ry, SCE02001A.this.Mon_1.rz);
                System.sleep(1);
                f -= 1.0f;
            }
        }

        void com2_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 3.6f, -0.1f, -1.75f, 90.0f, 2.92f, -0.1f, -2.45f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com2_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 1.71f, -0.1f, -3.67f, 90.0f, 2.44f, -0.1f, -2.93f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com2_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(2.7f, f + 1.38f, -2.7f);
                SCE02001A.this.com_2.setTranslate(SCE02001A.this.com_2.px, f, SCE02001A.this.com_2.pz);
                SCE02001A.this.Mon_2.setTranslate(SCE02001A.this.Mon_2.px, 0.9f + f, SCE02001A.this.Mon_2.pz);
                SCE02001A.this.Mon_22.setTranslate(SCE02001A.this.Mon_2.px, SCE02001A.this.Mon_2.py, SCE02001A.this.Mon_2.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com2_isu_r1() {
            float f = 4.0f;
            while (f > -117.0f) {
                float f2 = Math.cos(Math.toRadians(f + 315.0f));
                float f3 = Math.sin(Math.toRadians(f + 315.0f));
                float f4 = f2 * com_2_a + 2.7f;
                float f5 = f3 * com_2_a + -2.7f;
                float f6 = f2 * mon_2_a + 2.7f;
                float f7 = f3 * mon_2_a + -2.7f;
                SCE02001A.this.com_2.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_2.setRotate(0.0f, f + 315.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_2.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_2.setRotate(0.0f, f + 315.0f, 0.0f);
                SCE02001A.this.Mon_22.setTranslate(SCE02001A.this.Mon_2.px, SCE02001A.this.Mon_2.py, SCE02001A.this.Mon_2.pz);
                SCE02001A.this.Mon_22.setRotate(SCE02001A.this.Mon_2.rx, SCE02001A.this.Mon_2.ry, SCE02001A.this.Mon_2.rz);
                System.sleep(1);
                f -= 1.0f;
            }
        }

        void com2_isu_r2() {
            float f = -117.0f;
            while (f < 5.0f) {
                float f2 = Math.cos(Math.toRadians(f + 315.0f));
                float f3 = Math.sin(Math.toRadians(f + 315.0f));
                float f4 = f2 * com_2_a + 2.7f;
                float f5 = f3 * com_2_a + -2.7f;
                float f6 = f2 * mon_2_a + 2.7f;
                float f7 = f3 * mon_2_a + -2.7f;
                SCE02001A.this.com_2.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_2.setRotate(0.0f, f + 315.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_2.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_2.setRotate(0.0f, f + 315.0f, 0.0f);
                SCE02001A.this.Mon_22.setTranslate(SCE02001A.this.Mon_2.px, SCE02001A.this.Mon_2.py, SCE02001A.this.Mon_2.pz);
                SCE02001A.this.Mon_22.setRotate(SCE02001A.this.Mon_2.rx, SCE02001A.this.Mon_2.ry, SCE02001A.this.Mon_2.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void com3_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 3.8f, -0.1f, 1.3f, 90.0f, 3.8f, -0.1f, 0.33f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com3_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 3.8f, -0.1f, -1.28f, 90.0f, 3.8f, -0.1f, -0.34f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com3_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(3.8f, f, 0.0f);
                SCE02001A.this.com_3.setTranslate(SCE02001A.this.com_3.px, f, SCE02001A.this.com_3.pz);
                SCE02001A.this.Mon_3.setTranslate(SCE02001A.this.Mon_3.px, 0.9f + f, SCE02001A.this.Mon_3.pz);
                SCE02001A.this.Mon_32.setTranslate(SCE02001A.this.Mon_3.px, SCE02001A.this.Mon_3.py, SCE02001A.this.Mon_3.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com3_isu_r1() {
            float f = 0.0f;
            while (f > -84.0f) {
                float f2 = Math.cos(Math.toRadians(f + 270.0f));
                float f3 = Math.sin(Math.toRadians(f + 270.0f));
                float f4 = f2 * com_3_a + 3.8f;
                float f5 = f3 * com_3_a + 0.0f;
                float f6 = f2 * mon_3_a + 3.8f;
                float f7 = f3 * mon_3_a + 0.0f;
                SCE02001A.this.com_3.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_3.setRotate(0.0f, f + 270.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_3.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_3.setRotate(0.0f, f + 270.0f, 0.0f);
                SCE02001A.this.Mon_32.setTranslate(SCE02001A.this.Mon_3.px, SCE02001A.this.Mon_3.py, SCE02001A.this.Mon_3.pz);
                SCE02001A.this.Mon_32.setRotate(SCE02001A.this.Mon_3.rx, SCE02001A.this.Mon_3.ry, SCE02001A.this.Mon_3.rz);
                System.sleep(1);
                f -= 1.0f;
            }
        }

        void com3_isu_r2() {
            float f = -85.0f;
            while (f < 1.0f) {
                float f2 = Math.cos(Math.toRadians(f + 270.0f));
                float f3 = Math.sin(Math.toRadians(f + 270.0f));
                float f4 = f2 * com_3_a + 3.8f;
                float f5 = f3 * com_3_a + 0.0f;
                float f6 = f2 * mon_3_a + 3.8f;
                float f7 = f3 * mon_3_a + 0.0f;
                SCE02001A.this.com_3.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_3.setRotate(0.0f, f + 270.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_3.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_3.setRotate(0.0f, f + 270.0f, 0.0f);
                SCE02001A.this.Mon_32.setTranslate(SCE02001A.this.Mon_3.px, SCE02001A.this.Mon_3.py, SCE02001A.this.Mon_3.pz);
                SCE02001A.this.Mon_32.setRotate(SCE02001A.this.Mon_3.rx, SCE02001A.this.Mon_3.ry, SCE02001A.this.Mon_3.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void com4_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 1.8f, -0.1f, 3.57f, 90.0f, 2.45f, -0.1f, 2.93f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com4_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, 3.62f, -0.1f, 1.76f, 90.0f, 2.93f, -0.1f, 2.46f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com4_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(2.7f, f, 2.7f);
                SCE02001A.this.com_4.setTranslate(SCE02001A.this.com_4.px, f, SCE02001A.this.com_4.pz);
                SCE02001A.this.Mon_4.setTranslate(SCE02001A.this.Mon_4.px, 0.9f + f, SCE02001A.this.Mon_4.pz);
                SCE02001A.this.Mon_42.setTranslate(SCE02001A.this.Mon_4.px, SCE02001A.this.Mon_4.py, SCE02001A.this.Mon_4.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com4_isu_r1() {
            System.sleep(270);
            float f = 0.0f;
            while (f > -36.0f) {
                float f2 = Math.cos(Math.toRadians(f + 225.0f));
                float f3 = Math.sin(Math.toRadians(f + 225.0f));
                float f4 = f2 * com_4_a + 2.7f;
                float f5 = f3 * com_4_a + 2.7f;
                float f6 = f2 * mon_4_a + 2.7f;
                float f7 = f3 * mon_4_a + 2.7f;
                SCE02001A.this.com_4.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_4.setRotate(0.0f, f + 225.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_4.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_4.setRotate(0.0f, f + 225.0f, 0.0f);
                SCE02001A.this.Mon_42.setTranslate(SCE02001A.this.Mon_4.px, SCE02001A.this.Mon_4.py, SCE02001A.this.Mon_4.pz);
                SCE02001A.this.Mon_42.setRotate(SCE02001A.this.Mon_4.rx, SCE02001A.this.Mon_4.ry, SCE02001A.this.Mon_4.rz);
                System.sleep(1);
                f -= 0.5f;
            }
        }

        void com4_isu_r2() {
            float f = -35.0f;
            while (f < 1.0f) {
                float f2 = Math.cos(Math.toRadians(f + 225.0f));
                float f3 = Math.sin(Math.toRadians(f + 225.0f));
                float f4 = f2 * com_4_a + 2.7f;
                float f5 = f3 * com_4_a + 2.7f;
                float f6 = f2 * mon_4_a + 2.7f;
                float f7 = f3 * mon_4_a + 2.7f;
                SCE02001A.this.com_4.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_4.setRotate(0.0f, f + 225.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_4.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_4.setRotate(0.0f, f + 225.0f, 0.0f);
                SCE02001A.this.Mon_42.setTranslate(SCE02001A.this.Mon_4.px, SCE02001A.this.Mon_4.py, SCE02001A.this.Mon_4.pz);
                SCE02001A.this.Mon_42.setRotate(SCE02001A.this.Mon_4.rx, SCE02001A.this.Mon_4.ry, SCE02001A.this.Mon_4.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void com4_isu_reset() {
            float f = 0.0f;
            float f2 = Math.cos(Math.toRadians(f + 225.0f));
            float f3 = Math.sin(Math.toRadians(f + 225.0f));
            float f4 = f2 * com_4_a + 2.7f;
            float f5 = f3 * com_4_a + 2.7f;
            float f6 = f2 * mon_4_a + 2.7f;
            float f7 = f3 * mon_4_a + 2.7f;
            SCE02001A.this.com_4.setTranslate(f4, 0.0f, f5);
            SCE02001A.this.com_4.setRotate(0.0f, f + 225.0f, 0.0f);
            this.setRotate(0.0f, f, 0.0f);
            SCE02001A.this.Mon_4.setTranslate(f6, 0.9f, f7);
            SCE02001A.this.Mon_4.setRotate(0.0f, f + 225.0f, 0.0f);
            SCE02001A.this.Mon_42.setTranslate(SCE02001A.this.Mon_4.px, SCE02001A.this.Mon_4.py, SCE02001A.this.Mon_4.pz);
            SCE02001A.this.Mon_42.setRotate(SCE02001A.this.Mon_4.rx, SCE02001A.this.Mon_4.ry, SCE02001A.this.Mon_4.rz);
        }

        void com5_h1_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, -3.58f, -0.1f, 1.8f, 90.0f, -2.93f, -0.1f, 2.45f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com5_h2_c() {
            System.sleep(60);
            float[] fArray = new float[]{1.0f, -1.8f, -0.1f, 3.62f, 90.0f, -2.47f, -0.1f, 2.95f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void com5_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(-2.7f, f, 2.7f);
                SCE02001A.this.com_5.setTranslate(SCE02001A.this.com_5.px, f, SCE02001A.this.com_5.pz);
                SCE02001A.this.Mon_5.setTranslate(SCE02001A.this.Mon_5.px, 0.9f + f, SCE02001A.this.Mon_5.pz);
                SCE02001A.this.Mon_52.setTranslate(SCE02001A.this.Mon_5.px, SCE02001A.this.Mon_5.py, SCE02001A.this.Mon_5.pz);
                System.sleep(1);
                f -= 0.02f;
            }
        }

        void com5_isu_r1() {
            float f = 0.0f;
            while (f < 36.0f) {
                float f2 = Math.cos(Math.toRadians(f + 135.0f));
                float f3 = Math.sin(Math.toRadians(f + 135.0f));
                float f4 = f2 * com_5_a + -2.7f;
                float f5 = f3 * com_5_a + 2.7f;
                float f6 = f2 * mon_5_a + -2.7f;
                float f7 = f3 * mon_5_a + 2.7f;
                SCE02001A.this.com_5.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_5.setRotate(0.0f, f + 135.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_5.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_5.setRotate(0.0f, f + 135.0f, 0.0f);
                SCE02001A.this.Mon_52.setTranslate(SCE02001A.this.Mon_5.px, SCE02001A.this.Mon_5.py, SCE02001A.this.Mon_5.pz);
                SCE02001A.this.Mon_52.setRotate(SCE02001A.this.Mon_5.rx, SCE02001A.this.Mon_5.ry, SCE02001A.this.Mon_5.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void com5_isu_r2() {
            float f = 35.0f;
            while (f > -1.0f) {
                float f2 = Math.cos(Math.toRadians(f + 135.0f));
                float f3 = Math.sin(Math.toRadians(f + 135.0f));
                float f4 = f2 * com_5_a + -2.7f;
                float f5 = f3 * com_5_a + 2.7f;
                float f6 = f2 * mon_5_a + -2.7f;
                float f7 = f3 * mon_5_a + 2.7f;
                SCE02001A.this.com_5.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.com_5.setRotate(0.0f, f + 135.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_5.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_5.setRotate(0.0f, f + 135.0f, 0.0f);
                SCE02001A.this.Mon_52.setTranslate(SCE02001A.this.Mon_5.px, SCE02001A.this.Mon_5.py, SCE02001A.this.Mon_5.pz);
                SCE02001A.this.Mon_52.setRotate(SCE02001A.this.Mon_5.rx, SCE02001A.this.Mon_5.ry, SCE02001A.this.Mon_5.rz);
                System.sleep(1);
                f -= 1.0f;
            }
        }

        void door1_c() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = 0.01f;
            fArray[2] = -0.15f;
            fArray[3] = -2.47f;
            fArray[4] = 120.0f;
            fArray[6] = -0.15f;
            fArray[7] = -0.83f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 110);
            this.move(this.posSPL, 0, true);
        }

        void door1_o() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -0.15f;
            fArray[3] = -0.83f;
            fArray[4] = 70.0f;
            fArray[5] = 0.01f;
            fArray[6] = -0.15f;
            fArray[7] = -2.47f;
            float[] fArray2 = fArray;
            System.sleep(10);
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 70);
            this.move(this.posSPL, 0, true);
        }

        void door2_c() {
            float[] fArray = new float[]{1.0f, -2.14f, -0.15f, 1.25f, 120.0f, -0.72f, -0.15f, 0.42f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 110);
            this.move(this.posSPL, 0, true);
        }

        void door2_o() {
            float[] fArray = new float[]{1.0f, -0.72f, -0.15f, 0.42f, 70.0f, -2.14f, -0.15f, 1.25f};
            System.sleep(10);
            this.posSPL.setCtrlVertex(fArray, 0, 2, 70);
            this.move(this.posSPL, 0, true);
        }

        void door3_c() {
            float[] fArray = new float[]{1.0f, 2.31f, -0.15f, 1.28f, 120.0f, 0.72f, -0.15f, 0.42f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 110);
            this.move(this.posSPL, 0, true);
        }

        void door3_o() {
            float[] fArray = new float[]{1.0f, 0.72f, -0.15f, 0.42f, 70.0f, 2.31f, -0.15f, 1.28f};
            System.sleep(10);
            this.posSPL.setCtrlVertex(fArray, 0, 2, 70);
            this.move(this.posSPL, 0, true);
        }

        void el_down() {
            Camera camera = Camera.create(2);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[4] = 300.0f;
            fArray[6] = -6.19f;
            float[] fArray2 = fArray;
            camera.setTranslate(0.0f, 0.0f, 0.25f);
            camera.setRotate(0.0f, 0.0f, 0.0f);
            camera.transSPL(fArray2, 0);
            int n = 0;
            while (n < 301) {
                this.setTranslate(camera.getTranslateX(), camera.getTranslateY(), camera.getTranslateZ());
                this.setRotate(camera.getRotateX(), camera.getRotateY(), camera.getRotateZ());
                SCE02001A.this.ziggy.setTranslate(0.0f, SCE02001A.this.elv.py - 0.07f, 0.0f);
                System.sleep(1);
                ++n;
            }
        }

        void el_up() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -6.19f;
            fArray[4] = 142.0f;
            float[] fArray2 = fArray;
            this.setTranslate(0.0f, -6.19f, 2.82f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.posSPL.setCtrlVertex(fArray2, 0, 1, 142);
            this.move(this.posSPL, 0, true);
        }

        void kemuri() {
            SCE02001A.this.eft0 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02001A.this.eft0.setScale(1.0f, 1.0f, 1.0f);
            SCE02001A.this.eft0.setTranslate(3.97f, 1.14f, -0.08f);
            SCE02001A.this.eft0.disp(true);
            SCE02001A.this.eft0.setForceLoop(false);
            SCE02001A.this.eft0.setClip(false);
        }

        void tume1_c() {
            float[] fArray = new float[]{1.0f, -1.95f, -0.15f, -0.99f, 120.0f, -0.96f, -0.15f, -0.56f};
            System.sleep(60);
            this.posSPL.setCtrlVertex(fArray, 0, 2, 120);
            this.move(this.posSPL, 0, true);
        }

        void tume1_o() {
            float[] fArray = new float[]{1.0f, -0.96f, -0.15f, -0.56f, 80.0f, -1.95f, -0.15f, -0.99f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 80);
            this.move(this.posSPL, 0, true);
        }

        void tume2_c() {
            float[] fArray = new float[]{1.0f, 1.91f, -0.15f, -1.06f, 120.0f, 0.96f, -0.15f, -0.56f};
            System.sleep(60);
            this.posSPL.setCtrlVertex(fArray, 0, 2, 120);
            this.move(this.posSPL, 0, true);
        }

        void tume2_o() {
            float[] fArray = new float[]{1.0f, 0.96f, -0.15f, -0.56f, 80.0f, 1.91f, -0.15f, -1.06f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 80);
            this.move(this.posSPL, 0, true);
        }

        void tume3_c() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -0.15f;
            fArray[3] = 2.2f;
            fArray[4] = 120.0f;
            fArray[6] = -0.15f;
            fArray[7] = 1.11f;
            float[] fArray2 = fArray;
            System.sleep(60);
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 120);
            this.move(this.posSPL, 0, true);
        }

        void tume3_o() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -0.15f;
            fArray[3] = 1.11f;
            fArray[4] = 80.0f;
            fArray[6] = -0.15f;
            fArray[7] = 2.2f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 80);
            this.move(this.posSPL, 0, true);
        }

        void yuri_h1_c() {
            System.sleep(60);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -6.19f;
            fArray[4] = 90.0f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void yuri_h2_c() {
            System.sleep(60);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -6.19f;
            fArray[4] = 90.0f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 90);
            this.move(this.posSPL, 0, true);
        }

        void yuri_isu_d() {
            float f = 0.0f;
            while (f >= -6.18f) {
                this.setTranslate(-2.7f, f, -2.7f);
                SCE02001A.this.yuri.setTranslate(SCE02001A.this.yuri.px, f, SCE02001A.this.yuri.pz);
                SCE02001A.this.Mon_Y.setTranslate(SCE02001A.this.Mon_Y.px, 0.9f + f, SCE02001A.this.Mon_Y.pz);
                SCE02001A.this.Mon_Y2.setTranslate(SCE02001A.this.Mon_Y.px, SCE02001A.this.Mon_Y.py, SCE02001A.this.Mon_Y.pz);
                System.sleep(1);
                f -= 0.05f;
            }
        }

        void yuri_isu_r1() {
            float f = 0.0f;
            while (f < 118.0f) {
                float f2 = Math.cos(Math.toRadians(f + 45.0f));
                float f3 = Math.sin(Math.toRadians(f + 45.0f));
                float f4 = f2 * yuri_a + -2.7f;
                float f5 = f3 * yuri_a + -2.7f;
                float f6 = f2 * mon_y_a + -2.7f;
                float f7 = f3 * mon_y_a + -2.7f;
                SCE02001A.this.yuri.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.yuri.setRotate(0.0f, f + 45.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_Y.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_Y.setRotate(0.0f, f + 45.0f, 0.0f);
                SCE02001A.this.Mon_Y2.setTranslate(SCE02001A.this.Mon_Y.px, SCE02001A.this.Mon_Y.py, SCE02001A.this.Mon_Y.pz);
                SCE02001A.this.Mon_Y2.setRotate(SCE02001A.this.Mon_Y.rx, SCE02001A.this.Mon_Y.ry, SCE02001A.this.Mon_Y.rz);
                System.sleep(1);
                f += 1.0f;
            }
        }

        void yuri_isu_r2() {
            float f = 117.0f;
            while (f > -1.0f) {
                float f2 = Math.cos(Math.toRadians(f + 45.0f));
                float f3 = Math.sin(Math.toRadians(f + 45.0f));
                float f4 = f2 * yuri_a + -2.7f;
                float f5 = f3 * yuri_a + -2.7f;
                float f6 = f2 * mon_y_a + -2.7f;
                float f7 = f3 * mon_y_a + -2.7f;
                SCE02001A.this.yuri.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.yuri.setRotate(0.0f, f + 45.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_Y.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_Y.setRotate(0.0f, f + 45.0f, 0.0f);
                SCE02001A.this.Mon_Y2.setTranslate(SCE02001A.this.Mon_Y.px, SCE02001A.this.Mon_Y.py, SCE02001A.this.Mon_Y.pz);
                SCE02001A.this.Mon_Y2.setRotate(SCE02001A.this.Mon_Y.rx, SCE02001A.this.Mon_Y.ry, SCE02001A.this.Mon_Y.rz);
                System.sleep(1);
                f -= 1.0f;
            }
        }

        void yuri_isu_r3() {
            float f = 0.0f;
            while (f > -142.0f) {
                float f2 = Math.cos(Math.toRadians(f + 45.0f));
                float f3 = Math.sin(Math.toRadians(f + 45.0f));
                float f4 = f2 * yuri_a + -2.7f;
                float f5 = f3 * yuri_a + -2.7f;
                float f6 = f2 * mon_y_a + -2.7f;
                float f7 = f3 * mon_y_a + -2.7f;
                SCE02001A.this.yuri.setTranslate(f4, 0.0f, f5);
                SCE02001A.this.yuri.setRotate(0.0f, f + 45.0f, 0.0f);
                this.setRotate(0.0f, f, 0.0f);
                SCE02001A.this.Mon_Y.setTranslate(f6, 0.9f, f7);
                SCE02001A.this.Mon_Y.setRotate(0.0f, f + 45.0f, 0.0f);
                SCE02001A.this.Mon_Y2.setTranslate(SCE02001A.this.Mon_Y.px, SCE02001A.this.Mon_Y.py, SCE02001A.this.Mon_Y.pz);
                SCE02001A.this.Mon_Y2.setRotate(SCE02001A.this.Mon_Y.rx, SCE02001A.this.Mon_Y.ry, SCE02001A.this.Mon_Y.rz);
                System.sleep(1);
                f -= 0.5f;
            }
        }

        void yuri_walk() {
            Camera camera = Camera.create(3);
            SCE02001A.this.yuri.setRotate(0.0f, -113.0f, 0.0f);
            SCE02001A.this.yuri.setTranslate(-3.16f, 0.0f, -2.91f);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -3.16f;
            fArray[3] = -2.91f;
            fArray[4] = 75.0f;
            fArray[5] = -5.62f;
            fArray[7] = -3.75f;
            float[] fArray2 = fArray;
            camera.setRotate(0.0f, -113.0f, 0.0f);
            camera.setTranslate(-3.16f, 0.0f, -2.91f);
            camera.transSPL(fArray2, 1);
            int n = 0;
            while (n < 76) {
                SCE02001A.this.yuri.setTranslate(camera.getTranslateX(), camera.getTranslateY(), camera.getTranslateZ());
                System.sleep(1);
                ++n;
            }
        }
    }

    class Ziggy
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Ziggy() {
        }

        void act16() {
            this.mtn(279, 0, 227, 8, 0, 0.68f, true);
        }

        void act20() {
            this.setTranslate(0.0f, -0.07f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(279, 228, 410, 0, 0, 1.0f, true);
        }

        void act22() {
            this.mtn(280, 0, 180, 8, 0, 1.0f, true);
        }

        void act23() {
            this.mtn(279, 0, 225, 8, 0, 1.0f, true);
        }

        void act27() {
            this.mtn(282, 0, 180, 8, 0, 1.0f, true);
        }

        void act29() {
            this.mtn(285, 0, 84, 8, 1, 0.7f, true);
        }

        void act32() {
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(280, 0, 180, 8, 0, 1.0f, true);
            System.sleep(235);
        }

        void act38() {
            this.mtn(282, 0, 90, 8, 0, 1.0f, true);
            System.sleep(37);
            this.mtn(289, 0, 48, 10, 0, 0.8f, true);
        }

        void act39() {
            SCE02001A.this.ziggy.look_default();
            this.setRotate(0.0f, -90.0f, 0.0f);
            SCE02001A.this.ziggy.look_char(SCE02001A.this.com_4);
            this.mtn(289, 0, 48, 10, 0, 1.0f, true);
        }

        void act42() {
            this.setRotate(0.0f, 32.5f, 0.0f);
            this.mtn(313, 0, 220, 8, 0, 1.0f, true);
        }

        void act44() {
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(279, 0, 225, 8, 0, 0.8f, true);
        }

        void act47() {
            this.mtn(315, 0, 310, 8, 0, 1.0f, true);
        }

        void act48() {
            this.setRotate(0.0f, 32.5f, 0.0f);
            this.mtn(313, 45, 195, 8, 0, 1.0f, true);
        }

        void down() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -0.07f;
            fArray[4] = 300.0f;
            fArray[6] = -6.26f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 2, 222);
            this.move(this.posSPL, 0, true);
        }

        void off() {
            this.setTranslate(0.0f, -6.26f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void up() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = -6.26f;
            fArray[4] = 142.0f;
            fArray[6] = -0.07f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 1, 142);
            this.move(this.posSPL, 0, true);
        }
    }

    class Woman
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Woman() {
        }

        void act16() {
            this.mtn(312, 0, 0, 8, 0, 1.0f, true);
        }

        void act20() {
            this.setTranslate(-0.8f, -0.1f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(312, 0, 210, 8, 0, 0.94f, true);
        }

        void act22() {
            float f = 1.0f;
            float f2 = 1.0f;
            float f3 = 1.0f;
            float f4 = -0.1f;
            while (f2 > 0.0f) {
                if (f > 0.0f) {
                    f -= 0.05f;
                }
                if (f3 > 0.0f) {
                    f3 -= 0.05f;
                }
                if (f < 0.2f) {
                    f2 -= 0.1f;
                    f4 += 0.1f;
                }
                this.setScale(f, f2, f3);
                this.setTranslate(-0.8f, f4, 0.0f);
                System.sleep(1);
            }
            this.setVisible(false);
        }

        void off() {
            this.setTranslate(-0.8f, -6.29f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void up() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -0.8f;
            fArray[2] = -6.29f;
            fArray[4] = 142.0f;
            fArray[5] = -0.8f;
            fArray[6] = -0.1f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 1, 142);
            this.move(this.posSPL, 0, true);
        }
    }

    class Yuri
            extends Chr {
        Chr face;

        Yuri() {
        }

        void act1() {
            this.setShadow(0, 0);
            this.mtn(265, 0, 90, 8, 8, 1.0f, true);
        }

        void act33() {
            this.mtn(286, 0, 82, 8, 0, 1.0f, true);
        }

        void act35() {
            this.mtn(288, 0, 137, 8, 0, 1.0f, true);
        }

        void act45() {
            this.mtn(291, 0, 330, 8, 0, 1.0f, true);
        }

        void act48() {
            this.mtn(294, 0, 0, 0, 0, 1.14f, true);
        }

        void act50() {
            this.mtn(294, 0, 197, 10, 0, 1.14f, true);
        }

        void act52() {
            this.mtn(296, 0, 453, 8, 0, 1.0f, true);
        }

        void act52_2() {
            this.setTranslate(-4.48f, 0.0f, -2.86f);
            this.setRotate(0.0f, -86.5f, 0.0f);
            this.mtn(299, 0, 360, 8, 0, 1.0f, true);
        }

        void act55() {
            this.mtn(299, 150, 360, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(304, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_2
            extends Chr {
        Chr face;

        Com_2() {
        }

        void act11() {
            this.mtn(275, 0, 300, 8, 0, 1.0f, true);
            SCE02001A.this.com_2.look_speed(0.17f);
            SCE02001A.this.com_2.look_char(SCE02001A.this.com_4);
        }

        void act4() {
            this.mtn(267, 0, 420, 8, 0, 1.0f, true);
        }

        void act5() {
            this.mtn(267, 0, 420, 8, 0, 1.0f, true);
        }

        void act9() {
            this.mtn(273, 0, 150, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(261, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_4
            extends Chr {
        Chr face;

        Com_4() {
        }

        void act12() {
            this.mtn(263, 0, 90, 8, 0, 1.0f, true);
        }

        void act14() {
            this.mtn(277, 0, 171, 15, 1, 1.0f, true);
        }

        void act2() {
            this.mtn(264, 0, 180, 8, 8, 1.0f, true);
        }

        void act27() {
            System.sleep(79);
            this.mtn(283, 0, 252, 8, 0, 1.0f, true);
            this.mtn(284, 0, 88, 8, 1, 1.0f, true);
        }

        void act39() {
            this.mtn(263, 0, 90, 8, 8, 1.0f, true);
        }

        void act40() {
            this.mtn(297, 0, 240, 8, 0, 1.0f, true);
        }

        void act43() {
            this.mtn(290, 0, 75, 8, 0, 1.0f, true);
        }

        void act51() {
            this.mtn(295, 0, 177, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(263, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_5
            extends Chr {
        Chr face;

        Com_5() {
        }

        void act46() {
            this.mtn(293, 0, 185, 8, 0, 1.0f, true);
        }

        void act6() {
            this.mtn(269, 0, 132, 8, 8, 1.2f, true);
        }

        void act7() {
            this.mtn(270, 0, 162, 8, 8, 1.0f, true);
        }

        void act8() {
            this.mtn(272, 0, 210, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(303, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_0
            extends Chr {
        Chr face;

        Com_0() {
        }

        void act15() {
            this.mtn(278, 0, 197, 8, 0, 1.0f, true);
        }

        void act2() {
            this.mtn(258, 0, 180, 8, 8, 1.0f, true);
        }

        void act25() {
            this.mtn(314, 13, 150, 32, 0, 1.0f, true);
        }

        void act3() {
            this.mtn(266, 0, 255, 8, 8, 1.5f, true);
        }

        void act32() {
            this.mtn(257, 0, 90, 8, 8, 1.0f, true);
            System.sleep(330);
        }

        void act34() {
            this.mtn(287, 0, 72, 8, 0, 1.0f, true);
        }

        void act40() {
            this.mtn(258, 0, 180, 8, 0, 0.9f, true);
        }

        void act46() {
            this.mtn(292, 0, 185, 8, 0, 1.0f, true);
        }

        void act5() {
            this.mtn(268, 0, 390, 8, 0, 1.0f, true);
        }

        void act53() {
            this.mtn(298, 0, 240, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(257, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_1
            extends Chr {
        Chr face;

        Com_1() {
        }

        void act2() {
            this.mtn(260, 0, 180, 8, 8, 1.0f, true);
        }

        void act23() {
            this.mtn(281, 0, 82, 8, 0, 1.0f, true);
        }

        void act38() {
            this.mtn(302, 0, 90, 8, 8, 1.0f, true);
        }

        void act8() {
            this.mtn(271, 0, 210, 8, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(259, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Com_3
            extends Chr {
        Chr face;

        Com_3() {
        }

        void act1() {
            this.mtn(257, 0, 90, 8, 0, 1.0f, true);
        }

        void act10() {
            this.mtn(274, 0, 212, 8, 0, 1.21f, true);
        }

        void act11() {
            this.mtn(276, 0, 314, 8, 0, 1.0f, true);
            SCE02001A.this.com_3.look_speed(0.17f);
            SCE02001A.this.com_3.look_char(SCE02001A.this.com_4);
        }

        void act25() {
            this.mtn(307, 0, 0, 0, 0, 1.0f, true);
        }

        void act26() {
            this.mtn(307, 0, 97, 8, 0, 0.3f, true);
        }

        void act36() {
            this.mtn(307, 0, 97, 8, 0, 1.0f, true);
        }

        void act9() {
            this.mtn(274, 0, 0, 0, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(262, 0, 90, 8, 8, 1.0f, true);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -28.7f;
            fArray[2] = 180.11f;
            fArray[4] = 195.0f;
            fArray[5] = -28.7f;
            fArray[6] = 183.18f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.change();
            SCE02001A.this.BaseCam.setClipRange(0.01f, 3000.0f);
            SCE02001A.this.BaseCam.setFov(40.0f);
            SCE02001A.this.BaseCam.setTranslate(-0.21f, 4.91f, -7.63f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut10() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-4.84f, 274.58f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(1.25f, 1.1f, 0.09f);
        }

        public void cut11() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 2.32f, 1.17f, 2.72f, 420.0f, 1.97f, 1.17f, 3.77f};
            SCE02001A.this.BaseCam.setRotate(-6.45f, 341.71f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut13() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.56f, 0.83f, 1.6f, 321.0f, 1.56f, 1.1f, 1.6f};
            SCE02001A.this.BaseCam.setRotate(1.1f, 225.49f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut15() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 4.03f, 1.71f, 1.45f, 224.0f, 3.66f, 1.71f, 1.67f};
            SCE02001A.this.BaseCam.setRotate(-13.8f, 121.02f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut16() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-30.56f, 22.56f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.48f, 1.58f, 4.62f);
        }

        public void cut17() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-37.02f, 9.54f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.9f, 4.23f, 5.63f);
        }

        public void cut18() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-7.92f, 362.76f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-0.07f, 1.54f, 3.87f);
        }

        public void cut19() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(35.34f, 337.8f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-0.67f, 0.2f, 1.43f);
        }

        public void cut2() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -4.34f;
            fArray[2] = 202.88f;
            fArray[4] = 195.0f;
            fArray[5] = -4.34f;
            fArray[6] = 205.88f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setTranslate(-3.9f, 1.86f, -6.28f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut20() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-29.19f, -22.41f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-1.45f, 2.17f, 3.04f);
        }

        public void cut21() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setFov(50.0f);
            SCE02001A.this.BaseCam.setRotate(-12.61f, 32.85f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(3.19f, 2.07f, 6.1f);
        }

        public void cut22() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-8.39f, 17.53f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.73f, 1.53f, 5.04f);
        }

        public void cut23() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = 3.21f;
            fArray[2] = 59.08f;
            fArray[4] = 442.0f;
            fArray[5] = 10.49f;
            fArray[6] = 59.08f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setTranslate(2.27f, 0.65f, 2.02f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut25() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.52f, 0.94f, -1.67f, 127.0f, 0.61f, 0.94f, -1.66f};
            SCE02001A.this.BaseCam.setRotate(-1.29f, 174.2f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut26() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-0.12f, 119.66f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(5.65f, 1.06f, -1.65f);
        }

        public void cut27() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02001A.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(1, 0.16f, 0.987f, 0.019f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02001A.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(2, 0.339f, -0.68f, 0.65f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02001A.this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(3, -0.07f, -0.662f, 0.746f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02001A.this.BaseCam.setRotate(-5.29f, 65.29f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(3.85f, 1.37f, 2.96f);
            System.sleep(79);
            SCE02001A.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02001A.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(1, 0.16f, 0.987f, 0.019f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02001A.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(2, -0.284f, -0.25f, -0.926f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02001A.this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(3, -0.075f, -0.699f, 0.711f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -2.12f, 2.07f, -0.67f, 302.0f, -2.06f, 2.07f, -0.77f};
            SCE02001A.this.BaseCam.setRotate(-16.45f, 237.72f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut3() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.02f, 1.3f, 0.34f, 225.0f, 0.02f, 1.3f, 0.74f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-10.52f, 179.12f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut30() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.97f, 5.38f, 10.76f, 390.0f, 2.11f, 5.38f, 11.91f};
            SCE02001A.this.BaseCam.setClipRange(7.0f, 1000.0f);
            SCE02001A.this.BaseCam.setFov(40.0f);
            SCE02001A.this.BaseCam.setRotate(-17.81f, 7.07f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut31() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.77f, 0.14f, -12.56f, 360.0f, 1.99f, 0.14f, -12.32f};
            SCE02001A.this.BaseCam.setClipRange(0.01f, 3000.0f);
            SCE02001A.this.BaseCam.setRotate(0.38f, 168.76f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut32() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.3f, 1.96f, -1.1f, 450.0f, 0.17f, 1.96f, -1.12f};
            SCE02001A.this.BaseCam.setClipRange(0.01f, 3000.0f);
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-17.16f, 175.08f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut33() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-17.38f, 43.39f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-0.48f, 1.71f, -0.18f);
        }

        public void cut34() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-5.12f, 198.88f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-0.55f, 1.22f, 2.36f);
        }

        public void cut35() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.52f, 1.93f, 2.8f, 137.0f, 1.15f, 1.93f, 2.33f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-15.52f, 38.46f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut36() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -3.26f;
            fArray[2] = 17.74f;
            fArray[4] = 225.0f;
            fArray[5] = -2.32f;
            fArray[6] = 9.92f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[]{1.0f, 3.25f, 1.64f, 8.05f, 225.0f, 2.64f, 2.08f, 7.64f};
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
            SCE02001A.this.BaseCam.transSPL(fArray3, 0);
        }

        public void cut37() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -20.83f;
            fArray[2] = 17.42f;
            fArray[4] = 442.0f;
            fArray[5] = -17.54f;
            fArray[6] = 17.42f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setFov(40.0f);
            SCE02001A.this.BaseCam.setTranslate(3.52f, 4.36f, 7.78f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut38() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-13.58f, 102.65f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.93f, 1.86f, -0.35f);
        }

        public void cut39() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.52f, 1.86f, -3.61f, 420.0f, -1.76f, 1.86f, -3.5f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-12.74f, 204.07f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut4() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-11.2f, 302.54f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.01f, 1.35f, -1.07f);
        }

        public void cut40() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-16.0f, 276.02f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-1.94f, 1.7f, 3.81f);
        }

        public void cut41() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -37.96f;
            fArray[2] = 311.72f;
            fArray[4] = 300.0f;
            fArray[5] = -37.96f;
            fArray[6] = 313.97f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setFov(30.0f);
            SCE02001A.this.BaseCam.setTranslate(-2.89f, 4.2f, 5.4f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut42() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.51f, 1.77f, 1.83f, 227.0f, 0.43f, 1.77f, 1.54f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-10.48f, 374.07f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut43() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-18.88f, 225.3f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(1.34f, 1.64f, 1.33f);
        }

        public void cut44() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.98f, 2.09f, -3.15f, 543.0f, -1.18f, 2.09f, -3.08f};
            SCE02001A.this.BaseCam.setRotate(-15.2f, 202.3f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut45() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.34f, 1.45f, -1.37f, 330.0f, -1.5f, 1.45f, -1.54f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-13.92f, 44.54f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut46() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -11.04f;
            fArray[2] = 96.83f;
            fArray[4] = 185.0f;
            fArray[5] = -11.04f;
            fArray[6] = 95.63f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setTranslate(2.4f, 1.38f, 3.2f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut47() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-13.8f, 52.88f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(2.2f, 2.02f, 1.44f);
        }

        public void cut48() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -3.18f, 3.85f, -4.58f, 410.0f, -3.18f, 2.66f, -4.58f};
            SCE02001A.this.BaseCam.setRotate(-34.5f, 208.8f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut5() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -5.06f;
            fArray[2] = 320.2f;
            fArray[4] = 300.0f;
            fArray[5] = -5.09f;
            fArray[6] = 317.53f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setTranslate(-1.45f, 1.23f, 5.04f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut50() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-26.59f, 13.99f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(-2.36f, 1.9f, -0.94f);
        }

        public void cut51() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-7.19f, 231.69f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.87f, 1.22f, 1.29f);
        }

        public void cut52() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[12];
            fArray[0] = 1.0f;
            fArray[1] = -14.94f;
            fArray[2] = 41.18f;
            fArray[4] = 285.0f;
            fArray[5] = -14.94f;
            fArray[6] = 41.18f;
            fArray[8] = 453.0f;
            fArray[9] = -12.56f;
            fArray[10] = 45.46f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[]{1.0f, -0.22f, 1.78f, 0.11f, 285.0f, -0.42f, 1.78f, 0.25f, 453.0f, -0.42f, 1.78f, 0.25f};
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
            SCE02001A.this.BaseCam.transSPL(fArray3, 0);
        }

        public void cut52_2() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02001A.this.light.setColor(1, 0.63f, 0.63f, 0.63f);
            SCE02001A.this.light.setDirection2(1, -0.108f, 0.0f, 0.994f);
            SCE02001A.this.light.setColor(2, 0.53f, 0.53f, 0.53f);
            SCE02001A.this.light.setDirection2(2, 0.491f, 0.0f, -0.871f);
            SCE02001A.this.light.setColor(3, 0.29f, 0.29f, 0.29f);
            SCE02001A.this.light.setDirection2(3, -0.881f, -0.471f, -0.033f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -7.21f, 1.69f, -2.91f, 110.0f, -7.78f, 1.69f, -2.95f};
            SCE02001A.this.BaseCam.setRotate(-13.57f, 265.46f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0, 1, 110);
        }

        public void cut53() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02001A.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(1, 0.16f, 0.987f, 0.019f);
            SCE02001A.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(2, -0.284f, -0.25f, -0.926f);
            SCE02001A.this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            SCE02001A.this.light.setDirection2(3, -0.075f, -0.699f, 0.711f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, 3.29f, 1.48f, -1.69f, 240.0f, 2.8f, 1.48f, -1.1f};
            SCE02001A.this.BaseCam.setRotate(-9.65f, 160.92f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut54() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -28.08f;
            fArray[2] = 207.15f;
            fArray[4] = 150.0f;
            fArray[5] = -28.08f;
            fArray[6] = 201.41f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setFov(40.0f);
            SCE02001A.this.BaseCam.setTranslate(-3.5f, 4.23f, -7.7f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut55() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02001A.this.light.setColor(1, 0.63f, 0.63f, 0.63f);
            SCE02001A.this.light.setDirection2(1, -0.108f, 0.0f, 0.994f);
            SCE02001A.this.light.setColor(2, 0.53f, 0.53f, 0.53f);
            SCE02001A.this.light.setDirection2(2, 0.491f, 0.0f, -0.871f);
            SCE02001A.this.light.setColor(3, 0.29f, 0.29f, 0.29f);
            SCE02001A.this.light.setDirection2(3, -0.881f, -0.471f, -0.033f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -1.56f, 1.28f, -4.1f, 172.0f, -2.0f, 1.23f, -4.01f};
            SCE02001A.this.BaseCam.setFov(25.0f);
            SCE02001A.this.BaseCam.setRotate(-6.5f, 102.32f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut56() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02001A.this.light.setColor(1, 0.63f, 0.63f, 0.63f);
            SCE02001A.this.light.setDirection2(1, -0.542f, 0.0f, 0.84f);
            SCE02001A.this.light.setColor(2, 0.53f, 0.53f, 0.53f);
            SCE02001A.this.light.setDirection2(2, 0.605f, 0.229f, -0.763f);
            SCE02001A.this.light.setColor(3, 0.29f, 0.29f, 0.29f);
            SCE02001A.this.light.setDirection2(3, -0.851f, -0.521f, -0.055f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -6.5f, 1.8f, -4.6f, 192.0f, -6.44f, 1.8f, -4.41f};
            SCE02001A.this.BaseCam.setRotate(-17.68f, 197.96f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut57() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02001A.this.light.setColor(1, 0.63f, 0.63f, 0.63f);
            SCE02001A.this.light.setDirection2(1, -0.542f, 0.0f, 0.84f);
            SCE02001A.this.light.setColor(2, 0.53f, 0.53f, 0.53f);
            SCE02001A.this.light.setDirection2(2, 0.605f, 0.229f, -0.763f);
            SCE02001A.this.light.setColor(3, 0.29f, 0.29f, 0.29f);
            SCE02001A.this.light.setDirection2(3, -0.851f, -0.521f, -0.055f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            float[] fArray = new float[]{1.0f, -15.6f, 1.55f, -5.31f, 180.0f, -16.29f, 1.55f, -5.47f};
            SCE02001A.this.BaseCam.setClipRange(7.0f, 1000.0f);
            SCE02001A.this.BaseCam.setRotate(-4.94f, 257.24f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut6() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 2.43f, 1.65f, 2.74f, 294.0f, 2.0f, 1.65f, 2.4f};
            SCE02001A.this.BaseCam.setRotate(-12.9f, 103.45f, 0.0f);
            SCE02001A.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut8() {
            SCE02001A.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -15.56f;
            fArray[2] = 176.6f;
            fArray[4] = 210.0f;
            fArray[5] = -15.0f;
            fArray[6] = 177.9f;
            float[] fArray2 = fArray;
            SCE02001A.this.BaseCam.setTranslate(-3.46f, 1.61f, -2.06f);
            SCE02001A.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut9() {
            SCE02001A.this.Timechk_CutChange();
            SCE02001A.this.BaseCam.setRotate(-6.4f, 294.78f, 0.0f);
            SCE02001A.this.BaseCam.setTranslate(0.68f, 1.2f, -1.75f);
        }
    }
}

