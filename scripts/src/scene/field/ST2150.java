import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK15_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2150
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK15_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono itembox;

    ST2150() {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(373, 1) == 1) {
                    System.println("クリーニング屋・４");
                    Runtime.jumpCF(2091, 4);
                    break;
                }
                if (Runtime.getFlags(360, 1) == 1) {
                    System.println("襲撃後外観街１・８");
                    Runtime.jumpCF(2230, 4);
                    break;
                }
                System.println("クリーニング屋・４");
                Runtime.jumpCF(2090, 4);
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(0, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFLockX(1, 0.0f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.itembox = new Uwamono(28680, 0.09f, 0.0f, -1.95f, 180.0f, 454);
        this.itembox.SetSymbol(28725);
        this.itembox.SetCallNo(1);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3243, 1, 1);
                break;
            }
        }
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
}

