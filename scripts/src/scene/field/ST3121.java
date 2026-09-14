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
import xeno.map.MC_GNK18_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3121
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK18_PRJ {
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
    Unit PUPU;
    Unit kakuheki;
    Effect light01;
    Effect light02;
    Effect light03;
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
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
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
    String[] DOA_00 = new String[]{"It will not open from this side. (temp)", "/[waitkey(64)]/[close()]"};

    ST3121() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 7.823f, -4.622f);
        this.camEV.setRotate(-33.941f, 0.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[1] = 2.919f;
        fArray[2] = 2.831f;
        fArray[3] = 5.828f;
        fArray[4] = 150.0f;
        fArray[6] = 2.931f;
        fArray[7] = 6.0f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[0] = 1.0f;
        fArray3[1] = -12.376f;
        fArray3[2] = 15.239f;
        fArray3[4] = 150.0f;
        fArray3[5] = -12.376f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 3, 150);
        this.camEV.rotateSPL(fArray4, 1, 3, 150);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.545f, 0.399f, 5.145f);
        this.camEV.setRotate(-4.568f, 31.939f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 2.931f, 6.0f);
        this.camEV.setRotate(-12.376f, 0.0f, 0.0f);
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
                Runtime.jumpCF(68596, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(68586, 4);
                break;
            }
            case 2: {
                Runtime.jumpCF(68666, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(68676, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -8.95f, 1.0f, -4.55f, 0.0f);
        this.teiten1.SetBgm(196619);
        this.teiten2 = new Uwamono(28690, 8.95f, 1.0f, -4.55f, 0.0f);
        this.teiten2.SetBgm(196619);
        this.teiten3 = new Uwamono(28690, -11.5f, 1.0f, -16.3f, 0.0f);
        this.teiten3.SetBgm(196618);
        this.teiten4 = new Uwamono(28690, 13.1f, 1.0f, -14.9f, 0.0f);
        this.teiten4.SetBgm(196618);
        this.teiten5 = new Uwamono(28690, 0.0f, 0.0f, -7.0f, 0.0f);
        this.teiten5.SetBgm(196619);
        Runtime.disable(524288);
        Stage.setVisible(-1, true);
        this.kakuheki = new Mapunits();
        this.kakuheki.mapUnit(43);
        this.kakuheki.start(4, null);
        this.E01 = new Effect(1647, -14.85f, 2.75f, -6.981f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.setRotate(0.0f, 180.0f, 0.0f);
        this.E01.noAttach(false);
        this.E02 = new Effect(1647, 14.85f, 2.75f, -6.981f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E02.noAttach(false);
        this.E03 = new Effect(1648, -0.3f, 2.75f, -16.95f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
        this.E03.noAttach(false);
        this.E04 = new Effect(1650, -2.584f, 2.816f, -11.959f, 0.0f);
        this.E04.disp(true);
        this.E04.setClip(true);
        this.E04.noAttach(false);
        this.E05 = new Effect(1650, 2.584f, 2.816f, -11.959f, 0.0f);
        this.E05.disp(true);
        this.E05.setClip(true);
        this.E05.noAttach(false);
        this.E06 = new Effect(1649, 0.0f, 2.4f, -6.55f, 0.0f);
        this.E06.disp(true);
        this.E06.setClip(true);
        this.E06.noAttach(false);
        this.E07 = new Effect(1651, -2.584f, 2.816f, -11.959f, 0.0f);
        this.E07.disp(false);
        this.E07.setClip(true);
        this.E07.setScale(1.0f, 0.0f, 1.0f);
        this.E07.noAttach(false);
        this.E08 = new Effect(1651, 2.584f, 2.816f, -11.959f, 0.0f);
        this.E08.disp(false);
        this.E08.setClip(true);
        this.E08.setScale(1.0f, 0.0f, 1.0f);
        this.E08.noAttach(false);
        this.E09 = new Effect(1642, 2.804f, -1.711f, -1.482f, 0.0f);
        this.E09.disp(false);
        this.E09.setClip(true);
        this.E09.setScale(1.0f, 0.8f, 1.0f);
        this.E09.noAttach(false);
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
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.35f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.45f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.45f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(3, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 4.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        new Uwamono(6, 40);
        if (Runtime.getFlags(3086, 1) == 0) {
            this.doorA = new Uwamono(1, 40, '\u0001');
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA = new Uwamono(1, 40, '\u0001');
            this.doorA.SetDoorType('\u0004');
        }
        this.doorB = new Uwamono(0, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(3, 40, '\u0001');
        new Uwamono(2, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(5, 40, '\u0001');
        new Uwamono(4, 40, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.PUPU = new Mapunits();
        this.PUPU.mapUnit(22);
        this.PUPU.start(4, null);
        this.PUPU.start(1, "Evt");
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

        void Evt() {
            Runtime.setPlayerControl(false);
            ST3121.this.cam0.setMode(-1);
            ST3121.this.EV_Camera02();
            System.sleep(60);
            ST3121.this.E09.disp(true);
            Sound.streamPlay(1195015, 48000);
            System.sleep(60);
            ST3121.this.EV_Camera00();
            int n = 0;
            while (true) {
                if (n >= 0 && n < 90) {
                    ST3121.this.E04.getScale();
                    ST3121.this.E04.setScale(1.0f, 0.011111111f * (float) (90 - n), 1.0f);
                    ST3121.this.E05.getScale();
                    ST3121.this.E05.setScale(1.0f, 0.011111111f * (float) (90 - n), 1.0f);
                }
                if (n == 90) {
                    ST3121.this.E04.disp(false);
                    ST3121.this.E05.disp(false);
                    ST3121.this.E07.disp(true);
                    ST3121.this.E08.disp(true);
                }
                if (n >= 91 && n < 181) {
                    ST3121.this.E07.setScale(1.0f, 0.011111111f * (float) (n - 91), 1.0f);
                    ST3121.this.E08.setScale(1.0f, 0.011111111f * (float) (n - 91), 1.0f);
                }
                if (n == 240) {
                    ST3121.this.EV_Camera01();
                }
                if (n >= 240 && n < 390) {
                    ST3121.this.kakuheki.getTranslate();
                    ST3121.this.kakuheki.setTranslate(ST3121.this.kakuheki.px, ST3121.this.kakuheki.py - 0.02f, ST3121.this.kakuheki.pz);
                    ST3121.this.E06.getTranslate();
                    ST3121.this.E06.setTranslate(ST3121.this.E06.px, ST3121.this.E06.py - 0.02f, ST3121.this.E06.pz);
                }
                if (n >= 390 && n < 420) {
                    ST3121.this.EV_Camera03();
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setPlayerControl(true);
            ST3121.this.fade.call(0);
            System.sleep(30);
            Runtime.jumpCF(68616, 3);
        }
    }
}

