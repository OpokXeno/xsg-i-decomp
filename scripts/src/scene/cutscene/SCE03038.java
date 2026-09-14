import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_DYU05_PRJ;
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

class SCE03038
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03038,
        MC_DYU05_PRJ,
        JNT_Human,
        JNT_Accesories,
        FLSallen,
        FLSjr,
        FLSshion,
        FLSziggy,
        FLSchaos,
        FLSkosmos {
    public static final int Twohand = 0;
    public static final int Rhand = 16;
    public static final int Lhand = 32;
    public static final int Open = 0;
    public static final int Close = 1;
    public static final float nc = 1.0E7f;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    Menu menu;
    Window win;
    Chr gold;
    Unit gold_unit;
    Effect gold_eft;
    boolean snd_chk = true;
    Camera BaseCam = Camera.create(2);
    Camera cam0 = Camera.create(0);
    Camera cam1 = Camera.create(1);
    Camera cam2 = Camera.create(2);
    Camera cam3 = Camera.create(3);
    Camera cam5 = Camera.create(5);
    Input pad0 = Input.create(0);
    Input pad1 = Input.create(1);
    int menuSelected;
    int selectMenu;
    Character jr;
    Character shion;
    Character allen;
    Character ziggy;
    Character2 kosmos;
    Character2 chaos;
    Character2 boy_a;
    Character2 girl_b;
    Character2 jiji_b;
    Character2 kidsm_a;
    Character2 kidsm_b;
    Character2 kidsw_b;
    Character2 man_b;
    Character2 oji_b;
    Character2 wman_b;
    Character2 box;
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    Thread _spl_thread_main;
    Thread thread_spl_cam2;
    Thread thread1;
    float[] shadowFilter = new float[4];
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    boolean Facechk_exec = true;
    int Facechk_faceno = 1;
    int Facechk_facemode = 1;
    int Facechk_frame = 1;
    int Facechk_mtnno;
    int msgsignal = 128;
    int cutwait = 0;
    boolean msgflag = true;
    Thread msg_clear = Thread.create(this, "msg_clear_thread");
    int msg_clearwait;
    boolean msg_clear_exec;
    int jisin_flag = 0;
    int jisin_input_flag = 0;
    int a = 24;
    int b = 12;
    int c = 24;
    int e = 2;
    int f = 2;
    int g = 2;
    float sokge_per = 0.5f;
    int bure_py_flag = 1;
    int __wait_loop_flag = 0;
    int kind_c0_u1_e2 = 0;
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
    public static final int CASE_MAX = 1;
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
    Thread timer_thread;
    Thread timer_thread_srv;
    int CutNo = 0;
    boolean Timechk_srv_exec;
    int Timechk_sc = 0;
    int Timechk_srv_time = 0;
    int Timechk_srv_totaltime = 0;
    Timer timer;
    int Timechk_cut = 1;
    Thread Capture_thread;
    Thread Capture_thread_srv;
    boolean Capture_thread_srv_exec = false;
    Menu Capturemenu;
    int Capture_mode;
    int CaptureFolder = 0;
    String FolderName0 = "eventmovie/";
    String FolderName;
    Light CameraTool_light = new Light(0);
    float light0_x;
    float light0_y;
    float light0_z;
    float light1_dx;
    float light1_dy;
    float light1_dz;
    float light1_rx;
    float light1_ry;
    float light1_rz;
    float light2_dx;
    float light2_dy;
    float light2_dz;
    float light2_rx;
    float light2_ry;
    float light2_rz;
    float light3_dx;
    float light3_dy;
    float light3_dz;
    float light3_rx;
    float light3_ry;
    float light3_rz;
    float tlight0_x;
    float tlight0_y;
    float tlight0_z;
    float tlight1_dx;
    float tlight1_dy;
    float tlight1_dz;
    float tlight1_rx;
    float tlight1_ry;
    float tlight1_rz;
    float tlight2_dx;
    float tlight2_dy;
    float tlight2_dz;
    float tlight2_rx;
    float tlight2_ry;
    float tlight2_rz;
    float tlight3_dx;
    float tlight3_dy;
    float tlight3_dz;
    float tlight3_rx;
    float tlight3_ry;
    float tlight3_rz;
    float[] light0data = new float[8];
    float[] light1dir = new float[8];
    float[] light1col = new float[8];
    float[] light2dir = new float[8];
    float[] light2col = new float[8];
    float[] light3dir = new float[8];
    float[] light3col = new float[8];
    Thread lightSPL_thread = Thread.create(this, "lightSPL_main");
    Lightwork lightwork0;
    Lightwork lightwork1;
    Lightwork lightwork2;
    Lightwork lightwork3;
    int lightcount;
    boolean Gnochk_exec;
    float[] gnoFilter = new float[4];
    int gnoptr = 0;
    float gno_alpha = 0.54f;
    float gno_filter = 87.5f;
    float gno_shake = 37.0f;
    float gno_ref = 0.72f;
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
    boolean ColorScreenSet_exec2;
    int[] Color_Rx;
    int[] Color_Gx;
    int[] Color_Bx;
    int[] Color_Ax;
    int[] Color_Zx;
    int Color_layerx;
    int layer_numx;
    boolean screen12_on_off;
    boolean screen13_on_off;
    boolean screen14_on_off;
    boolean screen15_on_off;
    int[] ColorScreenParam12;
    int[] ColorScreenParam13;
    int[] ColorScreenParam14;
    int[] ColorScreenParam15;
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
    Input Xpad1P;
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag;

    SCE03038() {
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
        this.ColorScreenSet_exec2 = false;
        this.Color_Rx = new int[4];
        this.Color_Gx = new int[4];
        this.Color_Bx = new int[4];
        this.Color_Ax = new int[4];
        this.Color_Zx = new int[]{147456, 147456, 147456, 147456};
        this.Color_layerx = 1;
        this.layer_numx = 0;
        this.screen12_on_off = false;
        this.screen13_on_off = false;
        this.screen14_on_off = false;
        this.screen15_on_off = false;
        int[] nArray4 = new int[8];
        nArray4[1] = 1;
        nArray4[2] = 0x100000;
        nArray4[3] = 1614815232;
        this.ColorScreenParam12 = nArray4;
        int[] nArray5 = new int[8];
        nArray5[1] = 1;
        nArray5[2] = 0x100000;
        nArray5[3] = 1614815232;
        this.ColorScreenParam13 = nArray5;
        int[] nArray6 = new int[8];
        nArray6[1] = 1;
        nArray6[2] = 0x100000;
        nArray6[3] = 1614815232;
        this.ColorScreenParam14 = nArray6;
        int[] nArray7 = new int[8];
        nArray7[1] = 1;
        nArray7[2] = 0x100000;
        nArray7[3] = 1614815232;
        this.ColorScreenParam15 = nArray7;
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
            if ((n2 & 0x10) != 0 && !this.Capture_thread_srv_exec) {
                this.Capture_SelectChr();
            }
            if ((n2 & 0x10) != 0 && !this.Capture_thread_srv_exec) {
                this.Capture_SelectFolder();
            }
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
                    this.menu = Menu.create();
                    this.menu.addQuery("キャプチャモード");
                    this.menu.addItem("全画面");
                    this.menu.addItem("ＢＧのみ（ＺＰＩＣなし）");
                    this.menu.addItem("ＢＧのみ（ＺＰＩＣあり）");
                    this.menu.addItem("キャラ単体（ＺＰＩＣなし）");
                    this.menu.addItem("キャラ単体（ＺＰＩＣあり）");
                    this.menu.addItem("オブジェ単体（ＺＰＩＣなし）");
                    this.menu.addItem("オブジェ単体（ＺＰＩＣあり）");
                    this.menu.addItem("テキスト（ＺＰＩＣなし）");
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
                    if (n2 != 7) break;
                    System.println("CaptureMODE ---TXT(pic)");
                    this.Capture_mode = 36864;
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
        this.menu = Menu.create();
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
                        this.menu = Menu.create();
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
                        this.menu = Menu.create();
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
        System.waitSignal(this.timer, this.CutNo + 1);
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
        System.waitSignal(this.timer, this.CutNo + 1);
        this.CaptureStart_CaptureTool();
        System.sleep(2);
        System.waitSignal(this.timer, this.CutNo + 1);
        this.CaptureEnd_CaptureTool();
        System.println("*********Capture End.*****************");
        this.Capture_thread_srv_exec = false;
    }

    void ColorScreenSet1() {
        if (this.ColorScreen_flag == 1) {
            Runtime.setDefocus(0, 2, this.test_param0);
        }
        this.ColorScreenSet_exec = true;
        System.println("ColorScreen  SetMode No.1");
        while (this.ColorScreenSet_exec) {
            this.btn2 = this.pad1.getButton();
            this.edge2 = this.pad1.getEdge();
            if ((this.edge2 & 0x100) != 0) {
                this.ColorScreen_printout1();
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
        System.println("ScreenColor SetMode No.1-------end");
    }

    void ColorScreenSet2() {
        this.ColorScreenSet_exec2 = true;
        System.println("ColorScreen  SetMode No.2");
        while (this.ColorScreenSet_exec2) {
            this.btn2 = this.pad1.getButton();
            this.edge2 = this.pad1.getEdge();
            if ((this.edge2 & 0x100) != 0) {
                this.ColorScreen_printout2();
                if ((this.edge2 & 0x800) != 0) {
                    Runtime.setRegister(0, this.layer_numx);
                    System.println("Layer番号 = /[$0]");
                }
            }
            if ((this.edge2 & 0x800) != 0) {
                this.ColorScreenSet_exec2 = false;
            }
            if ((this.edge2 & this._PADR3) != 0) {
                ++this.layer_numx;
                this.layer_numx %= 4;
                Runtime.setRegister(0, this.layer_numx);
                System.println("Layer番号 = /[$0]");
            }
            if ((this.edge2 & this._PADL3) != 0) {
                ++this.Color_layerx;
                this.Color_layerx %= 5;
                Runtime.setRegister(0, this.Color_layerx);
                System.println("Layer枚数 = /[$0]");
                if (this.Color_layerx == 0) {
                    this.screen12_on_off = false;
                    this.screen13_on_off = false;
                    this.screen14_on_off = false;
                    this.screen15_on_off = false;
                }
                if (this.Color_layerx == 1) {
                    this.screen12_on_off = true;
                    this.screen13_on_off = false;
                    this.screen14_on_off = false;
                    this.screen15_on_off = false;
                }
                if (this.Color_layerx == 2) {
                    this.screen12_on_off = true;
                    this.screen13_on_off = true;
                    this.screen14_on_off = false;
                    this.screen15_on_off = false;
                }
                if (this.Color_layerx == 3) {
                    this.screen12_on_off = true;
                    this.screen13_on_off = true;
                    this.screen14_on_off = true;
                    this.screen15_on_off = false;
                }
                if (this.Color_layerx == 4) {
                    this.screen12_on_off = true;
                    this.screen13_on_off = true;
                    this.screen14_on_off = true;
                    this.screen15_on_off = true;
                }
            }
            if ((this.btn2 & 0xF0) == 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    int n = this.layer_numx;
                    this.Color_Zx[n] = this.Color_Zx[n] - 1000;
                }
                if ((this.btn2 & 0x4000) != 0) {
                    int n = this.layer_numx;
                    this.Color_Zx[n] = this.Color_Zx[n] + 1000;
                }
            }
            if ((this.btn2 & 0x80) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_Rx[this.layer_numx] < 254) {
                        int n = this.layer_numx;
                        this.Color_Rx[n] = this.Color_Rx[n] + 2;
                    } else {
                        System.println("Red Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_Rx[this.layer_numx] != 0) {
                        int n = this.layer_numx;
                        this.Color_Rx[n] = this.Color_Rx[n] - 2;
                    } else {
                        System.println("Red Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x10) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_Gx[this.layer_numx] < 254) {
                        int n = this.layer_numx;
                        this.Color_Gx[n] = this.Color_Gx[n] + 2;
                    } else {
                        System.println("Green Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_Gx[this.layer_numx] != 0) {
                        int n = this.layer_numx;
                        this.Color_Gx[n] = this.Color_Gx[n] - 2;
                    } else {
                        System.println("Green Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x20) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_Bx[this.layer_numx] < 254) {
                        int n = this.layer_numx;
                        this.Color_Bx[n] = this.Color_Bx[n] + 2;
                    } else {
                        System.println("Blue Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_Bx[this.layer_numx] != 0) {
                        int n = this.layer_numx;
                        this.Color_Bx[n] = this.Color_Bx[n] - 2;
                    } else {
                        System.println("Blue Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x40) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Color_Ax[this.layer_numx] < 254) {
                        int n = this.layer_numx;
                        this.Color_Ax[n] = this.Color_Ax[n] + 2;
                    } else {
                        System.println("Alpha Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Color_Ax[this.layer_numx] != 0) {
                        int n = this.layer_numx;
                        this.Color_Ax[n] = this.Color_Ax[n] - 2;
                    } else {
                        System.println("Alpha Min!!");
                    }
                }
            }
            if (this.ColorScreen_flag == 0) {
                Runtime.setDefocus(12, 0, null);
                Runtime.setDefocus(13, 0, null);
                Runtime.setDefocus(14, 0, null);
                Runtime.setDefocus(15, 0, null);
                if (this.screen12_on_off) {
                    this.ColorScreenParam12[2] = this.Color_Zx[0];
                    this.ColorScreenParam12[3] = this.Color_Ax[0] * 0x1000000 + this.Color_Bx[0] * 65536 + this.Color_Gx[0] * 256 + this.Color_Rx[0];
                    Runtime.setDefocus(12, 1, this.ColorScreenParam12);
                }
                if (this.screen13_on_off) {
                    this.ColorScreenParam13[2] = this.Color_Zx[1];
                    this.ColorScreenParam13[3] = this.Color_Ax[1] * 0x1000000 + this.Color_Bx[1] * 65536 + this.Color_Gx[1] * 256 + this.Color_Rx[1];
                    Runtime.setDefocus(13, 1, this.ColorScreenParam13);
                }
                if (this.screen14_on_off) {
                    this.ColorScreenParam14[2] = this.Color_Zx[2];
                    this.ColorScreenParam14[3] = this.Color_Ax[2] * 0x1000000 + this.Color_Bx[2] * 65536 + this.Color_Gx[2] * 256 + this.Color_Rx[2];
                    Runtime.setDefocus(14, 1, this.ColorScreenParam14);
                }
                if (this.screen15_on_off) {
                    this.ColorScreenParam15[2] = this.Color_Zx[3];
                    this.ColorScreenParam15[3] = this.Color_Ax[3] * 0x1000000 + this.Color_Bx[3] * 65536 + this.Color_Gx[3] * 256 + this.Color_Rx[3];
                    Runtime.setDefocus(15, 1, this.ColorScreenParam15);
                }
            }
            System.sleep(1);
        }
        System.println("ScreenColor SetMode No.2-------end");
    }

    void ColorScreen_printout1() {
        if (this.ColorScreen_flag == 0) {
            this.ColorScreenParam[2] = this.Color_Z;
            this.ColorScreenParam[3] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
            Runtime.setRegister(0, this.ColorScreenParam[3]);
            Runtime.setRegister(1, this.ColorScreenParam[2]);
            if (this.Color_layer != 0) {
                System.println("*************ColorScreen Printout No.1*************");
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

    void ColorScreen_printout2() {
        System.println("*************ColorScreen PrintoutNo.2*************");
        if (this.screen12_on_off) {
            this.ColorScreenParam12[2] = this.Color_Zx[0];
            this.ColorScreenParam12[3] = this.Color_Ax[0] * 0x1000000 + this.Color_Bx[0] * 65536 + this.Color_Gx[0] * 256 + this.Color_Rx[0];
            Runtime.setRegister(0, this.ColorScreenParam12[3]);
            Runtime.setRegister(1, this.ColorScreenParam12[2]);
            System.println("int ColorScreenParam12[] = {");
            System.println("0,");
            System.println("1,");
            System.println("/[$1],");
            System.println("/[$0],");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("};");
            System.println("Runtime.setDefocus(12, 1, ColorScreenParam12);");
        }
        if (this.screen13_on_off) {
            this.ColorScreenParam13[2] = this.Color_Zx[1];
            this.ColorScreenParam13[3] = this.Color_Ax[1] * 0x1000000 + this.Color_Bx[1] * 65536 + this.Color_Gx[1] * 256 + this.Color_Rx[1];
            Runtime.setRegister(0, this.ColorScreenParam13[3]);
            Runtime.setRegister(1, this.ColorScreenParam13[2]);
            System.println("int ColorScreenParam13[] = {");
            System.println("0,");
            System.println("1,");
            System.println("/[$1],");
            System.println("/[$0],");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("};");
            System.println("Runtime.setDefocus(13, 1, ColorScreenParam13);");
        }
        if (this.screen14_on_off) {
            this.ColorScreenParam14[2] = this.Color_Zx[2];
            this.ColorScreenParam14[3] = this.Color_Ax[2] * 0x1000000 + this.Color_Bx[2] * 65536 + this.Color_Gx[2] * 256 + this.Color_Rx[2];
            Runtime.setRegister(0, this.ColorScreenParam14[3]);
            Runtime.setRegister(1, this.ColorScreenParam14[2]);
            System.println("int ColorScreenParam14[] = {");
            System.println("0,");
            System.println("1,");
            System.println("/[$1],");
            System.println("/[$0],");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("};");
            System.println("Runtime.setDefocus(14, 1, ColorScreenParam14);");
        }
        if (this.screen15_on_off) {
            this.ColorScreenParam15[2] = this.Color_Zx[3];
            this.ColorScreenParam15[3] = this.Color_Ax[3] * 0x1000000 + this.Color_Bx[3] * 65536 + this.Color_Gx[3] * 256 + this.Color_Rx[3];
            Runtime.setRegister(0, this.ColorScreenParam15[3]);
            Runtime.setRegister(1, this.ColorScreenParam15[2]);
            System.println("int ColorScreenParam15[] = {");
            System.println("0,");
            System.println("1,");
            System.println("/[$1],");
            System.println("/[$0],");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("0,");
            System.println("};");
            System.println("Runtime.setDefocus(15, 1, ColorScreenParam15);");
        }
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

    void Facechk(Chr chr) {
        System.println("Facechk...");
        this.Facechk_exec = true;
        while (this.Facechk_exec) {
            this.Pad_get();
            if ((this.edge1 & 0x1000) != 0) {
                ++this.Facechk_faceno;
                this.Facechk_mtnno = (this.Facechk_faceno - 1) * 2 + this.Facechk_facemode;
                chr.mtn(this.Facechk_mtnno, this.Facechk_frame, this.Facechk_frame, 0, 8, 1.0f, false);
                chr.start(4, null);
                Runtime.setRegister(0, this.Facechk_faceno);
                System.println("F-/[$0]");
            }
            if ((this.edge1 & 0x4000) != 0) {
                --this.Facechk_faceno;
                if (this.Facechk_faceno == 0) {
                    System.println("No Face!!!");
                    this.Facechk_faceno = 1;
                }
                this.Facechk_mtnno = (this.Facechk_faceno - 1) * 2 + this.Facechk_facemode;
                chr.mtn(this.Facechk_mtnno, this.Facechk_frame, this.Facechk_frame, 0, 8, 1.0f, false);
                chr.start(4, null);
                Runtime.setRegister(0, this.Facechk_faceno);
                System.println("F-/[$0]");
            }
            if ((this.rep1 & 0x2000) != 0) {
                ++this.Facechk_frame;
                if (this.Facechk_frame >= 120) {
                    this.Facechk_frame = 0;
                }
                this.Facechk_mtnno = (this.Facechk_faceno - 1) * 2 + this.Facechk_facemode;
                chr.mtn(this.Facechk_mtnno, this.Facechk_frame, this.Facechk_frame, 0, 8, 1.0f, false);
                chr.start(4, null);
            }
            if ((this.rep1 & 0x8000) != 0) {
                --this.Facechk_frame;
                if (this.Facechk_frame <= 0) {
                    this.Facechk_frame = 120;
                }
                this.Facechk_mtnno = (this.Facechk_faceno - 1) * 2 + this.Facechk_facemode;
                chr.mtn(this.Facechk_mtnno, this.Facechk_frame, this.Facechk_frame, 0, 8, 1.0f, false);
                chr.start(4, null);
            }
            if ((this.edge1 & this._PADL3) != 0) {
                if (this.Facechk_facemode == 1) {
                    System.println("mouth close.");
                    this.Facechk_facemode = 2;
                } else {
                    System.println("mouth open.");
                    this.Facechk_facemode = 1;
                }
                this.Facechk_mtnno = (this.Facechk_faceno - 1) * 2 + this.Facechk_facemode;
                chr.mtn(this.Facechk_mtnno, this.Facechk_frame, this.Facechk_frame, 0, 8, 1.0f, false);
                chr.start(4, null);
            }
            if ((this.edge1 & 0x800) != 0) {
                this.Facechk_exec = false;
            }
            if ((this.edge1 & 0x100) != 0) {
                Runtime.setRegister(0, this.Facechk_faceno);
                Runtime.setRegister(1, this.Facechk_frame);
                Runtime.setRegister(2, this.Facechk_facemode);
                System.println("F-/[$0]/[$2] frame(/[$1])");
            }
            System.sleep(1);
        }
        System.println("Facechk...end.");
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

    void Gno_CallParamSet(Chr chr) {
        this.gnoFilter[0] = this.gno_alpha;
        this.gnoFilter[1] = this.gno_filter;
        this.gnoFilter[2] = this.gno_shake;
        this.gnoFilter[3] = this.gno_ref;
        chr.setFilter(2);
        chr.setFilterParam(this.gnoFilter);
    }

    void Gno_printout() {
        Runtime.setRegister(0, this.gno_alpha);
        Runtime.setRegister(1, this.gno_filter);
        Runtime.setRegister(2, this.gno_shake);
        Runtime.setRegister(3, this.gno_ref);
        System.println("*************ColorScreen Printout*************");
        System.println("gnoFilter[0] = /[#0]f;");
        System.println("gnoFilter[1] = /[#1]f;");
        System.println("gnoFilter[2] = /[#2]f;");
        System.println("gnoFilter[3] = /[#3]f;");
    }

    void Gnochk(Chr chr) {
        this.Gnochk_exec = true;
        System.println("Gnochk...");
        while (this.Gnochk_exec) {
            this.edge1 = this.pad0.getEdge();
            this.btn1 = this.pad0.getButton();
            if ((this.edge1 & 0x100) != 0) {
                this.Gno_printout();
            }
            if ((this.edge1 & 0x800) != 0) {
                this.Gnochk_exec = false;
            }
            if ((this.edge1 & 0x2000) != 0) {
                ++this.gnoptr;
                if (this.gnoptr > 3) {
                    this.gnoptr = 3;
                }
                switch (this.gnoptr) {
                    case 0: {
                        System.println("--alpha--");
                        break;
                    }
                    case 1: {
                        System.println("--filter--");
                        break;
                    }
                    case 2: {
                        System.println("--shake--");
                        break;
                    }
                    case 3: {
                        System.println("--ref_alpha--");
                        break;
                    }
                }
            }
            if ((this.edge1 & 0x8000) != 0) {
                --this.gnoptr;
                if (this.gnoptr < 0) {
                    this.gnoptr = 0;
                }
                switch (this.gnoptr) {
                    case 0: {
                        System.println("--alpha--");
                        break;
                    }
                    case 1: {
                        System.println("--filter--");
                        break;
                    }
                    case 2: {
                        System.println("--shake--");
                        break;
                    }
                    case 3: {
                        System.println("--ref_alpha--");
                        break;
                    }
                }
            }
            if ((this.btn1 & 0x1000) != 0) {
                switch (this.gnoptr) {
                    case 0: {
                        this.gno_alpha += 0.01f;
                        if (this.gno_alpha >= 1.0f) {
                            System.println("alpha max!!");
                            this.gno_alpha = 1.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 1: {
                        this.gno_filter += 0.5f;
                        if (this.gno_filter >= 128.0f) {
                            System.println("filter max!!");
                            this.gno_filter = 128.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 2: {
                        this.gno_shake += 0.5f;
                        if (this.gno_shake >= 128.0f) {
                            System.println("shake max!!");
                            this.gno_shake = 128.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 3: {
                        this.gno_ref += 0.01f;
                        if (this.gno_ref >= 1.0f) {
                            System.println("ref_alpha max!!");
                            this.gno_ref = 1.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                }
            }
            if ((this.btn1 & 0x4000) != 0) {
                switch (this.gnoptr) {
                    case 0: {
                        this.gno_alpha -= 0.01f;
                        if (this.gno_alpha <= 0.0f) {
                            System.println("alpha min!!");
                            this.gno_alpha = 0.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 1: {
                        this.gno_filter -= 0.5f;
                        if (this.gno_filter <= 0.0f) {
                            System.println("filter min!!");
                            this.gno_filter = 0.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 2: {
                        this.gno_shake -= 0.5f;
                        if (this.gno_shake <= 0.0f) {
                            System.println("shake min!!");
                            this.gno_shake = 0.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                    case 3: {
                        this.gno_ref -= 0.01f;
                        if (this.gno_ref <= 0.0f) {
                            System.println("ref-alpha min!!");
                            this.gno_ref = 0.0f;
                        }
                        this.Gno_CallParamSet(chr);
                        break;
                    }
                }
            }
            System.sleep(1);
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

    void MotionPack_srv() {
        this.MotionPack_srv(1000000);
    }

    void MotionPack_srv(int n) {
        this.Timechk_CutChange();
        this.Timechk_sc = 0;
        while (this.Timechk_sc < n) {
            switch (this.Timechk_sc) {
                case 56: {
                    if (!this.snd_chk) {
                        Sound.streamPlay(338001);
                    }
                    this.MSG(33, this.shion.face, 1, "Allen!");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv2(int n) {
        this.Timechk_CutChange();
        this.Timechk_sc = 0;
        while (this.Timechk_sc < n) {
            switch (this.Timechk_sc) {
                case 5: {
                    if (!this.snd_chk) {
                        Sound.streamPlay(338002);
                    }
                    this.MSG(39, this.ziggy.face, 1, "Have you seen MOMO?!");
                    if (!this.snd_chk) {
                        Sound.streamPlay(338003);
                    }
                    this.MSG(135, this.allen.face, 1, "No, she's not back yet?\nI thought she was with you.");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv3(int n) {
        this.Timechk_CutChange();
        this.Timechk_sc = 0;
        while (this.Timechk_sc < n) {
            switch (this.Timechk_sc) {
                case 0: {
                    this.PCextends_start(0, 240, 0.97f);
                    if (!this.snd_chk) {
                        Sound.streamPlay(338004);
                    }
                    this.MSG(176, this.ziggy.face, 1, "She stayed behind to treat\nthe injured, but she wasn't\nthere when we returned.");
                    this.MSG(71, this.ziggy.face, 1, "We thought she might have\ncome back on her own...");
                    this.PCextends_end(n);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv4(int n) {
        this.Timechk_CutChange();
        this.Timechk_sc = 0;
        while (this.Timechk_sc < n) {
            switch (this.Timechk_sc) {
                case 0: {
                    this.PCextends_start(0, 300, 1.0f);
                    break;
                }
                case 2: {
                    if (!this.snd_chk) {
                        Sound.streamPlay(338005);
                    }
                    this.MSG(105, this.jr.face, 1, "Damn, where the hell is she...?");
                    this.MSG(180, this.jr.face, 1, "All right, I'll get our 100-Series to start\nsearching. They shouldn't have any\ntrouble, being the same model and all.");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srv5(int n) {
        this.Timechk_CutChange();
        this.Timechk_sc = 0;
        while (this.Timechk_sc < n) {
            switch (this.Timechk_sc) {
                case 15: {
                    if (!this.snd_chk) {
                        Sound.streamPlay(338006);
                    }
                    this.MSG(164, this.ziggy.face, 1, "Thanks. We've eliminated the Gnosis\nwithin the colony, but there isn't any\nguarantee they won't be back.");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void PCextends_end(int n) {
        this.TotalCutTime = this.Timechk_srv_totaltime - this.TotalCutTime;
        this.Timechk_srv_totaltime = this.NextPCStart;
        this.pc.init(1, 0, this.NextPCStart + 1, n, 1.0f);
        this.pc.start();
        System.println("------------ＭＰ速度変更は正常に終了しました。");
        Runtime.setRegister(0, this.TotalCutTime);
        Runtime.setRegister(1, this.CutNo);
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
        this.TotalCutTime = this.Timechk_srv_totaltime;
        this.BaseCutTime = n2 - n;
        this.pc.init(1, 0, n, n2, f);
        this.pc.start();
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        Runtime.setRegister(2, f);
        System.println("------------ＭＰの速度を変更します(/[$0]-/[$1]) ×/[#2]");
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
        this.timer.signal(this.Timechk_cut);
        ++this.Timechk_cut;
    }

    void Timechk_SceneEnd() {
        this.timer.signal(this.Timechk_cut);
        ++this.Timechk_cut;
        this.CaptureEnd_CaptureTool();
        Runtime.setRegister(0, this.Timechk_srv_totaltime);
        System.println("----------シーンは終了しました。(Total = /[$0])");
    }

    void Timechk_main() {
        this.CutNo = 0;
        this.timer_thread_srv = Thread.create(this, "Timerchk_srv_main");
        while (true) {
            this.Timechk_srv_time = 0;
            this.Timechk_srv_exec = true;
            this.Timerchk_srv();
            System.waitSignal(this.timer, this.CutNo + 1);
            int n = this.Timerchk_srv_end();
            Runtime.setRegister(0, this.Timechk_srv_totaltime);
            Runtime.setRegister(1, this.CutNo);
            Runtime.setRegister(2, n);
            System.println("-----Cut = /[$1] CutTime = /[$2](Total = /[$0])");
            ++this.CutNo;
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
            ++this.Timechk_sc;
            ++this.Timechk_srv_time;
            ++this.Timechk_srv_totaltime;
            System.sleep(1);
        }
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

    void ___wait() {
        int n = 0;
        int n2 = 0;
        do {
            System.sleep(1);
            n = this.pad0.getButton();
            n2 = this.pad1.getButton();
        } while (this.ColorScreenSet_exec || this.ColorScreenSet_exec2 || this.camtool0_objtool1_flag == 2 || (n2 & 0x100) != 0 || (n2 & 0x10) == 0);
        System.sleep(7);
    }

    void __chracter_splne() {
        int n = 0;
        int n2 = 0;
        float f = -1.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        float f4 = 0.0f;
        float f5 = 0.0f;
        float f6 = 0.0f;
        float f7 = 0.0f;
        float f8 = 0.0f;
        while (true) {
            if (this.jisin_input_flag == 1) {
                n2 = this.pad0.getButton();
                n = this.pad1.getButton();
                if ((n & 0x800) != 0) {
                    Runtime.setRegister(0, this.a);
                    Runtime.setRegister(1, this.b);
                    Runtime.setRegister(2, this.c);
                    Runtime.setRegister(3, this.e);
                    Runtime.setRegister(4, this.f);
                    Runtime.setRegister(5, this.g);
                    System.println(" a = /[$0];");
                    System.println(" b = /[$1];");
                    System.println(" c = /[$2];");
                    System.println(" e = /[$3];");
                    System.println(" f = /[$4];");
                    System.println(" g = /[$5];");
                    Runtime.setRegister(0, this.sokge_per);
                    System.println("sokge_per = /[#0]f;");
                    Runtime.setRegister(0, this.bure_py_flag);
                    System.println("bure_py_flag = /[$0];");
                }
                if ((n & 0x400) != 0) {
                    this.bure_py_flag = 0;
                    Runtime.setRegister(0, (float) this.bure_py_flag);
                    System.println("bure_py_flag = /[#0]");
                    System.sleep(4);
                }
                if ((n & 0x200) != 0) {
                    this.bure_py_flag = 1;
                    Runtime.setRegister(0, (float) this.bure_py_flag);
                    System.println("bure_py_flag = /[#0]");
                    System.sleep(4);
                }
                if ((n2 & 0x1000) != 0) {
                    this.sokge_per += 0.05f;
                    if ((n & 0x100) != 0) {
                        this.sokge_per += 0.1f;
                    }
                    if (this.sokge_per > 10.0f) {
                        this.sokge_per = 10.0f;
                    }
                    Runtime.setRegister(0, this.sokge_per);
                    System.println("sokge_per---------------(/[#0])");
                }
                if ((n2 & 0x4000) != 0) {
                    this.sokge_per -= 0.05f;
                    if ((n & 0x100) != 0) {
                        this.sokge_per -= 0.1f;
                    }
                    if (this.sokge_per <= 0.01f) {
                        this.sokge_per = 0.01f;
                    }
                    Runtime.setRegister(0, this.sokge_per);
                    System.println("sokge_per---------------(/[#0])");
                }
                if ((n & 0x1000) != 0) {
                    ++this.b;
                    if ((n & 0x100) != 0) {
                        this.b += 10;
                    }
                    if (this.b > 999) {
                        this.b = 999;
                    }
                    Runtime.setRegister(0, (float) this.b);
                    System.println("b---------------(/[#0])");
                }
                if ((n & 0x8000) != 0) {
                    --this.b;
                    if ((n & 0x100) != 0) {
                        this.b -= 10;
                    }
                    if (this.b <= 0) {
                        this.b = 1;
                    }
                    Runtime.setRegister(0, (float) this.b);
                    System.println("b---------------(/[#0])");
                }
                if ((n & 0x2000) != 0) {
                    ++this.a;
                    if ((n & 0x100) != 0) {
                        this.a += 10;
                    }
                    if (this.a > 999) {
                        this.a = 999;
                    }
                    Runtime.setRegister(0, (float) this.a);
                    System.println("a---------------(/[#0])");
                }
                if ((n & 0x4000) != 0) {
                    --this.a;
                    if ((n & 0x100) != 0) {
                        this.a -= 10;
                    }
                    if (this.a <= 0) {
                        this.a = 1;
                    }
                    Runtime.setRegister(0, (float) this.a);
                    System.println("a---------------(/[#0])");
                }
                if ((n & 1) != 0) {
                    ++this.c;
                    if ((n & 0x100) != 0) {
                        this.c += 10;
                    }
                    if (this.c > 999) {
                        this.c = 999;
                    }
                    Runtime.setRegister(0, (float) this.c);
                    System.println("c---------------(/[#0])");
                }
                if ((n & 4) != 0) {
                    --this.c;
                    if ((n & 0x100) != 0) {
                        this.c -= 10;
                    }
                    if (this.c <= 0) {
                        this.c = 1;
                    }
                    Runtime.setRegister(0, (float) this.c);
                    System.println("c---------------(/[#0])");
                }
                if ((n & 0x10) != 0) {
                    ++this.e;
                    if ((n & 0x100) != 0) {
                        this.e += 10;
                    }
                    if (this.e > 999) {
                        this.e = 999;
                    }
                    Runtime.setRegister(0, (float) this.e);
                    System.println("e---------------(/[#0])");
                }
                if ((n & 0x80) != 0) {
                    --this.e;
                    if ((n & 0x100) != 0) {
                        this.e -= 10;
                    }
                    if (this.e <= 0) {
                        this.e = 1;
                    }
                    Runtime.setRegister(0, (float) this.e);
                    System.println("e---------------(/[#0])");
                }
                if ((n & 0x20) != 0) {
                    ++this.f;
                    if ((n & 0x100) != 0) {
                        this.f += 10;
                    }
                    if (this.f > 999) {
                        this.f = 999;
                    }
                    Runtime.setRegister(0, (float) this.f);
                    System.println("f---------------(/[#0])");
                }
                if ((n & 0x40) != 0) {
                    --this.f;
                    if ((n & 0x100) != 0) {
                        this.f -= 10;
                    }
                    if (this.f <= 0) {
                        this.f = 1;
                    }
                    Runtime.setRegister(0, (float) this.f);
                    System.println("f---------------(/[#0])");
                }
                if ((n & 2) != 0) {
                    ++this.g;
                    if ((n & 0x100) != 0) {
                        this.g += 10;
                    }
                    if (this.g > 999) {
                        this.g = 999;
                    }
                    Runtime.setRegister(0, (float) this.g);
                    System.println("g---------------(/[#0])");
                }
                if ((n & 8) != 0) {
                    --this.g;
                    if ((n & 0x100) != 0) {
                        this.g -= 10;
                    }
                    if (this.g <= 0) {
                        this.g = 1;
                    }
                    Runtime.setRegister(0, (float) this.g);
                    System.println("g---------------(/[#0])");
                }
            }
            if (this.jisin_flag == 1) {
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = (f8 % (float) this.a + (f8 - (float) ((int) f8))) / 1000.0f;
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f3 = (f8 % (float) this.b + (f8 - (float) ((int) f8))) / 1000.0f;
                if (this.bure_py_flag == 1) {
                    f3 = (f3 + (float) this.b / 1000.0f * this.sokge_per) * f;
                }
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f4 = (f8 % (float) this.c + (f8 - (float) ((int) f8))) / 1000.0f;
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f5 = (f8 % (float) this.e + (f8 - (float) ((int) f8))) / 100.0f;
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f6 = (f8 % (float) this.f + (f8 - (float) ((int) f8))) / 100.0f;
                f8 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f7 = (f8 % (float) this.g + (f8 - (float) ((int) f8))) / 100.0f;
                this.cam5.setTranslate(this.cam1.getTranslateX() + f2, this.cam1.getTranslateY() + f3, this.cam1.getTranslateZ() + f4);
                this.cam5.setRotate(this.cam1.getRotateX() + f5, this.cam1.getRotateY() + f6, this.cam1.getRotateZ() + f7);
                this.cam5.setFov(this.cam1.getFov());
                f = f == -1.0f ? 1.0f : -1.0f;
            }
            System.sleep(1);
        }
    }

    void __set_position() {
        float[] fArray = new float[72];
        float[] fArray2 = new float[72];
        float[] fArray3 = new float[36];
        this.gold = this.shion;
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
                    this.Gnochk(this.gold);
                }
                if ((n2 & 0x20) != 0) {
                    System.println("-------------* ColorScreenSet() *-----------");
                    this.ColorScreen_flag = 0;
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    System.sleep(7);
                    this.ColorScreenSet1();
                }
                if ((n2 & 4) != 0) {
                    System.println("-------------* ColorScreenSet()_temae *-----------");
                    this.ColorScreen_flag = 1;
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    System.sleep(7);
                    this.ColorScreenSet1();
                }
                if ((n2 & 0x200) != 0) {
                    System.println("-------------* ColorScreenSet_No.2() *-----------");
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    System.sleep(7);
                    this.ColorScreenSet2();
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
                        if (this.kind_c0_u1_e2 == 0) {
                            this.obj_scale = this.gold.getScale();
                            f4 = this.obj_scale.x;
                            f5 = this.obj_scale.y;
                            f6 = this.obj_scale.z;
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.obj_scale = this.gold_unit.getScale();
                            f4 = this.obj_scale.x;
                            f5 = this.obj_scale.y;
                            f6 = this.obj_scale.z;
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.obj_scale = this.gold_eft.getScale();
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
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setScale(f3 += f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit.setScale(f3 += f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft.setScale(f3 += f7 / 100.0f, f3, f3);
                    }
                    Runtime.setRegister(0, f3);
                    System.println("one_scale-----------------/[#0]");
                }
                if ((n & 0x4000) != 0) {
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setScale(f3 -= f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit.setScale(f3 -= f7 / 100.0f, f3, f3);
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft.setScale(f3 -= f7 / 100.0f, f3, f3);
                    }
                    Runtime.setRegister(0, f3);
                    System.println("one_scale-----------------/[#0]");
                }
                if ((n & 0x100) != 0) {
                    if ((n & 0x20) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4, f5 += f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4, f5 += f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4, f5 += f7 / 100.0f, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 0x40) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4, f5 -= f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4, f5 -= f7 / 100.0f, f6);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4, f5 -= f7 / 100.0f, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 1) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4 += f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4 += f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4 += f7 / 100.0f, f5, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 4) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4 -= f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4 -= f7 / 100.0f, f5, f6);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4 -= f7 / 100.0f, f5, f6);
                        }
                        n6 = 27;
                    }
                    if ((n & 2) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4, f5, f6 += f7 / 100.0f);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4, f5, f6 += f7 / 100.0f);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4, f5, f6 += f7 / 100.0f);
                        }
                        n6 = 27;
                    }
                    if ((n & 8) != 0) {
                        if (this.kind_c0_u1_e2 == 0) {
                            this.gold.setScale(f4, f5, f6 -= f7 / 100.0f);
                        }
                        if (this.kind_c0_u1_e2 == 1) {
                            this.gold_unit.setScale(f4, f5, f6 -= f7 / 100.0f);
                        }
                        if (this.kind_c0_u1_e2 == 2) {
                            this.gold_eft.setScale(f4, f5, f6 -= f7 / 100.0f);
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
                if ((n2 & 0x100) != 0 && this.kind_c0_u1_e2 == 1) {
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
                            if (this.kind_c0_u1_e2 == 0) {
                                this.gold.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.gold.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                Runtime.setRegister(0, (float) n8);
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("gold-no-/[#0]|view-/[#1]|cap-/[#2]");
                            } else if (this.kind_c0_u1_e2 == 1) {
                                this.gold_unit.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.gold_unit.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                Runtime.setRegister(0, (float) n8);
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("gold-unit-no-/[#0]|view-/[#1]|cap-/[#2]");
                            } else if (this.kind_c0_u1_e2 == 2) {
                                this.gold_eft.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                this.gold_eft.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                Runtime.setRegister(0, (float) n8);
                                Runtime.setRegister(1, (float) n11);
                                Runtime.setRegister(2, (float) n12);
                                System.println("gold-eft-no-/[#0]|view-/[#1]|cap-/[#2]");
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
                            if (this.kind_c0_u1_e2 == 0) {
                                fArray[n11 * 4 + 1] = this.gold.px;
                                fArray[n11 * 4 + 2] = this.gold.py;
                                fArray[n11 * 4 + 3] = this.gold.pz;
                                fArray2[n11 * 4 + 1] = this.gold.rx;
                                fArray2[n11 * 4 + 2] = this.gold.ry;
                                fArray2[n11 * 4 + 3] = this.gold.rz;
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (this.kind_c0_u1_e2 == 1) {
                                fArray[n11 * 4 + 1] = this.gold_unit.px;
                                fArray[n11 * 4 + 2] = this.gold_unit.py;
                                fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                fArray3[n11 * 2 + 1] = this.cam0.getFov();
                            } else if (this.kind_c0_u1_e2 == 2) {
                                fArray[n11 * 4 + 1] = this.gold_eft.px;
                                fArray[n11 * 4 + 2] = this.gold_eft.py;
                                fArray[n11 * 4 + 3] = this.gold_eft.pz;
                                fArray2[n11 * 4 + 1] = this.gold_eft.rx;
                                fArray2[n11 * 4 + 2] = this.gold_eft.ry;
                                fArray2[n11 * 4 + 3] = this.gold_eft.rz;
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    this.gold.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                    this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    this.gold_unit.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                    this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1_e2 == 2) {
                                    this.gold_eft.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                    this.gold_eft.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    this.gold.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                    this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    this.gold_unit.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                    this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                } else if (this.kind_c0_u1_e2 == 2) {
                                    this.gold_eft.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                    this.gold_eft.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 2) {
                                    fArray[n11 * 4 + 1] = this.gold_eft.px;
                                    fArray[n11 * 4 + 2] = this.gold_eft.py;
                                    fArray[n11 * 4 + 3] = this.gold_eft.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_eft.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_eft.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_eft.rz;
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    this.gold.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                    this.gold.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                    Runtime.setRegister(0, (float) n8);
                                    Runtime.setRegister(1, (float) n11);
                                    Runtime.setRegister(2, (float) n12);
                                    System.println("gold-no-/[#0]|view-/[#1]|cap-/[#2]");
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    this.gold_unit.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                    this.gold_unit.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                    Runtime.setRegister(0, (float) n8);
                                    Runtime.setRegister(1, (float) n11);
                                    Runtime.setRegister(2, (float) n12);
                                    System.println("gold-unit-no-/[#0]|view-/[#1]|cap-/[#2]");
                                } else if (this.kind_c0_u1_e2 == 2) {
                                    this.gold_eft.setTranslate(fArray[n11 * 4 + 1], fArray[n11 * 4 + 2], fArray[n11 * 4 + 3]);
                                    this.gold_eft.setRotate(fArray2[n11 * 4 + 1], fArray2[n11 * 4 + 2], fArray2[n11 * 4 + 3]);
                                    Runtime.setRegister(0, (float) n8);
                                    Runtime.setRegister(1, (float) n11);
                                    Runtime.setRegister(2, (float) n12);
                                    System.println("gold-eft-no-/[#0]|view-/[#1]|cap-/[#2]");
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_eft.px;
                                    fArray[n11 * 4 + 2] = this.gold_eft.py;
                                    fArray[n11 * 4 + 3] = this.gold_eft.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_eft.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_eft.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_eft.rz;
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
                                if (this.kind_c0_u1_e2 == 0) {
                                    fArray[n11 * 4 + 1] = this.gold.px;
                                    fArray[n11 * 4 + 2] = this.gold.py;
                                    fArray[n11 * 4 + 3] = this.gold.pz;
                                    fArray2[n11 * 4 + 1] = this.gold.rx;
                                    fArray2[n11 * 4 + 2] = this.gold.ry;
                                    fArray2[n11 * 4 + 3] = this.gold.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_unit.px;
                                    fArray[n11 * 4 + 2] = this.gold_unit.py;
                                    fArray[n11 * 4 + 3] = this.gold_unit.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_unit.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_unit.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_unit.rz;
                                    fArray3[n11 * 2 + 1] = this.cam0.getFov();
                                } else if (this.kind_c0_u1_e2 == 1) {
                                    fArray[n11 * 4 + 1] = this.gold_eft.px;
                                    fArray[n11 * 4 + 2] = this.gold_eft.py;
                                    fArray[n11 * 4 + 3] = this.gold_eft.pz;
                                    fArray2[n11 * 4 + 1] = this.gold_eft.rx;
                                    fArray2[n11 * 4 + 2] = this.gold_eft.ry;
                                    fArray2[n11 * 4 + 3] = this.gold_eft.rz;
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
                                    if (this.kind_c0_u1_e2 == 0) {
                                        this.gold.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                        this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1_e2 == 1) {
                                        this.gold_unit.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                        this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1_e2 == 2) {
                                        this.gold_eft.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                                        this.gold_eft.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
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
                                    if (this.kind_c0_u1_e2 == 0) {
                                        this.gold.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                        this.gold.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1_e2 == 1) {
                                        this.gold_unit.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                        this.gold_unit.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
                                    } else if (this.kind_c0_u1_e2 == 2) {
                                        this.gold_eft.setTranslate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                                        this.gold_eft.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
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
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 2) != 0) {
                    this.rb_rx -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 4) != 0) {
                    this.rb_rz += 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 1) != 0) {
                    this.rb_rz -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 0x80) != 0) {
                    this.rb_ry += 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 0x20) != 0) {
                    this.rb_ry -= 2.5f * (float) n9 / 150.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                n6 = 1;
            }
            if (n6 == 0 && (n & 0x100) != 0) {
                if ((n & 0x200) != 0) {
                    this.rb_y += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 0x400) != 0) {
                    this.rb_y -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 8) != 0) {
                    this.rb_x += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 2) != 0) {
                    this.rb_x -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 4) != 0) {
                    this.rb_z += 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 1) != 0) {
                    this.rb_z -= 0.025f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setTranslate(this.rb_x, this.rb_y, this.rb_z);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_translate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_translate();
                    }
                    if (bl2) {
                        this.light.setGlobalPointLightPos(0, this.rb_x, this.rb_y, this.rb_z);
                    }
                }
                if ((n & 0x80) != 0) {
                    this.rb_ry += 2.5f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
                    }
                }
                if ((n & 0x20) != 0) {
                    this.rb_ry -= 2.5f * (float) n9 / 100.0f;
                    if (this.kind_c0_u1_e2 == 0) {
                        this.gold.setRotate(this.rb_rx, this.rb_ry, this.rb_rz);
                    }
                    if (this.kind_c0_u1_e2 == 1) {
                        this.gold_unit_set_rotate();
                    }
                    if (this.kind_c0_u1_e2 == 2) {
                        this.gold_eft_set_rotate();
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
                if (++n10 == 4) {
                    n10 = 3;
                }
                this.hand_cam0_shift(n10);
            }
            if (n7 <= 3 || n6 != 0 || (n & 0x40) == 0) continue;
            if ((n & 0x20) != 0) {
                if (++n8 > 1) {
                    n8 = 0;
                }
                n6 = 1;
            }
            if ((n & 0x80) != 0) {
                if (--n8 < 0) {
                    n8 = 1;
                }
                n6 = 1;
            }
            if (n6 != 1) continue;
            n7 = 0;
            this.gold_Visible_true();
            switch (n8) {
                case 0: {
                    this.kind_c0_u1_e2 = 0;
                    this.gold = this.ziggy;
                    break;
                }
                case 1: {
                    this.kind_c0_u1_e2 = 0;
                    this.gold = this.kosmos;
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
        while (this.__wait_loop_flag == 1) {
            System.sleep(1);
            n = this.pad0.getButton();
            n2 = this.pad1.getButton();
            if (this.ColorScreenSet_exec || this.ColorScreenSet_exec2 || this.camtool0_objtool1_flag == 2 || (n2 & 0x100) != 0 || (n2 & 0x10) == 0)
                continue;
            System.sleep(7);
            break;
        }
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
            if (this.kind_c0_u1_e2 == 1) {
                this.gold_unit.setVisible(n3, false);
            } else {
                this.gold.setVisible(n3, false);
            }
            if ((n2 & 0x10) != 0) {
                ++n3;
            }
            System.sleep(1);
            if (this.kind_c0_u1_e2 == 1) {
                this.gold_unit.setVisible(n3, true);
                continue;
            }
            this.gold.setVisible(n3, true);
        } while (!bl);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03038_F");
        Runtime.setFlags(363, 1, 1);
        System.println("XEVEJNAME:SCE03039A");
        Runtime.jumpEvent(3390);
    }

    public void cleanupOriginal() {
    }

    int floatDowner(float f) {
        float f2 = this.Abs(f * 100000.0f);
        return (int) f2;
    }

    int floatUpper(float f) {
        return (int) f;
    }

    void gold_Visible_false() {
        if (this.kind_c0_u1_e2 == 0) {
            this.gold.setVisible(false);
        }
        if (this.kind_c0_u1_e2 == 1) {
            this.gold_unit.py += 100.0f;
            this.gold_unit.setTranslate();
            this.gold_unit.py -= 100.0f;
        }
        if (this.kind_c0_u1_e2 == 2) {
            this.gold_eft.py += 100.0f;
            this.gold_eft.setTranslate();
            this.gold_eft.py -= 100.0f;
        }
    }

    void gold_Visible_true() {
        if (this.kind_c0_u1_e2 == 0) {
            this.gold.setVisible(true);
        }
        if (this.kind_c0_u1_e2 == 1) {
            this.gold_unit.setTranslate();
        }
        if (this.kind_c0_u1_e2 == 2) {
            this.gold_eft.setTranslate();
        }
    }

    void gold_eft_set_rotate() {
        this.gold_eft.rx = this.rb_rx;
        this.gold_eft.ry = this.rb_ry;
        this.gold_eft.rz = this.rb_rz;
        this.gold_eft.setRotate();
    }

    void gold_eft_set_translate() {
        this.gold_eft.px = this.rb_x;
        this.gold_eft.py = this.rb_y;
        this.gold_eft.pz = this.rb_z;
        this.gold_eft.setTranslate();
    }

    void gold_get_iti() {
        if (this.kind_c0_u1_e2 == 0) {
            this.rb_x = this.gold.px;
            this.rb_y = this.gold.py;
            this.rb_z = this.gold.pz;
            this.rb_rx = this.gold.rx;
            this.rb_ry = this.gold.ry;
            this.rb_rz = this.gold.rz;
        }
        if (this.kind_c0_u1_e2 == 1) {
            this.rb_x = this.gold_unit.px;
            this.rb_y = this.gold_unit.py;
            this.rb_z = this.gold_unit.pz;
            this.rb_rx = this.gold_unit.rx;
            this.rb_ry = this.gold_unit.ry;
            this.rb_rz = this.gold_unit.rz;
        }
        if (this.kind_c0_u1_e2 == 2) {
            this.rb_x = this.gold_eft.px;
            this.rb_y = this.gold_eft.py;
            this.rb_z = this.gold_eft.pz;
            this.rb_rx = this.gold_eft.rx;
            this.rb_ry = this.gold_eft.ry;
            this.rb_rz = this.gold_eft.rz;
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
        if (n == 3) {
            System.println("!!__wait_loop_flag_________on!!");
            System.sleep(3);
            this.cam1.change();
            this.__wait_loop_flag = 0;
        }
        if (n == 2) {
            System.println("!!__wait_loop_flag_________off!!");
            System.sleep(2);
            this.cam1.change();
            this.__wait_loop_flag = 1;
        }
        if (n == 1) {
            System.println("!!__wait_loop_flag_________off!!");
            System.sleep(3);
            this.cam_copy(this.cam0, this.cam1);
            this.cam0.change();
            this.__wait_loop_flag = 1;
        }
        if (n == 0) {
            System.println("!!__wait_loop_flag_________off!!");
            System.sleep(3);
            this.cam_copy(this.cam0, this.cam2);
            this.cam0.change();
            this.__wait_loop_flag = 1;
        }
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
        Runtime.setLocation(804);
        Stage.renderCommand(4);
        System.methodSignal(1);
        this.cam0.change();
        this.pc = PlayControl.create();
        this.pc.loadCamera("c03s38cut01.cam");
        this.pc.init(1, 4);
        this.allen = new Character(0x1000107, 0.0f, 0.0f, 0.0f, 0.0f);
        this.boy_a = new Character2(1561, 0.0f, 0.0f, 0.0f, 0.0f);
        this.girl_b = new Character2(1594, 0.0f, 0.0f, 0.0f, 0.0f);
        this.jiji_b = new Character2(1552, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kidsm_a = new Character2(1555, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kidsm_b = new Character2(1558, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kidsw_b = new Character2(1588, 0.0f, 0.0f, 0.0f, 0.0f);
        this.man_b = new Character2(1540, 0.0f, 0.0f, 0.0f, 0.0f);
        this.oji_b = new Character2(1546, 0.0f, 0.0f, 0.0f, 0.0f);
        this.wman_b = new Character2(1570, 0.0f, 0.0f, 0.0f, 0.0f);
        this.boy_a.renderCommand(22);
        this.girl_b.renderCommand(22);
        this.jiji_b.renderCommand(22);
        this.kidsm_a.renderCommand(22);
        this.kidsm_b.renderCommand(22);
        this.kidsw_b.renderCommand(22);
        this.man_b.renderCommand(22);
        this.oji_b.renderCommand(22);
        this.wman_b.renderCommand(22);
        this.boy_a.renderCommand(512);
        this.girl_b.renderCommand(512);
        this.jiji_b.renderCommand(512);
        this.kidsm_a.renderCommand(512);
        this.kidsm_b.renderCommand(512);
        this.kidsw_b.renderCommand(512);
        this.man_b.renderCommand(512);
        this.oji_b.renderCommand(512);
        this.wman_b.renderCommand(512);
        this.jr = new Character(0x1000005, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shion = new Character(0x1000001, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ziggy = new Character(0x1000006, 0.0f, 0.0f, 0.0f, 0.0f);
        this.chaos = new Character2(32, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kosmos = new Character2(2, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kosmos.setTranslate(0.86f, 0.0f, 0.41f);
        this.kosmos.setRotate(0.0f, 0.0f, 0.0f);
        this.chaos.setTranslate(-4.47f, 0.0f, 2.98f);
        this.chaos.setRotate(0.0f, 9.47f, 0.0f);
        this.box = new Character2(24633, 0.0f, 0.0f, 0.0f, 0.0f);
        this.jr.setVisible(false);
        this.shion.setVisible(false);
        this.ziggy.setVisible(false);
        this.chaos.setVisible(false);
        this.kosmos.setVisible(false);
        this.shion.setMotionFlags(0x2000000, true);
        this.jr.setMotionFlags(0x2000000, true);
        this.chaos.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.allen.setShadow(9, 16);
    }

    void initialize() {
    }

    void lightSPL(int n, int n2) {
        if (this.lightcount != 0) {
            this.lightSPL_thread.stop();
        }
        this.lightcount = n;
        this.light0data[0] = 0.0f;
        this.light0data[1] = this.light0_x;
        this.light0data[2] = this.light0_y;
        this.light0data[3] = this.light0_z;
        this.light0data[4] = n;
        this.light0data[5] = this.tlight0_x;
        this.light0data[6] = this.tlight0_y;
        this.light0data[7] = this.tlight0_z;
        this.light1dir[0] = 0.0f;
        this.light1dir[1] = this.light1_dx;
        this.light1dir[2] = this.light1_dy;
        this.light1dir[3] = this.light1_dz;
        this.light1dir[4] = n;
        this.light1dir[5] = this.tlight1_dx;
        this.light1dir[6] = this.tlight1_dy;
        this.light1dir[7] = this.tlight1_dz;
        this.light1col[0] = 0.0f;
        this.light1col[1] = this.light1_rx;
        this.light1col[2] = this.light1_ry;
        this.light1col[3] = this.light1_rz;
        this.light1col[4] = n;
        this.light1col[5] = this.tlight1_rx;
        this.light1col[6] = this.tlight1_ry;
        this.light1col[7] = this.tlight1_rz;
        this.light2dir[0] = 0.0f;
        this.light2dir[1] = this.light2_dx;
        this.light2dir[2] = this.light2_dy;
        this.light2dir[3] = this.light2_dz;
        this.light2dir[4] = n;
        this.light2dir[5] = this.tlight2_dx;
        this.light2dir[6] = this.tlight2_dy;
        this.light2dir[7] = this.tlight2_dz;
        this.light2col[0] = 0.0f;
        this.light2col[1] = this.light2_rx;
        this.light2col[2] = this.light2_ry;
        this.light2col[3] = this.light2_rz;
        this.light2col[4] = n;
        this.light2col[5] = this.tlight2_rx;
        this.light2col[6] = this.tlight2_ry;
        this.light2col[7] = this.tlight2_rz;
        this.light3dir[0] = 0.0f;
        this.light3dir[1] = this.light3_dx;
        this.light3dir[2] = this.light3_dy;
        this.light3dir[3] = this.light3_dz;
        this.light3dir[4] = n;
        this.light3dir[5] = this.tlight3_dx;
        this.light3dir[6] = this.tlight3_dy;
        this.light3dir[7] = this.tlight3_dz;
        this.light3col[0] = 0.0f;
        this.light3col[1] = this.light3_rx;
        this.light3col[2] = this.light3_ry;
        this.light3col[3] = this.light3_rz;
        this.light3col[4] = n;
        this.light3col[5] = this.tlight3_rx;
        this.light3col[6] = this.tlight3_ry;
        this.light3col[7] = this.tlight3_rz;
        this.lightwork0.dirSPL.setCtrlVertex(this.light0data, 0, n2 + 16, n);
        this.lightwork0.colSPL.setCtrlVertex(this.light0data, 0, n2 + 16, n);
        this.lightwork1.dirSPL.setCtrlVertex(this.light1dir, 0, n2 + 16, n);
        this.lightwork1.colSPL.setCtrlVertex(this.light1col, 0, n2 + 16, n);
        this.lightwork2.dirSPL.setCtrlVertex(this.light2dir, 0, n2 + 16, n);
        this.lightwork2.colSPL.setCtrlVertex(this.light2col, 0, n2 + 16, n);
        this.lightwork3.dirSPL.setCtrlVertex(this.light3dir, 0, n2 + 16, n);
        this.lightwork3.colSPL.setCtrlVertex(this.light3col, 0, n2 + 16, n);
        this.lightwork0.start(1, "spl");
        this.lightwork1.start(1, "spl");
        this.lightwork2.start(1, "spl");
        this.lightwork3.start(1, "spl");
        this.lightSPL_thread.start();
    }

    void lightSPL_init() {
        this.lightwork0 = new Lightwork();
        this.lightwork0.init(1, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork0.setVisible(false);
        this.lightwork1 = new Lightwork();
        this.lightwork1.init(1, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork1.setVisible(false);
        this.lightwork2 = new Lightwork();
        this.lightwork2.init(1, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork2.setVisible(false);
        this.lightwork3 = new Lightwork();
        this.lightwork3.init(1, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork3.setVisible(false);
    }

    void lightSPL_main() {
        System.println("lightSPL start.");
        while (this.lightcount != 0) {
            this.lightwork0.getTranslate();
            this.lightwork1.getTranslate();
            this.lightwork1.getRotate();
            this.lightwork2.getTranslate();
            this.lightwork2.getRotate();
            this.lightwork3.getTranslate();
            this.lightwork3.getRotate();
            this.CameraTool_light.setColor(0, this.lightwork0.px, this.lightwork0.py, this.lightwork0.pz);
            this.CameraTool_light.setColor(1, this.lightwork1.px, this.lightwork1.py, this.lightwork1.pz);
            this.CameraTool_light.setDirection2(1, this.lightwork1.rx, this.lightwork1.ry, this.lightwork1.rz);
            this.CameraTool_light.setColor(2, this.lightwork2.px, this.lightwork2.py, this.lightwork2.pz);
            this.CameraTool_light.setDirection2(2, this.lightwork2.rx, this.lightwork2.ry, this.lightwork2.rz);
            this.CameraTool_light.setColor(3, this.lightwork3.px, this.lightwork3.py, this.lightwork3.pz);
            this.CameraTool_light.setDirection2(3, this.lightwork3.rx, this.lightwork3.ry, this.lightwork3.rz);
            System.sleep(1);
            --this.lightcount;
        }
        System.println("lightSPL end.");
    }

    void lightsetColor(int n, float f, float f2, float f3) {
        switch (n) {
            case 0: {
                this.light0_x = f;
                this.light0_y = f2;
                this.light0_z = f3;
                break;
            }
            case 1: {
                this.light1_dx = f;
                this.light1_dy = f2;
                this.light1_dz = f3;
                break;
            }
            case 2: {
                this.light2_dx = f;
                this.light2_dy = f2;
                this.light2_dz = f3;
                break;
            }
            case 3: {
                this.light3_dx = f;
                this.light3_dy = f2;
                this.light3_dz = f3;
                break;
            }
        }
    }

    void lightsetDirection2(int n, float f, float f2, float f3) {
        switch (n) {
            case 1: {
                this.light1_rx = f;
                this.light1_ry = f2;
                this.light1_rz = f3;
                break;
            }
            case 2: {
                this.light2_rx = f;
                this.light2_ry = f2;
                this.light2_rz = f3;
                break;
            }
            case 3: {
                this.light3_rx = f;
                this.light3_ry = f2;
                this.light3_rz = f3;
                break;
            }
        }
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
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.jr.face, "FLSjr.fpk");
        this.loadarc(this.shion.face, "FLSshion.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.Timechk();
        Sound.streamPlay(1390075, 48000);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.55f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.15f, 0.664f, 0.732f);
        this.light.setColor(2, 0.36f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.747f, 0.472f, -0.468f);
        this.light.setColor(3, 0.27f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.247f, -0.663f, 0.707f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosmos.look_char(this.allen);
        this.chaos.look_char(this.allen);
        this.allen.mtn(258, 0, 1.0f, true);
        this.boy_a.mtn(259, 0, 1.0f, true);
        this.girl_b.mtn(260, 0, 1.0f, true);
        this.jiji_b.mtn(261, 0, 1, 0, 0, 1.0f, true);
        this.kidsm_a.mtn(262, 0, 1.0f, true);
        this.kidsm_b.mtn(263, 0, 1.0f, true);
        this.kidsw_b.mtn(264, 0, 1.0f, true);
        this.man_b.mtn(265, 0, 1.0f, true);
        this.oji_b.mtn(266, 0, 1.0f, true);
        this.wman_b.mtn(268, 0, 1.0f, true);
        this.box.mtn(257, 0, 1.0f, true);
        this.allen.start(5, null);
        this.boy_a.start(5, null);
        this.girl_b.start(5, null);
        this.jiji_b.start(5, null);
        this.kidsm_a.start(5, null);
        this.kidsm_b.start(5, null);
        this.kidsw_b.start(5, null);
        this.man_b.start(5, null);
        this.oji_b.start(5, null);
        this.wman_b.start(5, null);
        this.box.start(5, null);
        this.pc.start();
        this.MotionPack_srv(90);
        this.pc.stop();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.55f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.725f, 0.465f, 0.508f);
        this.light.setColor(2, 0.36f, 0.4f, 0.4f);
        this.light.setDirection2(2, -0.385f, 0.474f, -0.792f);
        this.light.setColor(3, 0.27f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.572f, -0.65f, -0.499f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        float[] fArray = new float[]{1.0f, 11.65f, 1.76f, 28.11f, 64.0f, 11.65f, 1.76f, 28.11f, 127.0f, 11.65f, 1.76f, 28.11f};
        float[] fArray2 = new float[12];
        fArray2[0] = 1.0f;
        fArray2[1] = -9.84f;
        fArray2[2] = -128.07f;
        fArray2[4] = 64.0f;
        fArray2[5] = -10.47f;
        fArray2[6] = -125.27f;
        fArray2[8] = 127.0f;
        fArray2[9] = -10.82f;
        fArray2[10] = -124.24f;
        float[] fArray3 = fArray2;
        this.boy_a.setVisible(false);
        this.girl_b.setVisible(false);
        this.jiji_b.setVisible(false);
        this.kidsm_a.setVisible(false);
        this.kidsm_b.setVisible(false);
        this.kidsw_b.setVisible(false);
        this.man_b.setVisible(false);
        this.oji_b.setVisible(false);
        this.wman_b.setVisible(false);
        this.jr.setVisible(true);
        this.shion.setVisible(true);
        this.ziggy.setVisible(true);
        this.chaos.setVisible(true);
        this.kosmos.setVisible(true);
        this.allen.start(1, "allen_act2");
        this.jr.mtn(271, 0, 1.0f, true);
        this.shion.mtn(273, 0, 1.0f, true);
        this.ziggy.mtn(274, 0, 1.0f, true);
        this.box.mtn(269, 0, 1.0f, true);
        this.kosmos.mtn(274, 0, 0.6f, true);
        this.chaos.setTranslate(-4.42f, 0.0f, 3.66f);
        this.chaos.setRotate(0.0f, 9.47f, 0.0f);
        this.chaos.mtn(271, 0, 0.7f, true);
        this.box.start(5, null);
        this.cam2.transSPL(fArray, 1);
        this.cam2.rotateSPL(fArray3, 1);
        this.cam2.setFov(29.78f);
        this.cam2.change();
        this.MotionPack_srv2(180);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.55f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.816f, 0.57f, -0.092f);
        this.light.setColor(2, 0.36f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.491f, 0.503f, -0.712f);
        this.light.setColor(3, 0.27f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.306f, -0.637f, -0.707f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.setVisible(false);
        this.kosmos.setTranslate(0.6f, 0.0f, 0.41f);
        this.kosmos.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.change();
        this.pc.loadCamera("c03s38cut03.cam");
        this.pc.init(1, 0);
        this.shion.mtn(276, 0, 1.0f, true);
        this.ziggy.mtn(277, 0, 1.0f, true);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.pc.start();
        this.MotionPack_srv3(247);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.55f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.816f, 0.57f, -0.092f);
        this.light.setColor(2, 0.36f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.491f, 0.503f, -0.712f);
        this.light.setColor(3, 0.27f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.306f, -0.637f, -0.707f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosmos.setVisible(false);
        this.chaos.setTranslate(-4.01f, 0.0f, 3.81f);
        this.chaos.setRotate(0.0f, 9.47f, 0.0f);
        this.chaos.look_speed(0.05f);
        this.chaos.look_char(this.jr);
        this.pc.loadCamera("c03s38cut04.cam");
        this.pc.init(1, 0);
        this.jr.mtn(278, 0, 0.97f, true);
        this.ziggy.start(1, "ziggy_act4");
        this.jr.start(5, null);
        this.ziggy.start(5, null);
        this.pc.start();
        this.MotionPack_srv4(300);
        this.pc.stop();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.55f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.874f, 0.47f, -0.119f);
        this.light.setColor(2, 0.36f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.491f, 0.502f, -0.712f);
        this.light.setColor(3, 0.27f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.306f, -0.637f, -0.707f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.chaos.setVisible(false);
        this.jr.setVisible(false);
        this.ziggy.start(1, "ziggy_act5");
        this.cam2.setTranslate(13.34f, 1.49f, 29.32f);
        this.cam2.setRotate(13.46f, -147.43f, 0.0f);
        this.cam2.setFov(25.0f);
        this.cam2.change();
        this.MotionPack_srv5(180);
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

    void sFACE(int n, Chr chr, int n2) {
        chr.mtn(n2, 0, 120, 5, 9, 1.0f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 5, 9, 1.0f, false);
        chr.start(4, null);
    }

    void sFACE(Chr chr, int n) {
        chr.mtn(n, 9, 1.0f, false);
        chr.start(4, null);
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

    void tlightsetColor(int n, float f, float f2, float f3) {
        switch (n) {
            case 0: {
                this.tlight0_x = f;
                this.tlight0_y = f2;
                this.tlight0_z = f3;
                break;
            }
            case 1: {
                this.tlight1_dx = f;
                this.tlight1_dy = f2;
                this.tlight1_dz = f3;
                break;
            }
            case 2: {
                this.tlight2_dx = f;
                this.tlight2_dy = f2;
                this.tlight2_dz = f3;
                break;
            }
            case 3: {
                this.tlight3_dx = f;
                this.tlight3_dy = f2;
                this.tlight3_dz = f3;
                break;
            }
        }
    }

    void tlightsetDirection2(int n, float f, float f2, float f3) {
        switch (n) {
            case 1: {
                this.tlight1_rx = f;
                this.tlight1_ry = f2;
                this.tlight1_rz = f3;
                break;
            }
            case 2: {
                this.tlight2_rx = f;
                this.tlight2_ry = f2;
                this.tlight2_rz = f3;
                break;
            }
            case 3: {
                this.tlight3_rx = f;
                this.tlight3_ry = f2;
                this.tlight3_rz = f3;
                break;
            }
        }
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Character
            extends Chr {
        Chr face;

        public Character(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.renderCommand(534);
            this.setMotionFlags(0x400000, false);
            this.setShadow(0, 0);
        }

        void allen_act2() {
            SCE03038.this.allen.mtn(270, 0, 1.0f, true);
        }

        void ziggy_act4() {
            SCE03038.this.ziggy.mtn(280, 8, 0.97f, true);
        }

        void ziggy_act5() {
            SCE03038.this.ziggy.mtn(282, 0, 0.9f, true);
        }
    }

    class Character2
            extends Chr {
        public Character2(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.renderCommand(534);
            this.setMotionFlags(0x400000, false);
            this.setShadow(0, 0);
        }
    }

    class Timer
            extends Chr {
        Timer() {
        }

        void init() {
            this.init(1, 0.0f, 0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }
    }

    class Lightwork
            extends Unit {
        Spline dirSPL = Spline.create();
        Spline colSPL = Spline.create();

        Lightwork() {
        }

        void spl() {
            this.move(this.dirSPL, false);
            this.rotate(this.colSPL, true);
        }
    }
}

