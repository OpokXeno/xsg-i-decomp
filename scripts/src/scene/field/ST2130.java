import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2130
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK13_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect cloud_1;
    Effect cloud_2;
    Effect cloud_3;
    Effect cloud_4;
    Effect cloud_5;
    int A_OR_B = Runtime.getFlags(6048, 1);
    String[] A1 = new String[]{"'Launch Pad'", "/[waitkey(64)]/[close()]"};
    String[] A2 = new String[]{"'City Sector 26 & 27'", "/[waitkey(64)]/[close()]"};
    String[] A3 = new String[]{"'Gaignun's Private Beach'", "/[waitkey(64)]/[close()]"};
    String[] A4 = new String[]{"'A.G.W.S. Parts Shop &", " Foundation Robot Academy'", "/[waitkey(64)]/[close()]"};

    ST2130() {
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    public void KickEvent(int n, int n2) {
        if (n != 100 || this.EnterCheck) {
            return;
        }
        this.EnterCheck = true;
        if (n2 == 0) {
            if (Runtime.getFlags(361, 1) == 1) {
                Runtime.setPlayerControl(false);
                System.println("ランチ発着場【MC_KUK16】・１");
                this.DefaultTalk(this.A1);
                this.menu = Menu.create();
                this.menu.addItem("Enter\nDon't enter");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setPlayerControl(true);
                        Runtime.jumpCF(2160, 1);
                        return;
                    }
                }
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                return;
            }
            if (Runtime.getFlags(7092, 4) == 13) {
                Runtime.setPlayerControl(false);
                System.println("ランチ発着場【MC_KUK16】・１");
                this.DefaultTalk(this.A1);
                this.menu = Menu.create();
                this.menu.addItem("Enter\nDon't enter");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setFlags(361, 1, 1);
                        Runtime.setFlags(3191, 1, 0);
                        Runtime.setFlags(3108, 1, 1);
                        Runtime.setFlags(7138, 1, 1);
                        Runtime.setFlags(3162, 1, 1);
                        Runtime.setPlayerControl(true);
                        Runtime.jumpEvent(3368);
                        return;
                    }
                }
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                return;
            }
            Runtime.setPlayerControl(false);
            System.println("ランチ発着場【MC_KUK16】・１");
            this.DefaultTalk(this.A1);
            this.menu = Menu.create();
            this.menu.addItem("Enter\nDon't enter");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpCF(2160, 1);
                    return;
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            return;
        }
        if (n2 == 1) {
            if (Runtime.getFlags(373, 1) == 1) {
                Runtime.setPlayerControl(false);
                System.println("外観２【MC_KUK03】・２");
                this.DefaultTalk(this.A2);
                this.menu = Menu.create();
                this.menu.addItem("Enter\nDon't enter");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setPlayerControl(true);
                        Runtime.jumpCF(2171, 2);
                        return;
                    }
                }
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                return;
            }
            if (Runtime.getFlags(360, 1) == 1) {
                Runtime.setPlayerControl(false);
                System.println("襲撃後外観１【MC_KUK03B】・２");
                this.DefaultTalk(this.A2);
                this.menu = Menu.create();
                this.menu.addItem("Enter\nDon't enter");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setPlayerControl(true);
                        Runtime.jumpCF(2170, 2);
                        return;
                    }
                }
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                return;
            }
            Runtime.setPlayerControl(false);
            System.println("外観１【MC_KUK03】・２");
            this.DefaultTalk(this.A2);
            this.menu = Menu.create();
            this.menu.addItem("Enter\nDon't enter");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpCF(2030, 2);
                    return;
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            return;
        }
        if (n2 == 2) {
            Runtime.setPlayerControl(false);
            System.println("人工ビーチ【MC_KUK02】・１");
            this.DefaultTalk(this.A3);
            this.menu = Menu.create();
            this.menu.addItem("Enter\nDon't enter");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpCF(2020, 1);
                    return;
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            return;
        }
        if (n2 == 3) {
            Runtime.setPlayerControl(false);
            System.println("AGWSパーツショップ【MC_KUK12】・１");
            this.DefaultTalk(this.A4);
            this.menu = Menu.create();
            this.menu.addItem("Enter\nDon't enter");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpCF(2120, 1);
                    return;
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            return;
        }
        this.EnterCheck = false;
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            default:
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.45f, 0.45f, 0.45f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 18.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
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
        this.cloud_1 = new Effect(1583, -25.0f, 4.0f, 10.0f, 0.0f);
        this.cloud_1.setScale(2.0f, 1.0f, 1.25f);
        this.cloud_1.setRotate(0.0f, -30.0f, 0.0f);
        this.cloud_2 = new Effect(1583, -10.0f, 2.0f, 10.0f, 0.0f);
        this.cloud_2.setScale(2.0f, 1.0f, 2.0f);
        this.cloud_3 = new Effect(1583, 5.0f, 2.0f, -10.0f, 0.0f);
        this.cloud_3.setScale(2.0f, 1.0f, 1.5f);
        this.cloud_4 = new Effect(1583, -1.75f, 3.5f, -15.0f, 0.0f);
        this.cloud_4.setScale(2.0f, 1.0f, 2.0f);
        this.cloud_5 = new Effect(1583, -17.0f, 4.0f, 18.0f, 0.0f);
        this.cloud_5.setScale(2.0f, 1.0f, 2.0f);
        Runtime.progressEffect(60);
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

