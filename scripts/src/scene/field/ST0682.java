import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS04B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0682
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS04B_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
    Player player;
    Camera cam1;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    int shion = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    int button_flg = 0;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect fade;
    Effect Yu01;
    Effect Yu02;
    Effect Yu03;
    Effect Yu04;
    Effect Yu05;
    Effect Yu06;
    Effect Yu07;
    Effect Yu08;
    Effect Yu09;
    Effect Yu10;
    Effect Yu11;
    Effect Yu12;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] DOOR = new String[]{"Press the bulkhead switch?\n", "/[waitkey(64)]/[close()]"};
    String[] CHAOS_01 = new String[]{"/[label(chaos)]", "There is an electromagnetic floor beyond this point.", "/[waitkey(64)]/[close()]"};

    ST0682() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.256143f, 1.8999887f, 13.655906f);
        this.camEV.setRotate(-8.917236f, 1.6799955f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.4431095f, 0.8799138f, 12.33396f);
        this.camEV.setRotate(1.1558828f, -132.6585f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.247754f, 1.2639976f, 26.324427f);
        this.camEV.setRotate(-31.644232f, -45.93935f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            default:
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        Runtime.disable(524288);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(65, false);
        Stage.setVisible(66, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -9.4f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, 13.5f, -1.0f, -25.0f, 0.0f);
        this.teiten2.SetBgm(196619);
        this.teiten3 = new Uwamono(28690, 7.0f, -1.0f, 13.0f, 0.0f);
        this.teiten3.SetBgm(196619);
        this.teiten4 = new Uwamono(28690, 9.0f, -1.0f, 13.0f, 0.0f);
        this.teiten4.SetBgm(196619);
        this.teiten5 = new Uwamono(28690, 7.5f, 0.0f, -9.5f, 0.0f);
        this.teiten5.SetBgm(196620);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.2f, 1.0f, 0.2f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 1.0f, 0.3f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -1.0f, -0.3f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -7.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 6.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 6.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFLockX(10, -8.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFLockX(13, 18.0f);
        this.cam0.setCFAngle(14, 0.0f, -10.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.getFlags(3040, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy1 = new Enepc();
                this.enemy1.init(16642, 3, 5.0f, 0.0f, 9.0f, 180.0f);
                this.enemy1.id = 1;
                this.enemy1.setGroup(0, 0, 0, 0);
                float[] fArray3 = new float[8];
                fArray3[0] = 5.0f;
                fArray3[2] = 9.0f;
                fArray3[3] = 1.0f;
                fArray3[4] = 5.0f;
                fArray3[6] = 10.0f;
                fArray3[7] = -1.0f;
                fArray2 = fArray3;
                this.enemy1.setParams(1, 5, 1, 3, fArray2);
                this.enemy1.enableDTKFlag(262144);
                this.enemy1.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy1 = new Enepc();
                this.enemy1.init(16642, 3, 5.0f, 0.0f, 9.0f, 180.0f);
                this.enemy1.id = 1;
                this.enemy1.setGroup(3, 3, 3, 3);
                this.enemy1.setParams(1, 5, 1, 3);
                this.enemy1.enableDTKFlag(262144);
                this.enemy1.disableDTKFlag(2);
                this.enemy1.getTranslate();
            }
        }
        if (Runtime.getFlags(3041, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy2 = new Enepc();
                this.enemy2.init(16642, 3, 7.0f, 0.0f, 9.0f, 180.0f);
                this.enemy2.id = 2;
                this.enemy2.setGroup(0, 0, 0, 0);
                float[] fArray4 = new float[8];
                fArray4[0] = 7.0f;
                fArray4[2] = 9.0f;
                fArray4[3] = 1.0f;
                fArray4[4] = 7.0f;
                fArray4[6] = 10.0f;
                fArray4[7] = -1.0f;
                fArray2 = fArray4;
                this.enemy2.setParams(1, 6, 2, 3, fArray2);
                this.enemy2.enableDTKFlag(262144);
                this.enemy2.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy2 = new Enepc();
                this.enemy2.init(16642, 3, 7.0f, 0.0f, 9.0f, 180.0f);
                this.enemy2.id = 2;
                this.enemy2.setGroup(3, 3, 3, 3);
                this.enemy2.setParams(1, 6, 2, 3);
                this.enemy2.enableDTKFlag(262144);
                this.enemy2.disableDTKFlag(2);
                this.enemy2.getTranslate();
            }
        }
        if (Runtime.getFlags(3042, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy3 = new Enepc();
                this.enemy3.init(16642, 3, 12.0f, 0.0f, 16.0f, 180.0f);
                this.enemy3.id = 3;
                this.enemy3.setGroup(1, 1, 2, 2);
                float[] fArray5 = new float[8];
                fArray5[0] = 12.0f;
                fArray5[2] = 16.0f;
                fArray5[3] = 1.0f;
                fArray5[4] = 12.0f;
                fArray5[6] = 14.0f;
                fArray5[7] = -1.0f;
                fArray2 = fArray5;
                this.enemy3.setParams(1, 7, 3, 3, fArray2);
                float[] fArray6 = new float[30];
                fArray6[0] = 12.0f;
                fArray6[2] = 16.0f;
                fArray6[3] = 12.0f;
                fArray6[5] = 14.0f;
                fArray6[6] = 12.0f;
                fArray6[8] = 12.0f;
                fArray6[9] = 12.0f;
                fArray6[11] = 10.0f;
                fArray6[12] = 14.0f;
                fArray6[14] = 10.0f;
                fArray6[15] = 16.0f;
                fArray6[17] = 10.0f;
                fArray6[18] = 18.0f;
                fArray6[20] = 10.0f;
                fArray6[21] = 18.0f;
                fArray6[23] = 12.0f;
                fArray6[24] = 18.0f;
                fArray6[26] = 14.0f;
                fArray6[27] = 18.0f;
                fArray6[29] = 16.0f;
                fArray = fArray6;
                this.enemy3.setParams(fArray);
                this.enemy3.enableDTKFlag(262144);
                this.enemy3.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy3 = new Enepc();
                this.enemy3.init(16642, 3, 12.0f, 0.0f, 16.0f, 180.0f);
                this.enemy3.id = 3;
                this.enemy3.setGroup(4, 4, 5, 5);
                this.enemy3.setParams(1, 7, 3, 3);
                this.enemy3.enableDTKFlag(262144);
                this.enemy3.disableDTKFlag(2);
                this.enemy3.getTranslate();
            }
        }
        if (Runtime.getFlags(3043, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy4 = new Enepc();
                this.enemy4.init(16642, 3, 19.0f, -3.0f, 22.0f, 180.0f);
                this.enemy4.id = 4;
                this.enemy4.setGroup(1, 1, 2, 2);
                fArray2 = new float[]{19.0f, -3.0f, 22.0f, 1.0f, 17.0f, -3.0f, 22.0f, -1.0f};
                this.enemy4.setParams(2, 8, 4, 3, fArray2);
                fArray = new float[]{19.0f, -3.0f, 22.0f, 17.0f, -3.0f, 22.0f, 17.0f, -3.0f, 24.0f, 15.0f, -3.0f, 24.0f, 13.0f, -3.0f, 24.0f, 11.0f, -3.0f, 24.0f, 9.0f, -3.0f, 24.0f, 7.0f, -3.0f, 24.0f};
                this.enemy4.setParams(fArray);
                this.enemy4.enableDTKFlag(262144);
                this.enemy4.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy4 = new Enepc();
                this.enemy4.init(16642, 3, 19.0f, -3.0f, 22.0f, 180.0f);
                this.enemy4.id = 4;
                this.enemy4.setGroup(4, 4, 5, 5);
                this.enemy4.setParams(2, 8, 4, 3);
                this.enemy4.enableDTKFlag(262144);
                this.enemy4.disableDTKFlag(2);
                this.enemy4.getTranslate();
            }
        }
        if (Runtime.getFlags(3025, 1) == 0) {
            this.npc1 = new NPC_NORMAL(3, 11, 0, 4, 5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 9);
            this.npc1.start(1, "Stop");
        } else if (Runtime.getFlags(3025, 1) == 1) {
            this.npc1 = new NPC_NORMAL(3, 11, 0, 4, 5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 9);
            this.npc1.start(1, "Stop2");
        }
        this.Yu01 = new Effect(1560, 2);
        this.Yu02 = new Effect(1560, 3);
        this.Yu03 = new Effect(1560, 4);
        this.Yu04 = new Effect(1560, 5);
        this.Yu05 = new Effect(1560, 6);
        this.Yu06 = new Effect(1560, 7);
        this.Yu07 = new Effect(1560, 8);
        this.Yu08 = new Effect(1560, 9);
        this.Yu09 = new Effect(1560, 10);
        this.Yu10 = new Effect(1560, 11);
        this.Yu11 = new Effect(1560, 12);
        this.Yu12 = new Effect(1560, 13);
        if (Runtime.getFlags(3025, 1) == 0) {
            this.Yu01.disp(false);
            this.Yu02.disp(false);
            this.Yu03.disp(false);
            this.Yu04.disp(false);
            this.Yu05.disp(false);
            this.Yu06.disp(false);
            this.Yu07.disp(false);
            this.Yu08.disp(false);
            this.Yu09.disp(false);
            this.Yu10.disp(false);
            this.Yu11.disp(false);
            this.Yu12.disp(false);
        } else {
            this.Yu01.disp(true);
            this.Yu02.disp(true);
            this.Yu03.disp(true);
            this.Yu04.disp(true);
            this.Yu05.disp(true);
            this.Yu06.disp(true);
            this.Yu07.disp(true);
            this.Yu08.disp(true);
            this.Yu09.disp(true);
            this.Yu10.disp(true);
            this.Yu11.disp(true);
            this.Yu12.disp(true);
        }
        if (Runtime.getFlags(3025, 1) == 0) {
            if (Runtime.getFlags(3040, 1) == 0) {
                this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF01.setTranslate(this.enemy1.px, this.enemy1.py, this.enemy1.pz);
                this.EF01.disp(true);
            }
            if (Runtime.getFlags(3041, 1) == 0) {
                this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF02.setTranslate(this.enemy2.px, this.enemy2.py, this.enemy2.pz);
                this.EF02.disp(true);
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF03.setTranslate(this.enemy3.px, this.enemy3.py, this.enemy3.pz);
                this.EF03.disp(true);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                this.EF04 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF04.setTranslate(this.enemy4.px, this.enemy4.py, this.enemy4.pz);
                this.EF04.disp(true);
            }
        } else if (Runtime.getFlags(3025, 1) == 1) {
            if (Runtime.getFlags(3040, 1) == 0) {
                this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF01.disp(false);
            }
            if (Runtime.getFlags(3041, 1) == 0) {
                this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF02.disp(false);
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF03.disp(false);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                this.EF04 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF04.disp(false);
            }
        }
        new Uwamono(51, 21);
        new Uwamono(52, 21);
        new Uwamono(53, 4);
        new Uwamono(54, 0);
        new Uwamono(55, 0);
        new Uwamono(56, 0);
        new Uwamono(58, 4);
        new Uwamono(59, 0);
        new Uwamono(60, 4);
        new Uwamono(61, 0);
        new Uwamono(62, 0);
        new Uwamono(63, 0);
        new Uwamono(64, 0);
        new Uwamono(57, 0);
        new Uwamono(49, 20);
        this.doorA = new Uwamono(39, 40, '\u0001');
        new Uwamono(40, 40, '\u0001', this.doorA);
        this.doorB = new Uwamono(41, 40, '\u0001');
        new Uwamono(42, 40, '\u0001', this.doorB);
        this.doorC = new Uwamono(43, 40, '\u0001');
        new Uwamono(44, 40, '\u0001', this.doorC);
        this.doorD = new Uwamono(45, 40, '\u0001');
        new Uwamono(46, 40, '\u0001', this.doorD);
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorA.SetDoorSpd(24);
        this.doorB.SetDoorSpd(24);
        this.doorC.SetDoorSpd(24);
        this.doorD.SetDoorSpd(24);
        if (Runtime.getFlags(3021, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
        } else if (Runtime.getFlags(3021, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
        }
        this.doorE = new Uwamono(37, 40, '\u0001');
        this.doorE.SetDoorType('\u0001');
        this.doorE.SetDoorScope(2.7f);
        this.doorF = new Uwamono(38, 40, '\u0001');
        this.doorF.SetDoorType('\u0002');
        this.doorG = new Uwamono(47, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.eve00 = new Effect(1415, 0);
        this.eve00.disp(true);
        this.eve02 = new Effect(1413, 1);
        this.eve02.disp(true);
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    class Player
            extends Chr {
        Player() {
        }

        void init() {
            this.setPlayer();
            this.setShadow(4, 16);
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }

        void Stop() {
            Runtime.setPlayerControl(false);
            ST0682.this.cam0.setMode(-1);
            if (Runtime.getFlags(3040, 1) == 0 || Runtime.getFlags(3041, 1) == 0) {
                ST0682.this.EV_Camera00();
                Sound.effectPlay(196639, 127, 64);
                if (Runtime.getFlags(3040, 1) == 0) {
                    System.sleep(30);
                    ST0682.this.EF01.disp(false);
                    ST0682.this.enemy1.kickEnepc(7, 5);
                    System.sleep(30);
                }
                if (Runtime.getFlags(3041, 1) == 0) {
                    System.sleep(30);
                    ST0682.this.EF02.disp(false);
                    Sound.effectStop(196639);
                    ST0682.this.enemy2.kickEnepc(7, 5);
                    System.sleep(30);
                }
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                ST0682.this.EV_Camera01();
                Sound.effectPlay(196639, 127, 64);
                System.sleep(30);
                ST0682.this.EF03.disp(false);
                Sound.effectStop(196639);
                ST0682.this.enemy3.kickEnepc(7, 5);
                System.sleep(30);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                ST0682.this.EV_Camera02();
                Sound.effectPlay(196639, 127, 64);
                System.sleep(30);
                ST0682.this.EF04.disp(false);
                Sound.effectStop(196639);
                ST0682.this.enemy4.kickEnepc(7, 5);
                System.sleep(30);
            }
            ST0682.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            ST0682.this.cam0.setMode(0);
            Runtime.jumpCF(66256, 4);
        }

        void Stop2() {
            Runtime.setPlayerControl(false);
            ST0682.this.cam0.setMode(-1);
            if (Runtime.getFlags(3040, 1) == 0 || Runtime.getFlags(3041, 1) == 0) {
                ST0682.this.EV_Camera00();
                if (Runtime.getFlags(3040, 1) == 0) {
                    ST0682.this.EF01.setTranslate(ST0682.this.enemy1.px, ST0682.this.enemy1.py, ST0682.this.enemy1.pz);
                    System.sleep(30);
                    ST0682.this.EF01.disp(true);
                    Sound.effectPlay(196639);
                    ST0682.this.enemy1.kickEnepc(7, 0);
                    System.sleep(30);
                }
                if (Runtime.getFlags(3041, 1) == 0) {
                    ST0682.this.EF02.setTranslate(ST0682.this.enemy2.px, ST0682.this.enemy2.py, ST0682.this.enemy2.pz);
                    System.sleep(30);
                    ST0682.this.EF02.disp(true);
                    Sound.effectPlay(196639);
                    ST0682.this.enemy2.kickEnepc(7, 0);
                    System.sleep(30);
                    Sound.effectStop(196639);
                }
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                ST0682.this.EV_Camera01();
                ST0682.this.EF03.setTranslate(ST0682.this.enemy3.px, ST0682.this.enemy3.py, ST0682.this.enemy3.pz);
                System.sleep(30);
                ST0682.this.EF03.disp(true);
                Sound.effectPlay(196639);
                ST0682.this.enemy3.kickEnepc(7, 0);
                System.sleep(30);
                Sound.effectStop(196639);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                ST0682.this.EV_Camera02();
                ST0682.this.EF04.setTranslate(ST0682.this.enemy4.px, ST0682.this.enemy4.py, ST0682.this.enemy4.pz);
                System.sleep(30);
                ST0682.this.EF04.disp(true);
                Sound.effectPlay(196639);
                ST0682.this.enemy4.kickEnepc(7, 0);
                System.sleep(30);
                Sound.effectStop(196639);
            }
            ST0682.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            ST0682.this.cam0.setMode(0);
            Runtime.jumpCF(66256, 4);
        }
    }
}

