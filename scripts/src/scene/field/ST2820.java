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
import xeno.map.MC_UTA12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2820
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA12_PRJ {
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
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    boolean light04_on = false;
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
    MAPUnit elv;
    MAPUnit start;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    int ele_move = 0;
    boolean player_move = true;
    Light light = new Light(0);
    int entrance;
    Effect fade;
    int page;
    String[] ele_serifu = new String[]{"Operate the elevator?", "/[waitkey(64)]/[close()]"};

    ST2820() {
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
                if (Runtime.getFlags(8057, 1) == 1) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("1");
                    if (this.player.py > 15.0f && Runtime.getFlags(8057, 1) == 0) {
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        return;
                    }
                    if (Runtime.getFlags(8053, 1) == 0) {
                        if (!(this.player.py > 15.0f)) {
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                        this.nwin(this.ele_serifu);
                        this.yesno();
                    } else {
                        if (!(this.player.py < 15.0f)) {
                            Runtime.setPlayerControl(true);
                            this.lo = 0;
                            return;
                        }
                        this.nwin(this.ele_serifu);
                        this.yesno();
                    }
                    switch (this.selected) {
                        case 0: {
                            this.player.getTranslate();
                            Runtime.setRegister(1, this.player.py);
                            System.println("player.py: /[#1]");
                            if (this.player.py > 15.0f) {
                                System.println("上");
                                this.camEV.setFov(40.0f);
                                this.camEV.setRotate(-81.04f, 0.0f, 0.0f);
                                this.camEV.setTranslate(0.15f, 43.13f, 1.07f);
                                this.camEV.change();
                                this.player_move = false;
                                this.ele_move = 2;
                                return;
                            }
                            System.println("下");
                            this.player_move = false;
                            this.ele_move = 1;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
            }
            case 1: {
                if (Runtime.getFlags(8057, 1) == 1) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("2");
                    this.nwin(this.ele_serifu);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.player.getTranslate();
                            Runtime.setRegister(1, this.player.py);
                            System.println("player.py: /[#1]");
                            if (this.player.py > 15.0f) {
                                System.println("上");
                                this.ele_move = 1;
                                this.player_move = true;
                                return;
                            }
                            System.println("下");
                            this.ele_move = 2;
                            this.player_move = true;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
            }
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("3");
                this.lo = 0;
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("4");
                this.lo = 0;
                Runtime.setPlayerControl(true);
                return;
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
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(2870, 8);
                break;
            }
            case 1: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(68366, 1);
                break;
            }
            case 2: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(2870, 12);
                break;
            }
            case 3: {
                this.cam0.setRotate(0.0f, 0.0f, 0.0f);
                Runtime.jumpCF(2870, 9);
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
        this.start = new Mapunits();
        this.start.mapUnit(0);
        this.start.start(4, null);
        this.light01 = new Effect(1020, -0.65f, 30.01f, 1.5f, 0.0f);
        this.light01.setRotate(90.0f, 0.0f, 0.0f);
        this.light01.setScale(1.0f, 1.0f, 2.0f);
        this.light01.disp(true);
        if (Runtime.getFlags(8057, 1) == 0) {
            this.light01.disp(false);
        }
        this.light03 = new Effect(1587, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light03.noAttach(false);
        this.light03.disp(false);
        this.light04 = new Effect(1542, -0.25f, 0.03f, 1.9f, 0.0f);
        this.light04.setRotate(90.0f, 0.0f, 0.0f);
        this.light04.setScale(0.3f, 0.6f, 0.3f);
        this.light04.disp(false);
        this.light04_on = true;
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.player.setID(2);
        this.camEV = Camera.create(1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -90.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFLockX(2, 0.0f);
        this.elv = new Mapunits();
        this.elv.mapUnit(85);
        this.elv.start(4, null);
        if (Runtime.getFlags(8053, 1) == 0) {
            this.elv.setTranslate(this.elv.px, 0.0f, this.elv.pz);
            this.player.setID(2);
            this.light04_on = false;
        } else {
            this.elv.setTranslate(this.elv.px, 30.0f, this.elv.pz);
            this.player.setID(1);
        }
        this.start.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2820.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2820.waitPage(this.win, 64);
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
            System.sleep(1);
            switch (ST2820.this.entrance) {
                case 0: {
                    System.println("0");
                    ST2820.this.cam0.changeID(1);
                    break;
                }
                case 1: {
                    System.println("1");
                    ST2820.this.cam0.changeID(1);
                    break;
                }
                case 2: {
                    System.println("2");
                    ST2820.this.cam0.changeID(1);
                    break;
                }
                case 3: {
                    System.println("3");
                    ST2820.this.cam0.changeID(1);
                    break;
                }
                case 4: {
                    System.println("4");
                    ST2820.this.cam0.changeID(2);
                    break;
                }
            }
            float f = 0.0f;
            float f2 = 460.0f;
            float f3 = 50.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 0.0f;
            float f8 = 30.0f;
            float f9 = 30.0f;
            boolean bl = false;
            while (true) {
                ST2820.this.player.getTranslate();
                if (ST2820.this.ele_move != 0) {
                    ST2820.this.elv.getTranslate();
                    if (f == 0.0f) {
                        Sound.streamPlay(1195011, 48000);
                        if (ST2820.this.ele_move == 1) {
                            f5 = 30.0f;
                            f6 = 0.0f;
                            if (ST2820.this.player_move) {
                                bl = false;
                            } else {
                                bl = true;
                                ST2820.this.cam0.setMode(-1);
                                ST2820.this.camEV.setTranslate(0.0f, 8.0f, 10.5f);
                                ST2820.this.camEV.setFov(45.0f);
                                ST2820.this.camEV.change();
                            }
                        }
                        if (ST2820.this.ele_move == 2) {
                            f5 = 0.0f;
                            f6 = 30.0f;
                        }
                        Runtime.enable(65536);
                        f4 = (f6 - f5) / (f2 - f3);
                        Runtime.disable(524288);
                        ST2820.this.light04_on = false;
                        ST2820.this.light03.disp(true);
                    }
                    if (f == 60.0f && ST2820.this.ele_move == 1) {
                        ST2820.this.cam0.changeID(2);
                    }
                    if (f > 0.0f && f <= f3) {
                        ST2820.this.elv.setTranslate(ST2820.this.elv.px, f5 += f * f4 / f3, ST2820.this.elv.pz);
                        ST2820.this.light03.setTranslate(ST2820.this.elv.px, f5, ST2820.this.elv.pz);
                        if (ST2820.this.player_move) {
                            ST2820.this.player.setTranslate(ST2820.this.player.px, f5, ST2820.this.player.pz);
                        }
                    }
                    if (f > f3 && f <= f2 - f3) {
                        ST2820.this.elv.setTranslate(ST2820.this.elv.px, f5 += f3 * f4 / f3, ST2820.this.elv.pz);
                        ST2820.this.light03.setTranslate(ST2820.this.elv.px, f5, ST2820.this.elv.pz);
                        if (ST2820.this.player_move) {
                            ST2820.this.player.setTranslate(ST2820.this.player.px, f5, ST2820.this.player.pz);
                        }
                    }
                    if (f > f2 - f3 && f <= f2) {
                        ST2820.this.elv.setTranslate(ST2820.this.elv.px, f5 += (f2 - f) * f4 / f3, ST2820.this.elv.pz);
                        ST2820.this.light03.setTranslate(ST2820.this.elv.px, f5, ST2820.this.elv.pz);
                        if (ST2820.this.player_move) {
                            ST2820.this.player.setTranslate(ST2820.this.player.px, f5, ST2820.this.player.pz);
                        }
                    }
                    if (f > f8 && f <= f2 - f8) {
                        ST2820.this.elv.setRotate(0.0f, (f - f8) * (360.0f / (f2 - f8 * 2.0f)), 0.0f);
                        ST2820.this.light03.setRotate(0.0f, (f - f8) * (360.0f / (f2 - f8 * 2.0f)), 0.0f);
                    }
                    boolean bl2 = f > 0.0f && f <= f2;
                    if (f == f2 - 60.0f && ST2820.this.ele_move == 2) {
                        ST2820.this.cam0.changeID(1);
                    }
                    if (f >= f2) {
                        f = -1.0f;
                        ST2820.this.lo = 0;
                        bl = false;
                        Runtime.enable(524288);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        ST2820.this.light03.disp(false);
                        ST2820.this.light03.clearEffect();
                        if (ST2820.this.ele_move == 1) {
                            ST2820.this.player.setID(2);
                            Runtime.setFlags(8053, 1, 0);
                            if (!ST2820.this.player_move) {
                                ST2820.this.cam0.setMode(0);
                                System.sleep(1);
                                ST2820.this.cam0.changeID(2);
                            }
                        }
                        if (ST2820.this.ele_move == 2) {
                            ST2820.this.player.setID(1);
                            Runtime.setFlags(8053, 1, 1);
                            ST2820.this.light04_on = true;
                            ST2820.this.cam0.changeID(1);
                            if (!ST2820.this.player_move) {
                                ST2820.this.cam0.setMode(0);
                            }
                        }
                        ST2820.this.ele_move = 0;
                    }
                    f += 1.0f;
                }
                if (Runtime.getFlags(8057, 1) == 1) {
                    if (f7 == 0.0f) {
                        ST2820.this.light04.disp(true);
                    }
                    if (f7 == 15.0f) {
                        ST2820.this.light04.disp(false);
                    }
                    if ((f7 += 1.0f) == 30.0f) {
                        f7 = 0.0f;
                    }
                }
                if (bl) {
                    ST2820.this.camEV.setView(ST2820.this.elv.px, ST2820.this.elv.py, ST2820.this.elv.pz);
                }
                System.sleep(1);
            }
        }
    }
}

