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
import xeno.map.MC_KAS03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2420
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS03_PRJ {
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
    MAPUnit start;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Effect fire06;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono box01;
    Uwamono box02;
    Uwamono box03;
    Uwamono box04;
    Uwamono box05;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    boolean e1_move = false;
    boolean e3_move = false;
    boolean e4_move = false;
    float time_e1 = 0.0f;
    float time_e3 = 0.0f;
    float time_e4 = 0.0f;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    int page;

    ST2420() {
    }

    void EOB(int n) {
        if (n == 0) {
            System.println("EOB:0");
        }
        if (n == 1) {
            this.light01.disp(false);
        }
        if (n == 2) {
            this.light02.disp(false);
        }
        if (n == 3) {
            this.light03.disp(false);
        }
        if (n == 4) {
            this.light04.disp(false);
        }
        if (n == 5) {
            this.light05.disp(false);
        }
    }

    void EOB_Escape(int n) {
        System.println("EOB_Escape");
        if (n == 1) {
            this.enemy1.kickEnepc(7, 7);
        }
        if (n == 2) {
            this.enemy2.kickEnepc(7, 7);
        }
        if (n == 3) {
            this.enemy3.kickEnepc(7, 7);
        }
        if (n == 4) {
            this.enemy4.kickEnepc(7, 7);
        }
        if (n == 5) {
            this.enemy5.kickEnepc(7, 7);
        }
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
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken:1");
                this.enemy1.kickEnepc(7, 8);
                this.enemy1.kickEnepc(4, 1);
                this.enemy1.moveEnepc(15, 2.8f, -8.5f, 30);
                this.time_e1 = 0.0f;
                this.e1_move = true;
                break;
            }
            case 2: {
                System.println("broken:2");
                this.enemy3.kickEnepc(7, 8);
                this.enemy3.kickEnepc(4, 1);
                this.enemy3.moveEnepc(15, 14.7f, 0.9f, 30);
                this.time_e3 = 0.0f;
                this.e3_move = true;
                break;
            }
            case 3: {
                System.println("broken:3");
                this.enemy4.kickEnepc(7, 8);
                this.enemy4.kickEnepc(4, 1);
                this.enemy4.moveEnepc(15, 20.3f, -17.4f, 30);
                this.time_e4 = 0.0f;
                this.e4_move = true;
                break;
            }
            case 4: {
                System.println("broken:4");
                this.enemy4.kickEnepc(7, 8);
                this.enemy4.kickEnepc(4, 1);
                this.enemy4.moveEnepc(15, 25.6f, -11.0f, 30);
                this.time_e4 = 0.0f;
                this.e4_move = true;
                break;
            }
            case 5: {
                System.println("broken:5");
                this.enemy4.kickEnepc(7, 8);
                this.enemy4.kickEnepc(4, 1);
                this.enemy4.moveEnepc(15, 29.6f, -17.6f, 30);
                this.time_e4 = 0.0f;
                this.e4_move = true;
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
                if (Runtime.getFlags(331, 1) == 0) {
                    Runtime.setFlags(331, 1, 1);
                    System.println("イベント3020a - 3021");
                    Runtime.jumpEvent(3200);
                }
                Runtime.jumpCF(2430, 1);
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
        this.start = new Mapunits();
        this.start.mapUnit(70);
        this.start.start(4, null);
        Stage.setColor(1.5f, 1.5f, 1.5f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1557, 5.2f, 10.0f, -4.0f, 0.0f);
        this.light01.disp(true);
        this.light01.setClip(false);
        this.light02 = new Effect(1557, 3.5f, 10.0f, 4.25f, 0.0f);
        this.light02.disp(true);
        this.light02.setClip(false);
        this.light03 = new Effect(1557, 15.0f, 10.0f, 2.4f, 0.0f);
        this.light03.disp(true);
        this.light03.setClip(false);
        this.light04 = new Effect(1557, 25.0f, 10.0f, -14.0f, 0.0f);
        this.light04.disp(true);
        this.light04.setClip(false);
        this.light05 = new Effect(1557, 29.5f, 10.0f, 2.0f, 0.0f);
        this.light05.disp(true);
        this.light05.setClip(false);
        this.fire01 = new Effect(1402, -9.2f, 0.0f, 3.1f, 0.0f);
        this.fire02 = new Effect(1402, -12.8f, 0.0f, -16.0f, 0.0f);
        this.fire02.setScale(5.0f, 1.0f, 1.0f);
        this.fire03 = new Effect(1402, 28.3f, 1.0f, -24.2f, 0.0f);
        this.fire03.setScale(3.0f, 2.0f, 3.0f);
        this.fire04 = new Effect(1402, 15.0f, 0.5f, -15.0f, 0.0f);
        this.fire04.setRotate(0.0f, 45.0f, 0.0f);
        this.fire05 = new Effect(1402, 36.7f, 0.0f, 0.3f, 0.0f);
        this.fire06 = new Effect(1402, 13.0f, 0.0f, -13.5f, 0.0f);
        this.fire06.setScale(1.8f, 1.5f, 1.5f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(8069, 1) == 0) {
            Runtime.setFlags(8069, 1, 1);
            Runtime.setOutFriend(1);
            Runtime.setOutFriend(3);
            Runtime.setPartyData(0x1010000, 5);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 6);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 7);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 5);
        }
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFPedestal(1, 27.469833f, 14.903844f, 1.4070365f, 39.35991f, -49.323887f, 8.779945f, 0.0f, 2.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        float[] fArray = new float[12];
        fArray[0] = 5.2f;
        fArray[2] = -4.0f;
        fArray[3] = -1.0f;
        fArray[4] = 3.6f;
        fArray[5] = 3.0f;
        fArray[6] = -7.7f;
        fArray[8] = 6.9f;
        fArray[9] = 3.0f;
        fArray[10] = -0.3f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16394, 1, 1, 7, 3, 5.2f, 0.0f, -4.0f, 0.0f, fArray2);
        this.light01.setTarget(this.enemy1);
        float[] fArray3 = new float[12];
        fArray3[0] = 5.2f;
        fArray3[2] = -4.0f;
        fArray3[3] = 3.6f;
        fArray3[4] = 3.0f;
        fArray3[5] = -7.7f;
        fArray3[6] = 5.2f;
        fArray3[8] = -4.0f;
        fArray3[9] = 6.9f;
        fArray3[10] = 3.0f;
        fArray3[11] = -0.3f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray5 = new float[12];
        fArray5[0] = 3.5f;
        fArray5[2] = 4.25f;
        fArray5[3] = -1.0f;
        fArray5[4] = -2.5f;
        fArray5[6] = 4.25f;
        fArray5[8] = 8.0f;
        fArray5[10] = 4.25f;
        float[] fArray6 = fArray5;
        this.enemy2 = new NpcEnemy(16394, 2, 2, 7, 3, 3.5f, 0.0f, 4.25f, 0.0f, fArray6);
        this.light02.setTarget(this.enemy2);
        float[] fArray7 = new float[12];
        fArray7[0] = 3.5f;
        fArray7[2] = 4.25f;
        fArray7[3] = -2.5f;
        fArray7[5] = 4.25f;
        fArray7[6] = 3.5f;
        fArray7[8] = 4.25f;
        fArray7[9] = 8.0f;
        fArray7[11] = 4.25f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.setGroup(0, 0, 0, 0);
        float[] fArray9 = new float[12];
        fArray9[0] = 15.94f;
        fArray9[2] = 1.984f;
        fArray9[3] = -1.0f;
        fArray9[4] = 17.5f;
        fArray9[6] = -2.5f;
        fArray9[8] = 14.6f;
        fArray9[10] = 6.1f;
        float[] fArray10 = fArray9;
        this.enemy3 = new NpcEnemy(16394, 3, 3, 7, 3, 15.94f, 0.0f, 1.984f, 0.0f, fArray10);
        this.light03.setTarget(this.enemy3);
        float[] fArray11 = new float[12];
        fArray11[0] = 15.94f;
        fArray11[2] = 1.984f;
        fArray11[3] = 17.5f;
        fArray11[5] = -2.5f;
        fArray11[6] = 15.94f;
        fArray11[8] = 1.984f;
        fArray11[9] = 14.6f;
        fArray11[11] = 6.1f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy3.setGroup(0, 0, 0, 0);
        float[] fArray13 = new float[12];
        fArray13[0] = 25.5f;
        fArray13[2] = -11.3f;
        fArray13[3] = -1.0f;
        fArray13[4] = 28.6f;
        fArray13[6] = -16.7f;
        fArray13[8] = 21.3f;
        fArray13[10] = -16.7f;
        float[] fArray14 = fArray13;
        this.enemy4 = new NpcEnemy(16394, 4, 4, 7, 3, 25.0f, 0.0f, -11.3f, 0.0f, fArray14);
        this.light04.setTarget(this.enemy4);
        float[] fArray15 = new float[9];
        fArray15[0] = 25.5f;
        fArray15[2] = -11.3f;
        fArray15[3] = 28.6f;
        fArray15[5] = -16.7f;
        fArray15[6] = 21.3f;
        fArray15[8] = -16.7f;
        float[] fArray16 = fArray15;
        this.enemy4.setParams(fArray16);
        this.enemy4.setGroup(0, 0, 0, 0);
        float[] fArray17 = new float[12];
        fArray17[0] = 29.5f;
        fArray17[2] = 2.0f;
        fArray17[3] = -1.0f;
        fArray17[4] = 26.0f;
        fArray17[6] = 5.0f;
        fArray17[8] = 33.0f;
        fArray17[10] = -1.0f;
        float[] fArray18 = fArray17;
        this.enemy5 = new NpcEnemy(16394, 5, 5, 7, 3, 29.5f, 0.0f, 2.0f, 0.0f, fArray18);
        this.light05.setTarget(this.enemy5);
        float[] fArray19 = new float[12];
        fArray19[0] = 29.5f;
        fArray19[2] = 2.0f;
        fArray19[3] = 26.0f;
        fArray19[5] = 5.0f;
        fArray19[6] = 29.5f;
        fArray19[8] = 2.0f;
        fArray19[9] = 33.0f;
        fArray19[11] = -1.0f;
        float[] fArray20 = fArray19;
        this.enemy5.setParams(fArray20);
        this.enemy5.setGroup(0, 0, 0, 0);
        this.enemy1.kickEnepc(10, 1, 0);
        this.enemy2.kickEnepc(10, 1, 0);
        this.enemy3.kickEnepc(10, 1, 0);
        this.enemy4.kickEnepc(10, 1, 0);
        this.enemy5.kickEnepc(10, 1, 0);
        this.enemy1.setTP(0);
        this.enemy2.setTP(0);
        this.enemy3.setTP(0);
        this.enemy4.setTP(0);
        this.enemy5.setTP(0);
        this.enemy1.dispRadar(false);
        this.enemy2.dispRadar(false);
        this.enemy3.dispRadar(false);
        this.enemy4.dispRadar(false);
        this.enemy5.dispRadar(false);
        new Uwamono(28678, -15.0f, 0.0f, -7.4f);
        this.item4 = new Uwamono(28677, 16.6f, 0.0f, 6.3f, 180.0f, 314);
        this.item4.SetSymbol(28684);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 276);
        this.item2 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 277);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 278);
        this.box01 = new Uwamono(18, 12);
        this.box01.SetCallNo(1);
        this.box02 = new Uwamono(19, 12);
        this.box02.SetCallNo(2);
        this.box03 = new Uwamono(20, 9, this.item1);
        this.box03.SetCallNo(3);
        this.box04 = new Uwamono(21, 9, this.item2);
        this.box04.SetCallNo(4);
        this.box05 = new Uwamono(22, 9, this.item3);
        this.box05.SetCallNo(5);
        this.start.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2420.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2420.waitPage(this.win, 64);
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

        void idle() {
            while (true) {
                if (ST2420.this.e1_move) {
                    if (ST2420.this.time_e1 == 90.0f) {
                        ST2420.this.enemy1.moveEnepc(15, 5.2f, -4.0f, 30);
                    }
                    if (ST2420.this.time_e1 == 120.0f) {
                        ST2420.this.enemy1.kickEnepc(4, 0);
                        ST2420.this.enemy1.kickEnepc(7, 7);
                        ST2420.this.e1_move = false;
                        ST2420.this.time_e1 = 0.0f;
                    }
                    ST2420.this.time_e1 += 1.0f;
                }
                if (ST2420.this.e3_move) {
                    if (ST2420.this.time_e3 == 90.0f) {
                        ST2420.this.enemy3.moveEnepc(15, 15.94f, 1.984f, 30);
                    }
                    if (ST2420.this.time_e3 == 120.0f) {
                        ST2420.this.enemy3.kickEnepc(4, 0);
                        ST2420.this.enemy3.kickEnepc(7, 7);
                        ST2420.this.e3_move = false;
                        ST2420.this.time_e3 = 0.0f;
                    }
                    ST2420.this.time_e3 += 1.0f;
                }
                if (ST2420.this.e4_move) {
                    if (ST2420.this.time_e4 == 90.0f) {
                        ST2420.this.enemy4.moveEnepc(15, 25.5f, -11.3f, 30);
                    }
                    if (ST2420.this.time_e4 == 120.0f) {
                        ST2420.this.enemy4.kickEnepc(4, 0);
                        ST2420.this.enemy4.kickEnepc(7, 7);
                        ST2420.this.e4_move = false;
                        ST2420.this.time_e4 = 0.0f;
                    }
                    ST2420.this.time_e4 += 1.0f;
                }
                System.sleep(1);
            }
        }
    }
}

