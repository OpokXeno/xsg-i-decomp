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
import xeno.map.MC_ELS02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0520
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS02_PRJ {
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
    Enepc npc10;
    Enepc npc15;
    Enepc npc20;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int talkFlag15;
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2010;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2015;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2042;
    int S2057;
    int hamago;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Unit monitor1;
    Unit monitor2;
    Unit monitor3;
    Unit monitor4;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect fade2;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect eve04;
    Effect eve05;
    Effect eve06;
    Effect eve07;
    Effect eve08;
    Effect eve09;
    Effect eve10;
    boolean kabeten = true;
    Uwamono shopA;
    Uwamono saveA;
    int epass = 0;
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
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    Uwamono teiten11;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Hey, Chief!!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D17 = new String[]{"/[label(Shion)]", "Oh, Allen, what's the matter?", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D18 = new String[]{"/[label(Allen)]", "Uh, um, Chief, I've been thinking a lot since then,", "/[waitkey(1)]/[clear()]", "and they seem to be pretty good people. So I'm thinking of trying to meet them halfway, bit-by-bit.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D19 = new String[]{"/[label(Shion)]", "Wow, Allen, I'm surprised.", "/[waitkey(1)]/[clear()]", "You're usually so hardheaded with your logic and preconceptions, so I was a little worried that you'd become like one of Wells' Aliens.", "/[waitkey(1)]/[clear()]", "Good luck. Bye!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D1A = new String[]{"/[label(Allen)]", "O-oh, no, that's not it. That is, my efforts...", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC7 = new String[]{"/[label(Allen)]", "Oh, um, Chief, actually...", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC8 = new String[]{"/[label(Shion)]", "Sorry, later, okay?!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC9 = new String[]{"/[label(Allen)]", "Oh, Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg01B81D16 = new String[]{"/[label(Allen)]", "Oh, Chief! Uh, um, I...that is, I don't want to make you feel bad...", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D17 = new String[]{"/[label(Shion)]", "Oh, sorry, but I'm in a hurry. Let's talk later.", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D18 = new String[]{"/[label(Allen)]", "Oh, uh, umm...Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg011C5348 = new String[]{"/[label(chaos)]", "Oh, Shion, how was it? Anything out of the ordinary?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5349 = new String[]{"/[label(Shion)]", "Nope, I didn't see any major problems.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534A = new String[]{"/[label(chaos)]", "Okay. Well, I hope it was just Hammer and the Captain's imagination then.", "/[waitkey(64)]/[close()]"};
    String[] msg011CB2F6 = new String[]{"/[label(Shion)]", "Hey, chaos, you don't trust me, do you?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F7 = new String[]{"/[label(chaos)]", "That's not true!", "/[waitkey(1)]/[clear()]", "Taking care of a problem or two is a piece of cake for you! Right?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F8 = new String[]{"/[label(Shion)]", "Yes, that's right, it's a piece of cake!", "/[waitkey(1)]/[clear()]", "...Hey, wait...you're still making fun of me, aren't you?", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "...A 100-Series Realian, and a prototype at that. I never thought I'd meet one in a place like this. Looks like I've got the Devil's own luck.", "/[waitkey(64)]/[close()]"};
    String[] msg00BA412B = new String[]{"/[label(chaos)]", "Shion, not yet, we have to go get Ziggy first, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg00BA412C = new String[]{"/[label(Shion)]", "Oh, right. I just freaked out, I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg00BAA0D9 = new String[]{"/[label(chaos)]", "I told you, not yet. Hurry up and get Ziggy.", "/[waitkey(64)]/[clear()]"};
    String[] msg00BAA0DA = new String[]{"/[label(Shion)]", "Oh, right. What the heck am I doing?", "/[waitkey(64)]/[close()]"};
    String[] msg00BB2440 = new String[]{"/[label(chaos)]", "Come on, Shion. Wait, are you doing this on purpose?", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E82 = new String[]{"/[label(Shion)]", "Oh? Aren't you going with Allen?", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E83 = new String[]{"/[label(Hammer)]", "Oh, Allen was crying over his food for some reason.", "/[waitkey(1)]/[clear()]", "No matter what I said, he just blubbered at me. It's really annoying, so I'm leaving him behind.", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E84 = new String[]{"/[label(Shion)]", "Hmm. I wonder what happened to him?", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E82c = new String[]{"/[label(chaos)]", "Oh? Aren't you going with Allen?", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E84c = new String[]{"/[label(Shion)]", "Hmm, I wonder what happened to him?", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E82g = new String[]{"/[label(Ziggy)]", "What's wrong? Isn't Allen with you?", "/[waitkey(64)]/[clear()]"};
    String[] msg00000001 = new String[]{"/[label(Hammer)]", "Looks like the repairs are done, so we'll be leaving port soon.", "/[waitkey(1)]/[clear()]", "The Captain's on the bridge, and he's pretty irritated right now. So if you need to take care of shopping, you'd better do it quickly.", "/[waitkey(64)]/[close()]"};
    String[] msgUMN = new String[]{"/[label()]", "Hey, do you know about that EVS plate there?", "/[waitkey(1)]/[clear()]", "The EVS is the general term for the Environmental Simulator Service offered by the U.M.N. Administration.", "/[waitkey(1)]/[clear()]", "You probably wouldn't understand a detailed explanation, so I'll explain it in layman terms.", "/[waitkey(1)]/[clear()]", "On top of the old save function, it can also activate various Environmental Simulators.", "/[waitkey(1)]/[clear()]", "The U.M.N. Administration currently supports four kinds of game centers. They also have battlefields based on recorded battle data. There are lots of ways to play\nwith it, so check it out.", "/[waitkey(1)]/[clear()]", "But in order to use a service, you'll need the passport for that particular service. Good luck getting them all!", "/[waitkey(64)]/[close()]"};
    String[] msgUMN2 = new String[]{"/[label()]", "Hey, do you know about that silver plate there?", "/[waitkey(1)]/[clear()]", "That plate is an online shop terminal provided by the U.M.N. Administration.", "/[waitkey(1)]/[clear()]", "You probably wouldn't understand a detailed explanation, so I'll explain it in layman terms.", "/[waitkey(1)]/[clear()]", "Basically, by touching this plate, you can use the online purchase service any time you want. Of course, you can't buy anything without money.", "/[waitkey(1)]/[clear()]", "Go out there and earn lots of money!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgLOCK = new String[]{"/[label(Warning)]", "'You crazy?!\n", " Opening this hatch during\n", " flight is strictly prohibited!!\n", " -The Great Matthews'", "/[waitkey(64)]/[close()]"};
    String[] Elv_1 = new String[]{"Would you like to go to B1?", "/[waitkey(64)]/[close()]"};
    String[] Elv_2 = new String[]{"Going to B1.", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Entry Corridor'", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0520() {
    }

    void CHAOS_TALK() {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.setTranslate(4.34f, 0.3f, -25.155f);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.npc4.kickEnepc(1, 9);
                this.npc4.setVisible(true);
                this.npc10.setVisible(true);
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00BA412B, 0);
                ST0520.waitPage(this.win, 64);
                this.npc10.moveEnepc(17, 90.0f, 0.1f, 20);
                this.npc10.kickEnepc(1, 10);
                this.win.print(this.msg00BA412C, 0);
                ST0520.waitPage(this.win, 64);
                System.sleep(20);
                this.fade.call(0);
                System.sleep(30);
                this.npc4.setVisible(false);
                this.npc10.setVisible(false);
                this.npc10.moveEnepc(17, 270.0f, -0.1f, 0);
                this.npc10.kickEnepc(0, 0);
                this.player.setTranslate(-9.685f, 0.3f, -25.029f);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.setTranslate(4.34f, 0.3f, -25.155f);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.npc4.kickEnepc(1, 9);
                this.npc4.setVisible(true);
                this.npc10.setVisible(true);
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00BAA0D9, 0);
                ST0520.waitPage(this.win, 64);
                this.npc10.moveEnepc(17, 90.0f, 0.1f, 20);
                this.npc10.kickEnepc(1, 10);
                this.win.print(this.msg00BAA0DA, 0);
                ST0520.waitPage(this.win, 64);
                System.sleep(20);
                this.fade.call(0);
                System.sleep(30);
                this.npc4.setVisible(false);
                this.npc10.setVisible(false);
                this.npc10.moveEnepc(17, 270.0f, -0.1f, 0);
                this.npc10.kickEnepc(0, 0);
                this.player.setTranslate(-9.685f, 0.3f, -25.029f);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(4.34f, 0.3f, -25.155f);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.npc4.kickEnepc(1, 9);
        this.npc4.setVisible(true);
        this.npc10.setVisible(true);
        System.sleep(10);
        this.npc10.moveEnepc(17, 90.0f, 0.1f, 20);
        this.npc10.kickEnepc(1, 10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00BB2440, 0);
        ST0520.waitPage(this.win, 64);
        System.sleep(20);
        this.fade.call(0);
        System.sleep(30);
        this.npc4.setVisible(false);
        this.npc10.setVisible(false);
        this.npc10.moveEnepc(17, 270.0f, -0.1f, 0);
        this.npc10.kickEnepc(0, 0);
        this.player.setTranslate(-9.685f, 0.3f, -25.029f);
        this.cam0.setMode(0);
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.4f, 5.447f, 3.137f);
        this.camEV.setRotate(-35.838f, 30.039f, 0.0f);
        this.camEV.setFov(45.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.785f, 2.631f, -22.532f);
        this.camEV.setRotate(-18.377f, 44.199f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.265f, 1.383f, 1.79f);
        this.camEV.setRotate(2.82f, 67.838f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.986f, 1.447f, -25.721f);
        this.camEV.setRotate(0.882f, 0.319f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.12f, 2.087f, -3.067f);
        this.camEV.setRotate(-3.978f, 269.907f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                Stage.setVisible(19, true);
                Stage.setVisible(6, false);
                Runtime.setFlags(5001, 1, 0);
                if (Runtime.getFlags(122, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7139, 1) != 0) break;
                Runtime.mailArriveSet(17);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                ST0520.waitPage(this.win, 64);
                System.sleep(15);
                Runtime.setFlags(7139, 1, 1);
                Runtime.mailExec(1);
                Runtime.setPlayerControl(true);
                break;
            }
            case 1: {
                this.doormove = 1;
                System.println("エレベーター");
                break;
            }
            case 2: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                if (this.S2057 == 1) {
                    this.win = Window.create();
                    this.win.print(this.msgLOCK, 0);
                    ST0520.waitPage(this.win, 64);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (this.ZIGGY == 1) {
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (this.S2040C == 1) {
                    this.fade.call(0);
                    System.sleep(30);
                    this.CHAOS_TALK();
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                this.win = Window.create();
                this.win.print(this.msgLOCK, 0);
                ST0520.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 3: {
                if (this.epass == 0) {
                    System.println("**********************************");
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_Camera02();
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(30, -12.0f, -0.5f, true);
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
                            this.EV_Camera02();
                            this.doormove = 2;
                            Sound.effectPlay(196713);
                            Runtime.setFlags(3039, 1, 1);
                            System.sleep(40);
                            Sound.effectPlay(196744);
                            this.ele.setArgs(12, -5.0f);
                            this.doorR2.start(1, "MoveD");
                            System.sleep(60);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.setPlayerControl(true);
                            if (this.S2013 != 0) {
                                Runtime.jumpCF(540, 2);
                                this.epass = 1;
                                break block0;
                            }
                            Runtime.setFlags(122, 1, 1);
                            Runtime.jumpEvent(2110);
                            System.println("フラグオン！");
                            this.epass = 1;
                            this.epass = 1;
                            return;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(50, -7.9f, -0.5f, true);
                    System.sleep(50);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    return;
                }
                this.epass = 1;
                break;
            }
            case 4: {
                if (this.epass == 0) break;
                System.println("on!!");
                this.epass = 0;
                break;
            }
            case 5: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.fade2.call(0);
                this.monitor1.signal(1);
                this.monitor2.signal(1);
                this.cam0.setMode(-1);
                Runtime.enable(65536);
                this.player.setTranslate(-1.22f, 0.4f, -27.14f);
                this.EV_Camera03();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAP, 0);
                ST0520.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(16);
                this.fade2.call(0);
                this.player.setTranslate(-3.99f, 0.31f, -27.15f);
                this.cam0.setMode(0);
                System.sleep(10);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 6: {
                Stage.setVisible(19, false);
                Stage.setVisible(6, true);
                break;
            }
            case 7: {
                if (Runtime.getFlags(122, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7139, 1) != 0) break;
                Runtime.mailArriveSet(17);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                ST0520.waitPage(this.win, 64);
                System.sleep(15);
                Runtime.setFlags(7139, 1, 1);
                Runtime.mailExec(1);
                Runtime.setPlayerControl(true);
                break;
            }
            case 8: {
                if (Runtime.getFlags(122, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7140, 1) != 0) break;
                switch (Runtime.mailReplyCheck(12)) {
                    case 1: {
                        Runtime.mailArriveSet(18);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7140, 1, 1);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Read email\nDon't read email");
                        System.waitFor(this.menu);
                        System.sleep(10);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.mailExec(1);
                                Runtime.setPlayerControl(true);
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                    case 2: {
                        Runtime.mailArriveSet(19);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7140, 1, 1);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Read email\nDon't read email");
                        System.waitFor(this.menu);
                        System.sleep(10);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.mailExec(1);
                                Runtime.setPlayerControl(true);
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                    case 3: {
                        Runtime.mailArriveSet(20);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7140, 1, 1);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Read email\nDon't read email");
                        System.waitFor(this.menu);
                        System.sleep(10);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.mailExec(1);
                                Runtime.setPlayerControl(true);
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                System.println("on!!");
                break;
            }
            case 9: {
                if (Runtime.getFlags(125, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7057, 2) != 2) {
                    return;
                }
                if (Runtime.getFlags(124, 1) != 1 || Runtime.getFlags(7145, 1) != 0) break;
                Runtime.mailArriveSet(25);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7145, 1, 1);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Read email\nDon't read email");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                break;
            }
            case 11: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.fade2.call(0);
                this.monitor3.signal(1);
                this.monitor4.signal(1);
                this.cam0.setMode(-1);
                Runtime.enable(65536);
                this.player.setTranslate(-1.22f, 0.4f, -27.14f);
                this.EV_Camera04();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAP, 0);
                ST0520.waitPage(this.win, 64);
                this.monitor3.signal(0);
                this.monitor4.signal(0);
                System.sleep(16);
                this.fade2.call(0);
                this.player.setTranslate(9.06f, 0.42f, -2.87f);
                this.cam0.setMode(0);
                System.sleep(10);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0520.waitPage(window, 64);
    }

    public void Talk_npc15(Enepc enepc, Window window) {
        this.Talk_npc15_1(window);
    }

    void Talk_npc15_1(Window window) {
        ++this.talkFlag15;
        switch (this.talkFlag15) {
            case 1: {
                this.npc15.kickEnepc(1, 9);
                window.print(this.msgUMN, 0);
                ST0520.waitPage(window, 64);
                return;
            }
        }
        this.npc15.kickEnepc(1, 9);
        window.print(this.msgUMN2, 0);
        ST0520.waitPage(window, 64);
        --this.talkFlag15;
        --this.talkFlag15;
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc3_7(window);
        } else if (this.ZIGGY == 1) {
            this.Talk_npc3_6(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc3_5(window);
        } else if (this.S2030 == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc3_4(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc3_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc3_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc3_1(Window window) {
    }

    void Talk_npc3_2(Window window) {
    }

    void Talk_npc3_3(Window window) {
    }

    void Talk_npc3_4(Window window) {
    }

    void Talk_npc3_5(Window window) {
    }

    void Talk_npc3_6(Window window) {
        if (Runtime.getLeader() == 1) {
            window.print(this.msg042F9E82, 0);
            ST0520.waitPage(window, 64);
            window.print(this.msg042F9E83, 0);
            ST0520.waitPage(window, 64);
        } else if (Runtime.getLeader() == 3) {
            window.print(this.msg042F9E82c, 0);
            ST0520.waitPage(window, 64);
            window.print(this.msg042F9E83, 0);
            ST0520.waitPage(window, 64);
        } else if (Runtime.getLeader() == 6) {
            window.print(this.msg042F9E82g, 0);
            ST0520.waitPage(window, 64);
            window.print(this.msg042F9E83, 0);
            ST0520.waitPage(window, 64);
        }
    }

    void Talk_npc3_7(Window window) {
        window.print(this.msg00000001, 0);
        ST0520.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc4_7(window);
        } else if (this.ZIGGY == 1) {
            this.Talk_npc4_6(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc4_5(window);
        } else if (this.S2030 == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc4_4(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc4_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc4_1(Window window) {
    }

    void Talk_npc4_2(Window window) {
    }

    void Talk_npc4_3(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg011C5348, 0);
                ST0520.waitPage(window, 64);
                window.print(this.msg011C5349, 0);
                ST0520.waitPage(window, 64);
                window.print(this.msg011C534A, 0);
                ST0520.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg011CB2F6, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg011CB2F7, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg011CB2F8, 0);
        ST0520.waitPage(window, 64);
    }

    void Talk_npc4_4(Window window) {
    }

    void Talk_npc4_5(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                Runtime.setPlayerControl(false);
                this.npc1.setVisible(true);
                window = Window.create();
                window.setSize(4, 45);
                window.setLocation(15, 305);
                window.print(this.msg00BA412B, 0);
                ST0520.waitPage(window, 64);
                window = Window.create();
                window.setSize(4, 45);
                window.setLocation(15, 305);
                window.print(this.msg00BA412C, 0);
                ST0520.waitPage(window, 64);
                this.npc1.setVisible(false);
                Runtime.setPlayerControl(true);
                return;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                this.npc1.setVisible(true);
                window = Window.create();
                window.setSize(4, 45);
                window.setLocation(15, 305);
                window.print(this.msg00BAA0D9, 0);
                ST0520.waitPage(window, 64);
                window = Window.create();
                window.setSize(4, 45);
                window.setLocation(15, 305);
                window.print(this.msg00BAA0DA, 0);
                ST0520.waitPage(window, 64);
                this.npc1.setVisible(false);
                Runtime.setPlayerControl(true);
                return;
            }
        }
        Runtime.setPlayerControl(false);
        this.npc1.setVisible(true);
        window = Window.create();
        window.setSize(4, 45);
        window.setLocation(15, 305);
        window.print(this.msg00BB2440, 0);
        ST0520.waitPage(window, 64);
        this.npc1.setVisible(false);
        Runtime.setPlayerControl(true);
    }

    void Talk_npc4_6(Window window) {
    }

    void Talk_npc4_7(Window window) {
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc5_7(window);
        } else if (this.ZIGGY == 1) {
            this.Talk_npc5_6(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc5_5(window);
        } else if (this.S2030 == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc5_4(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc5_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg00B81D16, 0);
                ST0520.waitPage(window, 64);
                window.print(this.msg00B81D17, 0);
                ST0520.waitPage(window, 64);
                window.print(this.msg00B81D18, 0);
                ST0520.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg00B81D19, 0);
                ST0520.waitPage(window, 64);
                window.print(this.msg00B81D1A, 0);
                ST0520.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg00B87CC7, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg00B87CC8, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg00B87CC9, 0);
        ST0520.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msg01B81D16, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg01B81D17, 0);
        ST0520.waitPage(window, 64);
        window.print(this.msg01B81D18, 0);
        ST0520.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
    }

    void Talk_npc5_4(Window window) {
    }

    void Talk_npc5_5(Window window) {
    }

    void Talk_npc5_6(Window window) {
    }

    void Talk_npc5_7(Window window) {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc8_5(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc8_4(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc8_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc8_2(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc8_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
    }

    void Talk_npc8_2(Window window) {
    }

    void Talk_npc8_3(Window window) {
        window.print(this.msg0436E833, 0);
        ST0520.waitPage(window, 64);
    }

    void Talk_npc8_4(Window window) {
    }

    void Talk_npc8_5(Window window) {
    }

    void doorset_1() {
        this.doorB = new Uwamono(54, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
    }

    void doorset_2() {
        this.doorB = new Uwamono(54, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S2013B == 0) {
                    Runtime.jumpCF(510, 1);
                    break;
                }
                if (this.S2014 == 0) {
                    Runtime.setFlags(124, 1, 1);
                    Runtime.jumpEvent(2140);
                    System.println("フラグオン！");
                    break;
                }
                if (this.S2014B == 0) {
                    Runtime.jumpCF(510, 1);
                    break;
                }
                if (this.S2028 == 0) {
                    Runtime.setFlags(126, 1, 1);
                    Runtime.jumpEvent(2150);
                    System.println("フラグオン！");
                    break;
                }
                Runtime.jumpCF(510, 1);
                break;
            }
            case 1: {
                if (Runtime.getFlags(7033, 1) == 0) {
                    Runtime.jumpCF(1211, 1);
                    break;
                }
                Runtime.jumpCF(1210, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(590, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(590, 2);
                break;
            }
            case 4: {
                Runtime.jumpCF(600, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(600, 2);
                break;
            }
            case 6: {
                Runtime.setFlags(5001, 1, 0);
                Runtime.jumpCF(610, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(530, 1);
                break;
            }
            case 8: {
                if (this.S2010 == 0) {
                    Runtime.jumpCF(540, 2);
                    break;
                }
                if (this.S2013 == 0) {
                    Runtime.setFlags(122, 1, 1);
                    Runtime.jumpEvent(2110);
                    System.println("フラグオン！");
                    break;
                }
                Runtime.jumpCF(540, 2);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(123, 1) == 0) {
            this.curry = new Obj();
            this.curry.init(24640, 0.0f, 0.0f, 0.0f, 0.0f);
            this.curry.start(4, null);
            this.curry.setTranslate(0.15f, 0.1f, -0.068f);
            this.curry.setRotate(260.0f, 90.0f, 0.0f);
            this.curry.setParent(this.player, 60);
            this.EFcurry = new Effect(1531, 0.0f, 0.0f, 0.0f, 0.0f);
            this.EFcurry.disp(true);
            this.EFcurry.setCaster(this.curry);
            this.EFcurry.noAttach(false);
        }
        this.S2009 = Runtime.getFlags(115, 1);
        this.S2010 = Runtime.getFlags(116, 1);
        this.S2013 = Runtime.getFlags(122, 1);
        this.S2013B = Runtime.getFlags(123, 1);
        this.S2014 = Runtime.getFlags(124, 1);
        this.S2014B = Runtime.getFlags(125, 1);
        this.S2028 = Runtime.getFlags(141, 1);
        this.MOMO = Runtime.getFlags(7014, 1);
        this.S2030 = Runtime.getFlags(143, 1);
        this.S2040C = Runtime.getFlags(158, 1);
        this.ZIGGY = Runtime.getFlags(7015, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S2057 = Runtime.getFlags(179, 1);
        this.hamago = Runtime.getFlags(7033, 1);
        Stage.setVisible(-1, true);
        this.monitor1 = new Object();
        this.monitor1.init(24613, -3.98f, 1.37f, -27.7f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18009, 0, 512, 260);
        this.monitor1.setArgs(2, 120, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Object();
        this.monitor2.init(24613, -3.98f, 1.37f, -27.69f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18012, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setScale(0.69f, 0.69f, 1.0f);
        this.monitor2.setScale(0.69f, 0.69f, 1.0f);
        this.monitor3 = new Object();
        this.monitor3.init(24613, 11.067f, 1.7f, -3.072f, 270.0f);
        this.monitor3.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor3.setArgs(1, 18009, 0, 512, 260);
        this.monitor3.setArgs(2, 120, 0, 0, -1);
        this.monitor3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor4 = new Object();
        this.monitor4.init(24613, 11.065f, 1.7f, -3.072f, 270.0f);
        this.monitor4.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor4.setArgs(1, 18014, 0, 256, 130);
        this.monitor4.setArgs(2, 64, 0, 1, -10);
        this.monitor4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor3.setScale(0.69f, 0.69f, 1.0f);
        this.monitor4.setScale(0.69f, 0.69f, 1.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        if (Runtime.getFlags(5001, 1) == 1) {
            Stage.setVisible(19, false);
            Stage.setVisible(6, true);
        } else if (Runtime.getFlags(5001, 1) == 0) {
            Stage.setVisible(19, true);
            Stage.setVisible(6, false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 9.2f, 0.3f, -28.0f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, 10.5f, 0.42f, 1.0f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.teiten3 = new Uwamono(28690, 10.5f, 0.42f, 3.3f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, 0.0f, -1.3f, -16.2f, 0.0f);
        this.teiten4.SetBgm(196614);
        this.teiten5 = new Uwamono(28690, 0.0f, -1.3f, -4.2f, 0.0f);
        this.teiten5.SetBgm(196614);
        this.teiten6 = new Uwamono(28690, 0.0f, -1.3f, 3.2f, 0.0f);
        this.teiten6.SetBgm(196614);
        this.teiten7 = new Uwamono(28690, 0.0f, -1.3f, 15.2f, 0.0f);
        this.teiten7.SetBgm(196614);
        this.teiten8 = new Uwamono(28690, -9.4f, 0.3f, -0.5f, 0.0f);
        this.teiten8.SetBgm(196615);
        this.teiten9 = new Uwamono(28690, 9.8f, 0.3f, 15.1f, 0.0f);
        this.teiten9.SetBgm(196634);
        this.teiten10 = new Uwamono(28690, 0.0f, 1.3f, 27.0f, 0.0f);
        this.teiten10.SetBgm(196617);
        this.teiten11 = new Uwamono(28690, -4.0f, 0.3f, -28.0f, 0.0f);
        this.teiten11.SetBgm(196612);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 0.0f, 5.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 5.0f, 0.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 7.8f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestal(5, 10.432f, 3.78f, -20.97f, 45.0f, -25.55f, 14.6f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(5, 1);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, 0.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, 0.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 0.0f);
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
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 1;
        this.fade2.args[2] = 0;
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_7();
        } else if (this.hamago == 1) {
            this.npcset_0();
        } else if (this.ZIGGY == 1) {
            this.npcset_6();
        } else if (this.S2040C == 1) {
            this.npcset_5();
        } else if (this.S2030 == 1) {
            this.npcset_9();
        } else if (this.S2028 == 1) {
            this.npcset_4();
        } else if (this.S2014B == 1) {
            this.npcset_3();
        } else if (this.S2014 == 1) {
            this.npcset_2();
        } else if (this.S2013B == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
        if (Runtime.getFlags(123, 1) == 1) {
            this.npc15 = new NPC_NORMAL(1612, 15, 0, 13, 19, 8.44f, 0.3f, -27.59f, 0.0f);
            this.npc15.talkto("Talk_npc15");
            this.npc15.setMotion(0, 10);
            this.npc15.setInvalidID(1);
        }
        if (this.S2057 == 1) {
            this.doorset_1();
        } else if (this.ZIGGY == 1) {
            this.doorset_2();
        } else {
            this.doorset_1();
        }
        this.doorA = new Uwamono(11, 40, '\u0001');
        this.doorC = new Uwamono(12, 40, '\u0001');
        this.doorD = new Uwamono(13, 40, '\u0001');
        this.doorE = new Uwamono(14, 40, '\u0001');
        this.doorF = new Uwamono(15, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        this.doorE.SetDoorType('\u0004');
        this.doorF.SetDoorType('\u0004');
        this.doorR1 = new Mapunits();
        this.doorR1.mapUnit(84);
        this.doorR1.start(4, null);
        this.doorR1.start(1, "automatic_door");
        this.doorL1 = new Mapunits();
        this.doorL1.mapUnit(85);
        this.doorL1.start(4, null);
        this.tA = new Uwamono(28672, -12.0f, -1.5f, -0.5f, 0.0f);
        this.tA.SetHitKind('\u0001');
        this.tA.SetSize(6.5f, 3.5f, 6.5f);
        if (Runtime.getFlags(3038, 1) == 1) {
            System.println("＊＊＊＊＊＊＊＊KOJI_38は立ってます＊＊＊＊＊＊＊＊＊");
            this.ele = new Obj();
            this.ele.initElevator(60, 0.0877193f, -5.0f);
            this.ele.setArgs(1, 0, 1);
            this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
            this.ele.start(1, "Up");
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(58);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py - 5.0f, this.doorR2.pz);
            this.doorR2.setRotate(0.0f, -90.0f, 0.0f);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(57);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py - 5.0f, this.doorL2.pz);
            this.doorL2.setRotate(0.0f, -90.0f, 0.0f);
        } else {
            System.println("＊＊＊＊＊＊＊＊KOJI_38は立ってません＊＊＊＊＊＊＊＊＊");
            this.ele = new Obj();
            this.ele.initElevator(60, 0.083333336f, 0.3f);
            this.ele.setArgs(1, 0, 1);
            this.ele.setArgs(12, 0.3f);
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(58);
            this.doorR2.start(4, null);
            this.doorR2.setRotate(0.0f, -90.0f, 0.0f);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(57);
            this.doorL2.start(4, null);
            this.doorL2.setRotate(0.0f, -90.0f, 0.0f);
        }
        this.saveA = Runtime.getFlags(123, 1) == 0 ? new Uwamono(28678, 8.0f, 0.3088749f, -24.75f) : new Uwamono(28733, 8.0f, 0.3088749f, -24.75f);
        this.shopA = new Uwamono(28679, 10.5f, 0.3088749f, -24.75f);
        this.shopA.SetShopNo(2);
        this.eve00 = new Effect(1414, 0);
        this.eve00.disp(true);
        this.eve01 = new Effect(1414, 1);
        this.eve01.disp(true);
        this.eve02 = new Effect(1410, 2);
        this.eve02.disp(true);
        this.eve03 = new Effect(1411, 3);
        this.eve03.disp(true);
        this.eve04 = new Effect(1414, 4);
        this.eve04.disp(true);
        this.eve05 = new Effect(1414, 5);
        this.eve05.disp(true);
        this.eve06 = new Effect(1413, 6);
        this.eve06.disp(true);
        this.eve07 = new Effect(1414, 7);
        this.eve07.disp(true);
        this.eve08 = new Effect(1414, 8);
        this.eve08.disp(true);
        this.eve09 = new Effect(1414, 9);
        this.eve09.disp(true);
        this.eve10 = new Effect(1414, 10);
        this.eve10.disp(true);
        Runtime.progressEffect(30);
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 1, 7, -2.57f, 0.0f, -1.05f, 90.0f);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_2() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 1, 7, -2.57f, 0.0f, -1.05f, 90.0f);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_3() {
        this.npc4 = new NPC_NORMAL(3, 4, 0, 81, 5, -2.57f, 0.0f, -1.05f, 90.0f);
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_4() {
    }

    void npcset_5() {
        this.npc4 = new NPC_NORMAL(3, 4, 0, 13, 5, -8.72f, 0.0f, -25.41f, 270.0f);
        this.npc10 = new NPC_NORMAL(1, 4, 0, 13, 14, -9.62f, 0.0f, -25.2f, 270.0f);
        this.npc4.setVisible(false);
        this.npc4.enableDTKFlag(262144);
        this.npc4.disableDTKFlag(131072);
        this.npc4.disableDTKFlag(65536);
        this.npc4.kickEnepc(4, 1);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        this.npc10.kickEnepc(4, 1);
    }

    void npcset_6() {
        this.npc3 = new NPC_NORMAL(278, 3, 0, 13, 3, -9.41f, 0.0f, -26.65f, 90.0f);
        this.npc3.talkto("Talk_npc3");
    }

    void npcset_7() {
        this.npc3 = new NPC_NORMAL(278, 3, 0, 2, 3, -0.03f, 0.0f, -19.56f, 0.0f);
        this.npc3.talkto("Talk_npc3");
    }

    void npcset_8() {
    }

    void npcset_9() {
        this.npc8 = new NPC_NORMAL(279, 8, 0, 17, 16, 9.06f, 0.41f, -1.8f, 300.0f);
        this.npc8.setMotion(0, 10);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.talkto("Talk_npc8");
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

    class Object
            extends Unit {
        Object() {
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }

        void Up() {
            ST0520.this.cam0.setMode(-1);
            ST0520.this.EV_Camera00();
            ST0520.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
            Runtime.setPlayerControl(false);
            ST0520.this.player.setTranslate(-10.5f, -5.3f, -0.4f);
            Sound.effectPlay(196745);
            ST0520.this.ele.setArgs(12, 0.3f);
            ST0520.this.doorR2.start(1, "MoveU");
            System.sleep(90);
            Sound.effectPlay(196713);
            ST0520.this.doormove = 1;
            System.sleep(40);
            Runtime.enable(65536);
            ST0520.this.player.mtn(2, 9, 1.0f, true);
            ST0520.this.player.move(30, -8.0f, -0.5f, true);
            System.sleep(35);
            Runtime.disable(65536);
            Runtime.setFlags(3038, 1, 0);
            ST0520.this.cam0.setMode(0);
            ST0520.this.epass = 1;
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
                    ST0520.this.doorR2.getTranslate();
                    ST0520.this.doorL2.getTranslate();
                    ST0520.this.doorR2.setTranslate(ST0520.this.doorR2.px, ST0520.this.doorR2.py - 0.083333336f, ST0520.this.doorR2.pz);
                    ST0520.this.doorL2.setTranslate(ST0520.this.doorL2.px, ST0520.this.doorL2.py - 0.083333336f, ST0520.this.doorL2.pz);
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
                    ST0520.this.doorR2.getTranslate();
                    ST0520.this.doorL2.getTranslate();
                    ST0520.this.doorR2.setTranslate(ST0520.this.doorR2.px, ST0520.this.doorR2.py + 0.083333336f, ST0520.this.doorR2.pz);
                    ST0520.this.doorL2.setTranslate(ST0520.this.doorL2.px, ST0520.this.doorL2.py + 0.083333336f, ST0520.this.doorL2.pz);
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
                ST0520.this.player.getTranslate();
                f2 = (-12.0f - ST0520.this.player.px) * (-12.0f - ST0520.this.player.px) + (0.0f - ST0520.this.player.pz) * (0.0f - ST0520.this.player.pz);
                if (f2 > 20.0f) {
                    if (ST0520.this.doormove == 1) {
                        Sound.effectPlay(196713);
                    }
                    ST0520.this.doormove = 2;
                }
                if (ST0520.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196713);
                    }
                    if (f == (float) n2) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0520.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0520.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0520.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0520.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0520.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == 0.0f) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0520.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0520.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0520.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0520.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0520.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0520.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0520.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
                } else if (Runtime.getFlags(3038, 1) == 0 && Runtime.getFlags(3039, 1) == 0) {
                    ST0520.this.tA.setTranslate(-12.0f, -1.5f, -0.5f);
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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }
}

