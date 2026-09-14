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

class ST0521
        extends Stage
        implements XenoConstants,
        CfConstants,
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
    int page;
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Oh, Chief!!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D17 = new String[]{"/[label(Shion)]", "Oh, Allen, what's the matter?", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D18 = new String[]{"/[label(Allen)]", "Uh, um, Chief, I've been thinking a lot since then,", "/[waitkey(1)]/[clear()]", "and they seem to be pretty good people. So I'm thinking of trying to meet them halfway, bit by bit.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D19 = new String[]{"/[label(Shion)]", "Hmm, sounds good. Good luck. Okay, bye!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D1A = new String[]{"/[label(Allen)]", "O-oh, no, that's not it. That is, my efforts...", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC7 = new String[]{"/[label(Allen)]", "Oh, um, Chief, actually...", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC8 = new String[]{"/[label(Shion)]", "Sorry, later, okay?!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC9 = new String[]{"/[label(Allen)]", "Oh, Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg01B81D16 = new String[]{"/[label(Allen)]", "Oh, Chief! Uh, um, I...that is, I don't want to make you feel bad...", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D17 = new String[]{"/[label(Shion)]", "Oh, sorry, but I'm in a hurry. Let's talk later.", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D18 = new String[]{"/[label(Allen)]", "Oh, uh, umm...Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg011C5348 = new String[]{"/[label(chaos)]", "Oh, Shion, how was it? Anything abnormal?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5349 = new String[]{"/[label(Shion)]", "Nope, I didn't see any major problems.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534A = new String[]{"/[label(chaos)]", "Okay. Well, I hope it was just Hammer and the Captain's imagination then.", "/[waitkey(64)]/[close()]"};
    String[] msg011CB2F6 = new String[]{"/[label(Shion)]", "Hey, chaos, you don't trust me, do you?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F7 = new String[]{"/[label(chaos)]", "That's not true!", "/[waitkey(1)]/[clear()]", "Taking care of a problem or two is a piece of cake for you! Right?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F8 = new String[]{"/[label(Shion)]", "Yup, that's right, it's a piece of cake!", "/[waitkey(1)]/[clear()]", "...Hey, wait...you're still making fun of me, aren't you?", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "...A 100-Series Realian, and a prototype at that. I never thought I'd meet one in a place like this. Looks like I've got the Devil's own luck.", "/[waitkey(64)]/[close()]"};
    String[] msg00BA412B = new String[]{"/[label(chaos)]", "Shion, not yet, we have to go get Ziggy first, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg00BA412C = new String[]{"/[label(Shion)]", "Oh, right. I just freaked out, I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg00BAA0D9 = new String[]{"/[label(chaos)]", "I told you, not yet. Hurry up and get Ziggy.", "/[waitkey(64)]/[clear()]"};
    String[] msg00BAA0DA = new String[]{"/[label(Shion)]", "Oh, right. What the heck am I doing?", "/[waitkey(64)]/[close()]"};
    String[] msg00BB2440 = new String[]{"/[label(chaos)]", "Come on, Shion. Wait, are you doing this on purpose?", "/[waitkey(64)]/[close()]"};
    String[] msgUMN = new String[]{"/[label()]", "Hey, do you know about that EVS plate there?", "/[waitkey(1)]/[clear()]", "The EVS is the general term for the Environmental Simulator Service offered by the U.M.N. Administration.", "/[waitkey(1)]/[clear()]", "You probably wouldn't understand a detailed explanation, so I'll explain it in layman terms.", "/[waitkey(1)]/[clear()]", "On top of the old save function, it can also activate various Environmental Simulators.", "/[waitkey(1)]/[clear()]", "The U.M.N. Administration currently supports four kinds of game centers. They also have battlefields based on recorded battle data. There are lots of ways to play\nwith it, so check it out.", "/[waitkey(1)]/[clear()]", "But in order to use a service, you'll need the passport for that particular service. Good luck getting them all!", "/[waitkey(64)]/[close()]"};
    String[] msgUMN2 = new String[]{"/[label()]", "Hey, do you know about that silver plate there?", "/[waitkey(1)]/[clear()]", "That plate is an online shop terminal provided by the U.M.N. Administration.", "/[waitkey(1)]/[clear()]", "You probably wouldn't understand a detailed explanation, so I'll explain it in layman terms.", "/[waitkey(1)]/[clear()]", "Basically, by touching this plate, you can use the online purchase service any time you want. Of course, you can't buy anything without money.", "/[waitkey(1)]/[clear()]", "Go out there and earn lots of money!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgLOCK = new String[]{"/[label(Danger)]", "You cannot exit the ship at this time.", "/[waitkey(64)]/[close()]"};
    String[] Elv_1 = new String[]{"Would you like to go to B1?", "/[waitkey(64)]/[close()]"};
    String[] Elv_2 = new String[]{"Going to B1.", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Entry Corridor'", "/[waitkey(64)]/[close()]"};
    String[] msgH001 = new String[]{"/[label(Hammer)]", "What's the matter? Oh, did you get lonely from not seeing me? Don't worry, the Elsa's currently under standby orders, so you can come see me anytime.", "/[waitkey(64)]/[close()]"};
    String[] msgH002 = new String[]{"/[label(Hammer)]", "What's up? You want to go somewhere? Then you'd better tell the Captain. Oh, but make sure you don't get ripped off.", "/[waitkey(64)]/[close()]"};
    String[] msgH003 = new String[]{"/[label(Hammer)]", "MOMO? Haven't seen her. At times like this, you could locate her right away by asking MOMO. But without MOMO around, I dunno what to do.", "/[waitkey(64)]/[close()]"};
    String[] msgH004 = new String[]{"/[label(Hammer)]", "Oh? What are you doing alone? Allen? He's not here. He's probably eating his frustration away again.", "/[waitkey(64)]/[close()]"};
    String[] msgH005 = new String[]{"/[label(Hammer)]", "Oh, you're safe! I'm so glad. It was a close call for the Elsa, too, but we locked all the hatches before the soldiers got in. Then again, our situation now is the\nsame as having been captured.", "/[waitkey(64)]/[close()]"};
    String[] msgH006 = new String[]{"/[label(Hammer)]", "We're in trouble, big trouble! The citizens, the Foundation, and the Gnosis are in a huge bind! Ahh, I don't know what's what!", "/[waitkey(64)]/[close()]"};
    String[] msgH007 = new String[]{"/[label(Hammer)]", "I heard. MOMO's in big trouble, right?! Don't worry, I'm ready. I'll go through a lake of fire for MOMO! Then again, we're out in space, aren't we?", "/[waitkey(64)]/[close()]"};
    String[] msgH008 = new String[]{"/[label(Hammer)]", "Well? Have you found MOMO? Don't give up. Keep at it!!", "/[waitkey(64)]/[close()]"};
    String[] msgH0081 = new String[]{"/[label(Hammer)]", "All that's left is to stop the Song of Nephilim, right?! Don't give up. Keep at it!!", "/[waitkey(64)]/[close()]"};
    String[] msgH009 = new String[]{"/[label(Hammer)]", "Sure, I'm ready. I couldn't ask for anything more than to be able to save the world! We'll be heroes if we defeat him!", "/[waitkey(64)]/[close()]"};
    String[] msgH00A = new String[]{"/[label(Hammer)]", "Oh? What's the matter? Weren't you resting in your room?", "/[waitkey(1)]/[clear()]", "Allen? I think I saw him heading toward the Foundation just now.", "/[waitkey(64)]/[close()]"};
    String[] msgA001 = new String[]{"/[label(Allen)]", "Oh, Chief, welcome back. Well? Did you rest well?", "/[waitkey(64)]/[clear()]"};
    String[] msgA002 = new String[]{"/[label(Shion)]", "Yup! I'm fine now.", "/[waitkey(64)]/[clear()]"};
    String[] msgA003 = new String[]{"/[label(Allen)]", "Then let's get working on doing maintenance on KOS-MOS, shall we?\n", "/[waitkey(64)]/[close()]"};
    String[] msgkey1 = new String[]{"/[label(Warning)]", "'You moron!\n", " It's locked because of the emergency!\n", " -The Great Matthews'", "/[waitkey(64)]/[clear()]"};
    String[] msgkey2 = new String[]{"/[label()]", "Unlock?", "/[waitkey(64)]/[close()]"};
    String[] msgkey3 = new String[]{"/[label()]", "Unlocked.", "/[waitkey(64)]/[close()]"};

    ST0521() {
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
        this.camEV.setTranslate(0.166f, 2.247f, -20.541f);
        this.camEV.setRotate(-14.252f, 11.195f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera05() {
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
        switch (n2) {
            case 0: {
                Stage.setVisible(19, true);
                Stage.setVisible(6, false);
                Runtime.setFlags(5001, 1, 0);
                break;
            }
            case 1: {
                this.doormove = 1;
                System.println("エレベーター");
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
                            Runtime.jumpCF(541, 2);
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
                ST0521.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(16);
                this.fade2.call(0);
                this.player.setTranslate(-3.99f, 0.31f, -27.15f);
                this.cam0.setMode(0);
                System.sleep(0);
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
            case 10: {
                if (Runtime.getFlags(315, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(314, 1) == 0) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.npc5.kickEnepc(4, 1);
                this.npc20.kickEnepc(4, 1);
                this.fade.call(0);
                System.sleep(30);
                this.npc20.setVisible(true);
                this.npc3.setVisible(false);
                if (Runtime.getLeader() == 1) {
                    Runtime.enable(65536);
                    this.player.setTranslate(100.0f, 0.0f, 100.0f);
                    this.player.rotY(1, 340.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                } else {
                    Runtime.enable(65536);
                    this.player.setTranslate(0.52f, 0.31f, -24.06f);
                    this.player.rotY(1, 340.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                }
                this.cam0.setMode(-1);
                this.EV_Camera04();
                this.npc20.look_char(this.npc5);
                this.npc5.look_char(this.npc20);
                System.sleep(10);
                this.npc5.kickEnepc(0, 9);
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msgA001, 0);
                ST0521.waitPage(this.win, 64);
                this.npc20.kickEnepc(0, 7);
                this.win.print(this.msgA002, 0);
                ST0521.waitPage(this.win, 64);
                this.npc5.kickEnepc(0, 9);
                this.win.print(this.msgA003, 0);
                ST0521.waitPage(this.win, 64);
                System.sleep(15);
                this.fade.call(0);
                System.sleep(30);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                Runtime.setFlags(315, 1, 1);
                System.println("Flag on!");
                Runtime.jumpEvent(3080);
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
                this.EV_Camera05();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAP, 0);
                ST0521.waitPage(this.win, 64);
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
        ST0521.waitPage(window, 64);
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
                ST0521.waitPage(window, 64);
                return;
            }
        }
        this.npc15.kickEnepc(1, 9);
        window.print(this.msgUMN2, 0);
        ST0521.waitPage(window, 64);
        --this.talkFlag15;
        --this.talkFlag15;
    }

    public void Talk_npc20(Enepc enepc) {
        this.Talk_npc20_1(this.win);
    }

    void Talk_npc20_1(Window window) {
        if (Runtime.getFlags(3161, 1) == 0) {
            window = Window.create();
            window.setSize(2, 25);
            window.setLocation(15, 15);
            window.print(this.msgkey1, 0);
            ST0521.waitPage(window, 64);
            window.print(this.msgkey2, 0);
            System.waitFor(window);
            this.menu = Menu.create();
            this.menu.addItem("Disarm\nDon't disarm");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    Runtime.setFlags(3161, 1, 1);
                    window = Window.create();
                    window.setSize(2, 25);
                    window.setLocation(15, 15);
                    window.print(this.msgkey3, 0);
                    ST0521.waitPage(window, 64);
                    this.doorB.SetDoorType('\u0004');
                    return;
                }
            }
            return;
        }
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (Runtime.getFlags(389, 1) == 1) {
            this.Talk_npc3_8(window);
        } else if (Runtime.getFlags(375, 1) == 1) {
            this.Talk_npc3_72(window);
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc3_7(window);
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc3_6(window);
        } else if (Runtime.getFlags(362, 1) == 1) {
            this.Talk_npc3_52(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc3_5(window);
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc3_3(window);
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.Talk_npc3_4(window);
        } else if (Runtime.getFlags(314, 1) == 1) {
            this.Talk_npc3_1(window);
        } else if (Runtime.getFlags(7162, 1) == 1) {
            this.Talk_npc3_1(window);
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.Talk_npc3_9(window);
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.Talk_npc3_0(window);
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.Talk_npc3_2(window);
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_npc3_0(window);
        } else {
            this.Talk_npc3_0(window);
        }
    }

    void Talk_npc3_0(Window window) {
        window.print(this.msgH001, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msgH002, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgH003, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        window.print(this.msgH004, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_4(Window window) {
        window.print(this.msgH005, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_5(Window window) {
        window.print(this.msgH006, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_52(Window window) {
        window.print(this.msgH006, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_6(Window window) {
        window.print(this.msgH007, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_7(Window window) {
        window.print(this.msgH008, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_72(Window window) {
        window.print(this.msgH0081, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_8(Window window) {
        window.print(this.msgH009, 0);
        ST0521.waitPage(window, 64);
    }

    void Talk_npc3_9(Window window) {
        window.print(this.msgH00A, 0);
        ST0521.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
    }

    void Talk_npc4_1(Window window) {
    }

    public void Talk_npc5(Enepc enepc) {
        this.Talk_npc5_1();
    }

    void Talk_npc5_1() {
        this.npc5.kickEnepc(4, 1);
        this.npc20.kickEnepc(4, 1);
        this.fade.call(0);
        System.sleep(30);
        this.npc20.setVisible(true);
        this.npc3.setVisible(false);
        if (Runtime.getLeader() == 1) {
            Runtime.enable(65536);
            this.player.setTranslate(100.0f, 0.0f, 100.0f);
            this.player.rotY(1, 340.0f, true);
            this.player.mtn(28, 9, 1.0f, true);
        } else {
            Runtime.enable(65536);
            this.player.setTranslate(0.52f, 0.31f, -24.06f);
            this.player.rotY(1, 340.0f, true);
            this.player.mtn(28, 9, 1.0f, true);
        }
        this.cam0.setMode(-1);
        this.EV_Camera04();
        this.npc20.look_char(this.npc5);
        this.npc5.look_char(this.npc20);
        System.sleep(10);
        this.npc5.kickEnepc(0, 9);
        this.win = Window.create();
        this.win.setSize(2, 25);
        this.win.setLocation(15, 305);
        this.win.print(this.msgA001, 0);
        ST0521.waitPage(this.win, 64);
        this.npc20.kickEnepc(0, 7);
        this.win.print(this.msgA002, 0);
        ST0521.waitPage(this.win, 64);
        this.npc5.kickEnepc(0, 9);
        this.win.print(this.msgA003, 0);
        ST0521.waitPage(this.win, 64);
        System.sleep(15);
        this.fade.call(0);
        System.sleep(30);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
        Runtime.setFlags(315, 1, 1);
        System.println("フラグオン！");
        Runtime.jumpEvent(3080);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
    }

    void Talk_npc8_1(Window window) {
    }

    void doorset_1() {
        this.doorB = new Uwamono(54, 40, '\u0001');
        if (Runtime.getFlags(3161, 1) == 0) {
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB.SetDoorType('\u0004');
        }
    }

    void doorset_2() {
        this.doorB = new Uwamono(54, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
    }

    public void entered(int var1_1) {
        Runtime.setRegister(0, var1_1);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (var1_1) {
            case 0: {
                Runtime.jumpCF(511, 1);
                break;
            }
            case 1: {
                if (Runtime.getFlags(389, 1) != 1) **GOTO lbl20
                if (Runtime.getFlags(7086, 1) != 1) **GOTO lbl15
                Runtime.jumpCF(1210, 1);
                **GOTO lbl58
                lbl15:

                if (Runtime.getFlags(7088, 1) == 1) {
                    Runtime.jumpCF(3030, 1);
                    break;
                }
                Runtime.jumpCF(1803, 1);
                break;
                lbl20:

                if (Runtime.getFlags(374, 1) == 1) {
                    Runtime.jumpCF(2860, 1);
                    break;
                }
                if (Runtime.getFlags(373, 1) == 1) {
                    Runtime.jumpCF(1802, 1);
                    break;
                }
                if (Runtime.getFlags(360, 1) == 1) {
                    Runtime.jumpCF(1802, 1);
                    break;
                }
                if (Runtime.getFlags(346, 1) == 1) {
                    Runtime.jumpCF(1800, 1);
                    break;
                }
                if (Runtime.getFlags(326, 1) == 1) {
                    Runtime.jumpCF(1801, 1);
                    break;
                }
                if (Runtime.getFlags(314, 1) == 1) {
                    if (Runtime.getFlags(7086, 1) == 1) {
                        Runtime.jumpCF(1210, 1);
                        break;
                    }
                    Runtime.jumpCF(1800, 1);
                    break;
                }
                if (Runtime.getFlags(310, 1) == 1) {
                    Runtime.jumpCF(1800, 1);
                } else {
                    if (Runtime.getFlags(304, 1) == 1) {
                        if (Runtime.getFlags(7086, 1) == 1) {
                            Runtime.jumpCF(1210, 1);
                            break;
                        }
                        Runtime.jumpCF(1800, 1);
                        break;
                    }
                    if (Runtime.getFlags(303, 1) == 1) {
                        Runtime.jumpCF(1800, 1);
                        break;
                    }
                    if (Runtime.getFlags(301, 1) == 1) {
                        Runtime.jumpCF(1800, 1);
                        break;
                    }
                    Runtime.jumpCF(1210, 1);
                    break;
                }
            }
            lbl58:

            case 2:
            {
                Runtime.jumpCF(591, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(591, 2);
                break;
            }
            case 4: {
                Runtime.jumpCF(601, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(601, 2);
                break;
            }
            case 6: {
                Runtime.setFlags(5001, 1, 0);
                Runtime.jumpCF(611, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(531, 1);
                break;
            }
            case 8: {
                Runtime.jumpCF(541, 2);
                break;
            }
        }
    }

    void init() {
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
        if (Runtime.getFlags(346, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.npcset_3();
        } else if (Runtime.getFlags(315, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(314, 1) == 1) {
            this.npcset_2();
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
        this.npc15 = new NPC_NORMAL(1612, 15, 0, 13, 19, 8.44f, 0.3f, -27.59f, 0.0f);
        this.npc15.talkto("Talk_npc15");
        this.npc15.setMotion(0, 10);
        this.npc15.setInvalidID(1);
        if (Runtime.getFlags(346, 1) == 1) {
            this.doorset_2();
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.doorset_1();
        } else {
            this.doorset_2();
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
        if (Runtime.getFlags(389, 1) == 1) {
            this.shopA.SetShopNo(13);
        } else if (Runtime.getFlags(372, 1) == 1) {
            this.shopA.SetShopNo(11);
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.shopA.SetShopNo(7);
        } else {
            this.shopA.SetShopNo(2);
        }
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
        this.npc3 = new NPC_NORMAL(278, 3, 0, 2, 3, -0.03f, 0.0f, -19.56f, 0.0f);
        this.npc3.talkto("Talk_npc3");
    }

    void npcset_2() {
        this.npc3 = new NPC_NORMAL(278, 3, 0, 2, 3, -2.92f, 0.31f, -27.11f, 0.0f);
        this.npc5 = new NPC_NORMAL(263, 5, 0, 14, 7, 0.1f, 0.31f, -22.61f, 220.0f);
        this.npc20 = new NPC_NORMAL(1, 20, 0, 14, 14, -0.48f, 0.31f, -23.25f, 20.0f);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(262144);
        this.npc5.setMotion(0, 10);
        this.npc5.setInvalidID(1);
        this.npc20.disableDTKFlag(3);
        this.npc20.enableDTKFlag(262144);
        this.npc20.disableDTKFlag(8);
        this.npc20.setVisible(false);
        this.npc20.setInvalidID(1);
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_3() {
        this.npc3 = new NPC_NORMAL(278, 3, 0, 2, 3, -0.03f, 0.0f, -19.56f, 0.0f);
        this.npc3.talkto("Talk_npc3");
        if (Runtime.getFlags(3161, 1) == 0) {
            this.npc20 = new NPC_NORMAL(278, 20, 0, 14, 3, -10.81f, 0.4f, -25.34f, 0.0f);
            this.npc20.setInvalidID(1);
            this.npc20.setVisible(false);
            this.npc20.talkto("Talk_npc20");
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
            ST0521.this.cam0.setMode(-1);
            ST0521.this.EV_Camera00();
            ST0521.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
            Runtime.setPlayerControl(false);
            ST0521.this.player.setTranslate(-10.5f, -5.3f, -0.4f);
            Sound.effectPlay(196745);
            ST0521.this.ele.setArgs(12, 0.3f);
            ST0521.this.doorR2.start(1, "MoveU");
            System.sleep(90);
            Sound.effectPlay(196713);
            ST0521.this.doormove = 1;
            System.sleep(40);
            Runtime.enable(65536);
            ST0521.this.player.mtn(2, 9, 1.0f, true);
            ST0521.this.player.move(30, -8.0f, -0.5f, true);
            System.sleep(35);
            Runtime.disable(65536);
            Runtime.setFlags(3038, 1, 0);
            ST0521.this.cam0.setMode(0);
            ST0521.this.epass = 1;
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
                    ST0521.this.doorR2.getTranslate();
                    ST0521.this.doorL2.getTranslate();
                    ST0521.this.doorR2.setTranslate(ST0521.this.doorR2.px, ST0521.this.doorR2.py - 0.083333336f, ST0521.this.doorR2.pz);
                    ST0521.this.doorL2.setTranslate(ST0521.this.doorL2.px, ST0521.this.doorL2.py - 0.083333336f, ST0521.this.doorL2.pz);
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
                    ST0521.this.doorR2.getTranslate();
                    ST0521.this.doorL2.getTranslate();
                    ST0521.this.doorR2.setTranslate(ST0521.this.doorR2.px, ST0521.this.doorR2.py + 0.083333336f, ST0521.this.doorR2.pz);
                    ST0521.this.doorL2.setTranslate(ST0521.this.doorL2.px, ST0521.this.doorL2.py + 0.083333336f, ST0521.this.doorL2.pz);
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
                ST0521.this.player.getTranslate();
                f2 = (-12.0f - ST0521.this.player.px) * (-12.0f - ST0521.this.player.px) + (0.0f - ST0521.this.player.pz) * (0.0f - ST0521.this.player.pz);
                if (f2 > 20.0f) {
                    if (ST0521.this.doormove == 1) {
                        Sound.effectPlay(196713);
                    }
                    ST0521.this.doormove = 2;
                }
                if (ST0521.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196713);
                    }
                    if (f == (float) n2) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0521.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0521.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0521.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0521.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0521.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == 0.0f) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0521.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0521.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0521.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                        ST0521.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2 - 90.0f, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0521.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0521.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0521.this.tA.setTranslate(-12.0f, 5.5f, -0.5f);
                } else if (Runtime.getFlags(3038, 1) == 0 && Runtime.getFlags(3039, 1) == 0) {
                    ST0521.this.tA.setTranslate(-12.0f, -1.5f, -0.5f);
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

