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

class ST0399
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

    ST0399() {
    }

    void Final_init(int n) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("【環境シミュレータ】・VOK10へjump！！");
                Runtime.jumpCF(389, 1);
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

