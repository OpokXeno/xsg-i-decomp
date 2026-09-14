import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_PRO06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0860
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO06_PRJ {
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
    boolean lo1 = false;
    boolean lo2 = false;
    boolean lo3 = false;
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    int sibuki_kazu = 0;
    Effect[] sibuki;
    int[] sibuki_start;
    Effect fade;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;

    ST0860() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            Runtime.setFlags(8036, 1, 1);
        }
        if (n == 2) {
            Runtime.setFlags(8037, 1, 1);
        }
        if (n == 3) {
            Runtime.setFlags(8038, 1, 1);
        }
        if (n == 4) {
            Runtime.setFlags(8039, 1, 1);
        }
        if (n == 5) {
            Runtime.setFlags(8040, 1, 1);
        }
        if (n == 6) {
            Runtime.setFlags(8041, 1, 1);
        }
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        System.println("bottom");
    }

    public void HashigoTop(int n) {
        System.println("top");
        switch (n) {
            case 0: {
                System.println("0");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(859, 2);
                break;
            }
            case 1: {
                System.println("1");
                System.println("２周目？");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(819, 3);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (this.lo1) break;
                if (Runtime.getFlags(110, 1) == 0) {
                    if (Runtime.getFlags(8036, 1) == 0) {
                        this.enemy1.setTranslate(53.7f, 5.1f, -2.7f);
                        this.enemy1.setInvalidID(0);
                    }
                    if (Runtime.getFlags(8037, 1) == 0) {
                        this.enemy2.setTranslate(52.7f, 5.1f, -1.2f);
                        this.enemy2.setInvalidID(0);
                    }
                }
                this.lo1 = true;
                break;
            }
            case 1: {
                if (this.lo2) break;
                if (Runtime.getFlags(110, 1) == 0) {
                    if (Runtime.getFlags(8038, 1) == 0) {
                        this.enemy3.setTranslate(21.9f, 5.1f, -20.4f);
                        this.enemy3.setInvalidID(0);
                    }
                    if (Runtime.getFlags(8039, 1) == 0) {
                        this.enemy4.setTranslate(18.1f, 5.1f, -18.8f);
                        this.enemy4.setInvalidID(0);
                    }
                }
                this.lo2 = true;
                break;
            }
            case 2: {
                if (this.lo3) break;
                if (Runtime.getFlags(110, 1) == 0) {
                    if (Runtime.getFlags(8040, 1) == 0) {
                        this.enemy5.setTranslate(-22.0f, 5.1f, -20.2f);
                        this.enemy5.setInvalidID(0);
                    }
                    if (Runtime.getFlags(8041, 1) == 0) {
                        this.enemy6.setTranslate(-21.9f, 5.1f, -17.9f);
                        this.enemy6.setInvalidID(0);
                    }
                }
                this.lo3 = true;
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
                Runtime.jumpCF(839, 3);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 29.0f, 0.0f, -10.0f, 0.0f);
        this.teiten1.SetBgm(196624);
        this.teiten2 = new Uwamono(28690, 26.0f, 0.0f, -13.0f, 0.0f);
        this.teiten2.SetBgm(196624);
        this.teiten3 = new Uwamono(28690, 22.5f, 0.0f, -16.5f, 0.0f);
        this.teiten3.SetBgm(196624);
        this.teiten4 = new Uwamono(28690, 19.0f, 0.0f, -20.0f, 0.0f);
        this.teiten4.SetBgm(196624);
        this.teiten5 = new Uwamono(28690, 14.5f, 0.0f, -20.5f, 0.0f);
        this.teiten5.SetBgm(196624);
        this.teiten6 = new Uwamono(28690, 9.5f, 0.0f, -22.0f, 0.0f);
        this.teiten6.SetBgm(196624);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, -0.4f, 1.0f, -0.25f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(1, 1, -0.4f, 1.0f, -0.25f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(2, 1, -0.4f, 1.0f, -0.25f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(3, 1, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(3, 2, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightCol(3, 3, 0.27f, 0.27f, 0.27f);
        Runtime.setIdLightVec(3, 1, -0.4f, 1.0f, -0.25f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(4, 1, -0.4f, 1.0f, -0.25f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(5, 1, -0.4f, 1.0f, -0.25f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        float[][] fArrayArray = new float[3][];
        float[] fArray = new float[3];
        fArray[0] = 11.6f;
        fArray[2] = -22.3f;
        fArrayArray[0] = fArray;
        float[] fArray2 = new float[3];
        fArray2[0] = 28.5f;
        fArray2[2] = -8.8f;
        fArrayArray[1] = fArray2;
        float[] fArray3 = new float[3];
        fArray3[0] = 24.6f;
        fArray3[2] = -17.8f;
        fArrayArray[2] = fArray3;
        float[][] fArrayArray2 = fArrayArray;
        this.sibuki_kazu = fArrayArray2.length;
        this.sibuki = new Effect[this.sibuki_kazu];
        this.sibuki_start = new int[this.sibuki_kazu];
        int n = 0;
        while (n < this.sibuki_kazu) {
            this.sibuki[n] = new Effect(1496, fArrayArray2[n][0], fArrayArray2[n][1], fArrayArray2[n][2], 0.0f);
            this.sibuki[n].setClip(false);
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFLockX(9, 17.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFPedestal(11, 49.924f, 9.031606f, -1.4850749f, 39.35953f, -60.541172f, -68.73986f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        this.cam0.setCFPedestal(12, -48.758102f, 8.167836f, -5.4706f, 40.0f, -46.322353f, 52.858818f, 0.0f, 2.0f);
        this.cam0.setCFHokan(12, 100.0f, 100.0f);
        this.cam0.setCFAngle(13, -28.0f, -25.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.015f, 0.015f);
        float[] fArray4 = new float[76];
        fArray4[0] = 52.5f;
        fArray4[1] = 1.6f;
        fArray4[2] = -2.5f;
        fArray4[3] = -1.0f;
        fArray4[4] = 50.5f;
        fArray4[5] = 1.6f;
        fArray4[6] = -2.2f;
        fArray4[8] = 45.5f;
        fArray4[9] = 1.6f;
        fArray4[10] = -2.0f;
        fArray4[11] = 1.0f;
        fArray4[12] = 40.5f;
        fArray4[13] = 1.6f;
        fArray4[14] = -1.9f;
        fArray4[15] = 2.0f;
        fArray4[16] = 35.9f;
        fArray4[17] = 1.6f;
        fArray4[18] = -1.8f;
        fArray4[19] = 3.0f;
        fArray4[20] = 26.7f;
        fArray4[21] = 1.6f;
        fArray4[22] = -10.1f;
        fArray4[23] = 4.0f;
        fArray4[24] = 23.3f;
        fArray4[25] = 1.6f;
        fArray4[26] = -13.6f;
        fArray4[27] = 5.0f;
        fArray4[28] = 20.2f;
        fArray4[29] = 1.6f;
        fArray4[30] = -16.6f;
        fArray4[31] = 6.0f;
        fArray4[32] = 17.3f;
        fArray4[33] = 1.6f;
        fArray4[34] = -19.3f;
        fArray4[35] = 7.0f;
        fArray4[36] = 16.1f;
        fArray4[37] = 1.6f;
        fArray4[38] = -19.3f;
        fArray4[39] = 8.0f;
        fArray4[40] = 12.4f;
        fArray4[41] = 1.6f;
        fArray4[42] = -19.3f;
        fArray4[43] = 9.0f;
        fArray4[44] = 8.1f;
        fArray4[45] = 1.6f;
        fArray4[46] = -19.3f;
        fArray4[47] = 10.0f;
        fArray4[48] = -1.1f;
        fArray4[49] = 1.6f;
        fArray4[50] = -19.3f;
        fArray4[51] = 11.0f;
        fArray4[52] = -10.6f;
        fArray4[53] = 1.6f;
        fArray4[54] = -19.3f;
        fArray4[55] = 12.0f;
        fArray4[56] = -16.8f;
        fArray4[57] = 1.6f;
        fArray4[58] = -19.3f;
        fArray4[59] = 13.0f;
        fArray4[60] = -21.7f;
        fArray4[61] = 1.6f;
        fArray4[62] = -19.0f;
        fArray4[63] = 14.0f;
        fArray4[64] = -29.3f;
        fArray4[65] = 1.6f;
        fArray4[66] = -10.6f;
        fArray4[67] = 15.0f;
        fArray4[68] = -32.8f;
        fArray4[69] = 1.6f;
        fArray4[70] = -7.3f;
        fArray4[71] = 16.0f;
        fArray4[72] = -46.6f;
        fArray4[73] = 1.6f;
        fArray4[74] = -8.9f;
        fArray4[75] = 17.0f;
        float[] fArray5 = fArray4;
        if (Runtime.getFlags(8036, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16646, 3, 70.0f, -100.0f, -2.9f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy1.setParams(1, 19, 1, 3, fArray5);
            this.enemy1.setInvalidID(1);
        }
        if (Runtime.getFlags(8037, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16646, 3, 70.8f, -100.0f, -1.24f, -90.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy2.setParams(1, 19, 2, 3, fArray5);
            this.enemy2.setInvalidID(1);
        }
        if (Runtime.getFlags(8038, 1) == 0) {
            this.enemy3 = new Enepc();
            this.enemy3.init(16646, 3, 72.8f, -100.0f, -0.6f, -90.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy3.setParams(1, 19, 3, 3, fArray5);
            this.enemy3.setInvalidID(1);
        }
        if (Runtime.getFlags(8039, 1) == 0) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16646, 3, 72.8f, -100.0f, -0.6f, -90.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy4.setParams(1, 19, 4, 3, fArray5);
            this.enemy4.setInvalidID(1);
        }
        if (Runtime.getFlags(8040, 1) == 0) {
            this.enemy5 = new Enepc();
            this.enemy5.init(16646, 3, 74.8f, -100.0f, -0.6f, -90.0f);
            this.enemy5.id = 5;
            this.enemy5.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy5.setParams(1, 19, 5, 3, fArray5);
            this.enemy5.setInvalidID(1);
        }
        if (Runtime.getFlags(8041, 1) == 0) {
            this.enemy6 = new Enepc();
            this.enemy6.init(16646, 3, 74.8f, -100.0f, -0.6f, -90.0f);
            this.enemy6.id = 6;
            this.enemy6.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
            this.enemy6.setParams(1, 19, 6, 3, fArray5);
            this.enemy6.setInvalidID(1);
        }
        this.enemy7 = new Enepc();
        this.enemy7.init(16646, 3, 74.8f, -100.0f, -0.6f, -90.0f);
        this.enemy7.id = 7;
        this.enemy7.setGroup(0, 0, 1, 1, 1, 2, 2, 3);
        this.enemy7.setParams(0, 19, 6, 3, fArray5);
        this.enemy7.kickEnepc(4, 2);
        this.enemy7.setVisible(false);
        this.enemy7.dispRadar(false);
        this.enemy7.setInvalidID(1);
        this.enemy7.kickEnepc(19, 1, 0, 490, 1);
        this.enemy7.kickEnepc(19, 2, 0, 490, 1);
        this.enemy7.kickEnepc(19, 3, 0, 350, 1);
        this.enemy7.kickEnepc(19, 4, 0, 350, 1);
        this.enemy7.kickEnepc(19, 5, 0, 350, 1);
        this.enemy7.kickEnepc(19, 6, 0, 350, 1);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 49);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 50);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 51);
        new Uwamono(28672, -29.3f, -1.0f, -14.1f, 0.0f);
        new Uwamono(163, 9);
        new Uwamono(164, 9, this.item1);
        new Uwamono(165, 9, this.item2);
        new Uwamono(166, 9);
        new Uwamono(14, 9, this.item3);
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
            this.setEdgeFall(1);
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
        NPC_NORMAL() {
        }

        void init() {
        }

        public void talk(Window window) {
        }

        public void touch() {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }

        public void talk() {
        }
    }
}

