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
import xeno.map.MC_UTA02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2720
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA02_PRJ {
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
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
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
    Uwamono U1;
    Uwamono U2;
    Uwamono U3;
    Uwamono U4;
    Uwamono U5;
    Uwamono U6;
    Uwamono U7;
    Uwamono U8;
    Uwamono U9;
    Uwamono U10;
    Uwamono U10a;
    Uwamono co01;
    Unit elv;
    MAPUnit start;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    boolean rakka = false;
    int rakka_h = 0;
    boolean ele_move = false;
    float ele_d = 0.0f;
    int weight = 0;
    float cam_h = 13.0f;
    boolean ele_up = false;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] elel_up = new String[]{"Operate the elevator?", "/[waitkey(64)]/[close()]"};
    String[] dontmove = new String[]{"No response. It seems to be linked to something.", "/[waitkey(64)]/[close()]"};
    String[] dontmove2 = new String[]{"No response.", "/[waitkey(64)]/[close()]"};
    String[] locked = new String[]{"It is too steep to climb.", "/[waitkey(64)]/[close()]"};

    ST2720() {
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
                if (!this.ele_up) {
                    this.lo = 1;
                    Runtime.setPlayerControl(false);
                    if (this.weight == 0 || this.weight % 3 != 0) {
                        this.nwin(this.dontmove);
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        return;
                    }
                    this.nwin(this.elel_up);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.enable(65536);
                            this.player.rotY(10, 180.0f, true);
                            System.sleep(10);
                            this.player.mtn(25, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(58);
                            System.sleep(10);
                            Runtime.disable(65536);
                            this.lo = 0;
                            this.ele_d = -8.0f + ((float) this.weight + 3.0f) / 3.0f * 10.0f;
                            this.ele_move = true;
                            Sound.effectPlay(196742);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                this.lo = 1;
                Runtime.setPlayerControl(false);
                this.nwin(this.dontmove2);
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.nwin(this.locked);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                this.hantei(1);
                break;
            }
            case 2: {
                this.hantei(2);
                break;
            }
            case 3: {
                this.hantei(1);
                break;
            }
            case 4: {
                this.hantei(1);
                break;
            }
            case 5: {
                this.hantei(2);
                break;
            }
            case 6: {
                this.hantei(4);
                break;
            }
            case 7: {
                this.hantei(2);
                break;
            }
            case 8: {
                this.hantei(1);
                break;
            }
            case 9: {
                this.hantei(1);
                break;
            }
            case 10: {
                this.U10a.setTranslate(0.0f, -20.0f, 20.0f);
                this.hantei(3);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.super_setFlags(8042, 3, 0);
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(2879, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68275, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(68285, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(68295, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(68305, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(68315, 1);
                break;
            }
            case 6: {
                Runtime.jumpCF(68325, 1);
                break;
            }
        }
    }

    void hantei(int n) {
        if (!this.ele_up) {
            this.weight += n;
            Runtime.setRegister(1, (float) this.weight);
            System.println("weight: /[#1]");
            if (this.weight % 3 == 0) {
                System.println("移動可");
                Sound.effectPlay(196741);
                this.light01.disp(true);
            } else {
                this.light01.disp(false);
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 0.0f, 2.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, 0.0f, 12.0f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, 0.0f, 22.0f, 0.0f, 0.0f);
        this.teiten3.SetBgm(196609);
        this.teiten4 = new Uwamono(28690, 0.0f, 32.0f, 0.0f, 0.0f);
        this.teiten4.SetBgm(196609);
        this.teiten5 = new Uwamono(28690, 0.0f, 42.0f, 0.0f, 0.0f);
        this.teiten5.SetBgm(196609);
        this.teiten6 = new Uwamono(28690, 0.0f, 52.0f, 0.0f, 0.0f);
        this.teiten6.SetBgm(196609);
        this.teiten7 = new Uwamono(28690, 0.0f, 62.0f, 0.0f, 0.0f);
        this.teiten7.SetBgm(196609);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightCol(1, 3, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        this.start = new Mapunits();
        this.start.mapUnit(0);
        this.start.start(4, null);
        this.light01 = new Effect(1584, 0.0f, 6.0f, -0.6f, 0.0f);
        this.light01.noAttach(false);
        this.light01.disp(false);
        this.light02 = new Effect(1056, 0.0f, 3.43f, -0.1f, 0.0f);
        this.light02.noAttach(false);
        this.light02.setRotate(30.0f, 0.0f, 0.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1020, 1.65f, 13.4f, -10.1f, 0.0f);
        this.light03.setScale(8.0f, 1.0f, 1.0f);
        this.light04 = new Effect(1020, 1.65f, 23.4f, -10.1f, 0.0f);
        this.light04.setScale(8.0f, 1.0f, 1.0f);
        this.light05 = new Effect(1020, 1.65f, 33.4f, -10.1f, 0.0f);
        this.light05.setScale(8.0f, 1.0f, 1.0f);
        this.light06 = new Effect(1020, 1.65f, 43.4f, -10.1f, 0.0f);
        this.light06.setScale(8.0f, 1.0f, 1.0f);
        this.light07 = new Effect(1020, 1.65f, 53.4f, -10.1f, 0.0f);
        this.light07.setScale(8.0f, 1.0f, 1.0f);
        this.light08 = new Effect(1020, 1.65f, 63.4f, -10.1f, 0.0f);
        this.light08.setScale(8.0f, 1.0f, 1.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setShootRange(0.8f);
        this.cam0.setCFPedestal(1, -3.39f, 13.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(1, 1);
        this.cam0.setCFPedestal(2, -3.39f, 23.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFPedestal(3, -3.39f, 33.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFPedestal(4, -3.39f, 43.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(4, 1);
        this.cam0.setCFPedestal(5, -3.39f, 53.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(5, 1);
        this.cam0.setCFPedestal(6, -3.39f, 63.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(6, 1);
        this.cam0.setCFPedestal(7, -3.39f, 73.0f, 16.89f, 40.0f, -30.25f, -16.82f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.elv = new Unit();
        this.elv.initElevator(60, 0.2f, 2.0f);
        this.elv.setArgs(12, 2.0f);
        this.U1 = new Uwamono(14, 0);
        this.U1.SetCallNo(1);
        this.U1.setElevatorMode(1);
        this.U2 = new Uwamono(15, 1);
        this.U2.SetCallNo(2);
        this.U2.setElevatorMode(1);
        this.U3 = new Uwamono(16, 0);
        this.U3.SetCallNo(3);
        this.U3.setElevatorMode(1);
        this.U4 = new Uwamono(33, 0);
        this.U4.SetCallNo(4);
        this.U4.setElevatorMode(1);
        this.U5 = new Uwamono(34, 1);
        this.U5.SetCallNo(5);
        this.U5.setElevatorMode(1);
        this.U6 = new Uwamono(43, 3);
        this.U6.SetCallNo(6);
        this.U6.setElevatorMode(1);
        this.U7 = new Uwamono(44, 32);
        this.U7.SetCallNo(7);
        this.U7.setElevatorMode(1);
        this.U8 = new Uwamono(46, 0);
        this.U8.SetCallNo(8);
        this.U8.setElevatorMode(1);
        this.U9 = new Uwamono(47, 0);
        this.U9.SetCallNo(9);
        this.U9.setElevatorMode(1);
        this.U10 = new Uwamono(45, 22);
        this.U10.SetCallNo(10);
        this.U10.setElevatorMode(1);
        this.U10a = new Uwamono(28672, -1.45f, 2.0f, -3.54f, 0.0f);
        this.U10a.setElevatorMode(1);
        this.U10a.SetSize(1.0f, 1.0f, 1.0f);
        this.co01 = new Uwamono(28672, -0.0f, 2.0f, -5.9f, 0.0f);
        this.co01.SetSize(3.0f, 1.0f, 0.1f);
        this.rakka_h = this.super_getFlags(8042, 3);
        if (this.rakka_h == 1) {
            this.cam0.setMode(-1);
            this.cam0.setRotate(-30.25f, -16.82f, 0.0f);
            this.cam0.setTranslate(-3.39f, this.cam_h, 16.89f);
        }
        if (this.rakka_h > 1) {
            this.rakka = true;
        }
        this.start.start(1, "idle");
    }

    void menu(String string) {
        this.menu = Menu.create();
        this.menu.addItem(string);
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2720.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2720.waitPage(this.win, 64);
    }

    int super_getFlags(int n, int n2) {
        int n3 = 0;
        int n4 = 0;
        int n5 = n2 - 1;
        while (n5 >= 0) {
            n4 = Runtime.getFlags(n + n5, 1);
            n3 += (n4 <<= n5);
            --n5;
        }
        Runtime.setRegister(1, (float) n3);
        System.println("super_getFlags: /[#1]");
        return n3;
    }

    void super_setFlags(int n, int n2, int n3) {
        Runtime.setRegister(1, (float) n3);
        System.println("super_setFlags: /[#1]");
        int n4 = 0;
        int n5 = 0;
        while (n5 < n2) {
            n4 = n3;
            n4 >>= n5;
            Runtime.setFlags(n + n5, 1, n4 &= 1);
            ++n5;
        }
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
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = (float) (ST2720.this.rakka_h * 10) + 6.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 50.0f;
            float f8 = 0.0f;
            float f9 = 0.0f;
            while (true) {
                if (ST2720.this.rakka) {
                    f4 = f3 - f / 30.0f * f / 30.0f * 3.8f;
                    if (f == 0.0f) {
                        Runtime.setPlayerControl(false);
                        ST2720.this.player.setTranslate(6.54f, 2.0f, -6.59f);
                        Runtime.enable(65536);
                        ST2720.this.player.mtn(4, 1, 1.5f, true);
                        Runtime.disable(524288);
                        ST2720.this.cam0.setMode(-1);
                    }
                    if (f4 > 14.0f) {
                        ST2720.this.cam0.setRotate(-70.38f, 0.0f, 0.0f);
                        ST2720.this.cam0.setTranslate(7.23f, f4, -1.12f);
                    }
                    if (f4 < 14.0f) {
                        if (f2 == 0.0f) {
                            ST2720.this.player.move(30, 2.36f, -2.51f, true);
                        }
                        if (f2 == 30.0f) {
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            ST2720.this.rakka = false;
                            Runtime.enable(524288);
                            ST2720.this.cam0.setMode(-1);
                            ST2720.this.cam0.setRotate(-30.25f, -16.82f, 0.0f);
                            ST2720.this.cam0.setTranslate(-3.39f, ST2720.this.cam_h, 16.89f);
                        }
                        f2 += 1.0f;
                    }
                    f += 1.0f;
                }
                if (ST2720.this.ele_move) {
                    if (f5 == 0.0f) {
                        f6 = (ST2720.this.ele_d - 2.0f) * 5.0f + f7 * 2.0f;
                        f9 = (ST2720.this.ele_d - 2.0f) / (f6 - f7);
                        ST2720.this.light01.getTranslate();
                        ST2720.this.light02.disp(false);
                        Runtime.disable(524288);
                    }
                    if (f5 > 0.0f && f5 <= f7) {
                        ST2720.this.elv.setArgs(12, (f8 += f5 * f9 / f7) + 2.0f);
                    }
                    if (f5 > f7 && f5 <= f6 - f7) {
                        ST2720.this.elv.setArgs(12, (f8 += f7 * f9 / f7) + 2.0f);
                    }
                    if (f5 > f6 - f7 && f5 <= f6) {
                        ST2720.this.elv.setArgs(12, (f8 += (f6 - f5) * f9 / f7) + 2.0f);
                    }
                    if (f5 > 0.0f && f5 <= f6) {
                        ST2720.this.cam0.setRotate(-30.25f, -16.82f, 0.0f);
                        ST2720.this.cam0.setTranslate(-3.39f, f8 + ST2720.this.cam_h, 16.89f);
                        ST2720.this.light01.setTranslate(ST2720.this.light01.px, f8 + 6.0f, ST2720.this.light01.pz);
                    }
                    if (f5 >= f6) {
                        Sound.effectStop(196742);
                        Sound.effectPlay(196777);
                        Runtime.setPlayerControl(true);
                        ST2720.this.ele_up = true;
                        ST2720.this.ele_move = false;
                        Runtime.enable(524288);
                        ST2720.this.light01.disp(false);
                    }
                    f5 += 1.0f;
                }
                System.sleep(1);
            }
        }
    }
}

