import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK22_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0220
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK22_PRJ {
    Camera cam1;
    Player player;
    Enepc npc1;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono itembox;
    Effect fade;
    Light light = new Light(0);
    int page;

    ST0220() {
    }

    void Final_init(int n) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        if (Runtime.getFlags(1025, 1) == 1) {
            Runtime.jumpCF(249, 7);
            Runtime.setFlags(1025, 1, 0);
            return;
        }
        if (Runtime.getFlags(3, 1) == 1) {
            Runtime.jumpCF(241, 7);
            return;
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.itembox = new Uwamono(28680, 0.0f, 0.0f, -2.288f, 180.0f, 7);
        this.itembox.SetSymbol(28727);
        this.itembox.SetCallNo(1);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.doorA = new Uwamono(1, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                System.println("アイテム入手フラグ・オン");
                Runtime.setFlags(3250, 1, 1);
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

