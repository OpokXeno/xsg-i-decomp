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
import xeno.map.MC_UTA10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2800
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA10_PRJ {
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
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Unit unit1;
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
    Uwamono itembox;
    Uwamono item1;
    Uwamono U1;
    Uwamono U2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    MAPUnit yuka;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] momo_0 = new String[]{"/[label(MOMO)]", "??", "/[waitkey(64)]/[close()]"};
    String[] momo_1 = new String[]{"/[label(MOMO)]", "Oh?! This is combat armor exclusively for the 100-Series Observational Units!!", "/[waitkey(1)]/[clear()]", "I'd heard of it, but I never thought it actually existed!!", "/[waitkey(64)]/[close()]"};
    String[] sys_1 = new String[]{"Learned Tech Attack, \"MOMO's Kiss.\"", "/[waitkey(64)]/[close()]"};
    String[] sys_2 = new String[]{"Learned Ether, \"Starlight.\"", "/[waitkey(64)]/[close()]"};

    ST2800() {
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
                if (Runtime.getFlags(8113, 1) != 0) break;
                System.println("変身セット");
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (Runtime.getLeader() == 4) {
                    Runtime.setFlags(8113, 1, 1);
                    this.nwin(this.momo_0);
                    Runtime.etherTecSet(6);
                    this.nwin(this.momo_1);
                    Sound.effectPlay(6);
                    this.nwin(this.sys_1);
                    Sound.effectPlay(6);
                    this.nwin(this.sys_2);
                    this.light01.disp(false);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
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
                Runtime.jumpCF(68326, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(68326, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(68326, 3);
                break;
            }
            case 3: {
                Runtime.jumpCF(68326, 6);
                break;
            }
            case 4: {
                Runtime.jumpCF(68326, 8);
                break;
            }
            case 5: {
                Runtime.jumpCF(68326, 9);
                break;
            }
            case 6: {
                Runtime.jumpCF(68326, 10);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Stage.setVisible(-1, true);
        this.light02 = new Effect(1586, 0.0f, -13.3f, -18.8f, 0.0f);
        this.light01 = new Effect(1018, 19.4f, 31.0f, -14.5f, 0.0f);
        this.light01.disp(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 15.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, -15.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 10.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, -10.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFAngle(14, -28.0f, 15.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        float[] fArray = new float[12];
        fArray[0] = -14.0f;
        fArray[2] = -16.0f;
        fArray[3] = -1.0f;
        fArray[4] = -9.4f;
        fArray[6] = -21.4f;
        fArray[8] = -14.0f;
        fArray[10] = -16.5f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(18433, 1, 1, 9, 3, -14.0f, 0.0f, -16.0f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[12];
        fArray3[0] = 14.0f;
        fArray3[2] = -16.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = 9.4f;
        fArray3[6] = -21.4f;
        fArray3[8] = 14.5f;
        fArray3[10] = -16.0f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(18433, 2, 2, 9, 3, 14.0f, 0.0f, -16.0f, 0.0f, fArray4);
        this.enemy2.setGroup(0, 1, 1, 1);
        float[] fArray5 = new float[8];
        fArray5[1] = 10.0f;
        fArray5[2] = -22.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -9.1f;
        fArray5[5] = 10.0f;
        fArray5[6] = -22.0f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(18433, 3, 3, 9, 3, -9.0f, 10.0f, -22.0f, 0.0f, fArray6);
        this.enemy3.setGroup(2, 2, 3, 3);
        float[] fArray7 = new float[8];
        fArray7[0] = 9.0f;
        fArray7[1] = 10.0f;
        fArray7[2] = -22.0f;
        fArray7[3] = -1.0f;
        fArray7[4] = 9.4f;
        fArray7[5] = 10.0f;
        fArray7[6] = -21.4f;
        float[] fArray8 = fArray7;
        this.enemy4 = new NpcEnemy(16644, 4, 4, 4, 5, 9.0f, 10.0f, -22.0f, 0.0f, fArray8);
        this.enemy4.setGroup(2, 3, 4, 4);
        float[] fArray9 = new float[8];
        fArray9[0] = -9.0f;
        fArray9[1] = 20.0f;
        fArray9[2] = -22.0f;
        fArray9[3] = -1.0f;
        fArray9[4] = -9.4f;
        fArray9[5] = 20.0f;
        fArray9[6] = -21.4f;
        float[] fArray10 = fArray9;
        this.enemy5 = new NpcEnemy(16644, 5, 5, 4, 5, -9.0f, 20.0f, -22.0f, 0.0f, fArray10);
        this.enemy5.setGroup(5, 5, 5, 5);
        float[] fArray11 = new float[8];
        fArray11[0] = 17.0f;
        fArray11[1] = 20.0f;
        fArray11[2] = -14.0f;
        fArray11[3] = -1.0f;
        fArray11[4] = 8.6f;
        fArray11[5] = 20.0f;
        fArray11[6] = -21.7f;
        float[] fArray12 = fArray11;
        this.enemy6 = new NpcEnemy(16648, 6, 6, 14, 7, 17.0f, 20.0f, -14.0f, 0.0f, fArray12);
        this.enemy6.setGroup(6, 6, 7, 7);
        float[] fArray13 = new float[12];
        fArray13[0] = -17.0f;
        fArray13[1] = 30.0f;
        fArray13[2] = -14.0f;
        fArray13[3] = -1.0f;
        fArray13[4] = -12.9f;
        fArray13[5] = 30.0f;
        fArray13[6] = -17.7f;
        fArray13[8] = -14.0f;
        fArray13[9] = 30.0f;
        fArray13[10] = -16.5f;
        float[] fArray14 = fArray13;
        this.enemy7 = new NpcEnemy(18433, 7, 7, 9, 3, -17.0f, 30.0f, -14.0f, 0.0f, fArray14);
        this.enemy7.setGroup(8, 8, 8, 8);
        float[] fArray15 = new float[12];
        fArray15[1] = 30.0f;
        fArray15[2] = -22.0f;
        fArray15[3] = -1.0f;
        fArray15[4] = 9.4f;
        fArray15[5] = 30.0f;
        fArray15[6] = -21.4f;
        fArray15[8] = 0.5f;
        fArray15[9] = 30.0f;
        fArray15[10] = -22.0f;
        float[] fArray16 = fArray15;
        this.enemy8 = new NpcEnemy(16648, 8, 8, 14, 7, 0.0f, 30.0f, -22.0f, 0.0f, fArray16);
        this.enemy8.setGroup(9, 9, 9, 9);
        this.enemy1.kickEnepc(19, 1, 0, 1080, 1);
        this.enemy1.kickEnepc(19, 2, 0, 1080, 1);
        this.enemy1.kickEnepc(19, 3, 0, 1080, 1);
        this.enemy1.kickEnepc(19, 4, 0, 1080, 1);
        this.enemy1.kickEnepc(19, 5, 0, 1080, 1);
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 135.0f, 362);
        this.itembox.SetSymbol(28684);
        this.item1 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 366);
        this.U1 = new Uwamono(161, 10, this.item1);
        this.U2 = new Uwamono(162, 10, this.itembox);
        this.U1.SetDiffSize(1.2f, 1.0f, 1.2f);
        this.U2.SetDiffSize(1.2f, 1.0f, 1.2f);
        this.yuka = new Mapunits();
        this.yuka.mapUnit(0);
        this.yuka.start(4, null);
        this.yuka.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2800.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2800.waitPage(this.win, 64);
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
            boolean bl = false;
            if (Runtime.getLeader() != 4) {
                bl = true;
            }
            while (true) {
                if (Runtime.getFlags(8113, 1) == 0) {
                    if (Runtime.getLeader() == 4) {
                        if (!bl) {
                            ST2800.this.light01.disp(true);
                        }
                        bl = true;
                    } else {
                        if (bl) {
                            ST2800.this.light01.disp(false);
                        }
                        bl = false;
                    }
                }
                System.sleep(1);
            }
        }
    }
}

