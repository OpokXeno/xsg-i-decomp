import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK13B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0390
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK13B_PRJ {
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
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    Uwamono doorA;
    Uwamono col1;
    Uwamono shopA;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono Base1;
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
    Effect fade;
    Light light = new Light(0);
    int page;
    String[] Npc_1_0 = new String[]{"What? The Connection Gear's Vaporizer Plug-in?", "/[waitkey(1)]/[clear()]", "A wrecked vehicle is blocking the corridor? Here! Take it!", "/[waitkey(64)]/[close()]"};
    String[] Npc_1_1 = new String[]{"Don't mind me! Hurry up and get out of here!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_1_2 = new String[]{"It's crawling with Gnosis up ahead, too!! Do you need any emergency provisions?", "/[waitkey(64)]/[close()]"};
    String[] Npc_1_3 = new String[]{"You're still wandering around here?!", "/[waitkey(64)]/[close()]"};
    String[] SHITAI = new String[]{"He's dead.", "/[waitkey(64)]/[close()]"};
    String[] T_01 = new String[]{"Obtained [Vaporizer Plug-in] for the Connection Gear.", "/[waitkey(64)]/[close()]"};
    String[] T_02 = new String[]{"You know all about destroying things in your way using this, right?!", "/[waitkey(64)]/[close()]"};
    String[] T_03 = new String[]{"You already know about destroying objects on the map with the □ Button, right?! When the blue cursor appears over an object, it means you can break it!", "/[waitkey(1)]/[clear()]", "If there are several objects to destroy, a dark blue cursor will appear.", "/[waitkey(1)]/[clear()]", "Use the R1/L1 Buttons to choose which one to break first!", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_0 = new String[]{"...Hey, what the heck are they? With one blow, they wrecked a container that the Vaporizer Plug-in couldn't even destroy!", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_1 = new String[]{"But you know, I just saw them caught in the electromagnetic net.", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_2 = new String[]{"Just leave me alone. There isn't a single comrade left to fight with me now.", "/[waitkey(64)]/[close()]"};

    ST0390() {
    }

    void Final_init(int n) {
    }

    public void TalkNPC(Enepc enepc, Window window) {
        window.print(this.SHITAI, 0);
        ST0390.waitPage(window, 64);
    }

    public void TalkNPC1(Enepc enepc) {
        if (Runtime.getFlags(3001, 2) == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_1_3, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
        } else if (Runtime.getFlags(3001, 2) == 1) {
            if (this.npc1talked == 0) {
                Runtime.setPlayerControl(false);
                Runtime.disable(524288);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Npc_1_0, 0);
                System.waitFor(this.win);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.T_02, 0);
                System.waitFor(this.win);
                Runtime.addItem(10, 7);
                Runtime.setFlags(3001, 2, 2);
                System.sleep(60);
                this.menu = Menu.create();
                this.menu.addItem("Umm...I forgot\nOf course I remember");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.T_03, 0);
                        System.waitFor(this.win);
                        this.npc1talked = 1;
                        Runtime.setShootFlag(true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Npc_1_2, 0);
                        System.waitFor(this.win);
                        System.sleep(30);
                        Runtime.enterShop(1);
                        Runtime.setShootFlag(true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Npc_1_1, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        Runtime.enable(524288);
                        break;
                    }
                    default: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Npc_1_2, 0);
                        System.waitFor(this.win);
                        System.sleep(30);
                        Runtime.enterShop(1);
                        Runtime.setShootFlag(true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Npc_1_1, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        Runtime.enable(524288);
                        this.npc1talked = 1;
                        break;
                    }
                }
            } else {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Npc_1_2, 0);
                System.waitFor(this.win);
                System.sleep(30);
                Runtime.enterShop(1);
                Runtime.setShootFlag(true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Npc_1_1, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                Runtime.enable(524288);
            }
        } else if (Runtime.getFlags(3001, 2) == 2) {
            Runtime.setPlayerControl(false);
            Runtime.disable(524288);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_1_2, 0);
            System.waitFor(this.win);
            System.sleep(30);
            Runtime.enterShop(1);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_1_1, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        this.npc4.enableDTKFlag(8);
        if (this.npc3talked == 0) {
            window.print(this.Dennoji_0, 0);
            ST0390.waitPage(window, 64);
            this.npc3talked = 1;
            return;
        }
        if (this.npc3talked == 1) {
            window.print(this.Dennoji_1, 0);
            ST0390.waitPage(window, 64);
            this.npc3talked = 2;
            return;
        }
        if (this.npc3talked == 2) {
            window.print(this.Dennoji_2, 0);
            ST0390.waitPage(window, 64);
            this.npc3talked = 0;
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
                if (Runtime.getFlags(42, 1) == 0) {
                    Runtime.jumpCF(65917, 1);
                    break;
                }
                Runtime.jumpCF(65918, 1);
                break;
            }
        }
    }

    void init() {
        this.light.setColor(0, 0.33f, 0.33f, 0.33f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, -0.007f, 1.0f, 0.029f);
        this.light.setColor(2, 0.32f, 0.32f, 0.32f);
        this.light.setDirection2(2, 0.024f, 0.861f, 0.509f);
        this.light.setColor(3, 0.08f, 0.08f, 0.08f);
        this.light.setDirection2(3, 0.064f, -0.996f, 0.069f);
        this.EF02 = new Effect(1527, -4.635f, -0.987f, -2.909f, 12.0f);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1527, 4.361f, -0.987f, -10.663f, 268.0f);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        this.EF05 = new Effect(1013, -3.032f, 2.632f, 5.271f, 0.0f);
        this.EF05.disp(true);
        this.EF05.setClip(true);
        this.EF06 = new Effect(1013, -3.032f, 2.632f, -1.271f, 0.0f);
        this.EF06.disp(true);
        this.EF06.setClip(true);
        this.EF07 = new Effect(1013, -3.032f, 2.632f, -7.271f, 0.0f);
        this.EF07.disp(true);
        this.EF07.setClip(true);
        this.EF08 = new Effect(1013, 3.032f, 2.632f, 6.729f, 0.0f);
        this.EF08.disp(true);
        this.EF08.setClip(true);
        this.EF09 = new Effect(1013, 3.032f, 2.632f, 0.729f, 0.0f);
        this.EF09.disp(true);
        this.EF09.setClip(true);
        this.EF10 = new Effect(1013, 3.032f, 2.632f, -6.271f, 0.0f);
        this.EF10.disp(true);
        this.EF10.setClip(true);
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
        Stage.setVisible(20, false);
        Stage.setVisible(6, false);
        Stage.setVisible(10, false);
        Stage.setVisible(11, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(527, 11, 0, 6, 3, 7.452f, 0.0f, 12.28f, -45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.enableDTKFlag(65536);
        this.npc1.disableDTKFlag(2);
        this.npc1.enableDTKFlag(4);
        this.npc2 = new NPC_NORMAL(527, 12, 0, 5, 4, -4.635f, -0.987f, -2.909f, 12.0f);
        this.npc2.talkto("TalkNPC");
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(2);
        this.npc2.enableDTKFlag(8);
        this.npc2.disableDTKFlag(1);
        this.npc2.setMotion(0, 1);
        this.npc3 = new NPC_NORMAL(527, 13, 0, 5, 4, 4.361f, -0.987f, -10.663f, 268.0f);
        this.npc3.talkto("TalkNPC");
        this.npc3.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(2);
        this.npc3.enableDTKFlag(8);
        this.npc3.disableDTKFlag(1);
        this.npc3.setMotion(0, 2);
        this.npc4 = new NPC_NORMAL(527, 14, 0, 5, 12, -7.552f, 0.0f, 12.093f, 45.0f);
        this.npc4.talkto("TalkNPC3");
        this.npc4.disableDTKFlag(8);
        this.npc4.disableDTKFlag(131072);
        this.npc4.disableDTKFlag(1);
        this.npc4.disableDTKFlag(2);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 6);
        this.col1 = new Uwamono(28672, -4.0f, 0.0f, 15.6f, 0.0f);
        this.doorA = new Uwamono(151, 42, '\u0001');
        new Uwamono(150, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.Base1 = new Uwamono(28672, 0.0f, -2.117f, -15.5f, 0.0f);
        this.Base1.SetSize(8.0f, 1.0f, 10.0f);
        this.item1 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 23);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 24);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 25);
        new Uwamono(19, 33, this.item1);
        new Uwamono(61, 33, this.item2);
        new Uwamono(64, 31, this.item3);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
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

