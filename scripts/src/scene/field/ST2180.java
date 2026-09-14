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
import xeno.map.MC_KUK04B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2180
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK04B_PRJ {
    int SAKU = Runtime.getFlags(6041, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    boolean EnterCheck = false;
    Enepc npc1;
    Enepc npc2;
    Unit elv;
    Uwamono K1;
    Uwamono K2;
    Uwamono K3;
    Uwamono K4;
    Uwamono K5;
    Uwamono K6;
    Uwamono K7;
    Uwamono K8;
    Light light = new Light(0);
    Effect button_red;
    Effect button_blue;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    Camera cam1;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc boss;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Uwamono co01;
    Uwamono co02;
    Uwamono co03;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    int page;
    String[] NPC1_TALK1 = new String[]{"Where would it be good?\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"There is a switch.", "/[waitkey(64)]/[close()]"};
    String[] Q2 = new String[]{"Will you fight?", "/[waitkey(64)]/[close()]"};
    String[] msghelp2 = new String[]{"/[label()]", "...", "/[waitkey(1)]/[clear()]", "Hey, don't talk to me.", "/[waitkey(1)]/[clear()]", "The monsters will find us.", "/[waitkey(64)]/[close()]"};
    String[] msghelp21 = new String[]{"/[label()]", "Damn, you figured it out. I thought you'd never be able to tell us apart. Maybe it was a bit unlikely after all.", "/[waitkey(1)]/[clear()]", "Oops, this is no place for long conversations. Gotta hurry up and run.", "/[waitkey(64)]/[close()]"};
    String[] msghelp3 = new String[]{"/[label()]", "Aaaah, please spare me! Don't kill me! I don't taste good anyway! Please, just let me go!", "/[waitkey(64)]/[clear()]"};
    String[] msghelp31 = new String[]{"/[label()]", "What? You're not a Gnosis?", "/[waitkey(64)]/[clear()]"};
    String[] msghelp32 = new String[]{"/[label()]", "Y-you scared me!", "/[waitkey(1)]/[clear()]", "I-I'm totally fine. Well, you know, I was checking to make sure none of the guests got left behind.", "/[waitkey(1)]/[clear()]", "It looks like they all made it safely. Well, guess I'll run too now!", "/[waitkey(64)]/[close()]"};
    String[] momo_1 = new String[]{"/[label(MOMO)]", "???", "/[waitkey(64)]/[close()]"};
    String[] majyo_1 = new String[]{"/[label(Mintia)]", "Hmm..."};
    String[] majyo_2 = new String[]{"/[label(Mintia)]", "Hmm..."};
    String[] majyo_3 = new String[]{"/[label(Mintia)]", "Good, good."};
    String[] majyo_11 = new String[]{"I-I hate to admit it, but I lost. All right, I'll give you this!", "/[waitkey(64)]/[close()]"};
    String[] majyo_12 = new String[]{"Learned Tech Attack, \"Dark Scepter.\"", "/[waitkey(64)]/[close()]"};
    String[] majyo_13 = new String[]{"/[label(Mintia)]", "That works pretty well against Gnosis if you know how to use it.", "/[waitkey(1)]/[clear()]", "Next time, I'll bring my number one pet. Let's play again, MOMO.", "/[waitkey(64)]/[close()]"};
    String[] msg0help2 = new String[]{"/[label()]", "13 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg1help2 = new String[]{"/[label()]", "12 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg2help2 = new String[]{"/[label()]", "11 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg3help2 = new String[]{"/[label()]", "10 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg4help2 = new String[]{"/[label()]", "9 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg5help2 = new String[]{"/[label()]", "8 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg6help2 = new String[]{"/[label()]", "7 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg7help2 = new String[]{"/[label()]", "6 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg8help2 = new String[]{"/[label()]", "5 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg9help2 = new String[]{"/[label()]", "4 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg10help2 = new String[]{"/[label()]", "3 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg11help2 = new String[]{"/[label()]", "2 more people to go!", "/[waitkey(64)]/[close()]"};
    String[] msg12help2 = new String[]{"/[label()]", "1 more person to go!", "/[waitkey(64)]/[close()]"};
    String[] msg13helpok = new String[]{"/[label()]", "Rescued everyone!", "/[waitkey(64)]/[close()]"};

    ST2180() {
    }

    void EOB(int n) {
        if (n == 14) {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(8095, 1, 1);
            Runtime.etherTecSet(8);
            System.sleep(1);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_11, 0);
            ST2180.waitPage(this.win, 64);
            Sound.effectPlay(6);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_12, 0);
            ST2180.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_13, 0);
            ST2180.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.657f, 2.639f, 2.069f);
        this.camEV.setRotate(-14.631f, 310.714f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.528f, 2.575f, -4.239f);
        this.camEV.setRotate(-20.557f, -7.699f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.184f, 2.327f, 4.246f);
        this.camEV.setRotate(-11.045f, 10.9f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.67f, 8.279f, 2.232f);
        this.camEV.setRotate(-15.465f, -41.899f, 0.0f);
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
        block0:
        switch (n2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                System.println("aaaaaa");
                this.EnterCheck = true;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.Q1, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Press\nDon't press\n");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        System.sleep(30);
                        Sound.effectPlay(58);
                        if (Runtime.getFlags(6041, 1) == 0) {
                            this.button_red.disp(false);
                            this.button_blue.disp(true);
                            this.player.setID(2);
                            Runtime.setFlags(6041, 1, 1);
                            this.cam0.setMode(-1);
                            this.EV_Camera03();
                            System.sleep(30);
                            Sound.effectPlay(196745);
                            this.elv.setArgs(12, 6.0f);
                            System.sleep(90);
                            this.EV_Camera04();
                            System.sleep(90);
                            this.cam0.setMode(0);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            break block0;
                        }
                        this.button_blue.disp(false);
                        this.button_red.disp(true);
                        this.player.setID(1);
                        Runtime.setFlags(6041, 1, 0);
                        this.cam0.setMode(-1);
                        this.EV_Camera04();
                        System.sleep(30);
                        Sound.effectPlay(196745);
                        this.elv.setArgs(12, 0.097f);
                        System.sleep(90);
                        this.EV_Camera03();
                        System.sleep(90);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 1: {
                this.player.getTranslate();
                if (this.player.py < 6.0f) {
                    return;
                }
                if (Runtime.getLeader() != 4 || Runtime.getFlags(8095, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.EnterCheck = true;
                System.println("魔女登場");
                Runtime.enable(65536);
                Runtime.disable(524288);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.momo_1, 0);
                ST2180.waitPage(this.win, 64);
                this.player.move(60, 12.0f, 1.5f, true);
                this.player.mtn(2, 9, 1.0f, true);
                System.sleep(63);
                this.player.mtn(1, 9, 1.0f, true);
                this.player.rotY(10, 180.0f, true);
                System.sleep(10);
                this.boss.kickEnepc(4, 0);
                System.sleep(10);
                Stage.setVisible(105, true);
                this.boss.setVisible(true);
                this.cam0.setMode(-1);
                this.cam1 = Camera.create(1);
                this.cam1.setTranslate(10.6f, 7.4f, 3.3f);
                this.cam1.setRotate(20.0f, -29.3f, -180.0f);
                this.cam1.setFov(40.0f);
                this.cam1.change();
                this.boss.kickEnepc(4, 2);
                System.sleep(30);
                this.boss.kickEnepc(4, 0);
                Sound.effectPlay(196747);
                System.sleep(25);
                float[] fArray = new float[8];
                fArray[2] = 3.0f;
                fArray[3] = 10.0f;
                fArray[4] = 40.0f;
                fArray[5] = -7.9f;
                fArray[6] = 3.0f;
                fArray[7] = 6.1f;
                float[] fArray2 = fArray;
                this.cam1.setTranslate(10.6f, 7.4f, 3.3f);
                float[] fArray3 = new float[8];
                fArray3[1] = 20.0f;
                fArray3[2] = -29.3f;
                fArray3[3] = -180.0f;
                fArray3[4] = 40.0f;
                fArray3[5] = -0.5f;
                fArray3[6] = -29.3f;
                float[] fArray4 = fArray3;
                this.cam1.rotateSPL(fArray4, 0, 1, 40);
                System.sleep(200);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_1, 0);
                System.sleep(60);
                this.win.close();
                System.sleep(40);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_2, 0);
                System.sleep(60);
                this.win.close();
                System.sleep(80);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_3, 0);
                System.sleep(50);
                this.win.close();
                System.sleep(10);
                this.boss.kickEnepc(4, 2);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Q2, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo\n");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.boss.kickEnepc(14, 0);
                        Runtime.enable(524288);
                        this.cam0.setMode(0);
                        Stage.setVisible(105, false);
                        Runtime.disable(65536);
                        this.EnterCheck = false;
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(67706, 6);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Ene_Stop();
        this.Ene_Vtrue();
        this.npc1.kickEnepc(4, 1);
        this.npc1.kickEnepc(1, 27);
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(-6.43f, 0.0f, -7.77f);
        this.player.rotY(1, 160.0f, true);
        this.npc1.moveEnepc(17, 0.0f, 0.1f, 5);
        System.sleep(2);
        Runtime.disable(65536);
        this.cam0.setMode(-1);
        this.EV_Camera02();
        this.player.look_char(this.npc1);
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp3, 0);
        ST2180.waitPage(this.win, 64);
        this.npc1.look_char(this.player);
        this.win.print(this.msghelp31, 0);
        ST2180.waitPage(this.win, 64);
        this.npc1.kickEnepc(1, 9);
        this.win.print(this.msghelp32, 0);
        ST2180.waitPage(this.win, 64);
        this.npc1.look_default();
        this.npc1.disableDTKFlag(2);
        this.npc1.kickEnepc(9, -1);
        this.npc1.kickEnepc(1, 3);
        System.sleep(8);
        this.npc1.moveEnepc(15, -6.08f, -4.37f, 30);
        System.sleep(40);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        ++this.count;
        Runtime.setFlags(7110, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Vtrue();
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc) {
        this.Ene_Stop();
        this.Ene_Vtrue();
        System.sleep(1);
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msghelp2, 0);
                ST2180.waitPage(this.win, 64);
                this.Ene_Vtrue();
                this.Ene_Restart();
                return;
            }
        }
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(12.03f, 0.13f, -0.646f);
        this.player.rotY(1, 210.0f, true);
        this.npc2.moveEnepc(17, 45.0f, -0.1f, 1);
        System.sleep(2);
        Runtime.disable(65536);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.player.look_char(this.npc2);
        this.npc2.look_char(this.player);
        System.sleep(10);
        this.npc2.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp21, 0);
        ST2180.waitPage(this.win, 64);
        this.npc2.look_default();
        this.npc2.disableDTKFlag(2);
        this.npc2.kickEnepc(9, -1);
        this.npc2.kickEnepc(1, 3);
        this.npc2.moveEnepc(17, 270.0f, -0.1f, 10);
        System.sleep(8);
        this.npc2.moveEnepc(15, 9.51f, -3.35f, 20);
        System.sleep(30);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(65536);
        this.npc2.setVisible(false);
        ++this.count;
        Runtime.setFlags(7109, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Vtrue();
        this.Ene_Restart();
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                break;
            }
            case 2: {
                System.println("case2");
                this.K1.getTranslate();
                System.sleep(13);
                this.K1.SetGravity(false);
                this.K1.setTranslate(this.K1.px, 0.0f, this.K1.pz);
                break;
            }
            case 3: {
                System.println("case3");
                this.K2.getTranslate();
                System.sleep(13);
                this.K2.SetGravity(false);
                this.K2.setTranslate(this.K2.px, 0.0f, this.K2.pz);
                break;
            }
            case 4: {
                System.println("case4");
                this.K3.getTranslate();
                System.sleep(13);
                this.K3.SetGravity(false);
                this.K3.setTranslate(this.K3.px, 0.0f, this.K3.pz);
                break;
            }
            case 5: {
                System.println("case5");
                break;
            }
            case 6: {
                System.println("case6");
                this.K5.getTranslate();
                System.sleep(13);
                this.K5.SetGravity(false);
                this.K5.setTranslate(this.K5.px, 0.0f, this.K5.pz);
                break;
            }
            case 7: {
                System.println("case7");
                this.K6.getTranslate();
                System.sleep(13);
                this.K6.SetGravity(false);
                this.K6.setTranslate(this.K6.px, 0.0f, this.K6.pz);
                break;
            }
            case 8: {
                System.println("case8");
                this.K7.getTranslate();
                System.sleep(13);
                this.K7.SetGravity(false);
                this.K7.setTranslate(this.K7.px, 0.0f, this.K7.pz);
                break;
            }
        }
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
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2180.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2180.waitPage(this.win, 64);
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
                System.println("襲撃後外観街１・４");
                Runtime.jumpCF(67706, 4);
                break;
            }
            case 1: {
                System.println("襲撃後宿屋２Ｆ・４");
                Runtime.jumpCF(67736, 4);
                break;
            }
            case 2: {
                System.println("襲撃後外観街１・６");
                Runtime.jumpCF(67706, 6);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.3515f, 0.3515f, 0.35f);
        Runtime.setIdLightCol(5, 1, 0.3515f, 0.3515f, 0.35f);
        Runtime.setIdLightCol(5, 2, 0.3515f, 0.3515f, 0.35f);
        Runtime.setIdLightCol(5, 3, 0.3515f, 0.3515f, 0.35f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, -0.25f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(6041, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(157, 0.041666668f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 0.097f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(157, 0.041666668f, 6.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 6.0f);
        }
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(105, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFAngle(5, -28.0f, -12.5f, 0.0f, 5.0f, 42.5f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.0f, 42.5f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        this.button_red = new Effect(1539, -11.483f, 7.15f, -4.122f, 0.0f);
        this.button_red.setScale(0.4f, 0.25f, 1.0f);
        this.button_red.setRotate(-55.0f, 0.0f, 0.0f);
        this.button_blue = new Effect(1542, -11.483f, 7.156f, -4.122f, 0.0f);
        this.button_blue.setScale(0.4f, 0.25f, 1.0f);
        this.button_blue.setRotate(-55.0f, 0.0f, 0.0f);
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
        if (Runtime.getFlags(7110, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1598, 1, 0, 14, 3, 0.0f, 0.0f, 0.0f, 0.0f);
            this.npc1.setInvalidID(1);
            this.npc1.disableDTKFlag(3);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setMotion(0, 27);
            this.npc1.talkto("Talk_npc1");
        }
        if (Runtime.getFlags(7109, 1) == 0) {
            this.npc2 = new NPC_NORMAL(1599, 2, 0, 14, 5, 11.68f, 0.2f, -0.98f, 360.0f);
            this.npc2.setMotion(0, 28);
            this.npc2.setInvalidID(1);
            this.npc2.disableDTKFlag(3);
            this.npc2.enableDTKFlag(262144);
            this.npc2.talkto("Talk_npc2");
        }
        this.fire01 = new Effect(1402, -4.0f, 0.0f, -2.0f, 0.0f);
        this.fire01.setScale(0.5f, 0.5f, 0.5f);
        this.fire02 = new Effect(1402, -10.9f, 0.0f, 3.0f, 0.0f);
        this.fire02.setScale(0.7f, 0.7f, 0.7f);
        this.fire03 = new Effect(1402, 5.0f, 0.0f, -2.0f, 0.0f);
        this.fire04 = new Effect(1402, 4.0f, 0.0f, 4.8f, 0.0f);
        this.fire04.setScale(0.7f, 0.7f, 0.7f);
        Runtime.progressEffect(30);
        this.co01 = new Uwamono(28672, -10.9f, 0.0f, 3.0f, 0.0f);
        this.co01.SetHitKind('\u0001');
        this.co01.SetSize(1.5f, 1.0f, 1.5f);
        this.co03 = new Uwamono(28672, 4.0f, 0.0f, 4.8f, 0.0f);
        this.co03.SetHitKind('\u0001');
        this.co03.SetSize(1.5f, 1.0f, 1.5f);
        float[] fArray = new float[12];
        fArray[0] = 9.1f;
        fArray[2] = 1.2f;
        fArray[3] = -1.0f;
        fArray[4] = 9.6f;
        fArray[6] = 1.2f;
        fArray[8] = 8.6f;
        fArray[10] = 1.2f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16388, 11, 1, 112, 10, 9.1f, 0.0f, 1.2f, 270.0f, fArray2);
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[12];
        fArray3[0] = -6.9f;
        fArray3[2] = 0.7f;
        fArray3[3] = -1.0f;
        fArray3[4] = -6.4f;
        fArray3[6] = 0.7f;
        fArray3[8] = -7.4f;
        fArray3[10] = 0.7f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16388, 12, 2, 110, 10, -6.9f, 0.0f, 0.7f, 0.0f, fArray4);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[12];
        fArray5[0] = 0.6f;
        fArray5[2] = -5.3f;
        fArray5[3] = -1.0f;
        fArray5[4] = 0.1f;
        fArray5[6] = -5.3f;
        fArray5[8] = 1.1f;
        fArray5[10] = -5.3f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16402, 13, 3, 120, 15, 0.6f, 0.0f, -5.3f, 0.0f, fArray6);
        this.enemy3.setGroup(2, 2, 2, 2);
        if (Runtime.getFlags(8095, 1) == 0) {
            this.boss = new NPC_NORMAL(20254, 14, 0, 14, 12, 0.0f, 0.0f, 0.0f, 0.0f);
            this.boss.kickEnepc(13, 0);
            this.boss.setVisible(1, false);
            this.boss.setVisible(2, false);
            this.boss.setVisible(3, false);
            this.boss.setVisible(4, false);
            this.boss.setInvalidID(1);
            this.boss.setMotion(0, 1);
            this.boss.kickEnepc(4, 2);
            this.boss.setVisible(false);
            this.boss.dispRadar(false);
            this.boss.setBatEvent(12);
            this.boss.setGroup(4);
            this.boss.disableDTKFlag(65536);
        }
        Stage.setVisible(89, false);
        Stage.setVisible(88, false);
        new Uwamono(28672, -8.2f, -1.0f, -8.6f, 0.0f);
        new Uwamono(28673, -4.6f, 0.0f, -1.2f, 0.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 255);
        this.K1 = new Uwamono(31, 48, this.item01);
        this.K1.SetGravity(true);
        this.K1.SetCallNo(1);
        this.K2 = new Uwamono(23, 48);
        this.K2.SetGravity(true);
        this.K2.SetCallNo(2);
        this.K3 = new Uwamono(20, 48);
        this.K3.SetGravity(true);
        this.K3.SetCallNo(3);
        this.K4 = new Uwamono(19, 48);
        this.K4.SetCallNo(4);
        this.K6 = new Uwamono(22, 48);
        this.K6.SetGravity(true);
        this.K6.SetCallNo(6);
        this.K7 = new Uwamono(21, 48);
        this.K7.SetGravity(true);
        this.K7.SetCallNo(7);
        this.K8 = new Uwamono(7, 48);
        this.K8.SetCallNo(8);
        if (Runtime.getFlags(7110, 1) == 0) {
            this.K5 = new Uwamono(33, 48, this.npc1);
            this.K5.SetGravity(true);
            this.K5.SetCallNo(5);
        } else {
            this.K5 = new Uwamono(33, 48);
            this.K5.SetGravity(true);
            this.K5.SetCallNo(5);
        }
        this.itembox = new Uwamono(28677, 7.49f, 0.0f, -6.36f, 180.0f, 243);
        this.itembox.SetSymbol(28684);
        if (Runtime.getFlags(6041, 1) == 0) {
            this.button_red.disp(true);
            this.button_blue.disp(false);
            this.player.setID(1);
        } else {
            this.button_red.disp(false);
            this.button_blue.disp(true);
            this.player.setID(2);
        }
        this.teiten1 = new Uwamono(28690, -4.0f, 0.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, -10.9f, 0.0f, 3.0f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 5.0f, 0.0f, -2.0f, 0.0f);
        this.teiten3.SetBgm(196614);
        this.teiten4 = new Uwamono(28690, 4.0f, 0.0f, 4.8f, 0.0f);
        this.teiten4.SetBgm(196614);
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

