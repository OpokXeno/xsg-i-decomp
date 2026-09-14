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
import xeno.map.MC_PRO01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Vector4f;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02002B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        JNT_Human,
        EventConstants,
        MC_PRO01_PRJ {
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int menuSelected;
    int i = 0;
    int Chand_R = 20;
    int Chand_L = 26;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Light light = new Light(0);
    Effect FO;
    Effect FI;
    Effect FI2;
    Human ziggy;
    Human yuri;
    Chr dammy;
    Chr U1;
    Chr U2;
    Chr Urobo;
    Chr Urobo2;
    Unit gun;
    Unit gun2;
    Unit syoukaki;
    Unit syoukaki2;
    Unit syoukaki3;
    Unit c4;
    Unit c42;
    Unit c43;
    Unit dummy1;
    Unit dummy2;
    Unit dummy3;
    Unit dummy4;
    Unit dummy5;
    Unit dummy6;
    Unit dummy7;
    Effect eft;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect spark1_ef;
    Effect spark2_ef;
    Effect spark3_ef;
    Effect spark4_ef;
    Effect spark5_ef;
    Effect spark6_ef;
    Effect spark7_ef;
    MAPUnit moni;
    MAPUnit hijikakeL;
    MAPUnit hijikakeR;
    MAPUnit the_sun;
    MAPUnit bar;
    Thread thread_cameratool;
    Input pad1P = Input.create(0);
    Input pad2P = Input.create(1);
    int btn0;
    int edge0;
    int rep0;
    int btn1;
    int edge1;
    int rep1;
    int btn2;
    int edge2;
    int rep2;
    static final int _PADR3 = 1024;
    static final int _PADL3 = 512;
    int printoutcount = 0;
    int selecttime;
    boolean selectrelease = false;
    int canceltime;
    boolean cancelrelease = false;
    int starttime;
    boolean startrelease = false;
    int marutime;
    boolean marurelease = false;
    int sikakutime;
    boolean sikakurelease = false;
    int sankakutime;
    boolean sankakurelease = false;
    int L3time;
    boolean L3release = false;
    int work0;
    int work1;
    float[] CamHistory = new float[70];
    float[] CamHispos = new float[40];
    float[] CamHisrot = new float[40];
    float[] CamHisfov = new float[20];
    boolean[] CamHistory_exist = new boolean[16];
    int[] CamHistory_linktable = new int[]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    int CamHistory_max = 10;
    int CamHistory_ptr = 0;
    int CamHistorywork;
    int CamHistorywork2;
    int CamHistorywork3;
    float CamHistoryposX;
    float CamHistoryposY;
    float CamHistoryposZ;
    float CamHistoryrotX;
    float CamHistoryrotY;
    float CamHistoryrotZ;
    float CamHistoryFov;
    float CamNowposX;
    float CamNowposY;
    float CamNowposZ;
    float CamNowrotX;
    float CamNowrotY;
    float CamNowrotZ;
    float CamNowFov;
    float CamHistoryposXinst;
    float CamHistoryposYinst;
    float CamHistoryposZinst;
    float CamHistoryrotXinst;
    float CamHistoryrotYinst;
    float CamHistoryrotZinst;
    float CamHistoryFovinst;
    int HistoryLook_ptr = 0;
    boolean SPLUPmode;
    boolean SPLDOWNmode;
    boolean manualcamera;
    float[] ManualCamRecord = new float[140];
    float[] ManualCampos = new float[84];
    float[] ManualCamrot = new float[84];
    float[] ManualCamfov = new float[21];
    int Manualcamrecord_dt = 15;
    int Manualcamrecord_max = 20;
    boolean Manualcamrecord_exec = false;
    int Manualcamrecord_time = 0;
    int Manualcamrecord_ptr;
    float Manualcamrec_posX;
    float Manualcamrec_posY;
    float Manualcamrec_posZ;
    float Manualcamrec_rotX;
    float Manualcamrec_rotY;
    float Manualcamrec_rotZ;
    float Manualcamrec_fov;
    int ReplayTime = 100;
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
    int Color_Zwide = 32768;
    int Color_layer = 1;
    int Color_work = 0;
    int Color_work2 = 0;
    int[] ColorScreenParam;
    boolean FocusSet_exec;
    int Focus_z;
    int Focus_zwide;
    int Focus_back;
    int Focus_before;
    int Focus_before_layer;
    int Focus_back_layer;
    static final int Layer_num = 4;
    int Focus_layer;
    boolean Focus_flag;
    int Focus_work0;
    int Focus_work1;
    Camera ManualCam;
    Camera BaseCam;
    Camera STCam;
    float basevpx;
    float basevpy;
    float basevpz;
    float basevrx;
    float basevry;
    float basevrz;
    float basehpx;
    float basehpy;
    float basehpz;
    float basehrx;
    float basehry;
    float basehrz;
    float vpx;
    float vpy;
    float vpz;
    float vrx;
    float vry;
    float vrz;
    float workpx;
    float workpy;
    float workpz;
    float workrx;
    float workry;
    float workrz;
    float axel_pos;
    float axel_rot;
    float speedshaketrans;
    float transCfX;
    float transCfY;
    float transCfZ;
    float rotCfX;
    float rotCfY;
    float rotCfZ;
    int VecTablemax;
    int VecTableCount;
    float VecTableWork;
    float[] VecTable;
    float[] ShakeTable;
    int ShakeTablemax;
    int ShakeTableCountX;
    int ShakeTableCountY;
    int ShakeCycle;
    Thread shakecamera;
    int ShakeCycleArg;
    float ShakePowerArg;
    boolean Vec2zeroFlag;
    boolean threadexec;
    float VIB_transCfX;
    float VIB_transCfY;
    float VIB_transCfZ;
    float VIB_transCfRX;
    float VIB_transCfRY;
    float VIB_transCfRZ;
    int VIB_time;
    Thread vibcamera;
    boolean VIBexec;
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
    boolean keywait_flag;
    Thread timer_thread;
    Thread timer_thread_srv;
    int CutNo;
    boolean Timechk_srv_exec;
    int Timechk_srv_time;
    int Timechk_srv_totaltime;
    int Timechk_srv_totaltime2;
    int Timechk_NextCut;
    Thread Capture_thread;
    Thread Capture_thread_srv;
    boolean Capture_thread_srv_exec;
    Menu Capturemenu;
    int Capture_mode;
    int CaptureFolder;
    String FolderName0;
    String FolderName;
    Menu menu;
    Light CameraTool_light;
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

    SCE02002B() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 1614815232;
        this.ColorScreenParam = nArray;
        this.Focus_z = 131072;
        this.Focus_zwide = 8192;
        this.Focus_back = 1;
        this.Focus_before = 1;
        this.Focus_before_layer = 1;
        this.Focus_back_layer = 3;
        this.Focus_layer = 3;
        this.Focus_work0 = 0;
        this.Focus_work1 = 0;
        this.speedshaketrans = 1.0f;
        this.transCfX = 0.0f;
        this.transCfY = 0.0f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.02f;
        this.rotCfY = 0.02f;
        this.rotCfZ = 0.001f;
        this.VecTablemax = 200;
        this.VecTableCount = 0;
        float[] fArray = new float[200];
        fArray[0] = 1.0f;
        fArray[1] = 1.0f;
        fArray[2] = 1.0f;
        fArray[3] = 1.0f;
        fArray[4] = 1.0f;
        fArray[5] = 1.0f;
        fArray[6] = 1.0f;
        fArray[7] = 1.0f;
        fArray[8] = 1.0f;
        fArray[9] = 1.0f;
        fArray[10] = 1.0f;
        fArray[11] = 1.0f;
        fArray[12] = 1.0f;
        fArray[13] = 1.0f;
        fArray[15] = 1.0f;
        fArray[16] = 1.0f;
        fArray[17] = 1.0f;
        fArray[18] = 1.0f;
        fArray[19] = 1.0f;
        fArray[20] = 1.0f;
        fArray[21] = 1.0f;
        fArray[22] = 1.0f;
        fArray[23] = 1.0f;
        fArray[24] = 1.0f;
        fArray[25] = 1.0f;
        fArray[26] = 1.0f;
        fArray[27] = 1.0f;
        fArray[28] = 1.0f;
        fArray[29] = 1.0f;
        fArray[30] = 1.0f;
        fArray[31] = 1.0f;
        fArray[32] = 1.0f;
        fArray[33] = 1.0f;
        fArray[34] = 1.0f;
        fArray[35] = 1.0f;
        fArray[36] = 1.0f;
        fArray[37] = 1.0f;
        fArray[38] = 1.0f;
        fArray[39] = 1.0f;
        fArray[40] = 1.0f;
        fArray[41] = 1.0f;
        fArray[42] = 1.0f;
        fArray[43] = 1.0f;
        fArray[44] = 1.0f;
        fArray[45] = 1.0f;
        fArray[47] = 1.0f;
        fArray[48] = 1.0f;
        fArray[49] = 1.0f;
        fArray[50] = 1.0f;
        fArray[51] = 1.0f;
        fArray[52] = 1.0f;
        fArray[53] = 1.0f;
        fArray[54] = 1.0f;
        fArray[55] = 1.0f;
        fArray[56] = 1.0f;
        fArray[57] = 1.0f;
        fArray[58] = 1.0f;
        fArray[59] = 1.0f;
        fArray[60] = 1.0f;
        fArray[61] = 1.0f;
        fArray[62] = 1.0f;
        fArray[63] = 1.0f;
        fArray[64] = 1.0f;
        fArray[65] = 1.0f;
        fArray[66] = 1.0f;
        fArray[67] = 1.0f;
        fArray[68] = 1.0f;
        fArray[69] = 1.0f;
        fArray[70] = 1.0f;
        fArray[71] = 1.0f;
        fArray[72] = 1.0f;
        fArray[73] = 1.0f;
        fArray[74] = 1.0f;
        fArray[75] = 1.0f;
        fArray[76] = 1.0f;
        fArray[77] = 1.0f;
        fArray[78] = 1.0f;
        fArray[79] = 1.0f;
        fArray[80] = 1.0f;
        fArray[81] = 1.0f;
        fArray[82] = 1.0f;
        fArray[83] = 1.0f;
        fArray[84] = 1.0f;
        fArray[85] = 1.0f;
        fArray[86] = 1.0f;
        fArray[87] = 1.0f;
        fArray[88] = 1.0f;
        fArray[89] = 1.0f;
        fArray[90] = 1.0f;
        fArray[92] = 1.0f;
        fArray[93] = 1.0f;
        fArray[94] = 1.0f;
        fArray[95] = 1.0f;
        fArray[96] = 1.0f;
        fArray[97] = 1.0f;
        fArray[98] = 1.0f;
        fArray[99] = 1.0f;
        fArray[100] = 1.0f;
        fArray[101] = 1.0f;
        fArray[102] = 1.0f;
        fArray[103] = 1.0f;
        fArray[104] = 1.0f;
        fArray[105] = 1.0f;
        fArray[106] = 1.0f;
        fArray[107] = 1.0f;
        fArray[108] = 1.0f;
        fArray[109] = 1.0f;
        fArray[110] = 1.0f;
        fArray[111] = 1.0f;
        fArray[112] = 1.0f;
        fArray[113] = 1.0f;
        fArray[115] = 1.0f;
        fArray[116] = 1.0f;
        fArray[117] = 1.0f;
        fArray[118] = 1.0f;
        fArray[119] = 1.0f;
        fArray[120] = 1.0f;
        fArray[121] = 1.0f;
        fArray[122] = 1.0f;
        fArray[123] = 1.0f;
        fArray[124] = 1.0f;
        fArray[125] = 1.0f;
        fArray[126] = 1.0f;
        fArray[127] = 1.0f;
        fArray[128] = 1.0f;
        fArray[129] = 1.0f;
        fArray[130] = 1.0f;
        fArray[131] = 1.0f;
        fArray[132] = 1.0f;
        fArray[133] = 1.0f;
        fArray[134] = 1.0f;
        fArray[135] = 1.0f;
        fArray[136] = 1.0f;
        fArray[137] = 1.0f;
        fArray[138] = 1.0f;
        fArray[139] = 1.0f;
        fArray[140] = 1.0f;
        fArray[141] = 1.0f;
        fArray[142] = 1.0f;
        fArray[143] = 1.0f;
        fArray[144] = 1.0f;
        fArray[145] = 1.0f;
        fArray[146] = 1.0f;
        fArray[147] = 1.0f;
        fArray[148] = 1.0f;
        fArray[149] = 1.0f;
        fArray[150] = 1.0f;
        fArray[151] = 1.0f;
        fArray[152] = 1.0f;
        fArray[153] = 1.0f;
        fArray[154] = 1.0f;
        fArray[155] = 1.0f;
        fArray[156] = 1.0f;
        fArray[157] = 1.0f;
        fArray[158] = 1.0f;
        fArray[159] = 1.0f;
        fArray[160] = 1.0f;
        fArray[161] = 1.0f;
        fArray[162] = 1.0f;
        fArray[163] = 1.0f;
        fArray[164] = 1.0f;
        fArray[165] = 1.0f;
        fArray[166] = 1.0f;
        fArray[167] = 1.0f;
        fArray[168] = 1.0f;
        fArray[169] = 1.0f;
        fArray[170] = 1.0f;
        fArray[171] = 1.0f;
        fArray[172] = 1.0f;
        fArray[173] = 1.0f;
        fArray[175] = 1.0f;
        fArray[176] = 1.0f;
        fArray[177] = 1.0f;
        fArray[178] = 1.0f;
        fArray[179] = 1.0f;
        fArray[180] = 1.0f;
        fArray[181] = 1.0f;
        fArray[182] = 1.0f;
        fArray[183] = 1.0f;
        fArray[184] = 1.0f;
        fArray[185] = 1.0f;
        fArray[186] = 1.0f;
        fArray[187] = 1.0f;
        fArray[188] = 1.0f;
        fArray[189] = 1.0f;
        fArray[190] = 1.0f;
        fArray[191] = 1.0f;
        fArray[192] = 1.0f;
        fArray[193] = 1.0f;
        fArray[194] = 1.0f;
        fArray[195] = 1.0f;
        fArray[196] = 1.0f;
        fArray[197] = 1.0f;
        fArray[198] = 1.0f;
        fArray[199] = 1.0f;
        this.VecTable = fArray;
        this.ShakeTable = new float[]{4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, 4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, 4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f, -2.0f, 2.0f, 2.0f, -3.0f, -2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 1.0f, -2.0f, 3.0f, 3.0f, -1.0f, 3.0f, 2.0f, -1.0f, -4.0f, -2.0f, 4.0f, -2.0f, 1.0f, -1.0f, 1.0f, -3.0f, 2.0f, -3.0f, 1.0f, -1.0f};
        this.ShakeTablemax = 180;
        this.ShakeTableCountX = 0;
        this.ShakeTableCountY = 35;
        this.ShakeCycle = 0;
        this.threadexec = false;
        this.VIB_time = 1;
        this.VIBexec = false;
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
        this.Objchk_key1 = 256;
        this.mtnframe = 0;
        this.CutNo = 0;
        this.Capture_thread_srv_exec = false;
        this.CaptureFolder = 0;
        this.FolderName0 = "eventmovie/";
        this.CameraTool_light = new Light(0);
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

    void CamCopy(Camera camera, Camera camera2) {
        camera.setTranslate(camera2.getTranslateX(), camera2.getTranslateY(), camera2.getTranslateZ());
        camera.setRotate(camera2.getRotateX(), camera2.getRotateY(), camera2.getRotateZ());
        camera.setFov(camera2.getFov());
    }

    void CamHistory_clear() {
        System.println("HISTORY CLEAR");
        this.CamHistory_ptr = 0;
        this.CamHistorywork = 0;
        while (this.CamHistorywork != this.CamHistory_max) {
            this.CamHistory_exist[this.CamHistorywork] = false;
            this.CamHistory_linktable[this.CamHistorywork] = -1;
            ++this.CamHistorywork;
        }
        this.CamHistorywork = 0;
        while (this.CamHistorywork != this.CamHistory_max * 7) {
            this.CamHistory[this.CamHistorywork] = 0.0f;
            ++this.CamHistorywork;
        }
    }

    void CamHistory_del(int n) {
        Runtime.setRegister(0, n);
        System.println("HISTORYDELETE PTR=/[$0]");
        this.CamHistorywork = this.CamHistory_linktable[n - 1];
        this.CamHistory_exist[this.CamHistorywork] = false;
        this.CamHistorywork = n - 1;
        while (this.CamHistorywork != this.CamHistory_max - 1) {
            this.CamHistory_linktable[this.CamHistorywork] = this.CamHistory_linktable[this.CamHistorywork + 1];
            ++this.CamHistorywork;
        }
        this.CamHistory_linktable[this.CamHistory_max - 1] = -1;
        --this.CamHistory_ptr;
    }

    void CamHistory_get(int n) {
        if (this.CamHistory_linktable[n] == -1) {
            System.println("ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRROR!!");
        } else {
            this.CamHistorywork3 = this.CamHistory_linktable[n] * 7;
            this.CamHistoryposX = this.CamHistory[this.CamHistorywork3];
            this.CamHistoryposY = this.CamHistory[this.CamHistorywork3 + 1];
            this.CamHistoryposZ = this.CamHistory[this.CamHistorywork3 + 2];
            this.CamHistoryrotX = this.CamHistory[this.CamHistorywork3 + 3];
            this.CamHistoryrotY = this.CamHistory[this.CamHistorywork3 + 4];
            this.CamHistoryrotZ = this.CamHistory[this.CamHistorywork3 + 5];
            this.CamHistoryFov = this.CamHistory[this.CamHistorywork3 + 6];
        }
    }

    void CamHistory_set() {
        if (this.CamHistorychk() != -1) {
            this.CamHistorywork = this.CamHistorychk();
            this.CamHistory_exist[this.CamHistorywork] = true;
            this.CamHistory_linktable[this.CamHistory_ptr] = this.CamHistorywork;
            this.CamHistorywork *= 7;
            this.CamHistory[this.CamHistorywork] = this.ManualCam.getTranslateX();
            this.CamHistory[this.CamHistorywork + 1] = this.ManualCam.getTranslateY();
            this.CamHistory[this.CamHistorywork + 2] = this.ManualCam.getTranslateZ();
            this.CamHistory[this.CamHistorywork + 3] = this.ManualCam.getRotateX();
            this.CamHistory[this.CamHistorywork + 4] = this.ManualCam.getRotateY();
            this.CamHistory[this.CamHistorywork + 5] = this.ManualCam.getRotateZ();
            this.CamHistory[this.CamHistorywork + 6] = this.ManualCam.getFov();
            ++this.CamHistory_ptr;
            Runtime.setRegister(0, this.CamHistory_ptr);
            System.println("HISTORY-SET /[$0]");
        } else {
            System.println("CameraHistory Overflow!!");
        }
    }

    void CamHistory_set_inst() {
        this.CamHistoryposXinst = this.ManualCam.getTranslateX();
        this.CamHistoryposYinst = this.ManualCam.getTranslateY();
        this.CamHistoryposZinst = this.ManualCam.getTranslateZ();
        this.CamHistoryrotXinst = this.ManualCam.getRotateX();
        this.CamHistoryrotYinst = this.ManualCam.getRotateY();
        this.CamHistoryrotZinst = this.ManualCam.getRotateZ();
        this.CamHistoryFovinst = this.ManualCam.getFov();
    }

    int CamHistorychk() {
        this.CamHistorywork = 0;
        while (this.CamHistorywork != this.CamHistory_max) {
            if (!this.CamHistory_exist[this.CamHistorywork]) {
                return this.CamHistorywork;
            }
            ++this.CamHistorywork;
        }
        return -1;
    }

    void CameraTest(int n) {
        System.println("Camera instant test");
        this.CamNowposX = this.ManualCam.getTranslateX();
        this.CamNowposY = this.ManualCam.getTranslateY();
        this.CamNowposZ = this.ManualCam.getTranslateZ();
        this.CamNowrotX = this.ManualCam.getRotateX();
        this.CamNowrotY = this.ManualCam.getRotateY();
        this.CamNowrotZ = this.ManualCam.getRotateZ();
        this.CamNowFov = this.ManualCam.getFov();
        float[] fArray = new float[]{1.0f, this.CamHistoryposXinst, this.CamHistoryposYinst, this.CamHistoryposZinst, 120.0f, this.CamNowposX, this.CamNowposY, this.CamNowposZ};
        float[] fArray2 = new float[]{1.0f, this.CamHistoryrotXinst, this.CamHistoryrotYinst, this.CamHistoryrotZinst, 120.0f, this.CamNowrotX, this.CamNowrotY, this.CamNowrotZ};
        float[] fArray3 = new float[]{1.0f, this.CamHistoryFovinst, 120.0f, this.CamNowFov};
        this.BaseCam.transSPL(fArray, 1);
        this.BaseCam.rotateSPL(fArray2, 1);
        this.BaseCam.fovSPL(fArray3, 1);
        this.STCam.fovSPL(fArray3, 1);
        switch (n) {
            case 16: {
                this.BaseCam.change();
                break;
            }
            case 32: {
                this.STCamera_on();
                break;
            }
            case 128: {
                this.HDCamera_on();
                break;
            }
        }
        this.Systemsleep2(120);
        this.ManualCam.change();
    }

    void CameraTest_History(int n) {
        if (this.CamHistory_ptr >= 1) {
            System.println("Cameratest_history");
            switch (n) {
                case 16: {
                    this.BaseCam.change();
                    break;
                }
                case 32: {
                    this.STCamera_on();
                    break;
                }
                case 128: {
                    this.HDCamera_on();
                    break;
                }
            }
            this.CamHistorywork = 0;
            this.CamHistorywork2 = 0;
            while (this.CamHistorywork <= this.CamHistory_ptr - 1) {
                this.CamHistory_get(this.CamHistorywork);
                this.CamHispos[this.CamHistorywork * 4] = this.CamHistorywork2 * 30 + 1;
                this.CamHispos[this.CamHistorywork * 4 + 1] = this.CamHistoryposX;
                this.CamHispos[this.CamHistorywork * 4 + 2] = this.CamHistoryposY;
                this.CamHispos[this.CamHistorywork * 4 + 3] = this.CamHistoryposZ;
                this.CamHisrot[this.CamHistorywork * 4] = this.CamHistorywork2 * 30 + 1;
                this.CamHisrot[this.CamHistorywork * 4 + 1] = this.CamHistoryrotX;
                this.CamHisrot[this.CamHistorywork * 4 + 2] = this.CamHistoryrotY;
                this.CamHisrot[this.CamHistorywork * 4 + 3] = this.CamHistoryrotZ;
                this.CamHisfov[this.CamHistorywork * 2] = this.CamHistorywork2 * 30 + 1;
                this.CamHisfov[this.CamHistorywork * 2 + 1] = this.CamHistoryFov;
                ++this.CamHistorywork;
                ++this.CamHistorywork2;
            }
            this.CamHistorywork = this.CamHistory_ptr;
            this.CamHistorywork2 = this.CamHistory_ptr - 1;
            while (this.CamHistorywork < this.CamHistory_max) {
                this.CamHispos[this.CamHistorywork * 4] = this.CamHistorywork2 * 30 + 1;
                this.CamHispos[this.CamHistorywork * 4 + 1] = this.CamHistoryposX;
                this.CamHispos[this.CamHistorywork * 4 + 2] = this.CamHistoryposY;
                this.CamHispos[this.CamHistorywork * 4 + 3] = this.CamHistoryposZ;
                this.CamHisrot[this.CamHistorywork * 4] = this.CamHistorywork2 * 30 + 1;
                this.CamHisrot[this.CamHistorywork * 4 + 1] = this.CamHistoryrotX;
                this.CamHisrot[this.CamHistorywork * 4 + 2] = this.CamHistoryrotY;
                this.CamHisrot[this.CamHistorywork * 4 + 3] = this.CamHistoryrotZ;
                this.CamHisfov[this.CamHistorywork * 2] = this.CamHistorywork2 * 30 + 1;
                this.CamHisfov[this.CamHistorywork * 2 + 1] = this.CamHistoryFov;
                ++this.CamHistorywork;
            }
            this.CamHistorywork = (int) this.History_timeset(this.ReplayTime);
            this.Debugprint2();
            this.BaseCam.rotateSPL(this.CamHisrot, 1, this.getSPLmode(), this.ReplayTime);
            this.BaseCam.fovSPL(this.CamHisfov, 1);
            this.STCam.fovSPL(this.CamHisfov, 1);
            Runtime.setRegister(0, this.CamHistorywork);
            System.println("WAIT=/[$0]");
            int n2 = 0;
            Camera camera = Camera.create(6);
            camera.rotateSPL(this.CamHispos, 1, this.getSPLmode(), this.ReplayTime);
            while (n2 <= this.CamHistorywork) {
                this.Pad_get_CameraTool();
                this.BaseCam.setTranslate(camera.getRotateX(), camera.getRotateY(), camera.getRotateZ());
                this.CamCopy(this.ManualCam, this.BaseCam);
                this.ManualCam.setFov(this.BaseCam.getFov());
                System.sleep(1);
                if ((this.edge2 & 0x40) != 0) {
                    n2 = this.CamHistorywork;
                    System.println("Cancel!!");
                }
                ++n2;
            }
            System.sleep(5);
            this.CamCopy(this.ManualCam, this.BaseCam);
            this.Debugprint2();
        } else {
            System.println("History is Empty.");
        }
    }

    void CameraThread() {
        this.waitCameraPlay(1);
        this.cam1.change();
        float[] fArray = new float[]{1.0f, 2.02f, 4.42f, -10.6f, 100.0f, 2.02f, 4.42f, -10.6f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -9.38f;
        fArray2[2] = 1395.47f;
        fArray2[4] = 100.0f;
        fArray2[5] = -18.34f;
        fArray2[6] = 1403.23f;
        float[] fArray3 = fArray2;
        this.cam1.transSPL(fArray, 1, 2, 200);
        this.cam1.rotateSPL(fArray3, 1, 2, 200);
        this.cam1.setFov(51.0f);
        this.waitCameraPlay(2);
        this.cam1.change();
        float[] fArray4 = new float[]{1.0f, 3.68f, 1.99f, -13.33f, 100.0f, 3.68f, 1.99f, -13.33f};
        float[] fArray5 = new float[8];
        fArray5[0] = 1.0f;
        fArray5[1] = -12.37f;
        fArray5[2] = -9.16f;
        fArray5[4] = 100.0f;
        fArray5[5] = -12.37f;
        fArray5[6] = -7.96f;
        float[] fArray6 = fArray5;
        this.cam1.transSPL(fArray4, 1, 1, 100);
        this.cam1.rotateSPL(fArray6, 1, 1, 100);
        this.cam1.setFov(32.8f);
        this.waitCameraPlay(102);
        this.cam1.change();
        float[] fArray7 = new float[]{1.0f, 0.38f, 3.6f, -14.22f, 100.0f, 0.38f, 3.6f, -14.22f};
        float[] fArray8 = new float[8];
        fArray8[0] = 1.0f;
        fArray8[1] = -34.01f;
        fArray8[2] = -74.04f;
        fArray8[4] = 100.0f;
        fArray8[5] = -34.01f;
        fArray8[6] = -84.65f;
        float[] fArray9 = fArray8;
        this.cam1.transSPL(fArray7, 1, 2, 120);
        this.cam1.rotateSPL(fArray9, 1, 2, 120);
        this.cam1.setFov(45.0f);
        this.waitCameraPlay(3);
        this.cam1.change();
        this.cam1.setTranslate(4.62f, 0.32f, -13.13f);
        this.cam1.setRotate(19.62f, 1756.75f, 0.0f);
        this.cam1.setFov(47.6f);
        this.waitCameraPlay(103);
        this.cam1.change();
        float[] fArray10 = new float[]{1.0f, 4.62f, 0.32f, -13.13f, 100.0f, 4.62f, 0.32f, -13.13f};
        float[] fArray11 = new float[8];
        fArray11[0] = 1.0f;
        fArray11[1] = 19.62f;
        fArray11[2] = 1756.75f;
        fArray11[4] = 100.0f;
        fArray11[5] = 19.62f;
        fArray11[6] = 1749.73f;
        float[] fArray12 = fArray11;
        this.cam1.transSPL(fArray10, 1, 3, 30);
        this.cam1.rotateSPL(fArray12, 1, 3, 30);
        this.cam1.setFov(47.4f);
        this.waitCameraPlay(4);
        this.cam1.change();
        float[] fArray13 = new float[]{1.0f, 1.98f, 2.44f, -18.48f, 100.0f, 1.98f, 1.26f, -18.48f};
        float[] fArray14 = new float[8];
        fArray14[0] = 1.0f;
        fArray14[1] = -31.9f;
        fArray14[2] = 1672.01f;
        fArray14[4] = 100.0f;
        fArray14[5] = -9.01f;
        fArray14[6] = 1672.01f;
        float[] fArray15 = fArray14;
        this.cam1.transSPL(fArray13, 1, 3, 60);
        this.cam1.rotateSPL(fArray15, 1, 3, 60);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(5);
        this.cam1.change();
        this.cam1.setTranslate(2.43f, 0.88f, -17.89f);
        this.cam1.setRotate(-7.62f, 1694.38f, 0.0f);
        this.cam1.setFov(32.4f);
        this.waitCameraPlay(6);
        this.cam1.change();
        this.cam1.setTranslate(-4.67f, 0.19f, -6.33f);
        this.cam1.setRotate(9.54f, -40.89f, 0.0f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(7);
        this.cam1.change();
        this.cam1.setTranslate(15.29f, 1.84f, -16.34f);
        this.cam1.setRotate(12.79f, 67.36f, -0.0f);
        this.cam1.setFov(51.0f);
        this.waitCameraPlay(8);
        this.cam1.change();
        this.cam1.setTranslate(-33.06f, 0.49f, 1.87f);
        this.cam1.setRotate(16.65f, 11.75f, 0.0f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(9);
        this.cam1.change();
        float[] fArray16 = new float[]{1.0f, -34.56f, 2.22f, -0.18f, 100.0f, -34.56f, 1.77f, -0.18f};
        float[] fArray17 = new float[8];
        fArray17[0] = 1.0f;
        fArray17[1] = -5.44f;
        fArray17[2] = -78.42f;
        fArray17[4] = 100.0f;
        fArray17[5] = -3.68f;
        fArray17[6] = -78.42f;
        float[] fArray18 = fArray17;
        this.cam1.transSPL(fArray16, 1, 1, 120);
        this.cam1.rotateSPL(fArray18, 1, 1, 120);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(10);
        this.cam1.change();
        this.cam1.setTranslate(-26.67f, 1.64f, -2.31f);
        this.cam1.setRotate(-1.54f, -229.27f, 0.0f);
        this.cam1.setFov(21.2f);
        this.waitCameraPlay(12);
        this.cam1.change();
        this.cam1.setTranslate(-26.67f, 1.64f, -2.31f);
        this.cam1.setRotate(-1.54f, -229.27f, 0.0f);
        this.cam1.setFov(21.2f);
        this.waitCameraPlay(13);
        this.cam1.change();
        float[] fArray19 = new float[]{1.0f, -32.98f, 0.64f, 1.17f, 100.0f, -32.98f, 0.99f, 1.17f};
        float[] fArray20 = new float[8];
        fArray20[0] = 1.0f;
        fArray20[1] = 4.93f;
        fArray20[2] = -80.8f;
        fArray20[4] = 100.0f;
        fArray20[5] = 4.93f;
        fArray20[6] = -80.8f;
        float[] fArray21 = fArray20;
        this.cam1.transSPL(fArray19, 1, 2, 240);
        this.cam1.rotateSPL(fArray21, 1, 2, 240);
        this.cam1.setFov(25.0f);
        this.waitCameraPlay(14);
        this.cam1.change();
        this.cam1.setTranslate(-30.58f, 1.49f, 1.09f);
        this.cam1.setRotate(3.72f, -72.84f, 0.0f);
        this.cam1.setFov(23.0f);
        this.waitCameraPlay(201);
        this.cam1.change();
        float[] fArray22 = new float[]{1.0f, -0.17f, 3.46f, -2.75f, 900.0f, 3.44f, 2.98f, -2.44f};
        float[] fArray23 = new float[8];
        fArray23[0] = 1.0f;
        fArray23[1] = -7.96f;
        fArray23[2] = 97.34f;
        fArray23[4] = 900.0f;
        fArray23[5] = -14.66f;
        fArray23[6] = 97.48f;
        float[] fArray24 = fArray23;
        this.cam1.transSPL(fArray22, 1, 1, 1000);
        this.cam1.rotateSPL(fArray24, 1, 1, 1000);
        this.cam1.setFov(45.8f);
        this.waitCameraPlay(202);
        float[] fArray25 = new float[]{1.0f, -1.11f, 1.15f, -2.55f, 100.0f, -1.11f, 1.05f, -2.55f};
        float[] fArray26 = new float[]{1.0f, -1.48f, 446.47f, -1.28f, 100.0f, -1.48f, 446.47f, -1.28f};
        this.cam2.transSPL(fArray25, 1, 1, 350);
        this.cam2.rotateSPL(fArray26, 1, 1, 350);
        this.cam2.setFov(25.4f);
        this.cam2.change();
        this.waitCameraPlay(203);
        this.cam1.change();
        float[] fArray27 = new float[]{1.0f, -2.73f, 0.82f, -5.11f, 100.0f, -2.96f, 0.82f, -5.05f};
        float[] fArray28 = new float[8];
        fArray28[0] = 1.0f;
        fArray28[1] = 3.44f;
        fArray28[2] = 193.81f;
        fArray28[4] = 100.0f;
        fArray28[5] = 3.44f;
        fArray28[6] = 197.05f;
        float[] fArray29 = fArray28;
        this.cam1.transSPL(fArray27, 1, 2, 300);
        this.cam1.rotateSPL(fArray29, 1, 2, 300);
        this.cam1.setFov(36.6f);
        this.waitCameraPlay(204);
        this.cam1.change();
        float[] fArray30 = new float[]{1.0f, 0.95f, 0.24f, -0.69f, 100.0f, 0.99f, 0.24f, -0.83f};
        float[] fArray31 = new float[8];
        fArray31[0] = 1.0f;
        fArray31[1] = 9.78f;
        fArray31[2] = 74.2f;
        fArray31[4] = 100.0f;
        fArray31[5] = 9.78f;
        fArray31[6] = 74.2f;
        float[] fArray32 = fArray31;
        this.cam1.transSPL(fArray30, 1, 2, 350);
        this.cam1.rotateSPL(fArray32, 1, 2, 350);
        this.cam1.setFov(32.6f);
        this.waitCameraPlay(205);
        this.cam1.change();
        float[] fArray33 = new float[]{1.0f, -1.72f, 0.91f, -2.24f, 100.0f, -1.9f, 0.97f, -2.32f};
        float[] fArray34 = new float[8];
        fArray34[0] = 1.0f;
        fArray34[1] = 5.63f;
        fArray34[2] = 64.67f;
        fArray34[4] = 100.0f;
        fArray34[5] = 5.63f;
        fArray34[6] = 64.67f;
        float[] fArray35 = fArray34;
        this.cam1.transSPL(fArray33, 1, 2, 150);
        this.cam1.rotateSPL(fArray35, 1, 2, 150);
        this.cam1.setFov(26.8f);
        this.waitCameraPlay(206);
        this.cam1.change();
        float[] fArray36 = new float[]{1.0f, -1.49f, 1.5f, -1.78f, 100.0f, -1.39f, 1.54f, -1.65f};
        float[] fArray37 = new float[8];
        fArray37[0] = 1.0f;
        fArray37[1] = 5.68f;
        fArray37[2] = 578.33f;
        fArray37[4] = 100.0f;
        fArray37[5] = 5.68f;
        fArray37[6] = 578.33f;
        float[] fArray38 = fArray37;
        this.cam1.transSPL(fArray36, 1, 1, 150);
        this.cam1.rotateSPL(fArray38, 1, 1, 150);
        this.cam1.setFov(26.8f);
        this.waitCameraPlay(207);
        this.cam1.change();
        this.cam1.setTranslate(-1.47f, 1.04f, -1.61f);
        this.cam1.setRotate(-0.85f, 408.6f, 0.0f);
        this.cam1.setFov(23.8f);
        this.waitCameraPlay(208);
        this.cam1.change();
        float[] fArray39 = new float[]{1.0f, 3.1f, 0.78f, -1.82f, 100.0f, 3.45f, 0.78f, -1.82f};
        float[] fArray40 = new float[8];
        fArray40[0] = 1.0f;
        fArray40[1] = 2.78f;
        fArray40[2] = 450.48f;
        fArray40[4] = 100.0f;
        fArray40[5] = 2.78f;
        fArray40[6] = 450.48f;
        float[] fArray41 = fArray40;
        this.cam1.transSPL(fArray39, 1, 2, 300);
        this.cam1.rotateSPL(fArray41, 1, 2, 300);
        this.cam1.setFov(34.0f);
        this.waitCameraPlay(209);
        this.cam1.change();
        this.cam1.setTranslate(-2.14f, 1.5f, -1.84f);
        this.cam1.setRotate(1.09f, 596.7f, 0.0f);
        this.cam1.setFov(26.8f);
        this.waitCameraPlay(210);
        this.cam1.change();
        float[] fArray42 = new float[]{1.0f, 1.73f, 4.3f, 2.1f, 100.0f, 2.1f, 4.3f, 1.74f};
        float[] fArray43 = new float[8];
        fArray43[0] = 1.0f;
        fArray43[1] = -33.07f;
        fArray43[2] = 763.99f;
        fArray43[4] = 100.0f;
        fArray43[5] = -33.07f;
        fArray43[6] = 763.99f;
        float[] fArray44 = fArray43;
        this.cam1.transSPL(fArray42, 1, 2, 250);
        this.cam1.rotateSPL(fArray44, 1, 2, 250);
        this.cam1.setFov(43.6f);
        this.waitCameraPlay(211);
        this.cam1.change();
        float[] fArray45 = new float[]{1.0f, -0.33f, 1.52f, -1.79f, 100.0f, -0.68f, 1.52f, -2.01f};
        float[] fArray46 = new float[8];
        fArray46[0] = 1.0f;
        fArray46[1] = 3.75f;
        fArray46[2] = 867.29f;
        fArray46[4] = 100.0f;
        fArray46[5] = 3.75f;
        fArray46[6] = 867.29f;
        float[] fArray47 = fArray46;
        this.cam1.transSPL(fArray45, 0, 2, 380);
        this.cam1.rotateSPL(fArray47, 0, 2, 380);
        this.cam1.setFov(28.2f);
        this.waitCameraPlay(212);
        this.cam1.change();
        float[] fArray48 = new float[]{1.0f, -0.66f, 0.73f, -2.16f, 100.0f, -0.76f, 0.73f, -2.79f};
        float[] fArray49 = new float[8];
        fArray49[0] = 1.0f;
        fArray49[1] = 5.93f;
        fArray49[2] = 819.5f;
        fArray49[4] = 100.0f;
        fArray49[5] = 5.93f;
        fArray49[6] = 819.5f;
        float[] fArray50 = fArray49;
        this.cam1.transSPL(fArray48, 1, 1, 600);
        this.cam1.rotateSPL(fArray50, 1, 1, 600);
        this.cam1.setFov(32.8f);
        this.waitCameraPlay(213);
        this.cam1.change();
        float[] fArray51 = new float[]{1.0f, -1.41f, 1.03f, -2.72f, 100.0f, -1.67f, 1.08f, -2.76f};
        float[] fArray52 = new float[8];
        fArray52[0] = 1.0f;
        fArray52[1] = 2.37f;
        fArray52[2] = 808.7f;
        fArray52[4] = 100.0f;
        fArray52[5] = 2.37f;
        fArray52[6] = 808.7f;
        float[] fArray53 = fArray52;
        this.cam1.transSPL(fArray51, 1, 2, 600);
        this.cam1.rotateSPL(fArray53, 1, 2, 600);
        this.cam1.setFov(24.6f);
        this.waitCameraPlay(16);
        this.cam1.change();
        float[] fArray54 = new float[]{1.0f, -30.12f, 1.22f, 0.61f, 100.0f, -30.61f, 0.55f, 2.25f};
        float[] fArray55 = new float[8];
        fArray55[0] = 1.0f;
        fArray55[1] = 22.77f;
        fArray55[2] = -19.78f;
        fArray55[4] = 100.0f;
        fArray55[5] = 22.77f;
        fArray55[6] = -19.78f;
        float[] fArray56 = fArray55;
        this.cam1.transSPL(fArray54, 1, 1, 300);
        this.cam1.rotateSPL(fArray56, 1, 1, 300);
        this.cam1.setFov(38.6f);
    }

    void CameraTool() {
    }

    void CameraTool_main() {
        System.println("*********CameraTool Standby*****************");
        this.STCamera_init();
        while (true) {
            this.Pad_get_CameraTool();
            if ((this.btn2 & 0x100) != 0) {
                ++this.selecttime;
                this.selectrelease = false;
            } else {
                this.selecttime = 0;
                this.selectrelease = true;
            }
            if ((this.btn2 & 0x40) != 0) {
                ++this.canceltime;
                this.cancelrelease = false;
            } else {
                this.canceltime = 0;
                this.cancelrelease = true;
            }
            if ((this.btn2 & 0x800) != 0) {
                ++this.starttime;
                this.startrelease = false;
            } else {
                this.starttime = 0;
                this.startrelease = true;
            }
            if ((this.btn2 & 0x20) != 0) {
                ++this.marutime;
                this.marurelease = false;
            } else {
                this.marutime = 0;
                this.marurelease = true;
            }
            if ((this.btn2 & 0x80) != 0) {
                ++this.sikakutime;
                this.sikakurelease = false;
            } else {
                this.sikakutime = 0;
                this.sikakurelease = true;
            }
            if ((this.btn2 & 0x10) != 0) {
                ++this.sankakutime;
                this.sankakurelease = false;
            } else {
                this.sankakutime = 0;
                this.sankakurelease = true;
            }
            if ((this.btn2 & 0x200) != 0) {
                ++this.L3time;
                this.L3release = false;
            } else {
                this.L3time = 0;
                this.L3release = true;
            }
            if ((this.btn2 & 0x800) != 0 && this.starttime == 30) {
                this.ReplayTime = this.inputNum(this.ReplayTime);
            }
            if ((this.btn2 & 0x100) != 0 && this.selecttime == 30) {
                this.CamHistory_set();
            }
            if ((this.btn2 & 0x40) != 0 && this.canceltime == 30) {
                this.CamHistory_clear();
            }
            if ((this.btn2 & 0x20) != 0 && this.marutime == 30) {
                this.ManualCam_record();
            }
            if ((this.btn2 & 0x80) != 0 && this.sikakutime == 30) {
                this.FocusSet();
            }
            if ((this.btn2 & 0x10) != 0 && this.sankakutime == 30) {
                this.ColorScreenSet();
            }
            if ((this.btn2 & 0x800) != 0) {
                if ((this.edge2 & 0x10) != 0) {
                    this.CameraTest(16);
                }
                if ((this.edge2 & 0x20) != 0) {
                    this.CameraTest(32);
                }
                if ((this.edge2 & 0x80) != 0) {
                    this.CameraTest(128);
                }
                if ((this.edge2 & 0x40) != 0 && this.HistoryLook_ptr != 0) {
                    this.CamHistory_del(this.HistoryLook_ptr);
                    if (this.CamHistory_ptr == 0) {
                        this.HistoryLook_ptr = 0;
                        this.ManualCam.change();
                    }
                }
                if ((this.edge2 & 8) != 0) {
                    this.HistoryLook_inc();
                    this.HistoryLook(this.HistoryLook_ptr);
                }
                if ((this.edge2 & 2) != 0) {
                    this.HistoryLook_dec();
                    this.HistoryLook(this.HistoryLook_ptr);
                }
            } else if ((this.btn2 & 0x100) != 0) {
                if ((this.edge2 & 0x10) != 0) {
                    this.CameraTest_History(16);
                }
                if ((this.edge2 & 0x20) != 0) {
                    this.CameraTest_History(32);
                }
                if ((this.edge2 & 0x80) != 0) {
                    this.CameraTest_History(128);
                }
                if ((this.edge2 & 0x1000) != 0) {
                    this.CamCopy(this.ManualCam, this.BaseCam);
                    this.ManualCam.change();
                }
                if ((this.edge2 & 0x4000) != 0) {
                    this.BaseCam.change();
                }
            }
            if ((this.edge2 & 0x10) != 0) {
                this.ManualCam_play();
            }
            if ((this.edge2 & 0x800) != 0) {
                this.Put_param();
                this.STCamera_off(this.ManualCam);
            }
            if ((this.edge2 & 0x800) != 0) {
                this.HistoryLook_ptr = 0;
            }
            if ((this.edge2 & 0x100) != 0) {
                this.CamHistory_set_inst();
            }
            if ((this.edge2 & 0x200) != 0) {
                this.SPLUPmode = !this.SPLUPmode;
                this.Debugprint7();
            }
            if ((this.edge2 & 0x400) != 0) {
                this.SPLDOWNmode = !this.SPLDOWNmode;
                this.Debugprint7();
            }
            System.sleep(1);
        }
    }

    void CaptureEnd_CaptureTool() {
        Runtime.CaptureEnd();
    }

    void CaptureStart_CaptureTool() {
        if (this.CaptureFolder < 10 && this.Capture_mode != 36864) {
            this.msg.clear();
        }
        System.println("*********Capture Start!!!*****************");
        Runtime.setRegister(0, this.Capture_mode);
        System.println("CaptureMode = /[$0]");
        Runtime.CaptureStart(this.FolderName, this.Capture_mode);
    }

    void CaptureTool() {
        System.sleep(1);
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
                    this.menu.addItem("全画面（アルファ）");
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
                        System.println("CaptureMODE ---All Screen(alpha).");
                        this.Capture_mode = 32769;
                        bl = false;
                    }
                    if (n2 == 2) {
                        System.println("CaptureMODE ---SoftImage BG(pic)");
                        this.Capture_mode = 33024;
                        bl = false;
                    }
                    if (n2 == 3) {
                        System.println("CaptureMODE ---SoftImage BG(pic+zpic)");
                        this.Capture_mode = 256;
                        bl = false;
                    }
                    if (n2 == 4) {
                        System.println("CaptureMODE ---Chr(pic)");
                        this.Capture_mode = 33280;
                        n = 1;
                    }
                    if (n2 == 5) {
                        System.println("CaptureMODE ---Chr(pic+zpic)");
                        this.Capture_mode = 512;
                        n = 1;
                    }
                    if (n2 == 6) {
                        System.println("CaptureMODE ---MObj(pic)");
                        this.Capture_mode = 33536;
                        n = 1;
                    }
                    if (n2 == 7) {
                        System.println("CaptureMODE ---MObj(pic+zpic)");
                        this.Capture_mode = 768;
                        n = 1;
                    }
                    if (n2 == 8) {
                        System.println("CaptureMODE ---TXT(pic)");
                        this.Capture_mode = 36864;
                        bl = false;
                    }
                    if (n2 == 9) {
                        System.println("CaptureMODE ---Particle(pic)");
                        this.Capture_mode = 34816;
                        bl = false;
                    }
                    if (n2 != 10) break;
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
                        this.menu.addItem("ｍｏｖｉｅ０３（たけなか）");
                        this.menu.addItem("ｍｏｖｉｅ０４（なかむら）");
                        this.menu.addItem("ｍｏｖｉｅ０５（いけうち）");
                        this.menu.addItem("ｍｏｖｉｅ０６（いけうち）");
                        this.menu.addItem("ｍｏｖｉｅ０７（ふち）");
                        this.menu.addItem("ｍｏｖｉｅ０８（ふち）");
                        this.menu.addItem("ｍｏｖｉｅ０９（保存）");
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
                        this.menu.addItem("ｍｏｖｉｅ１６（おの７６５）");
                        this.menu.addItem("ｍｏｖｉｅ１７（うなかみ）");
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
        this.NextCutWait();
        this.CaptureStart_CaptureTool();
        System.sleep(2);
        this.NextCutWait();
        this.CaptureEnd_CaptureTool();
        System.println("*********Capture End.*****************");
        this.Capture_thread_srv_exec = false;
    }

    void ColorScreenSet() {
        this.ColorScreenSet_exec = true;
        System.println("ColorScreen  SetMode");
        while (this.ColorScreenSet_exec) {
            this.Pad_get();
            if ((this.edge2 & 0x100) != 0) {
                this.ColorScreen_printout();
            }
            if ((this.edge2 & 0x800) != 0) {
                this.ColorScreenSet_exec = false;
            }
            if ((this.edge2 & 0x200) != 0) {
                ++this.Color_layer;
                this.Color_layer %= 5;
                Runtime.setRegister(0, this.Color_layer);
                System.println("Layer = /[$0]");
            }
            if ((this.btn2 & 0xF0) == 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    this.Color_Z -= 1000;
                }
                if ((this.btn2 & 0x4000) != 0) {
                    this.Color_Z += 1000;
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
            System.sleep(1);
        }
        System.println("ScreenColor SetMode-------end");
    }

    void ColorScreen_printout() {
        this.ColorScreenParam[2] = this.Color_Z;
        this.ColorScreenParam[3] = this.Color_A * 0x1000000 + this.Color_B * 65536 + this.Color_G * 256 + this.Color_R;
        Runtime.setRegister(0, this.ColorScreenParam[3]);
        Runtime.setRegister(1, this.ColorScreenParam[2]);
        if (this.Color_layer != 0) {
            System.println("*************ColorScreen Printout*************");
            System.println("ColorScreenParam = {");
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
    }

    void Debugprint() {
        System.println("*********DEBUGPRINT*********");
    }

    void Debugprint2() {
        System.println("*********CameraHistory SPL*********");
        System.println("pos[] ={");
        this.work0 = 0;
        while (this.work0 <= (this.CamHistory_ptr - 1) * 4) {
            this.setRegTXYZ(this.CamHispos[this.work0], this.CamHispos[this.work0 + 1], this.CamHispos[this.work0 + 2], this.CamHispos[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("rot[] ={");
        this.work0 = 0;
        while (this.work0 <= (this.CamHistory_ptr - 1) * 4) {
            this.setRegTXYZ(this.CamHisrot[this.work0], this.CamHisrot[this.work0 + 1], this.CamHisrot[this.work0 + 2], this.CamHisrot[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("fov[] ={");
        this.work0 = 0;
        while (this.work0 <= (this.CamHistory_ptr - 1) * 2) {
            this.setRegTF(this.CamHisfov[this.work0], this.CamHisfov[this.work0 + 1]);
            System.println("/[#0]f,/[#1]f,");
            this.work0 += 2;
        }
        System.println("};");
    }

    void Debugprint3() {
        System.println("*********DEBUGPRINT3*********");
        System.println("LINKTABLE");
        this.work0 = 0;
        while (this.work0 < this.CamHistory_max) {
            System.println(" " + this.CamHistory_linktable[this.work0] + "," + this.CamHistory_linktable[this.work0 + 1] + "," + this.CamHistory_linktable[this.work0 + 2] + "," + this.CamHistory_linktable[this.work0 + 3]);
            this.work0 += 4;
        }
    }

    void Debugprint4() {
        System.println("*********DEBUGPRINT4*********");
        System.println("HISPTR" + this.CamHistory_ptr);
        System.println("CAMHIS");
        this.work0 = 0;
        while (this.work0 <= this.CamHistory_ptr * 7) {
            System.println(" " + this.CamHistory[this.work0] + " " + this.CamHistory[this.work0 + 1] + " " + this.CamHistory[this.work0 + 2] + " " + this.CamHistory[this.work0 + 3] + " " + this.CamHistory[this.work0 + 4] + " " + this.CamHistory[this.work0 + 5] + " " + this.CamHistory[this.work0 + 6]);
            this.work0 += 7;
        }
    }

    void Debugprint5() {
        System.println("*********ManualCamera Replay*********");
        System.println("pos[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 4) {
            this.setRegTXYZ(this.ManualCampos[this.work0], this.ManualCampos[this.work0 + 1], this.ManualCampos[this.work0 + 2], this.ManualCampos[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("rot[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 4) {
            this.setRegTXYZ(this.ManualCamrot[this.work0], this.ManualCamrot[this.work0 + 1], this.ManualCamrot[this.work0 + 2], this.ManualCamrot[this.work0 + 3]);
            System.println("/[#0]f,/[#1]f,/[#2]f,/[#3]f,");
            this.work0 += 4;
        }
        System.println("};");
        System.println("fov[] ={");
        this.work0 = 0;
        while (this.work0 <= this.Manualcamrecord_ptr * 2) {
            this.setRegTF(this.ManualCamfov[this.work0], this.ManualCamfov[this.work0 + 1]);
            System.println("/[#0]f,/[#1]f,");
            this.work0 += 2;
        }
        System.println("};");
    }

    void Debugprint6() {
        System.println("*********DEBUGPRINT6*********");
        System.println("MANUALCAMPTR" + this.Manualcamrecord_ptr);
        System.println("MANUALCAMHIS");
        this.work0 = 0;
        while (this.work0 < (this.Manualcamrecord_ptr - 1) * 7) {
            System.println(" " + this.ManualCamRecord[this.work0] + " " + this.ManualCamRecord[this.work0 + 1] + " " + this.ManualCamRecord[this.work0 + 2] + " " + this.ManualCamRecord[this.work0 + 3] + " " + this.ManualCamRecord[this.work0 + 4] + " " + this.ManualCamRecord[this.work0 + 5] + " " + this.ManualCamRecord[this.work0 + 6]);
            this.work0 += 7;
        }
    }

    void Debugprint7() {
        if (this.SPLDOWNmode) {
            if (this.SPLUPmode) {
                System.println("PATH = SPLINE");
            } else {
                System.println("PATH = DOWN");
            }
        } else if (this.SPLUPmode) {
            System.println("PATH = UP");
        } else {
            System.println("PATH = LINEAR");
        }
    }

    void Earthquake_on() {
        this.STCamera_off();
        this.axel_pos = 5.0E-5f;
        this.axel_rot = 0.1f;
        this.transCfX = 0.0f;
        this.transCfY = 0.001f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.02f;
        this.rotCfY = 0.02f;
        this.rotCfZ = 0.05f;
        this.ShakeCycleArg = 1;
        this.ShakePowerArg = 0.1f;
        this.Vec2zeroFlag = true;
        this.vpx = 0.0f;
        this.vpy = 0.0f;
        this.vpz = 0.0f;
        this.vrx = 0.0f;
        this.vry = 0.0f;
        this.vrz = 0.0f;
        this.shakecamera = Thread.create(this, "ShakeRoutine_main");
        this.shakecamera.start();
        this.STCam.change();
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
            if ((this.edge2 & 0x100) != 0) {
                this.Focus_printout();
            }
            if ((this.edge2 & 0x800) != 0) {
                this.FocusSet_exec = false;
            }
            if ((this.edge2 & 0x400) != 0) {
                ++this.Focus_layer;
                if (this.Focus_layer > 4) {
                    this.Focus_layer = 0;
                }
                Runtime.setRegister(0, this.Focus_layer);
                System.println("Layer = /[$0]");
            }
            if ((this.edge2 & 0x200) != 0) {
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
                if (this.Focus_z + this.Focus_zwide * this.Focus_work1 > 0) {
                    Runtime.setDefocusQuick(this.Focus_work0, 0, 0, 0);
                    Runtime.setDefocusQuick(this.Focus_work0, 2, this.Focus_z + this.Focus_zwide * this.Focus_work1, this.Focus_before);
                }
                ++this.Focus_work0;
            }
        }
        if (this.Focus_back_layer != 0) {
            this.Focus_work1 = 0;
            while (this.Focus_work1 != this.Focus_back_layer) {
                ++this.Focus_work1;
                if (this.Focus_z - this.Focus_zwide * this.Focus_work1 > 0) {
                    Runtime.setDefocusQuick(this.Focus_work0, 0, 0, 0);
                    Runtime.setDefocusQuick(this.Focus_work0, 1, this.Focus_z - this.Focus_zwide * this.Focus_work1, this.Focus_back);
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

    float GetAxel(float f, float f2, float f3) {
        if (f < f2) {
            return f3 * -1.0f;
        }
        return f3;
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
            this.Pad_get();
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

    void HDCamera_on() {
        this.STCamera_off();
        this.axel_pos = 1.0E-4f;
        this.axel_rot = 0.01f;
        this.transCfX = 0.0f;
        this.transCfY = 0.001f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.02f;
        this.rotCfY = 0.02f;
        this.rotCfZ = 0.05f;
        this.ShakeCycleArg = 8;
        this.ShakePowerArg = 0.02f;
        this.Vec2zeroFlag = true;
        this.vpx = 0.0f;
        this.vpy = 0.0f;
        this.vpz = 0.0f;
        this.vrx = 0.0f;
        this.vry = 0.0f;
        this.vrz = 0.0f;
        this.shakecamera = Thread.create(this, "ShakeRoutine_main");
        this.shakecamera.start();
        this.STCam.change();
    }

    void HistoryLook(int n) {
        Runtime.setRegister(0, n);
        System.println("HISTORYLOOK PTR=/[$0]");
        if (n != 0) {
            this.CamHistory_get(n - 1);
            this.BaseCam.setTranslate(this.CamHistoryposX, this.CamHistoryposY, this.CamHistoryposZ);
            this.BaseCam.setRotate(this.CamHistoryrotX, this.CamHistoryrotY, this.CamHistoryrotZ);
            this.BaseCam.setFov(this.CamHistoryFov);
            this.BaseCam.change();
        }
    }

    void HistoryLook_dec() {
        if (this.CamHistory_ptr != 0 && this.HistoryLook_ptr > 1) {
            --this.HistoryLook_ptr;
        }
    }

    void HistoryLook_inc() {
        if (this.HistoryLook_ptr < this.CamHistory_ptr) {
            ++this.HistoryLook_ptr;
        }
    }

    float History_timeset(int n) {
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 1.0f;
        int n2 = 1;
        while (n2 <= this.CamHistory_ptr - 1) {
            f += this.Abs(this.CamHispos[n2 * 4 + 1] - this.CamHispos[(n2 - 1) * 4 + 1]);
            f += this.Abs(this.CamHispos[n2 * 4 + 2] - this.CamHispos[(n2 - 1) * 4 + 2]);
            f += this.Abs(this.CamHispos[n2 * 4 + 3] - this.CamHispos[(n2 - 1) * 4 + 3]);
            f2 += this.Abs(this.CamHisrot[n2 * 4 + 1] - this.CamHisrot[(n2 - 1) * 4 + 1]);
            f2 += this.Abs(this.CamHisrot[n2 * 4 + 2] - this.CamHisrot[(n2 - 1) * 4 + 2]);
            f2 += this.Abs(this.CamHisrot[n2 * 4 + 3] - this.CamHisrot[(n2 - 1) * 4 + 3]);
            ++n2;
        }
        float f4 = f / (float) (n - 1);
        float f5 = f2 / (float) (n - 1);
        n2 = 1;
        while (n2 <= this.CamHistory_ptr - 1) {
            float f6 = 0.0f;
            f6 += this.Abs(this.CamHispos[n2 * 4 + 1] - this.CamHispos[(n2 - 1) * 4 + 1]);
            f6 += this.Abs(this.CamHispos[n2 * 4 + 2] - this.CamHispos[(n2 - 1) * 4 + 2]);
            f6 += this.Abs(this.CamHispos[n2 * 4 + 3] - this.CamHispos[(n2 - 1) * 4 + 3]);
            float f7 = 0.0f;
            f7 += this.Abs(this.CamHisrot[n2 * 4 + 1] - this.CamHisrot[(n2 - 1) * 4 + 1]);
            f7 += this.Abs(this.CamHisrot[n2 * 4 + 2] - this.CamHisrot[(n2 - 1) * 4 + 2]);
            f7 += this.Abs(this.CamHisrot[n2 * 4 + 3] - this.CamHisrot[(n2 - 1) * 4 + 3]);
            if (f4 != 0.0f) {
                this.CamHispos[n2 * 4] = f6 / f4;
                int n3 = n2 * 4;
                this.CamHispos[n3] = this.CamHispos[n3] + this.CamHispos[(n2 - 1) * 4];
                this.CamHisrot[n2 * 4] = this.CamHispos[n2 * 4];
                this.CamHisfov[n2 * 2] = this.CamHispos[n2 * 4];
                f3 = this.CamHisrot[n2 * 4];
            } else if (f5 != 0.0f) {
                this.CamHisrot[n2 * 4] = f7 / f5;
                int n4 = n2 * 4;
                this.CamHisrot[n4] = this.CamHisrot[n4] + this.CamHisrot[(n2 - 1) * 4];
                this.CamHispos[n2 * 4] = this.CamHisrot[n2 * 4];
                this.CamHisfov[n2 * 2] = this.CamHisrot[n2 * 4];
                f3 = this.CamHisrot[n2 * 4];
            } else {
                f3 = n;
            }
            ++n2;
        }
        n2 = this.CamHistory_ptr;
        while (n2 < this.CamHistory_max) {
            this.CamHispos[n2 * 4] = f3;
            this.CamHisrot[n2 * 4] = f3;
            this.CamHisfov[n2 * 2] = f3;
            ++n2;
        }
        return f3;
    }

    void Keywait() {
        this.keywait_flag = true;
        while (this.keywait_flag) {
            this.Pad_get();
            if ((this.edge0 & 0x20) != 0) {
                this.keywait_flag = false;
            }
            if ((this.edge0 & 0x10) != 0) {
                this.keywait_flag = false;
            }
            if ((this.edge0 & 0x80) != 0) {
                this.keywait_flag = false;
            }
            System.sleep(1);
        }
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
        this.BaseCam.transSPL(this.ManualCampos, 1);
        this.BaseCam.rotateSPL(this.ManualCamrot, 1);
        this.BaseCam.fovSPL(this.ManualCamfov, 1);
        this.BaseCam.change();
        this.Systemsleep2(this.Manualcamrecord_time);
        this.ManualCam.change();
        this.Debugprint5();
        return 0;
    }

    int ManualCam_record() {
        System.println("ManualCam_record.");
        System.println("ManualCamera MOVE to start.");
        this.Manualcamrecord_exec = true;
        System.sleep(5);
        this.CamHistoryposX = this.ManualCam.getTranslateX();
        this.CamHistoryposY = this.ManualCam.getTranslateY();
        this.CamHistoryposZ = this.ManualCam.getTranslateZ();
        this.CamHistoryrotX = this.ManualCam.getRotateX();
        this.CamHistoryrotY = this.ManualCam.getRotateY();
        this.CamHistoryrotZ = this.ManualCam.getRotateZ();
        this.CamHistoryFov = this.ManualCam.getFov();
        while (this.Manualcamrecord_exec) {
            if (this.CamHistoryposX != this.ManualCam.getTranslateX()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryposY != this.ManualCam.getTranslateY()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryposZ != this.ManualCam.getTranslateZ()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotX != this.ManualCam.getRotateX()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotY != this.ManualCam.getRotateY()) {
                this.Manualcamrecord_exec = false;
            }
            if (this.CamHistoryrotZ != this.ManualCam.getRotateZ()) {
                this.Manualcamrecord_exec = false;
            }
            this.Pad_get_CameraTool();
            if ((this.edge2 & 0x40) != 0) {
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
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7] = this.ManualCam.getTranslateX();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 1] = this.ManualCam.getTranslateY();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 2] = this.ManualCam.getTranslateZ();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 3] = this.ManualCam.getRotateX();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 4] = this.ManualCam.getRotateY();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 5] = this.ManualCam.getRotateZ();
                this.ManualCamRecord[this.Manualcamrecord_ptr * 7 + 6] = this.ManualCam.getFov();
                ++this.Manualcamrecord_ptr;
                if (this.Manualcamrecord_ptr >= this.Manualcamrecord_max) {
                    System.println("End(TimeOver).");
                    this.Manualcamrecord_exec = false;
                }
            }
            this.Pad_get_CameraTool();
            if ((this.edge2 & 0x200) != 0) {
                System.println("End(Button).");
                Runtime.setRegister(0, this.Manualcamrecord_time + 1);
                System.println("Time = /[$0]");
                this.Manualcamrecord_exec = false;
            }
            if ((this.edge2 & 0x400) != 0) {
                System.println("End(Button).");
                Runtime.setRegister(0, this.Manualcamrecord_time + 1);
                System.println("Time = /[$0]");
                this.Manualcamrecord_exec = false;
            }
            ++this.Manualcamrecord_time;
            System.sleep(1);
        }
        this.Manualcamrec_posX = this.ManualCam.getTranslateX();
        this.Manualcamrec_posY = this.ManualCam.getTranslateY();
        this.Manualcamrec_posZ = this.ManualCam.getTranslateZ();
        this.Manualcamrec_rotX = this.ManualCam.getRotateX();
        this.Manualcamrec_rotY = this.ManualCam.getRotateY();
        this.Manualcamrec_rotZ = this.ManualCam.getRotateZ();
        this.Manualcamrec_fov = this.ManualCam.getFov();
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
        if ((this.edge1 & 0x200) != 0) {
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
        if ((this.edge1 & 0x200) != 0) {
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
        this.btn0 = this.pad1P.getButton() | this.pad2P.getButton();
        this.edge0 = this.pad1P.getEdge() | this.pad2P.getEdge();
        this.rep0 = this.pad1P.getRepeat() | this.pad2P.getRepeat();
        this.btn1 = this.pad1P.getButton();
        this.edge1 = this.pad1P.getEdge();
        this.rep1 = this.pad1P.getRepeat();
        this.btn2 = this.pad2P.getButton();
        this.edge2 = this.pad2P.getEdge();
        this.rep2 = this.pad2P.getRepeat();
    }

    void Pad_get_CameraTool() {
        this.btn0 = this.pad1P.getButton() | this.pad2P.getButton();
        this.edge0 = this.pad1P.getEdge() | this.pad2P.getEdge();
        this.rep0 = this.pad1P.getRepeat() | this.pad2P.getRepeat();
        this.btn1 = this.pad1P.getButton();
        this.edge1 = this.pad1P.getEdge();
        this.rep1 = this.pad1P.getRepeat();
        this.btn2 = this.pad2P.getButton();
        this.edge2 = this.pad2P.getEdge();
        this.rep2 = this.pad2P.getRepeat();
    }

    void Put_param() {
        Runtime.setRegister(0, this.printoutcount);
        System.println("*********printout /[$0]***********");
        this.setRegXYZ(this.ManualCam.getTranslateX(), this.ManualCam.getTranslateY(), this.ManualCam.getTranslateZ());
        System.println("ManualCam.setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.ManualCam.getRotateX(), this.ManualCam.getRotateY(), this.ManualCam.getRotateZ());
        System.println("ManualCam.setRotate(/[#0]f,/[#1]f,/[#2]f);");
        Runtime.setRegister(0, this.ManualCam.getFov());
        System.println("ManualCam.setFov(/[#0]f);");
        this.setRegXYZ(this.BaseCam.getTranslateX(), this.BaseCam.getTranslateY(), this.BaseCam.getTranslateZ());
        System.println("BaseCam.setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.BaseCam.getRotateX(), this.BaseCam.getRotateY(), this.BaseCam.getRotateZ());
        System.println("BaseCam.setRotate(/[#0]f,/[#1]f,/[#2]f);");
        Runtime.setRegister(0, this.BaseCam.getFov());
        System.println("BaseCam.setFov(/[#0]f);");
        ++this.printoutcount;
    }

    void STCameraPRO_on() {
        this.STCamera_off();
        this.axel_pos = 0.0f;
        this.axel_rot = 0.001f;
        this.transCfX = 0.0f;
        this.transCfY = 0.0f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.002f;
        this.rotCfY = 0.002f;
        this.rotCfZ = 0.001f;
        this.ShakeCycleArg = 8;
        this.ShakePowerArg = 0.002f;
        this.Vec2zeroFlag = true;
        this.vpx = 0.0f;
        this.vpy = 0.0f;
        this.vpz = 0.0f;
        this.vrx = 0.0f;
        this.vry = 0.0f;
        this.vrz = 0.0f;
        this.shakecamera = Thread.create(this, "ShakeRoutine_main");
        this.shakecamera.start();
        this.STCam.change();
    }

    void STCamera_init() {
        this.ManualCam = Camera.create(0);
        this.BaseCam = Camera.create(1);
        this.STCam = Camera.create(7);
    }

    void STCamera_init(int n, int n2) {
        this.ManualCam = Camera.create(0);
        this.BaseCam = Camera.create(n);
        this.STCam = Camera.create(n2);
    }

    void STCamera_off() {
        if (this.threadexec) {
            this.threadexec = false;
            this.shakecamera.stop();
        }
        if (!this.VIBexec) {
            this.BaseCam.change();
        }
    }

    void STCamera_off(Camera camera) {
        if (this.threadexec) {
            this.threadexec = false;
            this.shakecamera.stop();
            camera.change();
        }
    }

    void STCamera_on() {
        this.STCamera_off();
        this.axel_pos = 0.0f;
        this.axel_rot = 0.003f;
        this.transCfX = 0.0f;
        this.transCfY = 0.0f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.02f;
        this.rotCfY = 0.02f;
        this.rotCfZ = 0.001f;
        this.ShakeCycleArg = 8;
        this.ShakePowerArg = 0.005f;
        this.Vec2zeroFlag = true;
        this.vpx = 0.0f;
        this.vpy = 0.0f;
        this.vpz = 0.0f;
        this.vrx = 0.0f;
        this.vry = 0.0f;
        this.vrz = 0.0f;
        this.shakecamera = Thread.create(this, "ShakeRoutine_main");
        this.shakecamera.start();
        this.STCam.change();
    }

    void Shake(int n, float f) {
        ++this.ShakeCycle;
        if (this.ShakeCycle >= n) {
            this.ShakeCycle = 0;
            this.vrx += this.ShakeX() * f;
            this.vry += this.ShakeY() * f;
        }
    }

    void ShakeRoutine_main() {
        this.threadexec = true;
        this.workpx = this.BaseCam.getTranslateX() + this.transCfX;
        this.workpy = this.BaseCam.getTranslateY() + this.transCfY;
        this.workpz = this.BaseCam.getTranslateZ() + this.transCfZ;
        this.workrx = this.BaseCam.getRotateX() + this.rotCfX;
        this.workry = this.BaseCam.getRotateY() + this.rotCfY;
        this.workrz = this.BaseCam.getRotateZ() + this.rotCfZ;
        this.basehpx = this.BaseCam.getTranslateX();
        this.basehpy = this.BaseCam.getTranslateY();
        this.basehpz = this.BaseCam.getTranslateZ();
        this.basehrx = this.BaseCam.getRotateX();
        this.basehry = this.BaseCam.getRotateY();
        this.basehrz = this.BaseCam.getRotateZ();
        while (true) {
            this.vpx += this.GetAxel(this.BaseCam.getTranslateX(), this.workpx, this.axel_pos);
            this.vpy += this.GetAxel(this.BaseCam.getTranslateY(), this.workpy, this.axel_pos);
            this.vpz += this.GetAxel(this.BaseCam.getTranslateZ(), this.workpz, this.axel_pos);
            this.vrx += this.GetAxel(this.BaseCam.getRotateX(), this.workrx, this.axel_rot);
            this.vry += this.GetAxel(this.BaseCam.getRotateY(), this.workry, this.axel_rot);
            this.vrz += this.GetAxel(this.BaseCam.getRotateZ(), this.workrz, this.axel_rot);
            this.Shake(this.ShakeCycleArg, this.ShakePowerArg);
            this.Vec2zero(this.Vec2zeroFlag);
            this.basevpx = this.BaseCam.getTranslateX() - this.basehpx;
            this.basevpy = this.BaseCam.getTranslateY() - this.basehpy;
            this.basevpz = this.BaseCam.getTranslateZ() - this.basehpz;
            this.basevrx = this.BaseCam.getRotateX() - this.basehrx;
            this.basevry = this.BaseCam.getRotateY() - this.basehry;
            this.basevrz = this.BaseCam.getRotateZ() - this.basehrz;
            this.workpx += this.vpx + this.basevpx + this.VIB_transCfX;
            this.workpy += this.vpy + this.basevpy + this.VIB_transCfY;
            this.workpz += this.vpz + this.basevpz + this.VIB_transCfZ;
            this.STCam.setTranslate(this.workpx, this.workpy, this.workpz);
            this.workrx += this.vrx + this.basevrx + this.VIB_transCfRX;
            this.workry += this.vry + this.basevry + this.VIB_transCfRY;
            this.workrz += this.vrz + this.basevrz + this.VIB_transCfRZ;
            this.STCam.setRotate(this.workrx, this.workry, this.workrz);
            this.basehpx = this.BaseCam.getTranslateX();
            this.basehpy = this.BaseCam.getTranslateY();
            this.basehpz = this.BaseCam.getTranslateZ();
            this.basehrx = this.BaseCam.getRotateX();
            this.basehry = this.BaseCam.getRotateY();
            this.basehrz = this.BaseCam.getRotateZ();
            this.STCam.setFov(this.BaseCam.getFov());
            System.sleep(1);
        }
    }

    float ShakeX() {
        ++this.ShakeTableCountX;
        this.ShakeTableCountX %= this.ShakeTablemax;
        return this.ShakeTable[this.ShakeTableCountX];
    }

    float ShakeY() {
        ++this.ShakeTableCountY;
        this.ShakeTableCountY %= this.ShakeTablemax;
        return this.ShakeTable[this.ShakeTableCountY];
    }

    void Shock_on() {
        this.STCamera_off();
        this.axel_pos = 5.0E-5f;
        this.axel_rot = 1.0f;
        this.transCfX = 0.0f;
        this.transCfY = 0.001f;
        this.transCfZ = 0.0f;
        this.rotCfX = 0.02f;
        this.rotCfY = 0.02f;
        this.rotCfZ = 0.05f;
        this.ShakeCycleArg = 1;
        this.ShakePowerArg = 0.1f;
        this.Vec2zeroFlag = true;
        this.vpx = 0.0f;
        this.vpy = 0.0f;
        this.vpz = 0.0f;
        this.vrx = 0.0f;
        this.vry = 0.0f;
        this.vrz = 0.0f;
        this.shakecamera = Thread.create(this, "ShakeRoutine_main");
        this.shakecamera.start();
        this.STCam.change();
    }

    void SpeedShake() {
        this.speedshaketrans = this.Abs(this.basevpx) + this.Abs(this.basevpy) + this.Abs(this.basevpz);
        this.speedshaketrans *= 1000.0f;
        if (this.speedshaketrans < 1.0f) {
            this.speedshaketrans = 1.0f;
        }
        if (this.speedshaketrans > 10.0f) {
            this.speedshaketrans = 20.0f;
        }
    }

    void Systemsleep2(int n) {
        int n2 = 0;
        while (n2 <= n) {
            this.Pad_get_CameraTool();
            this.CamCopy(this.ManualCam, this.BaseCam);
            System.sleep(1);
            if ((this.edge2 & 0x40) != 0) {
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
        System.println("----------シーンは終了しました。");
        Runtime.setRegister(0, this.Timechk_srv_totaltime2);
        System.println("----------シーンは/[$0]フレームでした。");
    }

    void Timechk_SceneStart() {
        this.CutNo = 0;
        this.Timechk_NextCut = this.CutNo + 1;
        this.Timechk_srv_time = 0;
        this.Timechk_srv_totaltime = 0;
        this.Timechk_srv_totaltime2 = 0;
        System.println("----------シーンを開始します。");
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
            Runtime.setRegister(3, this.Timechk_srv_totaltime2);
            System.println("-----Cut = /[$1] CutTime = /[$2](Total = /[$0])(RealTotal = /[$3])");
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
            ++this.Timechk_srv_totaltime2;
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
        if ((this.edge1 & 0x400) != 0) {
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
        if ((this.edge1 & 0x400) != 0) {
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
        if ((this.edge1 & 0x400) != 0) {
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

    void VIBCamera_off() {
        this.VIB_transCfX = 0.0f;
        this.VIB_transCfY = 0.0f;
        this.VIB_transCfZ = 0.0f;
        this.VIB_transCfRX = 0.0f;
        this.VIB_transCfRY = 0.0f;
        this.VIB_transCfRZ = 0.0f;
        if (!this.threadexec) {
            this.BaseCam.change();
        }
    }

    void VIBCamera_on(float f) {
        this.VIB_transCfX = f;
        this.VIB_transCfY = f;
        this.VIB_transCfZ = f;
        if (!this.VIBexec) {
            this.vibcamera = Thread.create(this, "VibRoutine_main");
            this.vibcamera.start();
            this.STCam.change();
        }
    }

    void VIBCamera_on(float f, float f2, int n) {
        this.VIB_transCfX = f;
        this.VIB_transCfY = f;
        this.VIB_transCfZ = f;
        this.VIB_transCfRX = f2;
        this.VIB_transCfRY = f2;
        this.VIB_transCfRZ = 0.0f;
        this.VIB_time = n;
        if (!this.VIBexec) {
            this.vibcamera = Thread.create(this, "VibRoutine_main");
            this.vibcamera.start();
        }
        this.STCam.change();
    }

    void VIBCamera_on(float f, int n) {
        this.VIB_transCfX = f;
        this.VIB_transCfY = f;
        this.VIB_transCfZ = f;
        this.VIB_time = n;
        if (!this.VIBexec) {
            this.vibcamera = Thread.create(this, "VibRoutine_main");
            this.vibcamera.start();
        }
        this.STCam.change();
    }

    void Vec2zero(boolean bl) {
        if (bl) {
            ++this.VecTableCount;
            this.VecTableCount %= this.VecTablemax;
            this.vrx *= this.VecTable[this.VecTableCount];
            this.vry *= this.VecTable[this.VecTableCount];
        }
    }

    void VibRoutine_main() {
        this.VIBexec = true;
        while (true) {
            if (!this.threadexec) {
                this.STCam.setRotate(this.BaseCam.getRotateX(), this.BaseCam.getRotateY(), this.BaseCam.getRotateZ());
                this.workpx = this.BaseCam.getTranslateX() + this.VIB_transCfX;
                this.workpy = this.BaseCam.getTranslateY() + this.VIB_transCfY;
                this.workpz = this.BaseCam.getTranslateZ() + this.VIB_transCfZ;
                this.workrx = this.BaseCam.getRotateX() + this.VIB_transCfRX;
                this.workry = this.BaseCam.getRotateY() + this.VIB_transCfRY;
                this.workrz = this.BaseCam.getRotateZ() + this.VIB_transCfRZ;
                this.STCam.setTranslate(this.workpx, this.workpy, this.workpz);
                this.STCam.setRotate(this.workrx, this.workry, this.workrz);
                this.STCam.setFov(this.BaseCam.getFov());
            }
            this.VIB_transCfX *= -1.0f;
            this.VIB_transCfY *= -1.0f;
            this.VIB_transCfZ *= -1.0f;
            this.VIB_transCfRX *= -1.0f;
            this.VIB_transCfRY *= -1.0f;
            this.VIB_transCfRZ *= -1.0f;
            System.sleep(this.VIB_time);
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

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02002B_F");
        Runtime.setFlags(104, 1, 1);
        System.println("XEVEJNAME:CFJ2_20 XEVEJPOINT:POINT2_20");
        Runtime.jumpCF(820, 5);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(820, 5);
    }

    int floatDowner(float f) {
        float f2 = this.Abs(f * 100000.0f);
        return (int) f2;
    }

    int floatUpper(float f) {
        return (int) f;
    }

    int getSPLmode() {
        if (this.SPLDOWNmode) {
            if (this.SPLUPmode) {
                return 3;
            }
            return 1;
        }
        if (this.SPLUPmode) {
            return 0;
        }
        return 2;
    }

    void init() {
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam0.change();
        this.cam0.start(2, null);
        this.cam1.change();
        Runtime.setLocation(1101);
        Runtime.setLocation(1107);
        Runtime.setLocation(1100);
        Stage.setEffectRender(0);
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.ziggy = new Human();
        this.ziggy.init(0x1000006, 2.63f, 0.11f, -14.04f, 121.86f);
        this.ziggy.start(4, null);
        this.ziggy.face = this.ziggy.getChild(0x1000000);
        this.yuri = new Human();
        this.yuri.init(0x100011D, -2.84f, 0.0f, -2.86f, 47.0f);
        this.yuri.start(4, null);
        this.yuri.face = this.yuri.getChild(0x1000000);
        this.yuri.setVisible(false);
        this.dammy = new Human();
        this.dammy.init(24599, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dammy.start(4, null);
        this.dammy.setShadow(0, 0);
        this.dammy.setVisible(false);
        this.U1 = new U_TIC();
        this.U1.init(778, -2.5f, 0.0f, -5.51f, -147.49f);
        this.U1.start(4, null);
        this.U1.mtn(1, 8, 1.0f, true);
        this.U2 = new U_TIC();
        this.U2.init(778, -3.8f, 0.0f, -11.3f, -337.49f);
        this.U2.start(4, null);
        this.U2.mtn(1, 8, 1.0f, true);
        this.Urobo = new U_TIC();
        this.Urobo.init(17411, 1.06f, 0.0f, -22.43f, 0.0f);
        this.Urobo.setScale(1.0f, 1.0f, 1.0f);
        this.Urobo.start(4, null);
        this.Urobo2 = new U_TIC();
        this.Urobo2.init(17409, -4.24f, 0.0f, -22.56f, 0.0f);
        this.Urobo2.setScale(1.0f, 1.0f, 1.0f);
        this.Urobo2.start(4, null);
        this.gun = new Obj();
        this.gun.init(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.gun.start(4, null);
        this.gun.setTranslate(0.06f, -0.02f, 0.02f);
        this.gun.setRotate(0.0f, 3.0f, -5.0f);
        this.gun2 = new Obj();
        this.gun2.init(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.gun2.start(4, null);
        this.gun2.setTranslate(0.06f, -0.02f, 0.02f);
        this.gun2.setRotate(0.0f, 3.0f, -5.0f);
        this.gun2.setVisible(false);
        this.syoukaki = new Obj();
        this.syoukaki.init(24626, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syoukaki.start(4, null);
        this.syoukaki.setTranslate(0.09f, -0.06f, 0.02f);
        this.syoukaki.setRotate(87.0f, 2.0f, 82.0f);
        this.syoukaki2 = new Obj();
        this.syoukaki2.init(24626, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syoukaki2.start(4, null);
        this.syoukaki2.setTranslate(0.09f, -0.03f, -0.02f);
        this.syoukaki2.setRotate(88.0f, 2.0f, 97.0f);
        this.syoukaki3 = new Obj();
        this.syoukaki3.init(24626, 0.0f, 0.0f, 0.0f, 0.0f);
        this.syoukaki3.start(4, null);
        this.syoukaki3.setTranslate(-4.41f, 0.82f, -17.13f);
        this.syoukaki3.setRotate(-2.0f, -138.0f, 0.0f);
        this.syoukaki3.setVisible(false);
        this.c4 = new Obj();
        this.c4.init(24627, 0.0f, 0.0f, 0.0f, 0.0f);
        this.c4.start(4, null);
        this.c4.setTranslate(0.1f, 0.0f, 0.07f);
        this.c4.setRotate(6.0f, -190.99f, -183.0f);
        this.c4.setVisible(false);
        this.c42 = new Obj();
        this.c42.init(24627, 0.0f, 0.0f, 0.0f, 0.0f);
        this.c42.start(4, null);
        this.c42.setTranslate(5.481f, 0.616f, -13.844f);
        this.c42.setRotate(-0.19998f, -61.19484f, 0.40012f);
        this.c42.setScale(0.99f, 0.99f, 0.99f);
        this.c42.setVisible(false);
        this.c43 = new Obj();
        this.c43.init(24627, 0.0f, 0.0f, 0.0f, 0.0f);
        this.c43.start(4, null);
        this.c43.setTranslate(3.55896f, 0.742f, -17.57547f);
        this.c43.setRotate(-10.19965f, -63.39393f, -11.39968f);
        this.c43.setScale(0.99f, 0.99f, 0.99f);
        this.c43.setVisible(false);
        this.dummy1 = new Obj();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new Obj();
        this.dummy2.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy3 = new Obj();
        this.dummy3.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy4 = new Obj();
        this.dummy4.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this.dummy5 = new Obj();
        this.dummy5.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy5.setVisible(false);
        this.dummy6 = new Obj();
        this.dummy6.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy6.setVisible(false);
        this.dummy7 = new Obj();
        this.dummy7.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy7.setVisible(false);
        this.dummy1.setParent(this.ziggy, 63);
        this.dummy2.setParent(this.ziggy, 68);
        this.dummy3.setParent(this.ziggy, 54);
        this.dummy4.setParent(this.ziggy, 66);
        this.dummy5.setParent(this.ziggy, 72);
        this.dummy7.setParent(this.ziggy, 17);
        this.spark1_ef = new Effect(1432, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark2_ef = new Effect(1701, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark3_ef = new Effect(1432, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark4_ef = new Effect(1701, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark5_ef = new Effect(1432, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark7_ef = new Effect(1701, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark1_ef.setScale(0.21f, 0.21f, 0.21f);
        this.spark2_ef.setScale(0.22f, 0.22f, 0.22f);
        this.spark3_ef.setScale(0.22f, 0.22f, 0.22f);
        this.spark4_ef.setScale(0.15f, 0.15f, 0.15f);
        this.spark5_ef.setScale(0.242f, 0.242f, 0.242f);
        this.spark7_ef.setScale(0.22f, 0.22f, 0.22f);
        this.spark1_ef.disp(false);
        this.spark2_ef.disp(false);
        this.spark3_ef.disp(false);
        this.spark4_ef.disp(false);
        this.spark5_ef.disp(false);
        this.spark7_ef.disp(false);
        this.spark1_ef.setCaster(this.dummy1);
        this.spark2_ef.setCaster(this.dummy2);
        this.spark3_ef.setCaster(this.dummy3);
        this.spark4_ef.setCaster(this.dummy4);
        this.spark5_ef.setCaster(this.dummy5);
        this.spark7_ef.setCaster(this.dummy7);
        this.bar = new Mapunit();
        this.bar.init(25);
        this.bar.start(4, null);
        this.bar.setTranslate(0.0f, 0.0f, 0.0f);
        this.bar.setRotate(0.0f, 0.0f, 0.0f);
        this.eft = new Effect(1406, 9.97f, 3.48f, -20.48f, 0.0f);
        this.eft.setScale(1.0f, 1.0f, 1.0f);
        this.eft.disp(true);
        this.light01 = new Effect(1405, -19.0f, 4.3f, -4.3f, 0.0f);
        this.light01.disp(false);
        this.light02 = new Effect(1405, -11.8f, 4.3f, -4.3f, 0.0f);
        this.light02.disp(false);
        this.light03 = new Effect(1405, -3.0f, 4.3f, -4.3f, 0.0f);
        this.light03.disp(false);
        this.light04 = new Effect(1405, -19.0f, 4.3f, 0.5f, 0.0f);
        this.light04.disp(false);
        this.light05 = new Effect(1405, -11.8f, 4.3f, 0.5f, 0.0f);
        this.light05.disp(false);
        this.light06 = new Effect(1405, -3.0f, 4.3f, 0.5f, 0.0f);
        this.light06.disp(false);
        Runtime.setDefocusQuick(0, 0, 35880, 1);
        Runtime.setDefocusQuick(1, 0, 27688, 1);
        Runtime.setDefocusQuick(2, 0, 19496, 1);
        Runtime.setDefocusQuick(3, 0, 11304, 1);
        this.FO = new Effect(0);
        this.FO.args[0] = Integer.MIN_VALUE;
        this.FO.args[1] = 45;
        this.FO.args[2] = 0;
        this.FI = new Effect(0);
        this.FI.args[0] = Integer.MIN_VALUE;
        this.FI.args[1] = 30;
        this.FI.args[2] = 1;
        this.FI2 = new Effect(0);
        this.FI2.args[0] = Integer.MIN_VALUE;
        this.FI2.args[1] = 60;
        this.FI2.args[2] = 1;
    }

    int inputNum() {
        return this.inputNum(0);
    }

    int inputNum(int n) {
        int n2 = n;
        int n3 = 1;
        int n4 = 0;
        boolean bl = true;
        System.println("INPUT Number!!");
        this.putInt(n2, n3);
        while (bl) {
            this.Pad_get();
            if ((this.edge2 & 0x800) != 0) {
                bl = false;
            }
            if ((this.edge2 & 0x2000) != 0) {
                if (n3 != 1) {
                    n4 = n2 / (n3 /= 10);
                }
                this.putInt(n2, n3);
            }
            if ((this.edge2 & 0x8000) != 0) {
                if (n3 != 10000) {
                    n4 = n2 / (n3 *= 10);
                }
                this.putInt(n2, n3);
            }
            if ((this.edge2 & 0x1000) != 0) {
                n2 = n4 != 9 ? (n2 += n3) : (n2 += n3);
                n4 = n2 / n3 % 10;
                this.putInt(n2, n3);
            }
            if ((this.edge2 & 0x4000) != 0) {
                n2 = n4 != 0 ? (n2 -= n3) : (n2 -= n3);
                n4 = n2 / n3 % 10;
                this.putInt(n2, n3);
            }
            System.sleep(1);
        }
        Runtime.setRegister(0, n2);
        System.println("End.(RETURN = /[$0])");
        return n2;
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

    static void main() {
    }

    void msg_print(String string, String string2) {
        this.msg.print(string2);
    }

    void play() {
        this.CameraTool();
        this.Timechk();
        this.CameraPlay = 1;
        Sound.streamPlay(1290085, 48000);
        Runtime.setDefocusQuick(0, 1, 20880, 1);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.85f, 0.68f, 0.68f);
        this.light.setDirection2(1, 0.753f, 0.0f, -0.658f);
        this.light.setColor(2, 0.8f, 0.62f, 0.62f);
        this.light.setDirection2(2, 0.553f, 0.001f, -0.833f);
        this.light.setColor(3, 0.51f, 0.48f, 0.44f);
        this.light.setDirection2(3, 0.889f, -0.0f, 0.458f);
        Stage.setColor(1.43f, 1.43f, 1.43f);
        this.Urobo.start(1, "light_robo");
        this.Urobo2.start(1, "light_robo2");
        this.ziggy.face.mtn(287, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.ziggy.start(1, "ziggy_act1");
        System.sleep(60);
        System.sleep(59);
        this.ziggy.start(1, "ziggy_act2");
        System.sleep(1);
        this.CameraPlay = 2;
        System.sleep(1);
        Runtime.setDefocusQuick(1, 4, 122880, 80);
        System.sleep(20);
        System.sleep(89);
        this.ziggy.start(1, "ziggy_act3");
        System.sleep(1);
        this.CameraPlay = 102;
        Runtime.setDefocusQuick(1, 0, 122880, 80);
        System.sleep(44);
        this.ziggy.start(1, "ziggy_act4");
        System.sleep(1);
        this.CameraPlay = 3;
        Runtime.setDefocusQuick(0, 1, 22880, 2);
        Runtime.setDefocusQuick(1, 1, 14688, 2);
        System.sleep(39);
        this.CameraPlay = 103;
        this.c4.setVisible(true);
        System.sleep(30);
        this.c42.setVisible(true);
        this.c4.setVisible(false);
        System.sleep(21);
        this.c42.start(1, "c42_disa");
        this.waitclear(15);
        System.sleep(21);
        this.CameraPlay = 4;
        this.ziggy.start(1, "ziggy_act5");
        System.sleep(40);
        this.c4.setVisible(true);
        System.sleep(20);
        this.CameraPlay = 5;
        System.sleep(15);
        this.c43.setVisible(true);
        this.c4.setVisible(false);
        this.waitclear(30);
        this.c43.start(1, "c43_disa");
        System.sleep(29);
        this.ziggy.start(1, "ziggy_act7");
        System.sleep(1);
        this.CameraPlay = 6;
        Runtime.setDefocusQuick(0, 2, 13264, 2);
        Runtime.setDefocusQuick(1, 0, 14688, 2);
        this.U1.start(1, "U1_walk");
        System.sleep(30);
        System.sleep(45);
        this.U2.start(1, "U2_walk");
        System.sleep(15);
        System.sleep(29);
        this.ziggy.start(1, "ziggy_act8");
        this.U1.start(1, "U1_8");
        this.syoukaki.setTranslate(0.09f, -0.06f, 0.02f);
        this.syoukaki.setRotate(89.0f, 2.0f, 95.0f);
        System.sleep(1);
        this.CameraPlay = 7;
        Runtime.setDefocusQuick(1, 4, 122880, 64);
        System.sleep(30);
        this.FO.call(0);
        System.sleep(22);
        this.syoukaki.setVisible(false);
        this.syoukaki3.setVisible(true);
        this.syoukaki2.setParent(this.U1, 23);
        this.syoukaki2.setTranslate(0.29f, -0.78f, 0.14f);
        this.syoukaki2.setRotate(88.0f, 93.0f, 93.0f);
        System.sleep(8);
        System.sleep(15);
        Runtime.setLocation(1101);
        this.CameraPlay = 8;
        Runtime.setDefocusQuick(0, 2, 2264, 1);
        Runtime.setDefocusQuick(1, 0, 14688, 2);
        Runtime.setDefocusQuick(2, 4, 122880, 64);
        this.eft.disp(false);
        this.c42.setFilter(0);
        this.c43.setFilter(0);
        this.FI.call(0);
        this.light01.disp(true);
        this.light02.disp(true);
        this.light03.disp(true);
        this.light04.disp(true);
        this.light05.disp(true);
        this.light06.disp(true);
        this.syoukaki.setVisible(false);
        this.syoukaki2.setVisible(false);
        this.Urobo.setLightMode(0);
        this.Urobo.setVisible(false);
        this.Urobo2.setLightMode(0);
        this.Urobo2.setVisible(false);
        this.ziggy.start(1, "ziggy_act9");
        System.sleep(45);
        System.sleep(21);
        this.CameraPlay = 9;
        Runtime.setDefocusQuick(0, 1, 15880, 1);
        Runtime.setDefocusQuick(1, 1, 7688, 1);
        this.ziggy.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.ziggy.light.setColor(1, 0.49f, 0.32f, 0.32f);
        this.ziggy.light.setDirection2(1, -0.392f, 0.0f, -0.92f);
        this.ziggy.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.ziggy.light.setDirection2(2, 0.87f, 0.194f, -0.453f);
        this.ziggy.light.setColor(3, 0.57f, 0.25f, 0.21f);
        this.ziggy.light.setDirection2(3, -0.491f, -0.621f, -0.612f);
        this.ziggy.setTranslate(-37.36f, 0.0f, -1.37f);
        this.ziggy.setRotate(0.0f, 88.0f, 0.0f);
        this.U1.start(1, "U1_sniper");
        System.sleep(74);
        this.ziggy.start(1, "ziggy_act11");
        System.sleep(1);
        this.CameraPlay = 10;
        Runtime.setDefocusQuick(0, 0, 15880, 1);
        Runtime.setDefocusQuick(1, 0, 7688, 1);
        Runtime.setDefocusQuick(2, 0, 122880, 64);
        System.sleep(30);
        System.sleep(13);
        Runtime.mpeg2("2002B_1");
        this.CameraPlay = 12;
        Sound.streamPlay(1290087, 48000);
        Runtime.setDefocusQuick(0, 1, 15880, 1);
        Runtime.setDefocusQuick(1, 1, 7688, 1);
        this.ziggy.start(1, "ziggy_act13");
        this.ziggy.setVisible(true);
        this.U2.setVisible(false);
        this.gun.setVisible(false);
        this.waitclear(30);
        System.sleep(29);
        this.ziggy.start(1, "ziggy_act14");
        System.sleep(1);
        this.CameraPlay = 13;
        this.spark1_ef.disp(true);
        System.sleep(30);
        this.spark5_ef.disp(true);
        this.waitclear(30);
        this.spark3_ef.disp(true);
        this.dammy.start(1, "dammy_ghost_spark");
        this.spark2_ef.disp(true);
        this.spark7_ef.disp(true);
        System.sleep(27);
        this.dummy7.start(1, "___spark_scale_down2");
        this.dammy.start(1, "dammy_ghost");
        System.sleep(33);
        this.msg.print("Damn it.\nA malfunction.");
        this.ziggy.face.mtn(286, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(15);
        this.ziggy.face.mtn(287, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(12);
        this.spark4_ef.disp(true);
        this.ziggy.face.mtn(286, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(18);
        this.ziggy.face.mtn(287, 8, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(15);
        System.sleep(30);
        this.CameraPlay = 14;
        Runtime.setDefocusQuick(0, 0, 15880, 1);
        Runtime.setDefocusQuick(1, 0, 7688, 1);
        this.light01.disp(false);
        this.light02.disp(false);
        this.light03.disp(false);
        this.light04.disp(false);
        this.light05.disp(false);
        this.light06.disp(false);
        this.U1.setVisible(false);
        this.gun2.setVisible(false);
        this.syoukaki.setVisible(false);
        this.syoukaki2.setVisible(false);
        this.syoukaki3.setVisible(false);
        this.dammy.start(1, "dammy_ghost2");
        System.sleep(60);
        System.sleep(51);
        this.msg.print("...I see.");
        System.sleep(30);
        this.spark1_ef.disp(false);
        this.spark2_ef.disp(false);
        this.spark3_ef.disp(false);
        this.spark4_ef.disp(false);
        this.spark5_ef.disp(false);
        this.spark7_ef.disp(false);
        int[] nArray = new int[5];
        nArray[0] = 0x1000000;
        int[] nArray2 = nArray;
        Runtime.setDefocus(0, 10, nArray2);
        System.sleep(1);
        Runtime.setLocation(1107);
        this.CameraPlay = 201;
        Stage.setColor(1.0f, 0.88f, 0.88f);
        this.waitclear(15);
        this.yuri.start(1, "yuri_act201");
        this.ziggy.start(1, "ziggy_act201");
        this.bar.start(1, "doron");
        System.sleep(30);
        this.msg.print("So that 100-Series Realian\nis encoded with extensive \namounts of research data");
        this.waitclear(141);
        this.msg.print("left behind by the founder of U-TIC?");
        this.waitclear(117);
        System.sleep(15);
        this.msg.print("Yes.");
        this.yuri.face.mtn(286, 9, 0.7f, false);
        this.yuri.face.start(4, null);
        System.sleep(18);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(27);
        this.msg.print("It's data that could affect \nthe entire fate of mankind.");
        this.yuri.face.mtn(286, 9, 0.7f, false);
        this.yuri.face.start(4, null);
        System.sleep(75);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(12);
        System.sleep(30);
        this.msg.print("What is the founder's involvement?");
        this.waitclear(60);
        this.yuri.start(1, "yuri_act202");
        System.sleep(1);
        this.CameraPlay = 202;
        Runtime.setDefocusQuick(0, 1, 83880, 1);
        Runtime.setDefocusQuick(2, 1, 75688, 1);
        System.sleep(15);
        this.msg.print("The founder of U-TIC was the same man\nwho advocated the creation of that child:\nthe 100-Series Observational Unit.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(60);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(286, 9, 0.6f, false);
        this.yuri.face.start(4, null);
        System.sleep(18);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(6);
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(66);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(3);
        System.sleep(30);
        this.msg.print("Joachim Mizrahi...");
        this.yuri.face.mtn(286, 9, 0.9f, false);
        this.yuri.face.start(4, null);
        System.sleep(27);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(9);
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(18);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(15);
        System.sleep(15);
        this.msg.print("A madman who lost his humanity\nby immersing himself in science.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(114);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(15);
        System.sleep(20);
        this.yuri.start(1, "yuri_act203");
        System.sleep(1);
        this.CameraPlay = 203;
        Runtime.setDefocusQuick(0, 1, 17880, 1);
        Runtime.setDefocusQuick(2, 0, 75688, 1);
        System.sleep(69);
        this.msg.print("You look as if you have\nsomething to say...?");
        this.waitclear(60);
        System.sleep(20);
        this.yuri.start(1, "yuri_act204");
        System.sleep(1);
        this.CameraPlay = 204;
        Runtime.setDefocusQuick(0, 2, 56264, -2);
        System.sleep(60);
        this.msg.print("Yes, you guessed correctly.\nHe is my ex-husband.");
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(15);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(21);
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(30);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(12);
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(6);
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(3);
        System.sleep(21);
        this.CameraPlay = 205;
        Runtime.setDefocusQuick(0, 1, 39880, 2);
        this.msg.print("Do you want to know what it was\nlike being married to a murderer?");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(60);
        this.yuri.look_speed(0.0f);
        this.yuri.look_eye_speed(0.5f);
        this.yuri.look_char(this.ziggy);
        System.sleep(27);
        this.CameraPlay = 206;
        Runtime.setDefocusQuick(0, 1, 26880, -2);
        Runtime.setDefocusQuick(2, 1, 26688, -2);
        this.ziggy.start(1, "ziggy_act206");
        this.waitclear(39);
        System.sleep(30);
        this.msg.print("No.");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(15);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(30);
        System.sleep(15);
        this.CameraPlay = 207;
        Runtime.setDefocusQuick(0, 1, 37880, 1);
        Runtime.setDefocusQuick(2, 0, 18688, -2);
        this.yuri.start(1, "yuri_act207");
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(60);
        this.msg.print("In any case, it's certain that the Organization is frantically trying to\nget its hands on that data.");
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(27);
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(102);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(6);
        System.sleep(20);
        this.yuri.start(1, "yuri_act208");
        System.sleep(1);
        this.CameraPlay = 208;
        Runtime.setDefocusQuick(0, 1, 14880, 1);
        this.ziggy.start(1, "ziggy_act208");
        System.sleep(15);
        this.msg.print("I'm afraid we don't have\nmuch time to spare.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(72);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(18);
        System.sleep(21);
        this.msg.print("Understood.");
        this.waitclear(45);
        this.msg.print("I'll be leaving tomorrow\nat 0600 hours.");
        this.waitclear(96);
        System.sleep(21);
        this.msg.print("Our hopes rest on you.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(30);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(12);
        System.sleep(21);
        this.CameraPlay = 209;
        Runtime.setDefocusQuick(0, 1, 29880, -2);
        System.sleep(45);
        this.msg.print("There's...one thing I'd like to clarify.");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(21);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(15);
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(42);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(6);
        System.sleep(21);
        this.CameraPlay = 210;
        Runtime.setDefocusQuick(0, 1, 6880, -2);
        this.yuri.look_default();
        this.yuri.start(1, "yuri_act210");
        this.ziggy.start(1, "ziggy_act210");
        System.sleep(30);
        this.msg.print("Yes?");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(27);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(9);
        System.sleep(21);
        this.CameraPlay = 211;
        Runtime.setDefocusQuick(0, 1, 26880, -2);
        this.ziggy.setTranslate(-0.82f, -0.09f, -0.99f);
        this.msg.print("My instructions are to take the Realian\nto the Miltian star system...");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(69);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(27);
        this.msg.print("Yet she's registered with the\ngovernment as your daughter.");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(30);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(6);
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(24);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(3);
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(51);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(30);
        this.msg.print("Why is it that you don't want me \nto bring her back here?");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(45);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(15);
        this.CameraPlay = 212;
        Runtime.setDefocusQuick(0, 1, 43880, 1);
        Runtime.setDefocusQuick(1, 1, 35688, 1);
        this.yuri.start(1, "yuri_act212");
        this.waitclear(63);
        System.sleep(21);
        this.msg.print("We're currently carrying out an \noperation based in the area\n between Miltia and Michtam.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(156);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(15);
        System.sleep(24);
        this.msg.print("An operation vital to the human race.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(51);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(24);
        System.sleep(15);
        this.msg.print("That's why I'm sending her there.\nThat's all I can say right now.");
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(36);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(9);
        this.yuri.face.mtn(286, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(21);
        System.sleep(15);
        System.sleep(60);
        this.msg.print("Besides...");
        this.yuri.face.mtn(286, 9, 0.7f, false);
        this.yuri.face.start(4, null);
        System.sleep(30);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(18);
        this.CameraPlay = 213;
        Runtime.setDefocusQuick(0, 1, 49880, -2);
        System.sleep(48);
        this.yuri.look_speed(0.0f);
        this.yuri.look_eye_speed(0.3f);
        this.yuri.look_point(-2.43f, 0.33f, -2.92f);
        this.msg.print("This way,\nI won't have to see her either.");
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        System.sleep(24);
        this.yuri.face.mtn(286, 9, 0.8f, false);
        this.yuri.face.start(4, null);
        System.sleep(75);
        this.yuri.face.mtn(287, 9, 1.0f, false);
        this.yuri.face.start(4, null);
        this.waitclear(21);
        this.FO.call(0);
        System.sleep(45);
        Runtime.setLocation(1101);
        Stage.setColor(1.43f, 1.43f, 1.43f);
        this.CameraPlay = 16;
        this.FI2.call(0);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.ziggy.start(1, "ziggy_act16");
        System.sleep(90);
        this.msg.print("Juli Mizrahi...");
        this.ziggy.face.mtn(286, 9, 0.7f, false);
        this.ziggy.face.start(4, null);
        System.sleep(21);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(18);
        this.ziggy.face.mtn(286, 9, 0.7f, false);
        this.ziggy.face.start(4, null);
        System.sleep(27);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(33);
        this.msg.print("What a strange woman.");
        this.ziggy.face.mtn(286, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        System.sleep(30);
        this.ziggy.face.mtn(287, 9, 1.0f, false);
        this.ziggy.face.start(4, null);
        this.waitclear(21);
        System.sleep(21);
        this.Timechk_SceneEnd();
    }

    void putInt(int n) {
        Runtime.setRegister(0, n);
        System.println("/[$0]");
        this.msg.close();
        this.msg.print("/[$0]");
    }

    void putInt(int n, int n2) {
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        System.println("/[$0](Cursor = /[$1])");
        this.msg.close();
        this.msg.print("/[$0]");
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

    void waitCameraEnd(int n) {
        while (n != this.CameraEnd) {
            System.sleep(1);
        }
    }

    void waitCameraPlay(int n) {
        while (n != this.CameraPlay) {
            System.sleep(1);
        }
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Human
            extends Chr {
        Chr face;

        Human() {
        }

        void dammy_ghost() {
            float f = 0.0f;
            while (f < 1.0f) {
                SCE02002B.this.gnoFilter[0] = 0.0f + f;
                SCE02002B.this.gnoFilter[1] = 118.5f;
                SCE02002B.this.gnoFilter[2] = 25.5f;
                SCE02002B.this.gnoFilter[3] = 0.0f;
                SCE02002B.this.ziggy.setFilter(3);
                SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
                System.sleep(1);
                f += 0.005f;
            }
        }

        void dammy_ghost2() {
            SCE02002B.this.gnoFilter[0] = 1.0f;
            SCE02002B.this.gnoFilter[1] = 0.0f;
            SCE02002B.this.gnoFilter[2] = 0.0f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(0);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
        }

        void dammy_ghost_spark() {
            SCE02002B.this.gnoFilter[0] = 0.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(3);
            SCE02002B.this.gnoFilter[0] = 1.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(3);
            SCE02002B.this.gnoFilter[0] = 0.1f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(6);
            SCE02002B.this.gnoFilter[0] = 1.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(3);
            SCE02002B.this.gnoFilter[0] = 0.2f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(6);
            SCE02002B.this.gnoFilter[0] = 1.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(2);
            SCE02002B.this.gnoFilter[0] = 0.6f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(2);
            SCE02002B.this.gnoFilter[0] = 1.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(1);
            SCE02002B.this.gnoFilter[0] = 0.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            System.sleep(1);
        }

        void yuri_act201() {
            this.setVisible(true);
            this.setLightMode(1);
            SCE02002B.this.yuri.light.setColor(0, 0.27f, 0.23f, 0.21f);
            SCE02002B.this.yuri.light.setColor(1, 0.48f, 0.42f, 0.09f);
            SCE02002B.this.yuri.light.setDirection2(1, -1.0f, 0.0f, -0.024f);
            SCE02002B.this.yuri.light.setColor(2, 1.0f, 1.0f, 1.0f);
            SCE02002B.this.yuri.light.setDirection2(2, -0.894f, 0.144f, 0.425f);
            SCE02002B.this.yuri.light.setColor(3, 0.37f, 0.32f, 0.2f);
            SCE02002B.this.yuri.light.setDirection2(3, 0.644f, 0.001f, 0.765f);
            SCE02002B.this.yuri.setShadow(5, 32);
            this.mtn(276, 0, 0.7f, true);
        }

        void yuri_act202() {
            this.mtn(277, -2147483640, 0.8f, true);
        }

        void yuri_act203() {
            this.setTranslate(-2.91f, -0.02f, -2.91f);
            this.setRotate(0.0f, 47.0f, 0.0f);
            this.mtn(278, -2147483640, 1.0f, true);
        }

        void yuri_act204() {
            this.setTranslate(-2.84f, 0.0f, -2.86f);
            this.mtn(279, -2147483640, 1.0f, true);
        }

        void yuri_act207() {
            this.mtn(281, 8, 0.8f, true);
        }

        void yuri_act208() {
            this.mtn(283, 8, 0.9f, true);
        }

        void yuri_act210() {
            this.mtn(276, 8, 1.0f, true);
        }

        void yuri_act212() {
            this.mtn(285, 8, 0.9f, true);
        }

        void ziggy_act1() {
            SCE02002B.this.c4.setParent(SCE02002B.this.ziggy, 72);
            this.setTranslate(1.79f, 0.11f, -13.88f);
            this.setRotate(0.0f, 121.86f, 0.0f);
            SCE02002B.this.ziggy.setLightMode(1);
            SCE02002B.this.gnoFilter[0] = 0.0f;
            SCE02002B.this.gnoFilter[1] = 118.5f;
            SCE02002B.this.gnoFilter[2] = 25.5f;
            SCE02002B.this.gnoFilter[3] = 0.0f;
            SCE02002B.this.ziggy.setFilter(3);
            SCE02002B.this.ziggy.setFilterParam(SCE02002B.this.gnoFilter);
            SCE02002B.this.ziggy.light.setColor(0, 0.27f, 0.27f, 0.27f);
            SCE02002B.this.ziggy.light.setColor(1, 0.53f, 0.33f, 0.23f);
            SCE02002B.this.ziggy.light.setDirection2(1, -0.482f, 0.0f, -0.876f);
            SCE02002B.this.ziggy.light.setColor(2, 0.49f, 0.29f, 0.14f);
            SCE02002B.this.ziggy.light.setDirection2(2, -0.361f, 0.001f, -0.932f);
            SCE02002B.this.ziggy.light.setColor(3, 0.36f, 0.53f, 0.75f);
            SCE02002B.this.ziggy.light.setDirection2(3, 0.816f, -0.097f, 0.571f);
            SCE02002B.this.ziggy.setShadow(0, 0);
            this.mtn(257, 0, 1.0f, true);
        }

        void ziggy_act11() {
            this.setTranslate(-29.3f, 0.0f, 0.48f);
            this.setRotate(0.0f, 88.0f, 0.0f);
            this.mtn(270, 0, 1.0f, true);
        }

        void ziggy_act13() {
            this.setTranslate(-29.33f, 0.0f, 0.36f);
            this.setRotate(0.0f, 88.0f, 0.0f);
            this.mtn(272, 8, 1.0f, true);
        }

        void ziggy_act14() {
            SCE02002B.this.ziggy.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02002B.this.ziggy.light.setColor(1, 0.63f, 0.47f, 0.33f);
            SCE02002B.this.ziggy.light.setDirection2(1, -0.413f, 0.001f, -0.911f);
            SCE02002B.this.ziggy.light.setColor(2, 0.47f, 0.31f, 0.29f);
            SCE02002B.this.ziggy.light.setDirection2(2, -0.63f, 0.001f, -0.777f);
            SCE02002B.this.ziggy.light.setColor(3, 0.36f, 0.36f, 0.46f);
            SCE02002B.this.ziggy.light.setDirection2(3, 0.673f, -0.089f, 0.735f);
            this.setTranslate(-29.2f, 0.0f, 0.46f);
            this.setRotate(0.0f, 447.27f, 0.0f);
            this.mtn(273, 0, 1.0f, true);
        }

        void ziggy_act16() {
            this.setTranslate(-29.2f, 0.0f, 0.46f);
            this.setRotate(0.0f, 447.27f, 0.0f);
            SCE02002B.this.ziggy.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02002B.this.ziggy.light.setColor(1, 0.63f, 0.47f, 0.33f);
            SCE02002B.this.ziggy.light.setDirection2(1, -0.413f, 0.001f, -0.911f);
            SCE02002B.this.ziggy.light.setColor(2, 0.47f, 0.31f, 0.29f);
            SCE02002B.this.ziggy.light.setDirection2(2, -0.63f, 0.001f, -0.777f);
            SCE02002B.this.ziggy.light.setColor(3, 0.36f, 0.36f, 0.46f);
            SCE02002B.this.ziggy.light.setDirection2(3, 0.673f, -0.089f, 0.735f);
            this.mtn(274, 0, 0.9f, true);
        }

        void ziggy_act2() {
            this.setTranslate(3.67f, 0.11f, -14.54f);
            this.setRotate(0.0f, 120.86f, 0.0f);
            this.mtn(260, 8, 1.0f, true);
        }

        void ziggy_act201() {
            this.setTranslate(-0.82f, -0.09f, -0.99f);
            this.setRotate(0.0f, 236.01f, 0.0f);
            SCE02002B.this.ziggy.light.setColor(0, 0.3f, 0.26f, 0.24f);
            SCE02002B.this.ziggy.light.setColor(1, 0.78f, 0.68f, 0.43f);
            SCE02002B.this.ziggy.light.setDirection2(1, -1.0f, 0.0f, -0.024f);
            SCE02002B.this.ziggy.light.setColor(2, 0.82f, 0.78f, 0.47f);
            SCE02002B.this.ziggy.light.setDirection2(2, -0.889f, 0.0f, 0.458f);
            SCE02002B.this.ziggy.light.setColor(3, 0.47f, 0.39f, 0.27f);
            SCE02002B.this.ziggy.light.setDirection2(3, 0.644f, 0.001f, 0.765f);
            SCE02002B.this.ziggy.setShadow(5, 32);
            this.mtn(275, 8, 1.0f, true);
        }

        void ziggy_act206() {
            this.mtn(280, 8, 1.0f, true);
        }

        void ziggy_act208() {
            this.setRotate(0.0f, 252.01f, 0.0f);
            this.mtn(282, 0, 0.95f, true);
        }

        void ziggy_act210() {
            this.setRotate(0.0f, 239.01f, 0.0f);
            this.mtn(284, 8, 1.0f, true);
        }

        void ziggy_act3() {
            this.setTranslate(3.18f, 0.11f, -15.26f);
            this.setRotate(0.0f, 57.9f, 0.0f);
            this.mtn(261, 8, 1.0f, true);
        }

        void ziggy_act4() {
            this.setTranslate(5.0f, -0.02f, -14.45f);
            this.setRotate(0.0f, -684.78f, 0.0f);
            this.mtn(262, 8, 1.0f, true);
        }

        void ziggy_act5() {
            SCE02002B.this.c4.setParent(SCE02002B.this.ziggy, 60);
            SCE02002B.this.c4.setTranslate(0.109f, 0.0f, -0.048f);
            SCE02002B.this.c4.setRotate(6.0f, -9.98743f, -174.896f);
            this.setTranslate(4.39f, -0.02f, -15.45f);
            this.setRotate(0.0f, -516.065f, 0.0f);
            this.mtn(263, 8, 1.0f, true);
        }

        void ziggy_act7() {
            this.setTranslate(3.72f, 0.02f, -16.61f);
            this.setRotate(0.0f, 368.01f, 0.0f);
            this.mtn(264, 8, 1.0f, true);
        }

        void ziggy_act8() {
            this.setTranslate(10.61f, 1.14f, -17.58f);
            this.setRotate(0.0f, 91.78f, 0.0f);
            this.mtn(269, 8, 1.0f, true);
        }

        void ziggy_act9() {
            this.setTranslate(-38.36f, 0.0f, -1.57f);
            this.setRotate(0.0f, 88.0f, 0.0f);
            this.mtn(269, 8, 1.0f, true);
        }
    }

    class U_TIC
            extends Chr {
        U_TIC() {
        }

        void U1_8() {
            this.setTranslate(-4.5f, 0.0f, -16.7f);
            this.setRotate(0.0f, -147.2f, 0.0f);
            this.mtn(268, 8, 1.0f, true);
        }

        void U1_sniper() {
            SCE02002B.this.U1.setLightMode(1);
            SCE02002B.this.U1.light.setColor(0, 0.26f, 0.26f, 0.26f);
            SCE02002B.this.U1.light.setColor(1, 0.76f, 0.62f, 0.64f);
            SCE02002B.this.U1.light.setDirection2(1, 0.192f, -0.732f, 0.653f);
            SCE02002B.this.U1.light.setColor(2, 0.56f, 0.47f, 0.46f);
            SCE02002B.this.U1.light.setDirection2(2, 0.405f, -0.465f, 0.787f);
            SCE02002B.this.U1.light.setColor(3, 0.68f, 0.68f, 0.82f);
            SCE02002B.this.U1.light.setDirection2(3, -0.064f, 0.56f, -0.826f);
            this.setTranslate(-12.54f, 5.99f, -6.24f);
            this.setRotate(0.0f, 338.0f, 0.0f);
            SCE02002B.this.gun2.setVisible(true);
            SCE02002B.this.gun2.setParent(SCE02002B.this.U1, 72);
            this.mtn(271, 8, 1.0f, true);
        }

        void U1_walk() {
            SCE02002B.this.syoukaki.setParent(SCE02002B.this.U1, 72);
            SCE02002B.this.syoukaki2.setParent(SCE02002B.this.U1, 60);
            SCE02002B.this.U1.setShadow(5, 16);
            this.mtn(265, 0, 0.9f, false);
            this.move(180, -5.2f, -9.7f, true);
        }

        void U2_keikai() {
            SCE02002B.this.gun.setParent(SCE02002B.this.U2, 72);
            this.setTranslate(-22.1f, 0.0f, 0.3f);
            this.setRotate(0.0f, -277.53f, 0.0f);
            this.mtn(266, 8, 1.0f, true);
        }

        void U2_walk() {
            SCE02002B.this.gun.setParent(SCE02002B.this.U2, 72);
            SCE02002B.this.U2.setShadow(5, 16);
            this.mtn(266, 0, 1.0f, true);
        }

        void light_robo() {
            SCE02002B.this.Urobo.setLightMode(1);
            SCE02002B.this.Urobo.mtn(258, 0, 1.0f, true);
            SCE02002B.this.Urobo.light.setColor(0, 0.3f, 0.3f, 0.3f);
            SCE02002B.this.Urobo.light.setColor(1, 1.0f, 0.61f, 0.57f);
            SCE02002B.this.Urobo.light.setDirection2(1, 0.752f, -0.651f, 0.102f);
            SCE02002B.this.Urobo.light.setColor(2, 0.44f, 0.28f, 0.31f);
            SCE02002B.this.Urobo.light.setDirection2(2, 0.86f, -0.277f, 0.428f);
            SCE02002B.this.Urobo.light.setColor(3, 0.3f, 0.16f, 0.38f);
            SCE02002B.this.Urobo.light.setDirection2(3, -0.428f, -0.883f, -0.192f);
        }

        void light_robo2() {
            SCE02002B.this.Urobo2.setLightMode(1);
            SCE02002B.this.Urobo2.mtn(259, 0, 1.0f, true);
            SCE02002B.this.Urobo2.light.setColor(0, 0.3f, 0.3f, 0.3f);
            SCE02002B.this.Urobo2.light.setColor(1, 1.0f, 0.61f, 0.57f);
            SCE02002B.this.Urobo2.light.setDirection2(1, 0.752f, -0.651f, 0.102f);
            SCE02002B.this.Urobo2.light.setColor(2, 0.44f, 0.28f, 0.31f);
            SCE02002B.this.Urobo2.light.setDirection2(2, 0.86f, -0.277f, 0.428f);
            SCE02002B.this.Urobo2.light.setColor(3, 0.3f, 0.16f, 0.38f);
            SCE02002B.this.Urobo2.light.setDirection2(3, -0.428f, -0.883f, -0.192f);
        }
    }

    class Mapunit
            extends MAPUnit {
        Mapunit() {
        }

        void doron() {
            this.setVisible(false);
        }
    }

    class Obj
            extends Unit {
        Spline move1SPL = Spline.create();

        Obj() {
        }

        void ___spark_scale_down2() {
            float f = 0.15f;
            float f2 = 0.25f;
            float f3 = 0.2f;
            int n = 0;
            while (n < 122) {
                SCE02002B.this.spark1_ef.setScale(f3, f3, f3);
                SCE02002B.this.spark2_ef.setScale(f3, f3, f3);
                SCE02002B.this.spark3_ef.setScale(f2, f2, f2);
                SCE02002B.this.spark5_ef.setScale(f, f, f);
                SCE02002B.this.spark7_ef.setScale(f, f, f);
                f -= 9.836066E-4f;
                f2 -= 8.1967213E-4f;
                f3 -= 9.0163935E-4f;
                ++n;
                System.sleep(1);
            }
        }

        void arrival() {
            float[] fArray = new float[8];
            fArray[1] = -17.34f;
            fArray[3] = -34.8f;
            fArray[4] = 120.0f;
            fArray[5] = -17.34f;
            fArray[6] = 0.08f;
            fArray[7] = -5.5f;
            float[] fArray2 = fArray;
            this.move1SPL.setCtrlVertex(fArray2, 0, 17, 120);
            this.setRotateY(0.1f);
            this.rotY(360, 0.0f, false);
            this.move(this.move1SPL, true);
        }

        void c42_disa() {
            float f = 0.0f;
            while (f < 1.0f) {
                SCE02002B.this.gnoFilter[0] = 1.0f - f;
                SCE02002B.this.gnoFilter[1] = 118.5f;
                SCE02002B.this.gnoFilter[2] = 5.5f;
                SCE02002B.this.gnoFilter[3] = 0.0f;
                SCE02002B.this.c42.setFilter(3);
                SCE02002B.this.c42.setFilterParam(SCE02002B.this.gnoFilter);
                System.sleep(1);
                f += 0.03f;
            }
        }

        void c43_disa() {
            float f = 0.0f;
            while (f < 1.0f) {
                SCE02002B.this.gnoFilter[0] = 1.0f - f;
                SCE02002B.this.gnoFilter[1] = 118.5f;
                SCE02002B.this.gnoFilter[2] = 5.5f;
                SCE02002B.this.gnoFilter[3] = 0.0f;
                SCE02002B.this.c43.setFilter(3);
                SCE02002B.this.c43.setFilterParam(SCE02002B.this.gnoFilter);
                System.sleep(1);
                f += 0.03f;
            }
        }

        void drive() {
            this.setTranslate(0.99f, 0.0f, -7.24f);
            this.setRotate(0.0f, 210.0f, 0.0f);
            float[] fArray = new float[8];
            fArray[1] = 0.99f;
            fArray[3] = -7.24f;
            fArray[4] = 150.0f;
            fArray[5] = -3.81f;
            fArray[7] = -17.74f;
            float[] fArray2 = fArray;
            this.move1SPL.setCtrlVertex(fArray2, 1, 18, 150);
            this.move(this.move1SPL, true);
        }

        void landing() {
            float[] fArray = new float[8];
            fArray[1] = -17.34f;
            fArray[2] = 0.08f;
            fArray[3] = -5.5f;
            fArray[4] = 120.0f;
            fArray[5] = -17.34f;
            fArray[6] = -1.78f;
            fArray[7] = -5.5f;
            float[] fArray2 = fArray;
            this.move1SPL.setCtrlVertex(fArray2, 0, 17, 120);
            this.setRotateY(0.1f);
            this.rotY(360, 0.0f, false);
            this.move(this.move1SPL, true);
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

