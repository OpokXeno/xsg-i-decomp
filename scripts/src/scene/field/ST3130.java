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
import xeno.map.MC_GNK19_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3130
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK19_PRJ {
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
    Unit Dummy;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Light light = new Light(0);
    Effect E01;
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
    Uwamono itembox;
    Uwamono KOW01;
    Uwamono KOW02;
    Uwamono KOW03;
    Uwamono KOW04;
    Uwamono KOW05;
    Uwamono KOW06;
    Uwamono KOW07;
    Uwamono KOW08;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] SYS_00 = new String[]{"Would you like to jump down?", "/[waitkey(64)]/[close()]"};

    ST3130() {
    }

    void EOB(int n) {
        if (n == 8) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            this.cam0.setMode(0);
            this.cam0.setFog(0, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(1, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(2, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(3, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(4, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(5, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(6, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(7, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            this.cam0.setFog(8, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
            Runtime.setFlags(3226, 1, 1);
            Runtime.setFlags(3150, 1, 1);
            Sound.effectPlay(6);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            Runtime.addItemWin(10, 21);
        }
    }

    void EV_Camera00() {
        this.cam0.setTranslate(4.176f, 5.367f, 40.905f);
        this.cam0.setRotate(-23.972f, 21.099f, 0.0f);
        this.cam0.setFov(40.0f);
    }

    void EV_Camera01() {
        this.cam0.setTranslate(2.021f, -17.415f, 42.165f);
        this.cam0.setRotate(7.966f, 16.479f, 0.0f);
        this.cam0.setFov(40.0f);
    }

    void EV_Camera02() {
        this.cam0.setTranslate(8.037f, -13.386f, 17.786f);
        this.cam0.setRotate(-8.015f, 157.057f, 0.0f);
        this.cam0.setFov(40.0f);
        this.cam0.setFog(0, 35.0f, 36.0f, 0.2f, 1.0f, 40, 40, 40, 0);
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            block0:
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3150, 1) == 0) {
                        this.player.getTranslate();
                        if (this.player.py <= -5.0f) {
                            return;
                        }
                        System.println("1");
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SYS_00, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                System.sleep(2);
                                Sound.streamPlay(1195005, 48000);
                                float f = 0.0f;
                                float f2 = 0.0f;
                                this.player.getTranslate();
                                float f3 = (-1.372f - this.player.px) / 60.0f;
                                float f4 = (31.921f - this.player.pz) / 60.0f;
                                Runtime.enable(65536);
                                this.enemy6.kickEnepc(4, 1);
                                this.enemy7.kickEnepc(4, 1);
                                this.enemy8.kickEnepc(4, 1);
                                this.enemy6.kickEnepc(3, 0, 1, 1, 1, 100);
                                this.enemy7.kickEnepc(3, 0, 1, 1, 1, 100);
                                this.enemy8.kickEnepc(3, 0, 1, 1, 1, 100);
                                this.player.mtn(23, 1, 1.0f, true);
                                System.sleep(20);
                                this.cam0.setMode(-1);
                                this.EV_Camera00();
                                while (true) {
                                    this.player.getTranslate();
                                    f2 = (-9.8f * f / 60.0f + 5.0f) * f / 60.0f + 1.0f;
                                    if (f < 60.0f) {
                                        this.player.setTranslate(this.player.px + f3, f2, this.player.pz + f4);
                                    } else if (f2 >= -17.8f) {
                                        this.player.setTranslate(this.player.px, f2, this.player.pz);
                                    } else {
                                        this.player.setTranslate(this.player.px, -17.8f, this.player.pz);
                                    }
                                    if (f == 85.0f) {
                                        this.player.mtn(24, 1, 1.0f, true);
                                    }
                                    if (f == 90.0f) {
                                        this.EV_Camera01();
                                    }
                                    if (f == 120.0f) break;
                                    f += 1.0f;
                                    System.sleep(1);
                                }
                                this.player.mtn(1, 9, 1.0f, true);
                                this.EV_Camera02();
                                this.enemy6.kickEnepc(4, 1);
                                this.enemy7.kickEnepc(4, 1);
                                this.enemy8.kickEnepc(4, 1);
                                this.enemy6.setTranslate(0.0f, 0.0f, 0.0f);
                                this.enemy7.setTranslate(0.0f, 0.0f, 0.0f);
                                this.enemy8.setTranslate(0.0f, 0.0f, 0.0f);
                                this.enemy6.kickEnepc(0, 0);
                                System.sleep(75);
                                this.enemy6.kickEnepc(1, 1);
                                this.enemy7.kickEnepc(0, 0);
                                System.sleep(75);
                                this.enemy7.kickEnepc(1, 1);
                                System.sleep(60);
                                this.enemy8.kickEnepc(0, 0);
                                System.sleep(120);
                                this.enemy6.kickEnepc(4, 0);
                                this.enemy7.kickEnepc(4, 0);
                                this.enemy8.kickEnepc(14, 0);
                                break block0;
                            }
                        }
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        System.sleep(1);
                        this.player.move(60, -1.5f, 27.5f, true);
                        System.sleep(60);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    this.player.getTranslate();
                    if (this.player.py <= -5.0f) {
                        return;
                    }
                    System.println("1");
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            float f = 0.0f;
                            float f5 = 0.0f;
                            this.player.getTranslate();
                            float f6 = (-1.372f - this.player.px) / 60.0f;
                            float f7 = (31.921f - this.player.pz) / 60.0f;
                            Runtime.enable(65536);
                            this.player.mtn(23, 1, 1.0f, true);
                            System.sleep(20);
                            this.cam0.setMode(-1);
                            this.EV_Camera00();
                            while (true) {
                                this.player.getTranslate();
                                f5 = (-9.8f * f / 60.0f + 5.0f) * f / 60.0f + 1.0f;
                                if (f < 60.0f) {
                                    this.player.setTranslate(this.player.px + f6, f5, this.player.pz + f7);
                                } else if (f5 >= -17.8f) {
                                    this.player.setTranslate(this.player.px, f5, this.player.pz);
                                } else {
                                    this.player.setTranslate(this.player.px, -17.8f, this.player.pz);
                                }
                                if (f == 85.0f) {
                                    this.player.mtn(24, 1, 1.0f, true);
                                }
                                if (f == 90.0f) {
                                    this.EV_Camera01();
                                }
                                if (f == 120.0f) {
                                    Runtime.disable(65536);
                                    Runtime.setPlayerControl(true);
                                    this.cam0.setMode(0);
                                    break block0;
                                }
                                f += 1.0f;
                                System.sleep(1);
                            }
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    System.sleep(1);
                    this.player.move(60, -1.5f, 27.5f, true);
                    System.sleep(60);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
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
                Runtime.jumpCF(68676, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68656, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -1.25f, 0.0f, 28.0f, 0.0f);
        this.teiten1.SetBgm(196620);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1652, -1.453f, -12.329f, 26.606f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
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
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setFog(0, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(1, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(8, 20.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 30.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, -10.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 16.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, -11.5f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.5f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.enemy1 = new Enepc();
        this.enemy1.init(17415, 3, 32.0f, 0.0f, -0.25f, 270.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(1, 1, 1, 1);
        float[] fArray = new float[48];
        fArray[0] = 32.0f;
        fArray[2] = -0.25f;
        fArray[3] = 1.0f;
        fArray[4] = 31.0f;
        fArray[6] = -0.25f;
        fArray[7] = 2.0f;
        fArray[8] = 30.0f;
        fArray[10] = -0.25f;
        fArray[11] = 3.0f;
        fArray[12] = 29.0f;
        fArray[14] = -0.25f;
        fArray[15] = 4.0f;
        fArray[16] = 28.0f;
        fArray[18] = -0.25f;
        fArray[19] = 5.0f;
        fArray[20] = 27.0f;
        fArray[22] = -0.25f;
        fArray[23] = 6.0f;
        fArray[24] = 26.0f;
        fArray[26] = -0.25f;
        fArray[27] = 7.0f;
        fArray[28] = 25.0f;
        fArray[30] = -0.25f;
        fArray[31] = 8.0f;
        fArray[32] = 24.0f;
        fArray[34] = -0.25f;
        fArray[35] = 9.0f;
        fArray[36] = 23.0f;
        fArray[38] = -0.25f;
        fArray[39] = 10.0f;
        fArray[40] = 22.0f;
        fArray[42] = -0.25f;
        fArray[43] = 11.0f;
        fArray[44] = 21.0f;
        fArray[46] = -0.25f;
        fArray[47] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[36];
        fArray3[0] = 32.0f;
        fArray3[2] = -0.25f;
        fArray3[3] = 31.0f;
        fArray3[5] = -0.25f;
        fArray3[6] = 30.0f;
        fArray3[8] = -0.25f;
        fArray3[9] = 29.0f;
        fArray3[11] = -0.25f;
        fArray3[12] = 28.0f;
        fArray3[14] = -0.25f;
        fArray3[15] = 27.0f;
        fArray3[17] = -0.25f;
        fArray3[18] = 26.0f;
        fArray3[20] = -0.25f;
        fArray3[21] = 25.0f;
        fArray3[23] = -0.25f;
        fArray3[24] = 24.0f;
        fArray3[26] = -0.25f;
        fArray3[27] = 23.0f;
        fArray3[29] = -0.25f;
        fArray3[30] = 22.0f;
        fArray3[32] = -0.25f;
        fArray3[33] = 21.0f;
        fArray3[35] = -0.25f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 70, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(17414, 3, 2.0f, 0.0f, -0.25f, 90.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(2, 2, 2, 2);
        float[] fArray5 = new float[64];
        fArray5[0] = 2.0f;
        fArray5[2] = -0.25f;
        fArray5[3] = -1.0f;
        fArray5[4] = 3.0f;
        fArray5[6] = -0.25f;
        fArray5[8] = 4.0f;
        fArray5[10] = -0.25f;
        fArray5[11] = 1.0f;
        fArray5[12] = 5.0f;
        fArray5[14] = -0.25f;
        fArray5[15] = 2.0f;
        fArray5[16] = 6.0f;
        fArray5[18] = -0.25f;
        fArray5[19] = 3.0f;
        fArray5[20] = 7.0f;
        fArray5[22] = -0.25f;
        fArray5[23] = 4.0f;
        fArray5[24] = 8.0f;
        fArray5[26] = -0.25f;
        fArray5[27] = 5.0f;
        fArray5[28] = 9.0f;
        fArray5[30] = -0.25f;
        fArray5[31] = 6.0f;
        fArray5[32] = 10.0f;
        fArray5[34] = -0.25f;
        fArray5[35] = 7.0f;
        fArray5[36] = 11.0f;
        fArray5[38] = -0.25f;
        fArray5[39] = 8.0f;
        fArray5[40] = 12.0f;
        fArray5[42] = -0.25f;
        fArray5[43] = 9.0f;
        fArray5[44] = 13.0f;
        fArray5[46] = -0.25f;
        fArray5[47] = 10.0f;
        fArray5[48] = 14.0f;
        fArray5[50] = -0.25f;
        fArray5[51] = 11.0f;
        fArray5[52] = 15.0f;
        fArray5[54] = -0.25f;
        fArray5[55] = 12.0f;
        fArray5[56] = 16.0f;
        fArray5[58] = -0.25f;
        fArray5[59] = 13.0f;
        fArray5[60] = 17.0f;
        fArray5[62] = -0.25f;
        fArray5[63] = 14.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 3, fArray6);
        float[] fArray7 = new float[48];
        fArray7[0] = 2.0f;
        fArray7[2] = -0.25f;
        fArray7[3] = 3.0f;
        fArray7[5] = -0.25f;
        fArray7[6] = 4.0f;
        fArray7[8] = -0.25f;
        fArray7[9] = 5.0f;
        fArray7[11] = -0.25f;
        fArray7[12] = 6.0f;
        fArray7[14] = -0.25f;
        fArray7[15] = 7.0f;
        fArray7[17] = -0.25f;
        fArray7[18] = 8.0f;
        fArray7[20] = -0.25f;
        fArray7[21] = 9.0f;
        fArray7[23] = -0.25f;
        fArray7[24] = 10.0f;
        fArray7[26] = -0.25f;
        fArray7[27] = 11.0f;
        fArray7[29] = -0.25f;
        fArray7[30] = 12.0f;
        fArray7[32] = -0.25f;
        fArray7[33] = 13.0f;
        fArray7[35] = -0.25f;
        fArray7[36] = 14.0f;
        fArray7[38] = -0.25f;
        fArray7[39] = 15.0f;
        fArray7[41] = -0.25f;
        fArray7[42] = 16.0f;
        fArray7[44] = -0.25f;
        fArray7[45] = 17.0f;
        fArray7[47] = -0.25f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.kickEnepc(10, 70, 0);
        this.enemy5 = new Enepc();
        this.enemy5.init(17414, 13, -31.0f, 0.0f, -0.25f, 180.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(3, 3, 3, 3);
        float[] fArray9 = new float[56];
        fArray9[0] = -31.0f;
        fArray9[2] = -0.25f;
        fArray9[3] = 1.0f;
        fArray9[4] = -30.0f;
        fArray9[6] = -0.25f;
        fArray9[7] = 2.0f;
        fArray9[8] = -29.0f;
        fArray9[10] = -0.25f;
        fArray9[11] = 3.0f;
        fArray9[12] = -28.0f;
        fArray9[14] = -0.25f;
        fArray9[15] = 4.0f;
        fArray9[16] = -27.0f;
        fArray9[18] = -0.25f;
        fArray9[19] = 5.0f;
        fArray9[20] = -26.0f;
        fArray9[22] = -0.25f;
        fArray9[23] = 6.0f;
        fArray9[24] = -25.0f;
        fArray9[26] = -0.25f;
        fArray9[27] = 7.0f;
        fArray9[28] = -24.0f;
        fArray9[30] = -0.25f;
        fArray9[31] = 8.0f;
        fArray9[32] = -23.0f;
        fArray9[34] = -0.25f;
        fArray9[35] = 9.0f;
        fArray9[36] = -22.0f;
        fArray9[38] = -0.25f;
        fArray9[39] = 10.0f;
        fArray9[40] = -21.0f;
        fArray9[42] = -0.25f;
        fArray9[43] = 11.0f;
        fArray9[44] = -20.0f;
        fArray9[46] = -0.25f;
        fArray9[47] = 12.0f;
        fArray9[48] = -19.0f;
        fArray9[50] = -0.25f;
        fArray9[51] = 13.0f;
        fArray9[52] = -18.0f;
        fArray9[54] = -0.25f;
        fArray9[55] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy5.setParams(1, 1, 5, 13, fArray10);
        float[] fArray11 = new float[42];
        fArray11[0] = -31.0f;
        fArray11[2] = -0.25f;
        fArray11[3] = -30.0f;
        fArray11[5] = -0.25f;
        fArray11[6] = -29.0f;
        fArray11[8] = -0.25f;
        fArray11[9] = -28.0f;
        fArray11[11] = -0.25f;
        fArray11[12] = -27.0f;
        fArray11[14] = -0.25f;
        fArray11[15] = -26.0f;
        fArray11[17] = -0.25f;
        fArray11[18] = -25.0f;
        fArray11[20] = -0.25f;
        fArray11[21] = -24.0f;
        fArray11[23] = -0.25f;
        fArray11[24] = -23.0f;
        fArray11[26] = -0.25f;
        fArray11[27] = -22.0f;
        fArray11[29] = -0.25f;
        fArray11[30] = -21.0f;
        fArray11[32] = -0.25f;
        fArray11[33] = -20.0f;
        fArray11[35] = -0.25f;
        fArray11[36] = -19.0f;
        fArray11[38] = -0.25f;
        fArray11[39] = -18.0f;
        fArray11[41] = -0.25f;
        float[] fArray12 = fArray11;
        this.enemy5.setParams(fArray12);
        this.enemy5.kickEnepc(10, 70, 0);
        if (Runtime.getFlags(3150, 1) == 0) {
            this.enemy6 = new Enepc();
            this.enemy6.init(16652, 7, 100.0f, -100.0f, 100.0f, 0.0f);
            this.enemy6.id = 6;
            this.enemy6.setTogetherWith(7, 7, 8, 8);
            this.enemy6.setParams(0, 3, 6, 7);
            this.enemy6.dispRadar(false);
            this.enemy6.setMotion(0, 6);
            this.enemy6.setInvalidID(1);
            this.enemy7 = new Enepc();
            this.enemy7.init(16652, 8, 100.0f, -100.0f, 100.0f, 0.0f);
            this.enemy7.id = 7;
            this.enemy7.setTogetherWith(6, 6, 8, 8);
            this.enemy7.setParams(0, 3, 7, 8);
            this.enemy7.dispRadar(false);
            this.enemy7.setMotion(0, 6);
            this.enemy7.setInvalidID(1);
            this.enemy8 = new Enepc();
            this.enemy8.init(20242, 5, 100.0f, -100.0f, 100.0f, 0.0f);
            this.enemy8.id = 8;
            this.enemy8.setTogetherWith(6, 6, 7, 7);
            this.enemy8.setGroup(0, 0, 0, 0);
            this.enemy8.setBatEvent(8);
            this.enemy8.setParams(0, 3, 8, 5);
            this.enemy8.dispRadar(false);
            this.enemy8.setMotion(0, 6);
            this.enemy8.setInvalidID(1);
        }
        this.enemy1.kickEnepc(19, 1, 0, 1340, 1);
        this.itembox = new Uwamono(28677, 1.727f, 0.0f, -6.325f, 180.0f, 384);
        this.itembox.SetSymbol(28684);
        this.KOW01 = new Uwamono(5, 10);
        this.KOW02 = new Uwamono(6, 10);
        this.KOW03 = new Uwamono(7, 10);
        this.KOW04 = new Uwamono(8, 10, this.itembox);
        this.KOW05 = new Uwamono(9, 10);
        this.KOW06 = new Uwamono(10, 10);
        this.KOW07 = new Uwamono(11, 10);
        this.KOW08 = new Uwamono(12, 10);
        this.KOW01.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW02.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW03.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW04.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW05.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW06.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW07.SetSize(2.5f, 2.0f, 2.5f);
        this.KOW08.SetSize(2.5f, 2.0f, 2.5f);
        new Uwamono(13, 2);
        new Uwamono(14, 2);
        this.doorA = new Uwamono(54, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(0, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(1, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
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

        void Freeze() {
            ST3130.this.enemy6.kickEnepc(4, 2);
            ST3130.this.enemy7.kickEnepc(4, 2);
            ST3130.this.enemy8.kickEnepc(4, 2);
        }
    }
}

