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
import xeno.map.MC_GNU09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1490
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU09_PRJ {
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
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono Sub;
    Uwamono safety01;
    Uwamono safety02;
    Uwamono safety03;
    Uwamono safety04;
    Uwamono safety05;
    Uwamono safety06;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Uwamono item7;
    Uwamono item8;
    MAPUnit hasigo;
    MAPUnit ele;
    Unit monitor1;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect mokumoku;
    Effect s1;
    Effect s2;
    Effect s3;
    Effect s4;
    Effect s5;
    Effect s6;
    Effect s1e;
    Effect s2e;
    Effect s3e;
    Effect s4e;
    Effect s5e;
    Effect s6e;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    int moni = 0;
    boolean lo2 = true;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Effect fade;
    int page;
    String[] inf1 = new String[]{"There is something written on the elevator. Read it?\n", "/[waitkey(64)]/[close()]"};
    String[] inf2 = new String[]{"/[label(Information)]", "This elevator is currently in emergency shut-off mode.\n", "/[waitkey(1)]/[clear()]", "Please confirm the safety of the surrounding area and disarm the safety devices located on each floor if you wish to reactivate it./[waitkey(64)]/[close()]"};
    String[] hasigo1 = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] mj = new String[]{"Go to the lower level?", "/[waitkey(64)]/[close()]"};

    ST1490() {
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
                Runtime.setPlayerControl(false);
                this.moni = 2;
                this.lo = 1;
                System.println("情報");
                this.information();
                Runtime.setPlayerControl(true);
                this.lo = 0;
                this.moni = 0;
                break;
            }
            case 1: {
                this.player.getTranslate();
                if (this.player.py > 7.0f || this.player.py < 5.0f) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("梯子下げ");
                this.hasigo();
                Runtime.setPlayerControl(true);
                this.lo = 0;
                break;
            }
            case 2: {
                if (!this.lo2) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("マップジャンプ");
                this.ele2();
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 3: {
                System.println("lo2 = true");
                this.lo2 = true;
                break;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(8007, 1, 1);
                System.println("1");
                this.s1.disp(false);
                this.s1e.disp(true);
                this.cam1F();
                this.hantei();
                break;
            }
            case 2: {
                Runtime.setFlags(8008, 1, 1);
                System.println("2");
                this.s4.disp(false);
                this.s4e.disp(true);
                this.cam1F();
                this.hantei();
                break;
            }
            case 3: {
                Runtime.setFlags(8009, 1, 1);
                System.println("3");
                this.s2.disp(false);
                this.s2e.disp(true);
                this.cam2F();
                this.hantei();
                break;
            }
            case 4: {
                Runtime.setFlags(8010, 1, 1);
                System.println("4");
                this.s5.disp(false);
                this.s5e.disp(true);
                this.cam2F();
                this.hantei();
                break;
            }
            case 5: {
                Runtime.setFlags(8011, 1, 1);
                System.println("5");
                this.s3.disp(false);
                this.s3e.disp(true);
                this.cam3F();
                this.hantei();
                break;
            }
            case 6: {
                Runtime.setFlags(8012, 1, 1);
                System.println("6");
                this.s6.disp(false);
                this.s6e.disp(true);
                this.cam3F();
                this.hantei();
                break;
            }
            case 7: {
                Runtime.setFlags(8109, 1, 1);
                System.println("7");
                break;
            }
        }
    }

    void cam1F() {
        Runtime.setPlayerControl(false);
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.camEV = Camera.create(1);
        this.camEV.setRotate(-15.2f, 0.0f, 0.0f);
        this.camEV.setTranslate(0.0f, 3.4f, 0.75f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
        System.sleep(10);
        Runtime.setPlayerControl(false);
        System.sleep(50);
        Runtime.enable(524288);
        Runtime.setPlayerControl(true);
    }

    void cam2F() {
        Runtime.setPlayerControl(false);
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.camEV = Camera.create(1);
        this.camEV.setRotate(-15.2f, 0.0f, 0.0f);
        this.camEV.setTranslate(0.0f, 9.4f, 0.75f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
        System.sleep(10);
        Runtime.setPlayerControl(false);
        System.sleep(50);
        Runtime.enable(524288);
        Runtime.setPlayerControl(true);
    }

    void cam3F() {
        Runtime.setPlayerControl(false);
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.camEV = Camera.create(1);
        this.camEV.setRotate(-15.2f, 0.0f, 0.0f);
        this.camEV.setTranslate(0.0f, 15.4f, 0.75f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
        System.sleep(10);
        Runtime.setPlayerControl(false);
        System.sleep(50);
        Runtime.enable(524288);
        Runtime.setPlayerControl(true);
    }

    void ele() {
        Runtime.setPlayerControl(false);
        Runtime.disable(524288);
        System.sleep(10);
        this.cam0.setMode(-1);
        this.camEV = Camera.create(1);
        this.ele.getTranslate();
        this.camEV.setTranslate(0.0f, 12.5f, 7.2f);
        this.camEV.setView(this.ele.px, this.ele.py + 2.0f, this.ele.pz);
        this.camEV.setFov(40.0f);
        this.camEV.change();
        System.sleep(20);
        this.light03.disp(true);
        System.sleep(30);
        float f = 0.0f;
        float f2 = 0.0f;
        Sound.effectPlay(196745);
        while (f < 180.0f) {
            f2 = Math.sin(f * 3.14f / 180.0f) * 8.0f + 7.0f;
            this.light03.setTranslate(0.0f, 2.9f + f2, -7.8f);
            this.ele.setTranslate(0.0f, f2, -7.8f);
            this.ele.getTranslate();
            this.camEV.setView(this.ele.px, this.ele.py + 2.0f, this.ele.pz);
            f += 1.0f;
            System.sleep(1);
        }
        this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196616);
        System.sleep(20);
        this.camEV.setRotate(0.8f, 0.7f, 0.0f);
        this.camEV.setTranslate(0.0f, 1.5f, 1.8f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
        System.sleep(20);
        Sound.effectPlay(196709);
        this.doorB.DoorOpen();
        System.sleep(30);
        Runtime.enable(524288);
        Runtime.setPlayerControl(true);
        this.cam0.setMode(0);
    }

    void ele2() {
        this.nwin(this.mj);
        this.yesno();
        switch (this.selected) {
            case 0: {
                float f = 0.0f;
                float f2 = 0.0f;
                this.doorB.SetDoorType('\u0002');
                this.doorB.DoorClose();
                System.sleep(30);
                this.player.getTranslate();
                Runtime.enable(65536);
                this.cam0.setMode(-1);
                Runtime.disable(524288);
                this.player.rotY(10, 0.0f, true);
                System.sleep(10);
                Sound.effectPlay(196744);
                while (true) {
                    f2 = Math.sin(f * 3.14f / 180.0f) * 4.0f - 5.0f;
                    this.player.setTranslate(this.player.px, f2 + 1.0f, this.player.pz);
                    this.ele.setTranslate(0.0f, f2, -7.8f);
                    this.light03.setTranslate(0.0f, f2 + 2.9f, -7.8f);
                    System.sleep(1);
                    if ((f += 1.0f) == 110.0f) {
                        this.fade.call(0);
                    }
                    if (f != 140.0f) continue;
                    Runtime.enable(524288);
                    Runtime.disable(65536);
                    this.cam0.setMode(0);
                    Runtime.setFlags(8017, 1, 0);
                    Runtime.jumpCF(1510, 1);
                }
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
                Runtime.setFlags(8006, 1, 1);
                Runtime.jumpCF(1500, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1500, 3);
                break;
            }
            case 2: {
                Runtime.setFlags(6040, 1, 1);
                Runtime.jumpCF(1500, 4);
                break;
            }
            case 3: {
                Runtime.jumpCF(1500, 5);
                break;
            }
            case 4: {
                Runtime.jumpCF(1480, 1);
                break;
            }
        }
    }

    void hantei() {
        if (Runtime.getFlags(8007, 1) == 1) {
            if (Runtime.getFlags(8008, 1) == 1) {
                if (Runtime.getFlags(8009, 1) == 1) {
                    if (Runtime.getFlags(8010, 1) == 1) {
                        if (Runtime.getFlags(8011, 1) == 1) {
                            if (Runtime.getFlags(8012, 1) == 1) {
                                System.println("全部壊れた");
                                this.ele();
                                Runtime.setFlags(8014, 1, 1);
                            } else {
                                this.cam0.setMode(0);
                            }
                        } else {
                            this.cam0.setMode(0);
                        }
                    } else {
                        this.cam0.setMode(0);
                    }
                } else {
                    this.cam0.setMode(0);
                }
            } else {
                this.cam0.setMode(0);
            }
        } else {
            this.cam0.setMode(0);
        }
    }

    void hasigo() {
        if (Runtime.getFlags(8015, 1) == 0) {
            this.nwin(this.hasigo1);
            this.yesno();
            switch (this.selected) {
                case 0: {
                    System.println("梯子移動");
                    Runtime.setFlags(8015, 1, 1);
                    this.cam0.setMode(-1);
                    this.camEV = Camera.create(1);
                    this.camEV.setTranslate(-13.6f, 10.4f, 4.2f);
                    this.camEV.setRotate(-9.1f, -37.3f, 0.0f);
                    this.camEV.setFov(43.3f);
                    this.camEV.change();
                    Runtime.disable(524288);
                    Runtime.enable(65536);
                    this.player.setTranslate(7.3f, 6.0f, -7.85f);
                    this.player.rotY(10, 180.0f, true);
                    System.sleep(10);
                    this.player.mtn(25, 1, 1.0f, true);
                    System.sleep(30);
                    Sound.effectPlay(58);
                    System.sleep(10);
                    this.mokumoku.disp(true);
                    Sound.effectPlay(196747);
                    this.hasigo.getTranslate();
                    float f = 0.0f;
                    while (f < 80.0f) {
                        this.hasigo.setTranslate(this.hasigo.px, this.hasigo.py - 0.04f, this.hasigo.pz);
                        f += 1.0f;
                        System.sleep(1);
                    }
                    System.sleep(20);
                    Runtime.disable(65536);
                    Runtime.enable(524288);
                    this.light01.disp(false);
                    this.player.setID(1);
                    this.cam0.setMode(0);
                }
            }
            return;
        }
    }

    void information() {
        Object var1_1 = null;
        if (Runtime.getFlags(8014, 1) == 0) {
            this.nwin(this.inf2);
            return;
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.getFlags(8014, 1) == 1) {
            this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, -7.0f, 0.0f);
            this.teiten1.SetBgm(196616);
        }
        this.teiten2 = new Uwamono(28690, 8.5f, 0.0f, -1.5f, 0.0f);
        this.teiten2.SetBgm(196617);
        Stage.setColor(1.2f, 1.2f, 1.2f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.player.setID(2);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 9.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16391, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy1.kickEnepc(4, 1);
        this.enemy1.setVisible(false);
        this.enemy1.kickEnepc(19, 1, 0, 640, 1);
        this.light01 = new Effect(1011, 7.5f, 7.6f, -8.75f, 0.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1443, 8.5f, 1.7f, -1.45f, 0.0f);
        this.light02.noAttach(false);
        this.light02.setScale(1.0f, 2.3f, 1.0f);
        this.light02.setRotate(-18.0f, 0.0f, 0.0f);
        this.light03 = new Effect(1498, 0.0f, 17.9f, -7.8f, 0.0f);
        this.light03.noAttach(false);
        this.light03.disp(false);
        this.s1 = new Effect(1673, -0.78f, 1.4f, -8.0f, 0.0f);
        this.s1e = new Effect(1674, -0.78f, 1.4f, -8.0f, 0.0f);
        this.s1e.disp(false);
        this.s2 = new Effect(1673, -0.78f, 7.4f, -8.0f, 0.0f);
        this.s2e = new Effect(1674, -0.78f, 7.4f, -8.0f, 0.0f);
        this.s2e.disp(false);
        this.s3 = new Effect(1673, -0.78f, 13.4f, -8.0f, 0.0f);
        this.s3e = new Effect(1674, -0.78f, 13.4f, -8.0f, 0.0f);
        this.s3e.disp(false);
        this.s4 = new Effect(1673, 0.78f, 1.4f, -8.0f, 0.0f);
        this.s4.setRotate(0.0f, 180.0f, 0.0f);
        this.s4e = new Effect(1674, 0.78f, 1.4f, -8.0f, 0.0f);
        this.s4e.setRotate(0.0f, 180.0f, 0.0f);
        this.s4e.disp(false);
        this.s5 = new Effect(1673, 0.78f, 7.4f, -8.0f, 0.0f);
        this.s5.setRotate(0.0f, 180.0f, 0.0f);
        this.s5e = new Effect(1674, 0.78f, 7.4f, -8.0f, 0.0f);
        this.s5e.setRotate(0.0f, 180.0f, 0.0f);
        this.s5e.disp(false);
        this.s6 = new Effect(1673, 0.78f, 13.4f, -8.0f, 0.0f);
        this.s6.setRotate(0.0f, 180.0f, 0.0f);
        this.s6e = new Effect(1674, 0.78f, 13.4f, -8.0f, 0.0f);
        this.s6e.setRotate(0.0f, 180.0f, 0.0f);
        this.s6e.disp(false);
        this.mokumoku = new Effect(1490, -9.7f, 6.0f, -7.9f, 0.0f);
        this.mokumoku.setScale(0.5f, 1.0f, 1.0f);
        this.mokumoku.disp(false);
        this.hasigo = new Mapunits();
        this.hasigo.mapUnit(113);
        this.hasigo.start(4, null);
        this.hasigo.setRotate(0.0f, 0.0f, 90.0f);
        if (Runtime.getFlags(8015, 1) == 1) {
            this.hasigo.setTranslate(this.hasigo.px, this.hasigo.py - 3.2f, this.hasigo.pz);
            this.player.setID(1);
            this.light01.disp(false);
        }
        this.ele = new Mapunits();
        this.ele.mapUnit(90);
        this.ele.start(4, null);
        this.ele.setTranslate(0.0f, 15.0f, -7.8f);
        if (Runtime.getFlags(8014, 1) == 1) {
            this.light03.disp(true);
            if (Runtime.getFlags(8018, 1) == 0) {
                this.ele.setTranslate(0.0f, -1.0f, -7.8f);
                this.light03.setTranslate(0.0f, 1.9000001f, -7.8f);
            } else {
                this.player.setTranslate(0.0f, Math.sin(2.4422224f) * 4.0f - 5.0f + 1.0f, -7.8f);
                this.ele.setTranslate(0.0f, Math.sin(2.4422224f) * 4.0f - 5.0f, -7.8f);
                this.light03.setTranslate(0.0f, Math.sin(2.4422224f) * 4.0f - 5.0f + 2.9f, -7.8f);
            }
        }
        if (Runtime.getFlags(8007, 1) == 0) {
            this.safety01 = new Uwamono(8, 1);
            this.safety01.SetCallNo(1);
        } else {
            this.s1.disp(false);
            Stage.setVisible(8, false);
        }
        if (Runtime.getFlags(8008, 1) == 0) {
            this.safety02 = new Uwamono(9, 1);
            this.safety02.SetCallNo(2);
        } else {
            this.s4.disp(false);
            Stage.setVisible(9, false);
        }
        if (Runtime.getFlags(8009, 1) == 0) {
            this.safety03 = new Uwamono(10, 1);
            this.safety03.SetCallNo(3);
        } else {
            this.s2.disp(false);
            Stage.setVisible(10, false);
        }
        if (Runtime.getFlags(8010, 1) == 0) {
            this.safety04 = new Uwamono(11, 1);
            this.safety04.SetCallNo(4);
        } else {
            this.s5.disp(false);
            Stage.setVisible(11, false);
        }
        if (Runtime.getFlags(8011, 1) == 0) {
            this.safety05 = new Uwamono(12, 1);
            this.safety05.SetCallNo(5);
        } else {
            this.s3.disp(false);
            Stage.setVisible(12, false);
        }
        if (Runtime.getFlags(8012, 1) == 0) {
            this.safety06 = new Uwamono(13, 1);
            this.safety06.SetCallNo(6);
        } else {
            this.s6.disp(false);
            Stage.setVisible(13, false);
        }
        if (Runtime.getFlags(8109, 1) == 0) {
            this.Sub = new Uwamono(14, 31);
            this.Sub.SetCallNo(7);
        } else {
            Stage.setVisible(14, false);
        }
        new Uwamono(15, 30);
        new Uwamono(28678, -7.6f, 0.0f, 0.6f);
        this.doorA = new Uwamono(0, 42, '\u0001');
        new Uwamono(1, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(2, 40, '\u0001');
        new Uwamono(3, 40, '\u0001', this.doorB);
        this.doorB.SetDoorRange(0.7f);
        this.doorB.SetDoorType('\u0002');
        if (Runtime.getFlags(8014, 1) == 1 && Runtime.getFlags(8018, 1) == 0) {
            this.doorB.DoorOpen();
        }
        this.doorC = new Uwamono(4, 40, '\u0001');
        this.doorD = new Uwamono(5, 40, '\u0001');
        this.doorE = new Uwamono(6, 40, '\u0001');
        this.doorF = new Uwamono(7, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        this.doorE.SetDoorType('\u0004');
        this.doorF.SetDoorType('\u0004');
        this.monitor1 = new Mapunits();
        this.monitor1.init(24613, 8.55f, 1.9f, -1.2f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 1.3f, 1.9f);
        this.monitor1.setArgs(1, 10017, 0, 128, 112);
        this.monitor1.setArgs(2, 70, 0, 0, -1);
        this.monitor1.setArgs(3, -0.6f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        this.monitor1.start(1, "tenmetu");
        if (Runtime.getFlags(8018, 1) == 1) {
            this.lo2 = false;
            this.cam0.setMode(-1);
            this.camEV = Camera.create(1);
            this.camEV.setTranslate(0.0f, 8.0f, 1.03f);
            this.camEV.setRotate(-37.6f, 0.0f, 0.0f);
            this.camEV.setFov(35.0f);
            this.camEV.change();
            Runtime.enable(65536);
            float f = Math.sin(2.4422224f) * 4.0f - 5.0f;
            this.player.setTranslate(0.0f, f + 1.0f, -7.8f);
            this.ele.setTranslate(0.0f, f, -7.8f);
            this.light03.setTranslate(0.0f, f + 2.9f, -7.8f);
            System.println("ele3_1");
            this.ele.start(1, "ele3");
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1490.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1490.waitPage(this.win, 64);
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL() {
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }

        public void talk() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void ele3() {
            System.println("ele3_2");
            float f = 140.0f;
            float f2 = 0.0f;
            ST1490.this.doorB.SetDoorType('\u0002');
            System.sleep(30);
            ST1490.this.light03.disp(true);
            Sound.effectPlay(196745);
            do {
                f2 = Math.sin(f * 3.14f / 180.0f) * 4.0f - 5.0f;
                ST1490.this.player.setTranslate(0.0f, f2 + 1.0f, -7.8f);
                ST1490.this.ele.setTranslate(0.0f, f2, -7.8f);
                ST1490.this.light03.setTranslate(0.0f, f2 + 2.9f, -7.8f);
                System.sleep(1);
            } while ((f -= 1.0f) != 0.0f);
            ST1490.this.doorB.DoorOpen();
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST1490.this.cam0.setMode(0);
            Runtime.setFlags(8018, 1, 0);
        }

        void tenmetu() {
            int n = 1;
            while (true) {
                if (Runtime.getFlags(8014, 1) == 1) {
                    ST1490.this.moni = 1;
                    ST1490.this.light02.disp(false);
                }
                if (ST1490.this.moni == 0) {
                    ST1490.this.monitor1.signal(n);
                    n = n == 1 ? 0 : 1;
                }
                if (ST1490.this.moni == 1) {
                    ST1490.this.monitor1.signal(0);
                }
                if (ST1490.this.moni == 2) {
                    ST1490.this.monitor1.signal(2);
                }
                System.sleep(30);
            }
        }
    }
}

