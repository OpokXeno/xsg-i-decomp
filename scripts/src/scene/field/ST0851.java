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
import xeno.map.MC_PRO05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0851
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO05_PRJ {
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
    Enepc enemy2;
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect moni01;
    Effect moni02;
    Effect moni03;
    Effect moni04;
    Effect moni05;
    Effect moni06;
    Effect moni07;
    Effect moni08;
    Effect red01;
    Effect red02;
    Effect red03;
    boolean red = true;
    Effect fade;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    MAPUnit monitor1_1;
    MAPUnit monitor1_2;
    MAPUnit monitor1_3;
    MAPUnit monitor2_1;
    MAPUnit monitor2_2;
    MAPUnit monitor2_3;
    MAPUnit monitor3_1;
    MAPUnit monitor3_2;
    MAPUnit monitor3_3;
    MAPUnit monitor4_1;
    MAPUnit monitor4_2;
    MAPUnit monitor4_3;
    MAPUnit monitor5_1;
    MAPUnit monitor5_2;
    MAPUnit monitor5_3;
    MAPUnit monitor6_1;
    MAPUnit monitor6_2;
    MAPUnit monitor6_3;
    MAPUnit monitor7_1;
    MAPUnit monitor7_2;
    MAPUnit monitor7_3;
    MAPUnit monitor8_1;
    MAPUnit monitor8_2;
    MAPUnit monitor8_3;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] kagi = new String[]{"Operate terminal?", "/[waitkey(64)]/[close()]"};
    String[] kagi3 = new String[]{"Cannot be operated.", "/[waitkey(64)]/[close()]"};
    String[] kagi2 = new String[]{"It won't open. It seems to be locked.\n", "/[waitkey(64)]/[close()]"};
    String[] heisi = new String[]{"/[label(Soldier)]", "I think the intruders went that way! Find them!", "/[waitkey(64)]/[close()]"};
    String[] tanmatu = new String[]{"This must be the monitoring room. I can see every detail of the base's layout.\n", "/[waitkey(64)]/[close()]"};

    ST0851() {
    }

    void EOB(int n) {
        this.player.getTranslate();
        if (Runtime.getFlags(107, 1) == 1) {
            Sound.effectPlay(196748);
        }
        System.println("EOB****************************************************");
        if (n == 2) {
            Runtime.setFlags(8035, 1, 1);
        }
    }

    void EOB_Escape(int n) {
        this.player.getTranslate();
        if (Runtime.getFlags(107, 1) == 1) {
            Sound.effectPlay(196748);
        }
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        switch (n) {
            case 0: {
                System.println("0");
                if (Runtime.getFlags(109, 1) == 0) {
                    System.println("イベントSCE02005：挟み撃ちだ！　げへへ");
                    Runtime.setPlayerControl(false);
                    Sound.effectStop(196748);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(109, 1, 1);
                    Runtime.jumpEvent(2050);
                    break;
                }
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(860, 1);
                break;
            }
            default: {
                System.println("d");
            }
        }
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
                if (Runtime.getFlags(110, 1) == 0) {
                    if (Runtime.getFlags(8002, 1) == 0) {
                        this.enemy2.kickEnepc(4, 2);
                        System.sleep(1);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.kagi, 0);
                        this.lo = 1;
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.cam0.setMode(-1);
                                this.camEV = Camera.create(1);
                                this.camEV.setTranslate(0.14f, 4.21f, 10.18f);
                                this.camEV.setRotate(-35.16f, 58.18f, 0.0f);
                                this.camEV.setFov(40.0f);
                                this.camEV.change();
                                System.sleep(30);
                                this.light01.disp(false);
                                this.light02.disp(false);
                                this.light03.disp(true);
                                this.doorC.SetDoorType('\u0004');
                                Runtime.setFlags(8002, 1, 1);
                                System.sleep(20);
                                this.cam0.setMode(0);
                                Runtime.setPlayerControl(true);
                                this.lo = 0;
                                this.enemy2.kickEnepc(4, 0);
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        this.enemy2.kickEnepc(4, 0);
                        return;
                    }
                } else {
                    this.enemy2.kickEnepc(4, 2);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    this.nwin(this.kagi3);
                    this.lo = 0;
                    Runtime.setPlayerControl(true);
                    this.enemy2.kickEnepc(4, 0);
                }
                return;
            }
            case 1: {
                if (Runtime.getFlags(8002, 1) == 0 || Runtime.getFlags(110, 1) == 1) {
                    this.enemy2.kickEnepc(4, 2);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("鍵がかかってる");
                    this.nwin(this.kagi2);
                    this.lo = 0;
                    Runtime.setPlayerControl(true);
                    this.enemy2.kickEnepc(4, 0);
                }
            }
            case 2: {
                return;
            }
            case 6: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("あそこだーーー！");
                if (Runtime.getFlags(108, 1) == 0) {
                    this.enemy2.kickEnepc(4, 2);
                    System.sleep(1);
                    this.red = false;
                    this.doorB.DoorOpen();
                    Runtime.enable(65536);
                    this.player.rotY(10, 90.0f, true);
                    System.sleep(10);
                    this.nwin(this.heisi);
                    this.cam0.setMode(-1);
                    this.player.move(30, 5.0f, 2.5f, true);
                    this.player.mtn(2, 1, 1.0f, true);
                    System.sleep(17);
                    Runtime.disable(65536);
                    Runtime.setFlags(108, 1, 1);
                    System.println("イベント2004D:敵に追われて…　名前がジギー");
                    this.red01.disp(false);
                    Sound.effectStop(196748);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.jumpEvent(2043);
                    Runtime.setPlayerControl(true);
                    this.enemy2.kickEnepc(4, 0);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 7: {
                this.enemy2.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("端末");
                this.nwin(this.tanmatu);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                this.enemy2.kickEnepc(4, 0);
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
                Runtime.jumpCF(840, 2);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 5.876f, 0.5375f, -6.078f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, 7.845f, 0.5375f, -6.078f, 0.0f);
        this.teiten2.SetBgm(196621);
        this.teiten3 = new Uwamono(28690, 10.0687f, 0.5375f, -4.715f, 0.0f);
        this.teiten3.SetBgm(196621);
        this.teiten4 = new Uwamono(28690, 14.173f, 0.0f, -0.738f, 0.0f);
        this.teiten4.SetBgm(196622);
        this.teiten5 = new Uwamono(28690, 14.173f, 0.0f, 1.827f, 0.0f);
        this.teiten5.SetBgm(196622);
        this.teiten6 = new Uwamono(28690, 14.173f, 0.0f, 4.392f, 0.0f);
        this.teiten6.SetBgm(196622);
        this.teiten7 = new Uwamono(28690, -10.0f, 0.0f, 7.75f, 0.0f);
        this.teiten7.SetBgm(196623);
        Stage.setColor(1.35f, 1.35f, 1.35f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(1, 1, 0.15f, 1.0f, 0.1f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        this.monitor1_1 = new Mapunits();
        this.monitor1_1.mapUnit(87);
        this.monitor1_1.start(4, null);
        this.monitor1_2 = new Mapunits();
        this.monitor1_2.mapUnit(95);
        this.monitor1_2.start(4, null);
        this.monitor1_2.setVisible(false);
        this.monitor1_3 = new Mapunits();
        this.monitor1_3.mapUnit(103);
        this.monitor1_3.start(4, null);
        this.monitor1_3.setVisible(false);
        this.monitor2_1 = new Mapunits();
        this.monitor2_1.mapUnit(88);
        this.monitor2_1.start(4, null);
        this.monitor2_2 = new Mapunits();
        this.monitor2_2.mapUnit(96);
        this.monitor2_2.start(4, null);
        this.monitor2_2.setVisible(false);
        this.monitor2_3 = new Mapunits();
        this.monitor2_3.mapUnit(104);
        this.monitor2_3.start(4, null);
        this.monitor2_3.setVisible(false);
        this.monitor3_1 = new Mapunits();
        this.monitor3_1.mapUnit(89);
        this.monitor3_1.start(4, null);
        this.monitor3_2 = new Mapunits();
        this.monitor3_2.mapUnit(97);
        this.monitor3_2.start(4, null);
        this.monitor3_2.setVisible(false);
        this.monitor3_3 = new Mapunits();
        this.monitor3_3.mapUnit(105);
        this.monitor3_3.start(4, null);
        this.monitor3_3.setVisible(false);
        this.monitor4_1 = new Mapunits();
        this.monitor4_1.mapUnit(91);
        this.monitor4_1.start(4, null);
        this.monitor4_2 = new Mapunits();
        this.monitor4_2.mapUnit(99);
        this.monitor4_2.start(4, null);
        this.monitor4_2.setVisible(false);
        this.monitor4_3 = new Mapunits();
        this.monitor4_3.mapUnit(107);
        this.monitor4_3.start(4, null);
        this.monitor4_3.setVisible(false);
        this.monitor5_1 = new Mapunits();
        this.monitor5_1.mapUnit(84);
        this.monitor5_1.start(4, null);
        this.monitor5_2 = new Mapunits();
        this.monitor5_2.mapUnit(92);
        this.monitor5_2.start(4, null);
        this.monitor5_2.setVisible(false);
        this.monitor5_3 = new Mapunits();
        this.monitor5_3.mapUnit(100);
        this.monitor5_3.start(4, null);
        this.monitor5_3.setVisible(false);
        this.monitor6_1 = new Mapunits();
        this.monitor6_1.mapUnit(85);
        this.monitor6_1.start(4, null);
        this.monitor6_2 = new Mapunits();
        this.monitor6_2.mapUnit(93);
        this.monitor6_2.start(4, null);
        this.monitor6_2.setVisible(false);
        this.monitor6_3 = new Mapunits();
        this.monitor6_3.mapUnit(101);
        this.monitor6_3.start(4, null);
        this.monitor6_3.setVisible(false);
        this.monitor7_1 = new Mapunits();
        this.monitor7_1.mapUnit(86);
        this.monitor7_1.start(4, null);
        this.monitor7_2 = new Mapunits();
        this.monitor7_2.mapUnit(94);
        this.monitor7_2.start(4, null);
        this.monitor7_2.setVisible(false);
        this.monitor7_3 = new Mapunits();
        this.monitor7_3.mapUnit(102);
        this.monitor7_3.start(4, null);
        this.monitor7_3.setVisible(false);
        this.monitor8_1 = new Mapunits();
        this.monitor8_1.mapUnit(90);
        this.monitor8_1.start(4, null);
        this.monitor8_2 = new Mapunits();
        this.monitor8_2.mapUnit(98);
        this.monitor8_2.start(4, null);
        this.monitor8_2.setVisible(false);
        this.monitor8_3 = new Mapunits();
        this.monitor8_3.mapUnit(106);
        this.monitor8_3.start(4, null);
        this.monitor8_3.setVisible(false);
        this.monitor8_3.start(1, "idle");
        this.red01 = new Effect(1551, 0.0f, 0.0f, 0.0f, 0.0f);
        this.red02 = new Effect(1716, 3.5f, 3.3f, -5.19f, 0.0f);
        this.red02.noAttach(false);
        this.red02.setRotate(0.0f, 90.0f, 0.0f);
        this.red02.setScale(2.2f, 2.2f, 2.2f);
        this.red02.disp(false);
        this.moni01 = new Effect(1570, 5.5f, 2.5f, -7.0f, 0.0f);
        this.moni02 = new Effect(1570, 6.7f, 2.5f, -7.0f, 0.0f);
        this.moni03 = new Effect(1570, 8.0f, 2.5f, -7.0f, 0.0f);
        this.moni04 = new Effect(1570, 9.1f, 2.5f, -7.0f, 0.0f);
        this.moni05 = new Effect(1570, 5.5f, 3.2f, -6.8f, 0.0f);
        this.moni06 = new Effect(1570, 6.7f, 3.2f, -6.8f, 0.0f);
        this.moni07 = new Effect(1570, 8.0f, 3.2f, -6.8f, 0.0f);
        this.moni08 = new Effect(1570, 9.1f, 3.2f, -6.8f, 0.0f);
        this.light01 = new Effect(1020, 10.63f, 1.5f, -4.25f, 0.0f);
        this.light01.setRotate(90.0f, 30.0f, 0.0f);
        this.light01.setScale(3.0f, 3.0f, 1.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1020, -3.16f, 1.43f, 7.35f, 0.0f);
        this.light02.setRotate(-90.0f, 75.0f, 0.0f);
        this.light02.setScale(1.5f, 1.0f, 1.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1055, -3.14f, 1.35f, 7.35f, 0.0f);
        this.light03.setRotate(-90.0f, 75.0f, 0.0f);
        this.light03.setScale(1.5f, 1.0f, 1.0f);
        this.light03.disp(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(71, false);
        Stage.setVisible(14, false);
        Stage.setVisible(70, false);
        Stage.setVisible(69, false);
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 25.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        if (Runtime.getFlags(8035, 1) == 0) {
            float[] fArray = new float[24];
            fArray[0] = -5.4f;
            fArray[2] = -5.6f;
            fArray[3] = -1.0f;
            fArray[4] = -0.3f;
            fArray[6] = -5.6f;
            fArray[8] = -5.4f;
            fArray[10] = -1.8f;
            fArray[12] = -12.6f;
            fArray[14] = -1.8f;
            fArray[15] = 2.0f;
            fArray[16] = -5.4f;
            fArray[18] = -8.2f;
            fArray[20] = -12.7f;
            fArray[22] = -8.2f;
            fArray[23] = 4.0f;
            float[] fArray2 = fArray;
            this.enemy2 = new NpcEnemy(16643, 2, 0, 13, 3, 0.0f, 0.0f, 0.0f, 90.0f, fArray2);
            this.enemy2.setGroup(1, 1, 1, 1);
            new Uwamono(38, 4, this.enemy2);
        } else {
            this.enemy2 = new NpcEnemy(16643, 2, 0, 13, 5, 100.0f, 0.0f, 100.0f, 90.0f);
            this.enemy2.kickEnepc(4, 2);
            this.enemy2.setVisible(false);
            this.enemy2.setInvalidID(1);
            new Uwamono(38, 4);
        }
        this.npc1 = new NpcEnemy(16643, 2, 0, 0, 3, 0.0f, 0.0f, -15.0f, 0.0f);
        this.npc1.setInvalidID(1);
        this.npc1.dispRadar(false);
        this.npc1.setVisible(false);
        this.npc1.kickEnepc(19, 1, 0, 560, 1);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 57);
        this.item3 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 58);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 59);
        this.item5 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 60);
        this.item6 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 62);
        new Uwamono(43, 4, this.item2);
        new Uwamono(42, 4, this.item3);
        new Uwamono(41, 4, this.item4);
        new Uwamono(39, 4, this.item5);
        new Uwamono(40, 4, this.item6);
        new Uwamono(28673, -2.4f, 0.0f, -8.8f, 0.0f);
        this.doorA = new Uwamono(75, 42, '\u0001');
        new Uwamono(76, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(72, 42, '\u0001');
        new Uwamono(73, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(74, 40, '\u0001');
        if (Runtime.getFlags(8002, 1) == 0 || Runtime.getFlags(110, 1) == 1) {
            this.doorC.SetDoorType('\u0002');
        } else {
            this.light01.disp(false);
            this.light02.disp(false);
            this.light03.disp(true);
            this.doorC.SetDoorType('\u0004');
        }
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(108, 1) == 0) {
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB.SetDoorType('\u0004');
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0851.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0851.waitPage(this.win, 64);
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
            int n = 0;
            boolean bl = false;
            int n2 = 1;
            int n3 = 1;
            int n4 = 1;
            int n5 = 1;
            int n6 = 1;
            int n7 = 1;
            int n8 = 1;
            int n9 = 1;
            int n10 = 0;
            boolean bl2 = false;
            Sound.effectPlay(196748);
            while (true) {
                ST0851.this.player.getTranslate();
                if (ST0851.this.red) {
                    if (ST0851.this.player.px > 3.5f) {
                        if (!bl2) {
                            ST0851.this.red02.disp(true);
                            ST0851.this.red01.disp(false);
                            Sound.effectPlay(196748, 70, 10);
                        }
                        bl2 = true;
                    } else {
                        if (bl2) {
                            ST0851.this.red02.disp(false);
                            ST0851.this.red01.disp(true);
                            Sound.effectPlay(196748);
                        }
                        bl2 = false;
                    }
                }
                if (n10 % 10 == 0) {
                    n = Math.random();
                    n %= 8;
                    if (++n == 1) {
                        if (n2 == 1) {
                            ST0851.this.monitor1_1.setVisible(true);
                            ST0851.this.monitor1_2.setVisible(false);
                            ST0851.this.monitor1_3.setVisible(false);
                        }
                        if (n2 == 2) {
                            ST0851.this.monitor1_1.setVisible(false);
                            ST0851.this.monitor1_2.setVisible(true);
                            ST0851.this.monitor1_3.setVisible(false);
                        }
                        if (n2 == 3) {
                            ST0851.this.monitor1_1.setVisible(false);
                            ST0851.this.monitor1_2.setVisible(false);
                            ST0851.this.monitor1_3.setVisible(true);
                            n2 = 0;
                        }
                        ++n2;
                    }
                    if (n == 2) {
                        if (n3 == 1) {
                            ST0851.this.monitor2_1.setVisible(true);
                            ST0851.this.monitor2_2.setVisible(false);
                            ST0851.this.monitor2_3.setVisible(false);
                        }
                        if (n3 == 2) {
                            ST0851.this.monitor2_1.setVisible(false);
                            ST0851.this.monitor2_2.setVisible(true);
                            ST0851.this.monitor2_3.setVisible(false);
                        }
                        if (n3 == 3) {
                            ST0851.this.monitor2_1.setVisible(false);
                            ST0851.this.monitor2_2.setVisible(false);
                            ST0851.this.monitor2_3.setVisible(true);
                            n3 = 0;
                        }
                        ++n3;
                    }
                    if (n == 3) {
                        if (n4 == 1) {
                            ST0851.this.monitor3_1.setVisible(true);
                            ST0851.this.monitor3_2.setVisible(false);
                            ST0851.this.monitor3_3.setVisible(false);
                        }
                        if (n4 == 2) {
                            ST0851.this.monitor3_1.setVisible(false);
                            ST0851.this.monitor3_2.setVisible(true);
                            ST0851.this.monitor3_3.setVisible(false);
                        }
                        if (n4 == 3) {
                            ST0851.this.monitor3_1.setVisible(false);
                            ST0851.this.monitor3_2.setVisible(false);
                            ST0851.this.monitor3_3.setVisible(true);
                            n4 = 0;
                        }
                        ++n4;
                    }
                    if (n == 4) {
                        if (n5 == 1) {
                            ST0851.this.monitor4_1.setVisible(true);
                            ST0851.this.monitor4_2.setVisible(false);
                            ST0851.this.monitor4_3.setVisible(false);
                        }
                        if (n5 == 2) {
                            ST0851.this.monitor4_1.setVisible(false);
                            ST0851.this.monitor4_2.setVisible(true);
                            ST0851.this.monitor4_3.setVisible(false);
                        }
                        if (n5 == 3) {
                            ST0851.this.monitor4_1.setVisible(false);
                            ST0851.this.monitor4_2.setVisible(false);
                            ST0851.this.monitor4_3.setVisible(true);
                            n5 = 0;
                        }
                        ++n5;
                    }
                    if (n == 5) {
                        if (n6 == 1) {
                            ST0851.this.monitor5_1.setVisible(true);
                            ST0851.this.monitor5_2.setVisible(false);
                            ST0851.this.monitor5_3.setVisible(false);
                        }
                        if (n6 == 2) {
                            ST0851.this.monitor5_1.setVisible(false);
                            ST0851.this.monitor5_2.setVisible(true);
                            ST0851.this.monitor5_3.setVisible(false);
                        }
                        if (n6 == 3) {
                            ST0851.this.monitor5_1.setVisible(false);
                            ST0851.this.monitor5_2.setVisible(false);
                            ST0851.this.monitor5_3.setVisible(true);
                            n6 = 0;
                        }
                        ++n6;
                    }
                    if (n == 6) {
                        if (n7 == 1) {
                            ST0851.this.monitor6_1.setVisible(true);
                            ST0851.this.monitor6_2.setVisible(false);
                            ST0851.this.monitor6_3.setVisible(false);
                        }
                        if (n7 == 2) {
                            ST0851.this.monitor6_1.setVisible(false);
                            ST0851.this.monitor6_2.setVisible(true);
                            ST0851.this.monitor6_3.setVisible(false);
                        }
                        if (n7 == 3) {
                            ST0851.this.monitor6_1.setVisible(false);
                            ST0851.this.monitor6_2.setVisible(false);
                            ST0851.this.monitor6_3.setVisible(true);
                            n7 = 0;
                        }
                        ++n7;
                    }
                    if (n == 7) {
                        if (n8 == 1) {
                            ST0851.this.monitor7_1.setVisible(true);
                            ST0851.this.monitor7_2.setVisible(false);
                            ST0851.this.monitor7_3.setVisible(false);
                        }
                        if (n8 == 2) {
                            ST0851.this.monitor7_1.setVisible(false);
                            ST0851.this.monitor7_2.setVisible(true);
                            ST0851.this.monitor7_3.setVisible(false);
                        }
                        if (n8 == 3) {
                            ST0851.this.monitor7_1.setVisible(false);
                            ST0851.this.monitor7_2.setVisible(false);
                            ST0851.this.monitor7_3.setVisible(true);
                            n8 = 0;
                        }
                        ++n8;
                    }
                    if (n == 8) {
                        if (n9 == 1) {
                            ST0851.this.monitor8_1.setVisible(true);
                            ST0851.this.monitor8_2.setVisible(false);
                            ST0851.this.monitor8_3.setVisible(false);
                        }
                        if (n9 == 2) {
                            ST0851.this.monitor8_1.setVisible(false);
                            ST0851.this.monitor8_2.setVisible(true);
                            ST0851.this.monitor8_3.setVisible(false);
                        }
                        if (n9 == 3) {
                            ST0851.this.monitor8_1.setVisible(false);
                            ST0851.this.monitor8_2.setVisible(false);
                            ST0851.this.monitor8_3.setVisible(true);
                            n9 = 0;
                        }
                        ++n9;
                    }
                }
                if (++n10 == 10000) {
                    n10 = 0;
                }
                System.sleep(1);
            }
        }
    }
}

