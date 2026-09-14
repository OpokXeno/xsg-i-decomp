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
import xeno.map.MC_KAS05_PRJ;
import xeno.map.MC_KAS31_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE03017
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03017,
        MC_KAS31_PRJ,
        MC_KAS05_PRJ,
        JNT_Human,
        JNT_Accesories,
        FLSshion_h,
        FLSelly,
        FLSshion_ch,
        FLSshi_dad {
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    FaceChr shion = new FaceChr(30);
    FaceChr shion_ch = new FaceChr(8);
    FaceChr shi_dad = new FaceChr(297);
    FaceChr elly = new FaceChr(274);
    Chrs mobj116 = new Chrs(20597);
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    float[] shadowFilter = new float[4];
    Units target = new Units(24577);
    Effect eft1;
    Effect eft2;
    Effect eft3;
    Effect eft4;
    Effect eft5;
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE03017() {
    }

    void MotionPack_srv() {
        this.MotionPack_srv(1000000);
    }

    void MotionPack_srv(int n) {
        this.tool.Timechk_srv_totaltime = 631;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.13f);
                    this.light.setColor(1, 0.42f, 0.38f, 0.33f);
                    this.light.setDirection2(1, 0.856f, 0.027f, -0.516f);
                    this.light.setColor(2, 0.18f, 0.2f, 0.19f);
                    this.light.setDirection2(2, 0.278f, 0.705f, 0.652f);
                    this.light.setColor(3, 0.09f, 0.12f, 0.13f);
                    this.light.setDirection2(3, -0.938f, -0.346f, -0.024f);
                    Stage.setColor(0.96f, 0.96f, 0.96f);
                    Runtime.setDefocusQuick(0, 1, 84072, 1);
                    Runtime.setDefocusQuick(1, 1, 84072, 1);
                    Runtime.setDefocusQuick(2, 1, 84072, 1);
                    this.tool.FACE(this.shion.face, 12);
                    this.target.setTranslate(-7.2f, -0.07f, -51.6f);
                    this.target.setRotate(0.0f, 0.0f, 0.0f);
                    this.target.setScale(0.55f, 0.55f, 0.55f);
                    System.sleep(89);
                    int[] nArray = new int[5];
                    nArray[0] = 0x8000000;
                    int[] nArray2 = nArray;
                    Runtime.setDefocus(0, 10, nArray2);
                    break;
                }
                case 91: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.13f);
                    this.light.setColor(1, 0.42f, 0.38f, 0.33f);
                    this.light.setDirection2(1, 0.856f, 0.027f, -0.516f);
                    this.light.setColor(2, 0.18f, 0.2f, 0.19f);
                    this.light.setDirection2(2, 0.278f, 0.705f, 0.652f);
                    this.light.setColor(3, 0.09f, 0.12f, 0.13f);
                    this.light.setDirection2(3, -0.938f, -0.346f, -0.024f);
                    System.sleep(89);
                    int[] nArray = new int[5];
                    nArray[0] = 0x8000000;
                    int[] nArray3 = nArray;
                    Runtime.setDefocus(0, 10, nArray3);
                    break;
                }
                case 181: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.13f);
                    this.light.setColor(1, 0.42f, 0.38f, 0.33f);
                    this.light.setDirection2(1, 0.856f, 0.027f, -0.516f);
                    this.light.setColor(2, 0.18f, 0.2f, 0.19f);
                    this.light.setDirection2(2, 0.278f, 0.705f, 0.652f);
                    this.light.setColor(3, 0.09f, 0.12f, 0.13f);
                    this.light.setDirection2(3, -0.938f, -0.346f, -0.024f);
                    System.sleep(60);
                    this.tool.SoundstreamPlay(317001);
                    this.tool.FACE(25, this.shion.face, 11);
                    break;
                }
                case 271: {
                    this.tool.Timechk_CutChange();
                    this.target.setTranslate(-7.2f, 0.23f, -38.5f);
                    this.target.setRotate(0.0f, 50.0f, 0.0f);
                    this.target.setScale(1.65f, 1.65f, 1.65f);
                    this.map_set(1204, true);
                    Runtime.setDefocus(0, 0, null);
                    break;
                }
                case 421: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.12f);
                    this.light.setColor(1, 0.39f, 0.35f, 0.3f);
                    this.light.setDirection2(1, 0.206f, 0.674f, -0.71f);
                    this.light.setColor(2, 0.1f, 0.12f, 0.11f);
                    this.light.setDirection2(2, -0.493f, -0.792f, 0.36f);
                    this.light.setColor(3, 0.1f, 0.13f, 0.14f);
                    this.light.setDirection2(3, 0.327f, -0.318f, 0.89f);
                    this.target.setTranslate(-7.2f, -0.07f, -51.6f);
                    this.target.setRotate(0.0f, 0.0f, 0.0f);
                    this.target.setScale(0.55f, 0.55f, 0.55f);
                    this.shion.face.mtn(262, 0, 300, 8, 0, 1.0f, true);
                    this.shion.face.start(4, null);
                    this.tool.SoundstreamPlay(317002);
                    break;
                }
                case 481: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.12f);
                    this.light.setColor(1, 0.42f, 0.38f, 0.33f);
                    this.light.setDirection2(1, 0.477f, 0.809f, -0.343f);
                    this.light.setColor(2, 0.1f, 0.12f, 0.11f);
                    this.light.setDirection2(2, -0.493f, -0.792f, 0.36f);
                    this.light.setColor(3, 0.1f, 0.13f, 0.14f);
                    this.light.setDirection2(3, 0.327f, -0.318f, 0.89f);
                    this.eft1.disp(false);
                    this.eft2.disp(false);
                    this.eft3.disp(false);
                    this.eft4.disp(false);
                    this.eft5.disp(false);
                    break;
                }
                case 631: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.1f, 0.1f, 0.12f);
                    this.light.setColor(1, 0.4f, 0.36f, 0.31f);
                    this.light.setDirection2(1, 0.716f, 0.204f, -0.668f);
                    this.light.setColor(2, 0.06f, 0.08f, 0.07f);
                    this.light.setDirection2(2, -0.611f, -0.515f, 0.601f);
                    this.light.setColor(3, 0.16f, 0.19f, 0.2f);
                    this.light.setDirection2(3, 0.121f, -0.0f, 0.993f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.eft1.disp(false);
                    this.eft2.disp(false);
                    this.eft3.disp(false);
                    this.eft4.disp(false);
                    this.eft5.disp(false);
                    this.shion.face.mtn(262, 150, 300, 8, 0, 1.0f, true);
                    this.shion.face.start(4, null);
                    int n2 = 0;
                    while (n2 != 200) {
                        Stage.setVisible(n2, false);
                        ++n2;
                    }
                    break;
                }
                case 781: {
                    this.tool.Timechk_CutChange();
                    Runtime.setLocation(1230);
                    int n2 = 0;
                    while (n2 != 200) {
                        Stage.setVisible(n2, true);
                        ++n2;
                    }
                    this.light.setColor(0, 0.11f, 0.07f, 0.1f);
                    this.light.setColor(1, 0.54f, 0.41f, 0.23f);
                    this.light.setDirection2(1, -0.831f, 0.196f, 0.52f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, 0.422f, 0.385f, 0.821f);
                    this.light.setColor(3, 0.13f, 0.14f, 0.15f);
                    this.light.setDirection2(3, 0.279f, -0.549f, -0.788f);
                    Stage.setColor(0.67f, 0.5f, 0.34f);
                    Stage.setVisible(32, false);
                    Stage.setVisible(58, false);
                    Stage.setVisible(59, false);
                    Stage.setVisible(60, false);
                    Stage.setVisible(61, false);
                    this.tool.FACE(this.shion.face, 11, 90);
                    Runtime.setDefocusQuick(0, 1, 57880, 1);
                    Runtime.setDefocusQuick(1, 1, 49688, 1);
                    Runtime.setDefocusQuick(2, 1, 41496, 1);
                    this.tool.FACE(this.shion_ch.face, 6);
                    System.sleep(320);
                    this.tool.SoundstreamPlay(317003);
                    break;
                }
                case 1201: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.48f, 0.37f, 0.1f);
                    this.light.setDirection2(1, -0.988f, 0.037f, 0.15f);
                    this.light.setColor(2, 0.19f, 0.17f, 0.13f);
                    this.light.setDirection2(2, -0.38f, 0.005f, 0.925f);
                    this.light.setColor(3, 0.08f, 0.09f, 0.1f);
                    this.light.setDirection2(3, 0.717f, -0.0f, -0.698f);
                    this.shion.setLightMode(1);
                    this.shion.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.shion.light.setColor(1, 0.72f, 0.61f, 0.34f);
                    this.shion.light.setDirection2(1, -0.988f, 0.025f, 0.15f);
                    this.shion.light.setColor(2, 0.25f, 0.23f, 0.19f);
                    this.shion.light.setDirection2(2, 0.138f, 0.0f, 0.99f);
                    this.shion.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.shion.light.setDirection2(3, 0.776f, -0.521f, -0.356f);
                    break;
                }
                case 1381: {
                    this.tool.Timechk_CutChange();
                    this.shion.setLightMode(0);
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.39f, 0.16f);
                    this.light.setDirection2(1, -0.988f, 0.025f, 0.15f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.38f, 0.005f, 0.925f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.567f, -0.59f, -0.575f);
                    break;
                }
                case 1470: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.956f, 0.0f, 0.295f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, 0.292f, 0.25f, 0.923f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.567f, -0.589f, -0.575f);
                    break;
                }
                case 1560: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.39f, 0.16f);
                    this.light.setDirection2(1, -0.988f, 0.025f, 0.15f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.38f, 0.005f, 0.925f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.567f, -0.59f, -0.575f);
                    break;
                }
                case 1665: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.989f, 0.0f, 0.15f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.15f, 0.444f, 0.883f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.839f, -0.198f, -0.508f);
                    this.tool.SoundstreamPlay(317005);
                    break;
                }
                case 1710: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.39f, 0.16f);
                    this.light.setDirection2(1, -0.988f, 0.025f, 0.15f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, -0.38f, 0.005f, 0.925f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, 0.567f, -0.59f, -0.575f);
                    this.shion_ch.setMotionFlags(-1073741824, false);
                    System.sleep(40);
                    break;
                }
                case 1785: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.48f, 0.35f, 0.1f);
                    this.light.setDirection2(1, -0.988f, 0.037f, 0.15f);
                    this.light.setColor(2, 0.19f, 0.17f, 0.13f);
                    this.light.setDirection2(2, -0.38f, 0.005f, 0.925f);
                    this.light.setColor(3, 0.13f, 0.14f, 0.15f);
                    this.light.setDirection2(3, 0.909f, -0.253f, -0.33f);
                    this.shion.setLightMode(1);
                    this.shion.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.shion.light.setColor(1, 0.72f, 0.61f, 0.34f);
                    this.shion.light.setDirection2(1, -0.988f, 0.025f, 0.15f);
                    this.shion.light.setColor(2, 0.25f, 0.23f, 0.19f);
                    this.shion.light.setDirection2(2, 0.138f, 0.0f, 0.99f);
                    this.shion.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.shion.light.setDirection2(3, 0.776f, -0.521f, -0.356f);
                    break;
                }
                case 1890: {
                    this.tool.Timechk_CutChange();
                    this.shion.setLightMode(0);
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.41f, 0.16f);
                    this.light.setDirection2(1, -0.787f, 0.19f, 0.587f);
                    this.light.setColor(2, 0.17f, 0.15f, 0.11f);
                    this.light.setDirection2(2, 0.223f, 0.34f, 0.914f);
                    this.light.setColor(3, 0.15f, 0.16f, 0.17f);
                    this.light.setDirection2(3, 0.691f, -0.001f, -0.723f);
                    this.shion.setMotionFlags(0x40000000, true);
                    this.shion_ch.look_default();
                    System.sleep(30);
                    this.tool.SoundstreamPlay(317006);
                    this.tool._MSG(60, "子供シオン", "Daddy!");
                    this.tool.FACE(this.shion_ch.face, 3);
                    break;
                }
                case 1964: {
                    System.println("father cns!!");
                    this.shi_dad.mtn(259, 1965, 3950, 8, 0x40000008, 1.0f, false);
                    this.shi_dad.start(4, null);
                    this.shi_dad.setVisible(true);
                    break;
                }
                case 1965: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.48f, 0.33f, 0.1f);
                    this.light.setDirection2(1, -0.848f, 0.197f, 0.492f);
                    this.light.setColor(2, 0.12f, 0.1f, 0.06f);
                    this.light.setDirection2(2, 0.138f, 0.496f, 0.857f);
                    this.light.setColor(3, 0.1f, 0.11f, 0.13f);
                    this.light.setDirection2(3, 0.675f, -0.738f, -0.001f);
                    System.sleep(40);
                    this.tool.SoundstreamPlay(317007);
                    this.tool._MSG(90, "シオン", "Daddy...?");
                    this.tool.FACE(39, this.shion.face, 11);
                    break;
                }
                case 2045: {
                    this.tool.Timechk_CutChange();
                    this.shi_dad.start(5, null);
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.977f, 0.195f, 0.089f);
                    this.light.setColor(2, 0.16f, 0.14f, 0.12f);
                    this.light.setDirection2(2, 0.845f, 0.516f, -0.139f);
                    this.light.setColor(3, 0.17f, 0.18f, 0.18f);
                    this.light.setDirection2(3, -0.084f, -0.381f, -0.921f);
                    break;
                }
                case 2065: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.44f, 0.3f, 0.06f);
                    this.light.setDirection2(1, -0.913f, 0.0f, 0.408f);
                    this.light.setColor(2, 0.12f, 0.1f, 0.06f);
                    this.light.setDirection2(2, -0.009f, 0.475f, 0.88f);
                    this.light.setColor(3, 0.08f, 0.06f, 0.08f);
                    this.light.setDirection2(3, 0.835f, -0.549f, 0.043f);
                    this.shion.setMotionFlags(0x40000000, false);
                    this.tool.SoundstreamPlay(317008);
                    this.tool.MSG(90, this.shi_dad.face, 1, "All right, Shion...let's go home.");
                    this.tool.SoundstreamPlay(317009);
                    this.tool.MSG(50, this.shion_ch.face, 3, "We're not...");
                    System.sleep(20);
                    this.tool.SoundstreamPlay(317010);
                    this.tool.MSG(30, this.shion_ch.face, 3, "...going to see Mom?");
                    this.tool.SoundstreamPlay(317011);
                    this.tool.MSG(70, this.shi_dad.face, 1, "No...not today...");
                    System.sleep(20);
                    this.tool.SoundstreamPlay(317012);
                    this.tool.MSG(60, this.shi_dad.face, 1, "We'll visit her tomorrow,");
                    System.sleep(20);
                    this.tool.SoundstreamPlay(317013);
                    this.tool.MSG(30, this.shi_dad.face, 1, "okay?");
                    this.tool.SoundstreamPlay(317014);
                    this.tool.MSG(20, this.shion_ch.face, 1, "...Okay.");
                    break;
                }
                case 2690: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.44f, 0.3f, 0.06f);
                    this.light.setDirection2(1, -0.913f, 0.0f, 0.408f);
                    this.light.setColor(2, 0.12f, 0.1f, 0.06f);
                    this.light.setDirection2(2, -0.009f, 0.475f, 0.88f);
                    this.light.setColor(3, 0.08f, 0.06f, 0.08f);
                    this.light.setDirection2(3, 0.835f, -0.549f, 0.043f);
                    System.sleep(30);
                    this.tool.SoundstreamPlay(317015);
                    this.tool.MSG(50, this.shion.face, 11, "Wait...");
                    System.sleep(30);
                    this.tool.SoundstreamPlay(317016);
                    this.tool.MSG(30, this.shion.face, 11, "Don't go...");
                    break;
                }
                case 2900: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.955f, 0.031f, 0.295f);
                    this.light.setColor(2, 0.12f, 0.1f, 0.08f);
                    this.light.setDirection2(2, 0.283f, 0.589f, -0.757f);
                    this.light.setColor(3, 0.14f, 0.15f, 0.15f);
                    this.light.setDirection2(3, 0.567f, -0.59f, -0.575f);
                    this.tool.SoundstreamPlay(317017);
                    this.tool.MSG(40, "ネピリム（OFF）", "You cannot go...");
                    break;
                }
                case 3035: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.879f, 0.0f, 0.477f);
                    this.light.setColor(2, 0.22f, 0.2f, 0.16f);
                    this.light.setDirection2(2, -0.041f, 0.913f, 0.406f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, -0.329f, -0.453f, -0.828f);
                    break;
                }
                case 3230: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.879f, 0.0f, 0.477f);
                    this.light.setColor(2, 0.22f, 0.2f, 0.16f);
                    this.light.setDirection2(2, -0.041f, 0.913f, 0.406f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, -0.329f, -0.453f, -0.828f);
                    this.tool.SoundstreamPlay(317018);
                    this.tool.MSG(190, this.elly.face, 1, "That's right. This was the last day\nyou spent together with your father.");
                    break;
                }
                case 3425: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.955f, 0.031f, 0.295f);
                    this.light.setColor(2, 0.17f, 0.15f, 0.11f);
                    this.light.setDirection2(2, 0.472f, 0.027f, 0.881f);
                    this.light.setColor(3, 0.12f, 0.13f, 0.14f);
                    this.light.setDirection2(3, 0.798f, -0.571f, -0.195f);
                    this.shion.setMotionFlags(0x40000000, true);
                    break;
                }
                case 3500: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.955f, 0.031f, 0.295f);
                    this.light.setColor(2, 0.21f, 0.19f, 0.15f);
                    this.light.setDirection2(2, 0.472f, 0.0f, 0.882f);
                    this.light.setColor(3, 0.12f, 0.13f, 0.14f);
                    this.light.setDirection2(3, 0.798f, -0.571f, -0.195f);
                    this.tool.SoundstreamPlay(317019);
                    this.tool.MSG(89, this.elly.face, 1, "I've been waiting a long time for you...");
                    break;
                }
                case 3590: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.955f, 0.031f, 0.295f);
                    this.light.setColor(2, 0.23f, 0.21f, 0.17f);
                    this.light.setDirection2(2, 0.85f, 0.0f, 0.527f);
                    this.light.setColor(3, 0.09f, 0.1f, 0.11f);
                    this.light.setDirection2(3, 0.417f, -0.649f, -0.636f);
                    this.elly.setMotionFlags(0x40000000, false);
                    break;
                }
                case 3635: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.07f, 0.03f, 0.06f);
                    this.light.setColor(1, 0.54f, 0.43f, 0.16f);
                    this.light.setDirection2(1, -0.955f, 0.031f, 0.295f);
                    this.light.setColor(2, 0.21f, 0.19f, 0.15f);
                    this.light.setDirection2(2, 0.472f, 0.0f, 0.882f);
                    this.light.setColor(3, 0.12f, 0.13f, 0.14f);
                    this.light.setDirection2(3, 0.798f, -0.571f, -0.195f);
                    break;
                }
                case 3710: {
                    this.tool.Timechk_CutChange();
                    this.light.setColor(0, 0.06f, 0.03f, 0.05f);
                    this.light.setColor(1, 0.54f, 0.4f, 0.16f);
                    this.light.setDirection2(1, -0.879f, 0.0f, 0.477f);
                    this.light.setColor(2, 0.22f, 0.2f, 0.16f);
                    this.light.setDirection2(2, -0.041f, 0.913f, 0.406f);
                    this.light.setColor(3, 0.16f, 0.17f, 0.18f);
                    this.light.setDirection2(3, -0.329f, -0.453f, -0.828f);
                    this.elly.setMotionFlags(0x40000000, true);
                    System.sleep(100);
                    this.tool.SoundstreamPlay(317020);
                    this.tool.MSG(100, this.elly.face, 1, "We have much to talk about.");
                }
            }
            System.sleep(1);
        }
    }

    void PCextends_end(int n) {
        this.TotalCutTime = this.tool.Timechk_srv_totaltime - this.TotalCutTime;
        this.tool.Timechk_srv_totaltime = this.NextPCStart;
        this.pc.init(1, 0, this.NextPCStart + 1, n, 1.0f);
        this.pc.start();
        System.println("------------ＭＰ速度変更は正常に終了しました。");
        Runtime.setRegister(0, this.TotalCutTime);
        Runtime.setRegister(1, this.tool.CutNo);
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
        this.TotalCutTime = this.tool.Timechk_srv_totaltime;
        this.BaseCutTime = n2 - n;
        this.pc.init(1, 0, n, n2, f);
        this.pc.start();
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        Runtime.setRegister(2, f);
        System.println("------------ＭＰの速度を変更します(/[$0]-/[$1]) ×/[#2]");
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
        System.println("XEVEFLAG:EV03017_F");
        Runtime.setFlags(328, 1, 1);
        System.println("XEVEJNAME:SCE03018");
        Runtime.jumpEvent(3180);
    }

    public void cleanupOriginal() {
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(1230);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam1.change();
        this.pc = PlayControl.create();
        this.pc.loadCamera("c03s17.cam");
        this.eft1 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1.setScale(1.0f, 1.0f, 1.0f);
        this.eft1.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft1.setRotate(0.0f, 90.0f, 0.0f);
        this.eft1.disp(true);
        this.eft2 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft2.setScale(1.0f, 1.0f, 1.0f);
        this.eft2.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft2.setRotate(0.0f, 180.0f, 0.0f);
        this.eft2.disp(true);
        this.eft3 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft3.setScale(1.0f, 1.0f, 1.0f);
        this.eft3.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft3.setRotate(0.0f, 270.0f, 0.0f);
        this.eft3.disp(true);
        this.eft4 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft4.setScale(1.0f, 1.0f, 1.0f);
        this.eft4.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft4.setRotate(0.0f, 45.0f, 0.0f);
        this.eft4.disp(true);
        this.eft5 = new Effect(1550, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft5.setScale(1.0f, 1.0f, 1.0f);
        this.eft5.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft5.setRotate(0.0f, 80.0f, 0.0f);
        this.eft5.disp(true);
        this.eft1.setCaster(this.target);
        this.eft2.setCaster(this.target);
        this.eft3.setCaster(this.target);
        this.eft4.setCaster(this.target);
        this.eft5.setCaster(this.target);
        this.target.setVisible(false);
    }

    void initialize() {
    }

    static void main() {
    }

    void map_set(int n, boolean bl) {
        int n2 = 0;
        if (n == 1230) {
            while (n2 <= 70) {
                Stage.setVisible(n2, bl);
                ++n2;
            }
        }
        if (n == 1204) {
            while (n2 <= 40) {
                Stage.setVisible(n2, bl);
                ++n2;
            }
        }
    }

    void play() {
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.loadarc(this.shion_ch.face, "FLSshion_ch.fpk");
        this.tool.loadarc(this.shi_dad.face, "FLSshi_dad.fpk");
        this.tool.loadarc(this.elly.face, "FLSelly.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.SoundstreamDebug_init(999999);
        this.shion.renderCommand(512);
        this.shi_dad.renderCommand(534);
        this.elly.renderCommand(534);
        this.shion_ch.renderCommand(534);
        this.shion.setShadow(0, 0);
        this.elly.mtn(257, 631, 3950, 8, -1073741816, 1.0f, true);
        this.mobj116.mtn(258, 631, 3950, 8, 8, 1.0f, true);
        this.shion.mtn(260, 631, 3950, 8, 8, 1.0f, true);
        this.shion_ch.mtn(261, 631, 3950, 8, -1073741816, 1.0f, true);
        this.shi_dad.setVisible(false);
        this.elly.start(5, null);
        this.mobj116.start(5, null);
        this.shion.start(5, null);
        this.shion_ch.start(5, null);
        this.pc.init(1, 0, 631, 3950, 1.0f);
        this.pc.start();
        Runtime.mpeg2("3017_1");
        Sound.streamPlay(1390035, 48000);
        this.MotionPack_srv(3950);
        this.tool.Timechk_SceneEnd();
    }

    class Units
            extends Unit {
        public Units(int n) {
            this.init(n);
        }
    }

    class Chrs
            extends Chr {
        public Chrs(int n) {
            this.init(n);
            this.setShadow(0, 0);
        }
    }

    class FaceChr
            extends Chr {
        Chr face;
        int Mtnno;
        int Mtn_start;

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(7, 40);
        }
    }

    class Monitor
            extends Unit {
        float sz;
        int alpha;

        public Monitor() {
            this.init(24613);
        }

        public Monitor(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void moveloop() {
            float f = 0.0f;
            while (true) {
                this.setTranslate(1.0f + Math.sin(f) * 0.02f, 0.6f + Math.cos(f) * 0.02f, -2.23f);
                f += 0.1f;
                System.sleep(1);
            }
        }

        void off_a() {
            this.alpha = 96;
            while (this.alpha <= 0) {
                this.alpha -= 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 0, 0, 0, -1);
        }

        void off_s() {
            this.setScale(0.0f, 0.0f, 0.0f);
        }

        void on1() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on2() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on3() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on4() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
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
}

