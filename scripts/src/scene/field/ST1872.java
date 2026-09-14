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
import xeno.map.MC_DYU17_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1872
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU17_PRJ {
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
    Unit Oyasumi;
    Unit Kidou;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int pass = 0;
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
    Uwamono saveA;
    Uwamono itembox;
    Effect fade;
    Effect fade1;
    Effect fade2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;
    String[] Player_00 = new String[]{"/[label(Shion)]", "I'm really tired now...I think I'll call it a day.", "/[waitkey(64)]/[close()]"};
    String[] Player_01 = new String[]{"/[label(Shion)]", "Pushing ourselves too hard is bad for our health. We should turn in for the night.", "/[waitkey(64)]/[close()]"};
    String[] Player_02 = new String[]{"/[label(Shion)]", "It's a large ship. I think I'll go have some more fun.", "/[waitkey(64)]/[close()]"};
    String[] Player_03 = new String[]{"/[label(Shion)]", "I was a little too cold towards Allen earlier. I better apologize to him in person.", "/[waitkey(1)]/[clear()]", "I wonder where he went?", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "Before Kukai.", "/[waitkey(64)]/[close()]"};
    String[] KARI_01 = new String[]{"/[label(Temp Person)]", "You are in front of the Song of Nephilim. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Get some rest?", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};
    String[] D00_00 = new String[]{"heY! you seem to have more character latelY!", "/[waitkey(64)]/[close()]"};
    String[] D00_01 = new String[]{"you guys always seem busY. why don't you try a more easygoing lifestyle like minE?", "/[waitkey(64)]/[close()]"};

    ST1872() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 != 0 && n2 == 1) {
            block1:
            switch (n) {
                case 100: {
                    System.println("\\\\\\\\\\\\\\\\\\\\\\");
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.disable(524288);
                            this.fade1.call(0);
                            System.sleep(60);
                            Runtime.enable(65536);
                            this.player.rotY(1, 0.0f, true);
                            this.player.setTranslate(-5.5f, 0.6f, 0.0f);
                            Runtime.charAllRecovery();
                            this.fade2.call(0);
                            System.sleep(60);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SYS_01, 0);
                            System.waitFor(this.win);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            break block1;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.D00_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.D00_01, 0);
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
                Runtime.jumpCF(1752, 4);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
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
        Stage.setVisible(3, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.npc1 = new NPC_NORMAL(1612, 11, 0, 0, 3, 0.0f, 0.0f, -1.5f, 0.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        new Uwamono(1, 40);
        new Uwamono(0, 40);
        this.doorA = new Uwamono(2, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.itembox = new Uwamono(28677, -7.81f, 0.6f, -6.019f, 180.0f, 457);
        this.itembox.SetSymbol(28686);
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
        }
    }
}

