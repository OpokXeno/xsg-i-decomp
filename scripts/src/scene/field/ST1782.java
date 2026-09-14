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
import xeno.map.MC_DYU08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1782
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU08_PRJ {
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
    Unit unit1;
    Unit Doa;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
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
    Uwamono saveA;
    Effect fade;
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
    int page;
    String[] K01_00CN = new String[]{"Hey, hey, hey, you're in the way! I'm busy getting ready to launch. I don't have time to talk!", "/[waitkey(64)]/[close()]"};
    String[] K01_00CJ = new String[]{"Oh, Little Captain. Going out on a mission?", "/[waitkey(1)]/[clear()]", "Don't you dare fall behind those Federation guys! Give it all you've got! Give it your all!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CN = new String[]{"An A.G.W.S. is about to launch. It's dangerous, so please step away!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CJ = new String[]{"Little Master! Don't you need to head for the Foundation?", "/[waitkey(1)]/[clear()]", "You won't be able to get to the City Sectors of the Foundation from here!", "/[waitkey(1)]/[clear()]", "Please hurry towards the shuttle area!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CN = new String[]{"Hey, hey, hey, you're in the way! I'm busy getting ready to launch. I don't have time to talk!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CJ = new String[]{"Hey, Little Captain. Going out on a mission?", "/[waitkey(1)]/[clear()]", "Don't worry, we'll protect the ship.", "/[waitkey(1)]/[clear()]", "Hurry up and go rescue the little princess!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CN = new String[]{"An A.G.W.S. is about to launch. It's dangerous, so please step away!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CJ = new String[]{"We'll be okay here! No need to worry about us!", "/[waitkey(1)]/[clear()]", "Please just concentrate on rescuing your friend!", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "You are in front of the Foundation. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] KARI_01 = new String[]{"/[label(Temp Person)]", "You are in front of the Song of Nephilim. 【temp】", "/[waitkey(64)]/[close()]"};

    ST1782() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 != 0 && n2 == 1) {
            switch (n) {
                default:
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U02_00CN, 0);
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
                Runtime.jumpCF(1862, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(6036, 1) == 0) {
            this.teiten1 = new Uwamono(28690, 4.0f, 1.0f, -8.0f, 0.0f);
            this.teiten1.SetBgm(196619);
        }
        this.teiten2 = new Uwamono(28690, -7.0f, 1.0f, -5.5f, 0.0f);
        this.teiten2.SetBgm(196620);
        this.teiten3 = new Uwamono(28690, -7.0f, 1.0f, -4.0f, 0.0f);
        this.teiten3.SetBgm(196620);
        this.teiten4 = new Uwamono(28690, -7.0f, 1.0f, 4.0f, 0.0f);
        this.teiten4.SetBgm(196620);
        this.teiten5 = new Uwamono(28690, -7.0f, 1.0f, 5.5f, 0.0f);
        this.teiten5.SetBgm(196620);
        Stage.setVisible(-1, true);
        this.Doa = new Mapunits();
        this.Doa.mapUnit(42);
        this.Doa.start(4, null);
        this.Doa.setTranslate(this.Doa.px, this.Doa.py, this.Doa.pz - 4.0f);
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
        Stage.setVisible(25, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.saveA = new Uwamono(28678, -1.0f, 0.0f, 0.5f);
        this.npc1 = new NPC_NORMAL(527, 11, 0, 2, 3, -5.451f, 0.0f, 3.098f, 0.0f);
        this.npc2 = new NPC_NORMAL(527, 12, 0, 2, 3, -4.056f, 0.0f, -5.425f, 0.0f);
        if (Runtime.getFlags(373, 1) == 0) {
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
        } else {
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
        }
        this.npc1.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(131072);
        this.npc1.enableDTKFlag(12);
        this.npc2.enableDTKFlag(12);
        this.npc1.setMotion(0, 2);
        this.doorA = new Uwamono(36, 42, '\u0001');
        new Uwamono(37, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.player.setID(1);
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

