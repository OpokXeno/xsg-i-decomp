import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_DYU07_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.util.Vector4f;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE03002
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03002,
        MC_DYU07_PRJ,
        JNT_Human,
        FLSshion_h,
        FLSallen,
        FLSkosmos,
        FLSmomo_h {
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    boolean snd_chk = true;
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
    Menu menu;
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
    Unit door;
    Effect gold_eft;
    Effect eft0;
    Effect eft1;
    Enveft enveft;
    Effect hotal;
    Effect hotal1;
    Effect hotal2;
    Effect hotal_u;
    Effect hotal_d;
    Effect hotal_s;
    Unit space;
    Shion shion;
    Allen allen;
    Kosmos kosmos;
    Momo momo;
    Unit mark;
    Unit mark2;
    Unit mark3;
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
    public static final int CASE_MAX = 6;
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
    int Timechk_srv_time;
    int Timechk_srv_totaltime;
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
    int msgsignal = 128;
    int cutwait = 0;
    boolean msgflag = true;
    Thread msg_clear = Thread.create(this, "msg_clear_thread");
    int msg_clearwait;
    boolean msg_clear_exec;
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
    float[] light0data;
    float[] light1dir;
    float[] light1col;
    float[] light2dir;
    float[] light2col;
    float[] light3dir;
    float[] light3col;
    Thread lightSPL_thread;
    Lightwork lightwork0;
    Lightwork lightwork1;
    Lightwork lightwork2;
    Lightwork lightwork3;
    int lightcount;
    Input Xpad1P;
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag;

    SCE03002() {
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
        this.light0data = new float[8];
        this.light1dir = new float[8];
        this.light1col = new float[8];
        this.light2dir = new float[8];
        this.light2col = new float[8];
        this.light3dir = new float[8];
        this.light3col = new float[8];
        this.lightSPL_thread = Thread.create(this, "lightSPL_main");
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_translate();
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
                    if (this.kind_c0_u1 == 2) {
                        this.gold_eft_set_rotate();
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
                    if (this.kind_c0_u1 == 2) {
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
                if (++n10 == 3) {
                    n10 = 2;
                }
                this.hand_cam0_shift(n10);
            }
            if (n7 <= 3 || n6 != 0 || (n & 0x40) == 0) continue;
            if ((n & 0x20) != 0) {
                if (++n8 > 6) {
                    n8 = 0;
                }
                n6 = 1;
            }
            if ((n & 0x80) != 0) {
                if (--n8 < 0) {
                    n8 = 6;
                }
                n6 = 1;
            }
            if (n6 != 1) continue;
            n7 = 0;
            this.gold_Visible_true();
            switch (n8) {
                case 0: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.shion;
                    break;
                }
                case 1: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.allen;
                    break;
                }
                case 2: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.kosmos;
                    break;
                }
                case 3: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.momo;
                    break;
                }
                case 4: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.space;
                    break;
                }
                case 5: {
                    this.kind_c0_u1 = 2;
                    this.gold_eft = this.hotal;
                    break;
                }
                case 6: {
                    this.kind_c0_u1 = 1;
                    this.gold_unit = this.mark2;
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
        System.println("XEVEFLAG:EV03002_F");
        Runtime.setFlags(304, 1, 1);
        System.println("XEVEJNAME:CFJ3_30 XEVEJPOINT:POINT3_30");
        Runtime.jumpCF(1770, 2);
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
        if (this.kind_c0_u1 == 0) {
            this.gold.setVisible(false);
        }
        if (this.kind_c0_u1 == 1) {
            this.gold_unit.py += 100.0f;
            this.gold_unit.setTranslate();
            this.gold_unit.py -= 100.0f;
        }
        if (this.kind_c0_u1 == 2) {
            this.gold_eft.py += 100.0f;
            this.gold_eft.setTranslate();
            this.gold_eft.py -= 100.0f;
        }
    }

    void gold_Visible_true() {
        if (this.kind_c0_u1 == 0) {
            this.gold.setVisible(true);
        }
        if (this.kind_c0_u1 == 1) {
            this.gold_unit.setTranslate();
        }
        if (this.kind_c0_u1 == 2) {
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
        if (this.kind_c0_u1 == 2) {
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
        Runtime.setLocation(806);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.BaseCam = Camera.create(1);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.shion = new Shion();
        this.shion.init(0x100001E, 0.0f, 0.0f, 0.0f, 180.0f);
        this.shion.face = this.shion.getChild(0x1000000);
        this.shion.setTranslate(-3.67f, 1.02f, -8.81f);
        this.shion.setRotate(0.0f, 180.0f, 0.0f);
        this.shion.setVisible(false);
        this.shion.renderCommand(22);
        this.allen = new Allen();
        this.allen.init(0x1000107, 0.0f, 0.0f, 0.0f, 180.0f);
        this.allen.face = this.allen.getChild(0x1000000);
        this.allen.setTranslate(-2.62f, 0.09f, 11.98f);
        this.allen.setRotate(0.0f, 195.95f, 2.5f);
        this.allen.setVisible(true);
        this.allen.renderCommand(22);
        this.kosmos = new Kosmos();
        this.kosmos.init(0x1000002, 0.0f, 0.0f, 0.0f, 180.0f);
        this.kosmos.face = this.kosmos.getChild(0x1000000);
        this.kosmos.setTranslate(-0.5f, 0.06f, 0.75f);
        this.kosmos.setRotate(0.0f, 180.0f, 0.0f);
        this.kosmos.setVisible(true);
        this.kosmos.renderCommand(22);
        this.momo = new Momo();
        this.momo.init(0x1000021, 0.0f, 0.0f, 0.0f, 180.0f);
        this.momo.face = this.momo.getChild(0x1000000);
        this.momo.setTranslate(-9.77f, 1.09f, -5.66f);
        this.momo.setRotate(0.0f, 180.0f, 0.0f);
        this.momo.setVisible(true);
        this.momo.renderCommand(22);
        this.space = new Unit();
        this.space.init(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.setTranslate(0.0f, 0.0f, 0.0f);
        this.space.setRotate(0.0f, 0.0f, 0.0f);
        this.space.setScale(1.0f, 1.0f, 1.0f);
        this.space.setVisible(true);
        this.mark = new Unit();
        this.mark.init(24579, 0.04f, -0.01f, 0.01f, 0.0f);
        this.mark.setRotate(270.0f, 180.0f, 180.0f);
        this.mark.setParent(this.momo, 72);
        this.mark.setVisible(false);
        this.mark2 = new Unit();
        this.mark2.init(24579, -10.39f, 2.08f, -5.9f, 0.0f);
        this.mark2.setVisible(false);
        this.mark3 = new Unit();
        this.mark3.init(24579, 0.06f, -0.02f, -0.06f, 0.0f);
        this.mark3.setRotate(270.0f, 180.0f, 180.0f);
        this.mark3.setParent(this.shion, 60);
        this.mark3.setVisible(false);
        this.hotal = new Effect(1682, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hotal.setTranslate(0.0f, 0.0f, 0.0f);
        this.hotal.setMotion(false);
        this.hotal.setForceLoop(false);
        this.hotal.setClip(false);
        this.hotal.noAttach(false);
        this.hotal.disp(true);
        this.hotal1 = new Effect(1682, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hotal1.setTranslate(-6.65f, 0.4f, -4.4f);
        this.hotal1.setMotion(false);
        this.hotal1.setForceLoop(false);
        this.hotal1.setClip(false);
        this.hotal1.noAttach(false);
        this.hotal1.disp(true);
        this.hotal2 = new Effect(1682, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hotal2.setTranslate(0.85f, 1.3f, -5.0f);
        this.hotal2.setMotion(false);
        this.hotal2.setForceLoop(false);
        this.hotal2.setClip(false);
        this.hotal2.noAttach(false);
        this.hotal2.disp(true);
        this.hotal_d = new Effect(1684, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hotal_d.setCaster(this.mark);
        this.hotal_d.setMotion(false);
        this.hotal_d.setForceLoop(false);
        this.hotal_d.setClip(false);
        this.hotal_d.noAttach(false);
        this.hotal_d.disp(false);
        this.hotal_u = new Effect(1683, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hotal_u.setTranslate(-9.68f, 1.89f, -6.07f);
        this.hotal_u.setMotion(false);
        this.hotal_u.setForceLoop(false);
        this.hotal_u.setClip(false);
        this.hotal_u.noAttach(false);
        this.hotal_u.disp(false);
        this.enveft = new Enveft();
        this.enveft.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
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
        this.lightwork0.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork0.setVisible(false);
        this.lightwork1 = new Lightwork();
        this.lightwork1.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork1.setVisible(false);
        this.lightwork2 = new Lightwork();
        this.lightwork2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork2.setVisible(false);
        this.lightwork3 = new Lightwork();
        this.lightwork3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.lightwork3.setVisible(false);
    }

    void lightSPL_main() {
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
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.momo.face, "FLSmomo_h.fpk");
        this.BaseCam.setTranslate(-3.59f, 1.55f, 2.68f);
        this.BaseCam.setRotate(-9.92f, 309.61f, 0.0f);
        this.Timechk();
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(1, 0.508f, 0.166f, -0.845f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, -0.654f, 0.36f, 0.665f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.775f, -0.0f, 0.632f);
        boolean bl = false;
        int n = 0;
        while (n == 0) {
            Sound.streamPlay(1390021, 48000);
            this.BaseCam.change();
            Stage.setVisible(36, false);
            Stage.setVisible(55, false);
            this.kosmos.setVisible(false);
            this.allen.setVisible(false);
            System.println("CUT1 START");
            this.momo.start(1, "sit");
            this.camerawork.cut1();
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(90);
            System.println("CUT1 END");
            System.println("CUT2 START");
            this.camerawork.cut2();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, -0.125f, 0.628f, 0.768f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, 0.914f, 0.0f, -0.406f);
            this.light.setColor(3, 0.24f, 0.21f, 0.21f);
            this.light.setDirection2(3, -0.435f, -0.421f, 0.796f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(90);
            System.println("CUT2 END");
            System.println("CUT3 START");
            Stage.setVisible(18, false);
            this.camerawork.cut3();
            this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            this.light.setColor(1, 0.56f, 0.56f, 0.56f);
            this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
            this.light.setColor(2, 0.34f, 0.34f, 0.34f);
            this.light.setDirection2(2, 0.616f, 0.001f, -0.788f);
            this.light.setColor(3, 0.36f, 0.41f, 0.41f);
            this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.momo.start(1, "act5_1");
            System.sleep(60);
            System.println("CUT3 END");
            System.println("CUT4 START");
            Stage.setVisible(18, true);
            this.hotal_d = new Effect(1684, 0.0f, 0.0f, 0.0f, 0.0f);
            this.hotal_d.setCaster(this.mark);
            this.hotal_d.setMotion(false);
            this.hotal_d.setForceLoop(false);
            this.hotal_d.setClip(false);
            this.hotal_d.noAttach(false);
            this.hotal_d.disp(true);
            this.camerawork.cut4();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.light.setColor(2, 0.2f, 0.2f, 0.18f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(72);
            System.println("CUT4 END");
            System.println("CUT5 START");
            this.momo.start(1, "act5");
            this.enveft.start(1, "act5");
            this.camerawork.cut5();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, -0.088f, 0.551f, 0.83f);
            this.light.setColor(2, 0.32f, 0.32f, 0.3f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.momo.setMotionFlags(0x2000000, true);
            this.shion.setMotionFlags(0x2000000, true);
            System.sleep(48);
            if (!this.snd_chk) {
                Sound.streamPlay(302001);
            }
            this.MSG(57, this.momo.face, 1, "What do you all think...?");
            System.sleep(15);
            System.println("CUT5 END");
            System.println("CUT6-7 START");
            this.camerawork.cut6();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.9f);
            this.light.setDirection2(1, 0.258f, 0.787f, 0.561f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.25f, 0.22f, 0.22f);
            this.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            if (!this.snd_chk) {
                Sound.streamPlay(302002);
            }
            this.MSG(83, this.momo.face, 1, "Joachim Mizrahi, the lunatic...");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302003);
            }
            this.MSG(81, this.momo.face, 1, "Was Daddy really like that?");
            System.sleep(28);
            if (!this.snd_chk) {
                Sound.streamPlay(302004);
            }
            this._MSG(171, "モモ", "Since Daddy built us, does that\nmean Realians are bad people, too?");
            this.FACE(114, this.momo.face, 1);
            System.println("CUT6-7 END");
            System.println("CUT8 START");
            this.momo.start(1, "act8");
            this.camerawork.cut8();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.138f, 0.555f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.FACE(57, this.momo.face, 1);
            System.sleep(15);
            this.momo.setMotionFlags(0x2000000, false);
            System.println("CUT8 END");
            System.println("CUT9 START");
            this.momo.start(1, "sit");
            this.shion.start(1, "act9");
            this.shion.setVisible(true);
            this.camerawork.cut9();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, -0.088f, 0.551f, 0.83f);
            this.light.setColor(2, 0.32f, 0.32f, 0.3f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            if (!this.snd_chk) {
                Sound.streamPlay(302005);
            }
            System.sleep(14);
            this.MSG(35, this.shion.face, 1, "MOMO...");
            if (!this.snd_chk) {
                Sound.streamPlay(302905);
            }
            this.MSG(41, this.shion.face, 1, "We were looking all over for you.");
            System.println("CUT9 END");
            System.println("CUT10 START");
            this.kosmos.start(1, "act10");
            this.shion.start(1, "act10");
            this.kosmos.setVisible(true);
            this.camerawork.cut10();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.34f, 0.64f, 0.83f);
            this.light.setDirection2(1, -0.825f, 0.21f, 0.524f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.momo.setLightMode(1);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.34f, 0.64f, 0.83f);
            this.momo.light.setDirection2(1, 0.388f, 0.572f, 0.723f);
            this.momo.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.momo.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.momo.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.momo.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            System.sleep(30);
            this.momo.look_speed(0.25f);
            this.momo.look_char(this.shion);
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302006);
            }
            this.MSG(30, this.momo.face, 1, "Hello, Shion...");
            System.sleep(42);
            System.println("CUT10 END");
            System.println("CUT11 START");
            this.momo.start(1, "act11");
            this.camerawork.cut11();
            this.momo.look_speed(0.25f);
            this.momo.look_char(this.shion);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.21f, 0.51f, 0.7f);
            this.light.setDirection2(1, -0.793f, 0.0f, 0.609f);
            this.light.setColor(2, 0.26f, 0.26f, 0.24f);
            this.light.setDirection2(2, 0.599f, 0.0f, -0.8f);
            this.light.setColor(3, 0.25f, 0.22f, 0.22f);
            this.light.setDirection2(3, -0.958f, -0.278f, -0.069f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.41f, 0.71f, 0.9f);
            this.momo.light.setDirection2(1, 0.291f, 0.593f, 0.751f);
            this.momo.light.setColor(2, 0.28f, 0.28f, 0.26f);
            this.momo.light.setDirection2(2, -0.504f, 0.387f, 0.772f);
            this.momo.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.momo.light.setDirection2(3, 0.094f, -0.262f, -0.96f);
            if (!this.snd_chk) {
                Sound.streamPlay(302007);
            }
            System.sleep(15);
            this.MSG(32, this.shion.face, 1, 32, "What's wrong?");
            if (!this.snd_chk) {
                Sound.streamPlay(302008);
            }
            this.MSG(42, this.shion.face, 1, 42, "You look kinda down.");
            System.sleep(13);
            System.println("CUT11 END");
            System.println("CUT12 START");
            this.camerawork.cut12();
            this.momo.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.32f, 0.62f, 0.81f);
            this.light.setDirection2(1, 0.535f, 0.45f, 0.715f);
            this.light.setColor(2, 0.2f, 0.2f, 0.18f);
            this.light.setDirection2(2, 0.007f, 0.349f, -0.937f);
            this.light.setColor(3, 0.27f, 0.24f, 0.24f);
            this.light.setDirection2(3, 0.707f, -0.706f, 0.034f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(60);
            if (!this.snd_chk) {
                Sound.streamPlay(302009);
            }
            this.MSG(65, this.momo.face, 1, 65, "Shion...\nI...");
            System.sleep(37);
            System.println("CUT12 END");
            System.println("CUT13 START");
            this.kosmos.start(1, "act13");
            this.shion.start(1, "act13");
            this.momo.start(1, "sit");
            this.momo.look_default();
            this.camerawork.cut13();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, 0.703f, 0.503f, 0.502f);
            this.light.setColor(2, 0.28f, 0.28f, 0.25f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.27f, 0.24f, 0.24f);
            this.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.kosmos.setLightMode(1);
            this.kosmos.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.kosmos.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.kosmos.light.setDirection2(1, -0.221f, 0.737f, 0.639f);
            this.kosmos.light.setColor(2, 0.28f, 0.28f, 0.25f);
            this.kosmos.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.kosmos.light.setColor(3, 0.27f, 0.24f, 0.24f);
            this.kosmos.light.setDirection2(3, 0.49f, -0.491f, -0.72f);
            System.sleep(72);
            System.println("CUT13 END");
            System.println("CUT14 START");
            this.camerawork.cut14();
            this.kosmos.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, 0.852f, 0.521f, 0.051f);
            this.light.setColor(2, 0.13f, 0.13f, 0.11f);
            this.light.setDirection2(2, -0.999f, 0.024f, -0.038f);
            this.light.setColor(3, 0.24f, 0.21f, 0.21f);
            this.light.setDirection2(3, -0.555f, -0.814f, 0.17f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.setMotionFlags(0x2000000, false);
            this.shion.setMotionFlags(0x800000, true);
            if (!this.snd_chk) {
                Sound.streamPlay(302010);
            }
            this.MSG(75, this.momo.face, 1, "I was created by Joachim Mizrahi.");
            System.sleep(10);
            if (!this.snd_chk) {
                Sound.streamPlay(302011);
            }
            this.MSG(156, this.momo.face, 1, "He designed the 100-Series Realians\nback when the Federation funded him.");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302012);
            }
            this.MSG(29, this.shion.face, 1, 29, "I know...");
            System.sleep(30);
            System.println("CUT14 END");
            System.println("CUT15 START");
            this.shion.start(1, "act15");
            this.enveft.start(1, "act15");
            this.camerawork.cut15();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.138f, 0.555f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            if (!this.snd_chk) {
                Sound.streamPlay(302013);
            }
            this.MSG(132, this.momo.face, 1, "Shion, I...\nDaddy wasn't a lunatic.");
            System.sleep(15);
            if (!this.snd_chk) {
                Sound.streamPlay(302014);
            }
            this.MSG(90, this.momo.face, 1, 90, "...I don't even know what I am.");
            System.sleep(45);
            if (!this.snd_chk) {
                Sound.streamPlay(302015);
            }
            this.MSG(87, this.shion.face, 1, "Identity diffusion...");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302016);
            }
            this.MSG(49, this.shion.face, 1, "The eternal dilemma.");
            this.shion.setMotionFlags(0x800000, false);
            System.sleep(92);
            System.println("CUT15 END");
            System.println("CUT16 START");
            this.camerawork.cut16();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.902f, 0.27f, 0.337f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.34f, 0.31f, 0.31f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.setMotionFlags(0x2000000, true);
            System.sleep(150);
            System.println("CUT16 END");
            System.println("CUT17-18 START");
            this.shion.start(1, "act17");
            this.camerawork.cut17();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.32f, 0.62f, 0.81f);
            this.light.setDirection2(1, 0.535f, 0.45f, 0.715f);
            this.light.setColor(2, 0.19f, 0.19f, 0.17f);
            this.light.setDirection2(2, 0.557f, 0.353f, -0.752f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.677f, -0.732f, -0.076f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.look_speed(0.0f);
            this.shion.look_char(this.momo);
            if (!this.snd_chk) {
                Sound.streamPlay(302017);
            }
            this.MSG(83, this.shion.face, 1, "Do you know about the environmental\n bugs onboard this ship?");
            if (!this.snd_chk) {
                Sound.streamPlay(302018);
            }
            this.iMSG(149, this.shion.face, 1, "They're actually nanomachines,\nyou know, used to keep enclosed\nspaces like this clean.");
            if (!this.snd_chk) {
                Sound.streamPlay(302019);
            }
            this.MSG(249, this.shion.face, 1, "I'd say that...of all the things\nProfessor Mizrahi -- or anyone else --\ncreated, they rank up there pretty high.");
            System.sleep(1);
            this.shion.setMotionFlags(0x2000000, false);
            System.sleep(29);
            System.println("CUT17-18 END");
            System.println("CUT19 START");
            this.shion.look_default();
            this.shion.setVisible(false);
            this.camerawork.cut19();
            this.light.setColor(0, 0.08f, 0.08f, 0.11f);
            this.light.setColor(1, 0.32f, 0.62f, 0.81f);
            this.light.setDirection2(1, 0.4f, 0.505f, 0.764f);
            this.light.setColor(2, 0.25f, 0.25f, 0.23f);
            this.light.setDirection2(2, -0.681f, 0.001f, -0.732f);
            this.light.setColor(3, 0.26f, 0.23f, 0.23f);
            this.light.setDirection2(3, 0.481f, -0.752f, -0.45f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.start(1, "act20_1");
            if (!this.snd_chk) {
                Sound.streamPlay(302020);
            }
            this.MSG(201, this.shion.face, 1, "These bugs may be man-made, but they\nfunction as if they've existed all along.\nAlmost as if they were meant to be...");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302021);
            }
            this.MSG(74, this.shion.face, 1, "I think Realians are the same.");
            System.sleep(14);
            if (!this.snd_chk) {
                Sound.streamPlay(302022);
            }
            this.MSG(113, this.shion.face, 1, "The only difference between us,\nis the length of our histories.");
            System.println("CUT19 END");
            System.println("CUT20 START");
            this.enveft.start(1, "act20");
            this.shion.start(1, "act20");
            this.shion.setVisible(true);
            this.camerawork.cut20();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, -0.389f, 0.576f, -0.719f);
            this.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.light.setDirection2(3, 0.477f, -0.531f, -0.701f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302023);
            }
            this.MSG(167, this.shion.face, 1, "But regardless of our origins,\neach one of us has an important\nrole to fulfill in this world.");
            System.sleep(30);
            System.println("CUT20 END");
            System.println("CUT21 START");
            this.camerawork.cut21();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, -0.389f, 0.576f, -0.719f);
            this.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.light.setDirection2(3, 0.477f, -0.531f, -0.701f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.start(1, "act22_1");
            System.sleep(90);
            System.println("CUT21 END");
            System.println("CUT22 START");
            this.shion.start(1, "act22");
            this.camerawork.cut22();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, -0.389f, 0.576f, -0.719f);
            this.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.light.setDirection2(3, 0.477f, -0.531f, -0.701f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.kosmos.look_speed(0.5f);
            this.kosmos.look_char(this.momo);
            if (!this.snd_chk) {
                Sound.streamPlay(302024);
            }
            this.MSG(212, this.shion.face, 1, "Besides, I really doubt Dr. Mizrahi\nwas a dangerous person like\neveryone makes him out to be.");
            System.sleep(15);
            if (!this.snd_chk) {
                Sound.streamPlay(302025);
            }
            this.MSG(44, this.shion.face, 1, "And it's not just because of");
            System.sleep(13);
            if (!this.snd_chk) {
                Sound.streamPlay(302026);
            }
            this.MSG(166, this.shion.face, 1, "the environmental bugs.\nThe work he did on Realians\nwas incredibly insightful.");
            System.sleep(30);
            System.println("CUT22 END");
            System.println("CUT23 START");
            this.camerawork.cut23();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, -0.687f, 0.304f, 0.66f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, 0.341f, 0.44f, -0.831f);
            this.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.light.setDirection2(3, -0.861f, -0.479f, -0.17f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.start(1, "act24_1");
            this.momo.start(1, "act24_1");
            if (!this.snd_chk) {
                Sound.streamPlay(302027);
            }
            this.MSG(75, this.kosmos.face, 1, "The fundamental Gnosis research\nProfessor Mizrahi");
            this.MSG(138, this.kosmos.face, 1, "left behind played a critical\nrole in the development of\nmodern anti-Gnosis technology.");
            System.println("CUT23 END");
            System.println("CUT24 START");
            this.shion.start(1, "act24");
            this.momo.start(1, "act24");
            this.camerawork.cut24();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, -0.012f, 0.291f, 0.957f);
            this.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.light.setDirection2(2, 0.629f, 0.0f, -0.777f);
            this.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.light.setDirection2(3, -0.714f, -0.571f, -0.405f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.kosmos.setLightMode(1);
            this.kosmos.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.kosmos.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.kosmos.light.setDirection2(1, -0.687f, 0.304f, 0.66f);
            this.kosmos.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.kosmos.light.setDirection2(2, 0.341f, 0.44f, -0.831f);
            this.kosmos.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.kosmos.light.setDirection2(3, -0.861f, -0.479f, -0.17f);
            this.momo.setLightMode(1);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.momo.light.setDirection2(1, 0.301f, 0.441f, 0.845f);
            this.momo.light.setColor(2, 0.22f, 0.22f, 0.2f);
            this.momo.light.setDirection2(2, 0.596f, 0.001f, -0.803f);
            this.momo.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.momo.light.setDirection2(3, -0.998f, -0.0f, -0.065f);
            this.momo.setMotionFlags(0x2000000, true);
            if (!this.snd_chk) {
                Sound.streamPlay(302028);
            }
            this.MSG(60, this.shion.face, 1, "Thanks for the backup, KOS-MOS!");
            if (!this.snd_chk) {
                Sound.streamPlay(302029);
            }
            this.MSG(77, this.shion.face, 1, "I'm really glad to see you\nreact like that.");
            if (!this.snd_chk) {
                Sound.streamPlay(302030);
            }
            this.MSG(51, this.shion.face, 1, "Empathizing with the\nfeelings of others");
            if (!this.snd_chk) {
                Sound.streamPlay(302031);
            }
            this.MSG(75, this.shion.face, 1, "is a major factor in\nhuman relationships.");
            System.sleep(8);
            if (!this.snd_chk) {
                Sound.streamPlay(302032);
            }
            this.MSG(135, this.kosmos.face, 1, "Although I do not believe that the\ncurrent situation called for me to\nact in an empathetic manner,");
            this.MSG(86, this.kosmos.face, 1, "I am pleased to be of service to you.");
            System.println("CUT24 END");
            System.println("CUT25 START");
            this.momo.start(1, "act25");
            this.shion.look_speed(1.0f);
            this.shion.look_char(this.kosmos);
            this.camerawork.cut25();
            this.kosmos.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.138f, 0.555f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(15);
            if (!this.snd_chk) {
                Sound.streamPlay(302033);
            }
            this.MSG(99, this.momo.face, 1, "...I think KOS-MOS is really funny.");
            this.momo.setMotionFlags(0x2000000, false);
            this.shion.start(1, "act26_1");
            this.kosmos.look_default();
            System.sleep(45);
            System.println("CUT25 END");
            System.println("CUT26-27 START");
            this.shion.start(1, "act26");
            this.momo.start(1, "act26");
            this.momo.hairStop(0, 1);
            this.camerawork.cut26();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.194f, 0.367f, 0.91f);
            this.light.setColor(2, 0.18f, 0.18f, 0.16f);
            this.light.setDirection2(2, -0.202f, 0.291f, -0.935f);
            this.light.setColor(3, 0.28f, 0.25f, 0.25f);
            this.light.setDirection2(3, 0.983f, -0.182f, -0.039f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.kosmos.setLightMode(1);
            this.kosmos.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.kosmos.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.kosmos.light.setDirection2(1, 0.158f, 0.34f, 0.927f);
            this.kosmos.light.setColor(2, 0.14f, 0.14f, 0.12f);
            this.kosmos.light.setDirection2(2, -0.911f, 0.0f, -0.413f);
            this.kosmos.light.setColor(3, 0.2f, 0.17f, 0.17f);
            this.kosmos.light.setDirection2(3, -0.032f, -0.198f, -0.98f);
            if (!this.snd_chk) {
                Sound.streamPlay(302034);
            }
            this.MSG(95, this.shion.face, 1, 95, "Too bad she doesn't take\ninstruction very well.");
            System.sleep(29);
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302035);
            }
            this.MSG(68, this.momo.face, 1, "I never got to meet Daddy,");
            if (!this.snd_chk) {
                Sound.streamPlay(302036);
            }
            this.MSG(155, this.momo.face, 1, "but he used to talk to me all the\ntime before I was born.");
            System.sleep(30);
            this.shion.look_speed(0.01f);
            this.shion.look_char(this.momo);
            if (!this.snd_chk) {
                Sound.streamPlay(302037);
            }
            this.MSG(122, this.momo.face, 1, "It's all a little hazy,\nbut I remember him telling me that...");
            if (!this.snd_chk) {
                Sound.streamPlay(302038);
            }
            System.sleep(30);
            this.iMSG(102, this.momo.face, 1, 60, "I could become a real person\nif I did good deeds...");
            if (!this.snd_chk) {
                Sound.streamPlay(302039);
            }
            this.MSG(55, this.shion.face, 1, 25, "Really?");
            if (!this.snd_chk) {
                Sound.streamPlay(302040);
            }
            this.MSG(44, this.shion.face, 1, 60, "That's a wonderful memory to have...");
            if (!this.snd_chk) {
                Sound.streamPlay(302041);
            }
            this.MSG(30, this.momo.face, 1, 12, "Uh-huh.");
            System.sleep(1);
            System.println("CUT26-27 END");
            System.println("CUT28 START");
            Stage.setVisible(18, false);
            this.allen.setVisible(true);
            this.camerawork.cut3();
            this.kosmos.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, 0.703f, 0.504f, 0.502f);
            this.light.setColor(2, 0.28f, 0.28f, 0.25f);
            this.light.setDirection2(2, -0.678f, 0.493f, -0.545f);
            this.light.setColor(3, 0.27f, 0.24f, 0.24f);
            this.light.setDirection2(3, -0.595f, -0.789f, 0.15f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(22);
            this.kosmos.look_speed(0.4f);
            this.kosmos.look_char(this.allen);
            if (!this.snd_chk) {
                Sound.streamPlay(302042);
            }
            this.MSG(38, this.allen.face, 1, "Chief!");
            System.println("CUT28 END");
            System.println("CUT29 START");
            Stage.setVisible(18, true);
            this.allen.start(1, "act29");
            this.kosmos.look_default();
            this.shion.look_default();
            this.camerawork.cut29();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, -0.522f, 0.435f, 0.734f);
            this.light.setColor(2, 0.28f, 0.28f, 0.25f);
            this.light.setDirection2(2, -0.844f, 0.0f, -0.537f);
            this.light.setColor(3, 0.27f, 0.24f, 0.24f);
            this.light.setDirection2(3, 0.44f, -0.631f, -0.639f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.setLightMode(1);
            this.shion.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.shion.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.shion.light.setDirection2(1, 0.738f, 0.461f, 0.492f);
            this.shion.light.setColor(2, 0.23f, 0.23f, 0.2f);
            this.shion.light.setDirection2(2, -0.982f, 0.0f, -0.186f);
            this.shion.light.setColor(3, 0.28f, 0.25f, 0.25f);
            this.shion.light.setDirection2(3, 0.497f, -0.368f, -0.786f);
            this.momo.setLightMode(1);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.momo.light.setDirection2(1, 0.738f, 0.461f, 0.492f);
            this.momo.light.setColor(2, 0.23f, 0.23f, 0.2f);
            this.momo.light.setDirection2(2, -0.982f, 0.0f, -0.186f);
            this.momo.light.setColor(3, 0.28f, 0.25f, 0.25f);
            this.momo.light.setDirection2(3, 0.497f, -0.368f, -0.786f);
            if (!this.snd_chk) {
                Sound.streamPlay(302044);
            }
            System.sleep(17);
            this.MSG(47, this.allen.face, 1, "Great news!");
            this.shion.look_speed(0.2f);
            this.shion.look_char(this.allen);
            this.momo.look_speed(0.2f);
            this.momo.look_point(15.75f, 1.33f, -6.3f);
            this.shion.setMotionFlags(0x2000000, true);
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302045);
            }
            this.MSG(68, this.allen.face, 1, "After this ship arrives at\nthe Kukai Foundation,");
            System.println("CUT29 END");
            System.println("CUT30 START");
            this.camerawork.cut30();
            this.shion.start(1, "act32");
            this.shion.look_speed(0.02f);
            this.shion.look_point(-8.08f, 2.51f, -6.68f);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.light.setDirection2(1, -0.52f, 0.412f, 0.748f);
            this.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.light.setDirection2(2, 0.578f, 0.001f, -0.816f);
            this.light.setColor(3, 0.22f, 0.19f, 0.19f);
            this.light.setDirection2(3, -0.718f, -0.649f, 0.252f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.shion.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.shion.light.setDirection2(1, 0.126f, 0.564f, 0.816f);
            this.shion.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.shion.light.setDirection2(2, 0.363f, 0.001f, -0.932f);
            this.shion.light.setColor(3, 0.19f, 0.16f, 0.16f);
            this.shion.light.setDirection2(3, -0.851f, -0.001f, -0.525f);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.momo.light.setDirection2(1, 0.126f, 0.564f, 0.816f);
            this.momo.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.momo.light.setDirection2(2, 0.363f, 0.001f, -0.932f);
            this.momo.light.setColor(3, 0.19f, 0.16f, 0.16f);
            this.momo.light.setDirection2(3, -0.851f, -0.001f, -0.525f);
            this.kosmos.look_speed(0.04f);
            this.kosmos.look_char(this.allen);
            this.MSG(72, this.allen.face, 1, "they're gonna take us\nto Second Miltia!");
            if (!this.snd_chk) {
                Sound.streamPlay(302046);
            }
            this.MSG(66, this.shion.face, 1, "Second Miltia?");
            if (!this.snd_chk) {
                Sound.streamPlay(302047);
            }
            this.MSG(32, this.shion.face, 1, "Really?");
            this.shion.setMotionFlags(0x2000000, false);
            System.sleep(16);
            System.println("CUT30 END");
            System.println("CUT31 START");
            this.camerawork.cut31();
            this.kosmos.look_speed(0.01f);
            this.kosmos.look_char(this.allen);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.31f, 0.61f, 0.8f);
            this.light.setDirection2(1, -0.52f, 0.412f, 0.748f);
            this.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.light.setDirection2(2, 0.578f, 0.008f, -0.816f);
            this.light.setColor(3, 0.26f, 0.23f, 0.23f);
            this.light.setDirection2(3, -0.718f, -0.649f, 0.252f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.allen.setLightMode(1);
            this.allen.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.allen.light.setColor(1, 0.31f, 0.61f, 0.8f);
            this.allen.light.setDirection2(1, -0.52f, 0.412f, 0.748f);
            this.allen.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.allen.light.setDirection2(2, 0.578f, 0.008f, -0.816f);
            this.allen.light.setColor(3, 0.26f, 0.23f, 0.23f);
            this.allen.light.setDirection2(3, -0.718f, -0.649f, 0.252f);
            this.kosmos.setLightMode(1);
            this.kosmos.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.kosmos.light.setColor(1, 0.31f, 0.61f, 0.8f);
            this.kosmos.light.setDirection2(1, -0.52f, 0.412f, 0.748f);
            this.kosmos.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.kosmos.light.setDirection2(2, 0.578f, 0.008f, -0.816f);
            this.kosmos.light.setColor(3, 0.26f, 0.23f, 0.23f);
            this.kosmos.light.setDirection2(3, -0.718f, -0.649f, 0.252f);
            this.shion.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.shion.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.shion.light.setDirection2(1, 0.126f, 0.564f, 0.816f);
            this.shion.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.shion.light.setDirection2(2, 0.363f, 0.001f, -0.932f);
            this.shion.light.setColor(3, 0.19f, 0.16f, 0.16f);
            this.shion.light.setDirection2(3, -0.851f, -0.001f, -0.525f);
            this.momo.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.momo.light.setColor(1, 0.43f, 0.73f, 0.92f);
            this.momo.light.setDirection2(1, 0.126f, 0.564f, 0.816f);
            this.momo.light.setColor(2, 0.17f, 0.17f, 0.14f);
            this.momo.light.setDirection2(2, 0.363f, 0.001f, -0.932f);
            this.momo.light.setColor(3, 0.19f, 0.16f, 0.16f);
            this.momo.light.setDirection2(3, -0.851f, -0.001f, -0.525f);
            if (!this.snd_chk) {
                Sound.streamPlay(302048);
            }
            this.MSG(53, this.allen.face, 1, "Apparently, they have some work");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302049);
            }
            this.MSG(120, this.allen.face, 1, "to take care of out there.\nThe Captain's still whining about\nrepairing the Elsa, but...");
            if (!this.snd_chk) {
                Sound.streamPlay(302050);
            }
            this.MSG(115, this.shion.face, 1, "Some work...?\nI wonder...?");
            if (!this.snd_chk) {
                Sound.streamPlay(302051);
            }
            this.MSG(30, this.allen.face, 1, 14, "Huh?");
            System.sleep(30);
            System.println("CUT31 END");
            System.println("CUT32 START");
            this.shion.start(1, "act32_2");
            this.camerawork.cut32();
            this.allen.start(1, "act33_1");
            this.allen.setLightMode(0);
            this.momo.setLightMode(0);
            this.kosmos.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.shion.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.shion.light.setDirection2(1, 0.82f, 0.14f, 0.555f);
            this.shion.light.setColor(2, 0.24f, 0.24f, 0.22f);
            this.shion.light.setDirection2(2, -0.495f, 0.728f, -0.474f);
            this.shion.light.setColor(3, 0.29f, 0.26f, 0.26f);
            this.shion.light.setDirection2(3, 0.566f, -0.45f, -0.691f);
            if (!this.snd_chk) {
                Sound.streamPlay(302052);
            }
            this.MSG(15, this.shion.face, 1, "Well...");
            if (!this.snd_chk) {
                Sound.streamPlay(302053);
            }
            this.MSG(175, this.shion.face, 1, "I'm sure they've got their own \nreasons, but don't you think\nit's a little strange that");
            this.iMSG(62, this.shion.face, 1, "everyone's heading for Miltia?");
            System.println("CUT32 END");
            System.println("CUT33 START");
            this.allen.start(1, "act33");
            this.camerawork.cut33();
            this.shion.setLightMode(0);
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.33f, 0.63f, 0.82f);
            this.light.setDirection2(1, -0.52f, 0.412f, 0.748f);
            this.light.setColor(2, 0.18f, 0.18f, 0.15f);
            this.light.setDirection2(2, -0.017f, 0.654f, -0.756f);
            this.light.setColor(3, 0.23f, 0.2f, 0.2f);
            this.light.setDirection2(3, -0.807f, -0.485f, -0.338f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            this.shion.start(1, "act34_1");
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302054);
            }
            this.MSG(50, this.allen.face, 1, "Not really...");
            if (!this.snd_chk) {
                Sound.streamPlay(302055);
            }
            this.MSG(45, this.allen.face, 1, "It's probably just a coincidence.");
            if (!this.snd_chk) {
                Sound.streamPlay(302056);
            }
            this.iMSG(79, this.allen.face, 1, "I'm just glad we saved some money.\nNot to mention that");
            if (!this.snd_chk) {
                Sound.streamPlay(302057);
            }
            this.iMSG(80, this.allen.face, 1, "this ship's a million times better\nequipped than the Elsa.");
            if (!this.snd_chk) {
                Sound.streamPlay(302058);
            }
            this.iMSG(98, this.allen.face, 1, 98, "And we'll have nothing to worry about\nif we run into the Gnosis again.");
            System.sleep(24);
            if (!this.snd_chk) {
                Sound.streamPlay(302059);
            }
            this.MSG(53, this.allen.face, 1, "Hey, speaking of which,");
            if (!this.snd_chk) {
                Sound.streamPlay(302060);
            }
            this.iMSG(183, this.allen.face, 1, "did you check out your room?\nThey all have jacuzzis and minibars,\n and...Hey, what's wrong, Chief?");
            System.println("CUT33 END");
            System.println("CUT34 START");
            this.shion.start(1, "act34");
            this.camerawork.cut34();
            this.light.setColor(0, 0.1f, 0.1f, 0.13f);
            this.light.setColor(1, 0.41f, 0.71f, 0.85f);
            this.light.setDirection2(1, 0.636f, 0.131f, 0.76f);
            this.light.setColor(2, 0.21f, 0.21f, 0.19f);
            this.light.setDirection2(2, -0.697f, 0.686f, -0.209f);
            this.light.setColor(3, 0.23f, 0.2f, 0.2f);
            this.light.setDirection2(3, 0.393f, -0.779f, -0.489f);
            Stage.setColor(0.5f, 0.6f, 0.7f);
            System.sleep(30);
            if (!this.snd_chk) {
                Sound.streamPlay(302061);
            }
            this.MSG(60, this.shion.face, 1, "Nothing, nothing.");
            if (!this.snd_chk) {
                Sound.streamPlay(302062);
            }
            this.MSG(81, this.shion.face, 1, "I'm just jealous of how \neasily swayed you are...");
            System.println("CUT34 END");
            ++n;
        }
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

    void setRegXYZ(float f, float f2, float f3) {
        Runtime.setRegister(0, f);
        Runtime.setRegister(1, f2);
        Runtime.setRegister(2, f3);
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

    void whiteout() {
        this.whiteOut = new Effect(0);
        this.whiteOut.args[0] = -2130706433;
        this.whiteOut.args[1] = 30;
        this.whiteOut.args[2] = 0;
        this.whiteOut.call(0);
    }

    class Mapunits
            extends Unit {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        Mapunits() {
        }

        void open() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = 1.0f;
            fArray[3] = 9.0f;
            fArray[4] = 25.0f;
            fArray[5] = -0.5f;
            fArray[7] = 9.0f;
            float[] fArray2 = fArray;
            this.posSPL.setCtrlVertex(fArray2, 0, 0, 25);
            this.move(this.posSPL, 0, true);
            System.sleep(30);
            float[] fArray3 = new float[8];
            fArray3[0] = 1.0f;
            fArray3[1] = -0.5f;
            fArray3[3] = 9.0f;
            fArray3[4] = 30.0f;
            fArray3[5] = 1.0f;
            fArray3[7] = 9.0f;
            float[] fArray4 = fArray3;
            this.posSPL.setCtrlVertex(fArray4, 0, 1, 30);
            this.move(this.posSPL, 0, true);
        }
    }

    class Enveft
            extends Unit {
        Enveft() {
        }

        void act15() {
            System.sleep(438);
            SCE03002.this.hotal_d = new Effect(1684, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE03002.this.hotal_d.setCaster(SCE03002.this.mark3);
            SCE03002.this.hotal_d.setMotion(false);
            SCE03002.this.hotal_d.setForceLoop(false);
            SCE03002.this.hotal_d.setClip(false);
            SCE03002.this.hotal_d.noAttach(false);
            SCE03002.this.hotal_d.disp(true);
        }

        void act20() {
            System.sleep(155);
            SCE03002.this.hotal_d.disp(false);
            SCE03002.this.hotal_u = new Effect(1683, -10.41f, 2.08f, -5.9f, 0.0f);
            SCE03002.this.hotal_u.setMotion(false);
            SCE03002.this.hotal_u.setForceLoop(false);
            SCE03002.this.hotal_u.setClip(false);
            SCE03002.this.hotal_u.noAttach(false);
            SCE03002.this.hotal_u.disp(true);
        }

        void act5() {
            System.sleep(350);
            SCE03002.this.hotal_d.disp(false);
            SCE03002.this.hotal_u = new Effect(1683, -9.68f, 1.89f, -6.07f, 0.0f);
            SCE03002.this.hotal_u.setMotion(false);
            SCE03002.this.hotal_u.setForceLoop(false);
            SCE03002.this.hotal_u.setClip(false);
            SCE03002.this.hotal_u.noAttach(false);
            SCE03002.this.hotal_u.disp(true);
        }
    }

    class Shion
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Shion() {
        }

        void act10() {
            this.setTranslate(-6.61f, 0.5f, -3.24f);
            this.setRotate(0.0f, 215.83f, 0.0f);
            this.mtn(263, 0, 234, 8, 0, 1.0f, true);
        }

        void act13() {
            this.setTranslate(-10.33f, 1.02f, -5.59f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(267, 0, 402, 8, 0, 1.0f, true);
        }

        void act15() {
            this.mtn(269, 0, 690, 8, 0, 1.0f, true);
        }

        void act17() {
            this.mtn(273, 0, 0.8f, true);
        }

        void act20() {
            this.mtn(271, 0, 1.0f, true);
        }

        void act20_1() {
            this.mtn(271, 0, 1, 0, 0, 1.0f, true);
        }

        void act22() {
            this.mtn(273, 0, 0.9f, true);
        }

        void act22_1() {
            this.mtn(273, 0, 1, 0, 0, 0.9f, true);
        }

        void act24() {
            this.mtn(275, 0, 1.0f, true);
        }

        void act24_1() {
            this.setTranslate(-10.41f, 1.02f, -5.59f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(275, 0, 1, 0, 0, 1.0f, true);
        }

        void act26() {
            this.mtn(257, 0, 1.0f, true);
        }

        void act26_1() {
            this.setTranslate(-10.41f, 1.04f, -5.59f);
            this.mtn(257, 0, 1, 0, 0, 1.0f, true);
        }

        void act32() {
            this.setTranslate(-10.41f, 1.02f, -5.59f);
            this.mtn(283, 0, 1, 0, 0, 1.0f, true);
        }

        void act32_2() {
            this.mtn(283, 0, 1.0f, true);
        }

        void act34() {
            this.mtn(286, 0, 1.0f, true);
        }

        void act34_1() {
            this.look_default();
            this.mtn(286, 0, 1, 0, 0, 1.0f, true);
        }

        void act9() {
            this.setTranslate(-0.82f, 0.07f, 0.86f);
            this.setRotate(0.0f, 215.83f, 0.0f);
            this.mtn(287, 10, 100, 8, 0, 0.8f, true);
        }
    }

    class Kosmos
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Kosmos() {
        }

        void act10() {
            this.setTranslate(-6.06f, 0.33f, -2.55f);
            this.setRotate(0.0f, 206.67f, 0.0f);
            this.mtn(264, 0, 234, 8, 0, 1.0f, true);
        }

        void act13() {
            this.setTranslate(-8.44f, 1.0f, -5.79f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(288, 0, 120, 8, 8, 1.0f, true);
        }
    }

    class Momo
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Momo() {
        }

        void act11() {
            this.mtn(262, 0, 1.0f, true);
        }

        void act24() {
            this.mtn(289, 0, 1.0f, true);
        }

        void act24_1() {
            this.mtn(289, 0, 1, 0, 0, 1.0f, true);
        }

        void act25() {
            this.mtn(276, 0, 1.0f, true);
        }

        void act26() {
            this.setTranslate(-9.77f, 0.99f, -5.66f);
            this.mtn(276, 160, 1050, 8, 0, 1.1f, true);
        }

        void act5() {
            this.mtn(258, 0, 1.0f, true);
        }

        void act5_1() {
            this.mtn(258, 0, 1, 0, 0, 1.0f, true);
        }

        void act8() {
            this.mtn(259, 0, 1.0f, true);
        }

        void sit() {
            this.mtn(257, 0, 71, 8, 8, 1.0f, true);
        }
    }

    class Allen
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Allen() {
        }

        void act29() {
            this.mtn(279, 0, 726, 8, 8, 1.0f, true);
        }

        void act33() {
            this.mtn(285, 0, 1.0f, true);
        }

        void act33_1() {
            this.setTranslate(-8.08f, 1.01f, -6.68f);
            this.setRotate(0.0f, 278.28f, 2.5f);
            this.mtn(285, 0, 1, 0, 0, 1.0f, true);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setFov(25.0f);
            float[] fArray = new float[]{1.0f, 4.36f, 0.29f, 3.62f, 90.0f, 4.16f, 0.29f, 3.52f};
            SCE03002.this.BaseCam.setRotate(8.6f, 56.59f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut10() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setFov(40.0f);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -40.77f;
            fArray[2] = 216.56f;
            fArray[4] = 336.0f;
            fArray[5] = -40.77f;
            fArray[6] = 212.76f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(-12.13f, 6.72f, -9.26f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut11() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setFov(25.0f);
            SCE03002.this.BaseCam.setRotate(8.59f, 238.95f, 0.0f);
            SCE03002.this.BaseCam.setTranslate(-11.05f, 1.73f, -6.7f);
        }

        public void cut12() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -8.17f, 2.82f, -5.7f, 162.0f, -8.23f, 2.82f, -5.7f};
            SCE03002.this.BaseCam.setRotate(-32.98f, 83.77f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut13() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setFov(40.0f);
            float[] fArray = new float[]{1.0f, 2.22f, 1.8f, 9.72f, 72.0f, 2.09f, 1.8f, 9.52f};
            SCE03002.this.BaseCam.setRotate(-3.59f, 383.35f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut14() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setFov(25.0f);
            float[] fArray = new float[]{1.0f, -9.81f, 1.62f, -0.83f, 330.0f, -9.8f, 1.62f, -1.05f};
            SCE03002.this.BaseCam.setRotate(1.46f, -2.8f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut15() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -10.29f, 2.18f, -8.66f, 540.0f, -10.25f, 2.18f, -8.15f};
            SCE03002.this.BaseCam.setRotate(-8.49f, 184.59f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut16() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -10.38f, 2.63f, -6.59f, 150.0f, -10.38f, 2.55f, -6.59f};
            SCE03002.this.BaseCam.setRotate(-48.74f, 182.67f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut17() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -9.32f, 2.26f, -6.23f, 600.0f, -8.66f, 2.26f, -6.55f};
            SCE03002.this.BaseCam.setRotate(-7.5f, 122.89f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut19() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -10.92f, 2.23f, -6.07f, 600.0f, -10.76f, 2.23f, -6.03f};
            SCE03002.this.BaseCam.setRotate(-12.0f, 255.7f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut2() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -13.71f;
            fArray[2] = 54.89f;
            fArray[4] = 90.0f;
            fArray[5] = -13.71f;
            fArray[6] = 59.7f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(3.94f, 3.19f, 3.13f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut20() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setRotate(-16.68f, 175.9f, 0.0f);
            SCE03002.this.BaseCam.setTranslate(-10.19f, 2.52f, -7.2f);
        }

        public void cut21() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = 30.86f;
            fArray[2] = 53.84f;
            fArray[4] = 90.0f;
            fArray[5] = 36.22f;
            fArray[6] = 53.86f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(-10.1f, 2.04f, -6.01f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut22() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -10.15f, 3.64f, -8.37f, 480.0f, -10.15f, 3.41f, -8.37f};
            SCE03002.this.BaseCam.setRotate(-32.48f, 181.38f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut23() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -9.31f, 2.42f, -6.1f, 252.0f, -9.24f, 2.42f, -6.07f};
            SCE03002.this.BaseCam.setRotate(7.86f, 247.23f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut24() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -12.31f, 1.91f, -6.83f, 492.0f, -11.95f, 1.91f, -6.68f};
            SCE03002.this.BaseCam.setRotate(1.0f, 247.07f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut25() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setRotate(-14.58f, 179.39f, 0.0f);
            SCE03002.this.BaseCam.setTranslate(-9.74f, 2.39f, -6.81f);
        }

        public void cut26() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -7.15f, 2.9f, -6.76f, 810.0f, -8.55f, 2.68f, -6.79f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -19.52f;
            fArray2[2] = 117.11f;
            fArray2[4] = 810.0f;
            fArray2[5] = -20.96f;
            fArray2[6] = 129.82f;
            float[] fArray3 = fArray2;
            SCE03002.this.BaseCam.rotateSPL(fArray3, 0);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut29() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -5.59f;
            fArray[2] = 19.84f;
            fArray[4] = 162.0f;
            fArray[5] = -4.04f;
            fArray[6] = 21.14f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(-1.97f, 1.63f, 15.99f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut3() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = 10.31f;
            fArray[2] = -40.46f;
            fArray[4] = 60.0f;
            fArray[5] = 9.53f;
            fArray[6] = -40.46f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(-9.59f, 2.11f, -5.91f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut30() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -27.8f;
            fArray[2] = 215.32f;
            fArray[4] = 182.0f;
            fArray[5] = -31.79f;
            fArray[6] = 212.17f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setFov(40.0f);
            SCE03002.this.BaseCam.setTranslate(-11.49f, 5.52f, -9.98f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut31() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -13.24f, 1.95f, -7.83f, 378.0f, -12.72f, 1.95f, -7.54f};
            SCE03002.this.BaseCam.setFov(25.0f);
            SCE03002.this.BaseCam.setRotate(-1.96f, 243.27f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut32() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -9.39f, 2.4f, -6.22f, 252.0f, -9.46f, 2.4f, -6.18f};
            SCE03002.this.BaseCam.setRotate(-12.99f, 122.37f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut33() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -11.2f, 2.99f, -9.2f, 642.0f, -10.33f, 2.99f, -8.5f};
            SCE03002.this.BaseCam.setRotate(-15.25f, 231.15f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut34() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setRotate(-28.37f, 119.05f, 0.0f);
            SCE03002.this.BaseCam.setTranslate(-9.3f, 2.67f, -6.41f);
        }

        public void cut4() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -9.68f, 2.95f, -9.45f, 72.0f, -9.68f, 2.82f, -9.45f};
            SCE03002.this.BaseCam.setRotate(-12.46f, 180.37f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut5() {
            SCE03002.this.Timechk_CutChange();
            SCE03002.this.BaseCam.setRotate(-25.09f, 63.89f, 0.0f);
            SCE03002.this.BaseCam.setTranslate(-8.81f, 2.24f, -5.45f);
        }

        public void cut6() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[12];
            fArray[0] = 1.0f;
            fArray[1] = 7.61f;
            fArray[2] = 111.72f;
            fArray[4] = 210.0f;
            fArray[5] = 8.9f;
            fArray[6] = 109.49f;
            fArray[8] = 336.0f;
            fArray[9] = 16.04f;
            fArray[10] = 104.23f;
            float[] fArray2 = fArray;
            SCE03002.this.BaseCam.setTranslate(-8.55f, 1.7f, -6.38f);
            SCE03002.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut8() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -10.3f, 3.55f, -7.53f, 72.0f, -10.3f, 3.6f, -7.53f};
            SCE03002.this.BaseCam.setRotate(-41.56f, 197.95f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut9() {
            SCE03002.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.66f, 1.1f, -2.7f, 90.0f, -0.85f, 1.1f, -2.75f};
            SCE03002.this.BaseCam.setRotate(-3.72f, 77.05f, 0.0f);
            SCE03002.this.BaseCam.transSPL(fArray, 0);
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

