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
import xeno.map.MC_DYU12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1821
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU12_PRJ {
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
    Enepc enemy5;
    Enepc EXP0;
    Unit unit1;
    Unit DenDen;
    Unit DonDon;
    Unit ele0;
    Unit Step;
    Unit monitor1;
    Unit monitor2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
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
    int button1_flg = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] GUIDE_00 = new String[]{"Please enter your destination.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_01 = new String[]{"Going to the Hangar.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Park.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Isolation Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] Info_00 = new String[]{"Go to the bridge?", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Currently, all areas beyond this point are closed due to an emergency situation.\n", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Bridge Station'", "/[waitkey(64)]/[close()]"};

    ST1821() {
    }

    void Departure(int n) {
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.Step.start(1, "Nobi");
        Sound.effectPlay(196741);
        this.EXP0.kickEnepc(4, 1);
        this.EXP0.kickEnepc(0, 1);
        System.sleep(20);
        this.doorA.DoorOpen();
        System.sleep(25);
        this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
        Runtime.enable(65536);
        this.player.mtn(2, 9, 1.0f, true);
        this.player.move(60, 13.0f, 1.0f, true);
        System.sleep(35);
        this.doorA.DoorClose();
        this.EXP0.kickEnepc(0, 2);
        System.sleep(20);
        this.Step.start(1, "Chijimi");
        System.sleep(25);
        this.EXP0.kickEnepc(3, 2, 45, 45, 1, 100);
        this.player.setTranslate(-100.0f, -0.0f, -1.0f);
        Runtime.disable(65536);
        int n2 = 0;
        int n3 = 90;
        float f = 20.0f / (float) n3 / (float) n3;
        while (n2 < n3) {
            float f2 = 2.0f * f * (float) n2;
            this.EXP0.getTranslate();
            this.EXP0.setTranslate(this.EXP0.px, this.EXP0.py, this.EXP0.pz + f2);
            if (n2 == n3 - 30) {
                this.fade.call(0);
            }
            ++n2;
            System.sleep(1);
        }
        Runtime.setPlayerControl(true);
        Runtime.jumpCF(n, 0);
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.62f, 5.327f, 11.503f);
        this.camEV.setRotate(-13.309f, -19.634f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 7.62f, 5.327f, 11.503f, 240.0f, -1.637f, 5.327f, 1.0f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -13.309f;
        fArray2[2] = -19.634f;
        fArray2[4] = 240.0f;
        fArray2[5] = -13.309f;
        fArray2[6] = -90.0f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 240);
        this.camEV.rotateSPL(fArray3, 1, 3, 240);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.53f, 2.03f, 1.722f);
        this.camEV.setRotate(0.0f, 0.0f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (this.button1_flg == 1) {
                        return;
                    }
                    this.button1_flg = 1;
                    this.off();
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.GUIDE_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Hangar\nPark\nIsolation Area\nDock\nResidential Area\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1861);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1851);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1831);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1801);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1841);
                            return;
                        }
                        case 5: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.GUIDE_06, 0);
                            System.waitFor(this.win);
                            this.button1_flg = 0;
                            Runtime.setPlayerControl(true);
                            System.sleep(1);
                            this.on();
                            return;
                        }
                        default: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.GUIDE_06, 0);
                            System.waitFor(this.win);
                            this.button1_flg = 0;
                            Runtime.setPlayerControl(true);
                            System.sleep(1);
                            this.on();
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) return;
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    System.sleep(1);
                    Runtime.disable(262144);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 3) return;
        switch (n) {
            case 100: {
                System.sleep(1);
                Runtime.enable(262144);
                System.sleep(1);
                System.println("PPPPPPPPPPPPPPPPPPPPPPP");
                Runtime.setPlayerControl(false);
                this.keikoku_1();
                System.sleep(32);
                System.sleep(1);
                this.cam0.setMode(-1);
                this.EV_Camera02();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.PLAYER_01, 0);
                System.waitFor(this.win);
                this.cam0.setMode(0);
                System.sleep(1);
                this.monitor2.signal(0);
                System.sleep(30);
                this.keikoku_2();
                System.sleep(30);
                Runtime.setPlayerControl(true);
                System.sleep(1);
                Runtime.disable(262144);
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            default:
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -6.5f, 0.0f, -2.5f, 0.0f);
        this.teiten1.SetBgm(196623);
        this.teiten2 = new Uwamono(28690, 11.0f, 0.0f, 1.0f, 0.0f);
        this.teiten2.SetBgm(196622);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, -2.0f, 0.0f, 0.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, -15.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 17.5f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(18179, 5, -2.0f, 0.0f, -1.5f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[96];
        fArray[0] = -2.0f;
        fArray[2] = -1.5f;
        fArray[3] = 1.0f;
        fArray[4] = -2.0f;
        fArray[6] = -1.0f;
        fArray[7] = 2.0f;
        fArray[8] = -1.0f;
        fArray[10] = -0.5f;
        fArray[11] = 3.0f;
        fArray[15] = 4.0f;
        fArray[16] = 1.0f;
        fArray[19] = 5.0f;
        fArray[20] = 2.0f;
        fArray[23] = 6.0f;
        fArray[24] = 3.0f;
        fArray[27] = 7.0f;
        fArray[28] = 4.0f;
        fArray[31] = 8.0f;
        fArray[32] = 5.0f;
        fArray[34] = -0.5f;
        fArray[35] = 9.0f;
        fArray[36] = 6.0f;
        fArray[38] = -1.0f;
        fArray[39] = 10.0f;
        fArray[40] = 7.0f;
        fArray[42] = -1.5f;
        fArray[43] = 11.0f;
        fArray[44] = 8.0f;
        fArray[46] = -2.5f;
        fArray[47] = 12.0f;
        fArray[48] = 8.0f;
        fArray[50] = -3.5f;
        fArray[51] = 13.0f;
        fArray[52] = 7.0f;
        fArray[54] = -4.0f;
        fArray[55] = 14.0f;
        fArray[56] = 6.0f;
        fArray[58] = -4.0f;
        fArray[59] = 15.0f;
        fArray[60] = 5.0f;
        fArray[62] = -4.0f;
        fArray[63] = 16.0f;
        fArray[64] = 4.0f;
        fArray[66] = -4.0f;
        fArray[67] = 17.0f;
        fArray[68] = 3.0f;
        fArray[70] = -4.0f;
        fArray[71] = 18.0f;
        fArray[72] = 2.0f;
        fArray[74] = -4.0f;
        fArray[75] = 19.0f;
        fArray[76] = 1.0f;
        fArray[78] = -4.0f;
        fArray[79] = 20.0f;
        fArray[82] = -4.0f;
        fArray[83] = 21.0f;
        fArray[84] = -1.0f;
        fArray[86] = -4.0f;
        fArray[87] = 22.0f;
        fArray[88] = -2.0f;
        fArray[90] = -3.0f;
        fArray[91] = 23.0f;
        fArray[92] = -2.0f;
        fArray[94] = -1.5f;
        fArray[95] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 3, 1, 5, fArray2);
        float[] fArray3 = new float[33];
        fArray3[0] = -2.0f;
        fArray3[2] = -1.5f;
        fArray3[3] = -1.0f;
        fArray3[5] = -1.5f;
        fArray3[8] = -1.5f;
        fArray3[9] = 1.0f;
        fArray3[11] = -1.5f;
        fArray3[12] = 2.0f;
        fArray3[14] = -1.5f;
        fArray3[15] = 3.0f;
        fArray3[17] = -1.5f;
        fArray3[18] = 4.0f;
        fArray3[20] = -1.5f;
        fArray3[21] = 5.0f;
        fArray3[23] = -1.5f;
        fArray3[24] = 6.0f;
        fArray3[26] = -1.5f;
        fArray3[27] = 6.0f;
        fArray3[29] = -2.5f;
        fArray3[30] = 6.0f;
        fArray3[32] = -3.5f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(18179, 5, -8.0f, 0.0f, 1.0f, 90.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[28];
        fArray5[0] = -8.0f;
        fArray5[2] = 1.0f;
        fArray5[3] = 1.0f;
        fArray5[4] = -7.0f;
        fArray5[6] = 2.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = -6.0f;
        fArray5[10] = 3.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = -5.0f;
        fArray5[14] = 4.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = -4.0f;
        fArray5[18] = 5.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = -3.0f;
        fArray5[22] = 6.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = -8.0f;
        fArray5[26] = 1.0f;
        fArray5[27] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 2, 2, 5, fArray6);
        this.enemy3 = new Enepc();
        this.enemy3.init(18179, 5, 6.5f, 0.0f, 6.0f, 180.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[76];
        fArray7[0] = 6.5f;
        fArray7[2] = 6.0f;
        fArray7[3] = 1.0f;
        fArray7[4] = 6.5f;
        fArray7[6] = 5.0f;
        fArray7[7] = 2.0f;
        fArray7[8] = 6.5f;
        fArray7[10] = 4.0f;
        fArray7[11] = 3.0f;
        fArray7[12] = 6.0f;
        fArray7[14] = 3.0f;
        fArray7[15] = 4.0f;
        fArray7[16] = 5.0f;
        fArray7[18] = 2.5f;
        fArray7[19] = 5.0f;
        fArray7[20] = 4.15f;
        fArray7[22] = 3.0f;
        fArray7[23] = 6.0f;
        fArray7[24] = 4.15f;
        fArray7[26] = 3.6f;
        fArray7[27] = 7.0f;
        fArray7[28] = 4.15f;
        fArray7[30] = 4.8f;
        fArray7[31] = 8.0f;
        fArray7[32] = 4.5f;
        fArray7[34] = 6.0f;
        fArray7[35] = 9.0f;
        fArray7[36] = 4.0f;
        fArray7[38] = 7.0f;
        fArray7[39] = 10.0f;
        fArray7[40] = 3.0f;
        fArray7[42] = 7.0f;
        fArray7[43] = 11.0f;
        fArray7[44] = 2.0f;
        fArray7[46] = 7.0f;
        fArray7[47] = 12.0f;
        fArray7[48] = 1.0f;
        fArray7[50] = 7.0f;
        fArray7[51] = 13.0f;
        fArray7[54] = 7.0f;
        fArray7[55] = 14.0f;
        fArray7[56] = -1.0f;
        fArray7[58] = 7.0f;
        fArray7[59] = 15.0f;
        fArray7[60] = -1.15f;
        fArray7[62] = 6.25f;
        fArray7[63] = 16.0f;
        fArray7[64] = -1.15f;
        fArray7[66] = 5.1f;
        fArray7[67] = 17.0f;
        fArray7[68] = -1.15f;
        fArray7[70] = 3.7f;
        fArray7[71] = 18.0f;
        fArray7[72] = 6.5f;
        fArray7[74] = 6.0f;
        fArray7[75] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(1, 3, 3, 5, fArray8);
        float[] fArray9 = new float[30];
        fArray9[0] = 6.5f;
        fArray9[2] = 6.0f;
        fArray9[3] = 6.5f;
        fArray9[5] = 5.0f;
        fArray9[6] = 6.5f;
        fArray9[8] = 4.0f;
        fArray9[9] = 6.5f;
        fArray9[11] = 3.0f;
        fArray9[12] = 6.5f;
        fArray9[14] = 2.0f;
        fArray9[15] = 5.5f;
        fArray9[17] = 2.0f;
        fArray9[18] = 4.5f;
        fArray9[20] = 2.0f;
        fArray9[21] = 3.5f;
        fArray9[23] = 2.0f;
        fArray9[24] = 2.5f;
        fArray9[26] = 1.0f;
        fArray9[27] = 1.5f;
        fArray9[29] = 1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(fArray10);
        this.EXP0 = new NPC_NORMAL(20492, 1, 0, 0, 3, 13.0f, -1.0f, 1.0f, 180.0f);
        this.EXP0.talkto("TalkNPC1");
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.doorA = new Uwamono(106, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(75, 40, '\u0001');
        new Uwamono(74, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0002');
        if (Runtime.getFlags(3070, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt1");
        } else {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "algo");
        }
        if (Runtime.getFlags(3067, 1) == 1) {
            this.DonDon = new Mapunits();
            this.DonDon.mapUnit(2);
            this.DonDon.start(4, null);
            this.DonDon.start(1, "Evt2");
        }
        if (Runtime.getFlags(3067, 1) == 1) {
            this.ele0 = new Unit();
            this.ele0.initElevator(197, 0.065f, 13.0f);
            this.ele0.setArgs(1, 0, 1);
        } else {
            this.ele0 = new Unit();
            this.ele0.initElevator(197, 0.065f, 0.0f);
            this.ele0.setArgs(1, 0, 1);
            this.ele0.setArgs(12, 0.0f);
        }
        this.Step = new Mapunits();
        this.Step.mapUnit(120);
        this.Step.start(4, null);
        this.Step.setTranslate(-1.0f, -0.01f, 0.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, 2.53f, 2.25f, -2.25f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, 2.58f, 2.25f, -2.24f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18021, 0, 256, 130);
        this.monitor2.setArgs(2, 100, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    void keikoku_1() {
        int n = 0;
        while (true) {
            if (n == 0) {
                this.monitor1.signal(1);
                this.monitor1.setScale(0.0f, 0.0f, 0.0f);
            }
            if (n >= 2 && n < 32) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.033333335f * (float) n, 0.033333335f * (float) n, 0.033333335f * (float) n);
            }
            if (n == 32) break;
            ++n;
            System.sleep(1);
        }
        this.monitor2.signal(1);
    }

    void keikoku_2() {
        int n = 0;
        while (true) {
            if (n >= 0 && n < 30) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n));
            }
            if (n == 30) break;
            ++n;
            System.sleep(1);
        }
        this.monitor1.setScale(0.0f, 0.0f, 0.0f);
        this.monitor1.signal(0);
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
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
            this.setElevatorMode(1);
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

        void Chijimi() {
            int n = 0;
            ST1821.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1821.this.Step.setTranslate(ST1821.this.Step.px - 0.016666668f, ST1821.this.Step.py, ST1821.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt1() {
            ST1821.this.off();
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1821.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1821.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1821.this.EV_Camera01();
            System.sleep(1);
            ST1821.this.EXP0.kickEnepc(4, 1);
            ST1821.this.win = Window.create();
            ST1821.this.win.setSize(4, 45);
            ST1821.this.win.setLocation(15, 305);
            ST1821.this.win.print("Bridge");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1821.this.EXP0.getTranslate();
                ST1821.this.EXP0.setTranslate(ST1821.this.EXP0.px, ST1821.this.EXP0.py, ST1821.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1821.this.win.close();
            ST1821.this.player.setLocation(1, 3);
            ST1821.this.EXP0.kickEnepc(0, 1);
            ST1821.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1821.this.doorA.DoorOpen();
            System.sleep(15);
            ST1821.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1821.this.player.mtn(2, 9, 1.0f, true);
            ST1821.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1821.this.Step.start(1, "Chijimi");
            ST1821.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1821.this.EXP0.kickEnepc(4, 0);
            ST1821.this.EXP0.kickEnepc(4, 2);
            ST1821.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1821.this.cam0.setMode(0);
            Runtime.setFlags(3070, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
            ST1821.this.on();
        }

        void Evt2() {
            ST1821.this.off();
            ST1821.this.player.setRotateY(0.0f);
            Runtime.setPlayerControl(false);
            ST1821.this.player.setLocation(1, 2);
            ST1821.this.ele0.setArgs(12, 0.0f);
            System.sleep(240);
            ST1821.this.doorB.SetDoorType('\u0002');
            ST1821.this.doorB.DoorOpen();
            System.sleep(30);
            Runtime.enable(65536);
            ST1821.this.player.mtn(2, 9, 1.0f, true);
            ST1821.this.player.move(60, -6.5f, 1.0f, true);
            System.sleep(60);
            ST1821.this.doorB.DoorClose();
            ST1821.this.doorB.SetDoorType('\u0004');
            Runtime.setFlags(3067, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST1821.this.on();
        }

        void Nobi() {
            int n = 0;
            ST1821.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1821.this.Step.setTranslate(ST1821.this.Step.px + 0.016666668f, ST1821.this.Step.py, ST1821.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void algo() {
            ST1821.this.EXP0.kickEnepc(0, 2);
            int n = 0;
            while (n < 44) {
                System.sleep(1);
                ++n;
            }
            ST1821.this.EXP0.kickEnepc(4, 2);
        }
    }
}

