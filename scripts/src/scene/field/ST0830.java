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
import xeno.map.MC_PRO03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0830
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO03_PRJ {
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
    Effect light01;
    Effect light02;
    Effect light03;
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
    MAPUnit light10;
    MAPUnit light11;
    Uwamono doorA;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono box01;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int lo = 0;
    Effect fade;
    int page;
    String[] z8 = new String[]{"/[label(Ziggy)]", "An upside down cross with an elm tree...", "/[waitkey(1)]/[clear()]", "A solemn and powerful emblem. Come to think of it, this place is supposedly an old cathdral of an ancient religion.", "/[waitkey(1)]/[clear()]", "This asteroid was once the site of pilgrimages, but now that it has been abandoned, it's the perfect hiding place for the U-TIC Organization.", "/[waitkey(64)]/[close()]"};
    String[] z82 = new String[]{"It seems to be an emblem with an upside down cross and an elm tree that was used as a symbol for an ancient religion.", "/[waitkey(64)]/[close()]"};

    ST0830() {
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
                if (Runtime.getFlags(8048, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("聖堂の間イベント");
                if (Runtime.getLeader() == 6) {
                    this.nwin(this.z8);
                } else {
                    this.nwin(this.z82);
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
                Runtime.setFlags(8003, 1, 0);
                Runtime.jumpCF(820, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(840, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(860, 2);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
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
        Runtime.setIdLightCol(2, 0, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 1, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 2, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 3, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        this.light10 = new Mapunits();
        this.light10.mapUnit(15);
        this.light10.start(4, null);
        this.light10.start(1, "yurayura");
        this.light11 = new Mapunits();
        this.light11.mapUnit(16);
        this.light11.start(4, null);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFPedestal(3, 1.8260219f, 23.871622f, 1.60591f, 39.279556f, -59.919937f, 368.29843f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.light01 = new Effect(1405, -5.2f, 14.5f, 1.1f, 0.0f);
        this.light02 = new Effect(1405, 0.0f, 14.5f, 1.1f, 0.0f);
        this.light03 = new Effect(1405, 5.2f, 14.5f, 1.1f, 0.0f);
        Runtime.progressEffect(60);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 43);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 44);
        new Uwamono(28672, 0.0f, -0.64f, -8.0f, 0.0f);
        new Uwamono(192, 48, this.item2);
        new Uwamono(193, 48, this.item1);
        this.box01 = new Uwamono(222, 30);
        this.box01.SetBroken(false);
        this.item4 = new Uwamono(28677, -1.81f, 0.65f, -7.41f, 180.0f, 53);
        this.item4.SetSymbol(28683);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0830.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0830.waitPage(this.win, 64);
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

        void yurayura() {
            int n = 0;
            float f = 0.0f;
            ST0830.this.light10.getTranslate();
            ST0830.this.light11.getTranslate();
            while (true) {
                f = Math.sin((float) n * 3.14f / 180.0f);
                ST0830.this.light10.setTranslate(f / 500.0f + ST0830.this.light10.px, ST0830.this.light10.py, ST0830.this.light10.pz);
                ST0830.this.light11.setTranslate(f / 500.0f + ST0830.this.light11.px, ST0830.this.light11.py, ST0830.this.light11.pz);
                ST0830.this.light01.setTransOffset(f / 500.0f, 0.0f, 0.0f);
                System.sleep(1);
                if ((n += 2) != 10000) continue;
                n = 0;
            }
        }
    }
}

