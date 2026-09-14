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
import xeno.map.MC_KAS12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2460
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS12_PRJ {
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
    MAPUnit s_unit;
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
    Effect light11;
    Effect light12;
    Effect light13;
    Effect light14;
    Effect light15;
    Effect light16;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono co01;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] u_omote = new String[]{"A statue of the park mascot is on the ground.", "/[waitkey(64)]/[close()]"};
    String[] u_omote2 = new String[]{"/[label(Shion)]", "I've always liked this character since I was little. At first glance, it looks very cute, but there's something more to its allure than that.", "/[waitkey(64)]/[close()]"};
    String[] u_ura = new String[]{"/[label(Shion)]", "No matter how many times I see it, it still creeps me out.", "/[waitkey(64)]/[close()]"};

    ST2460() {
    }

    void EOB(int n) {
        if (n == 0) {
            System.println("EOB:0");
        }
        if (n == 1) {
            Runtime.setFlags(8083, 1, 1);
        }
    }

    void Final_init(int n) {
        this.npc1.kickEnepc(10, 200, 0);
        if (Runtime.getFlags(8083, 1) == 0) {
            this.enemy1.kickEnepc(10, 60, 0);
        }
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
                if (Runtime.getFlags(8083, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.enemy1.kickEnepc(4, 0);
                System.println("lo:0");
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.u_omote);
                if (Runtime.getLeader() == 1) {
                    this.nwin(this.u_omote2);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
                if (Runtime.getLeader() != 1) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.u_ura);
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
                System.println("公園【MC_KAS31】・１");
                Runtime.jumpCF(2560, 2);
                break;
            }
            case 1: {
                System.println("瓦礫２【MC_KAS13】・１");
                Runtime.jumpCF(2470, 1);
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
        this.s_unit = new Mapunits();
        this.s_unit.mapUnit(0);
        this.s_unit.start(4, null);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, -0.5f, 1.0f, 0.5f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, -0.5f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        float f = 6.7f;
        this.light01 = new Effect(1631, -17.7f, f, -3.5f, 0.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1631, -8.9f, f, -13.9f, 0.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1631, -5.4f, f, -17.4f, 0.0f);
        this.light03.disp(true);
        this.light04 = new Effect(1631, -1.8f, f, -21.0f, 0.0f);
        this.light04.disp(true);
        this.light05 = new Effect(1631, 1.6f, f, -24.5f, 0.0f);
        this.light05.disp(true);
        this.light06 = new Effect(1631, 5.2f, f, -28.1f, 0.0f);
        this.light06.disp(true);
        this.light07 = new Effect(1631, 10.6f, f, -30.8f, 0.0f);
        this.light07.disp(true);
        this.light08 = new Effect(1631, -7.8f, f, -3.5f, 0.0f);
        this.light08.disp(true);
        this.light09 = new Effect(1631, -2.3f, f, -7.4f, 0.0f);
        this.light09.disp(true);
        this.light10 = new Effect(1631, 1.1f, f, -10.9f, 0.0f);
        this.light10.disp(true);
        this.light11 = new Effect(1631, 4.7f, f, -14.4f, 0.0f);
        this.light11.disp(true);
        this.light12 = new Effect(1631, 8.2f, f, -18.0f, 0.0f);
        this.light12.disp(true);
        this.light13 = new Effect(1631, 19.6f, f, -26.7f, 0.0f);
        this.light13.disp(true);
        this.light14 = new Effect(1631, 23.7f, f, -17.7f, 0.0f);
        this.light14.disp(true);
        this.light15 = new Effect(1631, 7.8f, 2.7f, -0.7f, 0.0f);
        this.light15.disp(true);
        this.light16 = new Effect(1631, 3.5f, 2.7f, -0.7f, 0.0f);
        this.light16.disp(true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(45, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 6.5f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.npc1 = new NpcEnemy(8705, 11, 0, 4, 3, 8.1f, 0.0f, -5.1f, 0.0f);
        this.npc1.kickEnepc(10, 200, 0);
        this.npc1.setInvalidID(1);
        this.npc1.setVisible(3, false);
        this.npc1.setRotate(-30.0f, 1.0f, 90.0f);
        this.npc1.setTranslate(this.npc1.px, 0.9f, this.npc1.pz);
        this.npc1.dispRadar(false);
        this.npc1.disableDTKFlag(65536);
        this.co01 = new Uwamono(28672, 6.6f, 0.0f, -5.4f, 10.0f);
        this.co01.SetSize(2.7f, 1.0f, 1.5f);
        if (Runtime.getFlags(8083, 1) == 0) {
            float[] fArray = new float[28];
            fArray[0] = 2.7f;
            fArray[1] = 4.5f;
            fArray[2] = -23.7f;
            fArray[3] = -1.0f;
            fArray[4] = 3.47f;
            fArray[5] = 4.0f;
            fArray[6] = -22.85f;
            fArray[8] = 6.7f;
            fArray[9] = 4.0f;
            fArray[10] = -19.6f;
            fArray[11] = 1.0f;
            fArray[12] = 3.22f;
            fArray[13] = 4.0f;
            fArray[14] = -16.85f;
            fArray[15] = 2.0f;
            fArray[16] = 10.23f;
            fArray[17] = 4.0f;
            fArray[18] = -23.83f;
            fArray[19] = 2.0f;
            fArray[20] = 13.9f;
            fArray[21] = 4.0f;
            fArray[22] = -26.8f;
            fArray[23] = 4.0f;
            fArray[24] = 7.4f;
            fArray[25] = 4.0f;
            fArray[26] = -26.5f;
            fArray[27] = 4.0f;
            float[] fArray2 = fArray;
            this.enemy1 = new NpcEnemy(16401, 1, 1, 44, 5, 2.4f, 4.5f, -23.7f, 45.0f, fArray2);
            this.enemy1.kickEnepc(10, 46, 0);
            this.enemy1.setGroup(0, 0, 0, 0);
        }
        new Uwamono(28678, -8.3f, 4.0f, -0.6f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 280);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 281);
        this.item3 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 282);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 283);
        this.item5 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 284);
        new Uwamono(19, 13, this.item1);
        new Uwamono(20, 13);
        new Uwamono(21, 13, this.item2);
        new Uwamono(22, 13, this.item3);
        new Uwamono(23, 13);
        new Uwamono(24, 13, this.item4);
        new Uwamono(25, 13);
        new Uwamono(26, 13, this.item5);
        this.s_unit.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2460.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2460.waitPage(this.win, 64);
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
            if (Runtime.getFlags(8083, 1) == 0) {
                ST2460.this.enemy1.kickEnepc(4, 2);
            }
            ST2460.this.npc1.kickEnepc(4, 2);
        }
    }
}

