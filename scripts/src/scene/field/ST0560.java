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
import xeno.map.MC_ELS06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0560
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS06_PRJ {
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
    int BUTTON_F = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono itembox;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Light light = new Light(0);
    Uwamono teiten1;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg042F9E82 = new String[]{"/[label(Hammer)]", "Huh? What are you doing?", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E83 = new String[]{"/[label(Shion)]", "Oh, nothing. I'm just looking for the Commander.", "/[waitkey(1)]/[clear()]", "Hey, what happened to the pod we were in?", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E84 = new String[]{"/[label(Hammer)]", "Oh, if you mean the escape pod, we cleared it out already.", "/[waitkey(1)]/[clear()]", "After all, it may be small, but military escape pods use cutting edge technology. They sell for quite a lot to the right people.", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E85 = new String[]{"/[label(Shion)]", "I see, so you put the pod up for sale?", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E86 = new String[]{"/[label(Hammer)]", "Oh, shoot...", "/[waitkey(1)]/[clear()]", "I'm really sorry! Please pretend that you didn't hear that! It was all the Captain's idea.", "/[waitkey(64)]/[clear()]"};
    String[] msg042F9E87 = new String[]{"/[label(Shion)]", "Oh, it's completely all right. It's not like it belonged to me, and I'm sure to the military, it's just another piece of equipment.", "/[waitkey(64)]/[close()]"};
    String[] msg042FFE34 = new String[]{"/[label(Shion)]", "But Hammer, aren't you awfully quick to point a finger at the Captain?", "/[waitkey(64)]/[clear()]"};
    String[] msg042FFE35 = new String[]{"/[label(Hammer)]", "Oh, don't worry about it. After all, you're the only one who can protect yourself.", "/[waitkey(64)]/[close()]"};
    String[] msg01631D3F = new String[]{"/[label(Shion)]", "This is the catapult deck. This is where we took in the two of you earlier.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D40 = new String[]{"/[label(MOMO)]", "You saved us with the fork arm. Thank you so much.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D41 = new String[]{"/[label(Shion)]", "Not at all. We have to help each other out in times of need. I'm really sorry about your ship though. We had to throw it out because it was badly damaged.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D42 = new String[]{"/[label(Ziggy)]", "That's not a problem. It wasn't ours to begin with.", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D42 = new String[]{"/[label(Shion)]", "Oh, really?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D43 = new String[]{"/[label(Ziggy)]", "Yes, we borrowed it in order to escape.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D44 = new String[]{"/[label(MOMO)]", "You are amazing! I can't believe you actually caught our small ship using that arm.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D45 = new String[]{"/[label(Shion)]", "I just happened to have some practice operating a crane on the previous ship I was on. ", "That really helped out. But it was mostly luck.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D46 = new String[]{"/[label(MOMO)]", "Tee hee...", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Chief! Are you all right?! Are you hurt anywhere?! Thank goodness! You're always so reckless. What in the world happened?", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D17 = new String[]{"/[label(Shion)]", "I don't know. When I came to, I'd been swallowed by a Gnosis.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D18 = new String[]{"/[label(Allen)]", "What?! You were eaten by a Gnosis?!", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D19 = new String[]{"/[label(Shion)]", "Yes, and I found Commander Cherenkov at its center. But I wasn't able to save him. I don't know. I just don't understand how that all happened.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B81D1A = new String[]{"/[label(Allen)]", "Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D1B = new String[]{"/[label(Allen)]", "Umm...Let's get you upstairs for now. The Captain and the others are waiting for you on the bridge!", "/[waitkey(64)]/[close()]"};
    String[] msgkakunin1 = new String[]{"/[label(Shion)]", "Hmm...everything checks out normal.", "/[waitkey(64)]/[close()]"};
    String[] msgkakunin2 = new String[]{"/[label(Shion)]", "Hmmm, nothing seems to be wrong.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 14.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 14.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 14, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0560() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.56f, 1.439f, 21.913f);
        this.camEV.setRotate(-3.239f, 19.619f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.392f, 1.824f, 13.055f);
        this.camEV.setRotate(-8.479f, 150.537f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.985f, 3.047f, -37.344f);
        this.camEV.setRotate(-25.162f, -155.337f, 0.0f);
        this.camEV.setFov(34.999f);
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
                if (this.S2014 == 0) {
                    return;
                }
                if (this.S2014B != 0) break;
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.mtn(26, 1, 1.0f, true);
                System.sleep(40);
                Sound.effectPlay(196741);
                System.sleep(40);
                this.eve03.disp(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgkakunin1, 0);
                ST0560.waitPage(this.win, 64);
                Runtime.disable(65536);
                System.sleep(10);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgkakunin2, 0);
                ST0560.waitPage(this.win, 64);
                System.sleep(10);
                this.fade.call(0);
                System.sleep(30);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                Runtime.setFlags(125, 1, 1);
                Runtime.jumpEvent(2141);
                System.println("フラグオン！");
                this.BUTTON_F = 0;
                break;
            }
            case 2: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(3214, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(55);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3214, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3234, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3294, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                this.doorB.SetDoorType('\u0004');
                Runtime.setFlags(3294, 1, 1);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 3: {
                if (Runtime.getFlags(125, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(126, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7146, 1) != 0) break;
                switch (Runtime.mailReplyCheck(6)) {
                    case 1: {
                        Runtime.mailArriveSet(29);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        ST0560.waitPage(this.win, 64);
                        System.sleep(15);
                        Runtime.setFlags(7146, 1, 1);
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                }
            }
            case 4: {
                if (Runtime.getFlags(143, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(144, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7057, 2) != 2) {
                    return;
                }
                if (Runtime.getFlags(7147, 1) != 0) break;
                Runtime.mailArriveSet(34);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7147, 1, 1);
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

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0560.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc3_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc3_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg042F9E82, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg042F9E83, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg042F9E84, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg042F9E85, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg042F9E86, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg042F9E87, 0);
                ST0560.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg042FFE34, 0);
        ST0560.waitPage(window, 64);
        window.print(this.msg042FFE35, 0);
        ST0560.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
    }

    void Talk_npc3_3(Window window) {
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc5_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc5_1(Window window) {
    }

    void Talk_npc5_2(Window window) {
    }

    void Talk_npc5_3(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                Runtime.setPlayerControl(false);
                window.print(this.msg00B81D16, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg00B81D17, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg00B81D18, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg00B81D19, 0);
                ST0560.waitPage(window, 64);
                window.print(this.msg00B81D1A, 0);
                ST0560.waitPage(window, 64);
                Runtime.setPlayerControl(true);
                return;
            }
        }
        Runtime.setPlayerControl(false);
        window.print(this.msg00B81D1B, 0);
        ST0560.waitPage(window, 64);
        Runtime.setPlayerControl(true);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc6_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc6_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
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

    public void Talk_npc7(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc7_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc7_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_no(window);
        } else if (this.S2009 == 1) {
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
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(550, 1);
                break;
            }
            case 1: {
                Runtime.setFlags(7160, 1, 0);
                Runtime.jumpCF(550, 3);
                break;
            }
            case 2: {
                System.println("54_ON");
                Runtime.setFlags(3054, 1, 1);
                Runtime.jumpCF(650, 1);
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
        Stage.setVisible(8, false);
        Stage.setVisible(391, false);
        Stage.setVisible(218, false);
        Stage.setVisible(0, false);
        Stage.setVisible(392, false);
        Stage.setVisible(2, false);
        Stage.setVisible(219, false);
        Stage.setVisible(6, false);
        Stage.setVisible(7, false);
        Stage.setVisible(9, false);
        Stage.setVisible(10, false);
        Stage.setVisible(1, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(431, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, 1.0f, -35.8f, 0.0f);
        this.teiten1.SetBgm(196623);
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
        this.cam0.setCFAngle(2, 0.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFLockX(2, 7.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -10.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 7.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 7.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.item01 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 80);
        this.item02 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 81);
        this.item03 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 82);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 83);
        this.item05 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 84);
        this.item06 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 85);
        this.item07 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 86);
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 270.0f, 419);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        new Uwamono(439, 23, this.item01);
        new Uwamono(438, 23, this.item02);
        new Uwamono(435, 23);
        new Uwamono(436, 23, this.item03);
        new Uwamono(437, 23, this.item04);
        new Uwamono(420, 4);
        new Uwamono(419, 4);
        new Uwamono(421, 4, this.item05);
        new Uwamono(423, 0, this.itembox);
        new Uwamono(422, 0);
        new Uwamono(424, 0, this.item06);
        new Uwamono(425, 0, this.item07);
        if (Runtime.getFlags(3214, 1) == 0) {
            new Uwamono(440, 23);
        } else {
            Stage.setVisible(440, false);
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
        this.eve00 = new Effect(1443, 0);
        this.eve00.disp(true);
        this.eve01 = new Effect(1443, 1);
        this.eve01.disp(true);
        this.eve02 = new Effect(1443, 2);
        this.eve02.disp(true);
        if (Runtime.getFlags(125, 1) == 1) {
            this.eve03 = new Effect(1442, 3);
            this.eve03.disp(false);
        } else if (Runtime.getFlags(124, 1) == 0) {
            this.eve03 = new Effect(1442, 3);
            this.eve03.disp(false);
        } else {
            this.eve03 = new Effect(1442, 3);
            this.eve03.disp(true);
        }
        this.doorA = new Uwamono(432, 40, '\u0001');
        new Uwamono(433, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(3294, 1) == 0) {
            this.doorB = new Uwamono(434, 40, '\u0004');
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB = new Uwamono(434, 40, '\u0004');
            this.doorB.SetDoorType('\u0004');
        }
        if (this.S2057 == 1) {
            this.npcset_3();
        } else if (this.S2040C == 1) {
            this.npcset_0();
        } else if (this.S2030 == 1) {
            this.npcset_2();
        } else if (this.S2013B == 1) {
            this.npcset_0();
        } else if (this.S2009 == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3230, 1, 1);
                break;
            }
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc3 = new NPC_NORMAL(278, 3, 0, 1, 3, 0.49f, 0.0f, 8.24f, 0.0f);
        this.npc3.talkto("Talk_npc3");
    }

    void npcset_2() {
        if (Runtime.getFlags(7160, 1) == 1) {
            return;
        }
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 7, 2.96f, 0.0f, 17.58f, 225.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 9, 1.213f, 0.0f, 17.673f, 180.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 14, 1.97f, 0.0f, 16.16f, 0.0f);
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
        if (Runtime.getFlags(7021, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 14, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_3() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 13, 5, 1.07f, 0.0f, 22.01f, 180.0f);
        this.npc5.setMotion(0, 10);
        this.npc5.talkto("Talk_npc5");
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
            Runtime.setFlags(7021, 1, 1);
            Runtime.enable(65536);
            ST0560.this.cam0.setMode(-1);
            ST0560.this.EV_Camera01();
            ST0560.this.npc10.kickEnepc(0, 9);
            ST0560.this.npc6.kickEnepc(1, 27);
            ST0560.this.npc7.kickEnepc(1, 10);
            ST0560.this.npc6.setVisible(true);
            ST0560.this.npc7.setVisible(true);
            ST0560.this.npc10.setVisible(true);
            System.sleep(10);
            ST0560.this.win = Window.create();
            ST0560.this.win.setSize(4, 45);
            ST0560.this.win.setLocation(15, 305);
            ST0560.this.win.print(ST0560.this.msg01631D3F, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.EV_Camera02();
            System.sleep(10);
            ST0560.this.npc6.moveEnepc(17, 180.0f, -0.1f, 20);
            ST0560.this.npc7.moveEnepc(17, 120.0f, -0.1f, 40);
            ST0560.this.npc10.moveEnepc(17, 20.0f, 0.1f, 30);
            ST0560.this.npc7.kickEnepc(1, 0);
            ST0560.this.npc6.kickEnepc(1, 9);
            ST0560.this.win.print(ST0560.this.msg01631D40, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc10.kickEnepc(0, 9);
            ST0560.this.npc6.kickEnepc(1, 10);
            ST0560.this.win.print(ST0560.this.msg01631D41, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc7.moveEnepc(17, 160.0f, 0.1f, 10);
            ST0560.this.npc7.kickEnepc(0, 7);
            ST0560.this.win.print(ST0560.this.msg01631D42, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc10.moveEnepc(17, 340.0f, -0.1f, 10);
            ST0560.this.npc7.kickEnepc(1, 27);
            ST0560.this.npc6.kickEnepc(1, 27);
            ST0560.this.win.print(ST0560.this.msg11631D42, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc10.kickEnepc(1, 10);
            ST0560.this.npc7.kickEnepc(1, 9);
            ST0560.this.win.print(ST0560.this.msg01631D43, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc6.kickEnepc(1, 9);
            ST0560.this.win.print(ST0560.this.msg01631D44, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc10.moveEnepc(17, 20.0f, 0.1f, 10);
            ST0560.this.npc10.kickEnepc(0, 9);
            ST0560.this.win.print(ST0560.this.msg01631D45, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            ST0560.this.npc6.kickEnepc(0, 7);
            ST0560.this.win.print(ST0560.this.msg01631D46, 0);
            ST0560.waitPage(ST0560.this.win, 64);
            System.sleep(10);
            ST0560.this.fade.call(0);
            System.sleep(30);
            ST0560.this.npc6.setVisible(false);
            ST0560.this.npc7.setVisible(false);
            ST0560.this.npc10.setVisible(false);
            ST0560.this.npc11.setVisible(false);
            ST0560.this.player.setTranslate(2.52f, 0.0f, 24.0f);
            ST0560.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

