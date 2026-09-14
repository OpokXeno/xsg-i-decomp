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
import xeno.map.MC_KAS15_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2490
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS15_PRJ {
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
    Effect mokumoku;
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
    int page;

    ST2490() {
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
                System.println("地下水同【MC_KAS14】・５・シオン");
                Runtime.jumpCF(2489, 5);
                break;
            }
            case 1: {
                System.println("森２【MC_KAS16】・１");
                Runtime.jumpCF(2509, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -17.0f, 0.0f, 3.5f);
        this.teiten1.SetBgm(196611);
        this.teiten2 = new Uwamono(28690, 14.5f, 5.0f, 10.5f);
        this.teiten2.SetBgm(196612);
        Stage.setVisible(-1, true);
        Runtime.setDefocusQuick(0, 1, 5000, 2);
        Runtime.setDefocusQuick(1, 1, 4000, 2);
        Runtime.setDefocusQuick(2, 1, 3000, 2);
        Runtime.setDefocusQuick(3, 1, 2000, 2);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(2, 1, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(2, 2, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightCol(2, 3, 0.285f, 0.285f, 0.285f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setFog(1, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(2, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(3, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.light01 = new Effect(1626, 21.9f, 0.0f, -17.0f, 0.0f);
        this.light01.setRotate(0.0f, 90.0f, 0.0f);
        this.light02 = new Effect(1626, -7.5f, 0.0f, -2.4f, 0.0f);
        this.light02.setRotate(0.0f, 90.0f, 0.0f);
        this.light03 = new Effect(1626, 0.8f, 0.0f, -19.3f, 0.0f);
        this.light03.setRotate(0.0f, 90.0f, 0.0f);
        this.fire01 = new Effect(1402, 14.5f, -0.1f, 10.5f, 0.0f);
        this.fire01.setScale(0.3f, 0.3f, 0.3f);
        this.mokumoku = new Effect(1515, -18.1f, 1.0f, 2.5f, 0.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setShootRange(2.0f);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(8072, 1) == 0) {
            Runtime.setFlags(8072, 1, 1);
            Runtime.resetOutFriend(1);
            Runtime.resetOutFriend(3);
            Runtime.setOutFriend(6);
            Runtime.setOutFriend(4);
            Runtime.setOutFriend(5);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 1);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
        }
        float[] fArray = new float[8];
        fArray[0] = 3.6f;
        fArray[1] = 0.2f;
        fArray[2] = -0.0f;
        fArray[3] = -1.0f;
        fArray[4] = 3.6f;
        fArray[5] = 0.2f;
        fArray[6] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16402, 1, 1, 46, 3, 3.6f, 0.2f, -0.0f, 210.0f, fArray2);
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[12];
        fArray3[0] = 18.9f;
        fArray3[1] = 0.2f;
        fArray3[2] = -4.5f;
        fArray3[3] = -1.0f;
        fArray3[4] = 19.9f;
        fArray3[5] = 0.2f;
        fArray3[6] = -4.5f;
        fArray3[8] = 12.9f;
        fArray3[9] = 0.2f;
        fArray3[10] = -4.5f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16402, 2, 2, 39, 3, 18.9f, 0.2f, -4.5f, 270.0f, fArray4);
        this.enemy2.setGroup(0, 0, 0, 0);
        this.item1 = new Uwamono(28677, 14.5f, -0.2f, 8.5f, 180.0f, 433);
        this.item1.SetSymbol(28683);
        new Uwamono(23, 12);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2490.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2490.waitPage(this.win, 64);
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

