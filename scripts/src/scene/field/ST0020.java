import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0020
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK02_PRJ {
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
    Menu menu;
    Window win;
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
    int BUTTON_F = 0;
    Uwamono doorA;
    Uwamono doorB;
    int S07A3;
    int S07B;
    int S07C;
    int S08;
    int S012;
    int S014A;
    int S014B;
    int S015B;
    Unit monitor1;
    Unit monitor2;
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
    int page;
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Corridor 1'", "/[waitkey(64)]/[close()]"};
    String[] msg01496400 = new String[]{"/[label()]", "When you have lost sight of the path, when you have become lost in life, seek information. Information will surely give you a glimmer of hope.", "/[waitkey(64)]/[close()]"};
    String[] msg0161C79A = new String[]{"/[label()]", "Seek, or you will not be granted what it is you are looking for. There is no shame in asking for information.", "/[waitkey(64)]/[close()]"};
    String[] msg01624AFF = new String[]{"/[label()]", "Incidentally, you must not ask me, as I know nothing. I take pride in knowing nothing at all.", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "I have a bad feeling about this mission, but I can't put my finger on why.", "/[waitkey(1)]/[clear()]", "It's just a weird, uneasy feeling...", "/[waitkey(64)]/[clear()]"};
    String[] msg00000002 = new String[]{"/[label()]", "I agree with you.", "/[waitkey(1)]/[clear()]", "Some strangers are transporting odd objects into that hangar over there.", "/[waitkey(1)]/[clear()]", "There's something very odd about this mission.", "/[waitkey(64)]/[close()]"};
    String[] msg00000003 = new String[]{"/[label()]", "I have a bad feeling about this mission.", "/[waitkey(1)]/[clear()]", "I hope it's just my imagination.", "/[waitkey(64)]/[close()]"};
    String[] msg00000005 = new String[]{"/[label()]", "Oh, Chief, are you going to present your report? Good luck!", "/[waitkey(64)]/[close()]"};
    String[] msg00000006 = new String[]{"/[label()]", "Don't let that nasty Commander get to you.", "/[waitkey(64)]/[close()]"};
    String[] msg00000007 = new String[]{"/[label()]", "Oh yeah. Watch out for those people in the hangar next door.", "/[waitkey(1)]/[clear()]", "They act like they're so superior! They even yelled at me for no reason!", "/[waitkey(64)]/[close()]"};
    String[] msg00000008 = new String[]{"/[label()]", "Oh, you didn't see the Assistant Chief? He walked by just a minute ago.", "/[waitkey(64)]/[close()]"};
    String[] msg00000009 = new String[]{"/[label()]", "Don't you think the Assistant Chief has a weak presence? I don't think I would notice him even if I passed him.", "/[waitkey(64)]/[close()]"};
    String[] msg00000010 = new String[]{"/[label()]", "Oh, how did your report go?", "/[waitkey(1)]/[clear()]", "From the look on your face, I guess they were pretty hard on you.", "/[waitkey(64)]/[close()]"};
    String[] msg00000011 = new String[]{"/[label()]", "Don't let it bother you too much. I don't think that the Commander is well liked, even in this unit here.", "/[waitkey(64)]/[close()]"};
    String[] msg00000012 = new String[]{"/[label(Shion)]", "Let's see, I take the elevator up to get to the bridge, I think...", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label(Shion)]", "Oh, I have an email!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL2 = new String[]{"/[label()]", "I'm pretty sure the one I have is the \"RIOS 680RR.\" I'll try a few things with it later.", "/[waitkey(64)]/[close()]"};

    ST0020() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.227f, -0.035f, -0.85f);
        this.camEV.setRotate(-0.023f, 0.0f, 0.0f);
        this.camEV.setFov(40.319f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.067f, 0.124f, 0.028f);
        this.camEV.setRotate(-0.643f, -1.119f, 0.0f);
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
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Stage.setVisible(40, false);
                this.monitor1.signal(1);
                this.monitor2.signal(1);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAP, 0);
                ST0020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00000012, 0);
                ST0020.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(20);
                this.cam0.setMode(0);
                Stage.setVisible(40, true);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 2: {
                if (Runtime.getFlags(13, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7056, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Runtime.mailArriveSet(0);
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                ST0020.waitPage(this.win, 64);
                System.sleep(30);
                Runtime.setFlags(7056, 1, 1);
                Runtime.mailExec(1);
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg01496400, 0);
                ST0020.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0161C79A, 0);
                ST0020.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg01624AFF, 0);
        ST0020.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc2_3(window);
        } else if (this.S012 == 1) {
            this.Talk_npc2_1(window);
        } else if (this.S07C == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg00000005, 0);
                ST0020.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00000006, 0);
                ST0020.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000007, 0);
        ST0020.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg00000008, 0);
                ST0020.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000009, 0);
        ST0020.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg00000010, 0);
                ST0020.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000011, 0);
        ST0020.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        this.npc4.kickEnepc(4, 1);
        window.print(this.msg00000001, 0);
        ST0020.waitPage(window, 64);
        this.npc4.kickEnepc(1, 9);
        window.print(this.msg00000002, 0);
        ST0020.waitPage(window, 64);
        this.npc4.kickEnepc(4, 0);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg00000003, 0);
        ST0020.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(10, 1);
                break;
            }
            case 1: {
                if (Runtime.getFlags(13, 1) == 0) {
                    Runtime.setFlags(13, 1, 1);
                    Runtime.jumpEvent(1071);
                    break;
                }
                Runtime.jumpCF(30, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -14.0f, 1.5f, -3.0f, 0.0f);
        this.teiten1.SetBgm(196611);
        this.teiten2 = new Uwamono(28690, -9.0f, 1.5f, -3.0f, 0.0f);
        this.teiten2.SetBgm(196611);
        this.teiten3 = new Uwamono(28690, -4.0f, 1.5f, -3.0f, 0.0f);
        this.teiten3.SetBgm(196611);
        this.teiten4 = new Uwamono(28690, 6.0f, -1.0f, 18.0f, 0.0f);
        this.teiten4.SetBgm(196611);
        this.teiten5 = new Uwamono(28690, 6.5f, -1.0f, 25.0f, 0.0f);
        this.teiten5.SetBgm(196611);
        this.teiten6 = new Uwamono(28690, 13.5f, -1.0f, 25.0f, 0.0f);
        this.teiten6.SetBgm(196611);
        this.teiten7 = new Uwamono(28690, 10.0f, 0.0f, -3.5f, 0.0f);
        this.teiten7.SetBgm(196612);
        this.teiten8 = new Uwamono(28690, 14.0f, -2.0f, 18.0f, 0.0f);
        this.teiten8.SetBgm(196612);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 10.15f, -0.3f, -3.35f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18002, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Object();
        this.monitor2.init(24613, 10.15f, -0.3f, -3.35f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18003, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 10.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 1.8f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 13.0f);
        this.doorA = new Uwamono(44, 40, '\u0001');
        new Uwamono(43, 40, '\u0001', this.doorA);
        this.doorB = new Uwamono(38, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
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
        this.S07B = Runtime.getFlags(13, 1);
        this.S07C = Runtime.getFlags(14, 1);
        this.S08 = Runtime.getFlags(15, 1);
        this.S012 = Runtime.getFlags(19, 1);
        this.S015B = Runtime.getFlags(23, 1);
        this.npcset_1();
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(521, 1, 0, 13, 7, 8.84f, 0.0f, -1.82f, 160.0f);
        this.npc2 = new NPC_NORMAL(517, 2, 0, 7, 4, -9.41f, 0.0f, -1.82f, 270.0f);
        this.npc3 = new NPC_NORMAL(524, 3, 0, 13, 4, 12.13f, 0.0f, 25.84f, 200.0f);
        this.npc4 = new NPC_NORMAL(522, 4, 0, 14, 7, 11.91f, 0.0f, 24.87f, 15.0f);
        this.npc1.setMotion(0, 10);
        this.npc3.setMotion(0, 9);
        this.npc4.setMotion(0, 10);
        this.npc1.disableDTKFlag(2);
        this.npc1.enableDTKFlag(4);
        this.npc3.disableDTKFlag(3);
        this.npc4.enableDTKFlag(262144);
        this.npc4.disableDTKFlag(2);
        this.npc4.enableDTKFlag(4);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_2() {
    }

    void npcset_3() {
    }

    void npcset_4() {
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    class Object
            extends Unit {
        Object() {
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
}

