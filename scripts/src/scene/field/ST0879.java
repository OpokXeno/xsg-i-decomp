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
import xeno.map.MC_PRO07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0870
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO07_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
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
    Enepc ope_1;
    Enepc ope_2;
    Enepc ope_3;
    Enepc ope_4;
    Enepc enemy1;
    Effect fade;
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
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono screenA;
    Unit monitor1;
    Unit monitor2;
    Unit monitor3;
    Unit utic_fm;
    Unit utic_fm2;
    Unit utic_fm3;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;

    ST0870() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.setFlags(8003, 1, 1);
                Runtime.jumpCF(829, 3);
                break;
            }
            case 1: {
                Runtime.jumpCF(819, 4);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -6.355f, 0.0f, -5.89f, 0.0f);
        this.teiten1.SetBgm(196625);
        this.teiten2 = new Uwamono(28690, -6.355f, 0.0f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196625);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -6.349f, 0.0f);
        this.teiten3.SetBgm(196625);
        this.teiten4 = new Uwamono(28690, 0.0f, 0.0f, 3.86f, 0.0f);
        this.teiten4.SetBgm(196625);
        this.teiten5 = new Uwamono(28690, 6.121f, 0.0f, -3.0f, 0.0f);
        this.teiten5.SetBgm(196625);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, -0.5f, 1.0f, -0.35f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(1, 1, -0.5f, 1.0f, -0.35f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, -0.5f, 1.0f, -0.35f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(122, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        Stage.setVisible(125, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        this.doorB = new Uwamono(75, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorA = new Uwamono(123, 42, '\u0001');
        new Uwamono(124, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.utic_fm = new Unit();
        this.utic_fm.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm.setArgs(1, 20056, 0, 128, 112);
        this.utic_fm.setArgs(2, 52, 0, 15, -1);
        this.utic_fm.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm.signal(1);
        this.utic_fm.setScale(1.32f, 1.32f, 1.32f);
        this.utic_fm.setTranslate(0.53f, 2.03f, -3.03f);
        this.utic_fm.setRotate(0.0f, -90.0f, 0.0f);
        this.utic_fm2 = new Unit();
        this.utic_fm2.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm2.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm2.setArgs(1, 20051, 0, 128, 112);
        this.utic_fm2.setArgs(2, 52, 0, 15, -1);
        this.utic_fm2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm2.signal(1);
        this.utic_fm2.setScale(0.72f, 0.72f, 0.72f);
        this.utic_fm2.setTranslate(0.5f, 1.94f, 0.81f);
        this.utic_fm2.setRotate(-0.05f, 0.0f, 0.0f);
        this.utic_fm3 = new Unit();
        this.utic_fm3.init(24613, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm3.setArgs(0, 0.0f, 0.0f, 1.6f, 1.4f);
        this.utic_fm3.setArgs(1, 20047, 0, 128, 112);
        this.utic_fm3.setArgs(2, 52, 0, 15, -1);
        this.utic_fm3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.utic_fm3.signal(1);
        this.utic_fm3.setScale(0.72f, 0.72f, 0.72f);
        this.utic_fm3.setTranslate(0.5f, 1.94f, -6.79f);
        this.utic_fm3.setRotate(-0.05f, 0.0f, 0.0f);
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
        NPC_NORMAL() {
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }

        public void talk() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }
    }
}

