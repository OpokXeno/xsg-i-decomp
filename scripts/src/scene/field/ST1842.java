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

class ST1842
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
    String[] K01_00 = new String[]{"Hey, no loitering around here!", "/[waitkey(1)]/[clear()]", "A whole lot of people are evacuating from the Foundation.", "/[waitkey(1)]/[clear()]", "Make sure you don't get in the way!", "/[waitkey(64)]/[close()]"};
    String[] K02_00 = new String[]{"It's a terrible situation down there right now! There are lots of injured people too. Please hurry and call the medical team!", "/[waitkey(64)]/[close()]"};
    String[] K03_00 = new String[]{"Man, why did this happen to us?! What did we ever do to the Gnosis?!", "/[waitkey(64)]/[close()]"};
    String[] U01_00 = new String[]{"Hey, no loitering around here!", "/[waitkey(1)]/[clear()]", "A whole lot of people are evacuating from the Foundation.", "/[waitkey(1)]/[clear()]", "Make sure you don't get in the way!", "/[waitkey(64)]/[close()]"};
    String[] U02_00 = new String[]{"It's a terrible situation down there right now! There are lots of injured people too. Please hurry and call the medical team!", "/[waitkey(64)]/[close()]"};
    String[] U03_00 = new String[]{"Man, why did this happen to us?! What did we ever do to the Gnosis?!", "/[waitkey(64)]/[close()]"};
    String[] T1_1 = new String[]{"/[label(Axle)]", "Huh? Oh, Little Captain.", "/[waitkey(1)]/[clear()]", "Leave the safety of the Durandal to us!!", "/[waitkey(64)]/[close()]"};
    String[] T1_2 = new String[]{"/[label(Axle)]", "I intend to stand here everyday from now on and nip criminals in the bud.", "/[waitkey(1)]/[clear()]", "What?! WE look like criminals?!", "/[waitkey(1)]/[clear()]", "You gotta be kidding me! I may not look like a security guard, but I'm fighting crime night and day!!", "/[waitkey(64)]/[close()]"};
    String[] T2_1 = new String[]{"/[label(Riggs)]", "Little Captain? Is something big gonna happen on the Durandal?", "/[waitkey(1)]/[clear()]", "Well, when I say big, I mean something that I can still handle.", "/[waitkey(64)]/[close()]"};
    String[] T2_2 = new String[]{"/[label(Riggs)]", "Yo! Don't do anything too flashy and scratch up the Durandal!", "/[waitkey(64)]/[close()]"};
    String[] T3_1 = new String[]{"It's great that there's more life to the ship ever since the people from the Foundation came here, but there seem to be more weirdos now too.", "/[waitkey(64)]/[close()]"};
    String[] T3_2 = new String[]{"Ma'am! You should go back to the Residential Area. You're family's probably worried about you.", "/[waitkey(64)]/[close()]"};
    String[] T4_1 = new String[]{"Little Master! Please do something about this old lady!", "/[waitkey(1)]/[clear()]", "I told her that she can't sell things here, but she won't listen to me!!", "/[waitkey(64)]/[close()]"};
    String[] T4_2 = new String[]{"Man, why is she trying to sell stuff when she has nothing to sell?", "/[waitkey(64)]/[close()]"};
    String[] T5_1 = new String[]{"/[label(McLane)]", "'lo!! It's great that we organized a security unit on this ship, but nothing big happens!", "/[waitkey(1)]/[clear()]", "But when an incident does happen, it's too big of an incident for us to handle!!", "/[waitkey(64)]/[close()]"};
    String[] T5_2 = new String[]{"/[label(McLane)]", "Hmm? What? An incident?!", "/[waitkey(1)]/[clear()]", "There's a lost old lady? Maybe you should leave those kinds of incidents to the crew members here.", "/[waitkey(64)]/[close()]"};
    String[] T6_1 = new String[]{"Little Master! I've recently come across some shady looking people claiming to be a security staff!", "/[waitkey(1)]/[clear()]", "They're actually being pretty helpful, but there's something scary about them!", "/[waitkey(64)]/[close()]"};
    String[] T6_2 = new String[]{"Oh! Did the security staff do anything to you?", "/[waitkey(1)]/[clear()]", "What? They delivered something you lost?", "/[waitkey(1)]/[clear()]", "Maybe they are good people after all.", "/[waitkey(64)]/[close()]"};
    String[] T7_1 = new String[]{"Oh, why am I here?", "/[waitkey(1)]/[clear()]", "Normally, I'd be at the cafe at the Foundation, enjoying a cup of afternoon tea...", "/[waitkey(64)]/[close()]"};
    String[] T7_2 = new String[]{"Waah!! I wanna go back to the Foundation!!", "/[waitkey(64)]/[close()]"};
    String[] T8_1 = new String[]{"Hey! Little guy, come over here...", "/[waitkey(1)]/[clear()]", "No, not woof. Meow, right?! Meow!", "/[waitkey(64)]/[close()]"};
    String[] T8_2 = new String[]{"Oh my! Are you all feeling faint from seeing my irresistible body?", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Residential Area Station'", "/[waitkey(64)]/[close()]"};

    ST1842() {
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
                            this.Departure(1822);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1862);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1852);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1832);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1802);
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
        if (n2 != 1) return;
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
            window.print(this.K01_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.K01_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T1_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T1_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K02_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.K02_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T2_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T2_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K03_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.K03_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T3_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T3_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T4_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T4_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T5_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T5_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC6a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T6_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T6_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC7a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T7_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T7_2, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC8a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.T8_1, 0);
            System.waitFor(window);
        } else {
            window.print(this.T8_2, 0);
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
                Runtime.jumpCF(1752, 5);
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
        if (Runtime.getFlags(373, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1288, 11, 0, 3, 27, -1.372f, 0.0f, -1.475f, 30.0f);
            this.npc2 = new NPC_NORMAL(1025, 12, 0, 2, 13, 4.811f, 0.0f, 4.107f, 0.0f);
            this.npc3 = new NPC_NORMAL(1541, 13, 0, 3, 14, 2.328f, 0.0f, 5.715f, 0.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
            this.npc3.talkto("TalkNPC3");
            this.npc1.disableDTKFlag(131074);
            this.npc2.disableDTKFlag(131072);
            this.npc3.disableDTKFlag(131075);
            this.npc1.enableDTKFlag(8);
            this.npc2.enableDTKFlag(8);
            this.npc3.enableDTKFlag(8);
            this.npc1.enableDTKFlag(4);
            this.npc2.enableDTKFlag(4);
            this.npc3.enableDTKFlag(4);
            this.npc3.setMotion(0, 9);
            this.npc3.setMotion(0, 4);
            this.npc3.setInvalidID(1);
        } else {
            this.npc1 = new NPC_NORMAL(1609, 11, 0, 3, 20, 0.9212f, 0.0f, -1.872f, -45.0f);
            this.npc2 = new NPC_NORMAL(1609, 12, 0, 3, 32, 0.0452f, 0.0f, -2.006f, 15.0f);
            this.npc3 = new NPC_NORMAL(1025, 13, 0, 3, 12, 5.7156f, 0.0f, -4.632f, 135.0f);
            this.npc4 = new NPC_NORMAL(1288, 14, 0, 3, 13, 7.4238f, 0.0f, -4.848f, -45.0f);
            this.npc5 = new NPC_NORMAL(1609, 15, 0, 3, 34, -0.623f, 0.0f, -0.977f, -62.85f);
            this.npc6 = new NPC_NORMAL(780, 16, 0, 2, 13, 1.0223f, 0.0f, 3.6315f, 30.0f);
            this.npc7 = new NPC_NORMAL(1570, 17, 0, 3, 31, 1.1042f, 0.0f, 5.9022f, -15.0f);
            this.npc8 = new NPC_NORMAL(1579, 18, 0, 3, 30, 6.5424f, 0.0f, -5.307f, 0.0f);
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
            this.npc3.talkto("TalkNPC3a");
            this.npc4.talkto("TalkNPC4a");
            this.npc5.talkto("TalkNPC5a");
            this.npc6.talkto("TalkNPC6a");
            this.npc7.talkto("TalkNPC7a");
            this.npc8.talkto("TalkNPC8a");
            this.npc1.disableDTKFlag(131075);
            this.npc1.setMotion(0, 6);
            this.npc2.disableDTKFlag(131075);
            this.npc2.setMotion(0, 27);
            this.npc3.disableDTKFlag(131074);
            this.npc3.setMotion(0, 9);
            this.npc4.disableDTKFlag(131074);
            this.npc4.setMotion(0, 9);
            this.npc5.disableDTKFlag(131075);
            this.npc5.setMotion(0, 30);
            this.npc6.disableDTKFlag(131072);
            this.npc7.disableDTKFlag(131075);
            this.npc7.setMotion(0, 7);
            this.npc8.disableDTKFlag(131074);
            this.npc8.setMotion(0, 9);
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
            this.npc7.setInvalidID(1);
            this.npc8.setInvalidID(1);
        }
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
            ST1842.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1842.this.Step.setTranslate(ST1842.this.Step.px - 0.016666668f, ST1842.this.Step.py, ST1842.this.Step.pz);
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
            ST1842.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1842.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1842.this.EV_Camera01();
            System.sleep(1);
            ST1842.this.EXP0.kickEnepc(4, 1);
            ST1842.this.win = Window.create();
            ST1842.this.win.setSize(4, 45);
            ST1842.this.win.setLocation(15, 305);
            ST1842.this.win.print("Residential Area");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1842.this.EXP0.getTranslate();
                ST1842.this.EXP0.setTranslate(ST1842.this.EXP0.px, ST1842.this.EXP0.py, ST1842.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1842.this.win.close();
            ST1842.this.player.setLocation(1, 2);
            ST1842.this.EXP0.kickEnepc(0, 1);
            ST1842.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1842.this.doorA.DoorOpen();
            System.sleep(15);
            ST1842.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1842.this.player.mtn(2, 9, 1.0f, true);
            ST1842.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1842.this.Step.start(1, "Chijimi");
            ST1842.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1842.this.EXP0.kickEnepc(4, 0);
            ST1842.this.EXP0.kickEnepc(4, 2);
            ST1842.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1842.this.cam0.setMode(0);
            Runtime.setFlags(3072, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1842.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1842.this.Step.setTranslate(ST1842.this.Step.px + 0.016666668f, ST1842.this.Step.py, ST1842.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

