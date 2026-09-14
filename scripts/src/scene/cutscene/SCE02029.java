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
import xeno.util.Spline;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02029
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02029,
        MC_ELS01_PRJ,
        FLSshion,
        FLSmatehws_h,
        FLSmomo,
        FLSziggy,
        FLSallen,
        FLShammer_h,
        FLStonny_h,
        FLSchaos,
        FLSkosmos {
    Characters mathews;
    Characters hammer;
    Characters tonny;
    Characters chaos;
    Characters shion;
    Characters allen;
    Characters kosmos;
    Characters ziggy;
    Characters momo;
    Chr space;
    Chr monidev1;
    Chr monidev2;
    Chr monidev3;
    Unit elsa;
    MAPUnit doorL;
    MAPUnit doorR;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int cut_length;
    Light light = new Light(0);
    Effect elsa_fx;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02029() {
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.32f, 0.32f, 0.32f);
        this.light.setColor(1, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(1, -0.786f, 0.036f, 0.617f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, 0.656f, 0.711f, -0.251f);
        this.light.setColor(3, 0.39f, 0.39f, 0.39f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-0.94f, -0.07f, 0.65f);
        this.cam1.setRotate(6.0f, -61.0f, 0.0f);
        this.cam1.setFov(50.0f);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.124f, 0.0f, -0.992f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.421f, -0.777f, -0.468f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.467f, 0.551f, 0.692f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 44880, 1);
        this.cam1.setTranslate(-0.25f, 1.0f, 1.01f);
        this.cam1.setRotate(-2.5f, -265.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.025f, 0.0f, -1.0f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.263f, -0.746f, -0.612f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.53f, 0.725f, 0.44f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 15880, 1);
        this.cam1.setTranslate(-2.14f, 0.73f, -1.1f);
        this.cam1.setRotate(2.0f, -150.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.686f, 0.398f, -0.609f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.498f, -0.474f, 0.726f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.376f, 0.473f, 0.797f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 35880, 1);
        this.cam1.setTranslate(-0.22f, 0.97f, 5.04f);
        this.cam1.setRotate(-3.5f, -9.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.336f, 0.0f, -0.942f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.522f, -0.852f, 0.031f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.307f, 0.796f, 0.521f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-0.51f, 0.73f, 2.56f);
        this.cam1.setRotate(10.0f, -130.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.728f, 0.001f, -0.685f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.65f, -0.758f, -0.049f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.353f, 0.408f, 0.842f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 88880, 1);
        this.cam1.setTranslate(-0.74f, 0.75f, 2.7f);
        this.cam1.setRotate(3.5f, -43.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.336f, 0.0f, -0.942f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.522f, -0.852f, 0.031f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.307f, 0.796f, 0.521f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 103880, 1);
        this.cam1.setTranslate(0.17f, 0.73f, 3.17f);
        this.cam1.setRotate(3.5f, -135.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.728f, 0.001f, -0.685f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.65f, -0.758f, -0.049f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.353f, 0.408f, 0.842f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 73880, 1);
        this.cam1.setTranslate(-0.56f, 0.75f, 2.84f);
        this.cam1.setRotate(4.0f, -37.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.336f, 0.0f, -0.942f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.522f, -0.852f, 0.031f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.307f, 0.796f, 0.521f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 100880, 1);
        this.cam1.setTranslate(-0.11f, 0.84f, 3.83f);
        this.cam1.setRotate(4.5f, -113.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.728f, 0.001f, -0.685f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.65f, -0.758f, -0.049f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.353f, 0.408f, 0.842f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 63880, 1);
        this.cam1.setTranslate(-0.15f, 0.52f, 3.31f);
        this.cam1.setRotate(12.0f, -12.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.728f, 0.001f, -0.685f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.65f, -0.758f, -0.049f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.353f, 0.408f, 0.842f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 57880, 1);
        this.cam1.setTranslate(-1.19f, 0.27f, 5.0f);
        this.cam1.setRotate(18.5f, -401.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -1.64f, 0.27f, 4.41f);
        this.rSPL(this.cut_length, 18.5f, -423.0f, 0.0f);
        this.waitCameraPlay(12);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.695f, 0.001f, -0.719f);
        this.light.setColor(2, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(2, -0.282f, -0.764f, 0.58f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.145f, 0.676f, 0.722f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 156880, 1);
        this.cam1.setTranslate(-2.1f, 0.49f, 1.63f);
        this.cam1.setRotate(-3.5f, 32.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.093f, 0.0f, -0.996f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.409f, -0.895f, 0.18f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.483f, 0.779f, 0.399f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 23880, 1);
        this.cam1.setTranslate(-2.33f, 1.1f, -0.02f);
        this.cam1.setRotate(-5.0f, 212.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.395f, 0.0f, -0.919f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.658f, -0.751f, 0.056f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.44f, 0.762f, 0.475f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 38880, 1);
        this.cam1.setTranslate(0.75f, 0.64f, 1.03f);
        this.cam1.setRotate(2.0f, 98.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(135, 2.0f, 132.0f, 0.0f, 3);
        this.waitCameraPlay(15);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.536f, 0.207f, -0.818f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.55f, -0.823f, 0.142f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.453f, 0.562f, -0.692f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-1.92f, 1.02f, 2.39f);
        this.cam1.setRotate(1.0f, 185.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(16);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.488f, 0.279f, -0.827f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.762f, -0.607f, -0.227f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.591f, 0.628f, 0.506f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(-0.95f, 1.44f, 6.68f);
        this.cam1.setRotate(-3.0f, 187.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(135);
        this.tSPL(150, 0.29f, 1.44f, 6.53f, 3);
        this.waitCameraPlay(17);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.704f, 0.402f, -0.586f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.305f, -0.648f, 0.698f);
        this.light.setColor(3, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(3, -0.413f, 0.581f, 0.701f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.cam1.setTranslate(0.42f, 1.72f, 9.19f);
        this.cam1.setRotate(-15.5f, 361.0f, 0.0f);
        this.cam1.setFov(36.0f);
        this.waitCameraPlay(18);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.988f, 0.0f, -0.157f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.263f, -0.815f, 0.516f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.511f, 0.585f, 0.63f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 98880, 1);
        this.cam1.setTranslate(-1.38f, 1.07f, 4.53f);
        this.cam1.setRotate(-5.5f, 368.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(19);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.36f, 0.135f, -0.923f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, -0.551f, -0.696f, -0.46f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.44f, 0.673f, 0.594f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 19880, 1);
        this.cam1.setTranslate(-1.49f, 0.89f, 1.48f);
        this.cam1.setRotate(2.5f, 544.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(20);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.465f, 0.312f, -0.829f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.4f, -0.606f, -0.688f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.481f, 0.664f, 0.573f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 49880, 1);
        this.cam1.setTranslate(-0.27f, 1.15f, 5.08f);
        this.cam1.setRotate(5.5f, 527.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(21);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.465f, 0.312f, -0.829f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.4f, -0.606f, -0.688f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.481f, 0.664f, 0.573f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 121880, 1);
        this.cam1.setTranslate(-0.42f, 1.74f, 6.39f);
        this.cam1.setRotate(-4.0f, 541.0f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(45);
        this.tSPL(60, -0.77f, 1.22f, 6.37f, 3);
        this.waitCameraPlay(22);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.529f, 0.0f, -0.848f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.683f, -0.703f, 0.195f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.379f, 0.613f, 0.694f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 101880, 1);
        this.cam1.setTranslate(-0.65f, 1.05f, 3.72f);
        this.cam1.setRotate(-2.0f, 433.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(23);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.465f, 0.312f, -0.829f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.4f, -0.606f, -0.688f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.481f, 0.664f, 0.573f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 81880, 1);
        this.cam1.setTranslate(-0.65f, 1.32f, 5.64f);
        this.cam1.setRotate(2.0f, 542.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(24);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.25f, 0.0f, -0.968f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.817f, -0.339f, -0.467f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.398f, 0.736f, 0.547f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 46880, 1);
        this.cam1.setTranslate(0.3f, 0.43f, 2.12f);
        this.cam1.setRotate(9.5f, 494.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(90, 9.5f, 490.5f, 0.0f, 3);
        this.waitCameraPlay(25);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.553f, 0.001f, -0.833f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.587f, -0.807f, 0.058f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.042f, 0.607f, 0.794f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-1.15f, 0.8f, 4.01f);
        this.cam1.setRotate(15.0f, 408.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(26);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.18f, 0.0f, -0.984f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.299f, -0.705f, -0.643f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.557f, 0.757f, 0.343f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 112880, 1);
        this.cam1.setTranslate(-1.41f, 1.15f, 3.36f);
        this.cam1.setRotate(-8.0f, 165.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(27);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.621f, 0.0f, -0.784f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.632f, -0.664f, 0.4f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.319f, 0.522f, 0.791f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 39880, 1);
        this.cam1.setTranslate(0.03f, 0.85f, 5.11f);
        this.cam1.setRotate(-3.0f, 36.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.rSPL(this.cut_length - 30, -3.0f, 9.0f, 0.0f, 3);
        this.waitCameraPlay(28);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.018f, 0.0f, -1.0f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, -0.611f, -0.791f, 0.019f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.545f, 0.742f, 0.392f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 28880, 1);
        this.cam1.setTranslate(-0.97f, 1.11f, -0.04f);
        this.cam1.setRotate(-6.5f, -153.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(29);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.048f, 0.0f, -0.999f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, -0.657f, -0.747f, -0.101f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.648f, 0.609f, 0.458f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 54880, 1);
        this.cam1.setTranslate(0.17f, 0.84f, 2.58f);
        this.cam1.setRotate(2.5f, -154.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(30);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, -0.018f, 0.0f, -1.0f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, 0.595f, -0.791f, -0.142f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, 0.417f, 0.666f, 0.618f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 42880, 1);
        this.cam1.setTranslate(-2.14f, 0.76f, -0.43f);
        this.cam1.setRotate(1.0f, -196.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(31);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.611f, 0.224f, -0.759f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, 0.582f, -0.801f, 0.14f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.319f, 0.486f, 0.814f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 50880, 1);
        this.cam1.setTranslate(-2.42f, 1.01f, 3.57f);
        this.cam1.setRotate(-7.0f, -338.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(32);
        this.light.setColor(0, 0.22f, 0.22f, 0.22f);
        this.light.setColor(1, 0.57f, 0.57f, 0.57f);
        this.light.setDirection2(1, 0.213f, 0.0f, -0.977f);
        this.light.setColor(2, 0.27f, 0.27f, 0.27f);
        this.light.setDirection2(2, -0.535f, -0.818f, -0.211f);
        this.light.setColor(3, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(3, -0.314f, 0.75f, 0.583f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 66880, 1);
        this.cam1.setTranslate(-3.44f, 0.86f, 0.69f);
        this.cam1.setRotate(1.5f, -163.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, -3.44f, 1.0f, 0.69f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
    }

    void PlayPoint_c19() {
        this.mathews.setTranslate(-1.7f, -0.5f, 3.25f);
        this.mathews.setRotateY(90.0f);
        this.mathews.mtn(276, 225, 330, 0, 0, 1.0f, true);
        this.chaos.setVisible(true);
        this.shion.setVisible(true);
    }

    void PlayPoint_c24() {
        this.mathews.setTranslate(-1.7f, -0.5f, 3.25f);
        this.mathews.setRotateY(0.0f);
        this.shion.setTranslate(1.05f, 0.0f, 8.15f);
        this.shion.setRotate(0.0f, -159.0f, 0.0f);
        this.ziggy.setTranslate(-0.3f, 0.0f, 7.55f);
        this.ziggy.setRotate(0.0f, -180.0f, 0.0f);
        this.kosmos.setTranslate(0.45f, 0.0f, 9.6f);
        this.kosmos.setRotateY(160.0f);
        this.momo.setTranslate(-0.8f, 0.0f, 7.6f);
        this.momo.setRotateY(185.0f);
        this.chaos.setVisible(true);
        this.shion.setVisible(true);
        this.kosmos.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
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
        System.println("XEVEFLAG:EV02029_F");
        Runtime.setFlags(142, 1, 1);
        System.println("XEVEJNAME:SCE02030");
        Runtime.jumpEvent(2300);
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
        this.mathews = new Characters(0x1000140, -1.65f, -0.5f, -1.4f, 0.0f);
        this.hammer = new Characters(0x1000155, 2.35f, 0.0f, 6.35f, -180.0f);
        this.tonny = new Characters(16777556, 0.0f, -0.5f, 0.8f, 0.0f);
        this.chaos = new Characters(0x1000003, 0.0f, 0.0f, 12.0f, 180.0f);
        this.shion = new Characters(0x1000001, -0.5f, 0.0f, 12.5f, 180.0f);
        this.allen = new Characters(0x1000107, -2.6f, -0.5f, 1.45f, 180.0f);
        this.kosmos = new Characters(0x1000002, 0.0f, 0.0f, 14.0f, 180.0f);
        this.ziggy = new Characters(0x1000006, 0.0f, 0.0f, 15.0f, 180.0f);
        this.momo = new Characters(0x1000004, 0.0f, 0.0f, 16.0f, 180.0f);
        this.space = new Chr_units(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monidev1 = new Chr_units(24579, -3.6f, 0.37f, 1.42f, 0.0f);
        this.monidev1.setRotate(-85.0f, 16.0f, 73.0f);
        this.monidev1.setScale(10.0f, 10.0f, 10.0f);
        this.monidev2 = new Chr_units(24579, -3.62f, 0.4f, 1.45f, 0.0f);
        this.monidev2.setRotate(-87.5f, 25.0f, 3.0f);
        this.monidev2.setScale(10.0f, 10.0f, 10.0f);
        this.monidev3 = new Chr_units(24579, -3.55f, 0.4f, 1.22f, 0.0f);
        this.monidev3.setRotate(-76.5f, 23.0f, 49.0f);
        this.monidev3.setScale(10.0f, 10.0f, 10.0f);
        this.elsa = new Units(20482, 0.0f, 0.0f, 0.0f, 0.0f);
        this.elsa.setVisible(false);
        this.doorL = new Mapunits(4);
        this.doorR = new Mapunits(5);
        this.loadarc(this.mathews.face, "FLSmatehws_h.fpk");
        this.loadarc(this.hammer.face, "FLShammer_h.fpk");
        this.loadarc(this.tonny.face, "FLStonny_h.fpk");
        this.loadarc(this.chaos.face, "FLSchaos.fpk");
        this.loadarc(this.shion.face, "FLSshion.fpk");
        this.loadarc(this.allen.face, "FLSallen.fpk");
        this.loadarc(this.kosmos.face, "FLSkosmos.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.cam1.change();
        this.elsa_fx = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.elsa_fx.setCaster(this.elsa);
        this.elsa_fx.setTranslate(0.0f, -0.03f, -0.06f);
        this.elsa_fx.setScale(0.1f, 0.1f, 0.1f);
        this.elsa_fx.disp(false);
        this.chaos.setVisible(false);
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.ziggy.setVisible(false);
        this.momo.setVisible(false);
        Stage.setVisible(6, false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290030, 48000);
        this.space.start(1, "rotL");
        Stage.setVisible(-1, false);
        this.mathews.setVisible(false);
        this.hammer.setVisible(false);
        this.tonny.setVisible(false);
        this.allen.setVisible(false);
        this.elsa.start(1, "e_c1");
        this.CameraPlay = 1;
        System.sleep(180);
        this.tonny.setMotNoUpdate(2);
        this.tonny.mtn(260, 0, 0, 0, 0, 1.0f, true);
        this.tonny.setMotionFlags(0x800000, false);
        this.tonny.setMotionFlags(0x2000000, true);
        this.space.start(1, "stars_inBridge");
        Stage.setVisible(-1, true);
        Stage.setVisible(6, false);
        this.space.setRotateY(0.0f);
        this.elsa.setVisible(false);
        this.mathews.setVisible(true);
        this.hammer.setVisible(true);
        this.tonny.setVisible(true);
        this.allen.setVisible(true);
        this.CameraPlay = 2;
        this.face(this.allen.face, 2);
        this.allen.mtn(257, 0, 1.0f, true);
        this.mathews.mtn(258, 15, 570, 0, 0, 1.0f, true);
        System.sleep(105);
        this.msg.print("How's it look?");
        this.face(this.mathews.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(30);
        this.face(this.mathews.face, 2);
        System.sleep(15);
        this.msg.print("We got problems all over.");
        this.face(this.allen.face, 1, 0, 52, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("The entire radar system is down.");
        this.face(this.allen.face, 1, 0, 34, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("I bet the emitter's\ncompletely fried now.");
        this.face(this.allen.face, 1, 15, 73, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.tonny.setMotNoUpdate(0);
        this.CameraPlay = 3;
        this.hammer.mtn(259, 0, 178, 0, 0, 1.0f, true);
        this.tonny.mtn(260, 0, 178, 0, 0, 1.0f, true);
        this.msg.print("The generator output\nkeeps on dropping...");
        this.face(this.hammer.face, 1, 0, 54, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("I guess we ran the ship into the\nhyperspace column walls\na few times too many...");
        this.face(this.hammer.face, 1, 0, 91, 0, 0, 1.0f);
        this.waitclear(105);
        this.mathews.setTranslate(-1.65f, -0.5f, 0.6f);
        this.tonny.setRotateY(-15.0f);
        this.CameraPlay = 4;
        this.face(this.tonny.face, 2, 0, 80, 0, 0, 1.0f);
        this.hammer.mtn(259, 180, 363, 0, 0, 1.0f, true);
        this.tonny.mtn(260, 205, 388, 0, 0, 1.0f, true);
        this.msg.print("Thanks to a certain idiot who\nflies like a maniac...");
        this.waitclear(80);
        this.msg.print("What?! Hey!!");
        this.face(this.tonny.face, 5, 0, 105, 8, 1, 1.0f);
        this.waitclear(30);
        this.msg.print("Who do you think it was\nthat saved us?!");
        this.waitclear(75);
        this.tonny.setVisible(false);
        this.tonny.setMotNoUpdate(2);
        this.tonny.setTranslate(0.1f, -0.5f, 1.55f);
        this.tonny.mtn(261, 0, 0, 0, 0, 1.0f, true);
        this.tonny.setMotionFlags(0x800000, false);
        this.tonny.setMotionFlags(0x2000000, true);
        this.CameraPlay = 5;
        this.hammer.start(1, "c5_3");
        this.msg.print("I wish you'd consider what the\nnavigator has to go through!");
        this.face(this.hammer.face, 5, 0, 74, 0, 0, 1.0f);
        this.waitclear(80);
        this.msg.print("I'm getting carpal tunnel\nthanks to you!");
        this.face(this.hammer.face, 5, 0, 60, 0, 0, 1.0f);
        this.waitclear(60);
        this.tonny.setVisible(true);
        this.CameraPlay = 6;
        this.tonny.mtn(261, 0, 60, 0, 0, 0.95f, true);
        this.msg.print("Why don't you just direct-link it?!");
        this.face(this.tonny.face, 5, 0, 65, 0, 0, 1.0f);
        this.waitclear(65);
        this.tonny.setMotNoUpdate(0);
        this.hammer.setTranslate(1.05f, -0.5f, 4.25f);
        this.hammer.setRotateY(-155.0f);
        this.CameraPlay = 7;
        this.hammer.mtn(262, 0, 105, 0, 0, 0.93f, true);
        this.msg.print("Isn't it obvious?!");
        this.face(this.hammer.face, 5, 0, 35, 0, 0, 1.0f);
        this.waitclear(40);
        this.msg.print("It's way cooler to navigate by hand!");
        this.face(this.hammer.face, 5, 0, 75, 0, 0, 1.0f);
        this.waitclear(75);
        this.CameraPlay = 8;
        this.tonny.start(1, "c8_7");
        this.msg.print("Are you stupid or something?");
        this.face(this.tonny.face, 7, 0, 55, 8, 1, 1.0f);
        this.waitclear(55);
        this.CameraPlay = 9;
        this.hammer.mtn(264, 0, 105, 0, 0, 0.97f, true);
        this.msg.print("Mind your own business!");
        this.face(this.hammer.face, 5, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("At least I'm more useful than a certain out-of-control lunatic!");
        this.face(this.hammer.face, 5, 0, 65, 0, 0, 1.0f);
        this.waitclear(65);
        this.tonny.setTranslate(0.1f, -0.5f, 1.55f);
        this.tonny.setRotateY(0.0f);
        this.CameraPlay = 10;
        this.tonny.mtn(265, 0, 1.0f, true);
        this.msg.print("Oh, okay now!");
        this.face(this.tonny.face, 5, 0, 30, 0, 0, 1.0f);
        this.waitclear(30);
        this.cut_length = 150;
        this.hammer.setTranslate(0.1f, -0.5f, 4.55f);
        this.CameraPlay = 11;
        this.face(this.hammer.face, 6, 0, 60, 0, 0, 1.0f);
        this.hammer.mtn(266, 0, 1.0f, true);
        this.msg.print("So you think you're the man, huh?");
        this.face(this.tonny.face, 5, 0, 60, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("You wanna go? Whatchoo got?");
        this.face(this.hammer.face, 5, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        this.face(this.tonny.face, 5, 0, 45, 0, 0, 1.0f);
        this.face(this.hammer.face, 5, 0, 45, 0, 0, 1.0f);
        System.sleep(45);
        this.allen.setVisible(false);
        this.mathews.setTranslate(-2.0f, -0.5f, 1.0f);
        this.mathews.setRotate(0.0f, 0.0f, 0.0f);
        this.CameraPlay = 12;
        this.mathews.mtn(267, 0, 58, 0, 0, 1.0f, true);
        this.msg.print("That's enough!");
        this.waitclear(45);
        System.sleep(15);
        this.allen.setVisible(true);
        this.hammer.setTranslate(0.1f, -0.5f, 4.25f);
        this.hammer.setRotateY(130.0f);
        this.tonny.setTranslate(0.6f, -0.5f, 2.95f);
        this.tonny.setRotateY(-60.0f);
        this.CameraPlay = 13;
        this.mathews.start(1, "c13_15_12");
        this.hammer.start(1, "c13_13");
        this.tonny.start(1, "c13_14");
        this.face(this.hammer.face, 2);
        this.face(this.tonny.face, 2);
        this.msg.print("Both of ya morons, shut up!");
        this.face(this.mathews.face, 9, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("I'm sick of listening to your whining!");
        this.face(this.mathews.face, 9, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        this.allen.setRotateY(90.0f);
        this.CameraPlay = 14;
        this.allen.mtn(271, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("My head hurts enough just\nthinking about what these\nrepairs are gonna cost!");
        this.face(this.mathews.face, 5, 0, 95, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(15);
        this.chaos.setVisible(true);
        this.chaos.setRotateY(195.0f);
        this.CameraPlay = 15;
        this.chaos.start(1, "c15_16_16");
        this.doorL.start(1, "open_closeL");
        this.doorR.start(1, "open_closeR");
        this.msg.print("The last thing I need is to\nbabysit the two of you!");
        this.face(this.mathews.face, 9, 10, 84, 0, 0, 1.0f);
        this.waitclear(75);
        this.face(this.mathews.face, 10, 75, 120, 0, 0, 1.0f);
        System.sleep(45);
        this.shion.setVisible(true);
        this.shion.setRotateY(165.0f);
        this.CameraPlay = 16;
        this.shion.start(1, "c16_17_17");
        System.sleep(30);
        this.msg.print("Uh...we're back.");
        this.face(this.chaos.face, 1, 0, 39, 0, 0, 1.0f);
        this.waitclear(60);
        this.face(this.chaos.face, 2);
        System.sleep(30);
        this.doorL.start(1, "openL");
        this.doorR.start(1, "openR");
        this.msg.print("Ohhh...");
        this.face(this.shion.face, 1, 0, 45, 0, 0, 1.0f);
        System.sleep(15);
        this.waitclear(30);
        this.msg.print("If I had known I'd be doing this,\nI would've spent more time\nat the firing range.");
        this.face(this.shion.face, 1, 0, 120, 0, 0, 1.0f);
        System.sleep(60);
        this.doorL.start(1, "closeL");
        this.doorR.start(1, "closeR");
        this.waitclear(60);
        System.sleep(30);
        this.mathews.setTranslate(-1.7f, -0.5f, 3.25f);
        this.mathews.setRotateY(90.0f);
        this.CameraPlay = 17;
        this.hammer.start(1, "c17_18");
        this.tonny.start(1, "c17_19");
        this.mathews.mtn(276, 30, 330, 0, 0, 1.0f, true);
        this.msg.print("Hmm? What's going on?");
        this.waitclear(45);
        System.sleep(30);
        this.allen.setRotateY(15.0f);
        this.CameraPlay = 18;
        this.face(this.allen.face, 2);
        this.allen.mtn(296, 8, 1.0f, true);
        this.msg.print("Nothing. Good work.");
        this.face(this.mathews.face, 1, 0, 55, 0, 0, 1.0f);
        this.waitclear(60);
        this.doorL.start(1, "openL");
        this.doorR.start(1, "openR");
        System.sleep(30);
        this.msg.print("So,");
        this.face(this.mathews.face, 1, 0, 10, 0, 0, 1.0f);
        this.waitclear(30);
        this.allen.setVisible(false);
        this.tonny.setVisible(false);
        this.hammer.setVisible(false);
        this.allen.setRotateY(90.0f);
        this.chaos.setTranslate(-1.35f, 0.0f, 7.55f);
        this.chaos.setRotate(0.0f, 160.0f, 0.0f);
        this.shion.setTranslate(1.05f, 0.0f, 8.15f);
        this.shion.setRotate(0.0f, -159.0f, 0.0f);
        this.kosmos.setTranslate(0.45f, 0.0f, 9.6f);
        this.kosmos.setRotate(0.0f, -180.0f, 0.0f);
        this.ziggy.setTranslate(-0.3f, 0.0f, 8.45f);
        this.ziggy.setRotate(0.0f, -180.0f, 0.0f);
        this.momo.setTranslate(-0.8f, 0.0f, 9.15f);
        this.momo.setRotate(0.0f, -180.0f, 0.0f);
        this.kosmos.setVisible(true);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.CameraPlay = 19;
        this.doorL.start(1, "closeL");
        this.doorR.start(1, "closeR");
        this.shion.start(1, "c19_21");
        this.kosmos.start(1, "c19_22");
        this.ziggy.mtn(279, 0, 88, 0, 0, 1.0f, true);
        this.momo.mtn(280, 0, 283, 0, 0, 1.0f, true);
        this.chaos.mtn(281, 30, 315, 0, 0, 1.0f, true);
        this.face(this.chaos.face, 2, 0, 105, 0, 0, 1.0f);
        this.face(this.shion.face, 2, 0, 105, 0, 0, 1.0f);
        this.face(this.ziggy.face, 2, 0, 105, 0, 0, 1.0f);
        this.face(this.momo.face, 2, 0, 105, 0, 0, 1.0f);
        this.msg.print("who are these guys?");
        this.waitclear(45);
        System.sleep(45);
        this.ziggy.setTranslate(-0.3f, 0.0f, 8.6f);
        this.CameraPlay = 20;
        this.ziggy.start(1, "c20_21_23");
        this.face(this.ziggy.face, 2);
        this.face(this.momo.face, 4);
        this.msg.print("She's MOMO,");
        this.face(this.chaos.face, 1, 0, 39, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("and this is Ziggy.");
        this.face(this.chaos.face, 1, 0, 39, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("Looks like they're both with\nthe Federation government.");
        this.face(this.chaos.face, 1, 0, 62, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.kosmos.setRotateY(160.0f);
        this.momo.setTranslate(-0.8f, 0.0f, 7.6f);
        this.momo.setRotateY(185.0f);
        this.ziggy.setTranslate(-0.3f, 0.0f, 8.6f);
        this.CameraPlay = 21;
        this.kosmos.start(1, "c21_26");
        this.momo.start(1, "c21_27");
        this.waitclear(60);
        System.sleep(45);
        this.msg.print("Nice to meet you.");
        this.face(this.momo.face, 1, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.mathews.setRotateY(0.0f);
        this.allen.setVisible(false);
        this.CameraPlay = 22;
        this.mathews.start(1, "c22_28");
        this.msg.print("You two were the ones in\nthat ship out there?");
        this.face(this.mathews.face, 1, 27, 95, 0, 0, 1.0f);
        this.waitclear(75);
        this.ziggy.setTranslate(-0.3f, 0.0f, 7.55f);
        this.allen.setVisible(true);
        this.CameraPlay = 23;
        this.momo.start(1, "c23_29");
        this.ziggy.start(1, "c23_30");
        System.sleep(15);
        this.msg.print("Thank you very much for rescuing us.");
        this.face(this.momo.face, 1, 10, 75, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.msg.print("Thanks for your help.");
        this.face(this.ziggy.face, 1, 0, 15, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.ziggy.face, 1, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        System.sleep(15);
        this.chaos.setTranslate(-1.6f, -0.5f, 5.45f);
        this.chaos.setRotate(0.0f, 183.0f, 0.0f);
        this.CameraPlay = 24;
        this.mathews.mtn(287, 0, 175, 0, 0, 1.0f, true);
        this.chaos.start(1, "c24_32");
        this.msg.print("They were being held by the\nU-TIC Organization, but\nthey managed to escape.");
        this.face(this.chaos.face, 1, 0, 106, 0, 0, 1.0f);
        this.waitclear(115);
        this.CameraPlay = 25;
        this.msg.print("The U-TIC Organization?!");
        this.face(this.mathews.face, 9, 0, 46, 0, 0, 1.0f);
        this.waitclear(60);
        this.chaos.setTranslate(-1.6f, -0.5f, 4.45f);
        this.CameraPlay = 26;
        this.face(this.chaos.face, 1, 0, 0, 0, 0, 1.0f);
        this.chaos.mtn(289, 30, 120, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Captain...");
        this.face(this.chaos.face, 1, 0, 26, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.cut_length = 150;
        this.mathews.setTranslate(-1.7f, -0.5f, 3.25f);
        this.mathews.setRotateY(150.0f);
        this.tonny.setVisible(false);
        this.hammer.setVisible(false);
        this.allen.setVisible(true);
        this.allen.setTranslate(-2.6f, -0.45f, 1.6f);
        this.allen.setRotateY(60.0f);
        this.CameraPlay = 27;
        this.face(this.mathews.face, 2, 0, 0, 0, 0, 1.0f);
        this.mathews.mtn(290, 0, 148, 0, 0, 1.0f, true);
        this.allen.start(1, "c27_35");
        this.face(this.mathews.face, 1, 10, 111, 0, 0, 1.0f);
        System.sleep(30);
        this.msg.print("Never thought I'd have to hear\nthat cursed name again.");
        this.waitclear(75);
        System.sleep(45);
        this.hammer.setTranslate(1.1f, -0.5f, 4.25f);
        this.hammer.setRotateY(180.0f);
        this.tonny.setTranslate(1.15f, -0.5f, 2.9f);
        this.tonny.setRotateY(230.0f);
        this.hammer.setVisible(true);
        this.tonny.setVisible(true);
        this.shion.setVisible(false);
        this.kosmos.setVisible(false);
        this.CameraPlay = 28;
        this.face(this.mathews.face, 2, 0, 90, 0, 0, 1.0f);
        this.face(this.tonny.face, 2);
        this.mathews.mtn(290, 175, 535, 0, 0, 1.0f, true);
        this.hammer.mtn(292, 40, 403, 0, 0, 1.0f, true);
        this.tonny.mtn(293, 40, 405, 0, 0, 1.0f, true);
        this.msg.print("What is it? That U...whatever?");
        this.face(this.hammer.face, 1, 0, 16, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.hammer.face, 1, 0, 35, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("Nothing you need to worry about.");
        this.face(this.mathews.face, 1, 0, 50, 0, 0, 1.0f);
        this.waitclear(50);
        this.msg.print("Stop wasting time\nand plot our course already.");
        this.face(this.mathews.face, 1, 0, 84, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.msg.print("You heard him. It's nothing a stupid\nnavigator needs to worry about.");
        this.face(this.tonny.face, 1, 0, 93, 0, 0, 1.0f);
        this.waitclear(105);
        System.sleep(30);
        this.CameraPlay = 29;
        this.face(this.hammer.face, 6, 0, 120, 8, 9, 1.0f);
        this.hammer.mtn(292, 405, 568, 0, 0, 1.0f, true);
        System.sleep(165);
        this.hammer.setTranslate(-1.37f, -0.5f, 2.55f);
        this.hammer.setRotateY(225.0f);
        this.allen.setTranslate(-3.0f, -0.5f, 1.9999999f);
        this.allen.setRotateY(150.0f);
        this.mathews.setVisible(false);
        this.tonny.setVisible(false);
        this.chaos.setVisible(false);
        this.CameraPlay = 30;
        this.face(this.hammer.face, 6);
        this.face(this.allen.face, 2);
        this.allen.mtn(294, 0, 1.0f, true);
        this.hammer.mtn(295, 0, 1.0f, true);
        System.sleep(120);
        this.CameraPlay = 31;
        System.sleep(25);
        this.msg.print("Allen!");
        this.waitclear(45);
        this.msg.print("Hurry up and clean up the top of\nthe panel! This stuff's in the way!");
        System.sleep(60);
        this.waitclear(10);
        this.msg.print("Wha...? You're the one\nwho left these here.");
        this.face(this.allen.face, 1, 0, 15, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.allen.face, 1, 0, 73, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.cut_length = 120;
        this.shion.setVisible(true);
        this.kosmos.setVisible(false);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.shion.setRotateY(195.0f);
        this.ziggy.setRotateY(195.0f);
        this.momo.setRotateY(195.0f);
        this.CameraPlay = 32;
        this.msg.print("Don't take it out on me!");
        this.face(this.allen.face, 7, 0, 73, 8, 1, 1.0f);
        this.waitclear(90);
        System.sleep(30);
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

    void s() {
        System.waitSignal(this.msg, 255);
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
        System.println(">>>>>>> CUT /[$1]");
        this.DefocusClear();
    }

    void waitclear(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class Characters
            extends Chr {
        Chr face;
        Spline movSPL = Spline.create();

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(9, 48);
        }

        void c13_13() {
            this.mtn(269, 8, 1.0f, true);
        }

        void c13_14() {
            this.mtn(270, 8, 1.0f, true);
        }

        void c13_15_12() {
            this.mtn(268, 0, 1.0f, true);
        }

        void c15_16_16() {
            this.mtn(272, 0, 1.0f, true);
        }

        void c16_17_17() {
            this.mtn(273, 0, 1.0f, true);
        }

        void c17_18() {
            this.mtn(274, 8, 1.0f, true);
        }

        void c17_19() {
            this.mtn(275, 8, 1.0f, true);
        }

        void c19_21() {
            this.mtn(277, 8, 1.0f, true);
        }

        void c19_22() {
            this.mtn(278, 0, 1.0f, true);
        }

        void c20_21_23() {
            this.mtn(279, 100, 155, 0, 0, 1.0f, true);
            this.mtn(279, 100, 155, 0, 0, -1.0f, true);
            this.mtn(279, 100, 155, 0, 0, 1.0f, true);
            this.mtn(279, 100, 155, 0, 0, -1.0f, true);
            this.mtn(279, 100, 155, 0, 0, 1.0f, true);
            this.mtn(279, 100, 155, 0, 0, -1.0f, true);
            this.mtn(279, 100, 130, 0, 0, 1.0f, true);
        }

        void c21_26() {
            this.mtn(282, 8, 1.0f, true);
        }

        void c21_27() {
            this.mtn(283, 0, 1.0f, true);
        }

        void c22_28() {
            this.mtn(284, 0, 1.0f, true);
        }

        void c23_29() {
            this.mtn(285, 0, 1.0f, true);
        }

        void c23_30() {
            this.mtn(286, 0, 180, 0, 0, 1.0f, true);
        }

        void c24_32() {
            this.mtn(288, 0, 1.0f, true);
        }

        void c27_35() {
            this.mtn(291, 0, 1.0f, true);
        }

        void c5_3() {
            this.mtn(259, 390, 510, 0, 0, 1.0f, true);
            this.mtn(259, 500, 510, 0, 0, -0.5f, true);
        }

        void c8_7() {
            this.mtn(263, 0, 45, 0, 0, 1.0f, true);
            this.mtn(263, 40, 45, 0, 0, -0.5f, true);
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
        }

        void rotL() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.01f);
                System.sleep(1);
            }
        }

        void rotR() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setRotateY(this.ry - 0.01f);
                System.sleep(1);
            }
        }

        void stars_inBridge() {
            int n = SCE02029.this.CameraPlay;
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                if (n == SCE02029.this.CameraPlay - 1) {
                    this.setTranslate(0.0f, 0.0f, -10.0f);
                    n = SCE02029.this.CameraPlay;
                } else {
                    this.setTranslate(this.px, this.py, this.pz + 1.0f);
                }
                System.sleep(1);
            }
        }

        void stars_inBridge_slow() {
            this.setTranslate(0.0f, 0.0f, -10.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz - 0.1f);
                System.sleep(1);
            }
        }

        void stars_inSpaceL() {
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry + 0.05f);
                System.sleep(1);
            }
        }

        void stars_inSpaceR() {
            this.setRotateY(0.0f);
            while (true) {
                this.setRotateY(this.ry - 0.05f);
                System.sleep(1);
            }
        }
    }

    class Units
            extends Unit {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void e_c1() {
            this.setVisible(true);
            this.move(180, 0.0f, 0.1f, true);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
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

        void set27_1() {
            this.setTranslate(-0.86f, -0.05f, 4.25f);
            this.setRotate(6.0f, 80.0f, -4.0f);
        }

        void set27_2() {
            this.setTranslate(-0.81f, -0.0f, 4.2f);
            this.setRotate(1.1f, 78.5f, -0.8f);
        }

        void star_move() {
            this.setTranslate(0.0f, 0.0f, 15.0f);
            this.move(180, 0.0f, 0.0f, true);
        }
    }
}

