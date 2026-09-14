import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS15_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0650
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS15_PRJ {
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
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono itembox;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Effect fade;
    Light light = new Light(0);
    int page;

    ST0650() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(141, 1) == 0) {
                    System.println("54_OFF");
                    Runtime.setFlags(3054, 1, 0);
                    Runtime.jumpCF(560, 3);
                    break;
                }
                if (Runtime.getFlags(141, 1) == 1 && Runtime.getFlags(142, 1) == 0) {
                    System.println("53_OFF");
                    Runtime.setFlags(3053, 1, 0);
                    Runtime.jumpCF(700, 3);
                    break;
                }
                if (Runtime.getFlags(142, 1) != 1 || Runtime.getFlags(142, 1) != 1) break;
                Runtime.jumpCF(561, 3);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        Runtime.setFlags(3059, 1, 1);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
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
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.itembox = new Uwamono(28680, 0.0f, 0.0f, -5.0f, 180.0f, 119);
        this.itembox.SetSymbol(28727);
        this.itembox.SetCallNo(1);
        this.doorA = new Uwamono(55, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetSize(0.25f, 2.25f, 1.5f);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3254, 1, 1);
                break;
            }
        }
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
}

