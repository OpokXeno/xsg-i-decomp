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
import xeno.map.MC_DYU15_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1851
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU15_PRJ {
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
    Uwamono Col;
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
    String[] GUIDE_01 = new String[]{"Going to the Isolation Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Bridge.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Hangar.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Security Alert Level AAA. Entry beyond this point is prohibited.", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Park Station'", "/[waitkey(64)]/[close()]"};

    ST1851() {
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
        this.camEV.setTranslate(-7.013f, 1.839f, 0.986f);
        this.camEV.setRotate(0.21f, 0.0f, 0.0f);
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
                    this.menu.addItem("Isolation Area\nDock\nResidential Area\nBridge\nHangar\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1831);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1801);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1841);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1821);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1861);
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
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
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
        if (n2 != 2) return;
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
                Runtime.jumpCF(1771, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1771, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 0.5f, 0.0f, -9.0f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, 0.5f, 4.0f, -16.0f, 0.0f);
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
        Stage.setVisible(1, false);
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
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.enemy1 = new Enepc();
        this.enemy1.init(18179, 5, -8.0f, 0.0f, 2.5f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[16];
        fArray[0] = -8.0f;
        fArray[2] = 2.5f;
        fArray[3] = 1.0f;
        fArray[4] = -8.0f;
        fArray[6] = 3.5f;
        fArray[7] = 2.0f;
        fArray[8] = -8.0f;
        fArray[10] = 4.5f;
        fArray[11] = 3.0f;
        fArray[12] = -8.0f;
        fArray[14] = 5.5f;
        fArray[15] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 3, 1, 5, fArray2);
        float[] fArray3 = new float[12];
        fArray3[0] = -8.0f;
        fArray3[2] = 2.5f;
        fArray3[3] = -8.0f;
        fArray3[5] = 3.5f;
        fArray3[6] = -8.0f;
        fArray3[8] = 4.5f;
        fArray3[9] = -8.0f;
        fArray3[11] = 5.5f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(18179, 5, 0.0f, 0.0f, -3.5f, -45.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[16];
        fArray5[2] = -3.5f;
        fArray5[3] = 1.0f;
        fArray5[4] = -4.0f;
        fArray5[6] = -3.5f;
        fArray5[7] = 2.0f;
        fArray5[8] = -5.0f;
        fArray5[10] = -4.5f;
        fArray5[11] = 3.0f;
        fArray5[12] = -5.0f;
        fArray5[14] = -8.0f;
        fArray5[15] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 2, 2, 5, fArray6);
        this.enemy3 = new Enepc();
        this.enemy3.init(18179, 5, -2.5f, 0.0f, 1.0f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[12];
        fArray7[0] = -2.5f;
        fArray7[2] = 1.0f;
        fArray7[3] = 1.0f;
        fArray7[4] = 2.5f;
        fArray7[6] = 1.0f;
        fArray7[7] = 2.0f;
        fArray7[8] = 2.5f;
        fArray7[10] = 5.0f;
        fArray7[11] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(1, 3, 3, 5, fArray8);
        float[] fArray9 = new float[15];
        fArray9[0] = -2.5f;
        fArray9[2] = 1.0f;
        fArray9[3] = -1.5f;
        fArray9[5] = 1.0f;
        fArray9[6] = -0.5f;
        fArray9[8] = 1.0f;
        fArray9[9] = 1.5f;
        fArray9[11] = 1.0f;
        fArray9[12] = 2.5f;
        fArray9[14] = 1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(fArray10);
        this.EXP0 = new NPC_NORMAL(20492, 1, 0, 0, 3, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.talkto("TalkNPC1");
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.doorA = new Uwamono(0, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.Col = new Uwamono(28672, -0.5f, 0.0f, -8.0f, 0.0f);
        this.Col.SetSize(8.0f, 1.0f, 1.0f);
        if (Runtime.getFlags(3073, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt");
        } else {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "algo");
        }
        this.Step = new Mapunits();
        this.Step.mapUnit(4);
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
        this.monitor2.setArgs(1, 18017, 0, 256, 130);
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
            ST1851.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1851.this.Step.setTranslate(ST1851.this.Step.px + 0.016666668f, ST1851.this.Step.py, ST1851.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt() {
            ST1851.this.off();
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1851.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1851.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1851.this.EV_Camera01();
            System.sleep(1);
            ST1851.this.EXP0.kickEnepc(4, 1);
            ST1851.this.win = Window.create();
            ST1851.this.win.setSize(4, 45);
            ST1851.this.win.setLocation(15, 305);
            ST1851.this.win.print("Park");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1851.this.EXP0.getTranslate();
                ST1851.this.EXP0.setTranslate(ST1851.this.EXP0.px, ST1851.this.EXP0.py, ST1851.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1851.this.win.close();
            ST1851.this.player.setLocation(1, 3);
            ST1851.this.EXP0.kickEnepc(0, 1);
            ST1851.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1851.this.doorA.DoorOpen();
            System.sleep(15);
            ST1851.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1851.this.player.mtn(2, 9, 1.0f, true);
            ST1851.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1851.this.Step.start(1, "Chijimi");
            ST1851.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1851.this.EXP0.kickEnepc(4, 0);
            ST1851.this.EXP0.kickEnepc(4, 2);
            ST1851.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1851.this.cam0.setMode(0);
            Runtime.setFlags(3073, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
            ST1851.this.on();
        }

        void Nobi() {
            int n = 0;
            ST1851.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1851.this.Step.setTranslate(ST1851.this.Step.px - 0.016666668f, ST1851.this.Step.py, ST1851.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void algo() {
            ST1851.this.EXP0.kickEnepc(0, 2);
            int n = 0;
            while (n < 44) {
                System.sleep(1);
                ++n;
            }
            ST1851.this.EXP0.kickEnepc(4, 2);
        }
    }
}

