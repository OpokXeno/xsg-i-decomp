import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_PRO02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0820
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO02_PRJ {
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
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Enepc enemy9;
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
    MAPUnit light60;
    MAPUnit light61;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Effect fire06;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect fade;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono bgm;
    Uwamono bgmf01;
    Uwamono bgmf02;
    Uwamono bgmf03;
    Uwamono bgmf04;
    Uwamono bgmf05;
    Uwamono bgmf06;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] harigami = new String[]{"'Out of order. Currently, this door cannot be opened from this side. Pardon the inconvenience.\n", "-Security Department'", "/[waitkey(64)]/[close()]"};
    String[] z8 = new String[]{"/[label(Ziggurat 8)]", "(It looks like my footsteps will echo on this floor.)", "/[waitkey(1)]/[clear()]", "(It would be best not to run, in order to avoid alerting the enemy.)", "/[waitkey(1)]/[clear()]", "(I can walk by holding down the R2 Button while I move.)", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 4.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 4.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 4, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0820() {
    }

    void EOB_Always(int n) {
        System.println("EOB_Always");
        Runtime.progressEffect(30);
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
            case 0: {
                if (Runtime.getFlags(110, 1) != 0) break;
                Runtime.enable(262144);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.harigami, 0);
                System.waitFor(this.win);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                break;
            }
            case 1: {
                if (Runtime.getFlags(8045, 1) != 0) break;
                this.enemy1.kickEnepc(4, 2);
                this.enemy2.kickEnepc(4, 2);
                this.enemy3.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("ここの床は響きそうだイベント");
                this.nwin(this.z8);
                Runtime.setFlags(8045, 1, 1);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                this.enemy1.kickEnepc(4, 0);
                this.enemy2.kickEnepc(4, 0);
                this.enemy3.kickEnepc(4, 0);
                break;
            }
            case 2: {
                if (Runtime.getFlags(8001, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3204, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3204, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3224, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3284, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorB.SetDoorType('\u0004');
                    Runtime.setFlags(3284, 1, 1);
                    Runtime.setFlags(8001, 1, 1);
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
                if (Runtime.getFlags(110, 1) == 1) {
                    System.println("イベントSCE02007：マーグリス登場（プロレマCF終了）");
                    Runtime.setFlags(111, 1, 1);
                    Runtime.jumpEvent(2070);
                    break;
                }
                Runtime.jumpCF(810, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(830, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(870, 1);
                break;
            }
            case 3: {
                Runtime.setFlags(8001, 1, 1);
                System.println("Subroute");
                Runtime.jumpCF(890, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setEffectRender(1);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 8.0f, 0.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196617);
        this.teiten2 = new Uwamono(28690, 13.0f, 0.0f, -2.0f, 0.0f);
        this.teiten2.SetBgm(196617);
        this.teiten3 = new Uwamono(28690, 21.0f, 0.0f, -2.0f, 0.0f);
        this.teiten3.SetBgm(196617);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
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
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.325f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.325f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.325f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.325f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.425f, 0.425f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.425f, 0.425f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.425f, 0.425f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.425f, 0.425f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 0.5f, 0.5f);
        Runtime.setIdLightVec(4, 3, 0.0f, -0.5f, -0.5f);
        Runtime.setIdLightCol(5, 0, 0.2f, 0.225f, 0.225f);
        Runtime.setIdLightCol(5, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(5, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(5, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 0.5f, 0.5f);
        Runtime.setIdLightVec(5, 3, 0.0f, -0.5f, -0.5f);
        Stage.setVisible(-1, true);
        this.light10 = new Mapunits();
        this.light10.mapUnit(77);
        this.light10.start(4, null);
        this.light11 = new Mapunits();
        this.light11.mapUnit(78);
        this.light11.start(4, null);
        this.light60 = new Mapunits();
        this.light60.mapUnit(109);
        this.light60.start(4, null);
        this.light61 = new Mapunits();
        this.light61.mapUnit(110);
        this.light61.start(4, null);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(36, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestal(6, -22.933777f, 7.8154197f, 4.7298794f, 56.878613f, -29.384352f, -40.197193f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.03f, 0.03f);
        this.cam0.setCFPedestal(8, -15.044118f, 12.723508f, 2.3012047f, 30.319818f, -27.94679f, -19.799461f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        int n2 = 4;
        this.enemy1 = new Enepc();
        this.enemy1.init(17153, 3, -23.0f, 0.0f, -5.0f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray = new float[52];
        fArray[0] = -23.24f;
        fArray[2] = -5.0f;
        fArray[3] = -1.0f;
        fArray[4] = -20.6f;
        fArray[6] = -4.99f;
        fArray[8] = -15.3f;
        fArray[10] = -5.0f;
        fArray[11] = 1.0f;
        fArray[12] = -7.87f;
        fArray[14] = -4.6f;
        fArray[15] = 2.0f;
        fArray[16] = -5.08f;
        fArray[18] = -5.2f;
        fArray[19] = 3.0f;
        fArray[20] = -5.08f;
        fArray[22] = 0.87f;
        fArray[23] = 4.0f;
        fArray[24] = -9.0f;
        fArray[26] = 0.91f;
        fArray[27] = 5.0f;
        fArray[28] = -26.0f;
        fArray[30] = -5.0f;
        fArray[32] = -23.0f;
        fArray[34] = 0.73f;
        fArray[36] = -20.09f;
        fArray[38] = 0.84f;
        fArray[39] = 8.0f;
        fArray[40] = -14.0f;
        fArray[42] = 0.84f;
        fArray[43] = 9.0f;
        fArray[44] = -2.0f;
        fArray[46] = -5.0f;
        fArray[47] = 4.0f;
        fArray[48] = -1.8f;
        fArray[50] = -2.0f;
        fArray[51] = 11.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, n2, 1, 3, fArray2);
        float[] fArray3 = new float[18];
        fArray3[0] = -23.24f;
        fArray3[2] = -5.0f;
        fArray3[3] = -20.6f;
        fArray3[5] = -4.99f;
        fArray3[6] = -14.75f;
        fArray3[8] = -5.0f;
        fArray3[9] = -14.75f;
        fArray3[11] = 0.84f;
        fArray3[12] = -20.09f;
        fArray3[14] = 0.84f;
        fArray3[15] = -23.0f;
        fArray3[17] = 0.73f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(17153, 3, -12.5f, 0.0f, -5.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 0, 0);
        float[] fArray5 = new float[52];
        fArray5[0] = -12.5f;
        fArray5[2] = -5.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -7.87f;
        fArray5[6] = -4.6f;
        fArray5[8] = -5.08f;
        fArray5[10] = -5.2f;
        fArray5[11] = 1.0f;
        fArray5[12] = -5.08f;
        fArray5[14] = 0.87f;
        fArray5[15] = 2.0f;
        fArray5[16] = -9.0f;
        fArray5[18] = 0.91f;
        fArray5[19] = 3.0f;
        fArray5[20] = -14.0f;
        fArray5[22] = 0.84f;
        fArray5[23] = 4.0f;
        fArray5[24] = -20.6f;
        fArray5[26] = -4.99f;
        fArray5[28] = -23.24f;
        fArray5[30] = -5.0f;
        fArray5[31] = 6.0f;
        fArray5[32] = -23.0f;
        fArray5[34] = 0.73f;
        fArray5[35] = 7.0f;
        fArray5[36] = -20.09f;
        fArray5[38] = 0.84f;
        fArray5[39] = 8.0f;
        fArray5[40] = -26.0f;
        fArray5[42] = -5.0f;
        fArray5[43] = 7.0f;
        fArray5[44] = -2.0f;
        fArray5[46] = -5.0f;
        fArray5[47] = 2.0f;
        fArray5[48] = -1.8f;
        fArray5[50] = -2.0f;
        fArray5[51] = 11.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, n2, 2, 3, fArray6);
        float[] fArray7 = new float[33];
        fArray7[0] = -12.5f;
        fArray7[2] = -5.0f;
        fArray7[3] = -12.81f;
        fArray7[5] = -2.33f;
        fArray7[6] = -12.81f;
        fArray7[8] = 0.91f;
        fArray7[9] = -10.14f;
        fArray7[11] = 0.87f;
        fArray7[12] = -7.73f;
        fArray7[14] = 0.87f;
        fArray7[15] = -5.08f;
        fArray7[17] = 0.87f;
        fArray7[18] = -5.08f;
        fArray7[20] = -2.93f;
        fArray7[21] = -5.08f;
        fArray7[23] = -5.2f;
        fArray7[24] = -7.87f;
        fArray7[26] = -4.6f;
        fArray7[27] = -10.04f;
        fArray7[29] = -4.82f;
        fArray7[30] = -12.5f;
        fArray7[32] = -5.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(17153, 3, -5.08f, 0.0f, 0.87f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 0, 0);
        float[] fArray9 = new float[52];
        fArray9[0] = -5.08f;
        fArray9[2] = 0.87f;
        fArray9[3] = -1.0f;
        fArray9[4] = -9.0f;
        fArray9[6] = 0.91f;
        fArray9[8] = -14.0f;
        fArray9[10] = 0.84f;
        fArray9[11] = 1.0f;
        fArray9[12] = -12.5f;
        fArray9[14] = -5.0f;
        fArray9[15] = 2.0f;
        fArray9[16] = -7.87f;
        fArray9[18] = -4.6f;
        fArray9[19] = 3.0f;
        fArray9[20] = -5.08f;
        fArray9[22] = -5.2f;
        fArray9[23] = 4.0f;
        fArray9[24] = -2.0f;
        fArray9[26] = -5.0f;
        fArray9[27] = 5.0f;
        fArray9[28] = -1.8f;
        fArray9[30] = -2.0f;
        fArray9[31] = 6.0f;
        fArray9[32] = -20.6f;
        fArray9[34] = -4.99f;
        fArray9[35] = 3.0f;
        fArray9[36] = -23.24f;
        fArray9[38] = -5.0f;
        fArray9[39] = 8.0f;
        fArray9[40] = -23.0f;
        fArray9[42] = 0.73f;
        fArray9[43] = 9.0f;
        fArray9[44] = -20.09f;
        fArray9[46] = 0.84f;
        fArray9[47] = 10.0f;
        fArray9[48] = -26.0f;
        fArray9[50] = -5.0f;
        fArray9[51] = 9.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, n2, 3, 3, fArray10);
        float[] fArray11 = new float[33];
        fArray11[0] = -5.08f;
        fArray11[2] = 0.87f;
        fArray11[3] = -5.08f;
        fArray11[5] = -2.93f;
        fArray11[6] = -5.08f;
        fArray11[8] = -5.2f;
        fArray11[9] = -7.87f;
        fArray11[11] = -4.6f;
        fArray11[12] = -10.04f;
        fArray11[14] = -4.82f;
        fArray11[15] = -12.5f;
        fArray11[17] = -5.0f;
        fArray11[18] = -12.81f;
        fArray11[20] = -4.68f;
        fArray11[21] = -12.81f;
        fArray11[23] = -2.33f;
        fArray11[24] = -12.81f;
        fArray11[26] = 0.91f;
        fArray11[27] = -10.14f;
        fArray11[29] = 0.87f;
        fArray11[30] = -7.73f;
        fArray11[32] = 0.87f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy4 = new Enepc();
        this.enemy4.init(17153, 3, 6.3f, 0.0f, -7.7f, 0.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(0, 0, 1, 1);
        float[] fArray13 = new float[40];
        fArray13[0] = 6.3f;
        fArray13[2] = -7.7f;
        fArray13[3] = -1.0f;
        fArray13[4] = 6.0f;
        fArray13[6] = -8.0f;
        fArray13[8] = 3.0f;
        fArray13[10] = -8.0f;
        fArray13[11] = 1.0f;
        fArray13[12] = 2.8f;
        fArray13[14] = -2.0f;
        fArray13[15] = 2.0f;
        fArray13[16] = 10.5f;
        fArray13[18] = -8.0f;
        fArray13[19] = 1.0f;
        fArray13[20] = 10.5f;
        fArray13[22] = -2.0f;
        fArray13[23] = 4.0f;
        fArray13[24] = 6.2f;
        fArray13[26] = -2.0f;
        fArray13[27] = 1.0f;
        fArray13[28] = 6.0f;
        fArray13[30] = 3.5f;
        fArray13[31] = 6.0f;
        fArray13[32] = 10.5f;
        fArray13[34] = 3.5f;
        fArray13[35] = 7.0f;
        fArray13[36] = 3.0f;
        fArray13[38] = 3.5f;
        fArray13[39] = 7.0f;
        float[] fArray14 = fArray13;
        this.enemy4.setParams(1, n2, 4, 3, fArray14);
        float[] fArray15 = new float[24];
        fArray15[0] = 6.3f;
        fArray15[2] = -7.7f;
        fArray15[3] = 6.2f;
        fArray15[5] = -5.51f;
        fArray15[6] = 6.2f;
        fArray15[8] = -3.06f;
        fArray15[9] = 8.01f;
        fArray15[11] = -3.06f;
        fArray15[12] = 9.97f;
        fArray15[14] = -3.06f;
        fArray15[15] = 9.97f;
        fArray15[17] = -5.55f;
        fArray15[18] = 10.03f;
        fArray15[20] = -8.08f;
        fArray15[21] = 7.94f;
        fArray15[23] = -8.08f;
        float[] fArray16 = fArray15;
        this.enemy4.setParams(fArray16);
        this.enemy5 = new Enepc();
        this.enemy5.init(17153, 3, 6.3f, 0.0f, 3.1f, 0.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(0, 0, 1, 1);
        float[] fArray17 = new float[40];
        fArray17[0] = 6.3f;
        fArray17[2] = 3.1f;
        fArray17[3] = -1.0f;
        fArray17[4] = 6.0f;
        fArray17[6] = 3.5f;
        fArray17[8] = 3.0f;
        fArray17[10] = 3.5f;
        fArray17[11] = 1.0f;
        fArray17[12] = 3.0f;
        fArray17[14] = -2.0f;
        fArray17[15] = 2.0f;
        fArray17[16] = 10.5f;
        fArray17[18] = 3.5f;
        fArray17[19] = 1.0f;
        fArray17[20] = 10.5f;
        fArray17[22] = -2.0f;
        fArray17[23] = 4.0f;
        fArray17[24] = 6.0f;
        fArray17[26] = -2.0f;
        fArray17[27] = 1.0f;
        fArray17[28] = 6.0f;
        fArray17[30] = -8.0f;
        fArray17[31] = 6.0f;
        fArray17[32] = 3.0f;
        fArray17[34] = -8.0f;
        fArray17[35] = 7.0f;
        fArray17[36] = 10.5f;
        fArray17[38] = -8.0f;
        fArray17[39] = 7.0f;
        float[] fArray18 = fArray17;
        this.enemy5.setParams(1, n2, 5, 3, fArray18);
        float[] fArray19 = new float[27];
        fArray19[0] = 6.3f;
        fArray19[2] = 3.1f;
        fArray19[3] = 6.13f;
        fArray19[5] = 0.57f;
        fArray19[6] = 6.13f;
        fArray19[8] = -1.36f;
        fArray19[9] = 7.96f;
        fArray19[11] = -1.17f;
        fArray19[12] = 9.94f;
        fArray19[14] = -1.17f;
        fArray19[15] = 9.94f;
        fArray19[17] = 0.9f;
        fArray19[18] = 9.94f;
        fArray19[20] = 2.94f;
        fArray19[21] = 7.98f;
        fArray19[23] = 2.94f;
        fArray19[24] = 6.3f;
        fArray19[26] = 3.1f;
        float[] fArray20 = fArray19;
        this.enemy5.setParams(fArray20);
        this.enemy7 = new Enepc();
        this.enemy7.init(17153, 3, 15.0f, 0.0f, 3.0f, 0.0f);
        this.enemy7.id = 7;
        this.enemy7.setGroup(1, 1, 2, 2);
        float[] fArray21 = new float[76];
        fArray21[0] = 15.0f;
        fArray21[2] = 3.5f;
        fArray21[3] = -1.0f;
        fArray21[4] = 15.5f;
        fArray21[6] = 3.0f;
        fArray21[8] = 18.5f;
        fArray21[10] = 3.0f;
        fArray21[11] = 1.0f;
        fArray21[12] = 23.0f;
        fArray21[14] = 3.0f;
        fArray21[15] = 2.0f;
        fArray21[16] = 26.0f;
        fArray21[18] = 3.0f;
        fArray21[19] = 3.0f;
        fArray21[20] = 26.0f;
        fArray21[22] = -2.0f;
        fArray21[23] = 4.0f;
        fArray21[24] = 26.0f;
        fArray21[26] = -7.0f;
        fArray21[27] = 5.0f;
        fArray21[28] = 23.0f;
        fArray21[30] = -2.0f;
        fArray21[31] = 3.0f;
        fArray21[32] = 18.5f;
        fArray21[34] = -2.0f;
        fArray21[35] = 7.0f;
        fArray21[36] = 15.5f;
        fArray21[38] = -2.0f;
        fArray21[39] = 1.0f;
        fArray21[40] = 15.5f;
        fArray21[42] = -7.0f;
        fArray21[43] = 9.0f;
        fArray21[44] = 18.5f;
        fArray21[46] = -7.0f;
        fArray21[47] = 10.0f;
        fArray21[48] = 23.0f;
        fArray21[50] = -7.0f;
        fArray21[51] = 11.0f;
        fArray21[52] = 14.4f;
        fArray21[54] = 3.9f;
        fArray21[55] = 1.0f;
        fArray21[56] = 15.0f;
        fArray21[58] = 6.0f;
        fArray21[59] = 13.0f;
        fArray21[60] = 18.7f;
        fArray21[62] = 6.0f;
        fArray21[63] = 14.0f;
        fArray21[64] = 13.0f;
        fArray21[66] = -8.5f;
        fArray21[67] = 10.0f;
        fArray21[68] = 17.0f;
        fArray21[70] = -12.5f;
        fArray21[71] = 16.0f;
        fArray21[72] = 20.0f;
        fArray21[74] = -9.5f;
        fArray21[75] = 17.0f;
        float[] fArray22 = fArray21;
        this.enemy7.setParams(1, n2, 7, 3, fArray22);
        float[] fArray23 = new float[18];
        fArray23[0] = 15.0f;
        fArray23[2] = 3.0f;
        fArray23[3] = 15.06f;
        fArray23[5] = -0.65f;
        fArray23[6] = 15.06f;
        fArray23[8] = -4.54f;
        fArray23[9] = 15.06f;
        fArray23[11] = -7.51f;
        fArray23[12] = 15.06f;
        fArray23[14] = -4.54f;
        fArray23[15] = 15.06f;
        fArray23[17] = -0.65f;
        float[] fArray24 = fArray23;
        this.enemy7.setParams(fArray24);
        this.enemy8 = new Enepc();
        this.enemy8.init(17153, 3, 19.31f, 0.0f, -7.05f, 0.0f);
        this.enemy8.id = 8;
        this.enemy8.setGroup(1, 1, 2, 2);
        float[] fArray25 = new float[76];
        fArray25[0] = 19.0f;
        fArray25[2] = -7.05f;
        fArray25[3] = -1.0f;
        fArray25[4] = 18.5f;
        fArray25[6] = -7.0f;
        fArray25[8] = 23.0f;
        fArray25[10] = -7.0f;
        fArray25[11] = 1.0f;
        fArray25[12] = 26.0f;
        fArray25[14] = -7.0f;
        fArray25[15] = 2.0f;
        fArray25[16] = 26.0f;
        fArray25[18] = -2.0f;
        fArray25[19] = 3.0f;
        fArray25[20] = 26.0f;
        fArray25[22] = 3.0f;
        fArray25[23] = 4.0f;
        fArray25[24] = 23.0f;
        fArray25[26] = -2.0f;
        fArray25[27] = 2.0f;
        fArray25[28] = 18.5f;
        fArray25[30] = -2.0f;
        fArray25[31] = 6.0f;
        fArray25[32] = 15.5f;
        fArray25[34] = -7.0f;
        fArray25[35] = 1.0f;
        fArray25[36] = 15.5f;
        fArray25[38] = -2.0f;
        fArray25[39] = 8.0f;
        fArray25[40] = 15.5f;
        fArray25[42] = 3.0f;
        fArray25[43] = 9.0f;
        fArray25[44] = 18.5f;
        fArray25[46] = 3.0f;
        fArray25[47] = 10.0f;
        fArray25[48] = 23.0f;
        fArray25[50] = 3.0f;
        fArray25[51] = 11.0f;
        fArray25[52] = 14.4f;
        fArray25[54] = 3.9f;
        fArray25[55] = 10.0f;
        fArray25[56] = 15.0f;
        fArray25[58] = 6.0f;
        fArray25[59] = 13.0f;
        fArray25[60] = 18.7f;
        fArray25[62] = 6.0f;
        fArray25[63] = 14.0f;
        fArray25[64] = 13.0f;
        fArray25[66] = -8.5f;
        fArray25[67] = 8.0f;
        fArray25[68] = 17.0f;
        fArray25[70] = -12.5f;
        fArray25[71] = 16.0f;
        fArray25[72] = 20.0f;
        fArray25[74] = -9.5f;
        fArray25[75] = 17.0f;
        float[] fArray26 = fArray25;
        this.enemy8.setParams(1, n2, 8, 3, fArray26);
        float[] fArray27 = new float[24];
        fArray27[0] = 19.0f;
        fArray27[2] = -7.05f;
        fArray27[3] = 19.0f;
        fArray27[5] = -5.51f;
        fArray27[6] = 19.0f;
        fArray27[8] = -3.06f;
        fArray27[9] = 21.01f;
        fArray27[11] = -3.06f;
        fArray27[12] = 22.97f;
        fArray27[14] = -3.06f;
        fArray27[15] = 22.97f;
        fArray27[17] = -5.55f;
        fArray27[18] = 23.03f;
        fArray27[20] = -8.08f;
        fArray27[21] = 20.94f;
        fArray27[23] = -8.08f;
        float[] fArray28 = fArray27;
        this.enemy8.setParams(fArray28);
        this.enemy9 = new Enepc();
        this.enemy9.init(17153, 3, 19.39f, 0.0f, 2.71f, 0.0f);
        this.enemy9.id = 9;
        this.enemy9.setGroup(1, 1, 2, 2);
        float[] fArray29 = new float[76];
        fArray29[0] = 19.39f;
        fArray29[2] = 2.71f;
        fArray29[3] = -1.0f;
        fArray29[4] = 18.5f;
        fArray29[6] = 3.0f;
        fArray29[8] = 23.0f;
        fArray29[10] = 3.0f;
        fArray29[11] = 1.0f;
        fArray29[12] = 26.0f;
        fArray29[14] = 3.0f;
        fArray29[15] = 2.0f;
        fArray29[16] = 26.0f;
        fArray29[18] = -2.0f;
        fArray29[19] = 3.0f;
        fArray29[20] = 26.0f;
        fArray29[22] = -7.0f;
        fArray29[23] = 4.0f;
        fArray29[24] = 18.5f;
        fArray29[26] = -2.0f;
        fArray29[27] = 1.0f;
        fArray29[28] = 23.0f;
        fArray29[30] = -2.0f;
        fArray29[31] = 6.0f;
        fArray29[32] = 15.5f;
        fArray29[34] = 3.0f;
        fArray29[35] = 1.0f;
        fArray29[36] = 15.5f;
        fArray29[38] = -2.0f;
        fArray29[39] = 8.0f;
        fArray29[40] = 15.5f;
        fArray29[42] = -7.0f;
        fArray29[43] = 9.0f;
        fArray29[44] = 18.5f;
        fArray29[46] = -7.0f;
        fArray29[47] = 10.0f;
        fArray29[48] = 23.0f;
        fArray29[50] = -7.0f;
        fArray29[51] = 11.0f;
        fArray29[52] = 14.4f;
        fArray29[54] = 3.9f;
        fArray29[55] = 8.0f;
        fArray29[56] = 15.0f;
        fArray29[58] = 6.0f;
        fArray29[59] = 13.0f;
        fArray29[60] = 18.7f;
        fArray29[62] = 6.0f;
        fArray29[63] = 14.0f;
        fArray29[64] = 13.0f;
        fArray29[66] = -8.5f;
        fArray29[67] = 10.0f;
        fArray29[68] = 17.0f;
        fArray29[70] = -12.5f;
        fArray29[71] = 16.0f;
        fArray29[72] = 20.0f;
        fArray29[74] = -9.5f;
        fArray29[75] = 17.0f;
        float[] fArray30 = fArray29;
        this.enemy9.setParams(1, n2, 9, 3, fArray30);
        float[] fArray31 = new float[27];
        fArray31[0] = 19.39f;
        fArray31[2] = 2.71f;
        fArray31[3] = 19.13f;
        fArray31[5] = 0.57f;
        fArray31[6] = 19.13f;
        fArray31[8] = -1.36f;
        fArray31[9] = 20.96f;
        fArray31[11] = -1.17f;
        fArray31[12] = 22.94f;
        fArray31[14] = -1.17f;
        fArray31[15] = 22.94f;
        fArray31[17] = 0.9f;
        fArray31[18] = 22.94f;
        fArray31[20] = 2.94f;
        fArray31[21] = 20.98f;
        fArray31[23] = 2.94f;
        fArray31[24] = 19.3f;
        fArray31[26] = 3.1f;
        float[] fArray32 = fArray31;
        this.enemy9.setParams(fArray32);
        this.fire01 = new Effect(1714, 8.0f, 1.3f, -5.0f, 0.0f);
        this.fire02 = new Effect(1714, 8.0f, 1.3f, 1.0f, 0.0f);
        this.fire03 = new Effect(1714, 13.0f, 1.3f, -5.0f, 0.0f);
        this.fire04 = new Effect(1714, 13.0f, 1.3f, 1.0f, 0.0f);
        this.fire05 = new Effect(1714, 21.0f, 1.3f, -5.0f, 0.0f);
        this.fire06 = new Effect(1714, 21.0f, 1.3f, 1.0f, 0.0f);
        this.light01 = new Effect(1405, -19.0f, 4.3f, -4.3f, 0.0f);
        this.light02 = new Effect(1405, -11.8f, 4.3f, -4.3f, 0.0f);
        this.light03 = new Effect(1405, -3.0f, 4.3f, -4.3f, 0.0f);
        this.light04 = new Effect(1405, -19.0f, 4.3f, 0.5f, 0.0f);
        this.light05 = new Effect(1405, -11.8f, 4.3f, 0.5f, 0.0f);
        this.light06 = new Effect(1405, -3.0f, 4.3f, 0.5f, 0.0f);
        Runtime.progressEffect(30);
        this.light01.disp(false);
        this.light02.disp(false);
        this.light04.disp(false);
        this.light05.disp(false);
        this.light06.disp(false);
        this.item1 = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 420);
        this.item1.SetCallNo(1);
        this.item1.SetSymbol(28686);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 180.0f, 40);
        new Uwamono(28672, 2.2f, -1.0f, 3.0f, 0.0f);
        this.item4 = new Uwamono(28677, -10.5f, 6.0f, -8.1f, 180.0f, 52);
        this.item4.SetSymbol(28684);
        if (Runtime.getFlags(3204, 1) == 0) {
            new Uwamono(9, 31);
            new Uwamono(48, 24);
            new Uwamono(92, 39);
        } else {
            Stage.setVisible(9, false);
            Stage.setVisible(48, false);
            Stage.setVisible(92, false);
        }
        new Uwamono(90, 24, this.item3);
        new Uwamono(152, 5, this.item1);
        this.doorA = new Uwamono(153, 42, '\u0001');
        new Uwamono(154, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(74, 40, '\u0004');
        if (Runtime.getFlags(110, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(3284, 1) == 0) {
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB.SetDoorType('\u0004');
        }
        this.light10.start(1, "yurayura");
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3231, 1, 1);
                break;
            }
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0820.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0820.waitPage(this.win, 64);
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL() {
        }

        void init() {
        }

        public void talk(Window window) {
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void yurayura() {
            int n = 0;
            float f = 0.0f;
            float f2 = 0.0f;
            ST0820.this.light10.getTranslate();
            ST0820.this.light11.getTranslate();
            ST0820.this.light60.getTranslate();
            ST0820.this.light61.getTranslate();
            boolean bl = true;
            while (true) {
                f = Math.sin((float) n * 3.14f / 180.0f);
                f2 = Math.sin((float) (n + 60) * 3.14f / 180.0f);
                ST0820.this.light10.setTranslate(f / 500.0f + ST0820.this.light10.px, ST0820.this.light10.py, ST0820.this.light10.pz);
                ST0820.this.light11.setTranslate(f / 500.0f + ST0820.this.light11.px, ST0820.this.light11.py, ST0820.this.light11.pz);
                ST0820.this.light04.setTransOffset(f / 500.0f, 0.0f, 0.0f);
                ST0820.this.light60.setTranslate(ST0820.this.light60.px, ST0820.this.light60.py, f2 / 300.0f + ST0820.this.light60.pz);
                ST0820.this.light61.setTranslate(ST0820.this.light61.px, ST0820.this.light61.py, f2 / 300.0f + ST0820.this.light61.pz);
                ST0820.this.light03.setTransOffset(0.0f, 0.0f, f2 / 300.0f);
                ST0820.this.player.getTranslate();
                if (ST0820.this.player.px > 0.6f) {
                    if (!bl) {
                        ST0820.this.fire02.disp(true);
                        ST0820.this.fire03.disp(true);
                        ST0820.this.fire04.disp(true);
                        ST0820.this.fire05.disp(true);
                        ST0820.this.fire06.disp(true);
                        ST0820.this.light01.disp(false);
                        ST0820.this.light02.disp(false);
                        ST0820.this.light03.disp(false);
                        ST0820.this.light04.disp(false);
                        ST0820.this.light05.disp(false);
                        ST0820.this.light06.disp(false);
                    }
                    bl = true;
                } else {
                    if (bl) {
                        ST0820.this.fire02.disp(false);
                        ST0820.this.fire03.disp(false);
                        ST0820.this.fire04.disp(false);
                        ST0820.this.fire05.disp(false);
                        ST0820.this.fire06.disp(false);
                        ST0820.this.light01.disp(true);
                        ST0820.this.light02.disp(true);
                        ST0820.this.light03.disp(true);
                        ST0820.this.light04.disp(true);
                        ST0820.this.light05.disp(true);
                        ST0820.this.light06.disp(true);
                    }
                    bl = false;
                }
                System.sleep(1);
                if ((n += 2) != 10000) continue;
                n = 0;
            }
        }
    }
}

