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
import xeno.map.MC_VOK07B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0351
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK07B_PRJ {
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
    Unit[] unit;
    Unit hole1;
    Unit hole2;
    Unit hole3;
    Unit hole4;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int vtalked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect EF09;
    Effect EF10;
    Effect EF11;
    Effect fade;
    Unit Con1;
    Unit Con2;
    Unit monitor1;
    Uwamono Tena1;
    Uwamono Tena2;
    Light light = new Light(0);
    int page;
    int B1flg = 0;
    String[] msg57ADD19E = new String[]{"He's dead.", "/[waitkey(64)]/[clear()]", "There is a key to the crew man's quarters. Would you like to take it?", "/[waitkey(64)]/[close()]"};
    String[] msg57ADD19F = new String[]{"He's dead.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D45 = new String[]{"The door is locked.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D46 = new String[]{"The door is now unlocked.", "/[waitkey(64)]/[close()]"};
    String[] Virgil_01 = new String[]{"/[label(Virgil)]", "You! Quit dawdling and get your worthless ass out of here!", "/[waitkey(64)]/[close()]"};
    String[] Virgil_02 = new String[]{"/[label(Virgil)]", "When civilians like you run around like idiots, you end up getting us into the mess!", "/[waitkey(64)]/[close()]"};

    ST0351() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.017f, 1.291f, 19.599f);
        this.camEV.setRotate(2.783f, -108.857f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            default:
        }
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3011, 1) != 0) {
                        this.doorC.SetDoorType('\u0004');
                        return;
                    }
                    if (this.B1flg == 1) {
                        return;
                    }
                    if (Runtime.getFlags(3005, 1) == 0) {
                        this.B1flg = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg57AD5D45, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.B1flg = 0;
                        return;
                    }
                    this.B1flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msg57AD5D46, 0);
                    System.waitFor(this.win);
                    this.doorC.SetDoorType('\u0004');
                    Runtime.setPlayerControl(true);
                    Runtime.setFlags(3011, 1, 1);
                    this.B1flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 != 5) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3018, 1) != 0) return;
                if (this.vtalked == 0) {
                    this.cam0.setMode(-1);
                    this.EV_Camera00();
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
                    Runtime.setPlayerControl(false);
                    this.npc7.kickEnepc(4, 1);
                    this.npc7.kickEnepc(0, 1);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Virgil_01, 0);
                    System.waitFor(this.win);
                    this.cam0.setMode(0);
                    this.vtalked = 1;
                    this.npc7.kickEnepc(4, 0);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    return;
                }
                this.cam0.setMode(-1);
                this.EV_Camera00();
                Runtime.enable(65536);
                this.player.mtn(28, 1, 1.0f, true);
                this.player.getTranslate();
                this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
                Runtime.setPlayerControl(false);
                this.npc7.kickEnepc(4, 1);
                this.npc7.kickEnepc(0, 1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Virgil_02, 0);
                System.waitFor(this.win);
                this.cam0.setMode(0);
                this.vtalked = 0;
                this.npc7.kickEnepc(4, 0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getFlags(3005, 1) == 0) {
            window.print(this.msg57ADD19E, 0);
            System.waitFor(window);
            this.menu = Menu.create();
            this.menu.addItem("Yes\nNo");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.EF07.disp(false);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 6);
                    Runtime.setFlags(3005, 1, 1);
                    return;
                }
                case 1: {
                    return;
                }
            }
            return;
        }
        if (Runtime.getFlags(3005, 1) == 1) {
            window.print(this.msg57ADD19F, 0);
            ST0351.waitPage(window, 64);
            return;
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(65918, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(65896, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(65906, 1);
                break;
            }
            case 3: {
                if (Runtime.getFlags(36, 1) == 0) {
                    Runtime.setFlags(36, 1, 1);
                    Runtime.setFlags(3018, 1, 1);
                    Runtime.jumpEvent(1280);
                    break;
                }
                Runtime.jumpCF(65936, 1);
                break;
            }
        }
    }

    void init() {
        int n;
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.npc1 = new NPC_NORMAL(526, 1, 0, 1, 3, -10.2f, 0.0f, 26.2f, -45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.disableDTKFlag(131083);
        this.npc1.setMotion(0, 3);
        this.npc2 = new NPC_NORMAL(8449, 2, 0, 2, 4, -6.385884f, 0.0f, -9.128831f, 315.0f);
        this.npc2.setMotion(0, 1);
        this.npc2.dispRadar(false);
        this.npc6 = new NPC_NORMAL(8449, 6, 0, 3, 4, -12.218013f, 0.0f, 14.151712f, 215.0f);
        this.npc6.setMotion(0, 2);
        this.npc6.dispRadar(false);
        if (Runtime.getFlags(3018, 1) == 0) {
            this.npc7 = new Enepc();
            this.npc7.init(8450, 7, -8.28f, 0.0f, 19.873f, 225.0f);
            this.npc7.id = 7;
            this.npc7.setParams(0, 6, 7, 23);
            this.npc7.setInvalidID(1);
            this.npc7.setShadow(0, 0);
            this.npc8 = new Enepc();
            this.npc8.init(20225, 9, -10.146f, 0.0f, 13.61f, 315.0f);
            this.npc8.id = 8;
            this.npc8.setParams(0, 6, 8, 9);
            this.npc8.setMotion(0, 1);
            this.npc8.setTP(300);
            this.npc9 = new Enepc();
            this.npc9.init(16385, 11, -8.692f, 0.0f, 3.344f, 0.0f);
            this.npc9.id = 9;
            this.npc9.setParams(0, 7, 9, 11);
            this.npc9.setTP(300);
        }
        Stage.setVisible(-1, true);
        this.EF01 = new Effect(1401, -6.52f, -0.5f, -9.21f, 0.0f);
        this.EF01.disp(true);
        this.EF01.setClip(true);
        this.EF05 = new Effect(1402, -12.21f, 0.0f, 14.15f, 0.0f);
        this.EF05.disp(true);
        this.EF05.setClip(true);
        this.EF06 = new Effect(1404, -10.2f, 0.0f, 26.2f, -45.0f);
        this.EF06.disp(true);
        this.EF06.setClip(true);
        if (Runtime.getFlags(3005, 1) == 0) {
            this.EF07 = new Effect(1018, -10.172f, 0.3f, 26.274f, 0.0f);
            this.EF07.disp(true);
            this.EF07.setClip(true);
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.hole1 = new Obj();
        this.hole1.init(24613, -7.17f, 1.018f, -14.9f, 0.0f);
        this.hole1.setArgs(0, 0.0f, 0.5f, 1.0f, 0.75f);
        this.hole1.setArgs(1, 19901, 0, 0, 0);
        this.hole1.setArgs(2, 128, 0, 11, 1);
        this.hole1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hole1.setRotate(0.0f, 0.0f, 24.0f);
        this.hole1.setScale(1.25f, 1.25f, 0.0f);
        this.hole1.signal(1);
        this.hole2 = new Obj();
        this.hole2.init(24613, -7.486f, 2.214f, -14.9f, 0.0f);
        this.hole2.setArgs(0, 0.0f, 0.5f, 1.0f, 0.75f);
        this.hole2.setArgs(1, 19901, 0, 0, 0);
        this.hole2.setArgs(2, 128, 0, 11, 1);
        this.hole2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hole2.setRotate(0.0f, 0.0f, 125.0f);
        this.hole2.signal(1);
        this.hole3 = new Obj();
        this.hole3.init(24613, -9.124f, 1.362f, -14.9f, 0.0f);
        this.hole3.setArgs(0, 0.0f, 0.5f, 1.0f, 0.75f);
        this.hole3.setArgs(1, 19901, 0, 0, 0);
        this.hole3.setArgs(2, 128, 0, 11, 1);
        this.hole3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hole3.setRotate(0.0f, 0.0f, 124.0f);
        this.hole3.signal(1);
        this.hole4 = new Obj();
        this.hole4.init(24613, -8.84f, 1.128f, -14.9f, 0.0f);
        this.hole4.setArgs(0, 0.0f, 0.5f, 1.0f, 0.75f);
        this.hole4.setArgs(1, 19901, 0, 0, 0);
        this.hole4.setArgs(2, 128, 0, 11, 1);
        this.hole4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.hole4.setRotate(0.0f, 0.0f, 318.0f);
        this.hole4.setScale(1.54f, 1.54f, 0.0f);
        this.hole4.signal(1);
        if (Runtime.getFlags(3018, 1) == 0) {
            Runtime.setDefocusQuick(0, 1, 12345, 1);
            Runtime.setDefocusQuick(1, 1, 23456, 1);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.cam0.setFog(1, 12.0f, 18.0f, 0.0f, 0.5f, 128, 128, 128, 0);
            this.cam0.setFog(2, 13.0f, 18.0f, 0.0f, 0.5f, 128, 128, 128, 0);
            this.cam0.setFog(3, 13.0f, 18.0f, 0.0f, 0.5f, 128, 128, 128, 0);
            this.cam0.setFog(4, 13.0f, 18.0f, 0.0f, 0.5f, 128, 128, 128, 0);
            this.cam0.setFog(5, 12.0f, 18.0f, 0.0f, 0.5f, 128, 128, 128, 0);
        } else {
            this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
            this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
            this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
            this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
            this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        }
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -9.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.8f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestal(4, -18.872f, 4.022f, 16.727f, 65.0f, -44.62f, -21.666f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(4, 1);
        this.cam0.setCFAngle(5, -28.0f, 335.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(6, false);
        Stage.setVisible(7, false);
        Stage.setVisible(8, false);
        Stage.setVisible(9, false);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.doorA = new Uwamono(81, 42, '\u0001');
        new Uwamono(82, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.DoorClose();
        this.doorB = new Uwamono(83, 42, '\u0001');
        new Uwamono(84, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorB.DoorClose();
        this.doorC = new Uwamono(38, 40, '\u0001');
        this.doorC.SetDoorType('\u0002');
        this.doorC.DoorClose();
        this.doorD = new Uwamono(40, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorD.DoorClose();
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.Con1 = new Obj();
            this.Con1.init(20543, -7.43f, 0.0f, 10.97f, 28.0f);
            this.Tena1 = new Uwamono(28672, -7.43f, 0.0f, 10.97f, 28.0f);
            this.Tena1.SetSize(1.7f, 0.5f, 1.0f);
            this.Con2 = new Obj();
            this.Con2.init(20543, -10.68f, 0.0f, 9.79f, 135.0f);
            this.Tena2 = new Uwamono(28672, -10.68f, 0.0f, 9.79f, 135.0f);
            this.Tena2.SetSize(1.7f, 0.5f, 1.0f);
        }
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    class Obj
            extends Unit {
        Obj() {
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
    }

    class Object
            extends Unit {
        Object() {
        }
    }
}

