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
import xeno.map.MC_VOK05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0050
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK05_PRJ {
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
    Enepc npc20;
    Enepc npc21;
    Unit unit1;
    Unit Star;
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
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    Uwamono itembox1;
    Uwamono itembox2;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Effect EF01;
    int SHION_TALK_1;
    Uwamono doorA;
    Uwamono teiten1;
    Light light = new Light(0);
    int page;
    String[] msgTEST1 = new String[]{"/[label()]", "The current items are temporary. When you take an item, you're should get an email from Miyuki.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label(Shion)]", "Oh! I got an email from Miyuki. I wonder what it is?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL2 = new String[]{"/[label()]", "You have no new email.\n", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL3 = new String[]{"/[label()]", "You have already received this email.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL4 = new String[]{"/[label()]", "You can't see it!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL5 = new String[]{"/[label()]", "Too bad...", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL6 = new String[]{"/[label(Shion)]", "A magnificent plan...? What in the world is that girl planning?", "/[waitkey(64)]/[close()]"};
    String[] msgNeru = new String[]{"/[label(Shion)]", "I'm a little tired.", "/[waitkey(1)]/[clear()]", "I feel bad about doing this, but I think I'll rest a bit...", "/[waitkey(64)]/[close()]"};
    String[] msgNeru2 = new String[]{"/[label()]", "Good night.", "/[waitkey(64)]/[close()]"};
    String[] msgNeru3 = new String[]{"/[label(Shion)]", "That's right, before I go to sleep, I should check on everyone.", "/[waitkey(64)]/[close()]"};
    String[] msgNO_MWS = new String[]{"/[label(Shion)]", "Oh, no!\n", "I have a package waiting.", "/[waitkey(1)]/[clear()]", "I have to hurry to the A.G.W.S. hangar to get it!", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Would you like to rest?", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};

    ST0050() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.781f, 2.806f, -1.938f);
        this.camEV.setRotate(-63.275f, -24.059f, 0.0f);
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
                if (Runtime.getFlags(26, 1) == 1) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.disable(524288);
                            this.fade1.call(0);
                            System.sleep(60);
                            Runtime.charAllRecovery();
                            this.fade2.call(0);
                            System.sleep(60);
                            Sound.effectPlay(26);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SYS_01, 0);
                            System.waitFor(this.win);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            break;
                        }
                        default: {
                            Runtime.setPlayerControl(true);
                            break;
                        }
                    }
                } else {
                    if (Runtime.getFlags(25, 1) == 1) {
                        if (this.BUTTON_F == 1) {
                            return;
                        }
                        this.BUTTON_F = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgNeru, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Maybe I'll take a catnap\nI can't yet...I'm worried about everyone");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                Runtime.charAllRecovery();
                                System.println("*********全回復しました**************");
                                this.fade.call(0);
                                System.sleep(30);
                                Runtime.setPlayerControl(true);
                                this.BUTTON_F = 0;
                                Runtime.setFlags(26, 1, 1);
                                Runtime.jumpEvent(1181);
                                break block0;
                            }
                        }
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgNeru3, 0);
                        ST0050.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        break;
                    }
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.disable(524288);
                            this.fade1.call(0);
                            System.sleep(60);
                            Runtime.charAllRecovery();
                            this.fade2.call(0);
                            System.sleep(60);
                            Sound.effectPlay(26);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SYS_01, 0);
                            System.waitFor(this.win);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(7045, 1) == 0) {
                    if (Runtime.checkItem(10, 8) == 1) {
                        Runtime.setPlayerControl(false);
                        Runtime.mailArriveSet(1);
                        System.sleep(10);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        ST0050.waitPage(this.win, 64);
                        System.sleep(30);
                        Runtime.setFlags(7045, 1, 1);
                        Runtime.mailExec(1);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgMAIL6, 0);
                        ST0050.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    this.BUTTON_F = 0;
                    break;
                }
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Talk_npc1_1();
    }

    void Talk_npc1_1() {
        if (Runtime.checkItem(10, 8) == 0) {
            this.npc20.disableDTKFlag(131072);
            this.npc20.disableDTKFlag(65536);
            Runtime.enable(65536);
            this.cam0.setMode(-1);
            this.EV_Camera01();
            this.player.setTranslate(-1.33f, 0.0f, -2.54f);
            this.player.setRotate(0.0f, 180.0f, 0.0f);
            System.sleep(30);
            this.player.mtn(26, 1, 1.0f, true);
            System.sleep(60);
            Stage.setVisible(44, false);
            this.EF01.disp(false);
            System.sleep(30);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 8);
            this.cam0.setMode(0);
            Runtime.disable(65536);
            return;
        }
    }

    void Talk_npc1_2() {
    }

    void Talk_npc1_3() {
    }

    void Talk_npc1_4() {
    }

    public void Touch_npc1(Enepc enepc) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                this.doorA.DoorOpen();
                Runtime.jumpCF(40, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -3.0f, 0.0f, 3.0f, 0.0f);
        this.teiten1.SetBgm(196620);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        Stage.setVisible(1, false);
        this.Star = new Mapunits();
        this.Star.mapUnit(43);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        new Uwamono(28678, 2.0f, 0.0f, 0.0f);
        if (Runtime.checkItem(10, 8) == 1) {
            Stage.setVisible(44, false);
        }
        this.doorA = new Uwamono(10, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        if (Runtime.checkItem(10, 8) == 0) {
            this.EF01 = new Effect(1018, 0);
            this.EF01.disp(true);
        }
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
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.SHION_TALK_1 = Runtime.getFlags(7006, 1);
        this.npc20 = new NPC_NORMAL(1, 20, 0, 14, 3, -1.94f, 0.0f, -3.1f, 0.0f);
        if (Runtime.checkItem(10, 8) == 1) {
            this.npc20.disableDTKFlag(131072);
            this.npc20.disableDTKFlag(65536);
            this.npc20.setInvalidID(1);
            this.npc20.setVisible(false);
        } else {
            this.npc20.setInvalidID(1);
            this.npc20.setVisible(false);
            this.npc20.talkto("Talk_npc1");
        }
        if (Runtime.getFlags(7012, 1) != 1 && Runtime.getFlags(23, 1) == 1) {
            this.npc21 = new NPC_NORMAL(1, 21, 0, 14, 3, 100.0f, 100.0f, 100.0f, 0.0f);
            this.npc21.talkto("TalkNPC21");
            this.npc21.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(8);
            this.npc21.start(1, "mws");
        }
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
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

        void mws() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST0050.this.player.mtn(11, 1, 1.0f, true);
            ST0050.this.win = Window.create();
            ST0050.this.win.setSize(4, 45);
            ST0050.this.win.setLocation(15, 305);
            ST0050.this.win.print(ST0050.this.msgNO_MWS, 0);
            ST0050.waitPage(ST0050.this.win, 64);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

