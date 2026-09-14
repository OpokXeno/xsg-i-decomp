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
import xeno.map.MC_GNU02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1420
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU02_PRJ {
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    MAPUnit cube1;
    MAPUnit idou;
    int sibuki_kazu = 0;
    Effect[] sibuki;
    int[] sibuki_start;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int lo = 0;
    Effect fade;
    int page;
    String[] annai = new String[]{"It's shaped like a car.", "/[waitkey(64)]/[close()]"};

    ST1420() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (Runtime.getFlags(171, 1) != 0) break;
                Runtime.setFlags(171, 1, 1);
                System.println("イベント2050:アリアドネショッピングモールじゃないの？");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpEvent(2500);
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
                this.nwin(this.annai);
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
                Runtime.jumpCF(1419, 2);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1439, 1);
                break;
            }
        }
    }

    void init() {
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
        float[][] fArrayArray = new float[10][];
        float[] fArray = new float[3];
        fArray[0] = -5.3f;
        fArray[2] = 12.4f;
        fArrayArray[0] = fArray;
        float[] fArray2 = new float[3];
        fArray2[0] = 1.7f;
        fArray2[2] = 9.6f;
        fArrayArray[1] = fArray2;
        fArrayArray[2] = new float[]{-4.9f, 1.0f, -1.8f};
        fArrayArray[3] = new float[]{-4.1f, 2.0f, -8.3f};
        fArrayArray[4] = new float[]{4.4f, 2.0f, -19.0f};
        fArrayArray[5] = new float[]{10.2f, 2.0f, -6.4f};
        fArrayArray[6] = new float[]{12.7f, 2.0f, -8.3f};
        fArrayArray[7] = new float[]{21.2f, 2.0f, -5.6f};
        fArrayArray[8] = new float[]{30.7f, 2.1f, 5.3f};
        fArrayArray[9] = new float[]{32.3f, 4.2f, -26.6f};
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
        Stage.setVisible(19, false);
        Stage.setVisible(58, false);
        Stage.setVisible(67, false);
        this.idou = new MAPUnit();
        this.idou.mapUnit(22);
        this.idou.start(4, null);
        this.idou.getTranslate();
        this.idou.setTranslate(this.idou.px + 1.0f, this.idou.py - 0.5f, this.idou.pz);
        this.idou.setRotate(this.idou.px, this.idou.py - 30.0f, this.idou.pz);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, 4.268266f, 16.671795f, -14.320287f, 44.799866f, -75.65732f, 367.63287f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -20.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        float[] fArray3 = new float[12];
        fArray3[0] = -0.1f;
        fArray3[2] = 5.8f;
        fArray3[3] = -1.0f;
        fArray3[4] = 0.4f;
        fArray3[6] = 4.9f;
        fArray3[8] = 0.2f;
        fArray3[10] = 8.2f;
        float[] fArray4 = fArray3;
        this.enemy1 = new NpcEnemy(16394, 1, 1, 23, 5, -0.1f, 0.0f, 5.8f, 0.0f, fArray4);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[8];
        fArray5[0] = 9.5f;
        fArray5[1] = 2.1f;
        fArray5[2] = -7.8f;
        fArray5[3] = -1.0f;
        fArray5[4] = 6.7f;
        fArray5[5] = 2.0f;
        fArray5[6] = -7.8f;
        float[] fArray6 = fArray5;
        this.enemy2 = new NpcEnemy(16394, 2, 2, 23, 5, 9.5f, 2.0f, -7.8f, 0.0f, fArray6);
        float[] fArray7 = new float[]{6.7f, 2.0f, -8.8f, 11.5f, 2.1f, -8.1f};
        this.enemy2.setParams(fArray7);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray8 = new float[12];
        fArray8[0] = 22.2f;
        fArray8[1] = 2.0f;
        fArray8[2] = -4.4f;
        fArray8[3] = -1.0f;
        fArray8[4] = 20.9f;
        fArray8[5] = 2.0f;
        fArray8[6] = -6.1f;
        fArray8[8] = 25.2f;
        fArray8[9] = 2.0f;
        fArray8[10] = -4.2f;
        float[] fArray9 = fArray8;
        this.enemy3 = new NpcEnemy(16394, 3, 4, 23, 5, 22.2f, 2.0f, -4.4f, 0.0f, fArray9);
        float[] fArray10 = new float[]{20.9f, 2.0f, -6.1f, 25.2f, 2.0f, -4.2f};
        this.enemy3.setParams(fArray10);
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray11 = new float[8];
        fArray11[0] = 41.6f;
        fArray11[1] = 4.5f;
        fArray11[2] = -9.5f;
        fArray11[3] = -1.0f;
        fArray11[4] = 41.0f;
        fArray11[5] = 4.5f;
        fArray11[6] = -9.5f;
        float[] fArray12 = fArray11;
        this.enemy4 = new NpcEnemy(16391, 4, 6, 8, 3, 41.6f, 4.0f, -9.5f, 0.0f, fArray12);
        this.enemy4.setGroup(3, 3, 3, 3);
        float[] fArray13 = new float[12];
        fArray13[0] = 16.3f;
        fArray13[1] = 2.0f;
        fArray13[2] = -6.0f;
        fArray13[3] = -1.0f;
        fArray13[4] = 13.0f;
        fArray13[5] = 2.1f;
        fArray13[6] = -7.2f;
        fArray13[8] = 18.8f;
        fArray13[9] = 2.0f;
        fArray13[10] = -6.1f;
        float[] fArray14 = fArray13;
        this.enemy5 = new NpcEnemy(16391, 2, 3, 8, 3, 16.3f, 2.0f, -6.0f, 0.0f, fArray14);
        float[] fArray15 = new float[]{13.0f, 2.1f, -7.2f, 18.8f, 2.0f, -6.1f};
        this.enemy5.setParams(fArray15);
        this.enemy5.setGroup(2, 2, 2, 2);
        float[] fArray16 = new float[12];
        fArray16[0] = 30.1f;
        fArray16[1] = 2.0f;
        fArray16[2] = 2.7f;
        fArray16[3] = -1.0f;
        fArray16[4] = 26.7f;
        fArray16[5] = 2.0f;
        fArray16[6] = 2.6f;
        fArray16[8] = 31.0f;
        fArray16[9] = 2.0f;
        fArray16[10] = 2.2f;
        float[] fArray17 = fArray16;
        this.enemy6 = new NpcEnemy(16391, 2, 5, 8, 3, 30.1f, 2.0f, 2.7f, 0.0f, fArray17);
        float[] fArray18 = new float[]{26.7f, 2.0f, -2.6f, 31.0f, 2.0f, 2.2f};
        this.enemy6.setParams(fArray18);
        this.enemy6.setGroup(2, 2, 2, 2);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 155);
        new Uwamono(14, 40);
        this.item4 = new Uwamono(28677, 35.1f, 4.16f, -5.3f, 180.0f, 191);
        this.item4.SetSymbol(28684);
        new Uwamono(70, 0, this.item1);
        new Uwamono(71, 0);
        new Uwamono(28673, 13.5f, 2.0f, -9.1f, 0.0f);
        new Uwamono(28674, 33.5f, 2.0f, 2.5f, 0.0f);
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(10);
        this.cube1.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1420.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1420.waitPage(this.win, 64);
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
                    while (n < ST1420.this.sibuki_kazu) {
                        if (f4 == (float) ST1420.this.sibuki_start[n]) {
                            ST1420.this.sibuki[n].disp(true);
                        }
                        ++n;
                    }
                }
                f4 += 1.0f;
                f = Math.sin(f3 * 3.14f / 180.0f);
                f2 = Math.cos(f3 * 3.14f / 180.0f);
                f2 *= 4.0f;
                f5 = ST1420.this.cam0.getRotateX();
                f6 = ST1420.this.cam0.getRotateY();
                ST1420.this.cam0.setRotate(f5, f6, (f *= 4.0f) / 2.0f);
                if ((f3 += 1.0f) == 360.0f) {
                    f3 = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

