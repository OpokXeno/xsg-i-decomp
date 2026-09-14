import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_VOK11_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Vector4f;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01015
        extends Scene
        implements Xbufnum,
        XenoConstants,
        JNT_Human,
        EventConstants,
        MC_VOK11_PRJ,
        Pack01015 {
    public Shion shion;
    public Shion2 shion2;
    public Andrew andrew;
    public Andrew2 andrew2;
    public Moriyama moriyama;
    public Moriyama2 moriyama2;
    public M m;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int menuSelected;
    int count;
    int i = 0;
    int Chand_R = 20;
    Light light = new Light(0);
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    int Rhand = 16;
    int Lhand = 32;
    int Open = 0;
    int Close = 1;
    Chr ob_l;
    Chr ob_r;
    Chr op_l1;
    Chr op_l2;
    Chr op_l3;
    Chr op_r1;
    Chr op_r2;
    Chr op_r3;
    Chr op_upr1;
    Chr op_upr2;
    Unit army_1;
    Unit army_star;
    Unit mobj_133;
    Unit army_phone;
    Unit fm1;
    Unit fm2;
    Unit fm3;
    Unit fm4;
    Unit fm5;
    Unit fm6;
    Unit fm7;
    Unit fm8;
    Unit fm9;
    Unit fm10;
    Unit fm11;
    Unit fm12;
    Unit fm_cap1;
    Unit fm_cap2;
    Unit fm_momo;
    Unit sphere1;
    Unit sphere2;
    Unit ring_1a;
    Unit ring_1b;
    Unit ring_1c;
    Unit ring_2a;
    Unit ring_2b;
    Unit ring_2c;
    Unit sonar_ring_0;
    Unit sonar_ring_1;
    Unit sonar_ring_2;
    Unit sonar_ring_3;
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

    SCE01015() {
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
        float[] fArray = new float[]{1.0f, -1.45f, 1.26f, -4.46f, 100.0f, -1.72f, 1.26f, -5.1f};
        float[] fArray2 = new float[]{1.0f, -22.06f, 920.13f, 0.16f, 100.0f, -23.08f, 915.45f, 0.16f};
        this.cam1.transSPL(fArray, 1, 1, 250);
        this.cam1.rotateSPL(fArray2, 1, 1, 250);
        this.cam1.setFov(33.6f);
        this.waitCameraPlay(2);
        this.cam1.change();
        float[] fArray3 = new float[]{1.0f, -1.27f, -0.5f, 0.7f, 90.0f, -1.27f, -0.5f, 0.7f};
        float[] fArray4 = new float[]{1.0f, -0.92f, 212.02f, 0.16f, 90.0f, -3.34f, 208.7f, 0.16f};
        this.cam1.transSPL(fArray3, 1, 1, 150);
        this.cam1.rotateSPL(fArray4, 1, 1, 150);
        this.cam1.setFov(33.8f);
        this.waitCameraPlay(3);
        this.cam1.change();
        float[] fArray5 = new float[]{1.0f, -0.9f, -0.59f, 1.5f, 100.0f, -0.73f, -0.59f, 1.71f};
        this.cam1.transSPL(fArray5, 1, 1, 40);
        this.cam1.setRotate(-1.56f, -138.38f, 0.16f);
        this.cam1.setFov(26.4f);
        this.waitCameraPlay(4);
        this.cam1.change();
        this.cam1.setTranslate(0.3f, -0.58f, 2.53f);
        this.cam1.setRotate(1.18f, 430.13f, 0.16f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(5);
        this.cam1.change();
        this.cam1.setTranslate(-1.06f, -1.12f, 0.43f);
        this.cam1.setRotate(6.12f, 563.11f, 0.16f);
        this.cam1.setFov(33.6f);
        this.waitCameraPlay(105);
        this.cam1.change();
        float[] fArray6 = new float[]{1.0f, 6.12f, 563.11f, 0.16f, 100.0f, 7.62f, 567.31f, 0.16f};
        this.cam1.setTranslate(-1.06f, -1.12f, 0.43f);
        this.cam1.rotateSPL(fArray6, 1, 3, 100);
        this.cam1.setFov(33.6f);
        this.waitCameraPlay(6);
        this.cam1.change();
        float[] fArray7 = new float[]{1.0f, -4.11f, 3.14f, -6.07f, 100.0f, -4.11f, 3.14f, -6.07f};
        float[] fArray8 = new float[]{1.0f, -31.7f, 579.69f, 0.16f, 100.0f, -34.2f, 581.48f, 0.16f};
        this.cam1.transSPL(fArray7, 1, 1, 500);
        this.cam1.rotateSPL(fArray8, 1, 1, 500);
        this.cam1.setFov(36.48f);
        this.waitCameraPlay(7);
        this.cam1.change();
        this.cam1.setTranslate(-0.27f, -0.82f, -1.4f);
        this.cam1.setRotate(-47.55f, 890.97f, 0.16f);
        this.cam1.setFov(42.0f);
        this.waitCameraPlay(8);
        this.cam1.change();
        this.cam1.setTranslate(0.38f, -0.43f, 1.91f);
        this.cam1.setRotate(-11.51f, 1255.99f, 0.16f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(9);
        this.cam1.change();
        this.cam1.setTranslate(0.28f, -0.49f, 1.11f);
        this.cam1.setRotate(0.17f, 1131.29f, 0.16f);
        this.cam1.setFov(28.8f);
        this.waitCameraPlay(109);
        this.cam1.change();
        float[] fArray9 = new float[]{1.0f, 0.17f, 1131.29f, 0.16f, 100.0f, 1.59f, 1141.73f, 0.16f};
        this.cam1.setTranslate(0.28f, -0.49f, 1.11f);
        this.cam1.rotateSPL(fArray9, 0, 1, 120);
        this.cam1.setFov(28.8f);
        this.waitCameraPlay(10);
        this.cam1.change();
        this.cam1.setTranslate(0.27f, -0.64f, 1.42f);
        this.cam1.setRotate(3.55f, 169.31f, 0.16f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(11);
        this.cam1.change();
        this.cam1.setTranslate(1.04f, -0.64f, 3.4f);
        this.cam1.setRotate(3.55f, 46.05f, 0.16f);
        this.cam1.setFov(36.0f);
        this.waitCameraPlay(12);
        this.cam1.change();
        float[] fArray10 = new float[]{1.0f, -1.1f, 0.63f, 0.8f, 100.0f, -1.1f, 0.21f, 0.8f};
        float[] fArray11 = new float[8];
        fArray11[0] = 1.0f;
        fArray11[1] = -25.38f;
        fArray11[2] = -135.46f;
        fArray11[4] = 100.0f;
        fArray11[5] = -25.14f;
        fArray11[6] = -125.8f;
        float[] fArray12 = fArray11;
        this.cam1.transSPL(fArray10, 1, 3, 200);
        this.cam1.rotateSPL(fArray12, 1, 3, 200);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(13);
        this.cam1.change();
        this.cam1.setTranslate(-0.13f, -0.44f, 0.73f);
        this.cam1.setRotate(-2.24f, -137.97f, 0.0f);
        this.cam1.setFov(27.2f);
        this.waitCameraPlay(14);
        this.cam1.change();
        this.cam1.setTranslate(0.66f, -0.57f, 1.42f);
        this.cam1.setRotate(1.42f, 138.57f, 0.0f);
        this.cam1.setFov(31.4f);
        this.waitCameraPlay(16);
        this.cam1.change();
        this.cam1.setTranslate(0.91f, -0.88f, 0.18f);
        this.cam1.setRotate(0.24f, 1246.56f, 0.0f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(116);
        this.cam1.change();
        float[] fArray13 = new float[8];
        fArray13[0] = 1.0f;
        fArray13[1] = 0.24f;
        fArray13[2] = 1246.56f;
        fArray13[4] = 100.0f;
        fArray13[5] = 0.24f;
        fArray13[6] = 1236.04f;
        float[] fArray14 = fArray13;
        this.cam1.setTranslate(0.91f, -0.88f, 0.18f);
        this.cam1.rotateSPL(fArray14, 1, 3, 90);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(17);
        this.cam1.change();
        float[] fArray15 = new float[]{1.0f, -2.65f, -1.82f, 5.12f, 100.0f, -2.33f, -1.82f, 4.9f};
        this.cam1.transSPL(fArray15, 1, 1, 60);
        this.cam1.setRotate(17.32f, 664.81f, 0.0f);
        this.cam1.setFov(40.0f);
        this.waitCameraPlay(18);
        this.cam1.change();
        this.cam1.setTranslate(0.18f, -0.5f, 0.76f);
        this.cam1.setRotate(-3.94f, 561.53f, 0.0f);
        this.cam1.setFov(35.6f);
        this.waitCameraPlay(19);
        this.cam1.change();
        this.cam1.setTranslate(0.29f, -0.66f, 2.03f);
        this.cam1.setRotate(1.64f, 547.88f, 0.0f);
        this.cam1.setFov(30.4f);
        this.waitCameraPlay(20);
        this.cam1.change();
        this.cam1.setTranslate(0.29f, -0.53f, 3.05f);
        this.cam1.setRotate(2.42f, 688.4f, 0.0f);
        this.cam1.setFov(31.6f);
        this.waitCameraPlay(21);
        this.cam1.change();
        this.cam1.setTranslate(-5.12f, 3.11f, 8.91f);
        this.cam1.setRotate(-25.75f, 689.37f, 0.0f);
        this.cam1.setFov(41.6f);
        this.waitCameraPlay(121);
        this.cam1.change();
        float[] fArray16 = new float[]{1.0f, -5.12f, 3.11f, 8.91f, 100.0f, -5.12f, 3.11f, 8.91f};
        float[] fArray17 = new float[8];
        fArray17[0] = 1.0f;
        fArray17[1] = -25.75f;
        fArray17[2] = 689.37f;
        fArray17[4] = 100.0f;
        fArray17[5] = -23.85f;
        fArray17[6] = 690.33f;
        float[] fArray18 = fArray17;
        this.cam1.transSPL(fArray16, 1, 3, 300);
        this.cam1.rotateSPL(fArray18, 1, 3, 300);
        this.cam1.setFov(41.6f);
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
        System.println("XEVEFLAG:EV01015_F");
        Runtime.setFlags(23, 1, 1);
        System.println("XEVEJNAME:CFJ1_125 XEVEJPOINT:POINT_125");
        Runtime.jumpCF(110, 2);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpCF(110, 2);
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
        this.cam1.setTranslate(-1.84f, 1.26f, -5.53f);
        this.cam1.setRotate(-23.18f, 915.43f, 0.16f);
        this.cam1.setFov(40.0f);
        Runtime.setLocation(26);
        System.methodSignal(1);
        this.op_upr1 = new Ob_op();
        this.op_upr2 = new Ob_op();
        this.op_upr1.init(523, 5.22f, 0.105f, 13.78f, 135.0f);
        this.op_upr2.init(524, 5.22f, 0.105f, 16.78f, 135.0f);
        this.op_upr1.renderCommand(22);
        this.op_upr2.renderCommand(22);
        this.op_upr1.setShadow(0, 0);
        this.op_upr2.setShadow(0, 0);
        this.op_upr1.setMotNoUpdate(1);
        this.op_upr2.setMotNoUpdate(1);
        this.ob_l = new Ob_op();
        this.ob_r = new Ob_op();
        this.op_l1 = new Ob_op();
        this.op_l2 = new Ob_op();
        this.op_l3 = new Ob_op();
        this.op_r1 = new Ob_op();
        this.op_r2 = new Ob_op();
        this.op_r3 = new Ob_op();
        this.ob_l.init(524, -5.65f, -1.95f, 5.25f, 180.0f);
        this.ob_r.init(522, 5.6f, -1.95f, 5.3f, 180.0f);
        this.op_l1.init(523, -2.6f, -2.2f, -3.6f, -135.0f);
        this.op_l2.init(520, -2.55f, -2.25f, -0.6f, -135.0f);
        this.op_l3.init(522, -2.55f, -2.25f, 2.45f, -135.0f);
        this.op_r1.init(519, 2.55f, -2.25f, -3.55f, 135.0f);
        this.op_r2.init(521, 2.55f, -2.25f, -0.6f, 135.0f);
        this.op_r3.init(524, 2.6f, -2.2f, 2.4f, 135.0f);
        this.ob_l.renderCommand(22);
        this.ob_r.renderCommand(22);
        this.op_l1.renderCommand(22);
        this.op_l2.renderCommand(22);
        this.op_l3.renderCommand(22);
        this.op_r1.renderCommand(22);
        this.op_r2.renderCommand(22);
        this.op_r3.renderCommand(22);
        this.op_upr1.renderCommand(512);
        this.op_upr2.renderCommand(512);
        this.ob_l.renderCommand(512);
        this.ob_r.renderCommand(512);
        this.op_l1.renderCommand(512);
        this.op_l2.renderCommand(512);
        this.op_l3.renderCommand(512);
        this.op_r1.renderCommand(512);
        this.op_r2.renderCommand(512);
        this.op_r3.renderCommand(512);
        this.ob_l.setShadow(0, 0);
        this.ob_r.setShadow(0, 0);
        this.op_l1.setShadow(0, 0);
        this.op_l2.setShadow(0, 0);
        this.op_l3.setShadow(0, 0);
        this.op_r1.setShadow(0, 0);
        this.op_r2.setShadow(0, 0);
        this.op_r3.setShadow(0, 0);
        this.ob_l.setMotNoUpdate(1);
        this.ob_r.setMotNoUpdate(1);
        this.op_l1.setMotNoUpdate(1);
        this.op_l2.setMotNoUpdate(1);
        this.op_l3.setMotNoUpdate(1);
        this.op_r1.setMotNoUpdate(1);
        this.op_r2.setMotNoUpdate(1);
        this.op_r3.setMotNoUpdate(1);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.ob_l.setVisible(true);
        this.ob_r.setVisible(true);
        this.op_l3.setVisible(true);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.army_1 = new Navy(20485, 9.0f, -17.0f, 14.3f, 180.0f);
        this.army_1.setVisible(false);
        this.mobj_133 = new Navy(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.army_phone = new Navy(24587, 0.0f, 0.0f, 0.0f, 0.0f);
        this.army_phone.setTranslate(0.05f, 0.0f, 0.01f);
        this.army_phone.setRotate(10.0f, -16.0f, -150.0f);
        this.army_phone.setVisible(false);
        Stage.setVisible(137, false);
        Stage.setVisible(138, false);
        Stage.setVisible(151, false);
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
        this.fm1 = new Unit();
        this.fm1.init(24613, -3.41f, -1.8f, -4.45f, 45.0f);
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm1.setArgs(1, 15001, 0, 128, 112);
        this.fm1.setArgs(2, 96, 0, 0, -1);
        this.fm1.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm2 = new Unit();
        this.fm2.init(24613, -3.41f, -1.8f, -1.45f, 45.0f);
        this.fm2.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm2.setArgs(1, 15001, 0, 128, 112);
        this.fm2.setArgs(2, 96, 0, 0, -1);
        this.fm2.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm3 = new Unit();
        this.fm3.init(24613, -3.41f, -1.8f, 1.58f, 45.0f);
        this.fm3.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm3.setArgs(1, 15001, 0, 128, 112);
        this.fm3.setArgs(2, 96, 0, 0, -1);
        this.fm3.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm4 = new Unit();
        this.fm4.init(24613, 3.41f, -1.8f, -4.45f, -45.0f);
        this.fm4.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm4.setArgs(1, 15001, 0, 128, 112);
        this.fm4.setArgs(2, 96, 0, 0, -1);
        this.fm4.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm5 = new Unit();
        this.fm5.init(24613, 3.41f, -1.8f, -1.45f, -45.0f);
        this.fm5.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm5.setArgs(1, 15001, 0, 128, 112);
        this.fm5.setArgs(2, 96, 0, 0, -1);
        this.fm5.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm6 = new Unit();
        this.fm6.init(24613, 3.41f, -1.8f, 1.58f, -45.0f);
        this.fm6.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm6.setArgs(1, 15001, 0, 128, 112);
        this.fm6.setArgs(2, 96, 0, 0, -1);
        this.fm6.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm7 = new Unit();
        this.fm7.init(24613, -5.6f, -1.46f, 4.6f, 0.0f);
        this.fm7.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm7.setArgs(1, 15002, 0, 128, 114);
        this.fm7.setArgs(2, 96, 0, 0, -1);
        this.fm7.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm8 = new Unit();
        this.fm8.init(24613, 5.6f, -1.46f, 4.6f, 0.0f);
        this.fm8.setArgs(0, 0.0f, 0.5f, 1.0f, 0.4f);
        this.fm8.setArgs(1, 15002, 0, 128, 114);
        this.fm8.setArgs(2, 96, 0, 0, -1);
        this.fm8.setArgs(3, 0.0f, 0.1f, 0.0f, 0.0f);
        this.fm9 = new Unit();
        this.fm9.init(24613, -6.37f, 1.45f, 12.63f, 45.0f);
        this.fm9.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm9.setArgs(1, 15001, 0, 128, 112);
        this.fm9.setArgs(2, 96, 0, 0, -1);
        this.fm9.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm10 = new Unit();
        this.fm10.init(24613, -6.37f, 1.45f, 15.63f, 45.0f);
        this.fm10.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm10.setArgs(1, 15001, 0, 128, 112);
        this.fm10.setArgs(2, 96, 0, 0, -1);
        this.fm10.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm11 = new Unit();
        this.fm11.init(24613, 6.37f, 1.45f, 12.63f, -45.0f);
        this.fm11.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm11.setArgs(1, 15001, 0, 128, 112);
        this.fm11.setArgs(2, 96, 0, 0, -1);
        this.fm11.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm12 = new Unit();
        this.fm12.init(24613, 6.37f, 1.45f, 15.63f, -45.0f);
        this.fm12.setArgs(0, 0.0f, 0.0f, 1.5f, 0.63f);
        this.fm12.setArgs(1, 15001, 0, 128, 112);
        this.fm12.setArgs(2, 96, 0, 0, -1);
        this.fm12.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(0);
        this.fm2.signal(0);
        this.fm3.signal(0);
        this.fm4.signal(0);
        this.fm5.signal(0);
        this.fm6.signal(1);
        this.fm7.signal(0);
        this.fm8.signal(1);
        this.fm9.signal(0);
        this.fm10.signal(0);
        this.fm11.signal(0);
        this.fm12.signal(0);
        this.army_star = new Mapunits();
        this.army_star.mapUnit(0);
        this.army_star.start(4, null);
        this.ring_1a = new Mapunits();
        this.ring_1b = new Mapunits();
        this.ring_1c = new Mapunits();
        this.ring_1a.mapUnit(126);
        this.ring_1b.mapUnit(127);
        this.ring_1c.mapUnit(128);
        this.ring_1a.start(4, null);
        this.ring_1b.start(4, null);
        this.ring_1c.start(4, null);
        this.ring_1a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2a = new Mapunits();
        this.ring_2b = new Mapunits();
        this.ring_2c = new Mapunits();
        this.ring_2a.mapUnit(129);
        this.ring_2b.mapUnit(130);
        this.ring_2c.mapUnit(131);
        this.ring_2a.start(4, null);
        this.ring_2b.start(4, null);
        this.ring_2c.start(4, null);
        this.ring_2a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.sonar_ring_0 = new Mapunits();
        this.sonar_ring_1 = new Mapunits();
        this.sonar_ring_2 = new Mapunits();
        this.sonar_ring_3 = new Mapunits();
        this.sonar_ring_0.mapUnit(133);
        this.sonar_ring_1.mapUnit(134);
        this.sonar_ring_2.mapUnit(135);
        this.sonar_ring_3.mapUnit(136);
        this.sonar_ring_0.start(4, null);
        this.sonar_ring_1.start(4, null);
        this.sonar_ring_2.start(4, null);
        this.sonar_ring_3.start(4, null);
        this.sonar_ring_0.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_1.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_2.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_3.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_0.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_1.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_2.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_3.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_0.start(1, "sonar_ring_Large");
        this.sonar_ring_1.start(1, "sonar_ring_L");
        this.sonar_ring_2.start(1, "sonar_ring_R");
        this.sonar_ring_3.start(1, "sonar_ring_L");
        this.moriyama.start(1, "init2");
        this.moriyama2.start(1, "init2");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
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
        this.ob_l.start(1, "w_uti");
        this.ob_r.start(1, "init");
        this.op_r1.start(1, "m_uti");
        this.op_r2.start(1, "m_uti");
        this.op_r3.start(1, "w_uti");
        this.op_l1.start(1, "w_uti");
        this.op_l2.start(1, "init");
        this.op_l3.start(1, "m_uti");
        this.op_upr1.start(1, "w_uti");
        this.op_upr2.start(1, "init");
        System.sleep(1);
        Stage.setVisible(148, true);
        Stage.setVisible(149, true);
        Sound.streamPlay(1190061, 48000);
        this.CameraPlay = 1;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.46f, 0.392f, -0.797f);
        this.light.setColor(2, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(2, 0.815f, 0.507f, -0.281f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.166f, -0.798f, -0.579f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.mobj_133.start(1, "mobj_133_act");
        Stage.setVisible(0, false);
        this.shion.start(1, "act");
        this.shion2.start(1, "act");
        this.andrew.start(1, "act");
        this.andrew2.start(1, "act");
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.shion2.face.mtn(303, 9, 1.0f, false);
        this.shion2.face.start(4, null);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.andrew2.face.mtn(309, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        System.sleep(30);
        this.msg.print("I see.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(30);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(15);
        this.msg.print("I understand the basic specs.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.andrew2.face.mtn(308, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        System.sleep(60);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.andrew2.face.mtn(309, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        this.waitclear(30);
        System.sleep(14);
        this.CameraPlay = 2;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 43880, 1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, -0.935f, 0.34f, -0.102f);
        this.light.setColor(2, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(2, 0.409f, 0.206f, -0.889f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, -0.312f, -0.868f, -0.387f);
        this.mobj_133.start(1, "mobj_133_act");
        this.op_r1.start(1, "idou");
        this.shion.start(1, "act2");
        this.shion2.start(1, "act");
        this.andrew.start(1, "act2");
        this.andrew2.start(1, "act2");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(true);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(true);
        this.op_upr2.setVisible(true);
        this.fm1.signal(1);
        this.fm2.signal(1);
        this.fm3.signal(1);
        this.fm4.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.fm7.signal(1);
        this.fm8.signal(1);
        this.fm9.signal(1);
        this.fm10.signal(1);
        this.fm11.signal(1);
        this.fm12.signal(1);
        System.sleep(8);
        this.msg.print("So, where's the actual field data?");
        this.andrew.face.mtn(308, 9, 0.8f, false);
        this.andrew.face.start(4, null);
        System.sleep(12);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(7);
        System.sleep(12);
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(48);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(18);
        this.shion.look_speed(0.0f);
        this.shion.look_char(this.andrew);
        this.msg.print("Well, it's not quite ready...");
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(15);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(15);
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(36);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(12);
        System.sleep(15);
        this.msg.print("All I can provide today is up to A-7.");
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(60);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(21);
        this.msg.print("So you don't have it?/[wait(55)]");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.msg.clear();
        System.sleep(45);
        this.CameraPlay = 3;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 28880, 1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, -0.977f, 0.193f, -0.09f);
        this.light.setColor(2, 0.63f, 0.63f, 0.63f);
        this.light.setDirection2(2, 0.629f, 0.319f, -0.709f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, -0.591f, -0.373f, -0.715f);
        this.mobj_133.start(1, "mobj_133_act");
        this.shion.start(1, "act3");
        System.sleep(30);
        this.andrew.face.mtn(309, 8, 1.0f, false);
        this.andrew.face.start(4, null);
        this.msg.print("Well...");
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(15);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(24);
        System.sleep(8);
        this.msg.print("We'll begin testing with a mock-up unit\nvery soon, but the system is still\na little unstable, and...");
        this.shion.face.mtn(302, 9, 1.1f, false);
        this.shion.face.start(4, null);
        System.sleep(105);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(15);
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(54);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(2);
        System.sleep(7);
        this.CameraPlay = 4;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 74880, 1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.68f, 0.68f, 0.68f);
        this.light.setDirection2(1, 0.776f, 0.413f, -0.476f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, 0.443f, 0.334f, 0.832f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, 0.432f, -0.85f, 0.303f);
        this.mobj_133.start(1, "mobj_133_act");
        this.andrew.start(1, "act4");
        this.ob_l.setVisible(true);
        this.op_l1.setVisible(true);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(true);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.msg.print("Aren't you just making excuses?");
        System.sleep(6);
        System.sleep(9);
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(36);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(12);
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(21);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(6);
        this.msg.print("Listen, Chief Uzuki.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(30);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(15);
        System.sleep(4);
        System.sleep(1);
        this.CameraPlay = 5;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(1, -0.939f, 0.342f, -0.04f);
        this.light.setColor(2, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(2, 0.191f, 0.137f, -0.972f);
        this.light.setColor(3, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(3, -0.591f, -0.373f, -0.715f);
        Runtime.setDefocusQuick(0, 1, 45880, 1);
        this.mobj_133.start(1, "mobj_133_act");
        this.andrew.start(1, "act5");
        this.shion.start(1, "act5");
        this.shion2.start(1, "act5");
        this.m.setArgs(2, 101, 0, 5, -5);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(true);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(true);
        this.op_upr2.setVisible(true);
        this.fm1.signal(0);
        this.fm3.signal(0);
        this.fm4.signal(0);
        this.fm9.signal(0);
        this.fm7.signal(0);
        this.fm10.signal(0);
        this.fm11.signal(0);
        System.sleep(15);
        this.msg.print("Why do you think you're on this ship?");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(120);
        this.msg.print("Think about that for a moment./[wait(70)]");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.msg.clear();
        System.sleep(85);
        this.CameraPlay = 105;
        System.sleep(30);
        this.CameraPlay = 6;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 0, 45880, 1);
        Stage.setVisible(0, true);
        this.mobj_133.setVisible(false);
        Stage.setVisible(64, false);
        Stage.setVisible(65, false);
        Stage.setVisible(66, false);
        Stage.setVisible(67, false);
        Stage.setVisible(68, false);
        Stage.setVisible(69, false);
        Stage.setVisible(70, false);
        Stage.setVisible(151, false);
        Stage.setVisible(92, false);
        Stage.setVisible(93, false);
        Stage.setVisible(132, false);
        Stage.setVisible(133, false);
        Stage.setVisible(134, false);
        Stage.setVisible(135, false);
        Stage.setVisible(136, false);
        Stage.setVisible(124, false);
        Stage.setVisible(125, false);
        Stage.setVisible(91, false);
        Stage.setVisible(120, false);
        Stage.setVisible(121, false);
        Stage.setVisible(122, false);
        this.fm2.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.fm8.signal(1);
        this.fm12.signal(0);
        this.andrew.start(1, "act6");
        this.andrew2.start(1, "act6");
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.792f, 0.608f, 0.049f);
        this.light.setColor(2, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(2, 0.06f, 0.538f, -0.841f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, 0.16f, -0.684f, -0.712f);
        this.army_star.start(1, "star");
        this.army_1.start(1, "army_1");
        this.op_r1.start(1, "kaeru");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(true);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        System.sleep(35);
        this.msg.print("This fleet may be newly outfitted,\nbut it was assembled under major\ntime constraints.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(40);
        this.m.start(1, "moni_d");
        System.sleep(70);
        System.sleep(20);
        this.m.signal(0);
        this.waitclear(21);
        System.sleep(30);
        this.msg.print("And we have A.G.W.S. units, but the\n100-Series Observational Units that go\nwith them aren't available until later.");
        this.waitclear(195);
        System.sleep(21);
        this.msg.print("What would happen, if by some chance,\nwe were attacked by those things?");
        System.sleep(45);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.CameraPlay = 7;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 76880, -1);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, -0.532f, 0.42f, -0.735f);
        this.light.setColor(2, 0.77f, 0.77f, 0.77f);
        this.light.setDirection2(2, 0.793f, 0.462f, -0.396f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, 0.029f, -0.853f, -0.52f);
        Stage.setVisible(0, false);
        this.mobj_133.setVisible(true);
        this.fm1.signal(0);
        this.fm2.signal(1);
        this.fm3.signal(1);
        this.fm4.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.fm7.signal(1);
        this.fm8.signal(1);
        this.fm9.signal(1);
        this.fm10.signal(1);
        this.fm11.signal(1);
        this.fm12.signal(1);
        Stage.setVisible(64, true);
        Stage.setVisible(65, true);
        Stage.setVisible(66, true);
        Stage.setVisible(67, true);
        Stage.setVisible(68, true);
        Stage.setVisible(69, true);
        Stage.setVisible(70, true);
        Stage.setVisible(151, true);
        Stage.setVisible(124, true);
        Stage.setVisible(125, true);
        Stage.setVisible(91, true);
        Stage.setVisible(92, true);
        Stage.setVisible(93, true);
        Stage.setVisible(120, true);
        Stage.setVisible(121, true);
        Stage.setVisible(122, true);
        Stage.setVisible(148, true);
        Stage.setVisible(149, true);
        this.mobj_133.start(1, "mobj_133_act");
        this.shion.start(1, "act7");
        this.shion2.start(1, "act7");
        this.army_1.start(1, "army_1_act2");
        this.gnoFilter[0] = 0.35f;
        this.gnoFilter[1] = 0.0f;
        this.gnoFilter[2] = 0.0f;
        this.gnoFilter[3] = 0.0f;
        this.andrew2.setFilter(2);
        this.andrew2.setFilterParam(this.gnoFilter);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.andrew.renderCommand(0);
        this.andrew2.renderCommand(0);
        this.shion2.renderCommand(0);
        this.waitclear(75);
        this.msg.print("I don't think I need to describe\nthe outcome to you.");
        this.andrew2.face.mtn(308, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        this.waitclear(63);
        this.andrew2.face.mtn(309, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        System.sleep(21);
        this.msg.print("The purpose of KOS-MOS' deployment\nwas to address this issue.");
        this.andrew2.face.mtn(308, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        this.waitclear(69);
        this.andrew2.face.mtn(309, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        System.sleep(9);
        System.sleep(15);
        this.msg.print("Or am I mistaken?");
        this.andrew2.face.mtn(308, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        this.waitclear(36);
        this.andrew2.face.mtn(309, 9, 1.0f, false);
        this.andrew2.face.start(4, null);
        this.CameraPlay = 8;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 63880, 1);
        this.army_1.setVisible(false);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.65f, 0.65f, 0.65f);
        this.light.setDirection2(1, 0.3f, 0.217f, -0.929f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.936f, 0.281f, -0.212f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, -0.268f, -0.415f, -0.869f);
        this.andrew2.setFilter(0);
        this.andrew.renderCommand(512);
        this.andrew2.renderCommand(512);
        this.shion2.renderCommand(512);
        this.mobj_133.start(1, "mobj_133_act");
        this.andrew2.start(1, "act8");
        this.shion2.start(1, "act8");
        this.shion.start(1, "act8");
        this.op_r1.start(1, "idou");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(true);
        this.op_upr2.setVisible(true);
        System.sleep(10);
        this.msg.print("N-no, Sir...");
        this.shion.face.mtn(302, 9, 0.7f, false);
        this.shion.face.start(4, null);
        System.sleep(25);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(19);
        System.sleep(5);
        this.andrew.start(1, "act9");
        System.sleep(1);
        this.CameraPlay = 9;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(1, 0.912f, 0.391f, -0.123f);
        this.light.setColor(2, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(2, -0.459f, 0.41f, 0.788f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, 0.573f, -0.526f, 0.628f);
        this.mobj_133.start(1, "mobj_133_act");
        this.shion.start(1, "act9");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(true);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.fm1.signal(1);
        this.msg.print("Listen, you're not in a\nlaboratory anymore.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(30);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(15);
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(39);
        this.msg.print("You're on a warship.");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(30);
        this.CameraPlay = 109;
        this.waitclear(18);
        this.msg.print("This is a battlefield!");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(40);
        System.sleep(14);
        System.sleep(1);
        this.CameraPlay = 10;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 2, 115264, -2);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.66f, 0.66f, 0.66f);
        this.light.setDirection2(1, 0.301f, 0.228f, -0.926f);
        this.light.setColor(2, 0.46f, 0.46f, 0.46f);
        this.light.setDirection2(2, -0.905f, 0.136f, 0.403f);
        this.light.setColor(3, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(3, -0.475f, -0.505f, -0.72f);
        this.shion.start(1, "act10");
        this.andrew.start(1, "act10");
        this.mobj_133.start(1, "mobj_133_act");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        System.sleep(1);
        this.msg.print("KOS-MOS was supposed to be\nour frontline defense here,");
        this.waitclear(65);
        this.msg.print("and now you're telling us\nit hasn't even woken up yet?!");
        this.waitclear(117);
        System.sleep(3);
        this.CameraPlay = 11;
        this.Timechk_CutChange();
        this.mobj_133.start(1, "mobj_133_act");
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.74f, 0.74f, 0.74f);
        this.light.setDirection2(1, 0.909f, 0.414f, -0.04f);
        this.light.setColor(2, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(2, -0.455f, 0.538f, 0.71f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.637f, -0.663f, 0.393f);
        this.andrew.start(1, "act11");
        this.shion.start(1, "act11");
        this.shion.look_default();
        this.shion2.setVisible(false);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(true);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(true);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(false);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.msg.print("There's no point in having a weapon\nthat can't even get out of bed!");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(60);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(9);
        System.sleep(6);
        this.msg.print("That thing's only meaningful to us\nwhen it's fully operational!");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(84);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(22);
        this.msg.print("Why can't you...");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(30);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.msg.print("Let it go, Commander.");
        System.sleep(44);
        System.sleep(1);
        this.CameraPlay = 12;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 24880, 1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.66f, 0.66f, 0.66f);
        this.light.setDirection2(1, -0.885f, 0.442f, 0.146f);
        this.light.setColor(2, 0.79f, 0.79f, 0.79f);
        this.light.setDirection2(2, 0.007f, 0.63f, -0.777f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, -0.257f, -0.867f, -0.427f);
        this.army_star.start(1, "star");
        this.moriyama.start(1, "act12");
        this.moriyama2.start(1, "act12");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(true);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.shion.start(1, "act12");
        this.shion2.start(1, "act12");
        this.andrew.start(1, "act12");
        this.waitclear(30);
        this.msg.print("These people are working under\nserious time constraints,");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(310, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        System.sleep(99);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(311, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        this.waitclear(18);
        this.msg.print("just like our own squadron.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(310, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        this.waitclear(108);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(311, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        System.sleep(21);
        this.msg.print("Besides, they're only one step away\nfrom actual field testing.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(310, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        System.sleep(15);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(311, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        System.sleep(9);
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(310, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        System.sleep(48);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.moriyama2.face.mtn(311, 9, 1.0f, false);
        this.moriyama2.face.start(4, null);
        this.waitclear(15);
        System.sleep(14);
        System.sleep(1);
        this.CameraPlay = 13;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.67f, 0.67f, 0.67f);
        this.light.setDirection2(1, -0.898f, 0.347f, 0.27f);
        this.light.setColor(2, 0.62f, 0.62f, 0.62f);
        this.light.setDirection2(2, 0.007f, 0.687f, -0.727f);
        this.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(3, -0.628f, -0.604f, -0.491f);
        Runtime.setDefocusQuick(0, 1, 26880, 1);
        this.mobj_133.start(1, "mobj_133_act");
        this.moriyama.start(1, "act13");
        this.msg.print("In the end, we all want to see this\noperation completed without having to\nresort to that thing,");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(21);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(15);
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(84);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(15);
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(30);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(30);
        System.sleep(6);
        this.msg.print("don't we?");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(18);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(24);
        System.sleep(3);
        this.CameraPlay = 14;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.018f, 0.473f, -0.881f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.883f, 0.408f, 0.231f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.606f, -0.555f, -0.57f);
        this.mobj_133.start(1, "mobj_133_act");
        this.op_r1.start(1, "idou");
        this.ob_l.setVisible(true);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.andrew.setHand(this.Rhand | this.Close);
        this.andrew.start(1, "act14");
        this.msg.print("Of course, Captain,");
        this.andrew.face.mtn(308, 9, 1.1f, false);
        this.andrew.face.start(4, null);
        System.sleep(38);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(7);
        System.sleep(2);
        this.msg.print("but I believe...");
        this.andrew.face.mtn(308, 9, 1.1f, false);
        this.andrew.face.start(4, null);
        System.sleep(30);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(30);
        System.sleep(8);
        System.sleep(15);
        this.waitclear(30);
        this.msg.print("...Arg! Who's calling me...");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(15);
        this.army_phone.setVisible(true);
        this.CameraPlay = 16;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, -0.485f, 0.461f, -0.743f);
        this.light.setColor(2, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(2, 0.647f, 0.236f, -0.725f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.35f, -0.692f, -0.631f);
        this.andrew.setHand(this.Rhand | this.Open);
        this.mobj_133.start(1, "mobj_133_act");
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(true);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(false);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.shion.start(1, "act16");
        this.shion2.start(1, "act16");
        this.andrew.start(1, "act16");
        this.andrew2.start(1, "act16");
        this.moriyama.start(1, "act16");
        this.waitclear(30);
        System.sleep(90);
        this.msg.print("Is there a problem?");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(27);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(3);
        System.sleep(30);
        this.msg.print("Oh, uh, no.");
        this.andrew.face.mtn(308, 9, 0.3f, false);
        this.andrew.face.start(4, null);
        System.sleep(45);
        this.andrew.face.mtn(309, 9, 0.3f, false);
        this.andrew.face.start(4, null);
        this.waitclear(15);
        this.msg.print("Something urgent's come up...");
        this.andrew.face.mtn(308, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        System.sleep(33);
        this.andrew.face.mtn(309, 9, 0.3f, false);
        this.andrew.face.start(4, null);
        this.waitclear(35);
        this.msg.print("If you will excuse me.");
        this.andrew.face.mtn(308, 9, 0.3f, false);
        this.andrew.face.start(4, null);
        System.sleep(33);
        this.andrew.face.mtn(309, 9, 1.0f, false);
        this.andrew.face.start(4, null);
        this.waitclear(27);
        this.CameraPlay = 116;
        System.sleep(9);
        this.CameraPlay = 17;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.13f, 0.13f, 0.13f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.619f, 0.386f, 0.683f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.94f, 0.0f, -0.341f);
        this.light.setColor(3, 0.18f, 0.18f, 0.18f);
        this.light.setDirection2(3, -0.71f, -0.654f, 0.262f);
        Runtime.setDefocusQuick(0, 1, 29880, 1);
        this.andrew2.setVisible(false);
        this.mobj_133.start(1, "mobj_133_act");
        this.op_r1.start(1, "kaeru");
        Stage.setVisible(64, false);
        Stage.setVisible(65, false);
        Stage.setVisible(66, false);
        Stage.setVisible(67, false);
        Stage.setVisible(68, false);
        Stage.setVisible(69, false);
        Stage.setVisible(70, false);
        Stage.setVisible(151, false);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.moriyama.start(1, "act17");
        this.shion.start(1, "act17");
        this.andrew.start(1, "act17");
        this.andrew2.setVisible(false);
        System.sleep(30);
        this.army_phone.setVisible(false);
        System.sleep(60);
        this.msg.print("I wonder what's wrong?");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(42);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(8);
        System.sleep(30);
        this.msg.print("I don't know...");
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(35);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(35);
        this.msg.print("Well, never mind.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(30);
        this.moriyama.face.mtn(311, 9, 0.7f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(45);
        this.msg.print("That's enough for today,\nChief Uzuki.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(21);
        this.moriyama.face.mtn(311, 9, 0.7f, false);
        this.moriyama.face.start(4, null);
        System.sleep(21);
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(45);
        this.moriyama.face.mtn(311, 9, 0.7f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(15);
        this.msg.print("Let me know if any\nnew developments arise.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(69);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(21);
        this.CameraPlay = 18;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.11f, 0.11f, 0.11f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.674f, 0.396f, -0.624f);
        this.light.setColor(2, 0.52f, 0.52f, 0.52f);
        this.light.setDirection2(2, 0.71f, 0.271f, -0.65f);
        this.light.setColor(3, 0.22f, 0.22f, 0.22f);
        this.light.setDirection2(3, -0.459f, -0.748f, -0.48f);
        Runtime.setDefocusQuick(0, 1, 24880, 1);
        this.mobj_133.start(1, "mobj_133_act");
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(false);
        this.op_l2.setVisible(false);
        this.op_l3.setVisible(false);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.op_upr1.setVisible(true);
        this.op_upr2.setVisible(true);
        this.moriyama.start(1, "act18");
        this.shion.start(1, "act18");
        System.sleep(30);
        this.msg.print("I'm sorry we failed to meet your\nexpectations.");
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(36);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(21);
        this.shion.face.mtn(302, 9, 1.0f, false);
        this.shion.face.start(4, null);
        System.sleep(33);
        this.shion.face.mtn(303, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(15);
        this.msg.print("Oh, there's no need to apologize.");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(63);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(27);
        this.msg.print("What's important to us is");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(90);
        this.msg.print("how reliable the system is\nonce we start using it.");
        System.sleep(105);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(15);
        this.msg.print("Rushing the project\nwon't get us anywhere,");
        this.moriyama.face.mtn(310, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        System.sleep(51);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.shion.look_speed(0.0f);
        this.shion.look_point(0.7f, -3.1f, -0.6f);
        this.waitclear(24);
        System.sleep(9);
        this.CameraPlay = 19;
        this.Timechk_CutChange();
        Runtime.setDefocusQuick(0, 1, 34880, 1);
        Runtime.setDefocusQuick(1, 1, 26688, 1);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.69f, 0.275f, -0.67f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.71f, 0.271f, -0.65f);
        this.light.setColor(3, 0.24f, 0.24f, 0.24f);
        this.light.setDirection2(3, 0.16f, -0.706f, -0.69f);
        this.mobj_133.start(1, "mobj_133_act");
        this.shion.start(1, "act19");
        this.msg.print("so just calm down and take\nas much time as you need.");
        System.sleep(85);
        this.waitclear(5);
        this.msg.print("The government's funding it all,\nanyway, right?");
        System.sleep(75);
        this.shion.face.mtn(305, 9, 1.0f, false);
        this.shion.face.start(4, null);
        this.waitclear(60);
        System.sleep(1);
        this.CameraPlay = 20;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.11f, 0.11f, 0.11f);
        this.light.setColor(1, 0.8f, 0.8f, 0.8f);
        this.light.setDirection2(1, -0.889f, 0.277f, 0.364f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.735f, 0.412f, 0.539f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.15f, -0.569f, 0.808f);
        this.mobj_133.start(1, "mobj_133_act");
        this.moriyama.start(1, "act20");
        this.op_r1.setVisible(true);
        this.op_r1.start(1, "kaeru");
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(true);
        this.fm1.signal(1);
        this.fm2.signal(1);
        this.fm3.signal(1);
        this.fm4.signal(1);
        this.fm5.signal(1);
        this.fm6.signal(1);
        this.fm7.signal(0);
        this.fm8.signal(0);
        this.fm9.signal(0);
        this.fm10.signal(0);
        this.fm11.signal(0);
        this.fm12.signal(0);
        this.msg.print("You must be tired.");
        this.moriyama.face.mtn(310, 9, 1.3f, false);
        this.moriyama.face.start(4, null);
        System.sleep(27);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(15);
        System.sleep(10);
        this.msg.print("Take the rest of the day off.");
        this.moriyama.face.mtn(310, 9, 1.3f, false);
        this.moriyama.face.start(4, null);
        System.sleep(30);
        this.moriyama.face.mtn(311, 9, 1.0f, false);
        this.moriyama.face.start(4, null);
        this.waitclear(6);
        System.sleep(18);
        System.sleep(5);
        System.sleep(5);
        this.CameraPlay = 21;
        this.Timechk_CutChange();
        this.light.setColor(0, 0.11f, 0.11f, 0.11f);
        this.light.setColor(1, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(1, -0.715f, 0.276f, 0.642f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.808f, 0.474f, 0.35f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, -0.578f, -0.715f, 0.394f);
        Runtime.setDefocusQuick(0, 0, 34880, 1);
        Runtime.setDefocusQuick(1, 0, 26688, 1);
        Stage.setVisible(0, true);
        this.mobj_133.setVisible(false);
        Stage.setVisible(132, false);
        Stage.setVisible(133, false);
        Stage.setVisible(134, false);
        Stage.setVisible(135, false);
        Stage.setVisible(136, false);
        Stage.setVisible(124, false);
        Stage.setVisible(125, false);
        Stage.setVisible(120, false);
        Stage.setVisible(121, false);
        Stage.setVisible(122, false);
        Stage.setVisible(83, false);
        Stage.setVisible(84, false);
        Stage.setVisible(114, false);
        Stage.setVisible(115, false);
        Stage.setVisible(116, false);
        this.army_star.start(1, "star");
        this.op_l1.setVisible(true);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(true);
        this.op_upr1.setVisible(false);
        this.ob_l.setVisible(false);
        this.op_l1.setVisible(true);
        this.op_l2.setVisible(true);
        this.op_l3.setVisible(true);
        this.ob_r.setVisible(false);
        this.op_r1.setVisible(true);
        this.op_r2.setVisible(true);
        this.op_r3.setVisible(false);
        this.op_upr1.setVisible(false);
        this.op_upr2.setVisible(false);
        this.shion.start(1, "act21");
        this.shion2.start(1, "act21");
        this.moriyama.start(1, "act21");
        this.moriyama2.start(1, "act21");
        System.sleep(30);
        this.CameraPlay = 121;
        System.sleep(30);
        System.sleep(60);
        System.sleep(60);
        this.army_star.stop();
        this.ring_1a.stop();
        this.ring_1b.stop();
        this.ring_1c.stop();
        this.ring_2a.stop();
        this.ring_2b.stop();
        this.ring_2c.stop();
        this.sonar_ring_0.stop();
        this.sonar_ring_1.stop();
        this.sonar_ring_2.stop();
        this.sonar_ring_3.stop();
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

    class Shion
            extends Chr {
        Chr face;

        Shion() {
        }

        void act() {
            this.setShadow(0, 0);
            this.mtn(258, 2056, 1.0f, true);
        }

        void act10() {
            this.setTranslate(0.4f, -2.0f, 3.0f);
            this.setRotate(0.0f, 195.0f, 0.0f);
            this.setVisible(true);
            this.mtn(274, 2048, 1.0f, true);
        }

        void act11() {
            this.setTranslate(0.38f, -2.0f, 2.99f);
            this.setRotate(0.0f, 186.0f, 0.0f);
            this.setMotionFlags(Integer.MIN_VALUE, true);
            this.mtn(276, 0, 0.9f, true);
        }

        void act12() {
            this.setMotionFlags(Integer.MIN_VALUE, false);
            this.mtn(280, 2048, 0.98f, true);
        }

        void act16() {
            this.setTranslate(0.37f, -2.0f, 3.79f);
            this.setRotate(0.0f, 167.14f, 0.0f);
            this.mtn(287, 0, 0.95f, true);
        }

        void act17() {
            this.setTranslate(0.39f, -2.0f, 3.79f);
            this.setRotate(0.0f, 239.14f, 0.0f);
            this.mtn(289, 0, 1.0f, true);
            this.mtn(289, 0, -0.7f, true);
        }

        void act18() {
            this.setTranslate(0.44f, -2.0f, 3.29f);
            this.setRotate(0.0f, 159.14f, 0.0f);
            this.mtn(292, 0, 1.0f, true);
            this.mtn(292, 0, -1.0f, true);
        }

        void act19() {
            this.mtn(293, 0, 0.98f, true);
        }

        void act2() {
            this.mtn(259, 2048, 1.0f, true);
        }

        void act21() {
            this.setTranslate(0.46f, -2.0f, 3.74f);
            this.setRotate(0.0f, 159.14f, 0.0f);
            this.mtn(295, 2048, 1.0f, true);
        }

        void act3() {
            this.setRotate(0.0f, 220.0f, 0.0f);
            this.mtn(261, 2048, 1.0f, true);
        }

        void act5() {
            this.mtn(263, 2048, 1.0f, true);
            this.mtn(263, 300, 419, 1, 8, 1.0f, true);
        }

        void act7() {
            this.mtn(270, 0, 1.0f, true);
        }

        void act8() {
            this.setTranslate(0.4f, -2.0f, 3.0f);
            this.setRotate(0.0f, 200.0f, 0.0f);
            this.mtn(271, 2048, 1.0f, true);
        }

        void act9() {
            this.setVisible(false);
        }

        void init() {
            this.init(0x1000001, 0.4f, -2.0f, 3.0f, 180.0f);
            this.setMotionFlags(0x2000000, true);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }

        void init2() {
        }
    }

    class Shion2
            extends Chr {
        Chr face;

        Shion2() {
        }

        void act() {
            this.setShadow(0, 0);
            this.setSymmetryY(1);
            this.mtn(258, 8, 1.0f, true);
        }

        void act12() {
            this.setTranslate(0.38f, -2.0f, 2.99f);
            this.setRotate(0.0f, 186.0f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(280, 0, 1.0f, true);
        }

        void act16() {
            this.setTranslate(0.37f, -2.0f, 3.79f);
            this.setRotate(0.0f, 167.14f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(287, 0, 0.95f, true);
        }

        void act2() {
            this.setSymmetryY(1);
            this.mtn(258, 0, 1, 1, 2048, 1.0f, true);
        }

        void act21() {
            this.setTranslate(0.44f, -2.0f, 3.29f);
            this.setRotate(0.0f, 159.14f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(295, 10240, 1.0f, true);
        }

        void act5() {
            this.setRotate(0.0f, 220.0f, 0.0f);
            this.mtn(263, 10240, 1.0f, true);
            this.mtn(263, 300, 419, 1, 8200, 1.0f, true);
        }

        void act7() {
            this.setSymmetryY(1);
            this.setRotate(0.0f, 194.0f, 0.0f);
            SCE01015.this.gnoFilter[0] = 0.35f;
            SCE01015.this.gnoFilter[1] = 0.0f;
            SCE01015.this.gnoFilter[2] = 0.0f;
            SCE01015.this.gnoFilter[3] = 0.0f;
            SCE01015.this.shion2.setFilter(2);
            SCE01015.this.shion2.setFilterParam(SCE01015.this.gnoFilter);
            this.mtn(270, 0, 1.0f, true);
        }

        void act8() {
            this.setSymmetryY(1);
            SCE01015.this.gnoFilter[0] = 1.0f;
            SCE01015.this.gnoFilter[1] = 0.0f;
            SCE01015.this.gnoFilter[2] = 0.0f;
            SCE01015.this.gnoFilter[3] = 0.0f;
            SCE01015.this.shion2.setFilter(0);
            SCE01015.this.shion2.setFilterParam(SCE01015.this.gnoFilter);
            this.mtn(257, 0, 1, 1, 2048, 1.0f, true);
        }

        void init() {
            this.init(0x1000001, 0.4f, -2.0f, 3.0f, 180.0f);
            this.renderCommand(22);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }
    }

    class Andrew
            extends Chr {
        Chr face;

        Andrew() {
        }

        void act() {
            this.setShadow(0, 0);
            this.mtn(257, 0, 1.0f, true);
        }

        void act10() {
            this.setTranslate(-0.66f, -2.0f, 0.57f);
            this.setRotate(0.0f, 24.12f, 0.0f);
            this.mtn(273, 0, 1.0f, true);
        }

        void act11() {
            this.setTranslate(-0.03f, -2.0f, 1.96f);
            this.setRotate(0.0f, 24.12f, 0.0f);
            this.mtn(275, 0, 0.98f, true);
        }

        void act12() {
            this.mtn(279, 0, 0.98f, true);
        }

        void act14() {
            this.setTranslate(-0.17f, -2.0f, 2.22f);
            this.setRotate(0.0f, 102.0f, 0.0f);
            this.mtn(283, 2048, 0.9f, true);
        }

        void act16() {
            this.setTranslate(-0.21f, -2.0f, 2.29f);
            this.setRotate(0.0f, -38.0f, 0.0f);
            SCE01015.this.army_phone.setParent(SCE01015.this.andrew, 72);
            this.mtn(284, 0, 1.0f, true);
        }

        void act17() {
            this.setTranslate(-0.91f, -2.0f, 3.62f);
            this.setRotate(0.0f, -26.0f, 0.0f);
            this.mtn(288, 8, 1.0f, false);
            this.move(30, -1.7f, 5.0f, true);
            this.setVisible(false);
        }

        void act2() {
            this.mtn(260, 0, 0.95f, true);
        }

        void act4() {
            this.setRotate(0.0f, 50.0f, 0.0f);
            this.mtn(262, 0, 1.0f, true);
        }

        void act5() {
            this.mtn(265, 0, 1.0f, true);
        }

        void act6() {
            this.setTranslate(-1.02f, -2.0f, 1.51f);
            this.setRotate(0.0f, 179.0f, 0.0f);
            this.mtn(266, 0, 0.85f, true);
        }

        void act8() {
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.rotY(1, SCE01015.this.shion, false);
        }

        void act9() {
            this.setTranslate(-0.99f, -2.0f, -0.1f);
            this.setRotate(0.0f, 84.12f, 0.0f);
            this.mtn(272, 0, 159, 1, 0, 1.0f, true);
        }

        void init() {
            this.init(0x1000117, -0.8f, -2.0f, 2.0f, 160.0f);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }
    }

    class Andrew2
            extends Chr {
        Chr face;

        Andrew2() {
        }

        void act() {
            this.setShadow(0, 0);
            this.setSymmetryY(1);
            this.mtn(257, 10240, 1.0f, true);
        }

        void act16() {
            this.setTranslate(-0.21f, -2.0f, 2.29f);
            this.setRotate(0.0f, -38.0f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(285, 2048, 1.0f, true);
        }

        void act2() {
            this.setSymmetryY(1);
            this.mtn(257, 0, 1, 1, 2048, 1.0f, true);
        }

        void act6() {
            this.setTranslate(-1.05f, -2.0f, 1.51f);
            this.setRotate(0.0f, 178.0f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(266, 10240, 0.85f, true);
        }

        void act8() {
            this.setSymmetryY(1);
            this.mtn(257, 0, 1, 1, 2048, 1.0f, true);
        }

        void init() {
            this.init(0x1000117, -0.8f, -2.0f, 2.0f, 160.0f);
            this.renderCommand(22);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }
    }

    class Moriyama
            extends Chr {
        Chr face;

        Moriyama() {
        }

        void act12() {
            this.setShadow(0, 0);
            this.setVisible(true);
            this.setTranslate(2.3f, -2.0f, 5.55f);
            this.setRotate(0.0f, 189.14f, 0.0f);
            this.mtn(277, 0, 0.85f, true);
        }

        void act13() {
            this.setTranslate(1.01f, -2.0f, 1.86f);
            this.setRotate(0.0f, 236.14f, 0.0f);
            this.mtn(282, 0, 1.0f, true);
            this.mtn(282, 0, -1.0f, true);
        }

        void act16() {
            this.setTranslate(1.01f, -2.0f, 1.86f);
            this.setRotate(0.0f, 239.14f, 0.0f);
            this.mtn(286, 0, 0.95f, true);
        }

        void act17() {
            this.mtn(290, 0, 1.0f, true);
            this.mtn(290, 0, -0.2f, true);
        }

        void act18() {
            this.setTranslate(1.01f, -2.0f, 1.86f);
            this.setRotate(0.0f, 298.14f, 0.0f);
            this.mtn(291, 0, 1.0f, true);
            this.mtn(291, 0, -1.0f, true);
        }

        void act20() {
            this.setTranslate(0.95f, -2.0f, 1.8f);
            this.setRotate(0.0f, 345.14f, 0.0f);
            this.mtn(294, 0, 1.0f, true);
            this.mtn(294, 0, -1.0f, true);
        }

        void act21() {
            this.mtn(297, 0, 1.0f, true);
        }

        void init() {
            this.init(16777518, 2.8f, -0.7f, 6.3f, 180.0f);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }

        void init2() {
            this.setVisible(false);
        }
    }

    class Moriyama2
            extends Chr {
        Chr face;

        Moriyama2() {
        }

        void act12() {
            this.setShadow(0, 0);
            this.setVisible(true);
            this.setTranslate(2.3f, -2.0f, 5.55f);
            this.setRotate(0.0f, 189.14f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(277, 0, 0.85f, true);
        }

        void act21() {
            this.setTranslate(0.95f, -2.0f, 1.8f);
            this.setRotate(0.0f, 345.14f, 0.0f);
            this.setSymmetryY(1);
            this.mtn(297, 0, 1.0f, true);
        }

        void init() {
            this.init(16777518, 2.8f, -0.7f, 6.3f, 180.0f);
            this.renderCommand(22);
            this.renderCommand(512);
            this.face = this.getChild(0x1000000);
        }

        void init2() {
            this.setVisible(false);
        }
    }

    class Ob_op
            extends Chr {
        Ob_op() {
        }

        void idou() {
            this.setTranslate(-5.25f, 0.1f, 16.75f);
            this.setRotate(0.0f, -135.0f, 0.0f);
        }

        void init() {
            this.mtn(299, 8, 1.0f, true);
            this.renderCommand(512);
        }

        void init2() {
            this.mtn(299, 8, 1.0f, true);
        }

        void kaeru() {
            this.setTranslate(2.55f, -2.25f, -3.55f);
            this.setRotate(0.0f, 135.0f, 0.0f);
        }

        void m_uti() {
            this.mtn(301, 8, 1.0f, true);
        }

        void w_uti() {
            this.mtn(300, 8, 1.0f, true);
        }
    }

    class M
            extends Unit {
        int x;
        int y;

        M() {
        }

        void init() {
            this.init(24613, -0.07f, 0.56f, -1.59f, 0.0f);
            this.setArgs(0, 0.0f, 0.0f, 3.0f, 2.25f);
            this.setArgs(1, 20089, 0, 128, 112);
            this.setArgs(2, 72, 0, 5, 1);
            this.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
            this.signal(1);
        }

        void moni_d() {
            Spline spline = Spline.create();
            float[] fArray = new float[12];
            fArray[0] = 1.0f;
            fArray[1] = 1.0f;
            fArray[2] = 1.0f;
            fArray[3] = 1.0f;
            fArray[4] = 75.0f;
            fArray[5] = 1.05f;
            fArray[6] = 0.02f;
            fArray[7] = 1.0f;
            fArray[8] = 80.0f;
            float[] fArray2 = fArray;
            spline.setCtrlVertex(fArray2, 0, 3, 80);
            this.scale(spline, false);
            this.x = 0;
            while (this.x < 58) {
                this.y = this.x * this.x / 32;
                this.setArgs(2, 101 - this.y, 0, 15, -5);
                System.sleep(1);
                ++this.x;
            }
        }

        void visi_f() {
            this.setVisible(false);
        }

        void visi_t() {
            this.setVisible(true);
        }
    }

    class Navy
            extends Unit {
        float starZ;

        public Navy(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void army_1() {
            this.setVisible(true);
            float f = 14.3f;
            int n = 0;
            while (n < 1000) {
                this.setTranslate(9.0f, -17.0f, f);
                if (n < 1000) {
                    f += 0.001f;
                }
                System.sleep(1);
                ++n;
            }
        }

        void army_1_act2() {
            this.setTranslate(-3.1f, -17.0f, 6.8f);
        }

        void mobj_133_act() {
            this.starZ = 0.0f;
            this.setTranslate(0.0f, 0.0f, 0.0f);
            float f = 0.0f;
            while (f < 1000.0f) {
                this.setTranslate(0.0f, 0.0f, f);
                System.sleep(1);
                f += 1.0f;
            }
        }
    }

    class Mapunits
            extends Unit {
        float starZ;
        int starI;

        Mapunits() {
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.02f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.02f);
                System.sleep(1);
            }
        }

        void scope_rotate1() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            this.setTranslate(0.0f, 0.0f, 0.0f);
            block0:
            while (true) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f = f % 7.0f + (f - (float) ((int) f));
                f2 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = f2 % 60.0f + (f2 - (float) ((int) f2));
                while (true) {
                    if (!(f2 > 0.0f)) continue block0;
                    this.setTranslate(-Math.cos(Math.toRadians(f3 += f / -15.0f)) * 1.0E-5f, Math.sin(Math.toRadians(f3)) * 1.0E-5f, this.pz);
                    this.setRotate(this.rx, this.ry, -f3);
                    System.sleep(1);
                    f2 -= 1.0f;
                }
                break;
            }
        }

        void scope_rotate2() {
            float f = 0.8f;
            this.setTranslate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setTranslate(-Math.cos(Math.toRadians(f)) * 0.001f, Math.sin(Math.toRadians(f)) * 0.001f, this.pz);
                this.setRotate(this.rx, this.ry, -f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void scope_rotate3() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            this.setTranslate(0.0f, 0.0f, 0.0f);
            block0:
            while (true) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f = f % 7.0f + (f - (float) ((int) f));
                f2 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = f2 % 60.0f + (f2 - (float) ((int) f2));
                while (true) {
                    if (!(f2 > 0.0f)) continue block0;
                    this.setTranslate(-Math.cos(Math.toRadians(f3 += f / 15.0f)) * 1.0E-4f, Math.sin(Math.toRadians(f3)) * 1.0E-4f, this.pz);
                    this.setRotate(this.rx, this.ry, -f3);
                    System.sleep(1);
                    f2 -= 1.0f;
                }
                break;
            }
        }

        void scope_rotate4() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            this.setTranslate(0.0f, 0.0f, 0.0f);
            block0:
            while (true) {
                f = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f = f % 7.0f + (f - (float) ((int) f));
                f2 = (float) (Math.random() & 0x7FFFFFF) / 10000.0f;
                f2 = f2 % 60.0f + (f2 - (float) ((int) f2));
                while (true) {
                    if (!(f2 > 0.0f)) continue block0;
                    this.setTranslate(Math.cos(Math.toRadians(f3 += f / -20.0f)) * 1.0E-4f, -Math.sin(Math.toRadians(f3)) * 1.0E-4f, this.pz);
                    this.setRotate(this.rx, this.ry, -f3);
                    System.sleep(1);
                    f2 -= 1.0f;
                }
                break;
            }
        }

        void sonar_ring_L() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void sonar_ring_Large() {
            float f = 0.07f;
            this.setTranslate(0.0f, 0.0f, -9.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 9.0f, this.py, -9.0f + Math.sin(Math.toRadians(f)) * 9.0f);
                this.setRotateY(f);
                f += 0.07f;
                System.sleep(1);
            }
        }

        void sonar_ring_R() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(-Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, -f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void star() {
            this.starZ = 0.0f;
            this.starI = 0;
            while (this.starI < 1000) {
                this.setTranslate(0.0f, 0.0f, this.starZ);
                if (this.starI <= 1000) {
                    this.starZ += 0.01f;
                }
                ++this.starI;
                System.sleep(1);
            }
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

