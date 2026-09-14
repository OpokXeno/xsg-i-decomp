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
import xeno.map.MC_GNU08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1480
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU08_PRJ {
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono col;
    Uwamono takara01;
    Uwamono senncho;
    boolean sennchobl = false;
    MAPUnit ICBM;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Effect fade;
    int page;

    ST1480() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("1");
                this.enemy1.kickEnepc(4, 2);
                this.enemy2.kickEnepc(4, 2);
                this.enemy3.kickEnepc(4, 2);
                this.enemy4.kickEnepc(4, 2);
                this.sennchobl = true;
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
                Runtime.setFlags(8018, 1, 0);
                Runtime.jumpCF(1490, 5);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1450, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -4.0f, -0.3f, 8.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, 4.0f, -0.3f, 8.0f, 0.0f);
        this.teiten2.SetBgm(196615);
        Stage.setColor(1.25f, 1.25f, 1.25f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
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
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[1] = 1.0f;
        fArray[2] = 3.8f;
        fArray[3] = 1.0f;
        fArray[5] = 1.0f;
        fArray[6] = 3.8f;
        fArray[7] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16398, 1, 1, 5, 5, 0.0f, 1.0f, 3.8f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 2);
        float[] fArray3 = new float[]{-1.6f, 1.0f, -4.3f, 1.0f, -1.6f, 1.0f, -4.0f, -1.0f};
        this.enemy2 = new NpcEnemy(16398, 2, 2, 5, 5, -1.6f, 1.0f, -4.3f, 0.0f, fArray3);
        this.enemy2.setGroup(0, 0, 1, 2);
        float[] fArray4 = new float[]{2.7f, 1.0f, -8.6f, 1.0f, 2.7f, 1.0f, -9.6f, -1.0f};
        this.enemy3 = new NpcEnemy(16398, 3, 3, 5, 5, 2.7f, 1.0f, -9.6f, 0.0f, fArray4);
        this.enemy3.setGroup(0, 0, 1, 2);
        float[] fArray5 = new float[]{-5.2f, 1.0f, -11.6f, 1.0f, -5.2f, 1.0f, -10.6f, -1.0f};
        this.enemy4 = new NpcEnemy(16391, 4, 4, 8, 3, -5.2f, 1.0f, -11.6f, 0.0f, fArray5);
        this.enemy4.setGroup(3, 3, 3, 3);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 182);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 183);
        new Uwamono(39, 3);
        new Uwamono(40, 3, this.item1);
        new Uwamono(41, 3);
        new Uwamono(42, 35, this.item2);
        if (Runtime.getFlags(8110, 1) == 0) {
            this.senncho = new Uwamono(43, 14);
            this.senncho.setTranslate(-5.1f, 2.0f, -13.9f);
            this.senncho.SetCallNo(1);
            this.takara01 = new Uwamono(28677, -5.1f, 2.8f, -13.8f, 180.0f, 196);
            this.takara01.SetSymbol(28672);
        } else {
            Stage.setVisible(43, false);
            this.takara01 = new Uwamono(28677, -5.1f, 1.0f, -13.8f, 180.0f, 196);
            this.takara01.SetSymbol(28672);
        }
        this.takara01.SetSymbol(28672);
        this.col = new Uwamono(28672, -5.1f, 1.0f, -13.8f, 0.0f);
        this.col.SetSize(0.8f, 0.8f, 0.6f);
        new Uwamono(28673, 5.0f, 1.0f, -7.9f, 0.0f);
        this.doorA = new Uwamono(36, 42, '\u0001');
        new Uwamono(35, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetDoorRange(1.7f);
        this.doorB = new Uwamono(37, 42, '\u0001');
        new Uwamono(38, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorB.SetDoorRange(1.7f);
        this.ICBM = new Mapunits();
        this.ICBM.mapUnit(18);
        this.ICBM.start(1, "idle");
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
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            int n = 300;
            if (Runtime.getFlags(8110, 1) == 0) {
                ST1480.this.senncho.getTranslate();
            }
            ST1480.this.takara01.getTranslate();
            float f6 = 0.0f;
            float f7 = 0.0f;
            while (true) {
                f = Math.sin(f3 * 3.14f / 180.0f);
                f2 = Math.cos(f3 * 3.14f / 180.0f);
                ST1480.this.ICBM.setRotate((f2 *= 2.0f) / 4.0f, f2 / 4.0f, (f *= 2.0f) / 4.0f);
                if (Runtime.getFlags(8110, 1) == 0) {
                    ST1480.this.senncho.setTranslate(ST1480.this.senncho.px, f / 10.0f + 1.0f, ST1480.this.senncho.pz);
                    if (!ST1480.this.sennchobl) {
                        ST1480.this.takara01.setTranslate(ST1480.this.takara01.px, f / 10.0f + 2.8f, ST1480.this.takara01.pz);
                    } else {
                        f5 = (-9.8f * f4 / 30.0f + 9.0f) * f4 / 30.0f + 1.8f;
                        if ((double) f5 >= 1.0) {
                            ST1480.this.takara01.setTranslate(ST1480.this.takara01.px, f5 + 1.0f, ST1480.this.takara01.pz);
                        } else {
                            ST1480.this.takara01.setTranslate(ST1480.this.takara01.px, 1.0f, ST1480.this.takara01.pz);
                            Runtime.setFlags(8110, 1, 1);
                            ST1480.this.enemy1.kickEnepc(4, 0);
                            ST1480.this.enemy2.kickEnepc(4, 0);
                            ST1480.this.enemy3.kickEnepc(4, 0);
                            ST1480.this.enemy4.kickEnepc(4, 0);
                        }
                        f4 += 1.0f;
                    }
                }
                f6 = ST1480.this.cam0.getRotateX();
                f7 = ST1480.this.cam0.getRotateY();
                ST1480.this.cam0.setRotate(f6, f7, f / 2.0f);
                if ((f3 += 1.0f) == 360.0f) {
                    f3 = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

