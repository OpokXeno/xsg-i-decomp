import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK05B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0330
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK05B_PRJ {
    Player player;
    Camera cam1;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Effect E01;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] SYS_00 = new String[]{"Would you like to rest?", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};

    ST0330() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            block0:
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
                            this.player.setTranslate(0.0f, 0.0f, 0.0f);
                            Runtime.charAllRecovery();
                            this.fade2.call(0);
                            System.sleep(60);
                            Sound.effectPlay(26);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SYS_01, 0);
                            System.waitFor(this.win);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
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
                if (Runtime.getFlags(3007, 1) == 1) {
                    Runtime.jumpCF(65861, 2);
                    break;
                }
                if (Runtime.getFlags(3012, 1) == 1) {
                    Runtime.jumpCF(65857, 2);
                    break;
                }
                Runtime.jumpCF(65856, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -3.0f, 0.0f, 3.0f, 0.0f);
        this.teiten1.SetBgm(196620);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(2, false);
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
        this.E01 = new Effect(1437, 3.0f, 1.45f, 0.0f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.setRotate(0.0f, 90.0f, 0.0f);
        this.E01.setScale(1.0f, 1.25f, 1.0f);
        this.E01.noAttach(false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        Stage.setVisible(2, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        new Uwamono(28678, 2.0f, 0.0f, 0.0f);
        this.doorA = new Uwamono(10, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorA.DoorClose();
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
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
}

