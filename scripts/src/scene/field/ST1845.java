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

class ST1845
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
    String[] GUIDE_00 = new String[]{"This line is currently postponing all travel except to the Dock in order to secure an evacuation route.", "/[waitkey(1)]/[clear()]", "Would you like to go to the Dock?", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] P1_00 = new String[]{"...Uhh, Mary! Hurry, run!!", "/[waitkey(64)]/[close()]"};
    String[] P1_01 = new String[]{"No, stop! Don't you lay a hand on her!!", "/[waitkey(64)]/[close()]"};
    String[] P2_00 = new String[]{"Have you seen this guy's kid?", "/[waitkey(64)]/[close()]"};
    String[] P2_01 = new String[]{"It seems they got separated at the Foundation launch area.", "/[waitkey(64)]/[close()]"};
    String[] P3_00 = new String[]{"Is anyone here hurt?!", "/[waitkey(64)]/[close()]"};
    String[] P3_01 = new String[]{"How could this happen?! Gnosis attacking the Foundation...", "/[waitkey(1)]/[clear()]", "This has never happened before!", "/[waitkey(64)]/[close()]"};
    String[] P4_00 = new String[]{"The people of the Foundation are evacuating to the Residential Area up ahead.", "/[waitkey(64)]/[close()]"};
    String[] P4_01 = new String[]{"If you're looking for someone, you might want to look over there.", "/[waitkey(64)]/[close()]"};
    String[] P5_00 = new String[]{"The Durandal may have the functionalities of a city, but we can't accommodate all the people of the Foundation...", "/[waitkey(1)]/[clear()]", "Then again, there's no way we could abandon the people in the city either...", "/[waitkey(64)]/[close()]"};
    String[] P5_01 = new String[]{"No! What's the point of being pessimistic now?!\n", "/[waitkey(1)]/[clear()]", "Is anyone hurt?!", "/[waitkey(64)]/[close()]"};
    String[] P6_00 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] P6_01 = new String[]{"Sorry...would you please leave me alone for a while?\n", "/[waitkey(64)]/[close()]"};
    String[] P7_00 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] P7_01 = new String[]{"The fortune I worked so hard to get is now all gone because of those damn monsters!!", "/[waitkey(64)]/[close()]"};
    String[] P8_00 = new String[]{"Currently, they're doing everything they can to analyze the situation at the bridge.", "/[waitkey(64)]/[close()]"};
    String[] P8_01 = new String[]{"I wanted to make a more direct impact on people rather than just analyzing data.", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "I've been worried about MOMO since we left her on the Foundation.", "/[waitkey(1)]/[clear()]", "Maybe it'd be better if we went to get her.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "MOMO...should we really have left her alone?", "/[waitkey(1)]/[clear()]", "Perhaps we should see how she's doing.", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "MOMO...", "/[waitkey(1)]/[clear()]", "I wonder if she's okay on her own? We should go get her!", "/[waitkey(64)]/[close()]"};
    String[] KOS_00 = new String[]{"/[label(KOS-MOS)]", "MOMO is still on the Foundation.", "/[waitkey(1)]/[clear()]", "Strategically, it would be best to go rescue MOMO before the situation deteriorates.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "Damn!!", "/[waitkey(1)]/[clear()]", "It was probably a bad call to leave MOMO on Foundation.", "/[waitkey(64)]/[close()]", "We should go get her!", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Residential Area Station'", "/[waitkey(64)]/[close()]"};

    ST1845() {
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
                            this.doorA.DoorOpen();
                            this.Step.start(1, "Nobi");
                            Sound.effectPlay(196741);
                            this.EXP0.kickEnepc(4, 1);
                            this.EXP0.kickEnepc(0, 1);
                            System.sleep(45);
                            this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(60, 13.0f, 1.0f, true);
                            System.sleep(35);
                            this.doorA.DoorClose();
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
                            Runtime.setFlags(3069, 1, 1);
                            Runtime.jumpCF(1805, 0);
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
        if (n2 == 1) {
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
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3163, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.player.getTranslate();
                this.player.setTranslate(this.player.px + 0.3f, this.player.py, this.player.pz);
                Runtime.enable(65536);
                this.player.mtn(28, 1, 1.0f, true);
                System.sleep(20);
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
                Runtime.setPlayerControl(true);
                System.println("DDDDDDDDDDDDDDDDD");
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (this.npc1talked == 0) {
            window.print(this.P1_00, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.P1_01, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (this.npc2talked == 0) {
            window.print(this.P2_00, 0);
            System.waitFor(window);
            this.npc2talked = 1;
        } else {
            window.print(this.P2_01, 0);
            System.waitFor(window);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (this.npc3talked == 0) {
            window.print(this.P3_00, 0);
            System.waitFor(window);
            this.npc3talked = 1;
        } else {
            window.print(this.P3_01, 0);
            System.waitFor(window);
            this.npc3talked = 0;
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (this.npc4talked == 0) {
            window.print(this.P4_00, 0);
            System.waitFor(window);
            this.npc4talked = 1;
        } else {
            window.print(this.P4_01, 0);
            System.waitFor(window);
            this.npc4talked = 0;
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (this.npc5talked == 0) {
            window.print(this.P5_00, 0);
            System.waitFor(window);
            this.npc5talked = 1;
        } else {
            window.print(this.P5_01, 0);
            System.waitFor(window);
            this.npc5talked = 0;
        }
    }

    public void TalkNPC6(Enepc enepc, Window window) {
        if (this.npc6talked == 0) {
            window.print(this.P6_00, 0);
            System.waitFor(window);
            this.npc6talked = 1;
        } else {
            window.print(this.P6_01, 0);
            System.waitFor(window);
            this.npc6talked = 0;
        }
    }

    public void TalkNPC7(Enepc enepc, Window window) {
        if (this.npc7talked == 0) {
            window.print(this.P7_00, 0);
            System.waitFor(window);
            this.npc7talked = 1;
        } else {
            window.print(this.P7_01, 0);
            System.waitFor(window);
            this.npc7talked = 0;
        }
    }

    public void TalkNPC8(Enepc enepc, Window window) {
        if (this.npc8talked == 0) {
            window.print(this.P8_00, 0);
            System.waitFor(window);
            this.npc8talked = 8;
        } else {
            window.print(this.P8_01, 0);
            System.waitFor(window);
            this.npc8talked = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.setFlags(363, 1, 1);
                Runtime.jumpEvent(3380);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 11.0f, 0.0f, 1.0f, 0.0f);
        this.teiten1.SetBgm(196622);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1528, 5.951f, 0.0f, 0.972f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
        this.E02 = new Effect(1528, 4.092f, 0.0f, -4.003f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E02.setScale(0.5f, 0.5f, 0.5f);
        this.E02.noAttach(false);
        this.E03 = new Effect(1528, -0.566f, 0.0f, -2.71f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
        this.E03.setScale(0.5f, 0.5f, 0.5f);
        this.E03.noAttach(false);
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
        this.npc1 = new NPC_NORMAL(1543, 11, 0, 8, 17, -0.566f, 0.0f, -2.71f, -90.0f);
        this.npc2 = new NPC_NORMAL(1546, 12, 0, 3, 12, 0.0452f, 0.0f, -2.0062f, 15.0f);
        this.npc3 = new NPC_NORMAL(1025, 13, 0, 2, 12, 6.574f, 0.0f, -2.308f, 90.0f);
        this.npc4 = new NPC_NORMAL(1288, 14, 0, 2, 13, 0.7594f, 0.0f, 1.7111f, 234.9f);
        this.npc5 = new NPC_NORMAL(783, 15, 0, 2, 16, 5.52f, 0.0f, 0.351f, 62.859f);
        this.npc6 = new NPC_NORMAL(780, 16, 0, 3, 14, 4.8968f, 0.0f, 5.7382f, 30.0f);
        this.npc7 = new NPC_NORMAL(1540, 17, 0, 3, 17, 2.0635f, 0.0f, 4.1143f, 192.8f);
        this.npc8 = new NPC_NORMAL(1028, 18, 0, 7, 20, 2.6261f, 0.0f, 3.2373f, -45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc3.talkto("TalkNPC3");
        this.npc4.talkto("TalkNPC4");
        this.npc5.talkto("TalkNPC5");
        this.npc6.talkto("TalkNPC6");
        this.npc7.talkto("TalkNPC7");
        this.npc8.talkto("TalkNPC8");
        this.npc1.disableDTKFlag(131075);
        this.npc1.setMotion(0, 2);
        this.npc2.disableDTKFlag(131075);
        this.npc2.setMotion(0, 9);
        this.npc3.disableDTKFlag(131074);
        this.npc3.setMotion(1, 3);
        this.npc4.disableDTKFlag(131074);
        this.npc5.disableDTKFlag(131074);
        this.npc6.disableDTKFlag(131075);
        this.npc6.setMotion(0, 14);
        this.npc7.disableDTKFlag(131075);
        this.npc7.setMotion(0, 5);
        this.npc8.disableDTKFlag(131075);
        this.npc7.setMotion(0, 5);
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
        this.npc6.enableDTKFlag(4);
        this.npc7.enableDTKFlag(4);
        this.npc8.enableDTKFlag(4);
        this.npc6.setInvalidID(1);
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
            ST1845.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1845.this.Step.setTranslate(ST1845.this.Step.px - 0.016666668f, ST1845.this.Step.py, ST1845.this.Step.pz);
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
            ST1845.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1845.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1845.this.EV_Camera01();
            System.sleep(1);
            ST1845.this.EXP0.kickEnepc(4, 1);
            ST1845.this.win = Window.create();
            ST1845.this.win.setSize(4, 45);
            ST1845.this.win.setLocation(15, 305);
            ST1845.this.win.print("Residential Area");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1845.this.EXP0.getTranslate();
                ST1845.this.EXP0.setTranslate(ST1845.this.EXP0.px, ST1845.this.EXP0.py, ST1845.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1845.this.win.close();
            ST1845.this.player.setLocation(1, 2);
            ST1845.this.EXP0.kickEnepc(0, 1);
            ST1845.this.Step.start(1, "Nobi");
            ST1845.this.doorA.DoorOpen();
            System.sleep(45);
            ST1845.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1845.this.player.mtn(2, 9, 1.0f, true);
            ST1845.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1845.this.Step.start(1, "Chijimi");
            ST1845.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1845.this.EXP0.kickEnepc(4, 0);
            ST1845.this.EXP0.kickEnepc(4, 2);
            ST1845.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1845.this.cam0.setMode(0);
            Runtime.setFlags(3072, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1845.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1845.this.Step.setTranslate(ST1845.this.Step.px + 0.016666668f, ST1845.this.Step.py, ST1845.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

