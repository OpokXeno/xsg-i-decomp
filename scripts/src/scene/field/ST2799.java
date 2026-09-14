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
import xeno.map.MC_UTA09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST2790
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA09_PRJ {
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
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
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
    Uwamono box;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    MAPUnit wa;
    MAPUnit jyama;
    MAPUnit jyama2;
    int wa_move = 0;
    int jyama_move = 0;
    boolean jyama_down = false;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
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
    String[] btn = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 12.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 12.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 12, decoding complete.", "/[waitkey(64)]/[close()]"};
    int b_flg;
    String[] EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};

    ST2790() {
    }

    void EOB(int n) {
        if (n == 1) {
            Runtime.setFlags(8092, 1, 1);
        }
        if (n == 2) {
            Runtime.setFlags(8102, 1, 1);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 17);
            Runtime.setFlags(3222, 1, 1);
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
                if (this.player.py > 15.0f) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("下");
                this.nwin(this.btn);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        Runtime.enable(65536);
                        this.player.rotY(10, 0.0f, true);
                        System.sleep(10);
                        this.player.mtn(25, 1, 1.0f, true);
                        System.sleep(30);
                        Sound.effectPlay(58);
                        System.sleep(10);
                        Runtime.disable(65536);
                        this.cam0.setMode(-1);
                        this.camEV = Camera.create(1);
                        this.camEV.setRotate(-65.5f, 0.0f, 0.0f);
                        this.camEV.setTranslate(0.0f, 51.1f, 12.7f);
                        this.camEV.setFov(40.0f);
                        this.camEV.change();
                        Runtime.disable(524288);
                        this.wa_move = 1;
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 1: {
                Runtime.enable(262144);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("サブルート");
                if (Runtime.getFlags(3212, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3212, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3232, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3292, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3292, 1, 1);
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                Runtime.disable(262144);
                break;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("上");
                this.nwin(this.btn);
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
                        this.cam0.setMode(-1);
                        this.camEV = Camera.create(1);
                        this.camEV.setRotate(-65.5f, 0.0f, 0.0f);
                        this.camEV.setTranslate(0.0f, 51.1f, 12.7f);
                        this.camEV.setFov(40.0f);
                        this.camEV.change();
                        Runtime.disable(524288);
                        this.wa_move = 1;
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 3: {
                System.println("lo2 = true");
                this.player.getTranslate();
                if (this.player.py > 15.0f) {
                    return;
                }
                if (Runtime.getFlags(8092, 1) != 0) break;
                this.enemy1.kickEnepc(14, 0);
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("1");
                Runtime.setFlags(8051, 1, 1);
                this.light06.disp(false);
                if (Runtime.getFlags(8052, 1) != 1) break;
                System.println("両方壊れた");
                Runtime.setPlayerControl(false);
                this.jyama_move = 1;
                this.cam0.setMode(-1);
                this.camEV = Camera.create(1);
                this.camEV.setRotate(-35.1f, 0.0f, 0.0f);
                this.camEV.setTranslate(0.0f, 5.7f, 8.9f);
                this.camEV.setFov(40.0f);
                this.camEV.change();
                Runtime.disable(524288);
                break;
            }
            case 2: {
                System.println("2");
                Runtime.setFlags(8052, 1, 1);
                this.light05.disp(false);
                if (Runtime.getFlags(8051, 1) != 1) break;
                System.println("両方壊れた");
                Runtime.setPlayerControl(false);
                this.jyama_move = 1;
                this.cam0.setMode(-1);
                this.camEV = Camera.create(1);
                this.camEV.setRotate(-13.6f, 0.0f, 0.0f);
                this.camEV.setTranslate(0.0f, 3.0f, 10.0f);
                this.camEV.setFov(40.0f);
                this.camEV.change();
                Runtime.disable(524288);
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
                Runtime.jumpCF(68345, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(68345, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(68345, 3);
                break;
            }
            case 3: {
                Runtime.jumpCF(2879, 7);
                break;
            }
            case 4: {
                Runtime.jumpCF(2879, 5);
                break;
            }
            case 5: {
                Runtime.jumpCF(68345, 4);
                break;
            }
            case 6: {
                Runtime.jumpCF(68355, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(68345, 5);
                break;
            }
            case 8: {
                Runtime.jumpCF(68345, 6);
                break;
            }
            case 9: {
                Runtime.jumpCF(68345, 7);
                break;
            }
            case 10: {
                Runtime.jumpCF(2879, 6);
                break;
            }
        }
    }

    void evsExit() {
        System.println("evsExitをコールしました");
        if (this.b_flg == 1) {
            return;
        }
        this.b_flg = 1;
        Runtime.enable(262144);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.EVS, 0);
        ST2790.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                Runtime.evsExit();
                return;
            }
        }
        Runtime.setPlayerControl(true);
        Runtime.disable(262144);
        this.b_flg = 0;
    }

    void init() {
        new Uwamono(28734, 1.6f, 29.0f, 0.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 0.0f, 29.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196611);
        this.teiten2 = new Uwamono(28690, 0.0f, 0.0f, -5.4f, 0.0f);
        this.teiten2.SetBgm(196611);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, 0.0f, 0.0f);
        this.teiten3.SetBgm(196612);
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
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        this.light01 = new Effect(1020, -0.2f, 30.2f, -2.2f, 0.0f);
        this.light01.setRotate(90.0f, 0.0f, 0.0f);
        this.light01.setScale(2.0f, 1.0f, 1.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1020, 0.2f, 1.2f, -5.2f, 0.0f);
        this.light02.setRotate(90.0f, 0.0f, 0.0f);
        this.light02.setScale(2.0f, 1.0f, 1.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1659, -8.5f, 0.0f, 0.0f, 0.0f);
        this.light03.setRotate(0.0f, 90.0f, 0.0f);
        this.light03.noAttach(false);
        this.light03.disp(false);
        this.light04 = new Effect(1659, 8.5f, 0.0f, 0.0f, 0.0f);
        this.light04.setRotate(0.0f, -90.0f, 0.0f);
        this.light04.noAttach(false);
        this.light04.disp(false);
        this.light07 = new Effect(1659, -8.5f, 30.0f, 0.0f, 0.0f);
        this.light07.setRotate(180.0f, 90.0f, 0.0f);
        this.light07.noAttach(false);
        this.light07.disp(false);
        this.light08 = new Effect(1659, 8.5f, 30.0f, 0.0f, 0.0f);
        this.light08.setRotate(180.0f, -90.0f, 0.0f);
        this.light08.noAttach(false);
        this.light08.disp(false);
        this.light05 = new Effect(1585, 0.97f, 1.3f, 0.0f, 0.0f);
        this.light05.noAttach(false);
        this.light06 = new Effect(1585, -0.95f, 1.3f, 0.0f, 0.0f);
        this.light06.noAttach(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.player.setID(1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 0.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.7f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 4.275f);
        this.enemy1 = new NpcEnemy(20241, 1, 0, 34, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy1.kickEnepc(4, 1);
        this.enemy1.setVisible(false);
        this.enemy1.setInvalidID(1);
        this.enemy1.setMotion(0, 1);
        this.enemy1.dispRadar(false);
        this.enemy1.setGroup(0, 0, 0, 0);
        this.enemy1.setBatEvent(8);
        if (Runtime.getFlags(8051, 1) == 1 && Runtime.getFlags(8052, 1) == 1 && Runtime.getFlags(8092, 1) == 0) {
            this.enemy1.setTranslate(0.0f, 0.0f, 0.0f);
            this.enemy1.kickEnepc(4, 0);
            this.enemy1.setVisible(true);
        }
        if (Runtime.getFlags(8102, 1) == 0) {
            float[] fArray = new float[8];
            fArray[0] = 1.7f;
            fArray[1] = 20.0f;
            fArray[2] = -16.7f;
            fArray[3] = -1.0f;
            fArray[4] = -7.4f;
            fArray[5] = 20.0f;
            fArray[6] = -16.7f;
            float[] fArray2 = fArray;
            this.enemy2 = new NpcEnemy(16901, 2, 0, 14, 5, 1.7f, 20.0f, -16.7f, 270.0f, fArray2);
            this.enemy2.setGroup(1, 1, 1, 1);
        }
        this.jyama = new Mapunits();
        this.jyama.mapUnit(83);
        this.jyama.start(4, null);
        this.jyama2 = new Mapunits();
        this.jyama2.mapUnit(116);
        this.jyama2.start(4, null);
        if (Runtime.getFlags(8051, 1) == 1 && Runtime.getFlags(8052, 1) == 1) {
            this.jyama_down = true;
            this.jyama.getTranslate();
            this.jyama2.getTranslate();
            this.jyama.setTranslate(this.jyama.px, -1.425f, this.jyama.pz);
            this.jyama2.setTranslate(this.jyama2.px, -1.425f, this.jyama2.pz);
        }
        this.wa = new Mapunits();
        this.wa.mapUnit(143);
        this.wa.start(4, null);
        this.wa.getTranslate();
        if (n == 5) {
            Runtime.setFlags(8050, 1, 0);
        }
        if (Runtime.getFlags(8050, 1) == 1) {
            this.wa.setTranslate(this.wa.px, -30.0f, this.wa.pz);
            if (!this.jyama_down) {
                this.player.setID(2);
            } else {
                this.player.setID(3);
            }
        } else {
            this.player.setID(1);
            this.wa.setTranslate(this.wa.px, 0.0f, this.wa.pz);
        }
        this.item1 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 340);
        this.item4 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 343);
        if (Runtime.getFlags(8051, 1) == 0) {
            this.U1 = new Uwamono(81, 10);
            this.U1.SetCallNo(1);
        } else {
            Stage.setVisible(81, false);
            this.light06.disp(false);
        }
        if (Runtime.getFlags(8052, 1) == 0) {
            this.U2 = new Uwamono(248, 10);
            this.U2.SetCallNo(2);
        } else {
            Stage.setVisible(248, false);
            this.light05.disp(false);
        }
        new Uwamono(104, 4);
        new Uwamono(105, 4, this.item1);
        new Uwamono(106, 4, this.item4);
        if (Runtime.getFlags(8102, 1) == 0) {
            new Uwamono(107, 4, this.enemy2);
        } else {
            new Uwamono(107, 4);
        }
        if (Runtime.getFlags(3212, 1) == 0) {
            this.box = new Uwamono(108, 4);
        } else {
            Stage.setVisible(108, false);
        }
        this.doorA = new Uwamono(99, 40, '\u0004');
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDiffSize(0.0f, -0.01f, 0.0f);
        if (Runtime.getFlags(3292, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
        }
        this.wa.start(1, "idle");
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2790.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2790.waitPage(this.win, 64);
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
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            boolean bl = false;
            float f4 = -20.0f;
            if (Runtime.getFlags(8050, 1) == 1) {
                f2 = 360.0f;
            }
            ST2790.this.jyama.getTranslate();
            ST2790.this.wa.getTranslate();
            while (true) {
                if (ST2790.this.jyama_move == 1) {
                    Runtime.setPlayerControl(false);
                    if (f3 == 180.0f) {
                        ST2790.this.jyama_move = 0;
                        ST2790.this.jyama_down = true;
                        bl = true;
                        ST2790.this.player.setID(3);
                    }
                    ST2790.this.jyama.setTranslate(ST2790.this.jyama.px, Math.sin(f3 * 3.14f / 180.0f) * 0.75f - 0.675f, ST2790.this.jyama.pz);
                    ST2790.this.jyama2.setTranslate(ST2790.this.jyama2.px, Math.sin(f3 * 3.14f / 180.0f) * 0.75f - 0.675f, ST2790.this.jyama2.pz);
                    f3 += 1.0f;
                }
                if (bl) {
                    if (f4 == -20.0f) {
                        Sound.streamPlay(1195004, 48000);
                        float[] fArray = new float[8];
                        fArray[2] = 3.0f;
                        fArray[3] = 10.0f;
                        fArray[4] = 40.0f;
                        fArray[5] = -7.9f;
                        fArray[6] = 3.0f;
                        fArray[7] = 6.1f;
                        float[] fArray2 = fArray;
                        ST2790.this.camEV.setTranslate(0.0f, 3.0f, 10.0f);
                        float[] fArray3 = new float[8];
                        fArray3[1] = -13.6f;
                        fArray3[4] = 40.0f;
                        fArray3[5] = 26.0f;
                        float[] fArray4 = fArray3;
                        ST2790.this.camEV.rotateSPL(fArray4, 0, 1, 40);
                    }
                    if (f4 == 0.0f) {
                        ST2790.this.enemy1.kickEnepc(1, 0);
                    }
                    if (f4 == 10.0f) {
                        ST2790.this.enemy1.setVisible(true);
                        Runtime.enable(65536);
                        ST2790.this.player.look_char(ST2790.this.enemy1);
                    }
                    if (f4 >= 20.0f && f4 <= 285.0f) {
                        float f5 = Math.sin(-f4 * 360.0f / 190.0f * 3.14f / 180.0f);
                        float f6 = Math.cos(-f4 * 360.0f / 190.0f * 3.14f / 180.0f);
                        ST2790.this.camEV.setTranslate(f6 * 10.0f, 3.0f, f5 * 10.0f);
                        ST2790.this.camEV.setRotate(28.0f - f4 / 270.0f * 28.0f, -f4 * 360.0f / 190.0f, 0.0f);
                        ST2790.this.camEV.setFov(38.0f);
                    }
                    if (f4 == 494.0f) {
                        ST2790.this.enemy1.kickEnepc(4, 0);
                        ST2790.this.cam0.setMode(0);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        ST2790.this.player.look_default();
                        bl = false;
                    }
                    f4 += 1.0f;
                }
                if (ST2790.this.wa_move == 1) {
                    if (Runtime.getFlags(8050, 1) == 0) {
                        if (f2 == 0.0f) {
                            Sound.streamPlay(1195012, 48000);
                            ST2790.this.light07.disp(true);
                            ST2790.this.light08.disp(true);
                        }
                        if (f2 == 360.0f) {
                            Runtime.setFlags(8050, 1, 1);
                            ST2790.this.wa_move = 0;
                            ST2790.this.cam0.setMode(0);
                            Runtime.enable(524288);
                            ST2790.this.light07.disp(false);
                            ST2790.this.light07.clearEffect();
                            ST2790.this.light08.disp(false);
                            ST2790.this.light08.clearEffect();
                            if (!ST2790.this.jyama_down) {
                                ST2790.this.player.setID(2);
                            } else {
                                ST2790.this.player.setID(3);
                            }
                            Runtime.setPlayerControl(true);
                        }
                        ST2790.this.wa.setTranslate(ST2790.this.wa.px, -Math.sin((f2 + 360.0f) / 2.0f * 3.14f / 180.0f) * 15.0f - 15.0f, ST2790.this.wa.pz);
                        f2 += 1.0f;
                    }
                    if (Runtime.getFlags(8050, 1) == 1) {
                        if (f2 == 360.0f) {
                            ST2790.this.light03.disp(true);
                            ST2790.this.light04.disp(true);
                            Sound.streamPlay(1195012, 48000);
                        }
                        if (f2 == 0.0f) {
                            Runtime.setFlags(8050, 1, 0);
                            ST2790.this.wa_move = 0;
                            ST2790.this.light03.disp(false);
                            ST2790.this.light03.clearEffect();
                            ST2790.this.light04.disp(false);
                            ST2790.this.light04.clearEffect();
                            ST2790.this.cam0.setMode(0);
                            Runtime.enable(524288);
                            ST2790.this.player.setID(1);
                            Runtime.setPlayerControl(true);
                        }
                        ST2790.this.wa.setTranslate(ST2790.this.wa.px, -Math.sin((f2 + 360.0f) / 2.0f * 3.14f / 180.0f) * 15.0f - 15.0f, ST2790.this.wa.pz);
                        f2 -= 1.0f;
                    }
                }
                if ((f += 1.0f) == 360.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

