import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_DYU05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1751
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU05_PRJ {
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
    Enepc shion;
    Enepc chaos;
    Enepc ziggy;
    Enepc momo;
    Enepc jr;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc shitai_01;
    Enepc shitai_02;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
    int npc3talked = 0;
    int npc3btalked = 0;
    int npc4talked = 0;
    int npc4btalked = 0;
    int npc5talked = 0;
    int npc5btalked = 0;
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
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono BASE01;
    Uwamono BASE02;
    Uwamono BASE03;
    Effect fade;
    Effect fade1;
    Effect fade2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] SHI_00 = new String[]{"/[label(Shion)]", "Okay, let's go back to the Elsa and duplicate the combat records of the Woglinde and the Gnosis!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "Now, let's look for the equipment!!", "/[waitkey(1)]/[clear()]", "It should be somewhere in this area!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "Where could our equipment be?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_02A = new String[]{"/[label(Ziggy)]", "The equipment should be somewhere in this area.", "/[waitkey(64)]/[close()]"};
    String[] CHA_02A = new String[]{"/[label(chaos)]", "We have to get our equipment back first!!", "/[waitkey(64)]/[close()]"};
    String[] MOM_02A = new String[]{"/[label(MOMO)]", "We have to hurry and get our equipment back!!", "/[waitkey(64)]/[close()]"};
    String[] JUN_02A = new String[]{"/[label(Jr.)]", "Damn it! Where's the equipment?", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "Looks like our priority is to find our weapons.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "True. That would be the smartest thing to do.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_01 = new String[]{"/[label(Ziggy)]", "Is something the matter?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_02 = new String[]{"/[label(Ziggy)]", "It's okay. I didn't go all out.", "/[waitkey(64)]/[close()]"};
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "Do you think Lapis is okay?", "/[waitkey(64)]/[close()]"};
    String[] JR_00 = new String[]{"/[label(Jr.)]", "Wait a minute! Federation soldiers are wandering all over the place outside!", "/[waitkey(1)]/[clear()]", "There are at least two blocks to the dock area station. It's just too dangerous going unarmed!", "/[waitkey(64)]/[close()]"};

    ST1751() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 0.433f, 7.023f, 11.192f, 90.0f, 7.665f, 7.023f, 11.192f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -60.405f;
        fArray2[4] = 90.0f;
        fArray2[5] = -60.405f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 90);
        this.camEV.rotateSPL(fArray3, 1, 3, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -0.368f, 1.967f, 7.538f, 60.0f, -0.368f, 1.327f, 7.538f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -5.647f;
        fArray2[2] = -90.479f;
        fArray2[4] = 60.0f;
        fArray2[5] = -5.647f;
        fArray2[6] = -90.479f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 60);
        this.camEV.rotateSPL(fArray3, 1, 3, 60);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.368f, 1.327f, 7.538f);
        this.camEV.setRotate(-5.647f, -90.479f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.611f, 1.519f, 8.535f);
        this.camEV.setRotate(-13.067f, 33.6f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.659f, 1.839f, 6.84f);
        this.camEV.setRotate(-10.547f, -180.358f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.348f, 1.52f, 8.891f);
        this.camEV.setRotate(-1.121f, -71.319f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.324f, 2.287f, 8.25f);
        this.camEV.setRotate(-36.221f, 92.197f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.201f, 1.295f, 7.818f);
        this.camEV.setRotate(-0.897f, -79.158f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3076, 1) == 0) {
                        if (Runtime.getFlags(3077, 1) == 0) {
                            Runtime.setPlayerControl(false);
                            this.cam0.setMode(-1);
                            Runtime.disable(524288);
                            this.fade1.call(0);
                            System.sleep(60);
                            this.player.setTranslate(100.0f, 100.0f, 100.0f);
                            this.shion.setLocation(4, 0);
                            this.chaos.setLocation(4, 1);
                            this.ziggy.setLocation(4, 2);
                            this.momo.setLocation(4, 3);
                            this.jr.setLocation(4, 4);
                            this.shion.hairStop(0, 1);
                            this.chaos.hairStop(0, 1);
                            this.ziggy.hairStop(0, 1);
                            this.momo.hairStop(0, 1);
                            this.jr.hairStop(0, 1);
                            this.EV_Camera01();
                            this.fade2.call(0);
                            System.sleep(60);
                            this.shion.kickEnepc(4, 1);
                            this.shion.kickEnepc(9, 15);
                            this.shion.kickEnepc(1, 9);
                            System.sleep(60);
                            this.EV_Camera02();
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SHI_00, 0);
                            System.waitFor(this.win);
                            this.EV_Camera03();
                            this.jr.kickEnepc(4, 1);
                            this.jr.kickEnepc(0, 8);
                            System.sleep(40);
                            this.jr.kickEnepc(9, 11);
                            this.jr.kickEnepc(1, 9);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.JR_00, 0);
                            System.waitFor(this.win);
                            this.EV_Camera05();
                            this.chaos.kickEnepc(4, 1);
                            this.chaos.kickEnepc(9, 11);
                            this.chaos.kickEnepc(1, 9);
                            System.sleep(15);
                            this.shion.kickEnepc(9, 12);
                            this.shion.kickEnepc(1, 27);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.CHA_00, 0);
                            System.waitFor(this.win);
                            this.EV_Camera04();
                            this.chaos.kickEnepc(4, 1);
                            this.chaos.kickEnepc(1, 10);
                            this.ziggy.kickEnepc(4, 1);
                            this.ziggy.kickEnepc(0, 7);
                            System.sleep(40);
                            this.chaos.kickEnepc(9, 13);
                            this.ziggy.kickEnepc(1, 9);
                            this.shion.kickEnepc(9, 13);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.ZIG_00, 0);
                            System.waitFor(this.win);
                            this.EV_Camera06();
                            this.momo.kickEnepc(4, 1);
                            this.momo.kickEnepc(9, 13);
                            System.sleep(15);
                            this.ziggy.kickEnepc(9, 14);
                            this.ziggy.kickEnepc(1, 10);
                            System.sleep(30);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.ZIG_01, 0);
                            System.waitFor(this.win);
                            this.momo.kickEnepc(1, 9);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.MOM_00, 0);
                            System.waitFor(this.win);
                            this.ziggy.kickEnepc(0, 7);
                            System.sleep(40);
                            this.ziggy.kickEnepc(1, 9);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.ZIG_02, 0);
                            System.waitFor(this.win);
                            System.sleep(30);
                            this.shion.kickEnepc(9, 15);
                            this.chaos.kickEnepc(9, 11);
                            this.jr.kickEnepc(9, 11);
                            this.shion.kickEnepc(1, 9);
                            this.chaos.kickEnepc(1, 10);
                            this.ziggy.kickEnepc(1, 10);
                            this.momo.kickEnepc(1, 10);
                            this.jr.kickEnepc(1, 10);
                            this.EV_Camera07();
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SHI_01, 0);
                            System.waitFor(this.win);
                            this.fade1.call(0);
                            System.sleep(60);
                            this.player.setTranslate(1.7f, 0.0f, 8.0f);
                            this.shion.setTranslate(100.0f, 100.0f, 100.0f);
                            this.chaos.setTranslate(100.0f, 100.0f, 100.0f);
                            this.ziggy.setTranslate(100.0f, 100.0f, 100.0f);
                            this.momo.setTranslate(100.0f, 100.0f, 100.0f);
                            this.jr.setTranslate(100.0f, 100.0f, 100.0f);
                            this.cam0.setMode(0);
                            this.fade2.call(0);
                            System.sleep(60);
                            Runtime.setFlags(3077, 1, 1);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            break;
                        }
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.mtn(28, 1, 1.0f, true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        if (Runtime.getLeader() == 1) {
                            this.win.print(this.SHI_02, 0);
                        } else if (Runtime.getLeader() == 6) {
                            this.win.print(this.ZIG_02A, 0);
                        } else if (Runtime.getLeader() == 3) {
                            this.win.print(this.CHA_02A, 0);
                        } else if (Runtime.getLeader() == 4) {
                            this.win.print(this.MOM_02A, 0);
                        } else {
                            this.win.print(this.JUN_02A, 0);
                        }
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        break;
                    }
                    this.doorE.SetDoorType('\u0004');
                    break;
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
                Runtime.jumpCF(1881, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1731, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(1741, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1871, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(1841, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.5f, 0.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, 3.5f, 0.0f, 2.0f, 0.0f);
        this.teiten2.SetBgm(196615);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -13.0f, 0.0f);
        this.teiten3.SetBgm(196616);
        this.teiten4 = new Uwamono(28690, -12.5f, 0.0f, -17.5f, 0.0f);
        this.teiten4.SetBgm(196617);
        this.teiten5 = new Uwamono(28690, 13.5f, 0.0f, 5.2f, 0.0f);
        this.teiten5.SetBgm(196616);
        Stage.setVisible(-1, true);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 13.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 0.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.shion = new NPC_NORMAL(1, 11, 0, 4, 11, 100.0f, 100.0f, 100.0f, 0.0f);
        this.shion.setInvalidID(1);
        this.chaos = new NPC_NORMAL(3, 12, 0, 5, 12, 100.0f, 100.0f, 100.0f, 0.0f);
        this.chaos.setInvalidID(1);
        this.ziggy = new NPC_NORMAL(6, 13, 0, 6, 13, 100.0f, 100.0f, 100.0f, 0.0f);
        this.ziggy.setInvalidID(1);
        this.momo = new NPC_NORMAL(4, 14, 0, 7, 14, 100.0f, 100.0f, 100.0f, 0.0f);
        this.momo.setInvalidID(1);
        this.jr = new NPC_NORMAL(5, 15, 0, 8, 15, 100.0f, 100.0f, 100.0f, 0.0f);
        this.jr.setInvalidID(1);
        this.enemy2 = new Enepc();
        this.enemy2.init(18179, 3, 10.0f, 0.0f, 8.0f, 270.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 1, 1);
        float[] fArray = new float[40];
        fArray[0] = 10.0f;
        fArray[2] = 8.0f;
        fArray[3] = 1.0f;
        fArray[4] = 11.0f;
        fArray[6] = 8.0f;
        fArray[7] = 2.0f;
        fArray[8] = 12.0f;
        fArray[10] = 8.0f;
        fArray[11] = 3.0f;
        fArray[12] = 13.0f;
        fArray[14] = 8.0f;
        fArray[15] = 4.0f;
        fArray[16] = 14.0f;
        fArray[18] = 8.0f;
        fArray[19] = 5.0f;
        fArray[20] = 15.0f;
        fArray[22] = 8.0f;
        fArray[23] = 6.0f;
        fArray[24] = 16.0f;
        fArray[26] = 8.0f;
        fArray[27] = 7.0f;
        fArray[28] = 17.0f;
        fArray[30] = 8.0f;
        fArray[31] = 8.0f;
        fArray[32] = 18.0f;
        fArray[34] = 8.0f;
        fArray[35] = 9.0f;
        fArray[36] = 19.0f;
        fArray[38] = 8.0f;
        fArray[39] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy2.setParams(1, 3, 2, 3, fArray2);
        float[] fArray3 = new float[30];
        fArray3[0] = 10.0f;
        fArray3[2] = 8.0f;
        fArray3[3] = 11.0f;
        fArray3[5] = 8.0f;
        fArray3[6] = 12.0f;
        fArray3[8] = 8.0f;
        fArray3[9] = 13.0f;
        fArray3[11] = 8.0f;
        fArray3[12] = 14.0f;
        fArray3[14] = 8.0f;
        fArray3[15] = 15.0f;
        fArray3[17] = 8.0f;
        fArray3[18] = 16.0f;
        fArray3[20] = 8.0f;
        fArray3[21] = 17.0f;
        fArray3[23] = 8.0f;
        fArray3[24] = 18.0f;
        fArray3[26] = 8.0f;
        fArray3[27] = 19.0f;
        fArray3[29] = 8.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(fArray4);
        this.enemy3 = new Enepc();
        this.enemy3.init(18179, 3, 13.5f, 0.0f, 11.0f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[48];
        fArray5[0] = 13.5f;
        fArray5[2] = 11.0f;
        fArray5[3] = 1.0f;
        fArray5[4] = 13.5f;
        fArray5[6] = 12.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = 13.5f;
        fArray5[10] = 14.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = 13.5f;
        fArray5[14] = 16.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = 13.5f;
        fArray5[18] = 18.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = 13.5f;
        fArray5[22] = 20.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = 13.5f;
        fArray5[26] = 22.0f;
        fArray5[27] = 7.0f;
        fArray5[28] = 13.5f;
        fArray5[30] = 23.0f;
        fArray5[31] = 8.0f;
        fArray5[32] = 13.5f;
        fArray5[34] = 24.0f;
        fArray5[35] = 9.0f;
        fArray5[36] = 13.5f;
        fArray5[38] = 25.0f;
        fArray5[39] = 10.0f;
        fArray5[40] = 13.5f;
        fArray5[42] = 26.0f;
        fArray5[43] = 11.0f;
        fArray5[44] = 13.5f;
        fArray5[46] = 27.0f;
        fArray5[47] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy3.setParams(1, 3, 3, 3, fArray6);
        float[] fArray7 = new float[36];
        fArray7[0] = 13.5f;
        fArray7[2] = 11.0f;
        fArray7[3] = 13.5f;
        fArray7[5] = 12.0f;
        fArray7[6] = 13.5f;
        fArray7[8] = 14.0f;
        fArray7[9] = 13.5f;
        fArray7[11] = 16.0f;
        fArray7[12] = 13.5f;
        fArray7[14] = 18.0f;
        fArray7[15] = 13.5f;
        fArray7[17] = 20.0f;
        fArray7[18] = 13.5f;
        fArray7[20] = 22.0f;
        fArray7[21] = 13.5f;
        fArray7[23] = 23.0f;
        fArray7[24] = 13.5f;
        fArray7[26] = 24.0f;
        fArray7[27] = 13.5f;
        fArray7[29] = 25.0f;
        fArray7[30] = 13.5f;
        fArray7[32] = 26.0f;
        fArray7[33] = 13.5f;
        fArray7[35] = 27.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(fArray8);
        this.enemy4 = new Enepc();
        this.enemy4.init(18179, 3, 17.5f, 0.0f, 33.0f, 270.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(0, 0, 1, 1);
        float[] fArray9 = new float[20];
        fArray9[0] = 17.5f;
        fArray9[2] = 33.0f;
        fArray9[3] = 1.0f;
        fArray9[4] = 16.5f;
        fArray9[6] = 33.0f;
        fArray9[7] = 2.0f;
        fArray9[8] = 15.5f;
        fArray9[10] = 33.0f;
        fArray9[11] = 3.0f;
        fArray9[12] = 14.5f;
        fArray9[14] = 32.0f;
        fArray9[15] = 4.0f;
        fArray9[16] = 14.0f;
        fArray9[18] = 31.0f;
        fArray9[19] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy4.setParams(1, 2, 4, 3, fArray10);
        this.enemy5 = new Enepc();
        this.enemy5.init(18179, 3, 17.5f, 0.0f, 34.5f, 270.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(0, 0, 1, 1);
        float[] fArray11 = new float[40];
        fArray11[0] = 17.5f;
        fArray11[2] = 34.5f;
        fArray11[3] = 1.0f;
        fArray11[4] = 17.0f;
        fArray11[6] = 34.5f;
        fArray11[7] = 2.0f;
        fArray11[8] = 16.0f;
        fArray11[10] = 34.5f;
        fArray11[11] = 3.0f;
        fArray11[12] = 15.0f;
        fArray11[14] = 34.5f;
        fArray11[15] = 4.0f;
        fArray11[16] = 14.0f;
        fArray11[18] = 34.5f;
        fArray11[19] = 5.0f;
        fArray11[20] = 13.0f;
        fArray11[22] = 34.5f;
        fArray11[23] = 6.0f;
        fArray11[24] = 13.0f;
        fArray11[26] = 34.0f;
        fArray11[27] = 7.0f;
        fArray11[28] = 13.0f;
        fArray11[30] = 33.0f;
        fArray11[31] = 8.0f;
        fArray11[32] = 13.0f;
        fArray11[34] = 32.0f;
        fArray11[35] = 9.0f;
        fArray11[36] = 13.0f;
        fArray11[38] = 31.0f;
        fArray11[39] = -1.0f;
        float[] fArray12 = fArray11;
        this.enemy5.setParams(1, 2, 5, 3, fArray12);
        this.enemy6 = new Enepc();
        this.enemy6.init(17666, 5, 1.0f, 0.0f, 26.0f, 90.0f);
        this.enemy6.id = 6;
        this.enemy6.setGroup(2, 2, 2, 2);
        float[] fArray13 = new float[40];
        fArray13[0] = 1.0f;
        fArray13[2] = 26.0f;
        fArray13[3] = 1.0f;
        fArray13[4] = 2.0f;
        fArray13[6] = 26.0f;
        fArray13[7] = 2.0f;
        fArray13[8] = 3.0f;
        fArray13[10] = 26.0f;
        fArray13[11] = 3.0f;
        fArray13[12] = 4.0f;
        fArray13[14] = 26.0f;
        fArray13[15] = 4.0f;
        fArray13[16] = 5.0f;
        fArray13[18] = 26.0f;
        fArray13[19] = 5.0f;
        fArray13[20] = 6.0f;
        fArray13[22] = 26.0f;
        fArray13[23] = 6.0f;
        fArray13[24] = 7.0f;
        fArray13[26] = 26.0f;
        fArray13[27] = 7.0f;
        fArray13[28] = 8.0f;
        fArray13[30] = 26.0f;
        fArray13[31] = 8.0f;
        fArray13[32] = 9.0f;
        fArray13[34] = 26.0f;
        fArray13[35] = 9.0f;
        fArray13[36] = 10.0f;
        fArray13[38] = 26.0f;
        fArray13[39] = -1.0f;
        float[] fArray14 = fArray13;
        this.enemy6.setParams(1, 10, 4, 5, fArray14);
        this.shitai_01 = new NPC_NORMAL(18179, 7, 0, 13, 23, -18.874f, 0.0f, 6.915f, -45.0f);
        this.shitai_01.setInvalidID(1);
        this.shitai_01.disableDTKFlag(131087);
        this.shitai_01.setMotion(0, 1);
        this.shitai_01.setShadow(0, 0);
        this.shitai_02 = new NPC_NORMAL(18179, 8, 0, 13, 23, -14.816f, 0.0f, 7.068f, 0.0f);
        this.shitai_02.setInvalidID(1);
        this.shitai_02.disableDTKFlag(131087);
        this.shitai_02.setMotion(0, 2);
        this.shitai_02.setShadow(0, 0);
        this.BASE01 = new Uwamono(28672, -12.5f, -1.0f, -14.5f);
        this.BASE02 = new Uwamono(28672, -2.5f, -1.0f, 0.0f);
        this.BASE03 = new Uwamono(28672, 15.5f, -1.0f, 18.0f);
        this.BASE01.SetSize(15.0f, 1.0f, 8.0f);
        this.BASE02.SetSize(5.0f, 1.0f, 10.0f);
        this.BASE03.SetSize(5.0f, 1.0f, 10.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 209);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 210);
        this.item03 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 211);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 212);
        this.item05 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 213);
        new Uwamono(0, 20, this.item03);
        new Uwamono(1, 19);
        new Uwamono(2, 20);
        new Uwamono(43, 20, this.item01);
        new Uwamono(44, 20);
        new Uwamono(45, 19, this.item02);
        new Uwamono(17, 106);
        new Uwamono(16, 106, this.item04);
        new Uwamono(4, 32, this.item05);
        this.doorA = new Uwamono(21, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetSize(1.5f, 2.25f, 0.05f);
        this.doorB = new Uwamono(22, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(23, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(24, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(25, 40, '\u0001');
        this.doorE.SetDoorType('\u0002');
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        if (Runtime.getFlags(3139, 1) == 0) {
            Runtime.removeItem(10, 7);
            Runtime.setShootFlag(false);
            Runtime.setFlags(1017, 1, 1);
            Runtime.setFlags(3139, 1, 1);
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
    }
}

