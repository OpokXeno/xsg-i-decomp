import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KOU07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1270
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KOU07_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc npc15;
    Enepc npc16;
    Enepc npc17;
    Enepc npc18;
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
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int talkFlag14;
    int talkFlag15;
    int talkFlag16;
    int talkFlag17;
    int talkFlag18;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag6;
    int S2040C;
    int S2042;
    int S3;
    Light light = new Light(0);
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono itembox;
    Effect fade;
    int page;

    ST1270() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                return;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1250, 2);
                break;
            }
        }
    }

    void init() {
        this.S2040C = Runtime.getFlags(158, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S3 = Runtime.getFlags(179, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFLockX(1, -7.5f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.itembox = new Uwamono(28680, -7.5f, -3.0f, -10.01f, 180.0f, 453);
        this.itembox.SetSymbol(28724);
        this.itembox.SetCallNo(1);
        this.doorA = new Uwamono(1, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3242, 1, 1);
                break;
            }
        }
    }

    void npcset_1() {
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
    }
}

