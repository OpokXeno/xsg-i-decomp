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
import xeno.map.MC_GNK22_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3160
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK22_PRJ {
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
    Effect light01;
    Effect light02;
    Effect light03;
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int page;

    ST3160() {
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
                Runtime.jumpCF(3170, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68556, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
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
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setFog(0, 20.0f, 50.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(1, 20.0f, 50.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 14.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, 6.0f, 6.0f, 7.9f, 45.0f, -2.7f, 17.46f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16404, 3, -0.018f, 0.0f, 18.63f, 180.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(5, 5, 6, 6);
        float[] fArray = new float[32];
        fArray[0] = -0.018f;
        fArray[2] = 18.63f;
        fArray[3] = 1.0f;
        fArray[4] = -0.758f;
        fArray[6] = 17.352f;
        fArray[7] = 2.0f;
        fArray[8] = 0.137f;
        fArray[10] = 16.322f;
        fArray[11] = 3.0f;
        fArray[12] = 0.753f;
        fArray[14] = 15.167f;
        fArray[15] = 4.0f;
        fArray[16] = 1.216f;
        fArray[18] = 13.921f;
        fArray[19] = 5.0f;
        fArray[20] = 1.37f;
        fArray[22] = 12.211f;
        fArray[23] = 6.0f;
        fArray[24] = 1.493f;
        fArray[26] = 10.379f;
        fArray[27] = 7.0f;
        fArray[28] = 1.524f;
        fArray[30] = 8.7f;
        fArray[31] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[24];
        fArray3[0] = -0.018f;
        fArray3[2] = 18.63f;
        fArray3[3] = -0.758f;
        fArray3[5] = 17.352f;
        fArray3[6] = 0.137f;
        fArray3[8] = 16.322f;
        fArray3[9] = 0.753f;
        fArray3[11] = 15.167f;
        fArray3[12] = 1.216f;
        fArray3[14] = 13.921f;
        fArray3[15] = 1.37f;
        fArray3[17] = 12.211f;
        fArray3[18] = 1.493f;
        fArray3[20] = 10.379f;
        fArray3[21] = 1.524f;
        fArray3[23] = 8.7f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 70, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(16406, 5, -1.308f, 0.0f, 13.031f, 180.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(3, 3, 4, 4);
        float[] fArray5 = new float[32];
        fArray5[0] = -1.308f;
        fArray5[2] = 13.031f;
        fArray5[3] = 1.0f;
        fArray5[4] = -1.015f;
        fArray5[6] = 12.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = -0.552f;
        fArray5[10] = 10.846f;
        fArray5[11] = 3.0f;
        fArray5[12] = -0.46f;
        fArray5[14] = 9.815f;
        fArray5[15] = 4.0f;
        fArray5[16] = -0.676f;
        fArray5[18] = 8.907f;
        fArray5[19] = 5.0f;
        fArray5[20] = -0.799f;
        fArray5[22] = 7.815f;
        fArray5[23] = 6.0f;
        fArray5[24] = -1.262f;
        fArray5[26] = 6.907f;
        fArray5[27] = 7.0f;
        fArray5[28] = -1.493f;
        fArray5[30] = 5.907f;
        fArray5[31] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 5, fArray6);
        float[] fArray7 = new float[24];
        fArray7[0] = -1.308f;
        fArray7[2] = 13.031f;
        fArray7[3] = -0.815f;
        fArray7[5] = 12.0f;
        fArray7[6] = -0.352f;
        fArray7[8] = 10.846f;
        fArray7[9] = -0.26f;
        fArray7[11] = 9.815f;
        fArray7[12] = -0.476f;
        fArray7[14] = 8.907f;
        fArray7[15] = -0.599f;
        fArray7[17] = 7.815f;
        fArray7[18] = -1.062f;
        fArray7[20] = 6.907f;
        fArray7[21] = -1.493f;
        fArray7[23] = 5.907f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.kickEnepc(10, 70, 0);
        this.enemy3 = new Enepc();
        this.enemy3.init(16404, 3, 1.401f, 0.0f, 5.753f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1, 1, 2, 2);
        float[] fArray9 = new float[36];
        fArray9[0] = 1.401f;
        fArray9[2] = 5.753f;
        fArray9[3] = 1.0f;
        fArray9[4] = 1.093f;
        fArray9[6] = 4.537f;
        fArray9[7] = 2.0f;
        fArray9[8] = 0.507f;
        fArray9[10] = 3.507f;
        fArray9[11] = 3.0f;
        fArray9[12] = 1.401f;
        fArray9[14] = 5.753f;
        fArray9[15] = 4.0f;
        fArray9[16] = -0.11f;
        fArray9[18] = 2.322f;
        fArray9[19] = 5.0f;
        fArray9[20] = -0.925f;
        fArray9[22] = 0.982f;
        fArray9[23] = 6.0f;
        fArray9[24] = -0.075f;
        fArray9[26] = -0.018f;
        fArray9[27] = 7.0f;
        fArray9[28] = 0.537f;
        fArray9[30] = -1.141f;
        fArray9[31] = 8.0f;
        fArray9[32] = 1.247f;
        fArray9[34] = -2.326f;
        fArray9[35] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 1, 3, 3, fArray10);
        float[] fArray11 = new float[27];
        fArray11[0] = 1.401f;
        fArray11[2] = 5.753f;
        fArray11[3] = 1.093f;
        fArray11[5] = 4.537f;
        fArray11[6] = 0.507f;
        fArray11[8] = 3.507f;
        fArray11[9] = 1.401f;
        fArray11[11] = 5.753f;
        fArray11[12] = -0.11f;
        fArray11[14] = 2.322f;
        fArray11[15] = -0.925f;
        fArray11[17] = 0.982f;
        fArray11[18] = -0.075f;
        fArray11[20] = -0.018f;
        fArray11[21] = 0.537f;
        fArray11[23] = -1.141f;
        fArray11[24] = 1.247f;
        fArray11[26] = -2.326f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy3.kickEnepc(10, 70, 0);
        this.enemy4 = new Enepc();
        this.enemy4.init(16405, 7, -1.37f, 0.0f, -2.154f, 90.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(0, 0, 1, 1);
        float[] fArray13 = new float[36];
        fArray13[0] = -1.37f;
        fArray13[2] = -2.154f;
        fArray13[3] = 1.0f;
        fArray13[4] = -1.0f;
        fArray13[6] = -3.37f;
        fArray13[7] = 2.0f;
        fArray13[8] = -0.537f;
        fArray13[10] = -4.154f;
        fArray13[11] = 3.0f;
        fArray13[12] = -1.37f;
        fArray13[14] = -2.154f;
        fArray13[15] = 4.0f;
        fArray13[16] = 0.079f;
        fArray13[18] = -5.278f;
        fArray13[19] = 5.0f;
        fArray13[20] = 0.665f;
        fArray13[22] = -6.463f;
        fArray13[23] = 6.0f;
        fArray13[24] = 0.665f;
        fArray13[26] = -7.339f;
        fArray13[27] = 7.0f;
        fArray13[28] = 0.11f;
        fArray13[30] = -8.308f;
        fArray13[31] = 8.0f;
        fArray13[32] = 0.097f;
        fArray13[34] = -9.154f;
        fArray13[35] = -1.0f;
        float[] fArray14 = fArray13;
        this.enemy4.setParams(1, 1, 4, 7, fArray14);
        float[] fArray15 = new float[27];
        fArray15[0] = -1.37f;
        fArray15[2] = -2.154f;
        fArray15[3] = -1.0f;
        fArray15[5] = -3.37f;
        fArray15[6] = -0.537f;
        fArray15[8] = -4.154f;
        fArray15[9] = -1.37f;
        fArray15[11] = -2.154f;
        fArray15[12] = 0.079f;
        fArray15[14] = -5.278f;
        fArray15[15] = 0.665f;
        fArray15[17] = -6.463f;
        fArray15[18] = 0.665f;
        fArray15[20] = -7.339f;
        fArray15[21] = 0.11f;
        fArray15[23] = -8.308f;
        fArray15[24] = 0.097f;
        fArray15[26] = -9.154f;
        float[] fArray16 = fArray15;
        this.enemy4.setParams(fArray16);
        this.enemy4.kickEnepc(10, 70, 0);
        this.doorA = new Uwamono(34, 42, '\u0001');
        new Uwamono(35, 42, '\u0001', this.doorA);
        this.doorA.SetDoorRange(1.59f);
        this.doorA.SetDoorType('\u0004');
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

