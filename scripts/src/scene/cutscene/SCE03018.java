import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE03018
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack03018,
        FLSziggy,
        FLSmomo_h,
        FLSjr_h,
        FLSjr,
        FLSziggy_h {
    Effect FadeIn;
    Characters momo_h;
    Characters jr_h;
    Characters ziggy_h;
    Characters jr;
    Characters momo;
    Characters ziggy;
    Characters u_ship;
    Characters URTV_a;
    Characters URTV_b;
    Characters URTV_c;
    Characters URTV_d;
    Characters URTV_e;
    Characters URTV_f;
    Characters URTV_g;
    Characters URTV_h;
    Characters rifle_a;
    Characters rifle_b;
    Characters rifle_c;
    Characters rifle_d;
    Characters rifle_e;
    Characters rifle_f;
    Characters rifle_g;
    Characters rifle_h;
    Thread thread1;
    int cut_length;
    int CameraPlay = 0;
    int CameraEnd = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Light light = new Light(0);
    Effect u_ship_thruster;
    Effect searchlight;
    PlayControl pc;
    int cut = 0;
    float[] trans = new float[8];
    float[] rot = new float[8];
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE03018() {
    }

    void AllChrSPECOFF() {
        this.jr_h.renderCommand(530);
        this.momo_h.renderCommand(530);
        this.ziggy.renderCommand(530);
        this.URTV_a.renderCommand(530);
        this.URTV_b.renderCommand(530);
        this.URTV_c.renderCommand(530);
        this.URTV_d.renderCommand(530);
        this.URTV_e.renderCommand(530);
        this.URTV_f.renderCommand(530);
        this.URTV_g.renderCommand(530);
        this.URTV_h.renderCommand(530);
    }

    void AllChrSPECON() {
        this.jr_h.renderCommand(0);
        this.momo_h.renderCommand(0);
        this.ziggy.renderCommand(0);
        this.URTV_a.renderCommand(0);
        this.URTV_b.renderCommand(0);
        this.URTV_c.renderCommand(0);
        this.URTV_d.renderCommand(0);
        this.URTV_e.renderCommand(0);
        this.URTV_f.renderCommand(0);
        this.URTV_g.renderCommand(0);
        this.URTV_h.renderCommand(0);
    }

    void CameraThread() {
        this.DefocusClear();
        this.cam1.change();
        this.waitCameraPlay(1);
        this.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.light.setColor(1, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(1, 0.431f, 0.319f, 0.844f);
        this.light.setColor(2, 0.26f, 0.26f, 0.26f);
        this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
        this.light.setColor(3, 0.17f, 0.17f, 0.17f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.pc.init(1, 0);
        this.cam1.setTranslate(0.0f, -1000.0f, 0.0f);
        System.sleep(this.cut_length);
        this.waitCameraPlay(2);
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, -0.962f, 0.0f, -0.273f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, -0.424f, 0.001f, -0.906f);
        this.light.setColor(3, 0.0f, 0.0f, 0.0f);
        this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 59880, 1);
        this.pc.loadCamera("c03s18cut01_02.cam");
        this.pc.init(2, 0, 211, 510, 1.0f);
        this.pc.start();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(3);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.626f, 0.419f, -0.658f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.358f, 0.0f, 0.934f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.24f, -0.923f, 0.3f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 82880, 1);
        this.pc.loadCamera("c03s18cut03_05.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.waitCameraPlay(4);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.394f, 0.0f, -0.919f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.268f, 0.904f, 0.332f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.836f, -0.535f, 0.12f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 9880, 1);
        System.sleep(this.cut_length);
        this.waitCameraPlay(5);
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.28f, 0.36f, 0.4f);
        this.light.setDirection2(1, 0.588f, -0.635f, -0.501f);
        this.light.setColor(2, 0.03f, 0.07f, 0.09f);
        this.light.setDirection2(2, -0.828f, 0.469f, 0.309f);
        this.light.setColor(3, 0.0f, 0.0f, 0.0f);
        this.light.setDirection2(3, -0.511f, -0.841f, -0.18f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.jr_h.setLightMode(1);
        this.jr_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.jr_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.jr_h.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.jr_h.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.jr_h.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.jr_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.jr_h.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        this.momo_h.setLightMode(1);
        this.momo_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.momo_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.momo_h.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.momo_h.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.momo_h.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.momo_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.momo_h.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        this.ziggy_h.setLightMode(1);
        this.ziggy_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy_h.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.ziggy_h.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.ziggy_h.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.ziggy_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy_h.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        this.jr.setLightMode(1);
        this.jr.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.jr.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.jr.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.jr.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.jr.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.jr.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.jr.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        this.momo.setLightMode(1);
        this.momo.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.momo.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.momo.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.momo.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.momo.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.momo.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.momo.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        this.ziggy.setLightMode(1);
        this.ziggy.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy.light.setDirection2(1, 0.112f, 0.0f, -0.994f);
        this.ziggy.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.ziggy.light.setDirection2(2, -0.871f, 0.0f, 0.491f);
        this.ziggy.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy.light.setDirection2(3, -0.566f, -0.801f, -0.195f);
        Runtime.setDefocusQuick(0, 1, 10880, 1);
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(6);
        this.jr_h.setLightMode(0);
        this.momo_h.setLightMode(0);
        this.ziggy_h.setLightMode(0);
        this.jr.setLightMode(0);
        this.momo.setLightMode(0);
        this.ziggy.setLightMode(0);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.168f, 0.337f, -0.926f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.88f, 0.412f, 0.237f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, -0.697f, -0.637f, -0.329f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 45880, 1);
        this.cam1.change();
        this.cam1.setTranslate(-11.86f, 0.9f, 5.39f);
        this.cam1.setRotate(17.5f, 109.5f, 0.0f);
        this.cam1.setFov(25.0f);
        this.waitCameraPlay(7);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.045f, 0.342f, -0.939f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, 0.299f, 0.0f, 0.954f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.806f, -0.58f, 0.115f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 27880, 1);
        this.pc.loadCamera("c03s18cut06_07.cam");
        this.pc.init(2, 0, 181, 330, 0.75f);
        this.pc.start();
        this.cam2.change();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(8);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.63f, 0.03f, -0.776f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.438f, 0.0f, 0.899f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.647f, -0.745f, 0.161f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 40880, 1);
        this.pc.loadCamera("c03s18cut08_09.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(9);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.1f, 0.141f, -0.985f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.408f, 0.788f, 0.461f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.723f, -0.677f, 0.137f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 10880, 1);
        this.pc.init(2, 0, 240, 240, 1.0f);
        this.pc.start();
        System.sleep(10);
        this.pc.stop();
        this.waitCameraPlay(10);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.914f, 0.0f, -0.405f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.446f, 0.0f, 0.895f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.69f, -0.698f, 0.19f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.jr_h.setLightMode(1);
        this.jr_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.jr_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.jr_h.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.jr_h.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.jr_h.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.jr_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.jr_h.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        this.momo_h.setLightMode(1);
        this.momo_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.momo_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.momo_h.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.momo_h.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.momo_h.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.momo_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.momo_h.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        this.ziggy_h.setLightMode(1);
        this.ziggy_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy_h.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.ziggy_h.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.ziggy_h.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.ziggy_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy_h.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        this.jr.setLightMode(1);
        this.jr.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.jr.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.jr.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.jr.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.jr.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.jr.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.jr.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        this.momo.setLightMode(1);
        this.momo.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.momo.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.momo.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.momo.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.momo.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.momo.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.momo.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        this.ziggy.setLightMode(1);
        this.ziggy.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy.light.setDirection2(1, 0.327f, 0.0f, -0.945f);
        this.ziggy.light.setColor(2, 0.06f, 0.13f, 0.2f);
        this.ziggy.light.setDirection2(2, -0.583f, 0.0f, 0.812f);
        this.ziggy.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy.light.setDirection2(3, 0.637f, -0.513f, 0.575f);
        Runtime.setDefocusQuick(0, 1, 25880, 1);
        this.pc.loadCamera("c03s18cut10_11.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(11);
        this.jr_h.setLightMode(0);
        this.momo_h.setLightMode(0);
        this.ziggy_h.setLightMode(0);
        this.jr.setLightMode(0);
        this.momo.setLightMode(0);
        this.ziggy.setLightMode(0);
        this.light.setColor(0, 0.07f, 0.07f, 0.07f);
        this.light.setColor(1, 0.18f, 0.3f, 0.38f);
        this.light.setDirection2(1, 0.146f, 0.024f, -0.989f);
        this.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.light.setDirection2(2, -0.676f, 0.33f, 0.659f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, -0.673f, -0.738f, 0.056f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.change();
        Runtime.setDefocusQuick(0, 1, 122880, 1);
        this.cam1.setTranslate(-13.75f, 1.37f, 2.41f);
        this.cam1.setRotate(-18.04f, -158.67f, 0.0f);
        this.cam1.setFov(20.0f);
        this.waitCameraPlay(12);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.782f, 0.17f, -0.599f);
        this.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.light.setDirection2(2, -0.436f, 0.0f, 0.9f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.662f, -0.725f, 0.191f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 89880, 1);
        this.pc.loadCamera("c03s18cut12.cam");
        this.pc.init(2, 0);
        this.pc.start();
        this.cam2.change();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(13);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, -0.212f, 0.427f, -0.879f);
        this.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.light.setDirection2(2, 0.545f, 0.344f, 0.765f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, -0.378f, -0.801f, 0.464f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.ziggy.setLightMode(1);
        this.ziggy.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy.light.setDirection2(1, -0.08f, 0.206f, -0.975f);
        this.ziggy.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.ziggy.light.setDirection2(2, 0.138f, 0.567f, 0.812f);
        this.ziggy.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy.light.setDirection2(3, -0.829f, -0.512f, 0.224f);
        this.ziggy_h.setLightMode(1);
        this.ziggy_h.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.ziggy_h.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.ziggy_h.light.setDirection2(1, -0.08f, 0.206f, -0.975f);
        this.ziggy_h.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.ziggy_h.light.setDirection2(2, 0.138f, 0.567f, 0.812f);
        this.ziggy_h.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.ziggy_h.light.setDirection2(3, -0.829f, -0.512f, 0.224f);
        Runtime.setDefocusQuick(0, 1, 12880, 1);
        this.cam1.setTranslate(-16.41f, 2.83f, 4.99f);
        this.cam1.setRotate(-16.61f, -42.4f, 0.0f);
        this.cam1.setFov(41.54f);
        this.cam1.change();
        this.tSPL(this.cut_length, -16.43f, 2.17f, 4.98f);
        this.waitCameraPlay(14);
        this.ziggy.setLightMode(0);
        this.ziggy_h.setLightMode(0);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.413f, 0.314f, -0.855f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, 0.019f, 0.312f, 0.95f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.659f, -0.645f, -0.387f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 2, 59880, 1);
        this.pc.loadCamera("c03s18cut13_15.cam");
        this.pc.init(2, 0, 283, 607, 1.0f);
        this.pc.start();
        this.cam2.change();
        System.sleep(this.cut_length - 10);
        this.pc.stop();
        this.waitCameraPlay(15);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, -0.024f, 0.141f, -0.99f);
        this.light.setColor(2, 0.04f, 0.11f, 0.18f);
        this.light.setDirection2(2, -0.857f, 0.268f, 0.44f);
        this.light.setColor(3, 0.05f, 0.1f, 0.15f);
        this.light.setDirection2(3, -0.547f, -0.602f, 0.581f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 63880, 1);
        this.cam1.setTranslate(-13.46f, 0.79f, 4.41f);
        this.cam1.setRotate(-17.34f, -80.18f, 0.0f);
        this.cam1.setFov(25.0f);
        this.cam1.change();
        this.waitCameraPlay(16);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.331f, 0.321f, -0.887f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.093f, 0.026f, 0.995f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, 0.342f, -0.911f, 0.232f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 107880, 1);
        this.pc.loadCamera("c03s18cut16.cam");
        this.pc.init(2, 0);
        this.pc.start();
        this.cam2.change();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(17);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.038f, 0.0f, -0.999f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.857f, 0.268f, 0.44f);
        this.light.setColor(3, 0.05f, 0.1f, 0.15f);
        this.light.setDirection2(3, -0.547f, -0.602f, 0.581f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 75880, 1);
        this.pc.loadCamera("c03s18cut17.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(18);
        this.light.setColor(0, 0.07f, 0.07f, 0.07f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, -0.016f, 0.518f, -0.855f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.542f, 0.339f, 0.769f);
        this.light.setColor(3, 0.06f, 0.11f, 0.16f);
        this.light.setDirection2(3, -0.419f, -0.862f, -0.285f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 10880, 1);
        this.pc.loadCamera("c03s18cut18.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.pc.stop();
        this.waitCameraPlay(19);
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, 0.115f, 0.34f, -0.934f);
        this.light.setColor(2, 0.05f, 0.12f, 0.19f);
        this.light.setDirection2(2, -0.51f, 0.34f, 0.79f);
        this.light.setColor(3, 0.05f, 0.1f, 0.15f);
        this.light.setDirection2(3, -0.134f, -0.896f, -0.424f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 22880, 1);
        this.pc.loadCamera("c03s18cut19_20.cam");
        this.pc.init(2, 0);
        this.pc.start();
        System.sleep(this.cut_length);
        this.waitCameraPlay(20);
        this.light.setColor(0, 0.09f, 0.09f, 0.09f);
        this.light.setColor(1, 0.2f, 0.32f, 0.4f);
        this.light.setDirection2(1, -0.334f, 0.161f, -0.929f);
        this.light.setColor(2, 0.07f, 0.14f, 0.19f);
        this.light.setDirection2(2, 0.454f, 0.362f, 0.814f);
        this.light.setColor(3, 0.05f, 0.1f, 0.15f);
        this.light.setDirection2(3, 0.645f, -0.667f, -0.373f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setDefocusQuick(0, 1, 117880, 1);
        System.sleep(this.cut_length);
        this.pc.stop();
    }

    void DefocusClear() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
    }

    void URTV_setVisible(boolean bl) {
        this.URTV_a.setVisible(bl);
        this.URTV_b.setVisible(bl);
        this.URTV_c.setVisible(bl);
        this.URTV_d.setVisible(bl);
        this.URTV_e.setVisible(bl);
        this.URTV_f.setVisible(bl);
        this.URTV_g.setVisible(bl);
        this.URTV_h.setVisible(bl);
        this.rifle_a.setVisible(bl);
        this.rifle_b.setVisible(bl);
        this.rifle_c.setVisible(bl);
        this.rifle_d.setVisible(bl);
        this.rifle_e.setVisible(bl);
        this.rifle_f.setVisible(bl);
        this.rifle_g.setVisible(bl);
        this.rifle_h.setVisible(bl);
    }

    void URTV_setface(int n) {
        this.face(this.URTV_a.face, n);
        this.face(this.URTV_b.face, n);
        this.face(this.URTV_c.face, n);
        this.face(this.URTV_d.face, n);
        this.face(this.URTV_e.face, n);
        this.face(this.URTV_f.face, n);
        this.face(this.URTV_g.face, n);
        this.face(this.URTV_h.face, n);
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
        System.println("XEVEFLAG:EV03018_F");
        Runtime.setFlags(329, 1, 1);
        System.println("XEVEJNAME:SCE03019");
        Runtime.jumpEvent(3190);
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
        Runtime.setLocation(1202);
        this.ziggy_h = new Characters(16777251, 0.0f, 0.0f, 0.0f, 0.0f);
        this.jr_h = new Characters(0x1000022, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo_h = new Characters(0x1000021, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ziggy = new Characters(0x1000006, 0.0f, 0.0f, 0.0f, 0.0f);
        this.jr = new Characters(0x1000005, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo = new Characters(0x1000004, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_a = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_b = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_c = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_d = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_e = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_f = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_g = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.URTV_h = new Characters(16777263, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_a = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_b = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_c = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_d = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_e = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_f = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_g = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.rifle_h = new Characters(24607, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u_ship = new Characters(20554, 0.0f, 0.0f, 0.0f, 0.0f);
        this.loadarc(this.ziggy.face, "FLSziggy.fpk");
        this.loadarc(this.ziggy_h.face, "FLSziggy_h.fpk");
        this.loadarc(this.jr_h.face, "FLSjr_h.fpk");
        this.loadarc(this.jr.face, "FLSjr.fpk");
        this.loadarc(this.momo_h.face, "FLSmomo_h.fpk");
        this.loadarc(this.momo.face, "FLSmomo.fpk");
        this.loadarc(this.URTV_a.face, "FLSjr.fpk");
        this.loadarc(this.URTV_b.face, "FLSjr.fpk");
        this.loadarc(this.URTV_c.face, "FLSjr.fpk");
        this.loadarc(this.URTV_d.face, "FLSjr.fpk");
        this.loadarc(this.URTV_e.face, "FLSjr.fpk");
        this.loadarc(this.URTV_f.face, "FLSjr.fpk");
        this.loadarc(this.URTV_g.face, "FLSjr.fpk");
        this.loadarc(this.URTV_h.face, "FLSjr.fpk");
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
        this.pc = PlayControl.create();
        this.FadeIn = new Effect(0);
        this.FadeIn.args[0] = Integer.MIN_VALUE;
        this.FadeIn.args[1] = 150;
        this.FadeIn.args[2] = 1;
        this.u_ship_thruster = new Effect(1473, 0.0f, 0.0f, 0.0f, 0.0f);
        this.u_ship_thruster.setCaster(this.u_ship);
        this.u_ship_thruster.setMotion(true);
        this.u_ship_thruster.noAttach(false);
        this.u_ship_thruster.setScale(0.1f, 0.1f, 0.1f);
        this.u_ship_thruster.disp(false);
        this.searchlight = new Effect(1690, 0.0f, 0.0f, 0.0f, 0.0f);
        this.searchlight.setCaster(this.u_ship);
        this.searchlight.setMotion(true);
        this.searchlight.noAttach(false);
        this.searchlight.setClip(false);
        this.searchlight.setScale(0.1f, 0.1f, 0.1f);
        this.searchlight.disp(false);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        Sound.streamPlay(1390036, 48000);
        this.CameraPlay = 1;
        this.cut_length = 210;
        this.ziggy_h.setVisible(false);
        this.u_ship.setVisible(false);
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.URTV_setVisible(false);
        System.sleep(210);
        this.CameraPlay = 2;
        this.cut_length = 300;
        this.FadeIn.call(0);
        System.sleep(300);
        this.CameraPlay = 3;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.face(this.jr_h.face, 5, 0, 15, 0, 0, 0.2f);
        this.cut_length = 102;
        this.jr_h.mtn(257, 0, 502, 0, 0, 1.0f, true);
        this.u_ship.mtn(258, 0, 502, 0, 0, 1.0f, true);
        this.momo_h.mtn(259, 0, 502, 0, 0, 1.0f, true);
        this.ziggy.mtn(260, 0, 502, 0, 0, 1.0f, true);
        System.sleep(102);
        this.AllChrSPECOFF();
        this.CameraPlay = 4;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.ziggy.setVisible(true);
        this.u_ship.setVisible(true);
        this.URTV_setVisible(false);
        this.u_ship_thruster.disp(true);
        this.searchlight.disp(true);
        this.cut_length = 192;
        System.sleep(192);
        this.CameraPlay = 5;
        this.cut_length = 210;
        System.sleep(210);
        this.AllChrSPECON();
        this.CameraPlay = 6;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.u_ship_thruster.disp(false);
        this.searchlight.disp(false);
        this.cut_length = 180;
        this.jr_h.mtn(261, 0, 328, 0, 0, 1.0f, true);
        this.momo_h.mtn(262, 0, 328, 0, 0, 1.0f, true);
        this.ziggy.mtn(263, 0, 328, 0, 0, 1.0f, true);
        System.sleep(180);
        this.AllChrSPECOFF();
        this.CameraPlay = 7;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.ziggy.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 150;
        System.sleep(150);
        this.AllChrSPECON();
        this.CameraPlay = 8;
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(true);
        this.cut_length = 120;
        this.URTV_setface(2);
        this.URTV_a.mtn(272, 0, 250, 0, 0, 1.0f, true);
        this.URTV_b.mtn(273, 0, 250, 0, 0, 1.0f, true);
        this.URTV_c.mtn(274, 0, 250, 0, 0, 1.0f, true);
        this.URTV_d.mtn(275, 0, 250, 0, 0, 1.0f, true);
        this.URTV_e.mtn(276, 0, 250, 0, 0, 1.0f, true);
        this.URTV_f.mtn(277, 0, 250, 0, 0, 1.0f, true);
        this.URTV_g.mtn(278, 0, 250, 0, 0, 1.0f, true);
        this.URTV_h.mtn(279, 0, 250, 0, 0, 1.0f, true);
        this.rifle_a.mtn(264, 0, 250, 0, 0, 1.0f, true);
        this.rifle_b.mtn(265, 0, 250, 0, 0, 1.0f, true);
        this.rifle_c.mtn(266, 0, 250, 0, 0, 1.0f, true);
        this.rifle_d.mtn(267, 0, 250, 0, 0, 1.0f, true);
        this.rifle_e.mtn(268, 0, 250, 0, 0, 1.0f, true);
        this.rifle_f.mtn(269, 0, 250, 0, 0, 1.0f, true);
        this.rifle_g.mtn(270, 0, 250, 0, 0, 1.0f, true);
        this.rifle_h.mtn(271, 0, 250, 0, 0, 1.0f, true);
        System.sleep(120);
        this.CameraPlay = 9;
        this.cut_length = 132;
        System.sleep(132);
        this.AllChrSPECOFF();
        this.CameraPlay = 10;
        this.jr.setVisible(2, false);
        this.jr.setVisible(3, false);
        this.jr.setVisible(5, false);
        this.jr.setVisible(7, false);
        this.momo.setVisible(2, false);
        this.momo.setVisible(4, false);
        this.momo.setVisible(5, false);
        this.ziggy.setVisible(7, false);
        this.ziggy.setVisible(12, false);
        this.jr.renderCommand(530);
        this.momo.renderCommand(530);
        this.ziggy.renderCommand(530);
        this.URTV_a.renderCommand(530);
        this.URTV_b.renderCommand(530);
        this.URTV_c.renderCommand(530);
        this.URTV_d.renderCommand(530);
        this.URTV_e.renderCommand(530);
        this.URTV_f.renderCommand(530);
        this.URTV_g.renderCommand(530);
        this.URTV_h.renderCommand(530);
        this.jr.setMotionFlags(0x40000000, true);
        this.momo.setMotionFlags(0x40000000, true);
        this.ziggy.setMotionFlags(0x40000000, true);
        this.jr.face.setVisible(false);
        this.momo.face.setVisible(false);
        this.face(this.jr_h.face, 6, 0, 0, 0, 0, 1.0f);
        this.face(this.momo_h.face, 20, 0, 0, 0, 0, 1.0f);
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy_h.setVisible(false);
        this.jr.setVisible(true);
        this.momo.setVisible(true);
        this.ziggy.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(true);
        this.URTV_a.setVisible(false);
        this.URTV_b.setVisible(false);
        this.rifle_a.setVisible(false);
        this.rifle_b.setVisible(false);
        this.cut_length = 192;
        this.jr.mtn(289, 0, 190, 0, 0, 1.0f, true);
        this.momo.mtn(298, 0, 190, 0, 0, 1.0f, true);
        this.ziggy.mtn(299, 0, 370, 0, 0, 1.0f, true);
        this.URTV_c.mtn(292, 0, 370, 0, 0, 1.0f, true);
        this.URTV_d.mtn(293, 0, 370, 0, 0, 1.0f, true);
        this.URTV_e.mtn(294, 0, 370, 0, 0, 1.0f, true);
        this.URTV_f.mtn(295, 0, 370, 0, 0, 1.0f, true);
        this.URTV_g.mtn(296, 0, 370, 0, 0, 1.0f, true);
        this.URTV_h.mtn(297, 0, 370, 0, 0, 1.0f, true);
        this.rifle_c.mtn(282, 0, 370, 0, 0, 1.0f, true);
        this.rifle_d.mtn(283, 0, 370, 0, 0, 1.0f, true);
        this.rifle_e.mtn(284, 0, 370, 0, 0, 1.0f, true);
        this.rifle_f.mtn(285, 0, 370, 0, 0, 1.0f, true);
        this.rifle_g.mtn(286, 0, 370, 0, 0, 1.0f, true);
        this.rifle_h.mtn(287, 0, 370, 0, 0, 1.0f, true);
        System.sleep(192);
        this.AllChrSPECON();
        this.jr.setVisible(2, true);
        this.jr.setVisible(3, true);
        this.jr.setVisible(5, true);
        this.jr.setVisible(7, true);
        this.momo.setVisible(2, true);
        this.momo.setVisible(4, true);
        this.momo.setVisible(5, true);
        this.ziggy.setVisible(7, true);
        this.ziggy.setVisible(12, true);
        this.jr.renderCommand(0);
        this.momo.renderCommand(0);
        this.ziggy.renderCommand(0);
        this.jr.setMotionFlags(0x40000000, false);
        this.momo.setMotionFlags(0x40000000, false);
        this.ziggy.setMotionFlags(0x40000000, false);
        this.jr.face.setVisible(true);
        this.momo.face.setVisible(true);
        this.CameraPlay = 11;
        this.jr_h.setTranslate(-0.8f, -0.1f, 0.2f);
        this.momo_h.setTranslate(-1.0f, -0.15f, 0.45f);
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.jr.setVisible(false);
        this.momo.setVisible(false);
        this.ziggy.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 180;
        this.face(this.jr_h.face, 6, 0, 15, 0, 0, 1.0f);
        this.face(this.momo_h.face, 20, 0, 37, 0, 0, 1.0f);
        this.jr_h.mtn(289, 193, 371, 0, 0, 1.0f, true);
        this.momo_h.mtn(298, 193, 371, 0, 0, 1.0f, true);
        this.msg.print("No way...\nIt can't be...");
        System.sleep(15);
        this.face(this.jr_h.face, 5, 0, 15, 0, 0, 1.0f);
        System.sleep(60);
        this.face(this.jr_h.face, 5, 0, 15, 0, 0, 1.0f);
        this.waitclear(30);
        System.sleep(15);
        this.msg.print("Jr.?");
        this.face(this.momo_h.face, 19, 0, 37, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(15);
        this.CameraPlay = 12;
        this.jr_h.setTranslate(0.0f, 0.0f, 0.0f);
        this.momo_h.setTranslate(0.0f, 0.0f, 0.0f);
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(true);
        this.face(this.jr_h.face, 6);
        this.face(this.momo_h.face, 2);
        this.cut_length = 170;
        this.URTV_a.mtn(308, 0, 168, 0, 0, 1.0f, true);
        this.URTV_b.mtn(309, 0, 168, 0, 0, 1.0f, true);
        this.URTV_c.mtn(310, 0, 168, 0, 0, 1.0f, true);
        this.URTV_d.mtn(311, 0, 168, 0, 0, 1.0f, true);
        this.URTV_e.mtn(312, 0, 168, 0, 0, 1.0f, true);
        this.URTV_f.mtn(313, 0, 168, 0, 0, 1.0f, true);
        this.URTV_g.mtn(314, 0, 168, 0, 0, 1.0f, true);
        this.URTV_h.mtn(315, 0, 168, 0, 0, 1.0f, true);
        this.rifle_a.mtn(300, 0, 168, 0, 0, 1.0f, true);
        this.rifle_b.mtn(301, 0, 168, 0, 0, 1.0f, true);
        this.rifle_c.mtn(302, 0, 168, 0, 0, 1.0f, true);
        this.rifle_d.mtn(303, 0, 168, 0, 0, 1.0f, true);
        this.rifle_e.mtn(304, 0, 168, 0, 0, 1.0f, true);
        this.rifle_f.mtn(305, 0, 168, 0, 0, 1.0f, true);
        this.rifle_g.mtn(306, 0, 168, 0, 0, 1.0f, true);
        this.rifle_h.mtn(307, 0, 168, 0, 0, 1.0f, true);
        System.sleep(20);
        this.msg.print("No doubt about it.\nThey're U.R.T.V.");
        this.waitclear(150);
        this.AllChrSPECOFF();
        this.CameraPlay = 13;
        this.ziggy.renderCommand(530);
        this.URTV_a.face.setVisible(false);
        this.URTV_b.face.setVisible(false);
        this.URTV_c.face.setVisible(false);
        this.URTV_d.face.setVisible(false);
        this.URTV_e.face.setVisible(false);
        this.URTV_f.face.setVisible(false);
        this.URTV_g.face.setVisible(false);
        this.URTV_h.face.setVisible(false);
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(true);
        this.ziggy_h.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(true);
        this.URTV_a.setVisible(false);
        this.URTV_b.setVisible(false);
        this.cut_length = 282;
        this.ziggy.mtn(334, 0, 280, 0, 0, 1.0f, true);
        this.URTV_c.mtn(327, 0, 610, 0, 0, 1.0f, true);
        this.URTV_d.mtn(328, 0, 610, 0, 0, 1.0f, true);
        this.URTV_e.mtn(329, 0, 610, 0, 0, 1.0f, true);
        this.URTV_f.mtn(330, 0, 610, 0, 0, 1.0f, true);
        this.URTV_g.mtn(331, 0, 610, 0, 0, 1.0f, true);
        this.URTV_h.mtn(332, 0, 610, 0, 0, 1.0f, true);
        this.rifle_c.mtn(318, 0, 610, 0, 0, 1.0f, true);
        this.rifle_d.mtn(319, 0, 610, 0, 0, 1.0f, true);
        this.rifle_e.mtn(320, 0, 610, 0, 0, 1.0f, true);
        this.rifle_f.mtn(321, 0, 610, 0, 0, 1.0f, true);
        this.rifle_g.mtn(322, 0, 610, 0, 0, 1.0f, true);
        this.rifle_h.mtn(323, 0, 610, 0, 0, 1.0f, true);
        System.sleep(77);
        this.msg.print("...I see.");
        this.waitclear(30);
        System.sleep(25);
        this.msg.print("It would appear that this is\nmore than just a hallucination.");
        this.waitclear(110);
        System.sleep(40);
        this.AllChrSPECON();
        this.ziggy.renderCommand(0);
        this.ziggy.face.setVisible(true);
        this.ziggy.setVisible(false);
        this.ziggy_h.setVisible(true);
        this.CameraPlay = 14;
        this.jr_h.setMotionFlags(0x40000000, true);
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 270;
        this.face(this.momo_h.face, 14);
        this.ziggy_h.mtn(334, 283, 550, 0, 0, 1.0f, true);
        this.jr_h.mtn(324, 283, 550, 0, 0, 1.0f, true);
        this.momo_h.mtn(333, 283, 550, 0, 0, 1.0f, true);
        this.msg.print("What is this place?");
        this.face(this.ziggy_h.face, 1, 0, 16, 0, 0, 1.0f);
        this.waitclear(35);
        System.sleep(30);
        this.msg.print("Do you recognize it?");
        this.face(this.ziggy_h.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(45);
        System.sleep(100);
        this.msg.print("Jr...");
        this.face(this.ziggy_h.face, 1, 0, 16, 0, 0, 1.0f);
        this.waitclear(30);
        this.msg.print("What?!!");
        this.face(this.jr_h.face, 9, 0, 14, 0, 0, 1.0f);
        this.waitclear(30);
        this.CameraPlay = 15;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.ziggy.setVisible(false);
        this.ziggy_h.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.momo_h.setMotionFlags(0x40000000, true);
        this.cut_length = 60;
        this.face(this.jr_h.face, 10);
        this.jr_h.mtn(324, 553, 608, 0, 0, 1.0f, true);
        this.momo_h.mtn(333, 553, 608, 0, 0, 1.0f, true);
        this.face(this.momo_h.face, 26, 0, 60, 12, 1, 1.0f);
        System.sleep(60);
        this.AllChrSPECON();
        this.face(this.momo_h.face, 26);
        this.momo_h.setMotNoUpdate(2);
        this.momo_h.mtn(337, 0, 0, 0, 0, 1.0f, true);
        this.momo_h.setMotionFlags(0x40000000, false);
        this.momo_h.setMotionFlags(0x800000, false);
        this.momo_h.setMotionFlags(0x2000000, true);
        this.CameraPlay = 16;
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.ziggy_h.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.jr_h.setMotionFlags(0x40000000, false);
        this.cut_length = 105;
        this.ziggy_h.mtn(335, 0, 118, 0, 0, 1.0f, true);
        this.msg.print("I'm asking you if you\nrecognize this place.");
        this.face(this.ziggy_h.face, 1, 0, 73, 0, 0, 1.0f);
        this.waitclear(75);
        System.sleep(30);
        this.CameraPlay = 17;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.ziggy_h.setVisible(false);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 450;
        this.jr_h.mtn(336, 0, 448, 0, 0, 1.0f, true);
        this.momo_h.mtn(337, 0, 448, 0, 0, 1.0f, true);
        this.msg.print("Yeah, I do.");
        this.face(this.jr_h.face, 5, 0, 35, 0, 0, 1.0f);
        this.waitclear(75);
        this.msg.print("If this isn't an illusion,\nand my memory's correct,");
        this.face(this.jr_h.face, 5, 0, 35, 0, 0, 1.0f);
        System.sleep(60);
        this.face(this.jr_h.face, 5, 0, 51, 0, 0, 1.0f);
        this.waitclear(120);
        System.sleep(15);
        this.msg.print("this is...Miltia.");
        this.face(this.jr_h.face, 5, 0, 50, 0, 0, 1.0f);
        this.waitclear(105);
        this.msg.print("From fourteen years ago.");
        this.face(this.jr_h.face, 5, 0, 35, 0, 0, 1.0f);
        this.waitclear(75);
        this.CameraPlay = 18;
        this.jr_h.setVisible(true);
        this.momo_h.setVisible(true);
        this.ziggy.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.face(this.jr_h, 6);
        this.face(this.ziggy, 2);
        this.face(this.momo_h, 14);
        this.cut_length = 120;
        this.jr_h.mtn(338, 0, 118, 0, 0, 1.0f, true);
        this.momo_h.mtn(340, 0, 118, 0, 0, 1.0f, true);
        this.ziggy.mtn(341, 0, 118, 0, 0, 1.0f, true);
        System.sleep(30);
        System.sleep(40);
        this.msg.print("Where are you going?");
        this.face(this.ziggy.face, 1, 0, 32, 0, 0, 1.0f);
        this.waitclear(50);
        this.CameraPlay = 19;
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(true);
        this.ziggy.setVisible(false);
        this.ziggy_h.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 330;
        this.face(this.momo_h.face, 14, 0, 15, 0, 0, 1.0f);
        this.momo_h.mtn(342, 0, 513, 0, 0, 1.0f, true);
        this.ziggy_h.mtn(343, 0, 513, 0, 0, 1.0f, true);
        System.sleep(15);
        this.msg.print("I wonder what's troubling him?");
        this.face(this.momo_h.face, 13, 0, 76, 0, 0, 1.0f);
        this.waitclear(90);
        System.sleep(20);
        this.msg.print("It looks as though he's trying to\nchase after those people...");
        this.face(this.momo_h.face, 13, 0, 92, 0, 0, 1.0f);
        this.waitclear(100);
        this.msg.print("I'm sure he has his reasons.");
        this.face(this.ziggy_h.face, 1, 0, 98, 0, 0, 1.0f);
        this.waitclear(105);
        this.CameraPlay = 20;
        this.jr_h.setVisible(false);
        this.momo_h.setVisible(false);
        this.ziggy.setVisible(false);
        this.ziggy_h.setVisible(true);
        this.u_ship.setVisible(false);
        this.URTV_setVisible(false);
        this.cut_length = 185;
        System.sleep(20);
        this.msg.print("Nonetheless, \nwe don't know what's going on");
        this.face(this.ziggy_h.face, 1, 0, 57, 0, 0, 1.0f);
        this.waitclear(70);
        System.sleep(10);
        this.msg.print("and we can't let him go off alone.");
        this.face(this.ziggy_h.face, 1, 16, 57, 0, 0, 1.0f);
        this.waitclear(50);
        System.sleep(35);
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
        Runtime.setRegister(1, this.CameraPlay);
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
            this.setVisible(false);
            this.setShadow(0, 0);
        }
    }

    class Chr_units
            extends Chr {
        public Chr_units(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 0);
            this.setVisible(false);
        }
    }

    class Mapunits
            extends MAPUnit {
        public Mapunits(int n) {
            this.init(n);
            this.setTranslate(0.0f, 0.0f, 0.0f);
            this.setRotate(0.0f, 0.0f, 0.0f);
            this.setVisible(true);
            this.start(4, null);
        }
    }
}

