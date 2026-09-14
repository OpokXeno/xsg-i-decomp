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
import xeno.map.MC_VOK23_PRJ;
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

class SCE01001
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01001,
        MC_VOK23_PRJ,
        JNT_Human,
        FLSshion_h,
        FLSkosmos_h,
        FLSshion,
        FLSkosmos {
    allCHARA shion;
    allCHARA shionM;
    allCHARA kosmos;
    allCHARA kosmosM;
    allCHARA dummy;
    allCHARA shadow;
    Unit camera_view;
    Chr Fshion;
    Chr Fkosmos;
    Chr Fshion1;
    Chr Fkosmos1;
    Chr Fallen;
    Chr Fstaff1;
    MAPUnit sky;
    Spline movSPL;
    Spline rotSPL;
    Spline movSPL2;
    Spline rotSPL2;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera cam4;
    Camerawork camerawork = new Camerawork();
    Light light = new Light(0);
    int menuSelected;
    int msg_count = 1;
    Window win0;
    Window win1;
    Window win2;
    float next_px;
    float next_py;
    float next_pz;
    Effect fadeIn_white;
    Effect fadeOut_white;
    Effect flash;
    Effect flash2;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
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

    SCE01001() {
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
        this.Objchk_key1 = 2048;
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
        this.msg.waitkey(this.msg_count);
        System.waitSignal(this.msg, this.msg_count++);
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

    void Monitor_off() {
    }

    void Monitor_on() {
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
    }

    void SETcheck(String string, Unit unit) {
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
            this.shion.setVisible(false);
            this.kosmos.setVisible(false);
        }
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01001_F");
        Runtime.setFlags(1, 1, 1);
        System.println("XEVEJNAME:CFJ1_10 XEVEJPOINT:POINT_10");
        Runtime.jumpCF(210, 1);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(210, 1);
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
        Runtime.setLocation(38);
        this.shion = new allCHARA(30);
        this.shionM = new allCHARA(1);
        this.kosmos = new allCHARA(37);
        this.kosmosM = new allCHARA(14);
        this.dummy = new allCHARA(24582);
        this.shadow = new allCHARA(24582);
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.shionM.setVisible(false);
        this.kosmosM.setVisible(false);
        this.dummy.setVisible(false);
        this.shadow.setVisible(false);
        this.camera_view = new allUNIT(20482, 0.0f, 10.9f, 0.0f, 0.0f);
        this.camera_view.setVisible(false);
        this.Fshion = new allFACE();
        this.Fkosmos = new allFACE();
        this.Fshion1 = new allFACE();
        this.Fkosmos1 = new allFACE();
        this.Fallen = new allFACE();
        this.Fstaff1 = new allFACE();
        this.sky = new allMAP(35);
        this.sky.setPivot(0.0f, 0.0f, 0.0f);
        this.movSPL = Spline.create();
        this.rotSPL = Spline.create();
        this.movSPL2 = Spline.create();
        this.rotSPL2 = Spline.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam4 = Camera.create(4);
        this.cam0.start(3, null);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, -10.0f, 0.0f);
        this.cam1.setRotate(-180.0f, 0.0f, 0.0f);
        this.cam1.setFov(40.0f);
        this.fadeIn_white = new Effect(0);
        this.fadeIn_white.args[0] = -2130706433;
        this.fadeIn_white.args[1] = 30;
        this.fadeIn_white.args[2] = 0;
        this.fadeOut_white = new Effect(0);
        this.fadeOut_white.args[0] = -2130706433;
        this.fadeOut_white.args[1] = 30;
        this.fadeOut_white.args[2] = 1;
        this.flash = new Effect(0);
        this.flash.args[0] = -2130706433;
        this.flash.args[1] = 5;
        this.flash.args[2] = 1;
        this.flash2 = new Effect(0);
        this.flash2.args[0] = -2130706433;
        this.flash2.args[1] = 7;
        this.flash2.args[2] = 1;
        this.EF01 = new Effect(1515, 10.443f, 0.0f, -4.684f, 0.0f);
        this.EF01.disp(true);
        this.EF01.setScale(0.4f, 0.8f, 0.4f);
        this.EF01.setClip(true);
        this.EF02 = new Effect(1402, 7.751f, 0.0f, 6.093f, 0.0f);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1518, -13.145f, 0.0f, -14.699f, 0.0f);
        this.EF03.disp(true);
        this.EF03.setScale(0.8f, 0.8f, 0.8f);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1402, -7.005f, 0.0f, -5.338f, 0.0f);
        this.EF04.disp(true);
        this.EF04.setScale(0.6f, 0.5f, 0.3f);
        this.EF04.setClip(true);
        this.EF05 = new Effect(1571, -5.5f, 8.2f, -17.5f, 0.0f);
        this.EF05.disp(true);
        this.EF05.setRotate(90.0f, 0.0f, 0.0f);
        this.EF05.setClip(true);
        this.EF06 = new Effect(1571, 0.5f, 8.2f, -17.5f, 0.0f);
        this.EF06.disp(true);
        this.EF06.setRotate(90.0f, 0.0f, 0.0f);
        this.EF06.setClip(true);
        this.EF07 = new Effect(1571, 9.0f, 7.0f, -16.4f, 0.0f);
        this.EF07.disp(true);
        this.EF07.setRotate(90.0f, 0.0f, 90.0f);
        this.EF07.setClip(true);
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
            this.Objchk(this.Fshion);
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
        Stage.setEffectRender(0);
        this.CameraTool();
        this.CaptureTool();
        this.Timechk();
        this.STCamera_init();
        this.cam1.change();
        this.camerawork.cut0();
        this.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos_h.fpk");
        this.loadarc(this.shionM.face, "FLSshion.fpk");
        this.loadarc(this.kosmosM.face, "FLSkosmos.fpk");
        this.playSCENE2();
        this.playSCENE3();
        this.playSCENE4();
        this.playSCENE5();
        this.Timechk_SceneEnd();
    }

    void playAGAIN() {
        this.KEYwait("スタート");
        this.KEYwait("スタート　０→９");
        this.mapCHECK(0, 10);
        this.KEYwait("スタート　１０→１９");
        this.mapCHECK(10, 20);
        this.KEYwait("スタート　２０→２９");
        this.mapCHECK(20, 30);
        this.KEYwait("スタート　３０→３９");
        this.mapCHECK(30, 40);
        this.KEYwait("スタート　４０→４９");
        this.mapCHECK(40, 50);
        this.KEYwait("スタート　５０→５９");
        this.mapCHECK(50, 60);
        this.KEYwait("スタート　６０→６９");
        this.mapCHECK(60, 70);
        this.KEYwait("スタート　７０→７９");
        this.mapCHECK(70, 80);
        this.KEYwait("スタート　８０→８９");
        this.mapCHECK(80, 90);
        this.KEYwait("スタート　９０→９９");
        this.mapCHECK(90, 100);
        this.KEYwait("スタート　１００→１０９");
        this.mapCHECK(100, 110);
        this.KEYwait("スタート　１１０→１１９");
        this.mapCHECK(110, 120);
        this.KEYwait("スタート　１２０→１２９");
        this.mapCHECK(120, 130);
        this.KEYwait("スタート　１３０→１３９");
        this.mapCHECK(130, 140);
        this.KEYwait("スタート　１４０→１４９");
        this.mapCHECK(140, 150);
        this.KEYwait("スタート　１５０→１５９");
        this.mapCHECK(150, 160);
        this.KEYwait("スタート　１６０→１６９");
        this.mapCHECK(160, 170);
        this.KEYwait("スタート　１７０→１７９");
        this.mapCHECK(170, 180);
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
        this.SETcheck("シオン", this.shion);
        this.SETcheck("コスモス", this.kosmos);
    }

    void playSCENE1() {
    }

    void playSCENE2() {
        Runtime.mpeg2("1001_1");
        Sound.streamPlay(1191000, 48000);
        this.shion.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.shion.start(1, "act5_shion");
        this.kosmos.start(1, "act5_kosmos");
        this.kosmos.setVisible(true);
        this.Fshion.start(1, "face5_shion0");
        this.Fkosmos.start(1, "face5_kosmos");
        this.camerawork.cut5();
        System.sleep(60);
        this.msg_print("【シオン】", "Morning, KOS-MOS.\nHow do you feel?");
        System.sleep(42);
        System.sleep(25);
        this.waitclear(28);
        System.sleep(5);
        this.Fshion.start(1, "face5_shion2");
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "Good morning, Shion.\nAll systems are normal.");
        System.sleep(61);
        System.sleep(10);
        this.waitclear(59);
        System.sleep(5);
        Stage.renderCommand(4);
        this.shion.renderCommand(534);
        this.kosmos.renderCommand(534);
        this.shionM.renderCommand(534);
        this.kosmosM.renderCommand(534);
        this.shion.setMotionFlags(Integer.MIN_VALUE, false);
        this.kosmos.setMotionFlags(Integer.MIN_VALUE, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.shadow.start(1, "act6_shadow");
        this.dummy.start(1, "act6_chara");
        this.shion.start(1, "act6_shion");
        this.shionM.start(1, "act6_shionM");
        this.kosmos.start(1, "act6_kosmos");
        this.Fshion.start(1, "face6_shion");
        this.Fkosmos.start(1, "face6_kosmos");
        this.camerawork.cut6();
        System.sleep(5);
        this.msg_print("【シオン】", "Well, how about\nintroducing yourself?");
        this.waitclear(70);
        System.sleep(30);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "I am an Anti-Gnosis Humanoid\nFighting System, serial number\n00-00-00-00-1.");
        this.waitclear(285);
        System.sleep(15);
        this.msg.print("Development name KP-X.");
        this.waitclear(70);
        System.sleep(10);
        this.msg.print("Abbreviated name KOS-MOS.");
        this.waitclear(65);
        System.sleep(20);
        this.msg.print("As I am currently configured for\nsimulated battle, my output is limited\nto 22% of its normal capacity.");
        this.waitclear(210);
        System.sleep(10);
        this.msg.print("My estimated weapons\nspecifications are...");
        this.waitclear(55);
        this.Fshion.start(1, "face6_shion2");
        this.Fkosmos.start(1, "face6_kosmos2");
        this.msg_print("【シオン】", "All right, that's good enough.");
        this.waitclear(49);
        System.sleep(15);
        this.msg.print("Thanks.");
        this.waitclear(21);
        System.sleep(15);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "You are welcome.");
        this.waitclear(25);
        System.sleep(5);
    }

    void playSCENE3() {
        this.shadow.start(1, "act7_shadow");
        this.shion.start(1, "act7_shion");
        this.shionM.start(1, "act7_shionM");
        this.kosmos.start(1, "act7_kosmos");
        this.sky.start(1, "act7_sky");
        this.Fshion.start(1, "face7_shion");
        this.camerawork.cut7();
        this.msg_print("【シオン】", "All I have for you today are\nthe usual startup tests. Sorry to\nwake you up just for that.");
        this.waitclear(125);
        System.sleep(30);
        this.msg.print("You'll have to go back to sleep\nonce everything's checked out.");
        System.sleep(85);
        this.kosmos.start(1, "act8_kosmos");
        this.Fkosmos.start(1, "face8_kosmos");
        this.camerawork.cut8();
        this.waitclear(50);
        System.sleep(20);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "I see.");
        this.waitclear(35);
        System.sleep(30);
        this.shion.start(1, "act9_shion");
        this.Fshion.start(1, "face9_shion2");
        this.camerawork.cut7();
        System.sleep(20);
        this.msg_print("【シオン】", "Do you feel sad...or anything?");
        this.waitclear(85);
        System.sleep(5);
        this.dummy.start(1, "act10_chara");
        this.shadow.start(1, "act10_shadow");
        this.shionM.start(1, "act10_shionM");
        this.kosmosM.start(1, "act10_kosmosM");
        this.shion.start(1, "reset_pos");
        this.kosmos.start(1, "reset_pos");
        this.sky.start(1, "act10_sky");
        this.camerawork.cut10();
        System.sleep(10);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "A predetermined set of emotions\nhas been hard-coded into");
        this.waitclear(93);
        this.msg.print("my emotion module to better facilitate\ninteractions with humans.");
        this.waitclear(77);
        System.sleep(35);
        this.msg.print("In order to better facilitate\na relationship with you --");
        this.waitclear(83);
        this.msg.print("Chief Engineer Shion Uzuki of\nthe KOS-MOS Project,");
        this.waitclear(138);
        this.msg.print("Vector Industries\nFirst R&D Division --");
        this.waitclear(80);
        this.msg.print("I will emit an expression such as\nsadness, only when");
        this.waitclear(95);
        this.msg.print("that response is deemed necessary.\nHowever,");
        this.waitclear(136);
        System.sleep(11);
        this.msg.print("the emotion module of my program\nhas determined that this is\nnot necessary at this time.");
        this.waitclear(105);
        System.sleep(12);
        System.sleep(15);
        Stage.renderCommand(0);
        this.shion.renderCommand(0);
        this.kosmos.renderCommand(0);
        this.dummy.start(1, "act11_chara");
        this.shion.start(1, "act11_shion");
        this.kosmos.start(1, "act11_kosmos");
        this.Fshion.start(1, "face11_shion");
        this.Fkosmos.start(1, "face11_kosmos");
        this.camerawork.cut11();
        System.sleep(65);
        this.msg.print("I guess you're right.");
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("I of all people should know that.");
        this.waitclear(85);
        System.sleep(10);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "Your understanding is appreciated.");
        this.waitclear(65);
        System.sleep(45);
        this.msg_print("【シオン】", "But you know, KOS-MOS...");
        this.waitclear(50);
        System.sleep(20);
        this.msg_print("【シオン】", "I've got...mixed feelings about all this.");
        this.waitclear(85);
        System.sleep(20);
        this.kosmos.start(1, "act12_kosmos");
        this.camerawork.cut12();
        System.sleep(60);
    }

    void playSCENE4() {
        this.shion.start(1, "act13_shion");
        this.kosmos.start(1, "act13_kosmos");
        this.Fshion.start(1, "face13_shion");
        this.camerawork.cut13();
        System.sleep(5);
        this.msg_print("【シオン】", "Of course, I'm happy that you're\nawake...But the fact that");
        this.waitclear(110);
        this.msg.print("you'll go back to sleep makes me\na bit...sad.");
        this.waitclear(120);
        System.sleep(10);
        this.shion.renderCommand(22);
        this.kosmos.renderCommand(22);
        this.shion.start(1, "act14_shion");
        this.kosmos.start(1, "act14_kosmos");
        this.Fshion.start(1, "face14_shion");
        this.camerawork.cut14();
        this.msg.print("On the other hand,\nthe next time you wake up...");
        this.waitclear(100);
        this.msg.print("it may be a time of much bloodshed.");
        this.waitclear(110);
        System.sleep(10);
        this.Fshion.start(1, "face14_shion2");
        this.msg.print("So deep down inside,\nI hope that day never comes...");
        this.waitclear(160);
        System.sleep(5);
        this.shion.renderCommand(0);
        this.kosmos.renderCommand(0);
        this.kosmos.start(1, "act15_kosmos");
        this.camerawork.cut15();
        System.sleep(10);
        this.waitclear(50);
        System.sleep(15);
        this.shion.start(1, "act16_shion");
        this.kosmos.start(1, "act16_kosmos");
        this.Fshion.start(1, "face16_shion");
        this.camerawork.cut13();
        System.sleep(40);
        this.msg_print("【シオン】", "Understand?");
        this.waitclear(25);
        System.sleep(25);
        this.kosmos.start(1, "act17_kosmos");
        this.Fkosmos.start(1, "face17_kosmos");
        this.camerawork.cut15();
        System.sleep(15);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "The algorithms I have been\nprogrammed with do not support");
        this.waitclear(95);
        this.msg.print("the comprehension of\nillogical human thought.");
        this.waitclear(110);
        System.sleep(15);
    }

    void playSCENE5() {
        this.shion.start(1, "act18_shion");
        this.kosmos.start(1, "act18_kosmos");
        this.Fshion.start(1, "face18_shion");
        this.camerawork.cut18();
        this.msg_print("【シオン】", "Well...I hope you'll be able to...\nunderstand someday.");
        System.sleep(61);
        this.waitclear(144);
        this.Fshion.start(1, "face18_shion2");
        this.Fkosmos.start(1, "face18_kosmos2");
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "I will do my best.");
        this.waitclear(35);
        System.sleep(60);
        this.msg_print("【シオン】", "All right, KOS-MOS,\nshall we get started?");
        this.waitclear(70);
        System.sleep(20);
        this.msg.print("Allen, let's pick up from\nprocess 277 where we last left off.");
        this.waitclear(130);
        System.sleep(5);
        this.msg_print("【アレン】", "Roger.");
        this.waitclear(25);
        System.sleep(5);
        this.shion.start(1, "act19_shion");
        this.kosmos.start(1, "act19_kosmos");
        this.camerawork.cut19();
        this.msg_print("【アレン】", "Placing target drones in the Encephalon.");
        this.waitclear(90);
        System.sleep(5);
        this.msg.print("The drones are set to\n\"random movement\" and\n\"enemy ambush.\"");
        this.waitclear(110);
        System.sleep(10);
        this.shion.start(1, "act20_shion");
        this.kosmos.start(1, "act20_kosmos");
        this.Fshion.start(1, "face20_shion");
        this.Fshion1.start(1, "eyes20_shion");
        this.camerawork.cut20();
        System.sleep(10);
        this.msg.print("How about a test run before\nthe mission, Chief?");
        this.waitclear(85);
        System.sleep(10);
        this.msg_print("【シオン】", "Mmm, sure, let's do that.\nDid you get that, KOS-MOS?");
        System.sleep(39);
        System.sleep(10);
        this.waitclear(51);
        System.sleep(5);
        this.kosmos.start(1, "act21_kosmos");
        this.Fkosmos.start(1, "face21_kosmos");
        this.Fshion1.start(1, "eyes21_shion");
        this.camerawork.cut21();
        System.sleep(20);
        this.msg_print("【ＫＯＳ−ＭＯＳ】", "Affirmative.");
        this.waitclear(45);
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
            SCE01001.this.shionM.setVisible(true);
            SCE01001.this.kosmosM.setVisible(true);
            SCE01001.this.shion.setVisible(false);
            SCE01001.this.kosmos.setVisible(false);
        }

        public void act10_kosmosM() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(268, 8, 1.0f, true);
        }

        public void act10_shadow() {
            SCE01001.this.shionM.shadow_map_id(1);
            SCE01001.this.shionM.setShadow(4, 36);
            SCE01001.this.kosmosM.shadow_map_id(1);
            SCE01001.this.kosmosM.setShadow(10, 45);
        }

        public void act10_shionM() {
            this.setTranslate(-0.4f, 0.0f, 9.62f);
            this.setRotate(0.0f, 9.0f, 0.0f);
            this.setVisible(true);
            this.mtn(267, 0, 449, 8, 0, 1.0f, true);
            this.mtn(267, 450, 569, 8, 8, 1.0f, true);
        }

        public void act11_chara() {
            SCE01001.this.shion.setVisible(true);
            SCE01001.this.kosmos.setVisible(true);
            SCE01001.this.shionM.setVisible(false);
            SCE01001.this.kosmosM.setVisible(false);
        }

        public void act11_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(270, 0, 1.0f, true);
        }

        public void act11_shion() {
            this.setTranslate(-0.4f, 0.0f, 9.62f);
            this.setRotate(0.0f, 9.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(269, 8, 1.0f, true);
        }

        public void act12_kosmos() {
            this.mtn(271, 0, 1.0f, true);
        }

        public void act13_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(274, 0, 0, 8, 0, 1.0f, true);
        }

        public void act13_shion() {
            this.setTranslate(0.45f, 0.0f, 9.55f);
            this.setRotate(0.0f, 79.0f, 0.0f);
            this.setVisible(true);
            this.mtn(272, 8, 1.0f, true);
        }

        public void act14_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(274, 8, 1.0f, true);
        }

        public void act14_shion() {
            this.setTranslate(0.45f, 0.0f, 9.55f);
            this.setRotate(0.0f, -1.0f, 0.0f);
            this.mtn(273, 8, 1.0f, true);
        }

        public void act15_kosmos() {
            this.mtn(275, 8, 1.0f, true);
        }

        public void act16_kosmos() {
            this.mtn(277, 0, 0, 5, 0, 1.0f, true);
        }

        public void act16_shion() {
            this.setTranslate(0.45f, 0.0f, 9.55f);
            this.setRotate(0.0f, 32.22f, 0.0f);
            this.mtn(276, 0, 1.15f, true);
        }

        public void act17_kosmos() {
            this.mtn(277, 8, 1.0f, true);
        }

        public void act18_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(279, 0, 1.18f, true);
        }

        public void act18_shion() {
            this.setTranslate(0.49f, 0.0f, 9.49f);
            this.setRotate(0.0f, -26.4f, 0.0f);
            this.setVisible(true);
            this.mtn(278, 0, 1.18f, true);
        }

        public void act19_kosmos() {
            this.mtn(281, 8, 1.0f, true);
        }

        public void act19_shion() {
            this.setTranslate(-0.25f, 0.0f, 10.9f);
            this.setRotate(0.0f, -27.35f, 0.0f);
            this.mtn(280, 8, 1.0f, true);
        }

        public void act20_kosmos() {
            this.mtn(284, 0, 0, 5, 0, 1.0f, true);
        }

        public void act20_shion() {
            this.mtn(282, 0, 1.2f, true);
        }

        public void act21_kosmos() {
            this.mtn(284, 0, 1.0f, true);
        }

        public void act2_allen() {
            this.setTranslate(4.8f, 0.0f, -4.1f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.setVisible(true);
            this.mtn(257, 8, 1.3f, true);
        }

        public void act2_staff1() {
            this.setTranslate(4.8f, 0.0f, -9.1f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.setVisible(true);
            this.mtn(258, 8, 1.0f, true);
        }

        public void act4_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 12.49f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(259, -2147483640, 1.0f, true);
        }

        public void act5_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(261, -2147483640, 1.1f, true);
        }

        public void act5_shion() {
            this.setTranslate(0.24f, 0.0f, 14.47f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(260, -2147483640, 1.1f, true);
        }

        public void act6_chara() {
            SCE01001.this.shionM.setVisible(15, true);
            SCE01001.this.shionM.setVisible(16, true);
            System.sleep(70);
            SCE01001.this.shionM.setVisible(16, false);
            SCE01001.this.shionM.setVisible(18, true);
            System.sleep(342);
            System.sleep(73);
            System.sleep(55);
            SCE01001.this.shionM.setVisible(16, true);
            SCE01001.this.shionM.setVisible(18, false);
            System.sleep(30);
            SCE01001.this.shionM.setVisible(16, false);
            SCE01001.this.shionM.setVisible(18, true);
            System.sleep(220);
            System.sleep(55);
            SCE01001.this.shionM.setVisible(16, true);
            SCE01001.this.shionM.setVisible(18, false);
        }

        public void act6_kosmos() {
            this.setVisible(true);
            this.mtn(263, 0, 1.12f, true);
        }

        public void act6_shadow() {
            SCE01001.this.shion.shadow_map_id(1);
            SCE01001.this.shion.setShadow(6, 45);
            SCE01001.this.kosmos.setShadow(0, 0);
        }

        public void act6_shion() {
            this.setTranslate(0.24f, 0.0f, 11.76f);
            this.setRotate(0.0f, -170.0f, 0.0f);
            this.setVisible(true);
            SCE01001.this.shion.setVisible(23, false);
            SCE01001.this.shion.setVisible(24, false);
            this.mtn(262, 0, 1.115f, true);
        }

        public void act6_shionM() {
            this.setTranslate(0.24f, 0.0f, 11.76f);
            this.setRotate(0.0f, -170.0f, 0.0f);
            this.setVisible(true);
            SCE01001.this.shionM.face.setVisible(false);
            int n = 0;
            while (n < 20) {
                SCE01001.this.shionM.setVisible(n, false);
                ++n;
            }
            this.mtn(262, 0, 1.115f, true);
        }

        public void act7_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(265, 0, 0, 5, 0, 1.0f, true);
        }

        public void act7_shadow() {
            SCE01001.this.shion.setShadow(0, 0);
            SCE01001.this.kosmos.setShadow(0, 0);
        }

        public void act7_shion() {
            this.setTranslate(-0.4f, 0.0f, 9.62f);
            this.setRotate(0.0f, 0.5f, 0.0f);
            this.setVisible(true);
            SCE01001.this.shion.setVisible(23, true);
            SCE01001.this.shion.setVisible(24, true);
            this.mtn(264, 8, 1.25f, true);
        }

        public void act7_shionM() {
            int n = 0;
            while (n < 20) {
                SCE01001.this.shionM.setVisible(n, true);
                ++n;
            }
            SCE01001.this.shionM.face.setVisible(true);
            this.setVisible(false);
            this.setTranslate(0.0f, -999.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(Integer.MIN_VALUE, 0, 1.0f, true);
        }

        public void act8_kosmos() {
            this.setTranslate(-0.2f, 0.0f, 11.8f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.mtn(265, 8, 1.0f, true);
        }

        public void act9_shion() {
            this.setRotate(0.0f, 60.5f, 0.0f);
            this.mtn(266, 0, 1.1f, true);
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
    }

    class allFACE
            extends Chr {
        public allFACE() {
            this.init(24582);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(false);
        }

        public void FACE(Chr chr, int n) {
            chr.getChild(0x1000000).mtn(n, 9, 1.0f, false);
            chr.getChild(0x1000000).start(4, null);
        }

        public void FACE(Chr chr, int n, int n2, float f) {
            chr.getChild(0x1000000).mtn(n, n2, f, false);
            chr.getChild(0x1000000).start(4, null);
        }

        public void FACE(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
            chr.getChild(0x1000000).mtn(n, n2, n3, n4, n5, f, false);
            chr.getChild(0x1000000).start(4, null);
        }

        public void act6_hand() {
            System.sleep(90);
            SCE01001.this.shion.setVisible(20, false);
            SCE01001.this.shion.setVisible(21, true);
            System.sleep(515);
            SCE01001.this.shion.setVisible(20, true);
            SCE01001.this.shion.setVisible(21, false);
            System.sleep(30);
            SCE01001.this.shion.setVisible(20, false);
            SCE01001.this.shion.setVisible(21, true);
            System.sleep(315);
            SCE01001.this.shion.setVisible(20, true);
            SCE01001.this.shion.setVisible(21, false);
            System.sleep(30);
        }

        public void act_cam() {
            SCE01001.this.cam4.setTranslate(0.0f, -500.0f, 0.0f);
            SCE01001.this.cam4.setRotate(0.0f, 0.0f, 0.0f);
            System.sleep(90);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(2);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(82);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
        }

        public void act_cam2() {
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(2);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(17);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            System.sleep(1);
            SCE01001.this.cam4.change();
            System.sleep(1);
            SCE01001.this.cam1.change();
            SCE01001.this.flash2.call(0);
            System.sleep(5);
            SCE01001.this.flash2.call(0);
            System.sleep(7);
            SCE01001.this.flash2.call(0);
        }

        public void act_tool() {
            int[] nArray = new int[8];
            nArray[1] = 1;
            nArray[2] = 0x100000;
            nArray[3] = 0x40808080;
            nArray[4] = 2;
            nArray[5] = 2;
            int[] nArray2 = nArray;
            Runtime.setDefocus(15, 1, nArray2);
            float[] fArray = new float[32];
            fArray[0] = 1.0f;
            fArray[1] = 220.0f;
            fArray[4] = 36.0f;
            fArray[5] = 82.0f;
            fArray[8] = 72.0f;
            fArray[9] = 120.0f;
            fArray[12] = 108.00001f;
            fArray[13] = 134.0f;
            fArray[16] = 132.0f;
            fArray[17] = 192.0f;
            fArray[20] = 168.0f;
            fArray[21] = 150.0f;
            fArray[24] = 204.00002f;
            fArray[25] = 172.0f;
            fArray[28] = 228.00002f;
            fArray[29] = 122.0f;
            float[] fArray2 = fArray;
            SCE01001.this.cam2.rotateSPL(fArray2, 1);
            float[] fArray3 = new float[32];
            fArray3[0] = 1.0f;
            fArray3[1] = 6.0f;
            fArray3[4] = 21.0f;
            fArray3[5] = 3.0f;
            fArray3[8] = 42.0f;
            fArray3[12] = 63.0f;
            fArray3[13] = 3.0f;
            fArray3[16] = 77.0f;
            fArray3[17] = 6.0f;
            fArray3[20] = 98.0f;
            fArray3[21] = 6.0f;
            fArray3[24] = 119.0f;
            fArray3[28] = 133.0f;
            fArray3[29] = 6.0f;
            float[] fArray4 = fArray3;
            SCE01001.this.cam3.rotateSPL(fArray4, 1);
            int n = 0;
            SCE01001.this.shion.setTranslate(4.33f, -1.15f, -0.08f);
            SCE01001.this.shion.setRotate(6.0f, -0.01f, 228.96f);
            Runtime.setDefocusParam(15, 1, (int) SCE01001.this.shion.px);
            Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.shion.rz << 24) + 0x808080);
            Runtime.setDefocusParam(15, 4, (int) SCE01001.this.shion.py);
            Runtime.setDefocusParam(15, 5, (int) SCE01001.this.shion.pz);
            Runtime.setDefocusParam(15, 6, (int) SCE01001.this.shion.rx);
            Runtime.setDefocusParam(15, 7, (int) SCE01001.this.shion.ry);
            while (n < 90) {
                Runtime.setDefocusParam(15, 6, (int) SCE01001.this.cam3.getRotateX());
                ++n;
                System.sleep(1);
            }
            SCE01001.this.shion.setTranslate(8.77f, -1.05f, 0.67f);
            SCE01001.this.shion.setRotate(0.0f, 0.0f, 325.0f);
            Runtime.setDefocusParam(15, 1, (int) SCE01001.this.shion.px);
            Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.shion.rz << 24) + 0x808080);
            Runtime.setDefocusParam(15, 4, (int) SCE01001.this.shion.py);
            Runtime.setDefocusParam(15, 5, (int) SCE01001.this.shion.pz);
            Runtime.setDefocusParam(15, 6, (int) SCE01001.this.shion.rx);
            Runtime.setDefocusParam(15, 7, (int) SCE01001.this.shion.ry);
            n = 0;
            while (n < 120) {
                Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.cam2.getRotateX() << 24) + 0x808080);
                ++n;
                System.sleep(1);
            }
        }

        public void act_tool2() {
            int[] nArray = new int[8];
            nArray[1] = 1;
            nArray[2] = 0x100000;
            nArray[3] = 0x40808080;
            nArray[4] = 2;
            nArray[5] = 2;
            int[] nArray2 = nArray;
            Runtime.setDefocus(15, 1, nArray2);
            float[] fArray = new float[24];
            fArray[0] = 1.2f;
            fArray[4] = 36.0f;
            fArray[5] = 6.0f;
            fArray[8] = 72.0f;
            fArray[12] = 108.00001f;
            fArray[13] = 6.0f;
            fArray[16] = 132.0f;
            fArray[20] = 168.0f;
            fArray[21] = 6.0f;
            float[] fArray2 = fArray;
            SCE01001.this.cam3.rotateSPL(fArray2, 1);
            int n = 0;
            SCE01001.this.shion.setTranslate(4.33f, -1.15f, -0.08f);
            SCE01001.this.shion.setRotate(6.0f, -0.01f, 202.96f);
            Runtime.setDefocusParam(15, 1, (int) SCE01001.this.shion.px);
            Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.shion.rz << 24) + 0x808080);
            Runtime.setDefocusParam(15, 4, (int) SCE01001.this.shion.py);
            Runtime.setDefocusParam(15, 5, (int) SCE01001.this.shion.pz);
            Runtime.setDefocusParam(15, 6, (int) SCE01001.this.shion.rx);
            Runtime.setDefocusParam(15, 7, (int) SCE01001.this.shion.ry);
            while (n < 22) {
                Runtime.setDefocusParam(15, 6, (int) SCE01001.this.cam3.getRotateX());
                ++n;
                System.sleep(1);
            }
            SCE01001.this.shion.setTranslate(8.77f, -1.05f, 0.67f);
            SCE01001.this.shion.setRotate(0.0f, 0.0f, 325.0f);
            Runtime.setDefocusParam(15, 1, (int) SCE01001.this.shion.px);
            Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.shion.rz << 24) + 0x808080);
            Runtime.setDefocusParam(15, 4, (int) SCE01001.this.shion.py);
            Runtime.setDefocusParam(15, 5, (int) SCE01001.this.shion.pz);
            Runtime.setDefocusParam(15, 6, (int) SCE01001.this.shion.rx);
            Runtime.setDefocusParam(15, 7, (int) SCE01001.this.shion.ry);
            float[] fArray3 = new float[24];
            fArray3[0] = 1.0f;
            fArray3[1] = 142.0f;
            fArray3[4] = 21.0f;
            fArray3[5] = 202.0f;
            fArray3[8] = 42.0f;
            fArray3[9] = 134.0f;
            fArray3[12] = 63.0f;
            fArray3[13] = 134.0f;
            fArray3[16] = 77.0f;
            fArray3[17] = 134.0f;
            fArray3[20] = 98.0f;
            fArray3[21] = 134.0f;
            float[] fArray4 = fArray3;
            SCE01001.this.cam2.rotateSPL(fArray4, 1);
            n = 0;
            while (n < 90) {
                Runtime.setDefocusParam(15, 3, ((int) SCE01001.this.cam2.getRotateX() << 24) + 0x808080);
                ++n;
                System.sleep(1);
            }
        }

        public void eyes20_shion() {
            SCE01001.this.shion.look_default();
            System.sleep(154);
            System.sleep(15);
            SCE01001.this.shion.look_eye_speed(0.2f);
            SCE01001.this.shion.look_speed(0.0f);
            SCE01001.this.shion.look_char(SCE01001.this.kosmos);
        }

        public void eyes21_shion() {
            SCE01001.this.shion.look_default();
        }

        public void face11_kosmos() {
            System.sleep(65);
            System.sleep(75);
            System.sleep(95);
            System.sleep(4);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(56);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face11_shion() {
            System.sleep(65);
            System.sleep(4);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(51);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            System.sleep(15);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(74);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(10);
            System.sleep(65);
            System.sleep(21);
            SCE01001.this.shion.face.mtn(7, 102, 106, 8, 0, 0.2f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(24);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(39);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(20);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(74);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
        }

        public void face13_shion() {
            System.sleep(5);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(75);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(22);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(71);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(37);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face14_shion() {
            System.sleep(4);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(14);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(12);
            SCE01001.this.shion.face.mtn(7, 0, 55, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(55);
            SCE01001.this.shion.face.mtn(7, 55, 58, 8, 0, 0.02f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(29);
            SCE01001.this.shion.face.mtn(8, 0, 120, 4, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(73);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            System.sleep(10);
        }

        public void face14_shion2() {
            System.sleep(4);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(21);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(36);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(7, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(68);
            SCE01001.this.shion.face.mtn(8, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
        }

        public void face16_shion() {
            System.sleep(40);
            System.sleep(3);
            SCE01001.this.shion.face.mtn(7, 0, 10, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(7, 10, 17, 8, 0, 0.02f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(8);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
        }

        public void face17_kosmos() {
            System.sleep(15);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(57);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(10);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(128);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face18_kosmos2() {
            System.sleep(4);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(26);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face18_shion() {
            System.sleep(6);
            SCE01001.this.shion.face.mtn(7, 90, 101, 8, 0, 0.8f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(14);
            SCE01001.this.shion.face.mtn(7, 102, 106, 8, 0, 0.2f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(23);
            System.sleep(18);
            SCE01001.this.shion.face.mtn(7, 0, 106, 8, 0, -1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(40);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(39);
            SCE01001.this.shion.face.mtn(7, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(59);
            SCE01001.this.shion.face.mtn(8, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
        }

        public void face18_shion2() {
            System.sleep(95);
            System.sleep(3);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(44);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(20);
            System.sleep(6);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(18);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(12);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(27);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(56);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
        }

        public void face20_shion() {
            SCE01001.this.shion.face.mtn(2, 52, 52, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(2, 52, 56, 8, 0, 0.2f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(60);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(17);
            System.sleep(8);
            System.sleep(18);
            System.sleep(3);
            SCE01001.this.shion.face.mtn(1, 0, 120, 6, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(9);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            SCE01001.this.shion.face.mtn(2, 50, 56, 5, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.3f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(1, 46, 120, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(21);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(11);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(4);
        }

        public void face21_kosmos() {
            System.sleep(20);
            System.sleep(3);
            SCE01001.this.kosmos.face.mtn(1, 0, 29, 8, 0, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(30);
            SCE01001.this.kosmos.face.mtn(1, 30, 34, 8, 0, 0.25f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(2, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face5_kosmos() {
            System.sleep(160);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(38);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 3, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(4);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(14);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(10);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(54);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 4, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face5_shion() {
            System.sleep(60);
            System.sleep(3);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(39);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(25);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(24);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(5);
            System.sleep(115);
            SCE01001.this.shion.face.mtn(4, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
        }

        public void face5_shion0() {
            System.sleep(26);
            SCE01001.this.shion.face.mtn(285, 0, 140, 0, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(140);
        }

        public void face5_shion2() {
            System.sleep(116);
            SCE01001.this.shion.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
        }

        public void face6_kosmos() {
            System.sleep(105);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(275);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
            System.sleep(15);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(60);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
            System.sleep(10);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(55);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
            System.sleep(20);
            System.sleep(6);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(198);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(6);
            System.sleep(10);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(44);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(6);
        }

        public void face6_kosmos2() {
            System.sleep(85);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(15);
            SCE01001.this.kosmos.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(5);
        }

        public void face6_shion() {
            System.sleep(5);
            System.sleep(3);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(63);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(30);
            System.sleep(180);
            SCE01001.this.shion.face.mtn(2, 50, 52, 8, 0, 0.15f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(105);
            System.sleep(15);
            System.sleep(30);
            SCE01001.this.shion.face.mtn(2, 52, 120, 8, 0, 0.2f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(40);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
        }

        public void face6_shion2() {
            System.sleep(4);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(45);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(18);
            SCE01001.this.shion.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(3);
        }

        public void face7_shion() {
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(54);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(13);
            SCE01001.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(48);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            System.sleep(30);
            System.sleep(4);
            SCE01001.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(51);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(12);
            SCE01001.this.shion.face.mtn(1, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(63);
            SCE01001.this.shion.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
        }

        public void face8_kosmos() {
            System.sleep(70);
            System.sleep(5);
            SCE01001.this.kosmos.face.mtn(1, 0, 20, 8, 0, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(20);
            SCE01001.this.kosmos.face.mtn(1, 20, 24, 8, 0, 0.5f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(4);
            SCE01001.this.kosmos.face.mtn(2, 9, 1.0f, false);
            SCE01001.this.kosmos.face.start(4, null);
            System.sleep(6);
        }

        public void face9_shion() {
            System.sleep(20);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 0, 14, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(1, 14, 20, 8, 0, 0.05f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 20, 29, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(1, 29, 35, 8, 0, 0.05f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(1, 35, 44, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(12);
            SCE01001.this.shion.face.mtn(1, 44, 47, 8, 0, 0.1f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(6);
        }

        public void face9_shion2() {
            System.sleep(20);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 0, 14, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(1, 14, 20, 8, 0, 0.05f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(6);
            System.sleep(5);
            SCE01001.this.shion.face.mtn(1, 20, 29, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(10);
            SCE01001.this.shion.face.mtn(1, 29, 35, 8, 0, 0.05f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(5);
            System.sleep(15);
            SCE01001.this.shion.face.mtn(1, 50, 62, 8, 0, 1.0f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(12);
            SCE01001.this.shion.face.mtn(1, 59, 62, 8, 0, -0.3f, false);
            SCE01001.this.shion.face.start(4, null);
            System.sleep(4);
            System.sleep(6);
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

        public void act1_00() {
            float[] fArray = new float[]{1.0f, 0.77f, 0.01f, 13.9f, 30.0f, 1.3f, 0.01f, 11.99f, 60.0f, 1.76f, 0.59f, 9.05f, 90.0f, 1.1f, 1.43f, 7.64f, 110.0f, -0.3f, 2.0f, 7.61f, 140.0f, -1.8f, 1.85f, 7.61f, 170.0f, -0.7f, 1.7f, -0.03f, 190.0f, -0.3f, 1.68f, -0.03f};
            SCE01001.this.movSPL.setCtrlVertex(fArray, 1, 1, 210);
            this.move(SCE01001.this.movSPL, true);
        }

        public void act2_headset1() {
        }

        public void act3_00() {
            float[] fArray = new float[40];
            fArray[0] = 1.0f;
            fArray[2] = 3.1f;
            fArray[4] = 25.0f;
            fArray[5] = -2.3f;
            fArray[6] = 4.0f;
            fArray[8] = 45.0f;
            fArray[9] = -5.0f;
            fArray[10] = 3.8f;
            fArray[12] = 65.0f;
            fArray[13] = -1.81f;
            fArray[14] = 3.05f;
            fArray[16] = 90.0f;
            fArray[17] = 1.85f;
            fArray[18] = 3.85f;
            fArray[20] = 115.0f;
            fArray[21] = 3.5f;
            fArray[22] = 3.35f;
            fArray[24] = 125.0f;
            fArray[25] = 1.03f;
            fArray[26] = 2.77f;
            fArray[28] = 145.0f;
            fArray[29] = -0.2f;
            fArray[30] = 2.8f;
            fArray[32] = 160.0f;
            fArray[33] = -0.4f;
            fArray[34] = 2.75f;
            fArray[36] = 175.0f;
            fArray[37] = -0.2f;
            fArray[38] = 2.7f;
            float[] fArray2 = fArray;
            SCE01001.this.movSPL.setCtrlVertex(fArray2, 1, 2, 175);
            this.move(SCE01001.this.movSPL, true);
            this.getTranslate();
            float[] fArray3 = new float[8];
            fArray3[0] = 1.0f;
            fArray3[1] = this.px;
            fArray3[2] = this.py;
            fArray3[3] = this.pz;
            fArray3[4] = 15.0f;
            fArray3[5] = -0.2f;
            fArray3[6] = 1.4f;
            float[] fArray4 = fArray3;
            SCE01001.this.movSPL.setCtrlVertex(fArray4, 1, 2, 15);
            this.move(SCE01001.this.movSPL, true);
            this.getTranslate();
            float[] fArray5 = new float[12];
            fArray5[0] = 1.0f;
            fArray5[1] = this.px;
            fArray5[2] = this.py;
            fArray5[3] = this.pz;
            fArray5[4] = 20.0f;
            fArray5[5] = -0.2f;
            fArray5[6] = 1.9f;
            fArray5[8] = 35.0f;
            fArray5[9] = -0.2f;
            fArray5[10] = 1.55f;
            float[] fArray6 = fArray5;
            SCE01001.this.movSPL.setCtrlVertex(fArray6, 1, 2, 35);
            this.move(SCE01001.this.movSPL, true);
            this.getTranslate();
            float[] fArray7 = new float[16];
            fArray7[0] = 1.0f;
            fArray7[1] = this.px;
            fArray7[2] = this.py;
            fArray7[3] = this.pz;
            fArray7[4] = 82.5f;
            fArray7[5] = -0.28f;
            fArray7[6] = 1.98f;
            fArray7[8] = 165.0f;
            fArray7[9] = -0.14f;
            fArray7[10] = 1.85f;
            fArray7[12] = 210.0f;
            fArray7[13] = -0.2f;
            fArray7[14] = 1.9f;
            float[] fArray8 = fArray7;
            SCE01001.this.movSPL.setCtrlVertex(fArray8, 1, 2, 210);
            this.move(SCE01001.this.movSPL, true);
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
        }

        void act10_sky() {
            this.setRotateY(0.0f);
        }

        void act7_sky() {
            this.setRotateY(-60.0f);
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0() {
            SCE01001.this.cam1.setTranslate(0.0f, -2.0f, 0.0f);
            SCE01001.this.cam1.setRotate(-90.0f, 0.0f, 0.0f);
            SCE01001.this.cam1.setFov(40.0f);
            Runtime.setDefocusQuick(0, 0, 12072, 4);
            Runtime.setDefocusQuick(1, 0, 12072, 4);
            Runtime.setDefocusQuick(2, 0, 12072, 4);
            Runtime.setDefocusQuick(3, 0, 12072, 4);
        }

        public void cut00() {
            SCE01001.this.cam1.setTranslate(0.1f, 1.62f, 10.49f);
            SCE01001.this.cam1.setRotate(-15.34f, -180.59f, 0.0f);
            SCE01001.this.cam1.setFov(29.4f);
        }

        public void cut1() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.cam1.setFov(40.0f);
            float[] fArray = new float[]{1.0f, 0.21f, 1.85f, 15.37f, 60.0f, 0.69f, 1.85f, 14.25f, 90.0f, 0.69f, 1.85f, 14.25f, 120.0f, 0.69f, 1.85f, 14.25f, 180.0f, 0.69f, 1.85f, 14.25f, 240.0f, 0.69f, 1.85f, 14.25f};
            SCE01001.this.cam1.transSPL(fArray, 1, 1, 300);
            SCE01001.this.cam1.viewCNS(SCE01001.this.camera_view, 0.0f, 0.0f, 0.0f);
        }

        public void cut10() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.39f, 0.39f, 0.39f);
            SCE01001.this.light.setColor(1, 0.69f, 0.69f, 0.6f);
            SCE01001.this.light.setDirection2(1, 0.877f, 0.246f, 0.413f);
            SCE01001.this.light.setColor(2, 0.26f, 0.26f, 0.26f);
            SCE01001.this.light.setDirection2(2, -0.853f, 0.001f, -0.523f);
            SCE01001.this.light.setColor(3, 0.9f, 0.9f, 0.9f);
            SCE01001.this.light.setDirection2(3, 0.236f, -0.936f, -0.259f);
            Runtime.setDefocusQuick(0, 2, 16264, 4);
            Runtime.setDefocusQuick(1, 2, 24456, 4);
            Runtime.setDefocusQuick(2, 2, 32648, 4);
            Runtime.setDefocusQuick(3, 2, 40840, 4);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setFov(27.4f);
            float[] fArray = new float[]{1.0f, 8.65f, 1.77f, 23.21f, 900.0f, 4.46f, 1.77f, 24.56f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.87f;
            fArray2[2] = 33.24f;
            fArray2[4] = 900.0f;
            fArray2[5] = -7.87f;
            fArray2[6] = 13.48f;
            float[] fArray3 = fArray2;
            SCE01001.this.cam1.transSPL(fArray, 1, 1, 1110);
            SCE01001.this.cam1.rotateSPL(fArray3, 1, 1, 1110);
        }

        public void cut11() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setColor(1, 0.86f, 0.86f, 0.86f);
            SCE01001.this.light.setDirection2(1, 0.515f, 0.553f, -0.655f);
            SCE01001.this.light.setColor(2, 0.05f, 0.05f, 0.05f);
            SCE01001.this.light.setDirection2(2, -0.964f, 0.0f, -0.266f);
            SCE01001.this.light.setColor(3, 0.18f, 0.18f, 0.18f);
            SCE01001.this.light.setDirection2(3, 0.481f, -0.841f, -0.246f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 41072, 1);
            Runtime.setDefocusQuick(1, 1, 30072, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(0.19f, 1.23f, 7.49f);
            SCE01001.this.cam1.setRotate(3.56f, 176.22f, 0.0f);
            SCE01001.this.cam1.setFov(28.0f);
        }

        public void cut12() {
            SCE01001.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 1, 22880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setRotate(-0.95f, 194.8f, 0.0f);
            SCE01001.this.cam1.setFov(21.4f);
            float[] fArray = new float[]{1.0f, -0.66f, 1.51f, 9.77f, 100.0f, -0.57f, 1.51f, 10.11f};
            SCE01001.this.cam1.transSPL(fArray, 1);
        }

        public void cut13() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.11f, 0.11f, 0.11f);
            SCE01001.this.light.setColor(1, 0.74f, 0.74f, 0.74f);
            SCE01001.this.light.setDirection2(1, 0.719f, 0.158f, 0.677f);
            SCE01001.this.light.setColor(2, 0.25f, 0.25f, 0.25f);
            SCE01001.this.light.setDirection2(2, -0.863f, 0.0f, 0.505f);
            SCE01001.this.light.setColor(3, 0.14f, 0.14f, 0.14f);
            SCE01001.this.light.setDirection2(3, -0.444f, -0.0f, 0.896f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 0, 80000, 1);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-0.39f, 1.45f, 11.22f);
            SCE01001.this.cam1.setRotate(-1.02f, 331.1f, 0.0f);
            SCE01001.this.cam1.setFov(23.4f);
        }

        public void cut14() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.09f, 0.09f, 0.09f);
            SCE01001.this.light.setColor(1, 0.9f, 0.9f, 0.9f);
            SCE01001.this.light.setDirection2(1, 0.34f, 0.525f, 0.78f);
            SCE01001.this.light.setColor(2, 0.26f, 0.26f, 0.26f);
            SCE01001.this.light.setDirection2(2, -0.997f, 0.023f, 0.074f);
            SCE01001.this.light.setColor(3, 0.28f, 0.28f, 0.28f);
            SCE01001.this.light.setDirection2(3, -0.113f, -0.875f, 0.47f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 2, 66264, -2);
            Runtime.setDefocusQuick(1, 1, 9072, -2);
            SCE01001.this.cam1.setTranslate(-0.83f, 1.51f, 12.6f);
            SCE01001.this.cam1.setRotate(-0.73f, 335.2f, 0.0f);
            SCE01001.this.cam1.setFov(22.2f);
        }

        public void cut15() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setColor(1, 0.74f, 0.74f, 0.74f);
            SCE01001.this.light.setDirection2(1, 0.92f, 0.388f, 0.051f);
            SCE01001.this.light.setColor(2, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setDirection2(2, -0.781f, 0.181f, -0.598f);
            SCE01001.this.light.setColor(3, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setDirection2(3, -0.031f, -0.235f, -0.972f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 22880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(0.03f, 1.6f, 10.71f);
            SCE01001.this.cam1.setRotate(-7.73f, 180.57f, 0.0f);
            SCE01001.this.cam1.setFov(34.0f);
        }

        public void cut17() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setColor(1, 0.74f, 0.74f, 0.74f);
            SCE01001.this.light.setDirection2(1, 0.92f, 0.388f, 0.051f);
            SCE01001.this.light.setColor(2, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setDirection2(2, -0.781f, 0.181f, -0.598f);
            SCE01001.this.light.setColor(3, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setDirection2(3, -0.031f, -0.235f, -0.972f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 22880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(0.03f, 1.6f, 10.71f);
            SCE01001.this.cam1.setRotate(-7.73f, 180.57f, 0.0f);
            SCE01001.this.cam1.setFov(34.0f);
        }

        public void cut18() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.23f, 0.23f, 0.23f);
            SCE01001.this.light.setColor(1, 0.7f, 0.7f, 0.7f);
            SCE01001.this.light.setDirection2(1, 0.871f, 0.239f, -0.43f);
            SCE01001.this.light.setColor(2, 0.25f, 0.25f, 0.25f);
            SCE01001.this.light.setDirection2(2, -0.994f, 0.0f, -0.106f);
            SCE01001.this.light.setColor(3, 0.17f, 0.17f, 0.17f);
            SCE01001.this.light.setDirection2(3, 0.665f, -0.339f, -0.665f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 0, 14072, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(1.88f, 1.26f, 8.37f);
            SCE01001.this.cam1.setRotate(0.78f, 138.18f, 0.0f);
            SCE01001.this.cam1.setFov(27.0f);
        }

        public void cut19() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.17f, 0.17f, 0.17f);
            SCE01001.this.light.setColor(1, 0.45f, 0.45f, 0.45f);
            SCE01001.this.light.setDirection2(1, 0.423f, 0.809f, -0.408f);
            SCE01001.this.light.setColor(2, 0.28f, 0.28f, 0.28f);
            SCE01001.this.light.setDirection2(2, -0.814f, 0.0f, 0.58f);
            SCE01001.this.light.setColor(3, 0.37f, 0.37f, 0.37f);
            SCE01001.this.light.setDirection2(3, -0.556f, -0.0f, 0.831f);
            Runtime.setDefocusQuick(0, 0, 14072, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setFov(29.8f);
            float[] fArray = new float[]{1.0f, -6.37f, 0.63f, 20.23f, 285.0f, -6.24f, 0.645f, 19.41f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.6f;
            fArray2[2] = -40.38f;
            fArray2[4] = 285.0f;
            fArray2[5] = -2.6f;
            fArray2[6] = -40.38f;
            float[] fArray3 = fArray2;
            SCE01001.this.cam1.transSPL(fArray, 1, 1, 285);
            SCE01001.this.cam1.rotateSPL(fArray3, 1, 1, 285);
        }

        public void cut2() {
            SCE01001.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 1, 34072, 1);
            Runtime.setDefocusQuick(1, 1, 33072, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01001.this.light.setColor(1, 0.42f, 0.42f, 0.42f);
            SCE01001.this.light.setDirection2(1, 0.125f, 0.0f, 0.992f);
            SCE01001.this.light.setColor(2, 0.52f, 0.52f, 0.52f);
            SCE01001.this.light.setDirection2(2, 0.72f, 0.001f, 0.694f);
            SCE01001.this.light.setColor(3, 0.21f, 0.21f, 0.23f);
            SCE01001.this.light.setDirection2(3, -0.044f, 0.883f, 0.467f);
            SCE01001.this.cam1.setTranslate(6.28f, 1.54f, -2.47f);
            SCE01001.this.cam1.setRotate(-12.64f, 16.62f, 0.0f);
            SCE01001.this.cam1.setFov(40.0f);
            float[] fArray = new float[]{1.0f, 6.28f, 1.54f, -2.47f, 30.0f, 6.25f, 1.54f, -2.45f, 200.0f, 5.75f, 1.54f, -2.32f, 240.0f, 5.73f, 1.54f, -2.31f};
            SCE01001.this.cam1.transSPL(fArray, 1);
        }

        public void cut20() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setColor(1, 0.83f, 0.83f, 0.83f);
            SCE01001.this.light.setDirection2(1, 0.8f, 0.338f, 0.496f);
            SCE01001.this.light.setColor(2, 0.42f, 0.42f, 0.42f);
            SCE01001.this.light.setDirection2(2, -0.939f, 0.0f, 0.345f);
            SCE01001.this.light.setColor(3, 0.15f, 0.15f, 0.15f);
            SCE01001.this.light.setDirection2(3, -0.052f, 0.336f, 0.94f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 13072, 2);
            Runtime.setDefocusQuick(1, 1, 3072, 2);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setRotate(-5.3f, 333.2f, 0.0f);
            SCE01001.this.cam1.setFov(29.4f);
            float[] fArray = new float[]{1.0f, -0.32f, 1.52f, 12.26f, 3.0f, -0.42f, 1.52f, 12.21f, 8.0f, -0.48f, 1.52f, 12.18f, 100.0f, -0.6f, 1.52f, 12.12f, 200.0f, -0.66f, 1.52f, 12.09f, 300.0f, -0.67f, 1.52f, 12.08f};
            SCE01001.this.cam1.transSPL(fArray, 0);
        }

        public void cut21() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.23f, 0.23f, 0.23f);
            SCE01001.this.light.setColor(1, 0.71f, 0.71f, 0.71f);
            SCE01001.this.light.setDirection2(1, 0.771f, 0.637f, -0.004f);
            SCE01001.this.light.setColor(2, 0.22f, 0.22f, 0.22f);
            SCE01001.this.light.setDirection2(2, -0.859f, 0.0f, 0.512f);
            SCE01001.this.light.setColor(3, 0.22f, 0.22f, 0.22f);
            SCE01001.this.light.setDirection2(3, 0.603f, -0.563f, -0.565f);
            Runtime.setDefocusQuick(0, 1, 13072, 2);
            Runtime.setDefocusQuick(1, 1, 3072, 2);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setFov(32.8f);
            float[] fArray = new float[]{1.0f, -0.92f, 1.2f, 10.94f, 165.0f, -0.84f, 1.25f, 11.03f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 16.03f;
            fArray2[2] = 222.3f;
            fArray2[4] = 165.0f;
            fArray2[5] = 14.98f;
            fArray2[6] = 222.3f;
            float[] fArray3 = fArray2;
            SCE01001.this.cam1.transSPL(fArray, 1, 1, 165);
            SCE01001.this.cam1.rotateSPL(fArray3, 1, 1, 165);
        }

        public void cut3() {
            SCE01001.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-0.2f, 1.59f, 13.3f);
            SCE01001.this.cam1.setFov(28.0f);
            SCE01001.this.cam1.viewCNS(SCE01001.this.camera_view, 0.0f, 0.0f, 0.0f);
        }

        public void cut4() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.1f, 0.1f, 0.1f);
            SCE01001.this.light.setColor(1, 0.68f, 0.68f, 0.68f);
            SCE01001.this.light.setDirection2(1, 0.372f, 0.924f, 0.088f);
            SCE01001.this.light.setColor(2, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setDirection2(2, -0.288f, 0.0f, 0.958f);
            SCE01001.this.light.setColor(3, 1.0f, 1.0f, 1.0f);
            SCE01001.this.light.setDirection2(3, 0.173f, 0.882f, 0.437f);
            Runtime.setDefocusQuick(0, 4, 393880, 53);
            Runtime.setDefocusQuick(1, 4, 385688, 53);
            Runtime.setDefocusQuick(2, 4, 377496, 53);
            Runtime.setDefocusQuick(3, 4, 369304, 53);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-0.2f, 1.59f, 13.3f);
            SCE01001.this.cam1.setRotate(-5.25f, 0.0f, 0.0f);
            float[] fArray = new float[]{1.0f, -0.2f, 1.59f, 13.3f, 120.0f, -0.2f, 1.55f, 14.1f, 150.0f, -0.2f, 1.52f, 14.15f};
            float[] fArray2 = new float[]{1.0f, 28.0f, 120.0f, 32.0f};
            SCE01001.this.cam1.transSPL(fArray, 1, 1, 150);
            SCE01001.this.cam1.fovSPL(fArray2, 1);
        }

        public void cut5() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.14f, 0.14f, 0.14f);
            SCE01001.this.light.setColor(1, 0.74f, 0.74f, 0.74f);
            SCE01001.this.light.setDirection2(1, -0.815f, 0.254f, 0.521f);
            SCE01001.this.light.setColor(2, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setDirection2(2, 0.502f, 0.232f, -0.833f);
            SCE01001.this.light.setColor(3, 0.2f, 0.2f, 0.2f);
            SCE01001.this.light.setDirection2(3, -0.574f, -0.109f, -0.812f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 116880, 1);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setFov(34.0f);
            float[] fArray = new float[]{1.0f, -0.4f, 1.65f, 10.85f, 80.0f, -0.49f, 1.68f, 10.95f, 180.0f, -0.58f, 1.7f, 11.05f, 330.0f, -0.58f, 1.7f, 11.05f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -14.8f;
            fArray2[2] = -147.2f;
            fArray2[4] = 80.0f;
            fArray2[5] = -14.8f;
            fArray2[6] = -144.5f;
            fArray2[8] = 180.0f;
            fArray2[9] = -14.8f;
            fArray2[10] = -139.3f;
            fArray2[12] = 330.0f;
            fArray2[13] = -14.8f;
            fArray2[14] = -139.3f;
            float[] fArray3 = fArray2;
            SCE01001.this.cam1.transSPL(fArray, 1);
            SCE01001.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut6() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.18f, 0.18f, 0.18f);
            SCE01001.this.light.setColor(1, 0.79f, 0.79f, 0.79f);
            SCE01001.this.light.setDirection2(1, 0.04f, 0.338f, 0.94f);
            SCE01001.this.light.setColor(2, 0.34f, 0.34f, 0.34f);
            SCE01001.this.light.setDirection2(2, -0.826f, 0.001f, -0.564f);
            SCE01001.this.light.setColor(3, 0.12f, 0.12f, 0.12f);
            SCE01001.this.light.setDirection2(3, -0.768f, -0.474f, 0.43f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 18880, 1);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setFov(30.4f);
            float[] fArray = new float[]{1.0f, -1.16f, 1.4f, 13.2f, 600.0f, -1.15f, 1.4f, 13.14f, 770.0f, -1.15f, 1.4f, 13.14f, 1080.0f, -1.15f, 1.4f, 13.14f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -1.21f;
            fArray2[2] = -40.81f;
            fArray2[4] = 600.0f;
            fArray2[5] = -2.19f;
            fArray2[6] = -37.83f;
            fArray2[8] = 890.0f;
            fArray2[9] = -2.19f;
            fArray2[10] = -21.19f;
            fArray2[12] = 995.0f;
            fArray2[13] = -2.19f;
            fArray2[14] = -18.6f;
            float[] fArray3 = fArray2;
            SCE01001.this.cam1.transSPL(fArray, 1);
            SCE01001.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut7() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE01001.this.light.setDirection2(1, 0.58f, 0.593f, 0.559f);
            SCE01001.this.light.setColor(2, 0.21f, 0.21f, 0.21f);
            SCE01001.this.light.setDirection2(2, -0.968f, -0.139f, 0.208f);
            SCE01001.this.light.setColor(3, 0.24f, 0.24f, 0.24f);
            SCE01001.this.light.setDirection2(3, -0.514f, 0.0f, 0.858f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 69880, 1);
            Runtime.setDefocusQuick(1, 1, 61688, 1);
            Runtime.setDefocusQuick(2, 1, 53496, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-2.22f, 0.97f, 10.5f);
            SCE01001.this.cam1.setRotate(9.32f, 293.0f, 0.0f);
            SCE01001.this.cam1.setFov(27.4f);
        }

        public void cut8() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setColor(1, 0.76f, 0.76f, 0.76f);
            SCE01001.this.light.setDirection2(1, 0.754f, 0.656f, -0.012f);
            SCE01001.this.light.setColor(2, 0.15f, 0.15f, 0.15f);
            SCE01001.this.light.setDirection2(2, -0.745f, 0.362f, -0.561f);
            SCE01001.this.light.setColor(3, 0.19f, 0.19f, 0.19f);
            SCE01001.this.light.setDirection2(3, -0.827f, -0.235f, -0.51f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-2.0f, 1.7f, 11.0f);
            SCE01001.this.cam1.setRotate(-11.45f, 257.3f, 0.0f);
            SCE01001.this.cam1.setFov(27.8f);
        }

        public void cut9() {
            SCE01001.this.Timechk_CutChange();
            SCE01001.this.light.setColor(0, 0.13f, 0.13f, 0.13f);
            SCE01001.this.light.setColor(1, 0.75f, 0.75f, 0.75f);
            SCE01001.this.light.setDirection2(1, 0.58f, 0.593f, 0.559f);
            SCE01001.this.light.setColor(2, 0.21f, 0.21f, 0.21f);
            SCE01001.this.light.setDirection2(2, -0.968f, -0.139f, 0.208f);
            SCE01001.this.light.setColor(3, 0.24f, 0.24f, 0.24f);
            SCE01001.this.light.setDirection2(3, -0.514f, 0.0f, 0.858f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 1, 69880, 1);
            Runtime.setDefocusQuick(1, 1, 61688, 1);
            Runtime.setDefocusQuick(2, 1, 53496, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(15, 0, null);
            SCE01001.this.cam1.setTranslate(-2.22f, 0.97f, 10.5f);
            SCE01001.this.cam1.setRotate(9.32f, 293.0f, 0.0f);
            SCE01001.this.cam1.setFov(27.4f);
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

