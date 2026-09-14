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
import xeno.map.MC_GNU06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1460
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU06_PRJ {
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
    Unit unit1;
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
    Uwamono tA;
    Uwamono tB;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    MAPUnit naka01;
    MAPUnit naka02;
    MAPUnit naka03;
    MAPUnit soto01;
    MAPUnit soto02;
    MAPUnit soto03;
    MAPUnit ele;
    MAPUnit tane;
    Unit elv;
    Effect light01;
    Effect light02;
    Effect light03;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int elemove = 0;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Effect fade;
    int page;
    String[] ele_s = new String[]{"Operate elevator?", "/[waitkey(64)]/[close()]"};

    ST1460() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            Runtime.setFlags(8025, 1, 1);
        }
        if (n == 2) {
            System.sleep(1);
            Runtime.setFlags(8027, 1, 1);
            if (Runtime.getFlags(3235, 1) == 0) {
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 30);
                Runtime.setFlags(3235, 1, 1);
            }
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
                if (this.elemove == 0) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    this.nwin(this.ele_s);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            System.println("エレベータ");
                            this.elemove = 1;
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (Runtime.getFlags(8117, 1) == 0) {
                    this.light01.disp(false);
                    Sound.effectPlay(6);
                    Runtime.setFlags(8117, 1, 1);
                    Runtime.addItemWin(10, 5);
                    this.tane.setTranslate(0.0f, -999.0f, 0.0f);
                    System.println("種");
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1479, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1479, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(1479, 4);
                break;
            }
            case 3: {
                Runtime.jumpCF(1459, 2);
                break;
            }
            case 4: {
                Runtime.jumpCF(1459, 3);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, 4.0f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, 0.0f, 4.0f, -8.0f, 0.0f);
        this.teiten2.SetBgm(196613);
        if (Runtime.getFlags(8117, 1) == 0) {
            this.light01 = new Effect(1018, 13.0f, 5.6f, -20.2f, 0.0f);
            this.light01.disp(true);
        }
        Stage.setColor(1.2f, 1.2f, 1.2f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 350.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 10.5f);
        this.cam0.setCFPedestal(6, 10.5f, 11.0f, -17.0f, 45.0f, -65.0f, -15.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(6, 1);
        this.cam0.setCFAngle(7, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        if (Runtime.getFlags(8025, 1) == 0) {
            this.enemy1 = new NpcEnemy(20234, 1, 0, 28, 5, 8.6f, 0.0f, 5.5f, -90.0f);
            this.enemy1.setGroup(0, 0, 0, 0);
            this.enemy1.setBatEvent(8);
        }
        if (Runtime.getFlags(8027, 1) == 0) {
            float[] fArray = new float[]{10.4f, 4.0f, -11.5f, 1.0f, 10.4f, 4.0f, -13.7f, -1.0f};
            this.enemy2 = new NpcEnemy(16397, 2, 0, 16, 3, 9.8f, 4.0f, -17.1f, 0.0f, fArray);
            this.enemy2.setGroup(1, 1, 1, 1);
        }
        this.naka01 = new Mapunits();
        this.naka01.mapUnit(113);
        this.naka01.start(4, null);
        this.naka02 = new Mapunits();
        this.naka02.mapUnit(114);
        this.naka02.start(4, null);
        this.naka03 = new Mapunits();
        this.naka03.mapUnit(112);
        this.naka03.start(4, null);
        this.soto01 = new Mapunits();
        this.soto01.mapUnit(243);
        this.soto01.start(4, null);
        this.soto02 = new Mapunits();
        this.soto02.mapUnit(242);
        this.soto02.start(4, null);
        this.soto03 = new Mapunits();
        this.soto03.mapUnit(241);
        this.soto03.start(4, null);
        this.elv = new Unit();
        this.elv.initElevator(2, 10.1f, 4.0f);
        this.elv.setArgs(12, 4.01f);
        if (Runtime.getFlags(8016, 1) == 1) {
            this.elv.setArgs(12, 0.0f);
        }
        this.tane = new Mapunits();
        this.tane.mapUnit(5);
        this.tane.start(4, null);
        if (Runtime.getFlags(8117, 1) == 1) {
            Stage.setVisible(5, false);
        }
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 167);
        this.item2 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 168);
        new Uwamono(10, 13, this.item1);
        new Uwamono(244, 4, this.item2);
        this.tA = new Uwamono(28672, -6.9f, 4.0f, -5.7f, 0.0f);
        this.tA.SetSize(2.0f, 1.0f, 0.2f);
        this.tB = new Uwamono(28672, -6.9f, 0.0f, -8.3f, 0.0f);
        this.tB.SetSize(2.0f, 1.0f, 0.2f);
        this.naka01.start(1, "idle");
        Runtime.setRegister(1, this.elv.py);
        System.println("elv.py: /[#1]");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1460.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1460.waitPage(this.win, 64);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            float f2 = 0.0f;
            ST1460.this.naka01.getTranslate();
            ST1460.this.naka02.getTranslate();
            ST1460.this.naka03.getTranslate();
            ST1460.this.soto01.getTranslate();
            ST1460.this.soto02.getTranslate();
            ST1460.this.soto03.getTranslate();
            ST1460.this.soto03.setTranslate(ST1460.this.soto03.px, ST1460.this.soto03.py - 5.0f, ST1460.this.soto03.pz);
            ST1460.this.naka03.setTranslate(ST1460.this.naka03.px, ST1460.this.naka03.py - 5.0f, ST1460.this.naka03.pz);
            while (true) {
                ST1460.this.soto01.setTranslate(ST1460.this.soto01.px, Math.sin(f * 3.14f / 180.0f) * 4.0f + 2.0f, ST1460.this.soto01.pz);
                ST1460.this.naka01.setTranslate(ST1460.this.naka01.px, Math.sin((f + 20.0f) * 3.14f / 180.0f) * 5.0f + 2.0f, ST1460.this.naka01.pz);
                ST1460.this.naka01.setRotateY(Math.sin((f + 180.0f) * 3.14f / 180.0f) * 1000.0f);
                ST1460.this.soto02.setTranslate(ST1460.this.soto02.px, Math.cos(f * 3.14f / 180.0f) * 4.0f - 1.0f, ST1460.this.soto02.pz);
                ST1460.this.naka02.setTranslate(ST1460.this.naka02.px, Math.cos((f + 20.0f) * 3.14f / 180.0f) * 5.0f - 1.0f, ST1460.this.naka02.pz);
                ST1460.this.naka02.setRotateY(Math.cos(f * 3.14f / 180.0f) * 1000.0f);
                ST1460.this.soto03.setTranslate(ST1460.this.soto03.px, Math.sin((f + 180.0f) * 3.14f / 180.0f) * 3.5f, ST1460.this.soto03.pz);
                ST1460.this.naka03.setTranslate(ST1460.this.naka03.px, Math.sin((f + 200.0f) * 3.14f / 180.0f) * 4.0f, ST1460.this.naka03.pz);
                ST1460.this.naka03.setRotateY(Math.sin(f * 3.14f / 180.0f) * 1000.0f);
                if (ST1460.this.elemove == 1) {
                    if (Runtime.getFlags(8016, 1) == 0) {
                        if (f2 == 0.0f) {
                            Sound.effectPlay(196742);
                        }
                        ST1460.this.elv.setArgs(12, Math.sin(f2 * 3.14f / 180.0f) * 2.0f + 2.0f);
                        f2 += 1.0f;
                        f2 += 1.0f;
                        if (f2 > 180.0f) {
                            ST1460.this.elemove = 0;
                            f2 = 0.0f;
                            Runtime.setFlags(8016, 1, 1);
                            Runtime.setPlayerControl(true);
                        }
                    } else {
                        if (f2 == 0.0f) {
                            Sound.effectPlay(196742);
                        }
                        ST1460.this.elv.setArgs(12, -Math.sin(f2 * 3.14f / 180.0f) * 2.0f + 2.0f);
                        f2 += 1.0f;
                        f2 += 1.0f;
                        if (f2 > 180.0f) {
                            ST1460.this.elemove = 0;
                            f2 = 0.0f;
                            Runtime.setFlags(8016, 1, 0);
                            Runtime.setPlayerControl(true);
                        }
                    }
                }
                if ((f += 1.0f) == 360.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

