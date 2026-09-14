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
import xeno.map.MC_VOK10B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0389
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK10B_PRJ {
    Player player;
    Camera cam1;
    Camera camEV;
    Menu menu;
    Window win;
    int entrance;
    int count = 0;
    int selected = 0;
    int selected1 = 0;
    int selected2 = 0;
    int selected3 = 0;
    int selected4 = 0;
    int mapno;
    int enemy;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int beforeA = 0;
    int dummy1talked = 0;
    int dummy2talked = 0;
    int pass1 = 0;
    int button_flg = 0;
    int button1_flg = 0;
    int button2_flg = 0;
    int Aopen = 0;
    int AL01;
    int Okkake = 0;
    int Kemusi_1 = 0;
    int Kemusi_2 = 0;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Unit[] unit;
    Unit crane01;
    Unit monitor1;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono T1;
    Uwamono T2;
    Uwamono T3;
    Uwamono itembox;
    Uwamono itemsymbol;
    Uwamono pole1;
    Uwamono pole2;
    Uwamono Bcar;
    Uwamono Kow1;
    Uwamono Kow2;
    Uwamono obj01;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono Base1;
    Uwamono Base2;
    Uwamono Base3;
    Uwamono Base4;
    Uwamono Base5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Chr dummy1;
    Chr dummy2;
    Effect FadeIn01;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect EF09;
    Effect EF10;
    Effect EF11;
    Effect EF12;
    Effect EF13;
    Effect EF14;
    Effect EF15;
    Effect EF16;
    Effect EF17;
    Effect EF18;
    Effect EF19;
    Effect fade;
    Light light = new Light(0);
    int page;
    String[] Dummy_3 = new String[]{"The airlock is operating in safety mode.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 7, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0389() {
    }

    void EV_Camera_1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.24f, 4.21f, -30.82f);
        this.camEV.setRotate(-39.22f, 302.99f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    this.cam0.setMode(-1);
                    this.EV_Camera_1();
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Dummy_3, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3207, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3207, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                        return;
                    } else if (Runtime.getFlags(3227, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                        return;
                    } else {
                        if (Runtime.getFlags(3287, 1) != 0) return;
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(56);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_03, 0);
                        System.waitFor(this.win);
                        this.doorF.SetDoorType('\u0004');
                        Runtime.setFlags(3287, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 9) {
            switch (n) {
                case 100: {
                    if (this.Kemusi_1 != 0) return;
                    this.Kemusi_1 = 1;
                    System.println("gunoyose_lgunoyose_lgunoyose_lgunoyose_lgunoyose_l");
                    this.enemy2.kickEnepc(7, 17);
                    this.enemy2.kickEnepc(4, 3);
                    this.enemy2.mtn(2, 9, 1.0f, true);
                    this.enemy2.move(45, -36.0f, -25.5f, true);
                    System.sleep(50);
                    this.enemy2.mtn(28, 9, 1.0f, true);
                    System.sleep(5);
                    this.enemy2.getTranslate();
                    this.EF02.setTranslate(this.enemy2.px, this.enemy2.py, this.enemy2.pz);
                    this.EF02.disp(true);
                    this.teiten5 = new Uwamono(28690, -36.0f, 0.0f, -26.22f, 0.0f);
                    this.teiten5.SetBgm(196644);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 10) {
            switch (n) {
                case 100: {
                    if (this.Kemusi_2 != 0) return;
                    this.Kemusi_2 = 1;
                    System.println("gunoyose_rgunoyose_rgunoyose_rgunoyose_rgunoyose_r");
                    this.enemy3.kickEnepc(7, 17);
                    this.enemy3.kickEnepc(4, 3);
                    this.enemy3.mtn(2, 9, 1.0f, true);
                    this.enemy3.move(45, -32.5f, -22.5f, true);
                    System.sleep(50);
                    this.enemy3.mtn(28, 9, 1.0f, true);
                    System.sleep(5);
                    this.enemy3.getTranslate();
                    this.EF03.setTranslate(this.enemy3.px, this.enemy3.py, this.enemy3.pz);
                    this.EF03.disp(true);
                    this.teiten6 = new Uwamono(28690, -32.2f, 0.0f, -23.0f, 0.0f);
                    this.teiten6.SetBgm(196644);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 11 || n2 == 12 || n2 != 13) return;
        switch (n) {
            default:
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("【環境シミュレータ】・VOK13へjump！！");
                Runtime.jumpCF(399, 1);
                break;
            }
            case 1: {
                System.println("【Environmental Simulator】・ Jump to VOK12!!");
                Runtime.jumpCF(419, 1);
                break;
            }
            case 2: {
                System.println("【環境シミュレータ】・VOK12へjump！！");
                Runtime.jumpCF(419, 3);
                break;
            }
            case 3: {
                System.println("【環境シミュレータ】・VOK07へjump！！");
                Runtime.jumpCF(359, 1);
                break;
            }
            case 4: {
                System.println("【環境シミュレータ】・VOK20へjump！！");
                Runtime.jumpCF(209, 1);
                break;
            }
        }
    }

    void init() {
        this.AL01 = Runtime.getFlags(3004, 1);
        this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF02.disp(false);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF03.disp(false);
        this.EF03.setClip(true);
        this.FadeIn01 = new Effect(0);
        this.FadeIn01.args[0] = Integer.MIN_VALUE;
        this.FadeIn01.args[1] = 60;
        this.FadeIn01.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.teiten3 = new Uwamono(28690, -36.0f, 0.0f, -26.22f, 0.0f);
        this.teiten3.SetBgm(196745);
        this.teiten4 = new Uwamono(28690, -32.2f, 0.0f, -23.0f, 0.0f);
        this.teiten4.SetBgm(196745);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 1, 0.82f, 0.49f, 0.12f);
        Runtime.setIdLightCol(1, 2, 0.77f, 0.42f, 0.24f);
        Runtime.setIdLightCol(1, 3, 0.74f, 0.42f, 0.26f);
        Runtime.setIdLightCol(2, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(2, 1, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(2, 2, 0.27f, 0.92f, 0.49f);
        Runtime.setIdLightCol(2, 3, 0.33f, 0.85f, 0.59f);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, -14.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 335.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, -36.8183f, 12.3756f, -13.09755f, 42.35f, -43.4036f, -13.3105f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 3, -3.0f, 0.0f, -26.5f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 3, 3);
        float[] fArray = new float[8];
        fArray[0] = -3.0f;
        fArray[2] = -26.5f;
        fArray[3] = 1.0f;
        fArray[4] = -2.3f;
        fArray[6] = -25.0f;
        fArray[7] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        this.enemy1.kickEnepc(10, 60, 0);
        this.enemy1.setShadow(0, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(16386, 5, -36.722f, 0.0f, -23.789f, 180.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(6, 6, 7, 8);
        float[] fArray3 = new float[8];
        fArray3[0] = -36.722f;
        fArray3[2] = -23.789f;
        fArray3[3] = 1.0f;
        fArray3[4] = -35.428f;
        fArray3[6] = -23.62f;
        fArray3[7] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(2, 15, 2, 5, fArray4);
        float[] fArray5 = new float[12];
        fArray5[0] = -36.722f;
        fArray5[2] = -23.789f;
        fArray5[3] = -35.428f;
        fArray5[5] = -23.62f;
        fArray5[6] = -35.808f;
        fArray5[8] = -22.537f;
        fArray5[9] = -37.031f;
        fArray5[11] = -22.706f;
        float[] fArray6 = fArray5;
        this.enemy2.setShadow(0, 0);
        this.enemy2.setParams(fArray6);
        this.enemy2.kickEnepc(10, 50, 0);
        this.enemy2.enableDTKFlag(262144);
        this.enemy3 = new Enepc();
        this.enemy3.init(16386, 5, -33.951f, 0.0f, -21.581f, 135.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(6, 6, 7, 8);
        float[] fArray7 = new float[8];
        fArray7[0] = -33.951f;
        fArray7[2] = -21.581f;
        fArray7[3] = 1.0f;
        fArray7[4] = -34.458f;
        fArray7[6] = -20.821f;
        fArray7[7] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(2, 15, 3, 5, fArray8);
        float[] fArray9 = new float[12];
        fArray9[0] = -33.951f;
        fArray9[2] = -21.581f;
        fArray9[3] = -34.458f;
        fArray9[5] = -20.821f;
        fArray9[6] = -33.487f;
        fArray9[8] = -20.175f;
        fArray9[9] = -32.911f;
        fArray9[11] = -21.046f;
        float[] fArray10 = fArray9;
        this.enemy3.setShadow(0, 0);
        this.enemy3.setParams(fArray10);
        this.enemy3.kickEnepc(10, 50, 0);
        this.enemy3.enableDTKFlag(262144);
        this.enemy4 = new Enepc();
        this.enemy4.init(16385, 3, -14.0f, 2.0f, 24.5f, 135.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(4, 4, 4, 5);
        float[] fArray11 = new float[]{-14.0f, 2.0f, 24.5f, 1.0f, -11.0f, 2.0f, 24.5f, 2.0f, -11.0f, 2.0f, 18.5f, 3.0f, -17.5f, 2.0f, 18.5f, 4.0f, -17.5f, 2.0f, 24.5f, -1.0f};
        this.enemy4.setParams(1, 19, 4, 3, fArray11);
        float[] fArray12 = new float[]{-14.0f, 2.0f, 24.5f, -11.0f, 2.0f, 24.5f, -11.0f, 2.0f, 18.5f, -17.5f, 2.0f, 18.5f, -17.5f, 2.0f, 24.5f};
        this.enemy4.setParams(fArray12);
        this.enemy4.kickEnepc(10, 75, 0);
        this.enemy4.setShadow(0, 0);
        this.enemy5 = new Enepc();
        this.enemy5.init(16385, 3, -14.0f, 2.0f, -4.0f, 225.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(4, 4, 4, 5);
        float[] fArray13 = new float[24];
        fArray13[0] = -14.0f;
        fArray13[1] = 2.0f;
        fArray13[2] = 4.0f;
        fArray13[3] = 1.0f;
        fArray13[4] = -14.0f;
        fArray13[6] = -16.0f;
        fArray13[7] = 2.0f;
        fArray13[8] = -9.0f;
        fArray13[10] = -16.0f;
        fArray13[11] = 3.0f;
        fArray13[12] = -9.0f;
        fArray13[14] = -25.5f;
        fArray13[15] = 4.0f;
        fArray13[16] = -19.0f;
        fArray13[18] = -25.5f;
        fArray13[19] = 5.0f;
        fArray13[20] = -19.0f;
        fArray13[22] = -16.0f;
        fArray13[23] = -1.0f;
        float[] fArray14 = fArray13;
        this.enemy5.setParams(1, 19, 5, 3, fArray14);
        float[] fArray15 = new float[]{-14.0f, 2.0f, 4.0f, -14.0f, 2.0f, 5.0f, -14.0f, 2.0f, 6.0f, -14.0f, 2.0f, 7.0f, -14.0f, 2.0f, 8.0f, -14.0f, 2.0f, 9.0f, -14.0f, 2.0f, 10.0f, -14.0f, 2.0f, 11.0f, -14.0f, 2.0f, 12.0f, -14.0f, 2.0f, 13.0f};
        this.enemy5.setParams(fArray15);
        this.enemy5.kickEnepc(10, 75, 0);
        this.enemy5.setShadow(0, 0);
        this.enemy7 = new Enepc();
        this.enemy7.init(20228, 7, 7.5f, 2.0f, 22.0f, -90.0f);
        this.enemy7.id = 7;
        this.enemy7.setGroup(9, 9, 9, 9);
        float[] fArray16 = new float[]{7.5f, 2.0f, 22.0f, 1.0f, 5.385f, 2.0f, 21.345f, 2.0f, 3.522f, 2.0f, 21.043f, 3.0f, 1.005f, 2.0f, 20.892f, 4.0f, -1.691028f, 2.0f, 21.787954f, 5.0f, -5.871f, 2.0f, 20.518f, 6.0f, -8.906f, 2.0f, 20.101f, 7.0f, -10.59f, 2.0f, 19.02f, 8.0f, -12.097f, 2.0f, 18.397f, 9.0f, -13.367f, 2.0f, 18.089f, 10.0f, -15.148f, 2.0f, 18.185f, 11.0f, -16.862f, 2.0f, 18.763f, 12.0f, -17.511f, 2.0f, 20.034f, 13.0f, -17.541f, 2.0f, 21.511f, 14.0f, -17.444f, 2.0f, 23.094f, 15.0f, -16.363f, 2.0f, 23.887f, 16.0f, -15.024f, 2.0f, 24.373f, 17.0f, -13.424f, 2.0f, 24.469f, 18.0f, -11.231f, 2.0f, 23.611f, 19.0f, -10.425f, 2.0f, 22.302f, -1.0f};
        this.enemy7.setParams(0, 12, 7, 7, fArray16);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(44, 42, '\u0001');
        new Uwamono(45, 42, '\u0001', this.doorA);
        this.doorA.DoorClose();
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(48, 42, '\u0001');
        new Uwamono(49, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(52, 42, '\u0001');
        new Uwamono(53, 42, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(56, 42, '\u0001');
        new Uwamono(57, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        if (Runtime.getFlags(3287, 1) == 0) {
            this.doorF = new Uwamono(39, 40, '\u0004');
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF = new Uwamono(39, 40, '\u0004');
            this.doorF.SetDoorType('\u0004');
        }
        this.Base1 = new Uwamono(28672, -20.0f, 1.0f, 18.5f, 0.0f);
        this.Base1.SetSize(3.0f, 1.0f, 3.0f);
        this.item1 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 15);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 16);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 17);
        this.item4 = new Uwamono(28672, -14.031f, 0.0f, -8.135f, 0.0f, 18);
        this.item5 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 19);
        new Uwamono(91, 0, this.item1);
        new Uwamono(158, 84, this.item2);
        this.obj01 = new Uwamono(157, 6, this.item3);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        new Uwamono(89, 0);
        new Uwamono(90, 0, this.item4);
        new Uwamono(159, 84);
        this.Kow2 = new Uwamono(97, 0, this.item5);
        this.Kow2.SetSize(1.0f, 1.0f, 3.0f);
        this.Kow2.SetBrokenEnemy(true);
        this.Kow2.SetBroken(false);
        this.Kow1 = new Uwamono(152, 4);
        this.Kow1.SetBrokenEnemy(true);
        Stage.setVisible(156, false);
        this.itembox = new Uwamono(28677, -34.098f, 0.0f, -23.771f, 135.0f, 30);
        this.itembox.SetSymbol(28683);
        this.T1 = new Uwamono(153, 9);
        this.T1.SetBroken(false);
        this.T2 = new Uwamono(154, 9);
        this.T2.SetBroken(false);
        this.T3 = new Uwamono(155, 9);
        this.T3.SetBroken(false);
        this.enemy2.kickEnepc(19, 1, 0, 220, 1);
        this.enemy2.kickEnepc(19, 2, 0, 220, 1);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 4.0f, 3.7f, 19.253f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18011, 0, 256, 130);
        this.monitor1.setArgs(2, 100, 0, 5, -6);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        Stage.setVisible(74, false);
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        this.enemy5.kickEnepc(4, 1);
        this.enemy7.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        this.enemy5.kickEnepc(1, 27);
        this.enemy7.kickEnepc(1, 27);
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        this.enemy5.kickEnepc(4, 0);
        this.enemy7.kickEnepc(4, 0);
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

    class Object
            extends Unit {
        Object() {
        }
    }
}

