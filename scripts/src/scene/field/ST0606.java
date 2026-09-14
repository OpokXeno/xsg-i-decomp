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
import xeno.map.MC_ELS10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0606
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS10_PRJ {
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
    Enepc npc20;
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
    int oyasumi;
    int BUTTON_F = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg011C5351 = new String[]{"/[label(Shion)]", "Allen?! Hey, what are you doing?", "/[waitkey(1)]/[clear()]", "This is the women's quarters you know.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5352 = new String[]{"/[label(Allen)]", "Uh, umm...C-C-Chief...", "/[waitkey(64)]/[close()]"};
    String[] msg111C5352 = new String[]{"/[label(Allen)]", "(Go, Allen!)", "/[waitkey(1)]/[clear()]", "(You're alone with the Chief now. You can tell her how you feel without any interruptions!)", "/[waitkey(1)]/[clear()]", "(Go, Allen! Go!!)", "/[waitkey(64)]/[close()]"};
    String[] msg011C5353 = new String[]{"/[label(Shion)]", "Oh, you just went to the wrong room, right? Allen, you're so scatterbrained.", "/[waitkey(64)]/[close()]"};
    String[] msg011C5354 = new String[]{"/[label(Allen)]", "(No, that isn't it.)", "/[waitkey(1)]/[clear()]", "(I was waiting for you, Chief!)", "/[waitkey(64)]/[close()]"};
    String[] msg011C5355 = new String[]{"/[label(Shion)]", "It's a good thing you're in my room. Just imagine if it was some other woman's room. You wouldn't get off so easily.", "/[waitkey(64)]/[close()]"};
    String[] msg011C5356 = new String[]{"/[label(Allen)]", "(I don't care about any other women...as long as I can be with you.)", "/[waitkey(1)]/[clear()]", "(If only I could express my feelings!)", "/[waitkey(64)]/[close()]"};
    String[] msg111C5356 = new String[]{"/[label(Allen)]", "C-Chief!", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5357 = new String[]{"/[label(Shion)]", "Allen, technically you are a man, so you need to be more careful.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5358 = new String[]{"/[label(Allen)]", "Uh, w-what? Uh, um, what do you mean, technically...?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5359 = new String[]{"/[label(Shion)]", "What? You're not a man?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C535A = new String[]{"/[label(Allen)]", "No, technically...I am.", "/[waitkey(1)]/[clear()]", "Yes.", "/[waitkey(1)]/[clear()]", "Uh...", "/[waitkey(64)]/[close()]"};
    String[] msg011CB307 = new String[]{"/[label(Shion)]", "Come on, you should hurry up and go. People will make a fuss if they find out you were here.", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB308 = new String[]{"/[label(Allen)]", "Uh, um, Chief...the thing is, I...", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB309 = new String[]{"/[label(Shion)]", "I'm really sorry, but I'm busy right now. Do you think we can talk later?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB30A = new String[]{"/[label(Allen)]", "Oh, okay...", "/[waitkey(64)]/[close()]"};
    String[] msg0436E842 = new String[]{"/[label(Shion)]", "This is Cabin 2.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E842 = new String[]{"/[label(Shion)]", "They actually let me use this huge cabin by myself!", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E843 = new String[]{"/[label(MOMO)]", "You get to use it all by yourself, Shion?", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E844 = new String[]{"/[label(Shion)]", "Yes. I think the Captain was trying to be thoughtful because I'm a woman.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E844 = new String[]{"/[label(MOMO)]", "The Captain did that? But why? It's more fun to be with everyone.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E845 = new String[]{"/[label(Shion)]", "Heh heh, that's true. To be honest, I was a little lonely. But starting today, you'll be with me, so I won't be so lonesome anymore. I hope we'll make good roommates, MOMO.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E846 = new String[]{"/[label(MOMO)]", "Yes, I hope we will be good roommates too!", "/[waitkey(64)]/[close()]"};
    String[] msg1436E846 = new String[]{"/[label(Shion)]", "I'm sorry, MOMO.", "/[waitkey(64)]/[close()]"};
    String[] msgoyasumi = new String[]{"/[label(Shion)]", "I'm tired. Maybe we should call it a day?", "/[waitkey(64)]/[close()]"};
    String[] msgoyasumi2 = new String[]{"/[label(Shion)]", "Good night.", "/[waitkey(64)]/[close()]"};
    String[] msgoyasumi3 = new String[]{"/[label(Shion)]", "Staying up late is bad for my skin too!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0606() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.013f, 1.727f, -4.711f);
        this.camEV.setRotate(-8.47f, 177.74f, 0.0f);
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

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (Runtime.getFlags(158, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(143, 1) == 0) {
                    return;
                }
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                switch (this.oyasumi) {
                    case 1: {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.print(this.msgoyasumi, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yeah, I'm a little sleepy\nNo, I have to show them around properly");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.win = Window.create();
                                this.win.print(this.msgoyasumi2, 0);
                                ST0606.waitPage(this.win, 64);
                                this.fade.call(0);
                                System.sleep(33);
                                Runtime.setFlags(144, 1, 1);
                                Runtime.setPlayerControl(true);
                                Runtime.jumpEvent(2310);
                                System.println("フラグオン！");
                                this.BUTTON_F = 0;
                                return;
                            }
                        }
                        this.win = Window.create();
                        this.win.print(this.msgoyasumi3, 0);
                        ST0606.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        return;
                    }
                }
                this.BUTTON_F = 0;
                return;
            }
            case 1: {
                if (Runtime.getFlags(158, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(143, 1) == 0) {
                    return;
                }
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                switch (this.oyasumi) {
                    case 1: {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.print(this.msgoyasumi, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yeah, I'm a little sleepy\nNo, I have to show them around properly");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.win = Window.create();
                                this.win.print(this.msgoyasumi2, 0);
                                ST0606.waitPage(this.win, 64);
                                this.fade.call(0);
                                System.sleep(33);
                                Runtime.setFlags(144, 1, 1);
                                Runtime.setPlayerControl(true);
                                Runtime.jumpEvent(2310);
                                System.println("フラグオン！");
                                this.BUTTON_F = 0;
                                return;
                            }
                        }
                        this.win = Window.create();
                        this.win.print(this.msgoyasumi3, 0);
                        ST0606.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        return;
                    }
                }
                this.BUTTON_F = 0;
                return;
            }
            case 2: {
                if (Runtime.getFlags(122, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(123, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7142, 1) != 0) break;
                Runtime.mailArriveSet(23);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7142, 1, 1);
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

    void Talk_no() {
        this.win.print(this.msgtalk_no, 0);
        ST0606.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0606.waitPage(window, 64);
    }

    public void Talk_npc20(Enepc enepc) {
        if (Runtime.checkItem(10, 64) == 0) {
            this.npc20.disableDTKFlag(65536);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 64);
            return;
        }
    }

    public void Talk_npc5(Enepc enepc) {
        if (this.S2057 == 1) {
            this.Talk_no();
        } else if (this.S2042 == 1) {
            this.Talk_no();
        } else if (this.S2040C == 1) {
            this.Talk_no();
        } else if (this.S2030 == 1) {
            this.Talk_npc5_2();
        } else if (this.S2028 == 1) {
            this.Talk_no();
        } else if (this.S2014B == 1) {
            this.Talk_npc5_1();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc5_1() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.npc5.kickEnepc(0, 0);
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg011C5351, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(0, 9);
                this.win.print(this.msg011C5352, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(0, 7);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg111C5352, 0);
                ST0606.waitPage(this.win, 64);
                this.player.mtn(11, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg011C5353, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(0, 8);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg011C5354, 0);
                ST0606.waitPage(this.win, 64);
                this.player.mtn(11, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg011C5355, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(0, 7);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg011C5356, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg111C5356, 0);
                ST0606.waitPage(this.win, 64);
                this.player.mtn(10, 1, 1.0f, true);
                this.win.print(this.msg011C5357, 0);
                ST0606.waitPage(this.win, 64);
                this.win.print(this.msg011C5358, 0);
                ST0606.waitPage(this.win, 64);
                this.win.print(this.msg011C5359, 0);
                ST0606.waitPage(this.win, 64);
                this.win.print(this.msg011C535A, 0);
                ST0606.waitPage(this.win, 64);
                this.npc5.kickEnepc(7, 14);
                Runtime.disable(65536);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg011CB307, 0);
        ST0606.waitPage(this.win, 64);
        this.npc5.kickEnepc(1, 9);
        this.win.print(this.msg011CB308, 0);
        ST0606.waitPage(this.win, 64);
        this.win.print(this.msg011CB309, 0);
        ST0606.waitPage(this.win, 64);
        this.win.print(this.msg011CB30A, 0);
        ST0606.waitPage(this.win, 64);
    }

    void Talk_npc5_2() {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66206, 5);
                break;
            }
            case 1: {
                Runtime.jumpCF(66206, 6);
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
        this.oyasumi = Runtime.getFlags(7026, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
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
        this.cam0.setCFPedestal(5, 6.45f, 4.0f, 1.9f, 50.0f, -62.23f, 0.0f, 0.0f, 2.0f);
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
        this.doorA = new Uwamono(71, 40, '\u0002');
        this.doorB = new Uwamono(72, 40, '\u0002');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_0();
        } else if (this.S2040C == 1) {
            this.npcset_0();
        } else if (this.S2030 == 1) {
            this.npcset_2();
        } else if (this.S2028 == 1) {
            this.npcset_0();
        } else if (this.S2014B == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
        this.npc20 = new NPC_NORMAL(263, 20, 0, 14, 3, 2.3f, 0.0f, 0.48f, 50.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.talkto("Talk_npc20");
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 1, 3, -1.59f, 0.0f, -3.44f, 50.0f);
        this.npc5.enableDTKFlag(262144);
        this.npc5.disableDTKFlag(1);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_2() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, -0.83f, 0.0f, 1.28f, 180.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, 1.4f, 0.0f, 0.895f, 225.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 9, 0.28f, 0.0f, 1.17f, 180.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.kickEnepc(4, 1);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        if (Runtime.getFlags(7025, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 9, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
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
            Runtime.setFlags(7025, 1, 1);
            Runtime.setFlags(7026, 1, 1);
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST0606.this.cam0.setMode(-1);
            ST0606.this.EV_Camera01();
            ST0606.this.npc10.kickEnepc(0, 9);
            ST0606.this.npc6.kickEnepc(1, 27);
            ST0606.this.npc7.kickEnepc(1, 27);
            ST0606.this.npc6.setVisible(true);
            ST0606.this.npc7.setVisible(true);
            ST0606.this.npc10.setVisible(true);
            System.sleep(10);
            ST0606.this.win = Window.create();
            ST0606.this.win.setSize(4, 45);
            ST0606.this.win.setLocation(15, 305);
            ST0606.this.win.print(ST0606.this.msg0436E842, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc10.moveEnepc(17, 240.0f, 0.1f, 20);
            ST0606.this.win.print(ST0606.this.msg1436E842, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc6.moveEnepc(17, 100.0f, -0.1f, 20);
            ST0606.this.npc6.kickEnepc(1, 10);
            ST0606.this.win.print(ST0606.this.msg0436E843, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc10.kickEnepc(0, 10);
            ST0606.this.win.print(ST0606.this.msg0436E844, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc6.kickEnepc(0, 9);
            ST0606.this.win.print(ST0606.this.msg1436E844, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc10.kickEnepc(0, 9);
            ST0606.this.win.print(ST0606.this.msg0436E845, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            ST0606.this.npc6.kickEnepc(0, 9);
            ST0606.this.win.print(ST0606.this.msg0436E846, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            System.sleep(10);
            ST0606.this.fade.call(0);
            System.sleep(30);
            ST0606.this.npc6.setVisible(false);
            ST0606.this.npc7.setVisible(false);
            ST0606.this.npc10.setVisible(false);
            ST0606.this.npc11.setVisible(false);
            ST0606.this.player.setTranslate(0.28f, 0.0f, -0.17f);
            ST0606.this.cam0.setMode(0);
            System.sleep(10);
            ST0606.this.win = Window.create();
            ST0606.this.win.print(ST0606.this.msgoyasumi, 0);
            System.waitFor(ST0606.this.win);
            ST0606.this.menu = Menu.create();
            ST0606.this.menu.addItem("Yeah, I'm a little sleepy\nNo, I have to show them around properly");
            System.waitFor(ST0606.this.menu);
            ST0606.this.selected = ST0606.this.menu.getSelected();
            switch (ST0606.this.selected) {
                case 0: {
                    ST0606.this.win = Window.create();
                    ST0606.this.win.print(ST0606.this.msgoyasumi2, 0);
                    ST0606.waitPage(ST0606.this.win, 64);
                    ST0606.this.fade.call(0);
                    System.sleep(33);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    Runtime.setFlags(144, 1, 1);
                    Runtime.jumpEvent(2310);
                    System.println("フラグオン！");
                }
            }
            ST0606.this.win = Window.create();
            ST0606.this.win.print(ST0606.this.msgoyasumi3, 0);
            ST0606.waitPage(ST0606.this.win, 64);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

