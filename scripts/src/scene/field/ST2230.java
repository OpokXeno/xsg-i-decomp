import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK09B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2230
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK09B_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    int PART_LOOK = 0;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc HASIGO;
    Uwamono doorA;
    Uwamono Atari;
    Light light = new Light(0);
    Enepc enemy1;
    Enepc enemy2;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono item01;
    Uwamono item02;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten23;
    int page;
    String[] msghelp7 = new String[]{"/[label()]", "Wait! Why am I being targeted?!", "/[waitkey(1)]/[clear()]", "I'm not even an important character! I'm just a part-timer!!", "/[waitkey(64)]/[close()]"};
    String[] msghelp8 = new String[]{"/[label()]", "What should I do? My son...my son's run away from home! If I don't find him, those monsters will eat him!", "/[waitkey(64)]/[close()]"};
    String[] msghelp81 = new String[]{"/[label()]", "What?! You'll look for him? Oh, thank goodness. I'm counting on you! Please save him!", "/[waitkey(64)]/[close()]"};
    String[] msghelp82 = new String[]{"/[label()]", "What did you say?! You found him? Oh, I'm so relieved. Thank you!", "/[waitkey(64)]/[close()]"};
    String[] msg00180545 = new String[]{"/[label()]", "(Information on son)", "/[waitkey(1)]/[clear()]", "", "//Part-time？", "/[waitkey(64)]/[close()]"};
    String[] msg00180547 = new String[0];
    String[] sub_01 = new String[]{"Discovered Segment Address No. 3.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 3.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 3, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] FISH_GET = new String[]{"/[label()]", "There's something in the pocket.", "/[waitkey(64)]/[close()]"};
    String[] msgPART_LOOK = new String[]{"/[label()]", "Aaaah! No! Stay away!!", "/[waitkey(64)]/[close()]"};
    String[] msg0help2 = new String[]{"/[label()]", "13 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg1help2 = new String[]{"/[label()]", "12 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg2help2 = new String[]{"/[label()]", "11 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg3help2 = new String[]{"/[label()]", "10 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg4help2 = new String[]{"/[label()]", "9 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg5help2 = new String[]{"/[label()]", "8 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg6help2 = new String[]{"/[label()]", "7 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg7help2 = new String[]{"/[label()]", "6 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg8help2 = new String[]{"/[label()]", "5 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg9help2 = new String[]{"/[label()]", "4 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg10help2 = new String[]{"/[label()]", "3 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg11help2 = new String[]{"/[label()]", "2 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg12help2 = new String[]{"/[label()]", "1 more person left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg13helpok = new String[]{"/[label()]", "Rescued everyone!", "/[waitkey(64)]/[close()]"};

    ST2230() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.551f, 2.493f, -0.083f);
        this.camEV.setRotate(-34.169f, -52.579f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.122f, 2.319f, -23.644f);
        this.camEV.setRotate(-26.306f, 515.511f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.851f, 2.223f, -22.374f);
        this.camEV.setRotate(-27.394f, -126.276f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.47f, 2.237f, -13.899f);
        this.camEV.setRotate(-18.94f, -29.719f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        this.enemy2.kickEnepc(4, 2);
    }

    void Ene_Vfalse() {
        this.enemy1.setVisible(false);
        this.enemy2.setVisible(false);
    }

    void Ene_Vtrue() {
        this.enemy1.setVisible(true);
        this.enemy2.setVisible(true);
    }

    public void HashigoTop(int n) {
        if (n == 0) {
            Runtime.setPlayerControl(false);
            this.fade.call(0);
            System.sleep(30);
            System.println("襲撃中外観街１・１５");
            Runtime.jumpCF(67746, 15);
        }
    }

    public void KickEvent(int n, int n2) {
        block0:
        switch (n2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3203, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3203, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3223, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3283, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3283, 1, 1);
                }
                this.BUTTON_F = 0;
                Runtime.setPlayerControl(true);
                return;
            }
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.checkItem(10, 71) == 0) {
                    this.Ene_Stop();
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.FISH_GET, 0);
                    ST2230.waitPage(this.win, 64);
                    this.menu = Menu.create();
                    this.menu.addItem("Take it\nDon't take it");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Sound.effectPlay(6);
                            Runtime.addItemWin(10, 71);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            this.Ene_Restart();
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    this.Ene_Restart();
                    break;
                }
                this.BUTTON_F = 0;
                this.Ene_Restart();
                break;
            }
            case 2: {
                if (this.PART_LOOK == 1) {
                    return;
                }
                if (Runtime.getFlags(7116, 1) == 1) {
                    return;
                }
                this.enemy1.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.player.look_char(this.npc1);
                this.cam0.setMode(-1);
                this.EV_Camera04();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgPART_LOOK, 0);
                ST2230.waitPage(this.win, 64);
                System.sleep(15);
                this.cam0.setMode(0);
                this.player.look_default();
                Runtime.setPlayerControl(true);
                ++this.PART_LOOK;
                this.enemy1.kickEnepc(4, 0);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        this.Ene_Vfalse();
        Runtime.enable(65536);
        this.enemy2.setTranslate(100.0f, 0.0f, 100.0f);
        this.player.setTranslate(8.39f, 0.0f, -20.55f);
        this.player.rotY(1, 120.0f, true);
        System.sleep(2);
        this.npc1.kickEnepc(1, 10);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        System.sleep(10);
        this.player.look_char(this.npc1);
        this.npc1.look_char(this.player);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp7, 0);
        ST2230.waitPage(this.win, 64);
        this.npc1.look_default();
        this.npc1.kickEnepc(1, 3);
        this.npc1.moveEnepc(17, 270.0f, -0.1f, 5);
        System.sleep(3);
        this.npc1.moveEnepc(15, 5.7f, -21.36f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.Ene_Vtrue();
        this.enemy2.setTranslate(7.0f, 0.0f, -20.9f);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        ++this.count;
        Runtime.setFlags(7092, 4, this.count);
        Runtime.setFlags(7116, 1, 1);
        this.countdown();
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc) {
        this.Ene_Stop();
        this.Ene_Vfalse();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(5.69f, 0.0f, -1.88f);
        this.player.rotY(1, 60.0f, true);
        System.sleep(2);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.player.look_char(this.npc2);
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp8, 0);
        ST2230.waitPage(this.win, 64);
        this.npc2.look_char(this.player);
        this.npc2.moveEnepc(17, 270.0f, -0.1f, 5);
        this.npc2.kickEnepc(1, 9);
        if (Runtime.getFlags(7115, 1) == 0) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp81, 0);
            ST2230.waitPage(this.win, 64);
        } else {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp82, 0);
            ST2230.waitPage(this.win, 64);
        }
        this.npc2.look_default();
        this.npc2.kickEnepc(9, -1);
        this.npc2.kickEnepc(1, 3);
        this.npc2.moveEnepc(17, 180.0f, -0.1f, 10);
        System.sleep(8);
        this.npc2.moveEnepc(15, 5.8f, -7.36f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(65536);
        this.npc2.setVisible(false);
        ++this.count;
        Runtime.setFlags(7092, 4, this.count);
        Runtime.setFlags(7117, 1, 1);
        this.player.setID(1);
        this.countdown();
        this.Ene_Vtrue();
        this.Ene_Restart();
    }

    void countdown() {
        if (Runtime.getFlags(7092, 4) == 14) {
            return;
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        switch (Runtime.getFlags(7092, 4)) {
            case 0: {
                this.win.print(this.msg0help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2230.waitPage(this.win, 64);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外概観2・5");
                Runtime.jumpCF(67746, 5);
                break;
            }
            case 1: {
                System.println("外概観2・11");
                Runtime.jumpCF(67746, 11);
                break;
            }
            case 2: {
                System.println("外概観2・13");
                Runtime.jumpCF(67746, 13);
                break;
            }
            case 3: {
                System.println("サブルート MC_KUK15");
                Runtime.jumpCF(2150, 1);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(1, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 0.5f, 1.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightVec(4, 1, -0.075f, 1.0f, 0.075f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 1, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 2, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 3, 0.4f, 0.375f, 0.375f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, -0.075f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        new Uwamono(28672, 2.8f, -1.0f, -23.4f, 0.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 266);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 267);
        if (Runtime.getFlags(3203, 1) == 0) {
            new Uwamono(2, 31);
        } else {
            Stage.setVisible(2, false);
        }
        new Uwamono(1, 31, this.item01);
        new Uwamono(0, 31);
        new Uwamono(47, 32, this.item02);
        this.HASIGO = new NPC_NORMAL(1571, 20, 0, 0, 3, 100.0f, 0.0f, 100.0f, 0.0f);
        this.HASIGO.setInvalidID(1);
        this.HASIGO.kickEnepc(4, 2);
        this.HASIGO.setVisible(false);
        this.HASIGO.dispRadar(false);
        this.HASIGO.disableDTKFlag(131072);
        this.HASIGO.disableDTKFlag(65536);
        this.HASIGO.kickEnepc(19, 1, 0, 280, 1);
        this.doorA = new Uwamono(71, 40, '\u0004');
        if (Runtime.getFlags(3283, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        this.doorA.SetDiffSize(0.0f, -0.27f, 0.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, -25.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFPedestal(4, 3.095878f, 4.7519803f, -8.383345f, 51.079285f, -12.538941f, 329.77567f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(7, 0.02f, 0.02f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(12, 0.015f, 0.015f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(13, 0.02f, 0.02f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(14, 100.0f, 100.0f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 3.0f, 45.0f);
        this.cam0.setCFHokan(15, 0.02f, 0.02f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.Atari = new Uwamono(28672, 1.84f, 1.54f, -16.11f, 0.0f);
        this.Atari.SetSize(2.0f, 1.0f, 2.0f);
        if (Runtime.getFlags(7116, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1571, 1, 0, 14, 5, 9.0f, 0.0f, -21.04f, 300.0f);
            this.npc1.disableDTKFlag(3);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setMotion(0, 27);
            this.npc1.talkto("Talk_npc1");
        }
        if (Runtime.getFlags(7117, 1) == 0) {
            this.npc2 = new NPC_NORMAL(1605, 2, 0, 14, 3, 6.55f, 0.0f, -1.41f, 300.0f);
            this.npc2.disableDTKFlag(3);
            this.npc2.enableDTKFlag(262144);
            this.npc2.setMotion(0, 10);
            this.npc2.talkto("Talk_npc2");
        }
        this.fire01 = new Effect(1402, 1.9f, 0.0f, -17.0f, 0.0f);
        this.fire01.setScale(1.3f, 1.3f, 1.3f);
        this.fire02 = new Effect(1402, 1.9f, 0.0f, -16.1f, 0.0f);
        this.fire03 = new Effect(1402, 3.9f, 0.0f, -11.1f, 0.0f);
        this.fire03.setScale(0.5f, 0.5f, 0.5f);
        this.fire04 = new Effect(1402, 12.8f, 3.5f, -18.8f, 0.0f);
        this.fire04.setScale(0.8f, 0.8f, 0.8f);
        Runtime.progressEffect(30);
        float[] fArray = new float[12];
        fArray[0] = 4.8f;
        fArray[2] = -14.1f;
        fArray[3] = -1.0f;
        fArray[4] = 4.3f;
        fArray[6] = -14.1f;
        fArray[8] = 5.3f;
        fArray[10] = -14.1f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16392, 11, 1, 115, 9, 4.8f, 0.0f, -14.1f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[8];
        fArray3[0] = 7.7f;
        fArray3[2] = -20.9f;
        fArray3[3] = -1.0f;
        fArray3[4] = 7.0f;
        fArray3[6] = -20.9f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16392, 12, 2, 137, 9, 7.7f, 0.0f, -20.9f, 90.0f, fArray4);
        this.enemy2.setGroup(0, 0, 0, 0);
        this.enemy2.setMotion(0, 27);
        new Uwamono(28674, 3.1f, 0.0f, -11.8f, 0.0f);
        this.teiten23 = new Uwamono(28690, 1.9f, 0.0f, -16.55f, 0.0f);
        this.teiten23.SetBgm(196614);
        this.teiten4 = new Uwamono(28690, 5.0f, 0.0f, -11.1f, 0.0f);
        this.teiten4.SetBgm(196615);
        this.teiten5 = new Uwamono(28690, 15.0f, 3.6f, -19.0f, 0.0f);
        this.teiten5.SetBgm(196615);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2230.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2230.waitPage(this.win, 64);
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
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
}

