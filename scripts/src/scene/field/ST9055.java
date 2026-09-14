import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2580
        extends Stage
        implements XenoConstants,
        CfConstants {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
    Player player;
    Camera cam1;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    MAPUnit elza;
    MAPUnit space;
    MAPUnit dog;
    MAPUnit kuk;
    MAPUnit dyu;
    MAPUnit last;
    MAPUnit uta;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono item1;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Effect fade2;
    int page;

    ST2580() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
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
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 30;
        this.fade2.args[2] = 1;
        Stage.setVisible(-1, true);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.1f, 0.1f, 0.1f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, -1.0f, 0.0f, 0.0f);
        this.light.setColor(2, 0.0f, 0.0f, 0.0f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.0f, 0.0f, 0.0f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light01.noAttach(false);
        this.light01.setScale(0.1f, 0.1f, 0.1f);
        this.light01.disp(true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        this.player.setVisible(false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFLockX(1, 0.0f);
        this.elza = new Mapunits();
        this.elza.init(20482, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space = new Mapunits();
        this.space.init(20614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.space.setRotate(70.0f, 0.0f, 0.0f);
        this.last = new Mapunits();
        this.last.init(20505, 0.0f, 0.0f, 100.0f, 0.0f);
        this.last.setScale(25.0f, 25.0f, 25.0f);
        this.last.setRotate(10.0f, 30.0f, 0.0f);
        this.uta = new Mapunits();
        this.uta.init(20562, 0.0f, 0.0f, 100.0f, 0.0f);
        this.uta.setScale(0.75f, 0.75f, 0.75f);
        this.uta.setRotate(10.0f, 30.0f, 0.0f);
        this.elza.start(1, "idle");
        this.fade2.call(0);
        this.cam0.setMode(-1);
        this.cam1 = Camera.create(1);
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.4f, 0.0f, 3.8f);
        this.camEV.setView(this.elza.px, this.elza.py, this.elza.pz);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void itemget(int n) {
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2580.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2580.waitPage(this.win, 64);
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    void yesno() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            while (true) {
                ST2580.this.elza.setTranslate(0.0f, 0.0f, f3 / 15.0f);
                ST2580.this.light01.setTranslate(0.0f, -0.02f, f3 / 15.0f - 0.02f);
                ST2580.this.elza.getTranslate();
                ST2580.this.camEV.setView(ST2580.this.elza.px, ST2580.this.elza.py, ST2580.this.elza.pz);
                if ((f3 += 1.0f) == 120.0f) {
                    ST2580.this.fade.call(0);
                }
                if (f3 == 150.0f) {
                    Runtime.setPlayerControl(true);
                    Runtime.jumpCF(3030, 1);
                }
                System.sleep(1);
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

    class Obj
            extends Unit {
        Obj() {
        }
    }
}

