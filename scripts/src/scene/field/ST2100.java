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
import xeno.map.MC_KUK10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2100
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK10_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int king = 0;
    int car = 0;
    Uwamono doorA;
    Uwamono kow;
    Uwamono kow2;
    Uwamono kow3;
    Uwamono kow4;
    Unit tana;
    Uwamono col;
    Effect Obj600;
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
    boolean EnterCheck = false;
    Light light = new Light(0);
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
    Effect button;
    Uwamono teiten1;
    Uwamono item01;
    Uwamono item02;
    int TANA = Runtime.getFlags(6047, 1);
    int page;
    String[] Q = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "What do you want? There's no switch behind me. Don't concern yourself with unnecessary things. Go away!", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "What do you want? Don't you give up? Like I said, there's no switch behind me! Something terrible will happen if you press it. Go away!", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Heh heh heh, you don't belong here. You'd better hurry on outta here if you know what's good for ya.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "This is Master King's warehouse. What're you doing here?", "/[waitkey(1)]/[clear()]", "King absolutely loves cats. He loves cats so much, he'd easily give up the life of one of his underlings for a cat. Don't you think we're really patient to put up\nwith that?", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "This is the best way to kill time. Junk is brought to this warehouse from colonies all over the place.", "/[waitkey(1)]/[clear()]", "There are lots of stuff that can be used with a bit of fixing.", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label()]", "Huh?! My c-car? Hey, did you see the car that was here? Am I dreaming or something?", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Hey, were you scared? Mr. King scared you, didn't he?", "/[waitkey(1)]/[clear()]", "There's nothing more frightening than the way he changes when he talks to cats. Oh, that's a taboo subject in this town.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Dance, dance, dance! Well? Pretty cool, huh?", "/[waitkey(1)]/[clear()]", "I'm going to spread this dance throughout the Foundation. Hah! Hah! Hah!", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "Dance, dance, dance!", "/[waitkey(1)]/[clear()]", "Well? Doesn't this make you want to dance too? It's a dance you'll never forget, once you've seen it! Hah! Hah! Hah!", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "Heh heh, this is Master King's exclusive dealmaking area. This is no place for tourists like you.", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label(King)]", "Who the hell are you?!", "/[waitkey(1)]/[clear()]", "Have you come here to see the great King, who controls the Foundation from behind the scenes?", "/[waitkey(64)]/[close()]"};
    String[] msg80002 = new String[]{"/[label(King)]", "Well, you either have guts or are simply and idiot to come here after hearing stories about me.", "/[waitkey(64)]/[close()]"};
    String[] msg80003 = new String[]{"/[label(King)]", "What? You never heard any stories about me?! You guys are unbelievable.", "/[waitkey(64)]/[close()]"};
    String[] msg80004 = new String[]{"/[label(King)]", "Perfect timing. We'll see if you can keep that calm look on your faces after you've seen my powers. Haaah!", "/[waitkey(64)]/[close()]"};
    String[] msg80005 = new String[]{"/[label(King)]", "Mwa ha ha ha ha ha! What do you think of my powers?! You scared? I bet you are. Cat got your tongue?", "/[waitkey(1)]/[clear()]", "Mwa ha ha ha ha ha!", "/[waitkey(64)]/[close()]"};
    String[] msg80006 = new String[]{"/[label(King)]", "Oh, my, Fran#27ois!", "/[waitkey(64)]/[clear()]"};
    String[] msg800061 = new String[]{"/[label(King)]", "My darling kitty! I was sooo worried about you.", "/[waitkey(64)]/[clear()]"};
    String[] msg800062 = new String[]{"/[label(King)]", "Waaait, where are you goooing? Don't do anything naughty now.", "/[wait(120)]/[close()]"};
    String[] msg80010 = new String[]{"/[label()]", "Meow!", "/[waitkey(64)]/[close()]"};
    String[] msg80007 = new String[]{"/[label(King)]", "Y-you're not half bad. How about you come work for me? You'll get to do whatever you want in this city. First, I'll teach you how to take care of cats! Isn't that incredible?", "/[waitkey(64)]/[close()]"};
    String[] msg80008 = new String[]{"/[label(King)]", "Humph, you think you can compete with just that? Go home!", "/[waitkey(64)]/[close()]"};
    String[] msg80009 = new String[]{"/[label(King)]", "How long are you going to hang around here? Get outta here before you get hurt!", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME1 = new String[]{"/[label(Pink Bug)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME2 = new String[]{"/[label(Pink Bug)]", "I wanna see Tom...", "/[waitkey(64)]/[close()]"};
    String[] msgTALK_ME3 = new String[]{"/[label(Pink Bug)]", "I wonder if Tom's doing okay? I wanna go back to the Dock Colony...", "/[waitkey(64)]/[close()]"};
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

    ST2100() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.586f, 2.586f, -10.825f);
        this.camEV.setRotate(-20.159f, 57.518f, 0.0f);
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
        this.camEV.setTranslate(4.366f, 2.07f, -9.544f);
        this.camEV.setRotate(-15.449f, 385.252f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
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
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2100.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2100.waitPage(this.win, 64);
                Runtime.setFlags(7128, 1, 1);
                return;
            }
        }
    }

    void TALK_ME2() {
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
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2100.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2100.waitPage(this.win, 64);
                Runtime.setFlags(7128, 1, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME13, 0);
        ST2100.waitPage(this.win, 64);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg10001, 0);
                ST2100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg10002, 0);
        ST2100.waitPage(window, 64);
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
                ST2100.waitPage(this.win, 64);
                this.TALK_ME();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME9, 0);
                ST2100.waitPage(this.win, 64);
                this.win.print(this.msgTALK_ME10, 0);
                ST2100.waitPage(this.win, 64);
                Sound.effectPlay(10);
                this.win.print(this.msgTALK_ME11, 0);
                ST2100.waitPage(this.win, 64);
                Runtime.setFlags(7156, 1, 1);
                Runtime.setFlags(7137, 1, 1);
                return;
            }
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgTALK_ME12, 0);
            ST2100.waitPage(this.win, 64);
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
                ST2100.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME2, 0);
                ST2100.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME3, 0);
        ST2100.waitPage(this.win, 64);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2100.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2100.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        switch (this.car) {
            case 1: {
                this.npc4.kickEnepc(1, 0);
                window.print(this.msg40002, 0);
                ST2100.waitPage(window, 64);
                this.npc4.setMotion(0, 0);
                return;
            }
        }
        window.print(this.msg40001, 0);
        ST2100.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2100.waitPage(window, 64);
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
                ST2100.waitPage(window, 64);
                return;
            }
        }
        this.npc6.kickEnepc(1, 27);
        window.print(this.msg60002, 0);
        ST2100.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2100.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc) {
        this.Talk_npc8_1();
    }

    void Talk_npc8_1() {
        if (Runtime.getFlags(7159, 1) == 0) {
            Runtime.disable(524288);
            this.fade.call(0);
            System.sleep(30);
            this.npc8.look_char(this.player);
            this.player.setTranslate(3.07f, 0.0f, -13.77f);
            this.player.setRotate(0.0f, 270.0f, 0.0f);
            this.cam0.setMode(-1);
            this.EV_Camera03();
            this.npc8.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80001, 0);
            ST2100.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80004, 0);
            ST2100.waitPage(this.win, 64);
            this.npc8.look_default();
            System.sleep(15);
            this.npc8.kickEnepc(0, 26);
            System.sleep(30);
            this.npc8.kickEnepc(1, 29);
            System.sleep(30);
            this.kow3.SendSignal();
            System.sleep(10);
            this.npc8.look_char(this.player);
            this.npc8.kickEnepc(1, 28);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80005, 0);
            ST2100.waitPage(this.win, 64);
            this.npc9 = new NPC_NORMAL(1611, 9, 0, 14, 12, 5.18f, 0.0f, -17.23f, 335.0f);
            this.npc9.setInvalidID(1);
            this.npc9.enableDTKFlag(262144);
            this.npc9.kickEnepc(4, 1);
            this.npc9.kickEnepc(1, 1);
            this.player.look_char(this.npc9);
            this.npc8.look_char(this.npc9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 15);
            this.win.print(this.msg80010, 0);
            ST2100.waitPage(this.win, 64);
            this.npc9.moveEnepc(15, 2.56f, -9.52f, 240);
            System.sleep(240);
            this.npc9.setTranslate(100.0f, 0.0f, 100.0f);
            this.npc8.setVisible(false);
            this.npc10 = new NPC_NORMAL(1604, 10, 0, 14, 21, 2.52f, 0.0f, -13.88f, 0.0f);
            this.npc10.setInvalidID(1);
            this.npc10.enableDTKFlag(262144);
            this.npc10.kickEnepc(4, 1);
            this.npc10.kickEnepc(0, 16);
            this.cam0.setMode(0);
            this.player.look_char(this.npc10);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80006, 0);
            ST2100.waitPage(this.win, 64);
            this.npc10.kickEnepc(1, 9);
            this.win.print(this.msg800061, 0);
            ST2100.waitPage(this.win, 64);
            this.npc10.kickEnepc(1, 3);
            this.npc10.moveEnepc(15, 2.69f, -8.45f, 90);
            this.win.print(this.msg800062, 0);
            ST2100.waitPage(this.win, 64);
            System.sleep(30);
            this.player.look_default();
            this.npc10.setTranslate(100.0f, 0.0f, 100.0f);
            this.npc8.disableDTKFlag(131072);
            this.npc8.disableDTKFlag(65536);
            Runtime.setFlags(7159, 1, 1);
            Runtime.enable(524288);
            return;
        }
        switch (this.king) {
            case 1: {
                this.npc8.look_char(this.player);
                this.npc8.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg80007, 0);
                ST2100.waitPage(this.win, 64);
                this.npc8.look_default();
                return;
            }
        }
        this.npc8.look_char(this.player);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg80009, 0);
        ST2100.waitPage(this.win, 64);
        this.npc8.look_default();
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                this.kow2 = new Uwamono(17, 13);
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
                this.npc8.setMotion(0, 0);
                break;
            }
            case 4: {
                System.println("case4");
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
                System.println("外観街２・３");
                Runtime.jumpCF(2070, 3);
                break;
            }
            case 1: {
                System.println("外観街２・４");
                Runtime.jumpCF(2070, 4);
                break;
            }
            case 2: {
                System.println("外観街２・１４");
                Runtime.jumpCF(2070, 14);
                break;
            }
            case 3: {
                System.println("外観街２・１２");
                Runtime.jumpCF(2070, 12);
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
        this.teiten1 = new Uwamono(28690, 7.0f, 4.8f, -24.849f, 0.0f);
        this.teiten1.SetBgm(196612);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setShootRange(1.0f);
        this.item01 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 241);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 242);
        new Uwamono(37, 4, this.item01);
        this.kow = new Uwamono(36, 4);
        this.kow.SetCallNo(1);
        this.kow4 = new Uwamono(16, 18, this.item02);
        this.kow4.SetCallNo(4);
        if (Runtime.getFlags(7159, 1) == 0) {
            this.kow3 = new Uwamono(27, 11);
            this.kow3.SetBroken(false);
        } else {
            this.kow3 = new Uwamono(27, 11);
            this.kow3.SetCallNo(3);
        }
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
            this.button.disp(false);
        }
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
        this.npc1 = new NPC_NORMAL(1541, 1, 0, 14, 10, 7.0f, 3.6f, -24.26f, 0.0f);
        this.npc2 = new NPC_NORMAL(1561, 2, 0, 14, 13, 1.64f, 0.0f, -5.71f, 100.0f);
        this.npc3 = new NPC_NORMAL(1609, 3, 0, 14, 14, 2.37f, 0.1f, -6.41f, 0.0f);
        this.npc4 = new NPC_NORMAL(1546, 4, 0, 14, 15, 8.73f, 0.0f, -9.12f, 270.0f);
        this.npc5 = new NPC_NORMAL(527, 5, 0, 14, 13, 5.61f, 0.0f, -18.67f, 0.0f);
        this.npc6 = new NPC_NORMAL(1543, 6, 0, 7, 10, 1.19f, 3.6f, -16.97f, 90.0f);
        this.npc7 = new NPC_NORMAL(1609, 7, 0, 14, 13, 12.36f, 3.6f, -10.55f, 0.0f);
        this.npc8 = new NPC_NORMAL(1604, 8, 0, 14, 11, 2.52f, 0.0f, -13.88f, 310.0f);
        this.npc20 = new NPC_NORMAL(1604, 20, 0, 14, 11, 8.51f, 0.0f, -21.66f, 0.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 10);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 10);
        this.npc2.setInvalidID(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 28);
        this.npc3.setInvalidID(1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(262144);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 16);
        this.npc5.setMotion(0, 9);
        this.npc6.setMotion(0, 27);
        this.npc7.setMotion(0, 10);
        this.npc7.setInvalidID(1);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(262144);
        this.npc8.setMotion(0, 30);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.dispRadar(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc20.talkto("Talk_npc20");
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

