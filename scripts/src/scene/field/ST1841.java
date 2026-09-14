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
import xeno.map.MC_DYU14_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1841
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU14_PRJ {
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
    int npc2btalked = 0;
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
    String[] GUIDE_01 = new String[]{"Going to the Bridge.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Hangar.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Park.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Isolation Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Residential Area Station'", "/[waitkey(64)]/[close()]"};

    ST1841() {
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
        this.camEV.setTranslate(2.0f, 2.159f, 0.69f);
        this.camEV.setRotate(-2.049f, 0.0f, 0.0f);
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
                    this.menu.addItem("Bridge\nHangar\nPark\nIsolation Area\nDock\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1821);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1861);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1851);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1831);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1801);
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
        if (n2 != 1) return;
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
            case 0: {
                Runtime.jumpCF(1751, 5);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 11.0f, 0.0f, 1.0f, 0.0f);
        this.teiten1.SetBgm(196622);
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
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(18179, 5, 4.0f, 0.0f, 7.0f, -90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[48];
        fArray[0] = 4.0f;
        fArray[2] = 7.0f;
        fArray[3] = 1.0f;
        fArray[4] = 5.0f;
        fArray[6] = 7.0f;
        fArray[7] = 2.0f;
        fArray[8] = 6.0f;
        fArray[10] = 7.0f;
        fArray[11] = 3.0f;
        fArray[12] = 7.0f;
        fArray[14] = 7.0f;
        fArray[15] = 4.0f;
        fArray[16] = 7.0f;
        fArray[18] = 6.0f;
        fArray[19] = 5.0f;
        fArray[20] = 7.0f;
        fArray[22] = 5.0f;
        fArray[23] = 6.0f;
        fArray[24] = 7.0f;
        fArray[26] = 4.0f;
        fArray[27] = 7.0f;
        fArray[28] = 7.0f;
        fArray[30] = 3.0f;
        fArray[31] = 8.0f;
        fArray[32] = 7.0f;
        fArray[34] = 2.0f;
        fArray[35] = 9.0f;
        fArray[36] = 6.0f;
        fArray[38] = 2.0f;
        fArray[39] = 10.0f;
        fArray[40] = 5.0f;
        fArray[42] = 2.0f;
        fArray[43] = 11.0f;
        fArray[44] = 4.0f;
        fArray[46] = 2.0f;
        fArray[47] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 5, 1, 5, fArray2);
        float[] fArray3 = new float[24];
        fArray3[0] = 4.0f;
        fArray3[2] = 7.0f;
        fArray3[3] = 5.0f;
        fArray3[5] = 7.0f;
        fArray3[6] = 6.0f;
        fArray3[8] = 7.0f;
        fArray3[9] = 7.0f;
        fArray3[11] = 7.0f;
        fArray3[12] = 7.0f;
        fArray3[14] = 6.0f;
        fArray3[15] = 7.0f;
        fArray3[17] = 5.0f;
        fArray3[18] = 7.0f;
        fArray3[20] = 4.0f;
        fArray3[21] = 7.0f;
        fArray3[23] = 3.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(18179, 5, -1.0f, 0.0f, -2.0f, 90.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[80];
        fArray5[0] = -1.0f;
        fArray5[2] = -2.0f;
        fArray5[3] = 1.0f;
        fArray5[6] = -2.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = 1.0f;
        fArray5[10] = -2.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = 2.0f;
        fArray5[14] = -2.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = 3.0f;
        fArray5[18] = -2.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = 4.0f;
        fArray5[22] = -2.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = 5.0f;
        fArray5[26] = -2.0f;
        fArray5[27] = 7.0f;
        fArray5[28] = 6.0f;
        fArray5[30] = -2.0f;
        fArray5[31] = 8.0f;
        fArray5[32] = 7.0f;
        fArray5[34] = -2.0f;
        fArray5[35] = 9.0f;
        fArray5[36] = 7.0f;
        fArray5[38] = -3.0f;
        fArray5[39] = 10.0f;
        fArray5[40] = 7.0f;
        fArray5[42] = -4.0f;
        fArray5[43] = 11.0f;
        fArray5[44] = 6.0f;
        fArray5[46] = -4.0f;
        fArray5[47] = 12.0f;
        fArray5[48] = 5.0f;
        fArray5[50] = -4.0f;
        fArray5[51] = 13.0f;
        fArray5[52] = 4.0f;
        fArray5[54] = -4.0f;
        fArray5[55] = 14.0f;
        fArray5[56] = 3.0f;
        fArray5[58] = -4.0f;
        fArray5[59] = 15.0f;
        fArray5[60] = 2.0f;
        fArray5[62] = -4.0f;
        fArray5[63] = 16.0f;
        fArray5[64] = 1.0f;
        fArray5[66] = -4.0f;
        fArray5[67] = 17.0f;
        fArray5[70] = -4.0f;
        fArray5[71] = 18.0f;
        fArray5[72] = -1.0f;
        fArray5[74] = -4.0f;
        fArray5[75] = 19.0f;
        fArray5[76] = -1.0f;
        fArray5[78] = -3.0f;
        fArray5[79] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 4, 2, 5, fArray6);
        float[] fArray7 = new float[60];
        fArray7[0] = -1.0f;
        fArray7[2] = -2.0f;
        fArray7[5] = -2.0f;
        fArray7[6] = 1.0f;
        fArray7[8] = -2.0f;
        fArray7[9] = 2.0f;
        fArray7[11] = -2.0f;
        fArray7[12] = 3.0f;
        fArray7[14] = -2.0f;
        fArray7[15] = 4.0f;
        fArray7[17] = -2.0f;
        fArray7[18] = 5.0f;
        fArray7[20] = -2.0f;
        fArray7[21] = 6.0f;
        fArray7[23] = -2.0f;
        fArray7[24] = 7.0f;
        fArray7[26] = -2.0f;
        fArray7[27] = 7.0f;
        fArray7[29] = -3.0f;
        fArray7[30] = 7.0f;
        fArray7[32] = -4.0f;
        fArray7[33] = 6.0f;
        fArray7[35] = -4.0f;
        fArray7[36] = 5.0f;
        fArray7[38] = -4.0f;
        fArray7[39] = 4.0f;
        fArray7[41] = -4.0f;
        fArray7[42] = 3.0f;
        fArray7[44] = -4.0f;
        fArray7[45] = 2.0f;
        fArray7[47] = -4.0f;
        fArray7[48] = 1.0f;
        fArray7[50] = -4.0f;
        fArray7[53] = -4.0f;
        fArray7[54] = -1.0f;
        fArray7[56] = -4.0f;
        fArray7[57] = -1.0f;
        fArray7[59] = -3.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(18179, 5, 2.0f, 0.0f, 3.0f, 270.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray9 = new float[68];
        fArray9[0] = 2.0f;
        fArray9[2] = 3.0f;
        fArray9[3] = 1.0f;
        fArray9[4] = 1.0f;
        fArray9[6] = 2.0f;
        fArray9[7] = 2.0f;
        fArray9[10] = 1.5f;
        fArray9[11] = 3.0f;
        fArray9[12] = -1.0f;
        fArray9[14] = 2.0f;
        fArray9[15] = 4.0f;
        fArray9[16] = -1.0f;
        fArray9[18] = 3.5f;
        fArray9[19] = 5.0f;
        fArray9[20] = 0.2f;
        fArray9[22] = 4.0f;
        fArray9[23] = 6.0f;
        fArray9[24] = 0.2f;
        fArray9[26] = 5.0f;
        fArray9[27] = 7.0f;
        fArray9[28] = 0.2f;
        fArray9[30] = 6.0f;
        fArray9[31] = 8.0f;
        fArray9[32] = 0.2f;
        fArray9[34] = 7.0f;
        fArray9[35] = 9.0f;
        fArray9[36] = -0.25f;
        fArray9[38] = 7.3f;
        fArray9[39] = 10.0f;
        fArray9[40] = -0.9f;
        fArray9[42] = 7.3f;
        fArray9[43] = 11.0f;
        fArray9[44] = -1.5f;
        fArray9[46] = 7.3f;
        fArray9[47] = 12.0f;
        fArray9[48] = -1.8f;
        fArray9[50] = 7.0f;
        fArray9[51] = 13.0f;
        fArray9[52] = -1.8f;
        fArray9[54] = 6.5f;
        fArray9[55] = 14.0f;
        fArray9[56] = -1.8f;
        fArray9[58] = 6.0f;
        fArray9[59] = 15.0f;
        fArray9[60] = -1.8f;
        fArray9[62] = 5.0f;
        fArray9[63] = 16.0f;
        fArray9[64] = -1.8f;
        fArray9[66] = 4.0f;
        fArray9[67] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 5, 3, 5, fArray10);
        float[] fArray11 = new float[12];
        fArray11[0] = 2.0f;
        fArray11[2] = 3.0f;
        fArray11[3] = 1.0f;
        fArray11[5] = 3.0f;
        fArray11[8] = 3.0f;
        fArray11[9] = -1.0f;
        fArray11[11] = 3.0f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.EXP0 = new NPC_NORMAL(20492, 1, 0, 0, 3, 13.0f, -1.0f, 1.0f, 180.0f);
        this.EXP0.talkto("TalkNPC1");
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.doorA = new Uwamono(4, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        if (Runtime.getFlags(3072, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(0);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt");
        } else {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(0);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "algo");
        }
        this.Step = new Mapunits();
        this.Step.mapUnit(12);
        this.Step.start(4, null);
        this.Step.setTranslate(-1.0f, -0.01f, 0.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, 2.0f, 2.25f, -2.85f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, 2.0f, 2.25f, -2.84f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18022, 0, 256, 130);
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
            if (n >= 0 && n < 30) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.033333335f * (float) n, 0.033333335f * (float) n, 0.033333335f * (float) n);
            }
            if (n == 30) break;
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
            ST1841.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1841.this.Step.setTranslate(ST1841.this.Step.px - 0.016666668f, ST1841.this.Step.py, ST1841.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt() {
            ST1841.this.off();
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1841.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1841.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1841.this.EV_Camera01();
            System.sleep(1);
            ST1841.this.EXP0.kickEnepc(4, 1);
            ST1841.this.win = Window.create();
            ST1841.this.win.setSize(4, 45);
            ST1841.this.win.setLocation(15, 305);
            ST1841.this.win.print("Residential Area");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1841.this.EXP0.getTranslate();
                ST1841.this.EXP0.setTranslate(ST1841.this.EXP0.px, ST1841.this.EXP0.py, ST1841.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1841.this.win.close();
            ST1841.this.player.setLocation(1, 2);
            ST1841.this.EXP0.kickEnepc(0, 1);
            ST1841.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1841.this.doorA.DoorOpen();
            System.sleep(15);
            ST1841.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1841.this.player.mtn(2, 9, 1.0f, true);
            ST1841.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1841.this.Step.start(1, "Chijimi");
            ST1841.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1841.this.EXP0.kickEnepc(4, 0);
            ST1841.this.EXP0.kickEnepc(4, 2);
            ST1841.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1841.this.cam0.setMode(0);
            Runtime.setFlags(3072, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
            ST1841.this.on();
        }

        void Nobi() {
            int n = 0;
            ST1841.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1841.this.Step.setTranslate(ST1841.this.Step.px + 0.016666668f, ST1841.this.Step.py, ST1841.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void algo() {
            ST1841.this.EXP0.kickEnepc(0, 2);
            int n = 0;
            while (n < 44) {
                System.sleep(1);
                ++n;
            }
            ST1841.this.EXP0.kickEnepc(4, 2);
        }
    }
}

