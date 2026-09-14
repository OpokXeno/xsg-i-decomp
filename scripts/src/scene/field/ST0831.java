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
import xeno.map.MC_PRO03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0831
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO03_PRJ {
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
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Effect light01;
    Effect light02;
    Effect light03;
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
    MAPUnit cube1;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;
    String[] commander = new String[]{"/[label(Captain)]", "Two enemy entities. An enhanced cyborg and a child-model Realian.\n", "/[waitkey(1)]/[clear()]", "Our mission is to prevent them from getting past this point. Aggressive measures are prohibited. Attacking the Realian is strictly prohibited.", " /[waitkey(64)]/[close()]"};
    String[] zigy1 = new String[]{"/[label(Ziggy)]", "These guys are certainly not idiots. Their response time is quick.", "/[waitkey(1)]/[clear()]", "They also change their routes. Looks like I really have my work cut out for me this time.", "/[waitkey(64)]/[close()]"};
    String[] zigy2 = new String[]{"/[label(Ziggy)]", "Those guys aren't very accommodating. They show no signs of moving.\n", "/[waitkey(64)]/[close()]"};
    String[] z81 = new String[]{"/[label(Ziggurat 8)]", "These guys are certainly not idiots. Their response time is quick.", "/[waitkey(1)]/[clear()]", "They also change their routes. Looks like I really have my work cut out for me this time.", "/[waitkey(64)]/[close()]"};
    String[] z82 = new String[]{"/[label(Ziggurat 8)]", "Those guys aren't very accommodating. They show no signs of moving.\n", "/[waitkey(64)]/[close()]"};
    String[] momo1 = new String[]{"/[label(MOMO)]", "There is an A.G.W.S. and an infantry unit beyond the bridge. It is completely blocked off.", "/[waitkey(1)]/[clear()]", "Perhaps we should look for an alternate route.", "/[waitkey(64)]/[close()]"};
    String[] momo2 = new String[]{"/[label(MOMO)]", "As I thought, we can't get through this way. I really think we need to go through another route...", "/[waitkey(64)]/[close()]"};

    ST0831() {
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
                Runtime.jumpCF(820, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(840, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(860, 2);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 1, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 2, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 3, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFPedestal(3, 1.8260219f, 23.871622f, 1.60591f, 39.279556f, -59.919937f, 368.29843f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        int n2 = 5;
        this.enemy1 = new Teki();
        this.enemy1.init(17409, 5, -8.9f, 10.1f, 4.4f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(1);
        this.enemy1.setParams(0, 6, 1, 5);
        this.enemy2 = new Teki();
        this.enemy2.init(17153, 3, -7.3f, 10.1f, 2.9f, 90.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(1);
        this.enemy2.setParams(0, n2, 2, 3);
        this.enemy3 = new Teki();
        this.enemy3.init(17153, 3, -6.5f, 10.1f, 3.8f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1);
        this.enemy3.setParams(0, n2, 3, 3);
        this.enemy4 = new Teki();
        this.enemy4.init(17153, 3, -6.5f, 10.1f, 5.1f, 90.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(1);
        this.enemy4.setParams(0, n2, 4, 3);
        this.enemy5 = new Teki();
        this.enemy5.init(17153, 3, -7.3f, 10.1f, 6.0f, 90.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(1);
        this.enemy5.setParams(0, n2, 5, 3);
        if (Runtime.getFlags(8004, 1) == 0) {
            this.enemy6 = new Teki();
            this.enemy6.init(17153, 3, -5.0f, 10.1f, 4.4f, 270.0f);
            this.enemy6.id = 6;
            this.enemy6.setGroup(1);
            this.enemy6.setParams(0, n2, 6, 3);
        }
        this.light01 = new Effect(1405, -5.2f, 14.5f, 1.1f, 0.0f);
        this.light02 = new Effect(1405, 0.0f, 14.5f, 1.1f, 0.0f);
        this.light03 = new Effect(1405, 5.2f, 14.5f, 1.1f, 0.0f);
        Runtime.progressEffect(60);
        new Uwamono(192, 48);
        new Uwamono(193, 48);
        new Uwamono(222, 30);
        new Uwamono(28677, -1.81f, 0.65f, -7.41f, 180.0f, 53);
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(0);
        this.cube1.start(1, "idle");
        this.enemy1.start(1, "damedakorya");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0831.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0831.waitPage(this.win, 64);
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

    class Teki
            extends Enepc {
        Teki() {
        }

        void damedakorya() {
            if (Runtime.getFlags(8004, 1) == 0) {
                Runtime.setPlayerControl(false);
                System.sleep(10);
                ST0831.this.cam0.setMode(-1);
                ST0831.this.camEV = Camera.create(1);
                ST0831.this.camEV.setRotate(-51.79f, 0.0f, 0.0f);
                System.sleep(30);
                float[] fArray = new float[]{1.0f, 6.4989543f, 23.0f, 12.998999f, 90.0f, -5.6f, 23.0f, 12.998999f};
                ST0831.this.camEV.transSPL(fArray, 1, 1, 90);
                ST0831.this.camEV.setFov(40.0f);
                ST0831.this.camEV.change();
                System.sleep(100);
                ST0831.this.nwin(ST0831.this.commander);
                ST0831.this.cam0.setMode(0);
                System.sleep(10);
                if (Runtime.getLeader() == 6) {
                    if (Runtime.getFlags(108, 1) == 1) {
                        ST0831.this.nwin(ST0831.this.zigy1);
                    } else {
                        ST0831.this.nwin(ST0831.this.z81);
                    }
                } else {
                    ST0831.this.nwin(ST0831.this.momo1);
                }
                System.sleep(15);
                Runtime.setFlags(8004, 1, 1);
                ST0831.this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(840, 1);
            } else {
                Runtime.setPlayerControl(false);
                System.sleep(10);
                ST0831.this.cam0.setMode(-1);
                ST0831.this.camEV = Camera.create(1);
                ST0831.this.camEV.setRotate(-51.79f, 0.0f, 0.0f);
                System.sleep(30);
                float[] fArray = new float[]{1.0f, 6.4989543f, 23.0f, 12.998999f, 90.0f, -5.6f, 23.0f, 12.998999f};
                ST0831.this.camEV.transSPL(fArray, 1, 1, 90);
                ST0831.this.camEV.setFov(40.0f);
                ST0831.this.camEV.change();
                System.sleep(100);
                ST0831.this.cam0.setMode(0);
                System.sleep(10);
                if (Runtime.getLeader() == 6) {
                    if (Runtime.getFlags(108, 1) == 1) {
                        ST0831.this.nwin(ST0831.this.zigy2);
                    } else {
                        ST0831.this.nwin(ST0831.this.z82);
                    }
                } else {
                    ST0831.this.nwin(ST0831.this.momo2);
                }
                System.sleep(15);
                ST0831.this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(840, 1);
            }
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            Input input = Input.create(0);
            boolean bl = false;
            boolean bl2 = false;
            while (true) {
                System.sleep(1);
            }
        }
    }
}

