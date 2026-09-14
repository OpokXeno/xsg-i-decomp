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
import xeno.map.MC_GNK02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST3010
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK02_PRJ {
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
    Unit MJele;
    Unit PUPU;
    Unit kanban;
    Unit Kidou0;
    Unit Kidou1;
    Effect light01;
    Effect light02;
    Effect light03;
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
    Uwamono col;
    Effect fade;
    Light light = new Light(0);
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
    String[] SYS_00 = new String[]{"Use the elevator?", "/[waitkey(64)]/[close()]"};

    ST3010() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            block0:
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3132, 1) != 0) break;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Sound.effectPlay(196715);
                            this.kanban.start(1, "nobiA");
                            System.sleep(45);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(30, 0.0f, -2.0f, true);
                            System.sleep(35);
                            this.player.rotY(15, 0.0f, true);
                            System.sleep(20);
                            this.player.mtn(28, 9, 1.0f, true);
                            Sound.effectPlay(196715);
                            this.kanban.start(1, "nobiB");
                            System.sleep(60);
                            this.MJele.start(1, "Down");
                            break block0;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(60, 0.0f, 2.0f, true);
                    System.sleep(60);
                    this.doorA.DoorClose();
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
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
                Runtime.jumpCF(3150, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(68676, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 0.0f, -3.2f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196609);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3133, 1) == 1) {
            this.MJele = new Mapunits();
            this.MJele.mapUnit(1);
            this.MJele.start(4, null);
            this.MJele.setTranslate(this.MJele.px, -33.2f, this.MJele.pz);
        } else {
            this.MJele = new Mapunits();
            this.MJele.mapUnit(1);
            this.MJele.start(4, null);
        }
        if (Runtime.getFlags(3133, 1) == 1) {
            this.kanban = new Mapunits();
            this.kanban.mapUnit(39);
            this.kanban.start(4, null);
            this.kanban.setTranslate(this.kanban.px, -33.2f, this.kanban.pz);
        } else {
            this.kanban = new Mapunits();
            this.kanban.mapUnit(39);
            this.kanban.start(4, null);
        }
        this.col = new Uwamono(28672, 0.0f, -3.2f, -2.0f, 0.0f);
        this.col.SetSize(3.0f, 1.0f, 3.5f);
        if (Runtime.getFlags(3133, 1) == 1) {
            this.Kidou0 = new Mapunits();
            this.Kidou0.mapUnit(24);
            this.Kidou0.start(4, null);
            this.Kidou0.start(1, "Evt");
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
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 3, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 0.0f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, -15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -20.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16405, 3, -8.723f, 0.0f, 4.724f, 180.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray = new float[36];
        fArray[0] = -8.723f;
        fArray[2] = 4.724f;
        fArray[3] = 1.0f;
        fArray[4] = -8.565f;
        fArray[6] = 3.369f;
        fArray[7] = 2.0f;
        fArray[8] = -8.589f;
        fArray[10] = 2.078f;
        fArray[11] = 3.0f;
        fArray[12] = -8.193f;
        fArray[14] = 1.008f;
        fArray[15] = 4.0f;
        fArray[16] = -7.589f;
        fArray[18] = -0.535f;
        fArray[19] = 5.0f;
        fArray[20] = -7.589f;
        fArray[22] = -2.015f;
        fArray[23] = 6.0f;
        fArray[24] = -6.991f;
        fArray[26] = -3.527f;
        fArray[27] = 7.0f;
        fArray[28] = -6.109f;
        fArray[30] = -4.724f;
        fArray[31] = 8.0f;
        fArray[32] = -4.881f;
        fArray[34] = -5.448f;
        fArray[35] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[27];
        fArray3[0] = -8.723f;
        fArray3[2] = 4.724f;
        fArray3[3] = -8.565f;
        fArray3[5] = 3.369f;
        fArray3[6] = -8.589f;
        fArray3[8] = 2.078f;
        fArray3[9] = -8.193f;
        fArray3[11] = 1.008f;
        fArray3[12] = -7.589f;
        fArray3[14] = -0.535f;
        fArray3[15] = -7.589f;
        fArray3[17] = -2.015f;
        fArray3[18] = -6.991f;
        fArray3[20] = -3.527f;
        fArray3[21] = -6.109f;
        fArray3[23] = -4.724f;
        fArray3[24] = -4.881f;
        fArray3[26] = -5.448f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 70, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(16405, 3, -2.74f, 0.0f, -5.7f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[28];
        fArray5[0] = -2.74f;
        fArray5[2] = -5.7f;
        fArray5[3] = 1.0f;
        fArray5[4] = -1.732f;
        fArray5[6] = -5.448f;
        fArray5[7] = 2.0f;
        fArray5[8] = -0.85f;
        fArray5[10] = -5.826f;
        fArray5[11] = 3.0f;
        fArray5[12] = -0.126f;
        fArray5[14] = -5.567f;
        fArray5[15] = 4.0f;
        fArray5[16] = 0.819f;
        fArray5[18] = -5.794f;
        fArray5[19] = 5.0f;
        fArray5[20] = 1.606f;
        fArray5[22] = -5.465f;
        fArray5[23] = 6.0f;
        fArray5[24] = 1.972f;
        fArray5[26] = -5.92f;
        fArray5[27] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 3, fArray6);
        float[] fArray7 = new float[21];
        fArray7[0] = -2.74f;
        fArray7[2] = -5.7f;
        fArray7[3] = -1.732f;
        fArray7[5] = -5.448f;
        fArray7[6] = -0.85f;
        fArray7[8] = -5.826f;
        fArray7[9] = -0.126f;
        fArray7[11] = -5.567f;
        fArray7[12] = 0.819f;
        fArray7[14] = -5.794f;
        fArray7[15] = 1.606f;
        fArray7[17] = -5.465f;
        fArray7[18] = 1.972f;
        fArray7[20] = -5.92f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.kickEnepc(10, 70, 0);
        this.enemy3 = new Enepc();
        this.enemy3.init(16405, 3, 4.157f, 0.0f, -5.7f, 180.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1, 1, 1, 1);
        float[] fArray9 = new float[44];
        fArray9[0] = 4.157f;
        fArray9[2] = -5.7f;
        fArray9[3] = 1.0f;
        fArray9[4] = 5.259f;
        fArray9[6] = -5.164f;
        fArray9[7] = 2.0f;
        fArray9[8] = 6.141f;
        fArray9[10] = -4.377f;
        fArray9[11] = 3.0f;
        fArray9[12] = 7.085f;
        fArray9[14] = -3.495f;
        fArray9[15] = 4.0f;
        fArray9[16] = 7.558f;
        fArray9[18] = -2.425f;
        fArray9[19] = 5.0f;
        fArray9[20] = 7.589f;
        fArray9[22] = -1.291f;
        fArray9[23] = 6.0f;
        fArray9[24] = 7.517f;
        fArray9[26] = 0.059f;
        fArray9[27] = 7.0f;
        fArray9[28] = 8.241f;
        fArray9[30] = 1.191f;
        fArray9[31] = 8.0f;
        fArray9[32] = 8.313f;
        fArray9[34] = 2.488f;
        fArray9[35] = 9.0f;
        fArray9[36] = 8.597f;
        fArray9[38] = 3.464f;
        fArray9[39] = 10.0f;
        fArray9[40] = 8.786f;
        fArray9[42] = 4.755f;
        fArray9[43] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 1, 3, 3, fArray10);
        float[] fArray11 = new float[33];
        fArray11[0] = 4.157f;
        fArray11[2] = -5.7f;
        fArray11[3] = 5.259f;
        fArray11[5] = -5.164f;
        fArray11[6] = 6.141f;
        fArray11[8] = -4.377f;
        fArray11[9] = 7.085f;
        fArray11[11] = -3.495f;
        fArray11[12] = 7.558f;
        fArray11[14] = -2.425f;
        fArray11[15] = 7.589f;
        fArray11[17] = -1.291f;
        fArray11[18] = 7.517f;
        fArray11[20] = 0.059f;
        fArray11[21] = 8.241f;
        fArray11[23] = 1.191f;
        fArray11[24] = 8.313f;
        fArray11[26] = 2.488f;
        fArray11[27] = 8.597f;
        fArray11[29] = 3.464f;
        fArray11[30] = 8.786f;
        fArray11[32] = 4.755f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy3.kickEnepc(10, 70, 0);
        this.doorA = new Uwamono(6, 42, '\u0001');
        new Uwamono(5, 42, '\u0001', this.doorA);
        this.doorA.SetDoorRange(1.74f);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(4, 40, '\u0001');
        new Uwamono(3, 40, '\u0001', this.doorB);
        this.doorB.SetDoorRange(0.74f);
        this.doorB.SetDoorType('\u0004');
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
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
        Mapunits() {
        }

        void Down() {
            int n = 0;
            ST3010.this.kanban.getTranslate();
            ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, -3.2f, ST3010.this.kanban.pz);
            ST3010.this.MJele.getTranslate();
            ST3010.this.MJele.setTranslate(ST3010.this.MJele.px, -3.2f, ST3010.this.MJele.pz);
            ST3010.this.player.getTranslate();
            ST3010.this.player.setTranslate(ST3010.this.player.px, -3.2f, ST3010.this.player.pz);
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196748);
                }
                if (n >= 1 && n < 150) {
                    ST3010.this.kanban.getTranslate();
                    ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, Math.sin((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.kanban.pz);
                    ST3010.this.MJele.getTranslate();
                    ST3010.this.MJele.setTranslate(ST3010.this.MJele.px, Math.sin((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.MJele.pz);
                    ST3010.this.player.getTranslate();
                    ST3010.this.player.setTranslate(ST3010.this.player.px, Math.sin((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.player.pz);
                }
                if (n == 150) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3132, 1, 1);
            Runtime.disable(65536);
            ST3010.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.jumpCF(68556, 0);
        }

        void Evt() {
            ST3010.this.off();
            Runtime.disable(524288);
            ST3010.this.player.setTranslate(0.0f, -33.2f, -2.0f);
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST3010.this.MJele.start(1, "GO");
            System.sleep(165);
            Sound.effectPlay(196715);
            ST3010.this.kanban.start(1, "nobiA");
            System.sleep(45);
            ST3010.this.player.mtn(2, 9, 1.0f, true);
            ST3010.this.player.move(60, 0.0f, 2.0f, true);
            System.sleep(65);
            ST3010.this.player.mtn(28, 9, 1.0f, true);
            Sound.effectPlay(196715);
            ST3010.this.kanban.start(1, "nobiB");
            System.sleep(45);
            Runtime.disable(65536);
            Runtime.setFlags(3133, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
            ST3010.this.on();
        }

        void GO() {
            int n = 0;
            ST3010.this.kanban.getTranslate();
            ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, -38.2f, ST3010.this.kanban.pz);
            ST3010.this.MJele.getTranslate();
            ST3010.this.MJele.setTranslate(ST3010.this.MJele.px, -38.2f, ST3010.this.MJele.pz);
            ST3010.this.player.getTranslate();
            ST3010.this.player.setTranslate(ST3010.this.player.px, -38.2f, ST3010.this.player.pz);
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196749);
                }
                if (n >= 1 && n < 150) {
                    ST3010.this.kanban.getTranslate();
                    ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, Math.cos((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.kanban.pz);
                    ST3010.this.MJele.getTranslate();
                    ST3010.this.MJele.setTranslate(ST3010.this.MJele.px, Math.cos((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.MJele.pz);
                    ST3010.this.player.getTranslate();
                    ST3010.this.player.setTranslate(ST3010.this.player.px, Math.cos((float) n * 3.14f / 300.0f) * 35.0f - 38.2f, ST3010.this.player.pz);
                }
                if (n == 150) break;
                ++n;
                System.sleep(1);
            }
            ST3010.this.kanban.getTranslate();
            ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, -3.2f, ST3010.this.kanban.pz);
            ST3010.this.MJele.getTranslate();
            ST3010.this.MJele.setTranslate(ST3010.this.MJele.px, -3.2f, ST3010.this.MJele.pz);
            ST3010.this.player.getTranslate();
            ST3010.this.player.setTranslate(ST3010.this.player.px, -3.2f, ST3010.this.player.pz);
        }

        void nobiA() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 45) {
                    ST3010.this.kanban.getTranslate();
                    ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, ST3010.this.kanban.py + 0.044444446f, ST3010.this.kanban.pz);
                }
                if (n == 45) break;
                ++n;
                System.sleep(1);
            }
        }

        void nobiB() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 45) {
                    ST3010.this.kanban.getTranslate();
                    ST3010.this.kanban.setTranslate(ST3010.this.kanban.px, ST3010.this.kanban.py - 0.044444446f, ST3010.this.kanban.pz);
                }
                if (n == 45) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

