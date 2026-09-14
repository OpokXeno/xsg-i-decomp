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
import xeno.map.MC_KAS18_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2520
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS18_PRJ {
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
    Uwamono doorA;
    Uwamono doorB;
    Uwamono box1;
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
    String[] sub_01 = new String[]{"Discovered Segment Address No. 15.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 15.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 15, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST2520() {
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
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3215, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3215, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3235, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3295, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3295, 1, 1);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken:1");
                Runtime.setFlags(8074, 1, 1);
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
                Runtime.jumpCF(2439, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(2539, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(2539, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(2539, 3);
                break;
            }
            case 4: {
                Runtime.jumpCF(2539, 4);
                break;
            }
            case 5: {
                Runtime.jumpCF(2589, 1);
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
        this.teiten1 = new Uwamono(28690, -12.0f, 0.0f, -9.5f);
        this.teiten1.SetBgm(196614);
        this.teiten2 = new Uwamono(28690, -12.0f, 0.0f, 9.5f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 25.0f, 3.0f, -3.5f);
        this.teiten3.SetBgm(196615);
        this.teiten4 = new Uwamono(28690, 25.0f, 3.0f, 0.0f);
        this.teiten4.SetBgm(196615);
        this.teiten5 = new Uwamono(28690, 25.0f, 3.0f, 3.5f);
        this.teiten5.SetBgm(196615);
        Stage.setColor(1.25f, 1.25f, 1.25f);
        this.light.setColor(0, 0.325f, 0.325f, 0.325f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, -0.3f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
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
        Runtime.setIdLightCol(3, 0, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(3, 1, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(3, 2, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightCol(3, 3, 0.375f, 0.375f, 0.425f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setShootRange(1.0f);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 298);
        new Uwamono(10, 12, this.item2);
        if (Runtime.getFlags(8074, 1) == 0) {
            this.box1 = new Uwamono(34, 126);
            this.box1.SetCallNo(1);
            this.box1.SetParticle(1429);
        } else {
            Stage.setVisible(34, false);
        }
        this.doorA = new Uwamono(5, 40, '\u0004');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(4, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        if (Runtime.getFlags(3295, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2520.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2520.waitPage(this.win, 64);
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

