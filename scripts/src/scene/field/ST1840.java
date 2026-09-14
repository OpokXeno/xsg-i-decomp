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

class ST1840
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
    int npc22talked = 0;
    int button1_flg = 0;
    int shishi = 0;
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
    String[] POL_00 = new String[]{"Phew, I'm finally done. Now, guess I'll take a shower and have a drink!", "/[waitkey(1)]/[clear()]", "Policemen are human too -- we enjoy a drink or two after work!", "/[waitkey(64)]/[close()]"};
    String[] POL_S1 = new String[]{"A girl? I don't know. She hasn't come here. If she had, I'd definitely remember!", "/[waitkey(1)]/[clear()]", "It's rare to see a girl walking around on this ship!", "/[waitkey(64)]/[close()]"};
    String[] POL_S2 = new String[]{"Well, I'll let you know if I hear anything!", "/[waitkey(64)]/[close()]"};
    String[] CREW_00 = new String[]{"That casino is great!", "/[waitkey(1)]/[clear()]", "It helps me forget about my exhaustion from work!", "/[waitkey(64)]/[close()]"};
    String[] CREW_01 = new String[]{"But is there actually any cheating going on?", "/[waitkey(64)]/[close()]"};
    String[] CREW_02 = new String[]{"Oh, are you done with work for the day?", "/[waitkey(1)]/[clear()]", "Lucky you! I'm on my way to my shift now. I have to work hard to make up for what I lost at the casino yesterday.", "/[waitkey(64)]/[close()]"};
    String[] CREW_03 = new String[]{"Little Master went so far as to build a casino on the ship. I don't know whether to call it luxurious or what, but I think he is an incredible person.", "/[waitkey(64)]/[close()]"};
    String[] CREW_04 = new String[]{"Well, thanks to him, we can have fun and enjoy ourselves.", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_00 = new String[]{"Shoot! I was so engrossed in gambling, my ship left port.", "/[waitkey(1)]/[clear()]", "I can't get back to the Foundation by myself.", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_01 = new String[]{"Oh well, guess I'll just play some more!!", "/[waitkey(64)]/[close()]"};
    String[] K01_00 = new String[]{"Man, was I arrested?! I'm a police officer.", "/[waitkey(1)]/[clear()]", "Does Federation law allow for the arrest of innocent people?!", "/[waitkey(64)]/[close()]"};
    String[] K02_00 = new String[]{"Allen? Oh, the guy wearing the Vector uniform!", "/[waitkey(1)]/[clear()]", "I saw that guy running towards the launch area, crying.", "/[waitkey(64)]/[close()]"};
    String[] K02_01 = new String[]{"I remember him really well because the way he was running was pretty pathetic.", "/[waitkey(64)]/[close()]"};
    String[] K03_00 = new String[]{"Man, that pisses me off! I was on a roll and then those soldiers had to come in and get in the way!", "/[waitkey(1)]/[clear()]", "You know what that means? That means I have to go back to the casino and play all over again!", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Residential Area Station'", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1840() {
    }

    void Departure(int n) {
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.Step.start(1, "Nobi");
        Sound.effectPlay(196741);
        this.EXP0.kickEnepc(4, 1);
        this.EXP0.kickEnepc(0, 1);
        Sound.effectPlay(196745);
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
                            this.Departure(1820);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1860);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1850);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1830);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1800);
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
                    }
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
            return;
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
            }
            return;
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3168, 1) == 0 && Runtime.getFlags(346, 1) == 1 && Runtime.getFlags(347, 1) == 0) {
                    if (Runtime.mailReplyCheck(61) == 2) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.mailArriveSet(67);
                        Runtime.setFlags(3168, 1, 1);
                        System.waitFor(this.win);
                        System.sleep(15);
                        Runtime.setPlayerControl(true);
                        Runtime.mailExec(1);
                    } else if (Runtime.mailReplyCheck(61) == 1 || Runtime.mailReplyCheck(61) == 3) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.mailArriveSet(68);
                        Runtime.setFlags(3168, 1, 1);
                        System.waitFor(this.win);
                        System.sleep(15);
                        Runtime.setPlayerControl(true);
                        Runtime.mailExec(1);
                    }
                }
                if (Runtime.getFlags(304, 1) != 1) return;
                if (Runtime.getFlags(310, 1) != 0) return;
                if (Runtime.getFlags(7057, 2) != 2) return;
                if (Runtime.getFlags(3177, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAIL1, 0);
                Runtime.mailArriveSet(56);
                Runtime.setFlags(3177, 1, 1);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Read email\nDon't read email");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        window.print(this.POL_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        window.print(this.K01_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC1s(Enepc enepc, Window window) {
        if (this.shishi == 0) {
            window.print(this.POL_S1, 0);
            System.waitFor(window);
            this.shishi = 1;
        } else {
            window.print(this.POL_S2, 0);
            System.waitFor(window);
            this.shishi = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2btalked == 0) {
                window.print(this.CREW_00, 0);
                System.waitFor(window);
                this.npc2btalked = 1;
            } else {
                window.print(this.CREW_01, 0);
                System.waitFor(window);
                this.npc2btalked = 0;
            }
        } else if (this.npc2talked == 0) {
            window.print(this.CREW_02, 0);
            System.waitFor(window);
            this.npc2talked = 1;
        } else if (this.npc2talked == 1) {
            window.print(this.CREW_03, 0);
            System.waitFor(window);
            this.npc2talked = 2;
        } else {
            window.print(this.CREW_04, 0);
            System.waitFor(window);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (this.npc22talked == 0) {
            window.print(this.K02_00, 0);
            System.waitFor(window);
            this.npc22talked = 1;
        } else {
            window.print(this.K02_01, 0);
            System.waitFor(window);
            this.npc22talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (this.npc3talked == 0) {
            window.print(this.PEOPLE_00, 0);
            System.waitFor(window);
            this.npc3talked = 1;
        } else {
            window.print(this.PEOPLE_01, 0);
            System.waitFor(window);
            this.npc3talked = 0;
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        window.print(this.K03_00, 0);
        System.waitFor(window);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1750, 5);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 11.0f, 0.0f, 1.0f, 0.0f);
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
        this.npc1 = new NPC_NORMAL(1288, 11, 0, 3, 5, -1.372f, 0.0f, -1.475f, 30.0f);
        this.npc2 = new NPC_NORMAL(1025, 12, 0, 2, 8, 4.811f, 0.0f, 4.107f, 0.0f);
        this.npc3 = new NPC_NORMAL(1541, 13, 0, 3, 9, 2.328f, 0.0f, 5.715f, 0.0f);
        if (Runtime.getFlags(346, 1) == 0) {
            if (Runtime.getFlags(304, 1) == 0) {
                this.npc1.talkto("TalkNPC1s");
            } else {
                this.npc1.talkto("TalkNPC1");
            }
            this.npc2.talkto("TalkNPC2");
            this.npc3.talkto("TalkNPC3");
        } else {
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
            this.npc3.talkto("TalkNPC3a");
        }
        this.npc1.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(8);
        this.npc2.enableDTKFlag(8);
        this.npc3.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc2.enableDTKFlag(4);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 4);
        this.npc3.setInvalidID(1);
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
            ST1840.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1840.this.Step.setTranslate(ST1840.this.Step.px - 0.016666668f, ST1840.this.Step.py, ST1840.this.Step.pz);
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
            ST1840.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1840.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1840.this.EV_Camera01();
            System.sleep(1);
            ST1840.this.EXP0.kickEnepc(4, 1);
            ST1840.this.win = Window.create();
            ST1840.this.win.setSize(4, 45);
            ST1840.this.win.setLocation(15, 305);
            ST1840.this.win.print("Residential Area");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1840.this.EXP0.getTranslate();
                ST1840.this.EXP0.setTranslate(ST1840.this.EXP0.px, ST1840.this.EXP0.py, ST1840.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1840.this.win.close();
            ST1840.this.player.setLocation(1, 2);
            ST1840.this.EXP0.kickEnepc(0, 1);
            ST1840.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1840.this.doorA.DoorOpen();
            System.sleep(15);
            ST1840.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1840.this.player.mtn(2, 9, 1.0f, true);
            ST1840.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1840.this.Step.start(1, "Chijimi");
            ST1840.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1840.this.EXP0.kickEnepc(4, 0);
            ST1840.this.EXP0.kickEnepc(4, 2);
            ST1840.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1840.this.cam0.setMode(0);
            Runtime.setFlags(3072, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1840.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1840.this.Step.setTranslate(ST1840.this.Step.px + 0.016666668f, ST1840.this.Step.py, ST1840.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

