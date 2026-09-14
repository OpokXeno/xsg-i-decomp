import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK06B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0340
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK06B_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    Camera cam1;
    int entrance;
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
    Unit[] unit;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    int npc9talked = 0;
    int npc10talked = 0;
    int npc11talked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Effect fade;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Light light = new Light(0);
    int page;
    String[] msg57ADD19F = new String[]{"Dead...", "/[waitkey(64)]/[close()]"};

    ST0340() {
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
                Runtime.jumpCF(409, 2);
                System.println("【環境シミュレータ】・VOK14へjump！！");
                break;
            }
            case 1: {
                Runtime.jumpCF(329, 3);
                System.println("【環境シミュレータ】・VOK04へjump！！");
                break;
            }
        }
    }

    void init() {
        this.light.setColor(0, 0.5f, 0.5f, 0.5f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 0.834f, 0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -0.872f, -0.49f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 1, 1.0f, 0.45f, 0.0f);
        Runtime.setIdLightCol(1, 2, 1.0f, 0.6f, 0.0f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.0f);
        Stage.setVisible(-1, true);
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, 16.03f, 8.29f, 7.392f, 40.0f, -72.072f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFPedestal(3, 0.03f, 11.074f, -15.613f, 40.0f, -87.891f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFAngle(4, -28.0f, 25.0f, 0.0f, 11.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        Stage.setVisible(115, false);
        Stage.setVisible(9, false);
        Stage.setVisible(8, false);
        this.doorA = new Uwamono(68, 42, '\u0001');
        new Uwamono(69, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.DoorClose();
        this.doorB = new Uwamono(70, 42, '\u0001');
        new Uwamono(71, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorB.DoorClose();
        this.doorC = new Uwamono(7, 42, '\u0001');
        new Uwamono(4, 42, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(6, 42, '\u0001');
        new Uwamono(5, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(3, 42, '\u0001');
        new Uwamono(1, 42, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0004');
        this.doorF = new Uwamono(2, 42, '\u0001');
        new Uwamono(0, 42, '\u0001', this.doorF);
        this.doorF.SetDoorType('\u0004');
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

