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
import xeno.map.MC_ELS03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0536
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS03_PRJ {
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
    Enepc npc15;
    Enepc npc16;
    Enepc npc20;
    Enepc npc21;
    Enepc sakana3;
    Enepc sakana2;
    Enepc sakana1;
    Unit unit1;
    Unit sara1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono doorA;
    Uwamono tA;
    MAPUnit doorR1;
    MAPUnit doorL1;
    MAPUnit doorR2;
    MAPUnit doorL2;
    int doormove = 0;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Unit curry;
    Effect EFcurry;
    Unit spoon;
    int page;
    String[] msg0102DABF = new String[]{"/[label(Matthews)]", "*Burp* Looks like I ate a bit too much. It's not every day you get to eat curry that bad.", "/[waitkey(64)]/[close()]"};
    String[] msg0102DABF1 = new String[]{"/[label(Matthews)]", "More importantly, what the hell happened? A military ship with combat weapons and civilian technicians. That sure doesn't look like nothing to me. Are we talking experiments with prototype weapons here?", "/[waitkey(64)]/[clear()]"};
    String[] msg0102DAC0 = new String[]{"/[label(Shion)]", "Very shrewd of you. You're half correct. It's true that KOS-MOS was assigned as an actual combat unit with the intention of testing her. But unfortunately, I just receive orders and have no idea what goes on between the higher ranking military officials and my company.", "/[waitkey(64)]/[clear()]"};
    String[] msg0102DAC1 = new String[]{"/[label(Matthews)]", "Oh, is that right?", "/[waitkey(64)]/[close()]"};
    String[] msg0102DAC11 = new String[]{"/[label(Matthews)]", "Well, let's leave it at that. I've got no complaints as long as we get paid.", "/[waitkey(64)]/[close()]"};
    String[] msg0202DABF = new String[]{"/[label(Matthews)]", "Hey Ms. Vector, I don't know what happened to you guys, and I have no interest in finding out. But we're not a charity. You better be able to pay up.", "/[waitkey(64)]/[clear()]"};
    String[] msg0202DAC0 = new String[]{"/[label(Shion)]", "Oh, yes, of course. You have a guarantee from Vector.", "/[waitkey(1)]/[clear()]", "But you know, you seem so preoccupied with money. Are you in some kind of trouble?", "/[waitkey(64)]/[clear()]"};
    String[] msg0202DAC1 = new String[]{"/[label(Matthews)]", "Grr...t-that's none of your business! It's my prerogative whether I be knee-deep in quicksand or up to my ears in debt!", "/[waitkey(64)]/[close()]"};
    String[] msg01033A6E = new String[]{"/[label(Matthews)]", "Listen, Ms. Vector. Right now, you're a customer of mine, but if you stick your nose where it doesn't belong, I won't hesitate to toss you off this ship. Got it?!", "/[waitkey(64)]/[close()]"};
    String[] msg0103AED0 = new String[]{"/[label(Shion)]", "Um, Captain? Please let me know if there is anything I can do.", "/[waitkey(64)]/[clear()]"};
    String[] msg0103AED1 = new String[]{"/[label(Matthews)]", "Well, for starters, keep that puppet of yours in check. I can't afford to have her running around freely and wrecking the Elsa again.", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E81 = new String[]{"/[label(Matthews)]", "What? You're going to give a tour of the ship, right? Hurry up and go then.", "/[waitkey(1)]/[clear()]", "Sheesh...first, we got a combat weapon and a buncha weird scientists, and now we have a cyborg and a Realian. Why are all these troublesome people flocking\nto me?", "/[waitkey(64)]/[close()]"};
    String[] msg142F9E81 = new String[]{"/[label(Matthews)]", "Man, I don't need any more headaches.", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E82 = new String[]{"/[label(Hammer)]", "Great job! That's KOS-MOS for ya. And MOMO's amazing too! What a huge difference compared to our useless Tony here!", "/[waitkey(64)]/[close()]"};
    String[] msg00BA4129 = new String[]{"/[label(Tony)]", "I can't believe this! I'm actually getting all riled up being compared to a little munchkin like her.", "/[waitkey(64)]/[close()]"};
    String[] msg00BAA0D5 = new String[]{"/[label(Tony)]", "Hey, MOMO, how about we have a little match next time?", "/[waitkey(64)]/[close()]"};
    String[] msg00BAA0D7 = new String[]{"/[label(chaos)]", "They have foul mouths, but they aren't as bad as they look. Don't let them bother you.", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Chi-ef...thank goodness you're safe!", "/[waitkey(1)]/[clear()]", "If something were to happen to you, I'd...I'd...", "/[waitkey(64)]/[close()]"};
    String[] msg10B81D16 = new String[]{"/[label(Allen)]", "Chi-ef...please don't do anything rash like that again.", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "...A 100-Series Realian, and a prototype at that. I never thought I'd meet one in a place like this. Looks like I've got the Devil's own luck.", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D15 = new String[]{"/[label(Shion)]", "Um, Allen?\n", "/[waitkey(64)]/[clear()]"};
    String[] msg01B81D16 = new String[]{"/[label(Allen)]", "Pwease weave me awone.\n", "(Please leave me alone.)", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D17 = new String[]{"/[label(Shion)]", "Listen, Allen. Commander Cherenkov has disappeared.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D18 = new String[]{"/[label(Allen)]", "Dat haph\n", "(That has)", "/[waitkey(1)]/[clear()]", "noffin to woo wiff we.\n", "(nothing to do with me.)\n", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D19 = new String[]{"/[label(Allen)]", "Oo\n", "(You)\n", "/[waitkey(1)]/[clear()]", "air mowa abou da commama dan we!\n", "(care more about the Commander than me!)\n", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D1B = new String[]{"/[label(Shion)]", "A-Allen? Talk or eat, pick one, okay?", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC8 = new String[]{"/[label(Allen)]", "Ai'm wonna ea oo muj an' kiw ma fewf.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC9 = new String[]{"/[label(chaos)]", "Is he going to be okay? He seems to be taking it pretty hard.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CCA = new String[]{"/[label(Shion)]", "Oh, he'll be just fine. Allen might not look it, but he's actually surprisingly tough. Not even a swarm of Gnosis could kill him.", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CCB = new String[]{"/[label(Allen)]", "Ugh, uh, un...Jief...", "/[waitkey(64)]/[close()]"};
    String[] msgROBO1 = new String[]{"/[label()]", "Hey, welcome! What'll it be?", "/[waitkey(1)]/[clear()]", "You guys are lucky! We got some good stuff in today!", "/[waitkey(1)]/[clear()]", "It's the passport for the Card Game, and it comes with a Starter Set! It's a great deal! You hardly ever come across these things nowadays.", "/[waitkey(1)]/[clear()]", "Oh, don't worry about the price. Call it a freebie!", "/[waitkey(64)]/[close()]"};
    String[] msgROBO2 = new String[]{"/[label()]", "Hello!", "/[waitkey(1)]/[clear()]", "I'm a top-of-the-line bartending robot! I'll make any kind of drink you want!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};

    ST0536() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.107f, 1.835f, 2.731f);
        this.camEV.setRotate(-14.77f, 29.519f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 9: {
                this.sakana3.kickEnepc(10, 270, 0);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                System.println("0");
                this.doormove = 1;
                break;
            }
            case 1: {
                if (Runtime.getFlags(123, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(124, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7057, 2) != 2) {
                    return;
                }
                if (Runtime.getFlags(7143, 1) != 0) break;
                Runtime.mailArriveSet(24);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7143, 1, 1);
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
        }
    }

    void Talk_no() {
        this.win.print(this.msgtalk_no, 0);
        ST0536.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0536.waitPage(window, 64);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc1_5(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc1_4(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc1_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc1_2(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc1_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    public void Talk_npc15(Enepc enepc, Window window) {
        this.Talk_npc15_1(window);
    }

    void Talk_npc15_1(Window window) {
        this.npc15.kickEnepc(4, 1);
        this.npc15.kickEnepc(1, 9);
        if (Runtime.checkItem(10, 13) == 0) {
            window.print(this.msgROBO1, 0);
            ST0536.waitPage(window, 64);
            System.sleep(15);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 13);
            System.sleep(30);
            this.npc15.kickEnepc(4, 0);
            return;
        }
        window.print(this.msgROBO2, 0);
        ST0536.waitPage(window, 64);
        this.npc15.kickEnepc(4, 0);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0102DABF, 0);
                ST0536.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0102DABF1, 0);
                ST0536.waitPage(window, 64);
                window.print(this.msg0102DAC0, 0);
                ST0536.waitPage(window, 64);
                window.print(this.msg0102DAC1, 0);
                ST0536.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0102DAC11, 0);
        ST0536.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0202DABF, 0);
                ST0536.waitPage(window, 64);
                window.print(this.msg0202DAC0, 0);
                ST0536.waitPage(window, 64);
                window.print(this.msg0202DAC1, 0);
                ST0536.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg01033A6E, 0);
                ST0536.waitPage(window, 64);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg0103AED0, 0);
        ST0536.waitPage(window, 64);
        window.print(this.msg0103AED1, 0);
        ST0536.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc1_3(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg042F9E81, 0);
                ST0536.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg142F9E81, 0);
        ST0536.waitPage(window, 64);
    }

    void Talk_npc1_4(Window window) {
    }

    void Talk_npc1_5(Window window) {
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc2_5(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc2_4(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc2_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc2_2(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc2_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc2_1(Window window) {
    }

    void Talk_npc2_2(Window window) {
    }

    void Talk_npc2_3(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg00BA4129, 0);
                ST0536.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00BAA0D5, 0);
        ST0536.waitPage(window, 64);
    }

    void Talk_npc2_4(Window window) {
    }

    void Talk_npc2_5(Window window) {
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc3_5(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc3_4(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc3_2(window);
        } else if (this.S2009 == 1) {
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
        this.npc3.kickEnepc(9, 100);
        window.print(this.msg042F9E82, 0);
        ST0536.waitPage(window, 64);
        this.npc3.kickEnepc(9, -1);
    }

    void Talk_npc3_4(Window window) {
    }

    void Talk_npc3_5(Window window) {
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc4_5(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc4_4(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc4_2(window);
        } else if (this.S2009 == 1) {
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
        window.print(this.msg00BAA0D7, 0);
        ST0536.waitPage(window, 64);
    }

    void Talk_npc4_4(Window window) {
    }

    void Talk_npc4_5(Window window) {
    }

    public void Talk_npc5(Enepc enepc) {
        if (this.S2057 == 1) {
            this.Talk_no();
        } else if (this.S2042 == 1) {
            this.Talk_npc5_5();
        } else if (this.S2040C == 1) {
            this.Talk_npc5_4();
        } else if (this.S2030 == 1) {
            this.Talk_npc5_3();
        } else if (this.S2013B == 1) {
            this.Talk_no(this.win);
        } else if (this.S2013 == 1) {
            this.Talk_npc5_2();
        } else if (this.S2009 == 1) {
            this.Talk_npc5_1();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc5_1() {
    }

    void Talk_npc5_2() {
    }

    void Talk_npc5_3() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00B81D16, 0);
                ST0536.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10B81D16, 0);
        ST0536.waitPage(this.win, 64);
    }

    void Talk_npc5_4() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.enable(65536);
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.npc4.setVisible(true);
                this.npc20.setVisible(true);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00B81D15, 0);
                ST0536.waitPage(this.win, 64);
                this.win.print(this.msg01B81D16, 0);
                ST0536.waitPage(this.win, 64);
                this.npc20.kickEnepc(0, 9);
                this.win.print(this.msg00B81D17, 0);
                ST0536.waitPage(this.win, 64);
                this.win.print(this.msg00B81D18, 0);
                ST0536.waitPage(this.win, 64);
                this.win.print(this.msg00B81D19, 0);
                ST0536.waitPage(this.win, 64);
                this.npc20.kickEnepc(0, 10);
                this.win.print(this.msg00B81D1B, 0);
                ST0536.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                this.npc4.setVisible(false);
                this.npc20.setVisible(false);
                this.player.setTranslate(-1.184f, 0.0f, 0.55f);
                this.cam0.setMode(0);
                Runtime.disable(65536);
                return;
            }
        }
        this.fade.call(0);
        System.sleep(30);
        Runtime.enable(65536);
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
        this.npc4.setVisible(true);
        this.npc20.setVisible(true);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00B87CC8, 0);
        ST0536.waitPage(this.win, 64);
        this.npc4.kickEnepc(0, 9);
        this.npc4.moveEnepc(17, 360.0f, 0.1f, 20);
        this.win.print(this.msg00B87CC9, 0);
        ST0536.waitPage(this.win, 64);
        this.npc20.kickEnepc(1, 9);
        this.npc20.moveEnepc(17, 210.0f, -0.1f, 20);
        this.win.print(this.msg00B87CCA, 0);
        ST0536.waitPage(this.win, 64);
        this.fade.call(0);
        System.sleep(30);
        this.npc4.setVisible(false);
        this.npc20.setVisible(false);
        this.npc4.kickEnepc(0, 0);
        this.npc4.moveEnepc(17, 290.0f, -0.1f, 0);
        this.npc20.kickEnepc(0, 0);
        this.npc20.moveEnepc(17, 240.0f, 0.1f, 0);
        this.player.setTranslate(-1.184f, 0.0f, 0.55f);
        this.cam0.setMode(0);
        Runtime.disable(65536);
    }

    void Talk_npc5_5() {
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
        ST0536.waitPage(window, 64);
    }

    void Talk_npc8_4(Window window) {
    }

    void Talk_npc8_5(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66206, 8);
                break;
            }
            case 1: {
                Runtime.jumpCF(66152, 2);
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
        Stage.renderCommand(4);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -3.0f, 0.0f, -6.0f, 0.0f);
        this.teiten1.SetBgm(196616);
        this.teiten2 = new Uwamono(28690, 0.0f, 0.0f, 7.0f, 0.0f);
        this.teiten2.SetBgm(196617);
        this.teiten3 = new Uwamono(28690, -3.0f, 0.0f, -3.3f, 0.0f);
        this.teiten3.SetBgm(196634);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
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
        this.eve00 = new Effect(1424, 0);
        this.eve00.disp(true);
        this.eve00.setScale(0.8f, 0.8f, 0.8f);
        this.eve01 = new Effect(1424, 1);
        this.eve01.disp(true);
        this.eve01.setScale(0.8f, 0.8f, 0.8f);
        this.eve02 = new Effect(1424, 2);
        this.eve02.disp(true);
        this.eve02.setScale(0.8f, 0.8f, 0.8f);
        Runtime.progressEffect(20);
        this.doorA = new Uwamono(32, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorR1 = new Mapunits();
        this.doorR1.mapUnit(1);
        this.doorR1.start(4, null);
        this.doorR1.start(1, "automatic_door");
        this.doorL1 = new Mapunits();
        this.doorL1.mapUnit(2);
        this.doorL1.start(4, null);
        this.doorR2 = new Mapunits();
        this.doorR2.mapUnit(3);
        this.doorR2.start(4, null);
        this.doorL2 = new Mapunits();
        this.doorL2.mapUnit(4);
        this.doorL2.start(4, null);
        this.tA = new Uwamono(28674, 0.0f, -1.5f, -7.875f, 0.0f);
        this.tA.SetSize(3.4f, 2.0f, 3.4f);
        this.tA.SetBroken(false);
        if (Runtime.getFlags(143, 1) == 1 && Runtime.getFlags(7052, 1) == 0) {
            Runtime.setOutFriend(2);
            Runtime.setOutFriend(3);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 5);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 6);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(7052, 1, 1);
        }
        this.npcset_6();
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_5();
        } else if (this.S2040C == 1) {
            this.npcset_4();
        } else if (this.S2030 == 1) {
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
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 76, 10, 1.68f, 0.1f, 0.38f, 330.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc1.setMotion(0, 3);
        this.npc1.talkto("Talk_npc1");
    }

    void npcset_10() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 76, 10, 1.68f, 0.1f, 0.38f, 330.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc1.setMotion(0, 3);
        this.npc1.talkto("Talk_npc1");
        this.sakana3 = new NPC_NORMAL2(20594, 9, 12, 12, 15, -2.999f, 1.6f, -3.35f, 270.0f);
        this.sakana3.talkto("Talk_sakana3");
        this.sakana3.setInvalidID(1);
        this.sakana3.dispRadar(false);
        this.sakana2 = new NPC_NORMAL2(20592, 11, 13, 12, 15, -2.148f, 1.5f, -3.25f, 270.0f);
        this.sakana2.talkto("Talk_sakana2");
        this.sakana2.setInvalidID(1);
        this.sakana2.dispRadar(false);
        this.sakana1 = new NPC_NORMAL2(20592, 12, 11, 12, 15, -3.448f, 1.8f, -3.15f, 90.0f);
        this.sakana1.talkto("Talk_sakana1");
        this.sakana1.setInvalidID(1);
        this.sakana1.dispRadar(false);
    }

    void npcset_2() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 76, 10, 1.68f, 0.1f, 0.38f, 330.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc1.setMotion(0, 3);
        this.npc1.talkto("Talk_npc1");
    }

    void npcset_3() {
        this.npc1 = new NPC_NORMAL2(275, 1, 0, 76, 10, 1.68f, 0.1f, 0.38f, 330.0f);
        this.npc2 = new NPC_NORMAL2(277, 2, 0, 76, 10, 1.58f, 0.0f, -2.61f, 0.0f);
        this.npc3 = new NPC_NORMAL2(278, 3, 0, 76, 10, 2.6f, 0.0f, 1.58f, 180.0f);
        this.npc4 = new NPC_NORMAL(3, 4, 0, 13, 6, -3.41f, 0.0f, 4.29f, 120.0f);
        this.npc5 = new NPC_NORMAL2(263, 5, 0, 76, 10, 2.2f, 0.0f, -2.51f, 0.0f);
        this.npc20 = new NPC_NORMAL2(263, 20, 0, 76, 10, 2.2f, 0.0f, -3.11f, 0.0f);
        this.npc21 = new NPC_NORMAL2(278, 21, 0, 76, 10, 1.3f, 0.0f, 1.58f, 9.0f);
        this.npc20.setVisible(false);
        this.npc21.setVisible(false);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc1.setMotion(0, 3);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 5);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setInvalidID(1);
        this.npc3.setMotion(0, 4);
        this.npc4.setMotion(0, 10);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setInvalidID(1);
        this.npc5.setMotion(0, 1);
        this.npc20.setInvalidID(1);
        this.npc21.setInvalidID(1);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc20.talkto("Talk_npc5");
        this.npc21.talkto("Talk_npc3");
    }

    void npcset_4() {
        this.sara1 = new Unit();
        this.sara1.init(24628, -2.15f, 1.175f, 0.1f, 0.0f);
        this.npc4 = new NPC_NORMAL(3, 4, 0, 13, 6, -1.18f, 0.0f, -0.66f, 290.0f);
        this.npc5 = new NPC_NORMAL(263, 5, 0, 13, 17, -1.78f, 0.0f, 0.1f, 270.0f);
        this.spoon = new Obj();
        this.spoon.init(24629, 0.0f, 0.0f, 0.0f, 0.0f);
        this.spoon.start(4, null);
        this.spoon.setTranslate(0.147f, -0.03f, 0.043f);
        this.spoon.setRotate(-36.998f, -59.294f, -15.198f);
        this.spoon.setParent(this.npc5, 72);
        this.spoon.setVisible(true);
        this.npc20 = new NPC_NORMAL(1, 4, 0, 13, 9, -1.18f, 0.0f, 0.55f, 240.0f);
        this.npc4.enableDTKFlag(262144);
        this.npc4.kickEnepc(4, 1);
        this.npc4.setVisible(false);
        this.npc20.enableDTKFlag(262144);
        this.npc20.kickEnepc(4, 1);
        this.npc20.setVisible(false);
        this.npc5.disableDTKFlag(3);
        this.npc5.setInvalidID(1);
        this.npc5.setMotion(0, 2);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_5() {
    }

    void npcset_6() {
        this.npc15 = new NPC_NORMAL(1612, 15, 0, 13, 19, -3.14f, 0.0f, 0.13f, 90.0f);
        this.npc15.setMotion(0, 9);
        this.npc15.setMotion(0, 27);
        this.npc15.setInvalidID(1);
        this.npc15.enableDTKFlag(262144);
        this.npc15.renderCommand(23);
        this.npc16 = new NPC_NORMAL(1612, 16, 0, 13, 19, -1.84f, 0.0f, 0.13f, 90.0f);
        this.npc16.setInvalidID(1);
        this.npc16.setVisible(false);
        this.npc16.talkto("Talk_npc15");
        this.npc16.renderCommand(23);
        this.sakana3 = new NPC_NORMAL2(20594, 9, 12, 72, 15, -2.999f, 2.0f, -3.35f, 270.0f);
        this.sakana3.talkto("Talk_sakana3");
        this.sakana3.enableDTKFlag(262144);
        this.sakana3.dispRadar(false);
        this.sakana3.kickEnepc(10, 270, 0);
        this.sakana3.renderCommand(23);
        this.sakana2 = new NPC_NORMAL2(20592, 11, 13, 75, 15, -2.148f, 2.0f, -3.25f, 270.0f);
        this.sakana2.talkto("Talk_sakana2");
        this.sakana2.dispRadar(false);
        this.sakana2.renderCommand(23);
        this.sakana1 = new NPC_NORMAL2(20592, 12, 11, 75, 15, -3.448f, 2.0f, -3.15f, 90.0f);
        this.sakana1.talkto("Talk_sakana1");
        this.sakana1.dispRadar(false);
        this.sakana1.renderCommand(23);
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
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }

    class NPC_NORMAL2
            extends Enepc {
        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void automatic_door() {
            float f = 0.0f;
            float f2 = 0.0f;
            int n = 7;
            int n2 = 15;
            int n3 = 45;
            while (true) {
                ST0536.this.player.getTranslate();
                f2 = (0.0f - ST0536.this.player.px) * (0.0f - ST0536.this.player.px) + (-7.875f - ST0536.this.player.pz) * (-7.875f - ST0536.this.player.pz);
                if (f2 < 4.0f && ST0536.this.doormove == 2) {
                    ST0536.this.doormove = 1;
                }
                if (f2 > 9.0f) {
                    ST0536.this.doormove = 2;
                }
                if (ST0536.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196715);
                    }
                    if (f <= (float) n2) {
                        ST0536.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0536.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0536.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0536.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0536.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == (float) n2) {
                        Sound.effectPlay(196715);
                    }
                    if (f <= (float) n2) {
                        ST0536.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0536.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0536.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0536.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0536.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0536.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0536.this.tA.setTranslate(0.0f, -3.5f, -7.875f);
                } else {
                    ST0536.this.tA.setTranslate(0.0f, -1.5f, -7.875f);
                }
                System.sleep(1);
            }
        }
    }
}

