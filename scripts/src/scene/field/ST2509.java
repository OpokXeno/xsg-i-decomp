import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KAS16_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2500
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS16_PRJ {
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
    Enepc enemy6;
    Unit unit1;
    MAPUnit mapunit;
    Unit dummy;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect light10;
    Effect mizu;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono item1;
    Uwamono k1;
    Uwamono k2;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    boolean j1 = true;
    boolean j2 = true;
    boolean j3 = true;
    boolean j4 = true;
    boolean TREE_KESHI = false;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] are = new String[]{"That's the mascot that was on the ground at the park.", "/[waitkey(64)]/[close()]"};

    ST2500() {
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
                if (!this.j4) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0 右下");
                Stage.setVisible(24, false);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 62);
                System.println("ドリンク 4");
                this.j4 = false;
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 1: {
                if (!this.j2) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:1 右上");
                Stage.setVisible(22, false);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 62);
                System.println("ドリンク 2");
                this.j2 = false;
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
                if (!this.j1) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:2 左上");
                Stage.setVisible(21, false);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 62);
                System.println("ドリンク 1");
                this.j1 = false;
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 3: {
                if (!this.j3) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:3 左下");
                Stage.setVisible(23, false);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 62);
                System.println("ドリンク 3");
                this.j3 = false;
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 4: {
                if (Runtime.getFlags(8061, 1) != 0) break;
                Runtime.setFlags(8061, 1, 1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("イベント起動");
                Runtime.disable(524288);
                this.cam0.setMode(-1);
                this.camEV = Camera.create(1);
                this.camEV.setTranslate(-17.7f, 2.7f, -0.5f);
                this.camEV.setRotate(-7.9f, -23.4f, 0.0f);
                this.camEV.setFov(42.5f);
                this.camEV.change();
                Runtime.enable(65536);
                this.player.rotY(10, 135.0f, true);
                System.sleep(10);
                Runtime.disable(65536);
                this.npc1.kickEnepc(4, 0);
                System.sleep(80);
                this.k1.SendSignal();
                System.sleep(30);
                this.camEV.setTranslate(-13.6f, 2.7f, -11.0f);
                this.camEV.setRotate(-3.6f, -23.4f, 0.0f);
                this.camEV.setFov(42.5f);
                this.camEV.change();
                System.sleep(200);
                Stage.setVisible(24, false);
                this.dummy.setVisible(true);
                System.sleep(10);
                Sound.effectPlay(524294);
                this.light03.disp(true);
                System.sleep(30);
                this.light03.disp(false);
                float[] fArray = new float[8];
                fArray[1] = -15.5f;
                fArray[2] = -26.6f;
                fArray[4] = 120.0f;
                fArray[5] = -15.5f;
                fArray[6] = -51.0f;
                float[] fArray2 = fArray;
                this.camEV.setTranslate(-17.7f, 2.7f, -0.5f);
                this.camEV.rotateSPL(fArray2, 0, 1, 120);
                this.camEV.setFov(42.5f);
                this.camEV.change();
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                Sound.effectPlay(524290);
                System.sleep(10);
                this.npc1.kickEnepc(4, 2);
                System.sleep(20);
                this.nwin(this.are);
                this.cam0.setMode(0);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                Runtime.enable(524288);
                break;
            }
            case 5: {
                Stage.setVisible(7, this.TREE_KESHI);
                Stage.setVisible(8, this.TREE_KESHI);
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken:1");
                this.item1.SetGravity(true);
                System.sleep(10);
                this.mizu.disp(true);
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
                System.println("MC_KAS15・２");
                Runtime.jumpCF(2499, 2);
                break;
            }
            case 1: {
                System.println("MC_KAS17・１");
                Runtime.jumpCF(2519, 1);
                break;
            }
            case 2: {
                System.println("MC_KAS17・２");
                Runtime.jumpCF(2519, 2);
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
        Runtime.setDefocusQuick(0, 1, 5000, 1);
        Runtime.setDefocusQuick(1, 1, 4000, 1);
        Runtime.setDefocusQuick(2, 1, 3000, 1);
        Runtime.setDefocusQuick(3, 1, 2000, 1);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
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
        Runtime.setIdLightCol(2, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(2, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(2, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(2, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(4, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(4, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(4, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(7, 0.02f, 0.02f);
        this.cam0.setCFAngle(8, -28.0f, -15.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(8, 0.015f, 0.015f);
        this.cam0.setFog(1, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(2, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(3, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(4, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(5, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(6, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(7, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.cam0.setFog(8, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 250);
        this.light01 = new Effect(1626, -26.0f, 0.0f, -7.7f, 0.0f);
        this.light01.setRotate(0.0f, 90.0f, 0.0f);
        this.light02 = new Effect(1626, -9.8f, 1.6f, -16.0f, 0.0f);
        this.light02.setRotate(0.0f, 90.0f, 0.0f);
        this.light03 = new Effect(1747, -11.9f, 1.0f, -16.5f, 0.0f);
        this.light03.setScale(3.0f, 3.0f, 3.0f);
        this.light03.disp(false);
        this.light07 = new Effect(1626, 15.3f, 5.0f, -10.0f, 0.0f);
        this.light07.setRotate(0.0f, 90.0f, 0.0f);
        this.light08 = new Effect(1626, 7.5f, 0.0f, -28.3f, 0.0f);
        this.light08.setRotate(0.0f, 90.0f, 0.0f);
        this.light10 = new Effect(1660, -11.9f, 1.0f, -16.5f, 0.0f);
        this.light10.disp(false);
        this.light10.setForceLoop(true);
        this.mizu = new Effect(1746, -3.0f, -0.5f, -4.0f, 0.0f);
        this.mizu.disp(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.npc1 = new NpcEnemy(8705, 1, 0, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.npc1.setVisible(3, false);
        this.npc1.setInvalidID(1);
        this.npc1.setMotion(0, 60);
        this.npc1.dispRadar(false);
        this.npc1.setShadow(0, 16);
        this.npc1.disableDTKFlag(65536);
        if (Runtime.getFlags(8061, 1) == 1) {
            this.npc1.setVisible(false);
        }
        this.dummy = new Obj();
        this.dummy.init(24663, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy.setParent(this.npc1, 60);
        this.dummy.setTranslate(0.21f, -0.21f, 0.01f);
        this.dummy.setRotate(120.0f, 0.0f, 0.0f);
        this.dummy.setScale(1.0f, 1.0f, 1.0f);
        this.dummy.setVisible(false);
        float[] fArray = new float[12];
        fArray[0] = 3.1f;
        fArray[1] = 1.6f;
        fArray[2] = -1.7f;
        fArray[3] = -1.0f;
        fArray[4] = 3.1f;
        fArray[5] = 1.6f;
        fArray[8] = 3.1f;
        fArray[9] = 1.6f;
        fArray[10] = -3.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16402, 1, 3, 39, 5, 3.1f, 1.6f, -1.7f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 2, 2);
        float[] fArray3 = new float[12];
        fArray3[0] = 15.7f;
        fArray3[1] = 0.7f;
        fArray3[2] = 4.3f;
        fArray3[3] = -1.0f;
        fArray3[4] = 14.7f;
        fArray3[5] = 0.7f;
        fArray3[6] = 4.3f;
        fArray3[8] = 16.7f;
        fArray3[9] = 0.7f;
        fArray3[10] = 4.3f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16402, 2, 2, 39, 5, 15.7f, 0.7f, 4.3f, 0.0f, fArray4);
        this.enemy2.setGroup(0, 0, 2, 2);
        float[] fArray5 = new float[8];
        fArray5[0] = -11.1f;
        fArray5[2] = 8.6f;
        fArray5[3] = -1.0f;
        fArray5[4] = -12.1f;
        fArray5[6] = 8.6f;
        float[] fArray6 = fArray5;
        this.enemy4 = new NpcEnemy(16388, 4, 4, 24, 7, -11.1f, 0.0f, 8.6f, 90.0f, fArray6);
        this.enemy4.setGroup(3, 3, 3, 3);
        float[] fArray7 = new float[12];
        fArray7[0] = 0.8f;
        fArray7[1] = 0.6f;
        fArray7[2] = -10.8f;
        fArray7[3] = -1.0f;
        fArray7[4] = -1.8f;
        fArray7[5] = 0.6f;
        fArray7[6] = -10.8f;
        fArray7[8] = 1.8f;
        fArray7[9] = 0.6f;
        fArray7[10] = -10.8f;
        float[] fArray8 = fArray7;
        this.enemy5 = new NpcEnemy(16390, 5, 5, 14, 11, 0.8f, 0.6f, -10.8f, 270.0f, fArray8);
        this.enemy5.setGroup(4, 4, 4, 4);
        float[] fArray9 = new float[12];
        fArray9[0] = -2.1f;
        fArray9[2] = -1.3f;
        fArray9[3] = -1.0f;
        fArray9[4] = -3.1f;
        fArray9[6] = -1.3f;
        fArray9[8] = -1.1f;
        fArray9[10] = -1.3f;
        float[] fArray10 = fArray9;
        this.enemy6 = new NpcEnemy(20227, 6, 6, 29, 9, -2.1f, 0.0f, -1.3f, 0.0f, fArray10);
        this.enemy6.setGroup(5, 5, 5, 5);
        this.enemy6.setTP(0);
        this.item1 = new Uwamono(28677, -3.0f, 2.0f, -4.0f, 180.0f, 316);
        this.item1.SetSymbol(28684);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 292);
        new Uwamono(13, 23);
        new Uwamono(14, 23);
        new Uwamono(15, 33);
        this.k2 = new Uwamono(18, 14);
        this.k2.SetCallNo(1);
        new Uwamono(43, 57);
        new Uwamono(11, 58, this.item2);
        new Uwamono(25, 10);
        new Uwamono(26, 10);
        new Uwamono(27, 10);
        this.k1 = new Uwamono(28, 10);
        if (Runtime.checkItem(10, 62) != 0) {
            if (Runtime.checkItem(10, 62) == 1) {
                Stage.setVisible(24, false);
                this.j4 = false;
            } else if (Runtime.checkItem(10, 62) == 2) {
                Stage.setVisible(23, false);
                Stage.setVisible(24, false);
                this.j3 = false;
                this.j4 = false;
            } else if (Runtime.checkItem(10, 62) == 3) {
                Stage.setVisible(22, false);
                Stage.setVisible(23, false);
                Stage.setVisible(24, false);
                this.j2 = false;
                this.j3 = false;
                this.j4 = false;
            } else {
                Stage.setVisible(21, false);
                Stage.setVisible(22, false);
                Stage.setVisible(23, false);
                Stage.setVisible(24, false);
                this.j1 = false;
                this.j2 = false;
                this.j3 = false;
                this.j4 = false;
            }
        }
        if (n == 3) {
            this.enemy1.kickEnepc(4, 2);
            this.enemy2.kickEnepc(4, 2);
            this.enemy4.kickEnepc(4, 2);
            this.enemy5.kickEnepc(4, 2);
            this.enemy6.kickEnepc(4, 2);
            this.enemy1.setVisible(false);
            this.enemy2.setVisible(false);
            this.enemy4.setVisible(false);
            this.enemy5.setVisible(false);
            this.enemy6.setVisible(false);
        }
        this.mapunit = new Mapunits();
        this.mapunit.mapUnit(0);
        this.mapunit.start(4, null);
        this.mapunit.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2500.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2500.waitPage(this.win, 64);
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

        void idle() {
            float f = 0.0f;
            while (true) {
                if (f == 10.0f) {
                    ST2500.this.npc1.kickEnepc(4, 2);
                }
                f += 1.0f;
                System.sleep(1);
            }
        }
    }
}

