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
import xeno.map.MC_VOK13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0130
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK13_PRJ {
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
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    Uwamono doorA;
    int selected0 = 0;
    int selected1 = 2;
    int exit0 = 0;
    int S014A;
    int S014B;
    int S015B;
    int SHION_ROOM;
    int GET_MWS;
    Chr AGWS01;
    Chr AGWS02;
    Effect fade;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Unit kidou;
    int page;
    String[] msg0ECA1A9E = new String[]{"/[label()]", "Besides, are we going to see any action during a large space battle between fleets?", "/[waitkey(64)]/[close()]"};
    String[] msg0ECA7A4B = new String[]{"/[label()]", "Besides, what are Gnosis anyway? Are they mobile weapons like these guys? You're a scientist, right? Don't you know anything about them?", "/[waitkey(64)]/[close()]"};
    String[] msg0F3B03B3 = new String[]{"/[label()]", "This really ticks me off. We have to escort an investigation we know nothing about and take on enemies that we know even less about!", "/[waitkey(1)]/[clear()]", "But when things really hit the fan, we're the ones who have to bear the full brunt of it. We should just leave it to the Realians to get their asses kicked by those ghosts!", "/[waitkey(64)]/[close()]"};
    String[] msg0F3B6360 = new String[]{"/[label()]", "The Realians are perfect for fighting Gnosis...living puppets versus ghosts.", "/[waitkey(64)]/[close()]"};
    String[] msg0EE5EF88 = new String[]{"/[label()]", "This is my first tour! And I got assigned an A.G.W.S. already. I'm the first in my class to get assigned. I'm so proud!", "/[waitkey(64)]/[close()]"};
    String[] msg0EE64F34 = new String[]{"/[label()]", "But to be honest, I'm really nervous...", "/[waitkey(64)]/[close()]"};
    String[] msg0FC04371 = new String[]{"/[label()]", "Oh, Chief Uzuki, you want to get on an A.G.W.S. again? You're a hard worker. Study it all you like, though it's just a simulation.", "/[waitkey(64)]/[close()]"};
    String[] msg0FC04373 = new String[]{"/[label()]", "Well? Did you get anything out of it? Would like to try it again?", "/[waitkey(64)]/[close()]"};
    String[] msg0FC04375 = new String[]{"/[label()]", "All right. I'll boot it up again. Please hang on for a minute.", "/[waitkey(64)]/[close()]"};
    String[] msg0FC04377 = new String[]{"/[label()]", "You're welcome. I'm happy to be of service.", "/[waitkey(64)]/[close()]"};
    String[] msg0FC0A3241 = new String[]{"/[label()]", "Oh, Chief Uzuki, here to use the A.G.W.S. simulator?", "\n", "I'm sorry, but it's still being tuned.", "/[waitkey(64)]/[close()]"};
    String[] msg0FC0A324 = new String[]{"/[label()]", "Oh, Chief Uzuki, here to use the A.G.W.S. simulator?", "/[waitkey(64)]/[close()]"};
    String[] msg0FC0A326 = new String[]{"/[label()]", "All right. I'll boot up the simulator.", "/[waitkey(1)]/[clear()]", "I'm sure you already know this, but this is an A.G.W.S. battle simulator, so please be sure to use an A.G.W.S. during combat.", "/[waitkey(1)]/[clear()]", "Once in a while, we get people who fight bare-handed, but that defeats the purpose of the simulator. Anyway, good luck.", "/[waitkey(64)]/[close()]"};
    String[] msg0FC0A328 = new String[]{"/[label()]", "Please feel free to contact me should there be anything I can help you with.", "/[waitkey(64)]/[close()]"};
    String[] msg0E397D98 = new String[]{"/[label()]", "There's something to be said about women and machines. Give them a little daily attention, and they'll respond in the right way.", "/[waitkey(64)]/[close()]"};
    String[] msg0E39DD44 = new String[]{"/[label()]", "But they'll still let you down occasionally, anyway. Oops, I didn't mean that.", "/[waitkey(64)]/[close()]"};
    String[] msg0EE9873D = new String[]{"/[label()]", "You're from Vector, right? Don't have anything for you today.", "/[waitkey(1)]/[clear()]", "Or are you sending something?", "/[waitkey(64)]/[close()]"};
    String[] msg0EE9873E = new String[]{"/[label()]", "You're from Vector, right? There's a package for you. It's addressed to Miss Uzuki.", "/[waitkey(1)]/[clear()]", "It looks dangerous, so be careful while handling it.", "/[waitkey(64)]/[clear()]"};
    String[] msgMIYUKI = new String[]{"/[label(Shion)]", "Hmmm, a M.W.S.", "/[waitkey(1)]/[clear()]", "So this is what Miyuki's email was about. Hmph, I guess she still intends to use me as her guinea pig.", "/[waitkey(64)]/[close()]"};
    String[] msgMIYUKI2 = new String[]{"/[label()]", "What? There's nothing else here for you.", "/[waitkey(64)]/[close()]"};
    String[] msg0EE9E6E9 = new String[]{"/[label()]", "If ya wanna send somethin', lemme know. There's no charge for usin' our transporter .", "/[waitkey(64)]/[close()]"};
    String[] msg0FB68A9A = new String[]{"/[label()]", "Oh man, we're screwed. We don't know anything about them Gnosis, or how effective our ammo's going to be against them.", "/[waitkey(64)]/[close()]"};
    String[] msg0FB6EA47 = new String[]{"/[label()]", "You can't just depend on hollow points, since we don't know whether they'll be soft targets. I hear there might be close combat inside the ship, so you can't use APs. How much tracer am I supposed to mix?", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"/[label()]", "The 2nd lines are not ready yet.", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "You have no idea just how terrifying they really are.", "/[waitkey(1)]/[clear()]", "Only after fighting them will you realize just how futile it is.", "/[waitkey(64)]/[close()]"};
    String[] msg00000002 = new String[]{"/[label()]", "A.G.W.S. and Realians are completely ineffective against them, since they're not subject to our laws of reality.", "/[waitkey(64)]/[close()]"};
    String[] msg00000003 = new String[]{"/[label()]", "All we can do is pray.", "/[waitkey(1)]/[clear()]", "You ought to pray too, that we don't get attacked.", "/[waitkey(64)]/[close()]"};
    String[] msgAGWS = new String[]{"/[label()]", "Oh, this? It's an A.G.W.S. capsule. You can carry an A.G.W.S. inside, and deploy it as necessary.", "/[waitkey(1)]/[clear()]", "It uses the same space compression technology used in transportation, to create a totally self-contained, portable A.G.W.S. hangar.", "/[waitkey(64)]/[close()]"};
    String[] msgAGWS2 = new String[]{"/[label()]", "Oh, don't forget, you can only recover FHP (Frame HP) for the A.G.W.S. at special places like specialty shops or large hangars.", "/[waitkey(1)]/[clear()]", "So you better pay attention, or your A.G.W.S. will quickly become useless. The key is to selectively use it at the right times.", "/[waitkey(1)]/[clear()]", "Oh, and one more thing. You can't recover FHP on this ship.", "/[waitkey(64)]/[close()]"};
    String[] msgSHOP1 = new String[]{"/[label()]", "Well, if it isn't the Vector Chief. What can I help you with today? I'll sell you whatever you need, real cheap!", "/[waitkey(64)]/[close()]"};
    String[] msgSHOP2 = new String[]{"/[label()]", "Thanks for your business!", "/[waitkey(1)]/[clear()]", "Oh, I almost forgot, I have some interesting information. You know that \"Vaporizer Plug-in\" being used experimentally in the simulator?", "/[waitkey(64)]/[close()]"};
    String[] msgSHOP5 = new String[]{"/[label()]", "Yes, that's the one. It destroys obstacles via Connection Gear. They're announcing a working model soon, so Vector is sending us prototypes.", "/[waitkey(1)]/[clear()]", "If you're interested, come back later.", "/[waitkey(64)]/[close()]"};
    String[] msgSHOP3 = new String[]{"/[label()]", "What, the inventory not to your satisfaction?", "/[waitkey(1)]/[clear()]", "Well, I've got some interesting information then. You know that \"Vaporizer Plug-in\" they're experimenting with in the simulator?", "/[waitkey(64)]/[close()]"};
    String[] msgSHOP4 = new String[]{"/[label(Shion)]", "Yes, my company developed it.", "/[waitkey(64)]/[close()]"};

    ST0130() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            default:
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0ECA1A9E, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0ECA7A4B, 0);
        ST0130.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0130.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg0F3B03B3, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0F3B6360, 0);
        ST0130.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0130.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0EE5EF88, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0EE64F34, 0);
        ST0130.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0130.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc) {
        this.Talk_npc4_1();
    }

    void Talk_npc4_1() {
        if (Runtime.getFlags(7029, 1) == 1) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg0FC04371, 0);
            ST0130.waitPage(this.win, 64);
        } else {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg0FC0A324, 0);
            ST0130.waitPage(this.win, 64);
        }
        this.menu = Menu.create();
        this.menu.addItem("I'd like to try it\nNot right now");
        this.menu.setCursor(1);
        System.waitFor(this.menu);
        this.selected0 = this.menu.getSelected();
        switch (this.selected0) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0FC0A326, 0);
                ST0130.waitPage(this.win, 64);
                Runtime.evsSetRetPoint();
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(9006, 0);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0FC0A328, 0);
        ST0130.waitPage(this.win, 64);
    }

    void Talk_npc4_2() {
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg0E397D98, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0E39DD44, 0);
        ST0130.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0130.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.S015B = Runtime.getFlags(23, 1);
        this.GET_MWS = Runtime.getFlags(7012, 1);
        if (this.S015B == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg0EE9873D, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0EE9E6E9, 0);
        ST0130.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        if (this.GET_MWS == 0) {
            Runtime.setFlags(7009, 1, 1);
            Runtime.setFlags(7012, 1, 1);
            Runtime.setFlags(7055, 1, 1);
            window.print(this.msg0EE9873E, 0);
            ST0130.waitPage(window, 64);
            window.print(this.msgMIYUKI, 0);
            ST0130.waitPage(window, 64);
            System.sleep(15);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 10);
            System.sleep(40);
        } else {
            window.print(this.msgMIYUKI2, 0);
            ST0130.waitPage(window, 64);
        }
    }

    public void Talk_npc7(Enepc enepc) {
        this.Talk_npc7_1();
    }

    void Talk_npc7_1() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgSHOP1, 0);
        ST0130.waitPage(this.win, 64);
        this.menu = Menu.create();
        this.menu.addItem("Could you show me?\nI'm not really interested");
        System.waitFor(this.menu);
        this.selected0 = this.menu.getSelected();
        switch (this.selected0) {
            case 0: {
                System.sleep(15);
                Runtime.enterShop(1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSHOP2, 0);
                ST0130.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSHOP4, 0);
                ST0130.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSHOP5, 0);
                ST0130.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgSHOP3, 0);
        ST0130.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgSHOP4, 0);
        ST0130.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgSHOP5, 0);
        ST0130.waitPage(this.win, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0130.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00000002, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000003, 0);
        ST0130.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msgAGWS, 0);
                ST0130.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msgAGWS2, 0);
        ST0130.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(100, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -7.0f, 0.0f, 11.0f, 0.0f);
        this.teiten1.SetBgm(196636);
        this.teiten2 = new Uwamono(28690, -4.0f, 0.0f, 11.0f, 0.0f);
        this.teiten2.SetBgm(196636);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -12.0f, 0.0f);
        this.teiten3.SetBgm(196637);
        this.teiten3.SetBgmType('\u0001');
        this.light.setColor(0, 0.33f, 0.33f, 0.33f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, -0.007f, 1.0f, 0.029f);
        this.light.setColor(2, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(2, 0.024f, 0.861f, 0.509f);
        this.light.setColor(3, 0.08f, 0.08f, 0.08f);
        this.light.setDirection2(3, 0.064f, -0.996f, 0.069f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFPedestal(3, -8.66113f, 2.0461f, 17.84994f, 38.0f, -13.894236f, -30.553967f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFPedestal(4, 8.32784f, 2.0867f, 17.476f, 42.239f, -12.361f, 28.999f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(4, 1);
        this.EF02 = new Effect(1013, 0);
        this.EF02.disp(true);
        this.EF02.setLocation(6, 1);
        this.EF06 = new Effect(1013, 0);
        this.EF06.disp(true);
        this.EF06.setLocation(6, 5);
        Stage.setVisible(20, false);
        Stage.setVisible(6, false);
        Stage.setVisible(10, false);
        Stage.setVisible(11, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.doorA = new Uwamono(151, 42, '\u0001');
        new Uwamono(150, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        new Uwamono(19, 33);
        new Uwamono(61, 33);
        new Uwamono(64, 31);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S014A = Runtime.getFlags(21, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S015B = Runtime.getFlags(23, 1);
        this.SHION_ROOM = Runtime.getFlags(7009, 1);
        this.GET_MWS = Runtime.getFlags(7012, 1);
        if (this.S015B == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
        this.kidou = new Mapunits();
        this.kidou.mapUnit(28);
        if (Runtime.getFlags(1021, 1) == 1) {
            this.kidou.start(4, null);
            this.kidou.start(1, "Victory_Msg");
        } else if (Runtime.getFlags(1022, 1) == 1) {
            this.kidou.start(4, null);
            this.kidou.start(1, "Lose_Msg");
        } else {
            this.kidou.start(4, null);
        }
    }

    void npcset_1() {
        this.npc2 = new NPC_NORMAL2(525, 2, 0, 79, 10, 6.29f, 0.0f, 11.75f, 0.0f);
        this.npc3 = new NPC_NORMAL(525, 3, 0, 5, 6, 2.61f, 0.0f, 3.52f, 50.0f);
        this.npc4 = new NPC_NORMAL(527, 4, 0, 14, 5, 2.61f, -1.12f, -5.93f, 110.0f);
        this.npc5 = new NPC_NORMAL(527, 5, 0, 14, 5, -5.0f, 0.0f, 1.02f, 180.0f);
        this.npc6 = new NPC_NORMAL2(520, 6, 0, 79, 10, -6.59f, -0.05f, 15.34f, 90.0f);
        this.npc7 = new NPC_NORMAL2(527, 7, 0, 79, 5, 7.45f, 0.0f, 12.28f, -45.0f);
        this.npc9 = new NPC_NORMAL2(527, 9, 0, 79, 5, -6.29f, 0.0f, 12.07f, 180.0f);
        this.npc2.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 5);
        this.npc2.setTogetherWith(-2, -1, -1, -1);
        this.npc4.setMotion(0, 10);
        this.npc4.disableDTKFlag(2);
        this.npc4.enableDTKFlag(4);
        this.npc5.setMotion(0, 10);
        this.npc5.disableDTKFlag(2);
        this.npc5.enableDTKFlag(4);
        this.npc6.setMotion(0, 6);
        this.npc6.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.disableDTKFlag(8);
        this.npc7.setMotion(0, 10);
        this.npc9.setMotion(0, 9);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setInvalidID(1);
        this.npc9.disableDTKFlag(8);
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc8");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc9.talkto("Talk_npc9");
        this.npc11 = new NPC_NORMAL2(8449, 11, 0, 17, 8, 8.07f, 0.0f, -6.0f, 270.0f);
        this.npc11.setMotion(0, 2);
        this.npc11.setInvalidID(1);
        this.npc11.setTranslate(5.0f, -0.9f, -6.0f);
        this.npc11.dispRadar(false);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(65536);
        this.npc11.setLightMode(1);
        this.npc11.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.npc11.light.setColor(1, 0.3f, 0.3f, 0.59f);
        this.npc11.light.setDirection2(1, 0.934f, 0.0f, 0.356f);
        this.npc11.light.setColor(2, 0.05f, 0.05f, 0.05f);
        this.npc11.light.setDirection2(2, -0.998f, 0.0f, 0.056f);
        this.npc11.light.setColor(3, 0.8f, 0.8f, 0.8f);
        this.npc11.light.setDirection2(3, 1.0f, -0.0f, 0.013f);
        this.npc12 = new NPC_NORMAL2(8450, 12, 0, 17, 8, -7.07f, 0.0f, 0.0f, 90.0f);
        this.npc12.setMotion(0, 2);
        this.npc12.setInvalidID(1);
        this.npc12.setTranslate(-5.0f, -0.9f, 0.0f);
        this.npc12.dispRadar(false);
        this.npc12.disableDTKFlag(131072);
        this.npc12.disableDTKFlag(65536);
        this.npc12.setLightMode(1);
        this.npc12.light.setColor(0, 0.16f, 0.16f, 0.16f);
        this.npc12.light.setColor(1, 0.3f, 0.3f, 0.59f);
        this.npc12.light.setDirection2(1, -0.974f, 0.0f, 0.227f);
        this.npc12.light.setColor(2, 0.05f, 0.05f, 0.05f);
        this.npc12.light.setDirection2(2, 0.94f, 0.341f, 0.009f);
        this.npc12.light.setColor(3, 0.8f, 0.8f, 0.8f);
        this.npc12.light.setDirection2(3, -1.0f, -0.0f, 0.019f);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Lose_Msg() {
            System.println("*********シュミレーターで負けてきました**************");
            Runtime.setFlags(1021, 1, 0);
            Runtime.setFlags(1022, 1, 0);
            Runtime.setPlayerControl(false);
            Runtime.setPlayerControl(true);
        }

        void Victory_Msg() {
            System.println("*********シュミレーターで勝ってきました**************");
            Runtime.setFlags(1021, 1, 0);
            Runtime.setFlags(1022, 1, 0);
            Runtime.setPlayerControl(false);
            Runtime.setPlayerControl(true);
        }
    }
}

