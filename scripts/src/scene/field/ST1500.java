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
import xeno.map.MC_GNU10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1500
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU10_PRJ {
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono kadanA;
    Uwamono kadanB;
    boolean FlagCheck = false;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Effect fade;
    int page;
    String[] sub_01 = new String[]{"Discovered Segment Address No. 9.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 9.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 9, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST1500() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            Runtime.setFlags(8030, 1, 0);
        }
        if (n == 2) {
            Runtime.setFlags(8031, 1, 0);
            System.println("2");
        }
    }

    void Final_init(int n) {
        this.enemy1.kickEnepc(10, 50, 0);
        this.enemy2.kickEnepc(10, 55, 0);
    }

    public void KickEvent(int n, int n2) {
        if (this.lo == 1) {
            return;
        }
        switch (n) {
            case 100: {
                if (n2 == 0) break;
                if (n2 == 1) {
                    this.player.getTranslate();
                    if (this.player.py < 10.0f) {
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("サブルート扉見つけた");
                    if (Runtime.getFlags(3209, 1) == 0) {
                        Runtime.setFlags(3209, 1, 1);
                        Sound.effectPlay(55);
                        this.nwin(this.sub_01);
                    } else if (Runtime.getFlags(3229, 1) == 0) {
                        this.nwin(this.sub_02);
                    } else if (Runtime.getFlags(3289, 1) == 0) {
                        Sound.effectPlay(56);
                        this.nwin(this.sub_03);
                        this.doorE.SetDoorType('\u0004');
                        Runtime.setFlags(3289, 1, 1);
                    }
                    this.lo = 0;
                    Runtime.setPlayerControl(true);
                    break;
                }
                return;
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
                Runtime.jumpCF(1540, 1);
                break;
            }
            case 1: {
                Runtime.setFlags(8018, 1, 0);
                Runtime.jumpCF(1490, 1);
                break;
            }
            case 2: {
                Runtime.setFlags(8018, 1, 0);
                Runtime.jumpCF(1490, 2);
                break;
            }
            case 3: {
                Runtime.setFlags(8018, 1, 0);
                Runtime.jumpCF(1490, 3);
                break;
            }
            case 4: {
                Runtime.setFlags(8018, 1, 0);
                Runtime.jumpCF(1490, 4);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -4.0f, 0.0f, 10.0f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, -1.0f, 0.0f, 10.0f, 0.0f);
        this.teiten2.SetBgm(196618);
        this.teiten3 = new Uwamono(28690, 2.0f, 0.0f, 10.0f, 0.0f);
        this.teiten3.SetBgm(196618);
        this.teiten4 = new Uwamono(28690, 5.0f, 0.0f, 10.0f, 0.0f);
        this.teiten4.SetBgm(196618);
        this.teiten5 = new Uwamono(28690, -9.0f, 0.0f, -11.0f, 0.0f);
        this.teiten5.SetBgm(196619);
        this.teiten6 = new Uwamono(28690, 0.0f, 0.0f, -11.8f, 0.0f);
        this.teiten6.SetBgm(196619);
        Stage.setColor(1.2f, 1.2f, 1.2f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(11, false);
        if (Runtime.getFlags(6040, 1) == 1) {
            Stage.setVisible(7, false);
            Runtime.setFlags(6040, 1, 0);
        }
        Stage.setVisible(78, false);
        Stage.setVisible(86, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 345.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 15.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFPedestal(5, 9.592087f, 22.071644f, 10.425791f, 45.75959f, -79.38116f, 355.3305f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.03f, 0.03f);
        if (Runtime.getFlags(8030, 1) == 0) {
            float[] fArray2 = new float[8];
            fArray2[0] = 6.1f;
            fArray2[2] = 1.45f;
            fArray2[3] = -1.0f;
            fArray2[4] = 8.1f;
            fArray2[6] = 1.45f;
            fArray = fArray2;
            this.enemy1 = new NpcEnemy(16397, 1, 3, 31, 3, 6.1f, 0.0f, 1.45f, 90.0f, fArray);
            this.enemy1.kickEnepc(10, 50, 0);
            this.enemy1.setGroup(3, 3, 3, 3);
        }
        if (Runtime.getFlags(8031, 1) == 0) {
            float[] fArray3 = new float[8];
            fArray3[0] = -2.0f;
            fArray3[2] = 1.45f;
            fArray3[3] = -1.0f;
            fArray3[6] = 1.45f;
            fArray = fArray3;
            this.enemy2 = new NpcEnemy(16397, 2, 2, 31, 3, -2.0f, 0.0f, 1.45f, 180.0f, fArray);
            this.enemy2.kickEnepc(10, 55, 0);
            this.enemy2.setGroup(4, 4, 4, 4);
        }
        float[] fArray4 = new float[20];
        fArray4[0] = -0.3f;
        fArray4[2] = -4.4f;
        fArray4[3] = -1.0f;
        fArray4[4] = -2.8f;
        fArray4[6] = -4.7f;
        fArray4[8] = -2.8f;
        fArray4[10] = -8.8f;
        fArray4[11] = 1.0f;
        fArray4[12] = 2.8f;
        fArray4[14] = -4.7f;
        fArray4[16] = 2.8f;
        fArray4[18] = -8.8f;
        fArray4[19] = 3.0f;
        fArray = fArray4;
        this.enemy3 = new NpcEnemy(16398, 3, 1, 3, 5, -3.0f, 0.0f, -4.4f, 180.0f, fArray);
        this.enemy3.setGroup(0, 0, 1, 2);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 189);
        this.kadanA = new Uwamono(119, 68);
        this.kadanA.SetDiffSize(-0.5f, 0.0f, -0.5f);
        this.kadanB = new Uwamono(120, 68, this.item1);
        this.kadanB.SetDiffSize(-0.5f, 0.0f, -0.5f);
        this.item4 = new Uwamono(28677, 5.8f, 0.0f, 6.0f, 0.0f, 197);
        this.item4.SetSymbol(28684);
        new Uwamono(28673, -7.0f, 0.0f, -5.5f, 0.0f);
        new Uwamono(28674, 7.0f, 0.0f, -5.5f, 0.0f);
        this.doorA = new Uwamono(66, 40, '\u0001');
        this.doorB = new Uwamono(67, 40, '\u0001');
        this.doorC = new Uwamono(65, 40, '\u0001');
        this.doorD = new Uwamono(10, 40, '\u0001');
        this.doorE = new Uwamono(87, 40, '\u0004');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        if (Runtime.getFlags(3289, 1) == 0) {
            this.doorE.SetDoorType('\u0002');
        } else {
            this.doorE.SetDoorType('\u0004');
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1500.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1500.waitPage(this.win, 64);
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }
}

