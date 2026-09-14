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
import xeno.map.MC_EVE02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1891
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_EVE02_PRJ {
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
    Unit unit1;
    Unit UgoUgo;
    Unit Doa1;
    Unit Doa2;
    Unit Elv;
    Unit Elv2;
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
    Effect EF01;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;

    ST1891() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.694f, 4.679f, 1.146f);
        this.camEV.setRotate(-33.929f, 64.019f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 5.449f, 6.823f, 1.418f, 90.0f, 5.449f, 6.823f, 1.418f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -37.317f;
        fArray2[2] = 46.075f;
        fArray2[4] = 90.0f;
        fArray2[5] = -37.317f;
        fArray2[6] = 70.174f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(25.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        float[] fArray = new float[]{1.0f, 1.672f, 4.231f, 0.711f, 60.0f, 1.672f, 4.231f, 0.711f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -14.198f;
        fArray2[2] = 32.755f;
        fArray2[4] = 60.0f;
        fArray2[5] = -14.198f;
        fArray2[6] = 58.735f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 60);
        this.camEV.rotateSPL(fArray3, 1, 3, 60);
        this.camEV.setFov(30.0f);
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
            default:
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        this.EF01 = new Effect(1542, 1.3f, 4.0f, 0.08f, 0.0f);
        this.EF01.disp(true);
        this.EF01.setScale(0.1f, 0.05f, 0.1f);
        this.EF01.setRotate(0.0f, 90.0f, 0.0f);
        this.EF01.noAttach(false);
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
        Runtime.setPlayerMoveParam(8.0f, 24.0f, 9.895E-4f);
        Stage.setColor(0.64f, 0.64f, 0.64f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 30.0f, 35.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 10.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 10.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 10.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.Doa1 = new Mapunits();
        this.Doa1.mapUnit(17);
        this.Doa1.start(4, null);
        this.Doa1.setRotateY(-90.0f);
        this.Doa2 = new Mapunits();
        this.Doa2.mapUnit(19);
        this.Doa2.start(4, null);
        this.Doa2.setRotateY(-90.0f);
        this.Elv = new Mapunits();
        this.Elv.mapUnit(23);
        this.Elv.start(4, null);
        this.Elv2 = new Mapunits();
        this.Elv2.mapUnit(24);
        this.Elv2.start(4, null);
        this.Elv2.setTranslate(0.0f, 0.15f, 0.0f);
        this.UgoUgo = new Mapunits();
        this.UgoUgo.mapUnit(10);
        this.UgoUgo.start(4, null);
        this.UgoUgo.start(1, "Evt");
        this.player.setScale(0.25f, 0.25f, 0.25f);
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
            this.setShadow(0, 0);
            this.setElevatorMode(1);
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
        int time = 0;

        Mapunits() {
        }

        void Evt() {
            ST1891.this.cam0.setMode(-1);
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST1891.this.EV_Camera01();
            ST1891.this.player.mtn(4, 9, 1.0f, true);
            ST1891.this.player.move(90, 1.7f, -0.5f, true);
            System.sleep(89);
            ST1891.this.EV_Camera00();
            ST1891.this.player.setTranslate(1.7f, 3.84f, -0.5f);
            System.sleep(5);
            ST1891.this.player.mtn(4, 9, 1.0f, true);
            ST1891.this.player.move(40, 1.7f, 0.22f, true);
            System.sleep(41);
            ST1891.this.player.mtn(2, 9, 1.0f, true);
            ST1891.this.player.rotY(15, -90.0f, true);
            System.sleep(20);
            ST1891.this.player.mtn(1, 1, 1.0f, true);
            System.sleep(95);
            ST1891.this.player.mtn(8, 1, 1.0f, true);
            System.sleep(40);
            ST1891.this.player.mtn(26, 1, 1.0f, true);
            System.sleep(90);
            ST1891.this.player.mtn(2, 9, 1.0f, true);
            ST1891.this.player.rotY(15, -45.0f, true);
            System.sleep(20);
            ST1891.this.player.mtn(1, 9, 1.0f, true);
            System.sleep(5);
            while (true) {
                ST1891.this.player.getTranslate();
                ST1891.this.Doa1.getTranslate();
                ST1891.this.Doa2.getTranslate();
                ST1891.this.Elv.getTranslate();
                ST1891.this.Elv2.getTranslate();
                float f = (1.501f - ST1891.this.player.px) / 20.0f;
                float f2 = (3.887f - ST1891.this.player.py) / 20.0f;
                float f3 = (0.562f - ST1891.this.player.pz) / 20.0f;
                float f4 = (1.501f - ST1891.this.player.px) / 20.0f;
                float f5 = (3.925f - ST1891.this.player.py) / 20.0f;
                float f6 = (0.562f - ST1891.this.player.pz) / 20.0f;
                if (this.time == 0) {
                    Sound.streamPlay(1195003, 48000);
                }
                if (this.time >= 0 && this.time < 60) {
                    ST1891.this.Doa1.setTranslate(ST1891.this.Doa1.px, ST1891.this.Doa1.py, ST1891.this.Doa1.pz + 0.004166667f);
                    ST1891.this.Doa2.setTranslate(ST1891.this.Doa2.px, ST1891.this.Doa2.py, ST1891.this.Doa2.pz + -0.004166667f);
                }
                if (this.time >= 60 && this.time < 300) {
                    ST1891.this.Elv.setTranslate(ST1891.this.Elv.px, ST1891.this.Elv.py + 0.016250001f, ST1891.this.Elv.pz);
                    ST1891.this.Elv2.setTranslate(ST1891.this.Elv2.px, ST1891.this.Elv2.py + 0.016250001f, ST1891.this.Elv2.pz);
                }
                if (this.time >= 330 && this.time < 390) {
                    ST1891.this.Elv2.setTranslate(ST1891.this.Elv2.px, ST1891.this.Elv2.py - 0.0025000002f, ST1891.this.Elv2.pz);
                }
                if (this.time >= 410 && this.time < 430) {
                    ST1891.this.player.rotY(15, -45.0f, true);
                    ST1891.this.player.mtn(2, 9, 1.0f, true);
                }
                if (this.time >= 430 && this.time < 440) {
                    ST1891.this.player.rotY(2, -90.0f, true);
                }
                if (this.time >= 440 && this.time < 470) {
                    ST1891.this.player.getTranslate();
                    ST1891.this.player.setTranslate(ST1891.this.player.px + f4, ST1891.this.player.py + f5, ST1891.this.player.pz);
                }
                if (this.time >= 470 && this.time < 530) {
                    ST1891.this.player.move(60, 1.501f, 0.562f, true);
                }
                if (this.time >= 530 && this.time < 590) {
                    ST1891.this.player.mtn(28, 9, 1.0f, true);
                }
                if (this.time >= 590 && this.time < 650) {
                    ST1891.this.Elv2.setTranslate(ST1891.this.Elv2.px, ST1891.this.Elv2.py + 0.0025000002f, ST1891.this.Elv2.pz);
                }
                if (this.time >= 680 && this.time < 800) {
                    ST1891.this.Elv.setTranslate(ST1891.this.Elv.px, ST1891.this.Elv.py - 0.0075000003f, ST1891.this.Elv.pz);
                    ST1891.this.Elv2.setTranslate(ST1891.this.Elv2.px, ST1891.this.Elv2.py - 0.0075000003f, ST1891.this.Elv2.pz);
                    ST1891.this.player.setTranslate(ST1891.this.player.px, ST1891.this.player.py - 0.0075000003f, ST1891.this.player.pz);
                }
                if (this.time >= 800 && this.time < 860) {
                    ST1891.this.Doa1.setTranslate(ST1891.this.Doa1.px, ST1891.this.Doa1.py, ST1891.this.Doa1.pz - 0.004166667f);
                    ST1891.this.Doa2.setTranslate(ST1891.this.Doa2.px, ST1891.this.Doa2.py, ST1891.this.Doa2.pz - -0.004166667f);
                }
                if (this.time == 860) {
                    Runtime.setPlayerControl(true);
                    ST1891.this.fade.call(0);
                    Runtime.setFlags(7154, 1, 1);
                    Runtime.setFlags(3026, 1, 0);
                    Runtime.setFlags(3027, 1, 0);
                }
                if (this.time == 890) break;
                ++this.time;
                System.sleep(1);
            }
            ST1891.this.cam0.setMode(0);
            Runtime.jumpCF(541, 4);
            ST1891.this.cam0.setMode(0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

