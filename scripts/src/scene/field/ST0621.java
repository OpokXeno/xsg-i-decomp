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

class ST0621
        extends Stage
        implements XenoConstants,
        CfConstants,
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
    Uwamono teiten4;
    int page;
    String[] msg00B88FA5 = new String[]{"It's not here.\n", "Nor there.\n", "Or anywhere.", "/[waitkey(1)]/[clear()]", "Man, where the heck is it? Damn, you should be more organized. Whoever's responsible, come on out!", "/[waitkey(1)]/[clear()]", "That would be me! I'm the one responsible!", "/[waitkey(1)]/[clear()]", "Okay, this is not the time to be doing these kinds of stupid things!", "/[waitkey(64)]/[clear()]"};
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
    String[] msg01631D54 = new String[]{"/[label(Shion)]", "This is Hangar 1. They bring out the A.G.W.S. from here to the catapult.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D55 = new String[]{"/[label(MOMO)]", "Oh! That's your A.G.W.S., isn't it?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D56 = new String[]{"/[label(MOMO)]", "That's so great! It's so cool!", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D57 = new String[]{"/[label(Shion)]", "What? You want to pilot an /[font(A.G.W.S.)] too?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D58 = new String[]{"/[label(MOMO)]", "Yes! I would like to pilot an /[font(A.G.W.S.)] and help Ziggy!", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D59 = new String[]{"/[label(Shion)]", "Hmm, I see. So what kind do you want to pilot?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D60 = new String[]{"/[label(MOMO)]", "Well, let's see.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D61 = new String[]{"/[label(MOMO)]", "Tee hee...I want an /[font(A.G.W.S.)] like Ziggy.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D62 = new String[]{"/[label(Shion)]", "I see...one that's...like Ziggy...", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D63 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ELE = new String[]{"Use the lift?", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgrobo = new String[]{"/[label()]", "I'm the hardest worker on this ship.", "/[waitkey(1)]/[clear()]", "I even installed the locks on the slide deck.", "/[waitkey(1)]/[clear()]", "But when I was cleaning one of the cabins, I lost the /[color(0x329bbe)]Disarm Key/[color(0x808080)].", "/[waitkey(1)]/[clear()]", "Oh boy, oh no...", "/[waitkey(1)]/[clear()]", "It should be easy to find though. All I need to do is press the ○ Button.", "/[waitkey(1)]/[clear()]", "But it's such a pain because the cabin's huge.", "/[waitkey(1)]/[clear()]", "Oh boy, oh no...", "/[waitkey(64)]/[close()]"};
    String[] msgyobidasi1 = new String[]{"/[label(Ship Intercom)]", "Hey, Ms. Vector! If you're just lounging around, come to the bridge for a sec!", "/[waitkey(64)]/[close()]"};
    String[] msgyobidasi2 = new String[]{"/[label(Shion)]", "What could it be? I wonder if something happened?", "/[waitkey(64)]/[close()]"};
    String[] msgshoprobo1 = new String[]{"/[label()]", "hellO! i am in charge of maintaining this shiP!", "/[waitkey(1)]/[clear()]", "is there anything you neeD? i will sell you anything on this ship for cheaP! i'd appreciate your patronage so the captaiN can repay his debtS!", "/[waitkey(64)]/[close()]"};
    String[] msgshoprobo2 = new String[]{"/[label()]", "thank yoU! thank you very mucH! i look forward to your next visiT!", "/[waitkey(64)]/[close()]"};

    ST0621() {
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
        ST0621.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0621.waitPage(window, 64);
    }

    public void Talk_npc16(Enepc enepc) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgshoprobo1, 0);
        ST0621.waitPage(this.win, 64);
        System.sleep(15);
        if (Runtime.getFlags(389, 1) == 1) {
            Runtime.enterShop(14);
        } else if (Runtime.getFlags(372, 1) == 1) {
            Runtime.enterShop(12);
        } else if (Runtime.getFlags(326, 1) == 1) {
            Runtime.enterShop(8);
        } else {
            Runtime.enterShop(3);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgshoprobo2, 0);
        ST0621.waitPage(this.win, 64);
    }

    public void Talk_npc17(Enepc enepc, Window window) {
        window.print(this.msgrobo, 0);
        ST0621.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc2_2() {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(327, 1) == 1) {
                    Runtime.jumpCF(571, 1);
                    break;
                }
                if (Runtime.getFlags(326, 1) != 1) {
                    if (Runtime.getFlags(315, 1) == 1) {
                        Runtime.jumpCF(571, 1);
                        break;
                    }
                    Runtime.jumpCF(571, 1);
                    break;
                }
                Runtime.setFlags(327, 1, 1);
                System.println("Flag on!");
                Runtime.jumpEvent(3160);
                Runtime.jumpCF(571, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(581, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(631, 1);
                break;
            }
        }
    }

    void init() {
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
        Stage.renderCommand(4);
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
        if (Runtime.getFlags(373, 1) == 1) {
            this.teiten4 = new Uwamono(28690, 3.0f, -1.1f, 0.0f, 0.0f);
            this.teiten4.SetBgm(196637);
        }
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
        this.eve02 = new Effect(1420, 2);
        this.eve02.disp(true);
        this.eve03 = new Effect(1420, 3);
        this.eve03.disp(true);
        if (Runtime.getFlags(373, 1) == 1) {
            this.eve04 = new Effect(1420, 4);
            this.eve04.disp(true);
        }
        this.eve08 = new Effect(1013, 8);
        this.eve08.disp(true);
        this.eve09 = new Effect(1013, 9);
        this.eve09.disp(true);
        if (Runtime.getFlags(373, 1) == 1) {
            this.eve10 = new Effect(1013, 10);
            this.eve10.disp(true);
        }
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
        if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_0();
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
        this.npc15.setInvalidID(1);
        this.npc15.setMotion(0, 2);
        this.npc15.dispRadar(false);
        this.npc15.renderCommand(23);
        this.npc15.disableDTKFlag(131072);
        this.npc15.disableDTKFlag(65536);
        if (Runtime.getFlags(373, 1) == 1) {
            this.npc14 = new NPC_NORMAL2(8194, 14, 0, 17, 16, 5.0f, -1.0f, 0.0f, 270.0f);
            this.npc14.setInvalidID(1);
            this.npc14.setMotion(0, 2);
            this.npc14.dispRadar(false);
            this.npc14.renderCommand(23);
            this.npc14.disableDTKFlag(131072);
            this.npc14.disableDTKFlag(65536);
        }
        this.npc12.setInvalidID(1);
        this.npc15.setInvalidID(1);
    }

    void npcset_0() {
    }

    void npcset_1() {
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
                    ST0621.this.eve12.getTranslate();
                    ST0621.this.eve12.setTranslate(ST0621.this.eve12.px, ST0621.this.eve12.py + 0.058333334f, ST0621.this.eve12.pz);
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
                    ST0621.this.eve12.getTranslate();
                    ST0621.this.eve12.setTranslate(ST0621.this.eve12.px, ST0621.this.eve12.py - 0.058333334f, ST0621.this.eve12.pz);
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
                    ST0621.this.eve14.getTranslate();
                    ST0621.this.eve14.setTranslate(ST0621.this.eve14.px, ST0621.this.eve14.py - 0.058333334f, ST0621.this.eve14.pz);
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
                    ST0621.this.eve14.getTranslate();
                    ST0621.this.eve14.setTranslate(ST0621.this.eve14.px, ST0621.this.eve14.py + 0.058333334f, ST0621.this.eve14.pz);
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

