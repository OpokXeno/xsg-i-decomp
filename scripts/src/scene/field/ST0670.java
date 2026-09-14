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
import xeno.map.MC_ELS02B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0670
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS02B_PRJ {
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
    Unit unit1;
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
    int talkdoor = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    boolean kabeten = true;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono shopA;
    Uwamono saveA;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect eve04;
    Effect eve05;
    Effect eve06;
    Effect eve07;
    Effect eve08;
    Effect eve09;
    Effect eve10;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int button_flg = 0;
    int button2_flg = 0;
    int epass = 0;
    Uwamono tA;
    MAPUnit doorR1;
    MAPUnit doorL1;
    MAPUnit doorR2;
    MAPUnit doorL2;
    Unit ele;
    int elemove = 0;
    int elec = 0;
    int doormove = 0;
    boolean call = false;
    boolean syokika = true;
    Effect fade;
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
    Uwamono teiten10;
    Uwamono teiten11;
    int page;
    String[] KEIKOKU_01 = new String[]{"/[label(Warning)]", "'You crazy?!\n", " Opening this hatch during\n", " flight is strictly prohibited!!\n", " -The Great Matthews'", "/[waitkey(64)]/[close()]"};
    String[] hammar_0 = new String[]{"/[label(Hammer)]", "The intruders are at the catapult!!", "/[waitkey(1)]/[clear()]", "Leave the ship to us!!", "/[waitkey(64)]/[close()]"};
    String[] mathews_0 = new String[]{"/[label(Matthews)]", "Yo, sorry, but it's up to you, missy!! The catapult is the lowest level of the ship.", "/[waitkey(64)]/[close()]"};
    String[] tony_0 = new String[]{"/[label(Tony)]", "The enemies are Autonomous Combat Terminals, known as Auto-Techs! Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] Elv_1 = new String[]{"Would you like to go to B1?", "/[waitkey(64)]/[close()]"};
    String[] Elv_2 = new String[]{"Going to B1.", "/[waitkey(64)]/[close()]"};

    ST0670() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.4f, 5.447f, 3.137f);
        this.camEV.setRotate(-35.838f, 30.039f, 0.0f);
        this.camEV.setFov(45.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -4.898f, 1.383f, 3.343f, 37.0f, -4.265f, 1.383f, 1.79f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 2.82f;
        fArray2[2] = 67.838f;
        fArray2[4] = 37.0f;
        fArray2[5] = 2.82f;
        fArray2[6] = 67.838f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 37);
        this.camEV.rotateSPL(fArray3, 1, 3, 37);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.265f, 1.383f, 1.79f);
        this.camEV.setRotate(2.82f, 67.838f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    Stage.setVisible(19, true);
                    Stage.setVisible(6, false);
                    Runtime.setFlags(5001, 1, 0);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    this.doormove = 1;
                    System.println("エレベータ");
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
                    if (this.button2_flg == 1) {
                        return;
                    }
                    this.button2_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.KEIKOKU_01, 0);
                    System.waitFor(this.win);
                    this.button2_flg = 0;
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (this.epass == 0) {
                        System.println("**********************************");
                        Runtime.setPlayerControl(false);
                        this.cam0.setMode(-1);
                        this.EV_Camera02();
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(30, -12.0f, -0.5f, true);
                        System.sleep(30);
                        this.player.rotY(7, 90.0f, true);
                        System.sleep(7);
                        Runtime.disable(65536);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Elv_1, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.disable(65536);
                                this.EV_Camera02();
                                this.doormove = 2;
                                Sound.effectPlay(196713);
                                Runtime.setFlags(3039, 1, 1);
                                System.sleep(40);
                                this.ele.setArgs(12, -5.0f);
                                Sound.effectPlay(196744);
                                this.doorR2.start(1, "MoveD");
                                System.sleep(60);
                                this.fade.call(0);
                                System.sleep(30);
                                Runtime.setPlayerControl(true);
                                Runtime.jumpCF(66216, 2);
                                this.epass = 1;
                                return;
                            }
                        }
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(50, -7.9f, -0.5f, true);
                        System.sleep(50);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        this.cam0.setMode(0);
                        return;
                    }
                    this.epass = 1;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    if (this.epass == 0) return;
                    System.println("on!!");
                    this.epass = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 6) return;
        switch (n) {
            case 100: {
                Stage.setVisible(19, false);
                Stage.setVisible(6, true);
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66052, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(66132, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(66132, 2);
                break;
            }
            case 4: {
                Runtime.jumpCF(66142, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(66142, 2);
                break;
            }
            case 6: {
                Runtime.setFlags(5001, 1, 0);
                Runtime.jumpCF(66152, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(66072, 1);
                break;
            }
        }
    }

    void init() {
        Runtime.setFlags(141, 1, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        if (Runtime.getFlags(5001, 1) == 1) {
            Stage.setVisible(19, false);
            Stage.setVisible(6, true);
        } else if (Runtime.getFlags(5001, 1) == 0) {
            Stage.setVisible(19, true);
            Stage.setVisible(6, false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 9.2f, 0.3f, -28.0f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, 10.5f, 0.42f, 1.0f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.teiten3 = new Uwamono(28690, 10.5f, 0.42f, 3.3f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, 0.0f, -1.3f, -16.2f, 0.0f);
        this.teiten4.SetBgm(196614);
        this.teiten5 = new Uwamono(28690, 0.0f, -1.3f, -4.2f, 0.0f);
        this.teiten5.SetBgm(196614);
        this.teiten6 = new Uwamono(28690, 0.0f, -1.3f, 3.2f, 0.0f);
        this.teiten6.SetBgm(196614);
        this.teiten7 = new Uwamono(28690, 0.0f, -1.3f, 15.2f, 0.0f);
        this.teiten7.SetBgm(196614);
        this.teiten8 = new Uwamono(28690, -9.4f, 0.3f, -6.5f, 0.0f);
        this.teiten8.SetBgm(196615);
        this.teiten9 = new Uwamono(28690, 9.8f, 0.3f, 15.1f, 0.0f);
        this.teiten9.SetBgm(196634);
        this.teiten10 = new Uwamono(28690, 0.0f, 1.3f, 27.0f, 0.0f);
        this.teiten10.SetBgm(196617);
        this.teiten11 = new Uwamono(28690, -4.0f, 0.3f, -28.0f, 0.0f);
        this.teiten11.SetBgm(196612);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 0.0f, 5.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 5.0f, 0.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 7.8f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestal(5, 10.432f, 3.78f, -20.97f, 45.0f, -25.55f, 14.6f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(5, 1);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, 0.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, 0.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 0.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorA = new Uwamono(11, 40, '\u0001');
        this.doorA.SetDoorType('\u0001');
        this.doorB = new Uwamono(43, 40, '\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC = new Uwamono(12, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(13, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(14, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
        this.doorF = new Uwamono(15, 40, '\u0001');
        this.doorF.SetDoorType('\u0004');
        this.doorR1 = new Mapunits();
        this.doorR1.mapUnit(60);
        this.doorR1.start(4, null);
        this.doorR1.start(1, "automatic_door");
        this.doorL1 = new Mapunits();
        this.doorL1.mapUnit(59);
        this.doorL1.start(4, null);
        this.tA = new Uwamono(28672, -12.0f, -1.5f, -0.5f, 0.0f);
        this.tA.SetHitKind('\u0001');
        this.tA.SetSize(6.5f, 3.5f, 6.5f);
        if (Runtime.getFlags(3038, 1) == 1) {
            System.println("＊＊＊＊＊＊＊＊KOJI_38は立ってます＊＊＊＊＊＊＊＊＊");
            this.ele = new Obj();
            this.ele.initElevator(61, 0.0877193f, -5.0f);
            this.ele.setArgs(1, 0, 1);
            this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
            this.ele.start(1, "Up");
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(57);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py - 5.0f, this.doorR2.pz);
            this.doorR2.setRotate(0.0f, -90.0f, 0.0f);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(56);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py - 5.0f, this.doorL2.pz);
            this.doorL2.setRotate(0.0f, -90.0f, 0.0f);
        } else {
            System.println("＊＊＊＊＊＊＊＊KOJI_38は立ってません＊＊＊＊＊＊＊＊＊");
            this.ele = new Obj();
            this.ele.initElevator(61, 0.083333336f, 0.3f);
            this.ele.setArgs(1, 0, 1);
            this.ele.setArgs(12, 0.3f);
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(57);
            this.doorR2.start(4, null);
            this.doorR2.setRotate(0.0f, -90.0f, 0.0f);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(56);
            this.doorL2.start(4, null);
            this.doorL2.setRotate(0.0f, -90.0f, 0.0f);
        }
        this.saveA = new Uwamono(28733, 8.0f, 0.3088749f, -24.75f);
        this.shopA = new Uwamono(28679, 10.5f, 0.3088749f, -24.75f);
        this.shopA.SetShopNo(2);
        this.eve00 = new Effect(1414, 0);
        this.eve00.disp(true);
        this.eve01 = new Effect(1414, 1);
        this.eve01.disp(true);
        this.eve02 = new Effect(1410, 2);
        this.eve02.disp(true);
        this.eve03 = new Effect(1411, 3);
        this.eve03.disp(true);
        this.eve04 = new Effect(1414, 4);
        this.eve04.disp(true);
        this.eve05 = new Effect(1414, 5);
        this.eve05.disp(true);
        this.eve06 = new Effect(1413, 6);
        this.eve06.disp(true);
        this.eve07 = new Effect(1414, 7);
        this.eve07.disp(true);
        this.eve08 = new Effect(1414, 8);
        this.eve08.disp(true);
        this.eve09 = new Effect(1414, 9);
        this.eve09.disp(true);
        this.eve10 = new Effect(1414, 10);
        this.eve10.disp(true);
        Runtime.progressEffect(30);
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

        void Up() {
            ST0670.this.cam0.setMode(-1);
            ST0670.this.EV_Camera00();
            ST0670.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
            Runtime.setPlayerControl(false);
            ST0670.this.player.setTranslate(-10.5f, -5.3f, -0.4f);
            ST0670.this.ele.setArgs(12, 0.3f);
            Sound.effectPlay(196745);
            ST0670.this.doorR2.start(1, "MoveU");
            System.sleep(90);
            ST0670.this.doormove = 1;
            System.sleep(40);
            Runtime.enable(65536);
            ST0670.this.player.mtn(2, 9, 1.0f, true);
            ST0670.this.player.move(30, -8.0f, -0.5f, true);
            System.sleep(35);
            Runtime.disable(65536);
            Runtime.setFlags(3038, 1, 0);
            ST0670.this.cam0.setMode(0);
            ST0670.this.epass = 1;
            Runtime.setPlayerControl(true);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void MoveD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0670.this.doorR2.getTranslate();
                    ST0670.this.doorL2.getTranslate();
                    ST0670.this.doorR2.setTranslate(ST0670.this.doorR2.px, ST0670.this.doorR2.py - 0.083333336f, ST0670.this.doorR2.pz);
                    ST0670.this.doorL2.setTranslate(ST0670.this.doorL2.px, ST0670.this.doorL2.py - 0.083333336f, ST0670.this.doorL2.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void MoveU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0670.this.doorR2.getTranslate();
                    ST0670.this.doorL2.getTranslate();
                    ST0670.this.doorR2.setTranslate(ST0670.this.doorR2.px, ST0670.this.doorR2.py + 0.083333336f, ST0670.this.doorR2.pz);
                    ST0670.this.doorL2.setTranslate(ST0670.this.doorL2.px, ST0670.this.doorL2.py + 0.083333336f, ST0670.this.doorL2.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void automatic_door() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 7;
            int n2 = 30;
            int n3 = 45;
            while (true) {
                ST0670.this.player.getTranslate();
                f2 = (-12.0f - ST0670.this.player.px) * (-12.0f - ST0670.this.player.px) + (0.0f - ST0670.this.player.pz) * (0.0f - ST0670.this.player.pz);
                if (f2 > 20.0f) {
                    if (ST0670.this.doormove == 1) {
                        Sound.effectPlay(196713);
                    }
                    ST0670.this.doormove = 2;
                }
                if (ST0670.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 1.0f) {
                        Sound.effectPlay(196713);
                    }
                    if (f == (float) n2) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0670.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0670.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0670.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0670.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0670.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == 0.0f) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0670.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0670.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0670.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0670.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0670.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0670.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0670.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
                } else if (Runtime.getFlags(3038, 1) == 0 && Runtime.getFlags(3039, 1) == 0) {
                    ST0670.this.tA.setTranslate(-12.0f, -1.5f, -0.5f);
                }
                System.sleep(1);
            }
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }
}

