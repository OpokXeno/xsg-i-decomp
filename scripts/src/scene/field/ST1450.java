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
import xeno.map.MC_GNU05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1450
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU05_PRJ {
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
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
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
    Uwamono boxA;
    Uwamono boxB;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Uwamono shop;
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
    Uwamono teiten1;
    int lo = 0;
    Effect fade;
    int page;
    String[] annai_d = new String[]{"'Ariadne Department Store\n", " 2F Barber Shop     BARBER PAPA\n", " 2F Flower Shop     COSTMARY", "/[waitkey(1)]/[clear()]", " 1F Restaurant      DUCK\n", " Boutique              JUN'", "/[waitkey(64)]/[close()]"};
    String[] annai = new String[]{"'Federation Ministry of Energy -- Space-Time Anomaly Research Laboratory'", "/[waitkey(64)]/[close()]"};

    ST1450() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 6) {
            Runtime.setFlags(8024, 1, 1);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (Runtime.getFlags(173, 1) != 0) break;
                Runtime.setFlags(173, 1, 1);
                System.println("イベント2052:建物かしら？");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpEvent(2520);
                break;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai_d);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 3: {
                this.player.getTranslate();
                if (this.player.py > 5.0f) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai_d);
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
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1480, 2);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1460, 4);
                break;
            }
            case 2: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1460, 5);
                break;
            }
            case 3: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1440, 2);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        float[] fArray3;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 1.0f, 4.0f, -50.5f, 90.0f);
        this.teiten1.SetBgm(196611);
        this.teiten1.SetBgmType('\u0001');
        Stage.setColor(1.35f, 1.35f, 1.35f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        float[][] fArrayArray = new float[10][];
        fArrayArray[0] = new float[]{-19.3f, 8.0f, 10.0f};
        fArrayArray[1] = new float[]{6.6f, 8.0f, -2.3f};
        fArrayArray[2] = new float[]{9.4f, 8.3f, 2.5f};
        float[] fArray4 = new float[3];
        fArray4[0] = 26.1f;
        fArray4[2] = 17.1f;
        fArrayArray[3] = fArray4;
        fArrayArray[4] = new float[]{23.1f, 12.0f, -14.3f};
        fArrayArray[5] = new float[]{45.4f, 3.8f, 9.0f};
        fArrayArray[6] = new float[]{39.3f, 4.2f, 36.4f};
        fArrayArray[7] = new float[]{3.1f, 4.0f, 30.8f};
        fArrayArray[8] = new float[]{-29.0f, 4.0f, 30.8f};
        fArrayArray[9] = new float[]{-10.4f, 4.0f, -11.2f};
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
        Stage.setVisible(44, false);
        Stage.setVisible(93, false);
        Stage.setVisible(37, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 25.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -36.5f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 16.0f, 30.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 14.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 12.0f, 30.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFLockX(9, 2.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 30.0f, 30.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 16.0f, 30.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 25.0f, 30.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        if (n2 == 1 || n2 == 2 || n2 == 5) {
            float[] fArray5 = new float[12];
            fArray5[0] = 11.6f;
            fArray5[1] = 4.0f;
            fArray5[2] = -13.8f;
            fArray5[3] = -1.0f;
            fArray5[4] = 14.9f;
            fArray5[5] = 4.0f;
            fArray5[6] = -13.8f;
            fArray5[8] = 7.0f;
            fArray5[9] = 4.0f;
            fArray5[10] = -13.8f;
            fArray3 = fArray5;
            this.enemy1 = new NpcEnemy(16389, 1, 1, 18, 5, 11.6f, 4.0f, -13.8f, 0.0f, fArray3);
            this.enemy1.setGroup(4, 4, 5, 6);
            float[] fArray6 = new float[12];
            fArray6[0] = 38.2f;
            fArray6[1] = 4.2f;
            fArray6[2] = 36.4f;
            fArray6[3] = -1.0f;
            fArray6[4] = 39.8f;
            fArray6[5] = 4.2f;
            fArray6[6] = 36.6f;
            fArray6[8] = 36.1f;
            fArray6[9] = 4.4f;
            fArray6[10] = 36.4f;
            fArray6[11] = 1.0f;
            fArray2 = fArray6;
            this.enemy2 = new NpcEnemy(16389, 2, 5, 18, 5, 38.2f, 4.2f, 36.4f, 0.0f, fArray2);
            this.enemy2.setGroup(4, 4, 5, 6);
            float[] fArray7 = new float[12];
            fArray7[0] = 7.2f;
            fArray7[1] = 4.0f;
            fArray7[2] = 38.1f;
            fArray7[3] = -1.0f;
            fArray7[4] = 9.4f;
            fArray7[5] = 4.0f;
            fArray7[6] = 36.8f;
            fArray7[8] = 4.8f;
            fArray7[9] = 4.3f;
            fArray7[10] = 37.0f;
            fArray7[11] = 1.0f;
            fArray = fArray7;
            this.enemy3 = new NpcEnemy(16391, 3, 4, 8, 3, 7.2f, 4.0f, 38.1f, 0.0f, fArray);
            this.enemy3.setGroup(4, 4, 5, 6);
            float[] fArray8 = new float[12];
            fArray8[0] = -16.7f;
            fArray8[1] = 4.0f;
            fArray8[2] = -10.5f;
            fArray8[3] = -1.0f;
            fArray8[4] = -12.7f;
            fArray8[5] = 4.0f;
            fArray8[6] = -10.5f;
            fArray8[8] = -10.7f;
            fArray8[9] = 4.0f;
            fArray8[10] = -10.5f;
            fArray8[11] = 1.0f;
            float[] fArray9 = fArray8;
            this.enemy8 = new NpcEnemy(16391, 8, 8, 8, 3, -16.7f, 4.0f, -10.5f, 0.0f, fArray9);
            this.enemy8.setGroup(4, 4, 5, 6);
        }
        if (n2 == 0 || n2 == 3 || n2 == 4) {
            float[] fArray10 = new float[12];
            fArray10[0] = 8.2f;
            fArray10[1] = 8.2f;
            fArray10[2] = 1.3f;
            fArray10[3] = -1.0f;
            fArray10[4] = 5.8f;
            fArray10[5] = 8.2f;
            fArray10[6] = 1.3f;
            fArray10[8] = 8.2f;
            fArray10[9] = 8.1f;
            fArray10[10] = 4.8f;
            fArray10[11] = 1.0f;
            fArray3 = fArray10;
            this.enemy4 = new NpcEnemy(16391, 4, 6, 8, 3, 8.2f, 8.2f, 1.3f, 0.0f, fArray3);
            this.enemy4.setGroup(1, 1, 2, 3);
            float[] fArray11 = new float[12];
            fArray11[0] = 30.3f;
            fArray11[1] = 0.3f;
            fArray11[2] = 14.1f;
            fArray11[3] = -1.0f;
            fArray11[4] = 28.3f;
            fArray11[5] = 0.3f;
            fArray11[6] = 13.8f;
            fArray11[8] = 32.6f;
            fArray11[10] = 12.8f;
            fArray11[11] = 1.0f;
            fArray2 = fArray11;
            this.enemy5 = new NpcEnemy(16391, 5, 3, 8, 3, 30.3f, 0.3f, 14.1f, 0.0f, fArray2);
            this.enemy5.setGroup(1, 1, 2, 3);
            float[] fArray12 = new float[12];
            fArray12[0] = -17.3f;
            fArray12[1] = 7.6f;
            fArray12[2] = 5.5f;
            fArray12[3] = -1.0f;
            fArray12[4] = -15.7f;
            fArray12[5] = 7.8f;
            fArray12[6] = 5.1f;
            fArray12[8] = -20.9f;
            fArray12[9] = 7.7f;
            fArray12[10] = 4.7f;
            fArray12[11] = 1.0f;
            fArray = fArray12;
            this.enemy7 = new NpcEnemy(16389, 7, 2, 13, 5, -17.3f, 7.6f, 5.5f, 0.0f, fArray);
            this.enemy7.setGroup(1, 1, 2, 3);
        }
        if (Runtime.getFlags(8024, 1) == 0) {
            this.enemy6 = new NpcEnemy(20235, 6, 0, 28, 7, -34.1f, 4.0f, 25.0f, 0.0f);
            this.enemy6.setGroup(0, 0, 0, 0);
            this.enemy6.setBatEvent(8);
        }
        new Uwamono(28678, 35.2f, 0.0f, -3.6f);
        this.shop = new Uwamono(28679, 35.2f, 0.0f, -1.6f);
        this.shop.SetShopNo(5);
        this.item1 = new Uwamono(28677, -34.4f, 4.02f, 22.5f, 180.0f, 418);
        this.item1.SetCallNo(1);
        this.item1.SetSymbol(28686);
        this.item6 = new Uwamono(28677, -18.1f, 4.0f, -14.7f, 180.0f, 194);
        this.item6.SetSymbol(28672);
        this.item2 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 162);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 164);
        this.item4 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 165);
        this.item5 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 166);
        this.stoneA = new Uwamono(18, 13, this.item4);
        this.stoneB = new Uwamono(19, 13);
        this.stoneC = new Uwamono(20, 13);
        this.stoneD = new Uwamono(21, 13, this.item5);
        this.stoneE = new Uwamono(22, 13);
        this.boxA = new Uwamono(23, 4, this.item3);
        this.boxB = new Uwamono(24, 4, this.item2);
        new Uwamono(25, 40);
        new Uwamono(28674, 1.3f, 8.0f, 1.1f, 0.0f);
        this.doorA = new Uwamono(94, 42, '\u0001');
        new Uwamono(95, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(96);
        this.cube1.start(1, "idle");
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3229, 1, 1);
                break;
            }
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1450.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1450.waitPage(this.win, 64);
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
                    while (n < ST1450.this.sibuki_kazu) {
                        if (f4 == (float) ST1450.this.sibuki_start[n]) {
                            ST1450.this.sibuki[n].disp(true);
                        }
                        ++n;
                    }
                }
                f4 += 1.0f;
                f = Math.sin(f3 * 3.14f / 180.0f);
                f2 = Math.cos(f3 * 3.14f / 180.0f);
                ST1450.this.stoneA.setRotate(f *= 4.0f, f / 2.0f, f);
                ST1450.this.stoneB.setRotate(0.0f, f2 *= 4.0f, f2);
                ST1450.this.stoneC.setRotate(f2 / 2.0f, f2 / 2.0f, f);
                ST1450.this.stoneD.setRotate(f / 2.0f, f / 2.0f, f2);
                ST1450.this.boxA.setRotate(f2, f2, f);
                ST1450.this.boxB.setRotate(f2, f2, f);
                f5 = ST1450.this.cam0.getRotateX();
                f6 = ST1450.this.cam0.getRotateY();
                ST1450.this.cam0.setRotate(f5, f6, f / 2.0f);
                if ((f3 += 1.0f) == 360.0f) {
                    f3 = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

