import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK08B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0369
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK08B_PRJ {
    Player player;
    Camera cam1;
    Enepc npc1;
    Unit unit1;
    Unit Star;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    Uwamono doorA;
    Uwamono itembox;
    Effect fade;
    Effect E01;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;

    ST0369() {
    }

    void Final_init(int n) {
    }

    public void TalkNPC1(Enepc enepc) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("【Environmental Simulator】・ Jump to VOK07!!");
                Runtime.jumpCF(359, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, -3.0f, 0.0f);
        this.teiten1.SetBgm(196626);
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
        Stage.setVisible(-1, true);
        this.Star = new Mapunits();
        this.Star.mapUnit(31);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        Stage.setVisible(1, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.E01 = new Effect(1437, -3.0f, 1.45f, 0.0f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.setRotate(0.0f, 90.0f, 0.0f);
        this.E01.setScale(1.0f, 1.25f, 1.0f);
        this.E01.noAttach(false);
        this.doorA = new Uwamono(9, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorA.DoorClose();
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, -3.0f, 180.0f, 32);
        this.itembox.SetSymbol(28685);
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
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }
}

