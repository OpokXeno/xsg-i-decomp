import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
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
import xeno.vm.Thread;

class ST0681
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
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
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
    Effect fade;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
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
    Unit monitor1;
    Unit kidou;
    Thread SE01;
    Thread SE02;
    Thread SE03;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] DOOR = new String[]{"The anti-intruder program has been activated. All crew in affected sectors, please evacuate immediately.", "/[waitkey(1)]/[clear()]", "Repeat. Crew in affected sectors, please evacuate immediately.", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "If we use the electromagnetic floor, we might be able to slow down the enemies a bit!", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "Here's our chance. We have to get to the hangar right away...", "/[waitkey(64)]/[close()]"};
    String[] KOS_01 = new String[]{"/[label(KOS-MOS)]", "Our priority is to get rid of the mother ship that raided the hangar. Considering the current situation, if we confront the Auto-Techs here, we will waste 2143.15 seconds.", "/[waitkey(64)]/[close()]"};
    String[] KOS_02 = new String[]{"/[label(KOS-MOS)]", "Successfully prevented enemy Auto-Techs from invading ship. Shifting to primary objective -- eliminating the mother ship.", "/[waitkey(64)]/[close()]"};
    String[] CHA_01 = new String[]{"/[label(chaos)]", "So this is what the program is for. Good going, Captain.", "/[waitkey(64)]/[close()]"};
    String[] CHA_02 = new String[]{"/[label(chaos)]", "The enemy is in the hangar...", "We have to do something quickly!!", "/[waitkey(64)]/[close()]"};

    ST0681() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.0f, 1.73f, -10.0f);
        this.camEV.setRotate(-10.0f, 180.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.407f, 1.424f, -12.794f);
        this.camEV.setRotate(-12.002f, -168.218f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.3023973f, 2.8599262f, 7.101986f);
        this.camEV.setRotate(-21.857248f, -132.89716f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.5577016f, 1.451994f, 2.1152508f);
        this.camEV.setRotate(-8.398168f, -193.47694f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.219141f, 1.4519964f, -11.555018f);
        this.camEV.setRotate(-10.615789f, -152.77824f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.256143f, 1.8999887f, 13.655906f);
        this.camEV.setRotate(-8.917236f, 1.6799955f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.4431095f, 0.8799138f, 12.33396f);
        this.camEV.setRotate(1.1558828f, -132.6585f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.247754f, 1.2639976f, 26.324427f);
        this.camEV.setRotate(-31.644232f, -45.93935f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.7601614f, 1.6479667f, -7.120347f);
        this.camEV.setRotate(-12.596998f, 20.940952f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera08() {
        float[] fArray = new float[]{1.0f, 1.799f, 5.931f, -6.561f, 90.0f, 1.424f, 5.931f, -9.062f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -36.778f;
        fArray2[2] = -81.459f;
        fArray2[4] = 90.0f;
        fArray2[5] = -36.778f;
        fArray2[6] = -81.459f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorD.DoorClose();
        System.sleep(n);
        this.doorC.DoorClose();
        System.sleep(n);
        this.doorB.DoorClose();
        System.sleep(n);
        this.doorA.DoorClose();
    }

    void Kakuheki_Open() {
        int n = 10;
        this.doorA.DoorOpen();
        Sound.effectPlay(196713);
        System.sleep(n);
        this.doorB.DoorOpen();
        Sound.effectPlay(196713);
        System.sleep(n);
        this.doorC.DoorOpen();
        Sound.effectPlay(196713);
        System.sleep(n);
        this.doorD.DoorOpen();
        Sound.effectPlay(196713);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(3023, 1) == 0) {
                    Runtime.setFlags(3023, 1, 1);
                    Runtime.jumpCF(66236, 4);
                    break;
                }
                Runtime.jumpCF(66226, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(66206, 9);
                break;
            }
            case 2: {
                Runtime.jumpCF(66256, 2);
                break;
            }
        }
    }

    void init() {
        this.SE01 = Thread.create(this, "sound01");
        this.SE02 = Thread.create(this, "sound02");
        this.SE03 = Thread.create(this, "sound03");
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
        this.enemy1 = new Enepc();
        this.enemy1.init(16642, 3, 18.0f, 0.0f, 10.0f, 270.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray = new float[8];
        fArray[0] = 5.0f;
        fArray[2] = 9.0f;
        fArray[3] = 1.0f;
        fArray[4] = 5.0f;
        fArray[6] = 10.0f;
        fArray[7] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        this.enemy1.enableDTKFlag(262144);
        this.enemy2 = new Enepc();
        this.enemy2.init(16642, 3, 10.0f, 0.0f, 16.0f, 180.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[8];
        fArray3[0] = 7.0f;
        fArray3[2] = 9.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = 7.0f;
        fArray3[6] = 10.0f;
        fArray3[7] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(1, 1, 2, 3, fArray4);
        this.enemy2.enableDTKFlag(262144);
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
        float[] fArray6 = fArray5;
        this.enemy3.setParams(1, 7, 3, 3, fArray6);
        this.enemy3.enableDTKFlag(262144);
        this.enemy4 = new Enepc();
        this.enemy4.init(16642, 3, 19.0f, -3.0f, 22.0f, 180.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(1, 1, 2, 2);
        float[] fArray7 = new float[]{19.0f, -3.0f, 22.0f, 1.0f, 17.0f, -3.0f, 22.0f, -1.0f};
        this.enemy4.setParams(2, 8, 4, 3, fArray7);
        this.enemy4.enableDTKFlag(262144);
        if (Runtime.getLeader() == 1) {
            this.npc1 = new NPC_NORMAL(1, 11, 0, 4, 5, 4.0f, 0.0f, -9.0f, 90.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 9);
            this.npc1.setInvalidID(1);
            System.println("＊＊＊＊＊＊シオン登場＊＊＊＊＊＊＊＊＊");
        } else if (Runtime.getLeader() == 2) {
            this.npc1 = new NPC_NORMAL(2, 11, 0, 4, 7, 4.0f, 0.0f, -9.0f, 90.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 9);
            this.npc1.setInvalidID(1);
            System.println("＊＊＊＊＊＊コスモス登場＊＊＊＊＊＊＊＊＊");
        } else {
            this.npc1 = new NPC_NORMAL(3, 11, 0, 4, 14, 4.0f, 0.0f, -9.0f, 90.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 9);
            this.npc1.setInvalidID(1);
            System.println("＊＊＊＊＊＊ケイオス登場＊＊＊＊＊＊＊＊＊");
        }
        this.monitor1 = new Object();
        this.monitor1.init(24613, 7.742f, 2.457f, -9.5f, 270.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.138f, 1.584f);
        this.monitor1.setArgs(1, 10017, 0, 128, 112);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
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
        this.doorA.DoorOpen();
        this.doorB.DoorOpen();
        this.doorC.DoorOpen();
        this.doorD.DoorOpen();
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
        this.Yu01 = new Effect(1560, 2);
        this.Yu01.disp(false);
        this.Yu02 = new Effect(1560, 3);
        this.Yu02.disp(false);
        this.Yu03 = new Effect(1560, 4);
        this.Yu03.disp(false);
        this.Yu04 = new Effect(1560, 5);
        this.Yu04.disp(false);
        this.Yu05 = new Effect(1560, 6);
        this.Yu05.disp(false);
        this.Yu06 = new Effect(1560, 7);
        this.Yu06.disp(false);
        this.Yu07 = new Effect(1560, 8);
        this.Yu07.disp(false);
        this.Yu08 = new Effect(1560, 9);
        this.Yu08.disp(false);
        this.Yu09 = new Effect(1560, 10);
        this.Yu09.disp(false);
        this.Yu10 = new Effect(1560, 11);
        this.Yu10.disp(false);
        this.Yu11 = new Effect(1560, 12);
        this.Yu11.disp(false);
        this.Yu12 = new Effect(1560, 13);
        this.Yu12.disp(false);
        this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF01.disp(false);
        this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF02.disp(false);
        this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF03.disp(false);
        this.EF04 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF04.disp(false);
        this.kidou = new Mapunits();
        this.kidou.mapUnit(11);
        this.kidou.start(4, null);
        this.kidou.start(1, "Evt");
    }

    void keikoku() {
        int n = 0;
        while (true) {
            if (n >= 0 && n < 30) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.035633333f * (float) n, 0.033333335f * (float) n, 0.0528f * (float) n);
            }
            if (n == 60) break;
            ++n;
            System.sleep(1);
        }
    }

    void sound01() {
        boolean bl = false;
        while (true) {
            Sound.effectPlay(458754);
            Sound.effectPlay(458759);
            System.println("＊＊＊＊＊＊音が鳴った！！＊＊＊＊＊＊＊");
            System.sleep(10);
        }
    }

    void sound02() {
        boolean bl = false;
        while (true) {
            Sound.effectPlay(458754);
            System.sleep(10);
            System.println("＊＊＊＊＊＊音２が鳴った！！＊＊＊＊＊＊＊");
        }
    }

    void sound03() {
        boolean bl = false;
        while (true) {
            Sound.effectPlay(458758);
            System.sleep(10);
            System.println("＊＊＊＊＊＊音３が鳴った！！＊＊＊＊＊＊＊");
        }
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
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Evt() {
            Runtime.setPlayerControl(false);
            ST0681.this.cam0.setMode(-1);
            ST0681.this.EV_Camera001();
            ST0681.this.npc1.kickEnepc(4, 3);
            ST0681.this.npc1.kickEnepc(1, 3);
            System.sleep(2);
            ST0681.this.npc1.move(8, 5.15f, -8.416f, true);
            System.sleep(8);
            ST0681.this.npc1.move(15, 5.664f, -6.767f, true);
            System.sleep(15);
            ST0681.this.npc1.move(60, 6.0f, 3.0f, true);
            System.sleep(70);
            ST0681.this.npc1.kickEnepc(4, 1);
            ST0681.this.npc1.kickEnepc(1, 10);
            ST0681.this.EV_Camera01();
            ST0681.this.SE01.start();
            ST0681.this.enemy1.kickEnepc(4, 1);
            ST0681.this.enemy2.kickEnepc(4, 1);
            ST0681.this.enemy3.kickEnepc(4, 1);
            ST0681.this.enemy4.kickEnepc(4, 1);
            ST0681.this.enemy1.kickEnepc(1, 1);
            ST0681.this.enemy2.kickEnepc(1, 1);
            ST0681.this.enemy3.kickEnepc(1, 27);
            ST0681.this.enemy4.kickEnepc(1, 27);
            ST0681.this.enemy1.moveEnepc(15, 7.0f, 10.0f, 90);
            ST0681.this.enemy2.moveEnepc(15, 12.0f, 10.0f, 90);
            System.sleep(90);
            ST0681.this.enemy1.moveEnepc(17, 180.0f, -20.0f, 20);
            ST0681.this.enemy2.moveEnepc(17, 270.0f, 20.0f, 20);
            System.sleep(20);
            ST0681.this.enemy1.moveEnepc(15, 7.0f, 8.0f, 20);
            ST0681.this.enemy2.moveEnepc(15, 5.0f, 11.0f, 90);
            System.sleep(20);
            ST0681.this.enemy1.moveEnepc(17, 195.0f, 20.0f, 20);
            ST0681.this.enemy1.kickEnepc(1, 0);
            System.sleep(70);
            ST0681.this.enemy2.moveEnepc(17, 180.0f, -20.0f, 20);
            System.sleep(20);
            ST0681.this.enemy2.moveEnepc(15, 5.0f, 8.0f, 20);
            System.sleep(20);
            ST0681.this.enemy2.moveEnepc(17, 165.0f, -20.0f, 20);
            System.sleep(20);
            ST0681.this.SE01.stop();
            Sound.effectPlay(458756);
            ST0681.this.enemy1.kickEnepc(1, 3);
            ST0681.this.enemy2.kickEnepc(1, 3);
            ST0681.this.EV_Camera02();
            ST0681.this.win = Window.create();
            ST0681.this.win.setSize(4, 45);
            ST0681.this.win.setLocation(15, 305);
            if (Runtime.getLeader() == 1) {
                ST0681.this.win.print(ST0681.this.SHI_01, 0);
            } else if (Runtime.getLeader() == 2) {
                ST0681.this.win.print(ST0681.this.KOS_01, 0);
            } else {
                ST0681.this.win.print(ST0681.this.CHA_01, 0);
            }
            System.waitFor(ST0681.this.win);
            Sound.effectStop(458756);
            ST0681.this.EV_Camera03();
            ST0681.this.npc1.setRotate(0.0f, 180.0f, 0.0f);
            ST0681.this.npc1.kickEnepc(4, 0);
            System.sleep(1);
            ST0681.this.npc1.kickEnepc(4, 1);
            ST0681.this.npc1.kickEnepc(1, 3);
            System.sleep(1);
            ST0681.this.npc1.moveEnepc(15, 6.0f, -8.8f, 60);
            System.sleep(60);
            ST0681.this.EV_Camera08();
            ST0681.this.npc1.kickEnepc(1, 1);
            ST0681.this.npc1.moveEnepc(17, 90.0f, -20.0f, 20);
            System.sleep(20);
            ST0681.this.npc1.moveEnepc(15, 6.5f, -8.8f, 20);
            System.sleep(20);
            ST0681.this.npc1.kickEnepc(0, 25);
            System.sleep(25);
            Sound.effectPlay(196741);
            System.sleep(35);
            ST0681.this.win = Window.create();
            ST0681.this.win.setSize(4, 45);
            ST0681.this.win.setLocation(15, 305);
            ST0681.this.win.print(ST0681.this.DOOR, 0);
            System.waitFor(ST0681.this.win);
            ST0681.this.monitor1.signal(1);
            ST0681.this.monitor1.setScale(0.0f, 0.0f, 0.0f);
            Sound.effectPlay(32, 255, 64);
            System.sleep(30);
            ST0681.this.keikoku();
            System.sleep(25);
            ST0681.this.enemy1.kickEnepc(1, 1);
            ST0681.this.enemy2.kickEnepc(1, 1);
            ST0681.this.EV_Camera00();
            ST0681.this.Kakuheki_Close();
            Sound.effectPlay(196713);
            System.sleep(8);
            Sound.effectPlay(196713);
            System.sleep(8);
            Sound.effectPlay(196713);
            System.sleep(8);
            Sound.effectPlay(196713);
            System.sleep(8);
            ST0681.this.EV_Camera04();
            ST0681.this.SE02.start();
            Sound.effectPlay(196639);
            ST0681.this.Yu01.disp(true);
            ST0681.this.Yu02.disp(true);
            ST0681.this.Yu03.disp(true);
            ST0681.this.Yu04.disp(true);
            ST0681.this.Yu05.disp(true);
            ST0681.this.Yu06.disp(true);
            ST0681.this.Yu07.disp(true);
            ST0681.this.Yu08.disp(true);
            ST0681.this.Yu09.disp(true);
            ST0681.this.Yu10.disp(true);
            ST0681.this.Yu11.disp(true);
            ST0681.this.Yu12.disp(true);
            ST0681.this.enemy1.getTranslate();
            ST0681.this.EF01.setTranslate(ST0681.this.enemy1.px, ST0681.this.enemy1.py, ST0681.this.enemy1.pz);
            System.sleep(30);
            ST0681.this.EF01.disp(true);
            ST0681.this.enemy1.kickEnepc(4, 0);
            System.sleep(1);
            ST0681.this.enemy1.kickEnepc(4, 2);
            ST0681.this.enemy2.getTranslate();
            ST0681.this.EF02.setTranslate(ST0681.this.enemy2.px, ST0681.this.enemy2.py, ST0681.this.enemy2.pz);
            System.sleep(30);
            ST0681.this.EF02.disp(true);
            ST0681.this.enemy2.kickEnepc(4, 0);
            System.sleep(1);
            ST0681.this.enemy2.kickEnepc(4, 2);
            System.sleep(30);
            ST0681.this.SE02.stop();
            ST0681.this.EV_Camera05();
            ST0681.this.SE03.start();
            ST0681.this.enemy3.getTranslate();
            ST0681.this.EF03.setTranslate(ST0681.this.enemy3.px, ST0681.this.enemy3.py, ST0681.this.enemy3.pz);
            System.sleep(30);
            ST0681.this.EF03.disp(true);
            ST0681.this.enemy3.kickEnepc(4, 0);
            System.sleep(1);
            ST0681.this.enemy3.kickEnepc(4, 2);
            System.sleep(30);
            ST0681.this.EV_Camera06();
            ST0681.this.enemy4.getTranslate();
            ST0681.this.EF04.setTranslate(ST0681.this.enemy4.px, ST0681.this.enemy4.py, ST0681.this.enemy4.pz);
            System.sleep(30);
            ST0681.this.EF04.disp(true);
            ST0681.this.enemy4.kickEnepc(4, 0);
            System.sleep(1);
            ST0681.this.enemy4.kickEnepc(4, 2);
            System.sleep(30);
            ST0681.this.npc1.setRotate(0.0f, 72.0f, 0.0f);
            Sound.effectStop(196639);
            ST0681.this.SE03.stop();
            ST0681.this.EV_Camera07();
            ST0681.this.npc1.kickEnepc(1, 27);
            ST0681.this.win = Window.create();
            ST0681.this.win.setSize(4, 45);
            ST0681.this.win.setLocation(15, 305);
            if (Runtime.getLeader() == 1) {
                ST0681.this.win.print(ST0681.this.SHI_02, 0);
            } else if (Runtime.getLeader() == 2) {
                ST0681.this.win.print(ST0681.this.KOS_02, 0);
            } else {
                ST0681.this.win.print(ST0681.this.CHA_02, 0);
            }
            System.waitFor(ST0681.this.win);
            System.sleep(30);
            ST0681.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.jumpCF(66216, 5);
            ST0681.this.cam0.setMode(0);
        }
    }

    class Object
            extends Unit {
        Object() {
        }
    }
}

