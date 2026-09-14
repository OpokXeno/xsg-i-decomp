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
import xeno.map.MC_DYU01_PRJ;
import xeno.map.MC_UTK03_PRJ;
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

class SCE02038
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02038,
        MC_DYU01_PRJ,
        MC_UTK03_PRJ,
        JNT_Human,
        FLSjr_h,
        FLSmary,
        FLSshelley {
    allCHARA jr;
    allCHARA mary;
    allCHARA shelley;
    allCHARA utic1;
    allCHARA utic2;
    allCHARA utic3;
    allCHARA utic4;
    allCHARA dummy;
    Chr gunR1;
    Chr gunL1;
    Chr cable_l;
    Chr cable_s;
    Unit gunR2;
    Unit gunL2;
    Unit headset1;
    Unit headset2;
    Unit rifle1;
    Unit rifle2;
    Unit rifle3;
    Unit rifle4;
    Unit rifle3_;
    Unit bullet1;
    Unit bullet2;
    Unit bullet3;
    Unit BGmap;
    Unit space;
    Unit fog;
    Unit target;
    Unit point0;
    Unit point1;
    Unit point2;
    Chr Fjr;
    Chr Fshelley;
    Chr Fmary;
    Chr Fjr1;
    Chr Fshelley1;
    Chr Fmary1;
    MAPUnit door1;
    MAPUnit door2;
    MAPUnit ring_2a;
    MAPUnit ring_2b;
    MAPUnit ring_2c;
    MAPUnit model1;
    MAPUnit model2;
    MAPUnit model3;
    MAPUnit model4;
    Unit moniW0;
    Unit moniR0;
    Unit moniR1;
    Unit moniR2;
    Unit moniL0;
    Unit moniL1;
    Unit moniL2;
    Unit monitor1;
    Unit monitor2;
    Unit moniY0;
    Unit moniY1;
    Unit moniZ0;
    static final int toUtic = 0;
    static final int toDuran = 1;
    Spline movSPL;
    Spline rotSPL;
    Spline movSPL1;
    Spline rotSPL1;
    Spline movSPL2;
    Spline rotSPL2;
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
    Unit unit0;
    Unit unit1;
    Unit unit2;
    Effect spark;
    Effect spark1;
    Effect spark2;
    Effect smoke1;
    Effect smoke2;
    Effect shell1;
    Effect shell2;
    Effect shell3;
    Effect shell4;
    Effect impact1;
    Effect impact2;
    Effect blood;
    Effect fire1;
    Effect fire2;
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

    SCE02038() {
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

    public void FACEcheck(String string, Chr chr, int n) {
        this.msg.print(string);
        System.println("■■ " + chr + n + " =========================================");
        this.Objchk(chr, n);
        this.msg.clear();
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
        if (n == 0) {
            this.light1.setGlobalPointLightCol(0, 0.1f, 0.3f, 0.6f);
            this.light1.setGlobalPointLightPos(0, 6.06f, 1.35f, 2.5f);
            Stage.setVisible(137, true);
            Stage.setVisible(138, true);
            Stage.setVisible(139, true);
            Stage.setVisible(140, true);
            Stage.setVisible(141, true);
            Stage.setVisible(142, true);
            Stage.setVisible(0, true);
            Stage.setVisible(225, false);
            this.jr.setVisible(true);
            this.mary.setVisible(true);
            this.shelley.setVisible(false);
            this.utic1.setVisible(false);
            this.utic2.setVisible(false);
            this.utic3.setVisible(false);
            this.utic4.setVisible(false);
            this.headset1.setVisible(true);
            this.headset2.setVisible(true);
            this.monitor1.signal(0);
            this.monitor2.signal(0);
            this.moniY0.signal(0);
            this.moniY1.signal(0);
            this.moniZ0.signal(0);
        }
        if (n == 1) {
            this.light1.setGlobalPointLightReset();
            Stage.setVisible(137, false);
            Stage.setVisible(138, false);
            Stage.setVisible(139, false);
            Stage.setVisible(140, false);
            Stage.setVisible(141, false);
            Stage.setVisible(142, false);
            Stage.setVisible(0, false);
            Stage.setVisible(225, true);
            this.jr.setVisible(false);
            this.mary.setVisible(false);
            this.shelley.setVisible(true);
            this.utic1.setVisible(false);
            this.utic2.setVisible(false);
            this.utic3.setVisible(false);
            this.utic4.setVisible(false);
            this.headset1.setVisible(false);
            this.headset2.setVisible(false);
        }
    }

    void change_location() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEJNAME:SCE02039");
        Runtime.jumpEvent(2390);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(2390);
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
        Runtime.setLocation(800);
        Runtime.setLocation(1502);
        this.jr = new allCHARA(34);
        this.mary = new allCHARA(288);
        this.shelley = new allCHARA(289);
        this.utic1 = new allCHARA(778);
        this.utic2 = new allCHARA(778);
        this.utic3 = new allCHARA(778);
        this.utic4 = new allCHARA(778);
        this.dummy = new allCHARA(24582);
        this.utic1.setVisible(false);
        this.utic2.setVisible(false);
        this.utic3.setVisible(false);
        this.utic4.setVisible(false);
        this.dummy.setVisible(false);
        this.gunR1 = new allMECHA(13057);
        this.gunL1 = new allMECHA(13057);
        this.cable_l = new allMECHA(20578);
        this.cable_s = new allMECHA(20600);
        this.gunR1.setVisible(false);
        this.gunL1.setVisible(false);
        this.cable_l.setVisible(false);
        this.cable_s.setVisible(false);
        this.gunR2 = new allUNIT(13057);
        this.gunL2 = new allUNIT(13057);
        this.headset1 = new allUNIT(24580);
        this.headset2 = new allUNIT(24580);
        this.rifle1 = new allUNIT(24607);
        this.rifle2 = new allUNIT(24607);
        this.rifle3 = new allUNIT(24607);
        this.rifle4 = new allUNIT(24607);
        this.rifle3_ = new allUNIT(24607);
        this.bullet1 = new allUNIT(24658);
        this.bullet2 = new allUNIT(24658);
        this.bullet3 = new allUNIT(24658);
        this.BGmap = new allUNIT(24582);
        this.space = new allUNIT(20614, 0.0f, 0.0f, 0.0f, -6.0f);
        this.fog = new allUNIT(24582);
        this.target = new allUNIT(24582);
        this.point0 = new allUNIT(24582);
        this.point1 = new allUNIT(24582);
        this.point2 = new allUNIT(24582);
        this.gunR2.setVisible(false);
        this.gunL2.setVisible(false);
        this.headset1.setParent(this.jr, 48);
        this.headset1.setTranslate(0.01f, -0.02f, -0.0805f);
        this.headset1.setRotate(34.0f, 94.19f, 144.0f);
        this.headset1.setScale(1.15f, 1.0f, 1.0f);
        this.headset2.setParent(this.mary, 48);
        this.headset2.setTranslate(0.0f, -0.04f, -0.09f);
        this.headset2.setRotate(68.0f, 102.2f, 99.0f);
        this.headset2.setScale(1.0f, 1.0f, 1.0f);
        this.rifle1.setVisible(false);
        this.rifle2.setVisible(false);
        this.rifle3.setVisible(false);
        this.rifle4.setVisible(false);
        this.rifle3_.setVisible(false);
        this.bullet1.setVisible(false);
        this.bullet2.setVisible(false);
        this.bullet3.setVisible(false);
        this.fog.setVisible(false);
        this.target.setVisible(false);
        this.point0.setVisible(false);
        this.point1.setVisible(false);
        this.point2.setVisible(false);
        this.Fjr = new allFACE();
        this.Fmary = new allFACE();
        this.Fshelley = new allFACE();
        this.Fjr1 = new allFACE();
        this.Fmary1 = new allFACE();
        this.Fshelley1 = new allFACE();
        this.door1 = new allMAP(220);
        this.door2 = new allMAP(221);
        this.model1 = new allMAP(262);
        this.model2 = new allMAP(263);
        this.model3 = new allMAP(264);
        this.model4 = new allMAP(265);
        this.ring_2a = new allMAP(123);
        this.ring_2b = new allMAP(124);
        this.ring_2c = new allMAP(125);
        this.moniW0 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR0 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR1 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR2 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL0 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL1 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL2 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniW0.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniW0.setArgs(1, 20051, 0, 128, 112);
        this.moniW0.setArgs(2, 100, 0, 15, -1);
        this.moniW0.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR0.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniR0.setArgs(1, 20051, 0, 128, 112);
        this.moniR0.setArgs(2, 70, 0, 15, -1);
        this.moniR0.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR1.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniR1.setArgs(1, 20036, 0, 128, 112);
        this.moniR1.setArgs(2, 75, 0, 15, -1);
        this.moniR1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniR2.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniR2.setArgs(1, 20036, 0, 128, 112);
        this.moniR2.setArgs(2, 75, 0, 15, -1);
        this.moniR2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL0.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniL0.setArgs(1, 20051, 0, 128, 112);
        this.moniL0.setArgs(2, 75, 0, 15, -1);
        this.moniL0.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL1.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniL1.setArgs(1, 20036, 0, 128, 112);
        this.moniL1.setArgs(2, 75, 0, 15, -1);
        this.moniL1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniL2.setArgs(0, 0.0f, 0.0f, 1.0f, 0.6f);
        this.moniL2.setArgs(1, 20036, 0, 128, 112);
        this.moniL2.setArgs(2, 75, 0, 15, -1);
        this.moniL2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new allUNIT(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.monitor1.setArgs(1, 20021, 0, 128, 112);
        this.monitor1.setArgs(2, 100, 0, 15, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(0);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 0.8f, 0.3f);
        this.monitor2.setArgs(1, 20036, 0, 128, 112);
        this.monitor2.setArgs(2, 100, 0, 15, -1);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2.signal(0);
        this.moniY0 = new allUNIT(24613, -0.16f, 2.54f, 6.12f, 0.0f);
        this.moniY1 = new allUNIT(24613, this.moniY0.px - 0.27f, this.moniY0.py + 0.27f, this.moniY0.pz + 0.05f, 0.0f);
        this.moniY0.setArgs(0, 0.0f, 0.0f, 1.2f, 0.6f);
        this.moniY0.setArgs(1, 20061, 0, 128, 112);
        this.moniY0.setArgs(2, 80, 0, 15, -1);
        this.moniY0.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniY0.signal(0);
        this.moniY1.setArgs(0, 0.0f, 0.0f, 0.8f, 0.3f);
        this.moniY1.setArgs(1, 20040, 0, 128, 112);
        this.moniY1.setArgs(2, 100, 0, 15, -1);
        this.moniY1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniY1.signal(0);
        this.moniZ0 = new allUNIT(24613, -1.05f, 2.35f, 7.16f, 90.0f);
        this.moniZ0.setArgs(0, 0.0f, 0.0f, 1.2f, 0.6f);
        this.moniZ0.setArgs(1, 20040, 0, 128, 112);
        this.moniZ0.setArgs(2, 80, 0, 15, -1);
        this.moniZ0.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moniZ0.signal(0);
        this.movSPL = Spline.create();
        this.rotSPL = Spline.create();
        this.movSPL1 = Spline.create();
        this.rotSPL1 = Spline.create();
        this.movSPL2 = Spline.create();
        this.rotSPL2 = Spline.create();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam0.start(3, null);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, -10.0f, 0.0f);
        this.cam1.setRotate(-180.0f, 0.0f, 0.0f);
        this.cam1.setFov(40.0f);
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
        this.spark = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark.setTranslate(0.0f, 0.0f, 0.0f);
        this.spark.setScale(0.25f, 0.25f, 0.25f);
        this.spark.setCaster(this.target);
        this.spark.disp(false);
        this.spark.setForceLoop(true);
        this.spark.noAttach(false);
        this.spark1 = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark1.setTranslate(0.0f, 0.0f, 0.0f);
        this.spark1.setScale(0.25f, 0.25f, 0.25f);
        this.spark1.disp(false);
        this.spark1.setForceLoop(true);
        this.spark1.noAttach(false);
        this.spark2 = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spark2.setTranslate(0.0f, 0.0f, 0.0f);
        this.spark2.setScale(0.25f, 0.25f, 0.25f);
        this.spark2.disp(false);
        this.spark2.setForceLoop(true);
        this.spark2.noAttach(false);
        this.smoke1 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
        this.smoke1.setTranslate(0.0f, 0.0f, 0.0f);
        this.smoke1.setScale(0.25f, 0.25f, 0.25f);
        this.smoke1.disp(false);
        this.smoke1.setForceLoop(true);
        this.smoke1.noAttach(false);
        this.smoke2 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
        this.smoke2.setTranslate(0.0f, 0.0f, 0.0f);
        this.smoke2.setScale(0.25f, 0.25f, 0.25f);
        this.smoke2.disp(false);
        this.smoke2.setForceLoop(true);
        this.smoke2.noAttach(false);
        this.shell1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shell1.setScale(0.5f, 0.5f, 0.5f);
        this.shell1.setTranslate(0.0f, 0.0f, 0.0f);
        this.shell1.setCaster(this.rifle1);
        this.shell1.disp(false);
        this.shell1.setForceLoop(false);
        this.shell2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shell2.setScale(0.5f, 0.5f, 0.5f);
        this.shell2.setTranslate(0.0f, 0.0f, 0.0f);
        this.shell2.setCaster(this.rifle2);
        this.shell2.disp(false);
        this.shell2.setForceLoop(false);
        this.shell3 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shell3.setScale(0.5f, 0.5f, 0.5f);
        this.shell3.setTranslate(0.0f, 0.0f, 0.0f);
        this.shell3.setCaster(this.rifle3);
        this.shell3.disp(false);
        this.shell3.setForceLoop(false);
        this.shell4 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
        this.shell4.setScale(0.5f, 0.5f, 0.5f);
        this.shell4.setTranslate(0.0f, 0.0f, 0.0f);
        this.shell4.setCaster(this.rifle4);
        this.shell4.disp(false);
        this.shell4.setForceLoop(false);
        this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
        this.impact2.setScale(0.2f, 0.2f, 0.2f);
        this.impact2.setTranslate(0.0f, 0.0f, 0.0f);
        this.impact2.setRotate(200.0f, -23.0f, 200.0f);
        this.impact2.disp(false);
        this.impact2.setForceLoop(false);
        this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
        this.impact1.setScale(0.2f, 0.2f, 0.2f);
        this.impact1.setTranslate(0.0f, 0.0f, 0.0f);
        this.impact1.setRotate(200.0f, -23.0f, 200.0f);
        this.impact1.disp(false);
        this.impact1.setForceLoop(false);
        this.blood = new Effect(1525, 0.0f, 0.0f, 0.0f, 0.0f);
        this.blood.disp(false);
        this.blood.setTranslate(0.0f, 0.0f, 0.0f);
        this.blood.setRotate(0.0f, 0.0f, 0.0f);
        this.blood.setScale(0.5f, 0.5f, 0.5f);
        this.fire1 = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fire1.setTranslate(0.0f, 0.03f, 0.0f);
        this.fire1.setRotate(0.0f, 0.0f, 0.0f);
        this.fire1.setScale(0.2f, 0.2f, 0.2f);
        this.fire1.setCaster(this.gunR2);
        this.fire1.disp(false);
        this.fire1.setForceLoop(false);
        this.fire2 = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fire2.setTranslate(0.0f, 0.03f, 0.0f);
        this.fire2.setRotate(0.0f, 0.0f, 0.0f);
        this.fire2.setScale(0.2f, 0.2f, 0.2f);
        this.fire2.setCaster(this.gunL2);
        this.fire2.disp(false);
        this.fire2.setForceLoop(false);
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
        System.sleep(1);
        this.CameraTool();
        this.CaptureTool();
        this.Timechk();
        this.STCamera_init();
        this.cam1.change();
        this.camerawork.cut0();
        this.loadarc(this.jr.face, "FLSjr_h.fpk");
        this.loadarc(this.mary.face, "FLSmary.fpk");
        this.loadarc(this.shelley.face, "FLSshelley.fpk");
        this.playSCENE0();
        this.playSCENE1();
        this.playSCENE2();
        this.playSCENE3();
        this.playSCENE4();
        this.Timechk_SceneEnd();
    }

    void playAGAIN() {
        this.KEYwait("スタート");
        this.KEYwait("スタート　２１０→２１９");
        this.mapCHECK(210, 220);
        this.KEYwait("スタート　２２０→２２９");
        this.mapCHECK(220, 230);
        Stage.setVisible(225, false);
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
    }

    void playSCENE0() {
        Sound.streamPlay(1290104, 48000);
        Stage.renderCommand(4);
        this.mary.renderCommand(21);
        this.jr.renderCommand(22);
        this.utic1.renderCommand(22);
        this.utic2.renderCommand(22);
        this.utic3.renderCommand(22);
        this.utic4.renderCommand(22);
        this.jr.setMotionFlags(0x2000000, true);
        this.mary.setMotionFlags(0x2000000, true);
        this.changeLocation(0);
        this.light1.setGlobalPointLightCol(0, 0.04f, 0.26f, 0.28f);
        this.light1.setGlobalPointLightPos(0, 6.06f, 1.35f, 2.5f);
        this.jr.start(1, "act1_jr");
        this.mary.start(1, "act1_mary");
        this.shelley.start(1, "reset_pos");
        this.utic1.start(1, "reset_pos");
        this.utic2.start(1, "reset_pos");
        this.utic3.start(1, "reset_pos");
        this.utic4.start(1, "reset_pos");
        this.space.start(1, "act1_space");
        this.Fjr.start(1, "face1_jr");
        this.Fmary.start(1, "face1_mary");
        this.Fjr1.start(1, "eyes1_jr");
        this.camerawork.cut1();
        System.sleep(135);
        this.jr.start(1, "act2_jr");
        this.mary.start(1, "act2_mary");
        this.shelley.start(1, "act2_shelley");
        this.space.start(1, "act2_space");
        this.moniW0.start(1, "act2_moniW0");
        this.moniR0.start(1, "act2_moniR0");
        this.moniR1.start(1, "act2_moniR1");
        this.moniR2.start(1, "act2_moniR2");
        this.moniL0.start(1, "act2_moniL0");
        this.moniL1.start(1, "act2_moniL1");
        this.moniL2.start(1, "act2_moniL2");
        this.model1.start(1, "roll");
        this.model2.start(1, "roll");
        this.model3.start(1, "model3");
        this.model4.start(1, "model4");
        Stage.setVisible(266, false);
        Stage.setVisible(267, false);
        this.Fmary.start(1, "face2_mary");
        this.camerawork.cut2();
        System.sleep(30);
        this.msg_print("【メリィ】", "We've hacked into\nthe enemy mainframe.");
        System.sleep(55);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("Okay, so now what?");
        System.sleep(39);
        this.waitclear(11);
        System.sleep(5);
        this.msg.print("Shall I copy or transmit it?");
        System.sleep(54);
        this.waitclear(11);
        Runtime.setLocation(800, 0);
        this.changeLocation(1);
        this.shelley.start(1, "act3_shelley");
        this.jr.start(1, "reset_pos");
        this.mary.start(1, "reset_pos");
        this.monitor1.start(1, "act3_monitor1");
        this.monitor2.start(1, "act3_monitor2");
        this.moniY0.start(1, "act3_monitor_front");
        this.moniZ0.start(1, "act3_monitor_side");
        this.moniW0.start(1, "act_monitor_all_off");
        this.model1.start(1, "model_reset");
        this.model2.start(1, "model_reset");
        this.model3.start(1, "model_reset");
        this.model4.start(1, "model_reset");
        Stage.setVisible(266, true);
        Stage.setVisible(267, true);
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.model1.start(1, "model_reset");
        this.model2.start(1, "model_reset");
        this.model1.start(1, "model_reset");
        this.model2.start(1, "model_reset");
        this.Fshelley.start(1, "face3_shelley");
        this.camerawork.cut3();
        System.sleep(20);
        this.msg_print("【メリィ】", "They've already entered\nthe manual override code...\nThe partitions are being deleted.");
        System.sleep(153);
        this.waitclear(12);
        System.sleep(15);
        this.msg.print("Hurry up and start your transmission.");
        System.sleep(38);
        this.waitclear(12);
        System.sleep(5);
        Runtime.setLocation(1502);
        this.changeLocation(0);
        this.light1.setGlobalPointLightCol(0, 0.04f, 0.26f, 0.28f);
        this.light1.setGlobalPointLightPos(0, 6.06f, 1.35f, 2.5f);
        this.jr.renderCommand(0);
        this.mary.renderCommand(0);
        this.jr.start(1, "act4_jr");
        this.mary.start(1, "act4_mary");
        this.shelley.start(1, "reset_pos");
        this.ring_2a.start(1, "ring_reset");
        this.ring_2b.start(1, "ring_reset");
        this.ring_2c.start(1, "ring_reset");
        this.moniW0.start(1, "act_monitor_all_on");
        this.model1.start(1, "roll");
        this.model2.start(1, "roll");
        this.model3.start(1, "model3");
        this.model4.start(1, "model4");
        Stage.setVisible(266, false);
        Stage.setVisible(267, false);
        this.Fmary.start(1, "face4_mary");
        this.Fjr.start(1, "face4_jr");
        this.Fjr1.start(1, "eyes4_jr");
        this.camerawork.cut4();
        this.msg_print("【メリィ】", "Roger.");
        System.sleep(27);
        this.waitclear(13);
        System.sleep(20);
        this.msg.print("You just clear your head out\nand wait for me.");
        System.sleep(75);
        this.waitclear(10);
        System.sleep(45);
        this.light1.setGlobalPointLightCol(0, 0.04f, 0.44f, 0.58f);
        this.light1.setGlobalPointLightPos(0, 4.3f, -0.01f, 3.1f);
        this.light1.setGlobalPointLightCol(1, 0.04f, 0.44f, 0.58f);
        this.light1.setGlobalPointLightPos(1, 4.3f, -0.01f, 1.87f);
        this.light1.setGlobalPointLightCol(2, 0.21f, 0.08f, 0.05f);
        this.light1.setGlobalPointLightPos(2, 3.7f, -0.01f, 2.5f);
        this.jr.start(1, "act5_jr");
        this.mary.start(1, "act5_mary");
        this.Fjr1.start(1, "eyes5_jr");
        this.camerawork.cut5();
        System.sleep(100);
        this.jr.setVisible(false);
        this.mary.start(1, "act6_mary");
        this.cable_l.start(1, "act6_cable_l");
        this.cable_s.start(1, "act6_cable_l2");
        this.Fmary.start(1, "face6_mary");
        this.camerawork.cut6();
        System.sleep(155);
        this.jr.setVisible(true);
        this.jr.start(1, "act7_jr");
        this.mary.start(1, "act7_mary");
        this.cable_l.start(1, "act7_cable_l");
        this.Fjr.start(1, "face7_jr");
        this.Fmary.start(1, "face7_mary");
        this.Fjr1.start(1, "eyes7_jr");
        this.Fmary1.start(1, "eyes7_mary");
        this.camerawork.cut7();
        System.sleep(60);
        this.msg_print("【Ｊｒ．】", "I tell ya,");
        System.sleep(10);
        this.camerawork.cut7_1();
        System.sleep(30);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("being able to interlink with your\nsister has gotta be real convenient.");
        System.sleep(100);
        this.waitclear(10);
        this.msg_print("【シェリィ】", "I wouldn't say that. She has so\nmany random thoughts.");
        System.sleep(61);
        System.sleep(54);
        this.waitclear(10);
        this.Fmary.start(1, "face7_1_mary");
        this.Fjr1.start(1, "eyes7_1_jr");
        this.Fmary1.start(1, "eyes7_1_mary");
        this.msg_print("【メリィ】", "I can't help it!");
        System.sleep(50);
        this.waitclear(10);
        this.msg.print("We're humans, not Realians.");
        System.sleep(95);
        this.waitclear(10);
        System.sleep(15);
        this.msg.print("You can telepathically talk to the\nKaiser, right, Little Master?");
        System.sleep(78);
        this.waitclear(12);
    }

    void playSCENE1() {
        this.changeLocation(0);
        this.jr.start(1, "act8_jr");
        this.mary.start(1, "act8_mary");
        this.cable_l.start(1, "act8_cable_l");
        this.Fjr.start(1, "face8_jr");
        this.Fmary.start(1, "face8_mary");
        this.Fjr1.start(1, "eyes8_jr");
        this.Fmary1.start(1, "eyes8_mary");
        this.camerawork.cut8();
        System.sleep(15);
        this.msg.print("That sounds a lot more\nconvenient to me.");
        System.sleep(55);
        this.waitclear(10);
        this.msg.print("You don't need this\nannoying contraption, either.");
        System.sleep(40);
        this.waitclear(10);
        this.msg_print("【Ｊｒ．】", "Not really...");
        System.sleep(42);
        this.waitclear(13);
        this.Fjr.start(1, "face8_1_jr");
        this.msg.print("I have to constantly make sure I watch\nwhat I'm thinking, or else I start\nsending thoughts I don't want to send.");
        System.sleep(80);
        this.camerawork.cut8_1();
        System.sleep(60);
        this.jr.start(1, "act9_jr");
        this.mary.start(1, "act9_mary");
        this.Fjr.start(1, "face9_99_jr");
        this.Fjr1.start(1, "eyes9_jr");
        this.camerawork.cut9();
        System.sleep(80);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("The whole thing's just\nas tiring as hell.");
        System.sleep(65);
        this.waitclear(10);
        this.Fjr.start(1, "face10_jr");
        this.Fmary.start(1, "face10_mary");
        this.Fjr1.start(1, "eyes10_jr");
        this.Fmary1.start(1, "eyes10_mary");
        this.camerawork.cut10();
        this.msg.print("It'd be easier to just\nwrite him a letter.");
        System.sleep(90);
        this.waitclear(10);
        this.msg_print("【メリィ】", "Oh, I didn't know it was like that.");
        System.sleep(51);
        this.waitclear(14);
        this.jr.start(1, "act11_jr");
        this.mary.look_default();
        this.Fjr.start(1, "face11_jr");
        this.Fjr1.start(1, "eyes11_jr");
        this.Fmary1.start(1, "eyes11_mary");
        this.camerawork.cut11();
        this.msg_print("【Ｊｒ．】", "Yep, that's how it is...");
        System.sleep(45);
        this.waitclear(10);
        System.sleep(15);
        this.waitclear(10);
        this.jr.renderCommand(533);
        this.mary.setVisible(false);
        this.cable_l.setVisible(false);
        this.jr.start(1, "act12_jr");
        this.mary.start(1, "act12_mary");
        this.utic1.start(1, "act12_utic1");
        this.utic2.start(1, "act12_utic2");
        this.utic3.start(1, "act12_utic3");
        this.utic4.start(1, "act12_utic4");
        this.rifle1.start(1, "act12_rifle1");
        this.rifle2.start(1, "act12_rifle2");
        this.rifle3.start(1, "act12_rifle3");
        this.rifle4.start(1, "act12_rifle4");
        this.door1.start(1, "act12_door1");
        this.door2.start(1, "act12_door2");
        this.Fjr1.start(1, "eyes12_jr");
        this.camerawork.cut12();
        System.sleep(30);
        this.mary.setVisible(true);
        this.jr.renderCommand(534);
        this.mary.renderCommand(534);
        this.jr.start(1, "act13_jr");
        this.mary.start(1, "act13_mary");
        this.gunR2.start(1, "act13_gunR2");
        this.space.setVisible(false);
        this.utic1.start(1, "act13_utic1");
        this.utic2.start(1, "act13_utic2");
        this.utic3.start(1, "act13_utic3");
        this.utic4.start(1, "act13_utic4");
        this.bullet1.start(1, "act13_bullet1");
        this.bullet2.start(1, "act13_bullet2");
        this.bullet3.start(1, "act13_bullet3");
        this.rifle1.start(1, "act13_rifle1");
        this.rifle2.start(1, "act13_rifle2");
        this.rifle3.start(1, "act13_rifle3");
        this.rifle4.start(1, "act13_rifle4");
        this.headset1.start(1, "act13_impact1");
        this.headset2.start(1, "act13_impact2");
        this.door1.start(1, "act13_door1");
        this.door2.start(1, "act13_door2");
        this.moniW0.start(1, "act_monitor_all_off");
        this.Fjr.start(1, "face13_jr");
        this.Fmary.start(1, "face13_mary");
        this.camerawork.cut13();
        System.sleep(22);
        System.sleep(48);
        System.println("roop_stop");
        this.rifle1.start(1, "_roop1_stop");
        this.rifle2.start(1, "_roop2_stop");
        this.rifle3.start(1, "_roop3_stop");
        this.rifle4.start(1, "_roop4_stop");
        this.shell1.disp(false);
        this.shell2.disp(false);
        this.shell3.disp(false);
        this.shell4.disp(false);
        this.bullet1.setVisible(false);
        this.bullet2.setVisible(false);
        this.bullet3.setVisible(false);
        this.jr.renderCommand(0);
        this.mary.renderCommand(0);
        this.jr.start(1, "act14_jr");
        this.gunR2.start(1, "act14_gunR2");
        this.gunL2.start(1, "act14_gunL2");
        this.space.setVisible(true);
        this.headset1.start(1, "act14_impact3");
        this.headset2.start(1, "act14_impact4");
        this.Fjr.start(1, "face14_jr");
        this.camerawork.cut14();
        System.sleep(20);
        this.camerawork.cut14_1();
        System.sleep(60);
        this.fire1.disp(false);
        this.fire2.disp(false);
        this.utic1.start(1, "act15_utic1");
        this.utic2.start(1, "act15_utic2");
        this.utic3.start(1, "act15_utic3");
        this.utic4.start(1, "act15_utic4");
        this.rifle3.start(1, "act15_rifle3");
        this.rifle4.start(1, "act15_rifle4");
        this.headset1.start(1, "act15_blood");
        this.Fjr.start(1, "face15_jr");
        this.camerawork.cut15();
        System.sleep(100);
        this.rifle3.start(1, "_roop3_stop");
        this.rifle4.start(1, "_roop4_stop");
        this.shell3.disp(false);
        this.shell4.disp(false);
    }

    void playSCENE2() {
        this.utic1.setVisible(true);
        this.utic2.setVisible(true);
        this.utic3.setVisible(true);
        this.utic4.setVisible(true);
        this.gunR1.setVisible(true);
        this.gunL1.setVisible(true);
        this.jr.start(1, "act16_jr");
        this.dummy.start(1, "act16_jr_hand");
        this.gunR1.start(1, "act16_gunR1");
        this.gunL1.start(1, "act16_gunL1");
        this.headset1.start(1, "act16_impact1");
        this.gunR2.setVisible(false);
        this.gunL2.setVisible(false);
        this.Fjr.start(1, "face16_jr");
        this.camerawork.cut16();
        System.sleep(90);
        System.sleep(140);
        this.Fjr.start(1, "face16_1_jr");
        this.camerawork.cut16_1();
        this.msg_print("【Ｊｒ．】", "You okay, Mary?");
        System.sleep(40);
        this.waitclear(10);
        System.sleep(10);
        this.jr.setMotionFlags(Integer.MIN_VALUE, false);
        this.mary.start(1, "act17_mary");
        this.cable_s.start(1, "act17_cable_s");
        this.gunR1.setVisible(false);
        this.gunL1.setVisible(false);
        this.target.start(1, "act17_short");
        this.headset1.start(1, "act17_impact1");
        this.headset2.start(1, "act17_impact2");
        this.gunR2.start(1, "act17_spark1");
        this.gunL2.start(1, "act17_spark2");
        this.door1.start(1, "act17_map");
        this.Fmary.start(1, "face17_mary");
        this.camerawork.cut17();
        this.msg_print("【メリィ】", "Yeah, I'm all right, but...\nLook what you did!");
        System.sleep(125);
        this.waitclear(10);
        this.msg_print("【シェリィ】", "I'm sorry to say that all data\nfrom the enemy mainframe");
        System.sleep(100);
        this.waitclear(10);
        this.jr.start(1, "act18_jr");
        this.cable_s.setVisible(false);
        this.headset1.start(1, "act18_impact1");
        this.Fjr.start(1, "face18_jr");
        this.Fjr1.start(1, "eyes18_jr");
        this.camerawork.cut18();
        this.msg.print("has been lost.");
        System.sleep(40);
        this.waitclear(10);
        System.sleep(25);
        this.msg_print("【Ｊｒ．】", "Ah...oops.");
        System.sleep(60);
        this.waitclear(10);
        this.jr.start(1, "act19_jr");
        this.mary.start(1, "act19_mary");
        this.Fmary.start(1, "face19_mary");
        this.camerawork.cut19();
        this.msg_print("【メリィ】", "It's because of all those cheesy\naction flicks you keep on watching!");
        System.sleep(130);
        this.waitclear(10);
        System.sleep(15);
        System.sleep(15);
        this.jr.start(1, "act20_jr");
        this.Fjr.start(1, "face20_jr");
        this.Fjr1.start(1, "eyes20_jr");
        this.camerawork.cut20();
        this.msg_print("【デュランダル乗員】", "They've launched a lifeboat.\nShall we pursue?");
        System.sleep(47);
        System.sleep(18);
        this.waitclear(10);
        this.msg_print("【Ｊｒ．】", "Nah, no need to pursue them\nthat far. We just need to\nknow where they're headed.");
        System.sleep(60);
        System.sleep(50);
        this.waitclear(10);
        this.msg.print("Place a trace request with the\nU.M.N. administration bureau.\nLet's head back.");
        System.sleep(100);
        System.sleep(25);
        this.waitclear(10);
        System.sleep(15);
    }

    void playSCENE3() {
        this.jr.renderCommand(533);
        this.mary.renderCommand(22);
        this.utic4.renderCommand(534);
        this.jr.setMotionFlags(0x40000000, false);
        this.jr.start(1, "act21_jr");
        this.mary.start(1, "act21_mary");
        this.utic4.start(1, "act21_utic4");
        this.BGmap.start(1, "act21_map");
        this.Fjr.start(1, "face21_jr");
        this.Fmary1.start(1, "eyes21_mary");
        this.camerawork.cut21();
        this.msg.print("Hurry up with the\nretrieval preparations");
        System.sleep(57);
        this.waitclear(13);
        this.msg.print("onboard the Durandal.\nAll hands, check your equipment.\nDon't leave any evidence behind.");
        System.sleep(15);
        this.mary.renderCommand(21);
        this.jr.renderCommand(534);
        this.jr.start(1, "act22_jr");
        this.mary.start(1, "act22_mary");
        this.utic4.start(1, "act22_utic4");
        this.space.start(1, "act22_map");
        this.Fmary.start(1, "face22_mary");
        this.Fmary1.start(1, "eyes22_mary");
        this.camerawork.cut22();
        System.sleep(45);
        System.sleep(10);
        System.sleep(35);
        this.waitclear(10);
        System.sleep(60);
        this.msg_print("【メリィ】", "Amazing...");
        System.sleep(50);
        this.waitclear(10);
        this.mary.start(1, "act23_mary");
        this.jr.start(1, "reset_pos");
        this.utic1.start(1, "reset_pos");
        this.utic2.start(1, "reset_pos");
        this.utic3.start(1, "reset_pos");
        this.utic4.start(1, "reset_pos");
        this.rifle1.start(1, "reset_pos");
        this.rifle2.start(1, "reset_pos");
        this.rifle3.start(1, "reset_pos");
        this.rifle4.start(1, "reset_pos");
        this.rifle3_.start(1, "reset_pos");
        this.headset1.start(1, "reset_pos");
        this.space.start(1, "act23_map");
        this.Fmary.start(1, "face23_mary");
        this.camerawork.cut23();
        System.sleep(30);
        this.msg.print("I can't believe how much his\npersonality changes when\nhe's holding a gun.");
        System.sleep(90);
        this.waitclear(10);
        this.msg_print("【シェリィ】", "A lust for conquest and absolute\npower, and the desire to have\na really big...gun.");
        System.sleep(195);
        this.waitclear(10);
        this.msg.print("I suppose that'd be\nthe standard explanation.");
        System.sleep(55);
        this.waitclear(10);
        this.mary.start(1, "act24_mary");
        this.shelley.start(1, "act24_shelley");
        this.space.start(1, "act24_map");
        this.Fmary.start(1, "face24_mary");
        this.camerawork.cut24();
        System.sleep(23);
        this.msg_print("【メリィ】", "You don't hold back, do you, Shelley?");
        this.waitclear(82);
        this.msg_print("【メリィ】", "Although, in Little Master's case,\nit's probably just a complex \nabout his height.");
        System.sleep(150);
        this.waitclear(10);
        System.sleep(19);
        this.msg_print("【メリィ】", "Probably.");
        this.waitclear(46);
        this.msg.print("But I think Little Master's plenty\nattractive just the way he is.");
        System.sleep(75);
        this.waitclear(10);
        System.sleep(30);
        this.msg_print("【シェリィ】", "True...");
        this.waitclear(45);
        System.sleep(15);
    }

    void playSCENE4() {
        Runtime.setLocation(800, 0);
        this.changeLocation(1);
        this.shelley.start(1, "act25_shelley");
        this.jr.start(1, "reset_pos");
        this.mary.start(1, "act25_mary");
        this.monitor1.start(1, "act3_monitor1");
        this.monitor2.start(1, "act3_monitor2");
        this.moniY0.start(1, "act3_monitor_front");
        this.moniZ0.start(1, "act3_monitor_side");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.Fshelley.start(1, "face25_shelley");
        this.camerawork.cut25();
        this.msg.print("Mary, we'll be leaving in\ntwo minutes. Hurry back, okay?");
        System.sleep(91);
        System.sleep(24);
        this.waitclear(10);
        System.sleep(10);
        this.msg.print("Roger.");
        System.sleep(25);
        this.waitclear(10);
        System.sleep(15);
        Runtime.setLocation(1502);
        this.changeLocation(0);
        this.light1.setGlobalPointLightCol(1, 0.04f, 0.44f, 0.58f);
        this.light1.setGlobalPointLightPos(1, 4.32f, 1.06f, 4.25f);
        this.light1.setGlobalPointLightCol(2, 0.04f, 0.22f, 0.25f);
        this.light1.setGlobalPointLightPos(2, 4.85f, 1.25f, 5.0f);
        this.mary.start(1, "act26_mary");
        this.shelley.start(1, "reset_pos");
        this.headset1.start(1, "act26_impact1");
        this.gunR2.start(1, "act26_spark1");
        this.ring_2a.start(1, "ring_reset");
        this.ring_2b.start(1, "ring_reset");
        this.ring_2c.start(1, "ring_reset");
        this.Fmary.start(1, "face26_mary");
        this.camerawork.cut26();
        System.sleep(135);
        this.Fmary.start(1, "face26_1_mary");
        this.msg_print("【メリィ】", "It's a shame...");
        System.sleep(28);
        this.waitclear(12);
        this.msg.print("Little Master doesn't have any\nchoice in the matter but to\nstay that size...");
        System.sleep(108);
        this.waitclear(12);
        System.sleep(60);
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

        public void act11_jr() {
            this.setTranslate(2.86f, 0.0f, 1.79f);
            this.setRotate(0.0f, 277.0f, 0.0f);
            this.mtn(267, 0, 80, 8, 8, 1.0f, true);
        }

        public void act12_jr() {
            this.setTranslate(2.86f, 0.0f, 1.61f);
            this.setRotate(0.0f, 184.0f, 0.0f);
            this.mtn(268, 20, 50, 8, -2147483640, 1.0f, true);
        }

        public void act12_mary() {
            this.setTranslate(3.57f, 0.0f, 3.12f);
            this.setRotate(0.0f, 136.0f, 0.0f);
            this.mtn(269, 0, 30, 8, 8, 1.0f, true);
        }

        public void act12_utic1() {
            this.setTranslate(-0.79f, 0.0f, 11.11f);
            this.setRotate(0.0f, 161.0f, 0.0f);
            this.setVisible(true);
            this.mtn(270, 10, 40, 8, 8, 1.0f, true);
        }

        public void act12_utic2() {
            this.setTranslate(-0.82f, 0.0f, 12.84f);
            this.setRotate(0.0f, 160.0f, 0.0f);
            this.setVisible(true);
            this.mtn(271, 0, 30, 8, 8, 1.0f, true);
        }

        public void act12_utic3() {
            this.setTranslate(1.48f, 0.0f, 11.15f);
            this.setRotate(0.0f, 175.0f, 0.0f);
            this.setVisible(true);
            this.mtn(272, 10, 40, 8, 8, 1.0f, true);
        }

        public void act12_utic4() {
            this.setTranslate(0.68f, 0.0f, 12.39f);
            this.setRotate(0.0f, 183.0f, 0.0f);
            this.setVisible(true);
            this.mtn(273, 0, 15, 8, 0, 0.5f, true);
        }

        public void act13_jr() {
            this.setTranslate(2.86f, 0.0f, 1.61f);
            this.setRotate(0.0f, 184.0f, 0.0f);
            this.mtn(268, 50, 99, 8, Integer.MIN_VALUE, 1.0f, true);
        }

        public void act13_mary() {
            this.setTranslate(3.57f, 0.0f, 3.12f);
            this.setRotate(0.0f, 136.0f, 0.0f);
            this.mtn(269, 40, 99, 8, 0, 1.0f, true);
        }

        public void act13_utic1() {
            this.setTranslate(-0.79f, 0.0f, 11.11f);
            this.setRotate(0.0f, 161.0f, 0.0f);
            this.setVisible(true);
            this.mtn(270, 30, 99, 8, 0, 1.0f, true);
        }

        public void act13_utic2() {
            this.setTranslate(-0.82f, 0.0f, 12.84f);
            this.setRotate(0.0f, 160.0f, 0.0f);
            this.setVisible(true);
            this.mtn(271, 30, 99, 8, 0, 1.0f, true);
        }

        public void act13_utic3() {
            this.setTranslate(1.48f, 0.0f, 11.15f);
            this.setRotate(0.0f, 175.0f, 0.0f);
            this.setVisible(true);
            this.mtn(272, 30, 99, 8, 0, 1.0f, true);
        }

        public void act13_utic4() {
            this.setTranslate(0.68f, 0.0f, 12.39f);
            this.setRotate(0.0f, 183.0f, 0.0f);
            this.setVisible(true);
            this.mtn(273, 20, 99, 8, 0, 0.8f, true);
        }

        public void act14_jr() {
            this.setTranslate(1.32f, 0.0f, 2.68f);
            this.setRotate(0.0f, 363.0f, 0.0f);
            this.mtn(274, 0, 79, 8, -2147483640, 1.0f, true);
        }

        public void act15_utic1() {
            this.setTranslate(-1.23f, 0.0f, 11.86f);
            this.setRotate(0.0f, 173.0f, 0.0f);
            this.mtn(275, 0, 50, 8, 8, 1.0f, true);
        }

        public void act15_utic2() {
            this.setTranslate(-0.96f, 0.0f, 13.77f);
            this.setRotate(0.0f, 160.0f, 0.0f);
            this.mtn(276, 0, 50, 8, 8, 1.0f, true);
        }

        public void act15_utic3() {
            this.setTranslate(0.31f, 0.0f, 9.57f);
            this.setRotate(0.0f, 53.0f, 0.0f);
            this.mtn(277, 0, 90, 8, 0, 1.0f, true);
            System.sleep(10);
        }

        public void act15_utic4() {
            this.setTranslate(-0.66f, 0.04f, 10.42f);
            this.setRotate(0.0f, 22.0f, 0.0f);
            this.mtn(278, 0, 100, 8, 0, 1.0f, true);
        }

        public void act16_jr() {
            this.setTranslate(-0.13f, 0.0f, 1.86f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.mtn(279, 0, 300, 8, -2147483640, 1.0f, true);
        }

        public void act16_jr_hand() {
            System.sleep(50);
            SCE02038.this.jr.setVisible(10, true);
            SCE02038.this.jr.setVisible(11, true);
            System.sleep(22);
            SCE02038.this.jr.setVisible(10, false);
            SCE02038.this.jr.setVisible(11, false);
            SCE02038.this.jr.setVisible(14, true);
            SCE02038.this.jr.setVisible(15, true);
            System.sleep(12);
            SCE02038.this.jr.setVisible(10, true);
            SCE02038.this.jr.setVisible(11, true);
            SCE02038.this.jr.setVisible(14, false);
            SCE02038.this.jr.setVisible(15, false);
            System.sleep(14);
            SCE02038.this.jr.setVisible(10, false);
            SCE02038.this.jr.setVisible(11, false);
            SCE02038.this.jr.setVisible(14, true);
            SCE02038.this.jr.setVisible(15, true);
            System.sleep(52);
            SCE02038.this.jr.setVisible(10, true);
            SCE02038.this.jr.setVisible(11, true);
            SCE02038.this.jr.setVisible(14, false);
            SCE02038.this.jr.setVisible(15, false);
            System.sleep(15);
            SCE02038.this.jr.setVisible(10, false);
            SCE02038.this.jr.setVisible(11, false);
            SCE02038.this.jr.setVisible(14, true);
            SCE02038.this.jr.setVisible(15, true);
            System.sleep(33);
            SCE02038.this.jr.setVisible(10, true);
            SCE02038.this.jr.setVisible(11, true);
            SCE02038.this.jr.setVisible(14, false);
            SCE02038.this.jr.setVisible(15, false);
        }

        public void act17_mary() {
            this.setTranslate(4.13f, 0.0f, 2.9f);
            this.setRotate(0.0f, 323.0f, 0.0f);
            this.mtn(280, 0, 239, 8, 0, 1.0f, true);
        }

        public void act18_jr() {
            this.setTranslate(-0.13f, 0.0f, 2.32f);
            this.setRotate(0.0f, 80.0f, 0.0f);
            this.mtn(281, 0, 265, 8, 8, 1.0f, true);
        }

        public void act19_jr() {
            this.mtn(281, 174, 264, 8, 8, 1.0f, true);
        }

        public void act19_mary() {
            this.setTranslate(3.75f, 0.0f, 3.48f);
            this.setRotate(0.0f, 291.0f, 0.0f);
            this.mtn(283, 0, 224, 8, 8, 1.0f, true);
        }

        public void act1_jr() {
            this.setTranslate(5.05f, 0.0f, 1.68f);
            this.setRotate(0.0f, 22.0f, 0.0f);
            this.mtn(257, 0, 420, 8, 0, 0.8f, true);
        }

        public void act1_mary() {
            this.setTranslate(4.98f, -0.04f, 2.46f);
            this.setRotate(0.0f, 86.0f, 0.0f);
            this.mtn(258, 0, 245, 8, 0, 0.8f, true);
        }

        public void act20_jr() {
            this.mtn(284, 60, 419, 8, 8, 1.0f, true);
        }

        public void act21_jr() {
            this.setTranslate(-0.04f, 0.0f, 8.9f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(7, false);
            this.mtn(285, 0, 60, 8, Integer.MIN_VALUE, 0.7f, true);
        }

        public void act21_mary() {
            this.setTranslate(3.71f, 0.0f, 3.26f);
            this.setRotate(0.0f, 650.0f, 0.0f);
            this.mtn(286, 0, 69, 8, 8, 1.0f, true);
        }

        public void act21_utic4() {
            SCE02038.this.utic1.setVisible(false);
            SCE02038.this.utic2.setVisible(false);
            SCE02038.this.utic3.setVisible(false);
            SCE02038.this.rifle1.setVisible(false);
            SCE02038.this.rifle2.setVisible(false);
            SCE02038.this.rifle3_.setVisible(false);
            SCE02038.this.utic4.setVisible(6, false);
            SCE02038.this.utic4.setVisible(7, false);
        }

        public void act22_jr() {
            this.setTranslate(-0.04f, 0.0f, 9.9f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.jr.face.setVisible(false);
            this.setVisible(7, true);
            this.mtn(285, 30, 240, 8, Integer.MIN_VALUE, 0.9f, true);
        }

        public void act22_mary() {
            this.setTranslate(3.71f, 0.0f, 3.26f);
            this.setRotate(0.0f, 323.0f, 0.0f);
            this.mtn(286, 0, 239, 8, 8, 1.0f, true);
        }

        public void act22_utic4() {
            SCE02038.this.utic1.setVisible(true);
            SCE02038.this.utic2.setVisible(true);
            SCE02038.this.utic3.setVisible(true);
            SCE02038.this.rifle1.setVisible(true);
            SCE02038.this.rifle2.setVisible(true);
            SCE02038.this.rifle3_.setVisible(true);
            SCE02038.this.utic4.setVisible(6, true);
            SCE02038.this.utic4.setVisible(7, true);
        }

        public void act23_mary() {
            this.setTranslate(3.04f, 0.0f, 2.27f);
            this.setRotate(0.0f, 35.0f, 0.0f);
            while (true) {
                this.mtn(287, 30, 404, 8, 0, 1.0f, true);
                this.mtn(287, 405, 495, 8, 0, 0.8f, true);
                this.mtn(287, 405, 495, 8, 0, 0.8f, true);
            }
        }

        public void act24_mary() {
            this.setTranslate(4.74f, 0.0f, 4.71f);
            this.setRotate(0.0f, 277.0f, 0.0f);
            this.mtn(288, 0, 529, 8, 8, 1.0f, true);
        }

        public void act24_shelley() {
            this.setTranslate(-0.29f, 0.99f, 6.85f);
            this.setRotate(0.0f, -166.0f, 0.0f);
            this.setVisible(true);
            this.mtn(289, 0, 0, 8, 8, 1.0f, true);
        }

        public void act25_mary() {
            this.setTranslate(4.78f, 0.0f, 4.74f);
            this.setRotate(0.0f, 630.3f, 0.0f);
            this.setVisible(true);
            this.mtn(290, 0, 0, 8, 0, 1.0f, true);
        }

        public void act25_shelley() {
            this.setTranslate(-0.29f, 0.99f, 6.85f);
            this.setRotate(0.0f, -166.0f, 0.0f);
            this.setVisible(true);
            this.mtn(289, 0, 214, 8, 8, 1.0f, true);
        }

        public void act26_mary() {
            this.setTranslate(4.78f, 0.0f, 4.74f);
            this.setRotate(0.0f, 630.3f, 0.0f);
            this.mtn(290, 0, 354, 8, 0, 1.0f, true);
            this.mtn(290, 345, 354, 8, 0, -0.8f, true);
        }

        public void act2_jr() {
            this.setTranslate(5.05f, 0.0f, 1.68f);
            this.setRotate(0.0f, 22.0f, 0.0f);
            this.mtn(257, 30, 420, 8, 0, 1.0f, true);
        }

        public void act2_mary() {
            this.setTranslate(4.98f, -0.04f, 2.46f);
            this.setRotate(0.0f, 86.0f, 0.0f);
            this.mtn(258, 30, 245, 8, 0, 1.0f, true);
        }

        public void act2_shelley() {
            this.setTranslate(-0.35f, 0.99f, 6.76f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(259, 0, 0, 8, 0, 0.95f, true);
        }

        public void act3_shelley() {
            this.setTranslate(-0.35f, 0.99f, 6.76f);
            this.setRotate(0.0f, 180.0f, 0.0f);
            this.setVisible(true);
            this.mtn(259, 0, 269, 8, 8, 0.95f, true);
        }

        public void act4_jr() {
            this.setTranslate(5.05f, 0.0f, 1.68f);
            this.setRotate(0.0f, 39.0f, 0.0f);
            this.mtn(257, 175, 270, 8, 0, -1.0f, true);
            this.mtn(257, 175, 330, 8, 0, 1.0f, true);
        }

        public void act4_mary() {
            this.setTranslate(5.04f, -0.02f, 2.47f);
            this.setRotate(0.0f, 196.0f, 0.0f);
            this.mtn(261, 0, 210, 8, 8, 1.0f, true);
        }

        public void act5_jr() {
            this.setTranslate(5.25f, 0.0f, 1.68f);
            this.setRotate(0.0f, 280.0f, 0.0f);
        }

        public void act5_mary() {
            this.setTranslate(5.42f, 0.0f, 3.12f);
            this.setRotate(0.0f, 192.0f, 0.0f);
            this.mtn(262, 0, 100, 8, 0, 1.0f, true);
        }

        public void act6_mary() {
            this.setTranslate(3.86f, 0.0f, 3.01f);
            this.setRotate(0.0f, 126.0f, 0.0f);
            while (true) {
                this.mtn(263, 0, 70, 8, 0, 1.0f, true);
                this.mtn(263, 71, 120, 8, 0, 0.6f, true);
            }
        }

        public void act7_jr() {
            this.setTranslate(4.5f, 0.0f, 1.85f);
            this.setRotate(0.0f, 284.0f, 0.0f);
            this.mtn(264, 0, 765, 8, 8, 1.2f, true);
        }

        public void act7_mary() {
            this.setTranslate(3.57f, 0.0f, 2.9f);
            this.setRotate(0.0f, 171.0f, 0.0f);
            this.mtn(265, 0, 854, 8, 8, 1.2f, true);
        }

        public void act8_jr() {
            this.setTranslate(3.86f, 0.0f, 2.07f);
            this.setRotate(0.0f, 271.0f, 0.0f);
            this.mtn(266, 0, 325, 8, 0, 1.0f, true);
            this.mtn(266, 60, 60, 8, 0, 1.0f, true);
        }

        public void act8_mary() {
            this.setTranslate(3.57f, 0.0f, 2.89f);
            this.setRotate(0.0f, 531.0f, 0.0f);
            this.mtn(265, 764, 853, 8, 8, 1.0f, true);
        }

        public void act9_jr() {
            this.setTranslate(5.26f, 0.0f, 2.07f);
            this.setRotate(0.0f, 271.0f, 0.0f);
            this.mtn(266, 325, 739, 8, 8, 1.0f, true);
        }

        public void act9_mary() {
            this.setTranslate(3.57f, 0.0f, 3.04f);
            this.setRotate(0.0f, 575.0f, 0.0f);
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

        void act16_gunL1() {
            this.setTranslate(SCE02038.this.jr.px, SCE02038.this.jr.py, SCE02038.this.jr.pz);
            this.setRotate(SCE02038.this.jr.rx, SCE02038.this.jr.ry, SCE02038.this.jr.rz);
            this.setVisible(true);
            this.mtn(294, 0, 300, 8, 8, 1.0f, true);
        }

        void act16_gunR1() {
            this.setTranslate(SCE02038.this.jr.px, SCE02038.this.jr.py, SCE02038.this.jr.pz);
            this.setRotate(SCE02038.this.jr.rx, SCE02038.this.jr.ry, SCE02038.this.jr.rz);
            this.setVisible(true);
            this.mtn(293, 0, 300, 8, 8, 1.0f, true);
        }

        public void act17_cable_s() {
            SCE02038.this.mary.getTranslate();
            this.setTranslate(SCE02038.this.mary.px, SCE02038.this.mary.py, SCE02038.this.mary.pz);
            this.setRotate(SCE02038.this.mary.rx, SCE02038.this.mary.ry, SCE02038.this.mary.rz);
            this.setVisible(true);
            this.mtn(295, 0, 239, 8, 0, 1.0f, true);
        }

        public void act6_cable_l() {
            this.setTranslate(SCE02038.this.mary.px, SCE02038.this.mary.py, SCE02038.this.mary.pz);
            this.setRotate(SCE02038.this.mary.rx, SCE02038.this.mary.ry, SCE02038.this.mary.rz);
            this.setVisible(true);
            while (true) {
                this.mtn(291, 0, 70, 8, 0, 1.0f, true);
                this.mtn(291, 71, 120, 8, 0, 0.6f, true);
            }
        }

        public void act6_cable_l2() {
            while (true) {
                SCE02038.this.cable_l.setTranslate(SCE02038.this.mary.px, SCE02038.this.mary.py, SCE02038.this.mary.pz);
                SCE02038.this.cable_l.setRotate(SCE02038.this.mary.rx, SCE02038.this.mary.ry, SCE02038.this.mary.rz);
                System.sleep(1);
            }
        }

        public void act7_cable_l() {
            this.setTranslate(SCE02038.this.mary.px, SCE02038.this.mary.py, SCE02038.this.mary.pz);
            this.setRotate(SCE02038.this.mary.rx, SCE02038.this.mary.ry, SCE02038.this.mary.rz);
            this.mtn(292, 0, 854, 8, 8, 1.2f, true);
        }

        public void act8_cable_l() {
            this.setTranslate(SCE02038.this.mary.px, SCE02038.this.mary.py, SCE02038.this.mary.pz);
            this.setRotate(SCE02038.this.mary.rx, SCE02038.this.mary.ry, SCE02038.this.mary.rz);
            this.mtn(292, 764, 854, 8, 8, 1.0f, true);
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

        public void eyes10_jr() {
            SCE02038.this.jr.look_default();
            System.sleep(90);
            SCE02038.this.jr.look_eye_speed(1.5f);
            SCE02038.this.jr.look_eye_set(-3.5f, -2.5f);
        }

        public void eyes10_mary() {
            SCE02038.this.mary.look_default();
            SCE02038.this.mary.look_speed(0.15f);
            SCE02038.this.mary.look_char(SCE02038.this.jr);
        }

        public void eyes11_jr() {
            SCE02038.this.jr.look_default();
            SCE02038.this.jr.look_eye_speed(0.5f);
            SCE02038.this.jr.look_eye_set(3.5f, -2.5f);
            System.sleep(50);
            SCE02038.this.jr.look_eye_speed(2.0f);
            SCE02038.this.jr.look_eye_set(0.0f, 0.0f);
        }

        public void eyes11_mary() {
            SCE02038.this.mary.look_default();
        }

        public void eyes12_jr() {
            SCE02038.this.jr.look_default();
        }

        public void eyes16_jr() {
            SCE02038.this.jr.look_default();
            SCE02038.this.jr.look_eye_speed(999.0f);
            SCE02038.this.jr.look_eye_set(3.0f, -1.5f);
            System.sleep(45);
            SCE02038.this.jr.look_default();
        }

        public void eyes18_jr() {
            SCE02038.this.jr.look_default();
            System.sleep(75);
            SCE02038.this.target.setTranslate(-0.5f, 0.9f, 1.17f);
            SCE02038.this.target.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.jr.look_eye_speed(1.5f);
            SCE02038.this.jr.look_eye_set(3.0f, 2.0f);
            System.sleep(60);
        }

        public void eyes1_jr() {
            SCE02038.this.jr.look_default();
        }

        public void eyes20_jr() {
            SCE02038.this.jr.look_default();
            System.sleep(60);
            SCE02038.this.jr.look_eye_speed(5.5f);
            SCE02038.this.jr.look_eye_set(3.0f, 2.0f);
            System.sleep(210);
            SCE02038.this.jr.look_eye_speed(5.0f);
            SCE02038.this.jr.look_eye_set(-3.0f, 2.0f);
            System.sleep(60);
            SCE02038.this.jr.look_default();
        }

        public void eyes21_mary() {
            SCE02038.this.mary.look_default();
            SCE02038.this.mary.look_speed(0.1f);
            while (true) {
                SCE02038.this.mary.look_char(SCE02038.this.jr);
                System.sleep(1);
            }
        }

        public void eyes22_mary() {
            SCE02038.this.mary.look_default();
        }

        public void eyes2_jr() {
            SCE02038.this.jr.look_default();
            System.sleep(195);
            SCE02038.this.jr.look_speed(0.7f);
            SCE02038.this.jr.look_char(SCE02038.this.mary);
            System.sleep(60);
            while (true) {
                SCE02038.this.jr.look_char(SCE02038.this.mary);
                System.sleep(1);
            }
        }

        public void eyes4_jr() {
            SCE02038.this.jr.look_default();
            System.sleep(160);
            SCE02038.this.jr.look_speed(0.09f);
            SCE02038.this.jr.look_char(SCE02038.this.mary);
        }

        public void eyes5_jr() {
            SCE02038.this.jr.look_default();
        }

        public void eyes7_1_jr() {
            SCE02038.this.jr.look_speed(0.09f);
            while (true) {
                SCE02038.this.jr.look_char(SCE02038.this.mary);
                System.sleep(1);
            }
        }

        public void eyes7_1_mary() {
            System.sleep(210);
            SCE02038.this.mary.look_speed(0.15f);
            while (true) {
                SCE02038.this.mary.look_char(SCE02038.this.jr);
                System.sleep(1);
            }
        }

        public void eyes7_jr() {
            SCE02038.this.jr.look_default();
        }

        public void eyes7_mary() {
            SCE02038.this.mary.look_default();
        }

        public void eyes8_jr() {
            SCE02038.this.jr.look_default();
            SCE02038.this.jr.look_eye_speed(10.0f);
            SCE02038.this.jr.look_eye_set(-3.5f, -2.5f);
            System.sleep(85);
            SCE02038.this.jr.look_eye_speed(1.5f);
            SCE02038.this.jr.look_eye_set(-0.0f, 1.0f);
        }

        public void eyes8_mary() {
            SCE02038.this.mary.look_default();
        }

        public void eyes9_jr() {
            SCE02038.this.jr.look_default();
        }

        public void face10_jr() {
            SCE02038.this.jr.face.mtn(15, 63, 63, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(5);
            SCE02038.this.jr.face.mtn(15, 63, 66, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(15, 66, 120, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(11, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(35);
            SCE02038.this.jr.face.mtn(12, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(16, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(15, 0, 2, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(16, 0, 120, 16, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(35);
        }

        public void face10_mary() {
            System.sleep(100);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(51);
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(14);
        }

        public void face11_jr() {
            SCE02038.this.jr.face.mtn(1, 105, 105, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(12);
            SCE02038.this.jr.face.mtn(1, 105, 110, 8, 0, 0.5f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(8);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(22);
            SCE02038.this.jr.face.mtn(2, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(13);
            SCE02038.this.jr.face.mtn(6, 0, 120, 3, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face13_jr() {
            SCE02038.this.jr.face.mtn(2, 0, 0, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face13_mary() {
            SCE02038.this.mary.face.mtn(2, 0, 0, 8, 0, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(14);
        }

        public void face14_jr() {
            SCE02038.this.jr.face.mtn(16, 63, 120, 8, 0, 0.5f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(15, 0, 3, 8, 0, 0.3f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
            SCE02038.this.jr.face.mtn(16, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
        }

        public void face15_jr() {
            SCE02038.this.jr.face.mtn(2, 63, 63, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face16_1_jr() {
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(40);
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
        }

        public void face16_jr() {
            System.sleep(25);
            SCE02038.this.jr.face.mtn(6, 63, 120, 8, 0, 0.1f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(2, 0, 120, 12, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(1, 0, 2, 8, 0, 0.25f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(45);
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face17_mary() {
            SCE02038.this.mary.look_default();
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(50);
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(15);
            SCE02038.this.mary.face.mtn(5, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(60);
            SCE02038.this.mary.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(5);
        }

        public void face18_jr() {
            SCE02038.this.jr.face.mtn(7, 60, 68, 2, 0, -1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
            SCE02038.this.jr.face.mtn(8, 60, 68, 2, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(7, 0, 3, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(17);
            SCE02038.this.jr.face.mtn(7, 0, 3, 8, 0, -0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(7, 0, 24, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(24);
            SCE02038.this.jr.face.mtn(7, 60, 68, 8, 0, 0.4f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(19);
            SCE02038.this.jr.face.mtn(7, 65, 68, 8, 0, -0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
        }

        public void face19_mary() {
            SCE02038.this.mary.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(23);
            SCE02038.this.mary.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            SCE02038.this.mary.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(87);
            SCE02038.this.mary.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            System.sleep(10);
        }

        public void face1_jr() {
            while (true) {
                SCE02038.this.jr.face.mtn(2, 60, 66, 8, 8, 1.0f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(7);
                SCE02038.this.jr.face.mtn(2, 0, 59, 8, 8, 1.0f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(60);
                SCE02038.this.jr.face.mtn(2, 60, 66, 8, 8, 0.4f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(18);
                SCE02038.this.jr.face.mtn(2, 67, 101, 8, 8, 1.0f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(35);
                SCE02038.this.jr.face.mtn(2, 102, 108, 8, 8, 0.7f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(10);
                SCE02038.this.jr.face.mtn(2, 109, 120, 8, 8, 1.0f, false);
                SCE02038.this.jr.face.start(4, null);
                System.sleep(42);
            }
        }

        public void face1_mary() {
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            SCE02038.this.mary.look_default();
        }

        public void face20_jr() {
            SCE02038.this.jr.face.mtn(1, 61, 65, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(40);
            SCE02038.this.jr.face.mtn(5, 60, 65, 8, 0, -0.3f, false);
            System.sleep(35);
            SCE02038.this.jr.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(14);
            SCE02038.this.jr.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(13);
            SCE02038.this.jr.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(23);
            SCE02038.this.jr.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(50);
            SCE02038.this.jr.face.mtn(6, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(85);
            SCE02038.this.jr.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(25);
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
        }

        public void face21_jr() {
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(7);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(13);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(50);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(35);
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
        }

        public void face22_mary() {
            System.sleep(120);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(50);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
        }

        public void face23_mary() {
            System.sleep(30);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(90);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
        }

        public void face24_mary() {
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(18);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(15);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(62);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            System.sleep(160);
            SCE02038.this.mary.face.mtn(3, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(20);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(9);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(26);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(45);
            SCE02038.this.mary.face.mtn(3, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(35);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
        }

        public void face25_shelley() {
            SCE02038.this.shelley.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(17);
            SCE02038.this.shelley.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(10);
            SCE02038.this.shelley.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(61);
            SCE02038.this.shelley.face.mtn(2, 0, 120, 2, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(3);
            SCE02038.this.shelley.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(24);
            SCE02038.this.shelley.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(10);
        }

        public void face26_1_mary() {
            SCE02038.this.mary.face.mtn(1, 97, 98, 8, 0, 0.2f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(20);
            SCE02038.this.mary.face.mtn(1, 94, 98, 8, 0, -0.3f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(8);
            System.sleep(12);
            SCE02038.this.mary.face.mtn(1, 94, 99, 8, 0, 0.3f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(27);
            System.sleep(21);
            SCE02038.this.mary.face.mtn(1, 0, 99, 8, 8, -1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(60);
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(12);
        }

        public void face26_mary() {
            while (true) {
                SCE02038.this.mary.face.mtn(1, 94, 98, 8, 0, 0.35f, false);
                SCE02038.this.mary.face.start(4, null);
                System.sleep(60);
                SCE02038.this.mary.face.mtn(1, 94, 98, 8, 0, -0.25f, false);
                SCE02038.this.mary.face.start(4, null);
                System.sleep(80);
            }
        }

        public void face2_mary() {
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(30);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(55);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(20);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(39);
            SCE02038.this.mary.face.mtn(4, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(16);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(54);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(11);
        }

        public void face3_shelley() {
            SCE02038.this.shelley.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(20);
            SCE02038.this.shelley.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(75);
            SCE02038.this.shelley.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(14);
            SCE02038.this.shelley.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(64);
            SCE02038.this.shelley.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(27);
            SCE02038.this.shelley.face.mtn(5, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(38);
            SCE02038.this.shelley.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.shelley.face.start(4, null);
            System.sleep(17);
        }

        public void face4_jr() {
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(120);
            SCE02038.this.jr.face.mtn(1, 0, 1, 8, 0, 0.25f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(30);
            SCE02038.this.jr.face.mtn(1, 0, 1, 8, 0, -0.25f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face4_mary() {
            SCE02038.this.mary.face.mtn(1, 40, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(27);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(33);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(30);
            SCE02038.this.mary.face.mtn(3, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(45);
            SCE02038.this.mary.face.mtn(4, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(55);
        }

        public void face6_mary() {
            System.sleep(70);
            SCE02038.this.mary.face.mtn(2, 53, 58, 8, 0, 0.2f, false);
            SCE02038.this.mary.face.start(4, null);
        }

        public void face7_1_mary() {
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 9, 0.4f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(20);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(30);
            SCE02038.this.mary.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(95);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            System.sleep(15);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(78);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(12);
        }

        public void face7_jr() {
            System.sleep(60);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(40);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(19);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(58);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(6);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(36);
            SCE02038.this.jr.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
        }

        public void face7_mary() {
            SCE02038.this.mary.face.mtn(2, 96, 96, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
        }

        public void face8_1_jr() {
            SCE02038.this.jr.face.mtn(296, 0, 120, 30, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(44);
            SCE02038.this.jr.face.mtn(5, 63, 66, 5, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(5, 66, 120, 5, 0, 1.3f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(46);
            SCE02038.this.jr.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(5, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
        }

        public void face8_jr() {
            SCE02038.this.jr.face.mtn(2, 0, 59, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(60);
            SCE02038.this.jr.face.mtn(2, 60, 66, 8, 0, 0.4f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(18);
            SCE02038.this.jr.face.mtn(2, 66, 100, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(17);
            SCE02038.this.jr.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(6, 60, 63, 8, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(25);
            SCE02038.this.jr.face.mtn(296, 0, 120, 30, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(40);
            SCE02038.this.jr.face.mtn(296, 40, 120, 8, 0, 0.4f, false);
            System.sleep(2);
            SCE02038.this.jr.face.mtn(297, 0, 120, 30, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(13);
        }

        public void face8_mary() {
            System.sleep(15);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(55);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
            SCE02038.this.mary.face.mtn(1, 0, 120, 8, 8, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(55);
            SCE02038.this.mary.face.mtn(2, 0, 120, 5, 9, 1.0f, false);
            SCE02038.this.mary.face.start(4, null);
            System.sleep(10);
        }

        public void face9_99_jr() {
            SCE02038.this.jr.face.mtn(5, 50, 60, 5, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(5, 60, 63, 5, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(296, 0, 120, 30, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(40);
            SCE02038.this.jr.face.mtn(5, 63, 66, 5, 0, 0.2f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(6, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(16, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
            SCE02038.this.jr.face.mtn(15, 10, 60, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(50);
            SCE02038.this.jr.face.mtn(15, 60, 63, 8, 0, 0.3f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(16, 63, 63, 8, 0, 0.8f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
        }

        public void face9_jr() {
            SCE02038.this.jr.face.mtn(7, 0, 120, 16, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(1, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(60);
            SCE02038.this.jr.face.mtn(2, 0, 120, 8, 9, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(20);
            SCE02038.this.jr.face.mtn(1, 10, 60, 8, 0, 1.0f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(50);
            SCE02038.this.jr.face.mtn(1, 60, 63, 8, 0, 0.3f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(15);
            SCE02038.this.jr.face.mtn(2, 63, 63, 8, 0, 0.8f, false);
            SCE02038.this.jr.face.start(4, null);
            System.sleep(10);
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

        void _roop1_stop() {
            System.sleep(1);
            SCE02038.this.shell1.disp(false);
            SCE02038.this.shell1.clearEffect();
        }

        void _roop2_stop() {
            System.sleep(1);
            SCE02038.this.shell2.disp(false);
            SCE02038.this.shell2.clearEffect();
        }

        void _roop3_stop() {
            System.sleep(1);
            SCE02038.this.shell3.disp(false);
            SCE02038.this.shell3.clearEffect();
        }

        void _roop4_stop() {
            System.sleep(1);
            SCE02038.this.shell4.disp(false);
            SCE02038.this.shell4.clearEffect();
        }

        void act12_rifle1() {
            SCE02038.this.rifle1.setParent(SCE02038.this.utic1, 72);
            SCE02038.this.rifle1.setTranslate(0.03f, -0.0f, 0.02f);
            SCE02038.this.rifle1.setRotate(6.0f, -23.0f, -3.0f);
            SCE02038.this.rifle1.setVisible(true);
        }

        void act12_rifle2() {
            SCE02038.this.rifle2.setParent(SCE02038.this.utic2, 72);
            SCE02038.this.rifle2.setTranslate(0.04f, -0.03f, 0.03f);
            SCE02038.this.rifle2.setRotate(15.0f, -9.0f, 12.0f);
            SCE02038.this.rifle2.setVisible(true);
            System.sleep(29);
            SCE02038.this.shell2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.shell2.setScale(0.5f, 0.5f, 0.5f);
            SCE02038.this.shell2.setTranslate(0.0f, 0.0f, 0.0f);
            SCE02038.this.shell2.setCaster(SCE02038.this.rifle2);
            SCE02038.this.shell2.setForceLoop(false);
            SCE02038.this.shell2.disp(true);
        }

        void act12_rifle3() {
            SCE02038.this.rifle3.setParent(SCE02038.this.utic3, 72);
            SCE02038.this.rifle3.setTranslate(0.03f, 0.0f, 0.01f);
            SCE02038.this.rifle3.setRotate(9.0f, -27.0f, 7.0f);
            SCE02038.this.rifle3.setVisible(true);
        }

        void act12_rifle4() {
            SCE02038.this.rifle4.setParent(SCE02038.this.utic4, 72);
            SCE02038.this.rifle4.setTranslate(0.04f, -0.02f, 0.05f);
            SCE02038.this.rifle4.setRotate(35.0f, -26.0f, 3.0f);
            SCE02038.this.rifle4.setVisible(true);
        }

        void act13_bullet1() {
            this.setTranslate(-0.34f, 0.81f, 11.91f);
            this.setRotate(0.0f, 156.0f, 0.0f);
            this.setVisible(true);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 15.0f;
            fArray[5] = 3.59f;
            fArray[6] = 0.635f;
            fArray[7] = -0.23f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            SCE02038.this.movSPL.setCtrlVertex(fArray2, 1, 1, 10);
            SCE02038.this.rotSPL.setCtrlVertex(fArray4, 0, 1, 10);
            this.move(SCE02038.this.movSPL, false);
            this.rotate(SCE02038.this.rotSPL, true);
        }

        void act13_bullet2() {
            this.setTranslate(-0.34f, 0.81f, 11.91f);
            this.setRotate(0.0f, 156.0f, 0.0f);
            this.setScale(1.6f, 1.6f, 1.6f);
            System.sleep(9);
            this.setVisible(true);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 15.0f;
            fArray[5] = 3.46f;
            fArray[6] = 0.81f;
            fArray[7] = 0.15f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            SCE02038.this.movSPL1.setCtrlVertex(fArray2, 1, 1, 10);
            SCE02038.this.rotSPL1.setCtrlVertex(fArray4, 0, 1, 10);
            this.move(SCE02038.this.movSPL1, false);
            this.rotate(SCE02038.this.rotSPL1, true);
        }

        void act13_bullet3() {
            this.setTranslate(0.08f, -0.015f, -0.01f);
            this.setRotate(-14.0f, 2.0f, 0.0f);
            System.sleep(18);
            this.setVisible(true);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 15.0f;
            fArray[5] = 3.52f;
            fArray[6] = 0.68f;
            fArray[7] = -0.23f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            SCE02038.this.movSPL2.setCtrlVertex(fArray2, 1, 1, 10);
            SCE02038.this.rotSPL2.setCtrlVertex(fArray4, 0, 1, 10);
            this.move(SCE02038.this.movSPL2, false);
            this.rotate(SCE02038.this.rotSPL2, true);
        }

        void act13_gunR2() {
            this.setParent(SCE02038.this.jr, 72);
            this.setTranslate(0.08f, -0.02f, 0.01f);
            this.setRotate(18.0f, -6.0f, 6.0f);
            System.sleep(8);
            this.setVisible(true);
            SCE02038.this.jr.setVisible(11, false);
            SCE02038.this.jr.setVisible(15, true);
            System.sleep(17);
            SCE02038.this.fire1 = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.fire1.setTranslate(0.0f, 0.03f, 0.0f);
            SCE02038.this.fire1.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.fire1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.fire1.setCaster(SCE02038.this.gunR2);
            SCE02038.this.fire1.setForceLoop(false);
            SCE02038.this.fire1.noAttach(false);
        }

        void act13_impact1() {
            System.sleep(15);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(2.0f, 0.05f, 4.8f);
            SCE02038.this.impact1.setRotate(0.0f, 164.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(6);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(2.22f, 0.03f, 4.3f);
            SCE02038.this.impact1.setRotate(0.0f, 148.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(6);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(2.51f, 1.07f, 0.74f);
            SCE02038.this.impact1.setRotate(0.0f, 164.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(30);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-1.44f, 2.7f, 10.97f);
            SCE02038.this.impact1.setRotate(102.0f, 211.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(6);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-0.79f, 3.87f, 11.8f);
            SCE02038.this.impact1.setRotate(-221.0f, -188.0f, 18.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
        }

        void act13_impact2() {
            System.sleep(15);
            System.sleep(9);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(1.8f, 0.04f, 3.64f);
            SCE02038.this.impact2.setRotate(0.0f, -192.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(9);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(2.95f, 0.04f, 3.21f);
            SCE02038.this.impact2.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(9);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(2.89f, -0.02f, 2.14f);
            SCE02038.this.impact2.setRotate(0.0f, 150.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(9);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(3.49f, 0.96f, 1.04f);
            SCE02038.this.impact2.setRotate(-324.0f, -230.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
        }

        void act13_rifle1() {
            System.sleep(15);
            SCE02038.this.shell1.disp(true);
            int n = 0;
            while (n < 11) {
                SCE02038.this.shell1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.shell1.setScale(0.5f, 0.5f, 0.5f);
                SCE02038.this.shell1.setTranslate(0.0f, 0.0f, 0.0f);
                SCE02038.this.shell1.setCaster(SCE02038.this.rifle1);
                SCE02038.this.shell1.setForceLoop(false);
                System.sleep(6);
                ++n;
            }
        }

        void act13_rifle2() {
            System.sleep(5);
            SCE02038.this.shell2.disp(true);
            int n = 0;
            while (n < 7) {
                SCE02038.this.shell2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.shell2.setScale(0.5f, 0.5f, 0.5f);
                SCE02038.this.shell2.setTranslate(0.0f, 0.0f, 0.0f);
                SCE02038.this.shell2.setCaster(SCE02038.this.rifle2);
                SCE02038.this.shell2.setForceLoop(false);
                System.sleep(9);
                ++n;
            }
        }

        void act13_rifle3() {
            System.sleep(40);
            SCE02038.this.shell3.disp(true);
            int n = 0;
            while (n < 8) {
                SCE02038.this.shell3 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.shell3.setScale(0.5f, 0.5f, 0.5f);
                SCE02038.this.shell3.setTranslate(0.0f, 0.0f, 0.0f);
                SCE02038.this.shell3.setCaster(SCE02038.this.rifle3);
                SCE02038.this.shell3.setForceLoop(false);
                System.sleep(8);
                ++n;
            }
        }

        void act13_rifle4() {
            System.sleep(45);
            SCE02038.this.shell4.disp(true);
            int n = 0;
            while (n < 7) {
                SCE02038.this.shell4 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.shell4.setScale(0.5f, 0.5f, 0.5f);
                SCE02038.this.shell4.setTranslate(0.0f, 0.0f, 0.0f);
                SCE02038.this.shell4.setCaster(SCE02038.this.rifle4);
                SCE02038.this.shell4.setForceLoop(false);
                System.sleep(10);
                ++n;
            }
        }

        void act14_gunL2() {
            this.setParent(SCE02038.this.jr, 60);
            this.setTranslate(0.07f, 0.01f, -0.01f);
            this.setRotate(0.0f, 11.0f, 0.0f);
            System.sleep(22);
            this.setVisible(true);
            SCE02038.this.jr.setVisible(10, false);
            SCE02038.this.jr.setVisible(14, true);
            System.sleep(19);
            int n = 0;
            while (n < 6) {
                SCE02038.this.fire2 = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.fire2.setTranslate(0.0f, 0.03f, 0.0f);
                SCE02038.this.fire2.setRotate(0.0f, 0.0f, 0.0f);
                SCE02038.this.fire2.setScale(0.2f, 0.2f, 0.2f);
                SCE02038.this.fire2.setCaster(SCE02038.this.gunL2);
                SCE02038.this.fire2.setForceLoop(false);
                System.sleep(6);
                ++n;
            }
        }

        void act14_gunR2() {
            this.setTranslate(0.08f, -0.02f, 0.01f);
            this.setRotate(18.0f, -6.0f, 6.0f);
            System.sleep(26);
            System.sleep(19);
            int n = 0;
            while (n < 6) {
                SCE02038.this.fire1 = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.fire1.setTranslate(0.0f, 0.03f, 0.0f);
                SCE02038.this.fire1.setRotate(0.0f, 0.0f, 0.0f);
                SCE02038.this.fire1.setScale(0.2f, 0.2f, 0.2f);
                SCE02038.this.fire1.setCaster(SCE02038.this.gunR2);
                SCE02038.this.fire1.setForceLoop(false);
                System.sleep(6);
                ++n;
            }
        }

        void act14_impact3() {
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(0.97f, 0.04f, 2.56f);
            SCE02038.this.impact1.setRotate(0.0f, 29.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(8);
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-0.47f, 0.04f, 3.6f);
            SCE02038.this.impact1.setRotate(0.0f, 22.0f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-0.03f, 0.06f, 2.06f);
            SCE02038.this.impact1.setRotate(0.0f, -315.99f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-0.54f, 0.06f, 1.66f);
            SCE02038.this.impact1.setRotate(0.0f, -319.99f, 0.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-1.95f, 0.01f, 0.76f);
            SCE02038.this.impact1.setRotate(0.0f, -347.99f, 40.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            System.sleep(8);
            SCE02038.this.impact1 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(-4.65f, 0.85f, -1.74f);
            SCE02038.this.impact1.setRotate(0.0f, -353.99f, 40.0f);
            SCE02038.this.impact1.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
        }

        void act14_impact4() {
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(0.08f, 0.02f, 2.21f);
            SCE02038.this.impact2.setRotate(0.0f, -24.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-4.69f, 0.85f, -2.02f);
            SCE02038.this.impact2.setRotate(0.0f, 40.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-1.9f, 0.1f, 2.45f);
            SCE02038.this.impact2.setRotate(0.0f, 44.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-0.92f, 0.02f, 3.01f);
            SCE02038.this.impact2.setRotate(0.0f, 40.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-2.12f, 0.02f, 2.31f);
            SCE02038.this.impact2.setRotate(0.0f, 40.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-4.48f, 0.02f, 1.07f);
            SCE02038.this.impact2.setRotate(0.0f, 37.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1699, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setTranslate(-2.11f, 0.05f, 3.08f);
            SCE02038.this.impact2.setRotate(0.0f, 58.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
        }

        void act15_blood() {
            System.sleep(40);
            SCE02038.this.blood.setScale(0.2f, 0.2f, 0.2f);
            SCE02038.this.blood.setTranslate(1.44f, 1.39f, 8.77f);
            SCE02038.this.blood.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.blood.setScale(0.5f, 0.5f, 0.5f);
            SCE02038.this.blood.disp(true);
            SCE02038.this.blood.setForceLoop(false);
        }

        void act15_rifle3() {
            System.sleep(36);
            SCE02038.this.ASmove(SCE02038.this.rifle3, 4, 0.03f, 0.0f, 0.01f, 9.0f, -27.0f, 2.0f);
            System.sleep(8);
            SCE02038.this.ASmove(SCE02038.this.rifle3, 4, 0.05f, -0.02f, 0.01f, 9.0f, -5.0f, -7.0f);
            System.sleep(4);
            SCE02038.this.ASmove(SCE02038.this.rifle3, 5, 0.05f, -0.02f, 0.03f, 9.0f, 7.0f, -7.0f);
            SCE02038.this.ASmove(SCE02038.this.rifle3, 2, 0.05f, -0.02f, 0.03f, 9.0f, -7.0f, -7.0f);
            SCE02038.this.rifle3.setVisible(false);
            SCE02038.this.rifle3_.setTranslate(1.91f, 0.06f, 9.94f);
            SCE02038.this.rifle3_.setRotate(264.99f, -173.99f, 457.97f);
            SCE02038.this.rifle3_.setVisible(true);
            SCE02038.this.ASmove(SCE02038.this.rifle3_, 15, 1.9f, 0.04f, 9.94f, 267.99f, -180.99f, 457.97f);
        }

        void act15_rifle4() {
            SCE02038.this.shell4.disp(true);
            int n = 0;
            while (n < 100) {
                SCE02038.this.shell4 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.shell4.setScale(0.5f, 0.5f, 0.5f);
                SCE02038.this.shell4.setTranslate(0.0f, 0.0f, 0.0f);
                SCE02038.this.shell4.setCaster(SCE02038.this.rifle4);
                SCE02038.this.shell4.setForceLoop(false);
                System.sleep(5);
                ++n;
            }
        }

        void act16_impact1() {
            SCE02038.this.smoke1 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.setScale(2.15f, 2.15f, 2.15f);
            SCE02038.this.smoke1.setTranslate(-0.29f, -0.09f, 3.13f);
            SCE02038.this.smoke1.setRotate(0.0f, 200.0f, 0.0f);
            SCE02038.this.smoke1.disp(true);
            SCE02038.this.smoke1.setForceLoop(false);
        }

        void act17_impact1() {
            System.sleep(15);
            SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(4.0f, 0.71f, 2.18f);
            SCE02038.this.impact1.setRotate(167.0f, 545.0f, 117.0f);
            SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
            while (true) {
                System.sleep(30);
                SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact1.setTranslate(4.0f, 0.71f, 2.18f);
                SCE02038.this.impact1.setRotate(167.0f, 545.0f, 117.0f);
                SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact1.disp(true);
                SCE02038.this.impact1.setForceLoop(false);
                System.sleep(70);
                SCE02038.this.spark1.disp(true);
                SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact1.setTranslate(4.0f, 0.71f, 2.18f);
                SCE02038.this.impact1.setRotate(167.0f, 545.0f, 117.0f);
                SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact1.disp(true);
                SCE02038.this.impact1.setForceLoop(false);
                System.sleep(20);
            }
        }

        void act17_impact2() {
            System.sleep(10);
            SCE02038.this.impact2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact2.setScale(0.1f, 0.1f, 0.1f);
            SCE02038.this.impact2.setTranslate(0.96f, 1.33f, -0.73f);
            SCE02038.this.impact2.setRotate(64.0f, 81.0f, 165.0f);
            SCE02038.this.impact2.disp(true);
            SCE02038.this.impact2.setForceLoop(false);
            System.sleep(30);
            while (true) {
                System.sleep(30);
                SCE02038.this.impact2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact2.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact2.setTranslate(0.96f, 1.33f, -0.73f);
                SCE02038.this.impact2.setRotate(64.0f, 81.0f, 165.0f);
                SCE02038.this.impact2.disp(true);
                SCE02038.this.impact2.setForceLoop(false);
                System.sleep(50);
                SCE02038.this.impact2 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact2.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact2.setTranslate(0.96f, 1.33f, -0.73f);
                SCE02038.this.impact2.setRotate(64.0f, 81.0f, 165.0f);
                SCE02038.this.impact2.disp(true);
                SCE02038.this.impact2.setForceLoop(false);
                System.sleep(30);
            }
        }

        void act17_short() {
            SCE02038.this.target.setParent(SCE02038.this.cable_s, 9);
            SCE02038.this.target.setTranslate(0.0f, 0.0f, 0.0f);
            SCE02038.this.target.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.spark.setCaster(SCE02038.this.target);
            SCE02038.this.spark.disp(true);
            SCE02038.this.spark.setForceLoop(true);
            System.sleep(245);
            SCE02038.this.spark.disp(false);
        }

        void act17_spark1() {
            SCE02038.this.smoke1 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.setScale(2.15f, 2.15f, 2.15f);
            SCE02038.this.smoke1.setTranslate(3.9f, 0.04f, 2.0f);
            SCE02038.this.smoke1.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.disp(true);
            SCE02038.this.smoke1.setForceLoop(false);
            SCE02038.this.spark1 = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.spark1.setScale(0.25f, 0.25f, 0.25f);
            SCE02038.this.spark1.setTranslate(3.97f, 0.52f, 2.2f);
            SCE02038.this.spark1.setRotate(200.0f, -23.0f, 200.0f);
            SCE02038.this.spark1.disp(true);
            SCE02038.this.spark1.setForceLoop(true);
            SCE02038.this.spark1.noAttach(true);
            while (true) {
                SCE02038.this.spark1.disp(false);
                System.sleep(5);
                SCE02038.this.spark1.disp(true);
                System.sleep(15);
                SCE02038.this.spark1.disp(false);
                System.sleep(30);
                SCE02038.this.spark1.disp(true);
                System.sleep(15);
                SCE02038.this.spark1.disp(false);
                System.sleep(15);
            }
        }

        void act17_spark2() {
            SCE02038.this.spark2 = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.spark2.setScale(0.25f, 0.25f, 0.25f);
            SCE02038.this.spark2.setTranslate(3.97f, 0.52f, 2.2f);
            SCE02038.this.spark2.setRotate(200.0f, -23.0f, 200.0f);
            SCE02038.this.spark2.disp(true);
            SCE02038.this.spark2.setForceLoop(true);
            SCE02038.this.spark2.noAttach(true);
            while (true) {
                SCE02038.this.spark2.disp(false);
                System.sleep(15);
                SCE02038.this.spark2.disp(true);
                System.sleep(60);
                SCE02038.this.spark2.disp(false);
                System.sleep(20);
                SCE02038.this.spark2.disp(true);
                System.sleep(15);
            }
        }

        void act18_impact1() {
            SCE02038.this.smoke1 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.setScale(2.55f, 2.55f, 2.55f);
            SCE02038.this.smoke1.setTranslate(-0.73f, 0.13f, 0.72f);
            SCE02038.this.smoke1.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.disp(true);
            SCE02038.this.smoke1.setForceLoop(true);
            SCE02038.this.smoke1 = new Effect(1534, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.setScale(2.15f, 2.15f, 2.15f);
            SCE02038.this.smoke1.setTranslate(3.9f, 0.04f, 2.0f);
            SCE02038.this.smoke1.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.disp(true);
            SCE02038.this.smoke1.setForceLoop(true);
            SCE02038.this.spark1 = new Effect(1520, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.spark1.setScale(0.25f, 0.25f, 0.25f);
            SCE02038.this.spark1.setTranslate(3.97f, 0.52f, 2.2f);
            SCE02038.this.spark1.setRotate(200.0f, -23.0f, 200.0f);
            SCE02038.this.spark1.disp(true);
            SCE02038.this.spark1.setForceLoop(true);
            System.sleep(15);
            SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
            SCE02038.this.impact1.setTranslate(4.0f, 0.71f, 2.18f);
            SCE02038.this.impact1.setRotate(167.0f, 545.0f, 117.0f);
            SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(true);
            while (true) {
                SCE02038.this.spark1.disp(false);
                System.sleep(5);
                SCE02038.this.spark1.disp(true);
                System.sleep(15);
                SCE02038.this.spark1.disp(false);
                System.sleep(10);
                SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact1.setTranslate(4.2f, 0.55f, 2.26f);
                SCE02038.this.impact1.setRotate(200.0f, -25.0f, 200.0f);
                SCE02038.this.impact1.disp(true);
                SCE02038.this.impact1.setForceLoop(false);
                System.sleep(20);
                SCE02038.this.spark1.disp(true);
                System.sleep(10);
                System.sleep(5);
                SCE02038.this.spark1.disp(false);
                System.sleep(15);
                SCE02038.this.spark1.disp(true);
                System.sleep(5);
                SCE02038.this.spark1.disp(false);
                System.sleep(15);
                SCE02038.this.spark1.disp(true);
                SCE02038.this.impact1 = new Effect(1537, 0.0f, 0.0f, 0.0f, 0.0f);
                SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
                SCE02038.this.impact1.setTranslate(4.2f, 0.55f, 2.26f);
                SCE02038.this.impact1.setRotate(200.0f, -25.0f, 200.0f);
                SCE02038.this.impact1.disp(true);
                SCE02038.this.impact1.setForceLoop(false);
                System.sleep(5);
                SCE02038.this.spark1.disp(false);
                System.sleep(15);
            }
        }

        void act1_space() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, -6.0f, 0.0f);
        }

        void act21_map() {
            Stage.setVisible(212, false);
            Stage.setVisible(213, false);
            Stage.setVisible(214, false);
            Stage.setVisible(215, false);
            Stage.setVisible(217, false);
            Stage.setVisible(218, false);
            Stage.setVisible(219, false);
            Stage.setVisible(220, false);
            Stage.setVisible(221, false);
            Stage.setVisible(222, false);
            Stage.setVisible(223, false);
            Stage.setVisible(226, false);
            Stage.setVisible(231, false);
            Stage.setVisible(232, false);
            Stage.setVisible(233, false);
            Stage.setVisible(234, false);
            Stage.setVisible(235, false);
            Stage.setVisible(245, false);
            Stage.setVisible(246, false);
            Stage.setVisible(247, false);
            Stage.setVisible(248, false);
            Stage.setVisible(251, false);
            Stage.setVisible(252, false);
            Stage.setVisible(253, false);
            Stage.setVisible(254, false);
            Stage.setVisible(255, false);
            Stage.setVisible(257, false);
            Stage.setVisible(258, false);
            Stage.setVisible(259, false);
            Stage.setVisible(260, false);
            Stage.setVisible(271, false);
            Stage.setVisible(272, false);
            Stage.setVisible(273, false);
            Stage.setVisible(274, false);
            Stage.setVisible(275, false);
            Stage.setVisible(276, false);
            Stage.setVisible(277, false);
            Stage.setVisible(278, false);
            Stage.setVisible(279, false);
            Stage.setVisible(280, false);
            Stage.setVisible(281, false);
            Stage.setVisible(282, false);
            Stage.setVisible(283, false);
            Stage.setVisible(284, false);
            Stage.setVisible(285, false);
            Stage.setVisible(286, false);
            Stage.setVisible(287, false);
            Stage.setVisible(288, false);
            Stage.setVisible(289, false);
            Stage.setVisible(290, false);
            Stage.setVisible(291, false);
        }

        void act22_map() {
            Stage.setVisible(212, true);
            Stage.setVisible(213, true);
            Stage.setVisible(214, true);
            Stage.setVisible(215, true);
            Stage.setVisible(217, true);
            Stage.setVisible(218, true);
            Stage.setVisible(219, true);
            Stage.setVisible(220, true);
            Stage.setVisible(221, true);
            Stage.setVisible(222, true);
            Stage.setVisible(223, true);
            Stage.setVisible(226, true);
            Stage.setVisible(231, true);
            Stage.setVisible(232, true);
            Stage.setVisible(233, true);
            Stage.setVisible(234, true);
            Stage.setVisible(235, true);
            Stage.setVisible(245, true);
            Stage.setVisible(246, true);
            Stage.setVisible(247, true);
            Stage.setVisible(248, true);
            Stage.setVisible(251, true);
            Stage.setVisible(252, true);
            Stage.setVisible(253, true);
            Stage.setVisible(254, true);
            Stage.setVisible(255, true);
            Stage.setVisible(257, true);
            Stage.setVisible(258, true);
            Stage.setVisible(259, true);
            Stage.setVisible(260, true);
            Stage.setVisible(271, true);
            Stage.setVisible(272, true);
            Stage.setVisible(273, true);
            Stage.setVisible(274, true);
            Stage.setVisible(275, true);
            Stage.setVisible(276, true);
            Stage.setVisible(277, true);
            Stage.setVisible(278, true);
            Stage.setVisible(279, true);
            Stage.setVisible(280, true);
            Stage.setVisible(281, true);
            Stage.setVisible(282, true);
            Stage.setVisible(283, true);
            Stage.setVisible(284, true);
            Stage.setVisible(285, true);
            Stage.setVisible(286, true);
            Stage.setVisible(287, true);
            Stage.setVisible(288, true);
            Stage.setVisible(289, true);
            Stage.setVisible(290, true);
            Stage.setVisible(291, true);
        }

        void act23_map() {
            Stage.setVisible(212, false);
            Stage.setVisible(213, false);
            Stage.setVisible(214, false);
            Stage.setVisible(215, false);
            Stage.setVisible(217, false);
            Stage.setVisible(218, false);
            Stage.setVisible(219, false);
            Stage.setVisible(220, false);
            Stage.setVisible(221, false);
            Stage.setVisible(222, false);
            Stage.setVisible(223, false);
            Stage.setVisible(226, false);
            Stage.setVisible(227, false);
            Stage.setVisible(228, false);
            Stage.setVisible(231, false);
            Stage.setVisible(232, false);
            Stage.setVisible(233, false);
            Stage.setVisible(234, false);
            Stage.setVisible(235, false);
            Stage.setVisible(239, false);
            Stage.setVisible(240, false);
            Stage.setVisible(246, false);
            Stage.setVisible(247, false);
            Stage.setVisible(249, false);
            Stage.setVisible(250, false);
            Stage.setVisible(251, false);
            Stage.setVisible(255, false);
            Stage.setVisible(256, false);
            Stage.setVisible(258, false);
            Stage.setVisible(259, false);
            Stage.setVisible(260, false);
            int n = 268;
            while (n < 294) {
                Stage.setVisible(n, false);
                ++n;
            }
            Stage.setVisible(299, false);
            Stage.setVisible(303, false);
        }

        void act24_map() {
            Stage.setVisible(212, true);
            Stage.setVisible(213, true);
            Stage.setVisible(214, true);
            Stage.setVisible(215, true);
            Stage.setVisible(217, true);
            Stage.setVisible(218, true);
            Stage.setVisible(219, true);
            Stage.setVisible(220, true);
            Stage.setVisible(221, true);
            Stage.setVisible(222, true);
            Stage.setVisible(223, true);
            Stage.setVisible(226, true);
            Stage.setVisible(227, true);
            Stage.setVisible(228, true);
            Stage.setVisible(231, true);
            Stage.setVisible(232, true);
            Stage.setVisible(233, true);
            Stage.setVisible(234, true);
            Stage.setVisible(235, true);
            Stage.setVisible(239, true);
            Stage.setVisible(240, true);
            Stage.setVisible(246, true);
            Stage.setVisible(247, true);
            Stage.setVisible(249, true);
            Stage.setVisible(250, true);
            Stage.setVisible(251, true);
            Stage.setVisible(255, true);
            Stage.setVisible(256, true);
            Stage.setVisible(258, true);
            Stage.setVisible(259, true);
            Stage.setVisible(260, true);
            int n = 268;
            while (n < 294) {
                Stage.setVisible(n, true);
                ++n;
            }
            Stage.setVisible(299, true);
            Stage.setVisible(303, true);
        }

        void act26_impact1() {
            System.sleep(15);
            SCE02038.this.impact1.setTranslate(4.0f, 0.71f, 2.18f);
            SCE02038.this.impact1.setRotate(167.0f, 545.0f, 117.0f);
            SCE02038.this.impact1.setScale(0.1f, 0.1f, 0.1f);
            SCE02038.this.impact1.disp(true);
            SCE02038.this.impact1.setForceLoop(false);
        }

        void act26_spark1() {
            SCE02038.this.smoke1.setScale(2.15f, 2.15f, 2.15f);
            SCE02038.this.smoke1.setTranslate(3.9f, 0.04f, 2.0f);
            SCE02038.this.smoke1.setRotate(0.0f, 0.0f, 0.0f);
            SCE02038.this.smoke1.disp(true);
            SCE02038.this.smoke1.setForceLoop(false);
            SCE02038.this.spark1.setScale(0.25f, 0.25f, 0.25f);
            SCE02038.this.spark1.setTranslate(3.97f, 0.52f, 2.2f);
            SCE02038.this.spark1.setRotate(200.0f, -23.0f, 200.0f);
            SCE02038.this.spark1.disp(true);
            SCE02038.this.spark1.setForceLoop(true);
            SCE02038.this.spark1.noAttach(true);
        }

        void act2_moniL0() {
            System.sleep(40);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px + 0.09f - 0.06f, SCE02038.this.moniW0.py + 0.2f, SCE02038.this.moniW0.pz - 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL1.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL1.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL1, false);
            this.rotate(SCE02038.this.rotSPL1, true);
        }

        void act2_moniL1() {
            System.sleep(48);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px + 0.035f - 0.06f, SCE02038.this.moniW0.py, SCE02038.this.moniW0.pz - 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL1.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL1.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL1, false);
            this.rotate(SCE02038.this.rotSPL1, true);
        }

        void act2_moniL2() {
            System.sleep(56);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px - 0.02f - 0.06f, SCE02038.this.moniW0.py - 0.2f, SCE02038.this.moniW0.pz - 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL1.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL1.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL1, false);
            this.rotate(SCE02038.this.rotSPL1, true);
        }

        void act2_moniR0() {
            System.sleep(40);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px + 0.09f - 0.06f, SCE02038.this.moniW0.py + 0.2f, SCE02038.this.moniW0.pz + 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL, false);
            this.rotate(SCE02038.this.rotSPL, true);
        }

        void act2_moniR1() {
            System.sleep(48);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px + 0.035f - 0.06f, SCE02038.this.moniW0.py, SCE02038.this.moniW0.pz + 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL, false);
            this.rotate(SCE02038.this.rotSPL, true);
        }

        void act2_moniR2() {
            System.sleep(56);
            SCE02038.this.moniW0.getTranslate();
            this.setTranslate(SCE02038.this.moniW0.px - 0.02f - 0.06f, SCE02038.this.moniW0.py - 0.2f, SCE02038.this.moniW0.pz + 0.6f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = this.px + 0.06f;
            fArray[6] = this.py;
            fArray[7] = this.pz;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, false);
            SCE02038.this.movSPL.setCtrlVertex(fArray2, 1, 1, 5);
            SCE02038.this.rotSPL.setCtrlVertex(fArray4, 0, 1, 5);
            this.move(SCE02038.this.movSPL, false);
            this.rotate(SCE02038.this.rotSPL, true);
        }

        void act2_moniW0() {
            this.setTranslate(6.06f, 1.35f, 2.5f);
            this.setRotate(-90.0f, -75.0f, -90.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            System.sleep(15);
            this.signal(1);
            this.getTranslate();
            float[] fArray = new float[8];
            fArray[1] = this.px;
            fArray[2] = this.py;
            fArray[3] = this.pz;
            fArray[4] = 5.0f;
            fArray[5] = 6.0f;
            fArray[6] = 1.35f;
            fArray[7] = 2.5f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[4];
            fArray3[1] = this.rx;
            fArray3[2] = this.ry;
            fArray3[3] = this.rz;
            float[] fArray4 = fArray3;
            this.sclX(10, 0.8f, false);
            this.sclY(10, 0.8f, false);
            this.sclZ(10, 0.8f, false);
            SCE02038.this.movSPL.setCtrlVertex(fArray2, 1, 1, 10);
            SCE02038.this.rotSPL.setCtrlVertex(fArray4, 0, 1, 10);
            this.move(SCE02038.this.movSPL, false);
            this.rotate(SCE02038.this.rotSPL, true);
            this.setScale(1.0f, 1.0f, 1.0f);
        }

        void act2_space() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void act3_monitor1() {
            this.setTranslate(-0.95f, 2.4f, 6.8f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            System.sleep(45);
            this.sclX(5, 0.3f, false);
            this.sclY(5, 0.3f, false);
            this.sclZ(5, 0.3f, true);
        }

        void act3_monitor2() {
            this.setTranslate(SCE02038.this.moniZ0.px + 0.05f, SCE02038.this.moniZ0.py - 0.2f, SCE02038.this.moniZ0.pz - 0.4f);
            this.setRotate(0.0f, 90.0f, 0.0f);
            this.setScale(0.0f, 0.0f, 0.0f);
            this.signal(1);
            System.sleep(30);
            this.sclX(5, 1.0f, false);
            this.sclY(5, 1.0f, false);
            this.sclZ(5, 1.0f, true);
        }

        void act3_monitor_front() {
            SCE02038.this.moniY0.setTranslate(0.08f, 2.54f, 6.12f);
            SCE02038.this.moniY0.setRotate(-13.0f, 0.0f, 0.0f);
            SCE02038.this.moniY0.signal(1);
            SCE02038.this.moniY1.setTranslate(SCE02038.this.moniY0.px - 0.27f, SCE02038.this.moniY0.py + 0.27f, SCE02038.this.moniY0.pz + 0.05f);
            SCE02038.this.moniY1.setRotate(SCE02038.this.moniY0.rx, SCE02038.this.moniY0.ry, SCE02038.this.moniY0.rz);
            SCE02038.this.moniY1.signal(1);
        }

        void act3_monitor_side() {
            SCE02038.this.moniZ0.signal(1);
        }

        void act_monitor_all_off() {
            SCE02038.this.moniW0.signal(0);
            SCE02038.this.moniR0.signal(0);
            SCE02038.this.moniR1.signal(0);
            SCE02038.this.moniR2.signal(0);
            SCE02038.this.moniL0.signal(0);
            SCE02038.this.moniL1.signal(0);
            SCE02038.this.moniL2.signal(0);
        }

        void act_monitor_all_on() {
            SCE02038.this.moniW0.signal(1);
            SCE02038.this.moniR0.signal(1);
            SCE02038.this.moniR1.signal(1);
            SCE02038.this.moniR2.signal(1);
            SCE02038.this.moniL0.signal(1);
            SCE02038.this.moniL1.signal(1);
            SCE02038.this.moniL2.signal(1);
        }

        public void fog_check() {
            SCE02038.this.fog.setTranslate(0.0f, 0.0f, 0.0f);
            SCE02038.this.fog.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                SCE02038.this.cam1.setFog(1, SCE02038.this.fog.px, SCE02038.this.fog.pz, SCE02038.this.fog.py, SCE02038.this.fog.ry / 1000.0f, 128, 128, 128, 0);
                System.sleep(1);
            }
        }

        public void point_check0() {
            while (true) {
                SCE02038.this.light1.setGlobalPointLightCol(0, SCE02038.this.point0.rx / 100.0f, SCE02038.this.point0.ry / 100.0f, SCE02038.this.point0.rz / 100.0f);
                SCE02038.this.light1.setGlobalPointLightPos(0, SCE02038.this.point0.px, SCE02038.this.point0.py, SCE02038.this.point0.pz);
                System.sleep(1);
            }
        }

        public void point_check1() {
            while (true) {
                SCE02038.this.light1.setGlobalPointLightCol(1, SCE02038.this.point1.rx / 100.0f, SCE02038.this.point1.ry / 100.0f, SCE02038.this.point1.rz / 100.0f);
                SCE02038.this.light1.setGlobalPointLightPos(1, SCE02038.this.point1.px, SCE02038.this.point1.py, SCE02038.this.point1.pz);
                System.sleep(1);
            }
        }

        public void point_check2() {
            while (true) {
                SCE02038.this.light1.setGlobalPointLightCol(2, SCE02038.this.point2.rx / 100.0f, SCE02038.this.point2.ry / 100.0f, SCE02038.this.point2.rz / 100.0f);
                SCE02038.this.light1.setGlobalPointLightPos(2, SCE02038.this.point2.px, SCE02038.this.point2.py, SCE02038.this.point2.pz);
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

        void act12_door1() {
            while (true) {
                this.setTranslate(-1.05f, 0.0f, 0.0f);
                this.setRotate(0.0f, 0.0f, 0.0f);
                SCE02038.this.ASmove(SCE02038.this.door1, 12, -1.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                System.sleep(28);
            }
        }

        void act12_door2() {
            while (true) {
                this.setTranslate(1.05f, 0.0f, 0.0f);
                this.setRotate(0.0f, 0.0f, 0.0f);
                SCE02038.this.ASmove(SCE02038.this.door2, 12, 1.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                System.sleep(28);
            }
        }

        void act13_door1() {
            this.setTranslate(-1.75f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void act13_door2() {
            this.setTranslate(1.75f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void act17_map() {
            while (true) {
                Stage.setVisible(302, false);
                System.sleep(2);
                Stage.setVisible(302, true);
                System.sleep(1);
                Stage.setVisible(302, false);
                System.sleep(30);
                Stage.setVisible(302, true);
                System.sleep(10);
                Stage.setVisible(302, false);
                System.sleep(1);
                Stage.setVisible(302, true);
                System.sleep(1);
                Stage.setVisible(302, false);
                System.sleep(1);
                Stage.setVisible(302, true);
                System.sleep(2);
                Stage.setVisible(302, false);
                System.sleep(5);
                Stage.setVisible(302, true);
                System.sleep(1);
                Stage.setVisible(302, false);
                System.sleep(2);
            }
        }

        void act22_door1() {
            this.setTranslate(-1.75f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            System.sleep(35);
            SCE02038.this.ASmove(SCE02038.this.door1, 30, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        void act22_door2() {
            this.setTranslate(1.75f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            System.sleep(35);
            SCE02038.this.ASmove(SCE02038.this.door2, 30, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        void model3() {
            this.setRotate(-16.0f, 5.0f, 13.0f);
        }

        void model4() {
            this.setRotate(-16.0f, 5.0f, 13.0f);
            this.getTranslate();
            this.getRotate();
            while (true) {
                this.setRotate(this.rx - 0.1f, this.ry, this.rz);
                System.sleep(1);
            }
        }

        void model_reset() {
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void ring_L() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }

        void ring_reset() {
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void roll() {
            this.setRotate(36.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotate(this.rx + 0.001f, this.ry + 1.0E-4f, this.rz + 0.15f);
                System.sleep(1);
            }
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut0() {
            SCE02038.this.light.setColor(0, 0.25f, 0.25f, 0.25f);
            SCE02038.this.light.setColor(1, 0.6f, 0.6f, 0.6f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.626f, 0.78f);
            SCE02038.this.light.setColor(2, 0.5f, 0.5f, 0.5f);
            SCE02038.this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
            SCE02038.this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            SCE02038.this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(0.0f, -10.0f, 4.0f);
            SCE02038.this.cam1.setRotate(-80.0f, 0.0f, 0.0f);
            SCE02038.this.cam1.setFov(40.0f);
        }

        public void cut1() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.16f, 0.18f, 0.22f);
            SCE02038.this.light.setColor(1, 0.35f, 0.4f, 0.45f);
            SCE02038.this.light.setDirection2(1, -0.914f, 0.117f, 0.389f);
            SCE02038.this.light.setColor(2, 0.22f, 0.25f, 0.3f);
            SCE02038.this.light.setDirection2(2, 0.881f, 0.01f, -0.473f);
            SCE02038.this.light.setColor(3, 0.24f, 0.32f, 0.38f);
            SCE02038.this.light.setDirection2(3, 0.529f, -0.74f, -0.415f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(34.38f);
            float[] fArray = new float[]{1.0f, -7.76f, 6.18f, -19.77f, 135.0f, -8.45f, 6.18f, -18.51f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.05f;
            fArray2[2] = 214.41f;
            fArray2[4] = 135.0f;
            fArray2[5] = -10.05f;
            fArray2[6] = 211.77f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut10() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.13f, 0.15f, 0.19f);
            SCE02038.this.light.setColor(1, 0.42f, 0.47f, 0.52f);
            SCE02038.this.light.setDirection2(1, -0.255f, 0.338f, 0.906f);
            SCE02038.this.light.setColor(2, 0.21f, 0.24f, 0.29f);
            SCE02038.this.light.setDirection2(2, -0.017f, 0.51f, -0.86f);
            SCE02038.this.light.setColor(3, 0.13f, 0.21f, 0.27f);
            SCE02038.this.light.setDirection2(3, -0.627f, -0.72f, -0.297f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(1.24f, 1.21f, 1.71f);
            SCE02038.this.cam1.setRotate(-2.15f, 243.88f, 0.0f);
            SCE02038.this.cam1.setFov(23.4f);
        }

        public void cut11() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.14f, 0.16f, 0.2f);
            SCE02038.this.light.setColor(1, 0.45f, 0.5f, 0.55f);
            SCE02038.this.light.setDirection2(1, 0.14f, 0.302f, 0.943f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.028f, -0.963f);
            SCE02038.this.light.setColor(3, 0.09f, 0.18f, 0.24f);
            SCE02038.this.light.setDirection2(3, -0.391f, -0.776f, 0.495f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(2.29f, 1.1f, 2.93f);
            SCE02038.this.cam1.setRotate(2.14f, 331.3f, 0.0f);
            SCE02038.this.cam1.setFov(23.33f);
        }

        public void cut12() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, -0.0f, 0.346f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.0f, -0.963f);
            SCE02038.this.light.setColor(3, 0.19f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.226f, -0.0f, -0.974f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            Runtime.setDefocusQuick(0, 4, 393880, 32);
            Runtime.setDefocusQuick(1, 4, 385688, 21);
            Runtime.setDefocusQuick(2, 4, 377496, 21);
            Runtime.setDefocusQuick(3, 4, 369304, 21);
            SCE02038.this.cam1.setTranslate(3.36f, 0.94f, 1.05f);
            SCE02038.this.cam1.setRotate(-2.9f, 136.19f, 0.64f);
            SCE02038.this.cam1.setFov(33.25f);
            float[] fArray = new float[]{1.0f, 3.36f, 0.94f, 1.05f, 15.0f, 3.36f, 0.94f, 1.05f};
            float[] fArray2 = new float[]{1.0f, -2.9f, 136.19f, 0.64f, 10.0f, -2.9f, 136.19f, 0.64f, 30.0f, -1.92f, 149.75f, 0.64f, 40.0f, -1.95f, 149.77f, 0.64f};
            SCE02038.this.cam1.rotateSPL(fArray2, 1, 0, 30);
        }

        public void cut13() {
            SCE02038.this.Timechk_CutChange();
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setRotate(1.12f, 158.0f, 0.0f);
            SCE02038.this.cam1.setFov(34.85f);
            float[] fArray = new float[]{1.0f, 3.37f, 0.65f, 0.21f, 10.0f, 3.37f, 0.65f, 0.21f, 25.0f, 3.52f, 0.65f, -0.15f, 30.0f, 3.56f, 0.65f, -0.21f};
            SCE02038.this.cam1.transSPL(fArray, 0);
        }

        public void cut14() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.41f, 0.46f, 0.51f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, 0.474f, 0.621f, -0.625f);
            SCE02038.this.light.setColor(3, 0.16f, 0.26f, 0.32f);
            SCE02038.this.light.setDirection2(3, -0.597f, -0.31f, 0.74f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            Runtime.setDefocusQuick(0, 4, 393880, 32);
            Runtime.setDefocusQuick(1, 4, 385688, 21);
            Runtime.setDefocusQuick(2, 4, 377496, 21);
            Runtime.setDefocusQuick(3, 4, 369304, 21);
            SCE02038.this.cam1.setTranslate(1.94f, 0.52f, 4.48f);
            SCE02038.this.cam1.setRotate(-4.55f, 37.52f, 0.0f);
            SCE02038.this.cam1.setFov(27.52f);
            float[] fArray = new float[]{1.0f, SCE02038.this.cam1.getTranslateX(), SCE02038.this.cam1.getTranslateY(), SCE02038.this.cam1.getTranslateZ(), 20.0f, SCE02038.this.cam1.getTranslateX() - 0.08f, SCE02038.this.cam1.getTranslateY(), SCE02038.this.cam1.getTranslateZ() - 0.09f};
            float[] fArray2 = new float[8];
            fArray2[1] = SCE02038.this.cam1.getRotateX();
            fArray2[2] = SCE02038.this.cam1.getRotateY();
            fArray2[3] = SCE02038.this.cam1.getRotateZ();
            fArray2[4] = 20.0f;
            fArray2[5] = SCE02038.this.cam1.getRotateX() + 4.3f;
            fArray2[6] = SCE02038.this.cam1.getRotateY() + 18.95f;
            fArray2[7] = SCE02038.this.cam1.getRotateZ();
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 0);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut14_1() {
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
        }

        public void cut15() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, -0.0f, 0.346f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.0f, -0.963f);
            SCE02038.this.light.setColor(3, 0.19f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.226f, -0.0f, -0.974f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(24.64f);
            float[] fArray = new float[]{1.0f, -4.11f, 0.9f, 5.22f, 16.0f, -3.5f, 0.9f, 5.68f, 31.0f, -2.77f, 0.9f, 6.24f, 46.0f, -2.12f, 0.9f, 6.72f, 61.0f, -2.03f, 0.9f, 6.79f, 76.0f, -2.03f, 0.9f, 6.79f, 95.0f, -2.03f, 0.9f, 6.79f};
            float[] fArray2 = new float[]{1.0f, 1.5f, 233.02f, 4.48f, 16.0f, 1.5f, 233.02f, 4.48f, 31.0f, 1.5f, 233.02f, 4.48f, 46.0f, 1.5f, 233.48f, 4.48f, 61.0f, 1.5f, 234.48f, 4.48f, 76.0f, 1.5f, 234.53f, 4.48f, 95.0f, 1.5f, 234.53f, 4.48f};
            SCE02038.this.cam1.transSPL(fArray, 0, 0, 75);
            SCE02038.this.cam1.rotateSPL(fArray2, 1, 0, 75);
        }

        public void cut16() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.16f, 0.18f, 0.22f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.425f, 0.389f, 0.817f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.978f, 0.0f, -0.209f);
            SCE02038.this.light.setColor(3, 0.16f, 0.24f, 0.3f);
            SCE02038.this.light.setDirection2(3, -0.392f, -0.759f, 0.52f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(-1.31f, 0.05f, 4.29f);
            SCE02038.this.cam1.setRotate(11.64f, 328.87f, 0.0f);
            SCE02038.this.cam1.setFov(21.75f);
            float[] fArray = new float[]{1.0f, -1.31f, 0.05f, 4.29f, 16.0f, -1.31f, 0.05f, 4.29f, 31.0f, -1.31f, 0.05f, 4.29f, 46.0f, -1.31f, 0.05f, 4.29f, 61.0f, -1.31f, 0.05f, 4.29f, 84.0f, -1.31f, 0.05f, 4.29f};
            float[] fArray2 = new float[24];
            fArray2[0] = 1.0f;
            fArray2[1] = 11.64f;
            fArray2[2] = 328.87f;
            fArray2[4] = 16.0f;
            fArray2[5] = 15.74f;
            fArray2[6] = 328.87f;
            fArray2[8] = 31.0f;
            fArray2[9] = 19.88f;
            fArray2[10] = 328.87f;
            fArray2[12] = 46.0f;
            fArray2[13] = 21.82f;
            fArray2[14] = 328.87f;
            fArray2[16] = 61.0f;
            fArray2[17] = 21.9f;
            fArray2[18] = 328.87f;
            fArray2[20] = 84.0f;
            fArray2[21] = 21.9f;
            fArray2[22] = 328.87f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut16_1() {
            float[] fArray = new float[20];
            fArray[1] = SCE02038.this.cam1.getRotateX();
            fArray[2] = SCE02038.this.cam1.getRotateY();
            fArray[3] = SCE02038.this.cam1.getRotateZ();
            fArray[4] = 16.0f;
            fArray[5] = SCE02038.this.cam1.getRotateX();
            fArray[6] = SCE02038.this.cam1.getRotateY() - 9.96f;
            fArray[7] = SCE02038.this.cam1.getRotateZ();
            fArray[8] = 31.0f;
            fArray[9] = SCE02038.this.cam1.getRotateX();
            fArray[10] = SCE02038.this.cam1.getRotateY() - 11.89f;
            fArray[11] = SCE02038.this.cam1.getRotateZ();
            fArray[12] = 46.0f;
            fArray[13] = SCE02038.this.cam1.getRotateX();
            fArray[14] = SCE02038.this.cam1.getRotateY() - 11.96f;
            fArray[15] = SCE02038.this.cam1.getRotateZ();
            fArray[16] = 68.0f;
            fArray[17] = SCE02038.this.cam1.getRotateX();
            fArray[18] = SCE02038.this.cam1.getRotateY() - 11.96f;
            fArray[19] = SCE02038.this.cam1.getRotateZ();
            float[] fArray2 = fArray;
            SCE02038.this.cam1.rotateSPL(fArray2, 1, 2, 90);
        }

        public void cut17() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.14f, 0.16f, 0.2f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.17f, 0.2f, 0.25f);
            SCE02038.this.light.setDirection2(2, 0.8f, 0.0f, -0.6f);
            SCE02038.this.light.setColor(3, 0.15f, 0.27f, 0.33f);
            SCE02038.this.light.setDirection2(3, -0.481f, -0.647f, 0.592f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(4.89f, 0.2f, 4.61f);
            SCE02038.this.cam1.setRotate(5.6f, 26.0f, 0.0f);
            SCE02038.this.cam1.setFov(21.8f);
            float[] fArray = new float[]{1.0f, 4.89f, 0.2f, 4.61f, 60.0f, 4.86f, 0.24f, 4.63f, 90.0f, 4.86f, 0.24f, 4.63f, 245.0f, 4.86f, 0.24f, 4.63f};
            float[] fArray2 = new float[40];
            fArray2[0] = 1.0f;
            fArray2[1] = 5.6f;
            fArray2[2] = 26.0f;
            fArray2[4] = 60.0f;
            fArray2[5] = 5.6f;
            fArray2[6] = 26.0f;
            fArray2[8] = 90.0f;
            fArray2[9] = 5.6f;
            fArray2[10] = 26.0f;
            fArray2[12] = 105.0f;
            fArray2[13] = 6.5f;
            fArray2[14] = 26.5f;
            fArray2[16] = 140.0f;
            fArray2[17] = 7.41f;
            fArray2[18] = 27.0f;
            fArray2[20] = 160.0f;
            fArray2[21] = 8.4f;
            fArray2[22] = 30.17f;
            fArray2[24] = 180.0f;
            fArray2[25] = 8.4f;
            fArray2[26] = 31.83f;
            fArray2[28] = 200.0f;
            fArray2[29] = 8.4f;
            fArray2[30] = 31.9f;
            fArray2[32] = 245.0f;
            fArray2[33] = 8.5f;
            fArray2[34] = 31.9f;
            fArray2[36] = 260.0f;
            fArray2[37] = 8.5f;
            fArray2[38] = 31.9f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut18() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.14f, 0.16f, 0.2f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.2f, 0.23f, 0.28f);
            SCE02038.this.light.setDirection2(2, 0.796f, 0.334f, -0.505f);
            SCE02038.this.light.setColor(3, 0.19f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, 0.458f, -0.671f, 0.583f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(1.37f, 1.38f, 3.07f);
            SCE02038.this.cam1.setRotate(-7.07f, 423.67f, 0.0f);
            SCE02038.this.cam1.setFov(23.4f);
        }

        public void cut19() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.14f, 0.16f, 0.2f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.332f, 0.478f, 0.813f);
            SCE02038.this.light.setColor(2, 0.18f, 0.21f, 0.26f);
            SCE02038.this.light.setDirection2(2, 0.095f, 0.719f, -0.688f);
            SCE02038.this.light.setColor(3, 0.21f, 0.3f, 0.36f);
            SCE02038.this.light.setDirection2(3, 0.141f, -0.732f, 0.667f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(4.82f, 1.48f, 4.13f);
            SCE02038.this.cam1.setRotate(-8.45f, 424.26f, 0.0f);
            SCE02038.this.cam1.setFov(23.72f);
        }

        public void cut2() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.16f, 0.18f, 0.22f);
            SCE02038.this.light.setColor(1, 0.35f, 0.4f, 0.45f);
            SCE02038.this.light.setDirection2(1, -0.23f, 0.137f, 0.963f);
            SCE02038.this.light.setColor(2, 0.22f, 0.25f, 0.3f);
            SCE02038.this.light.setDirection2(2, 0.907f, 0.418f, -0.054f);
            SCE02038.this.light.setColor(3, 0.24f, 0.32f, 0.38f);
            SCE02038.this.light.setDirection2(3, 0.741f, -0.623f, -0.249f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            Runtime.setDefocusQuick(0, 1, 35880, 1);
            Runtime.setDefocusQuick(1, 1, 27688, 1);
            Runtime.setDefocusQuick(2, 1, 19496, 1);
            SCE02038.this.cam1.setTranslate(6.18f, 1.03f, 4.23f);
            SCE02038.this.cam1.setRotate(3.9f, 382.29f, 0.0f);
            SCE02038.this.cam1.setFov(31.17f);
        }

        public void cut20() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.17f, 0.19f, 0.23f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.028f, -0.963f);
            SCE02038.this.light.setColor(3, 0.18f, 0.26f, 0.32f);
            SCE02038.this.light.setDirection2(3, 0.432f, -0.465f, 0.772f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(1.11f, 1.35f, 3.13f);
            SCE02038.this.cam1.setRotate(-6.19f, 421.79f, 0.0f);
            SCE02038.this.cam1.setFov(23.4f);
        }

        public void cut21() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.14f, 0.16f, 0.2f);
            SCE02038.this.light.setColor(1, 0.45f, 0.5f, 0.55f);
            SCE02038.this.light.setDirection2(1, -0.197f, 0.639f, 0.744f);
            SCE02038.this.light.setColor(2, 0.16f, 0.19f, 0.24f);
            SCE02038.this.light.setDirection2(2, 0.977f, 0.0f, 0.215f);
            SCE02038.this.light.setColor(3, 0.1f, 0.18f, 0.24f);
            SCE02038.this.light.setDirection2(3, -0.671f, -0.565f, 0.48f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(-0.33f, 1.16f, 11.23f);
            SCE02038.this.cam1.setRotate(-0.72f, -18.12f, 0.0f);
            SCE02038.this.cam1.setFov(40.0f);
        }

        public void cut22() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.582f, 0.508f, 0.635f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.775f, 0.001f, -0.631f);
            SCE02038.this.light.setColor(3, 0.24f, 0.35f, 0.41f);
            SCE02038.this.light.setDirection2(3, 0.565f, -0.741f, -0.364f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(4.46f, 1.29f, 0.32f);
            SCE02038.this.cam1.setRotate(2.7f, -206.22f, 0.0f);
            SCE02038.this.cam1.setFov(31.04f);
        }

        public void cut23() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, -0.415f, 0.239f, 0.878f);
            SCE02038.this.light.setColor(2, 0.17f, 0.2f, 0.25f);
            SCE02038.this.light.setDirection2(2, 0.595f, 0.456f, -0.662f);
            SCE02038.this.light.setColor(3, 0.13f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.627f, -0.778f, -0.019f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setRotate(10.4f, 19.74f, 0.0f);
            SCE02038.this.cam1.setFov(35.85f);
            float[] fArray = new float[]{1.0f, 5.69f, 1.17f, 10.08f, 495.0f, 5.3f, 1.17f, 6.16f};
            SCE02038.this.cam1.transSPL(fArray, 1, 3, 495);
        }

        public void cut24() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, -0.403f, 0.225f, 0.887f);
            SCE02038.this.light.setColor(2, 0.22f, 0.25f, 0.3f);
            SCE02038.this.light.setDirection2(2, 0.573f, 0.001f, -0.82f);
            SCE02038.this.light.setColor(3, 0.13f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.688f, -0.697f, -0.2f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(3.09f, 0.77f, 2.93f);
            SCE02038.this.cam1.setRotate(9.08f, -139.9f, 0.0f);
            SCE02038.this.cam1.setFov(28.8f);
        }

        public void cut25() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02038.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02038.this.light.setDirection2(1, -0.551f, 0.386f, -0.74f);
            SCE02038.this.light.setColor(2, 0.36f, 0.36f, 0.36f);
            SCE02038.this.light.setDirection2(2, -0.529f, 0.641f, 0.556f);
            SCE02038.this.light.setColor(3, 0.41f, 0.41f, 0.41f);
            SCE02038.this.light.setDirection2(3, -0.07f, -0.65f, -0.757f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(31.5f);
            float[] fArray = new float[]{1.0f, -1.3f, 2.11f, 5.78f, 210.0f, -1.3f, 2.11f, 5.78f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 10.7f;
            fArray2[2] = 587.5f;
            fArray2[4] = 210.0f;
            fArray2[5] = 10.7f;
            fArray2[6] = 587.5f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut26() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, -0.0f, 0.346f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.0f, -0.963f);
            SCE02038.this.light.setColor(3, 0.19f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.226f, -0.0f, -0.974f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 137880, 2);
            Runtime.setDefocusQuick(1, 0, 80000, 1);
            Runtime.setDefocusQuick(2, 0, 79000, 1);
            Runtime.setDefocusQuick(3, 0, 78000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setRotate(-25.34f, -124.5f, 0.0f);
            SCE02038.this.cam1.setFov(28.8f);
            float[] fArray = new float[]{1.0f, 4.72f, 1.51f, 5.68f, 100.0f, 4.41f, 1.51f, 5.62f, 130.0f, 4.32f, 1.51f, 5.59f, 160.0f, 4.3f, 1.51f, 5.58f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -12.98f;
            fArray2[2] = -10.95f;
            fArray2[4] = 100.0f;
            fArray2[5] = -12.42f;
            fArray2[6] = -15.97f;
            fArray2[8] = 130.0f;
            fArray2[9] = -12.42f;
            fArray2[10] = -18.35f;
            fArray2[12] = 160.0f;
            fArray2[13] = -12.42f;
            fArray2[14] = -18.9f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut3() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            SCE02038.this.light.setColor(1, 0.5f, 0.5f, 0.5f);
            SCE02038.this.light.setDirection2(1, -0.551f, 0.386f, -0.74f);
            SCE02038.this.light.setColor(2, 0.36f, 0.36f, 0.36f);
            SCE02038.this.light.setDirection2(2, -0.529f, 0.641f, 0.556f);
            SCE02038.this.light.setColor(3, 0.41f, 0.41f, 0.41f);
            SCE02038.this.light.setDirection2(3, -0.07f, -0.65f, -0.757f);
            Stage.setColor(1.0f, 1.0f, 1.0f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(31.5f);
            float[] fArray = new float[]{1.0f, -1.52f, 1.98f, 6.03f, 129.5f, -1.37f, 1.98f, 5.85f, 180.0f, -1.3f, 1.98f, 5.78f, 210.0f, -1.3f, 1.98f, 5.78f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = 13.5f;
            fArray2[2] = 230.9f;
            fArray2[4] = 129.5f;
            fArray2[5] = 13.5f;
            fArray2[6] = 227.7f;
            fArray2[8] = 180.0f;
            fArray2[9] = 13.5f;
            fArray2[10] = 227.5f;
            fArray2[12] = 210.0f;
            fArray2[13] = 13.5f;
            fArray2[14] = 227.5f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut4() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.09f, 0.11f, 0.15f);
            SCE02038.this.light.setColor(1, 0.35f, 0.4f, 0.45f);
            SCE02038.this.light.setDirection2(1, -0.304f, 0.168f, 0.938f);
            SCE02038.this.light.setColor(2, 0.41f, 0.44f, 0.49f);
            SCE02038.this.light.setDirection2(2, 0.919f, 0.384f, -0.093f);
            SCE02038.this.light.setColor(3, 0.28f, 0.32f, 0.38f);
            SCE02038.this.light.setDirection2(3, 0.556f, -0.732f, -0.395f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(5.94f, 1.15f, 3.19f);
            SCE02038.this.cam1.setRotate(-2.86f, 765.62f, 0.0f);
            SCE02038.this.cam1.setFov(31.5f);
        }

        public void cut4_1() {
            SCE02038.this.cam1.setFov(35.2f);
            float[] fArray = new float[]{1.0f, 1.87f, 1.44f, 12.95f, 10.0f, 1.87f, 1.44f, 12.95f, 20.0f, 1.87f, 1.44f, 12.95f, 30.0f, 1.87f, 1.44f, 12.95f, 40.0f, 1.87f, 1.44f, 12.93f, 50.0f, 1.88f, 1.44f, 12.9f, 255.0f, 1.9f, 1.44f, 12.05f, 285.0f, 1.9f, 1.44f, 12.0f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = SCE02038.this.cam1.getRotateX();
            fArray2[2] = SCE02038.this.cam1.getRotateY();
            fArray2[3] = SCE02038.this.cam1.getRotateZ();
            fArray2[4] = 255.0f;
            fArray2[5] = 1.47f;
            fArray2[6] = 489.0f;
            fArray2[8] = 285.0f;
            fArray2[9] = 1.47f;
            fArray2[10] = 489.0f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut5() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.028f, -0.963f);
            SCE02038.this.light.setColor(3, 0.11f, 0.34f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.227f, -0.429f, -0.874f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(2.2f, 0.42f, 4.25f);
            SCE02038.this.cam1.setRotate(5.24f, 306.63f, 0.0f);
            SCE02038.this.cam1.setFov(30.55f);
        }

        public void cut6() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.12f, 0.15f, 0.2f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.028f, -0.963f);
            SCE02038.this.light.setColor(3, 0.11f, 0.34f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.227f, -0.429f, -0.874f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(24.78f);
            float[] fArray = new float[]{1.0f, 2.28f, 0.81f, 3.7f, 100.0f, 2.6f, 0.81f, 3.6f, 145.0f, 2.7f, 0.81f, 3.55f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.92f;
            fArray2[2] = 668.05f;
            fArray2[4] = 100.0f;
            fArray2[5] = -8.0f;
            fArray2[6] = 668.05f;
            fArray2[8] = 130.0f;
            fArray2[9] = -7.98f;
            fArray2[10] = 668.05f;
            fArray2[12] = 145.0f;
            fArray2[13] = -7.95f;
            fArray2[14] = 668.05f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1, 3, 145);
            SCE02038.this.cam1.rotateSPL(fArray3, 1, 3, 145);
        }

        public void cut7() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.2f, 0.22f, 0.26f);
            SCE02038.this.light.setColor(1, 0.44f, 0.49f, 0.54f);
            SCE02038.this.light.setDirection2(1, 0.0f, 0.347f, 0.938f);
            SCE02038.this.light.setColor(2, 0.17f, 0.2f, 0.25f);
            SCE02038.this.light.setDirection2(2, -0.268f, 0.028f, -0.963f);
            SCE02038.this.light.setColor(3, 0.14f, 0.22f, 0.28f);
            SCE02038.this.light.setDirection2(3, -0.441f, -0.74f, -0.508f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(1.66f, 0.95f, 0.81f);
            SCE02038.this.cam1.setRotate(-6.33f, 225.29f, 0.0f);
            SCE02038.this.cam1.setFov(25.1f);
        }

        public void cut7_1() {
            float[] fArray = new float[8];
            fArray[1] = SCE02038.this.cam1.getRotateX();
            fArray[2] = SCE02038.this.cam1.getRotateY();
            fArray[3] = SCE02038.this.cam1.getRotateZ();
            fArray[4] = 135.0f;
            fArray[5] = 2.7f;
            fArray[6] = 231.9f;
            float[] fArray2 = fArray;
            SCE02038.this.cam1.rotateSPL(fArray2, 1, 3, 120);
        }

        public void cut8() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.13f, 0.15f, 0.19f);
            SCE02038.this.light.setColor(1, 0.35f, 0.4f, 0.45f);
            SCE02038.this.light.setDirection2(1, -0.392f, 0.244f, 0.887f);
            SCE02038.this.light.setColor(2, 0.17f, 0.2f, 0.25f);
            SCE02038.this.light.setDirection2(2, 0.942f, 0.028f, 0.334f);
            SCE02038.this.light.setColor(3, 0.13f, 0.29f, 0.35f);
            SCE02038.this.light.setDirection2(3, -0.785f, -0.607f, 0.127f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setFov(23.4f);
            float[] fArray = new float[]{1.0f, 2.73f, 1.31f, 3.21f, 30.0f, 2.73f, 1.31f, 3.21f, 150.0f, 2.75f, 1.31f, 2.92f, 180.0f, 2.75f, 1.31f, 2.92f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.61f;
            fArray2[2] = 306.46f;
            fArray2[4] = 30.0f;
            fArray2[5] = -7.61f;
            fArray2[6] = 306.46f;
            fArray2[8] = 150.0f;
            fArray2[9] = -7.61f;
            fArray2[10] = 306.46f;
            fArray2[12] = 180.0f;
            fArray2[13] = -7.61f;
            fArray2[14] = 306.46f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.transSPL(fArray, 1);
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut8_1() {
            float[] fArray = new float[8];
            fArray[0] = 1.0f;
            fArray[1] = SCE02038.this.cam1.getRotateX();
            fArray[2] = SCE02038.this.cam1.getRotateY();
            fArray[3] = SCE02038.this.cam1.getRotateZ();
            fArray[4] = 60.0f;
            fArray[5] = -7.61f;
            fArray[6] = 326.27f;
            float[] fArray2 = fArray;
            SCE02038.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut9() {
            SCE02038.this.Timechk_CutChange();
            SCE02038.this.light.setColor(0, 0.15f, 0.17f, 0.21f);
            SCE02038.this.light.setColor(1, 0.45f, 0.5f, 0.55f);
            SCE02038.this.light.setDirection2(1, 0.408f, 0.414f, 0.813f);
            SCE02038.this.light.setColor(2, 0.15f, 0.18f, 0.23f);
            SCE02038.this.light.setDirection2(2, -0.775f, 0.592f, 0.222f);
            SCE02038.this.light.setColor(3, 0.16f, 0.24f, 0.3f);
            SCE02038.this.light.setDirection2(3, -0.606f, -0.792f, 0.072f);
            Stage.setColor(0.65f, 0.65f, 0.65f);
            Runtime.setDefocusQuick(0, 0, 0x100000, 2);
            Runtime.setDefocusQuick(1, 0, 0x100000, 1);
            Runtime.setDefocusQuick(2, 0, 0x100000, 1);
            Runtime.setDefocusQuick(3, 0, 0x100000, 1);
            Runtime.setDefocus(0, 0, null);
            Runtime.setDefocus(15, 0, null);
            SCE02038.this.cam1.setTranslate(1.47f, 0.89f, 2.51f);
            SCE02038.this.cam1.setFov(23.4f);
            float[] fArray = new float[]{1.0f, SCE02038.this.cam1.getTranslateX() - 3.92f, SCE02038.this.cam1.getTranslateY() + 0.89f, SCE02038.this.cam1.getTranslateZ() + 0.43f, 100.0f, SCE02038.this.cam1.getTranslateX() - 3.92f, SCE02038.this.cam1.getTranslateY() + 0.89f, SCE02038.this.cam1.getTranslateZ() + 0.43f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.66f;
            fArray2[2] = 290.21f;
            fArray2[4] = 30.0f;
            fArray2[5] = 6.66f;
            fArray2[6] = 290.21f;
            fArray2[8] = 130.0f;
            fArray2[9] = 14.74f;
            fArray2[10] = 290.21f;
            float[] fArray3 = fArray2;
            SCE02038.this.cam1.rotateSPL(fArray3, 1);
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

