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
import xeno.map.MC_VOK15_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Toolkit;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01043
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        JNT_Human,
        Pack01043,
        MC_VOK15_PRJ {
    Light light = new Light(0);
    static final float nc = 1.0E7f;
    public static final int PADL3 = 512;
    public static final int PADR3 = 1024;
    Menu menu;
    Window win;
    Thread thread1;
    Thread _spl_thread_main;
    Input pad1;
    Input pad0;
    int andrew_flag = 0;
    int elza_flag = 0;
    int pod_flag = 0;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camerawork camerawork = new Camerawork();
    mapunits pod_21;
    mapunits pod_22;
    units konegi;
    units elza;
    kiki elza2;
    Unit pod2;
    robo andrew;
    robo shion;
    robo allen;
    robo allen2;
    units dummy1;
    units dummy2;
    units dummy3;
    units dummy4;
    units dummy5;
    units dummy6;
    units moni_15;
    units moni_17;
    units moni_18;
    units moni_19;
    units moni_20;
    units alle_moni;
    Effect eft0;
    Effect eft1;
    Effect p_ef01;
    Effect p_ef02;
    Effect p_ef03;
    Effect p_ef04;
    Effect p_ef05;
    Effect p_ef06;
    Effect p_ef07;
    Effect p_ef08;
    Effect p_ef09;
    Effect p_ef10;
    Effect p_ef11;
    Effect p_ef12;
    Effect hikari1;
    Effect hikari2;
    Effect hikari3;
    Effect hikari4;
    Effect hikari5;
    Effect hikari6;
    Effect hikari7;
    Effect hikari8;
    Effect hikari21;
    Effect hikari22;
    Effect hikari23;
    Effect hikari24;
    Effect hikari25;
    Effect hikari26;
    Effect hikari27;
    Effect hikari28;
    kiki zan_1;
    kiki zan_2;
    kiki zan_3;
    kiki zan_4;
    kiki zan_5;
    kiki zan_6;
    kiki zan_7;
    kiki zan_8;
    kiki zan_9;
    kiki zan_10;
    kiki zan_11;
    kiki zan_12;
    kiki zan_13;
    kiki zan_14;
    kiki zan_15;
    kiki zan_16;
    kiki zan_17;
    kiki zan_18;
    kiki zan_19;
    kiki zan_20;
    kiki zan_21;
    kiki zan_22;
    kiki zan_72;
    kiki zan_73;
    kiki zan_74;
    kiki zan_75;
    kiki zan_76;
    kiki zan_77;
    kiki zan_78;
    kiki zan_79;
    kiki zan_80;
    kiki zan_81;
    kiki zan_82;
    units star;
    units dummy;
    int kubi_flag = 0;
    static final int FACE_TALK_F = 1;
    static final int FACE_TALK_M = 3;
    int __wait_loop_flag = 0;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01043() {
    }

    void FACE(int n, robo robo2, int n2) {
        robo2.face.mtn(n2, 8, 1.0f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 8, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, robo robo2, int n2) {
        robo2.face.mtn(n2, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void FACE_SMOOTH(int n, robo robo2, int n2, float f) {
        robo2.face.mtn(n2, 0, 120, 7, 9, f, false);
        robo2.face.start(4, null);
        System.sleep(n);
        robo2.face.mtn(n2 + 1, 0, 120, 7, 9, 1.0f, false);
        robo2.face.start(4, null);
    }

    void MSGW(String string) {
        this.msg.clear();
        this.msg.print(string);
    }

    void TP_FACE(robo robo2, int n, float f) {
        robo2.face.mtn(n, 8, f, false);
        robo2.face.start(4, null);
    }

    void TP_FACE_SMOOTH(robo robo2, int n, float f) {
        robo2.face.mtn(n, 0, 120, 7, 9, f, false);
        robo2.face.start(4, null);
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

    void __chracter_splne() {
        float f = 37.92f;
        float f2 = 12.29f;
        float f3 = 2.38f;
        while (true) {
            if (this.kubi_flag == 1) {
                this.allen.look_point(this.shion.px, this.shion.py, this.shion.pz);
            }
            if (this.pod_flag == 1) {
                this.pod_21.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                this.pod_22.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
            }
            if (this.andrew_flag == 1) {
                this.andrew.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.andrew.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
            }
            if (this.elza_flag == 2) {
                this.elza.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.elza.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                this.elza2.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.elza2.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                this.elza.getTranslate();
                this.light.setGlobalPointLightPos(2, -7.999633f + (f - this.elza.px), 1.600002f + (f2 - this.elza.py), 75.197014f + (f3 - this.elza.pz));
            }
            if (this.elza_flag == 1) {
                this.elza.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.elza.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                this.elza2.setTranslate(this.cam3.getTranslateX(), this.cam3.getTranslateY(), this.cam3.getTranslateZ());
                this.elza2.setRotate(this.cam3.getRotateX(), this.cam3.getRotateY(), this.cam3.getRotateZ());
                this.andrew.setTranslate(this.cam2.getTranslateX(), this.cam2.getTranslateY(), this.cam2.getTranslateZ());
                this.andrew.setRotate(this.cam2.getRotateX(), this.cam2.getRotateY(), this.cam2.getRotateZ());
            }
            System.sleep(1);
        }
    }

    void __wait() {
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01043_F");
        Runtime.setFlags(56, 1, 1);
        System.println("XEVEJNAME:SCE01044A");
        Runtime.jumpEvent(1440);
    }

    void gareki_syoki_iti() {
        this.zan_1.setTranslate(12.97f, -0.4f, 56.9f);
        this.zan_1.setRotate(-658.33f, -400.0f, -172.5f);
        this.zan_2.setTranslate(7.94f, 1.45f, 61.64f);
        this.zan_2.setRotate(-658.63f, 202.5f, 30.0f);
        this.zan_3.setTranslate(6.6f, -0.53f, 59.2f);
        this.zan_3.setRotate(-255.0f, 0.0f, -262.5f);
        this.zan_4.setTranslate(13.0f, -1.25f, 27.75f);
        this.zan_4.setRotate(-495.0f, 60.0f, 0.0f);
        this.zan_5.setTranslate(11.23f, -1.4f, 57.99f);
        this.zan_5.setRotate(-1940.69f, -538.75f, -558.67f);
        this.zan_6.setTranslate(42.2f, -2.4f, -51.2f);
        this.zan_6.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_7.setTranslate(38.8f, 2.0f, -51.2f);
        this.zan_7.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_8.setTranslate(68.6f, -2.0f, -58.0f);
        this.zan_8.setRotate(-480.0f, 0.0f, 0.0f);
        this.zan_9.setTranslate(62.3f, -0.08f, -82.5f);
        this.zan_9.setRotate(-950.0f, -60.0f, -400.0f);
        this.zan_10.setTranslate(69.68f, -0.4f, -39.81f);
        this.zan_10.setRotate(-17.7f, 0.0f, 0.0f);
        this.zan_11.setTranslate(10.54f, -0.6f, 61.51f);
        this.zan_11.setRotate(-1224.18f, 243.15f, 444.0f);
        this.zan_12.setTranslate(8.0f, -1.2f, 24.8f);
        this.zan_12.setRotate(-1080.0f, 680.0f, -160.0f);
        this.zan_13.setTranslate(10.6f, 1.5f, 44.0f);
        this.zan_13.setRotate(-800.0f, 160.0f, 0.0f);
        this.zan_14.setTranslate(8.9f, 3.3f, 45.27f);
        this.zan_14.setRotate(-678.33f, -730.0f, 300.0f);
        this.zan_15.setTranslate(8.12f, -0.55f, 62.85f);
        this.zan_15.setRotate(-1294.74f, 240.0f, 0.0f);
        this.zan_16.setTranslate(12.67f, -0.27f, 41.34f);
        this.zan_16.setRotate(-97.38f, -45.0f, 774.17f);
        this.zan_17.setTranslate(10.4f, 1.5f, 60.5f);
        this.zan_17.setRotate(-120.0f, 240.0f, 530.0f);
        this.zan_18.setTranslate(10.25f, -1.55f, 52.42f);
        this.zan_18.setRotate(-852.5f, 470.0f, 627.5f);
        this.zan_19.setTranslate(0.6f, 0.0f, -75.2f);
        this.zan_19.setRotate(-1360.0f, 0.0f, -720.0f);
        this.zan_20.setTranslate(25.76f, 2.17f, -30.22f);
        this.zan_20.setRotate(-120.0f, 0.0f, 0.0f);
        this.zan_21.setTranslate(97.8f, -6.8f, -146.8f);
        this.zan_21.setRotate(-573.33f, -180.0f, 0.0f);
        this.zan_22.setTranslate(20.0f, 0.2f, -91.89f);
        this.zan_22.setRotate(-1270.0f, 100.0f, 0.0f);
        this.zan_72.setTranslate(6.2f, -0.4f, -41.2f);
        this.zan_72.setRotate(-400.0f, 0.0f, 0.0f);
        this.zan_73.setTranslate(50.2f, -6.0f, -64.0f);
        this.zan_73.setRotate(0.0f, -120.0f, 0.0f);
        this.zan_74.setTranslate(-12.6f, -6.8f, -182.4f);
        this.zan_74.setRotate(80.0f, 0.0f, 53.33f);
        this.zan_75.setTranslate(50.56f, -15.0f, -166.96f);
        this.zan_75.setRotate(-293.33f, -20.0f, 0.0f);
        this.zan_76.setTranslate(5.4f, 2.8f, 20.8f);
        this.zan_76.setRotate(-160.0f, -240.0f, 0.0f);
        this.zan_77.setTranslate(14.6f, 1.6f, 49.2f);
        this.zan_77.setRotate(-53.33f, -160.0f, 0.0f);
        this.zan_78.setTranslate(7.9f, -0.3f, 60.2f);
        this.zan_78.setRotate(-40.0f, -80.0f, 0.0f);
        this.zan_79.setTranslate(9.8f, 2.4f, 55.2f);
        this.zan_79.setRotate(-80.0f, 0.0f, 0.0f);
        this.zan_80.setTranslate(9.8f, 1.6f, 58.8f);
        this.zan_80.setRotate(-53.33f, 0.0f, 0.0f);
        this.zan_81.setTranslate(12.2f, 1.3f, 51.1f);
        this.zan_81.setRotate(-73.33f, -353.33f, 106.67f);
        this.zan_82.setTranslate(12.32f, -1.36f, 53.06f);
        this.zan_82.setRotate(-683.32f, -650.36f, -172.5f);
    }

    void gareki_syoki_iti2() {
        this.zan_1.setTranslate(12.97f, -0.4f, 56.9f);
        this.zan_1.setRotate(-658.33f, -400.0f, -172.5f);
        this.zan_2.setTranslate(7.94f, 1.45f, 61.64f);
        this.zan_2.setRotate(-658.63f, 202.5f, 30.0f);
        this.zan_3.setTranslate(6.6f, -0.53f, 59.2f);
        this.zan_3.setRotate(-255.0f, 0.0f, -262.5f);
        this.zan_4.setTranslate(13.0f, -1.25f, 27.75f);
        this.zan_4.setRotate(-495.0f, 60.0f, 0.0f);
        this.zan_5.setTranslate(11.23f, -1.4f, 57.99f);
        this.zan_5.setRotate(-1940.69f, -538.75f, -558.67f);
        this.zan_6.setTranslate(42.2f, -2.4f, -51.2f);
        this.zan_6.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_7.setTranslate(38.8f, 2.0f, -51.2f);
        this.zan_7.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_8.setTranslate(68.6f, -2.0f, -58.0f);
        this.zan_8.setRotate(-480.0f, 0.0f, 0.0f);
        this.zan_9.setTranslate(62.3f, -0.08f, -82.5f);
        this.zan_9.setRotate(-950.0f, -60.0f, -400.0f);
        this.zan_10.setTranslate(69.68f, -0.4f, -39.81f);
        this.zan_10.setRotate(-17.7f, 0.0f, 0.0f);
        this.zan_11.setTranslate(10.54f, -0.6f, 61.51f);
        this.zan_11.setRotate(-1224.18f, 243.15f, 444.0f);
        this.zan_12.setTranslate(8.0f, -1.2f, 24.8f);
        this.zan_12.setRotate(-1080.0f, 680.0f, -160.0f);
        this.zan_13.setTranslate(10.6f, 1.5f, 44.0f);
        this.zan_13.setRotate(-800.0f, 160.0f, 0.0f);
        this.zan_14.setTranslate(8.9f, 3.3f, 45.27f);
        this.zan_14.setRotate(-678.33f, -730.0f, 300.0f);
        this.zan_15.setTranslate(8.12f, -0.55f, 62.85f);
        this.zan_15.setRotate(-1294.74f, 240.0f, 0.0f);
        this.zan_16.setTranslate(12.67f, -0.27f, 41.34f);
        this.zan_16.setRotate(-97.38f, -45.0f, 774.17f);
        this.zan_17.setTranslate(10.4f, 1.5f, 60.5f);
        this.zan_17.setRotate(-120.0f, 240.0f, 530.0f);
        this.zan_18.setTranslate(10.25f, -1.55f, 52.42f);
        this.zan_18.setRotate(-852.5f, 470.0f, 627.5f);
        this.zan_19.setTranslate(0.6f, 0.0f, -75.2f);
        this.zan_19.setRotate(-1360.0f, 0.0f, -720.0f);
        this.zan_20.setTranslate(25.76f, 2.17f, -30.22f);
        this.zan_20.setRotate(-120.0f, 0.0f, 0.0f);
        this.zan_21.setTranslate(97.8f, -6.8f, -146.8f);
        this.zan_21.setRotate(-573.33f, -180.0f, 0.0f);
        this.zan_22.setTranslate(20.0f, 0.2f, -91.89f);
        this.zan_22.setRotate(-1270.0f, 100.0f, 0.0f);
        this.zan_72.setTranslate(6.2f, -0.4f, -41.2f);
        this.zan_72.setRotate(-400.0f, 0.0f, 0.0f);
        this.zan_73.setTranslate(50.2f, -6.0f, -64.0f);
        this.zan_73.setRotate(0.0f, -120.0f, 0.0f);
        this.zan_74.setTranslate(-12.6f, -6.8f, -182.4f);
        this.zan_74.setRotate(80.0f, 0.0f, 53.33f);
        this.zan_75.setTranslate(50.56f, -15.0f, -166.96f);
        this.zan_75.setRotate(-293.33f, -20.0f, 0.0f);
        this.zan_76.setTranslate(5.4f, 2.8f, 20.8f);
        this.zan_76.setRotate(-160.0f, -240.0f, 0.0f);
        this.zan_77.setTranslate(14.6f, 1.6f, 49.2f);
        this.zan_77.setRotate(-53.33f, -160.0f, 0.0f);
        this.zan_78.setTranslate(7.9f, -0.3f, 60.2f);
        this.zan_78.setRotate(-40.0f, -80.0f, 0.0f);
        this.zan_79.setTranslate(9.8f, 2.4f, 55.2f);
        this.zan_79.setRotate(-80.0f, 0.0f, 0.0f);
        this.zan_80.setTranslate(9.8f, 1.6f, 58.8f);
        this.zan_80.setRotate(-53.33f, 0.0f, 0.0f);
        this.zan_81.setTranslate(12.2f, 1.3f, 51.1f);
        this.zan_81.setRotate(-73.33f, -353.33f, 106.67f);
        this.zan_82.setTranslate(12.32f, -1.36f, 53.06f);
        this.zan_82.setRotate(-683.32f, -650.36f, -172.5f);
        this.eft0.setTranslate(0.0f, -0.022003174f, -0.030499682f);
        this.eft1.setTranslate(0.0f, -0.032501217f, -0.030499682f);
        this.pod2.setTranslate(-0.118999586f, -1.5037379f, 0.5524992f);
        this.pod2.setRotate(-84.31553f, 75.37493f, 31.266346f);
        this.zan_1.start(1, "ita_light2");
        this.zan_2.start(1, "ita_light2");
        this.zan_3.start(1, "ita_light2");
        this.zan_4.start(1, "ita_light2");
        this.zan_5.start(1, "ita_light2");
        this.zan_6.start(1, "iwa_light2");
        this.zan_7.start(1, "iwa_light2");
        this.zan_8.start(1, "iwa_light2");
        this.zan_9.start(1, "iwa_light2");
        this.zan_10.start(1, "iwa_light2");
        this.zan_11.start(1, "ita_light2");
        this.zan_12.start(1, "ita_light2");
        this.zan_13.start(1, "ita_light2");
        this.zan_14.start(1, "ita_light2");
        this.zan_15.start(1, "ita_light2");
        this.zan_16.start(1, "ita_light2");
        this.zan_17.start(1, "ita_light2");
        this.zan_18.start(1, "ita_light2");
        this.zan_19.start(1, "iwa_light2");
        this.zan_20.start(1, "iwa_light2");
        this.zan_21.start(1, "iwa_light2");
        this.zan_22.start(1, "iwa_light2");
        this.zan_72.start(1, "iwa_light2");
        this.zan_6.setScale(0.72f, 0.72f, 0.72f);
        this.zan_7.setScale(0.52f, 0.52f, 0.52f);
        this.zan_8.setScale(0.52f, 0.52f, 0.52f);
        this.zan_9.setScale(0.52f, 0.32f, 0.66f);
        this.zan_10.setScale(0.62f, 0.62f, 0.62f);
        this.zan_19.setScale(0.72f, 0.72f, 0.72f);
        this.zan_20.setScale(0.62f, 0.62f, 0.62f);
        this.zan_21.setScale(0.52f, 0.52f, 0.52f);
        this.zan_22.setScale(0.53f, 0.28f, 0.63f);
        this.zan_72.setScale(1.68f, 0.8f, 0.85f);
        this.zan_73.setScale(0.9f, 0.58f, 1.59f);
        this.zan_74.setScale(0.81f, 1.36f, 0.53f);
        this.zan_75.setScale(0.99f, 0.13f, 0.44f);
        this.zan_76.setScale(1.81f, 1.81f, 1.81f);
        this.zan_77.setScale(0.52f, 0.52f, 0.52f);
        this.zan_78.setScale(0.52f, 0.52f, 0.52f);
        this.zan_79.setScale(0.52f, 0.52f, 0.52f);
        this.zan_80.setScale(0.52f, 0.52f, 0.52f);
        this.zan_81.setScale(0.52f, 0.52f, 0.52f);
        this.zan_82.setScale(0.52f, 0.52f, 0.52f);
        this.elza2.setScale(82.31f, 82.31f, 82.31f);
        this.elza.setScale(82.31f, 82.31f, 82.31f);
        this.pod2.setTranslate(-0.118999586f, -1.5037379f, 0.5524992f);
        this.pod2.setRotate(-84.31553f, 75.37493f, 31.266346f);
        this.pod2.setParent(this.andrew, 13);
        this.dummy.setVisible(false);
        this.star.setScale(1.0f, 1.0f, 1.0f);
    }

    void init() {
        Runtime.setLocation(30);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.cam2 = Camera.create(2);
        this.cam3 = Camera.create(3);
        this.pad0 = Input.create(0);
        this.pad1 = Input.create(1);
        this.shion = new robo(0x100001E, 0.0f, 0.0f, 0.0f, 0.0f);
        this.allen = new robo(16777532, 0.0f, 0.0f, 0.0f, 0.0f);
        this.allen2 = new robo(16777532, 0.0f, 0.0f, 0.0f, 0.0f);
        this.andrew = new robo(0x1000119, 23.07f, -1.79f, -0.21f, 0.0f);
        this.shion.setVisible(false);
        this.allen.setVisible(false);
        this.allen2.setVisible(false);
        this.shion.setMotNoUpdate(2);
        this.allen.setMotNoUpdate(2);
        this.allen2.setMotNoUpdate(2);
        this.shion.setMotionFlags(0x1000000, false);
        this.shion.setMotionFlags(0x800000, false);
        this.shion.setMotionFlags(0x2000000, true);
        this.allen.setMotionFlags(0x1000000, false);
        this.allen.setMotionFlags(0x800000, false);
        this.allen.setMotionFlags(0x2000000, true);
        this.allen2.setMotionFlags(0x1000000, false);
        this.allen2.setMotionFlags(0x800000, false);
        this.allen2.setMotionFlags(0x2000000, true);
        this.dummy1 = new units();
        this.dummy1.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy1.setVisible(false);
        this.dummy2 = new units();
        this.dummy2.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setVisible(false);
        this.dummy3 = new units();
        this.dummy3.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy3.setVisible(false);
        this.dummy4 = new units();
        this.dummy4.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy4.setVisible(false);
        this.dummy5 = new units();
        this.dummy5.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy5.setVisible(false);
        this.dummy6 = new units();
        this.dummy6.init(24602, 2.0f, 0.0f, 0.0f, 0.0f);
        this.dummy6.setVisible(false);
        this.pod_21 = new mapunits();
        this.pod_21.init(21);
        this.pod_21.start(4, null);
        this.pod_22 = new mapunits();
        this.pod_22.init(22);
        this.pod_22.start(4, null);
        this.moni_15 = new units();
        this.moni_15.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_15.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.moni_15.setArgs(1, 22042, 0, 128, 112);
        this.moni_15.setArgs(2, 0, 0, 0, -1);
        this.moni_15.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_15.setScale(0.55f, 0.37f, 0.91f);
        this.moni_15.setTranslate(-0.0025f, 1.1774902f, 1.8749985f);
        this.moni_15.setRotate(-8.333328f, -179.99997f, 0.0f);
        this.moni_17 = new units();
        this.moni_17.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_17.setArgs(0, 0.0f, 0.0f, 1.6f, 0.0f);
        this.moni_17.setArgs(1, 20013, 0, 128, 112);
        this.moni_17.setArgs(2, 101, 0, 0, -1);
        this.moni_17.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_17.setScale(0.21f, 0.14f, 1.0f);
        this.moni_17.setTranslate(0.37974975f, 1.0547409f, 1.8497415f);
        this.moni_17.setRotate(-7.1333146f, -178.42499f, 0.0f);
        this.moni_18 = new units();
        this.moni_18.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_18.setArgs(0, 0.0f, 0.0f, 1.6f, 0.0f);
        this.moni_18.setArgs(1, 22043, 0, 128, 112);
        this.moni_18.setArgs(2, 101, 0, 0, -1);
        this.moni_18.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_18.setScale(0.21f, 0.155f, 1.02f);
        this.moni_18.setTranslate(0.38249996f, 1.2889938f, 1.8292336f);
        this.moni_18.setRotate(-8.499998f, -179.77437f, 0.0f);
        this.moni_19 = new units();
        this.moni_19.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_19.setArgs(0, 0.0f, 0.0f, 1.6f, 0.0f);
        this.moni_19.setArgs(1, 20014, 0, 128, 112);
        this.moni_19.setArgs(2, 101, 0, 0, -1);
        this.moni_19.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_19.setScale(0.22f, 0.16f, 1.0f);
        this.moni_19.setTranslate(-0.39174977f, 1.2947387f, 1.8244967f);
        this.moni_19.setRotate(-8.199994f, -179.99966f, 0.0f);
        this.moni_20 = new units();
        this.moni_20.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_20.setArgs(0, 0.0f, 0.0f, 1.6f, 0.0f);
        this.moni_20.setArgs(1, 20015, 0, 128, 112);
        this.moni_20.setArgs(2, 101, 0, 0, -1);
        this.moni_20.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.moni_20.setScale(0.22f, 0.15f, 0.28f);
        this.moni_20.setTranslate(-0.39299914f, 1.057991f, 1.8537484f);
        this.moni_20.setRotate(-8.399993f, -180.09947f, 0.0f);
        this.alle_moni = new units();
        this.alle_moni.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.alle_moni.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.alle_moni.setArgs(1, 22043, 0, 128, 112);
        this.alle_moni.setArgs(2, 114, 0, 15, -1);
        this.alle_moni.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.alle_moni.setScale(0.23f, 0.23f, 0.23f);
        this.konegi = new units();
        this.konegi.init(24577, 1.17f, 110.5f, 3.36f, 4.88f);
        this.konegi.setScale(0.72f, 0.72f, 0.72f);
        this.konegi.setVisible(false);
        this.star = new units();
        this.star.init(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.star.setScale(1.0f, 1.0f, 1.0f);
        this.elza2 = new kiki();
        this.elza2.init(20533, 3.0f, 0.0f, 0.0f, 0.0f);
        this.elza2.setTranslate(23.07f, -1.79f, -0.21f);
        this.elza2.setRotate(0.0f, 0.0f, 0.0f);
        this.elza2.setScale(100.0f, 100.0f, 100.0f);
        this.elza = new units();
        this.elza.init(20533, 3.0f, 0.0f, 0.0f, 0.0f);
        this.elza.setTranslate(23.07f, -1.79f, -0.21f);
        this.elza.setRotate(0.0f, 0.0f, 0.0f);
        this.elza.setScale(82.31f, 82.31f, 82.31f);
        this.zan_1 = new kiki();
        this.zan_1.init(20545, 2.0f, 0.0f, 0.0f, 0.0f);
        this.zan_1.setTranslate(12.97f, -0.4f, 56.9f);
        this.zan_1.setRotate(-658.33f, -400.0f, -172.5f);
        this.zan_2 = new kiki();
        this.zan_2.init(20546, 3.0f, 0.0f, 0.0f, 0.0f);
        this.zan_2.setTranslate(7.3f, 0.75f, 62.6f);
        this.zan_2.setRotate(-524.77f, 202.5f, 30.0f);
        this.zan_3 = new kiki();
        this.zan_3.init(20547, 4.0f, 0.0f, 0.0f, 0.0f);
        this.zan_3.setTranslate(6.6f, -0.53f, 59.2f);
        this.zan_3.setRotate(-255.0f, 0.0f, -262.5f);
        this.zan_4 = new kiki();
        this.zan_4.init(20548, 5.0f, 0.0f, 0.0f, 0.0f);
        this.zan_4.setTranslate(13.0f, -1.25f, 27.75f);
        this.zan_4.setRotate(-495.0f, 60.0f, 0.0f);
        this.zan_5 = new kiki();
        this.zan_5.init(20545, 25.0f, 0.0f, 0.0f, 0.0f);
        this.zan_5.setTranslate(11.23f, -1.4f, 57.99f);
        this.zan_5.setRotate(-1940.69f, -538.75f, -558.67f);
        this.zan_6 = new kiki();
        this.zan_6.init(20537, 7.0f, 0.0f, 0.0f, 0.0f);
        this.zan_6.setScale(0.72f, 0.72f, 0.72f);
        this.zan_6.setTranslate(42.2f, -2.4f, -51.2f);
        this.zan_6.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_6.start(1, "iwa_light");
        this.zan_7 = new kiki();
        this.zan_7.init(20538, 8.0f, 0.0f, 0.0f, 0.0f);
        this.zan_7.setScale(0.52f, 0.52f, 0.52f);
        this.zan_7.setTranslate(38.8f, 2.0f, -51.2f);
        this.zan_7.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_7.start(1, "iwa_light");
        this.zan_8 = new kiki();
        this.zan_8.init(20539, 9.0f, 0.0f, 0.0f, 0.0f);
        this.zan_8.setScale(0.52f, 0.52f, 0.52f);
        this.zan_8.setTranslate(68.6f, -2.0f, -58.0f);
        this.zan_8.setRotate(-480.0f, 0.0f, 0.0f);
        this.zan_8.start(1, "iwa_light");
        this.zan_9 = new kiki();
        this.zan_9.init(20540, 10.0f, 0.0f, 0.0f, 0.0f);
        this.zan_9.setScale(0.52f, 0.32f, 0.66f);
        this.zan_9.setTranslate(62.3f, -0.08f, -82.5f);
        this.zan_9.setRotate(-950.0f, -60.0f, -400.0f);
        this.zan_9.start(1, "iwa_light");
        this.zan_10 = new kiki();
        this.zan_10.init(20537, 24.0f, 0.0f, 0.0f, 0.0f);
        this.zan_10.setScale(0.62f, 0.62f, 0.62f);
        this.zan_10.setTranslate(69.68f, -0.4f, -39.81f);
        this.zan_10.setRotate(-17.7f, 0.0f, 0.0f);
        this.zan_10.start(1, "iwa_light");
        this.zan_11 = new kiki();
        this.zan_11.init(20545, 11.0f, 0.0f, 0.0f, 0.0f);
        this.zan_11.setTranslate(10.54f, -0.6f, 61.51f);
        this.zan_11.setRotate(-1224.18f, 243.15f, 444.0f);
        this.zan_12 = new kiki();
        this.zan_12.init(20546, 12.0f, 0.0f, 0.0f, 0.0f);
        this.zan_12.setTranslate(8.0f, -1.2f, 24.8f);
        this.zan_12.setRotate(-1080.0f, 680.0f, -160.0f);
        this.zan_13 = new kiki();
        this.zan_13.init(20547, 13.0f, 0.0f, 0.0f, 0.0f);
        this.zan_13.setTranslate(10.6f, 1.5f, 44.0f);
        this.zan_13.setRotate(-800.0f, 160.0f, 0.0f);
        this.zan_14 = new kiki();
        this.zan_14.init(20548, 14.0f, 0.0f, 0.0f, 0.0f);
        this.zan_14.setTranslate(8.9f, 3.3f, 45.27f);
        this.zan_14.setRotate(-678.33f, -730.0f, 300.0f);
        this.zan_15 = new kiki();
        this.zan_15.init(20545, 15.0f, 0.0f, 0.0f, 0.0f);
        this.zan_15.setTranslate(7.87f, -0.3f, 63.9f);
        this.zan_15.setRotate(-1178.75f, 240.0f, 0.0f);
        this.zan_16 = new kiki();
        this.zan_16.init(20546, 16.0f, 0.0f, 0.0f, 0.0f);
        this.zan_16.setTranslate(12.67f, -0.27f, 41.34f);
        this.zan_16.setRotate(-97.38f, -45.0f, 774.17f);
        this.zan_17 = new kiki();
        this.zan_17.init(20547, 17.0f, 0.0f, 0.0f, 0.0f);
        this.zan_17.setTranslate(10.4f, 1.5f, 60.5f);
        this.zan_17.setRotate(-120.0f, 240.0f, 530.0f);
        this.zan_18 = new kiki();
        this.zan_18.init(20548, 18.0f, 0.0f, 0.0f, 0.0f);
        this.zan_18.setTranslate(10.25f, -1.55f, 52.42f);
        this.zan_18.setRotate(-852.5f, 470.0f, 627.5f);
        this.zan_19 = new kiki();
        this.zan_19.init(20537, 19.0f, 0.0f, 0.0f, 0.0f);
        this.zan_19.setScale(0.72f, 0.72f, 0.72f);
        this.zan_19.setTranslate(0.6f, 0.0f, -75.2f);
        this.zan_19.setRotate(-1360.0f, 0.0f, -720.0f);
        this.zan_19.start(1, "iwa_light");
        this.zan_20 = new kiki();
        this.zan_20.init(20538, 21.0f, 0.0f, 0.0f, 0.0f);
        this.zan_20.setScale(0.62f, 0.62f, 0.62f);
        this.zan_20.setTranslate(58.57f, 2.17f, -44.27f);
        this.zan_20.setRotate(-120.0f, 0.0f, 0.0f);
        this.zan_20.start(1, "iwa_light");
        this.zan_21 = new kiki();
        this.zan_21.init(20539, 22.0f, 0.0f, 0.0f, 0.0f);
        this.zan_21.setScale(0.52f, 0.52f, 0.52f);
        this.zan_21.setTranslate(97.8f, -6.8f, -146.8f);
        this.zan_21.setRotate(-573.33f, -180.0f, 0.0f);
        this.zan_21.start(1, "iwa_light");
        this.zan_22 = new kiki();
        this.zan_22.init(20540, 23.0f, 0.0f, 0.0f, 0.0f);
        this.zan_22.setScale(0.53f, 0.28f, 0.63f);
        this.zan_22.setTranslate(20.0f, 0.2f, -91.89f);
        this.zan_22.setRotate(-1270.0f, 100.0f, 0.0f);
        this.zan_22.start(1, "iwa_light");
        this.zan_72 = new kiki();
        this.zan_72.init(20537, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_72.setScale(1.68f, 0.8f, 0.85f);
        this.zan_72.setTranslate(6.2f, -0.4f, -41.2f);
        this.zan_72.setRotate(-400.0f, 0.0f, 0.0f);
        this.zan_72.start(1, "iwa_light");
        this.zan_73 = new kiki();
        this.zan_73.init(20538, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_73.setScale(0.9f, 0.58f, 1.59f);
        this.zan_73.setTranslate(50.2f, -6.0f, -64.0f);
        this.zan_73.setRotate(0.0f, -120.0f, 0.0f);
        this.zan_73.start(1, "iwa_light");
        this.zan_74 = new kiki();
        this.zan_74.init(20539, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_74.setScale(0.81f, 1.36f, 0.53f);
        this.zan_74.setTranslate(-12.6f, -6.8f, -182.4f);
        this.zan_74.setRotate(80.0f, 0.0f, 53.33f);
        this.zan_74.start(1, "iwa_light");
        this.zan_75 = new kiki();
        this.zan_75.init(20540, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_75.setScale(0.99f, 0.13f, 0.44f);
        this.zan_75.setTranslate(50.56f, -15.0f, -166.96f);
        this.zan_75.setRotate(-293.33f, -20.0f, 0.0f);
        this.zan_75.start(1, "iwa_light");
        this.zan_76 = new kiki();
        this.zan_76.init(20547, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_76.setScale(1.81f, 1.81f, 1.81f);
        this.zan_76.setTranslate(5.4f, 2.8f, 20.8f);
        this.zan_76.setRotate(-160.0f, -240.0f, 0.0f);
        this.zan_77 = new kiki();
        this.zan_77.init(20546, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_77.setScale(0.52f, 0.52f, 0.52f);
        this.zan_77.setTranslate(14.6f, 1.6f, 49.2f);
        this.zan_77.setRotate(-53.33f, -160.0f, 0.0f);
        this.zan_78 = new kiki();
        this.zan_78.init(20545, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_78.setScale(0.52f, 0.52f, 0.52f);
        this.zan_78.setTranslate(7.9f, -0.3f, 60.2f);
        this.zan_78.setRotate(-40.0f, -80.0f, 0.0f);
        this.zan_79 = new kiki();
        this.zan_79.init(20545, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_79.setScale(0.52f, 0.52f, 0.52f);
        this.zan_79.setTranslate(9.8f, 2.4f, 55.2f);
        this.zan_79.setRotate(-80.0f, 0.0f, 0.0f);
        this.zan_80 = new kiki();
        this.zan_80.init(20546, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_80.setScale(0.52f, 0.52f, 0.52f);
        this.zan_80.setTranslate(9.8f, 1.6f, 58.8f);
        this.zan_80.setRotate(-53.33f, 0.0f, 0.0f);
        this.zan_81 = new kiki();
        this.zan_81.init(20547, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_81.setScale(0.52f, 0.52f, 0.52f);
        this.zan_81.setTranslate(12.2f, 1.3f, 51.1f);
        this.zan_81.setRotate(-73.33f, -353.33f, 106.67f);
        this.zan_82 = new kiki();
        this.zan_82.init(20548, 23.0f, 10.0f, 0.0f, 0.0f);
        this.zan_82.setScale(0.52f, 0.52f, 0.52f);
        this.zan_82.setTranslate(9.8f, -1.3f, 33.3f);
        this.zan_82.setRotate(-226.66f, -60.0f, 0.0f);
        this.pod2 = new units();
        this.pod2.init(20544, 0.0f, -0.1f, 0.0f, 0.0f);
        this.pod2.setTranslate(-0.34399956f, -2.1137373f, 0.77749914f);
        this.pod2.setRotate(-84.31553f, 75.37493f, 31.266348f);
        this.pod2.setParent(this.andrew, 13);
        this.hikari1 = new Effect(1456, 1.0f, 0.0f, 0.0f, 0.0f);
        this.hikari1.setCaster(this.pod2);
        this.hikari1.setTranslate(0.8050056f, 1.8199975f, 2.129971f);
        this.hikari1.noAttach(false);
        this.hikari2 = new Effect(1456, 2.0f, 0.0f, 0.0f, 0.0f);
        this.hikari2.setCaster(this.pod2);
        this.hikari2.setTranslate(-0.81749344f, 1.819992f, 2.129971f);
        this.hikari2.noAttach(false);
        this.hikari3 = new Effect(1456, 3.0f, 0.0f, 0.0f, 0.0f);
        this.hikari3.setCaster(this.pod2);
        this.hikari3.setTranslate(-0.81749344f, 1.819992f, -2.1600246f);
        this.hikari3.noAttach(false);
        this.hikari4 = new Effect(1456, 4.0f, 0.0f, 0.0f, 0.0f);
        this.hikari4.setCaster(this.pod2);
        this.hikari4.setTranslate(0.8050079f, 1.819992f, -2.1600246f);
        this.hikari4.noAttach(false);
        this.hikari5 = new Effect(1456, 5.0f, 0.0f, 0.0f, 0.0f);
        this.hikari5.setCaster(this.pod2);
        this.hikari5.setTranslate(0.81501305f, -0.12001794f, -2.1024835f);
        this.hikari5.noAttach(false);
        this.hikari6 = new Effect(1456, 6.0f, 0.0f, 0.0f, 0.0f);
        this.hikari6.setCaster(this.pod2);
        this.hikari6.setTranslate(-0.8124859f, -0.120018f, -2.1024835f);
        this.hikari6.noAttach(false);
        this.hikari7 = new Effect(1456, 7.0f, 0.0f, 0.0f, 0.0f);
        this.hikari7.setCaster(this.pod2);
        this.hikari7.setTranslate(-0.8124859f, -0.120018f, 2.1160135f);
        this.hikari7.noAttach(false);
        this.hikari8 = new Effect(1456, 8.0f, 0.0f, 0.0f, 0.0f);
        this.hikari8.setCaster(this.pod2);
        this.hikari8.setTranslate(0.7670136f, -0.120018f, 2.0940137f);
        this.hikari8.noAttach(false);
        this.hikari21 = new Effect(1456, 1.0f, 0.0f, 0.0f, 0.0f);
        this.hikari21.setCaster(this.pod2);
        this.hikari21.setTranslate(0.8050056f, 1.8199975f, 2.129971f);
        this.hikari21.noAttach(false);
        this.hikari22 = new Effect(1456, 2.0f, 0.0f, 0.0f, 0.0f);
        this.hikari22.setCaster(this.pod2);
        this.hikari22.setTranslate(-0.81749344f, 1.819992f, 2.129971f);
        this.hikari22.noAttach(false);
        this.hikari23 = new Effect(1456, 3.0f, 0.0f, 0.0f, 0.0f);
        this.hikari23.setCaster(this.pod2);
        this.hikari23.setTranslate(-0.81749344f, 1.819992f, -2.1600246f);
        this.hikari23.noAttach(false);
        this.hikari24 = new Effect(1456, 4.0f, 0.0f, 0.0f, 0.0f);
        this.hikari24.setCaster(this.pod2);
        this.hikari24.setTranslate(0.8050079f, 1.819992f, -2.1600246f);
        this.hikari24.noAttach(false);
        this.hikari25 = new Effect(1456, 5.0f, 0.0f, 0.0f, 0.0f);
        this.hikari25.setCaster(this.pod2);
        this.hikari25.setTranslate(0.81501305f, -0.12001794f, -2.1024835f);
        this.hikari25.noAttach(false);
        this.hikari26 = new Effect(1456, 6.0f, 0.0f, 0.0f, 0.0f);
        this.hikari26.setCaster(this.pod2);
        this.hikari26.setTranslate(-0.8124859f, -0.120018f, -2.1024835f);
        this.hikari26.noAttach(false);
        this.hikari27 = new Effect(1456, 7.0f, 0.0f, 0.0f, 0.0f);
        this.hikari27.setCaster(this.pod2);
        this.hikari27.setTranslate(-0.8124859f, -0.120018f, 2.1160135f);
        this.hikari27.noAttach(false);
        this.hikari28 = new Effect(1456, 8.0f, 0.0f, 0.0f, 0.0f);
        this.hikari28.setCaster(this.pod2);
        this.hikari28.setTranslate(0.7670136f, -0.120018f, 2.0940137f);
        this.hikari28.noAttach(false);
        this.hikari1.setScale(1.2f, 1.2f, 0.5f);
        this.hikari2.setScale(1.2f, 1.2f, 0.5f);
        this.hikari3.setScale(1.2f, 1.2f, 0.5f);
        this.hikari4.setScale(1.2f, 1.2f, 0.5f);
        this.hikari5.setScale(1.2f, 1.2f, 0.5f);
        this.hikari6.setScale(1.2f, 1.2f, 0.5f);
        this.hikari7.setScale(1.2f, 1.2f, 0.5f);
        this.hikari8.setScale(1.2f, 1.2f, 0.5f);
        this.hikari21.setScale(0.75f, 0.75f, 0.5f);
        this.hikari22.setScale(0.75f, 0.75f, 0.5f);
        this.hikari23.setScale(0.75f, 0.75f, 0.5f);
        this.hikari24.setScale(0.75f, 0.75f, 0.5f);
        this.hikari25.setScale(0.75f, 0.75f, 0.5f);
        this.hikari26.setScale(0.75f, 0.75f, 0.5f);
        this.hikari27.setScale(0.75f, 0.75f, 0.5f);
        this.hikari28.setScale(0.75f, 0.75f, 0.5f);
        this.hikari1.disp(false);
        this.hikari2.disp(false);
        this.hikari3.disp(false);
        this.hikari4.disp(false);
        this.hikari5.disp(false);
        this.hikari6.disp(false);
        this.hikari7.disp(false);
        this.hikari8.disp(false);
        this.hikari21.disp(false);
        this.hikari22.disp(false);
        this.hikari23.disp(false);
        this.hikari24.disp(false);
        this.hikari25.disp(false);
        this.hikari26.disp(false);
        this.hikari27.disp(false);
        this.hikari28.disp(false);
        this.eft0 = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.eft0.setCaster(this.elza);
        this.eft0.setTranslate(0.0f, -0.022003174f, -0.030499682f);
        this.eft0.disp(false);
        this.eft1 = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1.setScale(0.1f, 0.1f, 0.1f);
        this.eft1.setCaster(this.elza);
        this.eft1.setTranslate(0.0f, -0.032501217f, -0.030499682f);
        this.eft1.disp(false);
        this.p_ef01 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef01.setCaster(this.pod2);
        this.p_ef01.noAttach(false);
        this.p_ef01.setTranslate(-0.3499987f, 0.8799874f, -2.5999813f);
        this.p_ef01.setRotate(0.0f, 1.4999999f, 0.0f);
        this.p_ef01.setScale(0.02f, 0.01f, 0.04f);
        this.p_ef01.disp(true);
        this.p_ef02 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef02.setCaster(this.pod2);
        this.p_ef02.noAttach(false);
        this.p_ef02.setTranslate(-0.3499987f, 0.6099975f, -2.5999813f);
        this.p_ef02.setRotate(0.0f, 1.4999999f, 0.0f);
        this.p_ef02.setScale(0.02f, 0.01f, 0.04f);
        this.p_ef02.disp(true);
        this.p_ef03 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef03.setCaster(this.pod2);
        this.p_ef03.setTranslate(0.340001f, 0.6099929f, -2.6099813f);
        this.p_ef03.setRotate(0.0f, 1.4999999f, 0.0f);
        this.p_ef03.noAttach(false);
        this.p_ef03.setScale(0.02f, 0.01f, 0.04f);
        this.p_ef03.disp(true);
        this.p_ef04 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef04.setCaster(this.pod2);
        this.p_ef04.noAttach(false);
        this.p_ef04.setTranslate(0.340001f, 0.8799874f, -2.6099813f);
        this.p_ef04.setRotate(0.0f, 1.4999999f, 0.0f);
        this.p_ef04.setScale(0.02f, 0.01f, 0.04f);
        this.p_ef04.disp(true);
        this.p_ef05 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef05.setCaster(this.pod2);
        this.p_ef05.noAttach(false);
        this.p_ef05.setTranslate(-0.7249988f, 1.7574768f, -1.6899801f);
        this.p_ef05.setRotate(90.83329f, 1.4999999f, 0.0f);
        this.p_ef06 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef06.setCaster(this.pod2);
        this.p_ef06.noAttach(false);
        this.p_ef06.setTranslate(-0.7249988f, 1.7574768f, 1.8150196f);
        this.p_ef06.setRotate(90.83329f, 1.4999999f, 0.0f);
        this.p_ef07 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef07.setCaster(this.pod2);
        this.p_ef07.noAttach(false);
        this.p_ef07.setTranslate(0.655001f, -0.06252326f, -2.1249797f);
        this.p_ef07.setRotate(-75.83333f, 1.4999999f, -186.6666f);
        this.p_ef08 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef08.setCaster(this.pod2);
        this.p_ef08.noAttach(false);
        this.p_ef08.setTranslate(-0.9449971f, 1.657471f, -1.6849802f);
        this.p_ef08.setRotate(-169.16661f, 97.49999f, -253.33287f);
        this.p_ef09 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef09.setCaster(this.pod2);
        this.p_ef09.noAttach(false);
        this.p_ef09.setTranslate(-0.9449971f, 0.09747117f, -1.6849802f);
        this.p_ef09.setRotate(-169.16661f, 105.49999f, -253.33287f);
        this.p_ef10 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef10.setCaster(this.pod2);
        this.p_ef10.noAttach(false);
        this.p_ef10.setTranslate(0.9350019f, 1.6974638f, 1.7550176f);
        this.p_ef10.setRotate(-457.49887f, 218.99995f, -303.33203f);
        this.p_ef11 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef11.setCaster(this.pod2);
        this.p_ef11.noAttach(false);
        this.p_ef11.setTranslate(0.6950002f, 1.6974638f, -2.3249795f);
        this.p_ef11.setRotate(-489.16537f, 209.5f, -386.66534f);
        this.p_ef12 = new Effect(1556, 0.0f, 0.0f, 0.0f, 0.0f);
        this.p_ef12.setCaster(this.pod2);
        this.p_ef12.noAttach(false);
        this.p_ef12.setTranslate(-0.9449975f, 0.13746442f, 2.1150174f);
        this.p_ef12.setRotate(-489.1648f, 409.49997f, -466.66507f);
        this.p_ef05.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef06.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef07.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef08.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef09.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef10.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef11.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef12.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef01.disp(false);
        this.p_ef02.disp(false);
        this.p_ef03.disp(false);
        this.p_ef04.disp(false);
        this.p_ef05.disp(false);
        this.p_ef06.disp(false);
        this.p_ef07.disp(false);
        this.p_ef08.disp(false);
        this.p_ef09.disp(false);
        this.p_ef10.disp(false);
        this.p_ef11.disp(false);
        this.p_ef12.disp(false);
        this.zan_1.setShadow(0, 0);
        this.zan_2.setShadow(0, 0);
        this.zan_3.setShadow(0, 0);
        this.zan_4.setShadow(0, 0);
        this.zan_5.setShadow(0, 0);
        this.zan_6.setShadow(0, 0);
        this.zan_7.setShadow(0, 0);
        this.zan_8.setShadow(0, 0);
        this.zan_9.setShadow(0, 0);
        this.zan_10.setShadow(0, 0);
        this.zan_11.setShadow(0, 0);
        this.zan_12.setShadow(0, 0);
        this.zan_13.setShadow(0, 0);
        this.zan_14.setShadow(0, 0);
        this.zan_15.setShadow(0, 0);
        this.zan_16.setShadow(0, 0);
        this.zan_17.setShadow(0, 0);
        this.zan_18.setShadow(0, 0);
        this.zan_19.setShadow(0, 0);
        this.zan_20.setShadow(0, 0);
        this.zan_21.setShadow(0, 0);
        this.zan_22.setShadow(0, 0);
        this.zan_72.setShadow(0, 0);
        this.zan_73.setShadow(0, 0);
        this.zan_74.setShadow(0, 0);
        this.zan_75.setShadow(0, 0);
        this.zan_76.setShadow(0, 0);
        this.zan_77.setShadow(0, 0);
        this.zan_78.setShadow(0, 0);
        this.zan_79.setShadow(0, 0);
        this.zan_80.setShadow(0, 0);
        this.zan_81.setShadow(0, 0);
        this.zan_82.setShadow(0, 0);
        this.elza2.setShadow(0, 0);
        this.dummy = new units();
        this.dummy.init(24602, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy.setVisible(false);
        this._spl_thread_main = Thread.create(this, "__chracter_splne");
        this._spl_thread_main.start();
        System.methodSignal(1);
    }

    void loadarc(Chr chr, String string) {
        Object object = Toolkit.loadResource(string);
        Toolkit.loadResource((Object) chr, object, 3);
    }

    static void main() {
    }

    void play() {
        System.sleep(1);
        int n = 0;
        while (n <= 35) {
            Stage.setVisible(n, false);
            ++n;
        }
        this.elza.setVisible(false);
        this.elza2.setVisible(false);
        this.pod2.renderCommand(32);
        this.elza.renderCommand(32);
        this.elza2.renderCommand(32);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.hikari1.disp(true);
        this.hikari2.disp(true);
        this.hikari3.disp(true);
        this.hikari4.disp(true);
        this.hikari5.disp(true);
        this.hikari6.disp(true);
        this.hikari7.disp(true);
        this.hikari8.disp(true);
        this.elza2.setVisible(false);
        this.andrew.start(1, "sigamituki");
        this.andrew.setTranslate(23.13f, -2.4f, 2.04f);
        this.andrew.setRotate(55.88f, -481.1f, -14.58f);
        this.cam3.setTranslate(23.13f, -2.4f, 2.04f);
        this.cam3.setRotate(55.88f, -481.1f, -14.58f);
        float[] fArray = new float[]{1.0f, 23.13f, -2.4f, 2.04f, 128.0f, 22.25f, -0.12f, 1.45f};
        float[] fArray2 = new float[]{1.0f, 55.88f, -481.1f, -14.58f, 128.0f, 53.62f, -477.85f, 44.32f};
        this.cam3.transSPL(fArray, 1, 2, 159);
        this.cam3.rotateSPL(fArray2, 1, 2, 159);
        this.andrew_flag = 1;
        this.zan_2.setLightMode(1);
        this.zan_2.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zan_2.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.zan_2.light.setDirection2(1, -0.659f, 0.555f, 0.507f);
        this.zan_2.light.setColor(2, 0.47f, 0.47f, 0.47f);
        this.zan_2.light.setDirection2(2, -0.794f, 0.576f, 0.195f);
        this.zan_2.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.zan_2.light.setDirection2(3, -0.347f, -0.431f, 0.833f);
        this.zan_15.setLightMode(1);
        this.zan_15.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zan_15.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.zan_15.light.setDirection2(1, -0.659f, 0.555f, 0.507f);
        this.zan_15.light.setColor(2, 0.47f, 0.47f, 0.47f);
        this.zan_15.light.setDirection2(2, -0.794f, 0.576f, 0.195f);
        this.zan_15.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.zan_15.light.setDirection2(3, -0.347f, -0.431f, 0.833f);
        this.light.setColor(0, 0.0f, 0.0f, 0.0f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.232f, 0.359f, 0.904f);
        this.light.setColor(2, 0.59f, 0.59f, 0.59f);
        this.light.setDirection2(2, -0.691f, 0.723f, -0.015f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.654f, 0.731f, -0.197f);
        this.cam1.change();
        Sound.streamPlay(1190076, 48000);
        this.camerawork.cut77();
        this.__wait();
        this.zan_6.start(1, "zangai_stone_spin1");
        this.zan_7.start(1, "zangai_stone_spin2");
        this.zan_8.start(1, "zangai_stone_spin1");
        this.zan_9.start(1, "zangai_stone_spin2");
        this.zan_10.start(1, "zangai_stone_spin1");
        this.zan_19.start(1, "zangai_stone_spin2");
        this.zan_20.start(1, "zangai_stone_spin1");
        this.zan_21.start(1, "zangai_stone_spin2");
        this.zan_22.start(1, "zangai_stone_spin1");
        this.zan_15.start(1, "zangai_ita_spin2");
        this.zan_3.start(1, "zangai_ita_spin5");
        this.zan_2.start(1, "zangai_ita_spin1");
        this.zan_17.start(1, "zangai_ita_spin6");
        this.zan_18.start(1, "zangai_ita_spin8");
        this.zan_16.start(1, "zangai_spin7");
        this.zan_1.start(1, "zangai_spin6");
        this.zan_4.start(1, "zangai_spin5");
        this.zan_5.start(1, "zangai_spin4");
        this.zan_11.start(1, "zangai_spin3");
        this.zan_12.start(1, "zangai_spin2");
        this.zan_13.start(1, "zangai_spin1");
        this.zan_14.start(1, "zangai_spin8");
        System.sleep(156);
        this.__wait();
        this.camerawork.cut78();
        this.hikari1.disp(false);
        this.hikari2.disp(false);
        this.hikari3.disp(false);
        this.hikari4.disp(false);
        this.hikari5.disp(false);
        this.hikari6.disp(false);
        this.hikari7.disp(false);
        this.hikari8.disp(false);
        this.hikari21.disp(true);
        this.hikari22.disp(true);
        this.hikari23.disp(true);
        this.hikari24.disp(true);
        this.hikari25.disp(true);
        this.hikari26.disp(true);
        this.hikari27.disp(true);
        this.hikari28.disp(true);
        this.zan_1.setTranslate(6.02f, 1.0f, 67.7f);
        this.zan_1.setRotate(-340.02f, -565.57f, -72.5f);
        this.zan_2.setTranslate(9.17f, 0.62f, 61.7f);
        this.zan_2.setRotate(-520.87f, 234.22f, 69.15f);
        this.zan_3.setTranslate(5.52f, -0.53f, 70.8f);
        this.zan_3.setRotate(-1385.0f, 772.0f, -187.5f);
        this.zan_4.setTranslate(3.32f, 1.5f, 58.05f);
        this.zan_4.setRotate(-620.0f, 1175.62f, 134.07f);
        this.zan_5.setTranslate(8.84f, 1.05f, 69.19f);
        this.zan_5.setRotate(-1910.69f, -1712.86f, -570.14f);
        this.zan_6.setTranslate(5.4f, -4.0f, -57.6f);
        this.zan_6.setRotate(-800.0f, 0.0f, 160.0f);
        this.zan_7.setTranslate(-9.95f, 2.0f, -46.2f);
        this.zan_7.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_8.setTranslate(62.2f, -10.0f, -51.6f);
        this.zan_8.setRotate(-480.0f, 0.0f, 0.0f);
        this.zan_9.setTranslate(89.7f, -4.68f, -74.9f);
        this.zan_9.setRotate(-950.0f, 0.0f, -400.0f);
        this.zan_10.setTranslate(48.88f, 6.0f, -23.81f);
        this.zan_10.setRotate(-17.7f, 0.0f, 0.0f);
        this.zan_11.setTranslate(8.04f, -1.65f, 67.16f);
        this.zan_11.setRotate(-1476.08f, 1528.48f, 444.0f);
        this.zan_12.setTranslate(6.4f, -1.2f, 64.8f);
        this.zan_12.setRotate(-2986.32f, 680.0f, -297.48f);
        this.zan_13.setTranslate(6.4f, -1.65f, 53.55f);
        this.zan_13.setRotate(1566.36f, -56.87f, 75.0f);
        this.zan_14.setTranslate(12.5f, -1.7f, 60.87f);
        this.zan_14.setRotate(-1020.86f, -370.0f, -1261.59f);
        this.zan_15.setTranslate(6.29f, 0.6f, 72.6f);
        this.zan_15.setRotate(-1874.82f, 240.0f, 0.0f);
        this.zan_16.setTranslate(7.06f, -0.25f, 68.55f);
        this.zan_16.setRotate(-152.79f, -420.0f, 7058.7f);
        this.zan_17.setTranslate(10.4f, 1.5f, 60.5f);
        this.zan_17.setRotate(-120.0f, -961.18f, 530.0f);
        this.zan_18.setTranslate(8.32f, -1.2f, 69.57f);
        this.zan_18.setRotate(-1327.5f, 570.0f, -752.64f);
        this.zan_19.setTranslate(-5.45f, 3.45f, -66.8f);
        this.zan_19.setRotate(-1360.0f, 0.0f, -720.0f);
        this.zan_20.setTranslate(67.57f, 2.17f, -44.27f);
        this.zan_20.setRotate(-120.0f, 0.0f, 0.0f);
        this.zan_21.setTranslate(-0.65f, 12.85f, -69.85f);
        this.zan_21.setRotate(-280.0f, 0.0f, 240.0f);
        this.zan_22.setTranslate(-2.6f, -4.6f, -91.69f);
        this.zan_22.setRotate(-1230.0f, 160.0f, 0.0f);
        this.__wait();
        this.cam3.setTranslate(9.69f, -0.44f, 64.46f);
        this.cam3.setRotate(53.62f, -479.85f, 19.65f);
        float[] fArray3 = new float[]{1.0f, 9.69f, -0.44f, 64.46f, 128.0f, 9.25f, 0.71f, 63.63f};
        float[] fArray4 = new float[]{1.0f, 53.62f, -479.85f, 19.65f, 128.0f, 56.82f, -477.45f, 59.48f};
        this.cam3.transSPL(fArray3, 1, 2, 177);
        this.cam3.rotateSPL(fArray4, 1, 2, 177);
        this.zan_6.start(1, "zangai_stone_spin1");
        this.zan_7.start(1, "zangai_stone_spin2");
        this.zan_8.start(1, "zangai_stone_spin1");
        this.zan_9.start(1, "zangai_stone_spin2");
        this.zan_10.start(1, "zangai_stone_spin1");
        this.zan_19.start(1, "zangai_stone_spin2");
        this.zan_20.start(1, "zangai_stone_spin1");
        this.zan_21.start(1, "zangai_stone_spin2");
        this.zan_22.start(1, "zangai_stone_spin1");
        this.zan_15.start(1, "zangai_ita_spin2_cut2");
        this.zan_3.start(1, "zangai_ita_spin5_cut2");
        this.zan_2.start(1, "zangai_ita_spin1_cut2");
        this.zan_17.start(1, "zangai_ita_spin6_cut2");
        this.zan_18.start(1, "zangai_ita_spin8_cut2");
        this.zan_16.start(1, "zangai_spin7_cut2");
        this.zan_1.start(1, "zangai_spin6_cut2");
        this.zan_4.start(1, "zangai_spin5_cut2");
        this.zan_5.start(1, "zangai_spin4_cut2");
        this.zan_11.start(1, "zangai_spin3_cut2");
        this.zan_12.start(1, "zangai_spin2_cut2");
        this.zan_13.start(1, "zangai_spin1_cut2");
        this.zan_14.start(1, "zangai_spin8_cut2");
        System.sleep(170);
        int[] nArray = new int[5];
        nArray[0] = 0x2000000;
        int[] nArray2 = nArray;
        Runtime.setDefocus(0, 10, nArray2);
        System.sleep(1);
        this.hikari1.disp(false);
        this.hikari2.disp(false);
        this.hikari3.disp(false);
        this.hikari4.disp(false);
        this.hikari5.disp(false);
        this.hikari6.disp(false);
        this.hikari7.disp(false);
        this.hikari8.disp(false);
        this.hikari21.disp(false);
        this.hikari22.disp(false);
        this.hikari23.disp(false);
        this.hikari24.disp(false);
        this.hikari25.disp(false);
        this.hikari26.disp(false);
        this.hikari27.disp(false);
        this.hikari28.disp(false);
        this.zan_1.start(1, "kara_stop");
        this.zan_2.start(1, "kara_stop");
        this.zan_3.start(1, "kara_stop");
        this.zan_4.start(1, "kara_stop");
        this.zan_5.start(1, "kara_stop");
        this.zan_6.start(1, "kara_stop");
        this.zan_7.start(1, "kara_stop");
        this.zan_8.start(1, "kara_stop");
        this.zan_9.start(1, "kara_stop");
        this.zan_10.start(1, "kara_stop");
        this.zan_11.start(1, "kara_stop");
        this.zan_12.start(1, "kara_stop");
        this.zan_13.start(1, "kara_stop");
        this.zan_14.start(1, "kara_stop");
        this.zan_15.start(1, "kara_stop");
        this.zan_16.start(1, "kara_stop");
        this.zan_17.start(1, "kara_stop");
        this.zan_18.start(1, "kara_stop");
        this.zan_19.start(1, "kara_stop");
        this.zan_20.start(1, "kara_stop");
        this.zan_21.start(1, "kara_stop");
        this.zan_22.start(1, "kara_stop");
        this.zan_72.start(1, "kara_stop");
        this.zan_73.start(1, "kara_stop");
        this.zan_74.start(1, "kara_stop");
        this.zan_75.start(1, "kara_stop");
        this.zan_76.start(1, "kara_stop");
        this.zan_77.start(1, "kara_stop");
        this.zan_78.start(1, "kara_stop");
        this.zan_79.start(1, "kara_stop");
        this.zan_80.start(1, "kara_stop");
        this.zan_81.start(1, "kara_stop");
        this.zan_82.start(1, "kara_stop");
        this.zan_1.setVisible(false);
        this.zan_2.setVisible(false);
        this.zan_3.setVisible(false);
        this.zan_4.setVisible(false);
        this.zan_5.setVisible(false);
        this.zan_6.setVisible(false);
        this.zan_7.setVisible(false);
        this.zan_8.setVisible(false);
        this.zan_9.setVisible(false);
        this.zan_10.setVisible(false);
        this.zan_11.setVisible(false);
        this.zan_12.setVisible(false);
        this.zan_13.setVisible(false);
        this.zan_14.setVisible(false);
        this.zan_15.setVisible(false);
        this.zan_16.setVisible(false);
        this.zan_17.setVisible(false);
        this.zan_18.setVisible(false);
        this.zan_19.setVisible(false);
        this.zan_20.setVisible(false);
        this.zan_21.setVisible(false);
        this.zan_22.setVisible(false);
        this.zan_72.setVisible(false);
        this.zan_73.setVisible(false);
        this.zan_74.setVisible(false);
        this.zan_75.setVisible(false);
        this.zan_76.setVisible(false);
        this.zan_77.setVisible(false);
        this.zan_78.setVisible(false);
        this.zan_79.setVisible(false);
        this.zan_80.setVisible(false);
        this.zan_81.setVisible(false);
        this.zan_82.setVisible(false);
        this.star.setVisible(false);
        this.elza.setVisible(false);
        this.elza2.setVisible(false);
        this.pod2.setVisible(false);
        this.andrew.setVisible(false);
        n = 0;
        while (n <= 35) {
            Stage.setVisible(n, true);
            ++n;
        }
        this.shion.setVisible(true);
        this.allen.setVisible(true);
        this.allen2.setVisible(false);
        this.shion.face.mtn(289, 8, 1.0f, false);
        this.shion.face.start(4, null);
        this.allen.face.mtn(273, 8, 1.0f, false);
        this.allen.face.start(4, null);
        this.allen2.face.mtn(273, 8, 1.0f, false);
        this.allen2.face.start(4, null);
        this.shion.setMotionFlags(0x40000000, false);
        this.allen.setMotionFlags(0x40000000, false);
        this.konegi.setParent(this.allen, 60);
        this.konegi.setTranslate(0.10399982f, -0.016499987f, -0.0399999f);
        this.konegi.setRotate(-194.77834f, 4.753337f, -86.436295f);
        this.alle_moni.setParent(this.allen, 60);
        this.alle_moni.setTranslate(0.225f, 0.027499095f, -0.15000002f);
        this.alle_moni.setRotate(-20.210026f, 55.799995f, -126.46999f);
        this.cam1.change();
        this.__wait();
        this.alle_moni.signal(1);
        this.konegi.setVisible(true);
        this.camerawork.cut001();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.461f, 0.324f, 0.826f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.536f, 0.736f, -0.413f);
        this.light.setColor(3, 0.27f, 0.32f, 0.32f);
        this.light.setDirection2(3, -0.64f, -0.671f, 0.375f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setTranslate(-0.01f, 0.03f, 1.31f);
        this.shion.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.setTranslate(0.0f, 0.01f, -0.68f);
        this.allen.setRotate(0.0f, 0.0f, 0.0f);
        this.shion.start(1, "motion_01");
        this.allen.start(1, "motion_02");
        System.sleep(66);
        this.dummy1.start(1, "allen_serifu_001");
        System.sleep(69);
        this.__wait();
        this.camerawork.cut002();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.47f, 0.47f, 0.47f);
        this.light.setDirection2(1, -0.485f, 0.281f, 0.828f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.199f, 0.543f, -0.816f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, -0.16f, -0.734f, 0.66f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.start(1, "motion_03");
        System.sleep(96);
        this.MSGW("C'mon Chief, you give it a try.");
        this.dummy1.start(1, "allen_serifu_002");
        System.sleep(66);
        this.__wait();
        this.camerawork.cut003_0();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.616f, 0.001f, -0.788f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.alle_moni.setTranslate(0.28000003f, 0.08749615f, -0.12499974f);
        this.alle_moni.setRotate(-27.876621f, 21.29992f, -96.803185f);
        this.shion.setTranslate(0.018499985f, 0.031999998f, 1.2809936f);
        this.shion.setRotate(0.0f, 1.8499994f, 0.0f);
        this.allen.start(1, "motion_05");
        this.shion.start(1, "motion_04");
        this.dummy2.start(1, "shion_serifu_003_0");
        System.sleep(45);
        this.dummy1.start(1, "allen_moni_off");
        System.sleep(105);
        this.allen2.setTranslate(0.0f, 0.019999998f, -0.7249988f);
        this.allen2.setRotate(0.0f, 0.0f, 0.0f);
        this.allen2.start(1, "motion_06_mae");
        this.MSGW("I'm more concerned about KOS-MOS.");
        System.sleep(72);
        this.MSGW("Now where could she be...");
        System.sleep(66);
        this.MSGW("How would I know?");
        this.dummy1.start(1, "allen_serifu_003_0");
        System.sleep(45);
        this.__wait();
        this.camerawork.cut003_2();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.616f, 0.001f, -0.788f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen2.setVisible(true);
        this.allen.setVisible(false);
        this.konegi.setParent(this.allen2, 60);
        this.konegi.setTranslate(0.10399982f, -0.016499987f, -0.0399999f);
        this.konegi.setRotate(-194.77834f, 4.753337f, -86.436295f);
        this.allen2.start(1, "motion_06");
        this.allen.setTranslate(0.0f, 0.019999998f, -1.1799994f);
        this.allen.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.start(1, "motion_110_mae");
        this.MSGW("Besides, we're not\nout of the woods yet.");
        this.dummy1.start(1, "allen_serifu_003_2");
        System.sleep(63);
        this.MSGW("I mean, the Gnosis might still\nbe around...");
        System.sleep(96);
        this.__wait();
        this.camerawork.cut003_00();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.83f, 0.366f, -0.421f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen.setVisible(true);
        this.allen2.setVisible(false);
        this.konegi.setParent(this.allen, 60);
        this.konegi.setTranslate(0.10399982f, -0.016499987f, -0.0399999f);
        this.konegi.setRotate(-194.77834f, 4.753337f, -86.436295f);
        this.allen.setTranslate(0.0f, 0.019999998f, -1.1799994f);
        this.allen.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.start(1, "motion_110");
        this.shion.start(1, "motion_07");
        this.MSGW("I'm not worried about that right now.\nWe have to retrieve KOS-MOS...");
        this.dummy2.start(1, "shion_serifu_003__2");
        System.sleep(54);
        this.cam2.setTranslate(0.0f, 0.0f, 0.0f);
        this.wait_clr(96);
        this.__wait();
        this.camerawork.cut003_1();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.616f, 0.001f, -0.788f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.start(1, "motion_09_mae");
        float[] fArray5 = new float[12];
        fArray5[0] = 1.0f;
        fArray5[4] = 29.0f;
        fArray5[7] = -0.15f;
        fArray5[8] = 57.0f;
        fArray5[11] = -0.25f;
        float[] fArray6 = fArray5;
        this.cam2.transSPL(fArray6, 1);
        this.pod_flag = 1;
        this.shion.setTranslate(-0.04599987f, -0.009499995f, 1.1994765f);
        this.shion.setRotate(8.033324f, -7.4999967f, 0.0f);
        this.shion.start(1, "motion_09");
        this.dummy1.start(1, "moni_15_on");
        System.sleep(22);
        this.dummy2.start(1, "moni_sonotaon1");
        System.sleep(2);
        this.dummy4.start(1, "moni_sonotaon3");
        System.sleep(4);
        this.dummy3.start(1, "moni_sonotaon2");
        System.sleep(2);
        this.dummy5.start(1, "moni_sonotaon4");
        System.sleep(101);
        this.allen.start(1, "motion_010");
        this.dummy6.start(1, "allen_serifu_003_7");
        this.MSGW("Forget about KOS-MOS,\nwhat about us?");
        System.sleep(27);
        this.__wait();
        this.pod_flag = 0;
        this.camerawork.cut003_7();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.63f, 0.16f, 0.76f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.554f, 0.346f, -0.757f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.486f, -0.507f, 0.712f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.setVisible(0, true);
        this.shion.setTranslate(0.019999998f, 0.029999997f, 1.2749993f);
        this.shion.setRotate(0.0f, 1.9999998f, 0.0f);
        this.shion.start(1, "motion_09_2");
        this.allen.setTranslate(0.0f, 0.01f, -0.68f);
        this.allen.setRotate(0.0f, 0.0f, 0.0f);
        System.sleep(57);
        this.MSGW("Ahh...If I knew this was gonna happen,\nI wouldn't have let everyone else\ngo first...");
        System.sleep(147);
        this.allen.look_default();
        this.allen.look_speed(0.0f);
        this.allen.look_point(-0.36f, 5.21f, -1.22f);
        this.allen2.start(1, "motion_12_mae");
        this.MSGW("Maybe they're still around...\nyou think?");
        System.sleep(114);
        this.__wait();
        this.camerawork.cut003_8();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.143f, 0.0f, 0.99f);
        this.light.setColor(2, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(2, 0.554f, 0.346f, -0.757f);
        this.light.setColor(3, 0.18f, 0.23f, 0.23f);
        this.light.setDirection2(3, 0.486f, -0.507f, 0.712f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.shion.start(1, "motion_11");
        this.allen2.start(1, "motion_12");
        this.allen.look_default();
        this.allen.setVisible(false);
        this.allen2.setVisible(true);
        this.allen.start(1, "motion_14_mae");
        this.MSGW("I can't believe you...");
        System.sleep(57);
        this.__wait();
        this.moni_15.signal(0);
        this.moni_17.signal(0);
        this.moni_18.signal(0);
        this.moni_19.signal(0);
        this.moni_20.signal(0);
        this.moni_15.setTranslate(0.0f, 1000.0f, 0.0f);
        this.moni_17.setTranslate(0.0f, 1000.0f, 0.0f);
        this.moni_18.setTranslate(0.0f, 1000.0f, 0.0f);
        this.moni_19.setTranslate(0.0f, 1000.0f, 0.0f);
        this.moni_20.setTranslate(0.0f, 1000.0f, 0.0f);
        this.camerawork.cut003_9();
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.56f, 0.56f, 0.56f);
        this.light.setDirection2(1, -0.678f, 0.359f, 0.641f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.798f, 0.316f, -0.513f);
        this.light.setColor(3, 0.36f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.346f, -0.501f, 0.793f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.allen2.setVisible(false);
        this.allen.setVisible(true);
        Stage.setVisible(23, false);
        this.pod_21.setTranslate(0.0f, 0.031999964f, -0.24999997f);
        this.pod_21.setRotate(0.0f, 0.0f, 0.0f);
        this.pod_22.setTranslate(0.0f, 0.037994385f, -0.24999997f);
        this.pod_22.setRotate(0.0f, 0.0f, 0.0f);
        this.allen.start(1, "motion_14");
        this.shion.setTranslate(-0.01f, 0.03f, 1.3f);
        this.shion.setRotate(0.0f, 2.0f, 0.0f);
        this.shion.start(1, "motion_13");
        this.MSGW("Allen...\nHow can you be so insensitive? ");
        this.dummy2.start(1, "shion_serifu_003_9");
        System.sleep(114);
        this.MSGW("After all that's happened...");
        this.wait_clr(48);
        this.wait_clr(6);
        this.allen.setVisible(false);
        this.allen2.setVisible(false);
        this.shion.setVisible(false);
        n = 0;
        while (n <= 35) {
            Stage.setVisible(n, false);
            ++n;
        }
        this.moni_15.signal(0);
        this.moni_17.signal(0);
        this.moni_18.signal(0);
        this.moni_19.signal(0);
        this.moni_20.signal(0);
        this.alle_moni.signal(0);
        this.konegi.setVisible(false);
        this.__wait();
        Runtime.mpeg2("1043_1");
        Sound.streamPlay(1190075, 48000);
        this.zan_1.setVisible(true);
        this.zan_2.setVisible(true);
        this.zan_3.setVisible(true);
        this.zan_4.setVisible(true);
        this.zan_5.setVisible(true);
        this.zan_6.setVisible(true);
        this.zan_7.setVisible(true);
        this.zan_8.setVisible(true);
        this.zan_9.setVisible(true);
        this.zan_10.setVisible(true);
        this.zan_11.setVisible(true);
        this.zan_12.setVisible(true);
        this.zan_13.setVisible(true);
        this.zan_14.setVisible(true);
        this.zan_15.setVisible(true);
        this.zan_16.setVisible(true);
        this.zan_17.setVisible(true);
        this.zan_18.setVisible(true);
        this.zan_19.setVisible(true);
        this.zan_20.setVisible(true);
        this.zan_21.setVisible(true);
        this.zan_22.setVisible(true);
        this.zan_72.setVisible(true);
        this.zan_73.setVisible(true);
        this.zan_74.setVisible(true);
        this.zan_75.setVisible(true);
        this.zan_76.setVisible(true);
        this.zan_77.setVisible(true);
        this.zan_78.setVisible(true);
        this.zan_79.setVisible(true);
        this.zan_80.setVisible(true);
        this.zan_81.setVisible(true);
        this.zan_82.setVisible(true);
        this.star.setVisible(true);
        this.eft0.disp(true);
        this.eft1.disp(true);
        this.zan_2.setLightMode(0);
        this.zan_15.setLightMode(0);
        this.elza.setVisible(true);
        this.pod2.setVisible(false);
        this.gareki_syoki_iti2();
        this.pod2.renderCommand(32);
        this.elza.renderCommand(32);
        this.elza2.renderCommand(32);
        this.light.setGlobalPointLightCol(2, 0.17199068f, 0.14499626f, 0.11049711f);
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.andrew.start(1, "sigamituki");
        this.andrew.setScale(0.72f, 0.72f, 0.72f);
        this.pod2.setScale(0.72f, 0.72f, 0.72f);
        this.zan_2.setLightMode(1);
        this.zan_2.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zan_2.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.zan_2.light.setDirection2(1, -0.659f, 0.555f, 0.507f);
        this.zan_2.light.setColor(2, 0.47f, 0.47f, 0.47f);
        this.zan_2.light.setDirection2(2, -0.794f, 0.576f, 0.195f);
        this.zan_2.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.zan_2.light.setDirection2(3, -0.347f, -0.431f, 0.833f);
        this.zan_15.setLightMode(1);
        this.zan_15.light.setColor(0, 0.08f, 0.08f, 0.08f);
        this.zan_15.light.setColor(1, 0.72f, 0.72f, 0.72f);
        this.zan_15.light.setDirection2(1, -0.659f, 0.555f, 0.507f);
        this.zan_15.light.setColor(2, 0.47f, 0.47f, 0.47f);
        this.zan_15.light.setDirection2(2, -0.794f, 0.576f, 0.195f);
        this.zan_15.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.zan_15.light.setDirection2(3, -0.347f, -0.431f, 0.833f);
        this.light.setColor(0, 0.12f, 0.12f, 0.12f);
        this.light.setColor(1, 0.62f, 0.62f, 0.62f);
        this.light.setDirection2(1, 0.461f, 0.395f, 0.795f);
        this.light.setColor(2, 0.34f, 0.34f, 0.34f);
        this.light.setDirection2(2, 0.459f, 0.084f, 0.884f);
        this.light.setColor(3, 0.41f, 0.41f, 0.41f);
        this.light.setDirection2(3, 0.486f, 0.472f, 0.736f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam1.change();
        this.zan_3.setTranslate(5.73f, 1.31f, 47.92f);
        this.zan_3.setRotate(-70.0f, 0.0f, -262.5f);
        this.zan_20.setTranslate(37.0f, -6.15f, -50.06f);
        this.zan_20.setRotate(-120.0f, 0.0f, 0.0f);
        this.zan_7.setTranslate(18.96f, -4.4f, -51.2f);
        this.zan_7.setRotate(0.0f, 0.0f, 0.0f);
        this.zan_9.setTranslate(94.3f, -2.32f, -173.7f);
        this.zan_9.setRotate(-950.0f, -60.0f, -400.0f);
        this.zan_10.setTranslate(51.44f, -1.04f, -39.81f);
        this.zan_10.setRotate(-17.7f, 0.0f, 0.0f);
        this.cam3.setTranslate(37.92f, 12.29f, 2.38f);
        this.cam3.setRotate(11882.74f, -4863.37f, -373.1f);
        this.elza.setTranslate(37.92f, 12.29f, 2.38f);
        this.elza.setRotate(11882.74f, -4863.37f, -373.1f);
        float[] fArray7 = new float[]{1.0f, 37.92f, 12.29f, 2.38f, 86.0f, 37.92f, 12.29f, -2.79f, 171.0f, 37.92f, 12.29f, -37.94f, 256.0f, 37.92f, 12.29f, -96.82f, 341.0f, 37.92f, 12.29f, -153.45f};
        float[] fArray8 = new float[]{1.0f, 11882.74f, -4863.37f, -373.1f, 86.0f, 11882.74f, -4863.37f, -372.43f, 171.0f, 11882.74f, -4863.37f, -365.15f, 256.0f, 11882.74f, -4863.37f, -360.57f, 341.0f, 11882.74f, -4863.37f, -360.44f};
        this.cam3.transSPL(fArray7, 1);
        this.cam3.rotateSPL(fArray8, 1);
        this.elza_flag = 2;
        this.camerawork.cut75();
        System.sleep(276);
        this.gareki_syoki_iti();
        this.pod2.setVisible(true);
        this.andrew.setVisible(true);
        this.elza.setTranslate(113.76f, 6.18f, -89.73f);
        this.elza.setRotate(2.3f, 1046.45f, 3.33f);
        this.cam3.setTranslate(113.76f, 6.18f, -89.73f);
        this.cam3.setRotate(2.3f, 1046.45f, 3.33f);
        float[] fArray9 = new float[]{1.0f, 113.76f, 6.18f, -89.73f, 262.4f, 60.96f, 6.18f, -12.93f, 523.2f, 41.76f, 6.18f, 14.27f, 790.5f, 37.76f, 6.18f, 20.27f};
        float[] fArray10 = new float[]{1.0f, 2.3f, 1046.45f, 3.33f};
        this.cam3.transSPL(fArray9, 1);
        this.cam3.rotateSPL(fArray10, 1);
        this.elza_flag = 1;
        float[] fArray11 = new float[]{1.0f, 5.35f, 0.86f, 64.99f, 104.0f, 6.64f, 0.95f, 58.97f, 252.54001f, 11.32f, 0.95f, 54.21f, 378.2f, 19.39f, -0.58f, 43.01f, 503.86002f, 29.95f, -2.18f, 29.1f};
        float[] fArray12 = new float[]{1.0f, -352.5f, -376.38f, -10.53f, 104.0f, -357.33f, -385.99f, -2.5f, 252.54001f, -357.33f, -393.5f, 1.67f, 378.2f, -357.33f, -401.0f, -15.0f, 503.86002f, -357.33f, -401.0f, -15.0f};
        this.camerawork.cut76();
        this.pod2.setVisible(true);
        this.elza2.setVisible(true);
        this.p_ef01.disp(true);
        this.p_ef02.disp(true);
        this.p_ef03.disp(true);
        this.p_ef04.disp(true);
        this.p_ef05.disp(true);
        this.p_ef06.disp(true);
        this.p_ef07.disp(true);
        this.p_ef08.disp(true);
        this.p_ef09.disp(true);
        this.p_ef10.disp(true);
        this.p_ef11.disp(true);
        this.p_ef12.disp(true);
        this.elza.setVisible(false);
        this.elza2.setVisible(true);
        this.eft0.clearEffect();
        this.eft1.clearEffect();
        this.elza2.start(1, "mobj_052");
        this.elza2.setScale(102.0f, 102.0f, 102.0f);
        this.eft0.disp(false);
        this.eft1.disp(false);
        this.zan_6.start(1, "zangai_stone_spin1");
        this.zan_7.start(1, "zangai_stone_spin2");
        this.zan_8.start(1, "zangai_stone_spin1");
        this.zan_9.start(1, "zangai_stone_spin2");
        this.zan_10.start(1, "zangai_stone_spin1");
        this.zan_19.start(1, "zangai_stone_spin2");
        this.zan_20.start(1, "zangai_stone_spin1");
        this.zan_21.start(1, "zangai_stone_spin2");
        this.zan_22.start(1, "zangai_stone_spin1");
        this.zan_15.start(1, "zangai_ita_spin2");
        this.zan_3.start(1, "zangai_ita_spin5");
        this.zan_2.start(1, "zangai_ita_spin1");
        this.zan_17.start(1, "zangai_ita_spin6");
        this.zan_18.start(1, "zangai_ita_spin8");
        this.zan_16.start(1, "zangai_spin7");
        this.zan_1.start(1, "zangai_spin6");
        this.zan_4.start(1, "zangai_spin5");
        this.zan_5.start(1, "zangai_spin4");
        this.zan_11.start(1, "zangai_spin3");
        this.zan_12.start(1, "zangai_spin2");
        this.zan_13.start(1, "zangai_spin1");
        this.zan_14.start(1, "zangai_spin8");
        System.sleep(36);
        this.cam2.transSPL(fArray11, 1);
        this.cam2.rotateSPL(fArray12, 1);
        this.dummy.start(1, "sraster_on_off");
        System.sleep(366);
    }

    void pod_syoki_init() {
        this.pod_21.setTranslate(0.0f, 0.0f, -0.25f);
        this.pod_21.setRotate(0.0f, 0.0f, 0.0f);
        this.pod_22.setTranslate(0.0f, 0.0f, -0.25f);
        this.pod_22.setRotate(0.0f, 0.0f, 0.0f);
    }

    void ss1_off() {
        this.p_ef06.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef05.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef07.setScale(0.0f, 0.0f, 0.0f);
    }

    void ss1_on() {
        this.p_ef06.setScale(0.02f, 0.0f, 0.052f);
        this.p_ef05.setScale(0.015f, 0.0f, 0.047f);
        this.p_ef07.setScale(0.015f, 0.0f, 0.047f);
    }

    void ss2_off() {
        this.p_ef08.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef09.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef10.setScale(0.0f, 0.0f, 0.0f);
    }

    void ss2_on() {
        this.p_ef08.setScale(0.012f, 0.0f, 0.047f);
        this.p_ef09.setScale(0.012f, 0.0f, 0.047f);
        this.p_ef10.setScale(0.02f, 0.0f, 0.052f);
    }

    void ss3_off() {
        this.p_ef11.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef10.setScale(0.0f, 0.0f, 0.0f);
    }

    void ss3_on() {
        this.p_ef11.setScale(0.015f, 0.0f, 0.047f);
        this.p_ef10.setScale(0.02f, 0.0f, 0.052f);
    }

    void ss4_off() {
        this.p_ef06.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef07.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef12.setScale(0.0f, 0.0f, 0.0f);
    }

    void ss4_on() {
        this.p_ef06.setScale(0.015f, 0.0f, 0.047f);
        this.p_ef07.setScale(0.02f, 0.0f, 0.052f);
        this.p_ef12.setScale(0.02f, 0.0f, 0.052f);
    }

    void ss5_off() {
        this.p_ef07.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef09.setScale(0.0f, 0.0f, 0.0f);
        this.p_ef10.setScale(0.0f, 0.0f, 0.0f);
    }

    void ss5_on() {
        this.p_ef07.setScale(0.015f, 0.0f, 0.047f);
        this.p_ef09.setScale(0.02f, 0.0f, 0.052f);
        this.p_ef10.setScale(0.02f, 0.0f, 0.052f);
    }

    void wait_clr(int n) {
        System.sleep(n);
        this.msg.clear();
    }

    class robo
            extends Chr {
        Chr face;

        public robo(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.face = this.getChild(0x1000000);
            this.setShadow(0, 0);
        }

        public void motion_01() {
            this.mtn(257, 0, 0.5f, true);
        }

        public void motion_010() {
            this.mtn(266, 8, 0.67f, true);
        }

        public void motion_02() {
            this.mtn(258, 0, 0.5f, true);
        }

        public void motion_03() {
            this.mtn(259, 0, 0.92f, true);
        }

        public void motion_04() {
            this.mtn(260, 0, 0.67f, true);
        }

        public void motion_05() {
            this.mtn(261, 0, 0.68f, true);
        }

        public void motion_06() {
            this.mtn(262, 0, 0.97f, true);
        }

        public void motion_06_mae() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(262, 0, 0, 0, 0, 0.97f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_07() {
            this.mtn(263, 0, 0.52f, true);
        }

        public void motion_08() {
            this.mtn(264, 0, 0.7f, true);
        }

        public void motion_09() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(265, 0, 129, 42, 1, 0.72f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_09_2() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(257, 0, 101, 0, 0, 0.77f, true);
            this.mtn(257, 0, 101, 0, 0, -0.77f, true);
            this.mtn(257, 0, 101, 0, 0, 0.77f, true);
            this.mtn(257, 0, 101, 0, 0, -0.77f, true);
            this.mtn(257, 0, 101, 0, 0, 0.77f, true);
            this.mtn(257, 0, 101, 0, 0, -0.77f, true);
            this.mtn(257, 0, 101, 0, 0, 0.77f, true);
            this.mtn(257, 0, 101, 0, 0, -0.77f, true);
            this.mtn(257, 0, 101, 0, 0, 0.77f, true);
            this.mtn(257, 0, 101, 0, 0, -0.77f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_09_mae() {
            this.mtn(263, 75, 75, 0, 0, 1.0f, true);
        }

        public void motion_11() {
            this.mtn(267, 0, 0.82f, true);
        }

        public void motion_110() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(271, 0, 0.52f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_110_mae() {
            this.mtn(271, 0, 0, 0, 0, 0.52f, true);
        }

        public void motion_12() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(268, 0, 0.82f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_12_mae() {
            this.mtn(268, 0, 0, 0, 0, 0.82f, true);
        }

        public void motion_13() {
            this.setMotionFlags(0x1000000, true);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, false);
            this.mtn(269, 8, 0.62f, true);
        }

        public void motion_14() {
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, true);
            this.setMotionFlags(0x2000000, false);
            this.mtn(270, 8, 1.0f, true);
            this.setMotionFlags(0x1000000, false);
            this.setMotionFlags(0x800000, false);
            this.setMotionFlags(0x2000000, true);
        }

        public void motion_14_mae() {
            this.mtn(270, 0, 0, 0, 0, 1.0f, true);
        }

        void sigamituki() {
            this.mtn(299, 0, 0, 0, 0, 1.0f, true);
        }
    }

    class kiki
            extends Chr {
        kiki() {
        }

        public void elza_go_pod() {
            this.setTranslate(-28.46f, 11.7f, -72.49f);
            this.setRotate(3.5f, -262.5f, 362.75f);
        }

        public void elza_go_pod_2() {
            this.setTranslate(-29.36f, 11.7f, -72.49f);
            this.setRotate(3.5f, -405.0f, 362.75f);
        }

        void ita_light2() {
            this.setLightMode(1);
            this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            this.light.setColor(1, 0.56f, 0.56f, 0.56f);
            this.light.setDirection2(1, -0.232f, 0.359f, 0.904f);
            this.light.setColor(2, 0.59f, 0.59f, 0.59f);
            this.light.setDirection2(2, -0.691f, 0.723f, -0.015f);
            this.light.setColor(3, 0.5f, 0.5f, 0.5f);
            this.light.setDirection2(3, -0.654f, 0.731f, -0.197f);
        }

        void iwa_light() {
            this.setLightMode(1);
            this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            this.light.setColor(1, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(1, -0.522f, 0.777f, -0.352f);
            this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            this.light.setDirection2(2, -0.538f, 0.757f, -0.37f);
            this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            this.light.setDirection2(3, -0.563f, 0.752f, -0.342f);
        }

        void iwa_light2() {
            this.setLightMode(1);
            this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            this.light.setColor(1, 0.3f, 0.3f, 0.3f);
            this.light.setDirection2(1, -0.522f, 0.777f, -0.352f);
            this.light.setColor(2, 0.33f, 0.33f, 0.33f);
            this.light.setDirection2(2, -0.538f, 0.757f, -0.37f);
            this.light.setColor(3, 0.27f, 0.27f, 0.27f);
            this.light.setDirection2(3, -0.563f, 0.752f, -0.342f);
        }

        void kara_stop() {
        }

        void mobj_052() {
            this.mtn(298, 0, 22, 0, 0, 0.082f, true);
        }

        public void pod_go_elza() {
            this.setTranslate(-30.63f, 9.55f, -68.58f);
            this.setRotate(-179.37f, -347.27f, -223.0f);
        }

        public void pod_guru_guru() {
            this.setTranslate(-39.58f, 9.15f, -38.55f);
            this.setRotate(-1.0f, -195.0f, -228.0f);
        }

        public void pod_guru_guru_2() {
            this.setTranslate(-30.44f, 10.05f, -71.23f);
            this.setRotate(-167.37f, 6.73f, -193.0f);
        }

        public void pod_rotate() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.37f);
                this.setTranslate(this.px - 0.0122727f, this.py + 0.003227f, this.pz);
                System.sleep(1);
            }
        }

        public void pod_rotate2() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.22f);
                this.setTranslate(this.px - 0.0062727f, this.py + 0.0019072f, this.pz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin1() {
            while (true) {
                this.setRotate(this.rx + 0.027727f, this.ry, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin1_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx - 0.127727f, this.ry, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin2() {
            while (true) {
                this.setRotate(this.rx - 0.027727f, this.ry, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin2_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx - 0.127727f, this.ry, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin5() {
            while (true) {
                this.setRotate(this.rx, this.ry + 0.127f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin5_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx, this.ry + 0.127f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin6() {
            while (true) {
                this.setRotate(this.rx, this.ry - 0.1277f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin6_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx, this.ry - 0.4277f, this.rz - 0.042f);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin8() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.127f);
                System.sleep(1);
            }
        }

        public void zangai_ita_spin8_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx, this.ry, this.rz - 0.127f);
                System.sleep(1);
            }
        }

        public void zangai_spin1() {
            while (true) {
                this.setRotate(this.rx + 0.27727f, this.ry - 0.02f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin1_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx + 0.27727f, this.ry - 0.02f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin2() {
            while (true) {
                this.setRotate(this.rx - 0.27727f, this.ry, this.rz - 0.02f);
                System.sleep(1);
            }
        }

        public void zangai_spin2_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx - 0.27727f, this.ry, this.rz - 0.02f);
                System.sleep(1);
            }
        }

        public void zangai_spin3() {
            while (true) {
                this.setRotate(this.rx - 0.012f, this.ry + 0.2f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin3_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx - 0.012f, this.ry + 0.2f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin4() {
            while (true) {
                this.setRotate(this.rx, this.ry - 0.2f, this.rz - 0.002f);
                System.sleep(1);
            }
        }

        public void zangai_spin4_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx, this.ry - 0.2f, this.rz - 0.002f);
                System.sleep(1);
            }
        }

        public void zangai_spin5() {
            while (true) {
                this.setRotate(this.rx, this.ry + 0.2f, this.rz + 0.02f);
                System.sleep(1);
            }
        }

        public void zangai_spin5_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx, this.ry + 0.2f, this.rz + 0.02f);
                System.sleep(1);
            }
        }

        public void zangai_spin6() {
            while (true) {
                this.setRotate(this.rx - 0.02f, this.ry - 0.2f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin6_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py, this.pz);
                this.setRotate(this.rx - 0.02f, this.ry - 0.2f, this.rz);
                System.sleep(1);
            }
        }

        public void zangai_spin7() {
            while (true) {
                this.setRotate(this.rx + 0.02f, this.ry, this.rz + 0.7f);
                System.sleep(1);
            }
        }

        public void zangai_spin7_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.00127f, this.py - 7.27E-4f, this.pz);
                this.setRotate(this.rx, this.ry, this.rz - 0.27f);
                System.sleep(1);
            }
        }

        public void zangai_spin8() {
            while (true) {
                this.setRotate(this.rx - 0.02f, this.ry, this.rz - 0.2f);
                System.sleep(1);
            }
        }

        public void zangai_spin8_cut2() {
            while (true) {
                this.setTranslate(this.px - 0.0017f, this.py, this.pz);
                this.setRotate(this.rx - 0.02f, this.ry, this.rz - 0.2f);
                System.sleep(1);
            }
        }

        public void zangai_stone_spin1() {
            while (true) {
                this.setTranslate(this.px - 9.2727E-4f, this.py, this.pz);
                System.sleep(1);
            }
        }

        public void zangai_stone_spin2() {
            while (true) {
                this.setTranslate(this.px - 9.2727E-4f, this.py, this.pz);
                System.sleep(1);
            }
        }
    }

    class units
            extends Unit {
        units() {
        }

        void allen_moni_off() {
            float f = 1.4f;
            int n = 1;
            while (n <= 9) {
                SCE01043.this.alle_moni.setArgs(0, 0.0f, 0.5f, 1.6f, f);
                f -= 0.15555555f;
                ++n;
                System.sleep(1);
            }
            SCE01043.this.alle_moni.setArgs(0, 0.0f, 0.5f, 1.6f, 0.0f);
            System.sleep(1);
            SCE01043.this.alle_moni.signal(0);
        }

        void allen_serifu_001() {
            SCE01043.this.MSGW("Hello? Anyone?");
            SCE01043.this.FACE(42, SCE01043.this.allen, 272, 1.0f);
            System.sleep(12);
            SCE01043.this.MSGW("If anyone is scanning this channel,\nplease help!");
            SCE01043.this.FACE(102, SCE01043.this.allen, 272, 1.0f);
        }

        void allen_serifu_002() {
            System.sleep(3);
            SCE01043.this.FACE_SMOOTH(39, SCE01043.this.allen, 274, 1.0f);
        }

        void allen_serifu_003_0() {
            SCE01043.this.FACE_SMOOTH(36, SCE01043.this.allen, 276, 1.1f);
        }

        void allen_serifu_003_2() {
            SCE01043.this.FACE_SMOOTH(51, SCE01043.this.allen2, 286, 1.1f);
            System.sleep(12);
            SCE01043.this.FACE(75, SCE01043.this.allen2, 286, 1.1f);
        }

        void allen_serifu_003_7() {
            SCE01043.this.FACE(72, SCE01043.this.allen, 278, 1.1f);
            System.sleep(12);
            SCE01043.this.FACE_SMOOTH(21, SCE01043.this.allen, 280, 1.1f);
            System.sleep(12);
            SCE01043.this.FACE(102, SCE01043.this.allen, 280, 1.1f);
            System.sleep(12);
            SCE01043.this.FACE_SMOOTH(66, SCE01043.this.allen, 282, 1.1f);
            System.sleep(12);
            SCE01043.this.allen.look_char(SCE01043.this.shion);
            SCE01043.this.FACE(27, SCE01043.this.allen, 282, 1.1f);
        }

        public void elza_go_pod() {
            this.setTranslate(-28.46f, 11.7f, -72.49f);
            this.setRotate(3.5f, -262.5f, 362.75f);
        }

        public void elza_go_pod_2() {
            this.setTranslate(-29.36f, 11.7f, -72.49f);
            this.setRotate(3.5f, -405.0f, 362.75f);
        }

        void kara_stop() {
        }

        void moni_15_on() {
            SCE01043.this.moni_15.signal(1);
            float f = 0.0f;
            while (f < 32.0f) {
                SCE01043.this.moni_15.setArgs(2, (int) f, 0, 0, -1);
                f += 2.875f;
                System.sleep(1);
            }
            SCE01043.this.moni_15.setArgs(2, 92, 0, 0, -1);
        }

        void moni_sonotaon1() {
            SCE01043.this.moni_17.signal(1);
            float f = 0.0f;
            int n = 1;
            while (n <= 7) {
                SCE01043.this.moni_17.setArgs(0, 0.0f, 0.0f, 1.6f, f);
                f += 0.2f;
                ++n;
                System.sleep(1);
            }
            SCE01043.this.moni_17.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        }

        void moni_sonotaon2() {
            SCE01043.this.moni_18.signal(1);
            float f = 0.0f;
            int n = 1;
            while (n <= 7) {
                SCE01043.this.moni_18.setArgs(0, 0.0f, 0.0f, 1.6f, f);
                f += 0.2f;
                ++n;
                System.sleep(1);
            }
            SCE01043.this.moni_18.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        }

        void moni_sonotaon3() {
            SCE01043.this.moni_19.signal(1);
            float f = 0.0f;
            int n = 1;
            while (n <= 7) {
                SCE01043.this.moni_19.setArgs(0, 0.0f, 0.0f, 1.6f, f);
                f += 0.2f;
                ++n;
                System.sleep(1);
            }
            SCE01043.this.moni_19.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        }

        void moni_sonotaon4() {
            SCE01043.this.moni_20.signal(1);
            float f = 0.0f;
            int n = 1;
            while (n <= 7) {
                SCE01043.this.moni_20.setArgs(0, 0.0f, 0.0f, 1.6f, f);
                f += 0.2f;
                ++n;
                System.sleep(1);
            }
            SCE01043.this.moni_20.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        }

        public void pod_go_elza() {
            this.setTranslate(-30.63f, 9.55f, -68.58f);
            this.setRotate(-179.37f, -347.27f, -223.0f);
        }

        public void pod_guru_guru() {
            this.setTranslate(-39.58f, 9.15f, -38.55f);
            this.setRotate(-1.0f, -195.0f, -228.0f);
        }

        public void pod_guru_guru_2() {
            this.setTranslate(-30.44f, 10.05f, -71.23f);
            this.setRotate(-167.37f, 6.73f, -193.0f);
        }

        public void pod_rotate() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.37f);
                this.setTranslate(this.px - 0.0122727f, this.py + 0.003227f, this.pz);
                System.sleep(1);
            }
        }

        public void pod_rotate2() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.22f);
                this.setTranslate(this.px - 0.0062727f, this.py + 0.0019072f, this.pz);
                System.sleep(1);
            }
        }

        public void pod_rotate_stop() {
        }

        void shion_serifu_003_0() {
            SCE01043.this.MSGW("Don't worry...");
            SCE01043.this.FACE(33, SCE01043.this.shion, 296, 1.0f);
            System.sleep(12);
            SCE01043.this.MSGW("A rescue ship will come\nfor us eventually.");
            SCE01043.this.FACE(87, SCE01043.this.shion, 296, 1.0f);
            SCE01043.this.allen.face.mtn(273, 0, 120, 7, 9, 1.0f, false);
            SCE01043.this.allen.face.start(4, null);
            System.sleep(18);
            SCE01043.this.FACE_SMOOTH(60, SCE01043.this.shion, 292, 1.0f);
            System.sleep(12);
            SCE01043.this.FACE_SMOOTH(51, SCE01043.this.shion, 294, 1.0f);
        }

        void shion_serifu_003_9() {
            SCE01043.this.allen.face.mtn(277, 0, 120, 7, 8, 1.0f, false);
            SCE01043.this.allen.face.start(4, null);
            SCE01043.this.FACE_SMOOTH(24, SCE01043.this.shion, 290, 1.0f);
            System.sleep(12);
            SCE01043.this.FACE(66, SCE01043.this.shion, 290, 1.0f);
            System.sleep(12);
            SCE01043.this.allen.face.mtn(275, 0, 120, 7, 9, 1.0f, false);
            SCE01043.this.allen.face.start(4, null);
            SCE01043.this.FACE(37, SCE01043.this.shion, 290, 1.1f);
        }

        void shion_serifu_003__2() {
            SCE01043.this.FACE(48, SCE01043.this.shion, 284, 1.0f);
            System.sleep(12);
            SCE01043.this.FACE(15, SCE01043.this.shion, 284, 1.0f);
            System.sleep(21);
            SCE01043.this.FACE(48, SCE01043.this.shion, 284, 1.0f);
        }

        void sraster_on_off() {
            System.sleep(126);
            SCE01043.this.ss2_on();
            System.sleep(6);
            SCE01043.this.ss2_off();
            System.sleep(3);
            SCE01043.this.ss2_on();
            System.sleep(21);
            SCE01043.this.ss2_off();
            System.sleep(9);
            SCE01043.this.ss1_on();
            System.sleep(21);
            SCE01043.this.ss1_off();
            System.sleep(6);
            SCE01043.this.ss4_on();
            System.sleep(12);
            SCE01043.this.ss4_off();
            System.sleep(9);
            SCE01043.this.ss1_on();
            System.sleep(24);
            SCE01043.this.ss1_off();
            System.sleep(12);
            SCE01043.this.ss3_on();
            System.sleep(6);
            SCE01043.this.ss3_off();
            System.sleep(3);
            SCE01043.this.ss3_on();
            System.sleep(51);
            SCE01043.this.ss3_off();
            System.sleep(24);
            SCE01043.this.ss5_on();
            System.sleep(15);
            SCE01043.this.ss5_off();
            System.sleep(6);
            SCE01043.this.ss3_on();
            System.sleep(81);
            SCE01043.this.ss3_off();
            System.sleep(6);
            SCE01043.this.ss5_on();
            System.sleep(36);
            SCE01043.this.ss5_off();
            System.sleep(3);
            SCE01043.this.ss3_on();
            System.sleep(126);
            SCE01043.this.ss3_off();
        }

        public void zangai_stone_spin1() {
            while (true) {
                this.setTranslate(this.px - 9.2727E-4f, this.py, this.pz);
                System.sleep(1);
            }
        }

        public void zangai_stone_spin2() {
            while (true) {
                this.setTranslate(this.px - 9.2727E-4f, this.py, this.pz);
                System.sleep(1);
            }
        }
    }

    class mapunits
            extends MAPUnit {
        mapunits() {
        }
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        public void cut001() {
            SCE01043.this.cam1.setFov(29.54f);
            float[] fArray = new float[]{1.0f, -0.3532f, 1.3727778f, -1.9938f, 554.0f, -0.3532f, 1.1227778f, -1.993809f};
            float[] fArray2 = new float[4];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.782f;
            fArray2[2] = -168.594f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 0, 2, 554);
            SCE01043.this.cam1.rotateSPL(fArray3, 1, 2, 1);
        }

        public void cut002() {
            SCE01043.this.cam1.setFov(33.35f);
            SCE01043.this.cam0.setFov(33.35f);
            float[] fArray = new float[]{1.0f, -1.01f, 1.18f, -1.0f, 177.0f, -1.0f, 1.18f, -0.96f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -9.65f;
            fArray2[2] = -86.299995f;
            fArray2[4] = 177.0f;
            fArray2[5] = -9.67f;
            fArray2[6] = -87.9f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut003_0() {
            SCE01043.this.cam1.setFov(27.11f);
            SCE01043.this.cam0.setFov(27.11f);
            float[] fArray = new float[]{1.0f, 0.54f, 1.28f, 1.81f, 328.0f, 0.56f, 1.32f, 1.88f};
            float[] fArray2 = new float[]{1.0f, -8.3f, 20.97f, -0.0f, 328.0f, -8.76f, 20.97f, -0.0f};
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut003_00() {
            SCE01043.this.cam1.setFov(27.11f);
            SCE01043.this.cam0.setFov(27.11f);
            float[] fArray = new float[]{1.0f, 0.56f, 1.32f, 1.88f};
            float[] fArray2 = new float[]{1.0f, -8.76f, 20.97f, -0.0f};
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray2, 1);
        }

        public void cut003_1() {
            SCE01043.this.cam1.setFov(29.97f);
            SCE01043.this.cam0.setFov(29.97f);
            float[] fArray = new float[]{1.0f, -0.56f, 1.0f, 0.53f, 106.0f, -0.56f, 1.0f, 0.53f, 211.0f, -0.56f, 1.0f, 0.53f, 316.0f, -0.56f, 1.0f, 0.53f};
            float[] fArray2 = new float[16];
            fArray2[0] = 1.0f;
            fArray2[1] = -7.57f;
            fArray2[2] = -156.07f;
            fArray2[4] = 106.0f;
            fArray2[5] = -5.93f;
            fArray2[6] = -156.07f;
            fArray2[8] = 211.0f;
            fArray2[9] = -3.21f;
            fArray2[10] = -156.07f;
            fArray2[12] = 316.0f;
            fArray2[13] = -1.86f;
            fArray2[14] = -156.07f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut003_2() {
            SCE01043.this.cam1.setFov(24.27f);
            float[] fArray = new float[]{1.0f, 0.8f, 1.09f, -0.04f, 186.0f, 0.75f, 1.09f, -0.1f};
            SCE01043.this.cam1.transSPL(fArray, 0, 2, 240);
            SCE01043.this.cam1.setRotate(-0.248f, 38.898f, 0.0f);
        }

        public void cut003_7() {
            SCE01043.this.cam1.setFov(27.62f);
            SCE01043.this.cam0.setFov(27.62f);
            float[] fArray = new float[]{1.0f, 0.19f, 1.417f, 1.33f, 328.0f, 0.24f, 1.417f, 1.33f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -12.77f;
            fArray2[2] = 11.74f;
            fArray2[4] = 328.0f;
            fArray2[5] = -12.67f;
            fArray2[6] = 11.91f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
            float[] fArray4 = new float[]{1.0f, 27.62f, 328.0f, 27.6f};
            SCE01043.this.cam1.fovSPL(fArray4, 1);
        }

        public void cut003_8() {
            SCE01043.this.cam1.setFov(26.24f);
            SCE01043.this.cam0.setFov(26.24f);
            float[] fArray = new float[]{1.0f, 0.41f, 1.21f, -2.05f, 328.0f, 0.41f, 1.21f, -2.05f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -2.36f;
            fArray2[2] = 166.41f;
            fArray2[4] = 328.0f;
            fArray2[5] = -2.36f;
            fArray2[6] = 167.42f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut003_9() {
            SCE01043.this.cam1.setFov(27.71f);
            SCE01043.this.cam0.setFov(27.71f);
            float[] fArray = new float[]{1.0f, 0.78f, 0.92f, 2.33f, 358.0f, 0.7f, 0.92f, 2.36f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 6.98f;
            fArray2[2] = 20.75f;
            fArray2[4] = 358.0f;
            fArray2[5] = 6.98f;
            fArray2[6] = 20.75f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut75() {
            SCE01043.this.cam1.setFov(37.27f);
            SCE01043.this.cam0.setFov(37.27f);
            float[] fArray = new float[]{1.0f, 5.17f, -0.98f, 64.06f, 778.0f, 4.32f, -0.98f, 63.07f};
            float[] fArray2 = new float[20];
            fArray2[0] = 1.0f;
            fArray2[1] = 16.27f;
            fArray2[2] = -49.13f;
            fArray2[4] = 66.0f;
            fArray2[5] = 15.81f;
            fArray2[6] = -47.37f;
            fArray2[8] = 131.0f;
            fArray2[9] = 11.53f;
            fArray2[10] = -34.68f;
            fArray2[12] = 196.0f;
            fArray2[13] = 6.94f;
            fArray2[14] = -20.02f;
            fArray2[16] = 261.0f;
            fArray2[17] = 5.29f;
            fArray2[18] = -15.52f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut76() {
            SCE01043.this.cam1.setFov(37.77f);
            SCE01043.this.cam0.setFov(37.77f);
            float[] fArray = new float[]{1.0f, 6.27f, 0.21f, 64.43f, 774.0f, 6.93f, 0.21f, 65.24f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 2.29f;
            fArray2[2] = -24.82f;
            fArray2[4] = 774.0f;
            fArray2[5] = 2.29f;
            fArray2[6] = -24.82f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 1);
            SCE01043.this.cam1.rotateSPL(fArray3, 1);
        }

        public void cut77() {
            SCE01043.this.cam1.setFov(37.7727f);
            SCE01043.this.cam1.setRotate(-0.48f, -10.81f, 0.0f);
            float[] fArray = new float[]{1.0f, 7.6f, 0.17f, 65.41f, 304.0f, 7.86f, 0.17f, 65.47f};
            SCE01043.this.cam1.transSPL(fArray, 0);
        }

        public void cut78() {
            SCE01043.this.cam1.setFov(33.241f);
            SCE01043.this.cam1.setRotate(-3.08f, -11.6f, 0.0f);
            float[] fArray = new float[]{1.0f, 5.56f, 0.25f, 74.36f, 404.0f, 5.66f, 0.25f, 74.38f};
            SCE01043.this.cam1.transSPL(fArray, 0);
        }

        public void cut80() {
            SCE01043.this.cam1.setFov(32.57f);
            float[] fArray = new float[]{1.0f, -29.52f, 11.46f, -72.85f, 27.0f, -29.52f, 11.46f, -72.85f, 94.0f, -29.54f, 11.49f, -72.86f};
            float[] fArray2 = new float[12];
            fArray2[0] = 1.0f;
            fArray2[1] = 19.13f;
            fArray2[2] = 601.04f;
            fArray2[4] = 27.0f;
            fArray2[5] = 19.13f;
            fArray2[6] = 601.04f;
            fArray2[8] = 94.0f;
            fArray2[9] = 14.11f;
            fArray2[10] = 603.99f;
            float[] fArray3 = fArray2;
            SCE01043.this.cam1.transSPL(fArray, 0);
            SCE01043.this.cam1.rotateSPL(fArray3, 0);
        }

        public void cut81() {
            SCE01043.this.cam1.setFov(32.891f);
            SCE01043.this.cam1.setTranslate(-29.72f, 11.33f, -72.87f);
            SCE01043.this.cam1.setRotate(8.71f, 520.24f, 0.0f);
        }

        public void cut_black() {
            SCE01043.this.cam1.setFov(40.891f);
            SCE01043.this.cam1.setTranslate(89.84f, 359.33f, 0.07f);
            SCE01043.this.cam1.setRotate(0.0f, 1000.24f, 2.77f);
            SCE01043.this.msg.clear();
        }
    }
}

