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
import xeno.map.MC_GNU07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1470
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU07_PRJ {
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
    Uwamono doorA;
    Uwamono boxA;
    Uwamono boxB;
    Uwamono boxC;
    Uwamono boxD;
    Uwamono t01;
    Uwamono t02;
    Uwamono lift;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Uwamono item7;
    Uwamono item8;
    Uwamono item9;
    Uwamono item10;
    Uwamono item11;
    Uwamono item12;
    Uwamono item13;
    Uwamono item14;
    Uwamono item15;
    Uwamono item16;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int lo = 0;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Effect fade;
    int page;
    String[] sub_01 = new String[]{"Discovered Segment Address No. 13.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 13.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 13, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST1470() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 2) {
            Runtime.setFlags(8028, 1, 0);
            System.println("2");
        }
        if (n == 3) {
            Runtime.setFlags(8029, 1, 0);
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
                this.player.getTranslate();
                if (this.player.py > 2.0f) {
                    return;
                }
                Runtime.enable(262144);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3213, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3213, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3233, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3293, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3293, 1, 1);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
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
                Runtime.setFlags(8005, 1, 1);
                Runtime.jumpCF(1530, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1460, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(1460, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(1460, 3);
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
        this.teiten1 = new Uwamono(28690, -11.0f, 4.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196614);
        this.teiten2 = new Uwamono(28690, 0.0f, 4.0f, -11.0f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 11.0f, 4.0f, -2.0f, 0.0f);
        this.teiten3.SetBgm(196614);
        Stage.setColor(1.2f, 1.2f, 1.2f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
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
        Stage.setVisible(0, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        float[] fArray2 = new float[]{-4.334667f, 4.0f, -9.094129f, 1.0f, 3.7f, 4.0f, -9.3f, -1.0f};
        this.enemy1 = new NpcEnemy(16394, 1, 1, 13, 3, -0.2f, 4.0f, -9.7f, 0.0f, fArray2);
        this.enemy1.setGroup(2, 2, 3, 3);
        if (Runtime.getFlags(8029, 1) == 0) {
            float[] fArray3 = new float[8];
            fArray3[0] = -3.5f;
            fArray3[2] = -2.0f;
            fArray3[3] = 1.0f;
            fArray3[4] = -3.1f;
            fArray3[6] = -2.0f;
            fArray3[7] = -1.0f;
            fArray = fArray3;
            this.enemy3 = new NpcEnemy(16399, 3, 2, 34, 5, -3.1f, 0.0f, -2.0f, 0.0f, fArray);
            this.enemy3.setGroup(1, 1, 1, 1);
            this.enemy3.kickEnepc(10, 50, 0);
        }
        if (Runtime.getFlags(8028, 1) == 0) {
            float[] fArray4 = new float[8];
            fArray4[0] = 2.5f;
            fArray4[2] = -2.2f;
            fArray4[3] = 1.0f;
            fArray4[4] = 2.9f;
            fArray4[6] = -2.2f;
            fArray4[7] = -1.0f;
            fArray = fArray4;
            this.enemy2 = new NpcEnemy(16399, 2, 2, 34, 5, 2.9f, 0.0f, -2.2f, 0.0f, fArray);
            this.enemy2.setGroup(0, 0, 0, 0);
            this.enemy2.kickEnepc(10, 50, 0);
        }
        this.t01 = new Uwamono(28673, 4.1f, 2.0f, 2.5f, 0.0f);
        this.t01.SetGravity(true);
        this.t02 = new Uwamono(28673, 4.1f, 0.0f, 2.5f, 0.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 169);
        this.item2 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 170);
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 171);
        this.item4 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 172);
        this.item5 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 173);
        this.item6 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 174);
        this.item7 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 175);
        this.item8 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 176);
        this.item9 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 177);
        this.item10 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 178);
        this.item11 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 179);
        this.item12 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 180);
        this.item13 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 181);
        this.item14 = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 195);
        this.item14.SetSymbol(28672);
        if (Runtime.getFlags(3213, 1) == 0) {
            this.boxD = new Uwamono(34, 4);
            this.boxC = new Uwamono(12, 4);
            this.boxC.SetGravity(true);
        } else {
            Stage.setVisible(12, false);
            Stage.setVisible(34, false);
        }
        this.boxA = new Uwamono(10, 4, this.item14);
        this.boxA.SetGravity(true);
        this.boxB = new Uwamono(11, 4, this.item13);
        this.boxB.SetGravity(true);
        new Uwamono(13, 4);
        new Uwamono(14, 4, this.item12);
        new Uwamono(15, 4);
        new Uwamono(16, 4);
        new Uwamono(17, 4, this.item11);
        new Uwamono(18, 4);
        new Uwamono(19, 4, this.item10);
        new Uwamono(20, 4);
        new Uwamono(21, 4, this.item9);
        new Uwamono(22, 4);
        new Uwamono(23, 4);
        new Uwamono(24, 4, this.item8);
        new Uwamono(25, 4);
        new Uwamono(26, 4, this.item7);
        new Uwamono(27, 4, this.t02);
        new Uwamono(28, 4);
        new Uwamono(29, 4);
        new Uwamono(30, 4, this.item6);
        new Uwamono(31, 4, this.item5);
        new Uwamono(32, 4);
        new Uwamono(33, 4);
        new Uwamono(35, 4, this.item4);
        new Uwamono(36, 0, this.item3);
        new Uwamono(37, 0);
        new Uwamono(38, 0, this.item2);
        new Uwamono(39, 0);
        this.lift = new Uwamono(40, 36, this.item1);
        this.lift.SetDiffSize(2.0f, 1.0f, 1.0f);
        this.doorA = new Uwamono(1, 40, '\u0004');
        if (Runtime.getFlags(3293, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1470.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1470.waitPage(this.win, 64);
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
}

