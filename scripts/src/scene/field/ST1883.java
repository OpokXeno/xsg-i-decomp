import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_DYU18_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1883
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU18_PRJ {
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
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
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
    Uwamono shopA;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono item01;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;
    String[] JOE_00 = new String[]{"/[label(Great Joe)]", "A boy gunman?", "/[waitkey(1)]/[clear()]", "You know what awaits you if you are defeated in a gunfight, boy?", "/[waitkey(1)]/[clear()]", "And you still dare to challenge me?", "/[waitkey(64)]/[close()]"};
    String[] JOE_00_1 = new String[]{"/[label(Great Joe)]", "Well, this should be somewhat entertainin'.", "/[waitkey(1)]/[clear()]", "Time to die, boy!", "/[waitkey(64)]/[close()]"};
    String[] JOE_00_2 = new String[]{"/[label(Great Joe)]", "Is that gun an accessory?", "/[waitkey(1)]/[clear()]", "I don't have time for wussies. Get lost!!", "/[waitkey(64)]/[close()]"};
    String[] JOE_01 = new String[]{"/[label(Great Joe)]", "I lost...", "/[waitkey(1)]/[clear()]", "I let my guard down, but still, that was some gunmanship. You're a man worthy of calling a friend.", "/[waitkey(1)]/[clear()]", "I give this to you, as a token of our friendship.", "/[waitkey(64)]/[close()]"};
    String[] JOE_02 = new String[]{"/[label(Great Joe)]", "Farewell, my friend. I'll entrust that \"Swimsuit\" to you for now.", "/[waitkey(64)]/[close()]"};
    String[] MESS_00 = new String[]{"\n", "Learned Tech Attack, \"Soul Rhapsody.\"", "/[waitkey(64)]/[close()]"};
    String[] MESS_01 = new String[]{"\n", "Learned Ether, \"Magnum Joe.\"", "/[waitkey(64)]/[close()]"};

    ST1883() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            System.println("新技ＧＥＴなのか！？");
            Runtime.etherTecSet(7);
            Runtime.setFlags(3135, 1, 1);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.JOE_01, 0);
            System.waitFor(this.win);
            Sound.effectPlay(6);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.MESS_00, 0);
            System.waitFor(this.win);
            Sound.effectPlay(6);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.MESS_01, 0);
            System.waitFor(this.win);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.JOE_02, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
        }
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.527f, 2.659f, -5.333f);
        this.camEV.setRotate(-18.058f, -24.66f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -4.895f, 1.059f, -4.543f, 90.0f, -4.895f, 2.051f, -4.543f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -7.178f;
        fArray2[2] = 22.159f;
        fArray2[4] = 90.0f;
        fArray2[5] = -7.178f;
        fArray2[6] = 22.159f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.895f, 2.051f, -4.543f);
        this.camEV.setRotate(-7.178f, 22.159f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    void broken(int n) {
        block0:
        switch (n) {
            case 1: {
                System.println("broken1111111111111111111111111111");
                if (Runtime.getLeader() != 5 || Runtime.getFlags(3135, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.enemy1.setVisible(true);
                this.player.setVisible(false);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                System.sleep(1);
                Runtime.setPlayerControl(false);
                System.sleep(15);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                System.sleep(75);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.EV_Camera02();
                System.sleep(1);
                Runtime.setPlayerControl(false);
                System.sleep(22);
                this.EV_Camera00();
                System.sleep(1);
                Runtime.setPlayerControl(false);
                System.sleep(60);
                this.player.setVisible(true);
                this.cam0.setMode(0);
                this.enemy1.kickEnepc(4, 1);
                this.enemy1.kickEnepc(1, 9);
                this.player.look_char(this.enemy1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.JOE_00, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Bring it on\nI don't have time for this");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.JOE_00_1, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.player.look_default();
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy1.kickEnepc(14, 0);
                        break block0;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.JOE_00_2, 0);
                System.waitFor(this.win);
                this.enemy1.kickEnepc(4, 0);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                this.player.look_default();
                Runtime.jumpCF(1753, 1);
                break;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                System.sleep(30);
                System.println("broken22222222222222222222222222");
                Runtime.setPlayerControl(true);
                break;
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
                Runtime.jumpCF(1753, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(3, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        if (Runtime.getFlags(3135, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(20253, 3, -5.261f, 0.6f, -5.97f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            this.enemy1.setGroup(0);
            this.enemy1.enableDTKFlag(65536);
            this.enemy1.setParams(0, 2, 1, 3);
            this.enemy1.dispRadar(false);
            this.enemy1.talkto("TalkNPC1");
            this.enemy1.kickEnepc(10, 70, 0);
            this.enemy1.setBatEvent(12);
            this.enemy1.setVisible(false);
        }
        this.trap1 = new Uwamono(0, 40);
        if (Runtime.getFlags(3135, 1) == 0) {
            this.trap1.SetCallNo(1);
        }
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 217);
        this.trap2 = new Uwamono(1, 40, this.item01);
        this.trap2.SetCallNo(2);
        this.doorA = new Uwamono(2, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.shopA = new Uwamono(28679, 0.0f, 0.0f, 0.0f);
        this.shopA.SetShopNo(6);
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
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }
    }
}

