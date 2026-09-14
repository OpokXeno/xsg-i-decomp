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
import xeno.map.MC_ELS09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0590
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS09_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Enepc npc15;
    Enepc npc20;
    Unit unit1;
    Menu menu;
    Window win;
    Unit monitor1;
    Unit monitor2;
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
    int talkFlag62;
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Chief, I still think this is a bad idea.", "/[waitkey(1)]/[clear()]", "I don't trust my life to the crew of this junky ship. After all, these people have connections with the notorious Kukai Foundation.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D17 = new String[]{"/[label(Shion)]", "Sheesh, Allen, why are you such a worrywart? I just explained it to you, remember?", "/[waitkey(1)]/[clear()]", "Everything will be okay. They don't seem like such bad people. If need be, we've got KOS-MOS on our side.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D18 = new String[]{"/[label(Allen)]", "Well, don't forget KOS-MOS just abandoned us.", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC5 = new String[]{"/[label(Allen)]", "Chief, please follow proper procedures and contact Headquarters. No more acting on your own, okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC6 = new String[]{"/[label(Shion)]", "I know, I'll follow HQ's instructions.", "/[waitkey(1)]/[clear()]", "You know, it's not very manly of you to keep bringing up these trivial matters.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC7 = new String[]{"/[label(Allen)]", "W-what are you talking about? It's more like you're not detail oriented.", "/[waitkey(64)]/[close()]"};
    String[] msg01B81D16 = new String[]{"/[label(Allen)]", "Oh, Chief?!\n", "/[waitkey(1)]/[clear()]", "See, they are weird! Look at the strange powers that they used! There's no way a human being could go up against a Gnosis!", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D17 = new String[]{"/[label(Shion)]", "But there's one right here. chaos is a real flesh and blood human being, no matter how you look at it.", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D18 = new String[]{"/[label(Allen)]", "And I'm saying that's the weird part.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D19 = new String[]{"/[label(Shion)]", "Oh, give it up! It'll be okay! Trust me!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D1A = new String[]{"/[label(Allen)]", "Well, that's what worries me the most.", "/[waitkey(64)]/[close()]"};
    String[] msg01B87CC7 = new String[]{"/[label(Allen)]", "This still isn't a good idea. You saw the Captain's face, didn't you?", "/[waitkey(1)]/[clear()]", "Don't you think that he looks pretty dangerous? That Captain has definitely killed people before.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC8 = new String[]{"/[label(Shion)]", "Oh, you're so uptight. Girls won't like you if you're so annoying!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC9 = new String[]{"/[label(Allen)]", "Uh...", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "Enemies?! Damn, I never thought they'd get in this far. What's the situation?!", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E834 = new String[]{"/[label(Shion)]", "The Captain and the others are managing to hold the bridge. Commander, please stay here and return fire.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E835 = new String[]{"/[label(Cherenkov)]", "That's fine, but what are all of you going to do?", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E836 = new String[]{"/[label(Shion)]", "We will go take care of the enemy below.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E837 = new String[]{"/[label(Cherenkov)]", "Understood. Don't do anything reckless.", "/[waitkey(64)]/[close()]"};
    String[] msg02B81D16 = new String[]{"/[label(Allen)]", "Chief! Stop doing these dangerous things.", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC2 = new String[]{"/[label(Allen)]", "Chief, we should just escape in a pod or something.", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC21 = new String[]{"/[label(Allen)]", "Sheesh, why are we always getting into these dangerous situations?! At this rate, no amount of lives are going to help us.", "/[waitkey(64)]/[close()]"};
    String[] msg0436E83C = new String[]{"/[label(Shion)]", "This is Cabin 1. It's the room the people of this ship use.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E83C = new String[]{"/[label(Shion)]", "Allen and Commander Cherenkov also stay here.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E83E = new String[]{"/[label(MOMO)]", "It's a wonderful cabin.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E840 = new String[]{"/[label(Shion)]", "Yes, it is. I hear the Elsa was originally a luxury space cruiser. That's why there's a high-class feel to everything in both its exterior and interior, even\nthough it's a cargo-passenger ship.", "/[waitkey(64)]/[close()]"};
    String[] msgziggy1 = new String[]{"/[label(Shion)]", "Oh, MOMO! Have you seen Ziggy?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy2 = new String[]{"/[label(MOMO)]", "Ziggy? Please wait a minute.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "I found him! He just got off the elevator and is walking down the corridor below.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy3 = new String[]{"/[label(Shion)]", "Wow! You really are amazing, MOMO.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy4 = new String[]{"/[label(MOMO)]", "Hee hee hee, thank you very much!", "/[waitkey(64)]/[close()]"};
    String[] msgziggy5 = new String[]{"/[label(Shion)]", "By the way, what are you doing here?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy6 = new String[]{"/[label(chaos)]", "MOMO, have you seen Ziggy?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy7 = new String[]{"/[label(MOMO)]", "Ziggy? Please wait a minute.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "I found him! He just got off the elevator and is walking down the corridor below.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy8 = new String[]{"/[label(chaos)]", "Thank you. You really are amazing, MOMO.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy9 = new String[]{"/[label(MOMO)]", "Hee hee hee, thank you very much!", "/[waitkey(64)]/[close()]"};
    String[] msgziggy10 = new String[]{"/[label(chaos)]", "By the way, what are you doing here?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy11 = new String[]{"/[label(MOMO)]", "Oh, um, I was cleaning.", "/[waitkey(1)]/[clear()]", "The room was messy, so I thought I would tidy it up.", "/[waitkey(1)]/[clear()]", "Was that wrong of me?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy12 = new String[]{"/[label(chaos)]", "Not at all, nothing wrong with that. I'm sure the Captain and the others will be happy.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy13 = new String[]{"/[label(MOMO)]", "Good! I'll work hard!", "/[waitkey(64)]/[close()]"};
    String[] msgziggy14 = new String[]{"/[label(MOMO)]", "I'll be very happy if it makes everyone happy!", "/[waitkey(64)]/[close()]"};
    String[] msg0212875E = new String[]{"/[label(Shion)]", "MOMO, what are you doing?", "/[waitkey(64)]/[clear()]"};
    String[] msg0212875F = new String[]{"/[label(MOMO)]", "Oh, um, I was cleaning.", "/[waitkey(1)]/[clear()]", "The room was messy, so I thought I would tidy it up.", "/[waitkey(1)]/[clear()]", "Was that wrong of me?", "/[waitkey(64)]/[clear()]"};
    String[] msg02128760 = new String[]{"/[label(Shion)]", "Oh, not at all. MOMO, I bet you'll make a lucky guy very happy someday.", "/[waitkey(64)]/[clear()]"};
    String[] msg02128761 = new String[]{"/[label(MOMO)]", "Hee hee! I hope so!", "/[waitkey(64)]/[close()]"};
    String[] msg02128762 = new String[]{"/[label(MOMO)]", "If I work hard and become a good wife, it will make Ziggy happy too, won't it?!", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO1 = new String[]{"/[label(MOMO)]", "Oh, are you all going out?", "/[waitkey(1)]/[clear()]", "Then I will clean your rooms while you are gone.", "/[waitkey(64)]/[close()]"};
    String[] msg1436E833 = new String[]{"/[label(Cherenkov)]", "Sorry, leave me alone for a while.", "/[waitkey(64)]/[close()]"};
    String[] msg043747DF = new String[]{"/[label(Cherenkov)]", "My body...I mean, my head...hurts. Please...leave me alone...", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT = new String[]{"/[label()]", "You seem quite worn out. Don't push yourself too hard, it's bad for your health. How about getting some good rest?", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT1 = new String[]{"/[label()]", "Yes, I think that will be for the best. Pushing yourself will not result in anything good. Well then, good night...", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT2 = new String[]{"/[label()]", "I see, that's too bad. Please don't push yourself too hard.", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Cabin 1'", "/[waitkey(64)]/[close()]"};

    ST0590() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.165f, 1.183f, -4.059f);
        this.camEV.setRotate(0.979f, 142.756f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.06f, 1.197f, 2.971f);
        this.camEV.setRotate(-1.488f, 208.458f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.866f, 1.431f, 0.074f);
        this.camEV.setRotate(0.011f, -89.494f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 2: {
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
                ST0590.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(16);
                this.fade2.call(0);
                this.player.setTranslate(6.66f, 0.0f, -0.08f);
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
        ST0590.waitPage(window, 64);
    }

    public void Talk_npc15(Enepc enepc) {
        this.Talk_npc15_1();
    }

    void Talk_npc15_1() {
        this.npc15.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNIGHT, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Rest\nDon't rest");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgNIGHT1, 0);
                ST0590.waitPage(this.win, 64);
                System.sleep(15);
                Runtime.charAllRecovery();
                this.fade1.call(0);
                System.sleep(60);
                this.fade2.call(0);
                System.sleep(60);
                Sound.effectPlay(26);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_01, 0);
                ST0590.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNIGHT2, 0);
        ST0590.waitPage(this.win, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc5_7(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc5_6(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc5_5(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc5_4(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc5_2(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc5_1(Window window) {
    }

    void Talk_npc5_2(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg01B81D16, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msg01B81D17, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msg01B81D18, 0);
                ST0590.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg00B81D19, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msg00B81D1A, 0);
                ST0590.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg01B87CC7, 0);
        ST0590.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg00B87CC8, 0);
        ST0590.waitPage(window, 64);
        window.print(this.msg00B87CC9, 0);
        ST0590.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc5_3(Window window) {
    }

    void Talk_npc5_4(Window window) {
        if (Runtime.getLeader() == 1) {
            ++this.talkFlag5;
            switch (this.talkFlag5) {
                case 1: {
                    window.print(this.msg02B81D16, 0);
                    ST0590.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg00B87CC2, 0);
            ST0590.waitPage(window, 64);
            return;
        }
        window.print(this.msg00B87CC21, 0);
        ST0590.waitPage(window, 64);
    }

    void Talk_npc5_5(Window window) {
    }

    void Talk_npc5_6(Window window) {
    }

    void Talk_npc5_7(Window window) {
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc6_7(window);
        } else if (this.ZIGGY == 1) {
            this.Talk_npc6_61(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc6_6(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc6_5(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc6_4(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc6_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc6_2(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc6_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc6_1(Window window) {
    }

    void Talk_npc6_2(Window window) {
    }

    void Talk_npc6_3(Window window) {
    }

    void Talk_npc6_4(Window window) {
    }

    void Talk_npc6_5(Window window) {
    }

    void Talk_npc6_6(Window window) {
        if (Runtime.getLeader() == 1) {
            ++this.talkFlag6;
            switch (this.talkFlag6) {
                case 1: {
                    window.print(this.msgziggy1, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msgziggy2, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msgziggy3, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msgziggy4, 0);
                    ST0590.waitPage(window, 64);
                    return;
                }
                case 2: {
                    window.print(this.msgziggy5, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg0212875F, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg02128760, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg02128761, 0);
                    ST0590.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg02128762, 0);
            ST0590.waitPage(window, 64);
            return;
        }
        ++this.talkFlag62;
        switch (this.talkFlag62) {
            case 1: {
                window.print(this.msgziggy6, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy7, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy8, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy9, 0);
                ST0590.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msgziggy10, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy11, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy12, 0);
                ST0590.waitPage(window, 64);
                window.print(this.msgziggy13, 0);
                ST0590.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msgziggy14, 0);
        ST0590.waitPage(window, 64);
    }

    void Talk_npc6_61(Window window) {
        if (Runtime.getLeader() == 1) {
            ++this.talkFlag6;
            switch (this.talkFlag6) {
                case 1: {
                    window.print(this.msg0212875E, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg0212875F, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg02128760, 0);
                    ST0590.waitPage(window, 64);
                    window.print(this.msg02128761, 0);
                    ST0590.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg02128762, 0);
            ST0590.waitPage(window, 64);
            return;
        }
        window.print(this.msgMOMO1, 0);
        ST0590.waitPage(window, 64);
    }

    void Talk_npc6_7(Window window) {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc8_7(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc8_6(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc8_5(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc8_4(window);
        } else if (this.S2028 == 1) {
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
        ST0590.waitPage(window, 64);
        window.print(this.msg0436E834, 0);
        ST0590.waitPage(window, 64);
        window.print(this.msg0436E835, 0);
        ST0590.waitPage(window, 64);
        window.print(this.msg0436E836, 0);
        ST0590.waitPage(window, 64);
        window.print(this.msg0436E837, 0);
        ST0590.waitPage(window, 64);
    }

    void Talk_npc8_4(Window window) {
    }

    void Talk_npc8_5(Window window) {
    }

    void Talk_npc8_6(Window window) {
    }

    void Talk_npc8_7(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                this.npc8.kickEnepc(9, 100);
                window.print(this.msg1436E833, 0);
                ST0590.waitPage(window, 64);
                this.npc8.kickEnepc(9, -1);
                return;
            }
        }
        this.npc8.kickEnepc(9, 100);
        window.print(this.msg043747DF, 0);
        ST0590.waitPage(window, 64);
        this.npc8.kickEnepc(9, -1);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S2030 == 1) {
                    Runtime.jumpCF(520, 3);
                    break;
                }
                if (this.S2028 == 1) {
                    Runtime.jumpCF(670, 3);
                    break;
                }
                Runtime.jumpCF(520, 3);
                break;
            }
            case 1: {
                if (this.S2030 == 1) {
                    Runtime.jumpCF(520, 4);
                    break;
                }
                if (this.S2028 == 1) {
                    Runtime.jumpCF(670, 4);
                    break;
                }
                Runtime.jumpCF(520, 4);
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
        this.monitor1 = new Obj();
        this.monitor1.init(24613, 7.86f, 1.17f, 0.07f, 270.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18009, 0, 512, 260);
        this.monitor1.setArgs(2, 120, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Obj();
        this.monitor2.init(24613, 7.86f, 1.17f, 0.07f, 270.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18013, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setScale(0.8f, 0.8f, 1.0f);
        this.monitor2.setScale(0.8f, 0.8f, 1.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196626);
        this.teiten2 = new Uwamono(28690, 6.0f, 0.0f, -5.0f, 0.0f);
        this.teiten2.SetBgm(196627);
        this.teiten3 = new Uwamono(28690, 8.0f, 0.0f, 0.0f, 0.0f);
        this.teiten3.SetBgm(196627);
        this.teiten4 = new Uwamono(28690, 6.0f, 0.0f, 5.0f, 0.0f);
        this.teiten4.SetBgm(196627);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 12.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.5f, 45.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.5f, 45.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 5.5f, 45.0f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFPedestal(5, -6.45f, 4.0f, 1.9f, 50.0f, -62.23f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(5, 1);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 5.5f, 45.0f);
        this.cam0.setCFHokan(6, 0.03f, 0.03f);
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
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.doorA = new Uwamono(100, 40, '\u0001');
        this.doorB = new Uwamono(101, 40, '\u0002');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_7();
        } else if (this.S2040C == 1) {
            this.npcset_6();
        } else if (this.S2030 == 1) {
            this.npcset_5();
        } else if (this.MOMO == 1) {
            this.npcset_4();
        } else if (this.S2028 == 1) {
            this.npcset_3();
        } else if (this.S2013B == 1) {
            this.npcset_0();
        } else if (this.S2013 == 1) {
            this.npcset_2();
        } else if (this.S2009 == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
        this.npc15 = new NPC_NORMAL(1612, 15, 0, 14, 16, -5.22f, 0.1f, -4.94f, 45.0f);
        this.npc15.talkto("Talk_npc15");
        this.npc15.setMotion(0, 10);
        this.npc15.setInvalidID(1);
    }

    void npcset_0() {
    }

    void npcset_1() {
    }

    void npcset_2() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 1, 3, 0.94f, 0.0f, 1.74f, 0.0f);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_3() {
        this.npc8 = new NPC_NORMAL(279, 8, 0, 5, 9, 0.94f, 0.0f, 1.74f, 0.0f);
        this.npc8.talkto("Talk_npc8");
    }

    void npcset_4() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 1, 3, 0.94f, 0.0f, 1.74f, 0.0f);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_5() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, 0.19f, 0.0f, -1.59f, 90.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, 0.5f, 0.0f, 1.98f, 180.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 11, 0.85f, 0.0f, -1.06f, 135.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.kickEnepc(4, 1);
        this.npc6.disableDTKFlag(131072);
        this.npc6.disableDTKFlag(65536);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc7.disableDTKFlag(131072);
        this.npc7.disableDTKFlag(65536);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        if (Runtime.getFlags(7024, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 11, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_6() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 6, 5, -0.48f, 0.0f, -1.65f, 0.0f);
        this.npc6.talkto("Talk_npc6");
    }

    void npcset_7() {
        this.npc8 = new NPC_NORMAL2(279, 8, 0, 76, 9, -3.0f, 0.75f, -5.1f, 0.0f);
        this.npc20 = new NPC_NORMAL(279, 20, 0, 76, 9, -3.96f, 0.0f, -5.81f, 0.0f);
        this.npc8.disableDTKFlag(3);
        this.npc8.setInvalidID(1);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc8.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc8.moveEnepc(18, 45.0f, 0.0f, 0);
        this.npc8.talkto("Talk_npc8");
        this.npc20.talkto("Talk_npc8");
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

    class Obj
            extends Unit {
        Obj() {
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

        void ANNAI() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7024, 1, 1);
            Runtime.enable(65536);
            ST0590.this.cam0.setMode(-1);
            ST0590.this.EV_Camera01();
            ST0590.this.npc10.kickEnepc(0, 10);
            ST0590.this.npc6.kickEnepc(1, 27);
            ST0590.this.npc7.kickEnepc(1, 27);
            ST0590.this.npc6.setVisible(true);
            ST0590.this.npc7.setVisible(true);
            ST0590.this.npc10.setVisible(true);
            System.sleep(10);
            ST0590.this.win = Window.create();
            ST0590.this.win.setSize(4, 45);
            ST0590.this.win.setLocation(15, 305);
            ST0590.this.win.print(ST0590.this.msg0436E83C, 0);
            ST0590.waitPage(ST0590.this.win, 64);
            ST0590.this.npc10.moveEnepc(17, 180.0f, 0.1f, 20);
            ST0590.this.npc10.kickEnepc(0, 9);
            ST0590.this.win.print(ST0590.this.msg1436E83C, 0);
            ST0590.waitPage(ST0590.this.win, 64);
            ST0590.this.npc6.kickEnepc(0, 9);
            ST0590.this.npc6.moveEnepc(17, 60.0f, -0.1f, 20);
            ST0590.this.win.print(ST0590.this.msg0436E83E, 0);
            ST0590.waitPage(ST0590.this.win, 64);
            ST0590.this.npc10.kickEnepc(0, 10);
            ST0590.this.win.print(ST0590.this.msg0436E840, 0);
            ST0590.waitPage(ST0590.this.win, 64);
            System.sleep(10);
            ST0590.this.fade.call(0);
            System.sleep(30);
            ST0590.this.npc6.setVisible(false);
            ST0590.this.npc7.setVisible(false);
            ST0590.this.npc10.setVisible(false);
            ST0590.this.player.setTranslate(0.26f, 0.0f, -0.86f);
            ST0590.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }

    class NPC_NORMAL2
            extends Enepc {
        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
        }

        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(0, 0);
        }
    }
}

