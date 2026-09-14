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
import xeno.map.MC_ELS04B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0680
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS04B_PRJ {
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
    int shion = 0;
    int button1_flg = 0;
    int button2_flg = 0;
    int epass = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono item06;
    Uwamono item07;
    Uwamono item08;
    Uwamono item09;
    Uwamono item10;
    Uwamono Base01;
    Uwamono Base02;
    Uwamono Base03;
    Uwamono Base04;
    int button_flg = 0;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect fade;
    Effect Yu01;
    Effect Yu02;
    Effect Yu03;
    Effect Yu04;
    Effect Yu05;
    Effect Yu06;
    Effect Yu07;
    Effect Yu08;
    Effect Yu09;
    Effect Yu10;
    Effect Yu11;
    Effect Yu12;
    Light light = new Light(0);
    Unit monitor1;
    Uwamono tA;
    MAPUnit doorR1;
    MAPUnit doorL1;
    MAPUnit doorR2;
    MAPUnit doorL2;
    Unit ele;
    int elemove = 0;
    int elec = 0;
    int doormove = 0;
    boolean call = false;
    boolean syokika = true;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] DOOR = new String[]{"Press the bulkhead switch?\n", "/[waitkey(64)]/[close()]"};
    String[] Elv_1 = new String[]{"Would you like to go to 1F?", "/[waitkey(64)]/[close()]"};
    String[] Elv_2 = new String[]{"Going to 1F.", "/[waitkey(64)]/[close()]"};
    String[] MESS_01 = new String[]{"The anti-intruder program has been activated. Crew is prohibited from entering affected sectors.", "/[waitkey(64)]/[close()]"};
    String[] MESS_02 = new String[]{"/[label(Warning)]", "Use of cargo elevator is only authorized for the transportation of cargo.", "/[waitkey(64)]/[close()]"};

    ST0680() {
    }

    void EOB(int n) {
        System.println("EOB************************************************");
        switch (n) {
            case 1: {
                System.println("e1_dead****************************************");
                Runtime.setFlags(3040, 1, 1);
                if (Runtime.getFlags(3025, 1) != 1) break;
                this.EF01.disp(false);
                break;
            }
            case 2: {
                System.println("e2_dead****************************************");
                Runtime.setFlags(3041, 1, 1);
                if (Runtime.getFlags(3025, 1) != 1) break;
                this.EF02.disp(false);
                break;
            }
            case 3: {
                System.println("e3_dead****************************************");
                Runtime.setFlags(3042, 1, 1);
                if (Runtime.getFlags(3025, 1) != 1) break;
                this.EF03.disp(false);
                break;
            }
            case 4: {
                System.println("e4_dead****************************************");
                Runtime.setFlags(3043, 1, 1);
                if (Runtime.getFlags(3025, 1) != 1) break;
                this.EF04.disp(false);
                break;
            }
        }
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.6f, 2.223f, 3.876f);
        this.camEV.setRotate(-7.777f, 48.739f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.347498f, 2.0599706f, -11.649283f);
        this.camEV.setRotate(-13.913661f, 194.94048f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.256143f, 1.8999887f, 13.655906f);
        this.camEV.setRotate(-8.917236f, 1.6799955f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.231916f, 5.359226f, 18.12838f);
        this.camEV.setRotate(-38.13715f, 0.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.807f, 2.7f, 2.506f);
        this.camEV.setRotate(-13.857f, 62.319f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        System.println("bottom***********************");
        switch (n) {
            case 0: {
                System.println("bottom");
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(66226, 2);
                break;
            }
        }
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorD.DoorClose();
        System.sleep(n);
        this.doorC.DoorClose();
        System.sleep(n);
        this.doorB.DoorClose();
        System.sleep(n);
        this.doorA.DoorClose();
    }

    void Kakuheki_Open() {
        int n = 10;
        this.doorA.DoorOpen();
        System.sleep(n);
        this.doorB.DoorOpen();
        System.sleep(n);
        this.doorC.DoorOpen();
        System.sleep(n);
        this.doorD.DoorOpen();
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3021, 1) != 1) return;
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.MESS_01, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    this.doormove = 1;
                    System.println("エレベータ");
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2 || n2 == 3 || n2 == 4) return;
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    if (this.epass == 0) {
                        System.println("**********************************");
                        Runtime.setPlayerControl(false);
                        this.cam0.setMode(-1);
                        this.EV_Camera04();
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(30, -12.0f, 0.0f, true);
                        System.sleep(30);
                        this.player.rotY(7, 90.0f, true);
                        System.sleep(7);
                        Runtime.disable(65536);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Elv_1, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.disable(65536);
                                this.doormove = 2;
                                Sound.effectPlay(196713);
                                Runtime.setFlags(3038, 1, 1);
                                System.sleep(40);
                                this.ele.setArgs(12, 5.0f);
                                Sound.effectPlay(196744);
                                this.doorR2.start(1, "MoveD");
                                System.sleep(60);
                                this.fade.call(0);
                                System.sleep(30);
                                Runtime.setPlayerControl(true);
                                Runtime.jumpCF(66206, 2);
                                this.epass = 1;
                                return;
                            }
                        }
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(50, -7.9f, -0.0f, true);
                        System.sleep(50);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        this.cam0.setMode(0);
                        return;
                    }
                    this.epass = 1;
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
                    if (this.epass == 0) return;
                    System.println("on!!");
                    this.epass = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 7) return;
        switch (n) {
            case 100: {
                if (this.button2_flg == 1) {
                    return;
                }
                this.button2_flg = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.MESS_02, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                this.button2_flg = 0;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                System.println("＊＊＊＊CFイベント起動＊＊＊＊");
                if (Runtime.getFlags(3024, 1) != 0) break;
                System.println("押すぜ隔壁スイッチ！！");
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(3021, 1, 1);
                Runtime.setFlags(3024, 1, 1);
                Runtime.setFlags(3025, 1, 1);
                Runtime.jumpCF(66217, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(66206, 9);
                break;
            }
            case 2: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(66256, 2);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(65, false);
        Stage.setVisible(66, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -9.4f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, 13.5f, -1.0f, -25.0f, 0.0f);
        this.teiten2.SetBgm(196619);
        this.teiten3 = new Uwamono(28690, 7.0f, -1.0f, 13.0f, 0.0f);
        this.teiten3.SetBgm(196619);
        this.teiten4 = new Uwamono(28690, 9.0f, -1.0f, 13.0f, 0.0f);
        this.teiten4.SetBgm(196619);
        this.teiten5 = new Uwamono(28690, 7.5f, 0.0f, -9.5f, 0.0f);
        this.teiten5.SetBgm(196620);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -7.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 6.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 6.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFLockX(10, -8.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFLockX(13, 18.0f);
        this.cam0.setCFAngle(14, 0.0f, -10.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.getFlags(3040, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy1 = new Enepc();
                this.enemy1.init(16642, 3, 5.0f, 0.0f, 9.0f, 180.0f);
                this.enemy1.id = 1;
                this.enemy1.setGroup(0, 0, 0, 0);
                float[] fArray3 = new float[8];
                fArray3[0] = 5.0f;
                fArray3[2] = 9.0f;
                fArray3[3] = 1.0f;
                fArray3[4] = 5.0f;
                fArray3[6] = 10.0f;
                fArray3[7] = -1.0f;
                fArray2 = fArray3;
                this.enemy1.setParams(1, 0, 1, 3, fArray2);
                this.enemy1.enableDTKFlag(262144);
                this.enemy1.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy1 = new Enepc();
                this.enemy1.init(16642, 3, 5.0f, 0.0f, 9.0f, 180.0f);
                this.enemy1.id = 1;
                this.enemy1.setGroup(3, 3, 3, 3);
                this.enemy1.setParams(1, 5, 1, 3);
                this.enemy1.enableDTKFlag(262144);
                this.enemy1.disableDTKFlag(2);
                this.enemy1.getTranslate();
            }
        }
        if (Runtime.getFlags(3041, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy2 = new Enepc();
                this.enemy2.init(16642, 3, 7.0f, 0.0f, 9.0f, 180.0f);
                this.enemy2.id = 2;
                this.enemy2.setGroup(0, 0, 0, 0);
                float[] fArray4 = new float[8];
                fArray4[0] = 7.0f;
                fArray4[2] = 9.0f;
                fArray4[3] = 1.0f;
                fArray4[4] = 7.0f;
                fArray4[6] = 10.0f;
                fArray4[7] = -1.0f;
                fArray2 = fArray4;
                this.enemy2.setParams(1, 1, 2, 3, fArray2);
                this.enemy2.enableDTKFlag(262144);
                this.enemy2.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy2 = new Enepc();
                this.enemy2.init(16642, 3, 7.0f, 0.0f, 9.0f, 180.0f);
                this.enemy2.id = 2;
                this.enemy2.setGroup(3, 3, 3, 3);
                this.enemy2.setParams(1, 6, 2, 3);
                this.enemy2.enableDTKFlag(262144);
                this.enemy2.disableDTKFlag(2);
                this.enemy2.getTranslate();
            }
        }
        if (Runtime.getFlags(3042, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy3 = new Enepc();
                this.enemy3.init(16642, 3, 12.0f, 0.0f, 16.0f, 180.0f);
                this.enemy3.id = 3;
                this.enemy3.setGroup(1, 1, 2, 2);
                float[] fArray5 = new float[28];
                fArray5[0] = 12.0f;
                fArray5[2] = 16.0f;
                fArray5[3] = 1.0f;
                fArray5[4] = 12.0f;
                fArray5[6] = 15.0f;
                fArray5[7] = 2.0f;
                fArray5[8] = 12.0f;
                fArray5[10] = 12.0f;
                fArray5[11] = 3.0f;
                fArray5[12] = 13.0f;
                fArray5[14] = 11.0f;
                fArray5[15] = 4.0f;
                fArray5[16] = 16.0f;
                fArray5[18] = 11.0f;
                fArray5[19] = 5.0f;
                fArray5[20] = 17.0f;
                fArray5[22] = 12.0f;
                fArray5[23] = 6.0f;
                fArray5[24] = 18.0f;
                fArray5[26] = 13.0f;
                fArray5[27] = -1.0f;
                fArray2 = fArray5;
                this.enemy3.setParams(1, 2, 3, 3, fArray2);
                float[] fArray6 = new float[30];
                fArray6[0] = 12.0f;
                fArray6[2] = 16.0f;
                fArray6[3] = 12.0f;
                fArray6[5] = 14.0f;
                fArray6[6] = 12.0f;
                fArray6[8] = 12.0f;
                fArray6[9] = 12.0f;
                fArray6[11] = 10.0f;
                fArray6[12] = 14.0f;
                fArray6[14] = 10.0f;
                fArray6[15] = 16.0f;
                fArray6[17] = 10.0f;
                fArray6[18] = 18.0f;
                fArray6[20] = 10.0f;
                fArray6[21] = 18.0f;
                fArray6[23] = 12.0f;
                fArray6[24] = 18.0f;
                fArray6[26] = 14.0f;
                fArray6[27] = 18.0f;
                fArray6[29] = 16.0f;
                fArray = fArray6;
                this.enemy3.setParams(fArray);
                this.enemy3.enableDTKFlag(262144);
                this.enemy3.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy3 = new Enepc();
                this.enemy3.init(16642, 3, 12.0f, 0.0f, 16.0f, 180.0f);
                this.enemy3.id = 3;
                this.enemy3.setGroup(4, 4, 5, 5);
                this.enemy3.setParams(1, 7, 3, 3);
                this.enemy3.enableDTKFlag(262144);
                this.enemy3.disableDTKFlag(2);
                this.enemy3.getTranslate();
            }
        }
        if (Runtime.getFlags(3043, 1) == 0) {
            if (Runtime.getFlags(3025, 1) == 0) {
                this.enemy4 = new Enepc();
                this.enemy4.init(16642, 3, 18.0f, -3.0f, 22.0f, 180.0f);
                this.enemy4.id = 4;
                this.enemy4.setGroup(1, 1, 2, 2);
                fArray2 = new float[]{18.0f, -3.0f, 22.0f, 1.0f, 17.0f, -3.0f, 23.0f, 2.0f, 16.0f, -3.0f, 24.0f, -1.0f};
                this.enemy4.setParams(2, 3, 4, 3, fArray2);
                fArray = new float[]{18.0f, -3.0f, 22.0f, 16.0f, -3.0f, 24.0f, 14.0f, -3.0f, 24.0f, 12.0f, -3.0f, 24.0f, 10.0f, -3.0f, 24.0f, 8.0f, -3.0f, 24.0f, 6.0f, -3.0f, 24.0f};
                this.enemy4.setParams(fArray);
                this.enemy4.enableDTKFlag(262144);
                this.enemy4.getTranslate();
            } else if (Runtime.getFlags(3025, 1) == 1) {
                this.enemy4 = new Enepc();
                this.enemy4.init(16642, 3, 19.0f, -3.0f, 22.0f, 180.0f);
                this.enemy4.id = 4;
                this.enemy4.setGroup(4, 4, 5, 5);
                this.enemy4.setParams(2, 8, 4, 3);
                this.enemy4.enableDTKFlag(262144);
                this.enemy4.disableDTKFlag(2);
                this.enemy4.getTranslate();
            }
        }
        if (Runtime.getFlags(3021, 1) == 1) {
            this.monitor1 = new Obj();
            this.monitor1.init(24613, 7.742f, 2.457f, -9.5f, 270.0f);
            this.monitor1.setArgs(0, 0.0f, 0.0f, 2.138f, 1.584f);
            this.monitor1.setArgs(1, 10017, 0, 128, 112);
            this.monitor1.setArgs(2, 100, 0, 0, -1);
            this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
            this.monitor1.signal(1);
        }
        this.npc2 = new NPC_NORMAL(16642, 12, 0, 0, 5, 200.0f, 100.0f, 200.0f, 0.0f);
        this.npc2.kickEnepc(4, 2);
        this.npc2.kickEnepc(19, 1, 0, 385, 1);
        if (Runtime.getFlags(3025, 1) == 0) {
            this.player.setID(2);
        } else {
            this.player.setID(1);
        }
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 92);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 93);
        this.item03 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 94);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 95);
        this.item05 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 96);
        this.item06 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 97);
        this.item07 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 98);
        this.item08 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 99);
        this.item09 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 100);
        this.item10 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 101);
        new Uwamono(53, 4, this.item01);
        new Uwamono(63, 0);
        new Uwamono(57, 0, this.item02);
        new Uwamono(56, 0, this.item03);
        new Uwamono(55, 0);
        new Uwamono(54, 0);
        new Uwamono(64, 0, this.item04);
        new Uwamono(58, 4, this.item05);
        new Uwamono(51, 21, this.item06);
        new Uwamono(52, 21);
        new Uwamono(59, 0, this.item07);
        new Uwamono(62, 0, this.item08);
        new Uwamono(61, 0);
        new Uwamono(60, 4, this.item09);
        new Uwamono(49, 20, this.item10);
        this.Base01 = new Uwamono(28672, 14.0f, -1.0f, -19.0f, 0.0f);
        this.Base01.SetSize(5.0f, 1.0f, 5.0f);
        this.Base02 = new Uwamono(28672, -9.0f, -1.0f, -7.0f, 0.0f);
        this.Base02.SetSize(5.0f, 1.0f, 5.0f);
        this.Base03 = new Uwamono(28672, 17.5f, -1.0f, 9.5f, 0.0f);
        this.Base03.SetSize(5.0f, 1.0f, 5.0f);
        this.doorA = new Uwamono(39, 40, '\u0001');
        new Uwamono(40, 40, '\u0001', this.doorA);
        this.doorB = new Uwamono(41, 40, '\u0001');
        new Uwamono(42, 40, '\u0001', this.doorB);
        this.doorC = new Uwamono(43, 40, '\u0001');
        new Uwamono(44, 40, '\u0001', this.doorC);
        this.doorD = new Uwamono(45, 40, '\u0001');
        new Uwamono(46, 40, '\u0001', this.doorD);
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorA.SetDoorSpd(24);
        this.doorB.SetDoorSpd(24);
        this.doorC.SetDoorSpd(24);
        this.doorD.SetDoorSpd(24);
        if (Runtime.getFlags(3021, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
        } else if (Runtime.getFlags(3021, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
        }
        this.doorE = new Uwamono(37, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
        this.doorF = new Uwamono(38, 40, '\u0001');
        this.doorF.SetDoorType('\u0002');
        this.doorG = new Uwamono(47, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.doorR1 = new Mapunits();
        this.doorR1.mapUnit(71);
        this.doorR1.start(4, null);
        this.doorR1.start(1, "automatic_door");
        this.doorR1.setTranslate(this.doorR1.px, this.doorR1.py, this.doorR1.pz);
        this.doorL1 = new Mapunits();
        this.doorL1.mapUnit(70);
        this.doorL1.start(4, null);
        this.doorL1.setTranslate(this.doorL1.px, this.doorL1.py, this.doorL1.pz);
        this.tA = new Uwamono(28672, -12.0f, -5.5f, 0.0f, 0.0f);
        this.tA.SetHitKind('\u0001');
        this.tA.SetSize(6.5f, 3.0f, 6.5f);
        if (Runtime.getFlags(3039, 1) == 1) {
            System.println("********KOJI_39 is up********");
            this.ele = new Obj();
            this.ele.initElevator(73, 0.083333336f, 5.0f);
            this.ele.setArgs(1, 0, 1);
            this.ele.start(1, "Up");
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(74);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py + 4.7f, this.doorR2.pz);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(75);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py + 4.7f, this.doorL2.pz);
        } else {
            System.println("＊＊＊＊＊＊＊＊KOJI_39は立ってません＊＊＊＊＊＊＊＊＊");
            this.ele = new Obj();
            this.ele.initElevator(73, 0.083333336f, 0.0f);
            this.ele.setArgs(1, 0, 1);
            this.ele.setArgs(12, 0.0f);
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(74);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py - 0.3f, this.doorR2.pz);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(75);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py - 0.3f, this.doorL2.pz);
        }
        this.eve00 = new Effect(1415, 0);
        this.eve00.disp(true);
        this.eve02 = new Effect(1413, 1);
        this.eve02.disp(true);
        this.Yu01 = new Effect(1560, 2);
        this.Yu02 = new Effect(1560, 3);
        this.Yu03 = new Effect(1560, 4);
        this.Yu04 = new Effect(1560, 5);
        this.Yu05 = new Effect(1560, 6);
        this.Yu06 = new Effect(1560, 7);
        this.Yu07 = new Effect(1560, 8);
        this.Yu08 = new Effect(1560, 9);
        this.Yu09 = new Effect(1560, 10);
        this.Yu10 = new Effect(1560, 11);
        this.Yu11 = new Effect(1560, 12);
        this.Yu12 = new Effect(1560, 13);
        if (Runtime.getFlags(3025, 1) == 0) {
            this.Yu01.disp(false);
            this.Yu02.disp(false);
            this.Yu03.disp(false);
            this.Yu04.disp(false);
            this.Yu05.disp(false);
            this.Yu06.disp(false);
            this.Yu07.disp(false);
            this.Yu08.disp(false);
            this.Yu09.disp(false);
            this.Yu10.disp(false);
            this.Yu11.disp(false);
            this.Yu12.disp(false);
        } else {
            this.Yu01.disp(true);
            this.Yu02.disp(true);
            this.Yu03.disp(true);
            this.Yu04.disp(true);
            this.Yu05.disp(true);
            this.Yu06.disp(true);
            this.Yu07.disp(true);
            this.Yu08.disp(true);
            this.Yu09.disp(true);
            this.Yu10.disp(true);
            this.Yu11.disp(true);
            this.Yu12.disp(true);
        }
        if (Runtime.getFlags(3025, 1) == 0) {
            if (Runtime.getFlags(3040, 1) == 0) {
                this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF01.disp(false);
            }
            if (Runtime.getFlags(3041, 1) == 0) {
                this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF02.disp(false);
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF03.disp(false);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                this.EF04 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF04.disp(false);
            }
        } else if (Runtime.getFlags(3025, 1) == 1) {
            if (Runtime.getFlags(3040, 1) == 0) {
                this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.EF01.setTranslate(this.enemy1.px, this.enemy1.py, this.enemy1.pz);
                this.EF01.disp(true);
            }
            if (Runtime.getFlags(3041, 1) == 0) {
                this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.enemy2.getTranslate();
                this.EF02.setTranslate(this.enemy2.px, this.enemy2.py, this.enemy2.pz);
                this.EF02.disp(true);
            }
            if (Runtime.getFlags(3042, 1) == 0) {
                this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.enemy3.getTranslate();
                this.EF03.setTranslate(this.enemy3.px, this.enemy3.py, this.enemy3.pz);
                this.EF03.disp(true);
            }
            if (Runtime.getFlags(3043, 1) == 0) {
                this.EF04 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
                this.enemy4.getTranslate();
                this.EF04.setTranslate(this.enemy4.px, this.enemy4.py, this.enemy4.pz);
                this.EF04.disp(true);
            }
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
            this.setElevatorMode(1);
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }

        void Up() {
            ST0680.this.cam0.setMode(-1);
            ST0680.this.EV_Camera04();
            Runtime.setPlayerControl(false);
            ST0680.this.player.setTranslate(-10.5f, 5.5f, 0.0f);
            ST0680.this.ele.setArgs(4, 100.0f);
            ST0680.this.ele.setArgs(12, 5.0f);
            System.sleep(1);
            ST0680.this.ele.setArgs(4, 0.083333336f);
            ST0680.this.ele.setArgs(12, 0.0f);
            Sound.effectPlay(196745);
            ST0680.this.doorR2.start(1, "MoveU");
            System.sleep(90);
            ST0680.this.doormove = 1;
            System.sleep(40);
            Runtime.enable(65536);
            ST0680.this.player.mtn(2, 9, 1.0f, true);
            ST0680.this.player.move(30, -8.0f, 0.0f, true);
            System.sleep(35);
            Runtime.disable(65536);
            Runtime.setFlags(3039, 1, 0);
            ST0680.this.cam0.setMode(0);
            ST0680.this.epass = 1;
            Runtime.setPlayerControl(true);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void MoveD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0680.this.doorR2.getTranslate();
                    ST0680.this.doorL2.getTranslate();
                    ST0680.this.doorR2.setTranslate(ST0680.this.doorR2.px, ST0680.this.doorR2.py + 0.083333336f, ST0680.this.doorR2.pz);
                    ST0680.this.doorL2.setTranslate(ST0680.this.doorL2.px, ST0680.this.doorL2.py + 0.083333336f, ST0680.this.doorL2.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void MoveU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0680.this.doorR2.getTranslate();
                    ST0680.this.doorL2.getTranslate();
                    ST0680.this.doorR2.setTranslate(ST0680.this.doorR2.px, ST0680.this.doorR2.py - 0.083333336f, ST0680.this.doorR2.pz);
                    ST0680.this.doorL2.setTranslate(ST0680.this.doorL2.px, ST0680.this.doorL2.py - 0.083333336f, ST0680.this.doorL2.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void automatic_door() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 7;
            int n2 = 30;
            int n3 = 45;
            while (true) {
                ST0680.this.player.getTranslate();
                f2 = (-12.0f - ST0680.this.player.px) * (-12.0f - ST0680.this.player.px) + (0.0f - ST0680.this.player.pz) * (0.0f - ST0680.this.player.pz);
                if (f2 > 20.0f) {
                    if (ST0680.this.doormove == 1) {
                        Sound.effectPlay(196713);
                    }
                    ST0680.this.doormove = 2;
                }
                if (ST0680.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196713);
                    }
                    if (f == (float) n2) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0680.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0680.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0680.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0680.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0680.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == 0.0f) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0680.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0680.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0680.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0680.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0680.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0680.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0680.this.tA.setTranslate(-12.0f, -5.5f, -0.5f);
                } else if (Runtime.getFlags(3038, 1) == 0 && Runtime.getFlags(3039, 1) == 0) {
                    ST0680.this.tA.setTranslate(-12.0f, -1.5f, -0.5f);
                }
                System.sleep(1);
            }
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
}

