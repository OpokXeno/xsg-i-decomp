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
import xeno.map.MC_KUK10B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2241
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK10B_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono kow;
    Uwamono kow2;
    Uwamono kow3;
    Uwamono kow4;
    Unit tana;
    Uwamono col;
    Effect Obj600;
    Effect Obj601;
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
    Enepc npc20;
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
    int talkFlag20;
    int king = 0;
    int car = 0;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Enepc debug;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light10;
    Effect light09;
    Uwamono Atari;
    Uwamono Atari2;
    Effect button;
    Effect hokori;
    Uwamono teiten1;
    Uwamono item01;
    Uwamono item02;
    int TANA = Runtime.getFlags(6047, 1);
    int page;
    String[] Q = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Damn those Gnosis bastards, they destroy everything in their paths! We'll never be able to repair things at this rate.", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label()]", "Sheesh, they destroy everything in their paths! Couldn't they be a bit more considerate of the people doing the repairs?", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Fran#27ois! Shake!", "/[waitkey(1)]/[clear()]", "Sit!", "/[waitkey(1)]/[clear()]", "This cat is uncool. She doesn't do any tricks!", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Dance! Dance! Forget all the bad things by dancing! Ha ha ha!", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "Dance, dance, dance!", "/[waitkey(1)]/[clear()]", "So does this make you want to dance too? Once you've seen it, it's a dance you'll never forget! Hah! Hah! Hah!", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "Ha ha ha ha ha, dance, dance! What a weird dude! But this is fun!", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label(King)]", "Hey, kid! Don't pick on little Fran#27ois!", "/[waitkey(64)]/[clear()]"};
    String[] msg80002 = new String[]{"/[label()]", "Ha ha ha, King is mad!", "/[waitkey(64)]/[close()]"};
    String[] msg80003 = new String[]{"/[label(King)]", "Oh, nooo, that's not how I meant it. I'm not mad.", "/[waitkey(1)]/[clear()]", "I just felt sorry for my darling little kitty. Stay out of trouble now, you hear?", "/[waitkey(64)]/[clear()]"};
    String[] msg80004 = new String[]{"/[label()]", "Aaaaah! King's acting like a woman! He's scary!", "/[waitkey(64)]/[close()]"};
    String[] msg80005 = new String[]{"/[label()]", "You know, sandbags aren't free. I'm gonna bill you. I'm serious!", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Hiss!", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME1 = new String[]{"/[label(Pink Bug)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME2 = new String[]{"/[label(Pink Bug)]", "I wanna see Tom.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME3 = new String[]{"/[label(Pink Bug)]", "I wonder if Tom's doing okay. I wanna go back to the Dock Colony.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME4 = new String[]{"/[label(Pink Bug)]", "You're not going to destroy me? You're...talking to me? You wouldn't happen to be Tom's friend, would you?!", "/[waitkey(1)]/[clear()]", "A friend of Tom's is a friend of mine! How is Tom?", "/[waitkey(1)]/[clear()]", "I see. I'm glad Tom's doing okay! It was really bothering me that I didn't get to say goodbye to Tom.", "/[waitkey(1)]/[clear()]", "I know! As a token of our friendship, I'll tell you something interesting!\n", "/[waitkey(1)]/[clear()]", "What kind of story would you like to hear?", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME6 = new String[]{"/[label(Pink Bug)]", "In the Durandal's observation room, there's a Segment Key on the ground shaded by a potted tree. You'll need to look very carefully for it.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME7 = new String[]{"/[label(Pink Bug)]", "The bartender robot on the Elsa has the Passport you need to play the Card Game.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME8 = new String[]{"/[label(Pink Bug)]", "There's something that Shion and Miyuki are working on together, right? It's a weapon for KOS-MOS, right? But it seems they won't be able to complete that weapon. It's a real shame...", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME9 = new String[]{"/[label(Pink Bug)]", "Well, to commemorate us becoming friends, I'll take a picture. Are you ready?", "/[waitkey(64)]/[clear()]"};
    String[] msgTALK_ME10 = new String[]{"/[label(Pink Bug)]", "Okay, say cheese!", "/[waitkey(64)]/[clear()]"};
    String[] msgTALK_ME11 = new String[]{"/[label(Pink Bug)]", "When the photo's ready, I'll send it to you. Say hi to Tom for me!", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME12 = new String[]{"/[label(Pink Bug)]", "Is there something else you want me to tell you about?", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME13 = new String[]{"/[label(Pink Bug)]", "Is that it? Well then, say hi to Tom for me!", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME5 = new String[]{"/[label(Pink Bug)]", "I wonder how Tom is doing.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME14 = new String[]{"/[label(Pink Bug)]", "I thought you were my friend! You're so mean!", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME15 = new String[]{"/[label(Pink Bug)]", "It seems that specific enemies sometimes have Segment Keys too.", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME16 = new String[]{"/[label(Pink Bug)]", "You can get a lot out of reading your email often.", "/[waitkey(64)]/[close()]"};

    ST2241() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck && this.TANA == 0) {
            System.println("移動棚");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Q, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Press\nDon't press\n");
            System.waitFor(this.menu);
            if (this.menu.getSelected() == 0) {
                System.sleep(30);
                Sound.effectPlay(58);
                this.button.disp(false);
                this.hokori.disp(true);
                Sound.effectPlay(196749);
                float f = 11.504f;
                while (f >= 9.504f) {
                    this.tana.setTranslate(f, 3.6f, -24.727f);
                    System.sleep(1);
                    f -= 0.035f;
                }
                this.tana.setTranslate(9.504f, 3.6f, -24.727f);
                this.col.setTranslate(9.504f, 3.6f, -24.727f);
                System.sleep(10);
                this.hokori.disp(false);
                Runtime.setFlags(6047, 1, 1);
                this.TANA = Runtime.getFlags(6047, 1);
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
        }
    }

    void TALK_ME() {
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Somewhat interesting story\nPretty good story\nGood story\nGreat story\nNot really interested");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME7, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2241.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2241.waitPage(this.win, 64);
                Runtime.setFlags(7128, 1, 1);
                return;
            }
        }
    }

    void TALK_ME2() {
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Somewhat interesting story\nPretty good story\nGood story\nGreat story\n");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME7, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2241.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2241.waitPage(this.win, 64);
                Runtime.setFlags(7128, 1, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME13, 0);
        ST2241.waitPage(this.win, 64);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    public void Talk_npc20(Enepc enepc) {
        this.Talk_npc20_1();
    }

    void Talk_npc20_1() {
        if (Runtime.getFlags(7127, 1) == 1) {
            if (Runtime.getFlags(7137, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME4, 0);
                ST2241.waitPage(this.win, 64);
                this.TALK_ME();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME9, 0);
                ST2241.waitPage(this.win, 64);
                this.win.print(this.msgTALK_ME10, 0);
                ST2241.waitPage(this.win, 64);
                Sound.effectPlay(10);
                this.win.print(this.msgTALK_ME11, 0);
                ST2241.waitPage(this.win, 64);
                Runtime.setFlags(7156, 1, 1);
                Runtime.setFlags(7137, 1, 1);
                return;
            }
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgTALK_ME12, 0);
            ST2241.waitPage(this.win, 64);
            this.TALK_ME2();
            return;
        }
        ++this.talkFlag20;
        switch (this.talkFlag20) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME1, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME2, 0);
                ST2241.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME3, 0);
        ST2241.waitPage(this.win, 64);
    }

    void Talk_npc2_1(Window window) {
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        switch (this.car) {
            case 1: {
                window.print(this.msg40002, 0);
                ST2241.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg40001, 0);
        ST2241.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2241.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                this.npc6.kickEnepc(1, 27);
                window.print(this.msg60001, 0);
                ST2241.waitPage(window, 64);
                return;
            }
        }
        this.npc6.kickEnepc(1, 27);
        window.print(this.msg60002, 0);
        ST2241.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        this.npc7.kickEnepc(1, 27);
        window.print(this.msg70001, 0);
        ST2241.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        switch (this.king) {
            case 2: {
                window.print(this.msg80005, 0);
                ST2241.waitPage(window, 64);
                --this.king;
                --this.king;
                return;
            }
            case 1: {
                this.npc8.look_char(this.npc5);
                this.npc8.kickEnepc(1, 9);
                window.print(this.msg80003, 0);
                ST2241.waitPage(window, 64);
                this.npc5.look_char(this.npc8);
                window.print(this.msg80004, 0);
                ST2241.waitPage(window, 64);
                this.npc8.look_default();
                this.npc5.look_char(this.npc9);
                return;
            }
        }
        this.npc8.look_char(this.npc5);
        window.print(this.msg80001, 0);
        ST2241.waitPage(window, 64);
        this.npc5.look_char(this.npc8);
        window.print(this.msg80002, 0);
        ST2241.waitPage(window, 64);
        ++this.king;
        this.npc8.look_default();
        this.npc5.look_char(this.npc9);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg90001, 0);
        ST2241.waitPage(window, 64);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                this.kow2 = new Uwamono(26, 13);
                this.kow2.SetCallNo(2);
                break;
            }
            case 2: {
                System.println("case2");
                if (Runtime.getFlags(7137, 1) == 0) {
                    this.npc20.disableDTKFlag(131072);
                    this.npc20.disableDTKFlag(65536);
                    break;
                }
                this.npc20.disableDTKFlag(131072);
                this.npc20.disableDTKFlag(65536);
                break;
            }
            case 3: {
                System.println("case3");
                ++this.king;
                ++this.king;
                this.npc8.setMotion(0, 0);
                break;
            }
            case 4: {
                System.println("case4");
                this.npc4.setMotion(0, 6);
                ++this.car;
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後外観街２・３");
                Runtime.jumpCF(2211, 3);
                break;
            }
            case 1: {
                System.println("襲撃後外観街２・４");
                Runtime.jumpCF(2211, 4);
                break;
            }
            case 2: {
                System.println("襲撃後外観街２・１４");
                Runtime.jumpCF(2211, 14);
                break;
            }
            case 3: {
                System.println("襲撃後外観街２・１２");
                Runtime.jumpCF(2211, 12);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(3, 1, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(3, 2, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(3, 3, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightVec(3, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.45f, 0.45f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, -0.075f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1405, 0.8f, 4.2f, -12.2f, 0.0f);
        this.light02 = new Effect(1405, 12.2f, 5.8f, -16.3f, 0.0f);
        this.light03 = new Effect(1405, 7.8f, 5.8f, -30.2f, 0.0f);
        this.light04 = new Effect(1405, 1.0f, 9.2f, -30.7f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.5f, 0.5f, 0.5f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(60);
        this.button = new Effect(1545, 7.0f, 4.8f, -24.849f, 0.0f);
        this.button.setRotate(-40.0f, 0.0f, 0.0f);
        this.button.setScale(0.3f, 0.275f, 0.25f);
        this.hokori = new Effect(1612, 12.5f, 3.6f, -24.727f, 0.0f);
        this.hokori.disp(false);
        this.hokori.setScale(0.5f, 0.5f, 0.5f);
        this.teiten1 = new Uwamono(28690, 7.0f, 4.8f, -24.849f, 0.0f);
        this.teiten1.SetBgm(196612);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(17, false);
        Stage.setVisible(25, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setShootRange(1.0f);
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
        this.tana = new Unit();
        this.tana.mapUnit(40);
        this.tana.start(4, null);
        if (this.TANA == 0) {
            this.col = new Uwamono(28672, 11.504f, 3.6f, -24.727f, 0.0f);
            this.col.SetSize(2.0f, 2.5f, 0.2665f);
            this.col.SetHitKind('\u0002');
        } else {
            this.tana.setTranslate(9.504f, 3.6f, -24.727f);
            this.col = new Uwamono(28672, 9.504f, 3.6f, -24.727f, 0.0f);
            this.col.SetSize(2.0f, 2.5f, 0.2665f);
            this.col.SetHitKind('\u0002');
            this.button.disp(true);
        }
        this.npc4 = new NPC_NORMAL(1546, 4, 0, 14, 13, 8.73f, 0.0f, -9.12f, 270.0f);
        this.npc5 = new NPC_NORMAL(1588, 5, 0, 14, 13, 4.08f, 0.0f, -12.9f, 20.0f);
        this.npc6 = new NPC_NORMAL(1543, 6, 0, 7, 9, 1.19f, 3.6f, -16.97f, 90.0f);
        this.npc7 = new NPC_NORMAL(1558, 7, 0, 0, 9, 2.94f, 3.6f, -13.97f, 0.0f);
        this.npc8 = new NPC_NORMAL(1604, 8, 0, 14, 10, 2.52f, 0.0f, -13.88f, 310.0f);
        this.npc9 = new NPC_NORMAL(1611, 9, 0, 14, 11, 4.28f, 0.0f, -12.39f, 210.0f);
        this.npc20 = new NPC_NORMAL(1604, 20, 0, 14, 10, 8.51f, 0.0f, -21.66f, 0.0f);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 1);
        this.npc5.disableDTKFlag(3);
        this.npc5.setMotion(0, 3);
        this.npc6.setMotion(0, 27);
        this.npc7.setMotion(0, 27);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 30);
        this.npc9.disableDTKFlag(3);
        this.npc9.setMotion(0, 27);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.dispRadar(false);
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc20.talkto("Talk_npc20");
        this.item01 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 268);
        new Uwamono(11, 4);
        this.kow = new Uwamono(10, 4);
        this.kow.SetCallNo(1);
        this.kow3 = new Uwamono(28, 11);
        this.kow3.SetCallNo(3);
        this.kow4 = new Uwamono(27, 18, this.item01);
        this.kow4.SetCallNo(4);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.75f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 9.75f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(5, 0.03f, 0.03f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 9.75f, 40.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 9.75f, 45.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, -15.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(8, 0.0125f, 0.0125f);
        this.cam0.setCFAngle(9, -28.0f, 5.0f, 0.0f, 9.75f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
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

