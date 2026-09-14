import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1110
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK11_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Uwamono doorA;
    Uwamono IA;
    Uwamono IB;
    Light light = new Light(0);
    Effect fade;

    ST1110() {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1050, 6);
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setVisible(0, false);
        this.doorA = new Uwamono(1, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.IA = new Uwamono(28677, 1.725f, 0.0f, -0.5f, 90.0f, 425);
        this.IA.SetSymbol(28686);
        this.IA.SetCallNo(1);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
    }

    public void itemget(int n) {
        switch (n) {
            case 1: {
                System.println("サブルート16の鍵");
                Runtime.setFlags(3236, 1, 1);
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

