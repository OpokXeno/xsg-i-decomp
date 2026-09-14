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
import xeno.map.MC_GNK04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3020
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK04_PRJ {
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
    Enepc enemy5;
    Unit unit1;
    Unit MJele;
    Unit PUPU;
    Unit Step;
    Unit kanban;
    Unit Kidou0;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect fade;
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
    Light light = new Light(0);
    Uwamono doorA;
    Uwamono doorB;
    Uwamono col;
    Uwamono SAVE;
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
    String[] Info_00 = new String[]{"Use the elevator?", "/[waitkey(64)]/[close()]"};

    ST3020() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, -15.02f, 15.322f, 7.157f, 900.0f, -15.0f, -70.0f, 86.58047f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -61.366f;
        fArray2[2] = -10.178f;
        fArray2[4] = 900.0f;
        fArray2[5] = -28.53f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 900);
        this.camEV.rotateSPL(fArray3, 1, 2, 900);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera00a() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-15.02f, 15.322f, 7.157f);
        this.camEV.setRotate(-61.366f, -10.178f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -15.0f, -70.0f, 86.58047f, 900.0f, -15.02f, 15.322f, 7.157f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -28.53f;
        fArray2[4] = 900.0f;
        fArray2[5] = -61.366f;
        fArray2[6] = -10.178f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 900);
        this.camEV.rotateSPL(fArray3, 1, 2, 900);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.980146f, 3.5f, -0.2030067f);
        this.camEV.setRotate(-14.600572f, -20.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-15.0f, -70.0f, 86.58047f);
        this.camEV.setRotate(-28.53f, 0.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            if (Runtime.getFlags(3144, 2) != 0) {
                                Runtime.setPlayerControl(false);
                                Runtime.enable(65536);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(54);
                                this.E01.disp(true);
                                this.E02.disp(true);
                                Runtime.disable(65536);
                                System.sleep(1);
                                Runtime.enable(65536);
                                this.player.mtn(2, 9, 1.0f, true);
                                this.player.move(30, -14.0f, -2.0f, true);
                                System.sleep(35);
                                this.player.mtn(28, 9, 1.0f, true);
                                this.Step.start(1, "Down");
                                return;
                            }
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(54);
                            this.E01.disp(true);
                            this.E02.disp(true);
                            Runtime.disable(65536);
                            System.sleep(1);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(30, -14.0f, -2.0f, true);
                            System.sleep(35);
                            this.player.mtn(28, 9, 1.0f, true);
                            Sound.effectPlay(196746);
                            this.Step.start(1, "Down2");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(54);
                            this.E01.disp(true);
                            this.E02.disp(true);
                            Runtime.disable(65536);
                            System.sleep(1);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(30, -14.0f, 75.0f, true);
                            System.sleep(35);
                            this.player.mtn(28, 9, 1.0f, true);
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            this.Step.start(1, "Up");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3133, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Info_00, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.enable(65536);
                        Sound.effectPlay(196715);
                        this.kanban.start(1, "nobiA");
                        System.sleep(45);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(30, 12.0f, -8.5f, true);
                        System.sleep(35);
                        this.player.rotY(15, 0.0f, true);
                        System.sleep(20);
                        this.player.mtn(28, 9, 1.0f, true);
                        Sound.effectPlay(196715);
                        this.kanban.start(1, "nobiB");
                        System.sleep(60);
                        Runtime.setPlayerControl(false);
                        this.cam0.setMode(-1);
                        this.MJele.start(1, "Agari");
                        return;
                    }
                }
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(60, 12.0f, -3.0f, true);
                System.sleep(60);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
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
                Runtime.jumpCF(68696, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 12.0f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196610);
        this.teiten2 = new Uwamono(28690, -15.0f, 0.0f, -2.0f, 0.0f);
        this.teiten2.SetBgm(196611);
        this.teiten3 = new Uwamono(28690, -15.0f, -76.0f, 74.0f, 0.0f);
        this.teiten3.SetBgm(196611);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3132, 1) == 1) {
            this.MJele = new Mapunits();
            this.MJele.mapUnit(20);
            this.MJele.start(4, null);
            this.MJele.setTranslate(this.MJele.px, this.MJele.py + 10.0f, this.MJele.pz);
        } else {
            this.MJele = new Mapunits();
            this.MJele.mapUnit(20);
            this.MJele.start(4, null);
        }
        if (Runtime.getFlags(3132, 1) == 1) {
            this.kanban = new Mapunits();
            this.kanban.mapUnit(19);
            this.kanban.start(4, null);
            this.kanban.setTranslate(this.kanban.px, this.kanban.py + 10.0f, this.kanban.pz);
        } else {
            this.kanban = new Mapunits();
            this.kanban.mapUnit(19);
            this.kanban.start(4, null);
        }
        this.col = new Uwamono(28672, 12.0f, 0.0f, -8.5f, 0.0f);
        this.col.SetSize(3.0f, 1.0f, 3.5f);
        if (Runtime.getFlags(3090, 1) == 0) {
            this.Step = new Mapunits();
            this.Step.mapUnit(16);
            this.Step.start(4, null);
        } else if (Runtime.getFlags(3149, 1) == 0) {
            this.Step = new Mapunits();
            this.Step.mapUnit(16);
            this.Step.start(4, null);
            this.Step.setTranslate(this.Step.px, this.Step.py - 76.0f, this.Step.pz + 76.0f);
        } else {
            this.Step = new Mapunits();
            this.Step.mapUnit(16);
            this.Step.start(4, null);
            this.Step.setTranslate(this.Step.px, this.Step.py - 60.0f, this.Step.pz + 60.0f);
            this.Step.start(1, "Sdown");
        }
        if (Runtime.getFlags(3132, 1) == 1) {
            this.Kidou0 = new Mapunits();
            this.Kidou0.mapUnit(6);
            this.Kidou0.start(4, null);
            this.Kidou0.start(1, "Evt");
        }
        this.E01 = new Effect(1632, -9.12f, 3.2f, -4.5f, 0.0f);
        this.E01.disp(false);
        this.E01.setClip(true);
        this.E02 = new Effect(1632, -20.82f, 3.2f, -4.5f, 0.0f);
        this.E02.disp(false);
        this.E02.setClip(true);
        this.E02.noAttach(true);
        this.E02.setRotate(this.E01.rx + 180.0f, this.E01.ry + 180.0f, this.E01.rz);
        if (Runtime.getFlags(3090, 1) == 0) {
            this.E03 = new Effect(1417, -13.741f, 1.023f, -4.322f, 0.0f);
            this.E03.disp(true);
            this.E03.setClip(true);
            this.E03.noAttach(true);
        } else if (Runtime.getFlags(3149, 1) == 0) {
            this.E03 = new Effect(1417, -13.741f, 1.023f, -4.322f, 0.0f);
            this.E03.disp(true);
            this.E03.setClip(true);
            this.E03.noAttach(true);
            this.E03.setTranslate(this.E03.px, this.E03.py - 76.0f, this.E03.pz + 76.0f);
        } else {
            this.E03 = new Effect(1417, -13.741f, 1.023f, -4.322f, 0.0f);
            this.E03.disp(true);
            this.E03.setClip(true);
            this.E03.noAttach(true);
            this.E03.setTranslate(this.E03.px, this.E03.py - 60.0f, this.E03.pz + 60.0f);
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
        Stage.setVisible(17, false);
        Stage.setVisible(29, false);
        Stage.setVisible(18, false);
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
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, -15.0f);
        this.cam0.setCFPedestal(7, -15.0f, -70.0f, 86.58047f, 40.0f, -28.53f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, -15.02f, 15.322f, 7.157f, 40.0f, -61.366f, -10.178f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.SAVE = new Uwamono(28678, 9.0f, 0.0f, -3.0f);
        this.doorA = new Uwamono(21, 42, '\u0001');
        new Uwamono(22, 42, '\u0001', this.doorA);
        this.doorA.SetDoorRange(1.69f);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(3149, 1) == 1) {
            this.player.setTranslate(-15.0f, -60.118f, 60.0f);
        }
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

        void Agari() {
            int n = 0;
            ST3020.this.MJele.getTranslate();
            ST3020.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196748);
                }
                if (n >= 0 && n < 180) {
                    ST3020.this.kanban.getTranslate();
                    ST3020.this.kanban.setTranslate(ST3020.this.kanban.px, ST3020.this.kanban.py + 0.055555556f, ST3020.this.kanban.pz);
                    ST3020.this.MJele.getTranslate();
                    ST3020.this.MJele.setTranslate(ST3020.this.MJele.px, ST3020.this.MJele.py + 0.055555556f, ST3020.this.MJele.pz);
                    ST3020.this.player.getTranslate();
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py + 0.055555556f, ST3020.this.player.pz);
                }
                if (n == 180) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.setFlags(3133, 1, 1);
            Runtime.jumpCF(68546, 0);
        }

        void Down() {
            int n = 0;
            ST3020.this.MJele.getTranslate();
            ST3020.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    ST3020.this.cam0.setMode(-1);
                    ST3020.this.EV_Camera00();
                    Sound.effectPlay(196746);
                }
                if (n == 30) {
                    Sound.effectPlay(196750);
                }
                if (n == 900) {
                    Sound.effectStop(196750);
                    Sound.effectPlay(196747);
                }
                if (n >= 0 && n < 900) {
                    ST3020.this.Step.setTranslate(ST3020.this.Step.px, ST3020.this.Step.py - 0.08444444f, ST3020.this.Step.pz + 0.08444444f);
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py - 0.08444444f, ST3020.this.player.pz + 0.08444444f);
                    ST3020.this.E03.setTranslate(ST3020.this.E03.px, ST3020.this.E03.py - 0.08444444f, ST3020.this.E03.pz + 0.08444444f);
                }
                if (n == 900) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.E01.disp(false);
            ST3020.this.E02.disp(false);
            Runtime.setFlags(3090, 1, 1);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3020.this.cam0.setMode(0);
        }

        void Down2() {
            int n = 0;
            ST3020.this.MJele.getTranslate();
            ST3020.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    ST3020.this.cam0.setMode(-1);
                    ST3020.this.EV_Camera00();
                }
                if (n >= 0 && n < 900) {
                    ST3020.this.Step.setTranslate(ST3020.this.Step.px, ST3020.this.Step.py - 0.08444444f, ST3020.this.Step.pz + 0.08444444f);
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py - 0.08444444f, ST3020.this.player.pz + 0.08443333f);
                    ST3020.this.E03.setTranslate(ST3020.this.E03.px, ST3020.this.E03.py - 0.08444444f, ST3020.this.E03.pz + 0.08444444f);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.fade.call(0);
            System.sleep(30);
            Runtime.setFlags(3090, 1, 1);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3020.this.cam0.setMode(0);
            Runtime.jumpCF(68557, 0);
        }

        void Evt() {
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST3020.this.cam0.setMode(-1);
            ST3020.this.EV_Camera02();
            ST3020.this.player.setTranslate(12.0f, 10.0f, -8.5f);
            ST3020.this.MJele.start(1, "GO");
            System.sleep(195);
            Sound.effectPlay(196715);
            ST3020.this.kanban.start(1, "nobiA");
            System.sleep(45);
            ST3020.this.player.mtn(2, 9, 1.0f, true);
            ST3020.this.player.move(60, 12.0f, -4.5f, true);
            System.sleep(65);
            Runtime.disable(65536);
            Sound.effectPlay(196715);
            ST3020.this.kanban.start(1, "nobiB");
            System.sleep(45);
            Runtime.setFlags(3132, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void GO() {
            int n = 0;
            ST3020.this.MJele.getTranslate();
            ST3020.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196749);
                }
                if (n >= 0 && n < 180) {
                    ST3020.this.kanban.getTranslate();
                    ST3020.this.kanban.setTranslate(ST3020.this.kanban.px, ST3020.this.kanban.py - 0.055555556f, ST3020.this.kanban.pz);
                    ST3020.this.MJele.getTranslate();
                    ST3020.this.MJele.setTranslate(ST3020.this.MJele.px, ST3020.this.MJele.py - 0.055555556f, ST3020.this.MJele.pz);
                    ST3020.this.player.getTranslate();
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py - 0.055555556f, ST3020.this.player.pz);
                }
                if (n == 180) break;
                ++n;
                System.sleep(1);
            }
        }

        void Sdown() {
            int n = 0;
            ST3020.this.Step.getTranslate();
            ST3020.this.player.getTranslate();
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST3020.this.cam0.setMode(-1);
            ST3020.this.EV_Camera03();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 180) {
                    ST3020.this.Step.setTranslate(ST3020.this.Step.px, ST3020.this.Step.py - 0.08888889f, ST3020.this.Step.pz + 0.08888889f);
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py - 0.08888889f, ST3020.this.player.pz + 0.08888889f);
                    ST3020.this.E03.setTranslate(ST3020.this.E03.px, ST3020.this.E03.py - 0.08888889f, ST3020.this.E03.pz + 0.08888889f);
                }
                if (n == 180) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.E01.disp(false);
            ST3020.this.E02.disp(false);
            Sound.effectStop(196750);
            Sound.effectPlay(196747);
            Runtime.setFlags(3149, 1, 0);
            Runtime.setFlags(3090, 1, 1);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3020.this.cam0.setMode(0);
        }

        void Up() {
            int n = 0;
            ST3020.this.MJele.getTranslate();
            ST3020.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196746);
                }
                if (n == 30) {
                    Sound.effectPlay(196750);
                }
                if (n == 900) {
                    Sound.effectStop(196750);
                    Sound.effectPlay(196747);
                }
                if (n >= 0 && n < 900) {
                    ST3020.this.Step.setTranslate(ST3020.this.Step.px, ST3020.this.Step.py + 0.08444444f, ST3020.this.Step.pz - 0.08444444f);
                    ST3020.this.player.setTranslate(ST3020.this.player.px, ST3020.this.player.py + 0.08444444f, ST3020.this.player.pz - 0.08445556f);
                    ST3020.this.E03.setTranslate(ST3020.this.E03.px, ST3020.this.E03.py + 0.08444444f, ST3020.this.E03.pz - 0.08444444f);
                }
                if (n == 900) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.E01.disp(false);
            ST3020.this.E02.disp(false);
            Runtime.setFlags(3090, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3020.this.cam0.setMode(0);
        }

        void nobiA() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 45) {
                    ST3020.this.kanban.getTranslate();
                    ST3020.this.kanban.setTranslate(ST3020.this.kanban.px, ST3020.this.kanban.py + 0.044444446f, ST3020.this.kanban.pz);
                }
                if (n == 45) break;
                ++n;
                System.sleep(1);
            }
            ST3020.this.cam0.setMode(0);
        }

        void nobiB() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 45) {
                    ST3020.this.kanban.getTranslate();
                    ST3020.this.kanban.setTranslate(ST3020.this.kanban.px, ST3020.this.kanban.py - 0.044444446f, ST3020.this.kanban.pz);
                }
                if (n == 45) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

