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
import xeno.map.MC_KAS02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2410
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS02_PRJ {
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
    MAPUnit tree1;
    MAPUnit tree2;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono doorB;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] locked = new String[]{"It's locked.", "/[waitkey(64)]/[close()]"};
    String[] annai = new String[]{"'←Forest\n", " →Church'", "/[waitkey(64)]/[close()]"};
    String[] mj = new String[]{"There's a disturbance in space-time. ", "Jump into it?", "/[waitkey(64)]/[close()]"};

    ST2410() {
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
                this.nwin(this.locked);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.mj);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        Runtime.jumpCF(2440, 4);
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.camEV = Camera.create(1);
                this.cam0.setMode(-1);
                this.camEV.setRotate(-16.6f, 7.2f, 0.0f);
                this.camEV.setTranslate(-33.7f, 2.6f, -4.5f);
                this.camEV.setFov(42.5f);
                this.camEV.change();
                System.println("lo:0");
                this.nwin(this.annai);
                this.cam0.setMode(0);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("森２【MC_KAS17】・３");
                Runtime.jumpCF(2510, 3);
                break;
            }
            case 1: {
                System.println("教会内部【MC_KAS06】・１");
                if (Runtime.getFlags(337, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Runtime.setFlags(337, 1, 1);
                    System.println("イベント3025");
                    Runtime.jumpEvent(3250);
                    break;
                }
                Runtime.jumpCF(2450, 1);
                break;
            }
            case 2: {
                System.println("教会内部【MC_KAS06】・２");
                Runtime.jumpCF(2450, 2);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -31.1f, 0.0f, -12.1f);
        this.teiten1.SetBgm(196609);
        Stage.setVisible(-1, true);
        Stage.setVisible(27, false);
        Stage.setVisible(34, false);
        Stage.setVisible(35, false);
        Stage.setVisible(36, false);
        Stage.setVisible(43, false);
        Stage.setVisible(21, false);
        this.tree1 = new MAPUnit();
        this.tree1.mapUnit(18);
        this.tree1.start(4, null);
        this.tree1.setTranslate(-38.39f, 4.67f, -0.983f);
        this.tree2 = new MAPUnit();
        this.tree2.mapUnit(19);
        this.tree2.start(4, null);
        this.tree2.setRotateY(70.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1620, -31.1f, 0.0f, -12.1f, 0.0f);
        this.light01.disp(true);
        Runtime.progressEffect(30);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(8073, 1) == 0) {
            Runtime.setFlags(8073, 1, 1);
            Runtime.resetOutFriend(6);
            Runtime.resetOutFriend(4);
            Runtime.resetOutFriend(5);
            Runtime.setLockParty(1);
            Runtime.setLockParty(5);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 7);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 1);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
        }
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.0175f, 0.0175f);
        this.cam0.setCFAngle(2, -28.0f, -10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.0175f, 0.0175f);
        this.cam0.setCFPedestal(3, 5.78664f, 14.791878f, 16.077806f, 43.2f, -45.29731f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.0175f, 0.0175f);
        this.cam0.setCFAngle(5, -28.0f, 10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.0175f, 0.0175f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.0175f, 0.0175f);
        this.doorA = new Uwamono(0, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(1, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        if (Runtime.getFlags(8077, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2410.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2410.waitPage(this.win, 64);
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

