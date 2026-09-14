import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0580
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS08_PRJ {
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
    Unit Bed;
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
    int BUTTON_F = 0;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve00;
    Effect EF01;
    Light light = new Light(0);
    Uwamono teiten1;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg011C5348 = new String[]{"/[label(Shion)]", "Oh, chaos! Were you able to talk with KOS-MOS?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5349 = new String[]{"/[label(chaos)]", "No, she seemed to be asleep, so I didn't want to bother her. I'll try again later.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534A = new String[]{"/[label(Shion)]", "Oh, that's too bad.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534B = new String[]{"/[label(chaos)]", "But she was quite beautiful, even asleep.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534C = new String[]{"/[label(Shion)]", "Why thank you.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534C1 = new String[]{"/[label(Shion)]", "By the way, have you seen the Commander?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534C2 = new String[]{"/[label(chaos)]", "I saw the Commander go into the maintenance lab.", "/[waitkey(1)]/[clear()]", "Just go up the elevator on the left, in the hangar up ahead.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534C3 = new String[]{"/[label(Shion)]", "Really?! You're a great help, thanks.", "/[waitkey(64)]/[close()]"};
    String[] msg011C534C4 = new String[]{"/[label(chaos)]", "I think the Commander is at the top of the left elevator in the hangar.", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "The build of this ship is rather impressive for a passenger freighter. With this much equipment, it could even stand up to a military landing ship. It seems the Captain is quite a deceiver.", "/[waitkey(64)]/[close()]"};
    String[] msg1436E833 = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there. Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] msg01631D47 = new String[]{"/[label(Shion)]", "I wonder if this is a vacant room?", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D47 = new String[]{"/[label(Shion)]", "What do you think, Ziggy? How about we make this your room?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D48 = new String[]{"/[label(Ziggy)]", "Sure, that will be fine.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D49 = new String[]{"/[label(Shion)]", "Then, I'll ask the Captain later. This ship has all sorts of supplies onboard. They might even have a maintenance bed for cyborgs.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D4A = new String[]{"/[label(Ziggy)]", "Sorry for all the trouble. Thank you.", "/[waitkey(64)]/[close()]"};
    String[] msg010FE371 = new String[]{"/[label(Shion)]", "Oh, Ziggy!", "/[waitkey(64)]/[clear()]"};
    String[] msg010FE372 = new String[]{"/[label(Ziggy)]", "What is it?", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37C = new String[]{"/[label(Shion)]", "Commander Cherenkov is missing. Will you help me look for him?", "/[waitkey(64)]/[close()]"};
    String[] msg011FE37E = new String[]{"/[label(Ziggy)]", "The Commander? He is a soldier, so he should have training of that sort. I do not think you need to be overly concerned.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37F = new String[]{"/[label(Shion)]", "But the Captain said this is a bad place for military personnel to wander around.", "/[waitkey(1)]/[clear()]", "So I would feel more secure if you were with me.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE381 = new String[]{"/[label(Ziggy)]", "All right. In that case, I'll help you. Better safe than sorry.", "/[waitkey(64)]/[close()]"};
    String[] msgdenji = new String[]{"/[label(Warning)]", "Safety lock of electromagnetic floor confirmed.", "/[waitkey(1)]/[clear()]", "The anti-intruder program cannot be activated at this time.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0580() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.996f, 1.815f, 5.338f);
        this.camEV.setRotate(-11.034f, -31.018f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.434f, 1.079f, 3.966f);
        this.camEV.setRotate(5.316f, -38.858f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.123f, 2.903f, -2.573f);
        this.camEV.setRotate(-23.822f, -214.337f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.244f, 1.751f, 1.464f);
        this.camEV.setRotate(-7.562f, -78.058f, 0.0f);
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
            case 1: {
                if (Runtime.getFlags(162, 1) == 1) {
                    if (Runtime.getFlags(7151, 1) != 0) break;
                    Runtime.mailArriveSet(43);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    ST0580.waitPage(this.win, 64);
                    System.sleep(15);
                    Runtime.setFlags(7151, 1, 1);
                    Runtime.mailExec(1);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (Runtime.getFlags(143, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(144, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7148, 1) != 0) break;
                switch (Runtime.mailReplyCheck(25)) {
                    case 1: {
                        Runtime.mailArriveSet(38);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
                        Runtime.mailArriveSet(39);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
                        Runtime.mailArriveSet(40);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
            case 2: {
                if (Runtime.getFlags(162, 1) == 1) {
                    if (Runtime.getFlags(7151, 1) != 0) break;
                    Runtime.mailArriveSet(43);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    ST0580.waitPage(this.win, 64);
                    System.sleep(15);
                    Runtime.setFlags(7151, 1, 1);
                    Runtime.mailExec(1);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (Runtime.getFlags(143, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(144, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7148, 1) != 0) break;
                switch (Runtime.mailReplyCheck(25)) {
                    case 1: {
                        Runtime.setPlayerControl(false);
                        Runtime.mailArriveSet(38);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
                        Runtime.mailArriveSet(39);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
                        Runtime.mailArriveSet(40);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        Runtime.setFlags(7148, 1, 1);
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
            case 3: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgdenji, 0);
                ST0580.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    void Talk_no() {
        this.win.print(this.msgtalk_no, 0);
        ST0580.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0580.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc4_4(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S2028 == 1) {
            this.Talk_no(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc4_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg011C5348, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C5349, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C534A, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C534B, 0);
                ST0580.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg011C534C, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C534C1, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C534C2, 0);
                ST0580.waitPage(window, 64);
                window.print(this.msg011C534C3, 0);
                ST0580.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg011C534C4, 0);
        ST0580.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
    }

    void Talk_npc4_3(Window window) {
    }

    void Talk_npc4_4(Window window) {
    }

    public void Talk_npc7(Enepc enepc) {
        if (this.S2057 == 1) {
            this.Talk_no();
        } else if (this.S2042 == 1) {
            this.Talk_no();
        } else if (this.ZIGGY == 1) {
            this.Talk_no();
        } else if (this.S2040C == 1) {
            this.Talk_npc7_2();
        } else if (this.S2030 == 1) {
            this.Talk_npc7_1();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc7_1() {
    }

    void Talk_npc7_2() {
        Runtime.enable(65536);
        this.npc4.kickEnepc(0, 0);
        this.npc10.kickEnepc(0, 0);
        this.fade.call(0);
        System.sleep(30);
        this.npc4.setVisible(true);
        this.npc10.setVisible(true);
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.npc10.kickEnepc(0, 10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg010FE371, 0);
        ST0580.waitPage(this.win, 64);
        this.win.print(this.msg010FE372, 0);
        ST0580.waitPage(this.win, 64);
        this.npc10.kickEnepc(0, 9);
        this.win.print(this.msg011FE37C, 0);
        ST0580.waitPage(this.win, 64);
        this.EV_Camera04();
        this.npc4.setVisible(false);
        this.npc7.kickEnepc(9, -1);
        this.npc7.moveEnepc(17, 340.0f, 0.1f, 20);
        this.npc7.kickEnepc(0, 9);
        this.npc7.setMotion(0, 0);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg011FE37E, 0);
        ST0580.waitPage(this.win, 64);
        this.npc10.kickEnepc(0, 10);
        this.win.print(this.msg011FE37F, 0);
        ST0580.waitPage(this.win, 64);
        this.npc7.kickEnepc(0, 10);
        this.win.print(this.msg011FE381, 0);
        ST0580.waitPage(this.win, 64);
        this.fade.call(0);
        System.sleep(30);
        this.npc10.setVisible(false);
        this.npc7.setVisible(false);
        this.npc7.disableDTKFlag(65536);
        this.player.setTranslate(1.16f, 0.0f, 1.61f);
        Runtime.setFlags(7015, 1, 1);
        Runtime.resetOutFriend(6);
        Runtime.setPartyData(0x1010000, 3);
        Runtime.setPartyData(65538, 1);
        Runtime.setPartyData(0x1010004, 1);
        Runtime.setPartyData(65542, 2);
        Runtime.setPartyData(0x1010008, 5);
        Runtime.setPartyData(65546, 3);
        Runtime.setPartyData(16777260, 1);
        System.println("フラグオン！");
        this.cam0.setMode(0);
        Runtime.disable(65536);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc8_4(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc8_3(window);
        } else if (this.S2028 == 1) {
            this.Talk_no(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc8_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc8_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msg0436E833, 0);
        ST0580.waitPage(window, 64);
    }

    void Talk_npc8_3(Window window) {
        window.print(this.msg1436E833, 0);
        ST0580.waitPage(window, 64);
    }

    void Talk_npc8_4(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(550, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(540, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(620, 2);
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
        if (this.S2040C == 1) {
            this.Bed = new Obj();
            this.Bed.init(20579, 0.5f, -0.2f, 0.0f, 0.0f);
            this.player.setID(2);
        } else {
            this.player.setID(1);
        }
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(0, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 2.8f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196625);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
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
        this.eve00 = new Effect(1444, 0);
        this.eve00.disp(true);
        this.EF01 = new Effect(1011, 2.825f, 1.2f, -6.6f, 0.0f);
        this.EF01.setRotate(-130.0f, 180.0f, 0.0f);
        this.EF01.setScale(0.35f, 0.35f, 1.5f);
        this.EF01.disp(true);
        new Uwamono(28678, -2.0f, 0.0f, -4.5f);
        this.doorA = new Uwamono(11, 40, '\u0001');
        this.doorB = new Uwamono(21, 40, '\u0001');
        this.doorC = new Uwamono(4, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        if (Runtime.getFlags(162, 1) == 1 && Runtime.getFlags(7054, 1) == 0) {
            Runtime.resetOutFriend(2);
            Runtime.resetOutFriend(4);
            Runtime.setFlags(7054, 1, 1);
        }
        if (this.S2042 == 1) {
            this.npcset_6();
        } else if (this.ZIGGY == 1) {
            this.npcset_0();
        } else if (this.S2040C == 1) {
            this.npcset_5();
        } else if (this.S2030 == 1) {
            this.npcset_4();
        } else if (this.MOMO == 1) {
            this.npcset_3();
        } else if (this.S2028 == 1) {
            this.npcset_0();
        } else if (this.S2014B == 1) {
            this.npcset_2();
        } else if (this.S2013B == 1) {
            this.npcset_0();
        } else if (this.S2013 == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc4 = new NPC_NORMAL(3, 4, 0, 81, 3, -0.55f, 0.0f, -1.21f, 90.0f);
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_2() {
        this.npc8 = new NPC_NORMAL(279, 8, 0, 5, 9, -0.55f, 0.0f, -1.21f, 90.0f);
        this.npc8.talkto("Talk_npc8");
    }

    void npcset_3() {
        this.npc8 = new NPC_NORMAL(279, 8, 0, 5, 9, -0.55f, 0.0f, -1.21f, 90.0f);
        this.npc8.talkto("Talk_npc8");
    }

    void npcset_4() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, 0.44f, 0.0f, 1.66f, 20.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, 2.19f, 0.0f, 2.56f, 270.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 15, 1.27f, 0.0f, 1.21f, 330.0f);
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
        if (Runtime.getFlags(7023, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 15, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_5() {
        this.npc7 = new NPC_NORMAL(6, 7, 0, 76, 7, 1.32f, 0.0f, 0.63f, 250.0f);
        this.npc4 = new NPC_NORMAL(3, 4, 0, 13, 3, 0.16f, 0.0f, 1.65f, 125.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 15, 1.16f, 0.0f, 1.61f, 180.0f);
        this.npc4.enableDTKFlag(262144);
        this.npc4.disableDTKFlag(131072);
        this.npc4.disableDTKFlag(65536);
        this.npc4.kickEnepc(4, 1);
        this.npc4.setVisible(false);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 16);
        this.npc10.enableDTKFlag(262144);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        this.npc10.kickEnepc(4, 1);
        this.npc10.setVisible(false);
        this.npc7.talkto("Talk_npc7");
    }

    void npcset_6() {
        Runtime.setFlags(3039, 1, 0);
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
            Runtime.setFlags(7023, 1, 1);
            Runtime.setFlags(7043, 1, 1);
            Runtime.enable(65536);
            ST0580.this.cam0.setMode(-1);
            ST0580.this.EV_Camera01();
            ST0580.this.npc6.kickEnepc(1, 27);
            ST0580.this.npc7.kickEnepc(1, 10);
            ST0580.this.npc6.setVisible(true);
            ST0580.this.npc7.setVisible(true);
            ST0580.this.npc10.setVisible(true);
            System.sleep(10);
            ST0580.this.npc10.kickEnepc(0, 10);
            ST0580.this.win = Window.create();
            ST0580.this.win.setSize(4, 45);
            ST0580.this.win.setLocation(15, 305);
            ST0580.this.win.print(ST0580.this.msg01631D47, 0);
            ST0580.waitPage(ST0580.this.win, 64);
            ST0580.this.npc10.kickEnepc(1, 9);
            ST0580.this.npc10.moveEnepc(17, 20.0f, 0.1f, 20);
            ST0580.this.win.print(ST0580.this.msg11631D47, 0);
            ST0580.waitPage(ST0580.this.win, 64);
            ST0580.this.npc10.kickEnepc(1, 10);
            ST0580.this.npc7.kickEnepc(0, 9);
            ST0580.this.win.print(ST0580.this.msg01631D48, 0);
            ST0580.waitPage(ST0580.this.win, 64);
            ST0580.this.npc6.moveEnepc(17, 45.0f, 0.1f, 20);
            ST0580.this.npc10.kickEnepc(1, 9);
            ST0580.this.win.print(ST0580.this.msg01631D49, 0);
            ST0580.waitPage(ST0580.this.win, 64);
            ST0580.this.npc7.kickEnepc(0, 7);
            ST0580.this.win.print(ST0580.this.msg01631D4A, 0);
            ST0580.waitPage(ST0580.this.win, 64);
            System.sleep(10);
            ST0580.this.fade.call(0);
            System.sleep(30);
            ST0580.this.npc6.setVisible(false);
            ST0580.this.npc7.setVisible(false);
            ST0580.this.npc10.setVisible(false);
            ST0580.this.player.setTranslate(0.17f, 0.0f, 4.11f);
            ST0580.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

