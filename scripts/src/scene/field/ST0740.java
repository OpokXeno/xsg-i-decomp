import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS13B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0740
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS13B_PRJ {
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
    Enepc shion;
    Enepc kosmos;
    Enepc chaos;
    Enepc momo;
    Enepc ziggy;
    Enepc enemy1;
    Unit unit1;
    Unit Bed;
    Unit ST1;
    Unit Pod1;
    Unit Pod2;
    Unit Pod3;
    Unit Pod4;
    Unit Bke;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int shiontalked = 0;
    int kosmostalked = 0;
    int chaostalked = 0;
    int momo4talked = 0;
    int momo5talked = 0;
    int momo6talked = 0;
    int momo7talked = 0;
    int momo8talked = 0;
    boolean shionflg = false;
    boolean kosmosflg = false;
    Uwamono doorA;
    Uwamono itembox;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    Effect E08;
    Effect E09;
    Effect E10;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    int page;
    String[] ZIG_01 = new String[]{"/[label(Shion)]", "If we disable that thing, we'll put a stop to the Auto-Tech invasion.", "/[waitkey(64)]/[close()]"};

    ST0740() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            System.println("1");
            Runtime.setFlags(3026, 1, 0);
            Runtime.setFlags(3027, 1, 0);
            Runtime.setFlags(3039, 1, 1);
            Runtime.setFlags(7167, 1, 1);
            Runtime.setFlags(142, 1, 1);
            Runtime.jumpEvent(2290);
        }
    }

    void EV_Camera02() {
        float[] fArray = new float[]{1.0f, -1.641f, 0.827f, -6.456f, 90.0f, 0.815f, 0.827f, -6.548f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 7.374f;
        fArray2[4] = 90.0f;
        fArray2[5] = 7.374f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.206f, 1.884f, -8.412f);
        this.camEV.setRotate(-15.365f, 20.319f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 5.787f, -12.141f);
        this.camEV.setRotate(-9.545f, 180.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[2] = 5.787f;
        fArray[3] = -12.141f;
        fArray[4] = 90.0f;
        fArray[6] = 5.787f;
        fArray[7] = -12.141f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[0] = 1.0f;
        fArray3[1] = -9.545f;
        fArray3[2] = 180.0f;
        fArray3[4] = 90.0f;
        fArray3[5] = -22.005f;
        fArray3[6] = 180.0f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 2, 90);
        this.camEV.rotateSPL(fArray4, 1, 2, 90);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05a() {
        float[] fArray = new float[]{1.0f, 2.914f, -1.188f, -4.892f, 60.0f, 2.914f, 3.323f, -4.892f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 13.894f;
        fArray2[2] = 139.766f;
        fArray2[4] = 60.0f;
        fArray2[5] = 13.894f;
        fArray2[6] = 139.766f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 60);
        this.camEV.rotateSPL(fArray3, 1, 2, 60);
        this.camEV.setFov(55.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.815f, 0.827f, -6.548f);
        this.camEV.setRotate(7.374f, 0.0f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 != 0 && n2 == 1) {
            switch (n) {
                default:
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66266, 3);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        Stage.renderCommand(4);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(307, false);
        Stage.setVisible(308, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -0.5f, -2.0f, 12.9f, 0.0f);
        this.teiten1.SetBgm(196639);
        this.teiten2 = new Uwamono(28690, -7.673f, -1.25f, 8.148f, 0.0f);
        this.teiten2.SetBgm(196638);
        this.teiten3 = new Uwamono(28690, 1.455f, -1.75f, 11.179f, 0.0f);
        this.teiten3.SetBgm(196638);
        this.teiten4 = new Uwamono(28690, 3.104f, -1.75f, 19.83f, 0.0f);
        this.teiten4.SetBgm(196638);
        this.teiten5 = new Uwamono(28690, 5.467f, -1.75f, 8.883f, 0.0f);
        this.teiten5.SetBgm(196638);
        this.teiten6 = new Uwamono(28690, 3.1f, -2.0f, 16.6f, 0.0f);
        this.teiten6.SetBgm(196639);
        this.teiten7 = new Uwamono(28690, -8.583f, 0.5f, -10.49f, 0.0f);
        this.teiten7.SetBgm(196638);
        this.teiten8 = new Uwamono(28690, 6.472f, 0.0f, -4.284f, 0.0f);
        this.teiten8.SetBgm(196638);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightCol(1, 3, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setFog(0, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(1, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 17.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.E01 = new Effect(1401, -0.5f, -2.0f, 12.9f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E02 = new Effect(1402, -7.673f, -1.25f, 8.148f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E03 = new Effect(1402, 1.455f, -1.75f, 11.179f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
        this.E04 = new Effect(1402, 3.104f, -1.75f, 19.83f, 0.0f);
        this.E04.disp(true);
        this.E04.setClip(true);
        this.E05 = new Effect(1402, 5.467f, -1.75f, 8.883f, 0.0f);
        this.E05.disp(true);
        this.E05.setClip(true);
        this.E06 = new Effect(1401, 3.1f, -2.0f, 16.6f, 0.0f);
        this.E06.disp(true);
        this.E06.setClip(true);
        this.E07 = new Effect(1402, -8.583f, 0.5f, -10.49f, 0.0f);
        this.E07.disp(true);
        this.E07.setClip(true);
        this.E08 = new Effect(1402, 6.472f, 0.0f, -4.284f, 0.0f);
        this.E08.disp(true);
        this.E08.setClip(true);
        this.E09 = new Effect(1612, -1.0f, -1.75f, -3.5f, 0.0f);
        this.E09.disp(false);
        this.E09.setClip(true);
        this.E10 = new Effect(1612, 1.0f, -1.75f, -3.5f, 0.0f);
        this.E10.disp(false);
        this.E10.setClip(true);
        if (Runtime.getFlags(3029, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(20229, 25, 0.0f, -1.75f, 0.0f, 180.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray = new float[8];
            fArray[1] = -1.75f;
            fArray[2] = 4.0f;
            fArray[3] = 1.0f;
            fArray[5] = -1.75f;
            fArray[6] = 5.0f;
            fArray[7] = -1.0f;
            float[] fArray2 = fArray;
            this.enemy1.setParams(0, 8, 1, 25, fArray2);
            this.enemy1.setBatEvent(8);
            this.enemy1.enableDTKFlag(262144);
            this.enemy1.setInvalidID(1);
            this.enemy1.setShadow(0, 0);
            this.enemy1.renderCommand(22);
        } else {
            this.enemy1 = new Enepc();
            this.enemy1.init(20229, 25, 0.0f, -1.75f, 0.0f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray = new float[8];
            fArray[1] = -1.75f;
            fArray[2] = 4.0f;
            fArray[3] = 1.0f;
            fArray[5] = -1.75f;
            fArray[6] = 5.0f;
            fArray[7] = -1.0f;
            float[] fArray3 = fArray;
            this.enemy1.setParams(0, 8, 1, 25, fArray3);
            this.enemy1.setBatEvent(8);
            this.enemy1.enableDTKFlag(262144);
            this.enemy1.setMotion(0, 27);
            this.enemy1.setInvalidID(1);
            this.enemy1.setShadow(0, 0);
            this.enemy1.renderCommand(22);
        }
        if (Runtime.getFlags(3029, 1) == 0) {
            this.shion = new Enepc();
            this.shion.init(1, 11, -0.65f, 0.0f, -17.0f, 0.0f);
            this.shion.id = 11;
            this.shion.setParams(0, 2, 11, 11);
            this.shion.dispRadar(false);
            this.shion.setShadow(3, 16);
            this.shion.setInvalidID(1);
            this.shion.renderCommand(22);
            this.kosmos = new Enepc();
            this.kosmos.init(2, 7, -0.35f, 0.0f, -18.0f, 0.0f);
            this.kosmos.id = 12;
            this.kosmos.setParams(0, 3, 12, 7);
            this.kosmos.dispRadar(false);
            this.kosmos.setShadow(3, 16);
            this.kosmos.setInvalidID(1);
            this.kosmos.renderCommand(22);
            this.chaos = new Enepc();
            this.chaos.init(3, 5, 0.8f, 0.0f, -17.0f, 0.0f);
            this.chaos.id = 13;
            this.chaos.setParams(0, 4, 13, 5);
            this.chaos.dispRadar(false);
            this.chaos.setShadow(3, 16);
            this.chaos.setInvalidID(1);
            this.chaos.renderCommand(22);
            this.momo = new Enepc();
            this.momo.init(4, 9, -0.18f, 0.0f, -16.28f, 0.0f);
            this.momo.id = 14;
            this.momo.setParams(0, 5, 14, 9);
            this.momo.dispRadar(false);
            this.momo.setShadow(3, 16);
            this.momo.setInvalidID(1);
            this.momo.renderCommand(22);
            this.ziggy = new Enepc();
            this.ziggy.init(6, 13, 0.5f, 0.0f, -17.5f, 0.0f);
            this.ziggy.id = 15;
            this.ziggy.setParams(0, 6, 15, 13);
            this.ziggy.dispRadar(false);
            this.ziggy.setShadow(3, 16);
            this.ziggy.setInvalidID(1);
            this.ziggy.renderCommand(22);
        }
        this.Pod1 = new Unit();
        this.Pod1.init(20493, -11.5f, 0.611f, -10.5f, 90.0f);
        this.Pod1.renderCommand(22);
        this.Pod2 = new Unit();
        this.Pod2.init(20493, 11.5f, 0.611f, -10.5f, -90.0f);
        this.Pod2.renderCommand(22);
        this.Pod3 = new Unit();
        this.Pod3.init(20493, -11.5f, 0.611f, -5.5f, 90.0f);
        this.Pod4 = new Unit();
        this.Pod4.init(20493, 11.5f, 0.611f, -5.5f, -90.0f);
        this.Pod4.renderCommand(22);
        this.Bke = new Unit();
        this.Bke.init(20575, 2.5f, -3.5f, 18.5f, 0.0f);
        this.Bke.setRotate(3.059f, 20.0f, -6.318f);
        this.Bke.renderCommand(22);
        new Uwamono(67, 31);
        new Uwamono(66, 31);
        this.itembox = new Uwamono(28677, 0.0f, -1.886f, 19.05f, 0.0f, 118);
        this.itembox.SetSymbol(28684);
        this.doorA = new Uwamono(286, 42, '\u0001');
        new Uwamono(285, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(3029, 1) == 0) {
            this.player.setTranslate(50.0f, 50.0f, 50.0f);
        }
        if (Runtime.getFlags(3029, 1) == 0) {
            this.ST1 = new Mapunits();
            this.ST1.mapUnit(63);
            this.ST1.start(4, null);
            this.ST1.start(1, "Move");
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
            extends Unit {
        Mapunits() {
        }

        void Move() {
            Runtime.setPlayerControl(false);
            Runtime.disable(524288);
            ST0740.this.cam0.setMode(-1);
            ST0740.this.EV_Camera02();
            System.sleep(1);
            ST0740.this.shion.kickEnepc(4, 1);
            ST0740.this.kosmos.kickEnepc(4, 1);
            ST0740.this.chaos.kickEnepc(4, 1);
            ST0740.this.momo.kickEnepc(4, 1);
            ST0740.this.ziggy.kickEnepc(4, 1);
            ST0740.this.shion.kickEnepc(9, 15);
            ST0740.this.kosmos.kickEnepc(9, 1);
            ST0740.this.chaos.kickEnepc(9, 14);
            ST0740.this.momo.kickEnepc(9, 13);
            ST0740.this.ziggy.kickEnepc(9, 11);
            ST0740.this.shion.setLocation(4, 9);
            ST0740.this.kosmos.setLocation(4, 7);
            ST0740.this.chaos.setLocation(4, 8);
            ST0740.this.momo.setLocation(4, 6);
            ST0740.this.ziggy.setLocation(4, 10);
            ST0740.this.shion.kickEnepc(1, 27);
            ST0740.this.kosmos.kickEnepc(1, 0);
            ST0740.this.chaos.kickEnepc(1, 10);
            ST0740.this.momo.kickEnepc(1, 9);
            ST0740.this.ziggy.kickEnepc(1, 0);
            System.sleep(90);
            ST0740.this.EV_Camera06();
            System.sleep(30);
            ST0740.this.EV_Camera03();
            ST0740.this.shion.kickEnepc(1, 9);
            ST0740.this.win = Window.create();
            ST0740.this.win.setSize(4, 45);
            ST0740.this.win.setLocation(15, 305);
            ST0740.this.win.print(ST0740.this.ZIG_01, 0);
            System.waitFor(ST0740.this.win);
            ST0740.this.EV_Camera04();
            ST0740.this.enemy1.kickEnepc(4, 1);
            ST0740.this.player.setLocation(1, 2);
            ST0740.this.shion.kickEnepc(4, 0);
            ST0740.this.kosmos.kickEnepc(4, 0);
            ST0740.this.chaos.kickEnepc(4, 0);
            ST0740.this.momo.kickEnepc(4, 0);
            ST0740.this.ziggy.kickEnepc(4, 0);
            System.sleep(1);
            ST0740.this.shion.kickEnepc(4, 2);
            ST0740.this.kosmos.kickEnepc(4, 2);
            ST0740.this.chaos.kickEnepc(4, 2);
            ST0740.this.momo.kickEnepc(4, 2);
            ST0740.this.ziggy.kickEnepc(4, 2);
            ST0740.this.shion.setVisible(false);
            ST0740.this.kosmos.setVisible(false);
            ST0740.this.chaos.setVisible(false);
            ST0740.this.momo.setVisible(false);
            ST0740.this.ziggy.setVisible(false);
            ST0740.this.EV_Camera05();
            Sound.streamPlay(1195001, 48000);
            ST0740.this.enemy1.kickEnepc(0, 27);
            System.sleep(140);
            ST0740.this.EV_Camera05a();
            ST0740.this.E09.disp(true);
            Runtime.setFlags(3029, 1, 1);
            ST0740.this.fade1.call(0);
            System.sleep(58);
            ST0740.this.enemy1.setRotate(ST0740.this.enemy1.rx, ST0740.this.enemy1.ry + 180.0f, ST0740.this.enemy1.rz);
            System.sleep(2);
            ST0740.this.enemy1.setMotion(0, 27);
            ST0740.this.cam0.setMode(0);
            ST0740.this.fade2.call(0);
            System.sleep(60);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }
    }
}

