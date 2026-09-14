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
import xeno.map.MC_GNK15_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3090
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK15_PRJ {
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
    Enepc shi;
    Enepc jun;
    Enepc zig;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit Wall;
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
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int button_flg = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono SMK;
    Uwamono saveA;
    Uwamono itembox;
    Uwamono Uwacol;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
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
    String[] Info_00 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 17.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 17.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 17, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "A monitoring room for all of Proto Merkabah.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "It seems all the controls are centralized here.", "/[waitkey(1)]/[clear()]", "Can you locate the reactor core?", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "It seems to be far below where we are.", "/[waitkey(1)]/[clear()]", "It looks like we'll have to take several elevator shafts down to get there.", "/[waitkey(64)]/[close()]"};
    String[] JUN_01 = new String[]{"/[label(Jr.)]", "Looks like we still have a ways to go. Let's hurry, Shion.", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "Yes.", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"It's a holographic image of the Proto Merkabah. The structure consists of multiple layers, any of which can be purged.", "/[waitkey(64)]/[close()]"};

    ST3090() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 3.627f, 5.191f, -15.267f, 60.0f, 3.627f, 3.239f, -15.267f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -20.839f;
        fArray2[2] = 32.839f;
        fArray2[4] = 60.0f;
        fArray2[5] = -20.839f;
        fArray2[6] = 32.839f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 60);
        this.camEV.rotateSPL(fArray3, 1, 3, 60);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.061f, 1.927f, -20.092f);
        this.camEV.setRotate(-10.926f, -130.597f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.571f, 1.32f, -20.42f);
        this.camEV.setRotate(-4.264f, 213.992f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3085, 1) != 0) return;
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
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.E04.disp(false);
                            Sound.effectPlay(196743);
                            this.Wall.start(1, "Open");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3217, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3217, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3237, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3297, 1) != 0) return;
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(56);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_03, 0);
                    System.waitFor(this.win);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3297, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3143, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.fade1.call(0);
                    System.sleep(60);
                    this.player.setTranslate(100.0f, 0.0f, 100.0f);
                    this.shi.kickEnepc(4, 1);
                    this.jun.kickEnepc(4, 1);
                    this.zig.kickEnepc(4, 1);
                    this.shi.setLocation(4, 0);
                    this.jun.setLocation(4, 1);
                    this.zig.setLocation(4, 2);
                    this.cam0.setMode(-1);
                    this.EV_Camera00();
                    this.fade2.call(0);
                    System.sleep(60);
                    this.jun.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.JUN_00, 0);
                    System.waitFor(this.win);
                    System.sleep(30);
                    this.jun.kickEnepc(4, 0);
                    this.EV_Camera01();
                    this.zig.kickEnepc(9, 11);
                    this.shi.kickEnepc(9, 13);
                    this.zig.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ZIG_00, 0);
                    System.waitFor(this.win);
                    this.zig.kickEnepc(4, 0);
                    this.shi.kickEnepc(9, 13);
                    this.shi.kickEnepc(0, 7);
                    System.sleep(40);
                    this.shi.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_00, 0);
                    System.waitFor(this.win);
                    this.shi.kickEnepc(9, 12);
                    this.shi.kickEnepc(4, 0);
                    this.EV_Camera02();
                    this.jun.kickEnepc(4, 1);
                    this.jun.kickEnepc(9, 11);
                    this.jun.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.JUN_01, 0);
                    System.waitFor(this.win);
                    this.shi.kickEnepc(4, 1);
                    this.shi.kickEnepc(9, 12);
                    this.shi.kickEnepc(0, 7);
                    System.sleep(40);
                    this.shi.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_01, 0);
                    System.waitFor(this.win);
                    this.fade1.call(0);
                    System.sleep(30);
                    this.player.setTranslate(0.0f, 0.0f, -19.5f);
                    this.shi.setTranslate(100.0f, 100.0f, 100.0f);
                    this.jun.setTranslate(100.0f, 100.0f, 100.0f);
                    this.zig.setTranslate(100.0f, 100.0f, 100.0f);
                    Runtime.setFlags(3143, 1, 1);
                    this.cam0.setMode(0);
                    this.fade2.call(0);
                    System.sleep(60);
                    Runtime.setPlayerControl(true);
                    return;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_00, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3088, 1, 1);
                break;
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
                Runtime.jumpCF(68596, 3);
                break;
            }
            case 1: {
                Runtime.jumpCF(68586, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(68636, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -16.0f, 0.0f, -5.0f, 0.0f);
        this.teiten1.SetBgm(196617);
        this.teiten2 = new Uwamono(28690, -16.5f, 0.0f, -16.5f, 0.0f);
        this.teiten2.SetBgm(196617);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -21.5f, 0.0f);
        this.teiten3.SetBgm(196617);
        this.teiten4 = new Uwamono(28690, 11.85f, 0.0f, -17.5f, 0.0f);
        this.teiten4.SetBgm(196617);
        this.teiten5 = new Uwamono(28690, 15.75f, 0.0f, -11.5f, 0.0f);
        this.teiten5.SetBgm(196617);
        this.teiten6 = new Uwamono(28690, 16.0f, 0.0f, -5.0f, 0.0f);
        this.teiten6.SetBgm(196617);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3085, 1) == 0) {
            this.Uwacol = new Uwamono(28672, -4.5f, 0.0f, -5.0f);
            this.Uwacol.SetSize(0.5f, 1.0f, 4.0f);
            this.Wall = new Mapunits();
            this.Wall.mapUnit(53);
            this.Wall.start(4, null);
        } else {
            this.Uwacol = new Uwamono(28672, -4.5f, 5.0f, -5.0f);
            this.Uwacol.SetSize(0.5f, 1.0f, 4.0f);
            this.Wall = new Mapunits();
            this.Wall.mapUnit(53);
            this.Wall.start(4, null);
            this.Wall.getTranslate();
            this.Wall.setTranslate(this.Wall.px, this.Wall.py + 5.0f, this.Wall.pz);
        }
        this.E01 = new Effect(1644, 0.0f, 0.0f, -25.838f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
        this.E02 = new Effect(1645, 0.02f, 2.575f, -25.849f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E02.noAttach(false);
        if (Runtime.getFlags(3085, 1) == 0) {
            this.E04 = new Effect(1417, -5.996f, 1.198f, -6.751f, 0.0f);
            this.E04.disp(true);
            this.E04.setClip(true);
            this.E04.setScale(1.3f, 1.3f, 1.3f);
            this.E04.setRotate(270.0f, 0.0f, 0.0f);
            this.E04.noAttach(false);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.saveA = new Uwamono(28678, -3.5f, 0.0f, -15.5f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 3, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.5f, 0.55f, 0.55f);
        Runtime.setIdLightCol(3, 3, 0.5f, 0.55f, 0.55f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, -3.0f, 5.0f, -13.72f, 40.0f, -25.0f, -15.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(7, 1);
        if (Runtime.getFlags(3143, 1) == 0) {
            this.shi = new NPC_NORMAL(1, 11, 0, 3, 5, 100.0f, 100.0f, 100.0f, 0.0f);
            this.shi.setInvalidID(1);
            this.jun = new NPC_NORMAL(5, 12, 0, 3, 6, 100.0f, 100.0f, 100.0f, 0.0f);
            this.jun.setInvalidID(1);
            this.zig = new NPC_NORMAL(6, 13, 0, 3, 7, 100.0f, 100.0f, 100.0f, 0.0f);
            this.zig.setInvalidID(1);
        }
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 383);
        this.itembox.SetSymbol(28672);
        new Uwamono(0, 4, this.itembox);
        if (Runtime.getFlags(3088, 1) == 0) {
            this.SMK = new Uwamono(3, 5);
            this.SMK.SetSize(1.8f, 3.0f, 1.0f);
            this.SMK.SetCallNo(1);
        } else {
            Stage.setVisible(3, false);
        }
        if (Runtime.getFlags(3297, 1) == 0) {
            this.doorA = new Uwamono(1, 40, '\u0004');
            this.doorA.SetDiffSize(0.0f, -0.01f, 0.0f);
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA = new Uwamono(1, 40, '\u0004');
            this.doorA.SetDiffSize(0.0f, -0.01f, 0.0f);
            this.doorA.SetDoorType('\u0004');
            this.doorA.SetDoorRange(0.1f);
        }
        this.doorB = new Uwamono(84, 40, '\u0001');
        new Uwamono(83, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(85, 40, '\u0001');
        new Uwamono(86, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(87, 40, '\u0001');
        new Uwamono(88, 40, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(89, 40, '\u0001');
        new Uwamono(90, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0004');
        this.doorF = new Uwamono(92, 40, '\u0001');
        new Uwamono(91, 40, '\u0001', this.doorF);
        this.doorF.SetDoorType('\u0004');
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

        void Open() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3090.this.Wall.getTranslate();
                    ST3090.this.Wall.setTranslate(ST3090.this.Wall.px, ST3090.this.Wall.py + 0.041666668f, ST3090.this.Wall.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            ST3090.this.Uwacol.setTranslate(ST3090.this.Uwacol.px, ST3090.this.Uwacol.py + 5.0f, ST3090.this.Uwacol.pz);
            Runtime.setFlags(3085, 1, 1);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

