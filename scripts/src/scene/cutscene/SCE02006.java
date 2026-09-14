import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Movie;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.map.MC_PRO01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02006
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack02006,
        MC_PRO01_PRJ,
        FLSziggy,
        FLSmomo_h {
    Characters ziggy;
    Characters momo;
    Agws agws1;
    Agws agws2;
    Agws agws3;
    Chr ship_A;
    Chr ship_B;
    Thread thread1;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    int cut_length;
    Light light = new Light(0);
    Movie movie;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02006() {
    }

    void CameraThread() {
        this.cam1.change();
        this.light.setColor(0, 0.23f, 0.23f, 0.23f);
        this.light.setColor(1, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(1, -0.112f, 0.0f, 0.994f);
        this.light.setColor(2, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(2, -0.326f, 0.0f, -0.945f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.735f, -0.3f, 0.608f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.waitCameraPlay(1);
        Runtime.setDefocusQuick(0, 1, 6880, 1);
        this.cam1.setTranslate(1.75f, 2.22f, 23.48f);
        this.cam1.setRotate(4.0f, -366.84f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(210, 3.23f, 2.22f, 23.66f, 3);
        System.sleep(105);
        this.rSPL(105, -3.0f, -383.84f, 0.0f, 3);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -0.288f, 0.283f, 0.915f);
        this.light.setColor(2, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(2, -0.59f, 0.001f, -0.808f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, -0.684f, -0.34f, 0.645f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 68880, 1);
        this.cam1.setTranslate(6.79f, 1.05f, 17.97f);
        this.cam1.setRotate(-13.5f, -66.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 6.79f, 1.22f, 17.97f);
        this.waitCameraPlay(3);
        Runtime.setDefocusQuick(0, 1, 80380, 1);
        this.cam1.setTranslate(7.31f, 1.05f, 17.9f);
        this.cam1.setRotate(-17.0f, -64.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, 0.471f, 0.266f, 0.841f);
        this.light.setColor(2, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(2, 0.593f, 0.001f, -0.805f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.74f, -0.507f, 0.442f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 52880, 1);
        this.cam1.setTranslate(9.47f, 0.65f, 18.1f);
        this.cam1.setRotate(6.0f, 62.5f, -1.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(104);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -0.328f, 0.274f, 0.904f);
        this.light.setColor(2, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(2, 0.879f, 0.369f, 0.301f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 0.225f, -0.612f, 0.758f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 63880, 1);
        this.cam1.setTranslate(7.43f, 0.72f, 18.58f);
        this.cam1.setRotate(12.0f, 22.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.388f, 0.326f, -0.862f);
        this.light.setColor(2, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(2, -0.925f, 0.312f, 0.219f);
        this.light.setColor(3, 0.12f, 0.12f, 0.12f);
        this.light.setDirection2(3, -0.53f, -0.474f, -0.703f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 106880, 1);
        this.cam1.setTranslate(6.63f, 1.27f, 16.37f);
        this.cam1.setRotate(-1.0f, -141.0f, 10.5f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(6);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -0.288f, 0.283f, 0.915f);
        this.light.setColor(2, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(2, -0.59f, 0.001f, -0.808f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, -0.684f, -0.34f, 0.645f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 2, 76264, 2);
        this.cam1.setTranslate(6.22f, 0.85f, 18.24f);
        this.cam1.setRotate(-10.0f, -63.5f, 5.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.23f, 0.23f, 0.23f);
        this.light.setColor(1, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(1, -0.112f, 0.0f, 0.994f);
        this.light.setColor(2, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(2, -0.326f, 0.0f, -0.945f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, -0.735f, -0.3f, 0.608f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 7880, 1);
        this.cam1.setTranslate(6.64f, 0.87f, 15.74f);
        this.cam1.setRotate(4.5f, 43.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 7.73f, 0.87f, 14.71f);
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, -0.684f, 0.628f, 0.372f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.493f, 0.228f, 0.84f);
        this.light.setColor(3, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(3, -0.684f, -0.341f, 0.645f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.cam1.setTranslate(6.98f, 0.98f, 18.69f);
        this.cam1.setRotate(-5.5f, -36.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 7.08f, 0.98f, 18.77f);
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, -0.328f, 0.274f, 0.904f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.879f, 0.37f, 0.301f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 0.225f, -0.612f, 0.758f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 25880, 1);
        Runtime.setDefocusQuick(1, 2, 62264, 2);
        this.cam1.setTranslate(10.82f, 0.6f, 19.95f);
        this.cam1.setRotate(2.96f, 48.11f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 11.28f, 0.6f, 19.32f);
        this.rSPL(this.cut_length, 2.96f, 60.61f, 0.0f);
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, 0.471f, 0.266f, 0.841f);
        this.light.setColor(2, 0.17f, 0.17f, 0.17f);
        this.light.setDirection2(2, 0.593f, 0.018f, -0.805f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.74f, -0.507f, 0.442f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 64880, 2);
        this.cam1.setTranslate(9.32f, 0.62f, 18.34f);
        this.cam1.setRotate(9.5f, 52.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(11);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.43f, 0.43f, 0.43f);
        this.light.setDirection2(1, -0.874f, 0.473f, -0.107f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, 0.493f, 0.227f, 0.84f);
        this.light.setColor(3, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(3, -0.18f, -0.57f, 0.802f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.cam1.setTranslate(8.12f, 0.75f, 18.45f);
        this.cam1.setRotate(-4.5f, -13.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(12);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -0.517f, 0.469f, 0.716f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.589f, 0.031f, -0.807f);
        this.light.setColor(3, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(3, -0.684f, -0.341f, 0.645f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 59880, 1);
        this.cam1.setTranslate(6.87f, 1.2f, 18.03f);
        this.cam1.setRotate(-14.0f, -62.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, -0.051f, 0.241f, 0.969f);
        this.light.setColor(2, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(2, -0.613f, 0.414f, -0.673f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 0.043f, -0.612f, 0.789f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.cam1.setTranslate(8.34f, 0.75f, 18.28f);
        this.cam1.setRotate(-8.75f, -2.6f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(14);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, -0.051f, 0.242f, 0.969f);
        this.light.setColor(2, 0.16f, 0.16f, 0.16f);
        this.light.setDirection2(2, -0.613f, 0.414f, -0.673f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.043f, -0.613f, 0.789f);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        Runtime.setDefocusQuick(0, 1, 30880, 1);
        this.cam1.setTranslate(7.48f, 0.38f, 19.94f);
        this.cam1.setRotate(16.5f, -27.5f, 0.0f);
        this.cam1.setFov(30.0f);
        System.sleep(165);
        this.tSPL(135, 7.95f, 0.38f, 20.12f);
        this.rSPL(135, 16.5f, -13.5f, 0.0f);
        this.waitCameraPlay(26);
        Runtime.setDefocusQuick(0, 1, 30880, 1);
        this.cam1.setTranslate(2.46f, 2.79f, 9.26f);
        this.cam1.setRotate(12.43f, -74.37f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(27);
        Runtime.setDefocusQuick(0, 1, 17880, 1);
        this.cam1.setTranslate(-0.52f, 4.27f, 11.51f);
        this.cam1.setRotate(-7.07f, -64.87f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 0.5f, 4.27f, 13.68f);
        this.waitCameraPlay(28);
        this.cam1.setTranslate(1.53f, 1.6f, 9.77f);
        this.cam1.setRotate(29.43f, -69.37f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(29);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, 0.372f, 0.277f, 0.886f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, -0.843f, 0.365f, 0.394f);
        this.light.setColor(3, 0.17f, 0.17f, 0.17f);
        this.light.setDirection2(3, 0.129f, -0.873f, 0.47f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 36880, 2);
        this.cam1.setTranslate(7.82f, 0.05f, 19.05f);
        this.cam1.setRotate(30.43f, -376.37f, 0.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 7.91f, 0.05f, 19.07f);
        this.waitCameraPlay(30);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.37f, 0.37f, 0.37f);
        this.light.setDirection2(1, 0.007f, 1.0f, -0.03f);
        this.light.setColor(2, 0.21f, 0.21f, 0.21f);
        this.light.setDirection2(2, 0.78f, 0.626f, 0.0f);
        this.light.setColor(3, 0.13f, 0.13f, 0.13f);
        this.light.setDirection2(3, -0.668f, -0.001f, -0.744f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(8.25f, 1.27f, 17.57f);
        this.cam1.setRotate(-58.57f, -171.87f, 21.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(31);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -0.24f, 0.192f, 0.952f);
        this.light.setColor(2, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(2, 0.566f, 0.001f, -0.824f);
        this.light.setColor(3, 0.23f, 0.23f, 0.23f);
        this.light.setDirection2(3, 0.372f, -0.908f, -0.192f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 91880, 1);
        this.cam1.setTranslate(9.19f, 0.07f, 18.23f);
        this.cam1.setRotate(36.43f, -289.37f, 8.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(32);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.42f, 0.42f, 0.42f);
        this.light.setDirection2(1, -0.156f, 0.953f, -0.258f);
        this.light.setColor(2, 0.14f, 0.14f, 0.14f);
        this.light.setDirection2(2, 0.843f, 0.0f, -0.538f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 0.251f, -0.637f, -0.729f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.setTranslate(8.66f, 0.85f, 17.44f);
        this.cam1.setRotate(-31.5f, 148.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(33);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, 0.471f, 0.266f, 0.841f);
        this.light.setColor(2, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(2, 0.593f, 0.018f, -0.805f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.74f, -0.507f, 0.442f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 50880, 1);
        this.cam1.setTranslate(9.53f, 0.8f, 18.66f);
        this.cam1.setRotate(12.5f, 58.5f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(34);
        Runtime.setDefocusQuick(0, 1, 79880, 1);
        this.cam1.setTranslate(7.84f, 1.4f, 18.06f);
        this.cam1.setRotate(16.93f, -282.87f, 0.0f);
        this.cam1.setFov(30.0f);
        this.waitCameraPlay(35);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.417f, 0.413f, 0.81f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.999f, 0.0f, 0.053f);
        this.light.setColor(3, 0.15f, 0.15f, 0.15f);
        this.light.setDirection2(3, 0.024f, -0.612f, 0.79f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 43880, 1);
        this.cam1.setTranslate(8.01f, 1.2f, 20.13f);
        this.cam1.setRotate(13.0f, 19.5f, -1.0f);
        this.cam1.setFov(30.0f);
        this.tSPL(this.cut_length, 8.01f, 0.85f, 20.13f);
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
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
        System.println("XEVEFLAG:EV02006_F");
        Runtime.setFlags(110, 1, 1);
        System.println("XEVEJNAME:CFJ2_B5 XEVEJPOINT:POINT2_B5");
        Runtime.jumpCF(9015, 0);
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
        Runtime.setLocation(1100);
        this.ziggy = new Characters(0x1000006, 7.7f, 0.0f, 17.32f, 0.0f);
        this.momo = new Characters(0x1000021, 8.5f, 0.0f, 17.32f, 0.0f);
        this.agws1 = new Agws(17412, 4.7f, 0.0f, 8.47f, -135.0f);
        this.agws2 = new Agws(17411, 2.1f, 0.0f, 11.62f, -45.0f);
        this.agws3 = new Agws(17411, 7.65f, 0.0f, 11.72f, -30.0f);
        this.ship_A = new Units(20497, -17.0f, -5.0f, -14.0f, 0.0f);
        this.ship_B = new Units(20591, -17.0f, -5.0f, -14.0f, 0.0f);
        this.loadarc(this.momo.face, "FLSmomo_h.fpk");
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.thread1 = Thread.create(this, "CameraThread");
        this.thread1.start();
        this.ziggy.signal(0);
        this.momo.signal(0);
        this.agws1.signal(0);
        this.agws2.signal(0);
        this.agws3.signal(0);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.cam1.setTranslate(0.0f, 0.0f, 0.0f);
        this.cam1.setRotate(0.0f, 0.0f, 0.0f);
        this.cam1.setFov(30.0f);
        this.cam1.change();
        this.ship_A.setVisible(false);
        this.ship_B.setVisible(false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1290097, 48000);
        this.momo.setShadow(0, 0);
        this.ziggy.setShadow(0, 0);
        this.face(this.momo.face, 34);
        this.cut_length = 255;
        this.CameraPlay = 1;
        this.agws1.start(1, "c1_1");
        this.agws2.start(1, "c1_2");
        this.agws3.start(1, "c1_3");
        this.ziggy.mtn(260, 8, 1.0f, true);
        this.momo.mtn(263, 0, 0, 0, 0, 1.0f, true);
        System.sleep(this.cut_length);
        this.momo.setShadow(7, 48);
        this.ziggy.setShadow(7, 48);
        this.momo.setMotionFlags(0x2000000, true);
        this.cut_length = 480;
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.CameraPlay = 2;
        this.face(this.momo.face, 34);
        this.ziggy.mtn(262, 0, 433, 0, 0, 1.0f, true);
        this.momo.mtn(263, 0, 433, 0, 0, 1.0f, true);
        System.sleep(30);
        this.msg.print("A.G.W.S. units...");
        this.face(this.ziggy.face, 1, 0, 15, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("This sure doesn't look like a simple resurgence of an armed group.");
        this.face(this.ziggy.face, 1, 0, 120, 0, 0, 1.0f);
        this.waitclear(135);
        System.sleep(15);
        this.msg.print("What should we do?");
        this.face(this.momo.face, 33, 0, 36, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("I don't know...");
        this.face(this.ziggy.face, 1, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        this.ziggy.look_speed(0.0f);
        this.ziggy.look_point(5.46f, 1.65f, 15.1f);
        this.msg.print("We won't be able to stay\nhere much longer.");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(30);
        this.ziggy.look_default();
        this.CameraPlay = 3;
        this.ziggy.mtn(260, 8, 1.0f, true);
        this.momo.mtn(263, 480, 643, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("...A large number of soldiers are\nmaking their way over here.");
        this.face(this.momo.face, 33, 0, 75, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(15);
        this.msg.print("At this rate, this room will be filled\nwith over a hundred soldiers\nin eight minutes.");
        this.face(this.momo.face, 33, 0, 45, 0, 0, 1.0f);
        System.sleep(45);
        this.CameraPlay = 4;
        this.face(this.ziggy.face, 2, 0, 105, 0, 0, 1.0f);
        this.ziggy.mtn(264, 0, 238, 0, 0, 1.0f, true);
        this.momo.mtn(263, 735, 930, 0, 0, 1.0f, true);
        this.waitclear(75);
        System.sleep(30);
        this.msg.print("Recon Realians are pretty handy.");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.agws1.setVisible(true);
        this.agws2.setVisible(true);
        this.agws3.setVisible(true);
        this.CameraPlay = 104;
        this.agws1.start(1, "c1_1");
        this.agws2.start(1, "c1_2");
        this.agws3.start(1, "c1_3");
        System.sleep(45);
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.ziggy.setTranslate(7.95f, 0.0f, 17.55f);
        this.CameraPlay = 5;
        this.ziggy.mtn(264, 240, 388, 0, 0, 1.0f, true);
        System.sleep(60);
        this.msg.print("I guess rushing them is\nout of the question.");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(15);
        this.CameraPlay = 6;
        this.ziggy.mtn(264, 390, 465, 0, 0, 1.0f, true);
        this.momo.mtn(265, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("Even for you?");
        this.face(this.momo.face, 33, 0, 15, 0, 0, 1.0f);
        this.waitclear(45);
        this.agws1.start(1, "c1_1");
        this.agws2.start(1, "c1_2");
        this.agws3.start(1, "c1_3");
        System.sleep(15);
        this.cut_length = 225;
        this.agws1.setVisible(true);
        this.agws2.setVisible(true);
        this.agws3.setVisible(true);
        this.ziggy.setVisible(false);
        this.momo.setVisible(false);
        this.ship_A.setVisible(true);
        this.ship_B.setVisible(true);
        this.CameraPlay = 7;
        System.sleep(55);
        this.msg.print("Not even a cyborg can take on\nthree A.G.W.S. units.");
        this.waitclear(90);
        this.msg.print("I'd be shot to pieces.");
        this.waitclear(55);
        System.sleep(25);
        this.cut_length = 360;
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.ziggy.setVisible(true);
        this.momo.setVisible(true);
        this.ship_A.setVisible(false);
        this.ship_B.setVisible(false);
        this.ziggy.setTranslate(7.7f, 0.0f, 17.32f);
        this.momo.setRotateY(-15.0f);
        this.CameraPlay = 8;
        this.ziggy.mtn(266, 0, 908, 0, 0, 1.0f, true);
        this.momo.mtn(267, 0, 908, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("What if the A.G.W.S. units\ncouldn't use their sensors?");
        this.face(this.momo.face, 33, 0, 75, 0, 0, 1.0f);
        this.waitclear(90);
        this.msg.print("Well, that would probably help.");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(70);
        this.msg.print("But why?");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.msg.print("I can reduce their ability\nto some extent.");
        this.face(this.momo.face, 33, 0, 75, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("How?");
        this.face(this.ziggy.face, 3, 0, 15, 12, 1, 1.0f);
        this.waitclear(30);
        this.cut_length = 405;
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.CameraPlay = 9;
        this.face(this.ziggy.face, 4);
        this.face(this.momo.face, 33);
        System.sleep(15);
        this.msg.print("The Hilbert Effect.");
        this.waitclear(60);
        this.msg.print("Using inverted Hilbert wavelengths,\nI can create cross-interference");
        this.waitclear(105);
        this.msg.print("that automatically shuts down the\nA.G.W.S.' D.S.S.S. enemy sensor system.");
        this.waitclear(120);
        this.msg.print("That should take out most of\ntheir external sensors.");
        this.waitclear(105);
        this.CameraPlay = 10;
        System.sleep(15);
        this.msg.print("The Hilbert Effect?");
        this.face(this.ziggy.face, 3, 0, 49, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("It's an anti-Gnosis\nmaterialization system.");
        this.waitclear(75);
        System.sleep(15);
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.face(this.ziggy.face, 1, 0, 0, 0, 0, 1.0f);
        this.CameraPlay = 11;
        this.momo.mtn(267, 870, 1138, 0, 0, 1.0f, true);
        this.msg.print("Normally it's spread out over a\nwide range via an amplifier");
        this.face(this.momo.face, 33, 0, 105, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("installed onboard a spacecraft,");
        this.face(this.momo.face, 33, 0, 48, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("but I can create the effect by myself\nin small areas like this.");
        this.face(this.momo.face, 33, 0, 92, 0, 0, 1.0f);
        this.waitclear(105);
        this.CameraPlay = 12;
        this.momo.mtn(267, 1140, 1318, 0, 0, 1.0f, true);
        this.ziggy.mtn(268, 0, 1.0f, true);
        this.face(this.momo.face, 34);
        System.sleep(30);
        this.msg.print("That might work.");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(45);
        this.msg.print("All right, go ahead.");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.ziggy.setVisible(false);
        this.momo.setTranslate(8.5f, 0.0f, 17.32f);
        this.momo.setRotateY(0.0f);
        this.CameraPlay = 13;
        this.momo.mtn(269, 0, 343, 0, 0, 1.0f, true);
        this.msg.print("Okay.");
        this.face(this.momo.face, 33, 0, 15, 0, 0, 1.0f);
        this.waitclear(45);
        this.ziggy.setVisible(false);
        this.CameraPlay = 14;
        System.sleep(75);
        this.msg.print("All right, here I go.");
        this.face(this.momo.face, 5, 0, 45, 16, 1, 1.0f);
        this.waitclear(45);
        this.face(this.momo.face, 6, 36, 58, 0, 0, 1.0f);
        System.sleep(180);
        this.DefocusClear();
        Runtime.mpeg2("2006_1");
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.agws1.setRotateY(-45.0f);
        this.agws2.setRotateY(30.0f);
        this.agws3.setRotateY(-75.0f);
        this.ziggy.setVisible(true);
        this.ziggy.setTranslate(6.6f, 0.0f, 16.5f);
        this.momo.setTranslate(8.5f, 0.0f, 17.6f);
        this.face(this.momo.face, 11, 0, 0, 0, 0, 1.0f);
        this.face(this.ziggy.face, 1, 0, 0, 0, 0, 1.0f);
        Sound.streamPlay(1290098, 48000);
        this.agws1.setVisible(true);
        this.agws2.setVisible(true);
        this.agws3.setVisible(true);
        this.agws1.setRotateY(-45.0f);
        this.CameraPlay = 26;
        this.agws1.start(1, "c26_27_20");
        System.sleep(90);
        this.cut_length = 270;
        this.agws2.setRotateY(30.0f);
        this.agws3.setRotateY(-75.0f);
        this.CameraPlay = 27;
        this.agws2.start(1, "c27_22");
        this.agws3.start(1, "c27_21");
        this.msg.print("What was that?\nWhat happened?!");
        this.waitclear(60);
        this.msg.print("Hilbert waves!");
        this.waitclear(45);
        this.msg.print("Somebody's using the\nHilbert Effect on us!");
        this.waitclear(75);
        this.msg.print("My enemy sensor system\nhas shut down!");
        this.waitclear(90);
        this.cut_length = 120;
        this.CameraPlay = 28;
        this.agws1.start(1, "c28_23");
        this.msg.print("It's the 100-Series! I can\ntell from the cross-interference!");
        this.waitclear(75);
        this.msg.print("Find her! She's close!");
        this.waitclear(45);
        this.face(this.momo.face, 21, 0, 0, 0, 0, 1.0f);
        this.cut_length = 195;
        this.agws1.setVisible(false);
        this.agws2.setVisible(false);
        this.agws3.setVisible(false);
        this.momo.setTranslate(8.4f, 0.0f, 17.95f);
        this.momo.setRotate(0.0f, 180.0f, 0.0f);
        this.ziggy.setTranslate(8.0f, 0.0f, 17.85f);
        this.ziggy.setRotateY(75.0f);
        this.CameraPlay = 29;
        this.ziggy.mtn(280, 0, 853, 0, 0, 1.0f, true);
        this.momo.mtn(281, 0, 855, 0, 0, 1.0f, true);
        System.sleep(45);
        this.msg.print("You mentioned cross-interference.");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(80);
        this.msg.print("Do you feel the effects, too?");
        this.face(this.ziggy.face, 1, 0, 56, 0, 0, 1.0f);
        this.waitclear(70);
        this.CameraPlay = 30;
        this.face(this.momo.face, 22, 0, 15, 0, 0, 1.0f);
        System.sleep(15);
        this.msg.print("I'm...fine...");
        this.face(this.momo.face, 21, 0, 15, 0, 0, 1.0f);
        System.sleep(30);
        this.face(this.momo.face, 21, 0, 15, 0, 0, 1.0f);
        this.waitclear(45);
        this.msg.print("I'm supposed...to have...\na much higher...");
        this.face(this.momo.face, 21, 0, 36, 0, 0, 1.0f);
        System.sleep(45);
        this.face(this.momo.face, 21, 0, 36, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("capacity...than them.");
        this.face(this.momo.face, 21, 0, 15, 0, 0, 0.5f);
        System.sleep(40);
        this.face(this.momo.face, 21, 0, 20, 0, 0, 1.0f);
        this.waitclear(20);
        this.CameraPlay = 31;
        System.sleep(15);
        this.msg.print("Rest a bit.");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(60);
        this.msg.print("I'll take care of this.");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(45);
        this.CameraPlay = 32;
        this.face(this.momo.face, 22, 0, 15, 0, 0, 1.0f);
        System.sleep(15);
        this.msg.print("I'm...okay...");
        this.face(this.momo.face, 13, 0, 36, 24, 1, 1.0f);
        this.waitclear(45);
        this.msg.print("I can still...help out.");
        this.face(this.momo.face, 21, 0, 15, 0, 0, 1.0f);
        System.sleep(15);
        this.CameraPlay = 33;
        this.face(this.ziggy.face, 2);
        this.waitclear(60);
        System.sleep(15);
        this.msg.print("My nanorepair function's\nstill operational...");
        this.waitclear(120);
        System.sleep(15);
        this.ziggy.setTranslate(7.0f, 0.0f, 17.85f);
        this.ziggy.setRotateY(75.0f);
        this.CameraPlay = 34;
        this.ziggy.mtn(282, 0, 1.0f, true);
        System.sleep(75);
        this.face(this.ziggy.face, 4, 0, 0, 24, 1, 1.0f);
        this.cut_length = 75;
        this.momo.setVisible(false);
        this.agws1.setVisible(true);
        this.agws2.setVisible(true);
        this.agws3.setVisible(true);
        this.agws1.setRotateY(0.0f);
        this.agws2.setRotateY(25.0f);
        this.agws3.setRotateY(0.0f);
        this.CameraPlay = 35;
        this.agws1.mtn(257, 0, 0, 0, 0, 1.0f, true);
        this.agws2.mtn(258, 0, 0, 0, 0, 1.0f, true);
        this.agws3.mtn(259, 0, 0, 0, 0, 1.0f, true);
        System.sleep(60);
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
        System.waitSignal(this.msg, 99);
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

        public Characters(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
        }

        void c16_14() {
            this.mtn(270, 0, 148, 0, 0, 1.0f, true);
        }

        void c17_14() {
            this.mtn(270, 120, 300, 0, 0, 1.0f, true);
        }

        void c22_23_13() {
            this.mtn(269, 555, 680, 0, 0, 0.5f, true);
        }

        void c22_23_14() {
            this.mtn(270, 0, 127, 0, 0, 0.5f, true);
        }

        void c24_25_18() {
            this.mtn(274, 0, 1.0f, true);
        }

        void c25_19() {
            this.mtn(275, 0, 1.0f, true);
        }
    }

    class Agws
            extends Chr {
        public Agws(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void c18_20_15() {
            this.mtn(271, 60, 200, 0, 0, 1.0f, true);
        }

        void c1_1() {
            this.mtn(257, 0, 0.85f, true);
        }

        void c1_2() {
            this.mtn(258, 0, 0.85f, true);
        }

        void c1_3() {
            this.mtn(259, 0, 0.85f, true);
        }

        void c20_22_16() {
            this.mtn(272, 0, 1.0f, true);
        }

        void c22_17() {
            this.mtn(273, 0, 1.0f, true);
        }

        void c26_27_20() {
            this.mtn(276, 0, 0, 0, 0, 1.0f, true);
            System.sleep(45);
            this.mtn(276, 0, 1.0f, true);
        }

        void c27_21() {
            System.sleep(15);
            this.mtn(277, 0, 1.0f, true);
        }

        void c27_22() {
            this.mtn(278, 0, 1.0f, true);
        }

        void c28_23() {
            this.mtn(279, 0, 1.0f, true);
        }
    }

    class Units
            extends Chr {
        public Units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }
    }
}

