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
import xeno.map.MC_DYU01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1713
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU01_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit ele0;
    Unit DonDon;
    Unit Star_1;
    Unit Star_2;
    Unit sphere1;
    Unit sphere2;
    Unit ring_1a;
    Unit ring_1b;
    Unit ring_1c;
    Unit ring_2a;
    Unit ring_2b;
    Unit ring_2c;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1_btalked = 0;
    int npc2talked = 0;
    int npc2_btalked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc4_btalked = 0;
    int npc5talked = 0;
    int npc5_btalked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc7_btalked = 0;
    int npc8talked = 0;
    int npc8_btalked = 0;
    int npc9talked = 0;
    int npc9_btalked = 0;
    int npc10talked = 0;
    int npc11talked = 0;
    int npc11_btalked = 0;
    int Tuika_00 = 0;
    int Tuika_01 = 0;
    int Tuika_02 = 0;
    int button_flg = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    MAPUnit mel;
    MAPUnit mil;
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
    String[] Info_00 = new String[]{"Go to the lower level?", "/[waitkey(64)]/[close()]"};
    String[] L01_00CN = new String[]{"This is impossible...Our shots aren't hitting them at all!", "/[waitkey(1)]/[clear()]", "I can't believe it.", "/[waitkey(1)]/[clear()]", "The Foundation's main cannon is supposed to be able to penetrate any kind of energy shield.", "/[waitkey(64)]/[close()]"};
    String[] L01_00CJ = new String[]{"Little Master, this is not normal. This is absolutely not normal.", "/[waitkey(1)]/[clear()]", "So please be careful!", "/[waitkey(64)]/[close()]"};
    String[] L02_00CN = new String[]{"MOMO was born there, correct?", "/[waitkey(1)]/[clear()]", "But why did they need such a huge thing to manufacture the 100-Series Observational Units?", "/[waitkey(1)]/[clear()]", "Did it have some other purpose?", "/[waitkey(64)]/[close()]"};
    String[] L02_00CJ = new String[]{"MOMO was born there, correct?", "/[waitkey(1)]/[clear()]", "But why did they need such a huge thing to manufacture the 100-Series Observational Units?", "/[waitkey(1)]/[clear()]", "Did it have some other purpose?", "/[waitkey(64)]/[close()]"};
    String[] L04_00CN = new String[]{"We'd seriously get blown to pieces!", "/[waitkey(1)]/[clear()]", "Not even the Durandal could survive a shot like that.", "/[waitkey(64)]/[close()]"};
    String[] L04_00CJ = new String[]{"This' no good, Little Master. That ain't normal. That's not something normal humans can handle!", "/[waitkey(64)]/[close()]"};
    String[] L05_00CN = new String[]{"It's amazing, the Gnosis are merging into the Proto Merkabah, one after another.", "/[waitkey(1)]/[clear()]", "It's as if they are becoming a part of the Proto Merkabah.", "/[waitkey(64)]/[close()]"};
    String[] L05_00CJ = new String[]{"Little Master, there are many Gnosis inside the Proto Merkabah.", "/[waitkey(1)]/[clear()]", "Please be careful!", "/[waitkey(64)]/[close()]"};
    String[] L08_00CN = new String[]{"No sign of life where the Federation fleet was destroyed. They've all...They've all been annihilated!", "/[waitkey(64)]/[close()]"};
    String[] L08_00CJ = new String[]{"Little Master! Are we going to end up like them too? We're not going to make it?", "/[waitkey(64)]/[close()]"};
    String[] L09_00CN = new String[]{"The Proto Merkabah has set its axis of fire.", "/[waitkey(1)]/[clear()]", "We believe the second shot will be fired in 10-20 minutes.", "/[waitkey(64)]/[close()]"};
    String[] L09_00CJ = new String[]{"What could be the matter?", "/[waitkey(1)]/[clear()]", "The D#23mmerung has been quiet for a while now. Could they have run into some trouble?", "/[waitkey(64)]/[close()]"};
    String[] L011_00CN = new String[]{"/[label(Mary)]", "Is that another one of Joachim's creations?", "/[waitkey(1)]/[clear()]", "There's the Song of Nephilim and the Proto Merkabah. What in the world did he want to do by creating those things?", "/[waitkey(1)]/[clear()]", "Really, I don't understand the minds of geniuses.", "/[waitkey(64)]/[close()]"};
    String[] L11_01CN = new String[]{"/[label(Mary)]", "At any rate, it's only right that you accept a challenge when it's been made, so give it all you've got!", "/[waitkey(64)]/[close()]"};
    String[] L011_00CJ = new String[]{"/[label(Mary)]", "After that battle with the Gnosis, both the Foundation and the Durandal are practically empty.", "/[waitkey(1)]/[clear()]", "Cheap attacks don't hit them at all. Little Master, we're counting on you!", "/[waitkey(64)]/[close()]"};
    String[] L07_00CN = new String[]{"/[label(Shelley)]", "Proto Merkabah.", "/[waitkey(1)]/[clear()]", "Unofficial reports say that the Proto Merkabah was sent into the Abyss by the Federation government after Joachim Mizrahi's death was verified during the Miltian Conflict.", "/[waitkey(1)]/[clear()]", "In other words, it was supposedly thrown into the double black hole that developed near Old Miltia.", "/[waitkey(1)]/[clear()]", "Why is something that was discarded in a black hole here?", "/[waitkey(1)]/[clear()]", "There may be a discrepancy between historical fact and what actually happened.", "/[waitkey(64)]/[close()]"};
    String[] L07_00CJ = new String[]{"/[label(Shelley)]", "All long-distance attacks on the Proto Merkabah have been deflected just before impact.", "/[waitkey(1)]/[clear()]", "Right now, the only option available to us is to go in directly and destroy the power reactor.", "/[waitkey(1)]/[clear()]", "Little Master, please come back safely!", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "Right before the last stage. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1713() {
    }

    void EV_Camera00() {
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[1] = -3.607f;
        fArray[2] = 8.345f;
        fArray[3] = -8.211f;
        fArray[4] = 240.0f;
        fArray[6] = 8.345f;
        fArray[7] = -8.211f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[0] = 1.0f;
        fArray3[1] = -27.583f;
        fArray3[2] = 202.139f;
        fArray3[4] = 240.0f;
        fArray3[5] = -27.583f;
        fArray3[6] = 180.0f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 3, 240);
        this.camEV.rotateSPL(fArray4, 1, 3, 240);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 7.893f, 5.113f, 4.012f, 200.0f, 7.893f, 3.065f, 4.012f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -12.843f;
        fArray2[2] = -211.314f;
        fArray2[4] = 200.0f;
        fArray2[5] = -12.843f;
        fArray2[6] = -211.314f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 200);
        this.camEV.rotateSPL(fArray3, 1, 3, 200);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3068, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(20, 0.0f, 16.5f, true);
                            System.sleep(25);
                            this.player.rotY(15, 180.0f, true);
                            System.sleep(20);
                            Runtime.disable(65536);
                            this.ele0.setArgs(12, -5.0f);
                            Sound.effectPlay(196745);
                            System.sleep(200);
                            Runtime.setFlags(3067, 1, 1);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.jumpCF(1823, 0);
                            return;
                        }
                        default: {
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(60, 0.0f, 13.0f, true);
                            System.sleep(60);
                            Runtime.setFlags(3068, 1, 0);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getLeader() != 5) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        this.npc8.kickEnepc(4, 1);
                        this.npc8.kickEnepc(9, 100);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.L08_00CN, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    } else {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        this.npc8.kickEnepc(4, 1);
                        this.npc8.kickEnepc(9, 100);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.L08_00CJ, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2 || n2 != 3) return;
        switch (n) {
            default:
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC11(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L011_00CJ, 0);
            System.waitFor(window);
        } else if (this.Tuika_00 == 0) {
            window.print(this.L011_00CN, 0);
            System.waitFor(window);
            this.Tuika_00 = 1;
        } else {
            window.print(this.L11_01CN, 0);
            System.waitFor(window);
            this.Tuika_00 = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L05_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L05_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC7(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L07_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L07_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC9(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.L09_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.L09_00CN, 0);
            System.waitFor(window);
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
        this.teiten1 = new Uwamono(28690, 5.0f, 0.0f, 15.5f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, -0.5f, 1.0f, 6.5f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -3.0f, -2.0f, 2.3f, 0.0f);
        this.teiten3.SetBgm(196609);
        this.teiten4 = new Uwamono(28690, 3.0f, -2.0f, 0.0f, 0.0f);
        this.teiten4.SetBgm(196609);
        this.teiten5 = new Uwamono(28690, -3.0f, -2.0f, -3.5f, 0.0f);
        this.teiten5.SetBgm(196609);
        Stage.setVisible(-1, true);
        this.ring_1a = new Mapunits();
        this.ring_1b = new Mapunits();
        this.ring_1c = new Mapunits();
        this.ring_1a.mapUnit(120);
        this.ring_1b.mapUnit(121);
        this.ring_1c.mapUnit(122);
        this.ring_1a.start(4, null);
        this.ring_1b.start(4, null);
        this.ring_1c.start(4, null);
        this.ring_1a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2a = new Mapunits();
        this.ring_2b = new Mapunits();
        this.ring_2c = new Mapunits();
        this.ring_2a.mapUnit(123);
        this.ring_2b.mapUnit(124);
        this.ring_2c.mapUnit(125);
        this.ring_2a.start(4, null);
        this.ring_2b.start(4, null);
        this.ring_2c.start(4, null);
        this.ring_2a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -1.0f, 1.0f, 3.0f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 2.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 1.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 10.0f, 0.0f, 7.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 350.0f, 0.0f, 7.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 6.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 0, 4, -2.609f, -2.213f, -3.609f, 225.0f);
        this.npc2 = new NPC_NORMAL(1028, 12, 0, 0, 4, 2.7f, -1.763f, -3.7f, 135.0f);
        this.npc4 = new NPC_NORMAL(1028, 14, 0, 0, 4, 2.609f, -1.763f, -0.609f, 135.0f);
        this.npc5 = new NPC_NORMAL(1028, 15, 0, 0, 4, -2.609f, -1.763f, 2.391f, 225.0f);
        this.npc7 = new NPC_NORMAL(289, 17, 0, 0, 4, -0.5f, 1.5f, 7.844f, 180.0f);
        this.npc8 = new NPC_NORMAL(1028, 18, 0, 3, 4, -5.625f, -1.5f, 5.219f, 180.0f);
        this.npc9 = new NPC_NORMAL(1028, 19, 0, 0, 4, 5.313f, 0.5f, 13.688f, 135.0f);
        this.npc11 = new NPC_NORMAL(288, 21, 0, 2, 7, -3.584f, 0.0f, 14.33f, 40.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc4.talkto("TalkNPC4");
        this.npc5.talkto("TalkNPC5");
        this.npc7.talkto("TalkNPC7");
        this.npc8.talkto("TalkNPC8");
        this.npc9.talkto("TalkNPC9");
        this.npc11.talkto("TalkNPC11");
        this.npc1.disableDTKFlag(131075);
        this.npc2.disableDTKFlag(131075);
        this.npc4.disableDTKFlag(131075);
        this.npc5.disableDTKFlag(131075);
        this.npc7.disableDTKFlag(131075);
        this.npc8.disableDTKFlag(131075);
        this.npc9.disableDTKFlag(131075);
        this.npc11.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(12);
        this.npc2.enableDTKFlag(12);
        this.npc4.enableDTKFlag(12);
        this.npc5.enableDTKFlag(12);
        this.npc7.enableDTKFlag(12);
        this.npc8.enableDTKFlag(12);
        this.npc9.enableDTKFlag(12);
        this.npc11.enableDTKFlag(12);
        this.npc1.setMotion(0, 1);
        this.npc2.setMotion(0, 2);
        this.npc4.setMotion(0, 4);
        this.npc5.setMotion(0, 3);
        this.npc7.setMotion(0, 2);
        this.npc8.setMotion(0, 3);
        this.npc9.setMotion(0, 4);
        this.npc1.setInvalidID(1);
        this.npc2.setInvalidID(1);
        this.npc4.setInvalidID(1);
        this.npc5.setInvalidID(1);
        this.npc7.setInvalidID(1);
        this.npc8.setInvalidID(1);
        this.npc9.setInvalidID(1);
        this.npc1.setShadow(0, 0);
        this.npc2.setShadow(0, 0);
        this.npc4.setShadow(0, 0);
        this.npc5.setShadow(0, 0);
        this.npc7.setShadow(0, 0);
        this.npc8.setShadow(0, 0);
        this.npc9.setShadow(0, 0);
        this.mel = new Mapunits();
        this.mel.init(20505, 0.0f, 30.0f, -150.0f, 0.0f);
        this.mel.setScale(45.0f, 45.0f, 45.0f);
        this.mil = new Mapunits();
        this.mil.init(20655, 0.0f, -10.0f, -7.0f, 0.0f);
        this.mil.setSortOffset(1.0E10f);
        if (Runtime.getFlags(3068, 1) == 1) {
            this.DonDon = new Mapunits();
            this.DonDon.mapUnit(133);
            this.DonDon.start(4, null);
            this.DonDon.start(1, "Evt0");
        }
        if (Runtime.getFlags(3068, 1) == 1) {
            this.ele0 = new Unit();
            this.ele0.initElevator(89, 0.025f, -5.0f);
            this.ele0.setArgs(1, 0, 1);
        } else {
            this.ele0 = new Unit();
            this.ele0.initElevator(89, 0.025f, 0.0f);
            this.ele0.setArgs(1, 0, 1);
            this.ele0.setArgs(12, 0.0f);
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

        void Evt0() {
            Runtime.setPlayerControl(false);
            ST1713.this.cam0.setMode(-1);
            ST1713.this.EV_Camera00();
            ST1713.this.player.setTranslate(0.0f, -5.0f, 16.0f);
            ST1713.this.ele0.setArgs(12, 0.0f);
            Sound.effectPlay(196747);
            System.sleep(200);
            Sound.effectPlay(196746);
            System.sleep(40);
            ST1713.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1713.this.player.mtn(2, 9, 1.0f, true);
            ST1713.this.player.move(60, 0.0f, 13.0f, true);
            System.sleep(65);
            Runtime.setFlags(3068, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }
    }
}

