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
import xeno.map.MC_ELS12B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0730
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS12B_PRJ {
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
    boolean npc1flg = false;
    boolean npc2flg = false;
    int button1_flg = 0;
    int button2_flg = 0;
    int e1pass = 0;
    int e2pass = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono col1;
    Uwamono col2;
    Uwamono col3;
    Uwamono col4;
    Uwamono trap1;
    Unit AGWS1;
    Unit L_dodai;
    Unit L_sasae1;
    Unit L_sasae2;
    Unit L_tesuri1;
    Unit L_tesuri2;
    Unit L_tesuri3;
    Unit L_tesuri4;
    Unit L_tesuri5;
    Unit L_tesuri6;
    Unit L_sousa;
    Unit R_dodai;
    Unit R_sasae1;
    Unit R_sasae2;
    Unit R_tesuri1;
    Unit R_tesuri2;
    Unit R_tesuri3;
    Unit R_tesuri4;
    Unit R_tesuri5;
    Unit R_tesuri6;
    Unit R_sousa;
    Unit elv;
    Unit elv2;
    Unit Ll;
    Unit Lr;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect eve04;
    Effect eve05;
    Effect eve06;
    Effect eve07;
    Effect eve08;
    Effect eve09;
    Effect eve10;
    Effect eve11;
    Effect eve12;
    Effect eve13;
    Effect eve14;
    Effect eve15;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] ELE = new String[]{"Use the lift?", "/[waitkey(64)]/[close()]"};

    ST0730() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            if (this.e1pass != 0) return;
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3026, 1) == 0) {
                        this.e1pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.elv.setArgs(4, 0.058333334f);
                                this.elv.setArgs(12, 2.5f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            case 1: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            default: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                    } else {
                        if (Runtime.getFlags(3026, 1) != 1) return;
                        this.e1pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.elv.setArgs(4, 0.058333334f);
                                this.elv.setArgs(12, -1.0f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move2");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            case 1: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            default: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    this.e1pass = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    this.e1pass = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 3) {
            if (this.e2pass != 0) return;
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3027, 1) == 0) {
                        this.e2pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.elv2.setArgs(4, 0.058333334f);
                                this.elv2.setArgs(12, -1.0f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move3");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            case 1: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            default: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                    } else {
                        if (Runtime.getFlags(3027, 1) != 1) return;
                        this.e2pass = 1;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ELE, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.elv2.setArgs(4, 0.058333334f);
                                this.elv2.setArgs(12, 2.5f);
                                Sound.effectPlay(196742);
                                this.L_dodai.start(1, "Move4");
                                System.sleep(60);
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            case 1: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                            default: {
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    this.e2pass = 0;
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 5) return;
        switch (n) {
            case 100: {
                this.e2pass = 0;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66246, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(66256, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(66276, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(129, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, -1.1f, -14.5f, 0.0f);
        this.teiten1.SetBgm(196631);
        this.teiten2 = new Uwamono(28690, -3.0f, -1.1f, 6.0f, 0.0f);
        this.teiten2.SetBgm(196637);
        this.teiten3 = new Uwamono(28690, 3.0f, -1.1f, -6.0f, 0.0f);
        this.teiten3.SetBgm(196637);
        this.teiten4 = new Uwamono(28690, -3.043f, -1.339f, -3.067f, 0.0f);
        this.teiten4.SetBgm(196638);
        this.teiten5 = new Uwamono(28690, -4.193f, -1.339f, 8.203f, 0.0f);
        this.teiten5.SetBgm(196638);
        this.teiten6 = new Uwamono(28690, 4.138f, -1.339f, -3.603f, 0.0f);
        this.teiten6.SetBgm(196638);
        this.teiten7 = new Uwamono(28690, 2.3421f, -1.339f, 7.3692f, 0.0f);
        this.teiten7.SetBgm(196638);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setFog(1, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, -6.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 6.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 15.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, -15.0f, 0.0f, 4.8f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        Stage.renderCommand(4);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16641, 3, -1.5f, -1.137f, -6.0f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[16];
        fArray[0] = -1.5f;
        fArray[1] = -1.137f;
        fArray[2] = -6.0f;
        fArray[3] = 1.0f;
        fArray[5] = -1.137f;
        fArray[6] = -6.0f;
        fArray[7] = 2.0f;
        fArray[9] = -1.137f;
        fArray[10] = -8.0f;
        fArray[11] = 3.0f;
        fArray[13] = -1.137f;
        fArray[14] = -11.0f;
        fArray[15] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 2, 1, 3, fArray2);
        this.enemy1.enableDTKFlag(262144);
        this.enemy1.renderCommand(22);
        this.enemy2 = new Enepc();
        this.enemy2.init(16641, 3, 1.5f, -1.137f, 0.0f, 270.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray3 = new float[8];
        fArray3[0] = 1.5f;
        fArray3[1] = -1.137f;
        fArray3[3] = 1.0f;
        fArray3[5] = -1.137f;
        fArray3[7] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(1, 2, 2, 3, fArray4);
        this.enemy2.enableDTKFlag(262144);
        this.enemy2.renderCommand(22);
        this.enemy3 = new Enepc();
        this.enemy3.init(16641, 3, -1.5f, -1.137f, 6.0f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[16];
        fArray5[0] = -1.5f;
        fArray5[1] = -1.137f;
        fArray5[2] = 6.0f;
        fArray5[3] = 1.0f;
        fArray5[5] = -1.137f;
        fArray5[6] = 6.0f;
        fArray5[7] = 2.0f;
        fArray5[9] = -1.137f;
        fArray5[10] = 9.0f;
        fArray5[11] = 3.0f;
        fArray5[13] = -1.137f;
        fArray5[14] = 11.5f;
        fArray5[15] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy3.setParams(1, 2, 3, 3, fArray6);
        this.enemy3.enableDTKFlag(262144);
        this.enemy3.renderCommand(22);
        this.npc2 = new NPC_NORMAL(8450, 12, 0, 3, 6, 5.0f, -1.0f, -6.0f, 270.0f);
        this.npc2.setMotion(0, 2);
        this.npc2.dispRadar(false);
        this.npc2.renderCommand(22);
        this.npc5 = new NPC_NORMAL(8193, 15, 0, 3, 6, -5.0f, -1.0f, 6.0f, 90.0f);
        this.npc5.setMotion(0, 2);
        this.npc5.dispRadar(false);
        this.npc5.renderCommand(22);
        new Uwamono(13, 0);
        new Uwamono(14, 0);
        new Uwamono(15, 0);
        this.doorA = new Uwamono(19, 42, '\u0001');
        new Uwamono(21, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(12, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(40, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.col1 = new Uwamono(28672, -6.0f, -1.0f, -14.5f, 0.0f);
        this.col2 = new Uwamono(28672, 6.0f, -1.0f, -14.5f, 0.0f);
        this.col3 = new Uwamono(28672, -6.0f, 2.5f, -11.2f, 0.0f);
        this.col4 = new Uwamono(28672, 6.0f, 2.5f, -11.2f, 0.0f);
        this.L_dodai = new Mapunits();
        this.L_dodai.mapUnit(161);
        this.L_dodai.start(4, null);
        if (Runtime.getFlags(3026, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(171, 3.5f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(4, 100.0f);
            this.elv.setArgs(12, -1.0f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(171, 3.5f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(4, 100.0f);
            this.elv.setArgs(12, 2.5f);
        }
        if (Runtime.getFlags(3027, 1) == 0) {
            this.elv2 = new Unit();
            this.elv2.initElevator(170, 3.5f, 0.0f);
            this.elv2.setArgs(1, 1, 1);
            this.elv2.setArgs(4, 100.0f);
            this.elv2.setArgs(12, 2.5f);
        } else {
            this.elv2 = new Unit();
            this.elv2.initElevator(170, 3.5f, 0.0f);
            this.elv2.setArgs(1, 1, 1);
            this.elv2.setArgs(4, 100.0f);
            this.elv2.setArgs(12, -1.0f);
        }
        this.eve02 = new Effect(1422, 2);
        this.eve02.disp(true);
        this.eve02.setClip(true);
        this.eve03 = new Effect(1422, 3);
        this.eve03.disp(true);
        this.eve03.setClip(true);
        this.eve08 = new Effect(1423, 8);
        this.eve08.disp(true);
        this.eve08.setClip(true);
        this.eve09 = new Effect(1423, 9);
        this.eve09.disp(true);
        this.eve09.setClip(true);
        this.E02 = new Effect(1402, -3.043f, -1.339f, -3.067f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E04 = new Effect(1402, -4.193f, -1.339f, 8.203f, 0.0f);
        this.E04.disp(true);
        this.E04.setClip(true);
        this.E06 = new Effect(1402, 4.138f, -1.339f, -3.603f, 0.0f);
        this.E06.disp(true);
        this.E06.setClip(true);
        this.E07 = new Effect(1402, 2.3421f, -1.339f, 7.3692f, 0.0f);
        this.E07.disp(true);
        this.E07.setClip(true);
        if (Runtime.getFlags(3026, 1) == 0) {
            this.eve12 = new Effect(1419, 12);
            this.eve12.noAttach(false);
            this.eve12.disp(true);
            this.eve12.setTranslate(this.eve12.px, this.eve12.py - 3.5f, this.eve12.pz);
        } else if (Runtime.getFlags(3026, 1) == 1) {
            this.eve12 = new Effect(1419, 12);
            this.eve12.noAttach(false);
            this.eve12.disp(true);
        }
        if (Runtime.getFlags(3027, 1) == 0) {
            this.eve14 = new Effect(1419, 13);
            this.eve14.noAttach(false);
            this.eve14.disp(true);
        } else if (Runtime.getFlags(3027, 1) == 1) {
            this.eve14 = new Effect(1419, 13);
            this.eve14.noAttach(false);
            this.eve14.disp(true);
            this.eve14.setTranslate(this.eve14.px, this.eve14.py - 3.5f, this.eve14.pz);
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

    class Mapunits
            extends Unit {
        Mapunits() {
        }

        void Move() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0730.this.eve12.getTranslate();
                    ST0730.this.eve12.setTranslate(ST0730.this.eve12.px, ST0730.this.eve12.py + 0.058333334f, ST0730.this.eve12.pz);
                    System.println("move");
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3026, 1, 1);
        }

        void Move2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0730.this.eve12.getTranslate();
                    ST0730.this.eve12.setTranslate(ST0730.this.eve12.px, ST0730.this.eve12.py - 0.058333334f, ST0730.this.eve12.pz);
                    System.println("move");
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3026, 1, 0);
        }

        void Move3() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0730.this.eve14.getTranslate();
                    ST0730.this.eve14.setTranslate(ST0730.this.eve14.px, ST0730.this.eve14.py - 0.058333334f, ST0730.this.eve14.pz);
                    System.println("move");
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3027, 1, 1);
        }

        void Move4() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0730.this.eve14.getTranslate();
                    ST0730.this.eve14.setTranslate(ST0730.this.eve14.px, ST0730.this.eve14.py + 0.058333334f, ST0730.this.eve14.pz);
                    System.println("move");
                }
                if (n == 61) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3027, 1, 0);
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
}

