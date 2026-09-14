import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK26_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0261
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK26_PRJ {
    Player player;
    Camera cam1;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Menu menu;
    Window win;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono item1;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono trap4;
    Uwamono trap5;
    MAPUnit dai;
    MAPUnit hako;
    Effect fade;
    int S03;
    int S03B1;
    Light light = new Light(0);
    int button_flg;
    int page;
    String[] Talk01_1 = new String[]{"Engaging in a Boss battle.\n", "Jumping to the event.\n", "/[waitkey(64)]/[close()]"};

    ST0261() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                this.S03B1 = Runtime.getFlags(4, 1);
                if (this.S03B1 == 0) {
                    Runtime.setPlayerControl(false);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpEvent(1031);
                }
                System.println("Event Pass");
                break;
            }
        }
    }

    void NotYet() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print("工事中です/[wait(30)]/[close()]");
    }

    void broken(int n) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 1);
                    return;
                }
                Runtime.jumpCF(240, 1);
                break;
            }
        }
    }

    void init() {
        Sound.effectPlay(196613);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.78f, 0.78f, 0.78f);
        this.light.setDirection2(1, -0.023f, 1.0f, 0.008f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, -0.015f, 0.81f, 0.586f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.024f, -0.0f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 0, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 1, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 2, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 3, 0.15f, 0.15f, 0.15f);
        this.S03 = Runtime.getFlags(3, 1);
        Stage.setVisible(-1, true);
        this.dai = new Mapunits();
        this.dai.init(4);
        this.dai.start(4, null);
        this.hako = new Mapunits();
        this.hako.init(12);
        this.hako.start(4, null);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(5, false);
        Stage.setVisible(14, false);
        Stage.setVisible(15, false);
        Stage.setVisible(16, false);
        Stage.setVisible(0, false);
        Stage.setVisible(13, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 8.5f, 18.0f, 0.0f, 0.4f, 255, 255, 255, 255);
        this.cam0.setFog(2, 8.5f, 18.0f, 0.0f, 0.4f, 255, 255, 255, 255);
        this.cam0.setFog(3, 8.5f, 18.0f, 0.0f, 0.4f, 255, 255, 255, 255);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 345.0f, 13.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 13.6f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, -3.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 13.6f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 3.5f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        if (Runtime.checkItem(10, 12) != 0) {
            Stage.setVisible(12, false);
            this.item1 = new Uwamono(28677, 0.0f, 0.0f, -4.5f, 180.0f, 406);
        } else {
            this.item1 = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 406);
            this.item1.SetSymbol(28683);
            new Uwamono(12, 0, this.item1);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Down() {
            System.println("Start Down");
            int n = 0;
            while (n < 30) {
                ST0261.this.dai.getTranslate();
                ST0261.this.dai.setTranslate(this.px, this.py - 0.05f * (float) n, this.pz);
                ST0261.this.hako.getTranslate();
                ST0261.this.hako.setTranslate(this.px, this.py - 0.05f * (float) n, this.pz);
                System.sleep(1);
                ++n;
            }
            ST0261.this.hako.setVisible(false);
        }

        void Up() {
            System.println("Start Up");
            int n = 0;
            while (n < 30) {
                ST0261.this.dai.getTranslate();
                ST0261.this.dai.setTranslate(this.px, -1.5f + 0.05f * (float) n, this.pz);
                System.sleep(1);
                ++n;
            }
        }
    }
}

