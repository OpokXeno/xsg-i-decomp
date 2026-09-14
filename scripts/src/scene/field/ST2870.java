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
import xeno.map.MC_UTA17_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2870
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA17_PRJ {
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
    int camNO = 0;
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
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect light10;
    Effect light11;
    Effect light12;
    Effect light13;
    Effect light14;
    Effect light15;
    Effect light16;
    Effect light17;
    Effect light18;
    Effect light19;
    Effect light20;
    Effect light21;
    Effect light22;
    Effect light23;
    Effect light24;
    Effect light25;
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
    MAPUnit elv1;
    MAPUnit elv2;
    int ele1_move = 0;
    int ele2_move = 0;
    MAPUnit h1;
    MAPUnit h1tR;
    MAPUnit h1tL;
    MAPUnit h2;
    MAPUnit h2tR;
    MAPUnit h2tL;
    MAPUnit h3;
    MAPUnit h3tRR;
    MAPUnit h3tRL;
    MAPUnit h3tL;
    boolean h1_move = false;
    boolean h2_move = false;
    boolean h3_move = false;
    boolean player_move = true;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    int entrance;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] h = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] ele_serifu = new String[]{"Operate the elevator?", "/[waitkey(64)]/[close()]"};
    String[] momo = new String[]{"/[label(MOMO)]", "I am not feeling well, so please exclude me from the battle members. Please.", "/[waitkey(64)]/[close()]"};
    String[] momo1 = new String[]{"/[label(MOMO)]", "Umm...", "/[waitkey(64)]/[close()]"};
    String[] momo2 = new String[]{"/[label(MOMO)]", "I am not feeling well, so please exclude me from the battle members. Thank you.", "/[waitkey(64)]/[close()]"};
    String[] momo3 = new String[]{"/[label(MOMO)]", "Yes...", "/[waitkey(64)]/[close()]"};
    String[] shion = new String[]{"/[label(Shion)]", "MOMO, are you okay?", "/[waitkey(64)]/[close()]"};

    ST2870() {
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
                if (Runtime.getFlags(8055, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("第１の塔クリアボタン");
                    this.nwin(this.h);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(8055, 1, 1);
                            Runtime.disable(524288);
                            this.light01.disp(false);
                            this.cam0.setMode(-1);
                            this.cam1.setRotate(-35.1f, -36.3f, 0.0f);
                            this.cam1.setTranslate(11.2f, 22.4f, 2.8f);
                            this.cam1.setFov(40.0f);
                            this.cam1.change();
                            float[] fArray = new float[8];
                            fArray[1] = -35.1f;
                            fArray[2] = -36.3f;
                            fArray[4] = 540.0f;
                            fArray[5] = -30.3f;
                            fArray[6] = -23.3f;
                            float[] fArray2 = fArray;
                            float[] fArray3 = new float[8];
                            fArray3[1] = 11.2f;
                            fArray3[2] = 22.4f;
                            fArray3[3] = 2.8f;
                            fArray3[4] = 540.0f;
                            fArray3[5] = -3.0f;
                            fArray3[6] = 74.3f;
                            fArray3[7] = 44.2f;
                            float[] fArray4 = fArray3;
                            this.cam1.rotateSPL(fArray2, 0, 3, 540);
                            this.cam1.transSPL(fArray4, 0, 3, 540);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            Runtime.enable(65536);
                            this.player.rotY(10, 90.0f, true);
                            System.sleep(10);
                            this.player.mtn(25, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(58);
                            System.sleep(10);
                            Runtime.disable(65536);
                            System.sleep(490);
                            System.sleep(20);
                            this.light15.disp(true);
                            Sound.effectPlay(196745);
                            System.sleep(10);
                            float[] fArray5 = new float[8];
                            fArray5[1] = -30.3f;
                            fArray5[2] = -23.3f;
                            fArray5[4] = 360.0f;
                            fArray5[5] = -17.8f;
                            fArray5[6] = -22.1f;
                            float[] fArray6 = fArray5;
                            float[] fArray7 = new float[8];
                            fArray7[1] = -3.0f;
                            fArray7[2] = 74.3f;
                            fArray7[3] = 44.2f;
                            fArray7[4] = 360.0f;
                            fArray7[5] = -2.0f;
                            fArray7[6] = 46.3f;
                            fArray7[7] = 19.5f;
                            float[] fArray8 = fArray7;
                            this.cam1.rotateSPL(fArray6, 0, 3, 540);
                            this.cam1.transSPL(fArray8, 0, 3, 540);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            System.sleep(360);
                            this.light18.disp(true);
                            Sound.effectPlay(196746);
                            System.sleep(90);
                            this.h1tR.setVisible(true);
                            this.h1tL.setVisible(true);
                            this.h1.setVisible(true);
                            this.light10.disp(true);
                            System.sleep(80);
                            this.light08.disp(true);
                            System.sleep(60);
                            this.cam0.setMode(0);
                            Runtime.enable(524288);
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 1: {
                if (Runtime.getFlags(8056, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("第２の塔クリアボタン");
                    this.nwin(this.h);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(8056, 1, 1);
                            this.camNO = 6;
                            this.cam0.setMode(-1);
                            this.cam1.setRotate(-48.9f, 0.0f, 0.0f);
                            this.cam1.setTranslate(4.0f, 25.8f, -31.0f);
                            this.cam1.setFov(40.0f);
                            this.cam1.change();
                            float[] fArray = new float[8];
                            fArray[1] = -48.9f;
                            fArray[4] = 360.0f;
                            fArray[5] = -48.9f;
                            float[] fArray9 = fArray;
                            float[] fArray10 = new float[8];
                            fArray10[1] = 4.0f;
                            fArray10[2] = 25.8f;
                            fArray10[3] = -31.0f;
                            fArray10[4] = 360.0f;
                            fArray10[6] = 76.6f;
                            fArray10[7] = -25.6f;
                            float[] fArray11 = fArray10;
                            this.cam1.rotateSPL(fArray9, 0, 3, 360);
                            this.cam1.transSPL(fArray11, 0, 3, 360);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            Runtime.disable(524288);
                            Runtime.enable(65536);
                            this.player.rotY(10, 180.0f, true);
                            System.sleep(10);
                            this.player.mtn(25, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(58);
                            System.sleep(10);
                            Runtime.disable(65536);
                            this.light02.disp(false);
                            System.sleep(310);
                            System.sleep(20);
                            this.light16.disp(true);
                            Sound.effectPlay(196743);
                            System.sleep(10);
                            float[] fArray12 = new float[8];
                            fArray12[1] = -48.9f;
                            fArray12[4] = 360.0f;
                            fArray12[5] = -42.7f;
                            float[] fArray13 = fArray12;
                            float[] fArray14 = new float[8];
                            fArray14[2] = 76.6f;
                            fArray14[3] = -25.6f;
                            fArray14[4] = 360.0f;
                            fArray14[5] = -13.1f;
                            fArray14[6] = 53.6f;
                            fArray14[7] = -9.0f;
                            float[] fArray15 = fArray14;
                            this.cam1.rotateSPL(fArray13, 0, 3, 360);
                            this.cam1.transSPL(fArray15, 0, 3, 360);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            System.sleep(270);
                            this.light19.disp(true);
                            Sound.effectPlay(196746);
                            System.sleep(90);
                            this.h2tR.setVisible(true);
                            this.h2tL.setVisible(true);
                            this.h2.setVisible(true);
                            this.light11.disp(true);
                            float[] fArray16 = new float[8];
                            fArray16[1] = -42.7f;
                            fArray16[4] = 360.0f;
                            fArray16[5] = -42.7f;
                            float[] fArray17 = fArray16;
                            float[] fArray18 = new float[8];
                            fArray18[1] = -13.1f;
                            fArray18[2] = 53.6f;
                            fArray18[3] = -9.0f;
                            fArray18[4] = 360.0f;
                            fArray18[6] = 53.6f;
                            fArray18[7] = 21.2f;
                            float[] fArray19 = fArray18;
                            this.cam1.rotateSPL(fArray17, 0, 3, 360);
                            this.cam1.transSPL(fArray19, 0, 3, 360);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            System.sleep(360);
                            this.light07.disp(true);
                            System.sleep(60);
                            Runtime.enable(524288);
                            this.cam0.setMode(0);
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 2: {
                if (Runtime.getFlags(8056, 1) == 1) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("第２の塔エレベータ");
                    this.nwin(this.ele_serifu);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.player.getTranslate();
                            if (this.player.py > 39.0f) {
                                System.println("上");
                                this.ele1_move = 1;
                                this.player_move = true;
                                this.camNO = 3;
                            } else {
                                System.println("下");
                                this.ele1_move = 2;
                                this.player_move = true;
                                this.camNO = 3;
                            }
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 3: {
                if (Runtime.getFlags(8057, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("第３の塔クリアボタン");
                    this.nwin(this.h);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(8057, 1, 1);
                            this.cam0.setMode(-1);
                            this.cam1.setRotate(-46.4f, -25.0f, 0.0f);
                            this.cam1.setTranslate(-27.7f, 23.6f, 18.5f);
                            this.cam1.setFov(40.0f);
                            this.cam1.change();
                            float[] fArray = new float[8];
                            fArray[1] = -46.4f;
                            fArray[2] = -25.0f;
                            fArray[4] = 360.0f;
                            fArray[5] = -46.4f;
                            fArray[6] = -25.0f;
                            float[] fArray20 = fArray;
                            float[] fArray21 = new float[8];
                            fArray21[1] = -27.7f;
                            fArray21[2] = 23.6f;
                            fArray21[3] = 18.5f;
                            fArray21[4] = 360.0f;
                            fArray21[5] = -35.9f;
                            fArray21[6] = 75.9f;
                            fArray21[7] = 27.9f;
                            float[] fArray22 = fArray21;
                            this.cam1.rotateSPL(fArray20, 0, 3, 360);
                            this.cam1.transSPL(fArray22, 0, 3, 360);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            Runtime.disable(524288);
                            Runtime.enable(65536);
                            this.player.rotY(10, 180.0f, true);
                            System.sleep(10);
                            this.player.mtn(25, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(58);
                            System.sleep(10);
                            Runtime.disable(65536);
                            this.light03.disp(false);
                            System.sleep(310);
                            System.sleep(20);
                            this.light04.disp(true);
                            Sound.effectPlay(196744);
                            System.sleep(10);
                            float[] fArray23 = new float[8];
                            fArray23[1] = -46.4f;
                            fArray23[2] = -25.0f;
                            fArray23[4] = 360.0f;
                            fArray23[5] = -46.4f;
                            fArray23[6] = -25.0f;
                            float[] fArray24 = fArray23;
                            float[] fArray25 = new float[8];
                            fArray25[1] = -35.9f;
                            fArray25[2] = 75.9f;
                            fArray25[3] = 27.9f;
                            fArray25[4] = 360.0f;
                            fArray25[5] = -6.5f;
                            fArray25[6] = 55.1f;
                            fArray25[7] = 17.1f;
                            float[] fArray26 = fArray25;
                            this.cam1.rotateSPL(fArray24, 0, 3, 360);
                            this.cam1.transSPL(fArray26, 0, 3, 360);
                            this.cam1.setFov(39.9f);
                            this.cam1.change();
                            System.sleep(270);
                            this.light20.disp(true);
                            Sound.effectPlay(196746);
                            this.light25.disp(true);
                            System.sleep(90);
                            this.h3tRR.setVisible(true);
                            this.h3tRL.setVisible(true);
                            this.h3tL.setVisible(true);
                            this.h3.setVisible(true);
                            this.light12.disp(true);
                            System.sleep(80);
                            this.light09.disp(true);
                            System.sleep(60);
                            this.cam0.setMode(0);
                            Runtime.enable(524288);
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 4: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("第３の塔エレベータ");
                this.nwin(this.ele_serifu);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        this.player.getTranslate();
                        if (this.player.py > 39.0f) {
                            System.println("上");
                            this.ele2_move = 1;
                        } else {
                            System.println("下");
                            this.ele2_move = 2;
                        }
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 5: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("5 第２の塔エレベータ下");
                this.player.getTranslate();
                if (this.player.py < 19.0f && Runtime.getFlags(8058, 1) == 1 && Runtime.getFlags(8056, 1) == 1) {
                    this.nwin(this.ele_serifu);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.ele1_move = 1;
                            this.player_move = false;
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 6: {
                if (this.camNO != 3) break;
                this.camNO = 0;
                this.cam0.setMode(0);
                break;
            }
            case 7: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("7 第２の塔エレベータ上");
                this.player.getTranslate();
                if (this.player.py > 39.0f && Runtime.getFlags(8058, 1) == 0 && Runtime.getFlags(8056, 1) == 1) {
                    this.nwin(this.ele_serifu);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.ele1_move = 2;
                            this.player_move = false;
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 8: {
                if (this.camNO != 3) break;
                this.camNO = 0;
                this.cam0.setMode(0);
                break;
            }
            case 9: {
                System.println("9");
                break;
            }
            case 10: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("10");
                if (Runtime.getFlags(376, 1) == 0) {
                    if (Runtime.getPartyData(0x1010000) == 6 || Runtime.getPartyData(0x1010004) == 6 || Runtime.getPartyData(0x1010008) == 6) {
                        if (Runtime.getFlags(8116, 1) == 0) {
                            Runtime.setFlags(8116, 1, 1);
                            System.println("バトルメンバーにモモがいる");
                            this.fade.call(0);
                            System.sleep(1);
                            this.npc1.kickEnepc(4, 0);
                            this.npc2.kickEnepc(4, 0);
                            this.npc3.kickEnepc(4, 0);
                            this.npc4.kickEnepc(4, 0);
                            this.npc5.kickEnepc(4, 0);
                            this.npc6.kickEnepc(4, 0);
                            System.sleep(1);
                            this.npc1.kickEnepc(4, 1);
                            this.npc2.kickEnepc(4, 1);
                            this.npc3.kickEnepc(4, 1);
                            this.npc4.kickEnepc(4, 1);
                            this.npc5.kickEnepc(4, 1);
                            this.npc6.kickEnepc(4, 1);
                            System.sleep(28);
                            this.npc1.setVisible(true);
                            this.npc2.setVisible(true);
                            this.npc3.setVisible(true);
                            this.npc4.setVisible(true);
                            this.npc5.setVisible(true);
                            this.npc6.setVisible(true);
                            this.player.setVisible(false);
                            this.cam0.setMode(-1);
                            this.cam1.setRotate(-42.9f, 59.2f, 0.0f);
                            this.cam1.setTranslate(5.1f, 6.1f, 2.5f);
                            this.cam1.setFov(40.0f);
                            this.cam1.change();
                            this.nwin(this.momo1);
                            this.npc1.kickEnepc(3, 1, 0, 26, 1, 70);
                            this.npc1.moveEnepc(15, 0.0f, 0.0f, 120);
                            System.sleep(120);
                            this.npc1.kickEnepc(1, 0);
                            this.nwin(this.momo2);
                            this.npc6.kickEnepc(0, 9);
                            this.nwin(this.shion);
                            this.npc1.look_char(this.npc6);
                            this.npc1.kickEnepc(0, 7);
                            this.nwin(this.momo3);
                            this.fade.call(0);
                            System.sleep(27);
                            this.npc1.kickEnepc(4, 0);
                            this.npc2.kickEnepc(4, 0);
                            this.npc3.kickEnepc(4, 0);
                            this.npc4.kickEnepc(4, 0);
                            this.npc5.kickEnepc(4, 0);
                            this.npc6.kickEnepc(4, 0);
                            System.sleep(1);
                            this.npc1.kickEnepc(4, 2);
                            this.npc2.kickEnepc(4, 2);
                            this.npc3.kickEnepc(4, 2);
                            this.npc4.kickEnepc(4, 2);
                            this.npc5.kickEnepc(4, 2);
                            this.npc6.kickEnepc(4, 2);
                            System.sleep(1);
                            this.npc1.setVisible(false);
                            this.npc2.setVisible(false);
                            this.npc3.setVisible(false);
                            this.npc4.setVisible(false);
                            this.npc5.setVisible(false);
                            this.npc6.setVisible(false);
                            this.player.setVisible(true);
                            this.cam0.setMode(0);
                            System.sleep(1);
                            this.cam0.changeID(2);
                        } else {
                            this.nwin(this.momo);
                        }
                    } else {
                        this.doorA.DoorOpen();
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setFlags(376, 1, 1);
                        Runtime.jumpEvent(3471);
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                break;
            }
            case 11: {
                System.println("11");
                break;
            }
        }
    }

    void cam1() {
        this.cam1.setRotate(-18.2f, 0.0f, 0.0f);
        this.cam1.setTranslate(0.0f, 69.0f, 58.2f);
        this.cam1.setFov(40.0f);
        this.cam1.change();
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(2860, 4);
                break;
            }
            case 1: {
                this.super_setFlags(8042, 3, 1);
                Runtime.jumpCF(2720, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(2740, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(2770, 2);
                break;
            }
            case 4: {
                Runtime.jumpCF(2790, 5);
                break;
            }
            case 5: {
                Runtime.jumpCF(2790, 11);
                break;
            }
            case 6: {
                Runtime.jumpCF(2790, 4);
                break;
            }
            case 7: {
                Runtime.jumpCF(2820, 1);
                break;
            }
            case 8: {
                Runtime.jumpCF(2820, 4);
                break;
            }
            case 9: {
                Runtime.jumpCF(2840, 2);
                break;
            }
            case 10: {
                Runtime.jumpCF(2830, 3);
                break;
            }
            case 11: {
                Runtime.jumpCF(2820, 3);
                break;
            }
            case 12: {
                Runtime.jumpCF(2770, 3);
                break;
            }
            case 13: {
                Runtime.jumpCF(2710, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
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
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        this.h1 = new Mapunits();
        this.h1.mapUnit(20);
        this.h1.start(4, null);
        this.h1.setVisible(false);
        this.h1tR = new Mapunits();
        this.h1tR.mapUnit(21);
        this.h1tR.start(4, null);
        this.h1tR.setVisible(false);
        this.h1tL = new Mapunits();
        this.h1tL.mapUnit(22);
        this.h1tL.start(4, null);
        this.h1tL.setVisible(false);
        this.h2 = new Mapunits();
        this.h2.mapUnit(45);
        this.h2.start(4, null);
        this.h2.setVisible(false);
        this.h2tR = new Mapunits();
        this.h2tR.mapUnit(47);
        this.h2tR.start(4, null);
        this.h2tR.setVisible(false);
        this.h2tL = new Mapunits();
        this.h2tL.mapUnit(46);
        this.h2tL.start(4, null);
        this.h2tL.setVisible(false);
        this.h3 = new Mapunits();
        this.h3.mapUnit(0);
        this.h3.start(4, null);
        this.h3.setVisible(false);
        this.h3tRR = new Mapunits();
        this.h3tRR.mapUnit(28);
        this.h3tRR.start(4, null);
        this.h3tRR.setVisible(false);
        this.h3tRL = new Mapunits();
        this.h3tRL.mapUnit(41);
        this.h3tRL.start(4, null);
        this.h3tRL.setVisible(false);
        this.h3tL = new Mapunits();
        this.h3tL.mapUnit(23);
        this.h3tL.start(4, null);
        this.h3tL.setVisible(false);
        this.light01 = new Effect(1056, 15.2f, 19.3f, -1.0f, 0.0f);
        this.light01.setRotate(0.0f, 0.0f, 10.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1056, 3.8f, 19.3f, -37.7f, 0.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1056, -26.1f, 19.3f, 14.2f, 0.0f);
        this.light03.noAttach(false);
        this.light03.disp(true);
        this.light15 = new Effect(1601, 30.0f, 53.5f, 0.2f, 0.0f);
        this.light15.disp(false);
        this.light16 = new Effect(1600, 0.0f, 53.5f, -52.2f, 0.0f);
        this.light16.disp(false);
        this.light04 = new Effect(1599, -30.0f, 53.5f, 0.2f, 0.0f);
        this.light04.disp(false);
        this.light05 = new Effect(1542, -8.69f, 40.05f, -33.67f, 0.0f);
        this.light05.setRotate(90.0f, 0.0f, 0.0f);
        this.light05.setScale(0.4f, 0.4f, 0.4f);
        this.light05.disp(false);
        this.light06 = new Effect(1542, -6.01f, 18.05f, -34.03f, 0.0f);
        this.light06.setRotate(90.0f, 0.0f, 0.0f);
        this.light06.setScale(0.4f, 0.4f, 0.4f);
        this.light06.disp(false);
        this.light07 = new Effect(1603, 0.0f, 41.4f, 7.8f, 0.0f);
        this.light07.setScale(1.0f, 3.0f, 1.0f);
        this.light07.noAttach(false);
        this.light07.disp(false);
        this.light08 = new Effect(1604, 0.6f, 41.4f, 8.8f, 0.0f);
        this.light08.noAttach(false);
        this.light08.setScale(1.0f, 3.0f, 1.0f);
        this.light08.disp(false);
        this.light09 = new Effect(1602, -0.6f, 41.4f, 8.8f, 0.0f);
        this.light09.noAttach(false);
        this.light09.setScale(1.0f, 3.0f, 1.0f);
        this.light09.disp(false);
        this.light10 = new Effect(1605, 15.0f, 37.5f, -26.0f, 0.0f);
        this.light10.noAttach(false);
        this.light10.setRotate(0.0f, 30.0f, 0.0f);
        this.light10.disp(false);
        this.light11 = new Effect(1605, -15.0f, 37.5f, -26.0f, 0.0f);
        this.light11.noAttach(false);
        this.light11.setRotate(0.0f, -30.0f, 0.0f);
        this.light11.disp(false);
        this.light12 = new Effect(1605, 0.0f, 37.5f, 0.1f, 0.0f);
        this.light12.noAttach(false);
        this.light12.setRotate(0.0f, 90.0f, 0.0f);
        this.light12.disp(false);
        this.light18 = new Effect(1606, 15.0f, 40.0f, -26.0f, 0.0f);
        this.light18.noAttach(false);
        this.light18.setRotate(0.0f, 120.0f, 0.0f);
        this.light18.setScale(5.0f, 1.0f, 1.0f);
        this.light18.disp(false);
        this.light19 = new Effect(1607, -15.0f, 40.0f, -26.0f, 0.0f);
        this.light19.noAttach(false);
        this.light19.setScale(5.0f, 1.0f, 1.0f);
        this.light19.setRotate(0.0f, 600.0f, 0.0f);
        this.light19.disp(false);
        this.light20 = new Effect(1629, 0.0f, 40.0f, 0.0f, 0.0f);
        this.light20.noAttach(false);
        this.light20.setScale(5.0f, 1.0f, 1.0f);
        this.light20.disp(false);
        this.light25 = new Effect(1629, 0.0f, 40.0f, 4.0f, 0.0f);
        this.light25.noAttach(false);
        this.light25.setRotate(0.0f, -90.0f, 0.0f);
        this.light25.disp(false);
        this.light14 = new Effect(1609, 0.0f, 25.0f, 0.0f, 0.0f);
        this.light14.setScale(30.0f, 30.0f, 30.0f);
        this.light14.noAttach(false);
        this.light13 = new Effect(1608, -7.3f, 18.0f, -32.2f, 0.0f);
        this.light13.noAttach(false);
        this.light13.setScale(1.4f, 1.0f, 1.4f);
        this.light13.setRotate(0.0f, -30.0f, 0.0f);
        this.light13.disp(false);
        this.light21 = new Effect(1608, -7.3f, 40.0f, -32.2f, 0.0f);
        this.light21.noAttach(false);
        this.light21.setScale(1.4f, 1.0f, 1.4f);
        this.light21.setRotate(180.0f, 30.0f, 0.0f);
        this.light21.disp(false);
        this.light17 = new Effect(1608, 0.0f, 0.0f, 8.3f, 0.0f);
        this.light17.noAttach(false);
        this.light17.setScale(1.6f, 2.0f, 1.6f);
        this.light17.disp(false);
        this.light22 = new Effect(1608, 0.0f, 40.0f, 8.3f, 0.0f);
        this.light22.noAttach(false);
        this.light22.setScale(1.6f, 2.0f, 1.6f);
        this.light22.setRotate(180.0f, 0.0f, 0.0f);
        this.light22.disp(false);
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 40.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 16.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 0.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 50.0f, 45.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 50.0f, 45.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, -44.1f, 33.36f, 21.19f, 45.0f, -35.34f, 10.66f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 42.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 40.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, -20.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, -20.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.cam0.setCFPedestal(15, -8.0f, 30.0f, -25.0f, 40.0f, -60.0f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(15, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(15, 1);
        this.cam0.setCFPedestal(16, -8.0f, 52.0f, -25.0f, 40.0f, -60.0f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(16, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(16, 1);
        this.cam0.setCFAngle(17, -28.0f, 0.0f, 0.0f, 40.0f, 45.0f);
        this.cam0.setCFHokan(17, 0.01f, 0.01f);
        this.cam0.setCFLockX(17, 0.0f);
        this.cam1 = Camera.create(1);
        this.cam0.setFog(0, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(1, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(2, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(3, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(4, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(5, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(6, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(7, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(8, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(9, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(10, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(11, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(12, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(13, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(14, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(15, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(16, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam0.setFog(17, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.cam1.setFog(1, 30.0f, 100.0f, 0.0f, 0.5f, 40, 40, 40, 0);
        this.npc1 = new NpcEnemy(4, 1, 0, 3, 20, 0.0f, 0.0f, 4.0f, 180.0f);
        this.npc2 = new NpcEnemy(5, 2, 0, 3, 23, 0.0f, 0.0f, -3.5f, 0.0f);
        this.npc3 = new NpcEnemy(6, 3, 0, 3, 26, -1.3f, 0.0f, -2.2f, 70.0f);
        this.npc4 = new NpcEnemy(3, 4, 0, 3, 29, 1.0f, 0.0f, -3.1f, 350.0f);
        this.npc5 = new NpcEnemy(2, 5, 0, 3, 32, 2.0f, 0.0f, -2.7f, 0.0f);
        this.npc6 = new NpcEnemy(1, 6, 0, 3, 35, -1.6f, 0.0f, -0.7f, 45.0f);
        this.npc1.disableDTKFlag(65536);
        this.npc2.disableDTKFlag(65536);
        this.npc3.disableDTKFlag(65536);
        this.npc4.disableDTKFlag(65536);
        this.npc5.disableDTKFlag(65536);
        this.npc6.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        this.npc2.setVisible(false);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc2.look_char(this.npc1);
        this.npc3.look_char(this.npc1);
        this.npc4.look_char(this.npc1);
        this.npc5.look_char(this.npc1);
        this.npc6.look_char(this.npc1);
        this.npc1.kickEnepc(4, 2);
        this.npc2.kickEnepc(4, 2);
        this.npc3.kickEnepc(4, 2);
        this.npc4.kickEnepc(4, 2);
        this.npc5.kickEnepc(4, 2);
        this.npc6.kickEnepc(4, 2);
        this.elv1 = new Mapunits();
        this.elv1.mapUnit(54);
        this.elv1.start(4, null);
        this.elv1.setTranslate(this.elv1.px, 18.0f, this.elv1.pz);
        this.elv2 = new Mapunits();
        this.elv2.mapUnit(53);
        this.elv2.start(4, null);
        this.doorA = new Uwamono(52, 42, '\u0001');
        new Uwamono(51, 42, '\u0001', this.doorA);
        if (Runtime.getFlags(376, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        Runtime.setRegister(1, (float) this.entrance);
        System.println("entrance: /[#1]");
        switch (this.entrance) {
            case 4: {
                if (Runtime.getFlags(8055, 1) == 1) {
                    this.player.setID(2);
                    break;
                }
                this.player.setID(4);
                break;
            }
            case 5: {
                if (Runtime.getFlags(8055, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 6: {
                if (Runtime.getFlags(8058, 1) == 0) {
                    this.elv1.setTranslate(this.elv1.px, 18.0f, this.elv1.pz);
                    if (Runtime.getFlags(8056, 1) == 1) {
                        this.player.setID(2);
                        break;
                    }
                    this.player.setID(4);
                    break;
                }
                this.elv1.setTranslate(this.elv1.px, 40.0f, this.elv1.pz);
                if (Runtime.getFlags(8056, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 7: {
                if (Runtime.getFlags(8058, 1) == 0) {
                    this.elv1.setTranslate(this.elv1.px, 18.0f, this.elv1.pz);
                    if (Runtime.getFlags(8056, 1) == 1) {
                        this.player.setID(2);
                        break;
                    }
                    this.player.setID(4);
                    break;
                }
                this.elv1.setTranslate(this.elv1.px, 40.0f, this.elv1.pz);
                if (Runtime.getFlags(8056, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 8: {
                if (Runtime.getFlags(8058, 1) == 0) {
                    this.elv1.setTranslate(this.elv1.px, 18.0f, this.elv1.pz);
                    if (Runtime.getFlags(8056, 1) == 1) {
                        this.player.setID(2);
                        break;
                    }
                    this.player.setID(4);
                    break;
                }
                this.elv1.setTranslate(this.elv1.px, 40.0f, this.elv1.pz);
                if (Runtime.getFlags(8056, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 12: {
                if (Runtime.getFlags(8057, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 13: {
                if (Runtime.getFlags(8057, 1) == 1) {
                    this.player.setID(1);
                    break;
                }
                this.player.setID(3);
                break;
            }
            case 14: {
                this.elv2.setTranslate(this.elv2.px, 0.0f, this.elv2.pz);
                this.light07.setTranslate(0.0f, 1.4f, 7.8f);
                this.light08.setTranslate(0.6f, 1.4f, 8.8f);
                this.light09.setTranslate(-0.6f, 1.4f, 8.8f);
                break;
            }
        }
        if (Runtime.getFlags(8055, 1) == 1) {
            this.h1.setVisible(true);
            this.h1tR.setVisible(true);
            this.h1tL.setVisible(true);
            this.light08.disp(true);
            this.light01.disp(false);
            this.light15.disp(true);
            this.light10.disp(true);
        }
        if (Runtime.getFlags(8056, 1) == 1) {
            this.h2.setVisible(true);
            this.h2tR.setVisible(true);
            this.h2tL.setVisible(true);
            this.light02.disp(false);
            this.light07.disp(true);
            this.light16.disp(true);
            this.light10.disp(true);
            this.light11.disp(true);
        }
        if (Runtime.getFlags(8057, 1) == 1) {
            this.h3.setVisible(true);
            this.h3tRR.setVisible(true);
            this.h3tRL.setVisible(true);
            this.h3tL.setVisible(true);
            this.light03.disp(false);
            this.light04.disp(true);
            this.light09.disp(true);
            this.light10.disp(true);
            this.light11.disp(true);
            this.light12.disp(true);
        }
        this.h1.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2870.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2870.waitPage(this.win, 64);
    }

    int super_getFlags(int n, int n2) {
        int n3 = 0;
        int n4 = 0;
        int n5 = n2 - 1;
        while (n5 >= 0) {
            n4 = Runtime.getFlags(n + n5, 1);
            n3 += (n4 <<= n5);
            --n5;
        }
        Runtime.setRegister(1, (float) n3);
        System.println("super_getFlags: /[#1]");
        return n3;
    }

    void super_setFlags(int n, int n2, int n3) {
        Runtime.setRegister(1, (float) n3);
        System.println("super_setFlags: /[#1]");
        int n4 = 0;
        int n5 = 0;
        while (n5 < n2) {
            n4 = n3;
            n4 >>= n5;
            Runtime.setFlags(n + n5, 1, n4 &= 1);
            ++n5;
        }
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
            this.setElevatorMode(1);
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
            float f2 = 460.0f;
            float f3 = 50.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 0.0f;
            ST2870.this.cam1 = Camera.create(1);
            ST2870.this.cam1.setFov(40.0f);
            int n = 0;
            float f8 = 0.0f;
            float f9 = 0.0f;
            float f10 = 0.0f;
            if (ST2870.this.entrance == 1) {
                if (Runtime.getFlags(8065, 1) == 0) {
                    Runtime.setFlags(8065, 1, 1);
                    Runtime.setPlayerControl(false);
                    ST2870.this.cam0.setMode(-1);
                    Runtime.disable(524288);
                    float[] fArray = new float[8];
                    fArray[1] = 12.6f;
                    fArray[2] = 379.6f;
                    fArray[4] = 360.0f;
                    fArray[5] = -30.339f;
                    fArray[6] = 360.0f;
                    float[] fArray2 = fArray;
                    float[] fArray3 = new float[8];
                    fArray3[1] = 4.85f;
                    fArray3[2] = 21.0f;
                    fArray3[3] = 61.8f;
                    fArray3[4] = 360.0f;
                    fArray3[5] = 53.0f;
                    fArray3[6] = 10.0f;
                    fArray3[7] = 38.0f;
                    float[] fArray4 = fArray3;
                    ST2870.this.cam1.rotateSPL(fArray2, 0, 3, 360);
                    ST2870.this.cam1.transSPL(fArray4, 0, 3, 360);
                    ST2870.this.cam1.setFov(40.0f);
                    ST2870.this.cam1.change();
                    System.sleep(360);
                    System.println("内部進入");
                    Runtime.setPlayerControl(true);
                    Runtime.enable(524288);
                    ST2870.this.cam0.setMode(0);
                }
            } else if (ST2870.this.entrance == 14) {
                System.sleep(1);
                ST2870.this.cam0.changeID(2);
            }
            while (true) {
                ST2870.this.player.getTranslate();
                if (ST2870.this.camNO == 0) {
                    if (n != ST2870.this.camNO) {
                        ST2870.this.cam0.setMode(0);
                        n = 0;
                    }
                } else if (ST2870.this.camNO == 2) {
                    if (n != ST2870.this.camNO) {
                        ST2870.this.cam0.setMode(-1);
                        ST2870.this.cam1.change();
                        n = 3;
                    }
                    ST2870.this.cam1();
                } else if (ST2870.this.camNO == 3) {
                    if (n != ST2870.this.camNO) {
                        ST2870.this.cam0.setMode(-1);
                        ST2870.this.cam1.change();
                        n = 3;
                    }
                    ST2870.this.cam1.setTranslate(-8.0f, ST2870.this.player.py + 12.0f, -25.0f);
                    ST2870.this.cam1.setRotate(-60.0f, 0.0f, 0.0f);
                } else if (ST2870.this.camNO == 4) {
                    if (n != ST2870.this.camNO) {
                        ST2870.this.cam0.setMode(-1);
                        ST2870.this.cam1.change();
                        n = 4;
                    }
                    ST2870.this.cam1.setTranslate(ST2870.this.player.px, 25.8f, 30.0f);
                    ST2870.this.cam1.setView(ST2870.this.player.px, ST2870.this.player.py + 1.3f, ST2870.this.player.pz);
                } else if (ST2870.this.camNO == 5) {
                    ST2870.this.elv1.getTranslate();
                    if (n != ST2870.this.camNO) {
                        ST2870.this.cam0.setMode(-1);
                        ST2870.this.cam1.change();
                        n = 5;
                    }
                    ST2870.this.cam1.setTranslate(-8.0f, ST2870.this.elv1.py + 12.0f, -25.0f);
                    ST2870.this.cam1.setRotate(-60.0f, 0.0f, 0.0f);
                } else if (ST2870.this.camNO == 6 && n != ST2870.this.camNO) {
                    n = 6;
                }
                if (ST2870.this.ele1_move != 0) {
                    ST2870.this.elv1.getTranslate();
                    if (f == 0.0f) {
                        if (ST2870.this.ele1_move == 1) {
                            f5 = 40.0f;
                            f6 = 18.0f;
                            ST2870.this.light21.disp(true);
                            if (!ST2870.this.player_move) {
                                ST2870.this.camNO = 5;
                            }
                        }
                        if (ST2870.this.ele1_move == 2) {
                            f5 = 18.0f;
                            f6 = 40.0f;
                            ST2870.this.light13.disp(true);
                            if (!ST2870.this.player_move) {
                                ST2870.this.camNO = 5;
                            }
                        }
                        Runtime.enable(65536);
                        f4 = (f6 - f5) / (f2 - f3);
                        Runtime.disable(524288);
                        Sound.streamPlay(1195011, 48000);
                    }
                    if (f > 0.0f && f <= f3) {
                        ST2870.this.elv1.setTranslate(ST2870.this.elv1.px, f5 += f * f4 / f3, ST2870.this.elv1.pz);
                        if (ST2870.this.player_move) {
                            ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        }
                    }
                    if (f > f3 && f <= f2 - f3) {
                        ST2870.this.elv1.setTranslate(ST2870.this.elv1.px, f5 += f3 * f4 / f3, ST2870.this.elv1.pz);
                        if (ST2870.this.player_move) {
                            ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        }
                    }
                    if (f > f2 - f3 && f <= f2) {
                        ST2870.this.elv1.setTranslate(ST2870.this.elv1.px, f5 += (f2 - f) * f4 / f3, ST2870.this.elv1.pz);
                        if (ST2870.this.player_move) {
                            ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        }
                    }
                    if (f >= f2) {
                        f = -1.0f;
                        ST2870.this.lo = 0;
                        ST2870.this.light13.disp(false);
                        ST2870.this.light13.clearEffect();
                        ST2870.this.light21.disp(false);
                        ST2870.this.light21.clearEffect();
                        if (!ST2870.this.player_move) {
                            ST2870.this.camNO = 0;
                            ST2870.this.cam0.setMode(0);
                        }
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        Runtime.enable(524288);
                        if (ST2870.this.ele1_move == 1) {
                            Runtime.setFlags(8058, 1, 0);
                            if (Runtime.getFlags(8056, 1) == 1) {
                                ST2870.this.player.setID(2);
                            } else {
                                ST2870.this.player.setID(4);
                            }
                        }
                        if (ST2870.this.ele1_move == 2) {
                            Runtime.setFlags(8058, 1, 1);
                            if (Runtime.getFlags(8056, 1) == 1) {
                                ST2870.this.player.setID(1);
                            } else {
                                ST2870.this.player.setID(3);
                            }
                        }
                        ST2870.this.ele1_move = 0;
                    }
                    f += 1.0f;
                }
                if (Runtime.getFlags(8056, 1) == 1) {
                    ST2870.this.light06.disp(false);
                    if (f7 == 0.0f) {
                        ST2870.this.light05.disp(true);
                        ST2870.this.light06.disp(true);
                    }
                    if (f7 == 15.0f) {
                        ST2870.this.light05.disp(false);
                        ST2870.this.light06.disp(false);
                    }
                    if ((f7 += 1.0f) == 30.0f) {
                        f7 = 0.0f;
                    }
                } else {
                    ST2870.this.light05.disp(false);
                    ST2870.this.light06.disp(false);
                }
                if (ST2870.this.ele2_move != 0) {
                    ST2870.this.elv2.getTranslate();
                    if (f == 0.0f) {
                        if (ST2870.this.ele2_move == 1) {
                            f5 = 40.0f;
                            f6 = 0.0f;
                            ST2870.this.light22.disp(true);
                            ST2870.this.cam0.changeID(2);
                        }
                        if (ST2870.this.ele2_move == 2) {
                            f5 = 0.0f;
                            f6 = 40.0f;
                            ST2870.this.light17.disp(true);
                        }
                        Runtime.disable(524288);
                        Runtime.enable(65536);
                        f4 = (f6 - f5) / (f2 - f3);
                        Sound.streamPlay(1195011, 48000);
                    }
                    if (f > 0.0f && f <= f3) {
                        ST2870.this.elv2.setTranslate(ST2870.this.elv2.px, f5 += f * f4 / f3, ST2870.this.elv2.pz);
                        ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        ST2870.this.light07.setTranslate(0.0f, f5 + 1.4f, 7.8f);
                        ST2870.this.light08.setTranslate(0.6f, f5 + 1.4f, 8.8f);
                        ST2870.this.light09.setTranslate(-0.6f, f5 + 1.4f, 8.8f);
                    }
                    if (f > f3 && f <= f2 - f3) {
                        ST2870.this.elv2.setTranslate(ST2870.this.elv2.px, f5 += f3 * f4 / f3, ST2870.this.elv2.pz);
                        ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        ST2870.this.light07.setTranslate(0.0f, f5 + 1.4f, 7.8f);
                        ST2870.this.light08.setTranslate(0.6f, f5 + 1.4f, 8.8f);
                        ST2870.this.light09.setTranslate(-0.6f, f5 + 1.4f, 8.8f);
                    }
                    if (f > f2 - f3 && f <= f2) {
                        ST2870.this.elv2.setTranslate(ST2870.this.elv2.px, f5 += (f2 - f) * f4 / f3, ST2870.this.elv2.pz);
                        ST2870.this.player.setTranslate(ST2870.this.player.px, f5, ST2870.this.player.pz);
                        ST2870.this.light07.setTranslate(0.0f, f5 + 1.4f, 7.8f);
                        ST2870.this.light08.setTranslate(0.6f, f5 + 1.4f, 8.8f);
                        ST2870.this.light09.setTranslate(-0.6f, f5 + 1.4f, 8.8f);
                    }
                    if (f >= f2) {
                        f = -1.0f;
                        ST2870.this.lo = 0;
                        if (ST2870.this.ele2_move == 2) {
                            ST2870.this.cam0.changeID(1);
                        }
                        ST2870.this.light17.disp(false);
                        ST2870.this.light17.clearEffect();
                        ST2870.this.light22.disp(false);
                        ST2870.this.light22.clearEffect();
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        Runtime.enable(524288);
                        ST2870.this.ele2_move = 0;
                    }
                    f += 1.0f;
                }
                System.sleep(1);
            }
        }
    }
}

