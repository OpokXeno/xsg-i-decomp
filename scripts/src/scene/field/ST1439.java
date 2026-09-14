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
import xeno.map.MC_GNU03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1430
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU03_PRJ {
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
    Uwamono box;
    Uwamono doorA;
    MAPUnit cube1;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    MAPUnit ele1;
    MAPUnit ele2;
    MAPUnit ele3;
    MAPUnit ele4;
    MAPUnit ele5;
    Unit elv;
    int elemove = 0;
    boolean boxbr = false;
    Effect light01;
    Effect hokori01;
    int sibuki_kazu = 0;
    Effect[] sibuki;
    int[] sibuki_start;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    int page;
    String[] ele_s = new String[]{"Operate elevator?", "/[waitkey(64)]/[close()]"};
    String[] hasamatteru = new String[]{"Something seems to be stuck underneath.", "/[waitkey(64)]/[close()]"};

    ST1430() {
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
                if (Runtime.getFlags(8013, 1) != 0 || this.elemove != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.nwin(this.ele_s);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        System.println("エレベータ");
                        this.hokori01.disp(false);
                        this.hokori01.clearEffect();
                        Runtime.enable(65536);
                        this.player.rotY(10, -90.0f, true);
                        System.sleep(10);
                        this.player.mtn(25, 1, 1.0f, true);
                        System.sleep(40);
                        Runtime.disable(65536);
                        this.elemove = 1;
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(8091, 1, 1);
                System.println("1");
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
                Runtime.jumpCF(1429, 2);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(1449, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
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
        this.light01 = new Effect(1020, 7.98f, 3.55f, 4.97f, 0.0f);
        this.light01.noAttach(false);
        this.light01.setRotate(90.0f, 0.0f, 0.0f);
        this.light01.setScale(1.0f, 5.0f, 1.0f);
        this.light01.disp(true);
        this.hokori01 = new Effect(1490, 10.5f, -4.0f, 4.3f, 0.0f);
        this.hokori01.disp(false);
        Stage.setVisible(-1, true);
        float[][] fArrayArray = new float[10][];
        fArrayArray[0] = new float[]{-12.9f, 1.83f, -3.6f};
        float[] fArray = new float[3];
        fArray[0] = -10.1f;
        fArray[2] = 8.5f;
        fArrayArray[1] = fArray;
        fArrayArray[2] = new float[]{-6.3f, 1.0f, 17.8f};
        fArrayArray[3] = new float[]{1.7f, -2.0f, 3.4f};
        fArrayArray[4] = new float[]{5.1f, 6.0f, -3.4f};
        fArrayArray[5] = new float[]{23.1f, 6.0f, 3.8f};
        float[] fArray2 = new float[3];
        fArray2[0] = -8.6f;
        fArray2[2] = -15.5f;
        fArrayArray[6] = fArray2;
        fArrayArray[7] = new float[]{11.1f, 2.22f, -15.2f};
        fArrayArray[8] = new float[]{15.6f, 2.85f, 3.7f};
        fArrayArray[9] = new float[]{13.9f, 2.33f, 6.1f};
        float[][] fArrayArray2 = fArrayArray;
        this.sibuki_kazu = fArrayArray2.length;
        this.sibuki = new Effect[this.sibuki_kazu];
        this.sibuki_start = new int[this.sibuki_kazu];
        int n = 0;
        while (n < this.sibuki_kazu) {
            this.sibuki[n] = new Effect(1496, fArrayArray2[n][0], fArrayArray2[n][1], fArrayArray2[n][2], 0.0f);
            this.sibuki[n].setClip(false);
            this.sibuki[n].disp(false);
            this.sibuki_start[n] = Math.random() % 100;
            if (this.sibuki_start[n] < 0) {
                this.sibuki_start[n] = this.sibuki_start[n] * -1;
            }
            ++n;
        }
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(34, false);
        Stage.setVisible(35, false);
        Stage.setVisible(36, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, -10.0f, 0.0f, 30.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, -10.0f, 0.0f, 14.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 17.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, -10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, -20.0f, 0.0f, 1.5f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, -30.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        float[] fArray3 = new float[]{-1.8f, 1.6f, -16.3f, 1.0f, 2.7f, 2.8f, -16.1f, -1.0f};
        this.enemy1 = new NpcEnemy(16391, 1, 1, 8, 3, 0.6f, 2.5f, -16.2f, 0.0f, fArray3);
        this.enemy1.setGroup(3, 3, 3, 3);
        float[] fArray4 = new float[]{18.8f, 6.0f, -2.4f, 1.0f, 22.1f, 6.0f, -2.5f, -1.0f};
        this.enemy2 = new NpcEnemy(16394, 2, 2, 23, 5, 20.5f, 6.0f, -2.6f, 0.0f, fArray4);
        this.enemy2.setGroup(1, 1, 2, 2);
        float[] fArray5 = new float[8];
        fArray5[0] = -5.2f;
        fArray5[2] = 16.1f;
        fArray5[3] = 1.0f;
        fArray5[4] = 0.4f;
        fArray5[6] = 15.4f;
        fArray5[7] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16394, 3, 3, 23, 5, -2.2f, 0.0f, 16.3f, 0.0f, fArray6);
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[]{13.1f, 0.1f, 20.1f, 1.0f, 17.3f, 0.1f, 20.6f, -1.0f};
        this.enemy4 = new NpcEnemy(16391, 4, 4, 8, 3, 15.0f, 0.2f, 21.5f, 0.0f, fArray7);
        this.enemy4.setGroup(3, 3, 3, 3);
        this.enemy1.kickEnepc(19, 1, 1, 560, 1);
        this.player.setID(1);
        this.ele1 = new Mapunits();
        this.ele1.mapUnit(21);
        this.ele1.start(4, null);
        this.ele2 = new Mapunits();
        this.ele2.mapUnit(26);
        this.ele2.start(4, null);
        this.ele3 = new Mapunits();
        this.ele3.mapUnit(27);
        this.ele3.start(4, null);
        this.ele4 = new Mapunits();
        this.ele4.mapUnit(80);
        this.ele4.start(4, null);
        this.ele5 = new Mapunits();
        this.ele5.mapUnit(152);
        this.ele5.start(4, null);
        this.elv = new Unit();
        this.elv.initElevator(21, 100.1f, 0.0f);
        this.elv.setArgs(12, 2.31f);
        if (Runtime.getFlags(8013, 1) == 1) {
            this.ele1.getTranslate();
            this.ele2.getTranslate();
            this.ele3.getTranslate();
            this.ele4.getTranslate();
            this.ele5.getTranslate();
            this.light01.getTranslate();
            float f = 0.0f;
            float f2 = 1.3f;
            float f3 = 0.7f;
            f = Math.sin(3.14f) * 2.16f + 0.16f;
            this.ele1.setTranslate(this.ele1.px, f, this.ele1.pz);
            this.ele2.setTranslate(this.ele2.px, f, this.ele2.pz);
            this.ele3.setTranslate(this.ele3.px, f + f3, this.ele3.pz);
            this.ele4.setTranslate(this.ele4.px, f, this.ele4.pz);
            this.ele5.setTranslate(this.ele5.px, f, this.ele5.pz);
            this.light01.setTranslate(7.98f, f + f2, 4.97f);
            this.elv.setArgs(12, -2.0f);
            this.light01.disp(false);
            this.player.setID(2);
        }
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 156);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 158);
        this.item4 = new Uwamono(28677, 27.6f, 6.0f, -9.2f, 90.0f, 192);
        this.item4.SetSymbol(28672);
        if (Runtime.getFlags(8091, 1) == 1) {
            Stage.setVisible(58, false);
        } else {
            this.box = new Uwamono(58, 4);
            this.box.SetCallNo(1);
        }
        new Uwamono(59, 39);
        new Uwamono(64, 39);
        new Uwamono(78, 4);
        new Uwamono(79, 4);
        new Uwamono(117, 4);
        new Uwamono(153, 56, this.item1);
        new Uwamono(154, 56, this.item2);
        new Uwamono(28673, -5.7f, 0.0f, 18.9f, 0.0f);
        new Uwamono(28673, 19.9f, 6.0f, -5.5f, 0.0f);
        this.doorA = new Uwamono(131, 42, '\u0001');
        new Uwamono(132, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(2);
        this.cube1.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1430.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1430.waitPage(this.win, 64);
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
            this.setElevatorMode(1);
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
            int n = 300;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 0.0f;
            ST1430.this.ele1.getTranslate();
            ST1430.this.ele2.getTranslate();
            ST1430.this.ele3.getTranslate();
            ST1430.this.ele4.getTranslate();
            ST1430.this.ele5.getTranslate();
            ST1430.this.light01.getTranslate();
            float f8 = 0.0f;
            float f9 = 1.3f;
            float f10 = 0.7f;
            boolean bl = false;
            while (true) {
                if (f5 < 101.0f) {
                    int n2 = 0;
                    while (n2 < ST1430.this.sibuki_kazu) {
                        if (f5 == (float) ST1430.this.sibuki_start[n2]) {
                            ST1430.this.sibuki[n2].disp(true);
                        }
                        ++n2;
                    }
                }
                f5 += 1.0f;
                f = Math.sin(f3 * 3.14f / 180.0f);
                f2 = Math.cos(f3 * 3.14f / 180.0f);
                f2 *= 4.0f;
                f6 = ST1430.this.cam0.getRotateX();
                f7 = ST1430.this.cam0.getRotateY();
                ST1430.this.cam0.setRotate(f6, f7, (f *= 4.0f) / 2.0f);
                if (ST1430.this.elemove == 1) {
                    Runtime.setPlayerControl(false);
                    if (Runtime.getFlags(8091, 1) == 1) {
                        if (f4 == 0.0f) {
                            Sound.effectPlay(196744);
                        }
                        f8 = Math.sin(f4 * 3.14f / 180.0f) * 2.16f + 0.16f;
                        ST1430.this.ele1.setTranslate(ST1430.this.ele1.px, f8, ST1430.this.ele1.pz);
                        ST1430.this.ele2.setTranslate(ST1430.this.ele2.px, f8, ST1430.this.ele2.pz);
                        ST1430.this.ele3.setTranslate(ST1430.this.ele3.px, f8 + f10, ST1430.this.ele3.pz);
                        ST1430.this.ele4.setTranslate(ST1430.this.ele4.px, f8, ST1430.this.ele4.pz);
                        ST1430.this.ele5.setTranslate(ST1430.this.ele5.px, f8, ST1430.this.ele5.pz);
                        ST1430.this.elv.setArgs(12, f8);
                        ST1430.this.light01.setTranslate(7.98f, f8 + f9, 4.97f);
                        f4 += 1.0f;
                        if (f4 > 180.0f) {
                            ST1430.this.elemove = 0;
                            f4 = 0.0f;
                            ST1430.this.light01.disp(false);
                            Runtime.setPlayerControl(true);
                            Runtime.setFlags(8013, 1, 1);
                            Sound.effectStop(196744);
                            Sound.effectPlay(196745);
                            ST1430.this.player.setID(2);
                        }
                    } else {
                        if (f4 == 0.0f) {
                            Sound.effectPlay(196744);
                        }
                        f8 = Math.sin(f4 * 3.14f / 180.0f) * 2.16f + 0.16f;
                        ST1430.this.ele1.setTranslate(ST1430.this.ele1.px, f8, ST1430.this.ele1.pz);
                        ST1430.this.ele2.setTranslate(ST1430.this.ele2.px, f8, ST1430.this.ele2.pz);
                        ST1430.this.ele3.setTranslate(ST1430.this.ele3.px, f8 + f10, ST1430.this.ele3.pz);
                        ST1430.this.ele4.setTranslate(ST1430.this.ele4.px, f8, ST1430.this.ele4.pz);
                        ST1430.this.ele5.setTranslate(ST1430.this.ele5.px, f8, ST1430.this.ele5.pz);
                        ST1430.this.elv.setArgs(12, f8);
                        ST1430.this.light01.setTranslate(7.98f, f8 + f9, 4.97f);
                        if (f8 < 0.0f) {
                            ST1430.this.hokori01.disp(true);
                            bl = true;
                            Sound.effectPlay(196746);
                        }
                        f4 = !bl ? (f4 += 1.0f) : (f4 -= 1.0f);
                        if (f4 < 0.0f) {
                            ST1430.this.elemove = 0;
                            f4 = 0.0f;
                            ST1430.this.nwin(ST1430.this.hasamatteru);
                            Runtime.setPlayerControl(true);
                            bl = false;
                        }
                    }
                }
                if ((f3 += 1.0f) == 360.0f) {
                    f3 = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

