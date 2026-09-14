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
import xeno.map.MC_ELS12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0620
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS12_PRJ {
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
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc npc15;
    Enepc npc16;
    Enepc npc17;
    Enepc npc20;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int button1_flg = 0;
    int button2_flg = 0;
    int e1pass = 0;
    int e2pass = 0;
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
    int tonnyhelp;
    int tonnynothelp;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono col1;
    Uwamono col2;
    Uwamono col3;
    Uwamono col4;
    Uwamono item01;
    Unit L_dodai;
    Unit L_sasae1;
    Unit L_sasae2;
    Unit L_tesuri1;
    Unit L_tesuri2;
    Unit L_tesuri3;
    Unit L_tesuri4;
    Unit L_tesuri5;
    Unit L_tesuri6;
    Unit L_sousa;
    Unit R_dodai;
    Unit R_sasae1;
    Unit R_sasae2;
    Unit R_tesuri1;
    Unit R_tesuri2;
    Unit R_tesuri3;
    Unit R_tesuri4;
    Unit R_tesuri5;
    Unit R_tesuri6;
    Unit R_sousa;
    Unit elv;
    Unit elv2;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
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
    Effect eve11;
    Effect eve12;
    Effect eve13;
    Effect eve14;
    Effect eve15;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg00B88FA5 = new String[]{"/[label(Tony)]", "It's not here.\n", "Nor there.\n", "Or anywhere.", "/[waitkey(1)]/[clear()]", "Man, where the heck is it? Damn, you should be more organized. Whoever's responsible, come on out!", "/[waitkey(1)]/[clear()]", "That would be me! I'm the one responsible!", "/[waitkey(1)]/[clear()]", "Okay, this is not the time to be doing these kinds of stupid things!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B88FA6 = new String[]{"/[label(Shion)]", "Umm...is something the matter?", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FA1 = new String[]{"/[label(Tony)]", "Hmm? Oh, Shion, it's you.", "/[waitkey(1)]/[clear()]", "I was just looking for something, but I just can't find it. How about it? You wanna help out little ol' me?", "/[waitkey(64)]/[close()]"};
    String[] msg10B88FA2 = new String[]{"/[label(Tony)]", "Really? Shion, you're the one I can always count on to be there.", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FA3 = new String[]{"/[label(Shion)]", "So what are you looking for?", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FA4 = new String[]{"/[label(Tony)]", "It's a map of all the hazardous sectors in space. The Captain went and lost it, and now he's yelling at me to find it.", "/[waitkey(1)]/[clear()]", "Well, I figure it's gotta be somewhere on this ship.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FA5 = new String[]{"/[label(Shion)]", "A /[color(0x329bbe)]Hazardous Area Map/[color(0x808080)]? Sure, I'll look for it.", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FA6 = new String[]{"/[label(Tony)]", "Thanks, you're a great help. Find it as quick as you can!", "/[waitkey(64)]/[close()]"};
    String[] msg10B88FA7 = new String[]{"/[label(Tony)]", "Oh, I see how it is...I know now what kind of person you are.", "/[waitkey(64)]/[close()]"};
    String[] msg10B88FA8 = new String[]{"/[label(Tony)]", "What? You're really going to help me after all?!", "/[waitkey(64)]/[close()]"};
    String[] msg10B88FA9 = new String[]{"/[label(Tony)]", "So did you find it?", "/[waitkey(1)]/[clear()]", "The /[color(0x329bbe)]Hazardous Area Map/[color(0x808080)]. Come on, check every corner!", "/[waitkey(64)]/[close()]"};
    String[] msg10B88FB1 = new String[]{"/[label(Tony)]", "Hey, you found it! Attagirl, Shion! Where the heck was it?", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FB2 = new String[]{"/[label(Shion)]", "Um, that's the weird thing. The Captain had it.", "/[waitkey(64)]/[clear()]"};
    String[] msg10B88FB3 = new String[]{"/[label(Tony)]", "What?!", "/[waitkey(1)]/[clear()]", "That stupid, senile jackass! Did a Gnosis suck out his brain or something? He ordered me to find it while he had it all along?! What?! Sheesh!!", "/[waitkey(1)]/[clear()]", "Oh, sorry. It's got nothing to do with you.", "/[waitkey(1)]/[clear()]", "Thanks for finding it. Hang on to this.", "/[waitkey(64)]/[close()]"};
    String[] msg00BA4128 = new String[]{"/[label(Tony)]", "Yo, Shion! Thanks for helping me earlier.", "/[waitkey(1)]/[clear()]", "You know, the Captain should quit getting us further into debt and spend some time getting organized instead!", "/[waitkey(64)]/[close()]"};
    String[] msg01631D54 = new String[]{"/[label(Shion)]", "This is Hangar 1, a hangar exclusively for A.G.W.S. They let me store mine here too.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D55 = new String[]{"/[label(MOMO)]", "It's a beautiful A.G.W.S. I've never seen an A.G.W.S. like it before.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D56 = new String[]{"/[label(Shion)]", "It's one of our prototypes.", "/[waitkey(1)]/[clear()]", "It was supposed to be tested during Woglinde's trial run, but I never imagined it would actually come into use.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D57 = new String[]{"/[label(MOMO)]", "Whose A.G.W.S. is that one over there?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D58 = new String[]{"/[label(Shion)]", "Oh, that's the Elsa's A.G.W.S. It seems chaos pilots it for the most part. I heard that Hammer put it together himself from junk parts salvaged from battlefields.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D59 = new String[]{"/[label(MOMO)]", "Really? Hammer is very skilled.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D60 = new String[]{"/[label(Shion)]", "I think he does all the repairs on the Elsa too. Someone like Hammer's a rare find. There aren't too many people like that, even in Vector Second R&D Division.", "/[waitkey(64)]/[close()]"};
    String[] msg01631D61 = new String[]{"/[label(MOMO)]", "Tee-hee, I want an A.G.W.S. like Ziggy.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D62 = new String[]{"/[label(Shion)]", "I see...", "One...like Ziggy...", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D63 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ELE = new String[]{"Use the lift?", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgrobo = new String[]{"/[label()]", "I'm the hardest worker on this ship.", "/[waitkey(1)]/[clear()]", "I even installed the locks on the slide deck.", "/[waitkey(1)]/[clear()]", "But when I was cleaning one of the cabins, I lost the /[color(0x329bbe)]Disarm Key/[color(0x808080)].", "/[waitkey(1)]/[clear()]", "Oh boy, oh no...", "/[waitkey(1)]/[clear()]", "It should be easy to find though. All I need to do is press the ○ Button.", "/[waitkey(1)]/[clear()]", "But it's such a pain because the cabin's huge.", "/[waitkey(1)]/[clear()]", "Oh boy, oh no...", "/[waitkey(64)]/[close()]"};
    String[] msgshoprobo1 = new String[]{"/[label()]", "hellO! i am in charge of maintaining this shiP!", "/[waitkey(1)]/[clear()]", "is there anything you neeD? i will sell you anything on this ship for cheaP! i'd appreciate your patronage so the captaiN can repay his debtS!", "/[waitkey(64)]/[close()]"};
    String[] msgshoprobo2 = new String[]{"/[label()]", "thank yoU! thank you very mucH! i look forward to your next visiT!", "/[waitkey(64)]/[close()]"};
    String[] msgyobidasi1 = new String[]{"/[label(Ship Intercom)]", "Hey, Ms. Vector! If you're just lounging around, come to the bridge for a sec!", "/[waitkey(64)]/[close()]"};
    String[] msgyobidasi2 = new String[]{"/[label(Shion)]", "What could it be? I wonder if something happened?", "/[waitkey(64)]/[close()]"};

    ST0620() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.058f, 4.539f, -9.246f);
        this.camEV.setRotate(-10.643f, 45.098f, 0.0f);
        this.camEV.setFov(39.999f);
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
        this.camEV.setTranslate(6.659f, 3.851f, -12.726f);
        this.camEV.setRotate(8.894f, 31.239f, 0.0f);
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
                if (this.e1pass == 0) {
                    if (Runtime.getFlags(3026, 1) == 0) {
                        this.e1pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.setFlags(3026, 1, 1);
                                this.elv.setArgs(4, 0.058333334f);
                                this.elv.setArgs(12, 2.5f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (Runtime.getFlags(3026, 1) != 1) break;
                    this.e1pass = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ELE, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3026, 1, 0);
                            this.elv.setArgs(4, 0.058333334f);
                            this.elv.setArgs(12, -1.0f);
                            Sound.effectPlay(196742);
                            this.L_dodai.start(1, "Move2");
                            System.sleep(60);
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
            case 1: {
                this.e1pass = 0;
                break;
            }
            case 2: {
                this.e1pass = 0;
                break;
            }
            case 3: {
                if (this.e2pass == 0) {
                    if (Runtime.getFlags(3027, 1) == 0) {
                        this.e2pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.setFlags(3027, 1, 1);
                                this.elv2.setArgs(4, 0.058333334f);
                                this.elv2.setArgs(12, -1.0f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move3");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (Runtime.getFlags(3027, 1) != 1) break;
                    this.e2pass = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ELE, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3027, 1, 0);
                            this.elv2.setArgs(4, 0.058333334f);
                            this.elv2.setArgs(12, 2.5f);
                            Sound.effectPlay(196742);
                            this.L_dodai.start(1, "Move4");
                            System.sleep(60);
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
            case 4: {
                this.e2pass = 0;
                break;
            }
            case 5: {
                this.e2pass = 0;
                break;
            }
        }
    }

    void Talk_no() {
        this.win.print(this.msgtalk_no, 0);
        ST0620.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0620.waitPage(window, 64);
    }

    public void Talk_npc16(Enepc enepc) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgshoprobo1, 0);
        ST0620.waitPage(this.win, 64);
        System.sleep(15);
        Runtime.enterShop(3);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgshoprobo2, 0);
        ST0620.waitPage(this.win, 64);
    }

    public void Talk_npc17(Enepc enepc, Window window) {
        window.print(this.msgrobo, 0);
        ST0620.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc) {
        if (this.S2057 == 1) {
            this.Talk_no();
        } else if (this.S2042 == 1) {
            this.Talk_no();
        } else if (this.S2040C == 1) {
            this.Talk_no();
        } else if (this.S2030 == 1) {
            this.Talk_npc2_3();
        } else if (this.S2014B == 1) {
            this.Talk_no();
        } else if (this.S2014 == 1) {
            this.Talk_npc2_1();
        } else if (this.S2013B == 1) {
            this.Talk_npc2_1();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc2_1() {
        this.tonnyhelp = Runtime.getFlags(7005, 1);
        this.tonnynothelp = Runtime.getFlags(7037, 1);
        if (Runtime.getFlags(7038, 1) == 1) {
            this.npc2.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg00BA4128, 0);
            ST0620.waitPage(this.win, 64);
            return;
        }
        if (Runtime.checkItem(10, 9) == 1) {
            this.npc2.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg10B88FB1, 0);
            ST0620.waitPage(this.win, 64);
            this.win.print(this.msg10B88FB2, 0);
            ST0620.waitPage(this.win, 64);
            this.win.print(this.msg10B88FB3, 0);
            ST0620.waitPage(this.win, 64);
            Sound.effectPlay(6);
            Runtime.addItemWin(0, 3);
            Runtime.removeItem(10, 9);
            Runtime.setFlags(7038, 1, 1);
            return;
        }
        switch (this.tonnyhelp) {
            case 1: {
                this.npc2.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10B88FA9, 0);
                ST0620.waitPage(this.win, 64);
                return;
            }
        }
        switch (this.tonnynothelp) {
            case 1: {
                this.npc2.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10B88FA8, 0);
                ST0620.waitPage(this.win, 64);
                this.help_sel();
                return;
            }
        }
        this.npc2.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00B88FA5, 0);
        ST0620.waitPage(this.win, 64);
        this.win.print(this.msg00B88FA6, 0);
        ST0620.waitPage(this.win, 64);
        this.win.print(this.msg10B88FA1, 0);
        ST0620.waitPage(this.win, 64);
        this.help_sel();
    }

    void Talk_npc2_2() {
    }

    void Talk_npc2_3() {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S2013 == 0) {
                    Runtime.jumpCF(570, 1);
                    break;
                }
                if (this.S2013B == 0) {
                    Runtime.setFlags(123, 1, 1);
                    Runtime.setFlags(7084, 1, 1);
                    System.println("フラグオン！");
                    Runtime.charAllRecovery();
                    System.println("*********全回復しました**************");
                    Runtime.jumpEvent(2131);
                    break;
                }
                Runtime.jumpCF(570, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(580, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(630, 1);
                break;
            }
        }
    }

    void help_sel() {
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I'll help\nI don't have time");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc2.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10B88FA2, 0);
                ST0620.waitPage(this.win, 64);
                this.win.print(this.msg10B88FA3, 0);
                ST0620.waitPage(this.win, 64);
                this.win.print(this.msg10B88FA4, 0);
                ST0620.waitPage(this.win, 64);
                this.win.print(this.msg10B88FA5, 0);
                ST0620.waitPage(this.win, 64);
                this.win.print(this.msg10B88FA6, 0);
                ST0620.waitPage(this.win, 64);
                Runtime.setFlags(7005, 1, 1);
                break;
            }
            default: {
                this.npc2.kickEnepc(1, 0);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10B88FA7, 0);
                ST0620.waitPage(this.win, 64);
                Runtime.setFlags(7037, 1, 1);
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
        this.tonnyhelp = Runtime.getFlags(7005, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(125, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, -1.1f, -14.5f, 0.0f);
        this.teiten1.SetBgm(196631);
        this.teiten2 = new Uwamono(28690, -3.0f, -1.1f, 6.0f, 0.0f);
        this.teiten2.SetBgm(196637);
        this.teiten3 = new Uwamono(28690, 3.0f, -1.1f, -6.0f, 0.0f);
        this.teiten3.SetBgm(196637);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, -6.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 6.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 15.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, -15.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        Stage.renderCommand(4);
        this.item01 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 87);
        new Uwamono(13, 0, this.item01);
        new Uwamono(14, 0);
        new Uwamono(15, 0);
        this.L_dodai = new Mapunits();
        this.L_dodai.mapUnit(157);
        this.L_dodai.start(4, null);
        if (Runtime.getFlags(3026, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(167, 3.5f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, -1.0f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(167, 3.5f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 2.5f);
        }
        if (Runtime.getFlags(7043, 1) == 1) {
            if (Runtime.getFlags(3027, 1) == 0) {
                this.elv2 = new Unit();
                this.elv2.initElevator(166, 3.5f, 0.0f);
                this.elv2.setArgs(1, 1, 1);
                this.elv2.setArgs(12, 2.5f);
            } else {
                this.elv2 = new Unit();
                this.elv2.initElevator(166, 3.5f, 0.0f);
                this.elv2.setArgs(1, 1, 1);
                this.elv2.setArgs(12, -1.0f);
            }
        } else if (Runtime.getFlags(3027, 1) == 0) {
            this.elv2 = new Unit();
            this.elv2.initElevator(166, 3.5f, 0.0f);
            this.elv2.setArgs(1, 1, 1);
            this.elv2.setArgs(12, 2.5f);
        } else {
            this.elv2 = new Unit();
            this.elv2.initElevator(166, 3.5f, 0.0f);
            this.elv2.setArgs(1, 1, 1);
            this.elv2.setArgs(12, -1.0f);
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
        this.doorA = new Uwamono(19, 42, '\u0001');
        new Uwamono(21, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(12, 40, '\u0001');
        this.doorC = new Uwamono(38, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.col1 = new Uwamono(28672, -6.0f, -1.0f, -14.5f, 0.0f);
        this.col2 = new Uwamono(28672, 6.0f, -1.0f, -14.5f, 0.0f);
        this.col3 = new Uwamono(28672, -6.0f, 2.5f, -11.2f, 0.0f);
        this.col4 = new Uwamono(28672, 6.0f, 2.5f, -11.2f, 0.0f);
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_0();
        } else if (this.S2040C == 1) {
            this.npcset_0();
        } else if (this.S2030 == 1) {
            this.npcset_3();
        } else if (this.S2014B == 1) {
            this.npcset_0();
        } else if (this.S2014 == 1) {
            this.npcset_2();
        } else if (this.S2013B == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
        this.npc17 = new NPC_NORMAL(1612, 17, 0, 13, 18, 4.35f, 0.0f, -4.65f, 150.0f);
        this.npc17.talkto("Talk_npc17");
        this.npc17.setMotion(0, 9);
        this.npc16 = new NPC_NORMAL(1612, 16, 0, 13, 18, -0.019f, -1.1f, -14.48f, 0.0f);
        this.npc16.talkto("Talk_npc16");
        this.npc16.setMotion(0, 10);
        this.npc12 = new NPC_NORMAL2(8450, 12, 0, 17, 16, 5.0f, -1.0f, -6.0f, 270.0f);
        this.npc12.setMotion(0, 2);
        this.npc12.setInvalidID(1);
        this.npc12.dispRadar(false);
        this.npc12.renderCommand(23);
        this.npc12.disableDTKFlag(131072);
        this.npc12.disableDTKFlag(65536);
        this.npc15 = new NPC_NORMAL2(8193, 15, 0, 17, 16, -5.0f, -1.0f, 6.0f, 90.0f);
        this.npc15.setMotion(0, 2);
        this.npc15.setInvalidID(1);
        this.npc15.dispRadar(false);
        this.npc15.renderCommand(23);
        this.npc15.disableDTKFlag(131072);
        this.npc15.disableDTKFlag(65536);
        if (Runtime.getFlags(143, 1) == 0) {
            this.eve02 = new Effect(1420, 2);
            this.eve02.disp(true);
            this.eve03 = new Effect(1420, 3);
            this.eve03.disp(true);
            this.eve08 = new Effect(1013, 8);
            this.eve08.disp(true);
            this.eve09 = new Effect(1013, 9);
            this.eve09.disp(true);
            if (Runtime.getFlags(3026, 1) == 0) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
                this.eve12.setTranslate(this.eve12.px, this.eve12.py - 3.5f, this.eve12.pz);
            } else if (Runtime.getFlags(3026, 1) == 1) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
            }
            if (Runtime.getFlags(3027, 1) == 0) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
            } else if (Runtime.getFlags(3027, 1) == 1) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
                this.eve14.setTranslate(this.eve14.px, this.eve14.py - 3.5f, this.eve14.pz);
            }
        } else if (Runtime.getFlags(158, 1) == 1) {
            this.eve02 = new Effect(1420, 2);
            this.eve02.disp(true);
            this.eve03 = new Effect(1420, 3);
            this.eve03.disp(true);
            this.eve08 = new Effect(1013, 8);
            this.eve08.disp(true);
            this.eve09 = new Effect(1013, 9);
            this.eve09.disp(true);
            if (Runtime.getFlags(3026, 1) == 0) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
                this.eve12.setTranslate(this.eve12.px, this.eve12.py - 3.5f, this.eve12.pz);
            } else if (Runtime.getFlags(3026, 1) == 1) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
            }
            if (Runtime.getFlags(3027, 1) == 0) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
            } else if (Runtime.getFlags(3027, 1) == 1) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
                this.eve14.setTranslate(this.eve14.px, this.eve14.py - 3.5f, this.eve14.pz);
            }
        } else if (Runtime.getFlags(7027, 1) == 1) {
            this.eve02 = new Effect(1420, 2);
            this.eve02.disp(true);
            this.eve03 = new Effect(1420, 3);
            this.eve03.disp(true);
            this.eve08 = new Effect(1013, 8);
            this.eve08.disp(true);
            this.eve09 = new Effect(1013, 9);
            this.eve09.disp(true);
            if (Runtime.getFlags(3026, 1) == 0) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
                this.eve12.setTranslate(this.eve12.px, this.eve12.py - 3.5f, this.eve12.pz);
            } else if (Runtime.getFlags(3026, 1) == 1) {
                this.eve12 = new Effect(1419, 12);
                this.eve12.disp(true);
            }
            if (Runtime.getFlags(3027, 1) == 0) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
            } else if (Runtime.getFlags(3027, 1) == 1) {
                this.eve14 = new Effect(1419, 13);
                this.eve14.disp(true);
                this.eve14.setTranslate(this.eve14.px, this.eve14.py - 3.5f, this.eve14.pz);
            }
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc2 = new NPC_NORMAL(277, 2, 0, 2, 3, -1.01f, 0.0f, 3.43f, 180.0f);
        this.npc2.enableDTKFlag(262144);
        this.npc2.talkto("Talk_npc2");
        if (Runtime.getFlags(7084, 1) == 1) {
            this.npc20 = new NPC_NORMAL(277, 2, 0, 14, 3, 100.0f, 100.0f, 100.0f, 180.0f);
            this.npc20.talkto("TalkNPC11");
            this.npc20.disableDTKFlag(131072);
            this.npc20.disableDTKFlag(8);
            this.npc20.start(1, "yobidasi");
        }
    }

    void npcset_2() {
        this.npc2 = new NPC_NORMAL(277, 2, 0, 2, 3, -1.01f, 0.0f, 3.43f, 180.0f);
        this.npc2.enableDTKFlag(262144);
        this.npc2.talkto("Talk_npc2");
    }

    void npcset_3() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, 4.05f, 2.5f, -14.19f, 0.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, 6.23f, 2.5f, -13.63f, 340.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 14, 4.89f, 2.5f, -14.18f, 0.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.setInvalidID(1);
        this.npc6.kickEnepc(4, 1);
        this.npc6.disableDTKFlag(131072);
        this.npc6.disableDTKFlag(65536);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc7.setInvalidID(1);
        this.npc7.disableDTKFlag(131072);
        this.npc7.disableDTKFlag(65536);
        this.npc10.setVisible(false);
        this.npc10.setInvalidID(1);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        if (Runtime.getFlags(7027, 1) == 1) {
            return;
        }
        this.npc20 = new NPC_NORMAL(1, 20, 0, 13, 14, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc20.talkto("TalkNPC11");
        this.npc20.setInvalidID(1);
        this.npc20.disableDTKFlag(131072);
        this.npc20.disableDTKFlag(8);
        this.npc20.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
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

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class Mapunits
            extends Unit {
        Mapunits() {
        }

        void Move() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0620.this.eve12.getTranslate();
                    ST0620.this.eve12.setTranslate(ST0620.this.eve12.px, ST0620.this.eve12.py + 0.058333334f, ST0620.this.eve12.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void Move2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0620.this.eve12.getTranslate();
                    ST0620.this.eve12.setTranslate(ST0620.this.eve12.px, ST0620.this.eve12.py - 0.058333334f, ST0620.this.eve12.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void Move3() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0620.this.eve14.getTranslate();
                    ST0620.this.eve14.setTranslate(ST0620.this.eve14.px, ST0620.this.eve14.py - 0.058333334f, ST0620.this.eve14.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void Move4() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0620.this.eve14.getTranslate();
                    ST0620.this.eve14.setTranslate(ST0620.this.eve14.px, ST0620.this.eve14.py + 0.058333334f, ST0620.this.eve14.pz);
                }
                if (n == 61) break;
                ++n;
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

        void ANNAI() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7027, 1, 1);
            Runtime.setFlags(7043, 1, 0);
            Runtime.enable(65536);
            ST0620.this.cam0.setMode(-1);
            ST0620.this.EV_Camera01();
            ST0620.this.npc10.kickEnepc(0, 9);
            ST0620.this.npc6.kickEnepc(1, 27);
            ST0620.this.npc7.kickEnepc(1, 27);
            ST0620.this.npc6.setVisible(true);
            ST0620.this.npc7.setVisible(true);
            ST0620.this.npc10.setVisible(true);
            System.sleep(10);
            ST0620.this.win = Window.create();
            ST0620.this.win.setSize(4, 45);
            ST0620.this.win.setLocation(15, 305);
            ST0620.this.win.print(ST0620.this.msg01631D54, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.npc6.kickEnepc(0, 11);
            ST0620.this.win.print(ST0620.this.msg01631D55, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.npc10.moveEnepc(17, 285.0f, -0.1f, 10);
            ST0620.this.npc6.kickEnepc(0, 10);
            ST0620.this.win.print(ST0620.this.msg01631D56, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.npc10.kickEnepc(1, 10);
            ST0620.this.npc6.moveEnepc(17, 45.0f, 0.1f, 20);
            ST0620.this.npc6.kickEnepc(0, 9);
            ST0620.this.win.print(ST0620.this.msg01631D57, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.npc6.kickEnepc(1, 10);
            ST0620.this.win.print(ST0620.this.msg01631D58, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.win.print(ST0620.this.msg01631D59, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.win.print(ST0620.this.msg01631D60, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            System.sleep(10);
            ST0620.this.fade.call(0);
            System.sleep(30);
            ST0620.this.npc6.setVisible(false);
            ST0620.this.npc7.setVisible(false);
            ST0620.this.npc10.setVisible(false);
            ST0620.this.npc20.setVisible(false);
            ST0620.this.eve02 = new Effect(1420, 2);
            ST0620.this.eve02.disp(true);
            ST0620.this.eve03 = new Effect(1420, 3);
            ST0620.this.eve03.disp(true);
            ST0620.this.eve08 = new Effect(1013, 8);
            ST0620.this.eve08.disp(true);
            ST0620.this.eve09 = new Effect(1013, 9);
            ST0620.this.eve09.disp(true);
            if (Runtime.getFlags(3026, 1) == 0) {
                ST0620.this.eve12 = new Effect(1419, 12);
                ST0620.this.eve12.disp(true);
                ST0620.this.eve12.setTranslate(ST0620.this.eve12.px, ST0620.this.eve12.py - 3.5f, ST0620.this.eve12.pz);
            } else if (Runtime.getFlags(3026, 1) == 1) {
                ST0620.this.eve12 = new Effect(1419, 12);
                ST0620.this.eve12.disp(true);
            }
            if (Runtime.getFlags(3027, 1) == 0) {
                ST0620.this.eve14 = new Effect(1419, 13);
                ST0620.this.eve14.disp(true);
            } else if (Runtime.getFlags(3027, 1) == 1) {
                ST0620.this.eve14 = new Effect(1419, 13);
                ST0620.this.eve14.disp(true);
                ST0620.this.eve14.setTranslate(ST0620.this.eve14.px, ST0620.this.eve14.py - 3.5f, ST0620.this.eve14.pz);
            }
            ST0620.this.player.setTranslate(4.89f, 0.0f, -14.18f);
            ST0620.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0620.this.elv2.setArgs(4, 0.058333334f);
        }

        void yobidasi() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7084, 1, 0);
            System.sleep(30);
            ST0620.this.win = Window.create();
            ST0620.this.win.setSize(4, 45);
            ST0620.this.win.setLocation(15, 15);
            ST0620.this.win.print(ST0620.this.msgyobidasi1, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            ST0620.this.win = Window.create();
            ST0620.this.win.setSize(4, 45);
            ST0620.this.win.setLocation(15, 305);
            ST0620.this.win.print(ST0620.this.msgyobidasi2, 0);
            ST0620.waitPage(ST0620.this.win, 64);
            System.sleep(20);
            Runtime.setPlayerControl(true);
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
    }
}

