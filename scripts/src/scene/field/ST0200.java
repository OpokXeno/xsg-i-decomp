import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK20_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0200
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK20_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    Camera cam1;
    Player player;
    Menu menu;
    Window win;
    Enepc npc1;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono itembox;
    Effect fade;
    int page;
    String[] SHION_00 = new String[]{"/[label(Shion)]", "What in the world could this be...?", "/[waitkey(64)]/[close()]"};

    ST0200() {
    }

    void Final_init(int n) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(3051, 1) == 1) {
                    Runtime.setFlags(3051, 1, 0);
                    Runtime.jumpCF(381, 5);
                    break;
                }
                if (Runtime.getFlags(3052, 1) != 1) break;
                Runtime.setFlags(3052, 1, 0);
                Runtime.jumpCF(382, 5);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(4, false);
        Stage.setVisible(1, false);
        Stage.setVisible(17, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 0.0f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.itembox = new Uwamono(28680, 0.0f, 0.0f, -9.5f, 180.0f, 35);
        this.itembox.SetSymbol(28726);
        this.itembox.SetCallNo(1);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.doorA = new Uwamono(13, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                if (Runtime.getFlags(50, 1) == 0) {
                    Runtime.setFlags(3247, 1, 1);
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(28, 9, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHION_00, 0);
                    System.waitFor(this.win);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                Runtime.setFlags(3247, 1, 1);
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
        }
    }
}

