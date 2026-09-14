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

class ST2240
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK10B_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int JOHNNY_LOOK = 0;
    Uwamono doorA;
    Uwamono kow;
    Uwamono kow2;
    Unit tana;
    Uwamono col;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc20;
    int talkFlag20;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Uwamono Atari;
    Uwamono Atari2;
    Enepc debug;
    Effect fire01;
    Effect fire02;
    Effect button;
    Effect hokori;
    Enepc enemy1;
    Enepc enemy2;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono item01;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    int TANA = Runtime.getFlags(6047, 1);
    int page;
    String[] Q = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] msghelp10 = new String[]{"/[label(King)]", "Oh, dear, what should I do?! My precious, precious kitten!", "/[waitkey(1)]/[clear()]", "If something were ever to happen to her, I couldn't live with myself!", "/[waitkey(64)]/[clear()]"};
    String[] msghelp101 = new String[]{"/[label(King)]", "Hey, you! You better go save my darling kitten!", "/[waitkey(1)]/[clear()]", "Got it? This is an order!", "/[waitkey(64)]/[close()]"};
    String[] msghelp102 = new String[]{"/[label(King)]", "Hey! You rescued my darling kitten, didn't you?!", "/[waitkey(1)]/[clear()]", "I'm impressed!", "/[waitkey(64)]/[close()]"};
    String[] msghelp11 = new String[]{"/[label(Johnny)]", "Oh, no you don't. You'll have to get by me first.", "/[waitkey(1)]/[clear()]", "I would never abandon my loved one to escape!", "/[waitkey(64)]/[close()]"};
    String[] msghelp12 = new String[]{"/[label(Mina)]", "Oh, you scared me!", "/[waitkey(1)]/[clear()]", "Johnny just shoved me in here all of a sudden.", "/[waitkey(64)]/[clear()]"};
    String[] msghelp121 = new String[]{"/[label(Mina)]", "Johnny?!", "/[waitkey(1)]/[clear()]", "What are you doing?! Let's get out of here!", "/[waitkey(64)]/[close()]"};
    String[] msghelp13 = new String[]{"/[label(Johnny)]", "Yes, ma'am!", "/[waitkey(64)]/[close()]"};
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
    String[] msgJOHNNY_LOOK = new String[]{"/[label(Johnny)]", "G-give it up! I'm not letting you past this point!!", "/[waitkey(64)]/[close()]"};
    String[] msg0help2 = new String[]{"/[label()]", "13 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg1help2 = new String[]{"/[label()]", "12 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg2help2 = new String[]{"/[label()]", "11 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg3help2 = new String[]{"/[label()]", "10 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg4help2 = new String[]{"/[label()]", "9 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg5help2 = new String[]{"/[label()]", "8 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg6help2 = new String[]{"/[label()]", "7 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg7help2 = new String[]{"/[label()]", "6 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg8help2 = new String[]{"/[label()]", "5 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg9help2 = new String[]{"/[label()]", "4 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg10help2 = new String[]{"/[label()]", "3 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg11help2 = new String[]{"/[label()]", "2 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg12help2 = new String[]{"/[label()]", "1 more person left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg13helpok = new String[]{"/[label()]", "Rescued everyone!", "/[waitkey(64)]/[close()]"};

    ST2240() {
    }

    void EOB(int n) {
        if (n == 12) {
            Runtime.setFlags(8106, 1, 1);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(11.749f, 5.921f, -12.478f);
        this.camEV.setRotate(-25.39f, 0.059f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.287f, 2.365f, -15.885f);
        this.camEV.setRotate(-22.876f, 11.94f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.188f, 5.759f, -19.295f);
        this.camEV.setRotate(-7.835f, 347.251f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.269f, 7.839f, -12.625f);
        this.camEV.setRotate(-52.421f, -0.699f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        if (Runtime.getFlags(8106, 1) == 0) {
            this.enemy2.kickEnepc(4, 0);
        }
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        if (Runtime.getFlags(8106, 1) == 0) {
            this.enemy2.kickEnepc(4, 2);
        }
    }

    void Ene_Vfalse() {
        this.enemy1.setVisible(false);
        if (Runtime.getFlags(8106, 1) == 0) {
            this.enemy2.setVisible(false);
        }
    }

    void Ene_Vtrue() {
        this.enemy1.setVisible(true);
        if (Runtime.getFlags(8106, 1) == 0) {
            this.enemy2.setVisible(true);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck && this.TANA == 0) {
            this.Ene_Stop();
            System.sleep(1);
            System.println("移動棚");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 15);
            this.win.print(this.Q, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Press\nDon't press\n");
            System.waitFor(this.menu);
            if (this.menu.getSelected() == 0) {
                System.sleep(30);
                Sound.effectPlay(58);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                System.sleep(30);
                this.button.disp(false);
                this.hokori.disp(true);
                Sound.effectPlay(196748);
                float f = 11.504f;
                while (f >= 9.504f) {
                    this.tana.setTranslate(f, 3.6f, -24.727f);
                    System.sleep(1);
                    f -= 0.035f;
                }
                this.tana.setTranslate(9.504f, 3.6f, -24.727f);
                this.col.setTranslate(9.504f, 3.6f, -24.727f);
                System.sleep(30);
                this.hokori.disp(false);
                Runtime.setFlags(6047, 1, 1);
                this.TANA = Runtime.getFlags(6047, 1);
            }
            System.sleep(10);
            this.EnterCheck = false;
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            this.Ene_Restart();
        }
        switch (n2) {
            case 1: {
                if (this.JOHNNY_LOOK == 1) {
                    return;
                }
                if (Runtime.getFlags(7120, 1) == 1) {
                    return;
                }
                this.enemy1.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.player.look_char(this.npc2);
                this.cam0.setMode(-1);
                this.EV_Camera04();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgJOHNNY_LOOK, 0);
                ST2240.waitPage(this.win, 64);
                System.sleep(15);
                this.cam0.setMode(0);
                this.player.look_default();
                Runtime.setPlayerControl(true);
                ++this.JOHNNY_LOOK;
                this.enemy1.kickEnepc(4, 0);
                break;
            }
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
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2240.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2240.waitPage(this.win, 64);
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
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 1: {
                if (Runtime.getFlags(7144, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgTALK_ME8, 0);
                    ST2240.waitPage(this.win, 64);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME16, 0);
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME15, 0);
                ST2240.waitPage(this.win, 64);
                return;
            }
            case 3: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME6, 0);
                ST2240.waitPage(this.win, 64);
                Runtime.setFlags(7128, 1, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME13, 0);
        ST2240.waitPage(this.win, 64);
    }

    public void Talk_npc1(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(11.21f, 3.6f, -14.64f);
        this.player.rotY(1, 110.0f, true);
        System.sleep(2);
        Runtime.disable(65536);
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.player.look_char(this.npc1);
        this.npc1.look_char(this.player);
        System.sleep(10);
        if (Runtime.getFlags(7114, 1) == 0) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp10, 0);
            ST2240.waitPage(this.win, 64);
            this.npc1.kickEnepc(1, 9);
            this.win.print(this.msghelp101, 0);
            ST2240.waitPage(this.win, 64);
        } else {
            this.npc1.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msghelp102, 0);
            ST2240.waitPage(this.win, 64);
        }
        this.npc1.look_default();
        this.npc1.disableDTKFlag(2);
        this.npc1.kickEnepc(9, -1);
        this.npc1.kickEnepc(1, 3);
        this.npc1.moveEnepc(17, 0.0f, 0.1f, 5);
        System.sleep(3);
        this.npc1.moveEnepc(15, 11.65f, -10.52f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        ++this.count;
        Runtime.setFlags(7119, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Ene_Stop();
        System.sleep(1);
        window.print(this.msghelp11, 0);
        ST2240.waitPage(window, 64);
        this.Ene_Restart();
    }

    public void Talk_npc20(Enepc enepc) {
        this.Talk_npc20_1();
    }

    void Talk_npc20_1() {
        this.Ene_Stop();
        if (Runtime.getFlags(7127, 1) == 1) {
            if (Runtime.getFlags(7137, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME4, 0);
                ST2240.waitPage(this.win, 64);
                this.TALK_ME();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME9, 0);
                ST2240.waitPage(this.win, 64);
                this.win.print(this.msgTALK_ME10, 0);
                ST2240.waitPage(this.win, 64);
                Sound.effectPlay(10);
                this.win.print(this.msgTALK_ME11, 0);
                ST2240.waitPage(this.win, 64);
                Runtime.setFlags(7156, 1, 1);
                Runtime.setFlags(7137, 1, 1);
                this.Ene_Restart();
                return;
            }
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgTALK_ME12, 0);
            ST2240.waitPage(this.win, 64);
            this.TALK_ME2();
            this.Ene_Restart();
            return;
        }
        ++this.talkFlag20;
        switch (this.talkFlag20) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME1, 0);
                ST2240.waitPage(this.win, 64);
                this.Ene_Restart();
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTALK_ME2, 0);
                ST2240.waitPage(this.win, 64);
                this.Ene_Restart();
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgTALK_ME3, 0);
        ST2240.waitPage(this.win, 64);
        this.Ene_Restart();
    }

    public void Talk_npc3(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(9.08f, 0.0f, -19.2f);
        this.player.rotY(1, 250.0f, true);
        this.npc3.moveEnepc(17, 45.0f, 0.1f, 5);
        System.sleep(2);
        this.Ene_Vfalse();
        this.cam0.setMode(-1);
        this.EV_Camera02();
        System.sleep(10);
        this.player.look_char(this.npc3);
        this.npc3.look_char(this.player);
        this.npc3.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp12, 0);
        ST2240.waitPage(this.win, 64);
        this.npc3.kickEnepc(1, 0);
        this.npc3.look_char(this.npc2);
        this.npc2.look_char(this.npc3);
        this.win.print(this.msghelp121, 0);
        ST2240.waitPage(this.win, 64);
        this.npc3.look_default();
        this.npc3.disableDTKFlag(2);
        this.npc3.kickEnepc(9, -1);
        this.npc3.kickEnepc(1, 3);
        this.npc3.moveEnepc(17, 0.0f, -0.1f, 5);
        System.sleep(3);
        this.npc3.moveEnepc(15, 8.4f, -12.92f, 40);
        System.sleep(20);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp13, 0);
        ST2240.waitPage(this.win, 64);
        this.npc2.look_default();
        this.npc2.disableDTKFlag(2);
        this.npc2.kickEnepc(9, -1);
        this.npc2.kickEnepc(1, 3);
        this.npc3.moveEnepc(17, 0.0f, 0.1f, 5);
        System.sleep(3);
        this.npc2.moveEnepc(15, 8.4f, -12.92f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.Ene_Vtrue();
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(65536);
        this.npc2.setVisible(false);
        this.npc3.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(65536);
        this.npc3.setVisible(false);
        ++this.count;
        ++this.count;
        Runtime.setFlags(7120, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Restart();
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
        }
    }

    void countdown() {
        if (Runtime.getFlags(7092, 4) == 14) {
            return;
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        switch (Runtime.getFlags(7092, 4)) {
            case 0: {
                this.win.print(this.msg0help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2240.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2240.waitPage(this.win, 64);
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
                Runtime.jumpCF(67746, 3);
                break;
            }
            case 1: {
                System.println("襲撃後外観街２・４");
                Runtime.jumpCF(67746, 4);
                break;
            }
            case 2: {
                System.println("襲撃後外観街２・１４");
                Runtime.jumpCF(67746, 14);
                break;
            }
            case 3: {
                System.println("襲撃後外観街２・１２");
                Runtime.jumpCF(67746, 12);
                break;
            }
        }
    }

    void init() {
        this.count = Runtime.getFlags(7092, 4);
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
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(4, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(4, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
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
        this.button = new Effect(1545, 7.0f, 4.8f, -24.849f, 0.0f);
        this.button.setRotate(-40.0f, 0.0f, 0.0f);
        this.button.setScale(0.3f, 0.275f, 0.25f);
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
        this.Atari = new Uwamono(28672, 4.04f, 1.54f, -21.18f, 0.0f);
        this.Atari.SetSize(2.0f, 3.0f, 1.5f);
        this.Atari2 = new Uwamono(28672, 8.9f, 3.59f, -22.99f, 0.0f);
        this.Atari2.SetSize(1.0f, 3.0f, 2.0f);
        if (Runtime.getFlags(7119, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1604, 1, 0, 14, 3, 11.79f, 4.0f, -15.15f, 340.0f);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(3);
            this.npc1.enableDTKFlag(262144);
            this.npc1.setMotion(0, 16);
            this.npc1.talkto("Talk_npc1");
        }
        if (Runtime.getFlags(7120, 1) == 0) {
            this.npc2 = new NPC_NORMAL(1613, 2, 0, 14, 6, 7.92f, 0.0f, -18.08f, 340.0f);
            this.npc2.disableDTKFlag(3);
            this.npc2.enableDTKFlag(262144);
            this.npc2.setMotion(0, 27);
            this.npc2.talkto("Talk_npc2");
            this.npc3 = new NPC_NORMAL(1606, 3, 0, 14, 3, 0.0f, 0.0f, 0.0f, 0.0f);
            this.npc3.disableDTKFlag(3);
            this.npc3.enableDTKFlag(262144);
            this.npc3.talkto("Talk_npc3");
            this.npc3.setMotion(0, 16);
            this.npc3.setInvalidID(1);
        }
        this.npc20 = new NPC_NORMAL(1604, 20, 0, 14, 11, 8.51f, 0.0f, -21.66f, 0.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.dispRadar(false);
        this.npc20.talkto("Talk_npc20");
        this.fire01 = new Effect(1402, 3.9f, 3.0f, -21.2f, 0.0f);
        this.fire02 = new Effect(1402, 9.0f, 3.6f, -23.25f, 0.0f);
        this.fire02.setScale(1.0f, 1.5f, 2.5f);
        this.hokori = new Effect(1612, 12.5f, 3.6f, -24.727f, 0.0f);
        this.hokori.disp(false);
        this.hokori.setScale(0.5f, 0.5f, 0.5f);
        this.teiten1 = new Uwamono(28690, 7.0f, 4.8f, -24.849f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, 3.9f, 3.0f, -21.2f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 9.0f, 3.6f, -23.25f, 0.0f);
        this.teiten3.SetBgm(196614);
        float[] fArray = new float[12];
        fArray[0] = 4.0f;
        fArray[1] = 3.61f;
        fArray[2] = -23.6f;
        fArray[3] = -1.0f;
        fArray[4] = 4.5f;
        fArray[5] = 3.61f;
        fArray[6] = -23.6f;
        fArray[8] = 3.5f;
        fArray[9] = 3.61f;
        fArray[10] = -23.6f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16392, 11, 1, 115, 12, 4.0f, 3.61f, -23.6f, 270.0f, fArray2);
        this.enemy1.setGroup(1, 1, 1, 1);
        if (Runtime.getFlags(8106, 1) == 0) {
            float[] fArray3 = new float[12];
            fArray3[0] = 6.4f;
            fArray3[2] = -16.5f;
            fArray3[3] = -1.0f;
            fArray3[4] = 5.9f;
            fArray3[6] = -16.5f;
            fArray3[8] = 6.9f;
            fArray3[10] = -16.5f;
            float[] fArray4 = fArray3;
            this.enemy2 = new NpcEnemy(16390, 12, 2, 139, 14, 6.4f, 0.0f, -16.5f, 130.0f, fArray4);
            this.enemy2.setGroup(0, 0, 0, 0);
            this.enemy2.setMotion(0, 27);
        }
        this.item01 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 268);
        new Uwamono(28, 11);
        new Uwamono(27, 18, this.item01);
        this.kow = new Uwamono(10, 4);
        this.kow.SetCallNo(1);
        if (Runtime.getFlags(7120, 1) == 0) {
            new Uwamono(11, 4, this.npc3);
        } else {
            new Uwamono(11, 4);
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

    class NpcEnemy
            extends Enepc {
        NpcEnemy(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }
}

