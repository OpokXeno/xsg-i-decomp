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
import xeno.map.MC_KAS06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2450
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS06_PRJ {
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
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    MAPUnit mapunit;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect light10;
    Effect light11;
    Effect light12;
    Effect light13;
    Effect light14;
    Effect light15;
    Effect light16;
    Effect light17;
    Effect light18;
    Effect light19;
    Effect light20;
    Effect light21;
    Effect light22;
    Effect light23;
    Effect light24;
    Effect light25;
    Effect light26;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] npc1_1 = new String[]{"/[label(Febronia)]", "Now, let us go.", "/[waitkey(64)]/[close()]"};
    String[] npc2_1 = new String[]{"/[label(Nephilim)]", "From the moment you open that door, you will have to face yourselves. It will be very difficult and sad...but it is important for you, as well as for us.", "/[waitkey(64)]/[close()]"};
    String[] open = new String[]{"It has been unlocked.", "/[waitkey(64)]/[close()]"};

    ST2450() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.lo == 1) {
            return;
        }
        switch (n2) {
            case 0: {
                if (Runtime.getFlags(8077, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                Runtime.setFlags(8077, 1, 1);
                this.nwin(this.open);
                this.doorB.SetDoorType('\u0004');
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void TalkNPC1(Enepc enepc) {
        this.nwin(this.npc1_1);
    }

    void TalkNPC2(Enepc enepc) {
        this.nwin(this.npc2_1);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("教会外観・２");
                Runtime.jumpCF(2419, 2);
                break;
            }
            case 1: {
                System.println("教会外観・３");
                Runtime.jumpCF(2419, 3);
                break;
            }
            case 2: {
                if (Runtime.getFlags(337, 1) == 1) {
                    if (Runtime.getFlags(338, 1) == 0) {
                        Runtime.setFlags(338, 1, 1);
                        System.println("イベントEV03026A");
                        Runtime.jumpEvent(3260);
                        break;
                    }
                    Runtime.jumpCF(2559, 1);
                    break;
                }
                Runtime.jumpCF(2559, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        Stage.setColor(0.825f, 0.8f, 0.75f);
        this.light.setColor(0, 0.175f, 0.175f, 0.175f);
        this.light.setColor(1, 0.7f, 0.7f, 0.7f);
        this.light.setDirection2(1, 0.833f, 0.292f, -0.47f);
        this.light.setColor(2, 0.28f, 0.28f, 0.28f);
        this.light.setDirection2(2, 0.347f, -0.444f, 0.826f);
        this.light.setColor(3, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(3, -0.959f, 0.028f, -0.282f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.7f, 0.7f, 0.7f);
        Runtime.setIdLightCol(1, 2, 0.28f, 0.28f, 0.28f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.833f, 0.292f, -0.47f);
        Runtime.setIdLightVec(1, 2, 0.347f, -0.444f, 0.826f);
        Runtime.setIdLightVec(1, 3, -0.959f, 0.028f, -0.282f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 12.0f, 50.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 12.5f, 0.0f, 12.0f, 50.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, -12.5f, 0.0f, 12.0f, 50.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFPedestal(4, 3.0167441f, 8.435723f, 2.684519f, 39.039787f, -23.524385f, 16.359806f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFPedestal(5, 1.8205006f, 8.403646f, -2.7248304f, 39.039783f, -46.74342f, -30.419678f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        float f = 5.3f;
        float f2 = 7.6f;
        this.light01 = new Effect(1758, f2, f, 7.5f, 0.0f);
        this.light02 = new Effect(1758, f2, f, 3.5f, 0.0f);
        this.light03 = new Effect(1758, f2, f, -0.5f, 0.0f);
        this.light04 = new Effect(1758, f2, f, -4.5f, 0.0f);
        this.light06 = new Effect(1758, -f2, f, 7.5f, 0.0f);
        this.light06.setRotate(0.0f, 180.0f, 0.0f);
        this.light07 = new Effect(1758, -f2, f, 3.5f, 0.0f);
        this.light07.setRotate(0.0f, 180.0f, 0.0f);
        this.light08 = new Effect(1758, -f2, f, -0.5f, 0.0f);
        this.light08.setRotate(0.0f, 180.0f, 0.0f);
        this.light09 = new Effect(1758, -f2, f, -4.5f, 0.0f);
        this.light09.setRotate(0.0f, 180.0f, 0.0f);
        this.light05 = new Effect(1621, 0.3f, 2.4f, -6.0f, 0.0f);
        this.light14 = new Effect(1405, 2.55f, 6.07f, -10.12f, 0.0f);
        this.light15 = new Effect(1405, -2.55f, 6.07f, -10.12f, 0.0f);
        this.light15 = new Effect(1405, -6.14f, 5.21f, -9.66f, 0.0f);
        this.light16 = new Effect(1405, -6.14f, 5.21f, -6.45f, 0.0f);
        this.light17 = new Effect(1405, -6.14f, 5.21f, -2.55f, 0.0f);
        this.light18 = new Effect(1405, -6.14f, 5.21f, 1.48f, 0.0f);
        this.light21 = new Effect(1405, 6.14f, 5.21f, -9.66f, 0.0f);
        this.light22 = new Effect(1405, 6.14f, 5.21f, -6.45f, 0.0f);
        this.light23 = new Effect(1405, 6.14f, 5.21f, -2.55f, 0.0f);
        this.light24 = new Effect(1405, 6.14f, 5.21f, 1.48f, 0.0f);
        Runtime.progressEffect(30);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(0, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(79, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        if (Runtime.getFlags(8077, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
        }
        this.doorC = new Uwamono(78, 40, '\u0002');
        this.doorC.SetDoorType('\u0004');
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2450.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2450.waitPage(this.win, 64);
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    void yesno() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
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

    class NpcEnemy
            extends Enepc {
        NpcEnemy(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }
    }
}

