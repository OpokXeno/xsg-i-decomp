import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK08B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2220
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK08B_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    boolean WallFlag = false;
    Uwamono doorA;
    Uwamono col;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    boolean EneterCheck = false;
    Light light = new Light(0);
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Uwamono co01;
    Uwamono co02;
    Uwamono co03;
    Uwamono co04;
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
    Effect fire;
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
    Uwamono teiten;
    int page;
    String[] NPC_TALK1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] msghelp9 = new String[]{"/[label()]", "...", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(64)]/[close()]"};
    String[] msghelp91 = new String[]{"/[label()]", "...Thanks, you saved me.\n", "/[waitkey(64)]/[close()]"};
    String[] msghelp92 = new String[]{"/[label()]", "G-go back there to escape. T-there's a shortcut to get outside...", "/[waitkey(64)]/[close()]"};
    String[] msgoyaji = new String[]{"/[label()]", "Stay away! Damn it, I won't let you bastards destroy this shop! This place is full of children's dreams!\n", "/[waitkey(64)]/[close()]"};
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

    ST2220() {
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.181f, 1.487f, -7.583f);
        this.camEV.setRotate(-2.594f, 108.575f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.043f, 2.287f, -7.188f);
        this.camEV.setRotate(-23.751f, 54.119f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(15.719f, 1.663f, -6.687f);
        this.camEV.setRotate(-9.566f, 77.639f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        this.enemy2.kickEnepc(4, 2);
        this.enemy3.kickEnepc(4, 2);
    }

    void Ene_Vfalse() {
        this.enemy1.setVisible(false);
        this.enemy2.setVisible(false);
        this.enemy3.setVisible(false);
    }

    void Ene_Vtrue() {
        this.enemy1.setVisible(true);
        this.enemy2.setVisible(true);
        this.enemy3.setVisible(true);
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(7118, 1) == 0) {
                    this.enemy1.kickEnepc(4, 2);
                    this.enemy3.kickEnepc(4, 2);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_Camera03();
                    Runtime.enable(65536);
                    this.player.setTranslate(15.64f, 0.0f, -6.97f);
                    System.sleep(10);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgoyaji, 0);
                    ST2220.waitPage(this.win, 64);
                    System.sleep(30);
                    Runtime.disable(65536);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    this.enemy1.kickEnepc(4, 0);
                    this.enemy3.kickEnepc(4, 0);
                    break;
                }
                this.enemy1.kickEnepc(4, 2);
                this.enemy3.kickEnepc(4, 2);
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                Runtime.enable(65536);
                this.player.setTranslate(15.64f, 0.0f, -6.97f);
                System.sleep(120);
                Runtime.disable(65536);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                this.enemy1.kickEnepc(4, 0);
                this.enemy3.kickEnepc(4, 0);
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(9.4f, 0.0f, -8.7f);
        this.player.rotY(1, 160.0f, true);
        System.sleep(2);
        this.Ene_Vfalse();
        this.cam0.setMode(-1);
        this.EV_Camera02();
        this.player.look_char(this.npc1);
        this.npc1.look_char(this.player);
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp9, 0);
        ST2220.waitPage(this.win, 64);
        this.npc1.kickEnepc(1, 0);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp91, 0);
        ST2220.waitPage(this.win, 64);
        this.npc1.look_default();
        this.npc1.kickEnepc(9, -1);
        this.npc1.kickEnepc(1, 1);
        this.npc1.moveEnepc(17, 20.0f, -0.1f, 5);
        System.sleep(5);
        this.npc1.moveEnepc(15, 11.51f, -6.36f, 80);
        System.sleep(90);
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp92, 0);
        ST2220.waitPage(this.win, 64);
        this.fade.call(0);
        System.sleep(30);
        this.Ene_Vtrue();
        this.cam0.setMode(0);
        this.player.look_default();
        ++this.count;
        Runtime.setFlags(7118, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.player.setID(1);
        this.countdown();
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc, Window window) {
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
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2220.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2220.waitPage(this.win, 64);
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
                System.println("襲撃後・外概観８");
                Runtime.jumpCF(67746, 8);
                break;
            }
            case 1: {
                System.println("襲撃後・外概観１０");
                Runtime.jumpCF(67746, 10);
                break;
            }
            case 2: {
                System.println("襲撃後・外概観９");
                Runtime.jumpCF(67746, 9);
                break;
            }
            case 3: {
                System.println("襲撃後・外概観６");
                Runtime.jumpCF(67746, 6);
                break;
            }
            case 4: {
                System.println("襲撃後・外概観７");
                Runtime.jumpCF(67746, 7);
                break;
            }
            case 5: {
                System.println("襲撃後外概観１６");
                Runtime.jumpCF(67746, 16);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
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
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, -0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(3, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, -0.075f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.015f, 0.015f);
        this.cam0.setCFLockX(9, 6.5f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 4.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.015f, 0.015f);
        this.cam0.setCFLockX(10, 6.5f);
        this.cam0.setCFAngle(11, -28.0f, 25.0f, 0.0f, 6.5f, 40.0f);
        this.cam0.setCFHokan(11, 0.015f, 0.015f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.015f, 0.015f);
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
        this.item01 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 264);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 265);
        new Uwamono(26, 67);
        new Uwamono(27, 77, this.item01);
        new Uwamono(28, 77);
        new Uwamono(29, 78, this.item02);
        if (Runtime.getFlags(7118, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1597, 1, 0, 14, 3, 9.38f, 0.0f, -9.74f, 45.0f);
            this.npc1.disableDTKFlag(3);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setInvalidID(1);
            this.npc1.setMotion(0, 27);
            this.npc1.enableDTKFlag(4);
            this.npc1.talkto("Talk_npc1");
            this.player.setID(2);
        }
        this.light01 = new Effect(1405, 17.8f, 2.1f, -19.5f, 0.0f);
        this.light02 = new Effect(1405, 13.7f, 2.1f, -21.8f, 0.0f);
        this.light03 = new Effect(1405, 11.5f, 2.1f, -21.8f, 0.0f);
        this.light04 = new Effect(1405, 9.3f, 2.1f, -21.8f, 0.0f);
        this.light05 = new Effect(1405, 5.2f, 2.1f, -21.0f, 0.0f);
        this.light06 = new Effect(1405, 5.2f, 2.1f, -16.0f, 0.0f);
        this.light07 = new Effect(1405, 5.2f, 2.1f, -12.0f, 0.0f);
        this.light08 = new Effect(1405, 5.2f, 2.1f, -7.0f, 0.0f);
        this.light09 = new Effect(1405, 2.3f, 2.1f, -5.5f, 0.0f);
        this.light10 = new Effect(1405, 0.2f, 2.1f, -1.0f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.5f, 0.5f, 0.5f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.5f, 0.5f, 0.5f);
        this.light05.setScale(0.5f, 0.5f, 0.5f);
        this.light06.setScale(0.5f, 0.5f, 0.5f);
        this.light07.setScale(0.5f, 0.5f, 0.5f);
        this.light08.setScale(0.5f, 0.5f, 0.5f);
        this.light09.setScale(0.5f, 0.5f, 0.5f);
        this.light10.setScale(0.5f, 0.5f, 0.5f);
        this.fire01 = new Effect(1402, 1.9f, 0.0f, -10.8f, 0.0f);
        this.fire02 = new Effect(1402, 0.9f, 0.0f, -14.8f, 0.0f);
        this.fire03 = new Effect(1402, 2.9f, 0.0f, -18.0f, 0.0f);
        this.fire03.setScale(0.7f, 0.7f, 0.7f);
        this.fire04 = new Effect(1402, 18.8f, 0.0f, -16.0f, 0.0f);
        this.fire04.setScale(0.5f, 0.5f, 0.5f);
        this.fire05 = new Effect(1402, 21.0f, 0.0f, -14.9f, 0.0f);
        this.fire05.setScale(0.8f, 0.8f, 0.8f);
        Runtime.progressEffect(30);
        this.co01 = new Uwamono(28672, 1.9f, 0.0f, -10.8f, 0.0f);
        this.co01.SetHitKind('\u0001');
        this.co01.SetSize(1.5f, 1.0f, 1.5f);
        this.co02 = new Uwamono(28672, 0.9f, 0.0f, -14.8f, 0.0f);
        this.co02.SetHitKind('\u0001');
        this.co02.SetSize(1.5f, 1.0f, 1.5f);
        this.co03 = new Uwamono(28672, 2.9f, 0.0f, -18.0f, 0.0f);
        this.co03.SetHitKind('\u0001');
        this.co03.SetSize(1.5f, 1.0f, 1.5f);
        this.co04 = new Uwamono(28672, 21.0f, 0.0f, -14.9f, 0.0f);
        this.co04.SetHitKind('\u0001');
        this.co04.SetSize(1.5f, 1.0f, 1.5f);
        this.teiten1 = new Uwamono(28690, 1.9f, 0.0f, -10.8f, 0.0f);
        this.teiten1.SetBgm(196614);
        this.teiten2 = new Uwamono(28690, 0.9f, 0.0f, -14.8f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 2.9f, 0.0f, -18.0f, 0.0f);
        this.teiten3.SetBgm(196615);
        this.teiten4 = new Uwamono(28690, 18.8f, 0.0f, -16.0f, 0.0f);
        this.teiten4.SetBgm(196615);
        this.teiten5 = new Uwamono(28690, 21.0f, 0.0f, -14.9f, 0.0f);
        this.teiten5.SetBgm(196614);
        float[] fArray = new float[12];
        fArray[0] = 6.5f;
        fArray[2] = -14.2f;
        fArray[3] = -1.0f;
        fArray[4] = 6.5f;
        fArray[6] = -14.7f;
        fArray[8] = 6.5f;
        fArray[10] = -13.7f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16390, 11, 1, 140, 7, 6.5f, 0.0f, -14.2f, 0.0f, fArray2);
        this.enemy1.setGroup(3, 3, 3, 3);
        this.enemy1.kickEnepc(10, 70, 0);
        float[] fArray3 = new float[12];
        fArray3[0] = 11.25f;
        fArray3[2] = -7.5f;
        fArray3[3] = -1.0f;
        fArray3[4] = 11.25f;
        fArray3[6] = -7.7f;
        fArray3[8] = 11.25f;
        fArray3[10] = -7.3f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16401, 12, 2, 136, 9, 11.25f, 0.0f, -7.5f, 225.0f, fArray4);
        this.enemy2.setGroup(0, 0, 0, 0);
        this.enemy2.kickEnepc(10, 70, 0);
        this.enemy2.setMotion(0, 27);
        float[] fArray5 = new float[12];
        fArray5[0] = 18.0f;
        fArray5[2] = -6.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = 18.0f;
        fArray5[6] = -6.5f;
        fArray5[8] = 18.0f;
        fArray5[10] = -5.5f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16402, 13, 3, 122, 11, 18.0f, 0.0f, -6.0f, 0.0f, fArray6);
        this.enemy3.setGroup(1, 1, 2, 2);
        new Uwamono(28673, 20.0f, 0.0f, -4.3f, 0.0f);
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

