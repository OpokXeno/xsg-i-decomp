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
import xeno.map.MC_KAS20_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2540
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS20_PRJ {
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
    Enepc HASHIGO;
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
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono box1;
    Uwamono box2;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    MAPUnit stone1;
    boolean rakka_stone = false;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    int page;

    ST2540() {
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
                if (Runtime.getFlags(8063, 1) != 0) break;
                this.rakka_stone = true;
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
                Runtime.jumpCF(2539, 5);
                break;
            }
            case 1: {
                Runtime.jumpCF(2539, 6);
                break;
            }
            case 2: {
                Runtime.jumpCF(2489, 3);
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
        this.stone1 = new Mapunits();
        this.stone1.mapUnit(151);
        this.stone1.start(4, null);
        this.teiten1 = new Uwamono(28690, -30.0f, 0.0f, 3.5f);
        this.teiten1.SetBgm(196617);
        this.teiten2 = new Uwamono(28690, -30.0f, 0.0f, -3.5f);
        this.teiten2.SetBgm(196617);
        this.teiten3 = new Uwamono(28690, 23.0f, 0.0f, 4.0f);
        this.teiten3.SetBgm(196618);
        Stage.setColor(1.15f, 1.15f, 1.15f);
        this.light.setColor(0, 0.325f, 0.325f, 0.325f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.325f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.325f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.325f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.325f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(4, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(4, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(4, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1402, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light01.noAttach(false);
        this.light01.disp(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 20.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, -15.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFPedestal(7, 16.918606f, 15.047904f, -4.412704f, 40.0f, -47.785645f, 37.559807f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, -10.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
        float[] fArray = new float[12];
        fArray[0] = -16.5f;
        fArray[2] = 0.1f;
        fArray[3] = -1.0f;
        fArray[4] = -16.0f;
        fArray[6] = 0.1f;
        fArray[8] = -17.0f;
        fArray[10] = 0.1f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16401, 1, 1, 19, 3, -16.5f, 0.0f, 0.1f, 270.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[12];
        fArray3[0] = 19.5f;
        fArray3[2] = 2.1f;
        fArray3[3] = -1.0f;
        fArray3[4] = 20.25f;
        fArray3[6] = 1.1f;
        fArray3[8] = 19.5f;
        fArray3[10] = 3.75f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16401, 2, 2, 19, 3, 19.5f, 0.0f, 2.1f, 0.0f, fArray4);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[12];
        fArray5[0] = -4.9f;
        fArray5[1] = 8.0f;
        fArray5[2] = -9.7f;
        fArray5[3] = -1.0f;
        fArray5[4] = -5.4f;
        fArray5[5] = 8.0f;
        fArray5[6] = -9.7f;
        fArray5[8] = -4.5f;
        fArray5[9] = 8.0f;
        fArray5[10] = -9.7f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16392, 3, 3, 36, 5, -4.9f, 8.0f, -9.7f, 90.0f, fArray6);
        this.enemy3.setGroup(2, 2, 2, 2);
        float[] fArray7 = new float[12];
        fArray7[0] = 1.9f;
        fArray7[2] = -5.8f;
        fArray7[3] = -1.0f;
        fArray7[4] = 1.9f;
        fArray7[6] = -9.6f;
        fArray7[8] = -2.5f;
        fArray7[10] = -6.5f;
        float[] fArray8 = fArray7;
        this.enemy4 = new NpcEnemy(16401, 4, 4, 19, 3, 1.9f, 0.0f, -5.8f, 270.0f, fArray8);
        this.enemy4.setGroup(1, 1, 1, 1);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 303);
        new Uwamono(10, 39, this.item3);
        if (Runtime.getFlags(8063, 1) == 0) {
            this.box1 = new Uwamono(9, 1);
            this.box1.SetCallNo(1);
            this.box2 = new Uwamono(73, 91);
            this.item1 = new Uwamono(28677, -17.7f, 3.4f, -8.3f, 135.0f, 318);
            this.item1.SetSymbol(28684);
        } else {
            Stage.setVisible(9, false);
            Stage.setVisible(73, false);
            this.item1 = new Uwamono(28677, -17.7f, 1.12f, -8.3f, 135.0f, 318);
            this.item1.SetSymbol(28684);
            this.stone1.setTranslate(0.0f, -100.0f, 0.0f);
        }
        this.enemy4.kickEnepc(19, 1, 0, 840, 1);
        this.stone1.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2540.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2540.waitPage(this.win, 64);
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
            ST2540.this.stone1.getTranslate();
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = (-16.7f - ST2540.this.stone1.px) / 30.0f;
            float f4 = (-8.2f - ST2540.this.stone1.pz) / 30.0f;
            boolean bl = false;
            while (true) {
                if (ST2540.this.rakka_stone) {
                    f2 = (-9.8f * f / 30.0f + 5.0f) * f / 30.0f + 10.0f;
                    Runtime.setPlayerControl(false);
                    if (f == 0.0f) {
                        ST2540.this.cam0.setMode(-1);
                        ST2540.this.camEV = Camera.create(1);
                        ST2540.this.camEV.setRotate(-21.7f, -54.5f, 0.0f);
                        ST2540.this.camEV.setTranslate(-28.7f, 12.6f, 2.0f);
                        ST2540.this.camEV.setFov(40.0f);
                        ST2540.this.camEV.change();
                        ST2540.this.light01.disp(true);
                    }
                    if (f < 30.0f) {
                        ST2540.this.stone1.setTranslate(ST2540.this.stone1.px + f3, f2, ST2540.this.stone1.pz + f4);
                        ST2540.this.stone1.setRotate(f * 3.0f, -f * 4.0f, -f * 5.0f);
                        ST2540.this.light01.setTranslate(ST2540.this.stone1.px + f3, f2, ST2540.this.stone1.pz + f4);
                    } else if (f2 >= 1.0f) {
                        ST2540.this.stone1.setTranslate(ST2540.this.stone1.px, f2, ST2540.this.stone1.pz);
                        ST2540.this.stone1.setRotate(f * 3.0f, -f * 4.0f, -f * 5.0f);
                        ST2540.this.light01.setTranslate(ST2540.this.stone1.px, f2, ST2540.this.stone1.pz);
                        Runtime.setRegister(1, f);
                        System.println("time: /[#1]");
                    } else {
                        ST2540.this.stone1.setTranslate(ST2540.this.stone1.px, 1.0f, ST2540.this.stone1.pz);
                        ST2540.this.light01.setTranslate(ST2540.this.stone1.px, 1.0f, ST2540.this.stone1.pz);
                    }
                    if (f2 <= 3.6f && !bl) {
                        ST2540.this.box2.SendSignal();
                        ST2540.this.item1.SetGravity(true);
                        bl = true;
                    }
                    if (f == 60.0f) {
                        float[] fArray = new float[8];
                        fArray[1] = -21.7f;
                        fArray[2] = -54.5f;
                        fArray[4] = 60.0f;
                        fArray[5] = -30.7f;
                        fArray[6] = -54.5f;
                        float[] fArray2 = fArray;
                        ST2540.this.camEV.rotateSPL(fArray2, 0, 1, 60);
                    }
                    if (f == 150.0f) break;
                    f += 1.0f;
                }
                System.sleep(1);
            }
            Runtime.setFlags(8063, 1, 1);
            Runtime.setPlayerControl(true);
            ST2540.this.rakka_stone = false;
            ST2540.this.cam0.setMode(0);
            ST2540.this.light01.disp(false);
        }
    }
}

