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
import xeno.map.MC_GNK11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3050
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK11_PRJ {
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
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit MJele;
    Unit PUPU;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    Effect E08;
    Effect fade;
    Light light = new Light(0);
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] Info_00 = new String[]{"Use the elevator?", "/[waitkey(64)]/[close()]"};

    ST3050() {
    }

    void EOB(int n) {
        if (n == 1) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3181, 1, 1);
        }
        if (n == 2) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3182, 1, 1);
        }
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-9.077975f, 4.8f, 7.499014f);
        this.camEV.setRotate(-21.16063f, -10.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            block0:
            switch (n) {
                case 100: {
                    this.player.getTranslate();
                    if (this.player.py >= 1.0f) {
                        return;
                    }
                    if (Runtime.getFlags(3084, 1) != 0) break;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.doorE.SetDoorType('\u0002');
                            this.doorE.DoorClose();
                            System.sleep(45);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(30, -7.463f, -1.66f, true);
                            System.sleep(35);
                            this.player.rotY(15, 0.0f, true);
                            System.sleep(20);
                            this.cam0.setMode(-1);
                            this.EV_Camera00();
                            this.player.mtn(28, 9, 1.0f, true);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(false);
                            Sound.effectPlay(196748);
                            this.MJele.setArgs(12, -10.0f);
                            System.sleep(180);
                            Runtime.setPlayerControl(true);
                            Runtime.setFlags(3083, 1, 1);
                            this.doorE.SetDoorType('\u0004');
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.jumpCF(68576, 0);
                            break block0;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(60, -7.463f, 1.437f, true);
                    System.sleep(60);
                    this.doorE.DoorClose();
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        Light light = new Light(0);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(68606, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68616, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(68626, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(68656, 2);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        this.teiten1 = new Uwamono(28690, -7.463f, 0.0f, -1.66f, 0.0f);
        this.teiten1.SetBgm(196614);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1638, -7.424f, 2.61f, 0.14f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E02 = new Effect(1640, 7.462f, 14.445f, -0.507f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E03 = new Effect(1640, 7.462f, 24.445f, -0.507f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
        this.E04 = new Effect(1640, 7.462f, 34.445f, -0.507f, 0.0f);
        this.E04.disp(true);
        this.E04.setClip(true);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFAngle(14, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(15, 0.01f, 0.01f);
        this.cam0.setCFAngle(16, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(16, 0.01f, 0.01f);
        this.cam0.setCFAngle(17, -28.0f, -10.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(17, 0.01f, 0.01f);
        if (Runtime.getFlags(3181, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16403, 3, 2.0f, 10.0f, 1.5f, 180.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 1, 1);
            fArray2 = new float[]{2.0f, 10.0f, 1.5f, 1.0f, 3.0f, 10.0f, 1.5f, 2.0f, 4.0f, 10.0f, 1.5f, 3.0f, 5.0f, 10.0f, 1.5f, 4.0f, 5.0f, 10.0f, 0.5f, 5.0f, 5.0f, 10.0f, -0.5f, 6.0f, 5.0f, 10.0f, -1.5f, 7.0f, 5.0f, 10.0f, -2.5f, 8.0f, 4.0f, 10.0f, -2.5f, 9.0f, 3.0f, 10.0f, -2.5f, -1.0f};
            this.enemy1.setParams(1, 1, 1, 3, fArray2);
            fArray = new float[]{2.0f, 10.0f, 1.5f, 3.0f, 10.0f, 1.5f, 4.0f, 10.0f, 1.5f, 5.0f, 10.0f, 1.5f, 5.0f, 10.0f, 0.5f, 5.0f, 10.0f, -0.5f, 5.0f, 10.0f, -1.5f, 5.0f, 10.0f, -2.5f, 4.0f, 10.0f, -2.5f, 3.0f, 10.0f, -2.5f, 2.0f, 10.0f, -2.5f};
            this.enemy1.setParams(fArray);
        }
        if (Runtime.getFlags(3182, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16403, 3, 2.0f, 20.0f, 1.5f, 0.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(0, 0, 1, 1);
            fArray2 = new float[]{2.0f, 20.0f, 1.5f, 1.0f, 3.0f, 20.0f, 1.5f, 2.0f, 4.0f, 20.0f, 1.5f, 3.0f, 5.0f, 20.0f, 1.5f, 4.0f, 5.0f, 20.0f, 0.5f, 5.0f, 5.0f, 20.0f, -0.5f, 6.0f, 5.0f, 20.0f, -1.5f, 7.0f, 5.0f, 20.0f, -2.5f, 8.0f, 4.0f, 20.0f, -2.5f, 9.0f, 3.0f, 20.0f, -2.5f, -1.0f};
            this.enemy2.setParams(1, 1, 2, 3, fArray2);
            fArray = new float[]{2.0f, 20.0f, 1.5f, 3.0f, 20.0f, 1.5f, 4.0f, 20.0f, 1.5f, 5.0f, 20.0f, 1.5f, 5.0f, 20.0f, 0.5f, 5.0f, 20.0f, -0.5f, 5.0f, 20.0f, -1.5f, 5.0f, 20.0f, -2.5f, 4.0f, 20.0f, -2.5f, 3.0f, 20.0f, -2.5f};
            this.enemy2.setParams(fArray);
        }
        if (Runtime.getFlags(3084, 1) == 1) {
            this.MJele = new Mapunits();
            this.MJele.initElevator(72, 0.055555556f, -10.0f);
            this.MJele.setArgs(1, 0, 1);
        } else {
            this.MJele = new Mapunits();
            this.MJele.initElevator(72, 0.055555556f, 0.0f);
            this.MJele.setArgs(1, 0, 1);
        }
        if (Runtime.getFlags(3084, 1) == 1) {
            this.PUPU = new Mapunits();
            this.PUPU.mapUnit(60);
            this.PUPU.start(4, null);
            this.PUPU.start(1, "Evt");
        }
        this.doorA = new Uwamono(18, 40, '\u0001');
        new Uwamono(17, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(16, 40, '\u0001');
        new Uwamono(15, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(14, 40, '\u0001');
        new Uwamono(13, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(12, 40, '\u0001');
        new Uwamono(11, 40, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(78, 40, '\u0001');
        new Uwamono(77, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0004');
        new Uwamono(28673, 3.4f, 20.0f, 0.0f, 0.0f);
        new Uwamono(28674, 3.4f, 10.0f, 0.0f, 0.0f);
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
            this.setElevatorMode(1);
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Evt() {
            ST3050.this.cam0.setMode(-1);
            ST3050.this.EV_Camera00();
            ST3050.this.player.setRotateY(0.0f);
            Runtime.setPlayerControl(false);
            ST3050.this.player.setLocation(1, 5);
            Sound.effectPlay(196749);
            ST3050.this.MJele.setArgs(12, 0.0f);
            System.sleep(180);
            ST3050.this.doorE.SetDoorType('\u0002');
            ST3050.this.doorE.DoorOpen();
            System.sleep(30);
            ST3050.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST3050.this.player.mtn(2, 9, 1.0f, true);
            ST3050.this.player.move(60, -7.463f, 1.437f, true);
            System.sleep(60);
            ST3050.this.doorE.DoorClose();
            ST3050.this.doorE.SetDoorType('\u0004');
            Runtime.setFlags(3084, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

