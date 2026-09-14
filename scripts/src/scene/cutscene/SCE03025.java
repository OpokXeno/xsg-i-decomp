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
import xeno.map.MC_KAS06_PRJ;
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

class SCE03025
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03025,
        MC_KAS06_PRJ,
        JNT_Human,
        FLSshion,
        FLSshion_h,
        FLSfeb,
        FLSallen,
        FLSmomo,
        FLSziggy,
        FLSchaos,
        FLSjr,
        FLSelly {
    allCHARA shion;
    allCHARA shionM;
    allCHARA feb;
    allCHARA allen;
    allCHARA momo;
    allCHARA ziggy;
    allCHARA chaos;
    allCHARA jr;
    allCHARA elly;
    allCHARA utre1;
    allCHARA utre2;
    allCHARA utre3;
    allCHARA utre4;
    allCHARA utre5;
    allCHARA dummy;
    PlayControl pc;
    Unit stained;
    Unit lighting;
    Unit fog;
    Unit target;
    Unit point0;
    Unit point1;
    Unit point2;
    static final float stR = 7.7f;
    static final float stL = -7.7f;
    static final float stY = 5.3f;
    static final float stZ1 = -4.49f;
    static final float stZ2 = -0.49f;
    static final float stZ3 = 3.52f;
    static final float stZ4 = 7.52f;
    Chr Fshion;
    Chr FshionM;
    Chr Ffeb;
    Chr Fallen;
    Chr Fmomo;
    Chr Fziggy;
    Chr Fchaos;
    Chr Fjr;
    Chr Felly;
    Chr Fshion1;
    Chr Ffeb1;
    Chr Fallen1;
    Chr Fmomo1;
    Chr Fziggy1;
    Chr Fchaos1;
    Chr Fjr1;
    Chr Felly1;
    MAPUnit door;
    static final int toUtic = 0;
    static final int toDuran = 1;
    Spline movSPL;
    Spline rotSPL;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camerawork camerawork = new Camerawork();
    Light light = new Light(0);
    Light light1 = new Light(1);
    int menuSelected;
    int msg_count = 1;
    Window win0;
    Window win1;
    Window win2;
    static final int Chand_R = 20;
    int Twohand = 0;
    int Rhand = 16;
    int Lhand = 32;
    int Open = 0;
    int Close = 1;
    float next_px;
    float next_py;
    float next_pz;
    Effect flash;
    Effect Rstained1;
    Effect Rstained2;
    Effect Rstained3;
    Effect Lstained1;
    Effect Lstained2;
    Effect Lstained3;
    Effect candle;
    Effect F_light1;
    Effect F_light2;
    Effect R_light1;
    Effect R_light2;
    Effect L_light1;
    Effect L_light2;
    int[] param1;
    int[] param2;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    Thread thread_cameratool;
    Window msg;
    Input pad1P;
    Input pad2P;
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
    int printoutcount;
    int selecttime;
    boolean selectrelease;
    int canceltime;
    boolean cancelrelease;
    int starttime;
    boolean startrelease;
    int marutime;
    boolean marurelease;
    int sikakutime;
    boolean sikakurelease;
    int sankakutime;
    boolean sankakurelease;
    int L3time;
    boolean L3release;
    int work0;
    int work1;
    float[] CamHistory;
    float[] CamHispos;
    float[] CamHisrot;
    float[] CamHisfov;
    boolean[] CamHistory_exist;
    int[] CamHistory_linktable;
    int CamHistory_max;
    int CamHistory_ptr;
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
    int HistoryLook_ptr;
    boolean SPLUPmode;
    boolean SPLDOWNmode;
    boolean manualcamera;
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
    int ReplayTime;
    boolean Facechk_exec;
    int Facechk_faceno;
    int Facechk_facemode;
    int Facechk_frame;
    int Facechk_mtnno;
    boolean Gnochk_exec;
    float[] gnoFilter;
    int gnoptr;
    float gno_alpha;
    float gno_filter;
    float gno_shake;
    float gno_ref;
    boolean FogSet_exec;
    int Fog_R;
    int Fog_G;
    int Fog_B;
    int Fog_A;
    float Fog_Z;
    float Fog_Zwide;
    float Fog_near;
    float Fog_far;
    float Fog_nearRatio;
    float Fog_farRatio;
    boolean ColorScreenSet_exec;
    int Color_R;
    int Color_G;
    int Color_B;
    int Color_A;
    int Color_Z;
    int Color_Zwide;
    int Color_layer;
    int Color_work;
    int Color_work2;
    int Color_mode;
    int[] ColorScreenParam;
    int[] ColorScreenParamBack;
    int[] ColorScreenParamZ;
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
    int msgsignal;
    int cutwait;
    boolean msgflag;
    Thread msg_clear;
    int msg_clearwait;
    boolean msg_clear_exec;
    Thread timer_thread;
    Thread timer_thread_srv;
    int CutNo;
    boolean Timechk_srv_exec;
    int Timechk_srv_time;
    int Timechk_srv_totaltime;
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

    SCE03025() {
        int[] nArray = new int[6];
        nArray[0] = 0x100000;
        nArray[1] = 0x40555555;
        nArray[4] = 512;
        nArray[5] = 448;
        this.param1 = nArray;
        int[] nArray2 = new int[6];
        nArray2[0] = 0x100000;
        nArray2[1] = -2130706433;
        nArray2[4] = 512;
        nArray2[5] = 448;
        this.param2 = nArray2;
        this.msg = Window.create(1);
        this.pad1P = Input.create(0);
        this.pad2P = Input.create(1);
        this.printoutcount = 0;
        this.selectrelease = false;
        this.cancelrelease = false;
        this.startrelease = false;
        this.marurelease = false;
        this.sikakurelease = false;
        this.sankakurelease = false;
        this.L3release = false;
        this.CamHistory = new float[70];
        this.CamHispos = new float[40];
        this.CamHisrot = new float[40];
        this.CamHisfov = new float[20];
        this.CamHistory_exist = new boolean[16];
        this.CamHistory_linktable = new int[]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        this.CamHistory_max = 10;
        this.CamHistory_ptr = 0;
        this.HistoryLook_ptr = 0;
        this.ManualCamRecord = new float[140];
        this.ManualCampos = new float[84];
        this.ManualCamrot = new float[84];
        this.ManualCamfov = new float[21];
        this.Manualcamrecord_dt = 15;
        this.Manualcamrecord_max = 20;
        this.Manualcamrecord_exec = false;
        this.Manualcamrecord_time = 0;
        this.ReplayTime = 100;
        this.Facechk_exec = true;
        this.Facechk_faceno = 1;
        this.Facechk_facemode = 1;
        this.Facechk_frame = 1;
        this.gnoFilter = new float[4];
        this.gnoptr = 0;
        this.gno_alpha = 0.54f;
        this.gno_filter = 87.5f;
        this.gno_shake = 37.0f;
        this.gno_ref = 0.72f;
        this.Fog_R = 128;
        this.Fog_G = 128;
        this.Fog_B = 128;
        this.Fog_A = 64;
        this.Fog_Z = 5.0f;
        this.Fog_Zwide = 4.0f;
        this.Fog_nearRatio = 0.0f;
        this.Fog_farRatio = 0.5f;
        this.Color_R = 0;
        this.Color_G = 0;
        this.Color_B = 0;
        this.Color_A = 96;
        this.Color_Z = 147456;
        this.Color_Zwide = 32768;
        this.Color_layer = 1;
        this.Color_work = 0;
        this.Color_work2 = 0;
        this.Color_mode = 1;
        int[] nArray3 = new int[8];
        nArray3[1] = 1;
        nArray3[2] = 0x100000;
        nArray3[3] = 1614815232;
        this.ColorScreenParam = nArray3;
        int[] nArray4 = new int[6];
        nArray4[0] = 1;
        nArray4[1] = 0x40808080;
        this.ColorScreenParamBack = nArray4;
        this.ColorScreenParamZ = new int[2];
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
        this.msgsignal = 128;
        this.cutwait = 0;
        this.msgflag = true;
        this.msg_clear = Thread.create(this, "msg_clear_thread");
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

    void ASmove(Chr chr, int n, float f, float f2, float f3, float f4) {
        chr.getTranslate();
        chr.getRotate();
        f = (f - chr.px) / (float) n;
        f2 = (f2 - chr.py) / (float) n;
        f3 = (f3 - chr.pz) / (float) n;
        f4 = (f4 - chr.ry) / (float) n;
        int n2 = 0;
        while (n2 < n) {
            chr.px += f;
            chr.py += f2;
            chr.pz += f3;
            chr.ry += f4;
            chr.setTranslate();
            chr.setRotate();
            System.sleep(1);
            ++n2;
        }
    }

    void ASmove(Chr chr, int n, float f, float f2, float f3, float f4, float f5, float f6) {
        chr.getTranslate();
        chr.getRotate();
        f = (f - chr.px) / (float) n;
        f2 = (f2 - chr.py) / (float) n;
        f3 = (f3 - chr.pz) / (float) n;
        f4 = (f4 - chr.rx) / (float) n;
        f5 = (f5 - chr.ry) / (float) n;
        f6 = (f6 - chr.rz) / (float) n;
        int n2 = 0;
        while (n2 < n) {
            chr.px += f;
            chr.py += f2;
            chr.pz += f3;
            chr.rx += f4;
            chr.ry += f5;
            chr.rz += f6;
            chr.setTranslate();
            chr.setRotate();
            System.sleep(1);
            ++n2;
        }
    }

    void ASmove(Unit unit, int n, float f, float f2, float f3, float f4) {
        unit.getTranslate();
        unit.getRotate();
        f = (f - unit.px) / (float) n;
        f2 = (f2 - unit.py) / (float) n;
        f3 = (f3 - unit.pz) / (float) n;
        f4 = (f4 - unit.ry) / (float) n;
        int n2 = 0;
        while (n2 < n) {
            unit.px += f;
            unit.py += f2;
            unit.pz += f3;
            unit.ry += f4;
            unit.setTranslate();
            unit.setRotate();
            System.sleep(1);
            ++n2;
        }
    }

    void ASmove(Unit unit, int n, float f, float f2, float f3, float f4, float f5, float f6) {
        unit.getTranslate();
        unit.getRotate();
        f = (f - unit.px) / (float) n;
        f2 = (f2 - unit.py) / (float) n;
        f3 = (f3 - unit.pz) / (float) n;
        f4 = (f4 - unit.rx) / (float) n;
        f5 = (f5 - unit.ry) / (float) n;
        f6 = (f6 - unit.rz) / (float) n;
        int n2 = 0;
        while (n2 < n) {
            unit.px += f;
            unit.py += f2;
            unit.pz += f3;
            unit.rx += f4;
            unit.ry += f5;
            unit.rz += f6;
            unit.setTranslate();
            unit.setRotate();
            System.sleep(1);
            ++n2;
        }
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
            if ((this.btn2 & 0x200) != 0 && this.L3time == 30) {
                this.FogSet();
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
                        this.menu.addItem("ｍｏｖｉｅ０３（たけなか）");
                        this.menu.addItem("ｍｏｖｉｅ０４（なかむら）");
                        this.menu.addItem("ｍｏｖｉｅ０５（いけうち）");
                        this.menu.addItem("ｍｏｖｉｅ０６（？？？？）");
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
            if ((this.edge2 & 0x400) != 0) {
                if (this.Color_mode == 1) {
                    System.println("Before.");
                    this.Color_mode = 3;
                } else {
                    System.println("Back.");
                    this.Color_mode = 1;
                }
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
            this.ColorScreenParamBack[1] = this.ColorScreenParam[3];
            this.Color_work = 15;
            this.Color_work2 = 0;
            this.ColorScreenParamZ[1] = this.Color_Z;
            Runtime.setDefocus(12, 0, null);
            Runtime.setDefocus(13, 0, null);
            Runtime.setDefocus(14, 0, null);
            Runtime.setDefocus(15, 0, null);
            while (this.Color_work2 != this.Color_layer) {
                if (this.Color_mode == 1) {
                    Runtime.setDefocus(this.Color_work, this.Color_mode, this.ColorScreenParam);
                }
                if (this.Color_mode == 3) {
                    Runtime.setDefocus(0, 2, this.ColorScreenParamZ);
                    Runtime.setDefocus(this.Color_work, this.Color_mode, this.ColorScreenParamBack);
                }
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
        Runtime.setRegister(2, this.Color_mode);
        Runtime.setRegister(3, this.Color_layer);
        if (this.Color_layer != 0) {
            System.println("*************ColorScreen Printout*************");
            if (this.Color_mode == 3) {
                System.println("ColorScreenParamZ = {");
                System.println("0,");
                System.println("/[$1],");
                System.println("};");
                System.println("Runtime.setDefocus(0,2,ColorScreenParamZ);");
                System.println("ColorScreenParamBack = {");
                System.println("1,");
                System.println("/[$0],");
                System.println("0,");
                System.println("0,");
                System.println("0,");
                System.println("0,");
                System.println("};");
                System.println("Runtime.setDefocus(15, /[$2], ColorScreenParamBack);");
                System.println("Layer = /[$3]");
            } else {
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
                System.println("Runtime.setDefocus(15, /[$2], ColorScreenParam);");
                this.Color_work = 15;
                this.Color_work2 = 1;
                while (this.Color_work2 != this.Color_layer) {
                    this.ColorScreenParam[2] = this.ColorScreenParam[2] - this.Color_Zwide;
                    Runtime.setRegister(1, this.ColorScreenParam[2]);
                    Runtime.setRegister(2, this.Color_work);
                    Runtime.setRegister(3, this.Color_mode);
                    System.println("ColorScreenParam[2] = /[$1]");
                    System.println("Runtime.setDefocus(/[$2], /[$3], ColorScreenParam);");
                    --this.Color_work;
                    ++this.Color_work2;
                }
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

    void FACE(int n, Chr chr, int n2) {
        chr.mtn(n2, 8, 1.0f, false);
        chr.start(4, null);
        System.sleep(n);
        chr.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
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
            if ((this.edge1 & 0x200) != 0) {
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

    void FogSet() {
        this.FogSet_exec = true;
        System.println("Fog  SetMode");
        while (this.FogSet_exec) {
            this.Pad_get();
            if ((this.edge2 & 0x100) != 0) {
                this.FogSet_printout();
            }
            if ((this.edge2 & 0x800) != 0) {
                this.FogSet_exec = false;
            }
            if ((this.btn2 & 8) != 0) {
                this.Fog_farRatio += 0.05f;
                if (this.Fog_farRatio > 1.0f) {
                    System.println("far Ratio Max!!");
                    this.Fog_farRatio = 1.0f;
                }
            }
            if ((this.btn2 & 2) != 0) {
                this.Fog_farRatio -= 0.05f;
                if (this.Fog_farRatio < 0.0f) {
                    System.println("far Ratio Min!!");
                    this.Fog_farRatio = 0.0f;
                }
            }
            if ((this.btn2 & 4) != 0) {
                this.Fog_nearRatio += 0.05f;
                if (this.Fog_nearRatio > 1.0f) {
                    System.println("Near Ratio Max!!");
                    this.Fog_nearRatio = 1.0f;
                }
            }
            if ((this.btn2 & 1) != 0) {
                this.Fog_nearRatio -= 0.05f;
                if (this.Fog_nearRatio < 0.0f) {
                    System.println("Near Ratio Min!!");
                    this.Fog_nearRatio = 0.0f;
                }
            }
            if ((this.btn2 & 0xF0) == 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    this.Fog_Z += 0.1f;
                }
                if ((this.btn2 & 0x4000) != 0) {
                    this.Fog_Z -= 0.1f;
                }
                if ((this.btn2 & 0x2000) != 0) {
                    this.Fog_Zwide -= 0.1f;
                }
                if ((this.btn2 & 0x8000) != 0) {
                    this.Fog_Zwide += 0.1f;
                }
            }
            if ((this.btn2 & 0x80) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Fog_R < 128) {
                        ++this.Fog_R;
                    } else {
                        System.println("Red Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Fog_R != 0) {
                        --this.Fog_R;
                    } else {
                        System.println("Red Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x10) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Fog_G < 128) {
                        ++this.Fog_G;
                    } else {
                        System.println("Green Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Fog_G != 0) {
                        this.Fog_G -= 2;
                    } else {
                        System.println("Green Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x20) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Fog_B < 128) {
                        this.Fog_B += 2;
                    } else {
                        System.println("Blue Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Fog_B != 0) {
                        this.Fog_B -= 2;
                    } else {
                        System.println("Blue Min!!");
                    }
                }
            }
            if ((this.btn2 & 0x40) != 0) {
                if ((this.btn2 & 0x1000) != 0) {
                    if (this.Fog_A < 128) {
                        ++this.Fog_A;
                    } else {
                        System.println("Alpha Max!!");
                    }
                }
                if ((this.btn2 & 0x4000) != 0) {
                    if (this.Fog_A != 0) {
                        --this.Fog_A;
                    } else {
                        System.println("Alpha Min!!");
                    }
                }
            }
            this.Fog_near = this.Fog_Z - this.Fog_Zwide;
            this.Fog_far = this.Fog_Z + this.Fog_Zwide;
            if (this.Fog_near < 0.0f) {
                this.Fog_near = 0.0f;
            }
            if (this.Fog_far < 0.0f) {
                this.Fog_far = 0.0f;
            }
            this.BaseCam.setFog(1, this.Fog_near, this.Fog_far, this.Fog_nearRatio, this.Fog_farRatio, this.Fog_R, this.Fog_G, this.Fog_B, this.Fog_A);
            this.ManualCam.setFog(0, this.Fog_near, this.Fog_far, this.Fog_nearRatio, this.Fog_farRatio, this.Fog_R, this.Fog_G, this.Fog_B, this.Fog_A);
            System.sleep(1);
        }
        System.println("Fog SetMode-------end");
    }

    void FogSet_printout() {
        Runtime.setRegister(0, this.Fog_near);
        Runtime.setRegister(1, this.Fog_far);
        Runtime.setRegister(2, this.Fog_nearRatio);
        Runtime.setRegister(3, this.Fog_farRatio);
        Runtime.setRegister(4, this.Fog_R);
        Runtime.setRegister(5, this.Fog_G);
        Runtime.setRegister(6, this.Fog_B);
        Runtime.setRegister(7, this.Fog_A);
        System.println("setFog(1,/[#0]f,/[#1]f,/[#2]f,/[#3]f,/[$4],/[$5],/[$6],/[$7]);");
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

    void KEYwait(String string) {
        this.msg.print(string);
        this.Objchk(this.Fjr);
        this.msg.clear();
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
        this.msg.clear();
        chr.mtn(n2, 8, 1.0f, false);
        chr.start(4, null);
        if (this.msgflag) {
            System.println("MSGprint(with Face).");
            System.println(string);
            this.msg.print(string);
        }
        System.sleep(n3);
        chr.mtn(n2 + 1, 0, 120, 15, 9, 1.0f, false);
        chr.start(4, null);
        System.println("Face End.");
        System.sleep(n - n3);
        this.msg.clear();
    }

    void MSG(String string) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        this.msg.print(string);
        System.println(string);
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

    void NAMEwin(int n, int n2, int n3, int n4, String string, int n5) {
        this.win0 = Window.create();
        this.win0.setSize(n, n2);
        this.win0.setLocation(n3, n4);
        this.win0.print(String.valueOf(string) + "/[wait(CLOSEwin)]/[close()]");
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
        effect.setForceLoop(true);
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
        if ((this.edge1 & 0x400) != 0) {
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
        if ((this.edge1 & 0x400) != 0) {
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
        System.println("cam1.setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.ManualCam.getRotateX(), this.ManualCam.getRotateY(), this.ManualCam.getRotateZ());
        System.println("cam1.setRotate   (/[#0]f,/[#1]f,/[#2]f);");
        Runtime.setRegister(0, this.ManualCam.getFov());
        System.println("cam1.setFov(/[#0]f);");
        this.setRegXYZ(this.BaseCam.getTranslateX(), this.BaseCam.getTranslateY(), this.BaseCam.getTranslateZ());
        System.println("BaseCam.setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.BaseCam.getRotateX(), this.BaseCam.getRotateY(), this.BaseCam.getRotateZ());
        System.println("BaseCam.setRotate(/[#0]f,/[#1]f,/[#2]f);");
        Runtime.setRegister(0, this.BaseCam.getFov());
        System.println("BaseCam.setFov(/[#0]f);");
        ++this.printoutcount;
    }

    void SETcheck(String string, Chr chr) {
        chr.getTranslate();
        chr.getRotate();
        chr.setVisible(true);
        this.msg.print(string);
        this.Objchk(chr);
        this.waitclear(5);
    }

    void SETcheck(String string, Unit unit) {
        unit.getTranslate();
        unit.getRotate();
        unit.setVisible(true);
        this.msg.print(string);
        this.Objchk(unit);
        this.waitclear(5);
    }

    void SETcheck(Chr chr) {
        chr.getTranslate();
        chr.getRotate();
        chr.setVisible(true);
        this.Objchk(chr);
        this.waitclear(5);
    }

    void SETcheck(Unit unit) {
        unit.getTranslate();
        unit.getRotate();
        unit.setVisible(true);
        this.Objchk(unit);
        this.waitclear(5);
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
    }

    void Timechk_SceneStart() {
        this.CutNo = 0;
        this.Timechk_NextCut = this.CutNo + 1;
        this.Timechk_srv_time = 0;
        this.Timechk_srv_totaltime = 0;
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
        System.println("setRotate   (/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
        System.println("setScale(/[#0]f,/[#1]f,/[#2]f);");
    }

    void Unit_print(Effect effect) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(effect.px, effect.py, effect.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(effect.rx, effect.ry, effect.rz);
        System.println("setRotate   (/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(this.Objchk_scale, this.Objchk_scale, this.Objchk_scale);
        System.println("setScale(/[#0]f,/[#1]f,/[#2]f);");
    }

    void Unit_print(Unit unit) {
        System.println("***********Objchk printout***********");
        this.setRegXYZ(unit.px, unit.py, unit.pz);
        System.println("setTranslate(/[#0]f,/[#1]f,/[#2]f);");
        this.setRegXYZ(unit.rx, unit.ry, unit.rz);
        System.println("setRotate   (/[#0]f,/[#1]f,/[#2]f);");
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

    void _MSG(int n, String string, String string2) {
        if (this.msg_clear_exec) {
            this.msg_clear.stop();
        }
        this.msg.clear();
        if (this.msgflag) {
            System.println("_MSGprint.");
            this.msg.print(string2);
            System.println(string2);
        }
        this.msg_clearwait = n;
        this.msg_clear.start();
    }

    void changeLocation(int n) {
        if (n == 0) {
            this.light1.setGlobalPointLightCol(0, 0.1f, 0.3f, 0.6f);
            this.light1.setGlobalPointLightPos(0, 6.06f, 1.35f, 2.5f);
            this.jr.setVisible(true);
        }
        if (n == 1) {
            this.light1.setGlobalPointLightReset();
            this.jr.setVisible(false);
        }
    }

    void change_location() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03025_F");
        Runtime.setFlags(337, 1, 1);
        System.println("XEVEJNAME:CFJ3_130 XEVEJPOINT:POINT3_130");
        Runtime.jumpCF(2450, 4);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(2450, 4);
    }

    void faceCHECK(String string, Chr chr, int n) {
        this.msg.clear();
        this.msg.print("\n" + string);
        System.println("===" + string + "==========================");
        this.Objchk(chr, n);
        this.Objchk(chr, n + 1);
        this.msg.clear();
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
        System.methodSignal(1);
        this.pc = PlayControl.create();
        Runtime.setLocation(1205);
        this.shion = new allCHARA(30);
        this.shionM = new allCHARA(1);
        this.feb = new allCHARA(284);
        this.allen = new allCHARA(263);
        this.momo = new allCHARA(4);
        this.ziggy = new allCHARA(6);
        this.chaos = new allCHARA(3);
        this.jr = new allCHARA(5);
        this.elly = new allCHARA(274);
        this.utre1 = new allCHARA(16901);
        this.utre2 = new allCHARA(16901);
        this.utre3 = new allCHARA(16901);
        this.utre4 = new allCHARA(16901);
        this.utre5 = new allCHARA(16901);
        this.dummy = new allCHARA(24582);
        this.shionM.setVisible(false);
        this.elly.setVisible(false);
        this.dummy.setVisible(false);
        this.stained = new allUNIT(24582);
        this.lighting = new allUNIT(24582);
        this.fog = new allUNIT(24582);
        this.target = new allUNIT(24582);
        this.point0 = new allUNIT(24582);
        this.point1 = new allUNIT(24582);
        this.point2 = new allUNIT(24582);
        this.stained.setVisible(false);
        this.fog.setVisible(false);
        this.target.setVisible(false);
        this.point0.setVisible(false);
        this.point1.setVisible(false);
        this.point2.setVisible(false);
        this.Fshion = new allFACE();
        this.FshionM = new allFACE();
        this.Ffeb = new allFACE();
        this.Fallen = new allFACE();
        this.Fmomo = new allFACE();
        this.Fziggy = new allFACE();
        this.Fchaos = new allFACE();
        this.Fjr = new allFACE();
        this.Felly = new allFACE();
        this.Fshion1 = new allFACE();
        this.Ffeb1 = new allFACE();
        this.Fallen1 = new allFACE();
        this.Fmomo1 = new allFACE();
        this.Fziggy1 = new allFACE();
        this.Fchaos1 = new allFACE();
        this.Fjr1 = new allFACE();
        this.Felly1 = new allFACE();
        this.door = new allMAP(78);
        this.movSPL = Spline.create();
        this.rotSPL = Spline.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam0.start(3, null);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, -10.0f, 0.0f);
        this.cam1.setRotate(-180.0f, 0.0f, 0.0f);
        this.cam1.setFov(40.0f);
        this.flash = new Effect(0);
        this.flash.args[0] = -2130706433;
        this.flash.args[1] = 5;
        this.flash.args[2] = 1;
        this.Rstained1 = new Effect(1758, 7.7f, 5.3f, -4.49f, 0.0f);
        this.Rstained1.setScale(1.0f, 1.0f, 1.0f);
        this.Rstained1.setClip(false);
        this.Rstained1.disp(true);
        this.Rstained2 = new Effect(1758, 7.7f, 5.3f, -0.49f, 0.0f);
        this.Rstained2.setScale(1.0f, 1.0f, 1.0f);
        this.Rstained2.setClip(false);
        this.Rstained2.disp(true);
        this.Rstained3 = new Effect(1758, 7.7f, 5.3f, 3.52f, 0.0f);
        this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
        this.Rstained3.setClip(false);
        this.Rstained3.disp(true);
        this.Lstained1 = new Effect(1758, -7.7f, 5.3f, -4.49f, 0.0f);
        this.Lstained1.setRotate(-180.0f, 0.0f, 0.0f);
        this.Lstained1.setScale(-1.0f, -1.0f, -1.0f);
        this.Lstained1.setClip(false);
        this.Lstained1.disp(true);
        this.Lstained2 = new Effect(1758, -7.7f, 5.3f, -0.49f, 0.0f);
        this.Lstained2.setRotate(-180.0f, 0.0f, 0.0f);
        this.Lstained2.setScale(-1.0f, -1.0f, -1.0f);
        this.Lstained2.setClip(false);
        this.Lstained2.disp(true);
        this.Lstained3 = new Effect(1758, -7.7f, 5.3f, 3.52f, 0.0f);
        this.Lstained3.setRotate(-180.0f, 0.0f, 0.0f);
        this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
        this.Lstained3.setClip(false);
        this.Lstained3.disp(true);
        this.candle = new Effect(1621, 0.305f, 2.33f, -6.005f, 0.0f);
        this.candle.setScale(0.6f, 0.6f, 0.6f);
        this.candle.disp(true);
        this.F_light1 = new Effect(1405, 2.57f, 5.94f, -10.07f, 0.0f);
        this.F_light1.setScale(0.7f, 0.7f, 0.7f);
        this.F_light1.disp(true);
        this.F_light2 = new Effect(1405, -2.33f, 5.94f, -10.07f, 0.0f);
        this.F_light2.setScale(0.7f, 0.7f, 0.7f);
        this.F_light2.disp(true);
        this.R_light1 = new Effect(1405, 5.95f, 5.3f, -10.45f, 0.0f);
        this.R_light1.setScale(0.5f, 0.5f, 0.5f);
        this.R_light1.disp(true);
        this.R_light2 = new Effect(1405, 5.95f, 5.3f, -6.45f, 0.0f);
        this.R_light2.setScale(0.5f, 0.5f, 0.5f);
        this.R_light2.disp(true);
        this.L_light1 = new Effect(1405, -6.0f, 5.3f, -10.45f, 0.0f);
        this.L_light1.setScale(0.5f, 0.5f, 0.5f);
        this.L_light1.disp(true);
        this.L_light2 = new Effect(1405, -6.0f, 5.3f, -6.45f, 0.0f);
        this.L_light2.setScale(0.5f, 0.5f, 0.5f);
        this.L_light2.disp(true);
    }

    void initialize() {
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

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    void loadarc(Unit unit, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) unit, object, 2);
    }

    void loadarc(Unit unit, String string, String string2) {
        Object object = Toolkit.loadResource(string);
        Object object2 = Toolkit.loadResource(string2);
        Toolkit.loadResource((Object) unit, object2, 1);
        Toolkit.loadResource((Object) unit, object, 2);
    }

    static void main() {
    }

    void mapCHECK(int n, int n2) {
        System.println("==" + n + "================================");
        int n3 = n;
        while (n3 < n2) {
            System.println("--" + n + n3 + "--------------------------------");
            Stage.setVisible(n3, false);
            System.sleep(2);
            Stage.setVisible(n3, true);
            System.sleep(2);
            Stage.setVisible(n3, false);
            System.sleep(2);
            Stage.setVisible(n3, true);
            System.sleep(2);
            Stage.setVisible(n3, false);
            this.Objchk(this.Fjr);
            ++n3;
        }
        int n4 = n;
        while (n4 < n2) {
            Stage.setVisible(n4, true);
            ++n4;
        }
    }

    void msg_clear_thread() {
        this.msg_clear_exec = true;
        System.sleep(this.msg_clearwait);
        this.msg.clear();
        this.msg_clear_exec = false;
    }

    void msg_print(String string, String string2) {
        this.msg.print(string2);
    }

    void play() {
        this.CameraTool();
        this.CaptureTool();
        this.Timechk();
        this.STCamera_init();
        this.cam1.change();
        this.camerawork.cut0();
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.shionM.face, "FLSshion.fpk");
        this.loadarc(this.feb.face, "FLSfeb.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.loadarc(this.jr.face, "FLSjr.fpk");
        this.loadarc(this.elly.face, "FLSelly.fpk");
        this.playSCENE0();
        this.playSCENE1();
        this.playSCENE2();
        this.playSCENE3();
        this.Timechk_SceneEnd();
    }

    void playAGAIN() {
        this.KEYwait("スタート　１８０→１８９");
        this.mapCHECK(180, 190);
        this.KEYwait("スタート　１９０→１９９");
        this.mapCHECK(190, 200);
        this.KEYwait("スタート　２００→２０９");
        this.mapCHECK(200, 210);
        this.KEYwait("スタート　２１０→２１９");
        this.mapCHECK(210, 220);
        this.KEYwait("スタート　２２０→２２９");
        this.mapCHECK(220, 230);
        this.KEYwait("スタート　２３０→２３９");
        this.mapCHECK(230, 240);
        this.KEYwait("スタート　２４０→２４９");
        this.mapCHECK(240, 250);
        this.KEYwait("スタート　２５０→２５９");
        this.mapCHECK(250, 260);
        this.KEYwait("スタート　２６０→２６９");
        this.mapCHECK(260, 270);
        this.KEYwait("スタート　２７０→２７９");
        this.mapCHECK(270, 280);
        this.KEYwait("スタート　２８０→２８９");
        this.mapCHECK(280, 290);
        this.KEYwait("スタート　２９０→２９９");
        this.mapCHECK(290, 300);
        this.KEYwait("スタート　３００→３０９");
        this.mapCHECK(300, 310);
        this.KEYwait("スタート　３１０→３１９");
        this.mapCHECK(310, 320);
        this.KEYwait("スタート　３２０→３２９");
        this.mapCHECK(320, 330);
        this.KEYwait("スタート　３３０→３３９");
        this.mapCHECK(330, 340);
        this.KEYwait("スタート　３４０→３４９");
        this.mapCHECK(340, 350);
        this.KEYwait("スタート　３５０→３５９");
        this.mapCHECK(350, 360);
        this.KEYwait("スタート　３６０→３６９");
        this.mapCHECK(360, 370);
        this.KEYwait("スタート　３７０→３７９");
        this.mapCHECK(370, 380);
        this.KEYwait("スタート　３８０→３８９");
        this.mapCHECK(380, 390);
        this.KEYwait("スタート　３９０→３９９");
        this.mapCHECK(390, 400);
        this.KEYwait("スタート　４００→４０９");
        this.mapCHECK(400, 410);
        this.KEYwait("スタート　４１０→４１９");
        this.mapCHECK(410, 420);
        this.KEYwait("スタート　４２０→４２９");
        this.mapCHECK(420, 430);
    }

    void playSCENE0() {
        Sound.streamPlay(1390060, 48000);
        Stage.renderCommand(4);
        this.shion.setMotionFlags(0x2000000, true);
        this.shionM.setMotionFlags(0x2000000, true);
        this.feb.setMotionFlags(0x2000000, true);
        this.momo.setMotionFlags(0x2000000, true);
        this.chaos.setMotionFlags(0x2000000, true);
        this.elly.setMotionFlags(0x40000000, true);
        this.allen.setMotionFlags(0x40000000, true);
        this.jr.setMotionFlags(0x40000000, true);
        this.ziggy.setMotionFlags(0x40000000, true);
        this.utre1.setMotionFlags(0x40000000, true);
        this.utre2.setMotionFlags(0x40000000, true);
        this.utre3.setMotionFlags(0x40000000, true);
        this.utre4.setMotionFlags(0x40000000, true);
        this.utre5.setMotionFlags(0x40000000, true);
        this.dummy.start(1, "act2_chara");
        this.feb.mtn(263, 0, 150, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.stained.start(1, "act2_stained_set");
        this.Fallen.start(1, "face2_allen");
        this.Fchaos.start(1, "face2_chaos");
        this.Fjr.start(1, "face2_jr");
        this.Fmomo.start(1, "face2_momo");
        this.Fshion.start(1, "face2_shion");
        this.Fziggy.start(1, "face2_ziggy");
        this.Ffeb.start(1, "face2_feb");
        this.camerawork.cut2();
        this.pc.loadCamera("c03s25cut02.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(150);
        this.chaos.renderCommand(534);
        this.momo.renderCommand(534);
        this.ziggy.renderCommand(534);
        this.allen.renderCommand(534);
        this.shion.renderCommand(534);
        this.elly.renderCommand(534);
        this.dummy.start(1, "act3_chara");
        this.allen.mtn(264, 0, 150, 8, 0, 1.0f, true);
        this.chaos.mtn(267, 0, 150, 8, 0, 1.0f, true);
        this.jr.mtn(268, 0, 150, 8, 0, 1.0f, true);
        this.momo.mtn(269, 0, 150, 8, 0, 1.0f, true);
        this.shion.mtn(270, 0, 150, 8, 0, 1.0f, true);
        this.ziggy.mtn(271, 0, 150, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.momo.start(5, null);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.stained.start(1, "act3_stained_set");
        this.lighting.start(1, "act3_light");
        this.Fallen.start(1, "face3_allen");
        this.Ffeb.start(1, "face_stop_feb");
        this.camerawork.cut3();
        this.pc.loadCamera("c03s25cut03.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(42);
        this.msg.print("Someone's here.");
        this.waitclear(45);
        System.sleep(20);
        this.msg.print("Those clothes...is she a Realian?");
        System.sleep(43);
        this.momo.renderCommand(512);
        this.allen.renderCommand(512);
        this.dummy.start(1, "act3b_chara");
        this.allen.mtn(265, 0, 157, 8, 0, 1.0f, true);
        this.momo.mtn(266, 0, 157, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.momo.start(5, null);
        this.stained.start(1, "act3b_stained_set");
        this.Fmomo.start(1, "face3b_momo");
        this.camerawork.cut3b();
        this.pc.loadCamera("c03s25cut03b.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.waitclear(35);
        this.msg.print("Yes...\nShe definitely seems to be a Realian,");
        this.waitclear(122);
        this.dummy.start(1, "act4_chara");
        this.feb.mtn(272, 0, 135, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.stained.start(1, "act4_stained_set");
        this.Ffeb.start(1, "face4_feb");
        this.camerawork.cut4();
        this.pc.loadCamera("c03s25cut04.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(25);
        this.msg.print("but...I also sense\nsomething different...");
        this.waitclear(80);
        System.sleep(30);
        this.allen.renderCommand(534);
        this.chaos.renderCommand(534);
        this.momo.renderCommand(534);
        this.ziggy.renderCommand(534);
        this.shion.renderCommand(534);
        this.dummy.start(1, "act5_chara");
        this.allen.mtn(273, 0, 390, 8, 0, 1.0f, true);
        this.chaos.mtn(274, 0, 390, 8, 0, 1.0f, true);
        this.jr.mtn(275, 0, 390, 8, 0, 1.0f, true);
        this.momo.mtn(276, 0, 390, 8, 0, 1.0f, true);
        this.shion.mtn(277, 0, 390, 8, 0, 1.0f, true);
        this.ziggy.mtn(278, 0, 390, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.momo.start(5, null);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.target.start(1, "act0_targetL1_reset");
        this.stained.start(1, "act5_stained_set");
        this.Fallen.start(1, "face5_allen");
        this.Fshion.start(1, "face5_shion");
        this.Fziggy.start(1, "face5_ziggy");
        this.Fchaos.start(1, "face5_chaos");
        this.Fjr.start(1, "face5_jr");
        this.Ffeb.start(1, "face_stop_feb");
        this.Fmomo.start(1, "face_stop_momo");
        this.camerawork.cut5();
        this.pc.loadCamera("c03s25cut05.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(20);
        this.msg.print("Is this based on one of\nour memories, too, Chief?");
        this.waitclear(110);
        System.sleep(51);
        this.msg.print("...Chief?");
        this.waitclear(39);
        System.sleep(50);
        this.msg.print("Are you...a Realian?");
        this.waitclear(105);
        System.sleep(15);
        this.dummy.start(1, "act6_chara");
        this.feb.mtn(279, 0, 354, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.shion.setVisible(true);
        this.shion.mtn(284, 0, 0, 8, 0, 1.0f, true);
        this.shion.start(5, null);
        this.stained.start(1, "act6_stained_set");
        this.Ffeb.start(1, "face6_feb");
        this.camerawork.cut6();
        this.pc.loadCamera("c03s25cut06.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(10);
        this.msg.print("Yes.\nMy name is Febronia.");
        this.waitclear(115);
        System.sleep(25);
        this.msg.print("I came to take care of this church\nbecause I longed for a place\nwhere Realians could find peace.");
        this.waitclear(190);
        System.sleep(14);
        this.shion.renderCommand(22);
        this.dummy.start(1, "act7a_chara");
        this.allen.mtn(280, 0, 199, 8, 0, 1.0f, true);
        this.chaos.mtn(281, 0, 199, 8, 0, 1.0f, true);
        this.jr.mtn(282, 0, 199, 8, 0, 1.0f, true);
        this.momo.mtn(283, 0, 199, 8, 0, 1.0f, true);
        this.shion.mtn(284, 0, 199, 8, 0, 1.0f, true);
        this.ziggy.mtn(285, 0, 199, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.momo.start(5, null);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.stained.start(1, "act7_stained_set");
        this.Fshion.start(1, "face7a_shion");
        this.camerawork.cut7a();
        this.pc.loadCamera("c03s25cut07a.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(8);
        this.msg.print("Febronia...");
        this.waitclear(65);
        System.sleep(20);
        this.msg.print("Do you know her?");
        this.waitclear(45);
        this.waitclear(55);
        System.sleep(5);
        this.dummy.start(1, "act_bank_on");
        this.feb.mtn(257, 0, 84, 8, 0, 1.0f, true);
        this.utre1.mtn(258, 0, 84, 8, 0, 1.0f, true);
        this.utre2.mtn(259, 0, 84, 8, 0, 1.0f, true);
        this.utre3.mtn(260, 0, 84, 8, 0, 1.0f, true);
        this.utre4.mtn(261, 0, 84, 8, 0, 1.0f, true);
        this.utre5.mtn(262, 0, 84, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.utre1.start(5, null);
        this.utre2.start(5, null);
        this.utre3.start(5, null);
        this.utre4.start(5, null);
        this.utre5.start(5, null);
        this.target.start(1, "act_bank_stained_set");
        this.Ffeb.start(1, "face_bank_feb");
        this.Ffeb1.start(1, "eyes_bank_feb");
        this.flash.call(0);
        this.camerawork.cut_bank();
        this.pc.loadCamera("c03s25bank.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(20);
    }

    void playSCENE1() {
        this.dummy.start(1, "act8_chara");
        this.chaos.mtn(286, 0, 120, 8, 0, 1.0f, true);
        this.jr.mtn(287, 0, 120, 8, 0, 1.0f, true);
        this.shion.mtn(288, 0, 120, 8, 0, 1.0f, true);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.shion.start(5, null);
        this.stained.start(1, "act8_stained_set");
        this.Fshion.start(1, "face8_shion");
        this.Ffeb.start(1, "face8_feb");
        this.Fjr.start(1, "face8_jr");
        this.Fchaos.start(1, "face8_chaos");
        this.Fshion1.start(1, "eyes8_shion");
        this.Ffeb1.start(1, "eyes_bank_off_feb");
        this.flash.call(0);
        this.camerawork.cut8();
        this.pc.loadCamera("c03s25cut08.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(120);
        this.dummy.start(1, "act9_chara");
        this.feb.mtn(289, 0, 60, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.shion.mtn(292, 0, 1, 8, 0, 1.0f, true);
        this.shion.start(5, null);
        this.stained.start(1, "act6_stained_set");
        this.Ffeb.start(1, "face9_feb");
        this.camerawork.cut9();
        this.pc.loadCamera("c03s25cut09.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(60);
        this.dummy.start(1, "act10_chara");
        this.chaos.mtn(290, 0, 150, 8, 0, 1.0f, true);
        this.jr.mtn(291, 0, 150, 8, 0, 1.0f, true);
        this.shion.mtn(292, 0, 150, 8, 0, 1.0f, true);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.shion.start(5, null);
        this.stained.start(1, "act8_stained_set");
        this.Fshion.start(1, "face10_shion");
        this.Fjr.start(1, "face10_jr");
        this.Fshion1.start(1, "eyes10_shion");
        this.camerawork.cut10();
        this.pc.loadCamera("c03s25cut10.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(22);
        System.sleep(75);
        this.msg.print("I...I know you...");
        System.sleep(53);
        this.dummy.start(1, "act11_chara");
        this.allen.mtn(293, 0, 330, 8, 0, 1.0f, true);
        this.feb.mtn(294, 0, 330, 8, 0, 1.0f, true);
        this.shion.mtn(295, 0, 330, 8, 0, 1.0f, true);
        this.ziggy.mtn(296, 0, 330, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.feb.start(5, null);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.chaos.setVisible(false);
        this.stained.start(1, "act11_stained_set");
        this.Fchaos.start(1, "face_stop_chaos");
        this.Fjr.start(1, "face_stop_jr");
        this.Fmomo.start(1, "face_stop_momo");
        this.Fziggy.start(1, "face_stop_ziggy");
        this.camerawork.cut11();
        this.pc.loadCamera("c03s25cut11.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.waitclear(61);
        System.sleep(57);
        this.msg.print("I...know...you...!");
        this.waitclear(114);
        System.sleep(98);
        this.dummy.start(1, "act12_chara");
        this.allen.mtn(297, 0, 330, 8, 0, 1.0f, true);
        this.chaos.mtn(298, 0, 330, 8, 0, 1.0f, true);
        this.jr.mtn(299, 0, 330, 8, 0, 1.0f, true);
        this.shion.mtn(300, 0, 330, 8, 0, 1.0f, true);
        this.ziggy.mtn(301, 0, 330, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.jr.start(5, null);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.chaos.setVisible(true);
        this.dummy.start(1, "act12_chara");
        this.Fshion.start(1, "face12_shion");
        this.Fallen.start(1, "face12_allen");
        this.Fjr.start(1, "face12_jr");
        this.Fchaos.start(1, "face12_chaos");
        this.Fshion1.start(1, "eyes12_shion");
        this.stained.start(1, "act12_stained_set");
        this.Ffeb.start(1, "face_stop_feb");
        this.Fmomo.start(1, "face_stop_momo");
        this.Fziggy.start(1, "face_stop_ziggy");
        this.camerawork.cut12();
        this.pc.loadCamera("c03s25cut12.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(10);
        this.msg.print("But...no...\nI don't want to remember...");
        this.waitclear(158);
        System.sleep(30);
        this.msg.print("It's...it's...");
        this.waitclear(107);
        System.sleep(25);
        this.dummy.start(1, "act_bank_on");
        this.feb.mtn(257, 0, 84, 8, 0, 1.0f, true);
        this.utre1.mtn(258, 0, 84, 8, 0, 1.0f, true);
        this.utre2.mtn(259, 0, 84, 8, 0, 1.0f, true);
        this.utre3.mtn(260, 0, 84, 8, 0, 1.0f, true);
        this.utre4.mtn(261, 0, 84, 8, 0, 1.0f, true);
        this.utre5.mtn(262, 0, 84, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.utre1.start(5, null);
        this.utre2.start(5, null);
        this.utre3.start(5, null);
        this.utre4.start(5, null);
        this.utre5.start(5, null);
        this.target.start(1, "act_bank_stained_set");
        this.Ffeb.start(1, "face_bank_feb");
        this.Ffeb1.start(1, "eyes_bank_feb");
        this.flash.call(0);
        this.camerawork.cut_bank();
        this.pc.loadCamera("c03s25bank.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(45);
    }

    void playSCENE2() {
        this.shionM.setMotionFlags(0x40000000, true);
        this.feb.setMotionFlags(0x40000000, true);
        this.momo.setMotionFlags(0x40000000, true);
        this.chaos.setMotionFlags(0x40000000, true);
        this.chaos.renderCommand(534);
        this.momo.renderCommand(534);
        this.ziggy.renderCommand(534);
        this.allen.renderCommand(534);
        this.shion.renderCommand(534);
        this.shionM.renderCommand(534);
        this.feb.renderCommand(534);
        this.dummy.start(1, "act14_chara");
        this.allen.mtn(302, 0, 135, 8, 0, 1.0f, true);
        this.chaos.mtn(304, 0, 135, 8, 0, 1.0f, true);
        this.feb.mtn(305, 0, 135, 8, 0, 1.0f, true);
        this.jr.mtn(306, 0, 135, 8, 0, 1.0f, true);
        this.momo.mtn(307, 0, 135, 8, 0, 1.0f, true);
        this.shionM.mtn(308, 0, 135, 8, 0, 1.0f, true);
        this.ziggy.mtn(309, 0, 135, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.feb.start(5, null);
        this.jr.start(5, null);
        this.momo.start(5, null);
        this.shionM.start(5, null);
        this.ziggy.start(5, null);
        this.stained.start(1, "act14_stained_set");
        this.Ffeb.start(1, "face_stop_feb");
        this.Ffeb1.start(1, "eyes_bank_off_feb");
        this.camerawork.cut14();
        this.pc.loadCamera("c03s25cut14.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(135);
        this.shionM.setMotionFlags(0x40000000, false);
        this.feb.setMotionFlags(0x40000000, false);
        this.momo.setMotionFlags(0x40000000, false);
        this.chaos.setMotionFlags(0x40000000, false);
        this.shionM.setMotionFlags(0x2000000, true);
        this.feb.setMotionFlags(0x2000000, true);
        this.momo.setMotionFlags(0x2000000, true);
        this.chaos.setMotionFlags(0x2000000, true);
        this.dummy.start(1, "act14b_chara");
        this.feb.mtn(303, 0, 150, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.stained.start(1, "act6_stained_set");
        this.lighting.start(1, "act14B_light");
        this.Ffeb.start(1, "face14b_feb");
        this.Ffeb1.start(1, "eyes14b_feb");
        this.camerawork.cut14b();
        this.pc.loadCamera("c03s25cut14b.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(30);
        this.msg.print("Follow me, Shion.");
        this.waitclear(85);
        System.sleep(35);
        this.dummy.start(1, "act15_chara");
        this.allen.mtn(310, 0, 105, 8, 0, 1.0f, true);
        this.chaos.mtn(311, 0, 105, 8, 0, 1.0f, true);
        this.momo.mtn(312, 0, 105, 8, 0, 1.0f, true);
        this.shion.mtn(313, 0, 105, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.momo.start(5, null);
        this.shion.start(5, null);
        this.stained.start(1, "act15_stained_set");
        this.Fshion.start(1, "face15_shion");
        this.Ffeb1.start(1, "eyes15_feb");
        this.camerawork.cut15();
        this.pc.loadCamera("c03s25cut15.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(105);
        this.dummy.start(1, "act16_chara");
        this.feb.mtn(314, 0, 460, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.Ffeb.start(1, "face16_feb");
        this.camerawork.cut16();
        this.pc.loadCamera("c03s25cut16.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(225);
        this.Felly.start(1, "face18_elly");
        this.camerawork.cut18();
        System.sleep(55);
        this.msg.print("The instant you open that door");
        this.waitclear(80);
        this.elly.setMotionFlags(0x40000000, false);
        this.elly.setMotionFlags(0x2000000, true);
        this.dummy.start(1, "act19_chara");
        this.allen.mtn(315, 0, 144, 8, 0, 1.0f, true);
        this.elly.mtn(316, 0, 144, 8, 0, 1.0f, true);
        this.shion.mtn(317, 0, 144, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.elly.start(5, null);
        this.shion.start(5, null);
        this.elly.setVisible(true);
        this.stained.start(1, "act19_stained_set");
        this.Felly.start(1, "face19_elly");
        this.camerawork.cut19();
        this.pc.loadCamera("c03s25cut19.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.msg.print("you will come face to face\nwith yourselves.");
        this.waitclear(142);
        this.dummy.start(1, "act20_chara");
        this.elly.mtn(318, 0, 135, 8, 0, 1.0f, true);
        this.elly.start(5, null);
        this.stained.start(1, "act20_stained_set");
        this.Felly.start(1, "face20_elly");
        this.camerawork.cut20();
        this.pc.loadCamera("c03s25cut20.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(15);
        this.msg.print("It will be an experience\nfull of sorrow");
        this.waitclear(90);
        System.sleep(30);
        this.feb.setMotionFlags(0x40000000, true);
        this.chaos.setMotionFlags(0x40000000, true);
        this.momo.setMotionFlags(0x40000000, true);
        this.shion.setMotionFlags(0x40000000, true);
        this.shionM.setMotionFlags(0x40000000, true);
        this.dummy.start(1, "act21_chara");
        this.allen.mtn(319, 0, 251, 8, 0, 1.0f, true);
        this.chaos.mtn(320, 0, 251, 8, 0, 1.0f, true);
        this.elly.mtn(321, 0, 251, 8, 0, 1.0f, true);
        this.jr.mtn(322, 0, 251, 8, 0, 1.0f, true);
        this.momo.mtn(323, 0, 251, 8, 0, 1.0f, true);
        this.shionM.mtn(324, 0, 251, 8, 0, 1.0f, true);
        this.ziggy.mtn(325, 0, 251, 8, 0, 1.0f, true);
        this.allen.start(5, null);
        this.chaos.start(5, null);
        this.elly.start(5, null);
        this.jr.start(5, null);
        this.momo.start(5, null);
        this.shionM.start(5, null);
        this.ziggy.start(5, null);
        this.shion.mtn(326, 0, 0, 8, 0, 1.0f, true);
        this.shion.start(5, null);
        this.stained.start(1, "act21_stained_set");
        this.lighting.start(1, "act21_light");
        this.FshionM.start(1, "face21_shionM");
        this.Fshion.start(1, "face21_shion");
        this.Felly.start(1, "face_stop_elly");
        this.camerawork.cut21();
        this.pc.loadCamera("c03s25cut21.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.msg.print("and pain...But it is, both to\nyou and to us, a very, very\nimportant experience.");
        System.sleep(251);
        this.feb.setMotionFlags(0x40000000, false);
        this.chaos.setMotionFlags(0x40000000, false);
        this.momo.setMotionFlags(0x40000000, false);
        this.shion.setMotionFlags(0x40000000, false);
        this.shionM.setMotionFlags(0x40000000, false);
        this.feb.setMotionFlags(0x2000000, true);
        this.momo.setMotionFlags(0x2000000, true);
        this.chaos.setMotionFlags(0x2000000, true);
        this.shion.setMotionFlags(0x2000000, true);
        this.shionM.setMotionFlags(0x2000000, true);
        this.shion.renderCommand(22);
        this.dummy.start(1, "act22_chara");
        this.shion.mtn(326, 0, 120, 8, 0, 1.0f, true);
        this.ziggy.mtn(327, 0, 120, 8, 0, 1.0f, true);
        this.shion.start(5, null);
        this.ziggy.start(5, null);
        this.elly.setVisible(false);
        this.stained.start(1, "act22_stained_set");
        this.Fshion.start(1, "face22_shion");
        this.Fziggy.start(1, "face22_ziggy");
        this.camerawork.cut22();
        this.pc.loadCamera("c03s25cut22.cam");
        this.pc.init(1, 0);
        this.pc.start();
        this.waitclear(69);
        System.sleep(51);
    }

    void playSCENE3() {
        this.dummy.start(1, "act24_chara");
        this.feb.mtn(328, 0, 75, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.shion.mtn(330, 0, 0, 8, 0, 1.0f, true);
        this.shion.start(5, null);
        this.Ffeb.start(1, "face24_feb");
        this.camerawork.cut24();
        this.pc.loadCamera("c03s25cut24.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(75);
        this.dummy.start(1, "act25_chara");
        this.chaos.mtn(329, 0, 90, 8, 0, 1.0f, true);
        this.shion.mtn(330, 0, 90, 8, 0, 1.0f, true);
        this.chaos.start(5, null);
        this.shion.start(5, null);
        this.stained.start(1, "act25_stained_set");
        this.Fshion.start(1, "face25_shion");
        this.camerawork.cut25();
        this.pc.loadCamera("c03s25cut25.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(90);
        this.dummy.start(1, "act26_chara");
        this.feb.mtn(331, 0, 135, 8, 0, 1.0f, true);
        this.shion.mtn(332, 0, 135, 8, 0, 1.0f, true);
        this.feb.start(5, null);
        this.shion.start(5, null);
        this.stained.start(1, "act26_stained_set");
        this.camerawork.cut26();
        this.pc.loadCamera("c03s25cut26.cam");
        this.pc.init(1, 0);
        this.pc.start();
        System.sleep(135);
    }

    void playSCENE4() {
        this.target.start(1, "act1_targetR1");
        this.msg.print("ターゲット Ｒ１");
        this.Objchk(this.target);
        this.msg.clear();
        this.target.start(1, "act1_targetR2");
        this.msg.print("ターゲット Ｒ２");
        this.Objchk(this.target);
        this.msg.clear();
        this.target.start(1, "act1_targetR3");
        this.msg.print("ターゲット Ｒ３");
        this.Objchk(this.target);
        this.msg.clear();
        this.target.start(1, "act1_targetL1");
        this.msg.print("ターゲット Ｌ１");
        this.Objchk(this.target);
        this.msg.clear();
        this.target.start(1, "act1_targetL2");
        this.msg.print("ターゲット Ｌ２");
        this.Objchk(this.target);
        this.msg.clear();
        this.target.start(1, "act1_targetL3");
        this.msg.print("ターゲット Ｌ３");
        this.Objchk(this.target);
        this.msg.clear();
    }

    void putInt(int n) {
        Runtime.setRegister(0, n);
        System.println("/[$0]");
    }

    void putInt(int n, int n2) {
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        System.println("/[$0](Cursor = /[$1])");
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

    void start() {
        this.CameraTool();
        this.CaptureTool();
        this.Timechk();
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

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class allCHARA
            extends Chr {
        Chr face;

        public allCHARA(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
        }

        public void act10_chara() {
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act11_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
        }

        public void act12_chara() {
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act14_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shionM.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.feb.face.setVisible(false);
            SCE03025.this.feb.setVisible(0, false);
            SCE03025.this.feb.setVisible(7, false);
            SCE03025.this.feb.setVisible(8, false);
            SCE03025.this.feb.setVisible(9, false);
            SCE03025.this.feb.setVisible(10, false);
            SCE03025.this.feb.setVisible(11, false);
            SCE03025.this.utre1.setVisible(false);
            SCE03025.this.utre2.setVisible(false);
            SCE03025.this.utre3.setVisible(false);
            SCE03025.this.utre4.setVisible(false);
            SCE03025.this.utre5.setVisible(false);
        }

        public void act14b_chara() {
            SCE03025.this.feb.face.setVisible(true);
            SCE03025.this.feb.setVisible(0, true);
            SCE03025.this.feb.setVisible(7, true);
            SCE03025.this.feb.setVisible(8, true);
            SCE03025.this.feb.setVisible(9, true);
            SCE03025.this.feb.setVisible(10, true);
            SCE03025.this.feb.setVisible(11, true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.shionM.setVisible(false);
            SCE03025.this.ziggy.setVisible(false);
        }

        public void act15_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act16_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.shion.setVisible(false);
        }

        public void act19_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.elly.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act20_chara() {
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.shion.setVisible(false);
        }

        public void act21_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.elly.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shionM.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.elly.face.setVisible(false);
        }

        public void act22_chara() {
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.elly.face.setVisible(true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.elly.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.elly.setVisible(false);
            SCE03025.this.shionM.setVisible(false);
        }

        public void act24_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.ziggy.setVisible(false);
        }

        public void act25_chara() {
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act26_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.chaos.setVisible(false);
        }

        public void act2_chara() {
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.shion.setVisible(false);
            SCE03025.this.ziggy.setVisible(false);
            SCE03025.this.utre1.setVisible(false);
            SCE03025.this.utre2.setVisible(false);
            SCE03025.this.utre3.setVisible(false);
            SCE03025.this.utre4.setVisible(false);
            SCE03025.this.utre5.setVisible(false);
        }

        public void act3_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.feb.setVisible(false);
            SCE03025.this.utre1.setVisible(false);
            SCE03025.this.utre2.setVisible(false);
            SCE03025.this.utre3.setVisible(false);
            SCE03025.this.utre4.setVisible(false);
            SCE03025.this.utre5.setVisible(false);
        }

        public void act3b_chara() {
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.shion.setVisible(false);
            SCE03025.this.ziggy.setVisible(false);
        }

        public void act4_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.momo.setVisible(false);
        }

        public void act5_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act6_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.ziggy.setVisible(false);
        }

        public void act7a_chara() {
            SCE03025.this.allen.setVisible(true);
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.momo.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.ziggy.setVisible(true);
            SCE03025.this.feb.setVisible(false);
        }

        public void act8_chara() {
            SCE03025.this.chaos.setVisible(true);
            SCE03025.this.jr.setVisible(true);
            SCE03025.this.shion.setVisible(true);
            SCE03025.this.feb.setVisible(false);
            SCE03025.this.utre1.setVisible(false);
            SCE03025.this.utre2.setVisible(false);
            SCE03025.this.utre3.setVisible(false);
            SCE03025.this.utre4.setVisible(false);
            SCE03025.this.utre5.setVisible(false);
        }

        public void act9_chara() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.shion.setVisible(false);
        }

        public void act_bank_off() {
            SCE03025.this.utre1.setVisible(false);
            SCE03025.this.utre2.setVisible(false);
            SCE03025.this.utre3.setVisible(false);
            SCE03025.this.utre4.setVisible(false);
            SCE03025.this.utre5.setVisible(false);
        }

        public void act_bank_on() {
            SCE03025.this.feb.setVisible(true);
            SCE03025.this.utre1.setVisible(true);
            SCE03025.this.utre2.setVisible(true);
            SCE03025.this.utre3.setVisible(true);
            SCE03025.this.utre4.setVisible(true);
            SCE03025.this.utre5.setVisible(true);
            SCE03025.this.allen.setVisible(false);
            SCE03025.this.chaos.setVisible(false);
            SCE03025.this.jr.setVisible(false);
            SCE03025.this.momo.setVisible(false);
            SCE03025.this.shion.setVisible(false);
            SCE03025.this.ziggy.setVisible(false);
        }

        public void reset_mot() {
            this.setVisible(false);
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(Integer.MIN_VALUE, 0, 1.0f, true);
        }

        public void reset_pos() {
            this.setVisible(false);
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }
    }

    class allMECHA
            extends Chr {
        public allMECHA(int n) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        public allMECHA(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }
    }

    class allFACE
            extends Chr {
        public allFACE() {
            this.init(24582);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }

        public void eyes10_shion() {
            System.sleep(5);
            SCE03025.this.shion.look_eye_speed(1.2f);
            SCE03025.this.shion.look_eye_set(-1.15f, 0.7f);
            System.sleep(20);
            SCE03025.this.shion.look_eye_speed(0.8f);
            SCE03025.this.shion.look_eye_set(0.5f, -0.4f);
            System.sleep(20);
            SCE03025.this.shion.look_eye_speed(1.1f);
            SCE03025.this.shion.look_eye_set(-1.2f, 1.5f);
            System.sleep(15);
            SCE03025.this.shion.look_eye_speed(0.7f);
            SCE03025.this.shion.look_eye_set(0.7f, 0.2f);
            System.sleep(20);
            SCE03025.this.shion.look_eye_speed(1.5f);
            SCE03025.this.shion.look_eye_set(-0.7f, -0.2f);
            System.sleep(10);
            SCE03025.this.shion.look_eye_speed(0.9f);
            SCE03025.this.shion.look_eye_set(0.5f, 0.3f);
            System.sleep(25);
            SCE03025.this.shion.look_eye_speed(1.1f);
            SCE03025.this.shion.look_eye_set(0.0f, 0.0f);
            System.sleep(20);
            SCE03025.this.shion.look_eye_speed(1.3f);
            SCE03025.this.shion.look_eye_set(1.0f, -0.5f);
            System.sleep(15);
        }

        public void eyes12_shion() {
            SCE03025.this.shion.look_default();
        }

        public void eyes14b_feb() {
            SCE03025.this.feb.look_default();
            SCE03025.this.feb.look_speed(0.0f);
            SCE03025.this.feb.look_char(SCE03025.this.shion);
        }

        public void eyes15_feb() {
            SCE03025.this.feb.look_default();
        }

        public void eyes8_shion() {
            SCE03025.this.shion.look_default();
            System.sleep(95);
            SCE03025.this.shion.look_eye_speed(0.95f);
            SCE03025.this.shion.look_eye_set(-1.5f, 0.2f);
            System.sleep(20);
            SCE03025.this.shion.look_eye_speed(0.65f);
            SCE03025.this.shion.look_eye_set(0.5f, -0.5f);
            System.sleep(5);
        }

        public void eyes_bank_feb() {
            SCE03025.this.feb.look_default();
            SCE03025.this.feb.look_eye_set(-2.0f, -16.0f);
            SCE03025.this.feb.look_eye_speed(999.0f);
        }

        public void eyes_bank_off_feb() {
            SCE03025.this.feb.look_default();
        }

        public void face10_jr() {
            System.sleep(22);
            System.sleep(10);
            System.sleep(10);
            SCE03025.this.jr.face.mtn(7, 0, 120, 2, 9, 0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(7);
            SCE03025.this.jr.face.mtn(7, 2, 2, 5, 0, 0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(38);
            SCE03025.this.jr.face.mtn(7, 0, 3, 5, 0, -0.2f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(45);
            SCE03025.this.jr.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(18);
        }

        public void face10_shion() {
            System.sleep(22);
            SCE03025.this.shion.face.mtn(7, 0, 120, 2, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(4);
            SCE03025.this.shion.face.mtn(7, 4, 4, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(20);
            SCE03025.this.shion.face.mtn(8, 0, 120, 2, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(51);
            SCE03025.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(27);
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
        }

        public void face12_allen() {
            SCE03025.this.allen.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(50);
            SCE03025.this.allen.face.mtn(7, 0, 3, 5, 0, 0.3f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(45);
            SCE03025.this.allen.face.mtn(7, 0, 3, 5, 0, -0.6f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(73);
            SCE03025.this.allen.face.mtn(7, 0, 3, 5, 0, 0.3f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(30);
            System.sleep(30);
            SCE03025.this.allen.face.mtn(7, 0, 3, 5, 0, -0.3f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(60);
            SCE03025.this.allen.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
        }

        public void face12_chaos() {
            System.sleep(50);
            SCE03025.this.chaos.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.chaos.face.start(4, null);
        }

        public void face12_jr() {
            System.sleep(60);
            SCE03025.this.jr.face.mtn(7, 0, 120, 2, 9, 0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(7);
            SCE03025.this.jr.face.mtn(7, 2, 2, 5, 0, 0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(60);
            SCE03025.this.jr.face.mtn(7, 0, 2, 5, 0, -0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(75);
            SCE03025.this.jr.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(45);
            SCE03025.this.jr.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(12);
            SCE03025.this.jr.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(30);
        }

        public void face12_shion() {
            System.sleep(10);
            System.sleep(5);
            SCE03025.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(24);
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(15);
            SCE03025.this.shion.face.mtn(7, 22, 52, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(30);
            SCE03025.this.shion.face.mtn(8, 52, 52, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(27);
            SCE03025.this.shion.face.mtn(7, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(51);
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(30);
            System.sleep(6);
            SCE03025.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(18);
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(51);
            SCE03025.this.shion.face.mtn(7, 40, 52, 5, 0, 0.8f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(24);
            SCE03025.this.shion.face.mtn(8, 52, 52, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(8);
        }

        public void face14_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 0, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face14b_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 0, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(10);
            SCE03025.this.feb.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(20);
            SCE03025.this.feb.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(25);
            SCE03025.this.feb.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(28);
            SCE03025.this.feb.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(22);
            SCE03025.this.feb.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(10);
        }

        public void face15_shion() {
            SCE03025.this.shion.face.mtn(8, 52, 52, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(60);
            SCE03025.this.shion.face.mtn(8, 52, 120, 5, 0, 0.25f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(45);
        }

        public void face16_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 0, 8, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face18_elly() {
            SCE03025.this.elly.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
        }

        public void face19_elly() {
            System.sleep(16);
            SCE03025.this.elly.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(119);
            SCE03025.this.elly.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(5);
            SCE03025.this.elly.face.mtn(6, 10, 10, 5, 0, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(2);
        }

        public void face20_elly() {
            SCE03025.this.elly.face.mtn(6, 10, 120, 5, 0, 0.5f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(15);
            SCE03025.this.elly.face.mtn(1, 0, 120, 8, 9, 0.8f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(45);
            SCE03025.this.elly.face.mtn(2, 0, 120, 5, 9, 0.8f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(9);
            SCE03025.this.elly.face.mtn(5, 0, 120, 8, 9, 0.8f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(24);
            SCE03025.this.elly.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(3);
            System.sleep(4);
            SCE03025.this.elly.face.mtn(6, 10, 120, 5, 0, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
            System.sleep(5);
        }

        public void face21_shion() {
            SCE03025.this.shion.face.mtn(8, 52, 52, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
        }

        public void face21_shionM() {
            SCE03025.this.shionM.face.mtn(8, 0, 120, 5, 8, 1.0f, false);
            SCE03025.this.shionM.face.start(4, null);
            System.sleep(131);
            SCE03025.this.shionM.face.mtn(8, 45, 56, 5, 0, 0.2f, false);
            SCE03025.this.shionM.face.start(4, null);
            System.sleep(90);
            SCE03025.this.shionM.face.mtn(8, 52, 56, 5, 0, -0.2f, false);
            SCE03025.this.shionM.face.start(4, null);
            System.sleep(30);
        }

        public void face22_shion() {
            System.sleep(30);
            SCE03025.this.shion.face.mtn(8, 52, 120, 5, 0, 0.15f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(55);
            SCE03025.this.shion.face.mtn(8, 50, 120, 5, 0, 0.25f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(35);
        }

        public void face22_ziggy() {
            SCE03025.this.ziggy.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
            System.sleep(120);
        }

        public void face24_feb() {
            System.sleep(10);
            SCE03025.this.feb.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(20);
        }

        public void face25_shion() {
            SCE03025.this.shion.face.mtn(7, 0, 2, 5, 0, 0.15f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(25);
            SCE03025.this.shion.face.mtn(7, 0, 2, 5, 0, -0.15f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(30);
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(15);
            SCE03025.this.shion.face.mtn(8, 50, 60, 5, 0, 0.35f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(30);
        }

        public void face2_allen() {
            SCE03025.this.allen.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
        }

        public void face2_chaos() {
            SCE03025.this.chaos.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.chaos.face.start(4, null);
        }

        public void face2_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face2_jr() {
            SCE03025.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
        }

        public void face2_momo() {
            SCE03025.this.momo.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
        }

        public void face2_shion() {
            SCE03025.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
        }

        public void face2_ziggy() {
            SCE03025.this.ziggy.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
        }

        public void face3_allen() {
            SCE03025.this.allen.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(42);
            SCE03025.this.allen.face.mtn(1, 0, 15, 5, 0, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(15);
            System.sleep(20);
            SCE03025.this.allen.face.mtn(2, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(35);
            SCE03025.this.allen.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(20);
            SCE03025.this.allen.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(15);
            SCE03025.this.allen.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(33);
            SCE03025.this.allen.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
        }

        public void face3b_momo() {
            SCE03025.this.momo.face.mtn(2, 40, 120, 5, 0, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(10);
            SCE03025.this.momo.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(20);
            SCE03025.this.momo.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(15);
            System.sleep(10);
            SCE03025.this.momo.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(14);
            SCE03025.this.momo.face.mtn(1, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(21);
            SCE03025.this.momo.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(9);
            SCE03025.this.momo.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
            System.sleep(45);
            SCE03025.this.momo.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
        }

        public void face4_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face5_allen() {
            System.sleep(20);
            SCE03025.this.allen.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(99);
            SCE03025.this.allen.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(69);
            SCE03025.this.allen.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(18);
            SCE03025.this.allen.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(14);
            System.sleep(45);
            SCE03025.this.allen.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
            System.sleep(60);
        }

        public void face5_chaos() {
            SCE03025.this.chaos.face.mtn(12, 0, 60, 10, 9, 1.0f, false);
            SCE03025.this.chaos.face.start(4, null);
        }

        public void face5_jr() {
            SCE03025.this.jr.face.mtn(12, 0, 60, 10, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
        }

        public void face5_shion() {
            System.sleep(30);
            SCE03025.this.shion.face.mtn(8, 0, 120, 10, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(60);
            SCE03025.this.shion.face.mtn(7, 0, 3, 5, 0, 0.6f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(45);
            SCE03025.this.shion.face.mtn(7, 0, 3, 5, 0, -0.7f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(30);
            SCE03025.this.shion.face.mtn(7, 0, 2, 5, 0, 0.6f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(45);
        }

        public void face5_ziggy() {
            System.sleep(20);
            System.sleep(200);
            System.sleep(50);
            SCE03025.this.ziggy.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
            System.sleep(21);
            SCE03025.this.ziggy.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
            System.sleep(36);
            SCE03025.this.ziggy.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
            System.sleep(36);
            SCE03025.this.ziggy.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
            System.sleep(12);
        }

        public void face6_feb() {
            System.sleep(10);
            SCE03025.this.feb.face.mtn(3, 0, 120, 10, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(15);
            SCE03025.this.feb.face.mtn(4, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(45);
            SCE03025.this.feb.face.mtn(3, 0, 120, 10, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(45);
            SCE03025.this.feb.face.mtn(4, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(10);
            System.sleep(25);
            SCE03025.this.feb.face.mtn(3, 0, 120, 10, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(69);
            System.sleep(5);
            SCE03025.this.feb.face.mtn(4, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(22);
            SCE03025.this.feb.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(72);
            SCE03025.this.feb.face.mtn(4, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(12);
        }

        public void face7a_shion() {
            SCE03025.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(8);
            System.sleep(10);
            SCE03025.this.shion.face.mtn(7, 0, 120, 5, 0, 0.73f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(45);
            SCE03025.this.shion.face.mtn(8, 0, 120, 16, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(10);
            System.sleep(20);
            System.sleep(35);
            SCE03025.this.shion.face.mtn(7, 0, 2, 5, 0, 0.4f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(10);
            System.sleep(15);
            SCE03025.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(40);
            System.sleep(5);
        }

        public void face8_chaos() {
            System.sleep(50);
            SCE03025.this.chaos.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.chaos.face.start(4, null);
        }

        public void face8_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 0, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face8_jr() {
            System.sleep(30);
            SCE03025.this.jr.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(35);
            SCE03025.this.jr.face.mtn(8, 0, 3, 5, 0, 0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(45);
            SCE03025.this.jr.face.mtn(8, 0, 3, 5, 0, -0.3f, false);
            SCE03025.this.jr.face.start(4, null);
            System.sleep(15);
        }

        public void face8_shion() {
            SCE03025.this.shion.face.mtn(7, 45, 53, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(45);
            SCE03025.this.shion.face.mtn(8, 0, 120, 20, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(40);
            SCE03025.this.shion.face.mtn(7, 0, 3, 10, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(20);
            SCE03025.this.shion.face.mtn(8, 0, 120, 10, 9, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
            System.sleep(15);
        }

        public void face9_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 120, 0, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(10);
            SCE03025.this.feb.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(15);
            System.sleep(30);
            SCE03025.this.feb.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
            System.sleep(10);
        }

        public void face_bank_feb() {
            SCE03025.this.feb.face.mtn(7, 5, 5, 5, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face_stop_allen() {
            SCE03025.this.allen.face.mtn(2, 0, 0, 5, 9, 1.0f, false);
            SCE03025.this.allen.face.start(4, null);
        }

        public void face_stop_chaos() {
            SCE03025.this.chaos.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.chaos.face.start(4, null);
        }

        public void face_stop_elly() {
            SCE03025.this.elly.face.mtn(2, 0, 120, 5, 0, 1.0f, false);
            SCE03025.this.elly.face.start(4, null);
        }

        public void face_stop_feb() {
            SCE03025.this.feb.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.feb.face.start(4, null);
        }

        public void face_stop_jr() {
            SCE03025.this.jr.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.jr.face.start(4, null);
        }

        public void face_stop_momo() {
            SCE03025.this.momo.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.momo.face.start(4, null);
        }

        public void face_stop_shion() {
            SCE03025.this.shion.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.shion.face.start(4, null);
        }

        public void face_stop_ziggy() {
            SCE03025.this.ziggy.face.mtn(2, 0, 0, 5, 0, 1.0f, false);
            SCE03025.this.ziggy.face.start(4, null);
        }
    }

    class allUNIT
            extends Unit {
        public allUNIT(int n) {
            this.init(n, this.px, this.py, this.pz, this.ry);
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        public allUNIT(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void act0_targetL1_reset() {
        }

        void act0_targetL2_reset() {
        }

        void act0_targetR1_reset() {
        }

        void act11_stained_set() {
            SCE03025.this.Rstained1.setTranslate(10.55f, 7.04f, -4.18f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained3.setTranslate(10.79f, 6.58f, -4.43f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 24.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 0.25f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Rstained2.setTranslate(7.21f, 4.78f, 1.15f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 21.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.2f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Lstained1.setTranslate(-11.46f, 6.99f, -4.68f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 11.5f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.8f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-11.04f, 6.29f, -4.62f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 22.5f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.23f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act12_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.setTranslate(8.67f, 5.73f, 1.31f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 24.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.25f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(10.06f, 6.66f, 3.57f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.setTranslate(-6.99f, 4.37f, 0.98f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 22.5f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.23f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.setTranslate(-9.07f, 6.0f, 3.65f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 11.5f);
            SCE03025.this.Lstained3.setScale(-1.0f, -0.8f, -1.0f);
            SCE03025.this.Lstained3.disp(false);
        }

        void act14B_light() {
            SCE03025.this.candle.disp(false);
        }

        void act14_stained_set() {
            SCE03025.this.Rstained1.setTranslate(9.5f, 6.3f, -3.87f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.setTranslate(11.6f, 6.58f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-10.49f, 6.75f, -4.43f);
            SCE03025.this.Lstained1.setRotate(-181.0f, 0.0f, 14.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-10.7f, 7.15f, 0.61f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 16.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act15_stained_set() {
            SCE03025.this.Rstained1.setTranslate(6.73f, 4.74f, 1.02f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 20.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.35f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.setTranslate(7.7f, 5.19f, 1.11f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(10.3f, 6.2f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.setTranslate(-9.0f, 6.03f, 0.61f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 16.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.setTranslate(-10.19f, 6.17f, 3.52f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 0.0f);
            SCE03025.this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained3.disp(true);
        }

        void act19_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained3.disp(false);
            SCE03025.this.Lstained2.setTranslate(-11.29f, 7.22f, -0.51f);
            SCE03025.this.Lstained2.setRotate(-181.0f, 0.0f, 14.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained1.setTranslate(-10.89f, 6.03f, 0.29f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 10.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
        }

        void act1_targetL1() {
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Lstained1.px, SCE03025.this.Lstained1.py, SCE03025.this.Lstained1.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Lstained1.rx, SCE03025.this.Lstained1.ry, SCE03025.this.Lstained1.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Lstained1.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Lstained1.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act1_targetL2() {
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Lstained2.px, SCE03025.this.Lstained2.py, SCE03025.this.Lstained2.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Lstained2.rx, SCE03025.this.Lstained2.ry, SCE03025.this.Lstained2.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Lstained2.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Lstained2.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act1_targetL3() {
            SCE03025.this.Lstained3.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Lstained3.px, SCE03025.this.Lstained3.py, SCE03025.this.Lstained3.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Lstained3.rx, SCE03025.this.Lstained3.ry, SCE03025.this.Lstained3.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Lstained3.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Lstained3.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act1_targetR1() {
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Rstained1.px, SCE03025.this.Rstained1.py, SCE03025.this.Rstained1.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Rstained1.rx, SCE03025.this.Rstained1.ry, SCE03025.this.Rstained1.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Rstained1.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Rstained1.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act1_targetR2() {
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Rstained2.px, SCE03025.this.Rstained2.py, SCE03025.this.Rstained2.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Rstained2.rx, SCE03025.this.Rstained2.ry, SCE03025.this.Rstained2.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Rstained2.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Rstained2.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act1_targetR3() {
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.target.setTranslate(SCE03025.this.Rstained3.px, SCE03025.this.Rstained3.py, SCE03025.this.Rstained3.pz);
            SCE03025.this.target.setRotate(SCE03025.this.Rstained3.rx, SCE03025.this.Rstained3.ry, SCE03025.this.Rstained3.rz);
            SCE03025.this.target.setVisible(true);
            while (true) {
                SCE03025.this.Rstained3.setTranslate(SCE03025.this.target.px, SCE03025.this.target.py, SCE03025.this.target.pz);
                SCE03025.this.Rstained3.setRotate(SCE03025.this.target.rx, SCE03025.this.target.ry, SCE03025.this.target.rz);
                System.sleep(1);
            }
        }

        void act20_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained1.setTranslate(-9.44f, 9.36f, -0.48f);
            SCE03025.this.Lstained1.setRotate(-181.0f, 0.0f, 43.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.35f, -0.8f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-10.78f, 8.76f, -0.11f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 32.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act21_light() {
            SCE03025.this.R_light1.setTranslate(5.95f, 5.3f, 1.55f);
            SCE03025.this.R_light1.setRotate(0.0f, 0.0f, 0.0f);
            SCE03025.this.R_light1.setScale(0.5f, 0.5f, 0.5f);
            SCE03025.this.R_light1.disp(true);
            SCE03025.this.R_light2.setTranslate(5.95f, 5.3f, 5.55f);
            SCE03025.this.R_light2.setRotate(0.0f, 0.0f, 0.0f);
            SCE03025.this.R_light2.setScale(0.5f, 0.5f, 0.5f);
            SCE03025.this.R_light2.disp(true);
        }

        void act21_stained_set() {
            SCE03025.this.Rstained2.setTranslate(8.3f, 5.7f, -0.49f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(10.9f, 6.7f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Rstained1.setTranslate(12.17f, 7.24f, 7.52f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.setTranslate(-11.05f, 5.94f, 0.21f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 16.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act22_stained_set() {
            SCE03025.this.Rstained2.setTranslate(10.03f, 6.32f, -1.76f);
            SCE03025.this.Rstained2.setRotate(0.0f, 21.0f, 0.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(8.7f, 5.71f, 3.45f);
            SCE03025.this.Rstained3.setRotate(0.0f, 5.0f, 4.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Rstained1.setTranslate(10.27f, 6.21f, 7.52f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.3f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.disp(false);
        }

        void act25_stained_set() {
            SCE03025.this.Rstained2.setTranslate(8.98f, 5.59f, -1.22f);
            SCE03025.this.Rstained2.setRotate(0.0f, 2.0f, 4.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(12.2f, 7.25f, 3.53f);
            SCE03025.this.Rstained3.setRotate(0.0f, -0.0f, 4.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Rstained1.setTranslate(14.47f, 8.01f, 7.52f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.disp(false);
        }

        void act26_stained_set() {
            SCE03025.this.Rstained1.setTranslate(8.8f, 6.18f, -4.42f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 5.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.setTranslate(7.67f, 5.75f, -2.1f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(7.47f, 5.44f, -2.07f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 23.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 0.2f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-10.64f, 5.63f, -2.29f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 21.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.25f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-10.88f, 6.17f, -2.77f);
            SCE03025.this.Lstained2.setRotate(-181.0f, 0.0f, 14.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act2_stained_set() {
            SCE03025.this.Rstained1.setTranslate(10.1f, 6.62f, -4.49f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 13.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.setTranslate(8.85f, 5.92f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-11.2f, 7.15f, -4.49f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 13.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.disp(false);
        }

        void act3_light() {
            SCE03025.this.F_light1.disp(false);
            SCE03025.this.F_light2.disp(false);
            SCE03025.this.R_light1.disp(false);
            SCE03025.this.R_light2.disp(false);
            SCE03025.this.L_light1.disp(false);
            SCE03025.this.L_light2.disp(false);
        }

        void act3_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.setTranslate(10.18f, 6.68f, -0.49f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 13.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(8.85f, 5.92f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 4.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.setTranslate(-9.59f, 6.27f, -0.49f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 13.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.7f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act3b_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained2.setTranslate(-9.7f, 6.57f, 0.84f);
            SCE03025.this.Lstained2.setRotate(-180.0f, -0.0f, 12.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.setTranslate(-11.3f, 7.25f, 3.52f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 9.0f);
            SCE03025.this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-9.76f, 7.25f, 7.52f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 13.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
        }

        void act4_stained_set() {
            SCE03025.this.Rstained1.setTranslate(10.52f, 5.88f, -3.94f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 19.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.3f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.setTranslate(9.3f, 5.88f, -2.79f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 13.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained1.setTranslate(-10.28f, 6.6f, -4.4f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 17.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.65f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.disp(false);
        }

        void act5_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.setTranslate(6.56f, 3.93f, 1.15f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 12.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.setTranslate(-11.08f, 6.48f, 1.11f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 0.0f);
            SCE03025.this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-13.89f, 7.5f, 7.52f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 0.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
        }

        void act6_stained_set() {
            SCE03025.this.Rstained1.setTranslate(8.64f, 5.74f, -4.18f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 14.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.7f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.setTranslate(8.81f, 6.0f, -4.43f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 24.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.25f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained1.setTranslate(-9.83f, 6.2f, -4.63f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 11.5f);
            SCE03025.this.Lstained1.setScale(-1.0f, -0.8f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-10.18f, 6.29f, -4.61f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 22.5f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.23f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.disp(false);
        }

        void act7_stained_set() {
            SCE03025.this.Rstained2.setTranslate(9.23f, 5.04f, 1.23f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 15.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.3f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(11.1f, 6.12f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 0.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Rstained1.setTranslate(12.0f, 6.65f, 7.52f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 0.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Lstained2.setTranslate(-9.82f, 5.63f, 0.88f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 17.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.3f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.setTranslate(-10.7f, 6.3f, 3.52f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 0.0f);
            SCE03025.this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained3.disp(true);
            SCE03025.this.Lstained1.setTranslate(-11.4f, 6.65f, 7.52f);
            SCE03025.this.Lstained1.setRotate(-180.0f, 0.0f, 0.0f);
            SCE03025.this.Lstained1.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained1.disp(true);
        }

        void act8_stained_set() {
            SCE03025.this.Rstained1.setTranslate(12.0f, 6.65f, 7.52f);
            SCE03025.this.Rstained1.setRotate(0.0f, 0.0f, 0.0f);
            SCE03025.this.Rstained1.setScale(1.0f, 0.3f, 1.0f);
            SCE03025.this.Rstained1.disp(true);
            SCE03025.this.Rstained2.setTranslate(7.42f, 4.93f, 1.23f);
            SCE03025.this.Rstained2.setRotate(0.0f, 0.0f, 23.0f);
            SCE03025.this.Rstained2.setScale(1.0f, 0.3f, 1.0f);
            SCE03025.this.Rstained2.disp(true);
            SCE03025.this.Rstained3.setTranslate(10.5f, 6.27f, 3.52f);
            SCE03025.this.Rstained3.setRotate(0.0f, 0.0f, 6.0f);
            SCE03025.this.Rstained3.setScale(1.0f, 1.0f, 1.0f);
            SCE03025.this.Rstained3.disp(true);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.setTranslate(-7.89f, 5.74f, 0.88f);
            SCE03025.this.Lstained2.setRotate(-180.0f, 0.0f, 26.0f);
            SCE03025.this.Lstained2.setScale(-1.0f, -0.3f, -1.0f);
            SCE03025.this.Lstained2.disp(true);
            SCE03025.this.Lstained3.setTranslate(-9.38f, 6.25f, 3.52f);
            SCE03025.this.Lstained3.setRotate(-180.0f, 0.0f, 9.0f);
            SCE03025.this.Lstained3.setScale(-1.0f, -1.0f, -1.0f);
            SCE03025.this.Lstained3.disp(true);
        }

        void act_bank_stained_set() {
            SCE03025.this.Rstained1.disp(false);
            SCE03025.this.Rstained2.disp(false);
            SCE03025.this.Rstained3.disp(false);
            SCE03025.this.Lstained1.disp(false);
            SCE03025.this.Lstained2.disp(false);
            SCE03025.this.Lstained3.disp(false);
        }

        public void fog_check() {
            SCE03025.this.fog.setTranslate(0.0f, 0.0f, 0.0f);
            SCE03025.this.fog.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                SCE03025.this.cam1.setFog(1, SCE03025.this.fog.px, SCE03025.this.fog.pz, SCE03025.this.fog.py, SCE03025.this.fog.ry / 1000.0f, 128, 128, 128, 0);
                System.sleep(1);
            }
        }

        public void point_check0() {
            while (true) {
                SCE03025.this.light1.setGlobalPointLightCol(0, SCE03025.this.point0.rx / 100.0f, SCE03025.this.point0.ry / 100.0f, SCE03025.this.point0.rz / 100.0f);
                SCE03025.this.light1.setGlobalPointLightPos(0, SCE03025.this.point0.px, SCE03025.this.point0.py, SCE03025.this.point0.pz);
                System.sleep(1);
            }
        }

        public void point_check1() {
            while (true) {
                SCE03025.this.light1.setGlobalPointLightCol(1, SCE03025.this.point1.rx / 100.0f, SCE03025.this.point1.ry / 100.0f, SCE03025.this.point1.rz / 100.0f);
                SCE03025.this.light1.setGlobalPointLightPos(1, SCE03025.this.point1.px, SCE03025.this.point1.py, SCE03025.this.point1.pz);
                System.sleep(1);
            }
        }

        public void point_check2() {
            while (true) {
                SCE03025.this.light1.setGlobalPointLightCol(2, SCE03025.this.point2.rx / 100.0f, SCE03025.this.point2.ry / 100.0f, SCE03025.this.point2.rz / 100.0f);
                SCE03025.this.light1.setGlobalPointLightPos(2, SCE03025.this.point2.px, SCE03025.this.point2.py, SCE03025.this.point2.pz);
                System.sleep(1);
            }
        }

        public void point_reset() {
        }

        public void reset_pos() {
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }
    }

    class allMAP
            extends MAPUnit {
        public allMAP(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(true);
            this.start(4, null);
        }

        void act14_map() {
            Stage.setVisible(43, false);
            Stage.setVisible(44, false);
            Stage.setVisible(45, false);
            Stage.setVisible(46, false);
            Stage.setVisible(47, false);
            Stage.setVisible(48, false);
            Stage.setVisible(49, false);
            Stage.setVisible(50, false);
            Stage.setVisible(51, false);
            Stage.setVisible(52, false);
            Stage.setVisible(53, false);
            Stage.setVisible(54, false);
            Stage.setVisible(55, false);
            Stage.setVisible(56, false);
            Stage.setVisible(57, false);
            Stage.setVisible(58, false);
            Stage.setVisible(59, false);
            Stage.setVisible(60, false);
            Stage.setVisible(61, false);
            Stage.setVisible(62, false);
            Stage.setVisible(63, false);
            Stage.setVisible(64, false);
            Stage.setVisible(65, false);
            Stage.setVisible(66, false);
            Stage.setVisible(67, false);
            Stage.setVisible(78, false);
            Stage.setVisible(79, false);
            Stage.setVisible(80, false);
            Stage.setVisible(81, false);
            Stage.setVisible(82, false);
            Stage.setVisible(83, false);
            Stage.setVisible(84, false);
            Stage.setVisible(85, false);
            Stage.setVisible(86, false);
            Stage.setVisible(87, false);
            Stage.setVisible(88, false);
            Stage.setVisible(89, false);
            Stage.setVisible(90, false);
            Stage.setVisible(91, false);
            Stage.setVisible(92, false);
            Stage.setVisible(93, false);
            Stage.setVisible(94, false);
            Stage.setVisible(95, false);
            Stage.setVisible(96, false);
            Stage.setVisible(97, false);
            Stage.setVisible(98, false);
            Stage.setVisible(99, false);
            Stage.setVisible(100, false);
            Stage.setVisible(101, false);
            Stage.setVisible(102, false);
            Stage.setVisible(103, false);
            Stage.setVisible(104, false);
            Stage.setVisible(105, false);
            Stage.setVisible(106, false);
            Stage.setVisible(107, false);
            Stage.setVisible(108, false);
            Stage.setVisible(109, false);
            Stage.setVisible(110, false);
            Stage.setVisible(111, false);
            Stage.setVisible(112, false);
            Stage.setVisible(113, false);
            Stage.setVisible(114, false);
            Stage.setVisible(115, false);
            Stage.setVisible(116, false);
            Stage.setVisible(117, false);
            Stage.setVisible(118, false);
            Stage.setVisible(119, false);
            Stage.setVisible(120, false);
            Stage.setVisible(121, false);
            Stage.setVisible(122, false);
            Stage.setVisible(123, false);
            Stage.setVisible(124, false);
            Stage.setVisible(125, false);
            Stage.setVisible(126, false);
            Stage.setVisible(127, false);
            Stage.setVisible(128, false);
            Stage.setVisible(129, false);
            Stage.setVisible(130, false);
            Stage.setVisible(131, false);
            Stage.setVisible(132, false);
            Stage.setVisible(133, false);
            Stage.setVisible(134, false);
            Stage.setVisible(135, false);
            Stage.setVisible(136, false);
            Stage.setVisible(137, false);
            Stage.setVisible(138, false);
            Stage.setVisible(139, false);
            Stage.setVisible(140, false);
            Stage.setVisible(141, false);
            Stage.setVisible(142, false);
            Stage.setVisible(143, false);
            Stage.setVisible(144, false);
            Stage.setVisible(145, false);
            Stage.setVisible(146, false);
            Stage.setVisible(147, false);
            Stage.setVisible(148, false);
            Stage.setVisible(149, false);
            Stage.setVisible(150, false);
            Stage.setVisible(151, false);
            Stage.setVisible(152, false);
            Stage.setVisible(153, false);
            Stage.setVisible(154, false);
            Stage.setVisible(155, false);
            Stage.setVisible(156, false);
            Stage.setVisible(157, false);
            Stage.setVisible(158, false);
            Stage.setVisible(159, false);
            Stage.setVisible(160, false);
            Stage.setVisible(161, false);
            Stage.setVisible(162, false);
            Stage.setVisible(163, false);
            Stage.setVisible(164, false);
            Stage.setVisible(165, false);
            Stage.setVisible(166, false);
            Stage.setVisible(167, false);
            Stage.setVisible(168, false);
            Stage.setVisible(169, false);
            Stage.setVisible(170, false);
            Stage.setVisible(171, false);
            Stage.setVisible(172, false);
            Stage.setVisible(173, false);
            Stage.setVisible(174, false);
            Stage.setVisible(175, false);
            Stage.setVisible(176, false);
            Stage.setVisible(177, false);
            Stage.setVisible(178, false);
            Stage.setVisible(179, false);
            Stage.setVisible(180, false);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0() {
            SCE03025.this.light.setColor(0, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE03025.this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
            SCE03025.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE03025.this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
            SCE03025.this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            SCE03025.this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut10() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.95f, 0.95f, 0.95f);
            SCE03025.this.light.setDirection2(1, 0.987f, 0.015f, 0.16f);
            SCE03025.this.light.setColor(2, 0.44f, 0.44f, 0.44f);
            SCE03025.this.light.setDirection2(2, -0.675f, 0.132f, -0.726f);
            SCE03025.this.light.setColor(3, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setDirection2(3, -0.147f, -0.905f, -0.399f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut11() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.53f, 0.53f, 0.53f);
            SCE03025.this.light.setDirection2(1, 0.94f, 0.338f, 0.043f);
            SCE03025.this.light.setColor(2, 0.38f, 0.38f, 0.38f);
            SCE03025.this.light.setDirection2(2, 0.014f, -0.341f, 0.94f);
            SCE03025.this.light.setColor(3, 0.68f, 0.68f, 0.68f);
            SCE03025.this.light.setDirection2(3, -0.776f, 0.39f, -0.496f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightCol(0, 0.26f, 0.2f, 0.13f);
            SCE03025.this.light1.setGlobalPointLightPos(0, 0.31f, 2.32f, -6.01f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut12() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE03025.this.light.setDirection2(1, 0.978f, 0.098f, 0.184f);
            SCE03025.this.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03025.this.light.setDirection2(2, -0.458f, 0.466f, -0.757f);
            SCE03025.this.light.setColor(3, 0.33f, 0.33f, 0.33f);
            SCE03025.this.light.setDirection2(3, -0.496f, -0.709f, -0.502f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut14() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE03025.this.light.setDirection2(1, 0.978f, 0.098f, 0.184f);
            SCE03025.this.light.setColor(2, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setDirection2(2, -0.458f, 0.466f, -0.757f);
            SCE03025.this.light.setColor(3, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setDirection2(3, -0.496f, -0.709f, -0.502f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut14b() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(1, 0.908f, 0.418f, -0.027f);
            SCE03025.this.light.setColor(2, 0.42f, 0.42f, 0.42f);
            SCE03025.this.light.setDirection2(2, 0.097f, -0.419f, 0.903f);
            SCE03025.this.light.setColor(3, 0.68f, 0.68f, 0.68f);
            SCE03025.this.light.setDirection2(3, -0.843f, 0.031f, -0.537f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut15() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE03025.this.light.setDirection2(1, 0.93f, 0.034f, 0.366f);
            SCE03025.this.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE03025.this.light.setDirection2(2, -0.241f, 0.143f, -0.96f);
            SCE03025.this.light.setColor(3, 0.32f, 0.32f, 0.32f);
            SCE03025.this.light.setDirection2(3, -0.026f, -0.827f, -0.562f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut16() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03025.this.light.setDirection2(1, 0.929f, 0.279f, -0.243f);
            SCE03025.this.light.setColor(2, 0.28f, 0.28f, 0.28f);
            SCE03025.this.light.setDirection2(2, -0.211f, -0.541f, 0.814f);
            SCE03025.this.light.setColor(3, 0.38f, 0.38f, 0.38f);
            SCE03025.this.light.setDirection2(3, -0.72f, 0.467f, -0.513f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            SCE03025.this.light1.setGlobalPointLightCol(1, 0.27f, 0.43f, 0.33f);
            SCE03025.this.light1.setGlobalPointLightPos(1, -4.56f, 4.46f, -9.73f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut18() {
            SCE03025.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut19() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.72f, 0.72f, 0.72f);
            SCE03025.this.light.setDirection2(1, 0.934f, 0.281f, 0.221f);
            SCE03025.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE03025.this.light.setDirection2(2, -0.718f, 0.008f, -0.696f);
            SCE03025.this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03025.this.light.setDirection2(3, 0.692f, -0.705f, -0.154f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.elly.setLightMode(1);
            SCE03025.this.elly.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.elly.light.setColor(1, 0.49f, 0.49f, 0.49f);
            SCE03025.this.elly.light.setDirection2(1, -0.346f, 0.021f, -0.938f);
            SCE03025.this.elly.light.setColor(2, 0.35f, 0.35f, 0.35f);
            SCE03025.this.elly.light.setDirection2(2, 0.916f, -0.231f, 0.328f);
            SCE03025.this.elly.light.setColor(3, 0.9f, 0.9f, 0.9f);
            SCE03025.this.elly.light.setDirection2(3, -0.664f, 0.568f, 0.486f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut2() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE03025.this.light.setDirection2(1, 0.929f, 0.279f, -0.243f);
            SCE03025.this.light.setColor(2, 0.57f, 0.57f, 0.57f);
            SCE03025.this.light.setDirection2(2, 0.097f, -0.419f, 0.903f);
            SCE03025.this.light.setColor(3, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(3, -0.794f, -0.033f, -0.607f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightCol(0, 0.29f, 0.28f, 0.22f);
            SCE03025.this.light1.setGlobalPointLightPos(0, 2.54f, 5.95f, -9.81f);
            SCE03025.this.light1.setGlobalPointLightCol(1, 0.29f, 0.28f, 0.22f);
            SCE03025.this.light1.setGlobalPointLightPos(1, -2.44f, 5.95f, -9.81f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut20() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.elly.setLightMode(0);
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.54f, 0.54f, 0.54f);
            SCE03025.this.light.setDirection2(1, -0.346f, 0.021f, -0.938f);
            SCE03025.this.light.setColor(2, 0.43f, 0.43f, 0.43f);
            SCE03025.this.light.setDirection2(2, 0.93f, -0.135f, 0.343f);
            SCE03025.this.light.setColor(3, 1.0f, 1.0f, 1.0f);
            SCE03025.this.light.setDirection2(3, -0.664f, 0.568f, 0.486f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut21() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.85f, 0.85f, 0.85f);
            SCE03025.this.light.setDirection2(1, 0.582f, 0.018f, -0.813f);
            SCE03025.this.light.setColor(2, 0.35f, 0.35f, 0.35f);
            SCE03025.this.light.setDirection2(2, -0.714f, -0.68f, -0.167f);
            SCE03025.this.light.setColor(3, 0.4f, 0.4f, 0.4f);
            SCE03025.this.light.setDirection2(3, -0.654f, 0.671f, 0.349f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut22() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE03025.this.light.setDirection2(1, 0.626f, 0.017f, -0.78f);
            SCE03025.this.light.setColor(2, 0.37f, 0.37f, 0.37f);
            SCE03025.this.light.setDirection2(2, -0.782f, -0.612f, 0.116f);
            SCE03025.this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03025.this.light.setDirection2(3, -0.859f, 0.441f, 0.26f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            Runtime.setDefocusQuick(0, 1, 74880, 2);
            Runtime.setDefocusQuick(1, 1, 29880, 1);
            Runtime.setDefocusQuick(2, 1, 21688, 1);
            Runtime.setDefocusQuick(3, 1, 132880, 2);
        }

        public void cut23() {
            SCE03025.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut24() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE03025.this.light.setDirection2(1, 0.833f, 0.292f, -0.47f);
            SCE03025.this.light.setColor(2, 0.26f, 0.26f, 0.26f);
            SCE03025.this.light.setDirection2(2, 0.347f, -0.444f, 0.826f);
            SCE03025.this.light.setColor(3, 0.26f, 0.26f, 0.26f);
            SCE03025.this.light.setDirection2(3, -0.959f, 0.028f, -0.282f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightCol(1, 0.27f, 0.43f, 0.33f);
            SCE03025.this.light1.setGlobalPointLightPos(1, -4.56f, 4.46f, -9.73f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut25() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE03025.this.light.setDirection2(1, 0.873f, 0.197f, -0.446f);
            SCE03025.this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            SCE03025.this.light.setDirection2(2, -0.726f, -0.671f, -0.152f);
            SCE03025.this.light.setColor(3, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setDirection2(3, -0.817f, 0.391f, -0.424f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut26() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE03025.this.light.setDirection2(1, 0.833f, 0.292f, -0.47f);
            SCE03025.this.light.setColor(2, 0.28f, 0.28f, 0.28f);
            SCE03025.this.light.setDirection2(2, 0.347f, -0.444f, 0.826f);
            SCE03025.this.light.setColor(3, 0.25f, 0.25f, 0.25f);
            SCE03025.this.light.setDirection2(3, -0.959f, 0.028f, -0.282f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightCol(1, 0.27f, 0.43f, 0.33f);
            SCE03025.this.light1.setGlobalPointLightPos(1, -4.56f, 4.46f, -9.73f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut3() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE03025.this.light.setDirection2(1, 0.894f, 0.008f, 0.448f);
            SCE03025.this.light.setColor(2, 0.45f, 0.45f, 0.45f);
            SCE03025.this.light.setDirection2(2, -0.773f, 0.37f, -0.515f);
            SCE03025.this.light.setColor(3, 0.35f, 0.35f, 0.35f);
            SCE03025.this.light.setDirection2(3, 0.609f, -0.417f, -0.675f);
            Stage.setColor(0.4f, 0.36f, 0.24f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut3b() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE03025.this.light.setDirection2(1, 0.894f, 0.008f, 0.448f);
            SCE03025.this.light.setColor(2, 0.45f, 0.45f, 0.45f);
            SCE03025.this.light.setDirection2(2, -0.764f, 0.546f, -0.344f);
            SCE03025.this.light.setColor(3, 0.35f, 0.35f, 0.35f);
            SCE03025.this.light.setDirection2(3, 0.491f, -0.39f, -0.779f);
            Stage.setColor(0.4f, 0.36f, 0.24f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut4() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE03025.this.light.setDirection2(1, 0.929f, 0.279f, -0.243f);
            SCE03025.this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            SCE03025.this.light.setDirection2(2, 0.097f, -0.419f, 0.903f);
            SCE03025.this.light.setColor(3, 0.55f, 0.55f, 0.55f);
            SCE03025.this.light.setDirection2(3, -0.741f, 0.389f, -0.547f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightCol(0, 0.26f, 0.2f, 0.13f);
            SCE03025.this.light1.setGlobalPointLightPos(0, 0.31f, 2.32f, -6.01f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut5() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(1, 0.987f, 0.015f, 0.16f);
            SCE03025.this.light.setColor(2, 0.35f, 0.35f, 0.35f);
            SCE03025.this.light.setDirection2(2, -0.755f, 0.265f, -0.6f);
            SCE03025.this.light.setColor(3, 0.26f, 0.26f, 0.26f);
            SCE03025.this.light.setDirection2(3, -0.036f, -0.47f, -0.882f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut6() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.85f, 0.85f, 0.85f);
            SCE03025.this.light.setDirection2(1, 0.998f, 0.005f, -0.063f);
            SCE03025.this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            SCE03025.this.light.setDirection2(2, -0.478f, -0.229f, 0.848f);
            SCE03025.this.light.setColor(3, 0.55f, 0.55f, 0.55f);
            SCE03025.this.light.setDirection2(3, -0.867f, 0.301f, -0.397f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut7a() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(1, 0.987f, 0.015f, 0.16f);
            SCE03025.this.light.setColor(2, 0.43f, 0.43f, 0.43f);
            SCE03025.this.light.setDirection2(2, -0.603f, 0.452f, -0.657f);
            SCE03025.this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            SCE03025.this.light.setDirection2(3, -0.147f, -0.905f, -0.399f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut8() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(1, 0.928f, 0.021f, 0.372f);
            SCE03025.this.light.setColor(2, 0.4f, 0.4f, 0.4f);
            SCE03025.this.light.setDirection2(2, -0.675f, 0.132f, -0.726f);
            SCE03025.this.light.setColor(3, 0.3f, 0.3f, 0.3f);
            SCE03025.this.light.setDirection2(3, -0.106f, -0.636f, -0.764f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            SCE03025.this.light1.setGlobalPointLightReset();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut9() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.8f, 0.8f, 0.8f);
            SCE03025.this.light.setDirection2(1, 0.908f, 0.418f, -0.027f);
            SCE03025.this.light.setColor(2, 0.42f, 0.42f, 0.42f);
            SCE03025.this.light.setDirection2(2, 0.097f, -0.419f, 0.903f);
            SCE03025.this.light.setColor(3, 0.68f, 0.68f, 0.68f);
            SCE03025.this.light.setDirection2(3, -0.843f, 0.031f, -0.537f);
            Stage.setColor(0.35f, 0.31f, 0.19f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
        }

        public void cut_bank() {
            SCE03025.this.Timechk_CutChange();
            SCE03025.this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            SCE03025.this.light.setColor(1, 0.02f, 0.09f, 0.09f);
            SCE03025.this.light.setDirection2(1, -0.151f, 0.984f, -0.091f);
            SCE03025.this.light.setColor(2, 0.14f, 0.14f, 0.14f);
            SCE03025.this.light.setDirection2(2, 0.983f, 0.024f, -0.182f);
            SCE03025.this.light.setColor(3, 0.11f, 0.13f, 0.15f);
            SCE03025.this.light.setDirection2(3, 0.18f, 0.036f, 0.983f);
            Stage.setColor(0.45f, 0.5f, 0.5f);
            SCE03025.this.light1.setGlobalPointLightCol(1, 0.27f, 0.43f, 0.33f);
            SCE03025.this.light1.setGlobalPointLightPos(1, -4.56f, 4.46f, -9.73f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
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

