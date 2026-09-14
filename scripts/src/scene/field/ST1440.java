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
import xeno.map.MC_GNU04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1440
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU04_PRJ {
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
    Unit unit1;
    MAPUnit yuka;
    MAPUnit kabeoL;
    MAPUnit kabeoR;
    MAPUnit kabetL;
    MAPUnit kabetR;
    MAPUnit idou;
    Effect hokori01;
    Effect hokori02;
    Effect hokori03;
    Effect hokori04;
    Effect hokori05;
    int sibuki_kazu = 0;
    Effect[] sibuki;
    int[] sibuki_start;
    Effect light01;
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
    Uwamono stone01;
    Uwamono Ibox;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono stoneA;
    Uwamono stoneB;
    Uwamono stoneC;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    boolean hasi = false;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Effect fade;
    int lo = 0;
    int page;
    String[] momo_0 = new String[]{"/[label(MOMO)]", "...?? I wonder what it could be?", "/[waitkey(64)]/[close()]"};
    String[] momo_1 = new String[]{"/[label(MOMO)]", "Battle armor exclusively for the 100-Series Observational Realians?", "/[waitkey(1)]/[clear()]", "Hmm, it seems a little suspicious, but it might come in handy for something! I'll take it for now!!", "/[waitkey(64)]/[close()]"};
    String[] sys_1 = new String[]{"Learned Tech Attack, \"Magic Caster.\"", "/[waitkey(64)]/[close()]"};
    String[] sys_2 = new String[]{"Learned Ether, \"Star Wind.\"", "/[waitkey(64)]/[close()]"};

    ST1440() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 4) {
            Runtime.setFlags(8023, 1, 1);
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
                if (Runtime.getFlags(8108, 1) != 0) break;
                System.println("変身セット");
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (Runtime.getLeader() == 4) {
                    Runtime.setFlags(8108, 1, 1);
                    this.nwin(this.momo_0);
                    Runtime.etherTecSet(5);
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

    void broken(int n) {
        switch (n) {
            case 1: {
                if (Runtime.getFlags(8020, 1) != 0) break;
                Runtime.setFlags(8020, 1, 1);
                System.println("橋が落ちる");
                Runtime.setPlayerControl(false);
                this.hasi = true;
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
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1430, 2);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1450, 4);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -35.0f, -1.0f, 0.0f, 90.0f);
        this.teiten1.SetBgm(196609);
        this.teiten1.SetBgmType('\u0001');
        this.teiten2 = new Uwamono(28690, 6.5f, 1.0f, -16.0f, 0.0f);
        this.teiten2.SetBgm(196610);
        this.teiten3 = new Uwamono(28690, 8.5f, 11.0f, -14.0f, 0.0f);
        this.teiten3.SetBgm(196610);
        this.teiten4 = new Uwamono(28690, 10.5f, 11.0f, -12.0f, 0.0f);
        this.teiten4.SetBgm(196610);
        this.teiten5 = new Uwamono(28690, 12.5f, 11.0f, -10.0f, 0.0f);
        this.teiten5.SetBgm(196610);
        this.teiten6 = new Uwamono(28690, 15.0f, 11.0f, -10.0f, 0.0f);
        this.teiten6.SetBgm(196610);
        this.teiten7 = new Uwamono(28690, 4.5f, 11.0f, -18.0f, 0.0f);
        this.teiten7.SetBgm(196610);
        Stage.setColor(1.4f, 1.4f, 1.4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.55f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        float[][] fArrayArray = new float[][]{{-2.8f, 1.7f, 11.0f}, {-4.9f, 7.0f, -5.1f}, {14.1f, 2.0f, 1.2f}, {-32.5f, -1.1f, 1.0f}, {1.0f, 11.1f, -9.6f}};
        this.sibuki_kazu = fArrayArray.length;
        this.sibuki = new Effect[this.sibuki_kazu];
        this.sibuki_start = new int[this.sibuki_kazu];
        int n = 0;
        while (n < this.sibuki_kazu) {
            this.sibuki[n] = new Effect(1496, fArrayArray[n][0], fArrayArray[n][1], fArrayArray[n][2], 0.0f);
            this.sibuki[n].setClip(false);
            this.sibuki[n].disp(false);
            this.sibuki_start[n] = Math.random() % 100;
            if (this.sibuki_start[n] < 0) {
                this.sibuki_start[n] = this.sibuki_start[n] * -1;
            }
            ++n;
        }
        this.hokori01 = new Effect(1490, 9.2f, -2.0f, -3.2f, 0.0f);
        this.hokori01.disp(false);
        this.hokori02 = new Effect(1490, 1.8f, -2.0f, -3.1f, 0.0f);
        this.hokori02.disp(false);
        this.hokori03 = new Effect(1490, 2.0f, 2.0f, -3.2f, 0.0f);
        this.hokori03.disp(false);
        this.hokori04 = new Effect(1490, 8.2f, -1.0f, -1.2f, 0.0f);
        this.hokori04.disp(false);
        this.hokori05 = new Effect(1490, 2.8f, -1.0f, -1.1f, 0.0f);
        this.hokori05.disp(false);
        this.light01 = new Effect(1018, -10.5f, 8.2f, -6.7f, 0.0f);
        this.light01.disp(false);
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(26, false);
        this.idou = new MAPUnit();
        this.idou.mapUnit(31);
        this.idou.start(4, null);
        this.idou.getTranslate();
        this.idou.getRotate();
        this.idou.setTranslate(this.idou.px, this.idou.py - 0.5f, this.idou.pz);
        this.idou.setRotate(this.idou.rx + 5.0f, this.idou.ry + 5.0f, this.idou.rz);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.player.setID(2);
        this.cam0.setCFAngle(1, -28.0f, -10.0f, 0.0f, 13.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, -10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 5.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, -10.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, -10.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, -20.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, -10.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        float[] fArray = new float[]{-3.2f, 2.0f, 2.3f, 1.0f, 0.3f, 2.0f, 5.3f, -1.0f};
        this.enemy1 = new NpcEnemy(16394, 1, 1, 23, 5, -1.4f, 2.0f, 3.3f, 0.0f, fArray);
        this.enemy1.setGroup(1, 1, 2, 2);
        float[] fArray2 = new float[]{-10.2f, 1.0f, 12.0f, 1.0f, -9.2f, 1.0f, 12.0f, -1.0f};
        this.enemy2 = new NpcEnemy(16394, 2, 2, 23, 5, -11.0f, 1.0f, 11.0f, 0.0f, fArray2);
        this.enemy2.setGroup(1, 1, 2, 2);
        float[] fArray3 = new float[]{-3.4f, 10.3f, -13.0f, 1.0f, 0.5f, 10.5f, -13.0f, -1.0f};
        this.enemy3 = new NpcEnemy(16389, 3, 3, 13, 3, -4.9f, 10.5f, -14.0f, 0.0f, fArray3);
        this.enemy3.setGroup(3, 3, 3, 4);
        if (Runtime.getFlags(8023, 1) == 0) {
            float[] fArray4 = new float[]{11.8f, 2.0f, 0.6f, 1.0f, 9.4f, 2.0f, -0.5f, -1.0f};
            this.enemy4 = new NpcEnemy(20233, 4, 0, 28, 7, 12.3f, 2.0f, -1.0f, -90.0f);
            this.enemy4.setGroup(0, 0, 0, 0);
            this.enemy4.setBatEvent(8);
        }
        this.yuka = new Mapunits();
        this.yuka.mapUnit(71);
        this.yuka.start(4, null);
        this.yuka.start(1, "idle");
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 159);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 160);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 161);
        new Uwamono(50, 13);
        this.stone01 = new Uwamono(82, 17);
        this.stone01.SetCallNo(1);
        this.stoneA = new Uwamono(84, 55, this.item3);
        this.stoneB = new Uwamono(85, 55, this.item2);
        this.stoneC = new Uwamono(86, 55);
        new Uwamono(83, 55, this.item1);
        this.Ibox = new Uwamono(28677, 9.2f, 2.0f, 0.1f, 180.0f, 193);
        this.Ibox.SetSymbol(28685);
        new Uwamono(28673, -14.2f, 1.0f, 11.8f, 0.0f);
        if (Runtime.getFlags(8020, 1) == 1) {
            this.yuka.getTranslate();
            this.yuka.setTranslate(this.yuka.px, this.yuka.py - 4.7f, this.yuka.pz);
            this.player.setID(1);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1440.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1440.waitPage(this.win, 64);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 0.0f;
            float f8 = 15.0f;
            float f9 = -2.6f;
            float f10 = 0.0f;
            ST1440.this.yuka.getTranslate();
            boolean bl = false;
            if (Runtime.getLeader() != 4) {
                bl = true;
            }
            while (true) {
                if (f5 < 101.0f) {
                    int n = 0;
                    while (n < ST1440.this.sibuki_kazu) {
                        if (f5 == (float) ST1440.this.sibuki_start[n]) {
                            ST1440.this.sibuki[n].disp(true);
                        }
                        ++n;
                    }
                }
                f5 += 1.0f;
                f3 = Math.sin(f * 3.14f / 180.0f);
                f4 = Math.cos(f * 3.14f / 180.0f);
                ST1440.this.stoneA.setRotate(f3 *= 4.0f, f3 / 2.0f, f3);
                ST1440.this.stoneB.setRotate(0.0f, f4 *= 4.0f, f4);
                ST1440.this.stoneC.setRotate(f4 / 2.0f, f4 / 2.0f, f3);
                f6 = ST1440.this.cam0.getRotateX();
                f7 = ST1440.this.cam0.getRotateY();
                ST1440.this.cam0.setRotate(f6, f7, f3 / 2.0f);
                if ((f += 1.0f) == 360.0f) {
                    f = 0.0f;
                }
                if (ST1440.this.hasi) {
                    if (f2 == 0.0f) {
                        ST1440.this.cam0.setMode(-1);
                        ST1440.this.camEV = Camera.create(1);
                        ST1440.this.camEV.setTranslate(2.0f, 16.01f, 13.8f);
                        ST1440.this.camEV.setRotate(-36.36f, 350.0f, 0.0f);
                        ST1440.this.camEV.setFov(35.0f);
                        Runtime.disable(524288);
                        ST1440.this.camEV.change();
                    }
                    if (f2 == 0.0f + f8 - 10.0f) {
                        ST1440.this.hokori03.disp(true);
                    }
                    if (f2 > 0.0f) {
                        Runtime.setPlayerControl(false);
                    }
                    if (f2 > 0.0f + f8 && f2 <= 30.0f + f8) {
                        ST1440.this.yuka.setRotate(-(f2 - f8) / 2.0f, 0.0f, -(f2 - f8));
                        ST1440.this.yuka.setTranslate(ST1440.this.yuka.px, f9 * (f2 - f8) * (f2 - f8) / 900.0f + 6.7f, ST1440.this.yuka.pz);
                    }
                    if (f2 == 30.0f + f8) {
                        Sound.effectPlay(196741);
                    }
                    if (f2 >= 30.0f + f8 && f2 <= 120.0f) {
                        if ((-12.8f * (f2 - 30.0f - f8) / 30.0f + 7.0f) * (f2 - 30.0f - f8) / 30.0f + 2.0f <= 2.0f) {
                            ST1440.this.Ibox.setTranslate(ST1440.this.Ibox.px, ST1440.this.Ibox.py, ST1440.this.Ibox.pz);
                        } else {
                            ST1440.this.Ibox.setTranslate(ST1440.this.Ibox.px, (-12.8f * (f2 - 30.0f - f8) / 30.0f + 7.0f) * (f2 - 30.0f - f8) / 30.0f + 2.0f, ST1440.this.Ibox.pz);
                        }
                    }
                    if (f2 == 25.0f + f8) {
                        ST1440.this.hokori01.disp(true);
                    }
                    if (f2 == 30.0f + f8) {
                        ST1440.this.hokori04.disp(true);
                    }
                    if (f2 >= 30.0f + f8 && f2 < 120.0f) {
                        ST1440.this.hokori01.setTranslate(9.2f + (f2 - 30.0f - f8) / 70.0f, -2.0f, -3.2f);
                        ST1440.this.hokori04.setTranslate(8.2f + (f2 - 30.0f - f8) / 60.0f, -1.0f, -1.2f);
                    }
                    if (f2 >= 30.0f + f8 && f2 <= 33.0f + f8) {
                        f10 = Math.sin((f - 30.0f - f8) * 160.0f * 3.14f / 180.0f);
                        ST1440.this.camEV.setTranslate(2.0f, 16.01f + f10 * 0.1f, 13.8f);
                        ST1440.this.camEV.setRotate(-36.36f, 350.0f, 0.0f);
                        ST1440.this.camEV.setFov(35.0f);
                        ST1440.this.camEV.change();
                    }
                    if (f2 > 30.0f + f8 && f2 <= 40.0f + f8) {
                        ST1440.this.yuka.setRotate(((f2 - 30.0f - f8) * 3.0f - 30.0f) / 2.0f, 0.0f, (f2 - 30.0f - f8) * 3.0f - 30.0f);
                        ST1440.this.yuka.setTranslate(ST1440.this.yuka.px, f9 * (f2 - f8) * (f2 - f8) / 900.0f + 6.7f, ST1440.this.yuka.pz);
                    }
                    if (f2 == 35.0f + f8) {
                        ST1440.this.hokori02.disp(true);
                    }
                    if (f2 == 40.0f + f8) {
                        ST1440.this.hokori05.disp(true);
                    }
                    if (f2 >= 40.0f + f8 && f2 < 120.0f) {
                        ST1440.this.hokori02.setTranslate(1.8f - (f2 - 40.0f - f8) / 70.0f, -2.0f, -3.1f);
                        ST1440.this.hokori05.setTranslate(2.8f - (f2 - 40.0f - f8) / 60.0f, -1.0f, -1.1f);
                    }
                    if (f2 >= 40.0f + f8 && f2 <= 45.0f + f8) {
                        f10 = Math.sin((f - 30.0f - f8) * 160.0f * 3.14f / 180.0f);
                        ST1440.this.camEV.setTranslate(2.0f, 16.01f + f10 * 0.2f, 13.8f);
                        ST1440.this.camEV.setRotate(-36.36f, 350.0f, 0.0f);
                        ST1440.this.camEV.setFov(35.0f);
                        ST1440.this.camEV.change();
                    }
                    if (f2 == 120.0f) {
                        Runtime.enable(524288);
                        ST1440.this.player.setID(1);
                        ST1440.this.cam0.setMode(0);
                        ST1440.this.hasi = false;
                        Runtime.setPlayerControl(true);
                    }
                    f2 += 1.0f;
                }
                if (Runtime.getFlags(8108, 1) == 0) {
                    if (Runtime.getLeader() == 4) {
                        if (!bl) {
                            ST1440.this.light01.disp(true);
                        }
                        bl = true;
                    } else {
                        if (bl) {
                            ST1440.this.light01.disp(false);
                        }
                        bl = false;
                    }
                }
                System.sleep(1);
            }
        }
    }
}

