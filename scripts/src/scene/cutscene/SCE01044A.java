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
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01044A
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        JNT_Human,
        MC_ELS01_PRJ,
        Pack01044,
        FLSmatehws_h,
        FLSchaos_h,
        FLSshion_h,
        FLStonny_h,
        FLSkosmos,
        FLSallen,
        FLSandrew {
    Characters mathews_h;
    Characters allen;
    Characters kosmos;
    Characters tonny_h;
    Characters hammer;
    Characters shion_h;
    Characters andrew;
    Characters chaos_h;
    Characters kosmos2;
    Chr_unit gnosis;
    Chr_unit elsa;
    Chr_unit space;
    Unit spice;
    Unit gun;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    int cut_length = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    Effect FadeIn;
    Effect FadeOut;
    Effect elsafx;
    float[] gnoFilter = new float[]{0.74f, 76.0f, 37.0f, 0.72f};
    MAPUnit doorL;
    MAPUnit doorR;
    static final int toSpace = 0;
    static final int toBridge = 1;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Unit Cseat2;
    Unit Cseat3;
    Unit Cseat4;
    Unit Cseat6;
    Unit Cseat7;
    MAPUnit Cseat1;
    MAPUnit Cseat5;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01044A() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.light.setColor(0, 0.37f, 0.37f, 0.37f);
        this.light.setColor(1, 0.73f, 0.73f, 0.73f);
        this.light.setDirection2(1, 0.494f, 0.249f, 0.833f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.702f, -0.001f, -0.712f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.change();
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.241f, 0.491f, -0.837f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.898f, 0.404f, 0.175f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.584f, -0.66f, -0.473f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 66880, 1);
        this.cam1.setTranslate(-1.56f, 1.11f, 9.42f);
        this.cam1.setRotate(3.5f, -481.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.303f, 0.57f, 0.764f);
        this.light.setColor(2, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(2, -0.739f, 0.572f, 0.356f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.571f, -0.813f, 0.111f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 20880, 1);
        this.cam1.setTranslate(-2.53f, 1.73f, 11.05f);
        this.cam1.setRotate(-7.5f, -27.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.431f, 0.506f, -0.747f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.989f, 0.141f, 0.054f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.594f, -0.541f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 42880, 1);
        this.cam1.setTranslate(-1.38f, 0.77f, 2.03f);
        this.cam1.setRotate(1.5f, -497.38f, 0.0f);
        this.cam1.setFov(30.0f);
        int n = 0;
        while (n < 30) {
            this.shion_h.getTranslate();
            this.shion_h.setTranslate(this.shion_h.px, -0.3f - 0.006666667f * (float) n, this.shion_h.pz);
            System.sleep(1);
            ++n;
        }
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.431f, 0.506f, -0.747f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.928f, 0.242f, 0.283f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.594f, -0.541f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 179380, 1);
        this.cam1.setTranslate(-0.11f, 1.06f, 3.17f);
        this.cam1.setRotate(-8.5f, -526.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.583f, 0.535f, 0.611f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.572f, 0.679f, 0.46f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.294f, -0.835f, 0.465f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(0.02f, 0.91f, 4.26f);
        this.cam1.setRotate(1.0f, -360.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.431f, 0.506f, -0.747f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.928f, 0.242f, 0.283f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.594f, -0.541f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 57880, 1);
        this.cam1.setTranslate(-1.09f, 1.16f, 2.42f);
        this.cam1.setRotate(-10.0f, -493.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.228f, 0.291f, -0.929f);
        this.light.setColor(2, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(2, -0.742f, 0.51f, 0.435f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.734f, -0.578f, -0.356f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 82880, 1);
        this.cam1.setTranslate(-0.85f, 1.03f, 3.45f);
        this.cam1.setRotate(-3.5f, -475.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.246f, 0.359f, -0.901f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.742f, 0.51f, 0.435f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.769f, -0.477f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 46880, 1);
        this.cam1.setTranslate(-1.48f, 0.88f, 2.43f);
        this.cam1.setRotate(0.0f, -485.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.121f, 0.621f, -0.774f);
        this.light.setColor(2, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(2, -0.742f, 0.51f, 0.435f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.769f, -0.477f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.17f, 1.05f, 3.49f);
        this.cam1.setRotate(-3.0f, -480.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.187f, 0.7f, -0.689f);
        this.light.setColor(2, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(2, -0.742f, 0.51f, 0.435f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.769f, -0.477f, -0.426f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-0.39f, 1.11f, 2.99f);
        this.cam1.setRotate(-48.5f, -142.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.508f, 0.472f, 0.721f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.873f, 0.414f, -0.258f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.733f, -0.411f, 0.542f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.87f, 0.78f, 3.86f);
        this.cam1.setRotate(8.5f, -52.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(12);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.491f, 0.501f, -0.712f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.493f, 0.51f, -0.705f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.535f, -0.82f, -0.204f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 54880, 1);
        this.cam1.setTranslate(-0.55f, 0.96f, 2.79f);
        this.cam1.setRotate(-3.5f, 205.12f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(120);
        this.rSPL(75, -3.5f, 158.12f, 0.0f, 3);
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(1, 0.431f, 0.506f, -0.747f);
        this.light.setColor(2, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(2, -0.799f, 0.344f, 0.493f);
        this.light.setColor(3, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.594f, -0.541f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-2.48f, 1.17f, 4.3f);
        this.cam1.setRotate(-14.5f, -100.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.483f, 0.328f, -0.812f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.853f, 0.521f, -0.013f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.293f, -0.947f, -0.132f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-1.13f, 1.12f, 3.59f);
        this.cam1.setRotate(-11.0f, -198.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(15);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.351f, 0.331f, 0.876f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.829f, 0.515f, -0.219f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.699f, -0.537f, 0.472f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.79f, 0.79f, 4.03f);
        this.cam1.setRotate(9.5f, -40.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(16);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.483f, 0.307f, -0.82f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.928f, 0.242f, 0.283f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.594f, -0.541f, -0.595f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 76880, 1);
        this.cam1.setTranslate(-1.88f, 1.0f, 3.41f);
        this.cam1.setRotate(-6.5f, -125.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(17);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.476f, 0.646f, -0.597f);
        this.light.setColor(2, 0.29f, 0.29f, 0.29f);
        this.light.setDirection2(2, 0.132f, 0.142f, 0.981f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, 0.091f, -0.837f, -0.54f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.38f, 0.97f, 3.7f);
        this.cam1.setRotate(-3.5f, -223.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(18);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.227f, 0.701f, 0.676f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.798f, 0.586f, -0.142f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.625f, -0.75f, 0.215f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 47880, 1);
        this.cam1.setTranslate(-0.6f, 0.24f, 3.24f);
        this.cam1.setRotate(12.5f, -798.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(19);
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.582f, 0.607f, -0.541f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, 0.928f, 0.243f, 0.282f);
        this.light.setColor(3, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(3, 0.012f, -0.64f, -0.768f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 64880, 1);
        this.cam1.setTranslate(1.92f, 0.39f, 1.39f);
        this.cam1.setRotate(16.0f, -906.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(20);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.53f, 0.53f, 0.53f);
        this.light.setDirection2(1, -0.598f, 0.602f, 0.529f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.323f, 0.027f, -0.946f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.841f, -0.465f, -0.276f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 117880, 1);
        this.cam1.setTranslate(1.37f, 1.09f, 2.52f);
        this.cam1.setRotate(-8.0f, -803.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(21);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.164f, 0.325f, -0.931f);
        this.light.setColor(2, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(2, 0.27f, 0.523f, 0.808f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.515f, -0.83f, -0.212f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(1.96f, 0.97f, 2.2f);
        this.cam1.setRotate(-3.0f, -933.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(22);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.661f, 0.525f, 0.536f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.061f, 0.239f, -0.969f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.551f, -0.792f, -0.262f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 28880, 1);
        this.cam1.setTranslate(-0.93f, 1.1f, 1.65f);
        this.cam1.setRotate(-6.5f, -840.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(23);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.313f, 0.395f, 0.863f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.53f, 0.505f, -0.681f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.584f, -0.766f, -0.269f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosmos.setLightMode(1);
        this.shion_h.setLightMode(1);
        this.kosmos.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.kosmos.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.kosmos.light.setDirection2(1, -0.013f, 0.635f, 0.773f);
        this.kosmos.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.kosmos.light.setDirection2(2, -0.256f, 0.606f, -0.753f);
        this.kosmos.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.kosmos.light.setDirection2(3, 0.769f, -0.552f, -0.322f);
        this.shion_h.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.shion_h.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.shion_h.light.setDirection2(1, -0.013f, 0.635f, 0.773f);
        this.shion_h.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.shion_h.light.setDirection2(2, -0.256f, 0.606f, -0.753f);
        this.shion_h.light.setColor(3, 0.27f, 0.27f, 0.27f);
        this.shion_h.light.setDirection2(3, 0.769f, -0.552f, -0.322f);
        Runtime.setDefocusQuick(0, 1, 38880, 1);
        this.cam1.setTranslate(3.19f, 1.14f, 2.37f);
        this.cam1.setRotate(-9.5f, -973.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(24);
        this.kosmos.setLightMode(0);
        this.shion_h.setLightMode(0);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.542f, 0.531f, 0.651f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.393f, 0.24f, -0.887f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.846f, -0.511f, -0.15f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 82880, 1);
        this.cam1.setTranslate(0.46f, 1.11f, 2.51f);
        this.cam1.setRotate(-8.0f, -833.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(25);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.313f, 0.395f, 0.863f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.53f, 0.505f, -0.681f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.584f, -0.766f, -0.269f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kosmos.setLightMode(1);
        this.kosmos.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.kosmos.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.kosmos.light.setDirection2(1, -0.093f, 0.697f, 0.711f);
        this.kosmos.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.kosmos.light.setDirection2(2, -0.268f, 0.613f, -0.743f);
        this.kosmos.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.kosmos.light.setDirection2(3, 0.769f, -0.64f, 0.002f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 42880, 1);
        this.cam1.setTranslate(2.33f, 1.16f, 2.31f);
        this.cam1.setRotate(-7.5f, -973.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(26);
        this.kosmos.setLightMode(0);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.542f, 0.531f, 0.651f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.393f, 0.242f, -0.887f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.846f, -0.512f, -0.15f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(0.61f, 1.05f, 2.73f);
        this.cam1.setRotate(-8.5f, -826.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(28);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.618f, 0.479f, 0.624f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.845f, 0.514f, 0.15f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.391f, -0.91f, 0.14f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 73880, 1);
        this.cam1.setTranslate(0.85f, 1.0f, 4.15f);
        this.cam1.setRotate(-4.0f, -743.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(29);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.603f, 0.444f, 0.663f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.393f, 0.24f, -0.887f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.846f, -0.511f, -0.15f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(0.95f, 0.98f, 2.89f);
        this.cam1.setRotate(-5.5f, -114.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(30);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.618f, 0.479f, 0.624f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.933f, 0.331f, 0.14f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.391f, -0.91f, 0.14f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 67880, 1);
        this.cam1.setTranslate(1.13f, 1.0f, 4.26f);
        this.cam1.setRotate(-4.0f, -736.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(31);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.602f, 0.567f, -0.562f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, -0.854f, 0.445f, -0.268f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.022f, -0.794f, -0.608f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion_h.setLightMode(1);
        this.shion_h.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.shion_h.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.shion_h.light.setDirection2(1, -0.857f, 0.512f, 0.058f);
        this.shion_h.light.setColor(2, 0.31f, 0.31f, 0.31f);
        this.shion_h.light.setDirection2(2, 0.569f, 0.68f, -0.462f);
        this.shion_h.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.shion_h.light.setDirection2(3, -0.479f, -0.765f, -0.43f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(1.6f, 0.8f, 1.78f);
        this.cam1.setRotate(0.5f, -551.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 1.6f, 0.88f, 1.78f);
        this.waitCameraPlay(32);
        this.shion_h.setLightMode(0);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.485f, 0.567f, 0.666f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.697f, 0.242f, 0.675f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, 0.274f, -0.859f, 0.433f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 97880, 1);
        this.cam1.setTranslate(1.8f, 0.9f, 4.38f);
        this.cam1.setRotate(-1.5f, -1059.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(33);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.567f, 0.665f, 0.485f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.325f, 0.434f, -0.84f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, 0.565f, -0.815f, -0.125f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion_h.setLightMode(1);
        this.shion_h.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.shion_h.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.shion_h.light.setDirection2(1, 0.151f, 0.521f, 0.84f);
        this.shion_h.light.setColor(2, 0.37f, 0.37f, 0.37f);
        this.shion_h.light.setDirection2(2, 0.184f, 0.621f, -0.762f);
        this.shion_h.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.shion_h.light.setDirection2(3, 0.796f, -0.577f, -0.185f);
        Runtime.setDefocusQuick(0, 1, 29880, 1);
        this.cam1.setTranslate(2.96f, 0.88f, 2.99f);
        this.cam1.setRotate(-2.0f, -980.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(34);
        this.shion_h.setLightMode(0);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.86f, 0.338f, 0.383f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.063f, 0.55f, -0.833f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, -0.43f, -0.903f, -0.011f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 103880, 1);
        this.cam1.setTranslate(0.31f, 0.97f, 3.49f);
        this.cam1.setRotate(-5.5f, -1158.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(35);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.799f, 0.598f, 0.063f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, -0.394f, 0.014f, -0.919f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.669f, -0.738f, -0.087f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.57f, 0.97f, 4.38f);
        this.cam1.setRotate(-3.0f, -255.38f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(36);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.49f, 0.49f, 0.49f);
        this.light.setDirection2(1, -0.124f, 0.51f, 0.851f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.672f, 0.45f, -0.588f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.725f, -0.647f, 0.235f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 45880, 1);
        this.cam1.setTranslate(-1.0f, 0.97f, 4.23f);
        this.cam1.setRotate(-1.5f, -34.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(105, -1.5f, -82.38f, 0.0f, 3);
        this.waitCameraPlay(37);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, -0.162f, 0.339f, 0.927f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.672f, 0.45f, -0.588f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.725f, -0.647f, 0.235f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 101880, 1);
        this.cam1.setTranslate(0.86f, 0.92f, 5.16f);
        this.cam1.setRotate(3.0f, -41.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(38);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, 0.646f, 0.412f, -0.643f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.709f, 0.556f, -0.434f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.514f, -0.592f, -0.62f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 118880, 1);
        this.cam1.setTranslate(0.48f, 0.74f, 3.32f);
        this.cam1.setRotate(7.5f, 191.62f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(39);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.51f, 0.51f, 0.51f);
        this.light.setDirection2(1, -0.124f, 0.51f, 0.851f);
        this.light.setColor(2, 0.36f, 0.36f, 0.36f);
        this.light.setDirection2(2, -0.672f, 0.45f, -0.588f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, -0.725f, -0.647f, 0.235f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 105880, 1);
        this.cam1.setTranslate(0.96f, 1.02f, 5.13f);
        this.cam1.setRotate(-3.5f, -42.88f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(40);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.58f, 0.58f, 0.58f);
        this.light.setDirection2(1, -0.039f, 0.339f, 0.94f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.796f, 0.575f, -0.19f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.564f, -0.75f, 0.345f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.73f, 1.11f, 3.76f);
        this.cam1.setRotate(-5.5f, 44.12f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(41);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(1, 0.94f, 0.34f, 0.031f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, -0.256f, 0.348f, -0.902f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.658f, -0.747f, -0.091f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 73072, 1);
        this.cam1.setTranslate(-0.5f, 0.99f, 3.46f);
        this.cam1.setRotate(-5.0f, 134.12f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(42);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(1, 0.61f, 0.474f, 0.635f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.127f, 0.682f, -0.72f);
        this.light.setColor(3, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(3, 0.745f, -0.657f, 0.115f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 108880, 1);
        this.cam1.setTranslate(-0.72f, 0.9f, 4.92f);
        this.cam1.setRotate(1.5f, 76.12f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -0.72f, 0.95f, 4.92f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
    }

    void SPECOFF(Chr chr) {
        chr.renderCommand(530);
    }

    void SPECON(Chr chr) {
        chr.renderCommand(0);
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

    void changeLocation(int n) {
        if (n == 0) {
            this.shion_h.setVisible(false);
            this.mathews_h.setVisible(false);
            this.hammer.setVisible(false);
            this.tonny_h.setVisible(false);
            this.allen.setVisible(false);
            this.chaos_h.setVisible(false);
            this.kosmos.setVisible(false);
            this.andrew.setVisible(false);
            Stage.setVisible(-1, false);
            this.space.stop();
        }
        if (n == 1) {
            this.shion_h.setVisible(true);
            this.mathews_h.setVisible(true);
            this.hammer.setVisible(true);
            this.tonny_h.setVisible(true);
            this.allen.setVisible(true);
            this.chaos_h.setVisible(true);
            this.kosmos.setVisible(true);
            Stage.setVisible(-1, true);
            Stage.setVisible(6, false);
        }
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01044A_F");
        Runtime.setFlags(58, 1, 1);
        System.println("XEVEJNAME:SCE01044B");
        Runtime.jumpEvent(1441);
    }

    void face(Chr chr, int n) {
        chr.mtn(n, 8, 1.0f, false);
        chr.start(4, null);
    }

    void face(Chr chr, int n, int n2, int n3, int n4, int n5, float f) {
        chr.mtn(n, n2, n3, n4, n5, f, false);
        chr.start(4, null);
    }

    void init() {
        Runtime.setLocation(46);
        this.shion_h = new Characters(0x100001E, 0.0f, 0.0f, 0.0f, 0.0f);
        this.allen = new Characters(0x1000107, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kosmos = new Characters(0x1000002, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kosmos2 = new Characters(0x1000002, 0.0f, 0.0f, 0.0f, 0.0f);
        this.kosmos2.setVisible(false);
        this.mathews_h = new Characters(0x1000140, 0.0f, 0.0f, 0.0f, 0.0f);
        this.chaos_h = new Characters(0x1000020, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hammer = new Characters(0x1000116, 0.0f, 0.0f, 0.0f, 0.0f);
        this.tonny_h = new Characters(16777556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.andrew = new Characters(0x1000117, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space = new Chr_unit(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.start(1, "stars_inBridge");
        this.doorL = new Mapunits(4);
        this.doorR = new Mapunits(5);
        this.loadarc(this.shion_h.face, "FLSshion_h.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.andrew.face, "FLSandrew.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.kosmos2.face, "FLSkosmos.fpk");
        this.loadarc(this.mathews_h.face, "FLSmatehws_h.fpk");
        this.loadarc(this.chaos_h.face, "FLSchaos_h.fpk");
        this.loadarc(this.tonny_h.face, "FLStonny_h.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.change();
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        Stage.setVisible(6, false);
        this.shion_h.renderCommand(16);
        this.kosmos.renderCommand(16);
        this.chaos_h.renderCommand(18);
        this.allen.renderCommand(18);
        this.mathews_h.renderCommand(18);
        this.hammer.renderCommand(18);
        this.tonny_h.renderCommand(18);
        this.andrew.renderCommand(18);
        this.FadeIn = new Effect(0);
        this.FadeIn.args[0] = Integer.MIN_VALUE;
        this.FadeIn.args[1] = 30;
        this.FadeIn.args[2] = 1;
        this.FadeOut = new Effect(0);
        this.FadeOut.args[0] = Integer.MIN_VALUE;
        this.FadeOut.args[1] = 30;
        this.FadeOut.args[2] = 0;
        this.Cseat1 = new Com_seat2(36);
        this.Cseat1.setPivot(-3.0f, 0.05f, 6.55f);
        this.Cseat2 = new Com_seat(37);
        this.Cseat3 = new Com_seat(38);
        this.Cseat4 = new Com_seat(39);
        this.Cseat5 = new Com_seat2(40);
        this.Cseat6 = new Com_seat(41);
        this.Cseat7 = new Com_seat(42);
        this.setComSeat(0.0f);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1190065, 48000);
        Stage.setVisible(35, false);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(-0.05f, 0.0f, 10.5f);
        this.shion_h.setRotateY(180.0f);
        this.CameraPlay = 1;
        this.doorL.start(1, "openedL");
        this.doorR.start(1, "openedR");
        this.shion_h.mtn(403, 0, 164, 0, 0, 1.3f, true);
        System.sleep(30);
        this.msg.print("Excuse us...");
        this.face(this.shion_h.face, 1, 0, 18, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(45);
        this.cut_length = 105;
        this.kosmos.renderCommand(18);
        this.shion_h.renderCommand(18);
        this.shion_h.setTranslate(-0.05f, 0.0f, 9.5f);
        this.andrew.setVisible(true);
        this.andrew.setTranslate(-1.5f, 0.0f, 10.5f);
        this.andrew.setRotateY(180.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(0.5f, 0.0f, 10.0f);
        this.allen.setRotateY(180.0f);
        this.mathews_h.setVisible(true);
        this.mathews_h.setTranslate(0.0f, -0.5f, 3.0f);
        this.kosmos.setVisible(true);
        this.kosmos.setTranslate(2.0f, -0.5f, 2.45f);
        this.kosmos.setRotateY(-60.0f);
        this.kosmos.look_speed(100.0f);
        this.kosmos.look_char(this.shion_h);
        this.mathews_h.look_speed(100.0f);
        this.mathews_h.look_char(this.shion_h);
        this.CameraPlay = 2;
        this.mathews_h.mtn(412, 8, 1.0f, true);
        this.kosmos.mtn(387, 8, 1.0f, true);
        this.shion_h.start(1, "shion_c2");
        this.allen.start(1, "allen_c2");
        this.andrew.start(1, "andrew_c2");
        System.sleep(this.cut_length);
        this.cut_length = 90;
        Stage.setVisible(35, true);
        this.kosmos.renderCommand(16);
        this.shion_h.renderCommand(16);
        this.shion_h.setCollision(false);
        this.shion_h.setTranslate(0.0f, -0.5f, 5.5f);
        this.allen.setCollision(false);
        this.allen.setTranslate(0.5f, 0.0f, 7.3f);
        this.kosmos.setVisible(false);
        this.chaos_h.setVisible(false);
        this.andrew.setVisible(false);
        this.hammer.setVisible(true);
        this.hammer.setTranslate(2.2f, 0.0f, 6.75f);
        this.hammer.setRotateY(-155.0f);
        this.chaos_h.setRotateY(135.0f);
        this.shion_h.look_speed(100.0f);
        this.shion_h.look_char(this.mathews_h);
        this.mathews_h.look_speed(100.0f);
        this.mathews_h.look_char(this.shion_h);
        this.CameraPlay = 3;
        this.face(this.shion_h.face, 50);
        this.face(this.allen.face, 2);
        this.doorL.start(1, "closeL");
        this.doorR.start(1, "closeR");
        this.hammer.mtn(390, 8, 1.0f, true);
        this.shion_h.mtn(410, 30, 118, 0, 0, 1.0f, true);
        this.allen.start(1, "allen_c3");
        System.sleep(this.cut_length);
        this.shion_h.setMotionFlags(0x40000000, true);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(0.0f, -0.5f, 4.0f);
        this.shion_h.setRotateY(180.0f);
        this.allen.setCollision(false);
        this.allen.setTranslate(0.5f, -0.5f, 5.5f);
        this.allen.look_speed(100.0f);
        this.allen.look_char(this.mathews_h);
        this.CameraPlay = 4;
        this.face(this.shion_h.face, 50, 0, 30, 0, 0, 1.0f);
        this.shion_h.mtn(386, 8, 1.0f, true);
        this.allen.mtn(410, 60, 150, 0, 0, 1.0f, true);
        System.sleep(30);
        this.msg.print("Um...we...");
        this.face(this.shion_h.face, 49, 0, 17, 0, 0, 1.0f);
        System.sleep(25);
        this.face(this.shion_h.face, 49, 0, 17, 0, 0, 1.0f);
        this.waitclear(35);
        this.mathews_h.look_default();
        this.shion_h.setVisible(false);
        this.tonny_h.setVisible(false);
        this.tonny_h.setTranslate(0.9f, -0.5f, 1.7f);
        this.tonny_h.setRotateY(-25.0f);
        this.tonny_h.look_speed(100.0f);
        this.tonny_h.look_char(this.allen);
        this.CameraPlay = 5;
        this.mathews_h.mtn(269, 0, 1.0f, true);
        this.tonny_h.mtn(391, 8, 1.0f, true);
        this.msg.print("I'm Captain Matthews.");
        this.face(this.mathews_h.face, 1, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(17);
        this.tonny_h.look_default();
        this.shion_h.setMotionFlags(0x40000000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.allen.look_default();
        this.shion_h.look_default();
        this.tonny_h.setVisible(false);
        this.shion_h.setVisible(true);
        this.mathews_h.setTranslate(0.0f, -0.5f, 4.0f);
        this.mathews_h.setRotateY(180.0f);
        this.allen.setTranslate(0.5f, -0.5f, 4.0f);
        this.allen.setRotateY(-150.0f);
        this.allen.look_speed(100.0f);
        this.allen.look_point(-0.48f, 1.28f, 2.69f);
        this.CameraPlay = 6;
        this.face(this.shion_h.face, 50, 0, 45, 0, 0, 1.0f);
        this.shion_h.mtn(270, 0, 300, 0, 0, 1.0f, true);
        this.mathews_h.mtn(272, 0, 1.0f, true);
        this.allen.mtn(388, 8, 0.6f, true);
        System.sleep(45);
        this.msg.print("My name is Shion Uzuki, and I'm from\nVector Industries First R&D Division.");
        this.face(this.shion_h.face, 1, 0, 120, 16, 1, 1.0f);
        this.waitclear(135);
        System.sleep(15);
        this.msg.print("And this is my co-worker,\nAllen Ridgeley,");
        this.face(this.shion_h.face, 1, 0, 70, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(32);
        this.allen.look_default();
        this.CameraPlay = 7;
        this.shion_h.mtn(273, 0, 373, 0, 0, 1.042f, true);
        this.allen.mtn(274, 15, 553, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("who also works in the same division.");
        this.face(this.shion_h.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(55);
        this.msg.print("We do apologize\nfor all the trouble our errant\nKOS-MOS may have caused you.");
        this.face(this.shion_h.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(135);
        System.sleep(20);
        this.msg.print("C'mon Allen!\nShow them your gratitude.");
        this.face(this.shion_h.face, 1, 0, 17, 0, 0, 1.0f);
        System.sleep(25);
        this.face(this.shion_h.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(50);
        this.msg.print("Ah...right.");
        this.face(this.allen.face, 1, 15, 34, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.mathews_h.setTranslate(0.0f, -0.5f, 3.0f);
        this.mathews_h.setRotateY(15.0f);
        this.mathews_h.look_speed(0.2f);
        this.mathews_h.look_char(this.allen);
        this.CameraPlay = 8;
        this.shion_h.mtn(273, 375, 553, 0, 0, 1.2f, true);
        this.mathews_h.mtn(389, 8, 1.0f, true);
        this.msg.print("Thank heavens you were around\nto rescue us.");
        this.face(this.allen.face, 3, 0, 52, 12, 1, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("We almost became space dust\nout there because of my\nstubborn boss here...");
        this.face(this.allen.face, 3, 0, 105, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.shion_h.face, 6, 0, 60, 8, 1, 1.0f);
        System.sleep(60);
        this.mathews_h.look_default();
        this.CameraPlay = 9;
        this.allen.mtn(276, 40, 103, 0, 0, 1.0f, true);
        this.face(this.allen.face, 3, 0, 34, 0, 0, 1.0f);
        System.sleep(20);
        this.face(this.allen.face, 7, 0, 15, 8, 1, 1.0f);
        this.waitclear(10);
        this.msg.print("Argh!!");
        this.waitclear(35);
        this.CameraPlay = 10;
        this.shion_h.mtn(277, 0, 45, 0, 0, 1.0f, true);
        this.allen.mtn(388, 8, 1.0f, true);
        System.sleep(47);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(0.0f, -0.5f, 4.0f);
        this.shion_h.setRotateY(180.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(0.5f, -0.5f, 4.0f);
        this.allen.setRotateY(-150.0f);
        this.mathews_h.setVisible(true);
        this.mathews_h.setTranslate(0.0f, -0.5f, 3.0f);
        this.mathews_h.setRotateY(15.0f);
        this.kosmos.setVisible(false);
        this.kosmos.setTranslate(2.0f, -0.5f, 2.45f);
        this.kosmos.setRotateY(-60.0f);
        this.hammer.setVisible(true);
        this.hammer.setTranslate(2.2f, 0.0f, 6.75f);
        this.hammer.setRotateY(-155.0f);
        this.tonny_h.setVisible(false);
        this.tonny_h.setTranslate(0.9f, -0.5f, 1.7f);
        this.tonny_h.setRotateY(-25.0f);
        this.chaos_h.setVisible(false);
        this.chaos_h.setTranslate(-1.6f, -0.52f, 4.6f);
        this.chaos_h.setRotateY(120.0f);
        this.andrew.setVisible(false);
        this.andrew.setTranslate(-1.5f, 0.0f, 8.0f);
        this.andrew.setRotateY(180.0f);
        this.shion_h.setMotNoUpdate(2);
        this.shion_h.mtn(281, 0, 0, 0, 0, 1.0f, true);
        this.shion_h.setMotionFlags(0x800000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.kosmos.setVisible(true);
        this.kosmos.setRotateY(-45.0f);
        this.kosmos.look_speed(10.0f);
        this.kosmos.look_char(this.shion_h);
        this.CameraPlay = 11;
        this.face(this.shion_h.face, 2);
        this.mathews_h.mtn(411, 30, 225, 0, 0, 1.0f, true);
        this.kosmos.mtn(387, 8, 1.0f, true);
        this.hammer.mtn(390, 8, 1.0f, true);
        this.msg.print("You outta save your thanks\nfor chaos over there.");
        this.face(this.mathews_h.face, 1, 10, 84, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.msg.print("We were just gonna leave you\nand fly on out of here.");
        this.face(this.mathews_h.face, 1, 10, 111, 0, 0, 1.0f);
        this.waitclear(105);
        this.kosmos.setMotionFlags(0x40000000, false);
        this.mathews_h.look_default();
        this.mathews_h.setVisible(false);
        this.kosmos.setVisible(false);
        this.kosmos.setRotateY(-60.0f);
        this.chaos_h.setVisible(true);
        this.chaos_h.setTranslate(-1.65f, -0.52f, 4.7f);
        this.chaos_h.setRotateY(120.0f);
        this.andrew.setVisible(true);
        this.andrew.look_char(this.shion_h);
        this.SPECOFF(this.andrew);
        this.SPECOFF(this.hammer);
        this.SPECOFF(this.allen);
        this.SPECOFF(this.chaos_h);
        this.shion_h.renderCommand(528);
        this.CameraPlay = 12;
        this.face(this.allen.face, 8);
        this.shion_h.mtn(281, 0, 328, 0, 0, 1.0f, true);
        this.chaos_h.mtn(283, 0, 328, 0, 0, 1.0f, true);
        this.allen.mtn(388, 8, 0.6f, true);
        this.andrew.mtn(415, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Oh...I see...");
        this.face(this.shion_h.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        this.face(this.shion_h.face, 2);
        System.sleep(165);
        this.SPECOFF(this.chaos_h);
        this.shion_h.renderCommand(0);
        this.hammer.setVisible(false);
        this.andrew.setVisible(false);
        this.CameraPlay = 13;
        System.sleep(15);
        this.msg.print("Thank you...chaos.");
        this.face(this.shion_h.face, 3, 0, 17, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.shion_h.face, 3, 0, 17, 0, 0, 1.0f);
        this.waitclear(45);
        this.SPECON(this.chaos_h);
        this.chaos_h.renderCommand(16);
        this.mathews_h.setRotateY(-15.0f);
        this.mathews_h.look_char(this.shion_h);
        this.CameraPlay = 14;
        this.shion_h.mtn(281, 345, 495, 0, 0, 1.0f, true);
        this.chaos_h.mtn(283, 345, 495, 0, 0, 1.0f, true);
        this.msg.print("No, don't mention it.");
        this.face(this.chaos_h.face, 3, 0, 26, 0, 0, 1.0f);
        this.waitclear(35);
        this.msg.print("After all, we all need\na little help sometimes.");
        this.face(this.chaos_h.face, 3, 0, 79, 0, 0, 1.0f);
        this.waitclear(90);
        this.chaos_h.renderCommand(18);
        this.mathews_h.setVisible(true);
        this.CameraPlay = 15;
        this.mathews_h.mtn(414, 0, 240, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Yeah...He often saves\nour hides, too. So...");
        this.face(this.mathews_h.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.mathews_h.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(30);
        System.sleep(10);
        this.msg.print("we can't turn him down\nwhen he asks for a favor.");
        this.face(this.mathews_h.face, 1, 0, 55, 0, 0, 1.0f);
        this.waitclear(65);
        System.sleep(5);
        this.mathews_h.look_default();
        this.shion_h.setMotionFlags(0x2000000, true);
        this.shion_h.setVisible(false);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(0.0f, -0.5f, 4.0f);
        this.shion_h.setRotateY(180.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(0.5f, -0.5f, 4.0f);
        this.allen.setRotateY(-150.0f);
        this.mathews_h.setVisible(true);
        this.mathews_h.setTranslate(0.0f, -0.5f, 3.0f);
        this.mathews_h.setRotateY(-15.0f);
        this.kosmos.setVisible(false);
        this.kosmos.setTranslate(2.0f, -0.5f, 2.45f);
        this.kosmos.setRotateY(-60.0f);
        this.hammer.setVisible(false);
        this.hammer.setTranslate(2.2f, 0.0f, 6.75f);
        this.hammer.setRotateY(-155.0f);
        this.tonny_h.setVisible(false);
        this.tonny_h.setTranslate(0.9f, -0.5f, 1.7f);
        this.tonny_h.setRotateY(-25.0f);
        this.chaos_h.setVisible(true);
        this.chaos_h.setTranslate(-1.65f, -0.52f, 4.7f);
        this.chaos_h.setRotateY(120.0f);
        this.andrew.setVisible(false);
        this.andrew.setTranslate(-1.5f, 0.0f, 8.0f);
        this.andrew.setRotateY(180.0f);
        this.shion_h.renderCommand(528);
        this.chaos_h.setVisible(false);
        this.shion_h.setTranslate(-1.0f, -0.5f, 4.35f);
        this.shion_h.setRotateY(-65.0f);
        this.allen.setTranslate(0.5f, -0.5f, 4.5f);
        this.hammer.setVisible(true);
        this.allen.look_speed(100.0f);
        this.allen.look_char(this.shion_h);
        this.CameraPlay = 16;
        this.face(this.kosmos.face, 2);
        this.face(this.shion_h.face, 2);
        this.shion_h.mtn(286, 0, 255, 0, 0, 1.0f, true);
        this.mathews_h.mtn(389, 8, 1.0f, true);
        this.kosmos.mtn(387, 8, 1.0f, true);
        this.hammer.mtn(390, 8, 1.0f, true);
        this.allen.mtn(388, 8, 1.0f, true);
        System.sleep(15);
        this.msg.print("I see.");
        this.face(this.shion_h.face, 1, 0, 17, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("That reminds me...");
        this.face(this.shion_h.face, 1, 0, 38, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(15);
        this.shion_h.renderCommand(0);
        this.SPECON(this.andrew);
        this.SPECON(this.hammer);
        this.SPECON(this.allen);
        this.kosmos.setMotNoUpdate(2);
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.kosmos.look_default();
        this.chaos_h.setVisible(false);
        this.shion_h.setRotateY(-90.0f);
        this.CameraPlay = 17;
        this.face(this.shion_h.face, 5, 0, 38, 8, 1, 1.0f);
        System.sleep(10);
        this.msg.print("Hey, KOS-MOS!");
        this.waitclear(45);
        this.msg.print("And just what do you think\nyou're doing?!");
        this.face(this.shion_h.face, 5, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        System.sleep(5);
        this.kosmos.setVisible(true);
        this.mathews_h.setVisible(false);
        this.shion_h.setTranslate(-1.15f, -0.5f, 4.35f);
        this.shion_h.setRotateY(120.0f);
        this.CameraPlay = 18;
        System.sleep(2);
        this.shion_h.mtn(287, 15, 206, 0, 0, 1.0f, true);
        System.sleep(28);
        this.msg.print("You left us and then tried to go to\nSecond Miltia by yourself!");
        System.sleep(58);
        this.hammer.setVisible(false);
        this.allen.setVisible(false);
        this.kosmos.setTranslate(2.0f, -0.6f, 2.45f);
        this.CameraPlay = 19;
        this.face(this.shion_h.face, 5, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(5);
        this.msg.print("Maybe your OS is malfunctioning\nor something?");
        this.face(this.shion_h.face, 5, 0, 60, 0, 0, 1.0f);
        System.sleep(35);
        this.shion_h.setVisible(false);
        this.shion_h.setMotNoUpdate(2);
        this.shion_h.setTranslate(1.35f, -0.5f, 2.95f);
        this.shion_h.mtn(288, 0, 0, 0, 0, 1.0f, true);
        this.shion_h.setMotionFlags(0x800000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.kosmos.setTranslate(2.0f, -0.5f, 2.45f);
        this.andrew.look_char(this.shion_h);
        this.CameraPlay = 20;
        this.waitclear(60);
        this.msg.print("Yes, that is possible.");
        this.face(this.kosmos.face, 1, 10, 60, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.shion_h.setVisible(true);
        this.andrew.setVisible(true);
        this.mathews_h.setTranslate(0.0f, -0.5f, 2.6f);
        this.mathews_h.setRotateY(0.0f);
        this.CameraPlay = 21;
        this.andrew.mtn(394, 8, 1.0f, true);
        this.shion_h.mtn(288, 0, 165, 0, 0, 0.95f, true);
        this.face(this.shion_h.face, 6, 0, 45, 0, 0, 1.0f);
        System.sleep(45);
        this.msg.print("By the way...\nwhat were the orders from HQ?");
        this.face(this.shion_h.face, 5, 0, 17, 0, 0, 1.0f);
        System.sleep(55);
        this.face(this.shion_h.face, 5, 0, 59, 0, 0, 1.0f);
        this.waitclear(75);
        this.shion_h.setVisible(false);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(1.35f, -0.5f, 2.95f);
        this.shion_h.setRotateY(120.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(0.5f, -0.5f, 4.0f);
        this.allen.setRotateY(-150.0f);
        this.mathews_h.setVisible(false);
        this.mathews_h.setTranslate(0.0f, -0.5f, 2.6f);
        this.mathews_h.setRotateY(0.0f);
        this.kosmos.setVisible(true);
        this.kosmos.setTranslate(2.0f, -0.5f, 2.45f);
        this.kosmos.setRotateY(-60.0f);
        this.hammer.setVisible(false);
        this.hammer.setTranslate(2.2f, 0.0f, 6.75f);
        this.hammer.setRotateY(-155.0f);
        this.tonny_h.setVisible(false);
        this.tonny_h.setTranslate(0.9f, -0.5f, 1.7f);
        this.tonny_h.setRotateY(-25.0f);
        this.chaos_h.setVisible(false);
        this.chaos_h.setTranslate(-1.65f, -0.52f, 4.7f);
        this.chaos_h.setRotateY(120.0f);
        this.andrew.setVisible(false);
        this.allen.setTranslate(0.5f, -0.5f, 4.5f);
        this.andrew.setRotateY(180.0f);
        this.tonny_h.setVisible(false);
        this.mathews_h.setVisible(true);
        this.kosmos.setTranslate(1.5f, -0.5f, 2.5f);
        this.kosmos.setRotateY(-75.0f);
        this.shion_h.setTranslate(1.75f, -0.5f, 3.35f);
        this.shion_h.setRotateY(135.0f);
        this.CameraPlay = 22;
        this.mathews_h.mtn(412, 8, 1.0f, true);
        this.kosmos.mtn(416, 0, 178, 0, 0, 1.0f, true);
        this.shion_h.mtn(291, 10, 188, 0, 0, 1.0f, true);
        this.face(this.shion_h.face, 6);
        this.kosmos.look_speed(0.3f);
        this.kosmos.look_char(this.mathews_h);
        this.msg.print("Captain Matthews.");
        this.face(this.kosmos.face, 1, 0, 29, 0, 0, 1.0f);
        System.sleep(30);
        this.mathews_h.look_speed(0.3f);
        this.mathews_h.look_char(this.kosmos);
        this.waitclear(15);
        this.msg.print("May I use the maintenance lab\nnext to the hangar?");
        this.face(this.kosmos.face, 1, 0, 90, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("Just a second, KOS-MOS...?");
        this.face(this.shion_h.face, 5, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        this.shion_h.setMotionFlags(0x40000000, true);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.mathews_h.setRotateY(60.0f);
        this.mathews_h.look_speed(100.0f);
        this.mathews_h.look_point(2.05f, 1.21f, 2.31f);
        this.kosmos.setTranslate(1.4f, -0.5f, 2.8f);
        this.kosmos.look_speed(100.0f);
        this.kosmos.look_char(this.mathews_h);
        this.CameraPlay = 23;
        this.shion_h.mtn(291, 257, 350, 0, 0, 1.0f, true);
        this.kosmos.mtn(387, 8, 1.0f, true);
        this.msg.print("Yeah, sure.");
        this.face(this.mathews_h.face, 1, 0, 27, 0, 0, 1.0f);
        this.waitclear(40);
        this.msg.print("What for?");
        this.face(this.mathews_h.face, 1, 0, 27, 0, 0, 1.0f);
        this.waitclear(20);
        this.msg.print("KOS-MOS!");
        this.face(this.shion_h.face, 5, 0, 38, 0, 0, 1.0f);
        this.waitclear(35);
        this.shion_h.setRotateY(120.0f);
        this.CameraPlay = 24;
        this.kosmos.mtn(387, 8, 1.0f, true);
        this.shion_h.start(1, "shion_h_c24");
        this.msg.print("Due to the limitations of the test-use\ncondenser, my energy reserves are\nalmost depleted.");
        this.face(this.kosmos.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.kosmos.face, 1, 43, 60, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("I wish to receive a co-generator\nbypass in order to replenish them.");
        this.face(this.kosmos.face, 1, 0, 100, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("Hey!\nWhat is wrong with you?\nAnswer me, KOS-MOS!!");
        this.face(this.shion_h.face, 9, 0, 120, 16, 1, 1.0f);
        this.waitclear(110);
        this.kosmos.setMotionFlags(0x40000000, true);
        this.shion_h.setRotateY(135.0f);
        this.mathews_h.setRotateY(75.0f);
        this.mathews_h.look_speed(100.0f);
        this.mathews_h.look_point(2.05f, 1.21f, 2.31f);
        this.kosmos.look_speed(100.0f);
        this.kosmos.look_char(this.mathews_h);
        this.CameraPlay = 25;
        this.kosmos.mtn(292, 30, 138, 0, 0, 1.0f, true);
        this.mathews_h.mtn(412, 8, 1.0f, true);
        this.msg.print("So, you'll cover the bill, right?");
        this.face(this.mathews_h.face, 1, 10, 84, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("Yes.");
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(35);
        this.shion_h.setMotionFlags(0x40000000, false);
        this.kosmos.setMotionFlags(0x40000000, false);
        this.kosmos2.setMotNoUpdate(2);
        this.kosmos2.setTranslate(1.25f, -0.5f, 2.8f);
        this.kosmos2.setRotateY(-20.0f);
        this.kosmos2.mtn(296, 104, 104, 0, 0, 1.0f, true);
        this.kosmos2.setMotionFlags(0x800000, false);
        this.kosmos2.setMotionFlags(0x2000000, true);
        this.shion_h.setRotateY(225.0f);
        this.kosmos.look_default();
        this.CameraPlay = 26;
        this.kosmos.start(1, "kosmos_c26");
        this.shion_h.mtn(293, 15, 348, 0, 0, 1.0f, true);
        this.msg.print("KOS-MO...");
        this.face(this.shion_h.face, 9, 0, 19, 0, 0, 1.0f);
        this.waitclear(15);
        this.msg.print("Shion.");
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(35);
        System.sleep(5);
        this.msg.print("Y-yes?");
        this.face(this.shion_h.face, 49, 0, 15, 12, 1, 1.0f);
        System.sleep(15);
        this.face(this.shion_h.face, 49, 38, 46, 0, 0, 1.0f);
        this.waitclear(30);
        System.sleep(25);
        this.kosmos.setVisible(false);
        this.kosmos2.setVisible(true);
        this.shion_h.setRotateY(210.0f);
        this.CameraPlay = 28;
        this.kosmos2.start(1, "kosmos_c28");
        System.sleep(15);
        this.msg.print("My sensors appear to be\nmalfunctioning.");
        this.face(this.kosmos2.face, 1, 0, 42, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("Please adjust them before we\ndisembark at our destination.");
        this.face(this.kosmos2.face, 1, 10, 120, 0, 0, 1.0f);
        this.waitclear(120);
        System.sleep(15);
        this.kosmos.setMotNoUpdate(2);
        this.kosmos.mtn(296, 0, 0, 0, 0, 1.0f, true);
        this.kosmos.setMotionFlags(0x800000, false);
        this.kosmos.setMotionFlags(0x2000000, true);
        this.shion_h.setMotionFlags(0x800000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.kosmos.setVisible(false);
        this.CameraPlay = 29;
        this.shion_h.mtn(295, 0, 1.0f, true);
        this.msg.print("Wha...?\nWhy should I...");
        this.face(this.shion_h.face, 21, 0, 70, 0, 0, 1.0f);
        this.waitclear(77);
        this.kosmos2.setVisible(false);
        this.kosmos.setVisible(true);
        this.shion_h.setTranslate(1.65f, -0.5f, 3.25f);
        this.shion_h.setRotateY(235.0f);
        this.kosmos.setVisible(true);
        this.kosmos.setTranslate(1.4f, -0.5f, 2.8f);
        this.kosmos.setRotateY(-20.0f);
        this.CameraPlay = 30;
        this.face(this.shion_h.face, 22);
        this.kosmos.mtn(296, 0, 1.0f, true);
        this.shion_h.mtn(297, 0, 268, 0, 0, 1.0f, true);
        this.msg.print("Please, Shion.");
        this.face(this.kosmos.face, 1, 0, 60, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("This is part of your job,\nis it not?");
        this.face(this.kosmos.face, 1, 0, 29, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.kosmos.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(15);
        this.face(this.kosmos.face, 2, 43, 90, 0, 0, 1.0f);
        System.sleep(30);
        this.cut_length = 120;
        this.kosmos.setTranslate(1.0f, -0.5f, 2.8f);
        this.kosmos.setRotateY(-5.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(-0.45f, -0.5f, 5.75f);
        this.allen.setRotate(0.0f, 75.0f, 0.0f);
        this.mathews_h.setVisible(false);
        this.CameraPlay = 31;
        this.kosmos.start(1, "kosmos_c31");
        this.allen.look_char(this.kosmos);
        this.allen.mtn(388, 8, 0.6f, true);
        System.sleep(120);
        this.allen.look_default();
        this.mathews_h.look_default();
        this.shion_h.setVisible(false);
        this.shion_h.setVisible(true);
        this.shion_h.setTranslate(1.35f, -0.5f, 2.95f);
        this.shion_h.setRotateY(120.0f);
        this.allen.setVisible(true);
        this.allen.setTranslate(-0.45f, -0.5f, 5.75f);
        this.allen.setRotate(0.0f, 75.0f, 0.0f);
        this.mathews_h.setVisible(false);
        this.mathews_h.setTranslate(0.0f, -0.5f, 2.6f);
        this.mathews_h.setRotateY(60.0f);
        this.kosmos.setVisible(false);
        this.hammer.setVisible(false);
        this.hammer.setTranslate(2.2f, 0.0f, 6.75f);
        this.hammer.setRotateY(-155.0f);
        this.tonny_h.setVisible(false);
        this.tonny_h.setTranslate(0.9f, -0.5f, 1.7f);
        this.tonny_h.setRotateY(-25.0f);
        this.chaos_h.setVisible(false);
        this.chaos_h.setTranslate(-1.65f, -0.52f, 4.7f);
        this.chaos_h.setRotateY(120.0f);
        this.andrew.setVisible(false);
        this.allen.setTranslate(0.5f, -0.5f, 4.5f);
        this.andrew.setRotateY(180.0f);
        this.kosmos.setVisible(false);
        this.shion_h.setTranslate(1.4f, -0.5f, 3.3f);
        this.shion_h.setRotateY(-15.0f);
        this.allen.look_default();
        this.CameraPlay = 32;
        this.shion_h.mtn(299, 0, 345, 0, 0, 1.0f, true);
        System.sleep(105);
        this.face(this.shion_h.face, 21, 0, 10, 0, 0, 0.3f);
        this.waitclear(45);
        this.face(this.shion_h.face, 22, 0, 30, 0, 0, 1.0f);
        System.sleep(22);
        this.shion_h.setRotateY(-10.0f);
        this.allen.setVisible(false);
        this.mathews_h.setVisible(true);
        this.mathews_h.setTranslate(-0.55f, -0.55f, 2.9f);
        this.mathews_h.setRotateY(60.0f);
        this.mathews_h.look_speed(100.0f);
        this.mathews_h.look_point(2.5f, 0.95f, 2.84f);
        this.chaos_h.setVisible(true);
        this.CameraPlay = 33;
        this.face(this.mathews_h.face, 2);
        this.face(this.chaos_h.face, 2);
        this.mathews_h.mtn(412, 8, 1.0f, true);
        this.chaos_h.mtn(413, 0, 175, 0, 0, 1.0f, true);
        this.msg.print("I'm terribly sorry.");
        this.waitclear(60);
        this.msg.print("The girl is just so stubborn.");
        this.waitclear(105);
        System.sleep(10);
        this.shion_h.setMotionFlags(0x800000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.shion_h.setRotateY(-90.0f);
        this.CameraPlay = 34;
        this.shion_h.mtn(403, 0, 75, 0, 0, 1.0f, true);
        this.msg.print("I do apologize for all the trouble\nshe's causing.");
        this.face(this.shion_h.face, 21, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.chaos_h.setMotionFlags(0x2000000, true);
        this.chaos_h.renderCommand(16);
        this.chaos_h.look_speed(100.0f);
        this.chaos_h.look_char(this.shion_h);
        this.CameraPlay = 35;
        this.chaos_h.mtn(419, 150, 395, 0, 0, 1.0f, true);
        this.msg.print("It's all right,\nI'm actually enjoying it.");
        this.face(this.chaos_h.face, 3, 0, 62, 0, 0, 1.0f);
        this.waitclear(80);
        this.msg.print("This ship's much livelier\nwith more crew members.");
        this.face(this.chaos_h.face, 3, 0, 94, 0, 0, 1.0f);
        this.waitclear(105);
        this.face(this.chaos_h.face, 4);
        System.sleep(15);
        this.msg.print("You got that right.");
        System.sleep(30);
        this.chaos_h.look_speed(0.1f);
        this.chaos_h.look_char(this.mathews_h);
        this.waitclear(15);
        this.chaos_h.look_default();
        this.chaos_h.renderCommand(530);
        this.SPECOFF(this.mathews_h);
        this.tonny_h.setVisible(true);
        this.tonny_h.setTranslate(0.9f, -0.5f, 0.5f);
        this.tonny_h.setRotateY(0.0f);
        this.shion_h.setTranslate(1.4f, -0.5f, 4.2f);
        this.CameraPlay = 36;
        this.face(this.shion_h.face, 50);
        this.tonny_h.mtn(302, 30, 125, 0, 0, 0.8f, true);
        this.shion_h.mtn(303, 15, 133, 0, 0, 1.0f, true);
        this.msg.print("We can always use some more\nwomen around here.");
        this.face(this.tonny_h.face, 3, 0, 93, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(15);
        this.SPECON(this.mathews_h);
        this.shion_h.setTranslate(1.4f, -0.5f, 4.2f);
        this.shion_h.setRotateY(0.0f);
        this.tonny_h.setTranslate(1.4f, -0.5f, 4.2f);
        this.tonny_h.setRotateY(0.0f);
        this.CameraPlay = 37;
        this.tonny_h.mtn(304, 0, 1.0f, true);
        this.shion_h.mtn(305, 0, 223, 0, 0, 1.0f, true);
        System.sleep(10);
        this.msg.print("How about it?");
        this.face(this.tonny_h.face, 3, 0, 9, 0, 0, 1.0f);
        this.face(this.shion_h.face, 16, 0, 120, 12, 9, 1.0f);
        this.waitclear(45);
        this.msg.print("Once we arrive, how about you and I\nget a suite and settle in with\na nice bottle of wine...?");
        this.face(this.tonny_h.face, 3, 0, 120, 0, 0, 1.0f);
        System.sleep(120);
        this.face(this.tonny_h.face, 3, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(5);
        this.shion_h.setMotNoUpdate(2);
        this.shion_h.mtn(307, 90, 90, 0, 0, 1.0f, true);
        this.shion_h.setMotionFlags(0x800000, false);
        this.shion_h.setMotionFlags(0x2000000, true);
        this.allen.setVisible(true);
        this.allen.setRotateY(175.0f);
        this.CameraPlay = 38;
        this.face(this.allen.face, 5, 0, 0, 0, 0, 1.0f);
        this.allen.mtn(306, 0, 75, 0, 0, 1.0f, true);
        this.msg.print("Wha...? HEEYYY!!");
        this.face(this.allen.face, 9, 0, 15, 0, 0, 0.7f);
        System.sleep(25);
        this.face(this.allen.face, 9, 0, 15, 0, 0, 0.3f);
        this.waitclear(35);
        this.shion_h.look_speed(0.0f);
        this.shion_h.look_eye_speed(1.0f);
        this.CameraPlay = 39;
        this.face(this.shion_h.face, 16, 0, 0, 0, 0, 1.0f);
        this.shion_h.mtn(307, 90, 180, 0, 0, 1.0f, true);
        this.tonny_h.start(1, "tonny_h_c39");
        this.msg.print("Huh?");
        this.face(this.shion_h.face, 15, 0, 17, 0, 0, 1.0f);
        System.sleep(15);
        this.shion_h.look_point(1.22f, 0.87f, 5.14f);
        this.waitclear(30);
        this.shion_h.look_speed(0.01f);
        this.shion_h.look_point(0.93f, 1.04f, 4.27f);
        this.msg.print("Uh...sure...");
        this.face(this.shion_h.face, 15, 0, 38, 0, 0, 1.0f);
        this.waitclear(45);
        this.mathews_h.look_default();
        this.mathews_h.setTranslate(-1.5f, -0.52f, 2.9f);
        this.mathews_h.setRotateY(35.5f);
        this.CameraPlay = 40;
        this.mathews_h.mtn(411, 0, 270, 0, 0, 0.9f, true);
        this.msg.print("All right, that's enough, Tony.");
        this.face(this.mathews_h.face, 1, 10, 84, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("Never mind her,\nat least she's a real girl.");
        this.face(this.mathews_h.face, 1, 10, 111, 0, 0, 1.0f);
        System.sleep(90);
        this.waitclear(15);
        this.msg.print("The other one's not even human --\nshe's battle weaponry.");
        this.face(this.mathews_h.face, 1, 10, 84, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.chaos_h.renderCommand(0);
        this.chaos_h.setTranslate(-1.6f, -0.52f, 4.6f);
        this.chaos_h.setRotateY(60.0f);
        this.CameraPlay = 41;
        this.chaos_h.start(1, "chaos_c41_42");
        System.sleep(15);
        this.msg.print("You think so...huh?");
        this.face(this.chaos_h.face, 3, 0, 16, 0, 0, 0.5f);
        this.waitclear(30);
        System.sleep(15);
        this.cut_length = 120;
        this.chaos_h.setRotateY(20.0f);
        this.CameraPlay = 42;
        System.sleep(15);
        this.msg.print("She appears to be just like\nany other human to me.");
        this.face(this.chaos_h.face, 3, 0, 79, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
    }

    void rSPL(int n, float f, float f2, float f3) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0);
    }

    void rSPL(int n, float f, float f2, float f3, int n2) {
        this.rot[0] = 1.0f;
        this.rot[1] = this.cam1.getRotateX();
        this.rot[2] = this.cam1.getRotateY();
        this.rot[3] = this.cam1.getRotateZ();
        this.rot[4] = n;
        this.rot[5] = f;
        this.rot[6] = f2;
        this.rot[7] = f3;
        this.cam1.rotateSPL(this.rot, 0, n2, n);
    }

    void rotComSeat(int n, float f, float f2) {
        float f3 = f2 - f;
        int n2 = 1;
        while (n2 < n + 1) {
            float f4 = f + f3 / (float) n * (float) n2;
            float f5 = 0.0175f * f4;
            float f6 = 1.24f - 0.031297326f * (f4 + 2.44f);
            float f7 = f6 * Math.cos(Math.toRadians(39.62f));
            float f8 = f6 * Math.sin(Math.toRadians(39.62f));
            this.Cseat1.setPivot(-3.0f, 0.05f - f7, 6.55f + f8);
            this.Cseat1.setTranslate(this.Cseat1.px, f7, 0.67f - f5 - f8);
            this.Cseat1.setRotate(-37.18f + f4, this.Cseat1.ry, this.Cseat1.rz);
            f7 = 0.05f + Math.cos(Math.toRadians(f4)) * 3.59f;
            f8 = 3.62f - f5 + (3.59f - Math.sin(Math.toRadians(f4)) * 3.59f);
            this.Cseat2.setTranslate(this.Cseat6.px, f7 - 0.1f, f8 - 0.76f);
            this.Cseat3.setTranslate(this.Cseat6.px, f7 - 0.01f, f8 - 0.71f);
            this.Cseat4.setTranslate(this.Cseat6.px - 0.24f, f7 + 0.2f, f8 - 1.02f);
            this.Cseat5.setTranslate(this.Cseat6.px + 3.0f, f7 - 2.22f, f8 - 3.69f);
            this.Cseat6.setTranslate(this.Cseat6.px, f7, f8);
            this.Cseat7.setTranslate(this.Cseat7.px, this.Cseat7.py, 7.22f - f5);
            this.Cseat7.setRotate(-37.18f + f4, this.Cseat7.ry, this.Cseat7.rz);
            System.sleep(1);
            ++n2;
        }
    }

    void rotComSeat(int n, float f, float f2, Chr chr) {
        float f3 = 0.0f;
        float f4 = f2 - f;
        int n2 = 1;
        while (n2 < n + 1) {
            float f5 = f + f4 / (float) n * (float) n2;
            f3 = 0.0175f * f5;
            this.Cseat1.setTranslate(this.Cseat1.px, -0.2f + Math.cos(Math.toRadians(f5)) * 3.65f, 2.88f - f3 + (3.65f - Math.sin(Math.toRadians(f5)) * 3.65f));
            this.Cseat2.setTranslate(this.Cseat2.px, -0.1f + Math.cos(Math.toRadians(f5)) * 3.65f, 2.93f - f3 + (3.65f - Math.sin(Math.toRadians(f5)) * 3.65f));
            this.Cseat3.setTranslate(this.Cseat3.px, 0.1f + Math.cos(Math.toRadians(f5)) * 3.65f, 2.58f - f3 + (3.65f - Math.sin(Math.toRadians(f5)) * 3.65f));
            this.Cseat4.setTranslate(this.Cseat4.px, 0.1f + Math.cos(Math.toRadians(f5)) * 3.65f, 2.58f - f3 + (3.65f - Math.sin(Math.toRadians(f5)) * 3.65f));
            this.Cseat5.setTranslate(this.Cseat5.px, -0.1f + Math.cos(Math.toRadians(f5)) * 3.65f, 3.63f - f3 + (3.65f - Math.sin(Math.toRadians(f5)) * 3.65f));
            this.Cseat6.setRotate(f5 - 40.0f, this.Cseat6.ry, this.Cseat6.rz);
            this.Cseat6.setTranslate(this.Cseat6.px, this.Cseat6.py, 7.255f - f3);
            chr.setTranslate(this.Cseat1.px, this.Cseat1.py - 0.27f, this.Cseat1.pz - 0.12f);
            System.sleep(1);
            ++n2;
        }
    }

    void s() {
        System.waitSignal(this.msg, 255);
    }

    void setComSeat(float f) {
        float f2 = 0.0175f * f;
        float f3 = 1.24f - 0.031297326f * (f + 2.44f);
        float f4 = f3 * Math.cos(Math.toRadians(39.62f));
        float f5 = f3 * Math.sin(Math.toRadians(39.62f));
        this.Cseat1.setPivot(-3.0f, 0.05f - f4, 6.55f + f5);
        this.Cseat1.setTranslate(this.Cseat1.px, f4, 0.67f - f2 - f5);
        this.Cseat1.setRotate(-37.18f + f, this.Cseat1.ry, this.Cseat1.rz);
        f4 = 0.05f + Math.cos(Math.toRadians(f)) * 3.59f;
        f5 = 3.62f - f2 + (3.59f - Math.sin(Math.toRadians(f)) * 3.59f);
        this.Cseat2.setTranslate(this.Cseat6.px, f4 - 0.1f, f5 - 0.76f);
        this.Cseat3.setTranslate(this.Cseat6.px, f4 - 0.01f, f5 - 0.71f);
        this.Cseat4.setTranslate(this.Cseat6.px - 0.24f, f4 + 0.2f, f5 - 1.02f);
        this.Cseat5.setTranslate(this.Cseat6.px + 3.0f, f4 - 2.22f, f5 - 3.69f);
        this.Cseat6.setTranslate(this.Cseat6.px, f4, f5);
        this.Cseat7.setTranslate(this.Cseat7.px, this.Cseat7.py, 7.22f - f2);
        this.Cseat7.setRotate(-37.18f + f, this.Cseat7.ry, this.Cseat7.rz);
    }

    void setComSeat(float f, Chr chr) {
        float f2 = 0.0f;
        f2 = 0.0175f * f;
        this.Cseat1.setTranslate(this.Cseat1.px, -0.2f + Math.cos(Math.toRadians(f)) * 3.65f, 2.88f - f2 + (3.65f - Math.sin(Math.toRadians(f)) * 3.65f));
        this.Cseat2.setTranslate(this.Cseat2.px, -0.1f + Math.cos(Math.toRadians(f)) * 3.65f, 2.93f - f2 + (3.65f - Math.sin(Math.toRadians(f)) * 3.65f));
        this.Cseat3.setTranslate(this.Cseat3.px, 0.1f + Math.cos(Math.toRadians(f)) * 3.65f, 2.58f - f2 + (3.65f - Math.sin(Math.toRadians(f)) * 3.65f));
        this.Cseat4.setTranslate(this.Cseat4.px, 0.1f + Math.cos(Math.toRadians(f)) * 3.65f, 2.58f - f2 + (3.65f - Math.sin(Math.toRadians(f)) * 3.65f));
        this.Cseat5.setTranslate(this.Cseat5.px, -0.1f + Math.cos(Math.toRadians(f)) * 3.65f, 3.63f - f2 + (3.65f - Math.sin(Math.toRadians(f)) * 3.65f));
        this.Cseat6.setRotate(f - 40.0f, this.Cseat6.ry, this.Cseat6.rz);
        this.Cseat6.setTranslate(this.Cseat6.px, this.Cseat6.py, 7.255f - f2);
        chr.setTranslate(this.Cseat1.px, this.Cseat1.py - 0.27f, this.Cseat1.pz - 0.12f);
    }

    void shakeScreen(int n, float f, float f2, float f3, float f4, float f5) {
        float f6 = this.cam1.getTranslateX();
        float f7 = this.cam1.getTranslateY();
        float f8 = this.cam1.getTranslateZ();
        int n2 = n / 22;
        float[] fArray = new float[]{1.0f, f6, f7, f8, 2.0f, f6 - (f4 *= 0.001f), f7, f8 - f4, 4.0f, f6, f7, f8, 6.0f, f6 + f4, f7, f8 + f4, 8.0f, f6, f7, f8, 10.0f, f6 - f4, f7, f8 - f4, 12.0f, f6, f7, f8, 14.0f, f6 + f4, f7, f8 + f4, 16.0f, f6, f7, f8, 18.0f, f6 - f4, f7, f8 - f4, 20.0f, f6, f7, f8, 22.0f, f6 + f4, f7, f8 + f4};
        float[] fArray2 = new float[]{1.0f, f, f2, f3, 2.0f, f, f2 + (f5 *= 0.001f), f3, 4.0f, f - f5, f2 - f5, f3, 6.0f, f - f5 * 2.5f, f2 + f5, f3, 8.0f, f - f5, f2 - f5, f3, 10.0f, f, f2 + f5, f3, 12.0f, f + f5, f2 - f5, f3, 14.0f, f + f5 * 2.5f, f2 + f5, f3, 16.0f, f + f5, f2 - f5, f3, 18.0f, f, f2 + f5, f3, 20.0f, f - f5, f2 - f5, f3, 22.0f, f - f5 * 2.5f, f2 + f5, f3};
        int n3 = 0;
        while (n3 < n / 22) {
            this.cam1.transSPL(fArray, 0);
            this.cam1.viewSPL(fArray2, 0);
            System.sleep(22);
            ++n3;
        }
        if (n % 22 != 0) {
            this.cam1.transSPL(fArray, 0);
            this.cam1.viewSPL(fArray2, 0);
        }
    }

    void shakeScreenFast(int n, float f, float f2, float f3, float f4, float f5) {
        float f6 = this.cam1.getTranslateX();
        float f7 = this.cam1.getTranslateY();
        float f8 = this.cam1.getTranslateZ();
        int n2 = n / 24;
        float[] fArray = new float[]{1.0f, f6, f7, f8, 2.0f, f6 - (f4 *= 0.001f), f7, f8 - f4, 3.0f, f6, f7, f8, 4.0f, f6 + f4, f7, f8 + f4, 5.0f, f6, f7, f8, 6.0f, f6 - f4, f7, f8 - f4, 7.0f, f6, f7, f8, 8.0f, f6 + f4, f7, f8 + f4, 9.0f, f6, f7, f8, 10.0f, f6 - f4, f7, f8 - f4, 11.0f, f6, f7, f8, 12.0f, f6 + f4, f7, f8 + f4};
        float[] fArray2 = new float[]{1.0f, f, f2, f3, 2.0f, f, f2 + (f5 *= 0.001f), f3, 4.0f, f - f5, f2 - f5, f3, 6.0f, f - f5 * 2.5f, f2 + f5, f3, 8.0f, f - f5, f2 - f5, f3, 10.0f, f, f2 + f5, f3, 12.0f, f + f5, f2 - f5, f3, 14.0f, f + f5 * 2.5f, f2 + f5, f3, 16.0f, f + f5, f2 - f5, f3, 18.0f, f, f2 + f5, f3, 20.0f, f - f5, f2 - f5, f3, 22.0f, f - f5 * 2.5f, f2 + f5, f3};
        int n3 = 0;
        while (n3 < n / 24) {
            this.cam1.transSPL(fArray, 0);
            this.cam1.viewSPL(fArray2, 0);
            System.sleep(12);
            this.cam1.transSPL(fArray, 0);
            System.sleep(12);
            ++n3;
        }
        if (n % 24 != 0) {
            this.cam1.transSPL(fArray, 0);
            this.cam1.viewSPL(fArray2, 0);
        }
    }

    void shakeScreenFastViewOnly(int n, float f, float f2, float f3, float f4) {
        int n2 = n / 24;
        float[] fArray = new float[]{1.0f, f, f2, f3, 2.0f, f, f2 + (f4 *= 0.001f), f3, 4.0f, f - f4, f2 - f4, f3, 6.0f, f - f4 * 2.5f, f2 + f4, f3, 8.0f, f - f4, f2 - f4, f3, 10.0f, f, f2 + f4, f3, 12.0f, f + f4, f2 - f4, f3, 14.0f, f + f4 * 2.5f, f2 + f4, f3, 16.0f, f + f4, f2 - f4, f3, 18.0f, f, f2 + f4, f3, 20.0f, f - f4, f2 - f4, f3, 22.0f, f - f4 * 2.5f, f2 + f4, f3};
        int n3 = 0;
        while (n3 < n / 24) {
            this.cam1.viewSPL(fArray, 0);
            System.sleep(12);
            System.sleep(12);
            ++n3;
        }
        if (n % 24 != 0) {
            this.cam1.viewSPL(fArray, 0);
        }
    }

    void tSPL(int n, float f, float f2, float f3) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0);
    }

    void tSPL(int n, float f, float f2, float f3, int n2) {
        this.trans[0] = 1.0f;
        this.trans[1] = this.cam1.getTranslateX();
        this.trans[2] = this.cam1.getTranslateY();
        this.trans[3] = this.cam1.getTranslateZ();
        this.trans[4] = n;
        this.trans[5] = f;
        this.trans[6] = f2;
        this.trans[7] = f3;
        this.cam1.transSPL(this.trans, 0, n2, n);
    }

    void waitCameraEnd(int n) {
        while (n != this.CameraEnd) {
            System.sleep(1);
        }
    }

    void waitCameraPlay(int n) {
        while (true) {
            if (n == this.CameraPlay) {
                ++this.cut;
                break;
            }
            System.sleep(1);
        }
        Runtime.setRegister(1, this.cut);
        System.println(">>>>>>>> CUT /[$1]");
        this.DefocusClear();
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Characters
            extends Chr {
        Chr face;

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setVisible(false);
            this.start(4, null);
        }

        void allen_c2() {
            this.setCollision(true);
            this.mtn(398, 8, 0.6f, false);
            this.move(SCE01044A.this.cut_length, this.px, this.pz - 2.7f, true);
        }

        void allen_c3() {
            this.setCollision(true);
            this.mtn(398, 8, 0.6f, false);
            this.move(SCE01044A.this.cut_length - 2, this.px, this.pz - 2.7f, true);
        }

        void andrew_c2() {
            this.mtn(399, 8, 0.6f, false);
            this.move(SCE01044A.this.cut_length, this.px, this.pz - 3.0f, true);
        }

        void andrew_c44() {
            this.mtn(399, 8, 0.6f, false);
            this.move(90, -0.95f, 4.0f, true);
            this.mtn(394, 8, 1.0f, false);
            this.setRotateY(-160.0f);
        }

        void chaos_c41_42() {
            this.mtn(405, 0, 1.0f, true);
            this.mtn(405, 0, -1.0f, true);
        }

        void hammer_c83() {
            this.mtn(350, 0, 150, 0, 0, 1.0f, true);
            this.mtn(350, 75, 150, 0, 0, -1.0f, true);
            this.mtn(350, 75, 90, 0, 0, 1.0f, true);
        }

        void kosmos_c26() {
            this.mtn(292, 155, 240, 0, 0, 1.0f, true);
            this.mtn(292, 222, 240, 0, 0, -0.5f, true);
        }

        void kosmos_c28() {
            this.mtn(296, 0, 104, 0, 0, -1.0f, true);
            this.mtn(296, 0, 104, 0, 0, 1.0f, true);
        }

        void kosmos_c31() {
            System.sleep(30);
            int n = 0;
            while (n < 70) {
                this.setTranslate(this.px, -0.5f + 0.007142857f * (float) n, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void shion_c2() {
            this.setCollision(true);
            this.mtn(397, 8, 0.6f, false);
            this.move(SCE01044A.this.cut_length - 2, this.px, this.pz - 3.0f, true);
        }

        void shion_h_c24() {
            SCE01044A.this.shion_h.mtn(291, 225, 289, 0, 0, -1.0f, true);
            SCE01044A.this.shion_h.mtn(291, 225, 289, 0, 0, 1.0f, true);
            SCE01044A.this.shion_h.mtn(291, 225, 289, 0, 0, -1.0f, true);
            SCE01044A.this.shion_h.mtn(291, 225, 289, 0, 0, 1.0f, true);
            SCE01044A.this.shion_h.mtn(291, 290, 360, 0, 0, 1.0f, true);
            SCE01044A.this.shion_h.mtn(291, 325, 360, 0, 0, -1.0f, true);
        }

        void tonny_h_c111() {
            this.setCollision(true);
            this.mtn(402, 8, 0.7f, false);
            this.move(SCE01044A.this.cut_length, this.px, this.pz - 2.0f, true);
        }

        void tonny_h_c39() {
            this.mtn(304, 236, 255, 0, 0, 0.22f, true);
        }
    }

    class Chr_unit
            extends Chr {
        public Chr_unit(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.start(4, null);
            this.setVisible(false);
            this.setShadow(0, 0);
        }

        void c113() {
            this.setTranslate(0.0f, 0.0f, -1.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(true);
            this.move(240, 0.0f, 4.0f, true);
        }

        void c56() {
            int n = 0;
            while (n < SCE01044A.this.cut_length) {
                this.setTranslate(this.px + 0.1f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void initMTN() {
            this.mtn(393, 8, 1.0f, false);
        }

        void star_rot() {
            this.setVisible(true);
            this.setRotate(0.0f, 0.0f, 0.0f);
            int n = 0;
            while (n < 600) {
                this.setRotate(this.rx, this.ry + 1.0f, this.rz);
                System.sleep(1);
                ++n;
            }
        }

        void stars_inBridge() {
            this.setVisible(true);
            int n = SCE01044A.this.CameraPlay;
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                this.getTranslate();
                if (n == SCE01044A.this.CameraPlay - 1) {
                    this.setTranslate(0.0f, 0.0f, -10.0f);
                    n = SCE01044A.this.CameraPlay;
                } else {
                    this.setTranslate(this.px, this.py, this.pz + 0.8f);
                }
                System.sleep(1);
            }
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setVisible(false);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
        }

        void closeL() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px - 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void closeR() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px + 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void openL() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px + 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void openR() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px - 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
        }

        void open_closeL() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px + 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
            System.sleep(60);
            int n2 = 0;
            while (n2 < 45) {
                this.setTranslate(this.px - 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n2;
            }
        }

        void open_closeR() {
            int n = 0;
            while (n < 45) {
                this.setTranslate(this.px - 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n;
            }
            System.sleep(60);
            int n2 = 0;
            while (n2 < 45) {
                this.setTranslate(this.px + 0.016444445f, this.py, this.pz);
                System.sleep(1);
                ++n2;
            }
        }

        void openedL() {
            this.setTranslate(this.px + 0.74f, this.py, this.pz);
        }

        void openedR() {
            this.setTranslate(this.px - 0.74f, this.py, this.pz);
        }
    }

    class Com_seat
            extends Unit {
        public Com_seat(int n) {
            this.mapUnit(n);
            this.start(4, null);
        }
    }

    class Com_seat2
            extends MAPUnit {
        public Com_seat2(int n) {
            this.init(n);
            this.start(4, null);
        }
    }
}

