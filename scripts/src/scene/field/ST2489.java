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
import xeno.map.MC_KAS14_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST2480
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS14_PRJ {
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
    Enepc HASIGO;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Unit unit1;
    MAPUnit mapunit;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect[] sibuki;
    int sibuki_kazu = 0;
    int[] sibuki_start;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] btn = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] btn_no = new String[]{"It's locked.", "/[waitkey(64)]/[close()]"};

    ST2480() {
    }

    void Final_init(int n) {
    }

    public void HashigoTop(int n) {
        switch (n) {
            case 0: {
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                System.println("瓦礫２【MC_KAS13】・３・シオン");
                Runtime.jumpCF(2479, 3);
                break;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                System.println("瓦礫２【MC_KAS13】・２・シオン");
                Runtime.jumpCF(2479, 2);
                break;
            }
            case 2: {
                if (Runtime.getFlags(334, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(334, 1, 1);
                    System.println("イベント3022");
                    Runtime.jumpEvent(3220);
                    break;
                }
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                System.println("タワー【MC_KAS05】・２・Jr");
                Runtime.jumpCF(2449, 2);
                break;
            }
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
                this.lo = 1;
                System.println("lo:0");
                if (Runtime.getFlags(8064, 1) == 0) {
                    this.enemy1.kickEnepc(4, 2);
                    this.enemy2.kickEnepc(4, 2);
                    this.enemy3.kickEnepc(4, 2);
                    this.enemy4.kickEnepc(4, 2);
                    this.enemy5.kickEnepc(4, 2);
                    this.enemy6.kickEnepc(4, 2);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.nwin(this.btn);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(8064, 1, 1);
                            this.cam0.setMode(-1);
                            this.cam1 = Camera.create(1);
                            this.cam1.setRotate(-21.0f, 18.1f, 0.0f);
                            this.cam1.setTranslate(11.3f, 6.6f, 5.1f);
                            this.cam1.setFov(40.0f);
                            this.cam1.change();
                            Runtime.enable(65536);
                            this.player.rotY(10, 180.0f, true);
                            System.sleep(10);
                            this.player.mtn(25, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(58);
                            System.sleep(10);
                            Runtime.disable(65536);
                            this.light01.disp(false);
                            this.doorA.SetDoorType('\u0004');
                            this.light02.disp(false);
                            this.light03.disp(true);
                            System.sleep(20);
                            this.lo = 0;
                            this.cam0.setMode(0);
                            Runtime.setPlayerControl(true);
                            this.enemy1.kickEnepc(4, 0);
                            this.enemy2.kickEnepc(4, 0);
                            this.enemy3.kickEnepc(4, 0);
                            this.enemy4.kickEnepc(4, 0);
                            this.enemy5.kickEnepc(4, 0);
                            this.enemy6.kickEnepc(4, 0);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    this.enemy1.kickEnepc(4, 0);
                    this.enemy2.kickEnepc(4, 0);
                    this.enemy3.kickEnepc(4, 0);
                    this.enemy4.kickEnepc(4, 0);
                    this.enemy5.kickEnepc(4, 0);
                    this.enemy6.kickEnepc(4, 0);
                    return;
                }
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:1");
                if (Runtime.getFlags(8064, 1) == 0) {
                    this.nwin(this.btn_no);
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
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("公園【MC_KAS31】・３・シオン");
                Runtime.jumpCF(2569, 3);
                break;
            }
            case 1: {
                System.println("地下鉄トンネル【MC_KAS20】・３・Jr");
                Runtime.jumpCF(2549, 3);
                break;
            }
            case 2: {
                if (Runtime.getFlags(334, 1) == 0) {
                    if (Runtime.getFlags(8071, 1) == 0) {
                        Runtime.setFlags(8071, 1, 1);
                        Runtime.resetOutFriend(6);
                        Runtime.resetOutFriend(4);
                        Runtime.resetOutFriend(5);
                        Runtime.setOutFriend(1);
                        Runtime.setOutFriend(3);
                        Runtime.setPartyData(0x1010000, 5);
                        Runtime.setPartyData(65538, 1);
                        Runtime.setPartyData(0x1010004, 6);
                        Runtime.setPartyData(65542, 2);
                        Runtime.setPartyData(0x1010008, 7);
                        Runtime.setPartyData(65546, 3);
                        Runtime.setPartyData(16777260, 5);
                    }
                    Runtime.jumpCF(2439, 3);
                    break;
                }
                System.println("森入り口【MC_KAS15】・１・シオン");
                Runtime.jumpCF(2499, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -17.0f, 0.0f, 1.75f);
        this.teiten1.SetBgm(196610);
        this.teiten2 = new Uwamono(28690, 4.65f, 0.0f, -1.85f);
        this.teiten2.SetBgm(196610);
        Stage.setVisible(-1, true);
        Stage.setColor(1.25f, 1.25f, 1.25f);
        this.light.setColor(0, 0.375f, 0.375f, 0.325f);
        this.light.setColor(1, 0.35f, 0.35f, 0.325f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.325f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.325f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(3, 1, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(3, 2, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(3, 3, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(4, 1, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(4, 2, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightCol(4, 3, 0.285f, 0.285f, 0.29f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.325f, 0.325f, 0.3f);
        Runtime.setIdLightCol(5, 1, 0.325f, 0.325f, 0.3f);
        Runtime.setIdLightCol(5, 2, 0.325f, 0.325f, 0.3f);
        Runtime.setIdLightCol(5, 3, 0.325f, 0.325f, 0.3f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.375f, 0.375f, 0.325f);
        Runtime.setIdLightCol(6, 1, 0.35f, 0.35f, 0.325f);
        Runtime.setIdLightCol(6, 2, 0.35f, 0.35f, 0.325f);
        Runtime.setIdLightCol(6, 3, 0.35f, 0.35f, 0.325f);
        Runtime.setIdLightVec(6, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1056, 7.57f, 3.48f, -1.82f, 0.0f);
        this.light01.setRotate(40.0f, 0.0f, 0.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1763, 10.35f, 4.9f, -6.4f, 0.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1764, 10.35f, 4.9f, -6.4f, 0.0f);
        this.light03.disp(false);
        float[][] fArrayArray = new float[][]{{-20.1f, 2.4f, -7.8f}, {-12.0f, 2.4f, -2.2f}, {-0.9f, 2.4f, -4.8f}, {17.1f, 2.4f, 8.0f}, {-19.7f, 2.4f, 10.3f}, {-8.0f, 2.4f, 13.5f}};
        this.sibuki_kazu = fArrayArray.length;
        this.sibuki = new Effect[this.sibuki_kazu];
        this.sibuki_start = new int[this.sibuki_kazu];
        int n = 0;
        while (n < this.sibuki_kazu) {
            this.sibuki[n] = new Effect(1496, fArrayArray[n][0], fArrayArray[n][1], fArrayArray[n][2], 0.0f);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFLockX(2, -21.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFPedestal(6, -5.766711f, 12.233509f, 9.54601f, 40.0f, -34.447567f, 53.781174f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFPedestal(7, 25.12926f, 10.327862f, 6.142165f, 40.0f, -35.245747f, 31.757225f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 12.5f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.015f, 0.015f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(10, 0.02f, 0.02f);
        this.cam0.setCFLockX(10, 10.5f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.015f, 0.015f);
        this.cam0.setCFLockX(11, 10.5f);
        float[] fArray = new float[12];
        fArray[0] = 4.5f;
        fArray[1] = 2.4f;
        fArray[2] = 1.0f;
        fArray[3] = -1.0f;
        fArray[4] = 4.0f;
        fArray[5] = 2.4f;
        fArray[6] = 1.0f;
        fArray[8] = 5.0f;
        fArray[9] = 2.4f;
        fArray[10] = 1.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16387, 1, 1, 29, 7, 4.5f, 2.4f, 1.0f, 270.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[12];
        fArray3[0] = 12.6f;
        fArray3[1] = 2.4f;
        fArray3[2] = 12.5f;
        fArray3[3] = -1.0f;
        fArray3[4] = 12.1f;
        fArray3[5] = 2.4f;
        fArray3[6] = 12.5f;
        fArray3[8] = 13.1f;
        fArray3[9] = 2.4f;
        fArray3[10] = 12.5f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16387, 2, 2, 29, 7, 12.6f, 2.4f, 12.5f, 270.0f, fArray4);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[12];
        fArray5[0] = 17.8f;
        fArray5[1] = 2.4f;
        fArray5[2] = -7.7f;
        fArray5[3] = -1.0f;
        fArray5[4] = 17.8f;
        fArray5[5] = 2.4f;
        fArray5[6] = -7.2f;
        fArray5[8] = 17.8f;
        fArray5[9] = 2.4f;
        fArray5[10] = -8.2f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16392, 3, 3, 36, 5, 17.8f, 2.4f, -7.7f, 0.0f, fArray6);
        this.enemy3.setGroup(2, 2, 2, 2);
        float[] fArray7 = new float[12];
        fArray7[0] = -13.5f;
        fArray7[2] = 6.5f;
        fArray7[3] = -1.0f;
        fArray7[4] = -13.3f;
        fArray7[6] = 6.3f;
        fArray7[8] = -13.8f;
        fArray7[10] = 6.8f;
        float[] fArray8 = fArray7;
        this.enemy4 = new NpcEnemy(20227, 4, 4, 29, 3, -13.5f, 0.0f, 6.5f, 270.0f, fArray8);
        this.enemy4.setGroup(0, 0, 1, 1);
        float[] fArray9 = new float[12];
        fArray9[0] = -4.5f;
        fArray9[2] = 0.35f;
        fArray9[3] = -1.0f;
        fArray9[4] = -4.3f;
        fArray9[6] = 0.33f;
        fArray9[8] = -4.7f;
        fArray9[10] = 0.37f;
        float[] fArray10 = fArray9;
        this.enemy5 = new NpcEnemy(20227, 5, 5, 29, 3, -4.5f, 0.0f, 0.35f, 270.0f, fArray10);
        this.enemy5.setGroup(0, 0, 1, 1);
        float[] fArray11 = new float[12];
        fArray11[0] = 11.3f;
        fArray11[2] = 6.65f;
        fArray11[3] = -1.0f;
        fArray11[4] = 11.8f;
        fArray11[6] = 6.65f;
        fArray11[8] = 10.8f;
        fArray11[10] = 6.65f;
        float[] fArray12 = fArray11;
        this.enemy6 = new NpcEnemy(20227, 6, 6, 29, 3, 11.3f, 0.0f, 6.65f, 270.0f, fArray12);
        this.enemy6.setGroup(0, 0, 1, 1);
        this.item1 = new Uwamono(28677, 10.4f, 2.5f, -10.4f, 180.0f, 421);
        this.item1.SetSymbol(28686);
        this.item1.SetCallNo(1);
        new Uwamono(28672, 7.8f, -1.0f, 4.8f, 0.0f);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 286);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 287);
        new Uwamono(0, 18, this.item3);
        new Uwamono(6, 30, this.item2);
        this.doorA = new Uwamono(14, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.enemy6.kickEnepc(19, 1, 3, 350, 1);
        this.enemy6.kickEnepc(19, 2, 0, 630, 1);
        this.enemy6.kickEnepc(19, 3, 0, 700, 1);
        this.enemy6.kickEnepc(19, 4, 3, 220, 1);
        if (Runtime.getFlags(8064, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
            this.light01.disp(false);
            this.light02.disp(false);
            this.light03.disp(true);
        }
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                System.println("ITEM:1");
                Runtime.setFlags(3232, 1, 1);
                break;
            }
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2480.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2480.waitPage(this.win, 64);
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
    }
}

