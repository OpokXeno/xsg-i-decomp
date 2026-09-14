import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1010
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK01_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int selected = 0;
    boolean EnterCheck = false;
    Uwamono door_0;
    Effect ef30203;
    Unit monitor;
    Unit kaban1;
    Unit kaban2;
    Unit kaban3;
    Unit card_key;
    Light light = new Light(0);
    Effect fade;
    int NO3_CARDKEY = Runtime.checkItem(10, 3);
    String[] Q1 = new String[]{"There is key card labeled /[color(0x329bbe)]Card No. 3/[color(0x808080)]. Pick it up?", "/[waitkey(64)]/[close()]"};
    String[] Get = new String[]{"You have obtained /[color(0x329bbe)]Card No. 3/[color(0x808080)].", "/[waitkey(64)]/[close()]"};

    ST1010() {
    }

    void DefaultLight() {
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void EV_Camera1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.331899f, 1.831971f, -4.469026f);
        this.camEV.setRotate(-19.918543f, 124.87783f, 0.0f);
        this.camEV.setFov(30.79883f);
        this.camEV.change();
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 100: {
                if (n2 != 0 || this.NO3_CARDKEY == 1 || this.EnterCheck) break;
                this.EnterCheck = true;
                Runtime.setPlayerControl(false);
                Runtime.disable(524288);
                Stage.setVisible(43, false);
                Stage.setVisible(42, false);
                Stage.setVisible(25, false);
                Stage.setVisible(26, false);
                Stage.setVisible(27, false);
                this.EV_Camera1();
                this.light.setColor(0, 0.45f, 0.45f, 0.45f);
                this.light.setColor(1, 0.35f, 0.35f, 0.35f);
                this.light.setDirection2(1, 0.15f, 1.0f, -0.05f);
                Stage.setColor(1.0f, 1.0f, 1.0f);
                this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
                Stage.setColor(1.0f, 1.0f, 1.0f);
                this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
                Stage.setColor(1.0f, 1.0f, 1.0f);
                this.player.setShadow(3, 16);
                if (this.DefaultMenu(this.Q1) == 0) {
                    Runtime.enable(65536);
                    this.player.getTranslate();
                    if (this.player.px >= -0.03175f) {
                        System.println("A - Type");
                        this.player.setRotateY(200.0f);
                    } else if (this.player.px >= -0.07735f && this.player.px < -0.03175f) {
                        System.println("B - Type");
                        this.player.setRotateY(195.0f);
                    } else if (this.player.px >= -0.25f && this.player.px < -0.07735f) {
                        System.println("C - Type");
                        this.player.setRotateY(190.0f);
                    } else {
                        System.println("D - Type");
                        this.player.setRotateY(185.0f);
                    }
                    System.sleep(10);
                    this.player.mtn(26, 1, 1.0f, true);
                    System.sleep(20);
                    Stage.setVisible(69, false);
                    this.ef30203.disp(false);
                    Sound.effectPlay(6);
                    this.DefaultTalk(this.Get);
                    this.player.mtn(0, 1, 1.0f, true);
                    Runtime.disable(65536);
                    System.sleep(40);
                    Runtime.addItem(10, 3);
                    this.NO3_CARDKEY = Runtime.checkItem(10, 3);
                }
                Stage.setVisible(43, true);
                Stage.setVisible(42, true);
                Stage.setVisible(25, true);
                Stage.setVisible(26, true);
                Stage.setVisible(27, true);
                this.player.setShadow(4, 16);
                this.cam0.setMode(0);
                this.DefaultLight();
                Runtime.enable(524288);
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1060, 4);
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
        Stage.setVisible(2, false);
        Stage.setVisible(4, false);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.kaban1 = new MAPUnit();
        this.kaban1.mapUnit(36);
        this.kaban1.start(4, null);
        this.kaban1.setScale(1.0f, 1.75f, 1.0f);
        this.kaban1.setTranslate(-0.25f, 0.955f, -3.75f);
        this.kaban1.setRotateY(-15.0f);
        this.kaban2 = new MAPUnit();
        this.kaban2.mapUnit(37);
        this.kaban2.start(4, null);
        this.kaban2.setScale(1.0f, 1.75f, 1.0f);
        this.kaban2.setTranslate(-0.25f, 0.878f, -3.75f);
        this.kaban2.setRotateY(-15.0f);
        this.kaban3 = new MAPUnit();
        this.kaban3.mapUnit(38);
        this.kaban3.start(4, null);
        this.kaban3.setScale(1.0f, 1.75f, 1.0f);
        this.kaban3.setTranslate(-0.274f, 0.88f, -4.05f);
        this.kaban3.setRotateY(-15.0f);
        this.card_key = new MAPUnit();
        this.card_key.mapUnit(69);
        this.card_key.start(4, null);
        this.card_key.setTranslate(-0.288f, 1.0f, -3.659f);
        this.door_0 = new Uwamono(7, 40, '\u0001');
        this.door_0.SetDoorType('\u0004');
        if (this.NO3_CARDKEY == 0) {
            this.ef30203 = new Effect(1018, 0);
            this.ef30203.disp(true);
        } else {
            Stage.setVisible(69, false);
        }
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
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

