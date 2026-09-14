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
import xeno.map.MC_KAS13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2470
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS13_PRJ {
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
    Enepc enemy6;
    Unit unit1;
    MAPUnit mapunit;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono box01;
    Uwamono item1;
    int lo = 0;
    boolean Kabekeshi = true;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] annai = new String[]{"Go down the manhole?", "/[waitkey(64)]/[close()]"};
    String[] npc1_1 = new String[]{"Really, how long is he going to make me wait? We said four o'clock in front of the park.", "/[waitkey(1)]/[clear()]", "*Sigh* Oh well. We haven't been on a date in a while, so I'm really looking forward to it.", "/[waitkey(1)]/[clear()]", "...\n", "Looking forward to it...\n", "Looking forward to...", "/[waitkey(1)]/[clear()]", "I am... looking...\n", "forward...\n", "Gwaaah!", "/[waitkey(64)]/[close()]"};

    ST2470() {
    }

    void EOB(int n) {
        if (n == 5) {
            Runtime.setFlags(8084, 1, 1);
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
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        System.println("地下水同【MC_KAS14 ・４");
                        Runtime.setPlayerControl(true);
                        Runtime.jumpCF(2489, 4);
                        return;
                    }
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                if (Runtime.getFlags(8094, 1) != 1) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setPlayerControl(true);
                        System.println("地下水同【MC_KAS14 ・２");
                        Runtime.jumpCF(2489, 2);
                        return;
                    }
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.nwin(this.npc1_1);
        System.println("Talk_npc1");
        this.npc1.kickEnepc(14, 0);
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken:1");
                Runtime.setFlags(8094, 1, 1);
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
                System.println("瓦礫１【MC_KAS12】・２");
                Runtime.jumpCF(2469, 2);
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
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, -1.0f, 1.0f, 0.35f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, -1.0f, 1.0f, 0.35f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, -0.015f, 1.0f, 0.15f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1624, -3.28f, 2.4f, -6.17f, 0.0f);
        this.light01.setRotate(0.0f, 40.0f, 0.0f);
        this.light02 = new Effect(1624, 0.79f, 2.4f, 6.4f, 0.0f);
        this.light02.setRotate(0.0f, 40.0f, 0.0f);
        this.light03 = new Effect(1623, -0.7f, 4.87f, -4.85f, 0.0f);
        this.light03.setRotate(0.0f, -45.0f, 0.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFLockX(3, 7.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 30.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFLockX(5, -4.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        float[] fArray = new float[12];
        fArray[0] = -5.3f;
        fArray[2] = -14.4f;
        fArray[3] = -1.0f;
        fArray[4] = -6.0f;
        fArray[6] = -14.4f;
        fArray[8] = -5.3f;
        fArray[10] = -14.9f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16401, 1, 1, 19, 3, -5.3f, 0.0f, -14.4f, 135.0f, fArray2);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[16];
        fArray3[0] = 10.1f;
        fArray3[2] = -12.7f;
        fArray3[3] = -1.0f;
        fArray3[4] = 9.2f;
        fArray3[6] = -12.7f;
        fArray3[8] = 7.0f;
        fArray3[10] = -11.5f;
        fArray3[11] = 1.0f;
        fArray3[12] = 4.0f;
        fArray3[14] = -9.3f;
        fArray3[15] = 2.0f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16401, 2, 2, 19, 3, 10.1f, 0.0f, -12.7f, 135.0f, fArray4);
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[12];
        fArray5[0] = 4.5f;
        fArray5[2] = -1.6f;
        fArray5[3] = -1.0f;
        fArray5[4] = 4.0f;
        fArray5[6] = -1.6f;
        fArray5[8] = 4.5f;
        fArray5[10] = -2.1f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16401, 3, 3, 19, 3, 4.5f, 0.0f, -1.6f, 315.0f, fArray6);
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[16];
        fArray7[0] = 9.3f;
        fArray7[1] = 0.25f;
        fArray7[2] = 8.7f;
        fArray7[3] = -1.0f;
        fArray7[4] = 8.75f;
        fArray7[5] = 0.25f;
        fArray7[6] = 7.6f;
        fArray7[8] = 6.66f;
        fArray7[9] = 0.25f;
        fArray7[10] = 9.8f;
        fArray7[11] = 1.0f;
        fArray7[12] = 9.9f;
        fArray7[13] = 0.25f;
        fArray7[14] = 9.9f;
        float[] fArray8 = fArray7;
        this.enemy4 = new NpcEnemy(16388, 4, 4, 24, 5, 9.3f, 0.25f, 8.7f, 225.0f, fArray8);
        this.enemy4.setGroup(2, 2, 2, 2);
        if (Runtime.getFlags(8084, 1) == 0) {
            this.npc1 = new NpcEnemy(1570, 5, 0, 6, 7, -16.2f, 0.0f, 3.4f, 45.0f);
            this.npc1.enableDTKFlag(65536);
            this.npc1.talkto("Talk_npc1");
            this.npc1.setGroup(3, 3, 3, 3);
        }
        this.item1 = new Uwamono(28677, 10.7f, 0.3f, 10.7f, 90.0f, 434);
        this.item1.SetSymbol(28683);
        if (Runtime.getFlags(8094, 1) == 0) {
            this.box01 = new Uwamono(2, 13);
            this.box01.SetCallNo(1);
        } else {
            Stage.setVisible(2, false);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2470.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2470.waitPage(this.win, 64);
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
    }
}

