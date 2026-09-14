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
import xeno.map.MC_SYO01_PRJ;
import xeno.map.MC_SYU01_PRJ;
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

class SCE03014
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03014,
        MC_SYU01_PRJ,
        MC_SYO01_PRJ,
        JNT_Human,
        FLSgaignun_h,
        FLShelmer,
        FLSlapis,
        FLSfried,
        FLSgove_w1 {
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    boolean snd_chk = false;
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
    Effect eft0;
    Effect eft1;
    Gaignun gaignun;
    Helmer helmer;
    Lapis lapis;
    Woman woman;
    Fried fried;
    Unit base;
    Unit baseR;
    Unit mark;
    MAPUnit isu;
    Unit PackG;
    Unit PackG2;
    Unit PackL;
    Unit Npc;
    Unit Tel;
    Unit Tel1;
    Unit Tel2;
    Effect whiteOut;
    Effect blackOut;
    int msgsignal = 128;
    int cutwait = 0;
    boolean msgflag = true;
    Thread msg_clear = Thread.create(this, "msg_clear_thread");
    int msg_clearwait;
    boolean msg_clear_exec;
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
    public static final int CASE_MAX = 4;
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

    SCE03014() {
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

    void Objchk(Unit unit2) {
        System.println("Objchk......(Unit)");
        this.Objchk_exec = true;
        this.ObjchkScale = unit2.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(unit2);
            this.Unit_put(unit2);
            System.sleep(1);
        }
        System.println("Objchk......End");
    }

    void Objchk(Unit unit2, Chr chr, int n) {
        System.println("Objchk......");
        this.Objchk_exec = true;
        this.ObjchkScale = unit2.getScale();
        this.Objchk_scalex = this.ObjchkScale.x;
        this.Objchk_scaley = this.ObjchkScale.y;
        this.Objchk_scalez = this.ObjchkScale.z;
        unit2.setParent(chr, n);
        while (this.Objchk_exec) {
            this.Pad_get();
            this.Objchk_Keychk_srv();
            this.Unit_mov(unit2);
            this.Unit_put(unit2);
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

    void Unit_mov(Unit unit2) {
        unit2.getTranslate();
        unit2.getRotate();
        unit2.px += this.Objchk_dx;
        unit2.py += this.Objchk_dy;
        unit2.pz += this.Objchk_dz;
        unit2.rx += this.Objchk_drx;
        unit2.ry += this.Objchk_dry;
        unit2.rz += this.Objchk_drz;
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
            this.Unit_print(unit2);
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

    void Unit_print(Unit unit2) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(unit2.px, unit2.py, unit2.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(unit2.rx, unit2.ry, unit2.rz);
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

    void Unit_put(Unit unit2) {
        unit2.setTranslate();
        unit2.setRotate();
        this.Objchk_scale = this.Objchk_scalex;
        unit2.setScale(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
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
        this.gold = this.gaignun;
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
                if (++n8 > 4) {
                    n8 = 0;
                }
                n6 = 1;
            }
            if ((n & 0x80) != 0) {
                if (--n8 < 0) {
                    n8 = 4;
                }
                n6 = 1;
            }
            if (n6 != 1) continue;
            n7 = 0;
            this.gold_Visible_true();
            switch (n8) {
                case 0: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.gaignun;
                    break;
                }
                case 1: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.helmer;
                    break;
                }
                case 2: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.lapis;
                    break;
                }
                case 3: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.fried;
                    break;
                }
                case 4: {
                    this.kind_c0_u1 = 0;
                    this.gold = this.woman;
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

    void blackout(int n) {
        this.blackOut = new Effect(0);
        this.blackOut.args[0] = Integer.MIN_VALUE;
        this.blackOut.args[1] = n;
        this.blackOut.args[2] = 0;
        this.blackOut.call(0);
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
        System.println("XEVEFLAG:EV03014_F");
        Runtime.setFlags(325, 1, 1);
        System.println("XEVEJNAME:SCE03015");
        Runtime.jumpEvent(3150);
    }

    public void cleanupOriginal() {
    }

    void crossFade() {
        int[] nArray = new int[5];
        nArray[0] = 0x2000000;
        int[] nArray2 = nArray;
        Runtime.setDefocus(0, 10, nArray2);
        System.sleep(1);
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
        float[] fArray = new float[4];
        Runtime.setLocation(300);
        this.mark = new Unit();
        this.mark.init(24579, 0.0f, 0.51f, -3.17f, 0.0f);
        this.isu = new Mapunits();
        this.isu.init(26);
        this.isu.start(4, null);
        this.isu.setPivot(-0.01f, 0.51f, -3.32f);
        this.isu.setTranslate(0.0f, 0.0f, 0.0f);
        this.isu.setRotate(0.0f, 0.0f, 0.0f);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.BaseCam = Camera.create(1);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.gaignun = new Gaignun();
        this.gaignun.init(16777555, 0.0f, 0.0f, 0.0f, 180.0f);
        this.gaignun.face = this.gaignun.getChild(0x1000000);
        this.gaignun.setTranslate(-0.75f, 0.0f, 2.76f);
        this.gaignun.setRotate(0.0f, 180.0f, 0.0f);
        this.gaignun.setVisible(true);
        this.gaignun.setShadow(5, 16);
        this.lapis = new Lapis();
        this.lapis.init(16777510, 0.0f, 0.0f, 0.0f, 180.0f);
        this.lapis.face = this.lapis.getChild(0x1000000);
        this.lapis.setTranslate(-0.42f, 0.0f, 1.2f);
        this.lapis.setRotate(0.0f, 180.0f, 0.0f);
        this.lapis.setVisible(true);
        this.lapis.setShadow(5, 16);
        this.helmer = new Helmer();
        this.helmer.init(16777507, 0.0f, 0.0f, 0.0f, 180.0f);
        this.helmer.face = this.helmer.getChild(0x1000000);
        this.helmer.setTranslate(0.01f, -0.05f, -3.14f);
        this.helmer.setRotate(0.0f, 0.0f, 0.0f);
        this.helmer.setVisible(false);
        this.helmer.setMotNoUpdate(2);
        this.woman = new Woman();
        this.woman.init(16777992, 0.0f, 0.0f, 0.0f, 180.0f);
        this.woman.face = this.woman.getChild(0x1000000);
        this.woman.setTranslate(0.71f, 0.06f, -3.19f);
        this.woman.setRotate(0.0f, 279.27f, 0.0f);
        this.woman.setVisible(false);
        this.fried = new Fried();
        this.fried.init(0x1000111, 0.0f, 0.0f, 0.0f, 180.0f);
        this.fried.face = this.fried.getChild(0x1000000);
        this.fried.setTranslate(-2.06f, 0.0f, -2.06f);
        this.fried.setRotate(0.0f, 85.0f, 0.0f);
        this.fried.setVisible(false);
        this.fried.look_point(3.66f, 1.91f, -3.77f);
        this.PackG = new Mapunits();
        this.PackG.init(24655, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PackG.setTranslate(0.15f, 0.02f, 0.05f);
        this.PackG.setRotate(20.47f, -11.89f, -5.05f);
        this.PackG.setVisible(false);
        this.PackG.setParent(this.gaignun, 72);
        this.PackG2 = new unit();
        this.PackG2.init(24655, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PackG2.setTranslate(0.13f, 0.05f, -0.04f);
        this.PackG2.setRotate(241.23f, 192.47f, -121.97f);
        this.PackG2.setVisible(false);
        this.PackG2.setParent(this.gaignun, 60);
        this.PackL = new unit();
        this.PackL.init(24655, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PackL.setTranslate(0.13f, 0.0f, -0.03f);
        this.PackL.setRotate(-17.24f, 199.19f, -15.4f);
        this.PackL.setVisible(true);
        this.PackL.setParent(this.lapis, 60);
        this.Npc = new unit();
        this.Npc.init(24597, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Npc.setTranslate(0.12f, -0.02f, 0.03f);
        this.Npc.setRotate(17.0f, 338.34f, -115.97f);
        this.Npc.setVisible(false);
        this.Npc.setParent(this.gaignun, 72);
        this.Tel = new unit();
        this.Tel.init(24657, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Tel.setTranslate(0.09f, -0.02f, 0.02f);
        this.Tel.setRotate(50.33f, 131.67f, 220.7f);
        this.Tel.setVisible(false);
        this.Tel.setParent(this.helmer, 72);
        this.Tel1 = new unit();
        this.Tel1.init(24657, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Tel1.setTranslate(-0.19f, 1.03f, -1.94f);
        this.Tel1.setRotate(0.0f, 162.5f, 0.0f);
        this.Tel1.setVisible(false);
        this.Tel2 = new unit();
        this.Tel2.init(24656, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Tel2.setTranslate(-0.19f, 0.96f, -1.95f);
        this.Tel2.setRotate(0.0f, 157.0f, 0.0f);
        this.Tel2.setVisible(false);
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
        this.loadarc(this.gaignun.face, "FLSgaignun_h.fpk");
        this.loadarc(this.helmer.face, "FLShelmer.fpk");
        this.loadarc(this.lapis.face, "FLSlapis.fpk");
        this.loadarc(this.fried.face, "FLSfried.fpk");
        this.loadarc(this.woman.face, "FLSgove_w1.fpk");
        Runtime.setLocation(300);
        Runtime.setLocation(1000);
        Runtime.setLocation(804);
        this.BaseCam.setTranslate(-3.59f, 1.55f, 2.68f);
        this.BaseCam.setRotate(-9.92f, 309.61f, 0.0f);
        this.CaptureTool();
        this.Timechk();
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        int n = 0;
        int n2 = 0;
        while (n2 == 0) {
            Sound.streamPlay(1390067, 48000);
            this.BaseCam.change();
            System.println("CUT1 START");
            Stage.setVisible(26, false);
            this.gaignun.start(1, "act1");
            this.lapis.start(1, "act1");
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.629f, 0.34f, -0.699f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, -0.329f, 0.525f, 0.785f);
            this.light.setColor(3, 0.35f, 0.35f, 0.35f);
            this.light.setDirection2(3, 0.426f, -0.241f, 0.872f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut1();
            System.sleep(210);
            System.println("CUT1 END");
            System.println("CUT2 START");
            this.gaignun.start(1, "act2");
            this.lapis.start(1, "act2");
            this.helmer.start(1, "act3s");
            this.PackL.start(1, "act2");
            System.sleep(1);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.942f, 0.0f, -0.337f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, -0.752f, 0.341f, 0.564f);
            this.light.setColor(3, 0.35f, 0.35f, 0.35f);
            this.light.setDirection2(3, 0.191f, -0.243f, 0.951f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut2();
            System.sleep(34);
            if (this.snd_chk) {
                Sound.streamPlay(314001);
            }
            this.MSG(30, this.lapis.face, 1, 27, "Open it.");
            System.sleep(345);
            if (this.snd_chk) {
                Sound.streamPlay(314002);
            }
            this.MSG(30, this.gaignun.face, 1, 21, "Helmer?");
            System.println("CUT2 END");
            System.println("CUT3 START");
            Runtime.setLocation(300);
            Stage.setVisible(26, true);
            this.helmer.start(1, "act3");
            this.helmer.setVisible(true);
            this.Tel.setVisible(true);
            this.Tel2.setVisible(true);
            this.lapis.setVisible(false);
            this.gaignun.setVisible(false);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.157f, 0.338f, 0.928f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, 0.895f, 0.442f, -0.06f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, -0.47f, -0.413f, 0.78f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut3();
            System.sleep(30);
            if (this.snd_chk) {
                Sound.streamPlay(314003);
            }
            this.MSG(42, this.helmer.face, 1, 42, "Sorry to keep you waiting, Gaignun.");
            System.sleep(33);
            System.println("CUT3 END");
            System.println("CUT4 START");
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.256f, 0.24f, 0.936f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, -0.757f, 0.452f, -0.473f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, -0.247f, -0.341f, 0.907f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut4();
            if (this.snd_chk) {
                Sound.streamPlay(314004);
            }
            this.MSG(170, this.helmer.face, 1, 168, "I'm in a somewhat difficult position\n myself at the moment, but I'm doing what\nI can with the Federation Parliament.");
            System.println("CUT4 END");
            System.println("CUT5 START");
            Runtime.setLocation(804);
            Stage.setVisible(26, false);
            this.gaignun.start(1, "act5");
            this.lapis.start(1, "act5");
            this.gaignun.setVisible(true);
            this.lapis.setVisible(true);
            this.Tel.setVisible(false);
            this.Tel2.setVisible(false);
            this.helmer.setVisible(false);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.143f, 0.336f, -0.931f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, -0.893f, 0.414f, -0.177f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, 0.939f, -0.34f, -0.054f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut5();
            if (this.snd_chk) {
                Sound.streamPlay(314005);
            }
            this.MSG(130, this.helmer.face, 1, 114, "Now, the woman beside you\nis Captain Lapis Roman.");
            if (this.snd_chk) {
                Sound.streamPlay(314006);
            }
            this.MSG(180, this.helmer.face, 1, 180, "Several years ago, I sent her to\ninfiltrate the military in order to keep\ntabs on the U-TIC members within it.");
            this.MSG(60, this.helmer.face, 1, 60, "She is one of my most\ntrusted subordinates.");
            if (this.snd_chk) {
                Sound.streamPlay(314007);
            }
            this.MSG(42, this.gaignun.face, 1, 42, "Always prepared, aren't you?");
            if (this.snd_chk) {
                Sound.streamPlay(314008);
            }
            this.MSG(75, this.helmer.face, 1, 75, "Caution is something that comes\nnaturally with age.");
            System.println("CUT5 END");
            System.println("CUT6 START");
            Runtime.setLocation(300);
            Stage.setVisible(26, true);
            this.helmer.setVisible(true);
            this.PackG2.setVisible(false);
            this.Tel.setVisible(true);
            this.Tel2.setVisible(true);
            this.lapis.setVisible(false);
            this.gaignun.setVisible(false);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, -0.019f, 0.341f, 0.94f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, 0.852f, 0.521f, 0.05f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, -0.46f, -0.467f, 0.755f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut6();
            if (this.snd_chk) {
                Sound.streamPlay(314009);
            }
            this.MSG(106, this.helmer.face, 1, 106, "Captain Roman will investigate\nthe Durandal's records");
            System.println("CUT6 END");
            System.println("CUT7 START");
            Runtime.setLocation(804);
            Stage.setVisible(26, false);
            this.helmer.setVisible(false);
            this.gaignun.start(1, "act7");
            this.helmer.start(1, "act8s");
            this.gaignun.setVisible(true);
            this.Tel.setVisible(false);
            this.Tel2.setVisible(false);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.069f, 0.233f, -0.97f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, 0.918f, 0.186f, -0.35f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, -0.778f, -0.547f, -0.31f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut7();
            if (this.snd_chk) {
                Sound.streamPlay(314010);
            }
            this.MSG(64, this.helmer.face, 1, 64, "before someone modifies them.");
            if (this.snd_chk) {
                Sound.streamPlay(314011);
            }
            this.MSG(36, this.helmer.face, 1, 36, "Please assist her.");
            if (this.snd_chk) {
                Sound.streamPlay(314012);
            }
            this.MSG(21, this.gaignun.face, 1, 21, "Understood.");
            System.sleep(30);
            System.println("CUT7 END");
            System.println("CUT8 START");
            Runtime.setLocation(300);
            Stage.setVisible(26, true);
            this.helmer.start(1, "act8");
            this.woman.start(1, "act8");
            this.helmer.setVisible(true);
            this.Npc.setVisible(false);
            this.Tel.setVisible(true);
            this.Tel2.setVisible(true);
            this.lapis.setVisible(false);
            this.gaignun.setVisible(false);
            this.woman.setVisible(true);
            this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            this.light.setDirection2(1, 0.152f, 0.235f, 0.96f);
            this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(2, -0.759f, 0.522f, -0.389f);
            this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(3, -0.2f, -0.342f, 0.918f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            this.camerawork.cut8();
            if (this.snd_chk) {
                Sound.streamPlay(314013);
            }
            this.MSG(60, this.gaignun.face, 1, "I'll give her the Durandal's\nmaster key.");
            System.sleep(15);
            if (this.snd_chk) {
                Sound.streamPlay(314014);
            }
            this.MSG(21, this.woman.face, 1, 21, "Sir?");
            System.sleep(39);
            if (this.snd_chk) {
                Sound.streamPlay(314015);
            }
            this.MSG(110, this.woman.face, 1, 110, "There's an EPR com from\nthe CEO of Vector...");
            System.sleep(59);
            if (this.snd_chk) {
                Sound.streamPlay(314016);
            }
            this.MSG(30, this.helmer.face, 1, "Vector?");
            if (this.snd_chk) {
                Sound.streamPlay(314017);
            }
            this.MSG(39, this.helmer.face, 1, 39, "All right. I'll take it.");
            System.sleep(30);
            if (this.snd_chk) {
                Sound.streamPlay(314018);
            }
            this.MSG(30, this.helmer.face, 1, 24, "If you'll excuse me.");
            if (this.snd_chk) {
                Sound.streamPlay(314019);
            }
            this.MSG(82, this.helmer.face, 1, 82, "I'll let you know if there are any developments.");
            if (this.snd_chk) {
                Sound.streamPlay(314020);
            }
            this.MSG(49, this.gaignun.face, 1, 49, "Busy as always...");
            if (this.snd_chk) {
                Sound.streamPlay(314021);
            }
            this.MSG(51, this.helmer.face, 1, 51, "Not half as much as you.");
            System.sleep(117);
            System.println("CUT8 END");
            n = 0;
            while (n < 100) {
                Stage.setVisible(n, false);
                ++n;
            }
            this.helmer.setVisible(false);
            Runtime.mpeg2("3014_1");
            ++n2;
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

    class Monitor
            extends Unit {
        Monitor() {
        }

        void off_s() {
            float f = 1.0f;
            System.sleep(405);
            int n = 0;
            while (n < 20) {
                this.setScale(1.0f, f -= 0.05f, 1.0f);
                System.sleep(1);
                ++n;
            }
            this.signal(0);
        }

        void on_s() {
            float f = 0.0f;
            this.signal(1);
            int n = 0;
            while (n < 20) {
                this.setScale(1.0f, f += 0.05f, 1.0f);
                System.sleep(1);
                ++n;
            }
            this.setScale(1.0f, 1.0f, 1.0f);
        }
    }

    class Mapunits
            extends MAPUnit {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();

        Mapunits() {
        }

        void rot() {
            System.sleep(77);
            System.sleep(5);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = 0.01f;
            fArray[4] = 29.0f;
            fArray[6] = 90.01f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[8];
            fArray3[0] = 1.0f;
            fArray3[2] = 90.01f;
            fArray3[4] = 41.0f;
            fArray3[6] = 180.0f;
            float[] fArray4 = fArray3;
            this.rotSPL.setCtrlVertex(fArray2, 0, 0, (int) fArray2[4]);
            this.rotate(this.rotSPL, 0, true);
            this.rotSPL.setCtrlVertex(fArray4, 0, 1, (int) fArray4[4]);
            this.rotate(this.rotSPL, 0, true);
        }
    }

    class unit
            extends Unit {
        unit() {
        }

        void act2() {
            System.sleep(65);
            SCE03014.this.PackG.setVisible(true);
            System.sleep(1);
            SCE03014.this.PackL.setVisible(false);
            System.sleep(39);
            SCE03014.this.PackG2.setVisible(true);
            System.sleep(1);
            SCE03014.this.PackG.setVisible(false);
            System.sleep(159);
            SCE03014.this.Npc.setVisible(true);
        }

        void act2s() {
            SCE03014.this.PackL.setVisible(true);
            SCE03014.this.PackG.setVisible(true);
            SCE03014.this.PackG2.setVisible(true);
        }
    }

    class Gaignun
            extends Chr {
        Spline posSPL = Spline.create();
        Spline rotSPL = Spline.create();
        Chr face;

        Gaignun() {
        }

        void act1() {
            System.sleep(105);
            this.mtn(258, 0, 105, 8, 8, 0.8f, true);
        }

        void act2() {
            this.setTranslate(-0.75f, 0.0f, 0.69f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(260, 0, 420, 8, 0, 1.0f, true);
        }

        void act2s() {
            this.setTranslate(-0.75f, 0.0f, 0.69f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(260, 0, 65, 8, 0, 1.0f, true);
        }

        void act2s2() {
            this.setTranslate(-0.75f, 0.0f, 0.69f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(260, 0, 105, 8, 0, 1.0f, true);
        }

        void act5() {
            this.setTranslate(-0.3f, 0.0f, 0.49f);
            this.setRotate(0.0f, 126.67f, 0.0f);
            this.mtn(262, 0, 400, 8, 0, 1.0f, true);
        }

        void act7() {
            this.mtn(264, 0, 80, 8, 8, 1.0f, true);
        }
    }

    class Lapis
            extends Chr {
        Chr face;

        Lapis() {
        }

        void act1() {
            System.sleep(84);
            this.mtn(257, 0, 105, 8, 8, 0.8f, true);
        }

        void act2() {
            this.setTranslate(-0.42f, 0.0f, -0.43f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(259, 0, 240, 8, 0, 1.0f, true);
        }

        void act2s() {
            this.setTranslate(-0.42f, 0.0f, -0.43f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(259, 0, 65, 8, 0, 1.0f, true);
        }

        void act5() {
            this.setTranslate(-0.42f, 0.0f, -0.41f);
            this.setRotate(0.0f, 15.0f, 0.0f);
            this.mtn(263, 0, 0, 0, 0, 1.0f, true);
            System.sleep(80);
            this.mtn(263, 0, 320, 8, 0, 1.0f, true);
        }
    }

    class Helmer
            extends Chr {
        Chr face;

        Helmer() {
        }

        void act20() {
            this.setTranslate(-0.04f, 0.01f, -3.37f);
            this.mtn(270, 0, 0, 0, 0, 1.0f, true);
            System.sleep(77);
            this.mtn(270, 0, 332, 8, 0, 1.0f, true);
        }

        void act3() {
            this.mtn(261, 0, 80, 8, 8, 1.0f, true);
        }

        void act3s() {
            this.mtn(261, 0, 0, 0, 0, 1.0f, true);
        }

        void act8() {
            this.setTranslate(0.0f, -0.02f, -3.09f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(266, 45, 780, 8, 0, 1.0f, true);
        }

        void act8s() {
            this.setTranslate(0.0f, -0.02f, -3.09f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(266, 45, 45, 0, 0, 1.0f, true);
        }

        void act9() {
            this.setTranslate(0.0f, 0.13f, -2.97f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(268, 0, 160, 8, 8, 1.0f, true);
        }
    }

    class Fried
            extends Chr {
        Chr face;

        Fried() {
        }

        void act10() {
            this.mtn(267, 0, 160, 8, 8, 1.0f, true);
        }
    }

    class Woman
            extends Chr {
        Chr face;

        Woman() {
        }

        void act8() {
            this.mtn(265, 0, 0, 8, 0, 1.0f, true);
            System.sleep(105);
            this.mtn(265, 1, 285, 8, 0, 1.0f, true);
        }

        void act9() {
            this.mtn(265, 0, 0, 8, 0, 1.0f, true);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut1() {
            SCE03014.this.Timechk_CutChange();
            SCE03014.this.BaseCam.setFov(40.0f);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -25.0f;
            fArray[2] = 357.98f;
            fArray[4] = 210.0f;
            fArray[5] = -28.7f;
            fArray[6] = 357.98f;
            float[] fArray2 = fArray;
            SCE03014.this.BaseCam.setTranslate(-0.33f, 6.0f, 5.34f);
            SCE03014.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut10() {
            SCE03014.this.Timechk_CutChange();
            SCE03014.this.BaseCam.setFov(25.0f);
            float[] fArray = new float[]{1.0f, -4.39f, 1.14f, -0.86f, 282.0f, -4.13f, 1.14f, -0.97f};
            SCE03014.this.BaseCam.setRotate(1.24f, 293.4f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut11() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.57f, 1.4f, -1.83f, 610.0f, 0.09f, 1.4f, -1.88f};
            SCE03014.this.BaseCam.setRotate(-10.74f, 83.93f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut12() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.83f, 1.4f, -2.07f, 150.0f, -1.14f, 1.4f, -2.33f};
            SCE03014.this.BaseCam.setRotate(5.76f, 290.43f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut13() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.83f, 1.3f, -1.14f, 150.0f, -1.85f, 1.3f, -1.23f};
            SCE03014.this.BaseCam.setRotate(-5.5f, 374.21f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut14() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = 5.45f;
            fArray[3] = 3.85f;
            fArray[4] = 210.0f;
            fArray[6] = 5.35f;
            fArray[7] = 3.85f;
            float[] fArray2 = fArray;
            SCE03014.this.BaseCam.setRotate(-28.11f, 0.0f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray2, 0);
        }

        public void cut17() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.83f, 1.4f, -2.07f, 410.0f, -1.14f, 1.4f, -2.33f};
            SCE03014.this.BaseCam.setRotate(5.76f, 290.43f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut18() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 8.78f, 10.62f, 3.91f, 666.0f, 11.76f, 10.62f, 5.85f};
            SCE03014.this.BaseCam.setFov(40.0f);
            SCE03014.this.BaseCam.setRotate(-37.54f, 56.88f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut19() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.7f, 2.73f, 3.37f, 620.0f, -0.7f, 1.51f, 3.37f};
            SCE03014.this.BaseCam.setFov(25.0f);
            SCE03014.this.BaseCam.setRotate(-1.63f, -7.12f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut2() {
            SCE03014.this.Timechk_CutChange();
            SCE03014.this.BaseCam.setFov(25.0f);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -22.47f;
            fArray[2] = 39.27f;
            fArray[4] = 440.0f;
            fArray[5] = -22.47f;
            fArray[6] = 45.26f;
            float[] fArray2 = fArray;
            SCE03014.this.BaseCam.setTranslate(0.81f, 2.28f, 1.77f);
            SCE03014.this.BaseCam.rotateSPL(fArray2, 0);
        }

        public void cut20() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.24f, 1.54f, 1.88f, 332.0f, 1.23f, 1.54f, 1.64f};
            SCE03014.this.BaseCam.setRotate(-8.33f, 9.06f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut3() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[2] = 5.45f;
            fArray[3] = 3.85f;
            fArray[4] = 102.0f;
            fArray[6] = 5.35f;
            fArray[7] = 3.85f;
            float[] fArray2 = fArray;
            SCE03014.this.BaseCam.setRotate(-28.11f, 0.0f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray2, 0);
        }

        public void cut4() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.91f, 1.46f, -1.62f, 204.0f, -1.03f, 1.46f, -1.71f};
            SCE03014.this.BaseCam.setRotate(-13.17f, -38.44f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut5() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.45f, 2.2f, -2.71f, 600.0f, 0.54f, 2.2f, -2.68f};
            SCE03014.this.BaseCam.setRotate(-16.46f, 161.62f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut6() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -0.03f, 1.41f, -0.6f, 206.0f, -0.03f, 1.41f, -0.64f};
            SCE03014.this.BaseCam.setRotate(-9.75f, -0.4f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut7() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 0.31f, 1.65f, -0.61f, 192.0f, 0.28f, 1.65f, -0.55f};
            SCE03014.this.BaseCam.setRotate(0.47f, 154.84f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
        }

        public void cut8() {
            SCE03014.this.Timechk_CutChange();
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = -23.12f;
            fArray[2] = -1.72f;
            fArray[4] = 780.0f;
            fArray[5] = -23.12f;
            fArray[6] = 0.34f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[]{1.0f, -0.13f, 1.98f, -1.13f, 780.0f, -0.13f, 1.98f, -1.29f};
            SCE03014.this.BaseCam.rotateSPL(fArray2, 0);
            SCE03014.this.BaseCam.transSPL(fArray3, 0);
        }

        public void cut9() {
            SCE03014.this.Timechk_CutChange();
            SCE03014.this.BaseCam.setFov(40.0f);
            float[] fArray = new float[]{1.0f, -0.83f, 1.54f, -5.83f, 258.0f, -0.78f, 1.54f, -5.55f};
            SCE03014.this.BaseCam.setRotate(6.12f, 189.64f, 0.0f);
            SCE03014.this.BaseCam.transSPL(fArray, 0);
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

