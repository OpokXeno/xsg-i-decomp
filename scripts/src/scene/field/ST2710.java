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
import xeno.map.MC_UTA01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2710
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA01_PRJ {
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Effect r01;
    Effect r02;
    Effect r03;
    Effect r04;
    Effect r05;
    Effect r06;
    Effect r07;
    Effect r08;
    Effect r09;
    Effect r10;
    Effect r11;
    Effect r12;
    Effect r13;
    Effect r14;
    Effect r15;
    Effect r16;
    Effect r17;
    Effect r18;
    Effect r19;
    Effect r20;
    Effect r21;
    Effect r22;
    Effect r23;
    Effect r24;
    Effect r25;
    Effect r26;
    Effect r27;
    Light light = new Light(0);
    Effect fade;
    int page;

    ST2710() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void Talk_npc1(Enepc enepc) {
        Runtime.setFlags(383, 1, 1);
        System.println("イベント3050C1");
        Runtime.setPlayerControl(false);
        this.fade.call(0);
        System.sleep(30);
        Runtime.jumpEvent(3503);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(2870, 14);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(0.54f, 0.792f, 1.056f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.14f, 0.2f, 0.24f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.21f, 0.3f, 0.36f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.21f, 0.3f, 0.36f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        this.r01 = new Effect(1621, 1.1149994f, 1.4599949f, 3.6149845f, 0.0f);
        this.r02 = new Effect(1621, 1.2549992f, 1.4799932f, 3.749978f, 0.0f);
        this.r03 = new Effect(1621, 1.379999f, 1.4599899f, 3.8899713f, 0.0f);
        this.r04 = new Effect(1621, -2.372492f, 1.2094842f, 4.3599596f, 0.0f);
        this.r05 = new Effect(1621, -2.5044825f, 1.2364794f, 4.4959497f, 0.0f);
        this.r06 = new Effect(1621, -2.6394725f, 1.2034744f, 4.6309395f, 0.0f);
        this.r07 = new Effect(1621, -1.8694682f, 2.210965f, 0.8499299f, 0.0f);
        this.r08 = new Effect(1621, -2.001465f, 2.2439609f, 0.9929275f, 0.0f);
        this.r09 = new Effect(1621, -2.1334558f, 2.2109578f, 1.124926f, 0.0f);
        this.r10 = new Effect(1621, -2.1914496f, 3.354951f, 9.788911f, 0.0f);
        this.r11 = new Effect(1621, -1.811946f, 3.3549442f, 10.008904f, 0.0f);
        this.r12 = new Effect(1621, -2.196938f, 3.3549442f, 10.223398f, 0.0f);
        this.r13 = new Effect(1621, -2.1939373f, 3.3549442f, 12.780386f, 0.0f);
        this.r14 = new Effect(1621, -1.8099306f, 3.3579438f, 12.999322f, 0.0f);
        this.r15 = new Effect(1621, -2.1879106f, 3.3579423f, 13.218257f, 0.0f);
        this.r16 = new Effect(1621, -2.1879106f, 3.3579423f, 15.794245f, 0.0f);
        this.r17 = new Effect(1621, -2.1879106f, 3.3579423f, 16.214241f, 0.0f);
        this.r18 = new Effect(1621, -1.8149096f, 3.3579423f, 16.000233f, 0.0f);
        this.r19 = new Effect(1621, 1.8120862f, 3.3579423f, 15.993722f, 0.0f);
        this.r20 = new Effect(1621, 2.1810668f, 3.3549414f, 16.215586f, 0.0f);
        this.r21 = new Effect(1621, 2.1840663f, 3.3549364f, 15.774527f, 0.0f);
        this.r22 = new Effect(1621, 2.186565f, 3.3546865f, 13.213046f, 0.0f);
        this.r23 = new Effect(1621, 2.186565f, 3.3546848f, 12.778292f, 0.0f);
        this.r24 = new Effect(1621, 1.8093137f, 3.351935f, 12.99751f, 0.0f);
        this.r25 = new Effect(1621, 2.1893103f, 3.3519228f, 10.216988f, 0.0f);
        this.r26 = new Effect(1621, 2.1893103f, 3.3519228f, 9.776916f, 0.0f);
        this.r27 = new Effect(1621, 1.807063f, 3.3519228f, 9.994109f, 0.0f);
        this.r01.setScale(0.42f, 0.42f, 0.42f);
        this.r02.setScale(0.42f, 0.42f, 0.42f);
        this.r03.setScale(0.42f, 0.42f, 0.42f);
        this.r04.setScale(0.42f, 0.42f, 0.42f);
        this.r05.setScale(0.42f, 0.42f, 0.42f);
        this.r06.setScale(0.42f, 0.42f, 0.42f);
        this.r07.setScale(0.42f, 0.42f, 0.42f);
        this.r08.setScale(0.42f, 0.42f, 0.42f);
        this.r09.setScale(0.42f, 0.42f, 0.42f);
        this.r10.setScale(0.42f, 0.42f, 0.42f);
        this.r11.setScale(0.42f, 0.42f, 0.42f);
        this.r12.setScale(0.42f, 0.42f, 0.42f);
        this.r13.setScale(0.42f, 0.42f, 0.42f);
        this.r14.setScale(0.42f, 0.42f, 0.42f);
        this.r15.setScale(0.42f, 0.42f, 0.42f);
        this.r16.setScale(0.42f, 0.42f, 0.42f);
        this.r17.setScale(0.42f, 0.42f, 0.42f);
        this.r18.setScale(0.42f, 0.42f, 0.42f);
        this.r19.setScale(0.42f, 0.42f, 0.42f);
        this.r20.setScale(0.42f, 0.42f, 0.42f);
        this.r21.setScale(0.42f, 0.42f, 0.42f);
        this.r22.setScale(0.42f, 0.42f, 0.42f);
        this.r23.setScale(0.42f, 0.42f, 0.42f);
        this.r24.setScale(0.42f, 0.42f, 0.42f);
        this.r25.setScale(0.42f, 0.42f, 0.42f);
        this.r26.setScale(0.42f, 0.42f, 0.42f);
        this.r27.setScale(0.42f, 0.42f, 0.42f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.npc1 = new NpcEnemy(268, 1, 0, 3, 3, 0.0f, 0.5f, 5.2f, 0.0f);
        this.npc1.talkto("Talk_npc1");
        this.doorA = new Uwamono(119, 42, '\u0001');
        new Uwamono(120, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2710.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2710.waitPage(this.win, 64);
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

