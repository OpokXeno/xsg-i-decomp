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

class ST0322
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK04B_PRJ {
    int smd = 0;
    Player player;
    Camera cam0;
    Camera cam1;
    Camera cam2;
    Camera camEV;
    Enepc kosmos;
    Enepc allen;
    Enepc virgil;
    Enepc shion;
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
    Uwamono kow;
    Uwamono ueki;
    Unit Cargo;
    Unit kidou;
    Effect FadeIn01;
    Effect EF01;
    Effect fade;
    Light light = new Light(0);
    int page;
    String[] DOOR = new String[]{"Press partition switch?\n", "/[waitkey(64)]/[close()]"};
    String[] Allen_0 = new String[]{"/[label(Allen)]", "Really Chief. I feel like I'm going to have a heart attack just watching you.", "/[waitkey(64)]/[close()]"};
    String[] Allen_2 = new String[]{"/[label(Allen)]", "Here you go. It's the prototype A.G.W.S. VX-10000 transport capsule that HQ sent over. At least it's better than nothing.", "/[waitkey(1)]/[clear()]", "You know how to use it, right?", "/[waitkey(64)]/[close()]"};
    String[] Ex_2 = new String[]{"/[label(Allen)]", "What?! This is the one developed by Second R&D! Did you forget this capsule is capable of carrying a single A.G.W.S? Press the × Button during battle to deploy it. It's essentially a portable A.G.W.S. hangar created by applying the space compression technology used in transportation. You can, of course, switch out the current A.G.W.S. and load another.", "/[waitkey(64)]/[close()]"};
    String[] Allen_1 = new String[]{"/[label(Allen)]", "Please be more careful, okay? I don't know what I'll do if anything were to happen to you...", "/[waitkey(64)]/[close()]"};
    String[] Allen_3 = new String[]{"/[label(Allen)]", "Nothing! Let's go! Let's find an escape pod quickly, and get out of here!!", "/[waitkey(64)]/[close()]"};
    String[] Shion_0 = new String[]{"/[label(Shion)]", "Of course! I just got a little confused with all that's been going on.", "/[waitkey(64)]/[close()]"};
    String[] Shion_1 = new String[]{"/[label(Shion)]", "I'm sorry. I sort of remember, but...", "/[waitkey(64)]/[close()]"};
    String[] Shion_2 = new String[]{"/[label(Shion)]", "What?! Did you say something?!", "/[waitkey(64)]/[close()]"};

    ST0322() {
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.0f, 3.5f, 18.36f);
        this.camEV.setRotate(-18.035f, 360.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.975f, 3.371f, 17.207f);
        this.camEV.setRotate(-2.442f, 115.517f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.466f, 3.531f, 16.804f);
        this.camEV.setRotate(-2.082f, 195.335f, 0.0f);
        this.camEV.setFov(40.0f);
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
                                    this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(26, 1, 1.0f, true);
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
                                    this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                                    this.player.mtn(26, 1, 1.0f, true);
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
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(65946, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(65866, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(65876, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(65846, 2);
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
        Runtime.disable(524288);
        this.FadeIn01 = new Effect(0);
        this.FadeIn01.args[0] = -268435456;
        this.FadeIn01.args[1] = 30;
        this.FadeIn01.args[2] = 0;
        this.EF01 = new Effect(1012, 0);
        if (Runtime.getFlags(3008, 1) == 0) {
            this.EF01.disp(true);
        } else {
            this.EF01.disp(false);
        }
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
        this.kosmos = new NPC_NORMAL(2, 11, 0, 6, 3, -7.542916f, 2.0f, 18.189001f, -30.0f);
        this.kosmos.disableDTKFlag(8);
        this.kosmos.disableDTKFlag(131072);
        this.kosmos.disableDTKFlag(1);
        this.kosmos.disableDTKFlag(2);
        this.kosmos.setMotion(0, 29);
        this.kosmos.setLocation(4, 1);
        this.kosmos.dispRadar(false);
        this.allen = new NPC_NORMAL(263, 12, 0, 6, 5, -7.241121f, 2.0f, 18.895382f, -90.0f);
        this.allen.disableDTKFlag(8);
        this.allen.disableDTKFlag(131072);
        this.allen.disableDTKFlag(1);
        this.allen.disableDTKFlag(2);
        this.allen.setLocation(4, 2);
        this.allen.dispRadar(false);
        this.virgil = new NPC_NORMAL(270, 13, 0, 6, 19, -8.811531f, 2.0f, 20.66158f, 135.0f);
        this.virgil.disableDTKFlag(8);
        this.virgil.disableDTKFlag(131072);
        this.virgil.disableDTKFlag(1);
        this.virgil.disableDTKFlag(2);
        this.virgil.setMotion(0, 29);
        this.virgil.setLocation(4, 3);
        this.virgil.dispRadar(false);
        this.shion = new NPC_NORMAL(1, 14, 0, 6, 9, -7.812862f, 2.0f, 18.814579f, 135.0f);
        this.shion.disableDTKFlag(8);
        this.shion.disableDTKFlag(131072);
        this.shion.disableDTKFlag(1);
        this.shion.disableDTKFlag(2);
        this.shion.setLocation(4, 0);
        this.shion.dispRadar(false);
        this.player.setTranslate(100.0f, 100.0f, 100.0f);
        this.kidou = new Mapunits();
        this.kidou.mapUnit(93);
        this.kidou.start(4, null);
        this.kidou.start(1, "Evt");
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
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

        void Evt() {
            ST0322.this.cam0.setMode(-1);
            ST0322.this.EV_Camera00();
            Runtime.setPlayerControl(false);
            ST0322.this.shion.kickEnepc(4, 1);
            ST0322.this.shion.kickEnepc(1, 27);
            ST0322.this.allen.kickEnepc(4, 1);
            ST0322.this.allen.kickEnepc(0, 9);
            ST0322.this.win = Window.create();
            ST0322.this.win.setSize(4, 45);
            ST0322.this.win.setLocation(15, 305);
            ST0322.this.win.print(ST0322.this.Allen_0, 0);
            System.sleep(120);
            System.waitFor(ST0322.this.win);
            ST0322.this.allen.kickEnepc(4, 0);
            ST0322.this.EV_Camera01();
            ST0322.this.allen.kickEnepc(4, 1);
            ST0322.this.allen.kickEnepc(0, 9);
            ST0322.this.win = Window.create();
            ST0322.this.win.setSize(4, 45);
            ST0322.this.win.setLocation(15, 305);
            ST0322.this.win.print(ST0322.this.Allen_2, 0);
            System.waitFor(ST0322.this.win);
            System.sleep(60);
            Runtime.setTakeAgws(8193);
            ST0322.this.allen.kickEnepc(4, 0);
            System.sleep(40);
            ST0322.this.menu = Menu.create();
            ST0322.this.menu.addItem("Of course\nUm, I forgot");
            System.waitFor(ST0322.this.menu);
            ST0322.this.selected = ST0322.this.menu.getSelected();
            switch (ST0322.this.selected) {
                case 0: {
                    System.sleep(40);
                    ST0322.this.shion.kickEnepc(0, 7);
                    System.sleep(20);
                    ST0322.this.shion.kickEnepc(0, 9);
                    ST0322.this.win = Window.create();
                    ST0322.this.win.setSize(4, 45);
                    ST0322.this.win.setLocation(15, 305);
                    ST0322.this.win.print(ST0322.this.Shion_0, 0);
                    System.waitFor(ST0322.this.win);
                    break;
                }
                default: {
                    System.sleep(40);
                    ST0322.this.shion.kickEnepc(0, 8);
                    System.sleep(23);
                    ST0322.this.shion.kickEnepc(0, 9);
                    ST0322.this.win = Window.create();
                    ST0322.this.win.setSize(4, 45);
                    ST0322.this.win.setLocation(15, 305);
                    ST0322.this.win.print(ST0322.this.Shion_1, 0);
                    System.waitFor(ST0322.this.win);
                    System.sleep(40);
                    ST0322.this.allen.kickEnepc(4, 1);
                    ST0322.this.allen.kickEnepc(0, 9);
                    ST0322.this.win = Window.create();
                    ST0322.this.win.setSize(4, 45);
                    ST0322.this.win.setLocation(15, 305);
                    ST0322.this.win.print(ST0322.this.Ex_2, 0);
                    System.waitFor(ST0322.this.win);
                    ST0322.this.allen.kickEnepc(4, 0);
                }
            }
            ST0322.this.shion.kickEnepc(0, 0);
            ST0322.this.shion.kickEnepc(9, 11);
            ST0322.this.allen.kickEnepc(4, 1);
            ST0322.this.allen.kickEnepc(0, 0);
            System.sleep(30);
            ST0322.this.allen.kickEnepc(0, 8);
            System.sleep(40);
            ST0322.this.win = Window.create();
            ST0322.this.win.setSize(4, 45);
            ST0322.this.win.setLocation(15, 305);
            ST0322.this.win.print(ST0322.this.Allen_1, 0);
            System.waitFor(ST0322.this.win);
            ST0322.this.allen.kickEnepc(4, 0);
            ST0322.this.shion.kickEnepc(9, 12);
            ST0322.this.shion.kickEnepc(4, 1);
            ST0322.this.shion.kickEnepc(0, 9);
            ST0322.this.win = Window.create();
            ST0322.this.win.setSize(4, 45);
            ST0322.this.win.setLocation(15, 305);
            ST0322.this.win.print(ST0322.this.Shion_2, 0);
            System.waitFor(ST0322.this.win);
            ST0322.this.allen.kickEnepc(4, 1);
            ST0322.this.allen.kickEnepc(9, 14);
            System.sleep(40);
            ST0322.this.allen.kickEnepc(0, 8);
            System.sleep(40);
            ST0322.this.allen.kickEnepc(0, 9);
            ST0322.this.win = Window.create();
            ST0322.this.win.setSize(4, 45);
            ST0322.this.win.setLocation(15, 305);
            ST0322.this.win.print(ST0322.this.Allen_3, 0);
            System.waitFor(ST0322.this.win);
            System.sleep(30);
            ST0322.this.FadeIn01.call(0);
            System.sleep(30);
            Runtime.setFlags(3007, 1, 1);
            Runtime.setPlayerControl(true);
            Runtime.jumpCF(65860, 3);
            ST0322.this.cam0.setMode(0);
        }
    }
}

