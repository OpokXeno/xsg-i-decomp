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
import xeno.map.MC_GNK21_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3150
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK21_PRJ {
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
    Enepc mom;
    Enepc shi;
    Enepc jun;
    Enepc cha;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit Kidou;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect fade;
    Light light = new Light(0);
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "This...\n", "is the room where I was born.", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "What?\n", "This is where you were...?", "/[waitkey(64)]/[close()]"};

    ST3150() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, -0.502f, 7.458f, 17.385f, 90.0f, -0.502f, 4.991f, 17.385f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -7.462f;
        fArray2[2] = -10.579f;
        fArray2[4] = 90.0f;
        fArray2[5] = -7.462f;
        fArray2[6] = -10.579f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(40.0f);
        this.camEV.change();
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
                Runtime.jumpCF(3010, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 0.0f, 0.6f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196623);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1654, 0.0f, -8.0f, 0.0f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
        this.E02 = new Effect(1655, 0.0f, -6.0f, 0.0f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E02.noAttach(false);
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 0.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        if (Runtime.getFlags(391, 1) == 0) {
            this.mom = new NPC_NORMAL(4, 1, 0, 1, 5, 0.262f, 0.6f, 10.749f, 0.0f);
            this.mom.setInvalidID(1);
            this.shi = new NPC_NORMAL(1, 2, 0, 1, 8, 0.262f, 0.6f, 10.749f, 0.0f);
            this.shi.setInvalidID(1);
            this.jun = new NPC_NORMAL(5, 3, 0, 1, 11, 0.262f, 0.6f, 10.749f, 0.0f);
            this.jun.setInvalidID(1);
            this.cha = new NPC_NORMAL(3, 4, 0, 1, 14, 0.262f, 0.6f, 10.749f, 180.0f);
            this.cha.setInvalidID(1);
        }
        if (Runtime.getFlags(391, 1) == 0) {
            this.Kidou = new Mapunits();
            this.Kidou.mapUnit(9);
            this.Kidou.start(4, null);
            Runtime.setFlags(391, 1, 1);
            this.Kidou.start(1, "Momo");
        }
        this.doorA = new Uwamono(0, 42, '\u0001');
        new Uwamono(1, 42, '\u0001', this.doorA);
        this.doorA.SetDoorRange(1.74f);
        this.doorA.SetDoorType('\u0004');
        Runtime.getFlags(391, 1);
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

        void Momo() {
            ST3150.this.cam0.setMode(-1);
            ST3150.this.EV_Camera00();
            Runtime.setPlayerControl(false);
            System.sleep(2);
            ST3150.this.mom.kickEnepc(4, 3);
            ST3150.this.mom.mtn(2, 9, 1.0f, true);
            System.sleep(2);
            ST3150.this.mom.move(90, 0.34f, 6.746f, true);
            System.sleep(1);
            ST3150.this.shi.kickEnepc(4, 3);
            ST3150.this.shi.mtn(2, 9, 1.0f, true);
            System.sleep(2);
            ST3150.this.shi.move(88, -0.3379f, 4.594f, true);
            System.sleep(1);
            ST3150.this.jun.kickEnepc(4, 3);
            ST3150.this.jun.mtn(2, 9, 1.0f, true);
            System.sleep(2);
            ST3150.this.jun.move(78, -0.333f, 8.666f, true);
            System.sleep(1);
            ST3150.this.cha.kickEnepc(4, 3);
            ST3150.this.cha.mtn(2, 9, 1.0f, true);
            System.sleep(2);
            ST3150.this.cha.move(85, 0.368f, 10.005f, true);
            System.sleep(1);
            System.sleep(80);
            ST3150.this.jun.mtn(1, 9, 1.0f, true);
            System.sleep(10);
            ST3150.this.shi.mtn(1, 1, 1.0f, true);
            ST3150.this.cha.mtn(1, 9, 1.0f, true);
            System.sleep(5);
            ST3150.this.mom.mtn(1, 9, 1.0f, true);
            System.sleep(30);
            ST3150.this.shi.kickEnepc(4, 0);
            ST3150.this.shi.kickEnepc(4, 1);
            ST3150.this.shi.kickEnepc(1, 1);
            ST3150.this.shi.moveEnepc(17, 15.0f, -20.0f, 20);
            System.sleep(20);
            ST3150.this.shi.kickEnepc(1, 0);
            ST3150.this.shi.rotY(30, 0.0f, true);
            System.sleep(35);
            ST3150.this.shi.mtn(1, 9, 1.0f, true);
            System.sleep(5);
            ST3150.this.mom.mtn(8, 1, 1.0f, true);
            System.sleep(40);
            ST3150.this.mom.mtn(10, 1, 1.0f, true);
            ST3150.this.win = Window.create();
            ST3150.this.win.setSize(4, 45);
            ST3150.this.win.setLocation(15, 305);
            ST3150.this.win.print(ST3150.this.MOM_00, 0);
            System.waitFor(ST3150.this.win);
            ST3150.this.shi.kickEnepc(0, 9);
            ST3150.this.win = Window.create();
            ST3150.this.win.setSize(4, 45);
            ST3150.this.win.setLocation(15, 305);
            ST3150.this.win.print(ST3150.this.SHI_00, 0);
            System.waitFor(ST3150.this.win);
            ST3150.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.jumpEvent(3550);
            ST3150.this.cam0.setMode(0);
        }
    }
}

