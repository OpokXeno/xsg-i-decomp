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
import xeno.map.MC_KAS19_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2530
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS19_PRJ {
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
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono item1;
    Uwamono doorA;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    int page;

    ST2530() {
    }

    void EOB(int n) {
        System.println("EOB");
        if (n == 4) {
            Runtime.setFlags(8081, 1, 1);
        }
        if (n == 5) {
            Runtime.setFlags(8082, 1, 1);
        }
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
            case 0: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.lo = 0;
                Runtime.setPlayerControl(true);
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
                Runtime.jumpCF(2529, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(2529, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(2529, 4);
                break;
            }
            case 3: {
                Runtime.jumpCF(2529, 5);
                break;
            }
            case 4: {
                Runtime.jumpCF(2549, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(2549, 2);
            }
        }
    }

    void init() {
        float[] fArray;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        this.teiten1 = new Uwamono(28690, 20.0f, 0.0f, 3.0f);
        this.teiten1.SetBgm(196616);
        this.teiten2 = new Uwamono(28690, 20.0f, 0.0f, -3.0f);
        this.teiten2.SetBgm(196616);
        Stage.setColor(1.15f, 1.15f, 1.15f);
        this.light.setColor(0, 0.325f, 0.325f, 0.325f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(2, 1, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(2, 2, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(2, 3, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.25f, 0.25f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setShootHeightCheck(false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setShootHeightCheck(false);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        float[] fArray2 = new float[16];
        fArray2[0] = 9.6f;
        fArray2[2] = 3.4f;
        fArray2[3] = -1.0f;
        fArray2[4] = 5.1f;
        fArray2[6] = 3.4f;
        fArray2[8] = 0.1f;
        fArray2[10] = 3.4f;
        fArray2[11] = 1.0f;
        fArray2[12] = 12.6f;
        fArray2[14] = 3.4f;
        float[] fArray3 = fArray2;
        this.enemy1 = new NpcEnemy(16649, 1, 1, 9, 3, 9.6f, 0.0f, 3.4f, 270.0f, fArray3);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray4 = new float[12];
        fArray4[0] = 3.8f;
        fArray4[2] = -3.9f;
        fArray4[3] = -1.0f;
        fArray4[4] = -0.6f;
        fArray4[6] = -3.9f;
        fArray4[8] = 7.2f;
        fArray4[10] = -3.9f;
        float[] fArray5 = fArray4;
        this.enemy2 = new NpcEnemy(16649, 2, 2, 9, 3, 3.8f, 0.0f, -3.9f, 270.0f, fArray5);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray6 = new float[20];
        fArray6[0] = 2.2f;
        fArray6[1] = 1.4f;
        fArray6[2] = -11.0f;
        fArray6[3] = -1.0f;
        fArray6[4] = 7.0f;
        fArray6[6] = -13.0f;
        fArray6[8] = 12.0f;
        fArray6[10] = -14.8f;
        fArray6[11] = 1.0f;
        fArray6[12] = 7.0f;
        fArray6[14] = -9.0f;
        fArray6[16] = 11.1f;
        fArray6[18] = -9.3f;
        fArray6[19] = 3.0f;
        float[] fArray7 = fArray6;
        this.enemy3 = new NpcEnemy(16649, 3, 3, 9, 3, 2.2f, 1.4f, -11.0f, 90.0f, fArray7);
        this.enemy3.setGroup(0, 0, 1, 1);
        if (Runtime.getFlags(8081, 1) == 0) {
            float[] fArray8 = new float[12];
            fArray8[0] = -13.3f;
            fArray8[1] = 4.9f;
            fArray8[2] = -2.7f;
            fArray8[3] = -1.0f;
            fArray8[4] = -13.8f;
            fArray8[5] = 4.9f;
            fArray8[6] = -2.7f;
            fArray8[8] = -12.8f;
            fArray8[9] = 4.9f;
            fArray8[10] = -2.7f;
            fArray = fArray8;
            this.enemy4 = new NpcEnemy(16392, 4, 4, 36, 5, -13.3f, 4.9f, -2.7f, 90.0f, fArray);
            this.enemy4.setGroup(3, 3, 3, 3);
        }
        if (Runtime.getFlags(8082, 1) == 0) {
            float[] fArray9 = new float[16];
            fArray9[0] = 5.0f;
            fArray9[1] = 1.4f;
            fArray9[2] = 11.0f;
            fArray9[3] = -1.0f;
            fArray9[4] = 10.0f;
            fArray9[5] = 1.4f;
            fArray9[6] = 11.0f;
            fArray9[8] = 2.6f;
            fArray9[9] = 1.4f;
            fArray9[10] = 9.5f;
            fArray9[13] = 1.4f;
            fArray9[14] = 11.0f;
            fArray = fArray9;
            this.enemy5 = new NpcEnemy(16388, 5, 5, 24, 7, 5.0f, 1.4f, 11.0f, 0.0f, fArray);
            this.enemy5.setGroup(2, 2, 2, 2);
        }
        new Uwamono(28672, -13.5f, 0.4f, -14.4f, 0.0f);
        new Uwamono(28672, 9.2f, 0.4f, -16.2f, 0.0f);
        this.item1 = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 317);
        this.item1.SetSymbol(28684);
        this.item2 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 299);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 300);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 301);
        new Uwamono(73, 29);
        new Uwamono(74, 29, this.item2);
        new Uwamono(75, 29);
        new Uwamono(76, 1, this.item1);
        new Uwamono(77, 12, this.item3);
        new Uwamono(78, 12, this.item4);
        if (Runtime.getFlags(8082, 1) == 0) {
            new Uwamono(79, 32, this.enemy5);
        } else {
            new Uwamono(79, 32);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2530.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2530.waitPage(this.win, 64);
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

