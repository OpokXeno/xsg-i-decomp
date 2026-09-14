import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1020
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK02_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    Uwamono IA;
    Uwamono K1;
    MAPUnit kow;
    Uwamono doorA;
    Effect Obj600;
    Effect Obj601;
    Effect Obj602;
    Effect fade;
    boolean EnterCheck = false;
    Light light = new Light(0);
    int ROOM4_CARDKEY = Runtime.checkItem(10, 4);
    String[] T_ELOS = new String[]{"It is marked \"T-ELOS.\" It seems to be a part for some sort of robot.", "/[waitkey(64)]/[close()]"};

    ST1020() {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck) {
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.T_ELOS, 0);
            System.waitFor(this.win);
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1060, 3);
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
        this.doorA = new Uwamono(7, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        this.kow = new MAPUnit();
        this.kow.mapUnit(56);
        this.kow.start(4, null);
        this.kow.setTranslate(2.475f, 0.0f, -0.85f);
        this.IA = new Uwamono(28677, 2.475f, 0.0f, -0.85f, 180.0f, 398);
        this.IA.SetSymbol(28686);
        this.K1 = new Uwamono(56, 0, this.IA);
        this.K1.getTranslate();
        this.K1.SetItem(this.IA);
        this.K1.SetCallNo(0);
        this.Obj600 = new Effect(1440, 0);
        this.Obj600.disp(true);
        this.Obj601 = new Effect(1440, 1);
        this.Obj601.disp(true);
        this.Obj602 = new Effect(1440, 2);
        this.Obj602.disp(true);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.425f, 0.425f, 0.475f);
        Runtime.setIdLightCol(2, 1, 0.5f, 0.5f, 0.625f);
        Runtime.setIdLightCol(2, 2, 0.5f, 0.5f, 0.625f);
        Runtime.setIdLightCol(2, 3, 0.5f, 0.5f, 0.625f);
        Runtime.setIdLightVec(2, 1, -0.1f, 1.0f, -0.1f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
    }

    public void itemget(int n) {
        switch (n) {
            case 1: {
                System.println("U-TIC_CARD get!");
                Runtime.addItem(10, 4);
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

