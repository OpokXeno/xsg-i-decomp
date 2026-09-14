import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK18_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0189
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK18_PRJ {
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

    ST0189() {
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
                System.println("【環境シミュレータ】・VOK04へjump！！");
                Runtime.jumpCF(329, 5);
                break;
            }
        }
    }

    void init() {
        this.light.setColor(0, 0.5f, 0.5f, 0.5f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 0.834f, 0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -0.872f, -0.49f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.itembox = new Uwamono(28680, 9.5f, 0.0f, 0.0f, 90.0f, 33);
        this.itembox.SetSymbol(28727);
        this.itembox.SetCallNo(1);
        this.doorA = new Uwamono(13, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetSize(0.25f, 2.25f, 1.5f);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3258, 1, 1);
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

