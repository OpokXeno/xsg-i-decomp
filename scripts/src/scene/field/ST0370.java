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
import xeno.map.MC_VOK09B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0370
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK09B_PRJ {
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
    Effect fade;
    Effect E01;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] msg57653BC8 = new String[]{"Aaaah! Stay away! Go away!!", "/[waitkey(64)]/[close()]"};
    String[] msg57653BC9 = new String[]{"/[label()]", "I don't want to die yet! As the old adage goes, \"Slow and steady wins the race,\" right?!", "/[waitkey(64)]/[close()]"};

    ST0370() {
    }

    void Final_init(int n) {
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (this.npc1talked == 0) {
            window.print(this.msg57653BC8, 0);
            ST0370.waitPage(window, 64);
            this.npc1talked = 1;
            return;
        }
        if (this.npc1talked == 1) {
            window.print(this.msg57653BC9, 0);
            ST0370.waitPage(window, 64);
            this.npc1talked = 0;
            return;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(65887, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, 3.0f, 0.0f);
        this.teiten1.SetBgm(196627);
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
        this.Star = new Mapunits();
        this.Star.mapUnit(31);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
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
        this.npc1 = new NPC_NORMAL(526, 1, 0, 8, 3, 1.68f, 0.0f, 2.97f, 45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.disableDTKFlag(8);
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(1);
        this.npc1.disableDTKFlag(2);
        this.npc1.setMotion(0, 8);
        new Uwamono(28678, -2.0f, 0.0f, 0.0f);
        this.doorA = new Uwamono(9, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorA.DoorClose();
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
    }
}

