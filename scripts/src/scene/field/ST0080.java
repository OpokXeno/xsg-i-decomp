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
import xeno.map.MC_VOK08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0080
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK08_PRJ {
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
    Unit Star;
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
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    Uwamono doorA;
    Uwamono teiten1;
    Effect fade;
    int eveflag_1;
    int eveflag_2;
    int S014A;
    int S014B;
    int S015B;
    Light light = new Light(0);
    int page;

    ST0080() {
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

    public void Talk_npc1(Enepc enepc, Window window) {
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
    }

    void Talk_npc1_2(Window window) {
    }

    void Talk_npc1_3(Window window) {
    }

    void Talk_npc1_4(Window window) {
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(70, 2);
                break;
            }
        }
    }

    void init() {
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, -3.0f, 0.0f);
        this.teiten1.SetBgm(196626);
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, -3.0f, 0.0f);
        this.teiten1.SetBgm(196626);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.Star = new Mapunits();
        this.Star.mapUnit(30);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
        Stage.setVisible(2, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.doorA = new Uwamono(10, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
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
    }

    void npcset_1() {
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
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

