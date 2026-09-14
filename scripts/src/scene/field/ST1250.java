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
import xeno.map.MC_KOU05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1250
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KOU05_PRJ {
    Player player;
    Camera cam1;
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
    Enepc npc18;
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
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int talkFlag14;
    int talkFlag15;
    int talkFlag16;
    int talkFlag17;
    int talkFlag18;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag6;
    int S2040C;
    int S2042;
    int S3;
    int angou1;
    int angou2;
    Light light = new Light(0);
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Effect light01;
    Effect light02;
    Effect light03;
    int page;
    String[] msg0001 = new String[]{"/[label(Shopkeeper)]", "What is it? If you want something, talk to the clerk upstairs.", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label(Shopkeeper)]", "\"○○○?\"", "/[waitkey(1)]/[clear()]", "...Where did you hear about that?", "/[waitkey(1)]/[clear()]", "Never mind, business is business. There's a man outside the shop. You'll know right away who I'm talking about because he's been watching you.", "/[waitkey(1)]/[clear()]", "You should give that man some money.", "/[waitkey(1)]/[clear()]", "If you get what I'm talkin' about, get the hell out of here! You're keeping me from my work.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 2.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 2.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 2, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] msgmail1 = new String[]{"/[label(Jank)]", "Hmm? You must really want to hear what I have to say for you to come all this way. Heh heh heh. Very well, if you want, I'll tell you some juicy inside info.", "/[waitkey(64)]/[close()]"};
    String[] msgmail2 = new String[]{"/[label(Jank)]", "Heh heh heh heh heh...\n", "/[waitkey(1)]/[clear()]", "I see, you are interested. Well then, first tell me your email address.", "/[waitkey(1)]/[clear()]", "You never know who might be listening out there, so it's best to exchange valuable information by email.", "/[waitkey(1)]/[clear()]", "Heh heh heh...", "/[waitkey(64)]/[close()]"};
    String[] msgmail3 = new String[]{"/[label(Jank)]", "Heh heh heh. Oh, don't worry. You won't regret it. Heh heh heh heh heh...", "/[waitkey(1)]/[clear()]", "Then, I'll send you email later. Enjoy it at your leisure. Heh heh heh...", "/[waitkey(64)]/[close()]"};
    String[] msgmail4 = new String[]{"/[label(Jank)]", "That's too bad. You don't want to know about this juicy tidbit, huh?", "/[waitkey(1)]/[clear()]", "Heh heh heh heh...", "/[waitkey(64)]/[close()]"};
    String[] msgmail5 = new String[]{"/[label(Jank)]", "Heh heh heh...", "/[waitkey(1)]/[clear()]", "I see, so you've changed your mind. The curiosity is eating away at you, isn't it?", "/[waitkey(64)]/[close()]"};
    String[] msgmail6 = new String[]{"/[label(Jank)]", "Heh heh heh...", "/[waitkey(1)]/[clear()]", "Are you enjoying life? Heh heh heh heh heh...", "/[waitkey(64)]/[close()]"};

    ST1250() {
    }

    void Final_init(int n) {
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
                if (Runtime.getFlags(3202, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(55);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3202, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3222, 1) == 0) {
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
                if (Runtime.getFlags(3282, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                this.doorA.SetDoorType('\u0004');
                Runtime.setFlags(3282, 1, 1);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc1_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc1_1(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
        switch (this.angou1) {
            case 1: {
                window.print(this.msg0002, 0);
                ST1250.waitPage(window, 64);
                Runtime.setFlags(7017, 1, 1);
                return;
            }
        }
        window.print(this.msg0001, 0);
        ST1250.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
    }

    public void Talk_npc2(Enepc enepc) {
        if (Runtime.getFlags(7057, 2) == 2) {
            this.Talk_npc2_3();
        } else if (Runtime.getFlags(7169, 2) == 0) {
            this.Talk_npc2_1();
        } else if (Runtime.getFlags(7169, 2) == 1) {
            this.Talk_npc2_2();
        } else {
            this.Talk_npc2_3();
        }
    }

    void Talk_npc2_1() {
        Runtime.setFlags(7169, 2, 1);
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail1, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Uh, if it's quick\nI'm sorry, I'm in a hurry");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail2, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Sure, that's fine\nNo, I don't think so");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(7169, 2, 2);
                        Runtime.setFlags(7057, 2, 2);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgmail3, 0);
                        ST1250.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail4, 0);
                ST1250.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmail4, 0);
        ST1250.waitPage(this.win, 64);
    }

    void Talk_npc2_2() {
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail5, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I really want to know\nNo thank you");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail2, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Sure, that's fine\nNo, I don't think so");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(7169, 2, 2);
                        Runtime.setFlags(7057, 2, 2);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgmail3, 0);
                        ST1250.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail4, 0);
                ST1250.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmail4, 0);
        ST1250.waitPage(this.win, 64);
    }

    void Talk_npc2_3() {
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail6, 0);
        ST1250.waitPage(this.win, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1240, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1270, 1);
                break;
            }
        }
    }

    void init() {
        this.S2040C = Runtime.getFlags(158, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S3 = Runtime.getFlags(179, 1);
        this.angou1 = Runtime.getFlags(7016, 1);
        this.angou2 = Runtime.getFlags(7017, 1);
        this.teiten1 = new Uwamono(28690, -11.409f, -0.32f, 4.336f, 0.0f);
        this.teiten1.SetBgm(196615);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(14, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 10.0f, 0.0f, 8.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.275f, 0.275f, 0.275f);
        this.light.setColor(1, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1631, -6.476f, -1.575f, 2.638f, 0.0f);
        this.light01.setScale(0.5f, 0.15f, 2.0f);
        this.light02 = new Effect(1405, -9.141f, -0.561f, 2.241f, 0.0f);
        new Uwamono(26, 31);
        new Uwamono(23, 13);
        new Uwamono(24, 13);
        new Uwamono(25, 13);
        new Uwamono(13, 9);
        if (Runtime.getFlags(3202, 1) == 0) {
            new Uwamono(27, 31);
        } else {
            Stage.setVisible(27, false);
        }
        if (Runtime.getFlags(3282, 1) == 0) {
            this.doorA = new Uwamono(36, 40, '\u0004');
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA = new Uwamono(36, 40, '\u0004');
            this.doorA.SetDoorType('\u0004');
        }
        this.npcset_1();
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1552, 1, 0, 14, 3, -8.63f, -3.2f, 2.96f, 120.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 9);
        this.npc1.talkto("Talk_npc2");
    }

    void npcset_2() {
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

