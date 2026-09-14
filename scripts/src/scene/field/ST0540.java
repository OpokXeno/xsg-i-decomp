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
import xeno.map.MC_ELS04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0540
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS04_PRJ {
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
    Enepc npc20;
    Enepc npc21;
    Enepc enemy1;
    Unit unit1;
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
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono item06;
    Uwamono item07;
    Uwamono item08;
    Uwamono item09;
    Uwamono item10;
    Uwamono Base01;
    Uwamono Base02;
    Uwamono Base03;
    Uwamono Base04;
    Uwamono Base05;
    int BUTTON_F = 0;
    int epass = 0;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Unit curry;
    Effect EFcurry;
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
    int page;
    String[] msg011C5348 = new String[]{"/[label(chaos)]", "Good morning, Shion.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5349 = new String[]{"/[label(chaos)]", "Shion, you're always so energetic.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534A = new String[]{"/[label(Shion)]", "Really?\n", "Is that weird?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534B = new String[]{"/[label(chaos)]", "No, not at all. I think it's great. I like that about you.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534D = new String[]{"/[label(Shion)]", "Then chaos, you should be more energetic too. After all, you're younger than I am!", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534E = new String[]{"/[label(chaos)]", "I suppose you're right.", "/[waitkey(64)]/[close()]"};
    String[] msg021C5348 = new String[]{"/[label(chaos)]", "There seems to be a lot of commotion this morning. Is there something wrong?", "/[waitkey(64)]/[clear()]"};
    String[] msg021C5349 = new String[]{"/[label(Shion)]", "Well,\n", "looks like something's wrong with the catapult. I'm going to check on it right now.", "/[waitkey(64)]/[clear()]"};
    String[] msg021C534A = new String[]{"/[label(chaos)]", "Hmm,\n", "the Captain and Tony really abuse the catapult, so it's probably falling apart.", "/[waitkey(64)]/[close()]"};
    String[] msg011CB2F7 = new String[]{"/[label(chaos)]", "Want some company?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F8 = new String[]{"/[label(Shion)]", "No, I'll be fine. I may not look it, but I did develop KOS-MOS after all! A problem or two on a passenger-\ncargo ship should be a piece of cake, you know?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F9 = new String[]{"/[label(chaos)]", "Well, no worries then.", "/[waitkey(64)]/[close()]"};
    String[] msg0107DF8C = new String[]{"/[label(Ziggy)]", "Are you in charge of this ship?", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF8E = new String[]{"/[label(Shion)]", "Oh, no. I'm just a passenger. Were you two the only ones on that ship?", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF90 = new String[]{"/[label(Ziggy)]", "Yes, that's right. I am a Federation government cyborg, Ziggurat...", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF91 = new String[]{"/[label(MOMO)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF92 = new String[]{"/[label(Ziggy)]", "Call me Ziggy...", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF93 = new String[]{"/[label(MOMO)]", "*Smile*", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF95 = new String[]{"/[label(Ziggy)]", "She's MOMO, a Realian. She is with the Federation government just like me.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF96 = new String[]{"/[label(MOMO)]", "Hello, I'm MOMO. Thank you for saving us.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF97 = new String[]{"/[label(Shion)]", "Well, it's a little early for gratitude. There are still some enemies onboard the ship. What exactly are they?", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF99 = new String[]{"/[label(Ziggy)]", "The enemy units that invaded the ship are the U-TIC Organization's Auto-Techs. Basically, they're unmanned craft. We escaped from their base, and they are the last of the pursuit. Unfortunately, the mother ship has a transfer system. The combat units will continue to arrive unless we destroy the mother ship.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9A = new String[]{"/[label(Shion)]", "That's not good news. If we don't do anything about it, this ship will be swarming with enemies and we'll be taken down.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9B = new String[]{"/[label(Ziggy)]", "I'm sorry for getting you involved in this.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9C = new String[]{"/[label(Shion)]", "Oh, don't let it bother you. It isn't your fault. More importantly, we have to do something about that mother ship!", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9D = new String[]{"/[label(Ziggy)]", "Right. We will help, too.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9E = new String[]{"/[label(Shion)]", "We? As in MOMO too?!", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DF9F = new String[]{"/[label(Ziggy)]", "Yes, she might not look it, but she's actually more useful than me.", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DFA1 = new String[]{"/[label(Shion)]", "Really?", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DFA2 = new String[]{"/[label(MOMO)]", "Well, I don't know about that, but I'll do my best!", "/[waitkey(64)]/[clear()]"};
    String[] msg0107DFA3 = new String[]{"/[label(Shion)]", "Okay,\n", "let's hurry!\n", "/[waitkey(64)]/[close()]"};
    String[] DOOR = new String[]{"Press the bulkhead switch?\n", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] Elv_1 = new String[]{"Go to 1F?", "/[waitkey(64)]/[close()]"};
    String[] Elv_2 = new String[]{"Going to 1F.", "/[waitkey(64)]/[close()]"};
    String[] kakuheki_1 = new String[]{"/[label(Shion)]", "It looks like an emergency switch.", "/[waitkey(64)]/[close()]"};
    String[] msgdenji = new String[]{"/[label(Warning)]", "Safety lock of electromagnetic floor confirmed.", "/[waitkey(1)]/[clear()]", "The anti-intruder program cannot be activated at this time. Closing partitions only.", "/[waitkey(64)]/[close()]"};
    String[] msgdenji2 = new String[]{"/[label(Warning)]", "Safety lock of electromagnetic floor confirmed.", "/[waitkey(1)]/[clear()]", "The anti-intruder program cannot be activated at this time. Opening partitions.", "/[waitkey(64)]/[close()]"};
    String[] msgLOCK = new String[]{"/[label(Warning)]", "Use of cargo elevator is restricted to the transportation of cargo only.", "/[waitkey(64)]/[close()]"};
    String[] msghasigo = new String[]{"/[label()]", "Go to the lower level?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0540() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.08f, 4.975f, 4.117f);
        this.camEV.setRotate(-31.437f, 34.739f, 0.0f);
        this.camEV.setFov(45.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.347498f, 2.0599706f, -11.649283f);
        this.camEV.setRotate(-13.913661f, 194.94048f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.807f, 2.7f, 2.506f);
        this.camEV.setRotate(-13.857f, 62.319f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        System.println("bottom***********************");
        switch (n) {
            case 0: {
                System.println("bottom");
                Runtime.setPlayerControl(false);
                System.println("キーコントロール奪いました８");
                this.cam0.setMode(-1);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(550, 2);
                break;
            }
        }
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorD.DoorClose();
        System.sleep(n);
        Sound.effectPlay(196710);
        this.doorC.DoorClose();
        System.sleep(n);
        Sound.effectPlay(196710);
        this.doorB.DoorClose();
        System.sleep(n);
        Sound.effectPlay(196710);
        this.doorA.DoorClose();
        System.sleep(n);
        Sound.effectPlay(196710);
    }

    void Kakuheki_Open() {
        int n = 10;
        Sound.effectPlay(196709);
        this.doorA.DoorOpen();
        System.sleep(n);
        Sound.effectPlay(196709);
        this.doorB.DoorOpen();
        System.sleep(n);
        Sound.effectPlay(196709);
        this.doorC.DoorOpen();
        System.sleep(n);
        Sound.effectPlay(196709);
        this.doorD.DoorOpen();
    }

    public void KickEvent(int var1_1, int var2_2) {
        if (var1_1 != 100) {
            return;
        }
        switch (var2_2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                System.println("キーコントロール奪いました２");
                if (this.S2030 == 0) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.kakuheki_1, 0);
                } else {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.DOOR, 0);
                }
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Press\nDon't press");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        if (Runtime.getFlags(7016, 1) == 0) {
                            Runtime.enable(65536);
                            this.player.mtn(2, 1, 1.0f, true);
                            this.player.move(20, 6.52f, -8.66f, true);
                            System.sleep(20);
                            this.player.rotY(7, 90.0f, true);
                            System.sleep(10);
                            Runtime.disable(65536);
                            System.sleep(5);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(45);
                            Sound.effectPlay(196741);
                            System.sleep(45);
                            this.eve00.disp(false);
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 15);
                            this.win.print(this.msgdenji, 0);
                            ST0540.waitPage(this.win, 64);
                            System.sleep(10);
                            this.Kakuheki_Close();
                            System.sleep(30);
                            this.cam0.setMode(0);
                            System.sleep(1);
                            this.eve00.disp(true);
                            Runtime.setPlayerControl(true);
                            Runtime.disable(65536);
                            Runtime.setFlags(7016, 1, 1);
                            this.BUTTON_F = 0;
                            **break;
                        }
                        Runtime.enable(65536);
                        this.player.mtn(2, 1, 1.0f, true);
                        this.player.move(20, 6.52f, -8.66f, true);
                        System.sleep(20);
                        this.player.rotY(7, 90.0f, true);
                        System.sleep(10);
                        Runtime.disable(65536);
                        System.sleep(5);
                        Runtime.enable(65536);
                        this.player.mtn(26, 1, 1.0f, true);
                        System.sleep(45);
                        Sound.effectPlay(196741);
                        System.sleep(45);
                        this.eve00.disp(false);
                        this.cam0.setMode(-1);
                        this.EV_Camera01();
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgdenji2, 0);
                        ST0540.waitPage(this.win, 64);
                        System.sleep(10);
                        this.Kakuheki_Open();
                        System.sleep(20);
                        this.cam0.setMode(0);
                        System.sleep(1);
                        Runtime.setPlayerControl(true);
                        this.eve00.disp(true);
                        Runtime.disable(65536);
                        Runtime.setFlags(7016, 1, 0);
                        this.BUTTON_F = 0;
                        **break;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                **break;
            }
            case 1: {
                this.doormove = 1;
                System.println("エレベーター");
                **break;
            }
            case 2: {
                if (this.epass == 0) {
                    System.println("**********************************");
                    Runtime.setPlayerControl(false);
                    System.println("キーコントロール奪いました３");
                    this.cam0.setMode(-1);
                    this.EV_Camera04();
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(30, -12.0f, 0.0f, true);
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
                            this.doormove = 2;
                            Sound.effectPlay(196713);
                            Runtime.setFlags(3038, 1, 1);
                            System.sleep(40);
                            Sound.effectPlay(196744);
                            this.ele.setArgs(12, 5.0f);
                            this.doorR2.start(1, "MoveD");
                            System.sleep(60);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.setPlayerControl(true);
                            Runtime.jumpCF(520, 9);
                            this.epass = 1;
                            return;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(50, -7.9f, -0.0f, true);
                    System.sleep(50);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    return;
                }
                this.epass = 1;
                **break;
            }
            case 3: {
                if (this.epass == 0) **break;
                System.println("on!!");
                this.epass = 0;
                **break;
            }
            case 4: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                System.println("キーコントロール奪いました４");
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgLOCK, 0);
                ST0540.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                **break;
            }
            case 5: {
                if (Runtime.getFlags(123, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7141, 1) != 0) **break;
                switch (Runtime.mailReplyCheck(6)) {
                    case 1: {
                        Runtime.mailArriveSet(21);
                        Runtime.setPlayerControl(false);
                        System.println("キーコントロール奪いました５");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        ST0540.waitPage(this.win, 64);
                        System.sleep(15);
                        Runtime.setFlags(7141, 1, 1);
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        **break;
                    }
                    case 2: {
                        Runtime.mailArriveSet(22);
                        Runtime.setPlayerControl(false);
                        System.println("キーコントロール奪いました６");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        ST0540.waitPage(this.win, 64);
                        System.sleep(15);
                        Runtime.setFlags(7141, 1, 1);
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        **break;
                    }
                    default: {
                        return;
                    }
                }
            }
            case 6: {
                if (Runtime.getFlags(162, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(7152, 1) == 0) {
                    switch (Runtime.mailReplyCheck(12)) {
                        case 2: {
                            switch (Runtime.mailReplyCheck(25)) {
                                case 1: {
                                    Runtime.mailArriveSet(44);
                                    Runtime.setPlayerControl(false);
                                    System.println("キーコントロール奪いました７");
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 15);
                                    this.win.print(this.msgMAIL1, 0);
                                    Runtime.setFlags(7152, 1, 1);
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
                                            **break;
                                        }
                                    }
                                    Runtime.setPlayerControl(true);
                                    **break;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
        lbl241:

    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0540.waitPage(window, 64);
    }

    public void Talk_npc21(Enepc enepc) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msghasigo, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go down\nStay here");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                System.sleep(15);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(550, 2);
                return;
            }
        }
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_no(window);
        } else if (this.MOMO == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S2014B == 1) {
            this.Talk_no(window);
        } else if (this.S2014 == 1) {
            this.Talk_no(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg011C5348, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011C5349, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011C534A, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011C534B, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011C534D, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011C534E, 0);
        ST0540.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg021C5348, 0);
                ST0540.waitPage(window, 64);
                window.print(this.msg021C5349, 0);
                ST0540.waitPage(window, 64);
                window.print(this.msg021C534A, 0);
                ST0540.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg011CB2F7, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011CB2F8, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg011CB2F9, 0);
        ST0540.waitPage(window, 64);
    }

    void Talk_npc4_3(Window window) {
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_no(window);
        } else if (this.MOMO == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc6_3(window);
        } else if (this.S2014B == 1) {
            this.Talk_no(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc6_2(window);
        } else if (this.S2013B == 1) {
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
        window.print(this.msg0107DF8C, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF8E, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF90, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF91, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF92, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF93, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF95, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF96, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF97, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF99, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9A, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9B, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9C, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9D, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9E, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DF9F, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DFA1, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DFA2, 0);
        ST0540.waitPage(window, 64);
        window.print(this.msg0107DFA3, 0);
        ST0540.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_no(window);
        } else if (this.MOMO == 1) {
            this.Talk_no(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc7_3(window);
        } else if (this.S2014B == 1) {
            this.Talk_no(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc7_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc7_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc7_1(Window window) {
    }

    void Talk_npc7_2(Window window) {
    }

    void Talk_npc7_3(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 1: {
                Runtime.jumpCF(520, 9);
                break;
            }
            case 2: {
                Runtime.jumpCF(580, 2);
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
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.player.setID(2);
        Stage.setVisible(65, false);
        Stage.setVisible(66, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -9.4f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, 13.5f, -1.0f, -25.0f, 0.0f);
        this.teiten2.SetBgm(196619);
        this.teiten3 = new Uwamono(28690, 7.0f, -1.0f, 13.0f, 0.0f);
        this.teiten3.SetBgm(196619);
        this.teiten4 = new Uwamono(28690, 9.0f, -1.0f, 13.0f, 0.0f);
        this.teiten4.SetBgm(196619);
        this.teiten5 = new Uwamono(28690, 7.5f, 0.0f, -9.5f, 0.0f);
        this.teiten5.SetBgm(196620);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -7.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 6.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 6.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFLockX(10, -8.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFLockX(13, 18.0f);
        this.cam0.setCFAngle(14, 0.0f, -10.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.enemy1 = new Enepc();
        this.enemy1.init(3, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy1.setVisible(false);
        this.enemy1.kickEnepc(4, 2);
        this.enemy1.kickEnepc(19, 1, 0, 385, 1);
        this.enemy1.setInvalidID(1);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 68);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 69);
        this.item03 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 70);
        this.item04 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 71);
        this.item05 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 72);
        this.item06 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 73);
        this.item07 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 74);
        this.item08 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 75);
        new Uwamono(53, 4, this.item01);
        new Uwamono(63, 0);
        new Uwamono(57, 0, this.item02);
        new Uwamono(56, 0);
        new Uwamono(55, 0, this.item03);
        new Uwamono(54, 0);
        new Uwamono(64, 0, this.item04);
        new Uwamono(58, 4, this.item05);
        new Uwamono(51, 21);
        new Uwamono(52, 21);
        new Uwamono(59, 0);
        new Uwamono(62, 0, this.item06);
        new Uwamono(61, 0);
        new Uwamono(60, 4, this.item07);
        new Uwamono(49, 20, this.item08);
        this.Base01 = new Uwamono(28672, 14.0f, -1.0f, -19.0f, 0.0f);
        this.Base01.SetSize(5.0f, 1.0f, 5.0f);
        this.Base02 = new Uwamono(28672, -9.0f, -1.0f, -7.0f, 0.0f);
        this.Base02.SetSize(5.0f, 1.0f, 5.0f);
        this.Base03 = new Uwamono(28672, 17.5f, -1.0f, 9.5f, 0.0f);
        this.Base03.SetSize(5.0f, 1.0f, 5.0f);
        this.Base04 = new Uwamono(28672, 2.66f, -1.0f, 12.46f, 0.0f);
        this.Base04.SetSize(5.0f, 1.0f, 5.0f);
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
        this.eve00 = new Effect(1415, 0);
        this.eve00.disp(true);
        this.eve02 = new Effect(1413, 1);
        this.eve02.disp(true);
        this.doorA = new Uwamono(39, 40, '\u0001');
        new Uwamono(40, 40, '\u0001', this.doorA);
        this.doorB = new Uwamono(41, 40, '\u0001');
        new Uwamono(42, 40, '\u0001', this.doorB);
        this.doorC = new Uwamono(43, 40, '\u0001');
        new Uwamono(44, 40, '\u0001', this.doorC);
        this.doorD = new Uwamono(45, 40, '\u0001');
        new Uwamono(46, 40, '\u0001', this.doorD);
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorA.SetDoorSpd(24);
        this.doorB.SetDoorSpd(24);
        this.doorC.SetDoorSpd(24);
        this.doorD.SetDoorSpd(24);
        if (Runtime.getFlags(7016, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
        } else if (Runtime.getFlags(7016, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
        }
        this.doorE = new Uwamono(37, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
        this.doorF = new Uwamono(38, 40, '\u0001');
        this.doorF.SetDoorType('\u0002');
        this.doorG = new Uwamono(47, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.doorR1 = new Mapunits();
        this.doorR1.mapUnit(71);
        this.doorR1.start(4, null);
        this.doorR1.start(1, "automatic_door");
        this.doorR1.setTranslate(this.doorR1.px, this.doorR1.py, this.doorR1.pz);
        this.doorL1 = new Mapunits();
        this.doorL1.mapUnit(70);
        this.doorL1.start(4, null);
        this.doorL1.setTranslate(this.doorL1.px, this.doorL1.py, this.doorL1.pz);
        this.tA = new Uwamono(28672, -12.0f, -5.5f, 0.0f, 0.0f);
        this.tA.SetHitKind('\u0001');
        this.tA.SetSize(6.5f, 3.0f, 6.5f);
        if (Runtime.getFlags(3039, 1) == 1) {
            System.println("********KOJI_39 is up*********");
            this.ele = new Obj();
            this.ele.initElevator(73, 0.083333336f, 5.0f);
            this.ele.setArgs(1, 0, 1);
            this.ele.start(1, "Up");
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(74);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py + 4.7f, this.doorR2.pz);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(75);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py + 4.7f, this.doorL2.pz);
        } else {
            System.println("********KOJI_39 is not up*********");
            this.ele = new Obj();
            this.ele.initElevator(73, 0.083333336f, 0.0f);
            this.ele.setArgs(1, 0, 1);
            this.ele.setArgs(12, 0.0f);
            Runtime.setPlayerControl(true);
            this.doorR2 = new Mapunits();
            this.doorR2.mapUnit(74);
            this.doorR2.start(4, null);
            this.doorR2.setTranslate(this.doorR2.px, this.doorR2.py - 0.3f, this.doorR2.pz);
            this.doorL2 = new Mapunits();
            this.doorL2.mapUnit(75);
            this.doorL2.start(4, null);
            this.doorL2.setTranslate(this.doorL2.px, this.doorL2.py - 0.3f, this.doorL2.pz);
        }
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_0();
        } else if (this.S2030 == 1) {
            this.npcset_0();
        } else if (this.S2028 == 1) {
            this.npcset_3();
        } else if (this.S2014B == 1) {
            this.npcset_0();
        } else if (this.S2014 == 1) {
            this.npcset_0();
        } else if (this.S2013B == 1) {
            this.npcset_0();
        } else {
            this.npcset_0();
        }
        if (Runtime.getFlags(123, 1) == 0) {
            this.npc21 = new NPC_NORMAL(3, 21, 0, 14, 3, 13.09f, 0.0f, -24.65f, 0.0f);
            this.npc21.talkto("Talk_npc21");
            this.npc21.setInvalidID(1);
            this.npc21.setVisible(false);
            this.Base05 = new Uwamono(28672, 14.3f, 0.0f, -25.61f, 0.0f);
            this.Base05.SetSize(3.0f, 2.0f, 3.0f);
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc4 = new NPC_NORMAL(3, 4, 0, 81, 3, 6.65f, 0.0f, -9.45f, 270.0f);
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_2() {
        this.npc4 = new NPC_NORMAL(3, 4, 0, 81, 3, 6.65f, 0.0f, -9.45f, 270.0f);
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_3() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, -6.83f, 0.0f, -0.31f, 270.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, -7.1f, 0.0f, 1.09f, 220.0f);
        this.npc6.talkto("Talk_npc6");
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

        void Up() {
            ST0540.this.cam0.setMode(-1);
            ST0540.this.EV_Camera04();
            Runtime.setPlayerControl(false);
            System.println("キーコントロール奪いました");
            ST0540.this.player.setTranslate(-10.5f, 6.0f, 0.0f);
            Sound.effectPlay(196745);
            ST0540.this.ele.setArgs(4, 100.0f);
            ST0540.this.ele.setArgs(12, 5.0f);
            System.sleep(1);
            ST0540.this.ele.setArgs(4, 0.083333336f);
            ST0540.this.ele.setArgs(12, 0.0f);
            ST0540.this.doorR2.start(1, "MoveU");
            System.sleep(90);
            Sound.effectPlay(196713);
            ST0540.this.doormove = 1;
            System.sleep(40);
            Runtime.enable(65536);
            ST0540.this.player.mtn(2, 9, 1.0f, true);
            ST0540.this.player.move(30, -8.0f, 0.0f, true);
            System.sleep(35);
            Runtime.disable(65536);
            Runtime.setFlags(3039, 1, 0);
            ST0540.this.cam0.setMode(0);
            ST0540.this.epass = 1;
            Runtime.setPlayerControl(true);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void MoveD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0540.this.doorR2.getTranslate();
                    ST0540.this.doorL2.getTranslate();
                    ST0540.this.doorR2.setTranslate(ST0540.this.doorR2.px, ST0540.this.doorR2.py + 0.083333336f, ST0540.this.doorR2.pz);
                    ST0540.this.doorL2.setTranslate(ST0540.this.doorL2.px, ST0540.this.doorL2.py + 0.083333336f, ST0540.this.doorL2.pz);
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
                    ST0540.this.doorR2.getTranslate();
                    ST0540.this.doorL2.getTranslate();
                    ST0540.this.doorR2.setTranslate(ST0540.this.doorR2.px, ST0540.this.doorR2.py - 0.083333336f, ST0540.this.doorR2.pz);
                    ST0540.this.doorL2.setTranslate(ST0540.this.doorL2.px, ST0540.this.doorL2.py - 0.083333336f, ST0540.this.doorL2.pz);
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
        }

        void MoveU2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0540.this.doorR2.getTranslate();
                    ST0540.this.doorL2.getTranslate();
                    ST0540.this.doorR2.setTranslate(ST0540.this.doorR2.px, ST0540.this.doorR2.py - 0.083333336f, ST0540.this.doorR2.pz);
                    ST0540.this.doorL2.setTranslate(ST0540.this.doorL2.px, ST0540.this.doorL2.py - 0.083333336f, ST0540.this.doorL2.pz);
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
                ST0540.this.player.getTranslate();
                f2 = (-12.0f - ST0540.this.player.px) * (-12.0f - ST0540.this.player.px) + (0.0f - ST0540.this.player.pz) * (0.0f - ST0540.this.player.pz);
                if (f2 > 20.0f) {
                    if (ST0540.this.doormove == 1) {
                        Sound.effectPlay(196713);
                    }
                    ST0540.this.doormove = 2;
                }
                if (ST0540.this.doormove == 1 && f < (float) (n2 + n)) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196713);
                    }
                    if (f == (float) n2) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0540.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0540.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0540.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0540.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                    f += 1.0f;
                }
                if (ST0540.this.doormove == 2 && f > 0.0f) {
                    if ((f -= 1.0f) == 0.0f) {
                        Sound.effectStop(196713);
                    }
                    if (f <= (float) n2) {
                        ST0540.this.doorR1.setRotate(0.0f, f * (float) n3 / (float) n2, 0.0f);
                        ST0540.this.doorL1.setRotate(0.0f, -f * (float) n3 / (float) n2, 0.0f);
                    }
                    if (f >= (float) n) {
                        ST0540.this.doorR2.setRotate(0.0f, (f - (float) n) * (float) n3 / (float) n2, 0.0f);
                        ST0540.this.doorL2.setRotate(0.0f, (-f + (float) n) * (float) n3 / (float) n2, 0.0f);
                    }
                }
                if (f > (float) (n2 + n)) {
                    ST0540.this.doormove = 0;
                }
                if (f <= 0.0f) {
                    ST0540.this.doormove = 0;
                }
                if (f > (float) n2) {
                    ST0540.this.tA.setTranslate(-12.0f, -5.5f, -0.5f);
                } else if (Runtime.getFlags(3038, 1) == 0 && Runtime.getFlags(3039, 1) == 0) {
                    ST0540.this.tA.setTranslate(-12.0f, -1.5f, -0.5f);
                }
                System.sleep(1);
            }
        }
    }
}

