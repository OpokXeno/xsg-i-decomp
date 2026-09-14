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

class ST3021
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
    Unit Dai;
    Unit Kabe01;
    Unit Kabe02;
    Effect E01;
    Effect fade;
    Light light = new Light(0);
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
    Uwamono col;
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

    ST3021() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 1) {
            System.println("1st1st1st1st1st1st1st1st1st1st1st1st1st1st1st1st1st");
            Runtime.setFlags(3144, 2, 1);
            Runtime.jumpCF(68557, 0);
        }
        if (n == 2) {
            System.println("2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd2nd");
            Runtime.setFlags(3144, 2, 2);
            Runtime.jumpCF(68557, 0);
        }
        if (n == 3) {
            System.println("3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd3rd");
            Runtime.setFlags(3144, 2, 3);
            Runtime.jumpCF(68557, 0);
        }
        if (n == 4) {
            System.println("4th4th4th4th4th4th4th4th4th4th4th4th4th4th4th4th4th");
            Runtime.setFlags(3149, 1, 1);
            Runtime.jumpCF(68556, 0);
        }
    }

    void EV_Camera01() {
        this.cam0.setTranslate(-12.403f, -27.686f, 37.578f);
        this.cam0.setRotate(-3.916f, 14.859f, 0.0f);
        this.cam0.setFov(40.0f);
    }

    void EV_Camera02() {
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

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 != 0 && n2 != 1 && n2 == 2) {
            switch (n) {
                default:
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
        this.Kabe01 = new Mapunits();
        this.Kabe01.mapUnit(27);
        this.Kabe01.start(4, null);
        this.Kabe01.start(1, "Down");
        this.Kabe02 = new Mapunits();
        this.Kabe02.mapUnit(15);
        this.Kabe02.start(4, null);
        this.Kabe02.start(1, "Cam");
        this.Dai = new Mapunits();
        this.Dai.mapUnit(16);
        this.Dai.start(4, null);
        this.Dai.setTranslate(this.Dai.px, this.Dai.py - 30.0f, this.Dai.pz + 30.0f);
        this.E01 = new Effect(1417, -13.741f, 1.023f, -4.322f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(true);
        this.E01.setTranslate(this.E01.px, this.E01.py - 30.0f, this.E01.pz + 30.0f);
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
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        if (Runtime.getFlags(3144, 2) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16405, 5, -14.0f, -29.0f, 30.0f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            this.enemy1.setParams(0, 3, 1, 5);
            this.enemy1.setTP(1);
            this.enemy1.setInvalidID(1);
            this.enemy1.setBatEvent(8);
            this.enemy1.dispRadar(false);
        } else if (Runtime.getFlags(3144, 2) == 1) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16406, 6, -14.0f, -29.0f, 30.0f, 0.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(1, 1, 1, 1);
            this.enemy2.setParams(0, 3, 2, 6);
            this.enemy2.setTP(1);
            this.enemy2.setInvalidID(1);
            this.enemy2.setBatEvent(8);
            this.enemy2.dispRadar(false);
        } else if (Runtime.getFlags(3144, 2) == 2) {
            this.enemy3 = new Enepc();
            this.enemy3.init(16404, 7, -14.0f, -29.0f, 30.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(2, 2, 2, 2);
            this.enemy3.setParams(0, 3, 3, 7);
            this.enemy3.setTP(1);
            this.enemy3.setInvalidID(1);
            this.enemy3.setBatEvent(8);
            this.enemy3.dispRadar(false);
        } else if (Runtime.getFlags(3144, 2) == 3) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16404, 7, -14.0f, -29.0f, 30.0f, 0.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(3, 3, 3, 3);
            this.enemy4.setParams(0, 3, 4, 7);
            this.enemy4.setTP(1);
            this.enemy4.setInvalidID(1);
            this.enemy4.setBatEvent(8);
            this.enemy4.dispRadar(false);
        }
        this.doorA = new Uwamono(21, 42, '\u0001');
        new Uwamono(22, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.player.setTranslate(-15.0f, -30.0f, 30.0f);
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

        void Cam() {
            ST3021.this.cam0.setMode(-1);
            ST3021.this.EV_Camera01();
        }

        void Down() {
            int n = 0;
            ST3021.this.Kabe01.getTranslate();
            ST3021.this.Kabe02.getTranslate();
            ST3021.this.player.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 450) {
                    ST3021.this.Kabe01.setTranslate(ST3021.this.Kabe01.px, ST3021.this.Kabe01.py + 0.08888889f, ST3021.this.Kabe01.pz - 0.08888889f);
                    ST3021.this.Kabe02.setTranslate(ST3021.this.Kabe01.px, ST3021.this.Kabe02.py + 0.08888889f, ST3021.this.Kabe02.pz - 0.08888889f);
                }
                if (n >= 225 && n < 450) {
                    if (Runtime.getFlags(3144, 2) == 0) {
                        ST3021.this.enemy1.setTP(1 + 4 * (n - 225));
                    } else if (Runtime.getFlags(3144, 2) == 1) {
                        ST3021.this.enemy2.setTP(1 + 4 * (n - 225));
                    } else if (Runtime.getFlags(3144, 2) == 2) {
                        ST3021.this.enemy3.setTP(1 + 4 * (n - 225));
                    } else if (Runtime.getFlags(3144, 2) == 3) {
                        ST3021.this.enemy4.setTP(1 + 4 * (n - 225));
                    }
                }
                if (n == 450) {
                    if (Runtime.getFlags(3144, 2) == 0) {
                        ST3021.this.enemy1.kickEnepc(14, 0);
                    } else if (Runtime.getFlags(3144, 2) == 1) {
                        ST3021.this.enemy2.kickEnepc(14, 0);
                    } else if (Runtime.getFlags(3144, 2) == 2) {
                        ST3021.this.enemy3.kickEnepc(14, 0);
                    } else if (Runtime.getFlags(3144, 2) == 3) {
                        ST3021.this.enemy4.kickEnepc(14, 0);
                    }
                }
                if (n == 900) break;
                ++n;
                System.sleep(1);
            }
        }
    }
}

