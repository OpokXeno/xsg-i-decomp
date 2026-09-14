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
import xeno.map.MC_GNK12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3060
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK12_PRJ {
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
    Unit unit1;
    Effect E01;
    Effect E02;
    Effect E03;
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
    Uwamono trap1;
    Uwamono trap2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int page;

    ST3060() {
    }

    void EOB(int n) {
        if (n == 1) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3183, 1, 1);
        }
        if (n == 2) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3184, 1, 1);
        }
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
                Runtime.jumpCF(68606, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(68616, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(68626, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(68656, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1640, -7.462f, 14.445f, -0.507f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E02 = new Effect(1640, -7.462f, 24.445f, -0.507f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E03 = new Effect(1640, -7.462f, 34.445f, -0.507f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
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
        this.cam0.setCFAngle(2, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        if (Runtime.getFlags(3183, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16396, 3, -2.0f, 10.0f, 1.7f, 180.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(2, 2, 3, 3);
            float[] fArray3 = new float[32];
            fArray3[0] = -2.0f;
            fArray3[1] = 10.0f;
            fArray3[2] = 1.7f;
            fArray3[3] = -1.0f;
            fArray3[4] = -3.0f;
            fArray3[5] = 10.0f;
            fArray3[6] = 1.7f;
            fArray3[8] = -4.0f;
            fArray3[9] = 10.0f;
            fArray3[10] = 1.7f;
            fArray3[11] = 1.0f;
            fArray3[12] = -5.0f;
            fArray3[13] = 10.0f;
            fArray3[14] = 0.5f;
            fArray3[15] = 2.0f;
            fArray3[16] = -5.0f;
            fArray3[17] = 10.0f;
            fArray3[18] = -0.5f;
            fArray3[19] = 3.0f;
            fArray3[20] = -5.0f;
            fArray3[21] = 10.0f;
            fArray3[22] = -1.5f;
            fArray3[23] = 4.0f;
            fArray3[24] = -4.0f;
            fArray3[25] = 10.0f;
            fArray3[26] = -2.5f;
            fArray3[27] = 5.0f;
            fArray3[28] = -3.0f;
            fArray3[29] = 10.0f;
            fArray3[30] = -2.5f;
            fArray3[31] = 6.0f;
            fArray2 = fArray3;
            this.enemy1.setParams(1, 1, 1, 3, fArray2);
            fArray = new float[]{-2.0f, 10.0f, 1.7f, -3.0f, 10.0f, 1.7f, -4.0f, 10.0f, 1.7f, -5.0f, 10.0f, 0.5f, -5.0f, 10.0f, -0.5f, -5.0f, 10.0f, -1.5f, -4.0f, 10.0f, -2.5f, -3.0f, 10.0f, -2.5f, -2.0f, 10.0f, -2.5f};
            this.enemy1.setParams(fArray);
            this.enemy1.kickEnepc(10, 85, 0);
        }
        if (Runtime.getFlags(3184, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16396, 3, -2.0f, 20.0f, 1.7f, 0.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(2, 2, 3, 3);
            float[] fArray4 = new float[32];
            fArray4[0] = -2.0f;
            fArray4[1] = 20.0f;
            fArray4[2] = 1.7f;
            fArray4[3] = -1.0f;
            fArray4[4] = -3.0f;
            fArray4[5] = 20.0f;
            fArray4[6] = 1.7f;
            fArray4[8] = -4.0f;
            fArray4[9] = 20.0f;
            fArray4[10] = 1.7f;
            fArray4[11] = 1.0f;
            fArray4[12] = -5.0f;
            fArray4[13] = 20.0f;
            fArray4[14] = 0.5f;
            fArray4[15] = 2.0f;
            fArray4[16] = -5.0f;
            fArray4[17] = 20.0f;
            fArray4[18] = -0.5f;
            fArray4[19] = 3.0f;
            fArray4[20] = -5.0f;
            fArray4[21] = 20.0f;
            fArray4[22] = -1.5f;
            fArray4[23] = 4.0f;
            fArray4[24] = -4.0f;
            fArray4[25] = 20.0f;
            fArray4[26] = -2.5f;
            fArray4[27] = 5.0f;
            fArray4[28] = -3.0f;
            fArray4[29] = 20.0f;
            fArray4[30] = -2.5f;
            fArray4[31] = 6.0f;
            fArray2 = fArray4;
            this.enemy2.setParams(1, 1, 2, 3, fArray2);
            fArray = new float[]{-2.0f, 20.0f, 1.7f, -3.0f, 20.0f, 1.7f, -4.0f, 20.0f, 1.7f, -5.0f, 20.0f, 0.5f, -5.0f, 20.0f, -0.5f, -5.0f, 20.0f, -1.5f, -4.0f, 20.0f, -2.5f, -3.0f, 20.0f, -2.5f};
            this.enemy2.setParams(fArray);
            this.enemy2.kickEnepc(10, 85, 0);
        }
        this.doorA = new Uwamono(12, 40, '\u0001');
        new Uwamono(11, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(6, 40, '\u0001');
        new Uwamono(5, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(10, 40, '\u0001');
        new Uwamono(9, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(8, 40, '\u0001');
        new Uwamono(7, 40, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.trap1 = new Uwamono(28675, -3.4f, 20.0f, -0.5f, 90.0f);
        this.trap2 = new Uwamono(28675, -3.4f, 10.0f, -0.5f, 90.0f);
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
    }
}

