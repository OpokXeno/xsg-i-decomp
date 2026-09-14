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
import xeno.map.MC_GNK09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3030
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK09_PRJ {
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
    Unit UTIC01;
    Unit UTIC02;
    Unit UTIC03;
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
    Uwamono doorA;
    Uwamono doorB;
    Uwamono itembox;
    Uwamono Dummy;
    Uwamono Kow01;
    Uwamono Kow02;
    Uwamono Kow03;
    Uwamono Kow04;
    Uwamono Kow05;
    Uwamono Kow06;
    Uwamono Kow07;
    Uwamono Kow08;
    Effect fade;
    Light light = new Light(0);
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int page;

    ST3030() {
    }

    void EOB(int n) {
        if (n == 5) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3179, 1, 1);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(521, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68576, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        this.UTIC01 = new Mapunits();
        this.UTIC01.init(20554, 0.0f, -4.0f, 8.5f, 90.0f);
        this.UTIC01.setScale(0.8f, 0.8f, 0.8f);
        this.UTIC02 = new Mapunits();
        this.UTIC02.init(20554, 0.0f, -4.0f, 0.0f, 90.0f);
        this.UTIC02.setScale(0.8f, 0.8f, 0.8f);
        this.UTIC03 = new Mapunits();
        this.UTIC03.init(20554, 0.0f, -4.0f, -10.0f, 90.0f);
        this.UTIC03.setScale(0.8f, 0.8f, 0.8f);
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
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
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
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(2, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(2, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(2, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 8.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, -15.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, -15.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16647, 4, -9.0f, 0.0f, -20.0f, 270.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[8];
        fArray[0] = -9.0f;
        fArray[2] = -20.0f;
        fArray[3] = -1.0f;
        fArray[4] = -2.0f;
        fArray[6] = -20.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 4, fArray2);
        float[] fArray3 = new float[12];
        fArray3[0] = -9.0f;
        fArray3[2] = -20.0f;
        fArray3[3] = -5.0f;
        fArray3[5] = -20.0f;
        fArray3[6] = -2.0f;
        fArray3[8] = -20.0f;
        fArray3[9] = -5.0f;
        fArray3[11] = -20.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(16647, 4, -1.5f, 0.0f, -16.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 1, 1, 1);
        float[] fArray5 = new float[16];
        fArray5[0] = -1.5f;
        fArray5[2] = -16.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -1.5f;
        fArray5[6] = -12.0f;
        fArray5[8] = -1.5f;
        fArray5[10] = -7.0f;
        fArray5[11] = 1.0f;
        fArray5[12] = -1.5f;
        fArray5[14] = -2.0f;
        fArray5[15] = 2.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 4, fArray6);
        float[] fArray7 = new float[18];
        fArray7[0] = -1.5f;
        fArray7[2] = -16.0f;
        fArray7[3] = -1.5f;
        fArray7[5] = -12.0f;
        fArray7[6] = -1.5f;
        fArray7[8] = -7.0f;
        fArray7[9] = -1.5f;
        fArray7[11] = -2.0f;
        fArray7[12] = -1.5f;
        fArray7[14] = -7.0f;
        fArray7[15] = -1.5f;
        fArray7[17] = -12.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(16647, 4, -2.0f, 0.0f, 16.0f, 180.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray9 = new float[16];
        fArray9[0] = -2.0f;
        fArray9[2] = 16.0f;
        fArray9[3] = -1.0f;
        fArray9[4] = -2.0f;
        fArray9[6] = 12.0f;
        fArray9[8] = -2.0f;
        fArray9[10] = 7.0f;
        fArray9[11] = 1.0f;
        fArray9[12] = -2.0f;
        fArray9[14] = 2.0f;
        fArray9[15] = 2.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 1, 3, 4, fArray10);
        float[] fArray11 = new float[18];
        fArray11[0] = -2.0f;
        fArray11[2] = 16.0f;
        fArray11[3] = -2.0f;
        fArray11[5] = 12.0f;
        fArray11[6] = -2.0f;
        fArray11[8] = 7.0f;
        fArray11[9] = -2.0f;
        fArray11[11] = 2.0f;
        fArray11[12] = -2.0f;
        fArray11[14] = 7.0f;
        fArray11[15] = -2.0f;
        fArray11[17] = 12.0f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy4 = new Enepc();
        this.enemy4.init(16647, 4, -8.0f, -4.0f, -15.4f, 270.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(2, 2, 2, 2);
        float[] fArray13 = new float[8];
        fArray13[0] = -8.0f;
        fArray13[1] = -4.0f;
        fArray13[2] = -15.4f;
        fArray13[3] = -1.0f;
        fArray13[4] = -7.0f;
        fArray13[5] = -4.0f;
        fArray13[6] = -15.4f;
        float[] fArray14 = fArray13;
        this.enemy4.setParams(1, 1, 4, 4, fArray14);
        float[] fArray15 = new float[]{-8.0f, -4.0f, -15.4f, -7.0f, -4.0f, -15.4f};
        this.enemy4.setParams(fArray15);
        if (Runtime.getFlags(3179, 1) == 0) {
            this.enemy5 = new Enepc();
            this.enemy5.init(16403, 6, 3.0f, -4.0f, 19.25f, 180.0f);
            this.enemy5.id = 5;
            this.enemy5.setGroup(3, 3, 3, 3);
            float[] fArray16 = new float[40];
            fArray16[0] = 3.0f;
            fArray16[1] = -4.0f;
            fArray16[2] = 19.25f;
            fArray16[3] = 1.0f;
            fArray16[4] = 2.0f;
            fArray16[5] = -4.0f;
            fArray16[6] = 19.25f;
            fArray16[7] = 2.0f;
            fArray16[8] = 1.0f;
            fArray16[9] = -4.0f;
            fArray16[10] = 19.25f;
            fArray16[11] = 3.0f;
            fArray16[13] = -4.0f;
            fArray16[14] = 19.25f;
            fArray16[15] = 4.0f;
            fArray16[16] = -1.0f;
            fArray16[17] = -4.0f;
            fArray16[18] = 19.25f;
            fArray16[19] = 5.0f;
            fArray16[20] = -2.0f;
            fArray16[21] = -4.0f;
            fArray16[22] = 19.25f;
            fArray16[23] = 6.0f;
            fArray16[24] = -3.0f;
            fArray16[25] = -4.0f;
            fArray16[26] = 19.25f;
            fArray16[27] = 7.0f;
            fArray16[28] = -4.0f;
            fArray16[29] = -4.0f;
            fArray16[30] = 19.25f;
            fArray16[31] = 8.0f;
            fArray16[32] = -5.0f;
            fArray16[33] = -4.0f;
            fArray16[34] = 19.25f;
            fArray16[35] = 9.0f;
            fArray16[36] = -5.8f;
            fArray16[37] = -4.0f;
            fArray16[38] = 20.25f;
            fArray16[39] = -1.0f;
            float[] fArray17 = fArray16;
            this.enemy5.setShadow(0, 0);
            this.enemy5.setParams(1, 0, 5, 6, fArray17);
        }
        this.itembox = new Uwamono(28677, 1.727f, 0.0f, -6.325f, 180.0f, 380);
        this.itembox.SetSymbol(28684);
        this.Dummy = new Uwamono(28672, -7.5f, -5.0f, -17.0f, 0.0f);
        this.Dummy.SetSize(5.0f, 1.0f, 5.0f);
        this.Kow01 = new Uwamono(32, 4);
        this.Kow02 = new Uwamono(33, 4);
        this.Kow03 = new Uwamono(34, 4);
        this.Kow04 = new Uwamono(35, 4);
        this.Kow05 = new Uwamono(37, 4);
        this.Kow01.SetSize(1.5f, 1.0f, 1.5f);
        this.Kow02.SetSize(1.5f, 1.0f, 1.5f);
        this.Kow03.SetSize(1.5f, 1.0f, 1.5f);
        this.Kow04.SetSize(1.5f, 1.0f, 1.5f);
        this.Kow05.SetSize(1.5f, 1.0f, 1.5f);
        new Uwamono(38, 1);
        new Uwamono(39, 11);
        new Uwamono(40, 33, this.itembox);
        new Uwamono(41, 1);
        this.Kow06 = Runtime.getFlags(3179, 1) == 0 ? new Uwamono(36, 4, this.enemy5) : new Uwamono(36, 4);
        this.Kow06.SetSize(1.5f, 1.0f, 1.5f);
        new Uwamono(28673, -5.4f, -4.0f, 18.4f, 0.0f);
        new Uwamono(28674, -1.0f, 0.0f, -22.2f, 0.0f);
        this.doorA = new Uwamono(80, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(73, 40, '\u0004');
        this.doorB.SetDoorType('\u0004');
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

