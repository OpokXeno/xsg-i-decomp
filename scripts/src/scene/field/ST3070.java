import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_GNK13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3070
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK13_PRJ {
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
    Enepc zig;
    Enepc jun;
    Enepc cha;
    Enepc mom;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
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
    Uwamono itembox;
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
    String[] CHA_00 = new String[]{"/[label(chaos)]", "It looks like some kind of data.", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "A Realian?", "/[waitkey(1)]/[clear()]", "But it's pretty old. The year T.C. 4474 made by Tyrrell, Lamech Model 3.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "Lamech Model 3...", "/[waitkey(1)]/[clear()]", "That's the model that was manufactured up until a year before the Miltian Conflict.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "The production year and manufacturer all differ, but they are all older than 4752.", "/[waitkey(64)]/[close()]"};
    String[] CHA_01 = new String[]{"/[label(chaos)]", "It seems all of these Realians were transferred to the Miltia maintenance facility due to some kind of abnormality.", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "But why is that data here? What is all of this data for?", "/[waitkey(64)]/[close()]"};
    String[] SHI_01_1 = new String[]{"/[label(Shion)]", "MOMO, did Dr. Mizrahi ever tell you anything？", "/[waitkey(64)]/[close()]"};
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "No, Mommy...never...", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "I see...", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"There's a large collection of data on early-model Realians stored here.", "/[waitkey(64)]/[close()]"};

    ST3070() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 14.079f, 3.591f, -9.629f, 90.0f, 14.079f, 3.591f, -9.629f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -18.797f;
        fArray2[2] = 71.419f;
        fArray2[4] = 90.0f;
        fArray2[5] = -18.797f;
        fArray2[6] = 35.079f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.847f, 9.095f, -9.722f);
        this.camEV.setRotate(-46.957f, -33.959f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3141, 1) == 0) {
                        Runtime.setPlayerControl(false);
                        this.fade1.call(0);
                        System.sleep(60);
                        this.player.setTranslate(100.0f, 0.0f, 100.0f);
                        this.shi.kickEnepc(4, 1);
                        this.zig.kickEnepc(4, 1);
                        this.jun.kickEnepc(4, 1);
                        this.cha.kickEnepc(4, 1);
                        this.mom.kickEnepc(4, 1);
                        this.shi.setLocation(4, 0);
                        this.zig.setLocation(4, 1);
                        this.jun.setLocation(4, 2);
                        this.cha.setLocation(4, 3);
                        this.mom.setLocation(4, 4);
                        this.cam0.setMode(-1);
                        this.EV_Camera00();
                        this.fade2.call(0);
                        System.sleep(60);
                        System.sleep(90);
                        this.EV_Camera01();
                        this.cha.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.CHA_00, 0);
                        System.waitFor(this.win);
                        this.shi.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_00, 0);
                        System.waitFor(this.win);
                        this.jun.kickEnepc(9, 11);
                        this.jun.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.JUN_00, 0);
                        System.waitFor(this.win);
                        this.zig.kickEnepc(0, 7);
                        System.sleep(40);
                        this.zig.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ZIG_00, 0);
                        System.waitFor(this.win);
                        this.cha.kickEnepc(0, 7);
                        System.sleep(40);
                        this.cha.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.CHA_01, 0);
                        System.waitFor(this.win);
                        this.shi.kickEnepc(9, 14);
                        this.shi.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_01, 0);
                        System.waitFor(this.win);
                        this.shi.kickEnepc(9, 15);
                        this.shi.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_01_1, 0);
                        System.waitFor(this.win);
                        this.mom.kickEnepc(9, 11);
                        this.mom.kickEnepc(0, 8);
                        System.sleep(40);
                        this.mom.kickEnepc(0, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.MOM_00, 0);
                        System.waitFor(this.win);
                        this.shi.kickEnepc(9, -1);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_02, 0);
                        System.waitFor(this.win);
                        this.fade1.call(0);
                        System.sleep(60);
                        this.player.setTranslate(9.0f, 0.0f, -15.0f);
                        this.shi.setTranslate(100.0f, 100.0f, 100.0f);
                        this.zig.setTranslate(100.0f, 100.0f, 100.0f);
                        this.jun.setTranslate(100.0f, 100.0f, 100.0f);
                        this.cha.setTranslate(100.0f, 100.0f, 100.0f);
                        this.mom.setTranslate(100.0f, 100.0f, 100.0f);
                        this.cam0.setMode(0);
                        Runtime.setFlags(3141, 1, 1);
                        this.fade2.call(0);
                        System.sleep(60);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    break;
                }
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
                Runtime.jumpCF(68596, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(68586, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -16.0f, 0.0f, -5.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, -16.5f, 0.0f, -10.5f, 0.0f);
        this.teiten2.SetBgm(196615);
        this.teiten3 = new Uwamono(28690, 16.5f, 0.0f, -5.0f, 0.0f);
        this.teiten3.SetBgm(196615);
        this.teiten4 = new Uwamono(28690, 16.5f, 0.0f, -10.5f, 0.0f);
        this.teiten4.SetBgm(196615);
        this.teiten5 = new Uwamono(28690, -5.0f, 0.0f, -10.5f, 0.0f);
        this.teiten5.SetBgm(196615);
        this.teiten6 = new Uwamono(28690, 2.9f, 0.0f, -13.45f, 0.0f);
        this.teiten6.SetBgm(196615);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1641, 8.972f, 2.449f, -17.184f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
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
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
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
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        if (Runtime.getFlags(3141, 1) == 0) {
            this.shi = new NPC_NORMAL(1, 11, 0, 3, 13, 100.0f, 100.0f, 100.0f, 0.0f);
            this.shi.setInvalidID(1);
            this.zig = new NPC_NORMAL(6, 12, 0, 3, 11, 100.0f, 100.0f, 100.0f, 0.0f);
            this.zig.setInvalidID(1);
            this.jun = new NPC_NORMAL(5, 13, 0, 3, 10, 100.0f, 100.0f, 100.0f, 0.0f);
            this.jun.setInvalidID(1);
            this.cha = new NPC_NORMAL(3, 14, 0, 3, 12, 100.0f, 100.0f, 100.0f, 0.0f);
            this.cha.setInvalidID(1);
            this.mom = new NPC_NORMAL(4, 15, 0, 3, 9, 100.0f, 100.0f, 100.0f, 0.0f);
            this.mom.setInvalidID(1);
        }
        this.enemy1 = new Enepc();
        this.enemy1.init(16403, 3, 6.5f, 0.0f, -5.0f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(1, 1, 4, 4);
        float[] fArray = new float[32];
        fArray[0] = 6.5f;
        fArray[2] = -5.0f;
        fArray[3] = -1.0f;
        fArray[4] = 7.5f;
        fArray[6] = -5.0f;
        fArray[8] = 8.5f;
        fArray[10] = -5.0f;
        fArray[11] = 1.0f;
        fArray[12] = 9.5f;
        fArray[14] = -5.0f;
        fArray[15] = 2.0f;
        fArray[16] = 10.5f;
        fArray[18] = -5.0f;
        fArray[19] = 3.0f;
        fArray[20] = 11.5f;
        fArray[22] = -5.0f;
        fArray[23] = 4.0f;
        fArray[24] = 12.5f;
        fArray[26] = -5.0f;
        fArray[27] = 5.0f;
        fArray[28] = 15.3f;
        fArray[30] = -2.1f;
        fArray[31] = 6.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        float[] fArray3 = new float[21];
        fArray3[0] = 6.5f;
        fArray3[2] = -5.0f;
        fArray3[3] = 7.5f;
        fArray3[5] = -5.0f;
        fArray3[6] = 8.5f;
        fArray3[8] = -5.0f;
        fArray3[9] = 9.5f;
        fArray3[11] = -5.0f;
        fArray3[12] = 10.5f;
        fArray3[14] = -5.0f;
        fArray3[15] = 11.5f;
        fArray3[17] = -5.0f;
        fArray3[18] = 12.5f;
        fArray3[20] = -5.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(16396, 23, -6.5f, 0.0f, -5.0f, 270.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(3, 3, 5, 5);
        float[] fArray5 = new float[32];
        fArray5[0] = -6.5f;
        fArray5[2] = -5.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -7.5f;
        fArray5[6] = -5.0f;
        fArray5[8] = -8.5f;
        fArray5[10] = -5.0f;
        fArray5[11] = 1.0f;
        fArray5[12] = -9.5f;
        fArray5[14] = -5.0f;
        fArray5[15] = 2.0f;
        fArray5[16] = -10.5f;
        fArray5[18] = -5.0f;
        fArray5[19] = 3.0f;
        fArray5[20] = -11.5f;
        fArray5[22] = -5.0f;
        fArray5[23] = 4.0f;
        fArray5[24] = -12.5f;
        fArray5[26] = -5.0f;
        fArray5[27] = 5.0f;
        fArray5[28] = -15.3f;
        fArray5[30] = -2.1f;
        fArray5[31] = 6.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 1, 23, fArray6);
        float[] fArray7 = new float[21];
        fArray7[0] = -6.5f;
        fArray7[2] = -5.0f;
        fArray7[3] = -7.5f;
        fArray7[5] = -5.0f;
        fArray7[6] = -8.5f;
        fArray7[8] = -5.0f;
        fArray7[9] = -9.5f;
        fArray7[11] = -5.0f;
        fArray7[12] = -10.5f;
        fArray7[14] = -5.0f;
        fArray7[15] = -11.5f;
        fArray7[17] = -5.0f;
        fArray7[18] = -12.5f;
        fArray7[20] = -5.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.kickEnepc(10, 85, 0);
        this.itembox = new Uwamono(28677, 1.727f, 0.0f, -6.325f, 180.0f, 382);
        this.itembox.SetSymbol(28685);
        this.doorA = new Uwamono(1, 40, '\u0001');
        new Uwamono(0, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(2, 40, '\u0001');
        new Uwamono(3, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(4, 40, '\u0001');
        new Uwamono(5, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(6, 40, '\u0001');
        new Uwamono(7, 40, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(9, 40, '\u0001');
        new Uwamono(8, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0004');
        new Uwamono(28673, 14.6f, 0.0f, -4.0f, 0.0f);
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
    }
}

