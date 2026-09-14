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
import xeno.map.MC_DYU16_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1860
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU16_PRJ {
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
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
    int npc3talked = 0;
    int npc3btalked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    int button1_flg = 0;
    int shishi = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono col1;
    Uwamono Otakara_A;
    Uwamono Otakara_B;
    Uwamono item01;
    Uwamono item02;
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
    String[] GUIDE_01 = new String[]{"Going to the Park.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Isolation Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Bridge.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] ENG1_00 = new String[]{"Awww, Little Master! Please handle your A.G.W.S. with a bit more care.", "/[waitkey(1)]/[clear()]", "We can't keep fixing it if you keep breaking it as soon as we fix it!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_01 = new String[]{"Please put yourself in our shoes!!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_02 = new String[]{"Arrrgh! Little Master's piloting sure is rough. His cutting edge A.G.W.S. is already falling apart.", "/[waitkey(64)]/[close()]"};
    String[] ENG1_03 = new String[]{"And to top things off, he's really strict about being meticulous when fixing it, and he always wants it done immediately.", "/[waitkey(64)]/[close()]"};
    String[] ENG2_00 = new String[]{"Oh, going out again, Little Master? Then please use another A.G.W.S.", "/[waitkey(64)]/[close()]"};
    String[] ENG2_01 = new String[]{"Your A.G.W.S. is in no condition to be used yet!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_02 = new String[]{"When Little Master uses an A.G.W.S., it's more often than not a dangerous investigation.", "/[waitkey(1)]/[clear()]", "So those of us servicing it have to put our hearts into maintaining it.", "/[waitkey(64)]/[close()]"};
    String[] POL_00 = new String[]{"Hello, Sir! No one has entered the armory under my watch!", "/[waitkey(64)]/[close()]"};
    String[] ENG4_00 = new String[]{"Oh, Little Master? You can't! You can't just take the weapons...", "/[waitkey(1)]/[clear()]", "I'm the one who always gets in trouble, you know!", "/[waitkey(64)]/[close()]"};
    String[] ENG4_01 = new String[]{"Oh, no, no, no!!", "/[waitkey(1)]/[clear()]", "There're all sorts of valuable things, like explosives, in the armory!!", "/[waitkey(1)]/[clear()]", "If you go in there without permission, I'm the one who gets in trouble!", "/[waitkey(64)]/[close()]"};
    String[] POL_01 = new String[]{"What do you want?\n", "/[waitkey(1)]/[clear()]", "This is the armory. This place does not concern guests.", "/[waitkey(64)]/[close()]"};
    String[] POL_02 = new String[]{"A girl with pink hair? This seems the least likely place you'd come across someone like that.", "/[waitkey(1)]/[clear()]", "And who is this girl? Is she a musician or something?", "/[waitkey(64)]/[close()]"};
    String[] DOA_00 = new String[]{"It's locked.", "/[waitkey(64)]/[close()]"};
    String[] DOA_01 = new String[]{"Used /[color(0x329bbe)]Armory Key/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] K01_00 = new String[]{"Man, those Federation guys messed with them!", "/[waitkey(1)]/[clear()]", "I better hurry and adjust them or Little Master will be really mad.", "/[waitkey(64)]/[close()]"};
    String[] K02_00 = new String[]{"What? A guy named Allen? There's no one here by that name.", "/[waitkey(1)]/[clear()]", "Maybe he went down to the Foundation to do some sightseeing?", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Hangar Station'", "/[waitkey(64)]/[close()]"};

    ST1860() {
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
        this.player.move(60, -13.0f, -1.0f, true);
        System.sleep(35);
        this.doorA.DoorClose();
        this.EXP0.kickEnepc(0, 2);
        System.sleep(20);
        this.Step.start(1, "Chijimi");
        System.sleep(25);
        this.EXP0.kickEnepc(3, 2, 45, 45, 1, 100);
        this.player.setVisible(false);
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
        this.camEV.setTranslate(-6.853f, 6.095f, 8.983f);
        this.camEV.setRotate(-16.049f, 25.459f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -6.853f, 6.095f, 8.983f, 240.0f, 1.745f, 6.095f, -1.0f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -16.049f;
        fArray2[2] = 25.459f;
        fArray2[4] = 240.0f;
        fArray2[5] = -17.109f;
        fArray2[6] = 90.0f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 240);
        this.camEV.rotateSPL(fArray3, 1, 3, 240);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.502f, 2.255f, -0.089f);
        this.camEV.setRotate(-5.149f, 0.0f, 0.0f);
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
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.GUIDE_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Park\nIsolation Area\nDock\nResidential Area\nBridge\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1850);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1830);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1800);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1840);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1820);
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
                    if (Runtime.getFlags(3107, 1) != 0) return;
                    if (Runtime.checkItem(10, 63) != 0) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.DOA_01, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.doorE.SetDoorType('\u0004');
                        Runtime.setFlags(3107, 1, 1);
                        return;
                    } else {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.DOA_00, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                    }
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
                System.println("PPPPPPPPPPPPPPPPPPPPPPP");
                Runtime.setPlayerControl(false);
                this.keikoku_1();
                System.sleep(32);
                this.cam0.setMode(-1);
                this.EV_Camera02();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.PLAYER_01, 0);
                System.waitFor(this.win);
                this.cam0.setMode(0);
                this.monitor2.signal(0);
                System.sleep(30);
                this.keikoku_2();
                System.sleep(30);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1btalked == 0) {
                window.print(this.ENG1_00, 0);
                System.waitFor(window);
                this.npc1btalked = 1;
            } else {
                window.print(this.ENG1_01, 0);
                System.waitFor(window);
                this.npc1btalked = 0;
            }
        } else if (this.npc1talked == 0) {
            window.print(this.ENG1_02, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.ENG1_03, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        window.print(this.K01_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2btalked == 0) {
                window.print(this.ENG2_00, 0);
                System.waitFor(window);
                this.npc2btalked = 1;
            } else {
                window.print(this.ENG2_01, 0);
                System.waitFor(window);
                this.npc2btalked = 0;
            }
        } else {
            window.print(this.ENG2_02, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        window.print(this.K02_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.POL_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.POL_01, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3x(Enepc enepc, Window window) {
        if (this.shishi == 0) {
            window.print(this.POL_02, 0);
            System.waitFor(window);
            this.shishi = 1;
        } else {
            window.print(this.POL_01, 0);
            System.waitFor(window);
            this.shishi = 0;
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.ENG4_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.ENG4_01, 0);
            System.waitFor(window);
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1780, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -11.0f, 0.0f, -1.0f, 0.0f);
        this.teiten1.SetBgm(196622);
        Stage.setVisible(-1, true);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 13.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 7.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 7.0f);
        this.npc1 = new NPC_NORMAL(527, 11, 0, 2, 6, -0.708f, 0.0f, -11.506f, 135.0f);
        this.npc1.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 2);
        this.npc2 = new NPC_NORMAL(527, 12, 0, 2, 5, 0.64f, 0.0f, -10.432f, 250.0f);
        this.npc2.disableDTKFlag(131074);
        this.npc2.enableDTKFlag(8);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 9);
        this.npc3 = new Enepc();
        this.npc3.init(1288, 5, -7.0f, 0.0f, 1.0f, 90.0f);
        this.npc3.id = 13;
        this.npc3.setParams(0, 3, 13, 5);
        float[] fArray = new float[48];
        fArray[0] = -7.0f;
        fArray[2] = 1.0f;
        fArray[3] = -6.0f;
        fArray[5] = 1.0f;
        fArray[6] = -5.0f;
        fArray[8] = 1.0f;
        fArray[9] = -4.0f;
        fArray[11] = 1.0f;
        fArray[12] = -3.0f;
        fArray[14] = 1.0f;
        fArray[15] = -2.0f;
        fArray[17] = 1.0f;
        fArray[18] = -1.0f;
        fArray[20] = 1.0f;
        fArray[23] = 1.0f;
        fArray[24] = 1.0f;
        fArray[26] = 1.0f;
        fArray[27] = 2.0f;
        fArray[29] = 1.0f;
        fArray[30] = 3.0f;
        fArray[32] = 1.0f;
        fArray[33] = 4.0f;
        fArray[35] = 1.0f;
        fArray[36] = 5.0f;
        fArray[38] = 1.0f;
        fArray[39] = 6.0f;
        fArray[41] = 1.0f;
        fArray[42] = 7.0f;
        fArray[44] = 1.0f;
        fArray[45] = 8.0f;
        fArray[47] = 1.0f;
        float[] fArray2 = fArray;
        this.npc3.setParams(fArray2);
        this.npc3.setShadow(3, 16);
        this.npc3.enableDTKFlag(8);
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc4 = new NPC_NORMAL(527, 14, 0, 2, 5, 5.886f, 0.0f, -4.182f, 0.0f);
            this.npc4.disableDTKFlag(131074);
            this.npc4.enableDTKFlag(8);
            this.npc4.enableDTKFlag(4);
        }
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
            if (Runtime.getFlags(304, 1) == 0) {
                this.npc3.talkto("TalkNPC3x");
            } else {
                this.npc3.talkto("TalkNPC3");
            }
            this.npc4.talkto("TalkNPC4");
        } else {
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
            this.npc3.talkto("TalkNPC3");
        }
        this.EXP0 = new Enepc();
        this.EXP0.init(20492, 3, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.id = 31;
        this.EXP0.setParams(0, 0, 31, 3);
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.Otakara_B = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 90.0f, 414);
        this.Otakara_B.SetSymbol(28686);
        this.Otakara_B.SetCallNo(1);
        this.item01 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 214);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 215);
        new Uwamono(26, 4, this.Otakara_B);
        new Uwamono(42, 4, this.item01);
        new Uwamono(43, 4, this.item02);
        new Uwamono(44, 0);
        this.doorA = new Uwamono(41, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorD = new Uwamono(48, 42, '\u0001');
        new Uwamono(49, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(62, 40, '\u0001');
        if (Runtime.getFlags(3107, 1) == 0) {
            this.doorE.SetDoorType('\u0002');
        } else {
            this.doorE.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(3074, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(0);
            this.DenDen.start(4, null);
            this.player.setLocation(1, 2);
            this.player.setVisible(false);
            this.DenDen.start(1, "Evt");
        }
        this.Step = new Mapunits();
        this.Step.mapUnit(0);
        this.Step.start(4, null);
        this.Step.setTranslate(1.0f, -0.01f, 0.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, -5.502f, 2.25f, -3.8f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, -5.502f, 2.25f, -3.79f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18020, 0, 256, 130);
        this.monitor2.setArgs(2, 100, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3225, 1, 1);
                break;
            }
        }
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
            ST1860.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1860.this.Step.setTranslate(ST1860.this.Step.px + 0.016666668f, ST1860.this.Step.py, ST1860.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt() {
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1860.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1860.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1860.this.EV_Camera01();
            System.sleep(1);
            ST1860.this.EXP0.kickEnepc(4, 1);
            ST1860.this.win = Window.create();
            ST1860.this.win.setSize(4, 45);
            ST1860.this.win.setLocation(15, 305);
            ST1860.this.win.print("Hangar");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1860.this.EXP0.getTranslate();
                ST1860.this.EXP0.setTranslate(ST1860.this.EXP0.px, ST1860.this.EXP0.py, ST1860.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1860.this.win.close();
            ST1860.this.player.setVisible(true);
            ST1860.this.EXP0.kickEnepc(0, 1);
            ST1860.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1860.this.doorA.DoorOpen();
            System.sleep(15);
            ST1860.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1860.this.player.mtn(2, 9, 1.0f, true);
            ST1860.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1860.this.Step.start(1, "Chijimi");
            ST1860.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1860.this.EXP0.kickEnepc(4, 0);
            ST1860.this.EXP0.kickEnepc(4, 2);
            ST1860.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1860.this.cam0.setMode(0);
            Runtime.setFlags(3074, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1860.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1860.this.Step.setTranslate(ST1860.this.Step.px - 0.016666668f, ST1860.this.Step.py, ST1860.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

