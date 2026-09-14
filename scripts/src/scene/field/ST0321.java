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
import xeno.map.MC_VOK04B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0321
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
    Enepc enemy10;
    Unit[] unit;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int entrance;
    int DoorLock = 0;
    int button_flg = 0;
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
    Uwamono item_box1;
    Uwamono Test;
    Uwamono kow1;
    Uwamono kow;
    Uwamono ueki;
    Unit Cargo;
    Unit monitor1;
    Effect EF01;
    Effect fade;
    Unit bar;
    Unit tray1;
    Unit tray2;
    Unit tray3;
    Effect eft3;
    Effect eft4;
    Effect eft5;
    Light light = new Light(0);
    int smd = 0;
    int page;
    String[] DOOR = new String[]{"Press partition switch?\n", "/[waitkey(64)]/[close()]"};
    String[] Tomiyama_1 = new String[]{"/[label(Shion)]", "Hmm...looks like the partition won't open. I better find another route.", "/[waitkey(64)]/[close()]"};

    ST0321() {
    }

    void EOB_Escape(int n) {
        System.println("EOB_Escape****************************************************");
        if (n == 1 && Runtime.getFlags(3031, 1) == 0) {
            System.println("1");
            Runtime.setFlags(3031, 1, 1);
            System.println("313131313131313131313131313131313131313131313131313131313131313131");
            Runtime.jumpEvent(67438);
        }
        if (n == 10 && Runtime.getFlags(3031, 1) == 0) {
            System.println("1");
            Runtime.setFlags(3031, 1, 1);
            System.println("313131313131313131313131313131313131313131313131313131313131313131");
            Runtime.jumpEvent(67438);
        }
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.0f, 3.5f, 18.36f);
        this.camEV.setRotate(-18.035f, 360.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(11.645f, 1.771f, -18.843f);
        this.camEV.setRotate(-5.395f, 168.479f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        float[] fArray = new float[]{1.0f, 11.87f, 2.859f, -3.452f, 60.0f, 11.87f, 0.811f, -3.452f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 6.903f;
        fArray2[2] = 15.419f;
        fArray2[4] = 60.0f;
        fArray2[5] = 6.903f;
        fArray2[6] = 15.419f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 60);
        this.camEV.rotateSPL(fArray3, 1, 2, 60);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        int n = 10;
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
        int n = 10;
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
                                    Runtime.enable(65536);
                                    this.player.setTranslate(14.81f, 0.0f, 9.48f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(50);
                                    this.cam0.setMode(-1);
                                    this.EV_Camera();
                                    this.Kakuheki_Open();
                                    System.sleep(30);
                                    this.EF01.disp(false);
                                    this.cam0.setMode(0);
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
                                    Runtime.enable(65536);
                                    this.player.setTranslate(14.81f, 0.0f, 9.48f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(50);
                                    this.cam0.setMode(-1);
                                    this.EV_Camera();
                                    this.Kakuheki_Close();
                                    System.sleep(30);
                                    this.EF01.disp(true);
                                    this.cam0.setMode(0);
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
                    }
                }
            }
            return;
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    this.enemy1.kickEnepc(4, 1);
                    this.enemy10.kickEnepc(4, 1);
                    this.enemy1.kickEnepc(1, 27);
                    this.enemy10.kickEnepc(1, 27);
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px, this.player.py, this.player.pz - 0.3f);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.cam0.setMode(-1);
                    this.EV_Camera01();
                    System.sleep(30);
                    Stage.setVisible(36, true);
                    Stage.setVisible(37, true);
                    Stage.setVisible(38, true);
                    Stage.setVisible(39, true);
                    Stage.setVisible(40, true);
                    Stage.setVisible(41, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Tomiyama_1, 0);
                    System.waitFor(this.win);
                    System.sleep(30);
                    this.cam0.setMode(0);
                    Stage.setVisible(36, false);
                    Stage.setVisible(37, false);
                    Stage.setVisible(38, false);
                    Stage.setVisible(39, false);
                    Stage.setVisible(40, false);
                    Stage.setVisible(41, false);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.enemy1.kickEnepc(4, 0);
                    this.enemy10.kickEnepc(4, 0);
                    break;
                }
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
                if (Runtime.getFlags(3009, 1) == 0) {
                    Runtime.jumpCF(65947, 4);
                    break;
                }
                Runtime.jumpCF(65949, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(65866, 1);
                break;
            }
        }
    }

    void init() {
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
        if (Runtime.getFlags(3008, 1) == 0) {
            this.EF01.disp(true);
        } else {
            this.EF01.disp(false);
        }
        if (Runtime.getFlags(3031, 1) == 1) {
            this.bar = new Mapunits();
            this.bar.mapUnit(136);
            this.bar.start(4, null);
            this.bar.setTranslate(this.bar.px - 0.06f, this.bar.py - 4.69f, this.bar.pz);
            this.bar.setRotate(-7.0f, 1.0f, -19.0f);
            this.tray1 = new Mapunits();
            this.tray1.mapUnit(135);
            this.tray1.start(4, null);
            this.tray1.setTranslate(this.tray1.px, this.tray1.py - 3.02f, this.tray1.pz + 0.2f);
            this.tray2 = new Mapunits();
            this.tray2.mapUnit(137);
            this.tray2.start(4, null);
            this.tray2.setTranslate(this.tray2.px, this.tray2.py - 2.81f, this.tray2.pz + 0.57f);
            this.tray3 = new Mapunits();
            this.tray3.mapUnit(138);
            this.tray3.start(4, null);
            this.tray3.setTranslate(this.tray3.px, this.tray3.py - 3.23f, this.tray3.pz + 0.26f);
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
        }
        Stage.setVisible(-1, true);
        this.Cargo = new Unit();
        this.Cargo.init(20580, 7.34f, -1.0f, 60.23f, 165.0f);
        if (Runtime.getFlags(3031, 1) == 1) {
            Stage.setVisible(196, false);
            Stage.setVisible(197, false);
        }
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
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
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
        this.doorF.SetDoorType('\u0002');
        this.doorF.DoorClose();
        this.doorG = new Uwamono(69, 42, '\u0001');
        new Uwamono(70, 42, '\u0001', this.doorG);
        if (Runtime.getFlags(43, 1) == 0) {
            this.doorG.SetDoorType('\u0004');
        } else {
            this.doorG.SetDoorType('\u0002');
        }
        this.doorH = new Uwamono(170, 42, '\u0001');
        new Uwamono(161, 42, '\u0001', this.doorH);
        this.doorH.SetDoorType('\u0004');
        this.doorH.DoorClose();
        this.doorI = new Uwamono(58, 40, '\u0001');
        this.doorI.SetDoorType('\u0004');
        this.doorI.DoorClose();
        this.doorJ = new Uwamono(61, 40, '\u0004');
        this.doorJ.SetDoorType('\u0002');
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
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 3, 5.5f, 0.0f, -25.5f, 72.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(7, 7, 7, 7);
        float[] fArray = new float[36];
        fArray[0] = 5.5f;
        fArray[2] = -25.5f;
        fArray[3] = 1.0f;
        fArray[4] = 6.827f;
        fArray[6] = -25.369f;
        fArray[7] = 2.0f;
        fArray[8] = 8.166f;
        fArray[10] = -24.957f;
        fArray[11] = 3.0f;
        fArray[12] = 9.213f;
        fArray[14] = -24.107f;
        fArray[15] = 4.0f;
        fArray[16] = 10.0f;
        fArray[18] = -23.0f;
        fArray[19] = 5.0f;
        fArray[20] = 10.0f;
        fArray[22] = -21.0f;
        fArray[23] = 6.0f;
        fArray[24] = 10.0f;
        fArray[26] = -19.0f;
        fArray[27] = 7.0f;
        fArray[28] = 10.0f;
        fArray[30] = -17.0f;
        fArray[31] = 8.0f;
        fArray[32] = 10.0f;
        fArray[34] = -15.0f;
        fArray[35] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 14, 1, 3, fArray2);
        this.enemy1.setTogetherWith(10, 10, 10, 10);
        this.enemy1.setBatEvent(29);
        this.enemy1.setTP(300);
        this.enemy1.setShadow(0, 0);
        this.enemy10 = new Enepc();
        this.enemy10.init(16385, 3, 10.0f, 0.0f, -20.0f, -15.0f);
        this.enemy10.id = 10;
        this.enemy10.setGroup(7, 7, 7, 7);
        float[] fArray3 = new float[24];
        fArray3[0] = 10.0f;
        fArray3[2] = -20.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = 10.0f;
        fArray3[6] = -19.0f;
        fArray3[7] = 2.0f;
        fArray3[8] = 10.0f;
        fArray3[10] = -18.0f;
        fArray3[11] = 3.0f;
        fArray3[12] = 10.0f;
        fArray3[14] = -17.0f;
        fArray3[15] = 4.0f;
        fArray3[16] = 10.0f;
        fArray3[18] = -16.0f;
        fArray3[19] = 5.0f;
        fArray3[20] = 10.0f;
        fArray3[22] = -15.0f;
        fArray3[23] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy10.setParams(1, 14, 10, 3, fArray4);
        this.enemy10.setTogetherWith(1, 1, 1, 1);
        this.enemy10.setBatEvent(29);
        this.enemy10.setTP(300);
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
        this.ueki = new Uwamono(200, 84);
        this.ueki.SetSize(0.5f, 2.0f, 0.5f);
        new Uwamono(125, 21);
        new Uwamono(147, 4);
        new Uwamono(148, 4);
        new Uwamono(153, 24);
        new Uwamono(180, 24);
        new Uwamono(181, 24);
        new Uwamono(182, 24);
        new Uwamono(198, 0);
        new Uwamono(201, 84);
        this.kow1 = new Uwamono(199, 6);
        this.kow1.SetBrokenEnemy(true);
        this.kow1.SetSize(2.5f, 0.5f, 2.5f);
        new Uwamono(126, 31);
        this.trap1 = new Uwamono(28674, 9.8234f, -0.3666f, 71.0247f, 0.0f);
        this.trap1.SetCallNo(1);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.monitor1 = new Object();
        this.monitor1.init(24613, 7.141f, 1.65f, -15.165f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18010, 0, 256, 130);
        this.monitor1.setArgs(2, 100, 0, 5, -6);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        Stage.setVisible(119, false);
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
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

