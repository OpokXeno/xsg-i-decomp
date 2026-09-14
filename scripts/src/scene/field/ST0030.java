import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0030
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK03_PRJ {
    Player player;
    Camera cam1;
    Camera camEV;
    Menu menu;
    Window win;
    Unit ring_g01;
    Unit ring_g02;
    Unit ring_g03;
    Unit ring_g10;
    Unit ring_g11;
    Unit ring_g12;
    Unit ring_g20;
    Unit ring_g21;
    Unit ring_g22;
    Enepc zohal;
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
    int touchFlag3;
    int touchFlag4;
    int touchFlag5;
    int touchFlag6;
    int touchFlag7;
    int touchFlag8;
    Uwamono doorA;
    Uwamono doorB;
    int selected = 0;
    int BUTTON_F = 0;
    int S07A1;
    int S07A2;
    int S07A3;
    int S07C;
    int S08;
    int S010;
    int S012;
    int S014A;
    int S014B;
    int S015B;
    Chr Crane;
    Chr Crane2;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    Uwamono teiten11;
    int page;
    String[] msg0001 = new String[]{"/[label()]", "Who the hell are you? What is a civilian doing wandering around in here?", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label()]", "Humph...Vector R&D, huh? Then do your job and don't meddle in other people's business!", "/[waitkey(64)]/[close()]"};
    String[] msg0003 = new String[]{"/[label()]", "If you have no business here, hurry up and leave.", "/[waitkey(64)]/[close()]"};
    String[] msg0004 = new String[]{"/[label()]", "Um, excuse me...is the Lieutenant Commander still here? Is he still barking orders?", "/[waitkey(64)]/[close()]"};
    String[] msg0005 = new String[]{"/[label()]", "I just happened to be away from my post. This is not good...", "/[waitkey(1)]/[clear()]", "I don't want to know what he's gonna say to me if he finds out I wasn't here while he was giving orders...", "/[waitkey(64)]/[close()]"};
    String[] msg0006 = new String[]{"/[label()]", "Damn. This is frustrating. The Woglinde is OUR ship. ", "Man, ever since we picked up that overgrown plywood, the guys in the Investigations Unit are suddenly acting\nlike big shots...", "/[waitkey(1)]/[clear()]", "I heard they're asking the Captain to make this area accessible by permission only. Equipment inspection is a pain already...", "/[waitkey(64)]/[close()]"};
    String[] msg0007 = new String[]{"/[label()]", "There's just something wrong with this situation. We were originally dispatched to investigate the planetary disappearance. ", "But it seems as though those Investigations Unit guys expected this flotsam to be here.", "/[waitkey(1)]/[clear()]", "I have no idea what's going on.", "/[waitkey(64)]/[close()]"};
    String[] msg0008 = new String[]{"/[label()]", "I've been a Marine for a long time, but I don't recognize any of them. I wonder which division they belong to?", "/[waitkey(64)]/[close()]"};
    String[] msg0009 = new String[]{"/[label()]", "Those Investigations Unit guys live in separate quarters and even eat separately. They're really a mysterious lot.", "/[waitkey(64)]/[close()]"};
    String[] msg0010 = new String[]{"/[label()]", "They seem to be friendly with the First Officer, Commander Cherenkov, though. Maybe they get along because they're both so overbearing.", "/[waitkey(64)]/[close()]"};
    String[] msg0011 = new String[]{"/[label()]", "Oh, you're one of Vector's engineers, right? How are you?!", "/[waitkey(1)]/[clear()]", "This is actually my first mission, so I've never even seen the enemy yet. I've heard that the enemy is an unknown life-form called \"Gnosis.\"", "/[waitkey(64)]/[close()]"};
    String[] msg0012 = new String[]{"/[label()]", "I wonder what the enemy hopes to accomplish by attacking us?", "/[waitkey(64)]/[close()]"};
    String[] msg0013 = new String[]{"/[label()]", "Oh, these things here are escape pods. We maintain these carefully so that we can rely on them at any time!", "/[waitkey(64)]/[close()]"};
    String[] msg0014 = new String[]{"/[label()]", "Well, it's odd for someone in maintenance to say this, but considering that these pods will only be used if something terrible happens to the Woglinde,", "/[waitkey(1)]/[clear()]", "I'd be happiest if they never got used at all.", "/[waitkey(64)]/[close()]"};
    String[] msg0101 = new String[]{"/[label(Lieutenant Commander)]", "Hey! You there, missy!!", "/[waitkey(64)]/[close()]"};
    String[] msg0102 = new String[]{"/[label(Lieutenant Commander)]", "Don't you get it when I tell you to stay away from there?!", "/[waitkey(1)]/[clear()]", "Don't you ever ignore my orders again! I have no sense of mercy, not even for women or children!!", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"There are no lines for the 2nd time yet.", "/[waitkey(64)]/[close()]"};
    String[] msgdame = new String[]{"/[label(Shion)]", "If I go this way, I'll wind up in the KOS-MOS lab again.", "/[waitkey(1)]/[clear()]", "I better hurry to the bridge.", "/[waitkey(64)]/[close()]"};
    String[] msgmail1 = new String[]{"/[label()]", "Miss, I've got some information I'd like to share with you. Well? Are you interested in hearing about it? No, no. It won't be much trouble for you at all.", "/[waitkey(1)]/[clear()]", "How about it?", "/[waitkey(64)]/[close()]"};
    String[] msgmail2 = new String[]{"/[label()]", "Ah, wonderful! Well then, first, could you please give me your email address?", "/[waitkey(1)]/[clear()]", "No, no, you never know who might be listening in, right? The safest way to share a secret is through email.", "/[waitkey(64)]/[close()]"};
    String[] msgmail3 = new String[]{"/[label()]", "Don't worry, you won't regret it. Well now, it's settled then.", "/[waitkey(1)]/[clear()]", "Let's see...everything appears to be in order!", "/[waitkey(1)]/[clear()]", "I'll send you an email in a little while. Don't worry, it won't take too long. Please be on the lookout for it.", "/[waitkey(64)]/[close()]"};
    String[] msgmail4 = new String[]{"/[label()]", "That's too bad. Well, I'll be here for a while, so if you change your mind, let me know.", "/[waitkey(64)]/[close()]"};
    String[] msgmail5 = new String[]{"/[label()]", "Ah, so you changed your mind? No, no. I understand. Curiosity got to you, right?", "/[waitkey(64)]/[close()]"};
    String[] msgmail6 = new String[]{"/[label()]", "Please be on a lookout for the email. Oh, and one more thing -- please don't tell anyone about this.", "/[waitkey(64)]/[close()]"};

    ST0030() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.322f, -7.219f, -4.968f);
        this.camEV.setRotate(25.284f, 491.325f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 31: {
                this.zohal.kickEnepc(4, 2);
                this.zohal.kickEnepc(1, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n == 1) {
            switch (n2) {
                case 3: {
                    this.npc1.enableDTKFlag(1);
                    this.npc1.disableDTKFlag(131072);
                    this.npc1.kickEnepc(7, 56);
                    break;
                }
                case 4: {
                    this.npc1.enableDTKFlag(1);
                    this.npc1.disableDTKFlag(131072);
                    this.npc1.kickEnepc(7, 56);
                    break;
                }
            }
        }
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (this.S08 != 0) break;
                this.fadeIn.call(0);
                System.sleep(10);
                Runtime.setPlayerControl(false);
                System.sleep(13);
                Runtime.setFlags(15, 1, 1);
                Runtime.setPlayerControl(true);
                Runtime.jumpEvent(1080);
            }
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0101, 0);
                ST0030.waitPage(this.win, 64);
                System.sleep(10);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.npc7.kickEnepc(0, 11);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0102, 0);
                ST0030.waitPage(this.win, 64);
                System.sleep(20);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 2: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(18, 1) == 1) {
                    this.BUTTON_F = 0;
                    break;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgdame, 0);
                ST0030.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc13(Enepc enepc) {
        if (Runtime.getFlags(7057, 2) == 0) {
            this.Talk_npc13_1();
        } else if (Runtime.getFlags(7057, 2) == 1) {
            this.Talk_npc13_2();
        } else {
            this.Talk_npc13_3();
        }
    }

    void Talk_npc13_1() {
        Runtime.setFlags(7057, 2, 1);
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail1, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Uh, only if it's quick\nI'm sorry but I'm in a hurry");
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
                this.menu.addItem("Sure, that's fine\nUm, no thanks");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(7057, 2, 2);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgmail3, 0);
                        ST0030.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail4, 0);
                ST0030.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmail4, 0);
        ST0030.waitPage(this.win, 64);
    }

    void Talk_npc13_2() {
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail5, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, curiosity got to me\nI'll pass");
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
                this.menu.addItem("Sure, that's fine\nUm, no thanks");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(7057, 2, 2);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgmail3, 0);
                        ST0030.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmail4, 0);
                ST0030.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmail4, 0);
        ST0030.waitPage(this.win, 64);
    }

    void Talk_npc13_3() {
        this.win = Window.create();
        this.win.setLocation(15, 305);
        this.win.setSize(4, 45);
        this.win.print(this.msgmail6, 0);
        ST0030.waitPage(this.win, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0002, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0003, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg0004, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0005, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0006, 0);
                ST0030.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0007, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0008, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg0009, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0010, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc4_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg0013, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0014, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg0011, 0);
                ST0030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0012, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    void Talk_npc6_3(Window window) {
        window.print(this.msgNO2, 0);
        ST0030.waitPage(window, 64);
    }

    public void Touch_npc1(Enepc enepc, Window window) {
        this.Touch_npc1_1(window);
    }

    void Touch_npc1_1(Window window) {
        this.npc1.kickEnepc(4, 1);
        this.npc1.kickEnepc(7, 56);
        this.npc1.disableDTKFlag(131072);
        this.npc1.kickEnepc(0, 11);
        window.print(this.msg0001, 0);
        ST0030.waitPage(window, 64);
        this.npc1.kickEnepc(4, 0);
        this.npc1.kickEnepc(7, 56);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(20, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(40, 4);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 11.5f, -0.5f, 27.0f, 0.0f);
        this.teiten1.SetBgm(196613);
        this.teiten2 = new Uwamono(28690, 6.5f, -0.5f, 27.0f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.teiten3 = new Uwamono(28690, 1.5f, -0.5f, 27.0f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, -3.5f, -0.5f, 27.0f, 0.0f);
        this.teiten4.SetBgm(196613);
        this.teiten5 = new Uwamono(28690, -8.5f, -0.5f, 27.0f, 0.0f);
        this.teiten5.SetBgm(196613);
        this.teiten6 = new Uwamono(28690, -13.5f, -0.5f, 27.0f, 0.0f);
        this.teiten6.SetBgm(196613);
        this.teiten7 = new Uwamono(28690, -7.0f, 3.0f, -5.0f, 0.0f);
        this.teiten7.SetBgm(196614);
        this.teiten8 = new Uwamono(28690, 7.0f, 3.0f, -5.0f, 0.0f);
        this.teiten8.SetBgm(196614);
        this.teiten9 = new Uwamono(28690, 0.0f, 3.0f, 6.0f, 0.0f);
        this.teiten9.SetBgm(196614);
        this.teiten10 = new Uwamono(28690, 0.0f, 3.0f, -1.0f, 0.0f);
        this.teiten10.SetBgm(196615);
        this.teiten11 = new Uwamono(28690, 0.0f, 0.0f, 12.0f, 0.0f);
        this.teiten11.SetBgm(196616);
        this.teiten11.SetBgmType('\u0001');
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 0.14f, 0.14f, 0.14f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.53f, 0.53f, 0.53f);
        Runtime.setIdLightCol(1, 3, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -0.5f);
        Stage.setVisible(-1, true);
        this.ring_g01 = new Mapunits();
        this.ring_g01.mapUnit(99);
        this.ring_g01.start(4, null);
        this.ring_g01.setTranslate(0.0f, 0.0f, -1.78f);
        this.ring_g01.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g01.start(1, "Lkaiten");
        this.ring_g02 = new Mapunits();
        this.ring_g02.mapUnit(100);
        this.ring_g02.start(4, null);
        this.ring_g02.setTranslate(0.0f, 0.0f, -1.78f);
        this.ring_g02.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g02.start(1, "Lkaiten");
        this.ring_g03 = new Mapunits();
        this.ring_g03.mapUnit(101);
        this.ring_g03.start(4, null);
        this.ring_g03.setTranslate(0.0f, 0.0f, -1.78f);
        this.ring_g03.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g03.start(1, "Lkaiten");
        this.ring_g10 = new Mapunits();
        this.ring_g10.mapUnit(102);
        this.ring_g10.start(4, null);
        this.ring_g10.setTranslate(0.0f, 5.2f, -1.78f);
        this.ring_g10.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g10.start(1, "RkaitenU");
        this.ring_g11 = new Mapunits();
        this.ring_g11.mapUnit(103);
        this.ring_g11.start(4, null);
        this.ring_g11.setTranslate(0.0f, 5.2f, -1.78f);
        this.ring_g11.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g11.start(1, "RkaitenU");
        this.ring_g12 = new Mapunits();
        this.ring_g12.mapUnit(104);
        this.ring_g12.start(4, null);
        this.ring_g12.setTranslate(0.0f, 5.2f, -1.78f);
        this.ring_g12.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g12.start(1, "RkaitenU");
        this.ring_g20 = new Mapunits();
        this.ring_g20.mapUnit(105);
        this.ring_g20.start(4, null);
        this.ring_g20.setTranslate(0.0f, -5.2f, -1.78f);
        this.ring_g20.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g20.start(1, "RkaitenD");
        this.ring_g21 = new Mapunits();
        this.ring_g21.mapUnit(106);
        this.ring_g21.start(4, null);
        this.ring_g21.setTranslate(0.0f, -5.2f, -1.78f);
        this.ring_g21.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g21.start(1, "RkaitenD");
        this.ring_g22 = new Mapunits();
        this.ring_g22.mapUnit(107);
        this.ring_g22.start(4, null);
        this.ring_g22.setTranslate(0.0f, -5.2f, -1.78f);
        this.ring_g22.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_g22.start(1, "RkaitenD");
        this.Crane = new UNIT_NORMAL(20580, 11.85f, -8.0f, -9.38f, 320.0f);
        this.Crane2 = new UNIT_NORMAL(20531, -11.84f, -8.0f, -8.3f, 270.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(10, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(11, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(12, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(13, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(14, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(15, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(16, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, -20.714f, 6.57399f, 19.951f, 56.4f, -63.3024f, 355.7017f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFPedestal(3, 23.53f, 10.861f, 21.426f, 53.91f, -68.0748f, 364.1f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, 0.0f);
        Stage.setVisible(8, false);
        Stage.setVisible(95, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.zohal = new NPC_NORMAL(20541, 31, 0, 79, 14, 0.0f, 3.0f, -1.5f, 0.0f);
        this.zohal.dispRadar(false);
        this.zohal.setInvalidID(1);
        this.doorA = new Uwamono(98, 40, '\u0001');
        new Uwamono(97, 40, '\u0001', this.doorA);
        this.doorB = new Uwamono(9, 42, '\u0001');
        new Uwamono(10, 42, '\u0001', this.doorB);
        if (Runtime.getFlags(18, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
        } else {
            this.doorA.SetDoorType('\u0002');
        }
        this.doorB.SetDoorType('\u0004');
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
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S015B = Runtime.getFlags(23, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S08 = Runtime.getFlags(15, 1);
        this.npcset_1();
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(528, 1, 0, 62, 8, -13.23f, 0.0f, 15.22f, 0.0f);
        this.npc2 = new NPC_NORMAL(527, 2, 0, 14, 8, 24.92f, 0.0f, 20.03f, -20.0f);
        this.npc3 = new NPC_NORMAL(522, 3, 0, 14, 11, -5.2f, -1.35f, 24.84f, 110.0f);
        this.npc4 = new NPC_NORMAL(527, 4, 0, 14, 8, -4.617f, 0.0f, 24.13f, -50.0f);
        this.npc5 = new NPC_NORMAL(527, 5, 0, 76, 5, 3.15f, 0.0f, 28.95f, -80.0f);
        this.npc6 = new NPC_NORMAL(522, 6, 0, 2, 8, 6.23f, 0.0f, 19.15f, 80.0f);
        this.npc7 = new NPC_NORMAL2(309, 7, 11, 17, 7, 11.93f, -7.0f, -3.17f, 270.0f);
        this.npc10 = new NPC_NORMAL2(527, 10, 11, 3, 11, -9.03f, -7.0f, 2.47f, 0.0f);
        this.npc11 = new NPC_NORMAL2(527, 11, 11, 3, 11, -12.23f, -7.0f, 0.37f, 0.0f);
        this.npc12 = new NPC_NORMAL2(527, 12, 12, 3, 11, 7.63f, -7.0f, -1.3f, 20.0f);
        this.npc14 = new NPC_NORMAL2(527, 14, 12, 3, 11, 14.51f, -7.0f, -3.71f, 50.0f);
        this.npc1.enableDTKFlag(393216);
        this.npc1.disableDTKFlag(8);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 16);
        this.npc4.disableDTKFlag(2);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 10);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc7.setMotion(0, 10);
        this.npc7.dispRadar(false);
        this.npc10.dispRadar(false);
        this.npc11.dispRadar(false);
        this.npc12.dispRadar(false);
        this.npc14.dispRadar(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc1.touchto("Touch_npc1");
        if (Runtime.mailArriveCheck(5) == 1) {
            return;
        }
        if (Runtime.mailArriveCheck(10) == 1) {
            return;
        }
        if (Runtime.mailArriveCheck(12) == 1) {
            return;
        }
        this.npc13 = new NPC_NORMAL2(527, 13, 12, 14, 11, 16.11f, 0.0f, 21.14f, 250.0f);
        this.npc13.talkto("Talk_npc13");
    }

    void npcset_2() {
    }

    void npcset_3() {
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    class UNIT_NORMAL
            extends Chr {
        UNIT_NORMAL(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
            this.setShadow(0, 16);
            this.setLightMode(1);
            this.dispRadar(false);
            this.light.setColor(0, 0.0f, 0.0f, 0.0f);
            this.light.setColor(1, 0.24f, 0.24f, 0.24f);
            this.light.setDirection2(1, -0.831f, 0.556f, -0.019f);
            this.light.setColor(2, 0.56f, 0.56f, 0.56f);
            this.light.setDirection2(2, 0.502f, 0.001f, 0.865f);
            this.light.setColor(3, 0.4f, 0.4f, 0.4f);
            this.light.setDirection2(3, 0.355f, -0.577f, 0.736f);
        }

        void init() {
        }
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
            extends Unit {
        Mapunits() {
        }

        void Lkaiten() {
            float f = 0.5f;
            this.setTranslate(0.0f, 0.0f, -1.78f);
            while (true) {
                this.setRotateY(f);
                f -= 0.12f;
                System.sleep(1);
            }
        }

        void RkaitenD() {
            float f = 0.5f;
            this.setTranslate(0.0f, -5.2f, -1.78f);
            while (true) {
                this.setRotateY(f);
                f += 0.12f;
                System.sleep(1);
            }
        }

        void RkaitenU() {
            float f = 0.5f;
            this.setTranslate(0.0f, 5.2f, -1.78f);
            while (true) {
                this.setRotateY(f);
                f += 0.12f;
                System.sleep(1);
            }
        }
    }
}

