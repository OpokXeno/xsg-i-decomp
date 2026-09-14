import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK03B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST2170
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK03B_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    MAPUnit crank;
    MAPUnit yane;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int selected0 = 0;
    int selected1 = 0;
    int BUTTON_F = 0;
    int discovery = 0;
    Uwamono doorA;
    Uwamono doorB;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    Camera cam1;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc boss;
    Enepc boss2;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Effect fire06;
    Effect fire07;
    Effect fire08;
    Effect kemuri1;
    Effect kemuri2;
    Effect light01;
    Effect light02;
    Effect singouki_red;
    Uwamono co01;
    Uwamono co02;
    MAPUnit s_unit;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    MAPUnit komono;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten134;
    Uwamono teiten56;
    int CRANK = Runtime.getFlags(6042, 1);
    int A_OR_B = Runtime.getFlags(6048, 1);
    int page;
    String[] NPC_TALK1 = new String[]{"Where would it be good?\n", "/[waitkey(64)]/[close()]"};
    String[] NPC_TALK2 = new String[]{"Please do not look for me.\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Turn the crank?\n", "/[waitkey(64)]/[close()]"};
    String[] msghelp_go1 = new String[]{"/[label(Mayor)]", "There are many people in the city that haven't been able to escape yet. Please, help them!!", "/[waitkey(1)]/[clear()]", "If there's anything you need, I'll sell it to you.", "/[waitkey(64)]/[close()]"};
    String[] msghelp_go2 = new String[]{"/[label(Mayor)]", "Hey, now, there are still people in the city who haven't escaped. Please make sure to help them!", "/[waitkey(64)]/[close()]"};
    String[] msghelp1 = new String[]{"/[label()]", "You saved me! The monsters attacked so suddenly. All I could do was hide in here.", "/[waitkey(1)]/[clear()]", "I thought I was going to die here in the trash. Thanks!", "/[waitkey(64)]/[close()]"};
    String[] msgnaka_key = new String[]{"/[label()]", "It's locked from the inside.", "/[waitkey(64)]/[close()]"};
    String[] msg0help = new String[]{"/[label(Mayor)]", "There are still 13 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg1help = new String[]{"/[label(Mayor)]", "There are still 12 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg2help = new String[]{"/[label(Mayor)]", "There are still 11 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg3help = new String[]{"/[label(Mayor)]", "There are still 10 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg4help = new String[]{"/[label(Mayor)]", "There are still 9 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg5help = new String[]{"/[label(Mayor)]", "There are still 8 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg6help = new String[]{"/[label(Mayor)]", "There are still 7 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg7help = new String[]{"/[label(Mayor)]", "There are still 6 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg8help = new String[]{"/[label(Mayor)]", "There are still 5 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg9help = new String[]{"/[label(Mayor)]", "There are still 4 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg10help = new String[]{"/[label(Mayor)]", "There are still 3 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg11help = new String[]{"/[label(Mayor)]", "There are still 2 people who need to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msg12help = new String[]{"/[label(Mayor)]", "There is still 1 person who needs to be rescued! I'm counting on you!", "/[waitkey(64)]/[close()]"};
    String[] msghelp_ok = new String[]{"/[label(Mayor)]", "You were a great help. Thanks to you, everyone has managed to escape.", "/[waitkey(1)]/[clear()]", "Are you short on anything? If there's anything you need, I'll sell it to you.", "/[waitkey(64)]/[close()]"};
    String[] msghelp_ok2 = new String[]{"/[label(Mayor)]", "Thank you. I better get going soon myself.\n", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 1.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No.1.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 1, decoding complete.", "/[waitkey(64)]/[close()]"};
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

    ST2170() {
    }

    void EOB(int n) {
        if (n == 15) {
            Runtime.setFlags(8107, 1, 1);
            this.boss2.dispRadar(false);
            this.boss2.kickEnepc(4, 2);
            this.boss2.setVisible(false);
            this.boss2.kickEnepc(7, 141);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-16.485f, 1.759f, -1.019f);
        this.camEV.setRotate(-11.282f, 5.339f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.914f, 4.374f, 0.771f);
        this.camEV.setRotate(-7.642f, -50.279f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        this.enemy3.kickEnepc(4, 2);
        this.enemy4.kickEnepc(4, 2);
    }

    void Ene_Vfalse() {
        this.enemy1.setVisible(false);
        this.enemy3.setVisible(false);
        this.enemy4.setVisible(false);
    }

    void Ene_Vtrue() {
        this.enemy1.setVisible(true);
        this.enemy3.setVisible(true);
        this.enemy4.setVisible(true);
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.EnterCheck) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py > 0.5f) {
                return;
            }
            if (Runtime.getFlags(7092, 4) != 13) {
                this.Ene_Stop();
            }
            System.println("クランク");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Q1, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Turn\nDon't turn\n");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            if (this.selected == 0) {
                this.cam0.setMode(-1);
                this.EV_Camera02();
                System.sleep(30);
                float f = 1.0f;
                float f2 = 15.0f;
                float f3 = 720.0f;
                float f4 = f3 / f2;
                if (this.CRANK == 0) {
                    float f5 = 0.0f;
                    while (f5 <= f3) {
                        Sound.effectPlay(196742);
                        float f6 = 3.83f - 0.49f / f4 * f;
                        float f7 = -6.15f + 1.85f / f4 * f;
                        this.crank.setRotateY(f5);
                        this.yane.setTranslate(13.75f, f6, f7);
                        System.sleep(1);
                        f += 1.0f;
                        f5 += f2;
                    }
                    this.yane.setTranslate(13.75f, 3.34f, -4.3f);
                    this.player.setID(2);
                    Runtime.setFlags(6042, 1, 1);
                    this.CRANK = Runtime.getFlags(6042, 1);
                } else {
                    int n3 = 0;
                    while ((float) n3 <= f3) {
                        Sound.effectPlay(196742);
                        float f8 = 3.34f + 0.49f / f4 * f;
                        float f9 = -4.3f - 1.85f / f4 * f;
                        this.crank.setRotateY(-n3);
                        this.yane.setTranslate(13.75f, f8, f9);
                        System.sleep(1);
                        f += 1.0f;
                        n3 = (int) ((float) n3 + f2);
                    }
                    this.yane.setTranslate(13.75f, 3.83f, -6.15f);
                    this.player.setID(1);
                    Runtime.setFlags(6042, 1, 0);
                    this.CRANK = Runtime.getFlags(6042, 1);
                }
            }
            this.EnterCheck = false;
            System.sleep(30);
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            if (Runtime.getFlags(7092, 4) != 13) {
                this.Ene_Restart();
            }
        }
        switch (n2) {
            case 1: {
                if (Runtime.getFlags(7108, 1) == 1) {
                    return;
                }
                if (this.discovery == 0) {
                    System.println("うえあげ");
                    ++this.discovery;
                    this.npc1.setTranslate(-16.3f, 0.7f, -4.65f);
                    break;
                }
                System.println("ぬけ");
                break;
            }
            case 2: {
                if (Runtime.getFlags(7108, 1) == 1) {
                    return;
                }
                if (this.discovery == 1) {
                    System.println("したさげ");
                    --this.discovery;
                    this.npc1.setTranslate(-16.3f, 0.0f, -4.65f);
                    break;
                }
                System.println("ぬけぬけ");
                break;
            }
            case 3: {
                Runtime.setFlags(7122, 1, 1);
                this.doorB.SetDoorType('\u0004');
                this.player.getTranslate();
                if (!(this.player.py > 7.0f)) break;
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
                System.sleep(16);
                Runtime.setPlayerControl(true);
                break;
            }
            case 4: {
                if (Runtime.getFlags(7092, 4) == 13) {
                    return;
                }
                if (Runtime.getFlags(7092, 4) != 13) {
                    this.Ene_Stop();
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msghelp_go2, 0);
                ST2170.waitPage(this.win, 64);
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(30, 23.96f, 3.6f, true);
                System.sleep(35);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                if (Runtime.getFlags(7092, 4) == 13) break;
                this.Ene_Restart();
                break;
            }
            case 5: {
                if (Runtime.getFlags(7122, 1) != 0) break;
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(7092, 4) != 13) {
                    this.Ene_Stop();
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgnaka_key, 0);
                ST2170.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                if (Runtime.getFlags(7092, 4) == 13) break;
                this.Ene_Restart();
                break;
            }
            case 6: {
                return;
            }
            case 7: {
                if (Runtime.getFlags(8107, 1) == 0 && Runtime.getFlags(7092, 4) == 13) {
                    if (Runtime.getFlags(8101, 1) == 0) {
                        Runtime.setFlags(8101, 1, 1);
                        Runtime.setPlayerControl(false);
                        this.EnterCheck = true;
                        System.println("boss登場");
                        Runtime.enable(65536);
                        Runtime.disable(524288);
                        this.boss.kickEnepc(4, 0);
                        System.sleep(10);
                        this.boss.setVisible(true);
                        this.cam0.setMode(-1);
                        this.cam1 = Camera.create(1);
                        this.cam1.setTranslate(25.1f, 0.9f, 10.4f);
                        this.cam1.setRotate(9.0f, 0.0f, 0.0f);
                        this.cam1.setFov(40.0f);
                        this.cam1.change();
                        this.player.setTranslate(16.0f, 0.0f, 1.8f);
                        this.player.setRotate(0.0f, 90.0f, 0.0f);
                        this.boss.kickEnepc(4, 1);
                        this.boss.kickEnepc(3, 0, 0, 175, 1, 60);
                        Sound.streamPlay(1195013, 48000);
                        this.cam1.setTranslate(25.1f, 0.9f, 10.4f);
                        float[] fArray = new float[12];
                        fArray[1] = 9.0f;
                        fArray[4] = 30.0f;
                        fArray[5] = 9.0f;
                        fArray[8] = 290.0f;
                        fArray[9] = 9.0f;
                        fArray[10] = 30.0f;
                        float[] fArray2 = fArray;
                        this.cam1.rotateSPL(fArray2, 0, 1, 290);
                        System.sleep(290);
                        this.boss2.setVisible(true);
                        this.boss2.kickEnepc(7, 135);
                        System.sleep(1);
                        this.boss2.dispRadar(true);
                        this.boss2.kickEnepc(4, 0);
                        this.boss.setVisible(false);
                        Runtime.enable(524288);
                        Runtime.disable(65536);
                        this.EnterCheck = false;
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                    } else {
                        this.boss.kickEnepc(14, 0);
                        Runtime.setFlags(8101, 1, 1);
                    }
                }
                return;
            }
            case 8: {
                Runtime.setPlayerControl(false);
                this.EnterCheck = true;
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3201, 1) == 0) {
                    Runtime.setFlags(3201, 1, 1);
                    Sound.effectPlay(55);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3221, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3281, 1) == 0) {
                    this.nwin(this.sub_03);
                    Sound.effectPlay(56);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3281, 1, 1);
                }
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        if (Runtime.getFlags(7092, 4) != 13) {
            this.Ene_Stop();
        }
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(-16.86f, 0.0f, -4.36f);
        this.player.rotY(1, 100.0f, true);
        this.npc1.moveEnepc(17, 300.0f, -0.1f, 1);
        System.sleep(2);
        Runtime.disable(65536);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.player.look_char(this.npc1);
        this.npc1.look_char(this.player);
        System.sleep(10);
        this.npc1.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp1, 0);
        ST2170.waitPage(this.win, 64);
        this.npc1.look_default();
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.disableDTKFlag(2);
        this.npc1.kickEnepc(9, -1);
        this.npc1.kickEnepc(1, 3);
        this.npc1.moveEnepc(17, 0.0f, 0.1f, 10);
        System.sleep(8);
        this.npc1.moveEnepc(15, -16.45f, 0.6f, 30);
        System.sleep(30);
        ++this.count;
        Runtime.setFlags(7092, 4, this.count);
        Runtime.setFlags(7108, 1, 1);
        this.countdown();
        this.fade.call(0);
        System.sleep(30);
        if (Runtime.getFlags(7092, 4) == 13) {
            Runtime.jumpCF(67706, 10);
        } else {
            this.player.look_default();
            this.npc1.setVisible(false);
            this.cam0.setMode(0);
            this.Ene_Restart();
        }
    }

    public void Talk_npc2(Enepc enepc) {
        if (Runtime.getFlags(7092, 4) == 13) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp_ok, 0);
            ST2170.waitPage(this.win, 64);
            this.menu = Menu.create();
            this.menu.addItem("Can I see it?\nNo, I'm not really interested");
            System.waitFor(this.menu);
            this.selected0 = this.menu.getSelected();
            switch (this.selected0) {
                case 0: {
                    System.sleep(15);
                    Runtime.enterShop(9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msghelp_ok2, 0);
                    ST2170.waitPage(this.win, 64);
                    return;
                }
            }
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp_ok2, 0);
            ST2170.waitPage(this.win, 64);
            return;
        }
        if (Runtime.getFlags(7092, 4) != 13) {
            this.Ene_Stop();
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp_go1, 0);
        ST2170.waitPage(this.win, 64);
        this.menu = Menu.create();
        this.menu.addItem("Can I see it?\nNo, I'm not really interested");
        System.waitFor(this.menu);
        this.selected0 = this.menu.getSelected();
        switch (this.selected0) {
            case 0: {
                System.sleep(15);
                Runtime.enterShop(9);
            }
        }
        switch (Runtime.getFlags(7092, 4)) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg1help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg2help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg4help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg5help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg6help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg7help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg8help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg9help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg11help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg12help, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            default: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msghelp_go1, 0);
                ST2170.waitPage(this.win, 64);
            }
        }
        if (Runtime.getFlags(7092, 4) != 13) {
            this.Ene_Restart();
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
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2170.waitPage(this.win, 64);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後外観街２・１");
                Runtime.jumpCF(67746, 1);
                break;
            }
            case 1: {
                System.println("外マップ");
                Runtime.jumpCF(2130, 2);
                break;
            }
            case 2: {
                System.println("襲撃後宿屋１Ｆ・１");
                Runtime.jumpCF(67726, 1);
                break;
            }
            case 3: {
                System.println("襲撃後酒場１Ｆ・１");
                Runtime.jumpCF(67716, 1);
                break;
            }
            case 4: {
                System.println("襲撃後宿屋２Ｆ・２");
                Runtime.jumpCF(67736, 2);
                break;
            }
            case 5: {
                System.println("襲撃後酒場２Ｆ・３");
                Runtime.jumpCF(67716, 3);
                break;
            }
            case 6: {
                System.println("襲撃後外観街２・２");
                Runtime.jumpCF(67746, 2);
                break;
            }
            case 7: {
                System.println("サブルート MC_KUK14");
                Runtime.jumpCF(2140, 1);
                break;
            }
            case 8: {
                System.println("襲撃後宿屋３Ｆ・３");
                Runtime.jumpCF(67736, 3);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
        int n = Runtime.getFlags(6048, 1);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 2, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 3, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(53, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
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
        if (Runtime.getFlags(7108, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1561, 1, 0, 14, 4, -16.3f, 0.0f, -4.65f, 0.0f);
            this.npc1.setMotion(0, 27);
            this.npc1.setInvalidID(1);
            this.npc1.disableDTKFlag(3);
            this.npc1.enableDTKFlag(262144);
            this.npc1.talkto("Talk_npc1");
        }
        if (Runtime.getFlags(7092, 4) != 13) {
            this.npc2 = new NPC_NORMAL(1600, 2, 0, 68, 4, 24.27f, 0.0f, 2.56f, 0.0f);
            this.npc2.talkto("Talk_npc2");
        } else {
            this.npc2 = new NPC_NORMAL(1600, 2, 0, 68, 4, -19.24f, 0.0f, 0.77f, 0.0f);
            this.npc2.talkto("Talk_npc2");
        }
        this.fire01 = new Effect(1402, 7.1f, 0.0f, -0.8f, 0.0f);
        this.fire02 = new Effect(1402, 8.1f, 0.0f, 7.6f, 0.0f);
        this.fire03 = new Effect(1402, 6.9f, 0.0f, 2.1f, 0.0f);
        this.fire04 = new Effect(1402, 4.1f, 0.0f, 1.4f, 0.0f);
        this.fire05 = new Effect(1402, -8.8f, 0.0f, 2.5f, 0.0f);
        this.fire06 = new Effect(1402, -8.0f, 0.0f, 1.7f, 0.0f);
        this.fire07 = new Effect(1402, 23.4f, 0.0f, -2.3f, 0.0f);
        this.fire08 = new Effect(1402, -17.9f, 0.0f, 6.5f, 0.0f);
        Runtime.progressEffect(30);
        this.kemuri1 = new Effect(1515, -20.5f, 11.5f, -5.6f, 0.0f);
        this.kemuri1.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri1.setClip(true);
        this.kemuri2 = new Effect(1515, -5.75f, 11.5f, -5.6f, 0.0f);
        this.kemuri2.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri2.setClip(true);
        this.singouki_red = new Effect(1539, -0.2375f, 1.3825f, 3.85f, 270.0f);
        this.singouki_red.setScale(0.275f, 0.275f, 0.275f);
        this.singouki_red.setRotate(25.0f, 295.0f, -10.0f);
        this.light01 = new Effect(1405, -13.4f, 3.2f, 1.0f, 0.0f);
        this.light01.setScale(0.8f, 0.8f, 0.8f);
        this.light02 = new Effect(1405, -4.9f, 4.0f, 6.2f, 0.0f);
        this.light02.setScale(0.3f, 0.3f, 0.3f);
        this.co01 = new Uwamono(28672, 7.1f, 0.0f, -0.8f, 0.0f);
        this.co01.SetHitKind('\u0001');
        this.co01.SetSize(1.5f, 1.0f, 1.5f);
        this.co02 = new Uwamono(28672, -8.8f, 0.0f, 2.5f, 0.0f);
        this.co02.SetHitKind('\u0001');
        this.co02.SetSize(1.5f, 1.0f, 1.5f);
        this.teiten2 = new Uwamono(28690, 8.1f, 0.0f, 7.6f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten134 = new Uwamono(28690, 6.1f, 0.0f, 0.65f, 0.0f);
        this.teiten134.SetBgm(196614);
        this.teiten56 = new Uwamono(28690, -8.4f, 0.0f, 2.1f, 0.0f);
        this.teiten56.SetBgm(196614);
        this.teiten7 = new Uwamono(28690, 23.4f, 0.0f, -2.3f, 0.0f);
        this.teiten7.SetBgm(196614);
        this.teiten8 = new Uwamono(28690, -17.9f, 0.0f, 6.5f, 0.0f);
        this.teiten8.SetBgm(196614);
        this.komono = new MAPUnit();
        this.komono.mapUnit(29);
        this.komono.start(4, null);
        this.komono.getTranslate();
        this.komono.setTranslate(this.komono.px, this.komono.py, this.komono.pz - 0.8f);
        if (Runtime.getFlags(7092, 4) != 13) {
            float[] fArray = new float[12];
            fArray[0] = 14.2f;
            fArray[2] = 3.4f;
            fArray[3] = -1.0f;
            fArray[4] = 14.7f;
            fArray[6] = 3.4f;
            fArray[8] = 13.7f;
            fArray[10] = 3.4f;
            float[] fArray2 = fArray;
            this.enemy1 = new NpcEnemy(16401, 11, 1, 105, 13, 14.2f, 0.0f, 3.4f, 0.0f, fArray2);
            this.enemy1.setGroup(1, 1, 1, 1);
            float[] fArray3 = new float[12];
            fArray3[0] = -4.1f;
            fArray3[2] = 1.0f;
            fArray3[3] = -1.0f;
            fArray3[4] = -4.6f;
            fArray3[6] = 1.0f;
            fArray3[8] = -3.6f;
            fArray3[10] = 1.0f;
            float[] fArray4 = fArray3;
            this.enemy3 = new NpcEnemy(16395, 13, 3, 130, 8, -4.1f, 0.0f, 1.0f, 0.0f, fArray4);
            this.enemy3.setGroup(2, 2, 2, 2);
            this.enemy3.kickEnepc(10, 50, 0);
            float[] fArray5 = new float[12];
            fArray5[0] = -14.4f;
            fArray5[2] = 3.9f;
            fArray5[3] = -1.0f;
            fArray5[4] = -14.9f;
            fArray5[6] = 3.9f;
            fArray5[8] = -13.9f;
            fArray5[10] = 3.9f;
            float[] fArray6 = fArray5;
            this.enemy4 = new NpcEnemy(16401, 14, 4, 105, 13, -14.4f, 0.0f, 3.9f, 0.0f, fArray6);
            this.enemy4.setGroup(1, 1, 1, 1);
        }
        if (Runtime.getFlags(8107, 1) == 0) {
            this.boss = new NpcEnemy(20240, 15, 0, 141, 10, 0.0f, 0.0f, 0.0f, 0.0f);
            this.boss.setInvalidID(1);
            this.boss.setMotion(0, 1);
            this.boss.setBatEvent(8);
            this.boss.setGroup(0, 0, 0, 0);
            this.boss.dispRadar(false);
            this.boss.setVisible(false);
            this.boss.disableDTKFlag(65536);
            this.boss2 = new NpcEnemy(20240, 16, 0, 141, 10, 20.3f, 0.0f, 3.7f, -90.0f);
            this.boss2.setMotion(0, 1);
            this.boss2.setInvalidID(1);
            this.boss2.setGroup(0, 0, 0, 0);
            this.boss2.setBatEvent(8);
            this.boss2.disableDTKFlag(65536);
            if (Runtime.getFlags(8101, 1) == 1) {
                this.boss2.dispRadar(true);
                this.boss2.kickEnepc(7, 135);
            } else {
                this.boss2.dispRadar(false);
                this.boss2.setVisible(false);
            }
        }
        new Uwamono(28675, -2.7f, 0.0f, -0.5f, 180.0f);
        new Uwamono(28674, 8.4f, 0.0f, 1.5f, 0.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.5f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFPedestal(5, -0.7949745f, 8.467842f, 12.857608f, 40.0f, -12.077396f, 40.858994f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestal(6, 4.723863f, 8.307665f, 12.93971f, 37.11962f, -10.071769f, -34.093002f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.015f, 0.015f);
        this.cam0.setCFAngle(9, -28.0f, 25.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, -21.341139f, 5.334243f, 4.9919443f, 46.565556f, -23.526102f, -30.57156f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        this.item01 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 252);
        this.item03 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 254);
        new Uwamono(68, 22, this.item01);
        new Uwamono(109, 33, this.item03);
        new Uwamono(110, 33);
        if (Runtime.getFlags(3201, 1) == 0) {
            new Uwamono(81, 31);
        } else {
            Stage.setVisible(81, false);
        }
        this.crank = new MAPUnit();
        this.crank.mapUnit(69);
        this.crank.start(4, null);
        this.yane = new MAPUnit();
        this.yane.mapUnit(113);
        this.yane.start(4, null);
        if (this.CRANK != 0) {
            this.yane.setTranslate(13.75f, 3.34f, -4.3f);
            this.player.setID(2);
        }
        this.doorA = new Uwamono(82, 40, '\u0004');
        if (Runtime.getFlags(3281, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(7108, 1) == 0) {
            this.doorB = new Uwamono(98, 40, '\u0002');
            this.doorB.SetDoorType('\u0002');
            this.doorB.SetSe0(196746);
            this.doorB.SetSe1(196746);
        } else {
            this.doorB = new Uwamono(98, 40, '\u0002');
            this.doorB.SetDoorType('\u0004');
            this.doorB.SetSe0(196746);
            this.doorB.SetSe1(196746);
        }
        this.s_unit = new Mapunits();
        this.s_unit.mapUnit(11);
        this.s_unit.start(4, null);
        this.s_unit.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2170.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2170.waitPage(this.win, 64);
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
            this.setEdgeFall(1);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            boolean bl = true;
            int n = 0;
            int n2 = 0;
            if (Runtime.getFlags(8107, 1) == 0) {
                if (Runtime.getFlags(8101, 1) == 0) {
                    ST2170.this.boss2.kickEnepc(4, 2);
                }
                System.sleep(1);
                ST2170.this.boss.kickEnepc(4, 2);
            }
            while (true) {
                if (bl) {
                    n2 = Math.random() % 5;
                    if (n2 == 0) {
                        bl = false;
                        ST2170.this.light01.disp(false);
                    }
                } else {
                    n = Math.random() % 2;
                    if (n == 0) {
                        bl = true;
                        ST2170.this.light01.disp(true);
                    }
                }
                System.sleep(10);
            }
        }
    }
}

