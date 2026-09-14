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
import xeno.map.MC_KUK12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2120
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK12_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    int selected1 = 0;
    int selected2 = 0;
    int selected3 = 0;
    int selected4 = 0;
    Unit AGWS1;
    Unit AGWS2;
    Unit AGWS3;
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
    Enepc npc20;
    Enepc Sco;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int talkFlag9;
    int talkFlag10;
    int talkFlag20;
    Uwamono doorA;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Effect denwa;
    Effect game;
    Effect EF01;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono teiten1;
    int page;
    String[] SCO = new String[]{"Flag ON (for debugging)", "/[waitkey(64)]/[close()]"};
    String[] msgTEST = new String[]{"/[label()]", "Hey, hey, there's work going on here! Don't come in, it's dangerous.", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Welcome! Count on us for all your A.G.W.S. needs! How may I help you?", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "Thank you very much. We look forward to serving you again!", "/[waitkey(64)]/[close()]"};
    String[] msg11001 = new String[]{"/[label()]", "Welcome! We will remain open, regardless of whether we are under attack, as long as there are customers! How may I help you?", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Take a look at this. It's Vector's popular model, the VX Series. It's awesome. Those beautiful lines are irresistible!", "/[waitkey(64)]/[close()]"};
    String[] msg21001 = new String[]{"/[label()]", "Hey, don't bother me! The enemy is right on our doorstep. I don't have time to be fooling around with you!", "/[waitkey(1)]/[clear()]", "I have to get this ready so we're ready to launch at a moment's notice. It'll be too late after the enemy attacks!", "/[waitkey(64)]/[close()]"};
    String[] msg22001 = new String[]{"/[label()]", "Thank goodness. I don't know what I would've done if this brand new one got scratched up. I want it to stay in mint condition.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "The facilities here are first-rate, even comparable to the military's hangars.", "/[waitkey(1)]/[clear()]", "We have plenty of different parts, and we can even restore FHP.", "/[waitkey(64)]/[close()]"};
    String[] msg31001 = new String[]{"/[label()]", "Oh, be careful. That's dangerous! The A.G.W.S. are preparing for launch right now.", "/[waitkey(1)]/[clear()]", "Fight the Gnosis?! No, don't be ridiculous. We're moving them to a safe place so they don't get damaged.", "/[waitkey(64)]/[close()]"};
    String[] msg32001 = new String[]{"/[label()]", "Man, you were a great help.", "/[waitkey(1)]/[clear()]", "Look. Thanks to you people, all the goods are safe. The store is hardly damaged as well. Now I can focus my efforts on my business.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Did you try that quiz game?", "/[waitkey(1)]/[clear()]", "There're quite a number of difficult questions. I've tried\nbeating it several times, but I can't do it at all.", "/[waitkey(64)]/[close()]"};
    String[] msg41001 = new String[]{"/[label()]", "Damn, I was so close to beating it, but now I have to start over again because of the Gnosis!", "/[waitkey(1)]/[clear()]", "I'm not going anywhere until I beat that game!", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Oh, be careful with those. All those lined up there are weapons for A.G.W.S. They're all incredibly powerful and could easily blow this shop away.", "/[waitkey(1)]/[clear()]", "Not that they're things that people could operate by hand anyway.", "/[waitkey(64)]/[close()]"};
    String[] msg51001 = new String[]{"/[label()]", "Hey, stay away! All the weapons are loaded for the Gnosis battle. If you mess with them in the wrong way, you could get blown to bits.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "They're all so cool! Which one should I pick? I like Vector's VX Series, but the AG Type that the Federation officially employs is awesome too. Since the Military\nuses them, AGs have a certain sense of safety behind them. Which one do you think is better?", "/[waitkey(64)]/[close()]"};
    String[] msg61001 = new String[]{"/[label()]", "Damn, I never thought the Gnosis would attack before I could get an A.G.W.S. I shouldn't have been so indecisive! Oh, what should I do?", "/[waitkey(1)]/[clear()]", "Should I still buy it now? What do you think?", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "Say, I hear yelling and laughing from the bottom of those stairs. It's apparently an A.G.W.S. factory, but it's still creepy, you know?", "/[waitkey(64)]/[close()]"};
    String[] msg71001 = new String[]{"/[label()]", "This is creepy...! The laughter and weird voices from below those stairs are getting louder! Are there Gnosis down there or something?!", "/[waitkey(64)]/[close()]"};
    String[] msg72001 = new String[]{"/[label()]", "That's weird...the Gnosis are gone, but the weird voices from downstairs haven't ceased!", "/[waitkey(1)]/[clear()]", "This is bad. There's probably something really nasty nesting down there.", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label()]", "Hey, could you get out of the way? I'm doing a camera test right now. Please don't interfere.", "/[waitkey(64)]/[close()]"};
    String[] msg81001 = new String[]{"/[label()]", "Hey, get away from there! We're taking off really soon, so get out of the way. I won't be able to escape if the enemy comes! Move!", "/[waitkey(64)]/[close()]"};
    String[] msg82001 = new String[]{"/[label()]", "...zzz...zzz. Mmm... zzzzzz.", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_00 = new String[]{"There is a game machine. Would you like to play?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_01 = new String[]{"Welcome to the world of Destroy Runner! This is just a simple quiz game that requires absolutely no strength or reflex skills.", "/[waitkey(1)]/[clear()]", "Question 1", "/[waitkey(1)]/[clear()]", "What's the name of the parts shop on the Dock Colony?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_02 = new String[]{"Correct!", "/[waitkey(1)]/[clear()]", "Question 2", "/[waitkey(1)]/[clear()]", "What's the name of the eccentric professor at the Foundation Robot Academy?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_03 = new String[]{"Correct!", "/[waitkey(1)]/[clear()]", "Question 3", "/[waitkey(1)]/[clear()]", "What kind of clothing is hanging second from the\nleft on the wall at the cleaners?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_04 = new String[]{"Correct!", "/[waitkey(1)]/[clear()]", "Question 4", "/[waitkey(1)]/[clear()]", "Who's all fired up about the drill?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_05 = new String[]{"Correct!", "/[waitkey(1)]/[clear()]", "Question 5", "/[waitkey(1)]/[clear()]", "What's the name of the girl that can't speak at the\nDock Clinic, the medical clinic on the Dock Colony?", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_06 = new String[]{"You answered all the questions correctly!", "/[waitkey(1)]/[clear()]", "Ta-da! Congratulations, you win! A luxurious item will be given to the winner as a prize.", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_NO1 = new String[]{"Wrong!", "/[waitkey(64)]/[clear()]"};
    String[] QUIZ_NO2 = new String[]{"Are you kidding me?!", "/[waitkey(64)]/[close()]"};
    String[] QUIZ_dame = new String[]{"No response. It seems to be broken...", "/[waitkey(64)]/[close()]"};

    ST2120() {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(7166, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Runtime.disable(524288);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.QUIZ_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Play\nDon't play");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 15);
                            this.win.print(this.QUIZ_01, 0);
                            System.waitFor(this.win);
                            this.menu = Menu.create();
                            this.menu.addItem("TAKE TO ME\nTALK TO ME\nSOCK IT TO ME");
                            System.waitFor(this.menu);
                            this.selected1 = this.menu.getSelected();
                            if (this.selected1 == 1) {
                                Sound.effectPlay(6);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.QUIZ_02, 0);
                                System.waitFor(this.win);
                                this.menu = Menu.create();
                                this.menu.addItem("Professor\nScott\nTom\nVanderkam");
                                System.waitFor(this.menu);
                                this.selected2 = this.menu.getSelected();
                                if (this.selected2 == 0) {
                                    Sound.effectPlay(6);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 15);
                                    this.win.print(this.QUIZ_03, 0);
                                    System.waitFor(this.win);
                                    this.menu = Menu.create();
                                    this.menu.addItem("Unattractive tuxedo\nIrresistible leopard print bathing suit\nSmelly-looking purple t-shirt\nStylish black suit\nSexy Chinese dress");
                                    System.waitFor(this.menu);
                                    this.selected3 = this.menu.getSelected();
                                    if (this.selected3 == 2) {
                                        Sound.effectPlay(6);
                                        this.win = Window.create();
                                        this.win.setSize(4, 45);
                                        this.win.setLocation(15, 15);
                                        this.win.print(this.QUIZ_04, 0);
                                        System.waitFor(this.win);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Swaine\nSvaine\nSwine\nHolder\nHolgar");
                                        System.waitFor(this.menu);
                                        this.selected4 = this.menu.getSelected();
                                        if (this.selected4 == 4) {
                                            Sound.effectPlay(6);
                                            this.win = Window.create();
                                            this.win.setSize(4, 45);
                                            this.win.setLocation(15, 15);
                                            this.win.print(this.QUIZ_05, 0);
                                            System.waitFor(this.win);
                                            this.menu = Menu.create();
                                            this.menu.addItem("Bunnie\nPink Bug\nLuty\nKing\nLovely MOMO");
                                            System.waitFor(this.menu);
                                            this.selected4 = this.menu.getSelected();
                                            if (this.selected4 == 2) {
                                                Sound.effectPlay(6);
                                                this.win = Window.create();
                                                this.win.setSize(4, 45);
                                                this.win.setLocation(15, 15);
                                                this.win.print(this.QUIZ_06, 0);
                                                ST2120.waitPage(this.win, 64);
                                                Sound.effectPlay(6);
                                                Runtime.addItemWin(10, 38);
                                                System.sleep(30);
                                                this.game.disp(false);
                                                Sound.effectStop(196614);
                                                Runtime.enable(524288);
                                                Runtime.setPlayerControl(true);
                                                this.BUTTON_F = 0;
                                                Runtime.setFlags(7166, 1, 1);
                                                break;
                                            }
                                            System.sleep(10);
                                            Sound.effectPlay(5);
                                            System.sleep(5);
                                            Sound.effectPlay(5);
                                            this.win = Window.create();
                                            this.win.setSize(4, 45);
                                            this.win.setLocation(15, 15);
                                            this.win.print(this.QUIZ_NO1, 0);
                                            ST2120.waitPage(this.win, 64);
                                            this.player.getTranslate();
                                            this.EF01.setTranslate(this.player.px, this.player.py, this.player.pz);
                                            this.EF01.disp(true);
                                            Sound.effectPlay(65540);
                                            this.win.print(this.QUIZ_NO2, 0);
                                            ST2120.waitPage(this.win, 64);
                                            System.sleep(16);
                                            Sound.effectStop(65540);
                                            this.BUTTON_F = 0;
                                            Runtime.enable(524288);
                                            Runtime.setPlayerControl(true);
                                            System.println("1111111111");
                                            break;
                                        }
                                        System.sleep(10);
                                        Sound.effectPlay(5);
                                        System.sleep(5);
                                        Sound.effectPlay(5);
                                        this.win = Window.create();
                                        this.win.setSize(4, 45);
                                        this.win.setLocation(15, 15);
                                        this.win.print(this.QUIZ_NO1, 0);
                                        ST2120.waitPage(this.win, 64);
                                        this.player.getTranslate();
                                        this.EF01.setTranslate(this.player.px, this.player.py, this.player.pz);
                                        this.EF01.disp(true);
                                        Sound.effectPlay(65540);
                                        this.win.print(this.QUIZ_NO2, 0);
                                        ST2120.waitPage(this.win, 64);
                                        System.sleep(16);
                                        Sound.effectStop(65540);
                                        this.EF01.disp(false);
                                        this.BUTTON_F = 0;
                                        Runtime.enable(524288);
                                        Runtime.setPlayerControl(true);
                                        System.println("1111111111");
                                        break;
                                    }
                                    System.sleep(10);
                                    Sound.effectPlay(5);
                                    System.sleep(5);
                                    Sound.effectPlay(5);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 15);
                                    this.win.print(this.QUIZ_NO1, 0);
                                    ST2120.waitPage(this.win, 64);
                                    this.player.getTranslate();
                                    this.EF01.setTranslate(this.player.px, this.player.py, this.player.pz);
                                    this.EF01.disp(true);
                                    Sound.effectPlay(65540);
                                    this.win.print(this.QUIZ_NO2, 0);
                                    ST2120.waitPage(this.win, 64);
                                    System.sleep(16);
                                    Sound.effectStop(65540);
                                    this.EF01.disp(false);
                                    this.BUTTON_F = 0;
                                    Runtime.enable(524288);
                                    Runtime.setPlayerControl(true);
                                    System.println("2222222222");
                                    break;
                                }
                                System.sleep(10);
                                Sound.effectPlay(5);
                                System.sleep(5);
                                Sound.effectPlay(5);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.QUIZ_NO1, 0);
                                ST2120.waitPage(this.win, 64);
                                this.player.getTranslate();
                                this.EF01.setTranslate(this.player.px, this.player.py, this.player.pz);
                                this.EF01.disp(true);
                                Sound.effectPlay(65540);
                                this.win.print(this.QUIZ_NO2, 0);
                                ST2120.waitPage(this.win, 64);
                                System.sleep(16);
                                Sound.effectStop(65540);
                                this.EF01.disp(false);
                                this.BUTTON_F = 0;
                                Runtime.enable(524288);
                                Runtime.setPlayerControl(true);
                                System.println("3333333333");
                                break;
                            }
                            System.sleep(10);
                            Sound.effectPlay(5);
                            System.sleep(5);
                            Sound.effectPlay(5);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 15);
                            this.win.print(this.QUIZ_NO1, 0);
                            ST2120.waitPage(this.win, 64);
                            this.player.getTranslate();
                            this.EF01.setTranslate(this.player.px, this.player.py, this.player.pz);
                            this.EF01.disp(true);
                            Sound.effectPlay(65540);
                            this.win.print(this.QUIZ_NO2, 0);
                            ST2120.waitPage(this.win, 64);
                            System.sleep(16);
                            Sound.effectStop(65540);
                            this.EF01.disp(false);
                            this.BUTTON_F = 0;
                            Runtime.enable(524288);
                            Runtime.setPlayerControl(true);
                            System.println("4444444444");
                        }
                    }
                    this.BUTTON_F = 0;
                    Runtime.enable(524288);
                    Runtime.setPlayerControl(true);
                    return;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.QUIZ_dame, 0);
                ST2120.waitPage(this.win, 64);
                this.BUTTON_F = 0;
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkNPC2(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SCO, 0);
        System.waitFor(this.win);
        Runtime.setFlags(3156, 1, 1);
        Runtime.setPlayerControl(true);
    }

    public void Talk_npc1(Enepc enepc) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc1_1();
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc1_2();
        } else {
            this.Talk_npc1_1();
        }
    }

    void Talk_npc1_1() {
        this.npc1.kickEnepc(4, 1);
        this.npc1.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10001, 0);
        ST2120.waitPage(this.win, 64);
        System.sleep(15);
        if (Runtime.getFlags(389, 1) == 1) {
            Runtime.enterShop(14);
        } else if (Runtime.getFlags(362, 1) == 1) {
            Runtime.enterShop(12);
        } else if (Runtime.getFlags(346, 1) == 1) {
            Runtime.enterShop(10);
        } else if (Runtime.getFlags(301, 1) == 1) {
            Runtime.enterShop(8);
        } else {
            Runtime.enterShop(1);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10002, 0);
        ST2120.waitPage(this.win, 64);
        this.npc1.kickEnepc(4, 0);
    }

    void Talk_npc1_2() {
        this.npc1.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg11001, 0);
        ST2120.waitPage(this.win, 64);
        System.sleep(15);
        if (Runtime.getFlags(389, 1) == 1) {
            Runtime.enterShop(14);
        } else if (Runtime.getFlags(362, 1) == 1) {
            Runtime.enterShop(12);
        } else if (Runtime.getFlags(346, 1) == 1) {
            Runtime.enterShop(10);
        } else if (Runtime.getFlags(301, 1) == 1) {
            Runtime.enterShop(8);
        } else {
            Runtime.enterShop(1);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10002, 0);
        ST2120.waitPage(this.win, 64);
        this.npc1.kickEnepc(1, 12);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc2_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msg21001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        window.print(this.msg22001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc3_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msg31001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        window.print(this.msg32001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc4_1(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msg41001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc5_1(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msg51001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc6_1(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg60001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        window.print(this.msg61001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc7_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc7_2(window);
        } else {
            this.Talk_npc7_1(window);
        }
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        window.print(this.msg71001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc7_3(Window window) {
        window.print(this.msg72001, 0);
        ST2120.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc8_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc8_2(window);
        } else {
            this.Talk_npc8_1(window);
        }
    }

    void Talk_npc8_1(Window window) {
        window.print(this.msg80001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msg81001, 0);
        ST2120.waitPage(window, 64);
    }

    void Talk_npc8_3(Window window) {
        window.print(this.msg82001, 0);
        ST2120.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("ミニマップ・４");
                Runtime.jumpCF(2130, 4);
                break;
            }
            case 1: {
                System.println("ロボ研");
                Runtime.jumpCF(2110, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.45f, 0.45f, 0.45f);
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
        Runtime.setIdLightVec(1, 1, 0.15f, 1.0f, 0.1f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        this.AGWS1 = new Unit();
        this.AGWS1.init(8449, -5.25f, -0.5f, 0.85f, 0.0f);
        this.AGWS1.setScale(1.2f, 1.2f, 1.2f);
        this.AGWS2 = new Unit();
        this.AGWS2.init(8449, 0.775f, -0.5f, 0.85f, 0.0f);
        this.AGWS2.setScale(1.2f, 1.2f, 1.2f);
        this.AGWS3 = new Unit();
        this.AGWS3.init(8195, 6.75f, -0.5f, 0.85f, 0.0f);
        this.AGWS3.setScale(1.2f, 1.2f, 1.2f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, -10.0f, 0.0f, 10.0f, 37.5f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, -15.0f, 0.0f, 10.0f, 37.5f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, -10.0f, 0.0f, 7.0f, 37.5f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 37.5f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, -15.0f, 0.0f, 8.0f, 37.5f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 7.0f, 37.5f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        this.denwa = new Effect(1646, -7.875f, 5.425f, -6.552f, 0.0f);
        this.denwa.setScale(0.25f, 1.0f, 0.675f);
        this.denwa.setRotate(32.5f, 0.0f, 0.0f);
        if (Runtime.getFlags(7166, 1) == 0) {
            this.game = new Effect(1411, 13.993f, 5.9775f, -7.148f, 0.0f);
            this.game.setScale(0.35f, 0.85f, 0.85f);
            this.game.setRotate(-51.0f, 0.0f, 0.0f);
        }
        this.EF01 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF01.disp(false);
        if (Runtime.checkItem(10, 38) == 0) {
            this.teiten1 = new Uwamono(28690, 13.993f, 5.9775f, -7.148f, 0.0f);
            this.teiten1.SetBgm(196614);
        }
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
        this.npc1 = new NPC_NORMAL(1592, 1, 0, 14, 8, -5.48f, 4.25f, -7.94f, 0.0f);
        this.npc1.setInvalidID(1);
        this.npc2 = new NPC_NORMAL(527, 2, 0, 14, 10, 7.16f, 1.39f, 1.55f, 180.0f);
        this.npc2.setInvalidID(1);
        this.npc3 = new NPC_NORMAL(527, 3, 0, 14, 10, -4.4f, 4.0f, -1.8f, 0.0f);
        this.npc3.setInvalidID(1);
        this.npc4 = new NPC_NORMAL(1543, 4, 0, 14, 9, 15.2f, 4.37f, -3.05f, 0.0f);
        this.npc5 = new NPC_NORMAL(527, 5, 0, 14, 17, 4.95f, 4.25f, -7.0f, 180.0f);
        this.npc5.setInvalidID(1);
        this.npc6 = new NPC_NORMAL(1543, 6, 0, 2, 9, -4.42f, 4.25f, -4.1f, 0.0f);
        this.npc7 = new NPC_NORMAL(1561, 7, 0, 14, 9, -11.66f, 4.25f, -4.49f, 0.0f);
        this.npc8 = new NPC_NORMAL(1561, 8, 0, 14, 9, -5.2f, 1.39f, 1.0f, 0.0f);
        this.npc8.setInvalidID(1);
        this.npc8.setVisible(false);
        this.npc20 = new NPC_NORMAL(1592, 20, 0, 14, 9, -5.48f, 4.25f, -6.94f, 0.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc1.disableDTKFlag(2);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 12);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 4);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 17);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 16);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc20.talkto("Talk_npc1");
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
}

