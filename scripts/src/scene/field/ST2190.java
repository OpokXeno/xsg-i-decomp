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
import xeno.map.MC_KUK05B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2190
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK05B_PRJ {
    int WALL = Runtime.getFlags(6043, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    int selected1 = 0;
    int selected2 = 0;
    int selected3 = 0;
    int selected4 = 0;
    Uwamono doorA;
    Uwamono col;
    Uwamono Atari;
    Unit tobira;
    Enepc enemy1;
    Enepc enemy2;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc20;
    boolean WALL_FLAG = false;
    Light light = new Light(0);
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Uwamono co01;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono itembox;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    int page;
    String[] msghelp_oji = new String[]{"/[label()]", "M-my treasure's inside the safe...the code...is...\n", "Photo...birth...day...", "/[waitkey(64)]/[close()]"};
    String[] msghelp_oji2 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] msghelp4 = new String[]{"/[label()]", "Waaah, it was so scary!", "/[waitkey(1)]/[clear()]", "The Gnosis...the Gnosis...killed my Dad!", "/[waitkey(64)]/[clear()]"};
    String[] msghelp41 = new String[]{"/[label()]", "Waaah!", "/[wait(30)]/[close()]"};
    String[] Door_01 = new String[]{"Please enter your P.I.N.", "/[waitkey(64)]/[close()]"};
    String[] Door_02 = new String[]{"The number is not valid.\n", "Please try again.", "/[waitkey(64)]/[close()]"};
    String[] Door_03 = new String[]{"Entry confirmed.\n", "Please enter the second digit.", "/[waitkey(64)]/[close()]"};
    String[] Door_04 = new String[]{"Entry confirmed.\n", "Please enter the third digit.", "/[waitkey(64)]/[close()]"};
    String[] Door_05 = new String[]{"Entry confirmed.\n", "Please enter the fourth digit.", "/[waitkey(64)]/[close()]"};
    String[] Door_06 = new String[]{"P.I.N. has been confirmed.\n", "Releasing lock.", "/[waitkey(64)]/[close()]"};
    String[] msg0help2 = new String[]{"/[label()]", "13 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg1help2 = new String[]{"/[label()]", "12 more people let to go!", "/[waitkey(64)]/[close()]"};
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
    String[] msg13helpok = new String[]{"/[label()]", "Everyone has been rescued!", "/[waitkey(64)]/[close()]"};

    ST2190() {
    }

    void EOB(int n) {
        if (n == 11) {
            Runtime.setFlags(8103, 1, 1);
        }
        if (Runtime.getFlags(7111, 1) == 1) {
            System.println("おやじ死亡中");
            this.npc1.kickEnepc(4, 1);
            this.npc1.kickEnepc(3, 28, 10, 10, 1, 100);
        }
    }

    void EOB_Always(int n) {
        if (Runtime.getFlags(7111, 1) == 1) {
            System.println("おやじ死亡中");
            this.npc1.kickEnepc(4, 1);
            this.npc1.kickEnepc(3, 28, 10, 10, 1, 100);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.92f, 1.523f, 2.167f);
        this.camEV.setRotate(-17.091f, 24.519f, 0.0f);
        this.camEV.setFov(47.499f);
        this.camEV.change();
    }

    void Ene_Restart() {
        if (Runtime.getFlags(8103, 1) == 0) {
            this.enemy1.kickEnepc(4, 0);
        }
    }

    void Ene_Stop() {
        if (Runtime.getFlags(8103, 1) == 0) {
            this.enemy1.kickEnepc(4, 2);
        }
    }

    void Ene_Vfalse() {
        if (Runtime.getFlags(8103, 1) == 0) {
            this.enemy1.setVisible(false);
        }
    }

    void Ene_Vtrue() {
        if (Runtime.getFlags(8103, 1) == 0) {
            this.enemy1.setVisible(true);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 1: {
                if (Runtime.getFlags(8103, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(7112, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7121, 1) == 0) {
                    if (this.BUTTON_F == 1) {
                        return;
                    }
                    this.BUTTON_F = 1;
                    this.Ene_Stop();
                    System.sleep(1);
                    Runtime.disable(524288);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.Door_01, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
                    System.waitFor(this.menu);
                    this.selected1 = this.menu.getSelected();
                    if (this.selected1 == 1) {
                        System.sleep(20);
                        Sound.effectPlay(10);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.Door_03, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
                        System.waitFor(this.menu);
                        this.selected2 = this.menu.getSelected();
                        if (this.selected2 == 0) {
                            System.sleep(20);
                            Sound.effectPlay(10);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 15);
                            this.win.print(this.Door_04, 0);
                            System.waitFor(this.win);
                            this.menu = Menu.create();
                            this.menu.addItem("0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
                            System.waitFor(this.menu);
                            this.selected3 = this.menu.getSelected();
                            if (this.selected3 == 2) {
                                System.sleep(20);
                                Sound.effectPlay(10);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.Door_05, 0);
                                System.waitFor(this.win);
                                this.menu = Menu.create();
                                this.menu.addItem("0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
                                System.waitFor(this.menu);
                                this.selected4 = this.menu.getSelected();
                                if (this.selected4 == 8) {
                                    System.sleep(20);
                                    Sound.effectPlay(11);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 15);
                                    this.win.print(this.Door_06, 0);
                                    System.waitFor(this.win);
                                    Runtime.setPlayerControl(true);
                                    this.BUTTON_F = 0;
                                    Runtime.setFlags(7121, 1, 1);
                                    Runtime.enable(524288);
                                    this.Ene_Restart();
                                    break;
                                }
                                System.sleep(20);
                                Sound.effectPlay(5);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.Door_02, 0);
                                System.waitFor(this.win);
                                this.BUTTON_F = 0;
                                Runtime.setPlayerControl(true);
                                System.println("1111111111");
                                Runtime.enable(524288);
                                this.Ene_Restart();
                                break;
                            }
                            System.sleep(20);
                            Sound.effectPlay(5);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 15);
                            this.win.print(this.Door_02, 0);
                            System.waitFor(this.win);
                            this.BUTTON_F = 0;
                            Runtime.setPlayerControl(true);
                            System.println("2222222222");
                            Runtime.enable(524288);
                            this.Ene_Restart();
                            break;
                        }
                        System.sleep(20);
                        Sound.effectPlay(5);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.Door_02, 0);
                        System.waitFor(this.win);
                        this.BUTTON_F = 0;
                        Runtime.setPlayerControl(true);
                        System.println("3333333333");
                        Runtime.enable(524288);
                        this.Ene_Restart();
                        break;
                    }
                    System.sleep(20);
                    Sound.effectPlay(5);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.Door_02, 0);
                    System.waitFor(this.win);
                    this.BUTTON_F = 0;
                    Runtime.setPlayerControl(true);
                    System.println("4444444444");
                    Runtime.enable(524288);
                    this.Ene_Restart();
                    break;
                }
                this.Ene_Stop();
                System.sleep(1);
                Runtime.disable(524288);
                Runtime.setPlayerControl(false);
                this.npc2.kickEnepc(4, 1);
                this.npc2.kickEnepc(1, 27);
                this.fade.call(0);
                System.sleep(28);
                this.tobira.setRotate(0.0f, 220.0f, 0.0f);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.player.setTranslate(-9.28f, 0.0f, 0.5f);
                System.sleep(2);
                System.sleep(30);
                this.player.look_char(this.npc2);
                this.npc2.look_char(this.player);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msghelp4, 0);
                ST2190.waitPage(this.win, 64);
                this.npc2.look_default();
                this.npc2.kickEnepc(9, -1);
                this.npc2.kickEnepc(1, 3);
                System.sleep(10);
                this.npc2.moveEnepc(15, -8.42f, 3.0f, 40);
                this.win.print(this.msghelp41, 0);
                System.sleep(50);
                this.fade.call(0);
                System.sleep(30);
                this.cam0.setMode(0);
                this.player.look_default();
                this.npc2.disableDTKFlag(131072);
                this.npc2.disableDTKFlag(65536);
                this.npc2.setVisible(false);
                ++this.count;
                Runtime.setFlags(7112, 1, 1);
                Runtime.setFlags(7092, 4, this.count);
                this.Atari.SetHitKind('\u0000');
                this.player.setID(2);
                this.countdown();
                System.sleep(15);
                Runtime.enable(524288);
                Runtime.setPlayerControl(true);
                this.Ene_Restart();
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Ene_Stop();
        if (Runtime.getFlags(7111, 1) == 0) {
            window.print(this.msghelp_oji, 0);
            ST2190.waitPage(window, 64);
            this.npc1.kickEnepc(1, 28);
            System.sleep(10);
            this.npc1.kickEnepc(4, 1);
            this.npc1.kickEnepc(3, 28, 10, 10, 1, 100);
            Runtime.setFlags(7111, 1, 1);
            this.Ene_Restart();
            return;
        }
        window.print(this.msghelp_oji2, 0);
        ST2190.waitPage(window, 64);
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc) {
        this.Ene_Stop();
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(-9.27f, 0.13f, 0.5f);
        this.player.rotY(1, 170.0f, true);
        System.sleep(2);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp4, 0);
        ST2190.waitPage(this.win, 64);
        this.win.print(this.msghelp41, 0);
        ST2190.waitPage(this.win, 64);
        this.npc2.kickEnepc(9, -1);
        this.npc2.kickEnepc(1, 3);
        System.sleep(20);
        this.npc2.moveEnepc(15, -8.42f, 3.0f, 40);
        System.sleep(50);
        this.cam0.setMode(0);
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(65536);
        this.npc2.setVisible(false);
        ++this.count;
        Runtime.setFlags(7112, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
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
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2190.waitPage(this.win, 64);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後外概観１・３");
                Runtime.jumpCF(67706, 3);
                break;
            }
            case 1: {
                System.println("襲撃後宿屋２Ｆ・１");
                Runtime.jumpCF(67736, 1);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(2, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.15f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, -0.15f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.Atari = new Uwamono(28672, -9.08f, 0.0f, -0.2f, 0.0f);
        this.Atari.SetSize(1.0f, 1.0f, 0.5f);
        if (Runtime.getFlags(7112, 1) == 0) {
            this.tobira = new Unit();
            this.tobira.mapUnit(43);
            this.tobira.start(4, null);
            this.tobira.setRotate(0.0f, 0.0f, 0.0f);
        } else {
            this.tobira = new Unit();
            this.tobira.mapUnit(43);
            this.tobira.start(4, null);
            this.tobira.setRotate(0.0f, 220.0f, 0.0f);
            this.Atari.SetHitKind('\u0000');
            this.player.setID(2);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 47.5f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 47.5f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 7.5f, 47.5f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 5.5f, 47.5f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 7.0f, 47.5f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
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
        this.itembox = new Uwamono(28677, -2.49f, 0.0f, -3.98f, 180.0f, 244);
        this.itembox.SetSymbol(28684);
        new Uwamono(28678, 9.0f, 0.0f, 3.9f);
        if (Runtime.getFlags(7111, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1614, 1, 0, 14, 7, 0.79f, 0.0f, 2.03f, 0.0f);
            this.npc1.setMotion(0, 27);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(65536);
            this.npc1.disableDTKFlag(2);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setInvalidID(1);
            this.npc20 = new NPC_NORMAL(1614, 20, 0, 14, 7, 0.79f, 0.0f, 2.03f, 0.0f);
            this.npc20.enableDTKFlag(262144);
            this.npc20.talkto("Talk_npc1");
            this.npc20.setVisible(false);
        } else {
            this.npc1 = new NPC_NORMAL(1614, 1, 0, 14, 7, 0.79f, 0.0f, 2.03f, 0.0f);
            this.npc1.setMotion(0, 27);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(65536);
            this.npc1.disableDTKFlag(2);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setInvalidID(1);
            this.npc1.kickEnepc(4, 1);
            this.npc1.kickEnepc(3, 28, 10, 10, 1, 100);
            this.npc20 = new NPC_NORMAL(1614, 20, 0, 14, 7, 0.79f, 0.0f, 2.03f, 0.0f);
            this.npc20.enableDTKFlag(262144);
            this.npc20.talkto("Talk_npc1");
            this.npc20.setVisible(false);
        }
        if (Runtime.getFlags(7112, 1) == 0) {
            this.npc2 = new NPC_NORMAL(1607, 2, 0, 14, 5, -8.98f, 0.2f, -0.73f, 0.0f);
            this.npc2.setInvalidID(1);
            this.npc2.disableDTKFlag(3);
            this.npc2.enableDTKFlag(262144);
            this.npc2.setMotion(0, 27);
            this.npc2.talkto("Talk_npc2");
        }
        this.fire01 = new Effect(1402, 5.4f, 0.0f, 1.1f, 0.0f);
        this.fire01.setScale(0.5f, 0.5f, 0.5f);
        this.fire03 = new Effect(1402, -5.3f, 0.0f, -3.2f, 0.0f);
        this.fire04 = new Effect(1402, -6.5f, 0.0f, 1.0f, 0.0f);
        this.fire04.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(30);
        this.co01 = new Uwamono(28672, 5.4f, 0.0f, 1.1f, 0.0f);
        this.co01.SetHitKind('\u0001');
        this.co01.SetSize(0.5f, 1.0f, 0.5f);
        if (Runtime.getFlags(8103, 1) == 0) {
            float[] fArray = new float[12];
            fArray[0] = -9.0f;
            fArray[2] = 1.5f;
            fArray[3] = -1.0f;
            fArray[4] = -8.5f;
            fArray[6] = 1.5f;
            fArray[8] = -9.0f;
            fArray[10] = 2.0f;
            float[] fArray2 = fArray;
            this.enemy1 = new NpcEnemy(16390, 11, 1, 102, 11, -9.0f, 0.0f, 1.5f, 0.0f, fArray2);
            this.enemy1.setGroup(0, 0, 1, 1);
        }
        this.teiten1 = new Uwamono(28690, 5.4f, 0.0f, 1.1f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten3 = new Uwamono(28690, -5.3f, 0.0f, -3.2f, 0.0f);
        this.teiten3.SetBgm(196614);
        this.teiten4 = new Uwamono(28690, -6.5f, 0.0f, 1.0f, 0.0f);
        this.teiten4.SetBgm(196615);
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

        void si() {
            ST2190.this.npc1.kickEnepc(0, 27);
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

