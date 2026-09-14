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
import xeno.map.MC_KAS04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2430
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS04_PRJ {
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
    MAPUnit mapunit;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Effect fire06;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    int page;
    String[] annai = new String[]{"'Subway North City Line\n", " Miltia Park Station'", "/[waitkey(64)]/[close()]"};

    ST2430() {
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        this.enemy2.kickEnepc(4, 2);
        this.enemy3.kickEnepc(4, 2);
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.lo == 1) {
            return;
        }
        switch (n2) {
            case 1: {
                this.Ene_Stop();
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                this.Ene_Restart();
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(2429, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(2529, 1);
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
        this.teiten1 = new Uwamono(28690, -17.0f, 0.0f, 3.5f);
        this.teiten1.SetBgm(196611);
        this.teiten2 = new Uwamono(28690, 16.5f, 11.0f, 10.5f);
        this.teiten2.SetBgm(196612);
        Stage.setColor(1.25f, 1.25f, 1.25f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(2, 1, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(2, 2, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(2, 3, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(3, 1, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(3, 2, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(3, 3, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        this.fire01 = new Effect(1402, -5.2f, 0.0f, -16.2f, 0.0f);
        this.fire01.setScale(2.0f, 2.0f, 2.0f);
        this.fire02 = new Effect(1402, 5.4f, 0.0f, 19.3f, 0.0f);
        this.fire02.setScale(3.0f, 2.0f, 2.0f);
        this.fire03 = new Effect(1402, -10.2f, 0.0f, 18.8f, 0.0f);
        this.fire03.setScale(1.0f, 1.0f, 1.0f);
        this.fire04 = new Effect(1402, -11.5f, 0.0f, -1.8f, 0.0f);
        this.fire04.setRotate(0.0f, 45.0f, 0.0f);
        this.fire05 = new Effect(1402, -11.9f, 0.0f, -13.3f, 0.0f);
        this.fire05.setScale(1.0f, 1.3f, 2.0f);
        this.fire06 = new Effect(1402, 7.2f, 0.0f, 6.7f, 0.0f);
        this.fire06.setScale(0.8f, 0.8f, 0.6f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 15.0f, 0.0f, 9.5f, 45.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 25.0f, 0.0f, 9.5f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 9.5f, 45.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 5.0f, 0.0f, 9.5f, 45.0f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        float[] fArray = new float[12];
        fArray[0] = -0.7f;
        fArray[2] = 1.2f;
        fArray[3] = -1.0f;
        fArray[4] = -5.3f;
        fArray[6] = 13.3f;
        fArray[10] = -2.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16649, 1, 1, 9, 3, -0.7f, 0.0f, 1.2f, 180.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[12];
        fArray3[2] = 14.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = -5.3f;
        fArray3[6] = 13.3f;
        fArray3[10] = -2.0f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16649, 2, 1, 9, 3, 0.0f, 0.0f, 14.0f, 180.0f, fArray4);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[12];
        fArray5[0] = -9.8f;
        fArray5[2] = 14.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -5.3f;
        fArray5[6] = 13.3f;
        fArray5[10] = -2.0f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16649, 3, 1, 9, 3, -9.8f, 0.0f, 14.0f, 90.0f, fArray6);
        this.enemy3.setGroup(0, 0, 1, 1);
        new Uwamono(28672, -15.0f, -1.0f, -15.5f, 0.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 279);
        new Uwamono(0, 76, this.item1);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2430.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2430.waitPage(this.win, 64);
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

    class NpcEnemy
            extends Enepc {
        NpcEnemy(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }
    }
}

