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
import xeno.map.MC_DYU13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1833
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU13_PRJ {
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
    String[] GUIDE_01 = new String[]{"Going to the Dock.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Bridge.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Hangar.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Park.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] POL_M_00 = new String[]{"Oh, Little Master.\n", "Nothing out of the ordinary so far, Sir!", "/[waitkey(64)]/[close()]"};
    String[] POL_M_01 = new String[]{"Beyond that door is a storage warehouse for important secrets.", "/[waitkey(1)]/[clear()]", "You cannot pass through this area without permission from either Master Gaignun or Little Master.", "/[waitkey(64)]/[close()]"};
    String[] POL_M_02 = new String[]{"Oh, you are a V.I.P.?! Please excuse me!!", "/[waitkey(64)]/[close()]"};
    String[] POL_W_00 = new String[]{"Little Master, what in the world is inside?", "/[waitkey(1)]/[clear()]", "The truth is, it bothers me so much, it's hard to concentrate on my guard duties.", "/[waitkey(64)]/[close()]"};
    String[] POL_W_01 = new String[]{"You are a V.I.P., correct? Please go right ahead.", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "Right before the last stage. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Isolation Area Station'", "/[waitkey(64)]/[close()]"};

    ST1833() {
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
        this.camEV.setTranslate(-7.013f, 1.967f, 0.714f);
        this.camEV.setRotate(0.59f, 0.0f, 0.0f);
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
                    this.menu.addItem("Dock\nResidential Area\nBridge\nHangar\nPark\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3069, 1, 1);
                            this.Departure(1803);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1843);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1823);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1863);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1853);
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
        if (n2 == 1 || n2 != 2) return;
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
            window.print(this.POL_M_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.POL_M_01, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.POL_W_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.POL_W_01, 0);
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
                Runtime.jumpCF(1760, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1833, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(1833, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 1.0f, 0.0f, -4.5f, 0.0f);
        this.teiten1.SetBgm(196624);
        this.teiten2 = new Uwamono(28690, 1.0f, 0.0f, 2.5f, 0.0f);
        this.teiten2.SetBgm(196624);
        this.teiten3 = new Uwamono(28690, -11.0f, 0.0f, -1.0f, 0.0f);
        this.teiten3.SetBgm(196622);
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
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.7f, 0.6f, 0.1f);
        Runtime.setIdLightCol(1, 3, 0.7f, 0.6f, 0.1f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 7.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(1288, 11, 0, 2, 5, -1.45f, 0.0f, -3.0f, -45.0f);
        this.npc2 = new NPC_NORMAL(1289, 12, 0, 3, 7, -1.45f, 0.0f, 1.0f, 225.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc1.disableDTKFlag(131074);
        this.npc2.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(8);
        this.npc2.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc2.enableDTKFlag(4);
        this.EXP0 = new NPC_NORMAL(20492, 1, 0, 0, 3, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.talkto("TalkNPC1");
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.doorA = new Uwamono(28, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(23, 42, '\u0001');
        new Uwamono(24, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        if (Runtime.getFlags(3071, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(10);
            this.DenDen.start(4, null);
            this.player.setLocation(1, 4);
            this.player.setVisible(false);
            this.DenDen.start(1, "Evt");
        }
        this.Step = new Mapunits();
        this.Step.mapUnit(42);
        this.Step.start(4, null);
        this.Step.setTranslate(1.0f, -0.01f, 0.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, -7.013f, 2.25f, -3.0f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, -7.013f, 2.25f, -2.99f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18018, 0, 256, 130);
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
            ST1833.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1833.this.Step.setTranslate(ST1833.this.Step.px + 0.016666668f, ST1833.this.Step.py, ST1833.this.Step.pz);
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
            ST1833.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1833.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1833.this.EV_Camera01();
            System.sleep(1);
            ST1833.this.EXP0.kickEnepc(4, 1);
            ST1833.this.win = Window.create();
            ST1833.this.win.setSize(4, 45);
            ST1833.this.win.setLocation(15, 305);
            ST1833.this.win.print("Isolation Area");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1833.this.EXP0.getTranslate();
                ST1833.this.EXP0.setTranslate(ST1833.this.EXP0.px, ST1833.this.EXP0.py, ST1833.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1833.this.win.close();
            ST1833.this.player.setVisible(true);
            ST1833.this.EXP0.kickEnepc(0, 1);
            ST1833.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1833.this.doorA.DoorOpen();
            System.sleep(15);
            ST1833.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1833.this.player.mtn(2, 9, 1.0f, true);
            ST1833.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1833.this.Step.start(1, "Chijimi");
            ST1833.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1833.this.EXP0.kickEnepc(4, 0);
            ST1833.this.EXP0.kickEnepc(4, 2);
            ST1833.this.doorA.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1833.this.cam0.setMode(0);
            Runtime.setFlags(3071, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1833.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1833.this.Step.setTranslate(ST1833.this.Step.px - 0.016666668f, ST1833.this.Step.py, ST1833.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

