import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0616
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS11_PRJ {
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
    Enepc npc12;
    Enepc npc13;
    Enepc npc15;
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
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2010;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2043;
    int S2042;
    int S2057;
    Unit sara1;
    Unit sara2;
    Unit sara3;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Light light = new Light(0);
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msgkarai = new String[]{"/[label(Allen)]", "What's the matter? Aren't you going to take the curry to the Commander?", "/[waitkey(1)]/[clear()]", "It'll get cold if you don't hurry.", "/[waitkey(64)]/[close()]"};
    String[] msg00A81D16 = new String[]{"/[label(Allen)]", "Chief, I still think this is a bad idea.", "/[waitkey(1)]/[clear()]", "I don't trust my life to the crew of this junky ship. After all, these people have connections with the notorious Kukai Foundation.", "/[waitkey(64)]/[clear()]"};
    String[] msg00A81D17 = new String[]{"/[label(Shion)]", "Sheesh, Allen, why are you such a worrywart? I just explained it to you, remember?", "/[waitkey(1)]/[clear()]", "Everything will be okay. They don't seem like such bad people. If need be, we've got KOS-MOS on our side.", "/[waitkey(64)]/[clear()]"};
    String[] msg00A81D18 = new String[]{"/[label(Allen)]", "Well, don't forget KOS-MOS just abandoned us.", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC5 = new String[]{"/[label(Allen)]", "Chief, please follow proper procedures and contact Headquarters. No more acting on your own, okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC6 = new String[]{"/[label(Shion)]", "I know, I'll follow HQ's instructions.", "/[waitkey(1)]/[clear()]", "You know, it's not very manly of you to keep bringing up these trivial matters.", "/[waitkey(64)]/[clear()]"};
    String[] msg00B87CC7 = new String[]{"/[label(Allen)]", "W-what are you talking about? It's more like you're not detail oriented.", "/[waitkey(64)]/[close()]"};
    String[] msg0162BD8A = new String[]{"/[label(Shion)]", "This is the kitchen.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8B = new String[]{"/[label(MOMO)]", "Oh, it's KOS-MOS.\n", "/[waitkey(64)]/[close()]"};
    String[] msg1162BD8B = new String[]{"/[label(MOMO)]", "I'll help out too!", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8C = new String[]{"/[label(KOS-MOS)]", "There is no need for additional manpower to complete this task.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8D = new String[]{"/[label(MOMO)]", "Oh, uh, I'm sorry.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8E = new String[]{"/[label(Shion)]", "Hey, KOS-MOS! That was rude! She was just trying to be nice.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8F = new String[]{"/[label(KOS-MOS)]", "Analysis of the current situation shows that I can manage without the help of others. There is no need to divert more personnel to this location.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD90 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD91 = new String[]{"See? She's sooo not cute! And she's always like this!", "/[waitkey(64)]/[close()]"};
    String[] msg01631D3D = new String[]{"/[label(KOS-MOS)]", "Dish washing requires a rather fine adjustment of one's power output.", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Chief! I don't care if it's launch preparations or what. Please go to the bridge and tell them not to make it shake so much!", "/[waitkey(64)]/[close()]"};
    String[] msg01B81D16 = new String[]{"/[label(Allen)]", "I was cleaning things up just as the Captain had ordered.", "/[waitkey(1)]/[clear()]", "But at this rate, more dishes will break than get washed!", "/[waitkey(64)]/[close()]"};
    String[] msg00B87CC3 = new String[]{"/[label(Allen)]", "Sheesh. How did I get stuck with dish duty?", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};

    ST0616() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.96f, 1.887f, 6.207f);
        this.camEV.setRotate(-10.763f, -27.118f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.994f, 2.463f, 3.946f);
        this.camEV.setRotate(-22.363f, 17.559f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
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

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0616.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc5_2(window);
        } else if (this.S2013 == 1) {
            this.Talk_no(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msgkarai, 0);
                ST0616.waitPage(window, 64);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                window.print(this.msg00A81D16, 0);
                ST0616.waitPage(window, 64);
                window.print(this.msg00A81D17, 0);
                ST0616.waitPage(window, 64);
                window.print(this.msg00A81D18, 0);
                ST0616.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg00B87CC5, 0);
        ST0616.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg00B87CC6, 0);
        ST0616.waitPage(window, 64);
        window.print(this.msg00B87CC7, 0);
        ST0616.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc5_2(Window window) {
    }

    void Talk_npc5_3(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg00B81D16, 0);
                ST0616.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg01B81D16, 0);
                ST0616.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00B87CC3, 0);
        ST0616.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2043 == 1) {
            this.Talk_npc9_3(window);
        } else if (this.S2040C == 1) {
            this.Talk_no(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc9_2(window);
        } else if (this.S2013 == 1) {
            this.Talk_no(window);
        } else if (this.S2009 == 1) {
            this.Talk_npc9_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc9_1(Window window) {
    }

    void Talk_npc9_2(Window window) {
        window.print(this.msg01631D3D, 0);
        ST0616.waitPage(window, 64);
    }

    void Talk_npc9_3(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.setFlags(5001, 1, 1);
                Runtime.jumpCF(66206, 7);
                break;
            }
            case 1: {
                Runtime.jumpCF(66072, 2);
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
        this.S2010 = Runtime.getFlags(116, 1);
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
        Stage.setVisible(5, false);
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(50, false);
        Stage.setVisible(51, false);
        Stage.setVisible(104, false);
        Stage.setVisible(105, false);
        Stage.setVisible(130, false);
        Stage.setVisible(118, false);
        Stage.setVisible(19, false);
        Stage.setVisible(20, false);
        Stage.setVisible(36, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(39, false);
        Stage.setVisible(106, false);
        Stage.setVisible(107, false);
        Stage.setVisible(108, false);
        Stage.setVisible(109, false);
        Stage.setVisible(111, false);
        Stage.setVisible(131, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        Stage.setVisible(128, false);
        Stage.setVisible(129, false);
        Stage.setVisible(122, false);
        Stage.setVisible(123, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
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
        this.eve01 = new Effect(1418, 0);
        this.eve01.disp(true);
        this.eve02 = new Effect(1418, 1);
        this.eve02.disp(true);
        this.doorA = new Uwamono(4, 40, '\u0001');
        this.doorB = new Uwamono(97, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        if (Runtime.getFlags(116, 1) == 0 && Runtime.getFlags(7051, 1) == 0) {
            Runtime.setOutFriend(4);
            Runtime.setOutFriend(6);
            Runtime.resetOutFriend(1);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(7051, 1, 1);
        }
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_3();
        } else if (this.S2040C == 1) {
            this.npcset_0();
        } else if (this.S2030 == 1) {
            this.npcset_2();
        } else if (this.S2013 == 1) {
            this.npcset_0();
        } else if (this.S2009 == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 3, 3, 1.07f, 0.0f, -0.5f, 45.0f);
        this.npc5.talkto("Talk_npc5");
    }

    void npcset_2() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, -2.19f, 0.0f, -1.2f, 90.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, -2.79f, 0.0f, -1.1f, 90.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 13, -2.07f, 0.0f, 0.1f, 90.0f);
        this.npc11 = new NPC_NORMAL(4, 11, 0, 12, 5, 1.48f, 0.0f, 0.2f, 350.0f);
        this.npc12 = new NPC_NORMAL(6, 12, 0, 17, 7, -0.54f, 0.0f, -3.07f, 0.0f);
        this.npc13 = new NPC_NORMAL(1, 13, 0, 13, 13, 0.58f, 0.0f, 1.49f, 60.0f);
        this.npc9 = new NPC_NORMAL(2, 9, 0, 13, 9, 1.1f, 0.0f, 1.4f, 45.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.kickEnepc(4, 1);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc9.disableDTKFlag(3);
        this.npc9.setMotion(0, 1);
        this.npc9.setInvalidID(1);
        this.npc9.setTranslate(1.2f, 0.1f, 1.7f);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        this.npc11.setVisible(false);
        this.npc11.enableDTKFlag(262144);
        this.npc11.kickEnepc(4, 1);
        this.npc11.setInvalidID(1);
        this.npc11.setTranslate(1.65f, 0.0f, 0.5f);
        this.npc12.setVisible(false);
        this.npc12.enableDTKFlag(262144);
        this.npc12.kickEnepc(4, 1);
        this.npc13.setVisible(false);
        this.npc13.enableDTKFlag(262144);
        this.npc13.kickEnepc(4, 1);
        this.npc9.talkto("Talk_npc9");
        this.sara1 = new Unit();
        this.sara1.init(24641, 1.6f, 0.6f, 2.0f, 0.0f);
        this.sara2 = new Unit();
        this.sara2.init(24641, 1.42f, 0.5f, 2.1f, 0.0f);
        this.sara3 = new Unit();
        this.sara3.init(24641, 1.75f, 0.5f, 1.8f, 0.0f);
        this.eve03 = new Effect(1407, 2);
        this.eve03.disp(true);
        if (Runtime.getFlags(7020, 1) == 1) {
            return;
        }
        this.npc15 = new NPC_NORMAL(1, 15, 0, 13, 13, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc15.talkto("TalkNPC11");
        this.npc15.setInvalidID(1);
        this.npc15.disableDTKFlag(131072);
        this.npc15.disableDTKFlag(8);
        this.npc15.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_3() {
        this.npc5 = new NPC_NORMAL(263, 5, 0, 13, 9, 1.2f, 0.1f, 1.7f, 45.0f);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.setInvalidID(1);
        this.npc5.talkto("Talk_npc5");
        this.sara1 = new Unit();
        this.sara1.init(24641, 1.6f, 0.6f, 2.0f, 0.0f);
        this.sara2 = new Unit();
        this.sara2.init(24641, 1.42f, 0.5f, 2.1f, 0.0f);
        this.sara3 = new Unit();
        this.sara3.init(24641, 1.75f, 0.5f, 1.8f, 0.0f);
        this.eve03 = new Effect(1407, 2);
        this.eve03.disp(true);
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
            Runtime.setFlags(7020, 1, 1);
            Runtime.enable(65536);
            ST0616.this.cam0.setMode(-1);
            ST0616.this.EV_Camera01();
            ST0616.this.npc10.kickEnepc(0, 9);
            ST0616.this.npc6.kickEnepc(1, 10);
            ST0616.this.npc7.kickEnepc(1, 10);
            ST0616.this.npc6.setVisible(true);
            ST0616.this.npc7.setVisible(true);
            ST0616.this.npc10.setVisible(true);
            System.sleep(10);
            ST0616.this.win = Window.create();
            ST0616.this.win.setSize(4, 45);
            ST0616.this.win.setLocation(15, 305);
            ST0616.this.win.print(ST0616.this.msg0162BD8A, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.win.print(ST0616.this.msg0162BD8B, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.fade.call(0);
            System.sleep(30);
            ST0616.this.npc6.setVisible(false);
            ST0616.this.npc7.setVisible(false);
            ST0616.this.npc10.setVisible(false);
            ST0616.this.npc11.kickEnepc(1, 9);
            ST0616.this.npc12.kickEnepc(1, 27);
            ST0616.this.npc13.kickEnepc(0, 10);
            ST0616.this.npc11.setVisible(true);
            ST0616.this.npc12.setVisible(true);
            ST0616.this.npc13.setVisible(true);
            ST0616.this.EV_Camera02();
            System.sleep(10);
            ST0616.this.npc11.kickEnepc(1, 27);
            ST0616.this.win = Window.create();
            ST0616.this.win.setSize(4, 45);
            ST0616.this.win.setLocation(15, 305);
            ST0616.this.win.print(ST0616.this.msg1162BD8B, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.win.print(ST0616.this.msg0162BD8C, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.npc12.kickEnepc(0, 8);
            ST0616.this.npc11.kickEnepc(1, 0);
            ST0616.this.win.print(ST0616.this.msg0162BD8D, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            System.sleep(1);
            ST0616.this.npc12.kickEnepc(1, 27);
            ST0616.this.npc13.kickEnepc(0, 9);
            ST0616.this.win.print(ST0616.this.msg0162BD8E, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.win.print(ST0616.this.msg0162BD8F, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            System.sleep(1);
            ST0616.this.npc13.kickEnepc(0, 8);
            ST0616.this.win.print(ST0616.this.msg0162BD90, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            ST0616.this.npc13.moveEnepc(17, 90.0f, 0.1f, 20);
            System.sleep(25);
            ST0616.this.npc13.kickEnepc(0, 9);
            ST0616.this.win.print(ST0616.this.msg0162BD91, 0);
            ST0616.waitPage(ST0616.this.win, 64);
            System.sleep(10);
            ST0616.this.fade.call(0);
            System.sleep(30);
            ST0616.this.npc11.setVisible(false);
            ST0616.this.npc12.setVisible(false);
            ST0616.this.npc13.setVisible(false);
            ST0616.this.npc15.setVisible(false);
            ST0616.this.player.setTranslate(0.61f, 0.0f, 1.48f);
            ST0616.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

