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
import xeno.map.MC_DYU10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1805
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU10_PRJ {
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
    Enepc enemy6;
    Enepc EXP0;
    Enepc EXP1;
    Unit unit1;
    Unit DenDen;
    Unit Pon;
    Unit Step;
    Unit RunRun;
    Unit monitor1;
    Unit monitor2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    Effect E08;
    Effect E09;
    Effect E10;
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
    String[] GUIDE_00 = new String[]{"This line is currently postponing all transportation except to the Residential Area in order to secure an evacuation route.", "/[waitkey(1)]/[clear()]", "Would you like to go to the Residential Area?", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_01 = new String[]{"Going to Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] P1_00 = new String[]{"People are not handling the realization that Gnosis exist very well.", "/[waitkey(64)]/[close()]"};
    String[] P1_01 = new String[]{"I guess the fact that we can stay so calm at a time like\nthis shows that we are different from humans after all.", "/[waitkey(64)]/[close()]"};
    String[] P2_00 = new String[]{"I never thought anything could scare me!", "/[waitkey(1)]/[clear()]", "But...but man, what are those monsters?!", "/[waitkey(64)]/[close()]"};
    String[] P2_01 = new String[]{"Do I want to go back to the Foundation? You've got to be joking!", "/[waitkey(1)]/[clear()]", "Like hell I'm going back to where those monsters are!!", "/[waitkey(64)]/[close()]"};
    String[] P3_00 = new String[]{"After a huge incident like this, we can't hide it from the civilians anymore.", "/[waitkey(64)]/[close()]"};
    String[] P3_01 = new String[]{"I worry about what happened,\n", "/[waitkey(1)]/[clear()]", "but what worries me more is what could potentially happen.", "/[waitkey(64)]/[close()]"};
    String[] P4_00 = new String[]{"I-I saw it! The people turned completely white right before my eyes.\n", "/[waitkey(1)]/[clear()]", "After that...a-after that...", "/[waitkey(64)]/[close()]"};
    String[] P4_01 = new String[]{"Your friend? How should I know?!", "/[waitkey(1)]/[clear()]", "I was barely able to escape myself!!", "/[waitkey(64)]/[close()]"};
    String[] P5_00 = new String[]{"The Foundation used to be so peaceful.", "/[waitkey(1)]/[clear()]", "I worry about the families that have become separated.", "/[waitkey(64)]/[close()]"};
    String[] P5_01 = new String[]{"I tried to get to my family, but the roads in the shuttle area were closed off, so...", "/[waitkey(64)]/[close()]"};
    String[] P6_00 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] P6_01 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] P7_00 = new String[]{"This area will be sealed off soon. Please hurry to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] P7_01 = new String[]{"Me?", "/[waitkey(1)]/[clear()]", "I plan to lead as many people as I can that are left in the Foundation to safety.", "/[waitkey(64)]/[close()]"};
    String[] P8_00 = new String[]{"Those bastards...t-they came in through the walls...", "/[waitkey(1)]/[clear()]", "and through the floors and ceilings! I had nowhere to run...and I...I...", "/[waitkey(64)]/[close()]"};
    String[] P8_01 = new String[]{"There's nowhere left to run!!", "/[waitkey(64)]/[close()]"};
    String[] ANO_00 = new String[]{"This shuttle is exclusively for direct flights to the Kukai Foundation.", "/[waitkey(1)]/[clear()]", "Would you like to go to the Foundation?", "/[waitkey(64)]/[close()]"};
    String[] ANO_01 = new String[]{"Understood.", "/[waitkey(1)]/[clear()]", "An evacuation advisory for the Foundation has been officially announced. Please be very careful during your stay.", "/[waitkey(64)]/[close()]"};
    String[] ANO_02 = new String[]{"Understood. We look forward to serving you again soon.", "/[waitkey(64)]/[close()]"};
    String[] SHI_11 = new String[]{"/[label(Shion)]", "I'm worried about MOMO.\n", "/[waitkey(1)]/[clear()]", "We better find her before we go back to the Elsa...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_11 = new String[]{"/[label(Ziggy)]", "...\n", "I am worried about MOMO.", "/[waitkey(64)]/[close()]"};
    String[] CHA_11 = new String[]{"/[label(chaos)]", "If we don't bring MOMO back with us, there's no knowing what the Captain might say...", "/[waitkey(64)]/[close()]"};
    String[] JR_11 = new String[]{"/[label(Jr.)]", "Man, what a pain.", "/[waitkey(1)]/[clear()]", "Where the heck did she go?", "/[waitkey(64)]/[close()]"};
    String[] KOS_11 = new String[]{"/[label(KOS-MOS)]", "The search for MOMO takes priority...", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Dock Area Station\n", "Boarding area for direct flights to the Foundation'", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "I've been worried about MOMO since we left her on the Foundation.", "/[waitkey(1)]/[clear()]", "Maybe it'd be better if we went to get her.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "MOMO...should we really have left her alone?", "/[waitkey(1)]/[clear()]", "Perhaps we should see how she's doing.", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "MOMO...", "/[waitkey(1)]/[clear()]", "I wonder if she's okay on her own? We should go get her!", "/[waitkey(64)]/[close()]"};
    String[] KOS_00 = new String[]{"/[label(KOS-MOS)]", "MOMO is still on the Foundation.", "/[waitkey(1)]/[clear()]", "Strategically, it would be best to go rescue MOMO before the situation deteriorates.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "Damn!!", "/[waitkey(1)]/[clear()]", "It was probably a bad call to leave MOMO on Foundation.", "/[waitkey(64)]/[close()]", "We should go get her!", "/[waitkey(64)]/[close()]"};

    ST1805() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-9.155f, 5.199f, 16.156f);
        this.camEV.setRotate(-17.549f, 17.735f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -9.155f, 5.199f, 16.156f, 120.0f, 5.006f, 4.271f, 8.578f, 240.0f, 0.858f, 3.343f, -1.0f};
        float[] fArray2 = new float[12];
        fArray2[0] = 1.0f;
        fArray2[1] = -17.549f;
        fArray2[2] = 17.735f;
        fArray2[4] = 120.0f;
        fArray2[5] = -11.869f;
        fArray2[6] = 53.867f;
        fArray2[8] = 240.0f;
        fArray2[9] = -6.189f;
        fArray2[10] = 90.0f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 240);
        this.camEV.rotateSPL(fArray3, 1, 3, 240);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.705f, 3.623f, -6.575f);
        this.camEV.setRotate(3.771f, -26.879f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.035f, 2.0f, 0.835f);
        this.camEV.setRotate(-1.87f, 0.0f, 0.0f);
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
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.disable(524288);
                            this.cam0.setMode(-1);
                            this.doorB.DoorOpen();
                            this.Step.start(1, "Nobi");
                            Sound.effectPlay(196741);
                            this.EXP0.kickEnepc(4, 1);
                            this.EXP0.kickEnepc(0, 1);
                            System.sleep(45);
                            this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(60, -13.0f, -1.0f, true);
                            System.sleep(35);
                            this.doorB.DoorClose();
                            this.Step.start(1, "Chijimi");
                            this.EXP0.kickEnepc(0, 2);
                            System.sleep(45);
                            this.EXP0.kickEnepc(3, 2, 45, 45, 1, 100);
                            this.player.setTranslate(-100.0f, -0.0f, -1.0f);
                            Runtime.disable(65536);
                            int n3 = 0;
                            int n4 = 90;
                            float f = 20.0f / (float) n4 / (float) n4;
                            while (n3 < n4) {
                                float f2 = 2.0f * f * (float) n3;
                                this.EXP0.getTranslate();
                                this.EXP0.setTranslate(this.EXP0.px, this.EXP0.py, this.EXP0.pz + f2);
                                if (n3 == n4 - 30) {
                                    this.fade.call(0);
                                }
                                ++n3;
                                System.sleep(1);
                            }
                            this.button1_flg = 0;
                            Runtime.setPlayerControl(true);
                            Runtime.setFlags(3072, 1, 1);
                            Runtime.jumpCF(1845, 0);
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
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px - 0.3f, this.player.py, this.player.pz);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (Runtime.getLeader() == 1) {
                        this.win.print(this.SHI_11, 0);
                    } else if (Runtime.getLeader() == 6) {
                        this.win.print(this.ZIG_11, 0);
                    } else if (Runtime.getLeader() == 3) {
                        this.win.print(this.CHA_11, 0);
                    } else if (Runtime.getLeader() == 2) {
                        this.win.print(this.KOS_11, 0);
                    } else {
                        this.win.print(this.JR_11, 0);
                    }
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ANO_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.ANO_01, 0);
                            System.waitFor(this.win);
                            Sound.streamPlay(1195008, 48000);
                            this.RunRun.start(1, "Evt3");
                            return;
                        }
                        default: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.ANO_02, 0);
                            System.waitFor(this.win);
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
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3163, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (Runtime.getLeader() == 1) {
                        this.win.print(this.SHI_00, 0);
                    } else if (Runtime.getLeader() == 6) {
                        this.win.print(this.ZIG_00, 0);
                    } else if (Runtime.getLeader() == 3) {
                        this.win.print(this.CHA_00, 0);
                    } else if (Runtime.getLeader() == 2) {
                        this.win.print(this.KOS_00, 0);
                    } else {
                        this.win.print(this.JUN_00, 0);
                    }
                    System.waitFor(this.win);
                    Runtime.setFlags(3162, 1, 1);
                    Runtime.disable(65536);
                    System.sleep(30);
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(60, -6.0f, -1.0f, true);
                    System.sleep(65);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 5) return;
        switch (n) {
            case 100: {
                System.println("PPPPPPPPPPPPPPPPPPPPPPP");
                Runtime.setPlayerControl(false);
                this.keikoku_1();
                System.sleep(32);
                this.cam0.setMode(-1);
                this.EV_Camera03();
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
        if (this.npc1talked == 0) {
            window.print(this.P1_00, 0);
            ST1805.waitPage(window, 64);
            this.npc1talked = 1;
        } else {
            window.print(this.P1_01, 0);
            ST1805.waitPage(window, 64);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (this.npc2talked == 0) {
            window.print(this.P2_00, 0);
            ST1805.waitPage(window, 64);
            this.npc2talked = 1;
        } else {
            window.print(this.P2_01, 0);
            ST1805.waitPage(window, 64);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (this.npc3talked == 0) {
            window.print(this.P3_00, 0);
            ST1805.waitPage(window, 64);
            this.npc3talked = 1;
        } else {
            window.print(this.P3_01, 0);
            ST1805.waitPage(window, 64);
            this.npc3talked = 0;
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (this.npc4talked == 0) {
            window.print(this.P4_00, 0);
            ST1805.waitPage(window, 64);
            this.npc4talked = 1;
        } else {
            window.print(this.P4_01, 0);
            ST1805.waitPage(window, 64);
            this.npc4talked = 0;
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (this.npc5talked == 0) {
            window.print(this.P5_00, 0);
            ST1805.waitPage(window, 64);
            this.npc5talked = 1;
        } else {
            window.print(this.P5_01, 0);
            ST1805.waitPage(window, 64);
            this.npc5talked = 0;
        }
    }

    public void TalkNPC6(Enepc enepc, Window window) {
        if (this.npc6talked == 0) {
            window.print(this.P6_00, 0);
            ST1805.waitPage(window, 64);
            this.npc6talked = 1;
        } else {
            window.print(this.P6_01, 0);
            ST1805.waitPage(window, 64);
            this.npc6talked = 0;
        }
    }

    public void TalkNPC7(Enepc enepc, Window window) {
        if (this.npc7talked == 0) {
            window.print(this.P7_00, 0);
            ST1805.waitPage(window, 64);
            this.npc7talked = 1;
        } else {
            window.print(this.P7_01, 0);
            ST1805.waitPage(window, 64);
            this.npc7talked = 0;
        }
    }

    public void TalkNPC8(Enepc enepc, Window window) {
        if (this.npc8talked == 0) {
            window.print(this.P8_00, 0);
            ST1805.waitPage(window, 64);
            this.npc8talked = 8;
        } else {
            window.print(this.P8_01, 0);
            ST1805.waitPage(window, 64);
            this.npc8talked = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(521, 2);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(3190, 1) == 0) {
            Runtime.battleChangeParty(4);
            Runtime.setOutFriend(4);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 1);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 2);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(3190, 1, 1);
        }
        this.teiten1 = new Uwamono(28690, -2.3f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, -2.3f, 2.56f, -14.0f, 0.0f);
        this.teiten2.SetBgm(196621);
        this.teiten3 = new Uwamono(28690, -11.0f, 0.0f, -1.0f, 0.0f);
        this.teiten3.SetBgm(196622);
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
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 14.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, -6.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 10.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 10.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -10.0f, 0.0f, 14.0f, 35.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFPedestal(13, 11.814281f, 8.0f, 11.174469f, 35.0f, -25.325975f, 350.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(14, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 10, 4, -7.167f, 0.0f, -9.437f, 0.0f);
        this.npc2 = new NPC_NORMAL(1546, 12, 0, 10, 14, -3.14f, 0.0f, -4.769f, 0.0f);
        this.npc3 = new NPC_NORMAL(1025, 13, 0, 10, 15, -1.913f, 0.0f, -0.823f, 0.0f);
        this.npc4 = new NPC_NORMAL(1561, 14, 0, 3, 22, -7.868f, 0.0f, 3.1892f, 45.0f);
        this.npc5 = new NPC_NORMAL(1540, 15, 0, 10, 15, -0.286f, 0.0f, 5.0621f, 0.0f);
        this.npc6 = new NPC_NORMAL(1609, 16, 0, 3, 19, -7.959f, 0.0f, 7.8489f, -195.0f);
        this.npc7 = new NPC_NORMAL(783, 17, 0, 3, 17, -0.155f, 2.5499f, -15.97f, -15.0f);
        this.npc8 = new NPC_NORMAL(1558, 18, 0, 10, 18, 5.9191f, 0.0f, 3.1093f, 0.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc3.talkto("TalkNPC3");
        this.npc4.talkto("TalkNPC4");
        this.npc5.talkto("TalkNPC5");
        this.npc6.talkto("TalkNPC6");
        this.npc7.talkto("TalkNPC7");
        this.npc8.talkto("TalkNPC8");
        this.npc1.disableDTKFlag(131074);
        this.npc2.disableDTKFlag(131074);
        this.npc3.disableDTKFlag(131074);
        this.npc4.disableDTKFlag(131075);
        this.npc4.setMotion(0, 2);
        this.npc5.disableDTKFlag(131074);
        this.npc6.disableDTKFlag(131075);
        this.npc6.setMotion(0, 1);
        this.npc7.disableDTKFlag(131075);
        this.npc8.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(8);
        this.npc2.enableDTKFlag(8);
        this.npc3.enableDTKFlag(8);
        this.npc4.enableDTKFlag(8);
        this.npc5.enableDTKFlag(8);
        this.npc6.enableDTKFlag(8);
        this.npc7.enableDTKFlag(8);
        this.npc8.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc2.enableDTKFlag(4);
        this.npc3.enableDTKFlag(4);
        this.npc4.enableDTKFlag(4);
        this.npc5.enableDTKFlag(4);
        this.npc7.enableDTKFlag(4);
        this.npc8.enableDTKFlag(4);
        this.npc4.setInvalidID(1);
        this.npc6.setInvalidID(1);
        this.npc8.kickEnepc(19, 1, 0, 430, 1);
        this.npc8.kickEnepc(19, 2, 0, 710, 1);
        this.EXP0 = new Enepc();
        this.EXP0.init(20492, 5, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.id = 31;
        this.EXP0.setParams(0, 0, 31, 5);
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.EXP1 = new Enepc();
        this.EXP1.init(20621, 30, 4.5f, 4.5f, -21.6f, 90.0f);
        this.EXP1.id = 32;
        this.EXP1.setParams(0, 0, 32, 30);
        this.EXP1.setInvalidID(1);
        this.EXP1.dispRadar(false);
        if (Runtime.getFlags(3108, 1) != 1) {
            this.EXP1.setMotion(0, 2);
        } else {
            this.EXP1.setMotion(0, 3);
        }
        this.doorA = new Uwamono(109, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(105, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        this.doorC = new Uwamono(108, 40, '\u0001');
        this.doorC.SetDoorType('\u0001');
        if (Runtime.getFlags(3069, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt");
        }
        if (Runtime.getFlags(3108, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt2");
        }
        this.RunRun = new Mapunits();
        this.RunRun.mapUnit(11);
        this.RunRun.start(4, null);
        this.Step = new Mapunits();
        this.Step.mapUnit(326);
        this.Step.start(4, null);
        this.Step.setTranslate(1.0f, -0.01f, 0.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, -7.013f, 2.25f, -3.0f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, -7.013f, 2.25f, -3.01f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18019, 0, 256, 130);
        this.monitor2.setArgs(2, 100, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        Runtime.setFlags(362, 1, 1);
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
            ST1805.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1805.this.Step.setTranslate(ST1805.this.Step.px + 0.016666668f, ST1805.this.Step.py, ST1805.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Down() {
            int n = 0;
            ST1805.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 120) {
                    ST1805.this.EXP1.setTranslate(ST1805.this.EXP1.px, ST1805.this.EXP1.py - 0.045833334f, ST1805.this.EXP1.pz);
                }
                if (n == 149) {
                    ST1805.this.player.setTranslate(6.0f, 4.95f, -22.0f);
                }
                if (n == 150) {
                    ST1805.this.EXP1.kickEnepc(0, 1);
                }
                if (n == 195) break;
                ++n;
                System.sleep(1);
            }
            ST1805.this.EXP1.kickEnepc(1, 2);
        }

        void Evt() {
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1805.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1805.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1805.this.EV_Camera01();
            System.sleep(1);
            ST1805.this.EXP0.kickEnepc(4, 1);
            ST1805.this.win = Window.create();
            ST1805.this.win.setSize(4, 45);
            ST1805.this.win.setLocation(15, 305);
            ST1805.this.win.print("Dock");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1805.this.EXP0.getTranslate();
                ST1805.this.EXP0.setTranslate(ST1805.this.EXP0.px, ST1805.this.EXP0.py, ST1805.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1805.this.win.close();
            ST1805.this.player.setLocation(1, 2);
            ST1805.this.EXP0.kickEnepc(0, 1);
            ST1805.this.Step.start(1, "Nobi");
            ST1805.this.doorB.DoorOpen();
            System.sleep(45);
            ST1805.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1805.this.player.mtn(2, 9, 1.0f, true);
            ST1805.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1805.this.Step.start(1, "Chijimi");
            ST1805.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1805.this.EXP0.kickEnepc(4, 0);
            ST1805.this.EXP0.kickEnepc(4, 2);
            ST1805.this.doorB.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1805.this.cam0.setMode(0);
            Runtime.setFlags(3069, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt2() {
            Runtime.disable(524288);
            ST1805.this.player.setTranslate(100.0f, 100.0f, 100.0f);
            Runtime.setPlayerControl(false);
            ST1805.this.EXP1.setTranslate(4.5f, 10.0f, -21.6f);
            ST1805.this.cam0.setMode(-1);
            ST1805.this.EV_Camera02();
            Sound.streamPlay(1195009, 48000);
            System.sleep(30);
            ST1805.this.EXP1.kickEnepc(4, 1);
            ST1805.this.EXP1.getTranslate();
            this.Down();
            ST1805.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1805.this.player.mtn(2, 9, 1.0f, true);
            ST1805.this.player.move(60, 6.0f, -20.0f, true);
            System.sleep(60);
            Runtime.disable(65536);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt3() {
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1805.this.cam0.setMode(-1);
            ST1805.this.EV_Camera02();
            System.sleep(30);
            ST1805.this.EXP1.kickEnepc(4, 1);
            ST1805.this.EXP1.getTranslate();
            this.Up();
            System.sleep(120);
            Runtime.disable(65536);
            ST1805.this.cam0.setMode(0);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1805.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1805.this.Step.setTranslate(ST1805.this.Step.px - 0.016666668f, ST1805.this.Step.py, ST1805.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Up() {
            int n = 0;
            ST1805.this.Step.getTranslate();
            while (true) {
                if (n == 0) {
                    ST1805.this.EXP1.kickEnepc(0, 0);
                }
                if (n == 45) {
                    ST1805.this.EXP1.kickEnepc(1, 3);
                }
                if (n == 75) {
                    ST1805.this.player.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n >= 75 && n < 195) {
                    ST1805.this.EXP1.setTranslate(ST1805.this.EXP1.px, ST1805.this.EXP1.py + 0.083333336f, ST1805.this.EXP1.pz);
                }
                if (n == 195) break;
                ++n;
                System.sleep(1);
            }
            ST1805.this.EXP1.kickEnepc(4, 0);
            Runtime.setPlayerControl(true);
            ST1805.this.fade.call(0);
            System.sleep(30);
            Runtime.setFlags(7138, 1, 1);
            Runtime.jumpCF(2160, 2);
        }
    }
}

