import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK12B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0419
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK12B_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
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
    int npc9talked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono Glass;
    Uwamono kow1;
    int passed1 = 0;
    int passed2 = 0;
    int passed3 = 0;
    int passed4 = 0;
    int button_flg = 0;
    int button2_flg = 0;
    int button3_flg = 0;
    Uwamono itembox;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono Base1;
    Uwamono SAVE;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono lc;
    Uwamono Evs;
    int Loc_flg = 0;
    int test = 0;
    int guno4pass = 0;
    int stop = 0;
    Unit monitor1;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    int smd = 0;
    int finalflg;
    int b_flg;
    int page;
    String[] Loc_6_3 = new String[]{"The ship monitor controls are broken.", "/[waitkey(64)]/[close()]"};
    String[] Sion_13 = new String[]{"No response.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 16, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};

    ST0419() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 23.956f, 3.731f, -6.846f, 90.0f, 23.956f, 2.675f, -6.846f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -15.841f;
        fArray2[2] = 33.979f;
        fArray2[4] = 90.0f;
        fArray2[5] = -15.841f;
        fArray2[6] = 33.979f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorD.DoorClose();
        System.sleep(n);
    }

    void Kakuheki_Open() {
        int n = 10;
        this.doorD.DoorOpen();
        System.sleep(n);
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0 || n2 == 1 || n2 == 2 || n2 == 3) return;
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF03.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                    this.player.setRotate(0.0f, 180.0f, 0.0f);
                    this.player.mtn(12, 1, 1.0f, true);
                    System.sleep(50);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    this.EF03.disp(true);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.npc1talked = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    if (this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF02.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                    this.player.setRotate(0.0f, 180.0f, 0.0f);
                    this.player.mtn(12, 1, 1.0f, true);
                    System.sleep(50);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    this.EF02.disp(true);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.npc1talked = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 6) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Loc_6_3, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 7) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3216, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3216, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    } else if (Runtime.getFlags(3236, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    } else {
                        if (Runtime.getFlags(3296, 1) != 0) return;
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(56);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_03, 0);
                        System.waitFor(this.win);
                        this.doorF.SetDoorType('\u0004');
                        Runtime.setFlags(3296, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 16) return;
        switch (n) {
            case 100: {
                this.off();
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Sion_13, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                this.on();
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
                System.println("【環境シミュレータ】・VOK10へjump！！");
                Runtime.jumpCF(389, 2);
                break;
            }
            case 2: {
                System.println("【環境シミュレータ】・VOK10へjump！！");
                Runtime.jumpCF(389, 3);
                break;
            }
            case 3: {
                System.println("【環境シミュレータ】・VOK04へjump！！");
                Runtime.jumpCF(329, 1);
                break;
            }
            case 4: {
                System.println("【環境シミュレータ】・VOK19へjump！！");
                Runtime.jumpCF(199, 1);
                break;
            }
        }
    }

    void evsExit() {
        System.println("evsExitをコールしました");
        if (this.b_flg == 1) {
            return;
        }
        this.b_flg = 1;
        Runtime.enable(262144);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.EVS, 0);
        ST0419.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                Runtime.evsExit();
                return;
            }
        }
        Runtime.setPlayerControl(true);
        Runtime.disable(262144);
        this.b_flg = 0;
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 24.0f, 0.0f, 22.0f, 0.0f);
        this.teiten1.SetBgm(196634);
        this.teiten2 = new Uwamono(28690, 22.0f, 0.0f, -20.0f, 0.0f);
        this.teiten2.SetBgm(196634);
        this.teiten3 = new Uwamono(28690, 30.0f, 0.0f, 1.0f, 0.0f);
        this.teiten3.SetBgm(196634);
        this.teiten4 = new Uwamono(28690, 25.0f, 0.0f, 1.0f, 0.0f);
        this.teiten4.SetBgm(196635);
        this.teiten5 = new Uwamono(28690, 20.5f, 0.0f, 13.0f, 0.0f);
        this.teiten5.SetBgm(196635);
        this.teiten6 = new Uwamono(28690, 20.5f, 0.0f, -9.0f, 0.0f);
        this.teiten6.SetBgm(196635);
        this.teiten7 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten7.SetBgm(196635);
        this.teiten8 = new Uwamono(28690, -14.0f, 0.0f, 16.0f, 0.0f);
        this.teiten8.SetBgm(196642);
        this.EF02 = new Effect(1011, 1);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1011, 2);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        Stage.setVisible(-1, true);
        Stage.renderCommand(4);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(23, false);
        Stage.setVisible(24, false);
        Stage.setVisible(25, false);
        Stage.setVisible(26, false);
        Stage.setVisible(27, false);
        Stage.setVisible(28, false);
        Stage.setVisible(31, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 21.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 3.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, 10.724f, 7.143f, -21.846f, 62.91f, -69.846f, -9.719f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, 27.6133f, 5.6059f, 4.9391f, 62.358f, -60.2295f, 0.2437f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFPedestal(9, -6.92025f, 4.6719f, 28.7121f, 40.0f, -20.5988f, -30.9395f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, 9.149f, 2.5f, -20.874f, 40.0f, -11.187f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFPedestal(13, 13.524f, 8.32379f, 15.4844f, 48.0f, -88.00234f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(13, 1);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 6, -1.0f, 0.0f, 21.0f, 45.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(3, 3, 3, 6);
        float[] fArray = new float[32];
        fArray[0] = -1.0f;
        fArray[2] = 21.0f;
        fArray[3] = 1.0f;
        fArray[4] = -11.0f;
        fArray[6] = 21.0f;
        fArray[7] = 2.0f;
        fArray[8] = -11.0f;
        fArray[10] = 19.0f;
        fArray[11] = 3.0f;
        fArray[12] = -17.0f;
        fArray[14] = 21.0f;
        fArray[15] = 4.0f;
        fArray[16] = -17.0f;
        fArray[18] = 26.0f;
        fArray[19] = 5.0f;
        fArray[20] = -11.0f;
        fArray[22] = 26.0f;
        fArray[23] = 6.0f;
        fArray[24] = -11.0f;
        fArray[26] = 21.0f;
        fArray[27] = 7.0f;
        fArray[28] = -1.0f;
        fArray[30] = 21.0f;
        fArray[31] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(3, 20, 1, 6, fArray2);
        float[] fArray3 = new float[24];
        fArray3[0] = -1.0f;
        fArray3[2] = 21.0f;
        fArray3[3] = -11.0f;
        fArray3[5] = 21.0f;
        fArray3[6] = -11.0f;
        fArray3[8] = 19.0f;
        fArray3[9] = -17.0f;
        fArray3[11] = 21.0f;
        fArray3[12] = -17.0f;
        fArray3[14] = 26.0f;
        fArray3[15] = -11.0f;
        fArray3[17] = 26.0f;
        fArray3[18] = -11.0f;
        fArray3[20] = 21.0f;
        fArray3[21] = -1.0f;
        fArray3[23] = 21.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.setShadow(0, 0);
        this.enemy1.renderCommand(22);
        this.enemy2 = new Enepc();
        this.enemy2.init(16385, 6, 21.0f, 0.0f, 21.0f, 45.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(3, 3, 3, 6);
        float[] fArray5 = new float[64];
        fArray5[0] = 21.0f;
        fArray5[2] = 21.0f;
        fArray5[3] = 1.0f;
        fArray5[4] = 20.0f;
        fArray5[6] = 21.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = 19.0f;
        fArray5[10] = 21.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = 18.0f;
        fArray5[14] = 21.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = 17.0f;
        fArray5[18] = 21.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = 16.0f;
        fArray5[22] = 21.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = 15.0f;
        fArray5[26] = 21.0f;
        fArray5[27] = 7.0f;
        fArray5[28] = 14.0f;
        fArray5[30] = 21.0f;
        fArray5[31] = 8.0f;
        fArray5[32] = 13.0f;
        fArray5[34] = 21.0f;
        fArray5[35] = 9.0f;
        fArray5[36] = 12.0f;
        fArray5[38] = 21.0f;
        fArray5[39] = 10.0f;
        fArray5[40] = 11.0f;
        fArray5[42] = 21.0f;
        fArray5[43] = 11.0f;
        fArray5[44] = 10.0f;
        fArray5[46] = 21.0f;
        fArray5[47] = 12.0f;
        fArray5[48] = 9.0f;
        fArray5[50] = 21.0f;
        fArray5[51] = 13.0f;
        fArray5[52] = 8.0f;
        fArray5[54] = 21.0f;
        fArray5[55] = 14.0f;
        fArray5[56] = 7.0f;
        fArray5[58] = 21.0f;
        fArray5[59] = 15.0f;
        fArray5[60] = 6.0f;
        fArray5[62] = 16.0f;
        fArray5[63] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(3, 20, 2, 6, fArray6);
        float[] fArray7 = new float[6];
        fArray7[0] = 21.0f;
        fArray7[2] = 21.0f;
        fArray7[3] = 21.0f;
        fArray7[5] = 16.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.enableDTKFlag(262144);
        this.enemy2.renderCommand(22);
        this.enemy3 = new Enepc();
        this.enemy3.init(20228, 10, 21.0f, 0.0f, 0.0f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(5, 5, 7, 7);
        float[] fArray9 = new float[8];
        fArray9[0] = 21.0f;
        fArray9[3] = 1.0f;
        fArray9[4] = 20.0f;
        fArray9[7] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setShadow(0, 0);
        this.enemy3.setParams(0, 2, 3, 10, fArray10);
        this.enemy3.renderCommand(22);
        this.enemy4 = new Enepc();
        this.enemy4.init(16385, 6, 20.5f, 0.0f, -12.5f, 192.5f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(3, 3, 3, 6);
        float[] fArray11 = new float[16];
        fArray11[0] = 20.5f;
        fArray11[2] = -12.5f;
        fArray11[3] = 1.0f;
        fArray11[4] = 20.888f;
        fArray11[6] = -17.132f;
        fArray11[7] = 2.0f;
        fArray11[8] = 3.043f;
        fArray11[10] = -17.509f;
        fArray11[11] = 3.0f;
        fArray11[12] = 2.783f;
        fArray11[14] = -26.988f;
        fArray11[15] = -1.0f;
        float[] fArray12 = fArray11;
        this.enemy4.setShadow(0, 0);
        this.enemy4.setParams(0, 8, 4, 6, fArray12);
        this.enemy4.enableDTKFlag(262144);
        this.enemy4.renderCommand(22);
        this.enemy5 = new Enepc();
        this.enemy5.init(16386, 8, 12.48f, 0.0f, -29.144f, 0.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(4, 4, 4, 4);
        this.enemy5.setShadow(0, 0);
        this.enemy5.setParams(1, 4, 5, 8);
        this.enemy5.kickEnepc(10, 50, 0);
        this.enemy5.renderCommand(22);
        this.npc5 = new NPC_NORMAL(1592, 15, 0, 26, 13, 19.0f, 1.8f, 0.7f, 35.0f);
        this.npc5.kickEnepc(10, 70, 0);
        this.npc5.setTP(500);
        this.npc5.setInvalidID(1);
        this.npc5.setMotion(0, 10);
        this.npc5.setShadow(0, 0);
        this.npc5.dispRadar(false);
        this.npc5.renderCommand(22);
        this.npc6 = new NPC_NORMAL(1595, 16, 0, 26, 13, 19.0f, 1.8f, 1.3f, 110.0f);
        this.npc6.kickEnepc(10, 70, 0);
        this.npc6.setTP(500);
        this.npc6.setInvalidID(1);
        this.npc6.setMotion(0, 9);
        this.npc6.setShadow(0, 0);
        this.npc6.dispRadar(false);
        this.npc6.renderCommand(22);
        this.doorA = new Uwamono(13, 42, '\u0001');
        new Uwamono(14, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(17, 42, '\u0001');
        new Uwamono(18, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(19, 42, '\u0001');
        new Uwamono(20, 42, '\u0002', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(119, 42, '\u0001');
        new Uwamono(118, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0002');
        this.doorD.SetDoorRange(2.5f);
        this.doorD.DoorOpen();
        this.doorE = new Uwamono(165, 40, '\u0001');
        new Uwamono(164, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0002');
        if (Runtime.getFlags(3296, 1) == 0) {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0004');
        }
        this.doorG = new Uwamono(0, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.Base1 = new Uwamono(28672, 27.5f, -1.0f, 0.0f, 0.0f);
        this.Base1.SetSize(8.0f, 1.0f, 10.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 20);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 21);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 22);
        new Uwamono(170, 84);
        this.lc = new Uwamono(168, 84, this.item1);
        this.lc.SetSize(0.75f, 1.0f, 0.755f);
        new Uwamono(169, 84);
        new Uwamono(129, 31);
        new Uwamono(130, 21);
        this.Glass = new Uwamono(162, 126);
        this.Glass.SetParticle(1761);
        new Uwamono(166, 21, this.item3);
        this.kow1 = new Uwamono(167, 6, this.item2);
        this.kow1.SetBrokenEnemy(true);
        this.kow1.SetSize(2.5f, 0.5f, 2.5f);
        this.itembox = new Uwamono(28677, 10.0f, 0.0f, -31.0f, 180.0f, 31);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        this.trap3 = new Uwamono(28673, -17.5f, 0.0f, 18.0f, 0.0f);
        this.Evs = new Uwamono(28734, 14.5f, 0.0f, -24.0f);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 18.1f, 2.0f, 1.0f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 2.38f);
        this.monitor1.setArgs(1, 18001, 0, 256, 128);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setScale(1.2f, 0.81f, 1.0f);
        this.monitor1.signal(1);
        Stage.setVisible(127, false);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3238, 1, 1);
                break;
            }
        }
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        if (Runtime.getFlags(3001, 2) == 2) {
            this.enemy5.kickEnepc(4, 1);
        }
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        if (Runtime.getFlags(3001, 2) == 2) {
            this.enemy5.kickEnepc(1, 27);
        }
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        if (Runtime.getFlags(3001, 2) == 2) {
            this.enemy5.kickEnepc(4, 0);
        }
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
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

    class Object
            extends Unit {
        Object() {
        }
    }
}

