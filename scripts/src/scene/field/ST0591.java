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

class ST0591
        extends Stage
        implements XenoConstants,
        CfConstants,
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
    String[] msg0436E83C = new String[]{"/[label(Shion)]", "This is the men's cabin.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E83C = new String[]{"/[label(Shion)]", "As you can see, this men's cabin holds true to the universal law; it's quite messy.", "/[waitkey(64)]/[clear()]"};
    String[] msg2436E83C = new String[]{"/[label(Shion)]", "We won't really have anything to do with this place, MOMO.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E83E = new String[]{"/[label(MOMO)]", "Really? Am I not allowed to come to this room?", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E840 = new String[]{"/[label(Shion)]", "No, you can, but it's nasty.", "/[waitkey(64)]/[close()]"};
    String[] msgziggy1 = new String[]{"/[label(Shion)]", "Oh, MOMO! Have you seen Ziggy?", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy2 = new String[]{"/[label(MOMO)]", "Ziggy? Please wait a minute.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "I found him! He just got off the elevator and is walking down the corridor below.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy3 = new String[]{"/[label(Shion)]", "Wow! You really are amazing, MOMO.", "/[waitkey(64)]/[clear()]"};
    String[] msgziggy4 = new String[]{"/[label(MOMO)]", "Hee hee hee, thank you very much!", "/[waitkey(64)]/[close()]"};
    String[] msgziggy5 = new String[]{"/[label(Shion)]", "By the way, what are you doing here?", "/[waitkey(64)]/[clear()]"};
    String[] msg0212875E = new String[]{"/[label(Shion)]", "MOMO, what are you doing?", "/[waitkey(64)]/[clear()]"};
    String[] msg0212875F = new String[]{"/[label(MOMO)]", "Oh, um, I was cleaning.", "/[waitkey(1)]/[clear()]", "The room was messy, so I thought I would tidy it up.", "/[waitkey(1)]/[clear()]", "Was that wrong of me?", "/[waitkey(64)]/[clear()]"};
    String[] msg02128760 = new String[]{"/[label(Shion)]", "Oh, not at all. MOMO, I bet you'll make a lucky guy very happy someday.", "/[waitkey(64)]/[clear()]"};
    String[] msg02128761 = new String[]{"/[label(MOMO)]", "Hee hee! I hope so!", "/[waitkey(64)]/[close()]"};
    String[] msg02128762 = new String[]{"/[label(MOMO)]", "If I work hard and become a good wife, it will make Ziggy happy too, won't it?!", "/[waitkey(64)]/[close()]"};
    String[] msg1436E833 = new String[]{"/[label(Cherenkov)]", "Sorry, leave me alone for a while.", "/[waitkey(64)]/[close()]"};
    String[] msg043747DF = new String[]{"/[label(Cherenkov)]", "I'm in no mood to play right now. Please leave me alone.", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT = new String[]{"/[label()]", "You seem quite worn out. Don't push yourself too hard, it's bad for your health. How about getting some good rest?", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT1 = new String[]{"/[label()]", "Yes, I think that will be for the best. Pushing yourself will not result in anything good. Well then, good night...", "/[waitkey(64)]/[close()]"};
    String[] msgNIGHT2 = new String[]{"/[label()]", "I see, that's too bad. Please don't push yourself too hard.", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Cabin 1'", "/[waitkey(64)]/[close()]"};

    ST0591() {
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
                ST0591.waitPage(this.win, 64);
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
        ST0591.waitPage(window, 64);
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
                ST0591.waitPage(this.win, 64);
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
                ST0591.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNIGHT2, 0);
        ST0591.waitPage(this.win, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc5_1(Window window) {
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc6_1(Window window) {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(521, 3);
                break;
            }
            case 1: {
                Runtime.jumpCF(521, 4);
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
        if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_0();
        } else {
            this.npcset_0();
        }
        this.npc15 = new NPC_NORMAL(1612, 15, 0, 14, 3, -5.22f, 0.1f, -4.94f, 45.0f);
        this.npc15.talkto("Talk_npc15");
        this.npc15.setMotion(0, 10);
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
    }
}

