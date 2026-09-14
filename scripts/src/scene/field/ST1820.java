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

class ST1820
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
    String[] SHI_00 = new String[]{"/[label(Shion)]", "I said some awful things to Allen that I shouldn't have. I better apologize to him in person.", "/[waitkey(64)]/[close()]"};
    String[] P01_00 = new String[]{"A girl with pink hair?", "/[waitkey(1)]/[clear()]", "I saw her board the train heading for the Park!", "/[waitkey(64)]/[close()]"};
    String[] P01_01 = new String[]{"Anyway, listen to this!", "/[waitkey(1)]/[clear()]", "I service this information board. But everyone keeps messing with it, so it breaks down!", "/[waitkey(1)]/[clear()]", "Well, I'm glad that it gets used so much, but still...", "/[waitkey(1)]/[clear()]", "Anyway, this is a big ship, so if you ever get lost, be sure to use it!!", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Bridge Station'", "/[waitkey(64)]/[close()]"};

    ST1820() {
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
                            this.Departure(1860);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1850);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1830);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1800);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1840);
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
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3067, 1) != 0) return;
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
                            System.sleep(45);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(30, -6.5f, -4.5f, true);
                            System.sleep(35);
                            this.player.rotY(15, 0.0f, true);
                            System.sleep(20);
                            this.doorB.DoorClose();
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(false);
                            Sound.effectPlay(196747);
                            this.ele0.setArgs(12, 13.0f);
                            System.sleep(200);
                            Runtime.setFlags(3068, 1, 1);
                            this.doorB.SetDoorType('\u0004');
                            Runtime.setFlags(6041, 1, 0);
                            Runtime.setFlags(6042, 1, 0);
                            Runtime.setFlags(6044, 1, 0);
                            Runtime.setFlags(6045, 1, 0);
                            Runtime.setFlags(6047, 1, 0);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.setPlayerControl(true);
                            Runtime.jumpCF(1710, 0);
                            return;
                        }
                        default: {
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(60, -6.5f, -0.5f, true);
                            System.sleep(60);
                            this.doorB.DoorClose();
                            Runtime.setFlags(3067, 1, 0);
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
        if (n2 == 2 || n2 != 3) return;
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
        if (this.npc1talked == 0) {
            window.print(this.P01_00, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.P01_01, 0);
            System.waitFor(window);
            this.npc1talked = 0;
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
        this.cam0.setCFAngle(3, -28.0f, 17.5f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        if (Runtime.getFlags(304, 1) == 0) {
            this.npc1 = new NPC_NORMAL(527, 11, 0, 4, 11, 4.43f, 0.0f, -2.394f, -92.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(131075);
            this.npc1.enableDTKFlag(8);
            this.npc1.enableDTKFlag(4);
            this.npc1.setInvalidID(1);
            this.npc1.setMotion(0, 1);
        }
        this.EXP0 = new NPC_NORMAL(20492, 1, 0, 0, 3, 13.0f, -1.0f, 1.0f, 180.0f);
        this.EXP0.talkto("TalkNPC1");
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.doorA = new Uwamono(106, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(75, 40, '\u0001');
        new Uwamono(74, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        if (Runtime.getFlags(3070, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt1");
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
            this.setShadow(1, 80);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Chijimi() {
            int n = 0;
            ST1820.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1820.this.Step.setTranslate(ST1820.this.Step.px - 0.016666668f, ST1820.this.Step.py, ST1820.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt1() {
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1820.this.EXP0.setTranslate(13.0f, -1.0f, -45.0f);
            ST1820.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1820.this.EV_Camera01();
            System.sleep(1);
            ST1820.this.EXP0.kickEnepc(4, 1);
            ST1820.this.win = Window.create();
            ST1820.this.win.setSize(4, 45);
            ST1820.this.win.setLocation(15, 305);
            ST1820.this.win.print("Bridge");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (0.6f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1820.this.EXP0.getTranslate();
                ST1820.this.EXP0.setTranslate(ST1820.this.EXP0.px, ST1820.this.EXP0.py, ST1820.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1820.this.win.close();
            ST1820.this.player.setLocation(1, 3);
            ST1820.this.EXP0.kickEnepc(0, 1);
            ST1820.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1820.this.doorA.DoorOpen();
            System.sleep(15);
            ST1820.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1820.this.player.mtn(2, 9, 1.0f, true);
            ST1820.this.player.move(90, 7.0f, 1.0f, true);
            System.sleep(30);
            ST1820.this.Step.start(1, "Chijimi");
            ST1820.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1820.this.EXP0.kickEnepc(4, 0);
            ST1820.this.EXP0.kickEnepc(4, 2);
            ST1820.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1820.this.cam0.setMode(0);
            Runtime.setFlags(3070, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt2() {
            ST1820.this.player.setRotateY(0.0f);
            Runtime.setPlayerControl(false);
            ST1820.this.player.setLocation(1, 2);
            Sound.effectPlay(196745);
            ST1820.this.ele0.setArgs(12, 0.0f);
            System.sleep(200);
            Sound.effectPlay(196746);
            System.sleep(40);
            ST1820.this.doorB.SetDoorType('\u0002');
            ST1820.this.doorB.DoorOpen();
            System.sleep(30);
            Runtime.enable(65536);
            ST1820.this.player.mtn(2, 9, 1.0f, true);
            ST1820.this.player.move(60, -6.5f, -0.5f, true);
            System.sleep(60);
            ST1820.this.doorB.DoorClose();
            ST1820.this.doorB.SetDoorType('\u0004');
            Runtime.setFlags(3067, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }

        void Nobi() {
            int n = 0;
            ST1820.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1820.this.Step.setTranslate(ST1820.this.Step.px + 0.016666668f, ST1820.this.Step.py, ST1820.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

