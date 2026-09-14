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
import xeno.map.MC_GNK18_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3120
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
    Uwamono Uwacol;
    Uwamono Glass;
    Uwamono itembox;
    Uwamono item01;
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
    String[] DOA_00 = new String[]{"It's locked from the other side.", "/[waitkey(64)]/[close()]"};

    ST3120() {
    }

    void EOB(int n) {
        if (n == 3) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3186, 1, 1);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    System.println("\\\\\\\\\\\\\\\\\\\\\\\\");
                    if (Runtime.getFlags(3086, 1) != 0) break;
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.DOA_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    System.sleep(1);
                    Runtime.disable(262144);
                    break;
                }
            }
        }
    }

    void broken(int n) {
        switch (n) {
            default:
        }
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
        if (Runtime.getFlags(3089, 1) == 0) {
            this.teiten5 = new Uwamono(28690, 0.0f, 0.0f, -7.0f, 0.0f);
            this.teiten5.SetBgm(196619);
        }
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3089, 1) == 0) {
            this.Uwacol = new Uwamono(28672, 0.0f, 0.0f, -6.85f);
            this.Uwacol.SetSize(4.0f, 1.0f, 0.5f);
            this.kakuheki = new Mapunits();
            this.kakuheki.mapUnit(43);
            this.kakuheki.start(4, null);
        } else {
            this.Uwacol = new Uwamono(28672, 0.0f, 5.0f, -6.85f);
            this.Uwacol.SetSize(4.0f, 1.0f, 0.5f);
            this.kakuheki = new Mapunits();
            this.kakuheki.mapUnit(43);
            this.kakuheki.start(4, null);
            this.kakuheki.setTranslate(this.kakuheki.px, this.kakuheki.py - 3.0f, this.kakuheki.pz);
        }
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
        if (Runtime.getFlags(3089, 1) == 0) {
            this.E04 = new Effect(1650, -2.584f, 2.816f, -11.959f, 0.0f);
            this.E04.disp(true);
            this.E04.setClip(true);
            this.E04.noAttach(false);
            this.E05 = new Effect(1650, 2.584f, 2.816f, -11.959f, 0.0f);
            this.E05.disp(true);
            this.E05.setClip(true);
            this.E05.noAttach(false);
        } else {
            this.E04 = new Effect(1651, -2.584f, 2.816f, -11.959f, 0.0f);
            this.E04.disp(true);
            this.E04.setClip(true);
            this.E04.noAttach(false);
            this.E05 = new Effect(1651, 2.584f, 2.816f, -11.959f, 0.0f);
            this.E05.disp(true);
            this.E05.setClip(true);
            this.E05.noAttach(false);
        }
        if (Runtime.getFlags(3089, 1) == 0) {
            this.E06 = new Effect(1649, 0.0f, 2.4f, -6.55f, 0.0f);
            this.E06.disp(true);
            this.E06.setClip(true);
            this.E06.noAttach(false);
        }
        if (Runtime.getFlags(3089, 1) == 0) {
            this.E07 = new Effect(1642, 2.804f, -1.711f, -1.482f, 0.0f);
            this.E07.disp(false);
            this.E07.setClip(true);
            this.E07.setScale(1.0f, 0.8f, 1.0f);
            this.E07.noAttach(false);
        } else {
            this.E07 = new Effect(1642, 2.804f, -1.711f, -1.482f, 0.0f);
            this.E07.disp(true);
            this.E07.setClip(true);
            this.E07.setScale(1.0f, 0.8f, 1.0f);
            this.E07.noAttach(false);
            Runtime.progressEffect(30);
        }
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
        this.enemy1 = new Enepc();
        this.enemy1.init(16396, 3, -7.0f, 0.0f, -18.0f, 180.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(3, 3, 5, 5);
        float[] fArray = new float[12];
        fArray[0] = -7.0f;
        fArray[2] = -18.0f;
        fArray[3] = -1.0f;
        fArray[4] = -7.0f;
        fArray[6] = -13.5f;
        fArray[8] = -12.0f;
        fArray[10] = -10.0f;
        fArray[11] = 1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[12];
        fArray3[0] = -7.0f;
        fArray3[2] = -18.0f;
        fArray3[3] = -7.0f;
        fArray3[5] = -13.5f;
        fArray3[6] = -12.0f;
        fArray3[8] = -10.0f;
        fArray3[9] = -7.0f;
        fArray3[11] = -13.5f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 85, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(16403, 13, 6.0f, 0.0f, -17.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(4, 7, 7, 7);
        float[] fArray5 = new float[12];
        fArray5[0] = 6.0f;
        fArray5[2] = -17.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = 6.0f;
        fArray5[6] = -14.0f;
        fArray5[8] = 11.0f;
        fArray5[10] = -10.0f;
        fArray5[11] = 1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 13, fArray6);
        float[] fArray7 = new float[12];
        fArray7[0] = 6.0f;
        fArray7[2] = -17.0f;
        fArray7[3] = 6.0f;
        fArray7[5] = -14.0f;
        fArray7[6] = 11.0f;
        fArray7[8] = -10.0f;
        fArray7[9] = 6.0f;
        fArray7[11] = -14.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        if (Runtime.getFlags(3186, 1) == 0) {
            this.enemy3 = new Enepc();
            this.enemy3.init(16396, 3, -3.0f, 0.0f, -19.0f, 90.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(8, 8, 8, 8);
            float[] fArray9 = new float[8];
            fArray9[0] = -3.0f;
            fArray9[2] = -19.0f;
            fArray9[3] = -1.0f;
            fArray9[6] = -18.7f;
            float[] fArray10 = fArray9;
            this.enemy3.setParams(1, 2, 3, 3, fArray10);
            this.enemy3.kickEnepc(10, 70, 0);
        }
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 378);
        this.Glass = new Uwamono(6, 40, this.item01);
        this.Glass.SetCallNo(1);
        this.itembox = new Uwamono(28677, 0.0f, 1.0f, -21.5f, 180.0f, 381);
        this.itembox.SetSymbol(28684);
        if (Runtime.getFlags(3086, 1) == 0) {
            this.doorA = new Uwamono(1, 40, '\u0002');
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA = new Uwamono(1, 40, '\u0002');
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
        new Uwamono(28674, -5.2f, 1.0f, -16.4f, 0.0f);
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

