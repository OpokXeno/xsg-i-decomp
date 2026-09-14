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
import xeno.map.MC_ELS07_PRJ;
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

class SCE02013B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02013B,
        MC_ELS07_PRJ,
        JNT_Human,
        FLSshion_h,
        FLSkosmos_h,
        FLSkosmos,
        FLSandrew_h {
    allCHARA shion;
    allCHARA shion2;
    allCHARA kosmos;
    allCHARA kosmosM;
    allCHARA andrew;
    allCHARA dummy;
    allCHARA shadow;
    Chr kosmos_tub;
    Unit handgun;
    Unit tray1;
    Unit tray2;
    Unit spoon;
    Unit fog;
    Unit target;
    Unit point0;
    Unit point1;
    Unit point2;
    Chr Fshion;
    Chr Fandrew;
    Chr Feat;
    Chr Fkosmos;
    Chr FkosmosM;
    Chr Fshion1;
    Chr Fandrew1;
    MAPUnit door;
    Spline movSPL;
    Spline rotSPL;
    Spline movSPL1;
    Spline rotSPL1;
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
    Thread thread1;
    int __flag01 = 0;
    int __flag02 = 0;
    static final int Chand_R = 20;
    int Twohand = 0;
    int Rhand = 16;
    int Lhand = 32;
    int Open = 0;
    int Close = 1;
    float next_px;
    float next_py;
    float next_pz;
    Unit unit0;
    Unit unit1;
    Unit unit2;
    Effect steam1;
    Effect steam2;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    Thread thread_cameratool;
    Window msg = Window.create(1);
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
    boolean Facechk_exec = true;
    int Facechk_faceno = 1;
    int Facechk_facemode = 1;
    int Facechk_frame = 1;
    int Facechk_mtnno;
    boolean Gnochk_exec;
    float[] gnoFilter = new float[4];
    int gnoptr = 0;
    float gno_alpha = 0.54f;
    float gno_filter = 87.5f;
    float gno_shake = 37.0f;
    float gno_ref = 0.72f;
    boolean FogSet_exec;
    int Fog_R = 128;
    int Fog_G = 128;
    int Fog_B = 128;
    int Fog_A = 64;
    float Fog_Z = 5.0f;
    float Fog_Zwide = 4.0f;
    float Fog_near;
    float Fog_far;
    float Fog_nearRatio = 0.0f;
    float Fog_farRatio = 0.5f;
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
    int Color_mode = 1;
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

    SCE02013B() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 1614815232;
        this.ColorScreenParam = nArray;
        int[] nArray2 = new int[6];
        nArray2[0] = 1;
        nArray2[1] = 0x40808080;
        this.ColorScreenParamBack = nArray2;
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
        this.Objchk(this.Fshion);
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

    void MTNcheck(Chr chr, int n, int n2) {
        int n3 = 0;
        int n4 = 0;
        String string = "０";
        String string2 = "０";
        String string3 = "０";
        int n5 = 0;
        while (n5 < n2) {
            n4 = n5;
            n3 = n4 / 100;
            string = this.MTNcheck_SUB(n3);
            n4 -= n3 * 100;
            n3 = n4 / 10;
            string2 = this.MTNcheck_SUB(n3);
            string3 = this.MTNcheck_SUB(n4 -= n3 * 10);
            chr.mtn(n, n5, n5 + 1, 8, 0, 1.0f, true);
            this.msg.print(String.valueOf(string) + string2 + string3);
            this.waitclear(45);
            ++n5;
        }
    }

    String MTNcheck_SUB(int n) {
        String string = "０";
        if (n == 0) {
            string = "０";
        }
        if (n == 1) {
            string = "１";
        }
        if (n == 2) {
            string = "２";
        }
        if (n == 3) {
            string = "３";
        }
        if (n == 4) {
            string = "４";
        }
        if (n == 5) {
            string = "５";
        }
        if (n == 6) {
            string = "６";
        }
        if (n == 7) {
            string = "７";
        }
        if (n == 8) {
            string = "８";
        }
        if (n == 9) {
            string = "９";
        }
        return string;
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

    void __win_thread() {
        int n = 0;
        boolean bl = false;
        while (true) {
            if (this.__flag01 == 1 && ++n > 689) {
                this.__flag01 = 0;
                n = 0;
            }
            System.sleep(1);
        }
    }

    void adjust0(Chr chr) {
        this.unit0.getTranslate();
        this.unit0.getRotate();
        chr.px = this.unit0.px;
        chr.pz = this.unit0.pz;
        chr.ry = this.unit0.ry;
        chr.setTranslate();
        chr.setRotateY(chr.ry);
    }

    void adjust1(Chr chr) {
        this.unit1.getTranslate();
        this.unit1.getRotate();
        chr.px = this.unit1.px;
        chr.pz = this.unit1.pz;
        chr.ry = this.unit1.ry;
        chr.setTranslate();
        chr.setRotateY(chr.ry);
    }

    void adjust2(Chr chr) {
        this.unit2.getTranslate();
        this.unit2.getRotate();
        chr.px = this.unit2.px;
        chr.pz = this.unit2.pz;
        chr.ry = this.unit2.ry;
        chr.setTranslate();
        chr.setRotateY(chr.ry);
    }

    void changeLocation(int n) {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02013B_F");
        Runtime.setFlags(123, 1, 1);
        System.println("XEVEJNAME:SCE02010");
        Runtime.jumpEvent(2100);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(570, 2);
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
        Runtime.setLocation(106);
        this.shion = new allCHARA(30);
        this.shion2 = new allCHARA(30);
        this.kosmos = new allCHARA(31);
        this.kosmosM = new allCHARA(2);
        this.andrew = new allCHARA(332);
        this.dummy = new allCHARA(24582);
        this.shadow = new allCHARA(24582);
        this.shion2.setVisible(false);
        this.kosmos.setVisible(false);
        this.kosmosM.setVisible(false);
        this.dummy.setVisible(false);
        this.shadow.setVisible(false);
        this.kosmos_tub = new allMECHA(20555, -0.5f, 0.0f, 0.45f, 180.0f);
        this.handgun = new allUNIT(24654);
        this.tray1 = new allUNIT(24640);
        this.tray2 = new allUNIT(24640);
        this.spoon = new allUNIT(24629);
        this.fog = new allUNIT(24582);
        this.target = new allUNIT(24582);
        this.point0 = new allUNIT(24582);
        this.point1 = new allUNIT(24582);
        this.point2 = new allUNIT(24582);
        this.tray2.setVisible(false);
        this.spoon.setVisible(false);
        this.fog.setVisible(false);
        this.target.setVisible(false);
        this.point0.setVisible(false);
        this.point1.setVisible(false);
        this.point2.setVisible(false);
        this.Fshion = new allFACE();
        this.Fkosmos = new allFACE();
        this.FkosmosM = new allFACE();
        this.Fandrew = new allFACE();
        this.Feat = new allFACE();
        this.Fshion1 = new allFACE();
        this.Fandrew1 = new allFACE();
        this.door = new allMAP(9);
        this.movSPL = Spline.create();
        this.rotSPL = Spline.create();
        this.movSPL1 = Spline.create();
        this.rotSPL1 = Spline.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam0.start(3, null);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 1.0f, 2.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.thread1 = Thread.create(this, "__win_thread");
        this.thread1.start();
        this.movSPL = Spline.create();
        this.rotSPL = Spline.create();
        this.movSPL1 = Spline.create();
        this.rotSPL1 = Spline.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam0.start(3, null);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, -10.0f, 0.0f);
        this.cam1.setRotate(-180.0f, 0.0f, 0.0f);
        this.cam1.setFov(40.0f);
        this.thread1 = Thread.create(this, "__win_thread");
        this.thread1.start();
        this.unit0 = new Units();
        this.unit0.init(24582, 0.0f, 0.0f, 0.0f, 0.0f);
        this.unit0.start(4, null);
        this.unit0.setVisible(false);
        this.unit1 = new Units();
        this.unit1.init(24582, 0.0f, 0.0f, 0.0f, 0.0f);
        this.unit1.start(4, null);
        this.unit1.setVisible(false);
        this.unit2 = new Units();
        this.unit2.init(24582, 0.0f, 0.0f, 0.0f, 0.0f);
        this.unit2.start(4, null);
        this.unit2.setVisible(false);
        this.steam1 = new Effect(1531, 0.0f, 0.0f, 0.0f, 0.0f);
        this.steam1.setTranslate(-0.06f, 0.07f, 0.02f);
        this.steam1.setRotate(0.0f, 0.0f, 0.0f);
        this.steam1.setScale(0.4f, 0.4f, 0.4f);
        this.steam1.setCaster(this.tray1);
        this.steam1.disp(true);
        this.steam1.setForceLoop(true);
        this.steam1.noAttach(true);
        this.steam2 = new Effect(1531, 0.0f, 0.0f, 0.0f, 0.0f);
        this.steam2.setTranslate(-0.06f, 0.07f, 0.02f);
        this.steam2.setRotate(0.0f, 0.0f, 0.0f);
        this.steam2.setScale(0.4f, 0.4f, 0.4f);
        this.steam2.setCaster(this.tray1);
        this.steam2.disp(true);
        this.steam2.setForceLoop(true);
        this.steam2.noAttach(false);
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
        int n3 = n;
        while (n3 < n2) {
            Stage.setVisible(n3, false);
            System.sleep(2);
            Stage.setVisible(n3, true);
            System.sleep(2);
            Stage.setVisible(n3, false);
            System.sleep(2);
            Stage.setVisible(n3, true);
            System.sleep(2);
            Stage.setVisible(n3, false);
            this.KEYwait("ＯＫ");
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
        this.loadarc(this.shion2.face, "FLSshion_h.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos_h.fpk");
        this.loadarc(this.kosmosM.face, "FLSkosmos.fpk");
        this.loadarc(this.andrew.face, "FLSandrew_h.fpk");
        Sound.streamPlay(1290081, 48000);
        this.playSCENE1();
        this.playSCENE2();
        this.playSCENE3();
        this.playSCENE4();
        this.Timechk_SceneEnd();
    }

    void playAGAIN() {
        this.KEYwait("配置用");
        Stage.setVisible(0, false);
        this.KEYwait("スタート　０→９");
        this.mapCHECK(0, 10);
        Stage.setVisible(0, false);
        this.KEYwait("スタート　１０→１９");
        this.mapCHECK(10, 20);
        this.SETcheck("シオン", this.shion);
        this.SETcheck("コスモス", this.kosmos);
        this.SETcheck("アンドリュー", this.andrew);
        this.SETcheck("ハンドガン", this.handgun);
        this.SETcheck("ｋｏｓ−ｍｏｓ調整槽", this.kosmos_tub);
        this.SETcheck("ポイント０", this.point0);
        this.point0.setTranslate(4.3f, -0.01f, 3.1f);
        this.point0.setRotate(4.0f, 44.0f, 58.0f);
        this.point0.start(1, "point_check0");
        this.SETcheck("ポイント０　座標", this.point0);
        this.point0.start(1, "point_reset");
        this.SETcheck("ポイント１", this.point1);
        this.light1.setGlobalPointLightCol(1, 0.52f, 0.45f, 0.43f);
        this.light1.setGlobalPointLightPos(1, 1.0f, 2.58f, 8.0f);
        this.point1.setTranslate(1.0f, 2.58f, 8.0f);
        this.point1.setRotate(52.0f, 45.0f, 43.0f);
        this.point1.start(1, "point_check1");
        this.SETcheck("ポイント１　座標", this.point1);
        this.point0.start(1, "point_reset");
        this.SETcheck("ポイント２", this.point2);
        this.light1.setGlobalPointLightCol(2, 0.04f, 0.22f, 0.25f);
        this.light1.setGlobalPointLightPos(2, 4.85f, 1.25f, 5.0f);
        this.point2.setTranslate(0.0f, 0.0f, 0.0f);
        this.point2.setRotate(4.0f, 22.0f, 25.0f);
        this.point2.start(1, "point_check2");
        this.SETcheck("ポイント２　座標", this.point2);
        this.point0.start(1, "point_reset");
    }

    void playSCENE1() {
        Stage.renderCommand(4);
        this.shion.renderCommand(534);
        this.andrew.renderCommand(534);
        this.kosmos.renderCommand(534);
        this.kosmosM.renderCommand(534);
        this.kosmos_tub.renderCommand(534);
        this.andrew.setMotionFlags(0x40000000, true);
        this.shion.setMotionFlags(0x40000000, true);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.kosmosM.setMotionFlags(0x40000000, true);
        this.light1.setGlobalPointLightCol(0, 0.08f, 0.3f, 0.8f);
        this.light1.setGlobalPointLightPos(0, this.kosmos_tub.px, 0.7f, 0.75f);
        this.light1.setGlobalPointLightCol(1, 0.52f, 0.45f, 0.43f);
        this.light1.setGlobalPointLightPos(1, 1.0f, 2.58f, 8.0f);
        this.dummy.start(1, "act1_chara");
        this.shadow.start(1, "act1_shadow");
        this.shion.start(1, "act1_shion");
        this.shion2.start(1, "reset_pos");
        this.andrew.start(1, "act1_andrew");
        this.kosmosM.start(1, "act1_kosmos_middle");
        this.kosmos.start(1, "reset_pos");
        this.kosmos_tub.start(1, "act1_kosmos_tub");
        this.door.start(1, "act1_door");
        this.handgun.start(1, "act1_handgun");
        this.tray1.start(1, "act1_tray");
        this.target.start(1, "act1_steam");
        this.Fshion.start(1, "face_stop_shion");
        this.Fkosmos.start(1, "face1_kosmos");
        this.FkosmosM.start(1, "face1_kosmos_middle");
        this.Fandrew.start(1, "face_stop_andrew");
        this.camerawork.cut1();
        System.sleep(80);
        this.camerawork.cut1_1();
        this.light1.setGlobalPointLightCol(0, 0.06f, 0.1f, 0.2f);
        this.light1.setGlobalPointLightPos(0, this.kosmos_tub.px, 0.7f, 0.75f);
        System.sleep(10);
        System.sleep(5);
        this.waitclear(10);
        System.sleep(40);
        this.andrew.start(1, "act2_andrew");
        this.handgun.start(1, "act2_handgun");
        this.Fandrew.start(1, "face2_andrew");
        this.camerawork.cut2();
        System.sleep(45);
        this.msg_print("【アンドリュー】", "You...");
        System.sleep(25);
        this.waitclear(15);
        System.sleep(15);
        this.shion.setMotionFlags(0x40000000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.light1.setGlobalPointLightCol(1, 0.0f, 0.0f, 0.0f);
        this.light1.setGlobalPointLightPos(1, 1.0f, 2.58f, 8.0f);
        this.shadow.start(1, "act3_shadow");
        this.shion.start(1, "act3_shion");
        this.kosmosM.start(1, "reset_pos");
        this.tray1.start(1, "act3_tray");
        this.target.start(1, "act3_steam");
        this.Fshion.start(1, "face3_shion");
        this.camerawork.cut3();
        this.msg_print("【シオン】", "Oh, umm...\nI didn't see you in your room...");
        System.sleep(54);
        System.sleep(6);
        System.sleep(70);
        this.waitclear(10);
        this.msg.print("I...");
        this.waitclear(25);
        this.light1.setGlobalPointLightCol(1, 0.52f, 0.45f, 0.43f);
        this.light1.setGlobalPointLightPos(1, 1.0f, 2.58f, 8.0f);
        this.dummy.start(1, "act4_chara");
        this.shadow.start(1, "act4_shadow");
        this.shion.start(1, "act4_shion");
        this.andrew.start(1, "act4_andrew");
        this.target.start(1, "act4_steam");
        this.kosmos_tub.start(1, "act0_kosmos_tub_full");
        this.camerawork.cut4();
        this.msg.print("thought you might be\na little hungry...");
        System.sleep(80);
        this.waitclear(10);
        this.waitclear(45);
        System.sleep(40);
        this.msg_print("【シオン】", "What were you doing?");
        System.sleep(42);
        this.waitclear(8);
    }

    void playSCENE2() {
        this.dummy.start(1, "act5_chara");
        this.shadow.start(1, "act5_shadow");
        this.andrew.start(1, "act5_andrew");
        this.handgun.setVisible(false);
        this.kosmosM.start(1, "act1_kosmos_middle");
        this.kosmos_tub.start(1, "act5_kosmos_tub");
        this.Fandrew.start(1, "face5_andrew");
        this.camerawork.cut5();
        System.sleep(30);
        this.msg_print("【アンドリュー】", "...I was looking at her.");
        System.sleep(45);
        this.waitclear(10);
        System.sleep(5);
        this.shadow.start(1, "act6_shadow");
        this.shion.start(1, "act6_shion");
        this.tray1.start(1, "act6_tray");
        this.door.start(1, "act6_door");
        this.handgun.setVisible(true);
        this.target.start(1, "act6_steam");
        this.Fshion.start(1, "face6_shion");
        this.camerawork.cut6();
        System.sleep(20);
        this.msg_print("【シオン】", "At KOS-MOS?\nWith a gun in your hand?");
        System.sleep(50);
        this.waitclear(40);
        System.sleep(15);
        this.andrew.start(1, "act7_andrew");
        this.handgun.start(1, "act7_handgun");
        this.Fandrew.start(1, "face7_andrew");
        this.camerawork.cut7();
        System.sleep(15);
        this.msg_print("【アンドリュー】", "Ah, this, it's...it's a bad habit.");
        System.sleep(66);
        this.waitclear(14);
        System.sleep(40);
        this.shadow.start(1, "act8_shadow");
        this.shion.start(1, "act8_shion");
        this.tray1.start(1, "act8_tray");
        this.Fshion.start(1, "face8_shion");
        this.camerawork.cut6();
        this.msg_print("【シオン】", "I see. Well,\nyou are a soldier, after all.");
        System.sleep(96);
        this.waitclear(9);
        System.sleep(10);
        this.msg.print("I know someone with a\nhabit like that.");
        System.sleep(45);
        this.waitclear(45);
        this.shion.start(1, "act9_shion");
        this.andrew.start(1, "act9_andrew");
        this.kosmosM.start(1, "reset_pos");
        this.shion2.start(1, "act9_shion2");
        this.tray1.start(1, "act9_tray");
        this.kosmos_tub.start(1, "act0_kosmos_tub_full");
        this.Fshion.start(1, "face9_shion");
        this.Fshion1.start(1, "eyes9_shion");
        this.camerawork.cut9();
        this.msg.print("He's always carrying a sword\naround with a weird grin\nplastered on his face.");
        System.sleep(95);
        this.waitclear(10);
        this.msg.print("Don't you think that's dangerous?");
        System.sleep(40);
        this.waitclear(10);
        System.sleep(15);
    }

    void playSCENE3() {
        this.dummy.start(1, "act10_chara");
        this.shion2.start(1, "act10_shion2");
        this.andrew.start(1, "act10_andrew");
        this.shion.start(1, "reset_pos");
        this.kosmos.start(1, "reset_pos");
        this.tray1.start(1, "act10_tray");
        this.Fshion.start(1, "face10_shion2");
        this.Fandrew.start(1, "face10_andrew");
        this.Fshion1.start(1, "eyes10_shion2");
        this.Fandrew1.start(1, "eyes10_andrew");
        this.camerawork.cut10();
        System.sleep(15);
        this.msg_print("【アンドリュー】", "Ah, yeah...\nThat's definitely dangerous...");
        System.sleep(115);
        this.waitclear(10);
        System.sleep(25);
        this.Fandrew.start(1, "face10_1_andrew");
        System.sleep(60);
        this.msg_print("【アンドリュー】", "So this is the android that can\ntake on the Gnosis...");
        System.sleep(80);
        System.sleep(10);
        this.shion.renderCommand(534);
        this.andrew.renderCommand(534);
        this.kosmos.renderCommand(517);
        this.kosmos_tub.renderCommand(534);
        this.dummy.start(1, "act11_chara");
        this.shion.start(1, "act11_shion");
        this.andrew.start(1, "act11_andrew");
        this.kosmosM.start(1, "act11_kosmos_middle");
        this.kosmos.start(1, "reset_pos");
        this.shion2.start(1, "reset_pos");
        this.target.start(1, "act11_steam");
        this.kosmos_tub.start(1, "act11_kosmos_tub");
        this.Fandrew.start(1, "face11_andrew");
        this.Fshion.start(1, "face11_shion");
        this.Fandrew1.start(1, "eyes11_andrew");
        this.camerawork.cut11();
        this.waitclear(30);
        System.sleep(30);
        this.msg.print("She sure doesn't look like it.");
        System.sleep(60);
        this.waitclear(10);
        this.Fandrew.start(1, "face11_1_andrew");
        this.Fshion.start(1, "face11_1_shion");
        this.Fandrew1.start(1, "eyes11_1_andrew");
        System.sleep(35);
        this.msg_print("【シオン】", "...Yeah.");
        System.sleep(18);
        this.waitclear(12);
        System.sleep(220);
        System.sleep(45);
        this.shion2.renderCommand(22);
        this.kosmos.renderCommand(5);
        this.shion.setVisible(false);
        this.shion2.setVisible(true);
        this.kosmos.setVisible(true);
        this.shion.start(1, "act12_shion");
        this.shion2.start(1, "act12_shion2");
        this.andrew.start(1, "reset_pos");
        this.kosmos.start(1, "act12_kosmos");
        this.kosmosM.start(1, "reset_pos");
        this.kosmos_tub.start(1, "act12_kosmos_tub");
        this.camerawork.cut12();
        System.sleep(60);
        this.msg_print("【シオン】", "But...");
        System.sleep(24);
        this.waitclear(6);
        System.sleep(30);
        this.shion.renderCommand(0);
        this.light1.setGlobalPointLightCol(2, 0.07f, 0.05f, 0.02f);
        this.light1.setGlobalPointLightPos(2, -0.34f, 1.39f, 0.85f);
        this.shion.setVisible(true);
        this.shion2.setVisible(false);
        this.shion.start(1, "act13_shion");
        this.shion2.start(1, "reset_pos");
        this.shion.setVisible(1, true);
        this.shion.setVisible(16, true);
        this.shion.setVisible(21, true);
        this.kosmos.start(1, "reset_pos");
        this.kosmosM.start(1, "act1_kosmos_middle");
        this.kosmos_tub.start(1, "reset_pos");
        this.Fshion.start(1, "face13_shion");
        this.Fshion1.start(1, "eyes13_shion");
        this.Fshion1.start(1, "eyes13_shion");
        this.camerawork.cut13();
        this.msg.print("regardless of what she looks like,\nshe's still a weapon.");
        System.sleep(80);
        this.waitclear(10);
        System.sleep(60);
        this.light1.setGlobalPointLightCol(1, 0.52f, 0.45f, 0.43f);
        this.light1.setGlobalPointLightPos(1, -2.57f, 4.94f, 3.73f);
        this.light1.setGlobalPointLightCol(2, 0.0f, 0.0f, 0.0f);
        this.light1.setGlobalPointLightPos(2, 0.0f, 0.0f, 0.0f);
        this.andrew.setVisible(true);
        this.shadow.start(1, "act14_shadow");
        this.andrew.start(1, "act14_andrew");
        this.shion.start(1, "act14_shion");
        this.tray1.start(1, "act14_tray");
        this.spoon.start(1, "act14_spoon");
        this.target.start(1, "act14_steam");
        this.kosmos_tub.start(1, "act0_kosmos_tub_full");
        this.Fandrew.start(1, "face14_andrew");
        this.Fandrew1.start(1, "eyes14_andrew");
        this.camerawork.cut14();
        System.sleep(45);
        this.msg_print("【アンドリュー】", "I suppose you're right...");
        System.sleep(40);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("Before we left the Woglinde...\nWhat was his name?");
        System.sleep(103);
        this.waitclear(12);
        this.light1.setGlobalPointLightCol(2, 0.07f, 0.05f, 0.02f);
        this.light1.setGlobalPointLightPos(2, -0.34f, 1.39f, 0.85f);
        this.shion.start(1, "act15_shion");
        this.Fshion.start(1, "face15_shion");
        this.Fshion1.start(1, "eyes15_shion");
        this.camerawork.cut15();
        this.msg.print("Lieutenant Virgil?\nHe was killed by this android's\nbattle algorithm, wasn't he?");
        System.sleep(102);
        this.waitclear(13);
        this.waitclear(75);
        System.sleep(15);
        this.andrew.renderCommand(0);
        this.andrew.start(1, "act16_andrew");
        this.spoon.start(1, "act16_spoon");
        this.Fandrew.start(1, "face16_andrew");
        this.Feat.start(1, "face16_eat");
        this.camerawork.cut16();
        this.msg_print("【アンドリュー】", "I'm not trying to blame you.");
        System.sleep(55);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("Logic, reason...they can\nbring about death.");
        System.sleep(105);
        this.waitclear(10);
        System.sleep(40);
        this.msg.print("Happens all the time.");
        System.sleep(40);
        this.waitclear(10);
        System.sleep(45);
        this.kosmos.renderCommand(534);
        this.kosmos_tub.renderCommand(0);
        this.light1.setGlobalPointLightCol(2, 0.07f, 0.05f, 0.02f);
        this.light1.setGlobalPointLightPos(2, -0.34f, 1.39f, 0.85f);
        this.shion.start(1, "act17_shion");
        this.andrew.start(1, "reset_pos");
        this.kosmos.start(1, "reset_pos");
        this.kosmosM.start(1, "act1_kosmos_middle");
        this.kosmos_tub.start(1, "act1_kosmos_tub");
        this.Fshion.start(1, "face17_shion");
        this.Fshion1.start(1, "eyes17_shion");
        this.camerawork.cut17();
        System.sleep(15);
        this.msg_print("【シオン】", "I...never thought...\nshe would ever do something like that.");
        System.sleep(174);
        this.waitclear(11);
        System.sleep(45);
        this.shion.renderCommand(534);
        this.andrew.setVisible(true);
        this.shadow.start(1, "act18_shadow");
        this.shion.start(1, "act18_shion");
        this.andrew.start(1, "act18_andrew");
        this.kosmosM.start(1, "reset_pos");
        this.kosmos_tub.start(1, "act0_kosmos_tub_full");
        this.Fandrew.start(1, "face18_andrew");
        this.Fshion.start(1, "face18_shion");
        this.camerawork.cut18();
        System.sleep(15);
        this.msg_print("【アンドリュー】", "You programmed her basic\nlogic architecture, right?");
        System.sleep(80);
        this.waitclear(10);
        this.msg_print("【シオン】", "Yes...pretty much.");
        System.sleep(45);
        this.waitclear(10);
        this.light1.setGlobalPointLightCol(2, 0.07f, 0.05f, 0.02f);
        this.light1.setGlobalPointLightPos(2, -0.47f, 1.38f, 2.11f);
        this.shion.renderCommand(0);
        this.shadow.start(1, "act19_shadow");
        this.shion.start(1, "act19_shion");
        this.Fshion.start(1, "face19_shion");
        this.camerawork.cut19();
        this.msg.print("I always thought of her like\na daughter...or more like\na friend, maybe...");
        System.sleep(108);
        this.waitclear(12);
        System.sleep(30);
        this.msg.print("...but...");
        System.sleep(55);
        this.waitclear(10);
        System.sleep(35);
        this.msg_print("【アンドリュー】", "I see...");
        System.sleep(30);
        this.waitclear(10);
        System.sleep(15);
    }

    void playSCENE4() {
        this.shadow.start(1, "act20_shadow");
        this.andrew.start(1, "act20_andrew");
        this.tray2.start(1, "act20_tray2");
        this.spoon.start(1, "act20_spoon");
        this.Fandrew.start(1, "face20_andrew");
        this.Feat.start(1, "face20_eat");
        this.camerawork.cut16();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.62f, 0.62f, 0.62f);
        this.light.setDirection2(1, 0.828f, 0.458f, -0.324f);
        this.light.setColor(2, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(2, -0.674f, 0.455f, -0.582f);
        this.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(3, -0.146f, -0.905f, -0.4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(15);
        this.msg.print("Well,");
        System.sleep(42);
        this.waitclear(9);
        this.msg.print("I doubt we could've saved\nthe Lieutenant from\nthat situation anyway.");
        System.sleep(96);
        this.waitclear(13);
        System.sleep(15);
        this.shion.renderCommand(534);
        this.shadow.start(1, "act21_shadow");
        this.shion.start(1, "act21_shion");
        this.Fshion.start(1, "face21_shion");
        this.camerawork.cut21();
        System.sleep(30);
        this.msg_print("【シオン】", "Thanks...\nfor trying to cheer me up.");
        System.sleep(27);
        this.waitclear(43);
        System.sleep(5);
        System.sleep(10);
        this.waitclear(5);
        System.sleep(5);
        this.shion.renderCommand(534);
        this.andrew.renderCommand(534);
        this.shadow.start(1, "act22_shadow");
        this.shion.start(1, "act22_shion");
        this.andrew.start(1, "act22_andrew");
        this.kosmosM.start(1, "reset_pos");
        this.kosmos_tub.start(1, "act22_kosmos_tub");
        this.spoon.start(1, "act22_spoon");
        this.target.start(1, "act22_steam");
        this.Fshion.start(1, "face_stop_shion");
        this.Fandrew.start(1, "face22_andrew");
        this.Feat.start(1, "face22_eat");
        this.camerawork.cut22();
        this.msg.print("I noticed you eat very neatly...");
        System.sleep(81);
        this.waitclear(9);
        System.sleep(10);
        this.msg_print("【アンドリュー】", "Hmm?\nAh, that's part of who I am.");
        System.sleep(10);
        System.sleep(85);
        this.waitclear(10);
        this.shion.start(1, "act23_shion");
        this.andrew.start(1, "act23_andrew");
        this.spoon.start(1, "act23_spoon");
        this.Fshion.start(1, "face23_shion");
        this.Fandrew.start(1, "face23_andrew");
        this.Feat.start(1, "face23_eat");
        this.camerawork.cut23();
        System.sleep(20);
        this.msg.print("At least now...that is.");
        System.sleep(35);
        this.waitclear(10);
        System.sleep(10);
        this.waitclear(75);
        this.shadow.start(1, "act24_shadow");
        this.shion.start(1, "act24_shion");
        this.andrew.start(1, "act24_andrew");
        this.spoon.start(1, "act24_spoon");
        this.Fshion.start(1, "face24_shion");
        this.Fandrew.start(1, "face24_andrew");
        this.Feat.start(1, "face24_eat");
        this.Fandrew1.start(1, "eyes24_andrew");
        this.camerawork.cut24();
        System.sleep(15);
        this.msg_print("【シオン】", "So, what were you doing\non the Woglinde?");
        System.sleep(110);
        this.waitclear(10);
        System.sleep(15);
        this.msg_print("【アンドリュー】", "Why...Why do you ask?");
        System.sleep(50);
        this.waitclear(10);
        this.andrew.renderCommand(534);
        this.shion.renderCommand(0);
        this.shion.start(1, "act25_shion");
        this.andrew.start(1, "act25_andrew");
        this.tray1.start(1, "act25_tray");
        this.spoon.setVisible(false);
        this.Fshion.start(1, "face25_shion");
        this.Fshion1.start(1, "eyes25_shion");
        this.camerawork.cut25();
        System.sleep(15);
        this.msg_print("【シオン】", "Just...wondering.");
        System.sleep(30);
        this.waitclear(10);
        System.sleep(15);
        this.msg.print("You somehow seemed...different...\nfrom the other crew members.");
        System.sleep(8);
        System.sleep(123);
        this.waitclear(9);
        System.sleep(30);
        this.shion.renderCommand(0);
        this.andrew.renderCommand(0);
        Stage.setVisible(4, false);
        this.shadow.start(1, "act26_shadow");
        this.shion.start(1, "act26_shion");
        this.andrew.start(1, "act26_andrew");
        this.tray1.start(1, "act26_tray1");
        this.tray2.start(1, "act26_tray2");
        this.spoon.setVisible(false);
        this.kosmos_tub.start(1, "act0_kosmos_tub_full");
        this.Fandrew.start(1, "face26_andrew");
        this.camerawork.cut26();
        System.sleep(30);
        this.msg_print("【アンドリュー】", "The Woglinde task force was\nassembled hastily.");
        System.sleep(105);
        this.waitclear(10);
        System.sleep(20);
        this.msg.print("Besides, I'm a soldier.\nI'll go to where I'm assigned.");
        System.sleep(103);
        System.sleep(27);
        this.waitclear(10);
        System.sleep(25);
        Stage.setVisible(4, true);
        this.shadow.start(1, "act27_shadow");
        this.shion.start(1, "act27_shion");
        this.andrew.start(1, "act27_andrew");
        this.tray2.start(1, "act27_tray");
        this.Fshion.start(1, "face27_shion");
        this.camerawork.cut27();
        System.sleep(60);
        this.shion.start(1, "act28_shion");
        this.Fshion.start(1, "face28_shion");
        this.camerawork.cut28();
        System.sleep(15);
        this.msg_print("【シオン】", "A...soldier?");
        System.sleep(58);
        this.waitclear(12);
        System.sleep(80);
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

    void trace0(Chr chr) {
        this.unit0.transCNS(chr, 0, 0.0f, 0.0f, 0.0f);
        this.unit0.rotYCNS(chr, 0, 0.0f, 0.0f, 1.0f);
    }

    void trace1(Chr chr) {
        this.unit1.transCNS(chr, 0, 0.0f, 0.0f, 0.0f);
        this.unit1.rotYCNS(chr, 0, 0.0f, 0.0f, 1.0f);
    }

    void trace2(Chr chr) {
        this.unit2.transCNS(chr, 0, 0.0f, 0.0f, 0.0f);
        this.unit2.rotYCNS(chr, 0, 0.0f, 0.0f, 1.0f);
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

        public void act10_andrew() {
            this.setTranslate(0.41f, 0.0f, 0.63f);
            this.setRotate(0.0f, 352.0f, 0.0f);
            this.setVisible(true);
            this.mtn(271, 30, 345, 8, 0, 1.0f, true);
        }

        public void act10_chara() {
            SCE02013B.this.shion2.setVisible(true);
            SCE02013B.this.shion.setVisible(false);
        }

        public void act10_shion2() {
            this.setTranslate(0.48f, 0.0f, 1.52f);
            this.setRotate(0.0f, 189.0f, 0.0f);
            this.setVisible(true);
            this.mtn(270, 30, 345, 8, 0, 1.0f, true);
        }

        public void act11_andrew() {
            this.setVisible(0, false);
            this.setVisible(1, false);
            this.setVisible(true);
            this.mtn(273, 0, 0, 8, 0, 1.0f, true);
            System.sleep(30);
            this.mtn(273, 0, 140, 8, 0, 1.0f, true);
            this.mtn(273, 141, 190, 8, 0, 0.8f, true);
            this.mtn(273, 191, 210, 8, 0, 0.8f, true);
            this.mtn(273, 211, 400, 8, 0, 0.8f, true);
        }

        public void act11_chara() {
            SCE02013B.this.shion.setVisible(true);
            SCE02013B.this.shion2.setVisible(false);
        }

        public void act11_kosmos_middle() {
            this.setTranslate(-0.5f, 0.01f, 0.75f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            SCE02013B.this.kosmosM.setVisible(0, true);
            SCE02013B.this.kosmosM.setVisible(13, true);
            int n = 1;
            while (n < 4) {
                SCE02013B.this.kosmosM.setVisible(n, false);
                ++n;
            }
            int n2 = 11;
            while (n2 < 13) {
                SCE02013B.this.kosmosM.setVisible(n2, false);
                ++n2;
            }
            int n3 = 14;
            while (n3 < 29) {
                SCE02013B.this.kosmosM.setVisible(n3, false);
                ++n3;
            }
            this.mtn(258, 0, 90, 8, -2147483640, 1.0f, true);
        }

        public void act11_shadow() {
            SCE02013B.this.tray1.setShadow(0, 0);
        }

        public void act11_shion() {
            this.setTranslate(0.43f, 0.0f, 1.52f);
            this.setRotate(0.0f, 179.0f, 0.0f);
            this.setVisible(true);
            this.setVisible(0, false);
            this.setVisible(10, false);
            this.mtn(272, 0, 465, 8, 8, 1.0f, true);
        }

        public void act12_kosmos() {
            this.setTranslate(-0.5f, 0.01f, 0.75f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            SCE02013B.this.kosmos.setVisible(0, true);
            int n = 13;
            while (n < 15) {
                SCE02013B.this.kosmos.setVisible(n, true);
                ++n;
            }
            SCE02013B.this.kosmos.setVisible(15, false);
            SCE02013B.this.kosmos.setVisible(16, false);
            SCE02013B.this.kosmos.setVisible(21, false);
            SCE02013B.this.kosmos.setVisible(22, false);
            SCE02013B.this.kosmos.setVisible(23, false);
            SCE02013B.this.kosmos.setVisible(24, false);
            SCE02013B.this.kosmos.setVisible(25, false);
            SCE02013B.this.kosmos.setVisible(26, false);
            SCE02013B.this.kosmos.setVisible(27, false);
            this.mtn(258, 0, 90, 8, -2147483640, 1.0f, true);
        }

        public void act12_shion() {
            this.setVisible(false);
            this.setTranslate(-0.62f, -0.03f, 0.86f);
            this.setRotate(0.0f, 184.0f, 0.0f);
            this.mtn(274, 111, 111, 8, 0, 1.0f, true);
        }

        public void act12_shion2() {
            this.setTranslate(-0.7f, -0.02f, 0.89f);
            this.setRotate(0.0f, 220.0f, 0.0f);
            this.setVisible(true);
            SCE02013B.this.shion2.face.setVisible(2, false);
            SCE02013B.this.shion2.setVisible(0, false);
            SCE02013B.this.shion2.setVisible(1, false);
            SCE02013B.this.shion2.setVisible(2, false);
            SCE02013B.this.shion2.setVisible(3, false);
            SCE02013B.this.shion2.setVisible(4, false);
            SCE02013B.this.shion2.setVisible(5, false);
            SCE02013B.this.shion2.setVisible(6, false);
            SCE02013B.this.shion2.setVisible(7, false);
            SCE02013B.this.shion2.setVisible(8, false);
            SCE02013B.this.shion2.setVisible(9, false);
            SCE02013B.this.shion2.setVisible(10, false);
            SCE02013B.this.shion2.setVisible(11, false);
            SCE02013B.this.shion2.setVisible(12, false);
            SCE02013B.this.shion2.setVisible(13, false);
            SCE02013B.this.shion2.setVisible(14, false);
            SCE02013B.this.shion2.setVisible(15, false);
            SCE02013B.this.shion2.setVisible(16, false);
            SCE02013B.this.shion2.setVisible(17, false);
            SCE02013B.this.shion2.setVisible(18, false);
            SCE02013B.this.shion2.setVisible(19, false);
            SCE02013B.this.shion2.setVisible(22, false);
            SCE02013B.this.shion2.setVisible(23, false);
            this.mtn(274, 0, 0, 8, 0, 1.0f, true);
            System.sleep(10);
            this.mtn(274, 0, 110, 8, 8, 1.0f, true);
        }

        public void act13_shion() {
            this.setTranslate(-0.62f, -0.03f, 0.86f);
            this.setRotate(0.0f, 184.0f, 0.0f);
            this.mtn(274, 111, 275, 8, 8, 1.0f, true);
        }

        public void act13_shion2() {
            SCE02013B.this.shion2.face.setVisible(2, true);
            SCE02013B.this.shion2.setVisible(1, true);
            SCE02013B.this.shion2.setVisible(16, true);
        }

        public void act14_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.mtn(275, 0, 245, 8, 0, 1.0f, true);
        }

        public void act14_shadow() {
            SCE02013B.this.andrew.shadow_map_id(4);
            SCE02013B.this.andrew.setShadow(10, 45);
            SCE02013B.this.tray1.setShadow(0, 0);
            SCE02013B.this.tray2.shadow_map_id(4);
            SCE02013B.this.tray2.shadow_map_id(12);
            SCE02013B.this.tray2.setShadow(6, 45);
        }

        public void act14_shion() {
            this.setVisible(false);
            this.setTranslate(0.07f, -0.03f, 0.82f);
            this.setRotate(0.0f, 171.0f, 0.0f);
            this.mtn(276, 0, 0, 8, 0, 1.0f, true);
        }

        public void act15_shion() {
            this.setTranslate(0.07f, -0.03f, 0.82f);
            this.setRotate(0.0f, 171.0f, 0.0f);
            this.setVisible(true);
            this.setVisible(0, true);
            this.mtn(276, 0, 240, 8, 8, 1.0f, true);
        }

        public void act16_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.setVisible(0, true);
            this.setVisible(1, true);
            this.mtn(277, 0, 360, 8, 0, 1.1f, true);
        }

        public void act17_shion() {
            this.mtn(278, 0, 190, 8, 0, 0.85f, true);
            this.mtn(278, 191, 210, 8, 0, 0.8f, true);
        }

        public void act18_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.setVisible(0, false);
            this.setVisible(1, false);
            this.setVisible(15, false);
            this.mtn(280, 0, 185, 8, 0, 1.0f, true);
        }

        public void act18_shadow() {
            SCE02013B.this.shion.setShadow(10, 45);
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.kosmos_tub.setShadow(10, 45);
            SCE02013B.this.tray2.setShadow(0, 0);
        }

        public void act18_shion() {
            this.setTranslate(0.62f, 0.0f, 0.46f);
            this.setRotate(0.0f, 364.99f, 0.0f);
            this.setVisible(0, true);
            this.setVisible(10, true);
            this.mtn(279, 0, 185, 8, 0, 1.0f, true);
        }

        public void act19_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.kosmos_tub.setShadow(0, 0);
        }

        public void act19_shion() {
            this.setTranslate(-0.46f, 0.0f, 2.47f);
            this.setRotate(0.0f, 311.0f, 0.0f);
            this.mtn(281, 0, 285, 8, 0, 0.93f, true);
        }

        public void act1_andrew() {
            this.setTranslate(0.29f, 0.0f, 0.71f);
            this.setRotate(0.0f, 280.0f, 0.0f);
            this.setVisible(true);
            this.setVisible(0, false);
            this.setVisible(1, false);
            this.mtn(257, 0, 20, 8, 0, 1.0f, true);
            this.mtn(257, 0, 20, 8, 0, -1.0f, true);
            this.mtn(257, 0, 105, 8, 0, 1.0f, true);
        }

        public void act1_chara() {
            SCE02013B.this.shion2.setVisible(false);
            SCE02013B.this.kosmos.setVisible(false);
        }

        public void act1_kosmos_middle() {
            this.setTranslate(-0.5f, 0.01f, 0.75f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            int n = 0;
            while (n < 26) {
                SCE02013B.this.kosmosM.setVisible(n, true);
                ++n;
            }
            this.mtn(258, 0, 90, 8, 0x40000008, 1.0f, true);
        }

        public void act1_shadow() {
            System.sleep(95);
            SCE02013B.this.shion.shadow_map_id(16);
            SCE02013B.this.shion.setShadow(10, 45);
            SCE02013B.this.andrew.setShadow(0, 0);
        }

        public void act1_shion() {
            this.setTranslate(1.0f, 0.0f, 9.33f);
            this.setRotate(0.0f, 179.0f, 0.0f);
            this.setVisible(true);
            System.sleep(40);
            this.mtn(259, 0, 105, 8, 0, 1.0f, true);
        }

        public void act20_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.setVisible(0, true);
            this.setVisible(1, true);
            this.setVisible(15, true);
            this.mtn(282, 0, 220, 8, 0, 1.15f, true);
        }

        public void act20_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.setShadow(10, 45);
            SCE02013B.this.tray2.setShadow(6, 45);
        }

        public void act21_shadow() {
            SCE02013B.this.shion.setShadow(10, 45);
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.kosmos_tub.setShadow(10, 45);
            SCE02013B.this.tray2.setShadow(0, 0);
        }

        public void act21_shion() {
            this.setTranslate(-0.04f, 0.0f, 2.42f);
            this.setRotate(0.0f, 306.0f, 0.0f);
            this.mtn(283, 0, 165, 8, 0, 1.32f, true);
        }

        public void act22_andrew() {
            this.setTranslate(-3.5f, 0.0f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.mtn(285, 0, 220, 8, 0, 1.07f, true);
        }

        public void act22_shadow() {
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.kosmos_tub.setShadow(0, 0);
            SCE02013B.this.tray2.setShadow(6, 45);
        }

        public void act22_shion() {
            this.setTranslate(-0.17f, 0.0f, 2.78f);
            this.setRotate(0.0f, 307.0f, 0.0f);
            this.setVisible(3, false);
            this.setVisible(9, false);
            this.setVisible(14, false);
            this.setVisible(15, false);
            this.setVisible(16, false);
            this.setVisible(17, false);
            this.mtn(284, 0, 220, 8, 0, 1.07f, true);
        }

        public void act23_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.mtn(287, 0, 200, 8, 0, 1.33f, true);
        }

        public void act23_shion() {
            this.setTranslate(-1.37f, 0.0f, 4.22f);
            this.setRotate(0.0f, 302.99f, 0.0f);
            this.setVisible(3, true);
            this.setVisible(9, true);
            this.setVisible(14, true);
            this.setVisible(15, true);
            this.setVisible(16, true);
            this.setVisible(17, true);
            this.mtn(286, 30, 60, 8, 0, 1.0f, true);
            this.mtn(286, 61, 170, 8, 0, 1.33f, true);
            this.mtn(286, 171, 200, 8, 0, 1.1f, true);
            this.mtn(286, 100, 200, 8, 0, -0.6f, true);
        }

        public void act24_andrew() {
            this.setTranslate(-3.5f, -0.02f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.setVisible(0, false);
            this.setVisible(1, false);
            this.mtn(289, 0, 220, 8, 0, 1.04f, true);
        }

        public void act24_shadow() {
            SCE02013B.this.shion.shadow_map_id(4);
            SCE02013B.this.shion.setShadow(10, 45);
            SCE02013B.this.andrew.setShadow(10, 45);
            SCE02013B.this.tray2.setShadow(6, 45);
        }

        public void act24_shion() {
            this.setTranslate(-1.75f, 0.0f, 4.85f);
            this.setRotate(0.0f, 271.0f, 0.0f);
            this.setVisible(10, false);
            this.mtn(288, 0, 220, 8, 0, 1.04f, true);
        }

        public void act25_andrew() {
            this.setTranslate(-3.5f, -0.06f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.mtn(291, 0, 285, 8, 0, 1.18f, true);
        }

        public void act25_shion() {
            this.setTranslate(-3.37f, 0.0f, 5.06f);
            this.setRotate(0.0f, 269.0f, 0.0f);
            this.mtn(290, 0, 285, 8, 0, 1.18f, true);
        }

        public void act26_andrew() {
            this.setTranslate(-2.91f, 0.0f, 4.3f);
            this.setRotate(0.0f, 97.0f, 0.0f);
            this.setVisible(0, true);
            this.setVisible(1, true);
            this.mtn(293, 0, 269, 8, 0, 1.0f, true);
            this.mtn(293, 270, 285, 8, 0, 0.75f, true);
            this.mtn(293, 286, 340, 8, 0, 1.0f, true);
        }

        public void act26_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.tray2.setShadow(0, 0);
        }

        public void act26_shion() {
            this.mtn(292, 0, 340, 8, 8, 1.0f, true);
        }

        public void act27_andrew() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(Integer.MIN_VALUE, 0, 1.0f, true);
        }

        public void act27_shadow() {
            SCE02013B.this.shion.setShadow(10, 45);
            SCE02013B.this.tray2.setShadow(6, 45);
        }

        public void act27_shion() {
            this.mtn(295, 0, 165, 8, 0, 1.0f, true);
        }

        public void act28_shion() {
            this.mtn(295, 0, 165, 8, 0, 1.0f, true);
        }

        public void act2_andrew() {
            this.setTranslate(0.39f, 0.01f, 0.69f);
            this.setRotate(0.0f, 279.0f, 0.0f);
            this.setVisible(true);
            this.mtn(260, 0, 105, 8, 0, 1.05f, true);
        }

        public void act3_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.tray1.shadow_map_id(4);
            SCE02013B.this.tray1.shadow_map_id(12);
            SCE02013B.this.tray1.setShadow(6, 45);
        }

        public void act3_shion() {
            this.setVisible(true);
            this.mtn(261, 0, 190, 8, 0, 1.0f, true);
        }

        public void act4_andrew() {
            this.setTranslate(0.39f, 0.0f, 0.71f);
            this.setRotate(0.0f, 352.0f, 0.0f);
            this.setVisible(true);
            this.setVisible(0, true);
            this.setVisible(1, true);
            this.mtn(262, 0, 225, 8, 8, 1.0f, true);
        }

        public void act4_chara() {
            SCE02013B.this.shion.face.setVisible(false);
        }

        public void act4_shadow() {
            SCE02013B.this.shion.shadow_map_reset();
            SCE02013B.this.shion.shadow_map_id(12);
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.shadow_map_id(12);
            SCE02013B.this.andrew.setShadow(10, 45);
            SCE02013B.this.tray1.setShadow(0, 0);
            SCE02013B.this.kosmos_tub.shadow_map_id(12);
            SCE02013B.this.kosmos_tub.setShadow(10, 45);
        }

        public void act4_shion() {
            SCE02013B.this.shion.setLightMode(1);
            SCE02013B.this.shion.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.shion.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.shion.light.setDirection2(1, -0.488f, 0.141f, -0.861f);
            SCE02013B.this.shion.light.setColor(2, 0.17f, 0.17f, 0.17f);
            SCE02013B.this.shion.light.setDirection2(2, 0.896f, 0.001f, 0.444f);
            SCE02013B.this.shion.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE02013B.this.shion.light.setDirection2(3, -0.494f, -0.604f, 0.625f);
            this.setTranslate(1.0f, 0.0f, 8.9f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(263, 0, 225, 8, 8, 1.0f, true);
        }

        public void act5_andrew() {
            this.setVisible(true);
            this.mtn(262, 226, 330, 8, 8, 1.0f, true);
        }

        public void act5_chara() {
            SCE02013B.this.shion.face.setVisible(true);
        }

        public void act5_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.setShadow(0, 0);
        }

        public void act6_shadow() {
            SCE02013B.this.shion.setShadow(0, 0);
            SCE02013B.this.andrew.setShadow(0, 0);
            SCE02013B.this.tray1.setShadow(6, 45);
        }

        public void act6_shion() {
            this.setTranslate(0.97f, 0.0f, 5.37f);
            this.setRotate(0.0f, 183.9f, 0.0f);
            SCE02013B.this.shion.setLightMode(0);
            this.setVisible(true);
            this.mtn(265, 0, 125, 8, 0, 1.0f, true);
        }

        public void act7_andrew() {
            this.setTranslate(0.41f, 0.0f, 0.63f);
            this.setRotate(0.0f, 352.0f, 0.0f);
            this.setVisible(true);
            this.mtn(266, 0, 135, 8, 8, 1.0f, true);
        }

        public void act8_shadow() {
            SCE02013B.this.kosmos_tub.setShadow(0, 0);
        }

        public void act8_shion() {
            this.setTranslate(0.89f, 0.0f, 4.69f);
            this.setRotate(0.0f, 187.0f, 0.0f);
            this.setVisible(true);
            this.mtn(267, 0, 225, 8, 0, 1.09f, true);
        }

        public void act9_andrew() {
            this.setVisible(true);
            this.mtn(271, 0, 60, 8, 8, 1.0f, true);
        }

        public void act9_shion() {
            this.setTranslate(0.89f, 0.0f, 4.12f);
            this.setRotate(0.0f, 189.0f, 0.0f);
            this.setVisible(true);
            this.mtn(268, 0, 210, 8, 8, 1.23f, true);
        }

        public void act9_shion2() {
            this.setVisible(false);
            this.setTranslate(0.48f, 0.0f, 1.52f);
            this.setRotate(0.0f, 189.0f, 0.0f);
            this.mtn(270, 30, 30, 8, 0, 1.0f, true);
        }

        public void reset_pos() {
            this.setVisible(false);
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(Integer.MIN_VALUE, 0, 1.0f, true);
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

        public void act0_kosmos_tub_full() {
            this.setTranslate(-0.5f, 0.0f, 0.45f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            int n = 0;
            while (n < 23) {
                SCE02013B.this.kosmos_tub.setVisible(n, true);
                ++n;
            }
            this.mtn(296, 171, 171, 8, 0, 1.0f, true);
        }

        public void act11_kosmos_tub() {
            this.setVisible(0, false);
            this.setVisible(1, false);
            int n = 2;
            while (n < 18) {
                SCE02013B.this.kosmos_tub.setVisible(n, false);
                ++n;
            }
            this.setVisible(19, false);
        }

        public void act12_kosmos_tub() {
            int n = 2;
            while (n < 18) {
                SCE02013B.this.kosmos_tub.setVisible(n, false);
                ++n;
            }
            this.setVisible(18, false);
            this.setVisible(19, false);
            this.setVisible(20, false);
        }

        public void act1_kosmos_tub() {
            this.setTranslate(-0.5f, 0.0f, 0.45f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            int n = 2;
            while (n < 18) {
                SCE02013B.this.kosmos_tub.setVisible(n, false);
                ++n;
            }
            this.setVisible(19, false);
            this.mtn(296, 171, 171, 8, 0, 1.0f, true);
        }

        public void act22_kosmos_tub() {
            int n = 0;
            while (n < 18) {
                SCE02013B.this.kosmos_tub.setVisible(n, false);
                ++n;
            }
            this.setVisible(20, false);
            this.setVisible(21, false);
        }

        public void act5_kosmos_tub() {
            int n = 2;
            while (n < 18) {
                SCE02013B.this.kosmos_tub.setVisible(n, false);
                ++n;
            }
            this.setVisible(18, false);
            this.setVisible(19, false);
            this.setVisible(20, false);
        }

        public void reset_pos() {
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(false);
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

        public void eyes10_andrew() {
            SCE02013B.this.andrew.look_default();
            System.sleep(15);
            SCE02013B.this.andrew.look_eye_speed(4.5f);
            SCE02013B.this.andrew.look_eye_set(-4.5f, 1.5f);
            System.sleep(60);
            SCE02013B.this.andrew.look_eye_speed(5.0f);
            SCE02013B.this.andrew.look_eye_set(4.5f, 1.5f);
            System.sleep(60);
            SCE02013B.this.andrew.look_eye_speed(4.5f);
            SCE02013B.this.andrew.look_eye_set(0.0f, -1.0f);
            System.sleep(30);
            SCE02013B.this.andrew.look_eye_speed(10.0f);
            SCE02013B.this.andrew.look_eye_set(6.0f, 2.0f);
            System.sleep(30);
        }

        public void eyes10_shion2() {
            SCE02013B.this.shion.look_default();
            System.sleep(165);
            System.sleep(115);
            SCE02013B.this.shion.look_eye_speed(4.0f);
            SCE02013B.this.shion.look_eye_set(-2.5f, 2.5f);
            System.sleep(35);
        }

        public void eyes11_1_andrew() {
            SCE02013B.this.andrew.look_default();
            System.sleep(50);
            SCE02013B.this.andrew.look_eye_speed(4.5f);
            SCE02013B.this.andrew.look_eye_set(8.0f, 1.5f);
            System.sleep(55);
            SCE02013B.this.andrew.look_eye_speed(4.5f);
            SCE02013B.this.andrew.look_eye_set(-8.0f, 1.5f);
            System.sleep(15);
        }

        public void eyes11_andrew() {
            SCE02013B.this.andrew.look_default();
        }

        public void eyes13_shion() {
            SCE02013B.this.shion.look_default();
            SCE02013B.this.shion.look_speed(0.1f);
            SCE02013B.this.shion.look_char(SCE02013B.this.kosmos);
            SCE02013B.this.shion.look_eye_speed(999.0f);
            SCE02013B.this.shion.look_eye_set(-1.5f, 1.5f);
            System.sleep(35);
            SCE02013B.this.shion.look_eye_speed(0.8f);
            SCE02013B.this.shion.look_eye_set(1.5f, 3.5f);
            System.sleep(30);
            SCE02013B.this.shion.look_eye_speed(0.8f);
            SCE02013B.this.shion.look_eye_set(-1.5f, 3.5f);
            System.sleep(12);
        }

        public void eyes14_andrew() {
            SCE02013B.this.andrew.look_default();
            System.sleep(150);
            SCE02013B.this.target.setTranslate(0.2f, 0.9f, 0.47f);
            SCE02013B.this.target.setRotate(0.0f, 0.0f, 0.0f);
            SCE02013B.this.andrew.look_eye_speed(1.0f);
            SCE02013B.this.andrew.look_eye_set(-5.0f, 1.5f);
            System.sleep(30);
            SCE02013B.this.andrew.look_eye_set(-2.0f, 1.5f);
        }

        public void eyes15_shion() {
            SCE02013B.this.shion.look_default();
            System.sleep(150);
            SCE02013B.this.shion.look_eye_speed(0.6f);
            SCE02013B.this.shion.look_eye_set(-0.0f, 1.5f);
        }

        public void eyes16_andrew() {
            SCE02013B.this.andrew.look_default();
            System.sleep(150);
            SCE02013B.this.target.setTranslate(0.2f, 0.9f, 0.47f);
            SCE02013B.this.target.setRotate(0.0f, 0.0f, 0.0f);
            SCE02013B.this.andrew.look_eye_speed(1.0f);
            SCE02013B.this.andrew.look_eye_set(-5.0f, 1.5f);
        }

        public void eyes17_shion() {
            SCE02013B.this.shion.look_default();
            SCE02013B.this.shion.look_eye_speed(999.0f);
            SCE02013B.this.shion.look_eye_set(-0.0f, 1.5f);
        }

        public void eyes1_andrew() {
            SCE02013B.this.andrew.look_default();
        }

        public void eyes24_andrew() {
            SCE02013B.this.andrew.look_default();
            SCE02013B.this.andrew.look_eye_speed(999.0f);
            SCE02013B.this.andrew.look_eye_set(0.0f, 1.5f);
            System.sleep(170);
            SCE02013B.this.andrew.look_eye_speed(9.0f);
            SCE02013B.this.andrew.look_eye_set(12.0f, 0.5f);
            System.sleep(55);
            SCE02013B.this.andrew.look_eye_speed(0.6f);
            SCE02013B.this.andrew.look_eye_set(10.0f, 1.5f);
        }

        public void eyes25_shion() {
            SCE02013B.this.shion.look_default();
            System.sleep(45);
            SCE02013B.this.shion.look_eye_speed(1.0f);
            SCE02013B.this.shion.look_eye_set(2.5f, 2.5f);
            System.sleep(30);
            SCE02013B.this.shion.look_eye_speed(0.8f);
            SCE02013B.this.shion.look_eye_set(0.0f, 1.5f);
            System.sleep(165);
            SCE02013B.this.shion.look_eye_speed(1.5f);
            SCE02013B.this.shion.look_eye_set(-5.0f, -1.0f);
            System.sleep(10);
        }

        public void eyes26_andrew() {
            SCE02013B.this.andrew.look_default();
            SCE02013B.this.andrew.look_eye_speed(999.0f);
            SCE02013B.this.andrew.look_eye_set(8.0f, 1.5f);
            System.sleep(30);
            System.sleep(30);
            SCE02013B.this.andrew.look_eye_speed(1.0f);
            SCE02013B.this.andrew.look_eye_set(0.0f, 0.0f);
            System.sleep(6);
            System.sleep(9);
            System.sleep(60);
            System.sleep(30);
            SCE02013B.this.andrew.look_eye_set(10.0f, 1.5f);
            System.sleep(70);
        }

        public void eyes9_shion() {
            SCE02013B.this.shion.look_default();
            System.sleep(95);
            System.sleep(10);
            SCE02013B.this.shion.look_eye_speed(2.0f);
            SCE02013B.this.shion.look_eye_set(0.0f, -1.0f);
            System.sleep(40);
        }

        public void face10_1_andrew() {
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 8, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(60);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(48);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 4, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(6);
            SCE02013B.this.andrew.face.mtn(3, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(16);
            SCE02013B.this.andrew.face.mtn(3, 16, 20, 5, 0, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.andrew.face.mtn(4, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face10_andrew() {
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.andrew.face.mtn(1, 28, 59, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(30);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(11);
            SCE02013B.this.andrew.face.mtn(1, 0, 19, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(19);
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face10_shion2() {
            System.sleep(270);
            SCE02013B.this.shion2.face.mtn(18, 0, 120, 16, 9, 1.0f, false);
            SCE02013B.this.shion2.face.start(4, null);
            System.sleep(5);
        }

        public void face11_1_andrew() {
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(19, 1, 120, 2, 1, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(2);
            SCE02013B.this.andrew.face.mtn(19, 2, 2, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(13);
            SCE02013B.this.andrew.face.mtn(19, 0, 2, 5, 0, -0.3f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(20, 54, 55, 5, 0, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(45);
            SCE02013B.this.andrew.face.mtn(20, 55, 57, 5, 0, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(20, 57, 59, 5, 0, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(30);
        }

        public void face11_1_shion() {
            System.sleep(35);
            SCE02013B.this.shion.face.mtn(17, 0, 120, 16, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(18);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            System.sleep(145);
            SCE02013B.this.shion.face.mtn(18, 50, 53, 5, 0, 0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(18, 50, 53, 5, 0, -0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(75);
        }

        public void face11_andrew() {
            System.sleep(60);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(60);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face11_shion() {
            SCE02013B.this.shion.face.mtn(18, 0, 120, 16, 8, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(130);
        }

        public void face13_shion() {
            SCE02013B.this.shion.face.mtn(17, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(36);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(27);
            SCE02013B.this.shion.face.mtn(17, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(17);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(18, 109, 110, 5, 0, -0.2f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(30);
        }

        public void face14_andrew() {
            System.sleep(45);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(40);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(48);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(46);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face15_shion() {
            SCE02013B.this.shion.face.mtn(7, 45, 50, 8, 0, 0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(34);
            SCE02013B.this.shion.face.mtn(7, 45, 50, 8, 0, -0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(35);
            SCE02013B.this.shion.getChild(0x1000000).mtn(7, 45, 67, 8, 0, 0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(40);
            SCE02013B.this.shion.face.mtn(7, 67, 76, 8, 0, 0.3f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face16_andrew() {
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(50);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(39);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(51);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(50);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(40);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(55);
        }

        public void face16_eat() {
            System.sleep(75);
            System.sleep(155);
            System.sleep(50);
            System.sleep(30);
            SCE02013B.this.andrew.face.mtn(1, 0, 7, 8, 0, 0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(18);
        }

        public void face17_shion() {
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(27);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(45);
            SCE02013B.this.shion.face.mtn(8, 50, 120, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(42);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(35);
            SCE02013B.this.shion.face.mtn(8, 103, 109, 5, 0, 0.2f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(8, 103, 109, 5, 0, -0.15f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
        }

        public void face18_andrew() {
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face18_shion() {
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 8, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            System.sleep(80);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(18);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face19_shion() {
            SCE02013B.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(44);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(36);
            SCE02013B.this.shion.face.mtn(17, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(28);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(7, 0, 120, 16, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(60);
            SCE02013B.this.shion.face.mtn(8, 0, 120, 4, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.shion.face.mtn(8, 103, 109, 5, 0, 0.15f, false);
            System.sleep(35);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(8, 103, 109, 5, 0, -0.25f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(20);
            System.sleep(25);
        }

        public void face1_andrew() {
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 8, 8, 0.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face1_kosmos() {
            SCE02013B.this.kosmos.face.mtn(2, 46, 46, 0, 0, 1.0f, false);
            SCE02013B.this.kosmos.face.start(4, null);
        }

        public void face1_kosmos_middle() {
            SCE02013B.this.kosmosM.face.mtn(2, 46, 46, 0, 0, 1.0f, false);
            SCE02013B.this.kosmosM.face.start(4, null);
        }

        public void face1_shion() {
            SCE02013B.this.shion.face.mtn(2, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face20_andrew() {
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(42);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(63);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(18);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 3, 8, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
        }

        public void face20_eat() {
            System.sleep(165);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, 0.5f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face21_shion() {
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(22);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(16, 45, 60, 8, 0, 0.6f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(16, 38, 60, 16, 0, -0.6f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.shion.face.mtn(16, 38, 56, 16, 0, 0.6f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(20);
        }

        public void face22_andrew() {
            System.sleep(90);
            System.sleep(10);
            SCE02013B.this.andrew.face.mtn(1, 0, 4, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(1, 0, 4, 5, 0, -1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(1, 0, 3, 5, 0, 0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(7);
            System.sleep(17);
            SCE02013B.this.andrew.face.mtn(1, 3, 5, 5, 0, 0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.andrew.face.mtn(1, 1, 5, 5, 0, -0.6f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(6);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(21);
            SCE02013B.this.andrew.face.mtn(1, 0, 4, 5, 0, 0.6f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face22_eat() {
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, 0.45f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(17);
            System.sleep(6);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, -0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(7);
        }

        public void face22_shion() {
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(50);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(19);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face23_andrew() {
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(35);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face23_eat() {
            System.sleep(70);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, 0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(28);
            System.sleep(7);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, -0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(7);
        }

        public void face23_shion() {
            System.sleep(75);
            SCE02013B.this.shion.face.mtn(16, 1, 120, 8, 9, 0.2f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(75);
        }

        public void face24_andrew() {
            System.sleep(15);
            System.sleep(90);
            SCE02013B.this.andrew.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            System.sleep(10);
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(45);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face24_eat() {
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, 0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            System.sleep(2);
            SCE02013B.this.andrew.face.mtn(1, 40, 45, 8, 0, -0.4f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(7);
        }

        public void face24_shion() {
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(16);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(27);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(67);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face25_shion() {
            SCE02013B.this.shion.face.mtn(2, 50, 120, 0, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(1, 38, 59, 0, 0, 0.7f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(2, 50, 59, 5, 0, -0.25f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(45);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(66);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(15, 0, 10, 8, 0, 0.2f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
        }

        public void face26_andrew() {
            System.sleep(30);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(36);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(60);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(10);
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(22);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(16);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(32);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.andrew.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(27);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(10);
        }

        public void face27_shion() {
            SCE02013B.this.shion.face.mtn(17, 104, 104, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
        }

        public void face28_shion() {
            SCE02013B.this.shion.face.mtn(17, 104, 104, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(17, 0, 104, 5, 0, -0.5f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(27);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(17, 0, 120, 10, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(21);
            SCE02013B.this.shion.face.mtn(17, 21, 30, 5, 0, 0.6f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face2_andrew() {
            SCE02013B.this.andrew.face.mtn(4, 0, 120, 10, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(25);
            SCE02013B.this.andrew.face.mtn(3, 0, 120, 16, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(25);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(5);
        }

        public void face3_shion() {
            SCE02013B.this.shion.face.mtn(17, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(6);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(24);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 0.5f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(12);
            System.sleep(6);
            SCE02013B.this.shion.face.mtn(17, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(60);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.shion.face.mtn(17, 0, 10, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            System.sleep(5);
            SCE02013B.this.shion.face.mtn(18, 0, 120, 5, 9, 0.6f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
        }

        public void face5_andrew() {
            System.sleep(30);
            SCE02013B.this.andrew.face.mtn(1, 8, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(33);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 0.7f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(12);
        }

        public void face6_shion() {
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(20);
            SCE02013B.this.shion.face.mtn(15, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(35);
            SCE02013B.this.shion.face.mtn(16, 50, 57, 5, 0, -0.9f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.shion.face.mtn(15, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(30);
            SCE02013B.this.shion.face.mtn(16, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(25);
        }

        public void face7_andrew() {
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(1, 0, 6, 8, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(6);
            System.sleep(6);
            SCE02013B.this.andrew.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(5);
            System.sleep(5);
            SCE02013B.this.andrew.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(12);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(11);
            SCE02013B.this.andrew.face.mtn(1, 0, 15, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
            System.sleep(15);
            SCE02013B.this.andrew.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face8_shion() {
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(24);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(22);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 8, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(50);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 8, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(19);
            SCE02013B.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(40);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(5);
            SCE02013B.this.shion.face.mtn(1, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(35);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
        }

        public void face9_shion() {
            SCE02013B.this.shion.face.mtn(1, 40, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(45);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(9);
            SCE02013B.this.shion.face.mtn(1, 30, 120, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(41);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(11, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(40);
            SCE02013B.this.shion.face.mtn(12, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
            System.sleep(10);
            SCE02013B.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }

        public void face_stop_andrew() {
            SCE02013B.this.andrew.face.mtn(Integer.MIN_VALUE, 0, 0, 5, 0, 1.0f, false);
            SCE02013B.this.andrew.face.start(4, null);
        }

        public void face_stop_shion() {
            SCE02013B.this.shion.face.mtn(Integer.MIN_VALUE, 0, 0, 5, 0, 1.0f, false);
            SCE02013B.this.shion.face.start(4, null);
        }
    }

    class allUNIT
            extends Unit {
        public allUNIT(int n) {
            this.init(n, this.px, this.py, this.pz, this.ry);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        public allUNIT(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        public void act10_tray() {
            this.setParent(SCE02013B.this.andrew, 72);
            this.setTranslate(0.16f, 0.07f, 0.17f);
            this.setRotate(-273.0f, 241.0f, -54.0f);
        }

        void act11_steam() {
            SCE02013B.this.steam1.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam1.disp(true);
            SCE02013B.this.steam2.setScale(0.0f, 0.0f, 0.0f);
            SCE02013B.this.steam2.disp(true);
            System.sleep(270);
            float f = 0.4f;
            float f2 = 0.0f;
            int n = 0;
            while (n < 40) {
                SCE02013B.this.steam1.setScale(f, f, f);
                SCE02013B.this.steam2.setScale(f2, f2, f2);
                f -= 0.01f;
                f2 += 0.01f;
                System.sleep(1);
                ++n;
            }
            SCE02013B.this.steam1.disp(false);
            SCE02013B.this.steam2.disp(true);
        }

        public void act14_spoon() {
            this.setParent(SCE02013B.this.andrew, 72);
            this.setTranslate(0.07f, -0.02f, 0.03f);
            this.setRotate(43.0f, 243.98f, 62.0f);
            this.setVisible(true);
        }

        void act14_steam() {
            SCE02013B.this.steam1.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam2.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam1.setCaster(SCE02013B.this.tray2);
            SCE02013B.this.steam2.setCaster(SCE02013B.this.tray2);
        }

        public void act14_tray() {
            SCE02013B.this.tray1.setVisible(false);
            SCE02013B.this.tray2.setTranslate(-3.12f, 0.66f, 4.24f);
            SCE02013B.this.tray2.setRotate(0.0f, 96.0f, 0.0f);
            SCE02013B.this.tray2.setVisible(true);
            SCE02013B.this.tray2.setVisible(7, false);
        }

        public void act16_spoon() {
            this.setTranslate(0.07f, -0.02f, 0.03f);
            this.setRotate(42.0f, 244.0f, 61.0f);
            System.sleep(75);
            System.sleep(155);
            System.sleep(50);
            SCE02013B.this.ASmove(SCE02013B.this.spoon, 15, 0.07f, -0.02f, 0.03f, 93.0f, 304.98f, 128.0f);
        }

        public void act1_handgun() {
            this.setParent(SCE02013B.this.andrew, 72);
            this.setTranslate(0.06f, -0.02f, 0.02f);
            this.setRotate(7.0f, -34.0f, 0.0f);
            SCE02013B.this.andrew.setVisible(12, false);
            SCE02013B.this.andrew.setVisible(15, true);
            this.setVisible(true);
            System.sleep(145);
            this.setTranslate(0.07f, -0.02f, 0.01f);
            this.setRotate(7.0f, 2.0f, 0.0f);
        }

        public void act1_steam() {
            SCE02013B.this.steam1.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam2.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam1.disp(true);
            SCE02013B.this.steam2.disp(false);
        }

        public void act1_tray() {
            this.setParent(SCE02013B.this.shion, 72);
            this.setTranslate(0.18f, 0.14f, 0.11f);
            this.setRotate(-282.0f, 57.0f, 35.0f);
            this.setScale(1.0f, 1.0f, 1.0f);
        }

        public void act20_spoon() {
            this.setParent(SCE02013B.this.andrew, 72);
            this.setTranslate(0.06f, -0.02f, 0.04f);
            this.setRotate(-5.0f, 250.97f, 13.0f);
            this.setVisible(true);
        }

        public void act20_tray2() {
            SCE02013B.this.tray2.setVisible(1, false);
            SCE02013B.this.tray2.setVisible(2, false);
        }

        public void act22_spoon() {
            this.setTranslate(0.05f, 0.0f, 0.06f);
            this.setRotate(-57.0f, 245.96f, -29.0f);
            System.sleep(22);
            this.setVisible(0, false);
            System.sleep(31);
            SCE02013B.this.ASmove(SCE02013B.this.spoon, 15, 0.05f, -0.01f, 0.04f, -48.0f, 249.96f, -12.0f);
            this.setVisible(0, true);
        }

        void act22_steam() {
            SCE02013B.this.steam1.disp(false);
            SCE02013B.this.steam2.disp(false);
        }

        public void act23_spoon() {
            this.setTranslate(0.06f, -0.01f, 0.05f);
            this.setRotate(-54.0f, 261.95f, -35.0f);
            System.sleep(86);
            System.sleep(14);
            System.sleep(5);
            System.sleep(5);
            this.setVisible(0, false);
            System.sleep(32);
            this.setVisible(0, true);
        }

        public void act24_spoon() {
            this.setTranslate(0.06f, -0.01f, 0.04f);
            this.setRotate(-12.0f, 262.97f, 12.0f);
            System.sleep(25);
            this.setVisible(0, false);
            System.sleep(30);
            SCE02013B.this.ASmove(SCE02013B.this.spoon, 15, 0.06f, 0.0f, 0.04f, -16.0f, 266.97f, 20.0f);
            SCE02013B.this.ASmove(SCE02013B.this.spoon, 15, 0.06f, -0.01f, 0.04f, -16.0f, 260.97f, 14.0f);
            this.setVisible(0, true);
        }

        public void act25_tray() {
            this.setParent(SCE02013B.this.andrew, 60);
            this.setTranslate(0.21f, 0.06f, -0.13f);
            this.setRotate(-99.99f, 128.0f, -31.0f);
            this.setVisible(true);
            this.setVisible(1, false);
            this.setVisible(2, false);
            this.setVisible(3, false);
            SCE02013B.this.tray2.setVisible(false);
        }

        public void act26_tray1() {
            this.setTranslate(0.28f, -0.02f, -0.12f);
            this.setRotate(-125.0f, -179.0f, 340.0f);
            System.sleep(61);
            this.setVisible(false);
        }

        public void act26_tray2() {
            this.setParent(SCE02013B.this.shion, 72);
            this.setTranslate(0.19f, 0.14f, 0.16f);
            this.setRotate(-252.95f, 79.0f, 26.0f);
            this.setVisible(1, false);
            this.setVisible(2, false);
            this.setVisible(3, false);
            System.sleep(60);
            this.setVisible(true);
            SCE02013B.this.tray2.setVisible(7, true);
        }

        public void act27_tray() {
            this.setParent(SCE02013B.this.shion, 60);
            this.setTranslate(0.24f, 0.1f, -0.14f);
            this.setRotate(41.0f, 66.99f, -179.99f);
        }

        public void act2_handgun() {
            this.setTranslate(0.07f, -0.02f, 0.01f);
            this.setRotate(9.0f, 7.0f, -1.0f);
            this.setScale(1.0f, 1.0f, 1.0f);
            SCE02013B.this.andrew.setVisible(12, false);
            SCE02013B.this.andrew.setVisible(15, true);
        }

        public void act3_steam() {
            SCE02013B.this.steam1.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam1.disp(true);
            SCE02013B.this.steam2.setScale(0.0f, 0.0f, 0.0f);
            SCE02013B.this.steam2.disp(true);
            System.sleep(35);
            float f = 0.4f;
            float f2 = 0.0f;
            int n = 0;
            while (n < 40) {
                SCE02013B.this.steam1.setScale(f, f, f);
                SCE02013B.this.steam2.setScale(f2, f2, f2);
                f -= 0.01f;
                f2 += 0.01f;
                System.sleep(1);
                ++n;
            }
            SCE02013B.this.steam1.disp(false);
            SCE02013B.this.steam2.disp(true);
        }

        public void act3_tray() {
            this.setTranslate(0.18f, 0.14f, 0.1f);
            this.setRotate(-284.0f, 51.0f, 40.0f);
        }

        public void act4_steam() {
            SCE02013B.this.steam1.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam2.setScale(0.4f, 0.4f, 0.4f);
            SCE02013B.this.steam1.disp(true);
            SCE02013B.this.steam2.disp(false);
            System.sleep(175);
            SCE02013B.this.steam1.disp(false);
            SCE02013B.this.steam2.disp(true);
        }

        void act6_steam() {
            SCE02013B.this.steam1.disp(true);
            SCE02013B.this.steam2.disp(false);
        }

        public void act6_tray() {
            this.setTranslate(0.22f, 0.06f, 0.19f);
            this.setRotate(-280.0f, 55.0f, 61.0f);
        }

        public void act7_handgun() {
            System.sleep(105);
            this.setVisible(false);
        }

        public void act8_tray() {
            this.setTranslate(0.29f, -0.04f, 0.11f);
            this.setRotate(-264.0f, 21.0f, 30.0f);
        }

        public void act9_tray() {
            this.setTranslate(0.24f, -0.01f, 0.15f);
            this.setRotate(-256.0f, 23.0f, 44.0f);
        }

        public void fog_check() {
            SCE02013B.this.fog.setTranslate(0.0f, 0.0f, 0.0f);
            SCE02013B.this.fog.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                SCE02013B.this.cam1.setFog(1, SCE02013B.this.fog.px, SCE02013B.this.fog.pz, SCE02013B.this.fog.py, SCE02013B.this.fog.ry / 1000.0f, 128, 128, 128, 0);
                System.sleep(1);
            }
        }

        public void point_check0() {
            while (true) {
                SCE02013B.this.light1.setGlobalPointLightCol(0, SCE02013B.this.point0.rx / 100.0f, SCE02013B.this.point0.ry / 100.0f, SCE02013B.this.point0.rz / 100.0f);
                SCE02013B.this.light1.setGlobalPointLightPos(0, SCE02013B.this.point0.px, SCE02013B.this.point0.py, SCE02013B.this.point0.pz);
                System.sleep(1);
            }
        }

        public void point_check1() {
            while (true) {
                SCE02013B.this.light1.setGlobalPointLightCol(1, SCE02013B.this.point1.rx / 100.0f, SCE02013B.this.point1.ry / 100.0f, SCE02013B.this.point1.rz / 100.0f);
                SCE02013B.this.light1.setGlobalPointLightPos(1, SCE02013B.this.point1.px, SCE02013B.this.point1.py, SCE02013B.this.point1.pz);
                System.sleep(1);
            }
        }

        public void point_check2() {
            while (true) {
                SCE02013B.this.light1.setGlobalPointLightCol(2, SCE02013B.this.point2.rx / 100.0f, SCE02013B.this.point2.ry / 100.0f, SCE02013B.this.point2.rz / 100.0f);
                SCE02013B.this.light1.setGlobalPointLightPos(2, SCE02013B.this.point2.px, SCE02013B.this.point2.py, SCE02013B.this.point2.pz);
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

        void act1_door() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            System.sleep(80);
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[4] = 10.0f;
            fArray[5] = 0.1f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[8];
            fArray3[0] = 1.0f;
            fArray3[4] = 10.0f;
            float[] fArray4 = fArray3;
            float[] fArray5 = new float[12];
            fArray5[0] = 1.0f;
            fArray5[1] = 0.1f;
            fArray5[4] = 12.0f;
            fArray5[5] = 0.2f;
            fArray5[8] = 27.0f;
            fArray5[9] = 1.5f;
            float[] fArray6 = fArray5;
            float[] fArray7 = new float[8];
            fArray7[0] = 1.0f;
            fArray7[4] = 27.0f;
            float[] fArray8 = fArray7;
            SCE02013B.this.movSPL.setCtrlVertex(fArray2, 1, 1, 10);
            SCE02013B.this.rotSPL.setCtrlVertex(fArray4, 0, 2, 10);
            this.move(SCE02013B.this.movSPL, false);
            this.rotate(SCE02013B.this.rotSPL, true);
            SCE02013B.this.movSPL.setCtrlVertex(fArray6, 1, 0, 27);
            SCE02013B.this.rotSPL.setCtrlVertex(fArray8, 0, 1, 27);
            this.move(SCE02013B.this.movSPL, false);
            this.rotate(SCE02013B.this.rotSPL, true);
        }

        void act1_door_1() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            System.sleep(60);
            SCE02013B.this.ASmove(SCE02013B.this.door, 3, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02013B.this.ASmove(SCE02013B.this.door, 10, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02013B.this.ASmove(SCE02013B.this.door, 25, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        void act6_door() {
            SCE02013B.this.ASmove(SCE02013B.this.door, 30, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0() {
            SCE02013B.this.cam1.setTranslate(0.0f, -1.5f, 0.0f);
            SCE02013B.this.cam1.setRotate(-90.0f, 0.0f, 0.0f);
            SCE02013B.this.cam1.setFov(40.0f);
            Runtime.setDefocusQuick(0, 0, 12072, 4);
            Runtime.setDefocusQuick(1, 0, 12072, 4);
            Runtime.setDefocusQuick(2, 0, 12072, 4);
            Runtime.setDefocusQuick(3, 0, 12072, 4);
        }

        public void cut1() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.05f, 0.05f, 0.05f);
            SCE02013B.this.light.setColor(1, 0.16f, 0.25f, 0.28f);
            SCE02013B.this.light.setDirection2(1, -0.864f, -0.394f, -0.313f);
            SCE02013B.this.light.setColor(2, 0.14f, 0.19f, 0.23f);
            SCE02013B.this.light.setDirection2(2, 0.646f, 0.402f, -0.649f);
            SCE02013B.this.light.setColor(3, 0.11f, 0.18f, 0.24f);
            SCE02013B.this.light.setDirection2(3, 0.682f, 0.724f, 0.104f);
            Stage.setColor(0.16f, 0.23f, 0.34f);
            SCE02013B.this.cam1.setTranslate(0.6f, 1.1f, -0.67f);
            SCE02013B.this.cam1.setRotate(0.0f, 161.94f, 0.0f);
            SCE02013B.this.cam1.setFov(38.04f);
        }

        public void cut10() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.872f, 0.438f, 0.22f);
            SCE02013B.this.light.setColor(2, 0.44f, 0.44f, 0.44f);
            SCE02013B.this.light.setDirection2(2, 0.654f, 0.57f, 0.497f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, -0.24f, -0.879f, 0.411f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.15f, 1.28f, 2.12f);
            SCE02013B.this.cam1.setRotate(9.6f, 333.26f, 0.0f);
            SCE02013B.this.cam1.setFov(29.86f);
        }

        public void cut11() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.58f, 0.58f, 0.58f);
            SCE02013B.this.light.setDirection2(1, -0.529f, 0.344f, 0.776f);
            SCE02013B.this.light.setColor(2, 0.39f, 0.39f, 0.39f);
            SCE02013B.this.light.setDirection2(2, -0.529f, 0.025f, -0.848f);
            SCE02013B.this.light.setColor(3, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(3, -0.651f, -0.758f, -0.032f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.94f, 0.95f, 0.82f);
            SCE02013B.this.cam1.setFov(40.0f);
            float[] fArray = new float[20];
            fArray[0] = 1.0f;
            fArray[1] = -1.7f;
            fArray[2] = 238.25f;
            fArray[4] = 30.0f;
            fArray[5] = -1.7f;
            fArray[6] = 238.25f;
            fArray[8] = 65.0f;
            fArray[9] = -1.0f;
            fArray[10] = 238.25f;
            fArray[12] = 165.0f;
            fArray[13] = 19.0f;
            fArray[14] = 255.0f;
            fArray[16] = 195.0f;
            fArray[17] = 20.0f;
            fArray[18] = 255.5f;
            float[] fArray2 = fArray;
            SCE02013B.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut12() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.57f, 0.57f, 0.57f);
            SCE02013B.this.light.setDirection2(1, 0.748f, 0.658f, -0.083f);
            SCE02013B.this.light.setColor(2, 0.25f, 0.25f, 0.25f);
            SCE02013B.this.light.setDirection2(2, -0.81f, 0.463f, -0.359f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, 0.561f, -0.001f, 0.828f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setRotate(-47.73f, 145.35f, 0.0f);
            SCE02013B.this.cam1.setFov(39.75f);
            float[] fArray = new float[]{1.0f, -0.28f, 1.24f, 0.93f, 120.0f, -0.31f, 1.24f, 0.91f};
            SCE02013B.this.cam1.transSPL(fArray, 1, 2, 120);
        }

        public void cut13() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.58f, 0.58f, 0.58f);
            SCE02013B.this.light.setDirection2(1, -1.0f, 0.0f, 0.028f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, 0.627f, 0.474f, 0.618f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, -0.323f, -0.916f, 0.239f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.77f, 0.96f, 1.21f);
            SCE02013B.this.cam1.setRotate(20.56f, 283.03f, 0.0f);
            SCE02013B.this.cam1.setFov(39.4f);
        }

        public void cut13_99() {
            SCE02013B.this.light.setColor(0, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setColor(1, 0.2f, 0.2f, 0.2f);
            SCE02013B.this.light.setDirection2(1, -0.906f, 0.226f, 0.357f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.light.setColor(2, 0.19f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(2, -0.897f, 0.0f, -0.442f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.light.setColor(3, 0.18f, 0.18f, 0.13f);
            SCE02013B.this.light.setDirection2(3, -0.798f, -0.0f, -0.603f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        public void cut14() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.783f, 0.268f, -0.561f);
            SCE02013B.this.light.setColor(2, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(2, -0.649f, 0.527f, -0.549f);
            SCE02013B.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE02013B.this.light.setDirection2(3, -0.068f, -0.752f, -0.656f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.05f, 1.37f, 0.0f);
            SCE02013B.this.cam1.setRotate(-6.17f, 144.69f, 0.0f);
            SCE02013B.this.cam1.setFov(31.4f);
        }

        public void cut15() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.54f, 0.54f, 0.54f);
            SCE02013B.this.light.setDirection2(1, -0.2f, 0.14f, 0.97f);
            SCE02013B.this.light.setColor(2, 0.3f, 0.3f, 0.3f);
            SCE02013B.this.light.setDirection2(2, -0.05f, 0.526f, -0.849f);
            SCE02013B.this.light.setColor(3, 0.09f, 0.09f, 0.09f);
            SCE02013B.this.light.setDirection2(3, -0.701f, -0.701f, -0.129f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-1.21f, 1.74f, 0.86f);
            SCE02013B.this.cam1.setRotate(-21.3f, 264.32f, 0.0f);
            SCE02013B.this.cam1.setFov(25.32f);
            float[] fArray = new float[12];
            fArray[0] = 1.0f;
            fArray[1] = SCE02013B.this.cam1.getRotateX();
            fArray[2] = SCE02013B.this.cam1.getRotateY();
            fArray[3] = SCE02013B.this.cam1.getRotateZ();
            fArray[4] = 115.0f;
            fArray[5] = SCE02013B.this.cam1.getRotateX();
            fArray[6] = SCE02013B.this.cam1.getRotateY();
            fArray[7] = SCE02013B.this.cam1.getRotateZ();
            fArray[8] = 205.0f;
            fArray[9] = -26.74f;
            fArray[10] = 261.48f;
            float[] fArray2 = fArray;
            SCE02013B.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut15_1() {
            float[] fArray = new float[]{1.0f, SCE02013B.this.cam1.getTranslateX(), SCE02013B.this.cam1.getTranslateY(), SCE02013B.this.cam1.getTranslateZ(), 100.0f, -1.21f, 1.74f, 0.86f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = SCE02013B.this.cam1.getRotateX();
            fArray2[2] = SCE02013B.this.cam1.getRotateY();
            fArray2[3] = SCE02013B.this.cam1.getRotateZ();
            fArray2[4] = 100.0f;
            fArray2[5] = -38.24f;
            fArray2[6] = 260.44f;
            float[] fArray3 = fArray2;
            SCE02013B.this.cam1.transSPL(fArray, 1);
            SCE02013B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut16() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.828f, 0.458f, -0.324f);
            SCE02013B.this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            SCE02013B.this.light.setDirection2(2, -0.674f, 0.455f, -0.582f);
            SCE02013B.this.light.setColor(3, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(3, -0.146f, -0.905f, -0.4f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.42f, 0.7f, 2.83f);
            SCE02013B.this.cam1.setRotate(7.5f, 149.8f, 0.0f);
            SCE02013B.this.cam1.setFov(40.0f);
        }

        public void cut17() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.44f, 0.44f, 0.44f);
            SCE02013B.this.light.setDirection2(1, -0.771f, 0.0f, 0.636f);
            SCE02013B.this.light.setColor(2, 0.23f, 0.23f, 0.23f);
            SCE02013B.this.light.setDirection2(2, -0.109f, 0.993f, 0.033f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, -0.093f, -0.768f, 0.633f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.1f, 1.37f, 2.4f);
            SCE02013B.this.cam1.setFov(26.4f);
            float[] fArray = new float[24];
            fArray[0] = 1.0f;
            fArray[1] = -4.44f;
            fArray[2] = 310.38f;
            fArray[4] = 55.0f;
            fArray[5] = -4.44f;
            fArray[6] = 310.38f;
            fArray[8] = 60.0f;
            fArray[9] = -4.4f;
            fArray[10] = 310.38f;
            fArray[12] = 175.0f;
            fArray[13] = 0.3f;
            fArray[14] = 310.38f;
            fArray[16] = 210.0f;
            fArray[17] = 0.37f;
            fArray[18] = 310.38f;
            fArray[20] = 245.0f;
            fArray[21] = 0.35f;
            fArray[22] = 310.38f;
            float[] fArray2 = fArray;
            SCE02013B.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut18() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.612f, 0.476f, 0.631f);
            SCE02013B.this.light.setColor(2, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(2, 0.969f, 0.241f, 0.053f);
            SCE02013B.this.light.setColor(3, 0.17f, 0.17f, 0.17f);
            SCE02013B.this.light.setDirection2(3, -0.088f, -0.833f, 0.546f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-3.69f, 1.18f, 5.09f);
            SCE02013B.this.cam1.setRotate(0.57f, 314.55f, 0.0f);
            SCE02013B.this.cam1.setFov(34.0f);
        }

        public void cut19() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.43f, 0.43f, 0.43f);
            SCE02013B.this.light.setDirection2(1, -0.29f, 0.892f, 0.346f);
            SCE02013B.this.light.setColor(2, 0.2f, 0.2f, 0.2f);
            SCE02013B.this.light.setDirection2(2, -0.934f, -0.165f, 0.316f);
            SCE02013B.this.light.setColor(3, 0.33f, 0.33f, 0.33f);
            SCE02013B.this.light.setDirection2(3, -0.481f, -0.241f, -0.843f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-1.4f, 1.5f, 1.93f);
            SCE02013B.this.cam1.setRotate(-4.61f, 251.21f, 0.0f);
            SCE02013B.this.cam1.setFov(32.4f);
        }

        public void cut1_1() {
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.426f, 0.462f, -0.778f);
            SCE02013B.this.light.setColor(2, 0.19f, 0.19f, 0.19f);
            SCE02013B.this.light.setDirection2(2, 0.763f, 0.553f, -0.335f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, -0.139f, -0.919f, -0.369f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
        }

        public void cut2() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.612f, 0.476f, 0.631f);
            SCE02013B.this.light.setColor(2, 0.31f, 0.31f, 0.31f);
            SCE02013B.this.light.setDirection2(2, 0.819f, 0.57f, 0.061f);
            SCE02013B.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE02013B.this.light.setDirection2(3, 0.61f, -0.684f, 0.401f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(1.08f, 1.58f, 2.16f);
            SCE02013B.this.cam1.setRotate(-4.24f, 31.34f, 0.0f);
            SCE02013B.this.cam1.setFov(34.6f);
        }

        public void cut21() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.547f, 0.52f, 0.656f);
            SCE02013B.this.light.setColor(2, 0.34f, 0.34f, 0.34f);
            SCE02013B.this.light.setDirection2(2, -0.969f, 0.0f, 0.246f);
            SCE02013B.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setDirection2(3, -0.284f, -0.811f, 0.512f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.26f, 1.5f, 4.17f);
            SCE02013B.this.cam1.setRotate(-8.28f, 314.4f, 0.0f);
            SCE02013B.this.cam1.setFov(35.2f);
        }

        public void cut22() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.828f, 0.458f, -0.324f);
            SCE02013B.this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            SCE02013B.this.light.setDirection2(2, -0.674f, 0.455f, -0.582f);
            SCE02013B.this.light.setColor(3, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(3, -0.146f, -0.905f, -0.4f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.45f, 1.08f, 1.81f);
            SCE02013B.this.cam1.setRotate(3.04f, 124.67f, 0.0f);
            SCE02013B.this.cam1.setFov(36.16f);
        }

        public void cut23() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.305f, 0.287f, -0.908f);
            SCE02013B.this.light.setColor(2, 0.22f, 0.22f, 0.22f);
            SCE02013B.this.light.setDirection2(2, -0.932f, 0.331f, 0.147f);
            SCE02013B.this.light.setColor(3, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(3, -0.584f, -0.782f, -0.218f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-3.94f, 1.02f, 2.58f);
            SCE02013B.this.cam1.setRotate(2.32f, 208.96f, 0.0f);
            SCE02013B.this.cam1.setFov(28.28f);
        }

        public void cut24() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, 0.926f, 0.375f, -0.053f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, -0.585f, 0.351f, 0.731f);
            SCE02013B.this.light.setColor(3, 0.16f, 0.16f, 0.16f);
            SCE02013B.this.light.setDirection2(3, 0.309f, -0.683f, 0.662f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-1.84f, 1.02f, 5.83f);
            SCE02013B.this.cam1.setRotate(-1.62f, 410.31f, 0.0f);
            SCE02013B.this.cam1.setFov(30.6f);
        }

        public void cut25() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02013B.this.light.setDirection2(1, 0.484f, 0.001f, -0.875f);
            SCE02013B.this.light.setColor(2, 0.28f, 0.28f, 0.28f);
            SCE02013B.this.light.setDirection2(2, 0.369f, 0.339f, 0.865f);
            SCE02013B.this.light.setColor(3, 0.17f, 0.17f, 0.17f);
            SCE02013B.this.light.setDirection2(3, 0.395f, -0.834f, 0.385f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.3f, 1.12f, 5.39f);
            SCE02013B.this.cam1.setRotate(-4.39f, 423.26f, 0.0f);
            SCE02013B.this.cam1.setFov(32.16f);
        }

        public void cut26() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02013B.this.light.setDirection2(1, 0.686f, 0.001f, 0.728f);
            SCE02013B.this.light.setColor(2, 0.37f, 0.37f, 0.37f);
            SCE02013B.this.light.setDirection2(2, -0.93f, 0.356f, 0.085f);
            SCE02013B.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE02013B.this.light.setDirection2(3, -0.219f, -0.918f, 0.33f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-4.06f, 1.11f, 5.39f);
            SCE02013B.this.cam1.setRotate(6.22f, 303.1f, 0.0f);
            SCE02013B.this.cam1.setFov(28.28f);
            float[] fArray = new float[]{1.0f, -4.06f, 1.11f, 5.39f, 60.0f, -4.06f, 1.11f, 5.39f, 195.0f, -3.85f, 1.06f, 5.65f, 225.0f, -3.84f, 1.05f, 5.66f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.22f;
            fArray2[2] = 303.1f;
            fArray2[4] = 60.0f;
            fArray2[5] = 6.22f;
            fArray2[6] = 303.1f;
            fArray2[8] = 195.0f;
            fArray2[9] = 2.07f;
            fArray2[10] = 308.18f;
            fArray2[12] = 225.0f;
            fArray2[13] = 2.0f;
            fArray2[14] = 308.18f;
            float[] fArray3 = fArray2;
            SCE02013B.this.cam1.transSPL(fArray, 1);
            SCE02013B.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut27() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02013B.this.light.setDirection2(1, 0.786f, 0.487f, -0.381f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, -0.072f, 0.691f, 0.719f);
            SCE02013B.this.light.setColor(3, 0.13f, 0.13f, 0.13f);
            SCE02013B.this.light.setDirection2(3, 0.311f, -0.645f, 0.698f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.42f, 1.14f, 5.42f);
            SCE02013B.this.cam1.setRotate(-29.58f, 57.75f, 0.0f);
            SCE02013B.this.cam1.setFov(27.4f);
        }

        public void cut28() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02013B.this.light.setDirection2(1, 0.62f, 0.459f, -0.636f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, 0.209f, 0.013f, 0.978f);
            SCE02013B.this.light.setColor(3, 0.21f, 0.21f, 0.21f);
            SCE02013B.this.light.setDirection2(3, 0.615f, -0.762f, 0.203f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-2.62f, 0.95f, 5.47f);
            SCE02013B.this.cam1.setRotate(3.63f, 46.4f, 0.0f);
            SCE02013B.this.cam1.setFov(32.0f);
            float[] fArray = new float[20];
            fArray[0] = 1.0f;
            fArray[1] = 1.96f;
            fArray[2] = 49.78f;
            fArray[4] = 50.0f;
            fArray[5] = 1.96f;
            fArray[6] = 49.78f;
            fArray[8] = 80.0f;
            fArray[9] = 3.1f;
            fArray[10] = 48.36f;
            fArray[12] = 150.0f;
            fArray[13] = 10.55f;
            fArray[14] = 48.45f;
            fArray[16] = 165.0f;
            fArray[17] = 10.55f;
            fArray[18] = 48.45f;
            float[] fArray2 = fArray;
            SCE02013B.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut3() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.35f, 0.35f, 0.35f);
            SCE02013B.this.light.setDirection2(1, -0.26f, 0.756f, -0.601f);
            SCE02013B.this.light.setColor(2, 0.27f, 0.27f, 0.27f);
            SCE02013B.this.light.setDirection2(2, 0.787f, 0.008f, -0.617f);
            SCE02013B.this.light.setColor(3, 0.34f, 0.34f, 0.34f);
            SCE02013B.this.light.setDirection2(3, -0.564f, -0.487f, -0.667f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.71f, 1.29f, 6.54f);
            SCE02013B.this.cam1.setRotate(-0.58f, 217.01f, 0.0f);
            SCE02013B.this.cam1.setFov(22.4f);
        }

        public void cut4() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.612f, 0.476f, 0.631f);
            SCE02013B.this.light.setColor(2, 0.31f, 0.31f, 0.31f);
            SCE02013B.this.light.setDirection2(2, 0.819f, 0.57f, 0.061f);
            SCE02013B.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE02013B.this.light.setDirection2(3, 0.61f, -0.684f, 0.401f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.69f, 1.13f, 10.02f);
            SCE02013B.this.cam1.setRotate(2.92f, 354.95f, 0.0f);
            SCE02013B.this.cam1.setFov(30.8f);
        }

        public void cut5() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.shion.setLightMode(0);
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.604f, 0.591f, 0.534f);
            SCE02013B.this.light.setColor(2, 0.26f, 0.26f, 0.26f);
            SCE02013B.this.light.setDirection2(2, 0.606f, 0.648f, 0.461f);
            SCE02013B.this.light.setColor(3, 0.12f, 0.12f, 0.12f);
            SCE02013B.this.light.setDirection2(3, 0.021f, -0.929f, 0.371f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.17f, 1.83f, 1.83f);
            SCE02013B.this.cam1.setRotate(-17.94f, 354.73f, 0.0f);
            SCE02013B.this.cam1.setFov(35.2f);
        }

        public void cut6() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.694f, 0.299f, -0.655f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, 0.969f, 0.243f, -0.042f);
            SCE02013B.this.light.setColor(3, 0.16f, 0.16f, 0.16f);
            SCE02013B.this.light.setDirection2(3, -0.258f, -0.81f, -0.526f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.21f, 1.44f, 3.04f);
            SCE02013B.this.cam1.setRotate(-2.18f, 563.22f, 0.0f);
            SCE02013B.this.cam1.setFov(35.4f);
        }

        public void cut7() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.15f, 0.15f, 0.15f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.604f, 0.592f, 0.534f);
            SCE02013B.this.light.setColor(2, 0.26f, 0.26f, 0.26f);
            SCE02013B.this.light.setDirection2(2, 0.606f, 0.648f, 0.461f);
            SCE02013B.this.light.setColor(3, 0.12f, 0.12f, 0.12f);
            SCE02013B.this.light.setDirection2(3, 0.021f, -0.928f, 0.371f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.26f, 1.77f, 1.84f);
            SCE02013B.this.cam1.setRotate(-12.54f, 726.34f, 0.0f);
            SCE02013B.this.cam1.setFov(35.8f);
        }

        public void cut8() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02013B.this.light.setDirection2(1, -0.46f, 0.26f, -0.849f);
            SCE02013B.this.light.setColor(2, 0.24f, 0.24f, 0.24f);
            SCE02013B.this.light.setDirection2(2, 0.969f, 0.243f, -0.042f);
            SCE02013B.this.light.setColor(3, 0.16f, 0.16f, 0.16f);
            SCE02013B.this.light.setDirection2(3, -0.684f, -0.73f, -0.021f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(0.26f, 1.77f, 1.84f);
            SCE02013B.this.cam1.setRotate(-12.54f, 726.34f, 0.0f);
            SCE02013B.this.cam1.setFov(35.8f);
        }

        public void cut9() {
            SCE02013B.this.Timechk_CutChange();
            SCE02013B.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE02013B.this.light.setColor(1, 0.62f, 0.62f, 0.62f);
            SCE02013B.this.light.setDirection2(1, -0.542f, 0.661f, -0.519f);
            SCE02013B.this.light.setColor(2, 0.25f, 0.25f, 0.25f);
            SCE02013B.this.light.setDirection2(2, 0.748f, 0.02f, -0.664f);
            SCE02013B.this.light.setColor(3, 0.2f, 0.2f, 0.2f);
            SCE02013B.this.light.setDirection2(3, -0.706f, -0.636f, -0.312f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            SCE02013B.this.cam1.setTranslate(-0.44f, 1.28f, 0.01f);
            SCE02013B.this.cam1.setFov(32.1f);
            float[] fArray = new float[12];
            fArray[0] = 1.0f;
            fArray[1] = 2.54f;
            fArray[2] = 213.51f;
            fArray[4] = 102.75f;
            fArray[5] = 3.44f;
            fArray[6] = 216.4f;
            fArray[8] = 120.0f;
            fArray[9] = 3.44f;
            fArray[10] = 216.4f;
            float[] fArray2 = fArray;
            SCE02013B.this.cam1.rotateSPL(fArray2, 1);
        }
    }

    class Units
            extends Unit {
        Units() {
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

