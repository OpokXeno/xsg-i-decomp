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
import xeno.map.MC_GNU01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1410
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU01_PRJ {
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
    Enepc enemy3;
    Enepc enemy4;
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
    Uwamono stoneA;
    Uwamono stoneB;
    Uwamono stoneC;
    Uwamono stoneD;
    Uwamono stoneE;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    MAPUnit cube1;
    int sibuki_kazu = 0;
    Effect[] sibuki;
    int[] sibuki_start;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Effect fade;
    int page;
    int b_flg;
    String[] EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};

    ST1410() {
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
            case 0: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1429, 1);
                break;
            }
        }
    }

    void evsExit() {
        System.println("evsExitをコールしました");
        if (this.b_flg == 1) {
            return;
        }
        this.b_flg = 1;
        Runtime.enable(262144);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.EVS, 0);
        ST1410.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                Runtime.evsExit();
                return;
            }
        }
        Runtime.setPlayerControl(true);
        Runtime.disable(262144);
        this.b_flg = 0;
    }

    void init() {
        new Uwamono(28734, -3.6f, 0.0f, 7.7f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.35f, 1.35f, 1.35f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.55f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        float[][] fArrayArray = new float[20][];
        float[] fArray = new float[3];
        fArray[0] = 2.4f;
        fArray[2] = 6.1f;
        fArrayArray[0] = fArray;
        float[] fArray2 = new float[3];
        fArray2[0] = -2.1f;
        fArray2[2] = 4.4f;
        fArrayArray[1] = fArray2;
        float[] fArray3 = new float[3];
        fArray3[0] = 4.3f;
        fArray3[2] = -4.5f;
        fArrayArray[2] = fArray3;
        float[] fArray4 = new float[3];
        fArray4[0] = -0.6f;
        fArray4[2] = 10.5f;
        fArrayArray[3] = fArray4;
        float[] fArray5 = new float[3];
        fArray5[0] = 4.4f;
        fArray5[2] = 14.3f;
        fArrayArray[4] = fArray5;
        float[] fArray6 = new float[3];
        fArray6[0] = -4.9f;
        fArray6[2] = 7.1f;
        fArrayArray[5] = fArray6;
        float[] fArray7 = new float[3];
        fArray7[0] = -0.8f;
        fArray7[2] = -1.5f;
        fArrayArray[6] = fArray7;
        float[] fArray8 = new float[3];
        fArray8[0] = 0.6f;
        fArray8[2] = -8.6f;
        fArrayArray[7] = fArray8;
        float[] fArray9 = new float[3];
        fArray9[0] = -4.8f;
        fArray9[2] = -7.9f;
        fArrayArray[8] = fArray9;
        float[] fArray10 = new float[3];
        fArray10[0] = -3.9f;
        fArray10[2] = -8.8f;
        fArrayArray[9] = fArray10;
        float[] fArray11 = new float[3];
        fArray11[0] = -7.8f;
        fArray11[2] = -6.6f;
        fArrayArray[10] = fArray11;
        float[] fArray12 = new float[3];
        fArray12[0] = -6.5f;
        fArray12[2] = -13.3f;
        fArrayArray[11] = fArray12;
        float[] fArray13 = new float[3];
        fArray13[0] = -7.8f;
        fArray13[2] = -15.7f;
        fArrayArray[12] = fArray13;
        float[] fArray14 = new float[3];
        fArray14[0] = -12.2f;
        fArray14[2] = -21.4f;
        fArrayArray[13] = fArray14;
        float[] fArray15 = new float[3];
        fArray15[0] = -12.2f;
        fArray15[2] = -21.9f;
        fArrayArray[14] = fArray15;
        float[] fArray16 = new float[3];
        fArray16[0] = -11.3f;
        fArray16[2] = -22.9f;
        fArrayArray[15] = fArray16;
        float[] fArray17 = new float[3];
        fArray17[0] = -2.8f;
        fArray17[2] = -15.1f;
        fArrayArray[16] = fArray17;
        float[] fArray18 = new float[3];
        fArray18[0] = -0.8f;
        fArray18[2] = -17.1f;
        fArrayArray[17] = fArray18;
        float[] fArray19 = new float[3];
        fArray19[0] = 6.7f;
        fArray19[2] = -21.8f;
        fArrayArray[18] = fArray19;
        float[] fArray20 = new float[3];
        fArray20[0] = 4.7f;
        fArray20[2] = 22.3f;
        fArrayArray[19] = fArray20;
        float[][] fArrayArray2 = fArrayArray;
        this.sibuki_kazu = fArrayArray2.length;
        this.sibuki = new Effect[this.sibuki_kazu];
        this.sibuki_start = new int[this.sibuki_kazu];
        int n = 0;
        while (n < this.sibuki_kazu) {
            this.sibuki[n] = new Effect(1496, fArrayArray2[n][0], fArrayArray2[n][1], fArrayArray2[n][2], 0.0f);
            this.sibuki[n].setClip(false);
            this.sibuki[n].disp(false);
            this.sibuki_start[n] = Math.random() % 100;
            if (this.sibuki_start[n] < 0) {
                this.sibuki_start[n] = this.sibuki_start[n] * -1;
            }
            ++n;
        }
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(40, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        float[] fArray21 = new float[12];
        fArray21[0] = -5.4f;
        fArray21[2] = -15.1f;
        fArray21[3] = 1.0f;
        fArray21[4] = -5.4f;
        fArray21[6] = -12.1f;
        fArray21[7] = 2.0f;
        fArray21[8] = -5.2f;
        fArray21[10] = -9.23f;
        fArray21[11] = -1.0f;
        float[] fArray22 = fArray21;
        this.enemy1 = new NpcEnemy(16394, 1, 1, 23, 3, -5.8f, 0.0f, -13.0f, 0.0f, fArray22);
        this.enemy1.setGroup(0, 0, 0, 1);
        float[] fArray23 = new float[8];
        fArray23[0] = 1.4f;
        fArray23[2] = -19.8f;
        fArray23[3] = 1.0f;
        fArray23[4] = -5.9f;
        fArray23[6] = -21.4f;
        fArray23[7] = -1.0f;
        float[] fArray24 = fArray23;
        this.enemy2 = new NpcEnemy(16394, 2, 2, 23, 3, 3.1f, 0.0f, -20.5f, 0.0f, fArray24);
        this.enemy2.setGroup(0, 0, 1, 1);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 152);
        this.item2 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 153);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 154);
        this.stoneA = new Uwamono(1, 58, this.item2);
        this.stoneB = new Uwamono(2, 54);
        this.stoneC = new Uwamono(3, 54, this.item3);
        this.stoneD = new Uwamono(4, 54);
        this.stoneE = new Uwamono(5, 54, this.item1);
        new Uwamono(28673, -8.2f, 0.0f, -9.4f, 0.0f);
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(40);
        this.cube1.start(1, "idle");
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
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
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            while (true) {
                if (f4 < 101.0f) {
                    int n = 0;
                    while (n < ST1410.this.sibuki_kazu) {
                        if (f4 == (float) ST1410.this.sibuki_start[n]) {
                            ST1410.this.sibuki[n].disp(true);
                        }
                        ++n;
                    }
                }
                f4 += 1.0f;
                f = Math.sin(f3 * 3.14f / 180.0f);
                f2 = Math.cos(f3 * 3.14f / 180.0f);
                ST1410.this.stoneA.setRotate(f *= 4.0f, f / 2.0f, f);
                ST1410.this.stoneA.setScale(1.0f + f / 40.0f, 1.0f + f / 40.0f, 1.0f + f / 40.0f);
                ST1410.this.stoneB.setRotate(0.0f, f2 *= 4.0f, f2);
                ST1410.this.stoneC.setRotate(f2 / 2.0f, f2 / 2.0f, f);
                ST1410.this.stoneD.setRotate(f / 2.0f, f / 2.0f, f2);
                ST1410.this.stoneE.setRotate(f2, f2, f);
                f5 = ST1410.this.cam0.getRotateX();
                f6 = ST1410.this.cam0.getRotateY();
                if ((f3 += 1.0f) == 360.0f) {
                    f3 = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

