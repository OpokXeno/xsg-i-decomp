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
import xeno.map.MC_VOK04B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0329
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK04B_PRJ {
    Player player;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera camEV;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy3a;
    Enepc enemy3b;
    Enepc enemy3c;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Enepc enemy9;
    Enepc enemy10;
    Enepc npc1;
    Unit[] unit;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int entrance;
    int button_flg = 0;
    int button2_flg = 0;
    int buttonB_flg = 0;
    Uwamono door01;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono doorJ;
    Uwamono doorK;
    Uwamono Tank01;
    Uwamono Tank02;
    Uwamono Tank03;
    Uwamono Tank04;
    Uwamono Tank05;
    Uwamono Tank06;
    Uwamono kontena01;
    Uwamono kontena02;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono trap4;
    Uwamono item_box1;
    Uwamono Test;
    Uwamono kow;
    Uwamono kow1;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Uwamono item7;
    Uwamono Base1;
    Uwamono Base2;
    Uwamono Base3;
    Uwamono Base4;
    Uwamono ueki;
    Uwamono Evs;
    Unit Cargo;
    Unit monitor1;
    Unit bar;
    Unit tray1;
    Unit tray2;
    Unit tray3;
    Effect EF01;
    Effect EF02;
    Effect fade;
    Effect eft3;
    Effect eft4;
    Effect eft5;
    Light light = new Light(0);
    int smd = 0;
    int b_flg;
    int page;
    String[] DOOR = new String[]{"Press partition switch?\n", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"No response.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 18.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 18.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 18, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};

    ST0329() {
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.63f, 1.698f, 16.256f);
        this.camEV.setRotate(-2.775f, 334.239f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera00_01() {
        float[] fArray = new float[]{1.0f, 2.478f, 4.328f, 24.502f, 90.0f, 9.08f, 4.328f, 15.722f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -21.412f;
        fArray2[2] = -36.939f;
        fArray2[4] = 90.0f;
        fArray2[5] = -21.412f;
        fArray2[6] = -36.939f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera00_01a() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.478f, 4.328f, 24.502f);
        this.camEV.setRotate(-21.412f, -36.939f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera00_01b() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.08f, 4.328f, 15.722f);
        this.camEV.setRotate(-21.412f, -36.939f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        int n = 12;
        this.doorE.DoorClose();
        System.sleep(n);
        this.doorD.DoorClose();
        System.sleep(n);
        this.doorC.DoorClose();
        System.sleep(n);
        this.doorB.DoorClose();
        System.sleep(n);
        this.doorA.DoorClose();
    }

    void Kakuheki_Open() {
        int n = 12;
        this.doorA.DoorOpen();
        System.sleep(n);
        this.doorB.DoorOpen();
        System.sleep(n);
        this.doorC.DoorOpen();
        System.sleep(n);
        this.doorD.DoorOpen();
        System.sleep(n);
        this.doorE.DoorOpen();
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    switch (Runtime.getFlags(3008, 1)) {
                        case 0: {
                            Runtime.setPlayerControl(false);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.DOOR, 0);
                            System.waitFor(this.win);
                            this.menu = Menu.create();
                            this.menu.addItem("Yes\nNo");
                            System.waitFor(this.menu);
                            this.selected = this.menu.getSelected();
                            switch (this.selected) {
                                case 0: {
                                    this.cam0.setMode(-1);
                                    Stage.setVisible(36, true);
                                    Stage.setVisible(37, true);
                                    Stage.setVisible(38, true);
                                    Stage.setVisible(39, true);
                                    Stage.setVisible(40, true);
                                    Stage.setVisible(41, true);
                                    this.EV_Camera();
                                    Runtime.enable(65536);
                                    this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(26, 1, 1.0f, true);
                                    System.sleep(30);
                                    Sound.effectPlay(196741);
                                    System.sleep(10);
                                    Sound.streamPlay(196021, 48000);
                                    System.sleep(349);
                                    Sound.effectPlay(196742);
                                    this.Kakuheki_Open();
                                    System.sleep(30);
                                    this.EF01.disp(true);
                                    this.cam0.setMode(0);
                                    Stage.setVisible(36, false);
                                    Stage.setVisible(37, false);
                                    Stage.setVisible(38, false);
                                    Stage.setVisible(39, false);
                                    Stage.setVisible(40, false);
                                    Stage.setVisible(41, false);
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    Runtime.setFlags(3008, 1, 1);
                                    this.button_flg = 0;
                                    return;
                                }
                            }
                            Runtime.setPlayerControl(true);
                            this.button_flg = 0;
                            return;
                        }
                        case 1: {
                            Runtime.setPlayerControl(false);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.DOOR, 0);
                            System.waitFor(this.win);
                            this.menu = Menu.create();
                            this.menu.addItem("Yes\nNo");
                            System.waitFor(this.menu);
                            this.selected = this.menu.getSelected();
                            switch (this.selected) {
                                case 0: {
                                    this.cam0.setMode(-1);
                                    Stage.setVisible(36, true);
                                    Stage.setVisible(37, true);
                                    Stage.setVisible(38, true);
                                    Stage.setVisible(39, true);
                                    Stage.setVisible(40, true);
                                    Stage.setVisible(41, true);
                                    this.EV_Camera();
                                    Runtime.enable(65536);
                                    this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(26, 1, 1.0f, true);
                                    System.sleep(30);
                                    Sound.effectPlay(196741);
                                    System.sleep(10);
                                    Sound.streamPlay(196011, 48000);
                                    System.sleep(382);
                                    Sound.effectPlay(196742);
                                    this.Kakuheki_Close();
                                    System.sleep(30);
                                    this.EF01.disp(true);
                                    this.cam0.setMode(0);
                                    Stage.setVisible(36, false);
                                    Stage.setVisible(37, false);
                                    Stage.setVisible(38, false);
                                    Stage.setVisible(39, false);
                                    Stage.setVisible(40, false);
                                    Stage.setVisible(41, false);
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    Runtime.setFlags(3008, 1, 0);
                                    this.button_flg = 0;
                                    return;
                                }
                            }
                            Runtime.setPlayerControl(true);
                            this.button_flg = 0;
                            return;
                        }
                        default: {
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2) return;
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    if (this.buttonB_flg == 1) {
                        return;
                    }
                    this.buttonB_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.buttonB_flg = 0;
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
                    if (Runtime.getFlags(3218, 1) == 0) {
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
                        Runtime.setFlags(3218, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    } else if (Runtime.getFlags(3238, 1) == 0) {
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
                        if (Runtime.getFlags(3298, 1) != 0) return;
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
                        this.doorJ.SetDoorType('\u0004');
                        Runtime.setFlags(3298, 1, 1);
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
        if (n2 == 6 || n2 != 9) return;
        switch (n) {
            case 100: {
                if (this.buttonB_flg == 1) {
                    return;
                }
                this.buttonB_flg = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_00, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                this.buttonB_flg = 0;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                this.Tank05.SendSignal();
                System.sleep(10);
                this.Tank06.SendSignal();
                System.sleep(20);
                this.Tank03.SendSignal();
                System.sleep(10);
                this.Tank04.SendSignal();
                System.sleep(5);
                this.Tank01.SendSignal();
                System.sleep(20);
                this.Tank02.SendSignal();
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
                System.println("【環境シミュレータ】・VOK12へjump！！");
                Runtime.jumpCF(419, 4);
                break;
            }
            case 1: {
                System.println("【環境シミュレータ】・VOK05へjump！！");
                Runtime.jumpCF(339, 1);
                break;
            }
            case 2: {
                System.println("【環境シミュレータ】・VOK06へjump！！");
                Runtime.jumpCF(349, 2);
                break;
            }
            case 3: {
                System.println("【環境シミュレータ】・VOK03へjump！！");
                Runtime.jumpCF(319, 2);
                break;
            }
            case 4: {
                System.println("【環境シミュレータ】・VOK18へjump！！");
                Runtime.jumpCF(189, 1);
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
        ST0329.waitPage(this.win, 64);
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
        Runtime.setFlags(3018, 1, 1);
        Runtime.setFlags(43, 1, 1);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 1, 0.47f, 0.47f, 0.47f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -0.5f);
        Runtime.setIdLightCol(2, 0, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(2, 1, 0.47f, 0.47f, 0.47f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -0.5f);
        this.EF01 = new Effect(1012, 0);
        this.EF01.disp(true);
        this.EF02 = new Effect(1401, 12.0f, -0.5f, 21.0f, 270.0f);
        this.EF02.disp(true);
        Stage.setVisible(-1, true);
        this.Cargo = new Unit();
        this.Cargo.init(20580, 7.34f, -1.0f, 60.23f, 165.0f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 9.0f);
        this.cam0.setCFAngle(2, -28.0f, 335.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 10.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, 3.5f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestal(9, 16.848f, 2.755f, 14.868f, 53.12f, -25.776f, 397.915f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFPedestal(10, 16.848f, 2.755f, 14.868f, 53.12f, -25.776f, 397.915f, 0.0f, 2.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(10, 1);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFLockX(12, 7.0f);
        Stage.setVisible(36, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(39, false);
        Stage.setVisible(40, false);
        Stage.setVisible(41, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(50, false);
        Stage.setVisible(42, false);
        Stage.setVisible(43, false);
        Stage.setVisible(44, false);
        Stage.setVisible(45, false);
        Stage.setVisible(46, false);
        Stage.setVisible(47, false);
        Stage.setVisible(173, false);
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(51, false);
        Stage.setVisible(36, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(51, false);
        Stage.setVisible(196, false);
        Stage.setVisible(197, false);
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorK = new Uwamono(54, 40, '\u0003');
        this.doorK.SetDoorType('\u0004');
        this.doorA = new Uwamono(107, 42, '\u0001');
        new Uwamono(108, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(109, 42, '\u0001');
        new Uwamono(110, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(111, 42, '\u0001');
        new Uwamono(112, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(113, 42, '\u0001');
        new Uwamono(114, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(115, 42, '\u0001');
        new Uwamono(116, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(65, 42, '\u0001');
        new Uwamono(66, 42, '\u0001', this.doorF);
        this.doorF.SetDoorType('\u0004');
        this.doorF.DoorClose();
        this.doorG = new Uwamono(69, 42, '\u0001');
        new Uwamono(70, 42, '\u0001', this.doorG);
        this.doorG.SetDoorType('\u0002');
        this.doorH = new Uwamono(170, 42, '\u0001');
        new Uwamono(161, 42, '\u0001', this.doorH);
        this.doorH.SetDoorType('\u0002');
        this.doorI = new Uwamono(58, 40, '\u0001');
        this.doorI.SetDoorType('\u0004');
        if (Runtime.getFlags(3298, 1) == 0) {
            this.doorJ = new Uwamono(61, 40, '\u0004');
            this.doorJ.SetDoorType('\u0002');
        } else {
            this.doorJ = new Uwamono(61, 40, '\u0004');
            this.doorJ.SetDoorType('\u0004');
        }
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorE.SetDoorType('\u0002');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorE.SetDoorRange(3.0f);
        if (Runtime.getFlags(3008, 1) == 0) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
            this.doorE.DoorClose();
        } else if (Runtime.getFlags(3008, 1) == 1) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
            this.doorE.DoorOpen();
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.npc1 = new NPC_NORMAL(8449, 11, 0, 10, 11, 12.0f, 0.0f, 21.0f, 270.0f);
        this.npc1.setMotion(0, 1);
        this.npc1.dispRadar(false);
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 3, 8.0f, 0.0f, -32.0f, 263.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray = new float[84];
        fArray[0] = 8.0f;
        fArray[2] = -32.0f;
        fArray[3] = 1.0f;
        fArray[4] = 7.463f;
        fArray[6] = -31.285f;
        fArray[7] = 2.0f;
        fArray[8] = 8.715f;
        fArray[10] = -30.927f;
        fArray[11] = 3.0f;
        fArray[12] = 7.853f;
        fArray[14] = -30.152f;
        fArray[15] = 4.0f;
        fArray[16] = 6.45f;
        fArray[18] = -30.45f;
        fArray[19] = 5.0f;
        fArray[20] = 5.973f;
        fArray[22] = -31.881f;
        fArray[23] = 6.0f;
        fArray[24] = 4.781f;
        fArray[26] = -31.046f;
        fArray[27] = 7.0f;
        fArray[28] = 4.602f;
        fArray[30] = -29.615f;
        fArray[31] = 8.0f;
        fArray[32] = 4.84f;
        fArray[34] = -28.244f;
        fArray[35] = 9.0f;
        fArray[36] = 5.079f;
        fArray[38] = -26.992f;
        fArray[39] = 10.0f;
        fArray[40] = 5.473f;
        fArray[42] = -25.8f;
        fArray[43] = 11.0f;
        fArray[44] = 6.271f;
        fArray[46] = -24.489f;
        fArray[47] = 12.0f;
        fArray[48] = 8.06f;
        fArray[50] = -23.952f;
        fArray[51] = 13.0f;
        fArray[52] = 9.55f;
        fArray[54] = -23.058f;
        fArray[55] = 14.0f;
        fArray[56] = 10.027f;
        fArray[58] = -20.733f;
        fArray[59] = 15.0f;
        fArray[60] = 9.908f;
        fArray[62] = -17.871f;
        fArray[63] = 16.0f;
        fArray[64] = 9.908f;
        fArray[66] = -16.023f;
        fArray[67] = 17.0f;
        fArray[68] = 9.848f;
        fArray[70] = -13.043f;
        fArray[71] = 18.0f;
        fArray[72] = 9.967f;
        fArray[74] = -9.466f;
        fArray[75] = 19.0f;
        fArray[76] = 9.908f;
        fArray[78] = -6.545f;
        fArray[79] = 20.0f;
        fArray[80] = 10.0f;
        fArray[82] = -4.0f;
        fArray[83] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        this.enemy10 = new Enepc();
        this.enemy10.init(16385, 3, 4.0f, 0.0f, -35.0f, 15.0f);
        this.enemy10.id = 10;
        this.enemy10.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[64];
        fArray3[0] = 4.0f;
        fArray3[2] = -35.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = 4.781f;
        fArray3[6] = -31.046f;
        fArray3[7] = 2.0f;
        fArray3[8] = 4.602f;
        fArray3[10] = -29.615f;
        fArray3[11] = 3.0f;
        fArray3[12] = 4.84f;
        fArray3[14] = -28.244f;
        fArray3[15] = 4.0f;
        fArray3[16] = 5.079f;
        fArray3[18] = -26.992f;
        fArray3[19] = 5.0f;
        fArray3[20] = 5.473f;
        fArray3[22] = -25.8f;
        fArray3[23] = 6.0f;
        fArray3[24] = 6.271f;
        fArray3[26] = -24.489f;
        fArray3[27] = 7.0f;
        fArray3[28] = 8.06f;
        fArray3[30] = -23.952f;
        fArray3[31] = 8.0f;
        fArray3[32] = 9.55f;
        fArray3[34] = -23.058f;
        fArray3[35] = 9.0f;
        fArray3[36] = 10.027f;
        fArray3[38] = -20.733f;
        fArray3[39] = 10.0f;
        fArray3[40] = 9.908f;
        fArray3[42] = -17.871f;
        fArray3[43] = 11.0f;
        fArray3[44] = 9.908f;
        fArray3[46] = -16.023f;
        fArray3[47] = 12.0f;
        fArray3[48] = 9.848f;
        fArray3[50] = -13.043f;
        fArray3[51] = 13.0f;
        fArray3[52] = 9.967f;
        fArray3[54] = -9.466f;
        fArray3[55] = 14.0f;
        fArray3[56] = 9.908f;
        fArray3[58] = -6.545f;
        fArray3[59] = 15.0f;
        fArray3[60] = 10.0f;
        fArray3[62] = -4.0f;
        fArray3[63] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy10.setParams(1, 0, 10, 3, fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(16386, 5, -2.83f, 2.0f, 18.34f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(5, 5, 6, 6);
        float[] fArray5 = new float[24];
        fArray5[0] = -2.83f;
        fArray5[1] = 2.0f;
        fArray5[2] = 18.34f;
        fArray5[3] = 1.0f;
        fArray5[4] = -0.02f;
        fArray5[5] = 2.0f;
        fArray5[6] = 19.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = 7.79f;
        fArray5[10] = 19.62f;
        fArray5[11] = 3.0f;
        fArray5[12] = 9.93f;
        fArray5[14] = 22.54f;
        fArray5[15] = 4.0f;
        fArray5[16] = 9.93f;
        fArray5[18] = 26.85f;
        fArray5[19] = 5.0f;
        fArray5[20] = 9.82f;
        fArray5[21] = -1.0f;
        fArray5[22] = 33.17f;
        fArray5[23] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(2, 7, 2, 5, fArray6);
        this.enemy2.kickEnepc(10, 90, 0);
        this.enemy3a = new Enepc();
        this.enemy3a.init(20227, 7, 10.0f, 0.0f, 19.0f, 52.0f);
        this.enemy3a.id = 31;
        this.enemy3a.setGroup(1, 1, 1, 1);
        float[] fArray7 = new float[24];
        fArray7[0] = 10.0f;
        fArray7[2] = 19.0f;
        fArray7[3] = 1.0f;
        fArray7[4] = 11.5f;
        fArray7[6] = 14.83f;
        fArray7[7] = 2.0f;
        fArray7[8] = 10.0f;
        fArray7[10] = 12.06f;
        fArray7[11] = 3.0f;
        fArray7[12] = 10.0f;
        fArray7[14] = 16.96f;
        fArray7[15] = 4.0f;
        fArray7[16] = 10.0f;
        fArray7[18] = 22.03f;
        fArray7[19] = 5.0f;
        fArray7[20] = 10.0f;
        fArray7[22] = 26.3f;
        fArray7[23] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy3a.setParams(2, 2, 31, 7, fArray8);
        this.enemy3a.setTogetherWith(32, 32, 33, 33);
        this.enemy3a.kickEnepc(10, 50, 0);
        this.enemy3b = new Enepc();
        this.enemy3b.init(20227, 7, 10.0f, 0.0f, 21.0f, 90.0f);
        this.enemy3b.id = 32;
        this.enemy3b.setGroup(1, 1, 1, 1);
        float[] fArray9 = new float[24];
        fArray9[0] = 10.0f;
        fArray9[2] = 21.0f;
        fArray9[3] = 1.0f;
        fArray9[4] = 11.5f;
        fArray9[6] = 14.83f;
        fArray9[7] = 2.0f;
        fArray9[8] = 10.0f;
        fArray9[10] = 12.06f;
        fArray9[11] = 3.0f;
        fArray9[12] = 10.0f;
        fArray9[14] = 16.96f;
        fArray9[15] = 4.0f;
        fArray9[16] = 10.0f;
        fArray9[18] = 22.03f;
        fArray9[19] = 5.0f;
        fArray9[20] = 10.0f;
        fArray9[22] = 26.3f;
        fArray9[23] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3b.setParams(2, 2, 32, 7, fArray10);
        this.enemy3b.setTogetherWith(31, 31, 33, 33);
        this.enemy3b.kickEnepc(10, 50, 0);
        this.enemy3c = new Enepc();
        this.enemy3c.init(20227, 7, 10.0f, 0.0f, 23.0f, 148.0f);
        this.enemy3c.id = 33;
        this.enemy3c.setGroup(1, 1, 1, 1);
        float[] fArray11 = new float[24];
        fArray11[0] = 10.0f;
        fArray11[2] = 23.0f;
        fArray11[3] = 1.0f;
        fArray11[4] = 11.5f;
        fArray11[6] = 14.83f;
        fArray11[7] = 2.0f;
        fArray11[8] = 10.0f;
        fArray11[10] = 12.06f;
        fArray11[11] = 3.0f;
        fArray11[12] = 10.0f;
        fArray11[14] = 16.96f;
        fArray11[15] = 4.0f;
        fArray11[16] = 10.0f;
        fArray11[18] = 22.03f;
        fArray11[19] = 5.0f;
        fArray11[20] = 10.0f;
        fArray11[22] = 26.3f;
        fArray11[23] = -1.0f;
        float[] fArray12 = fArray11;
        this.enemy3c.setParams(2, 2, 33, 7, fArray12);
        this.enemy3c.setTogetherWith(31, 31, 32, 32);
        this.enemy3c.kickEnepc(10, 50, 0);
        this.enemy4 = new Enepc();
        this.enemy4.init(20228, 9, 8.156f, -1.0f, 46.103f, 0.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(4, 4, 4, 4);
        float[] fArray13 = new float[]{8.156f, -1.0f, 46.103f, 1.0f, 9.0f, -1.0f, 42.0f, 2.0f, 9.0f, -1.0f, 46.0f, 3.0f, 9.0f, -1.0f, 50.0f, -1.0f};
        this.enemy4.setParams(3, 3, 4, 9, fArray13);
        float[] fArray14 = new float[]{8.156f, -1.0f, 46.103f, 9.085f, -1.0f, 43.925f, 9.811f, -1.0f, 41.95f, 9.869f, -1.0f, 39.365f};
        this.enemy4.setParams(fArray14);
        this.enemy7 = new Enepc();
        this.enemy7.init(16386, 5, 8.0f, -1.0f, 53.0f, 0.0f);
        this.enemy7.id = 7;
        this.enemy7.setGroup(5, 5, 6, 6);
        float[] fArray15 = new float[]{8.0f, -1.0f, 53.0f, 1.0f, 9.581f, -1.0f, 55.117f, 2.0f, 9.941f, -1.0f, 59.243f, 3.0f, 10.6f, -1.0f, 63.81f, 4.0f, 10.809f, 0.363f, 68.089f, 5.0f, 10.75f, 0.363f, 71.0f, 6.0f, 10.206f, -1.0f, 72.694f, 7.0f, 8.748f, -1.0f, 72.757f, 8.0f, 7.648f, -1.0f, 71.579f, -1.0f};
        this.enemy7.setParams(4, 11, 7, 5, fArray15);
        float[] fArray16 = new float[]{8.0f, -1.0f, 53.0f, 7.888f, -1.0f, 55.585f, 7.711f, -1.0f, 53.5f, 7.977f, -1.0f, 51.792f, 8.443f, -1.0f, 50.129f};
        this.enemy7.setParams(fArray16);
        this.enemy7.kickEnepc(10, 65, 0);
        this.enemy8 = new Enepc();
        this.enemy8.init(16386, 5, -1.8f, -1.0f, 47.75f, 0.0f);
        this.enemy8.id = 8;
        this.enemy8.setGroup(2, 2, 2, 2);
        float[] fArray17 = new float[]{-1.8f, -1.0f, 47.75f, 1.0f, -1.8f, -1.0f, 45.5f, 2.0f, -1.8f, -1.0f, 42.5f, 3.0f, -1.0f, -1.0f, 42.5f, 4.0f, -1.0f, -1.0f, 45.5f, 5.0f, -1.0f, -1.0f, 47.75f, 6.0f, -1.0f, -1.0f, 50.0f, 7.0f, -1.8f, -1.0f, 50.0f, -1.0f};
        this.enemy8.setParams(3, 12, 8, 5, fArray17);
        this.enemy8.kickEnepc(10, 65, 0);
        this.enemy9 = new Enepc();
        this.enemy9.init(16385, 3, 9.0f, -1.0f, 82.0f, 180.0f);
        this.enemy9.id = 9;
        this.enemy9.setGroup(6, 6, 6, 6);
        float[] fArray18 = new float[]{9.0f, -1.0f, 82.0f, 1.0f, 9.0f, -1.0f, 84.0f, -1.0f};
        this.enemy9.setParams(4, 13, 9, 3, fArray18);
        this.Tank01 = new Uwamono(185, 0);
        this.Tank02 = new Uwamono(188, 0);
        this.Tank03 = new Uwamono(186, 0);
        this.Tank04 = new Uwamono(189, 0);
        this.Tank05 = new Uwamono(187, 0);
        this.Tank06 = new Uwamono(190, 0);
        this.Tank01.SetBroken(false);
        this.Tank02.SetBroken(false);
        this.Tank03.SetBroken(false);
        this.Tank04.SetBroken(false);
        this.Tank05.SetBroken(false);
        this.Tank06.SetBroken(false);
        this.Base3 = new Uwamono(28672, 5.0f, 0.0f, -41.0f, 0.0f);
        this.Base3.SetSize(3.0f, 5.0f, 2.0f);
        this.bar = new Mapunits();
        this.bar.mapUnit(136);
        this.bar.start(4, null);
        this.bar.setTranslate(this.bar.px - 0.06f, this.bar.py - 4.69f, this.bar.pz);
        this.bar.setRotate(this.bar.rx - 7.0f, this.bar.ry + 1.0f, this.bar.rz - 19.0f);
        this.tray1 = new Mapunits();
        this.tray1.mapUnit(135);
        this.tray1.start(4, null);
        this.tray1.setTranslate(this.tray1.px, this.tray1.py - 3.02f, this.tray1.pz + 0.2f);
        this.tray1.setRotate(this.tray1.rx + 19.0f, 0.0f, 0.0f);
        this.tray2 = new Mapunits();
        this.tray2.mapUnit(137);
        this.tray2.start(4, null);
        this.tray2.setTranslate(this.tray2.px, this.tray2.py - 2.81f, this.tray2.pz + 0.57f);
        this.tray2.setRotate(this.tray2.rx + 19.0f, 0.0f, 0.0f);
        this.tray3 = new Mapunits();
        this.tray3.mapUnit(138);
        this.tray3.start(4, null);
        this.tray3.setTranslate(this.tray3.px, this.tray3.py - 3.23f, this.tray3.pz + 0.26f);
        this.tray3.setRotate(this.tray3.rx + 19.0f, 0.0f, 0.0f);
        this.eft3 = new Effect(1402, 4.14f, 1.3f, -40.43f, 187.0f);
        this.eft3.setScale(1.5f, 1.5f, 1.5f);
        this.eft3.disp(true);
        this.eft3.setClip(true);
        this.eft3.noAttach(false);
        this.eft5 = new Effect(1402, 6.14f, 1.34f, -40.43f, 187.0f);
        this.eft5.setScale(1.0f, 1.0f, 1.0f);
        this.eft5.disp(true);
        this.eft5.setClip(true);
        this.eft5.noAttach(false);
        this.Base1 = new Uwamono(28672, -4.0f, 1.0f, 15.5f, 0.0f);
        this.Base1.SetSize(5.0f, 1.0f, 2.0f);
        this.Base2 = new Uwamono(28672, 9.0f, -2.0f, 80.5f, 0.0f);
        this.Base2.SetSize(10.0f, 1.0f, 8.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 8);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 9);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 10);
        this.item4 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 11);
        this.item5 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 12);
        this.item6 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 13);
        this.item7 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 14);
        this.ueki = new Uwamono(200, 84);
        this.ueki.SetSize(0.3f, 2.0f, 0.3f);
        new Uwamono(125, 21);
        new Uwamono(147, 4, this.enemy8);
        new Uwamono(148, 4, this.item4);
        new Uwamono(153, 24, this.item6);
        new Uwamono(180, 24);
        new Uwamono(181, 24);
        new Uwamono(182, 24, this.item7);
        new Uwamono(198, 0, this.item5);
        new Uwamono(201, 84, this.item3);
        this.kow1 = new Uwamono(199, 6, this.item1);
        this.kow1.SetSize(2.5f, 0.5f, 2.5f);
        new Uwamono(126, 31, this.item2);
        this.trap1 = new Uwamono(28674, 9.8234f, -0.3666f, 71.0247f, 0.0f);
        this.trap1.SetCallNo(1);
        this.trap2 = new Uwamono(28674, 6.5f, -1.0f, 38.0f, 0.0f);
        this.trap3 = new Uwamono(28673, -4.5f, 2.0f, 20.5f, 0.0f);
        this.trap4 = new Uwamono(28673, 9.5f, 0.0f, -30.5f, 0.0f);
        this.Evs = new Uwamono(28734, 8.0f, -1.0f, 87.0f);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 7.141f, 1.65f, -15.165f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18010, 0, 256, 130);
        this.monitor1.setArgs(2, 100, 0, 5, -6);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        Stage.setVisible(119, false);
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy10.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3a.kickEnepc(4, 1);
        this.enemy3b.kickEnepc(4, 1);
        this.enemy3c.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        this.enemy7.kickEnepc(4, 1);
        this.enemy8.kickEnepc(4, 1);
        this.enemy9.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy10.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3a.kickEnepc(1, 27);
        this.enemy3b.kickEnepc(1, 27);
        this.enemy3c.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        this.enemy7.kickEnepc(1, 27);
        this.enemy8.kickEnepc(1, 27);
        this.enemy9.kickEnepc(1, 27);
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy10.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3a.kickEnepc(4, 0);
        this.enemy3b.kickEnepc(4, 0);
        this.enemy3c.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        this.enemy7.kickEnepc(4, 0);
        this.enemy8.kickEnepc(4, 0);
        this.enemy9.kickEnepc(4, 0);
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

    class Object
            extends Unit {
        Object() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }
    }
}

