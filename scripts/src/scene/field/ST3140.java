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
import xeno.map.MC_GNK20_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3140
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK20_PRJ {
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
    Enepc mom;
    Enepc jun;
    Enepc kos;
    Enepc cha;
    Enepc shi;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit Sora;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect E01;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int button_flg = 0;
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
    Uwamono BABABA;
    Uwamono BIBIBI;
    Uwamono item01;
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
    String[] DOA_00 = new String[]{"Remove the lock?", "/[waitkey(64)]/[close()]"};
    String[] DOA_01 = new String[]{"The lock has been removed.", "/[waitkey(64)]/[close()]"};
    String[] Dummy_00 = new String[]{"Explanation of Lost Jerusalem (temp).", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 6.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 6.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 6, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "What an amazing hologram.", "/[waitkey(64)]/[close()]"};
    String[] KOS_00 = new String[]{"/[label(KOS-MOS)]", "All observable space appears to be projected here.", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "All observable space...", "/[waitkey(1)]/[clear()]", "...?", "/[waitkey(1)]/[clear()]", "I wonder what that black area is? It doesn't look like a black hole.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "Guess it's not quite all observable space.", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "Lost Jerusalem...", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "What?", "/[waitkey(64)]/[close()]"};
    String[] CHA_01 = new String[]{"/[label(chaos)]", "Lost Jerusalem was once our homeland in the distant past, but no one can go near it now. Actually, nobody knows its location anymore.", "/[waitkey(1)]/[clear()]", "It could be that pitch black part there.", "/[waitkey(64)]/[close()]"};
    String[] JUN_01 = new String[]{"/[label(Jr.)]", "I've heard that the government has been working on a project to find Lost Jerusalem for quite some time.", "/[waitkey(1)]/[clear()]", "...It looks like the radius of that black region is at least several hundred million light years.", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "Our homeland is somewhere in there...", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Coordinate axis 0,0,0...\n", "Lost Jerusalem. What could be there?", "/[waitkey(64)]/[close()]"};

    ST3140() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.687f, 3.471f, 10.879f);
        this.camEV.setRotate(-24.114f, 29.139f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.947f, 6.671f, 3.186f);
        this.camEV.setRotate(-43.594f, -32.8f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3086, 1) != 0) return;
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.DOA_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.DOA_01, 0);
                            System.waitFor(this.win);
                            Runtime.setFlags(3086, 1, 1);
                            this.doorC.SetDoorType('\u0004');
                            Runtime.setPlayerControl(true);
                            System.sleep(1);
                            Runtime.disable(262144);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    System.sleep(1);
                    Runtime.disable(262144);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3140, 1) == 0) {
                        this.player.getTranslate();
                        if (this.player.py <= -2.0f) {
                            return;
                        }
                        Runtime.setPlayerControl(false);
                        this.fade1.call(0);
                        System.sleep(60);
                        this.player.setTranslate(100.0f, 0.0f, 100.0f);
                        this.shi.kickEnepc(4, 1);
                        this.kos.kickEnepc(4, 1);
                        this.mom.kickEnepc(4, 1);
                        this.jun.kickEnepc(4, 1);
                        this.cha.kickEnepc(4, 1);
                        this.enemy1.kickEnepc(4, 2);
                        this.enemy2.kickEnepc(4, 2);
                        this.enemy3.kickEnepc(4, 2);
                        this.enemy1.setVisible(false);
                        this.enemy2.setVisible(false);
                        this.enemy3.setVisible(false);
                        this.shi.setLocation(4, 0);
                        this.kos.setLocation(4, 1);
                        this.mom.setLocation(4, 2);
                        this.jun.setLocation(4, 3);
                        this.cha.setLocation(4, 4);
                        this.cam0.setMode(-1);
                        this.EV_Camera00();
                        this.fade2.call(0);
                        System.sleep(60);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.MOM_00, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.KOS_00, 0);
                        System.waitFor(this.win);
                        this.EV_Camera01();
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_00, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.JUN_00, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.CHA_00, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_01, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.CHA_01, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.JUN_01, 0);
                        System.waitFor(this.win);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SHI_02, 0);
                        System.waitFor(this.win);
                        this.fade1.call(0);
                        System.sleep(60);
                        this.cam0.setMode(0);
                        this.player.setTranslate(0.0f, 0.0f, 4.0f);
                        this.shi.setTranslate(100.0f, 100.0f, 100.0f);
                        this.kos.setTranslate(100.0f, 100.0f, 100.0f);
                        this.mom.setTranslate(100.0f, 100.0f, 100.0f);
                        this.jun.setTranslate(100.0f, 100.0f, 100.0f);
                        this.cha.setTranslate(100.0f, 100.0f, 100.0f);
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy2.kickEnepc(4, 0);
                        this.enemy3.kickEnepc(4, 0);
                        this.enemy1.setVisible(true);
                        this.enemy2.setVisible(true);
                        this.enemy3.setVisible(true);
                        Runtime.setFlags(3140, 1, 1);
                        this.fade2.call(0);
                        System.sleep(60);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    this.player.getTranslate();
                    if (this.player.py <= -2.0f) {
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3206, 1) == 0) {
                    this.player.getTranslate();
                    if (this.player.py >= -2.0f) {
                        return;
                    }
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Sound.effectPlay(55);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3206, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
                if (Runtime.getFlags(3226, 1) == 0) {
                    this.player.getTranslate();
                    if (this.player.py >= -2.0f) {
                        return;
                    }
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
                if (Runtime.getFlags(3286, 1) != 0) return;
                this.player.getTranslate();
                if (this.player.py >= -2.0f) {
                    return;
                }
                if (this.button_flg == 1) {
                    return;
                }
                this.button_flg = 1;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                this.doorA.SetDoorType('\u0004');
                Runtime.setFlags(3286, 1, 1);
                Runtime.setPlayerControl(true);
                this.button_flg = 0;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3087, 1, 1);
                break;
            }
            case 2: {
                System.println("BIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBIBI");
                Runtime.setFlags(3187, 1, 1);
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
                Runtime.jumpCF(68546, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68666, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(68656, 4);
                break;
            }
            case 3: {
                Runtime.jumpCF(68646, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -5.0f, 0.0f, 12.5f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, 5.0f, 0.0f, 12.5f, 0.0f);
        this.teiten2.SetBgm(196621);
        this.teiten3 = new Uwamono(28690, -9.0f, 0.0f, -10.0f, 0.0f);
        this.teiten3.SetBgm(196621);
        this.teiten4 = new Uwamono(28690, 9.0f, 0.0f, -10.0f, 0.0f);
        this.teiten4.SetBgm(196621);
        this.teiten5 = new Uwamono(28690, 0.0f, -4.0f, 0.0f, 0.0f);
        this.teiten5.SetBgm(196622);
        Stage.setVisible(-1, true);
        this.Sora = new Mapunits();
        this.Sora.mapUnit(138);
        this.Sora.start(4, null);
        this.Sora.start(1, "KAITEN");
        this.E01 = new Effect(1653, 0.0f, 2.2f, 1.0f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
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
        Stage.setColor(1.2f, 1.2f, 1.2f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, -9.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 9.5f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 15.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFPedestal(6, 11.0f, 3.55f, 13.0f, 45.0f, -16.65f, -15.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(6, 1);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        if (Runtime.getFlags(3140, 1) == 0) {
            this.shi = new NPC_NORMAL(1, 11, 0, 3, 13, 100.0f, 100.0f, 100.0f, 0.0f);
            this.shi.setInvalidID(1);
            this.shi.setMotion(0, 9);
            this.kos = new NPC_NORMAL(2, 12, 0, 3, 11, 100.0f, 100.0f, 100.0f, 0.0f);
            this.kos.setInvalidID(1);
            this.kos.setMotion(0, 9);
            this.mom = new NPC_NORMAL(4, 13, 0, 3, 9, 100.0f, 100.0f, 100.0f, 0.0f);
            this.mom.setInvalidID(1);
            this.mom.setMotion(0, 9);
            this.jun = new NPC_NORMAL(5, 14, 0, 3, 10, 100.0f, 100.0f, 100.0f, 0.0f);
            this.jun.setInvalidID(1);
            this.jun.setMotion(0, 9);
            this.cha = new NPC_NORMAL(3, 15, 0, 3, 12, 100.0f, 100.0f, 100.0f, 0.0f);
            this.cha.setInvalidID(1);
            this.cha.setMotion(0, 9);
        }
        this.enemy1 = new Enepc();
        this.enemy1.init(16396, 3, 8.5f, 0.0f, 7.7f, 180.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(2, 2, 3, 3);
        float[] fArray = new float[12];
        fArray[0] = 8.5f;
        fArray[2] = 7.7f;
        fArray[3] = -1.0f;
        fArray[4] = 7.0f;
        fArray[6] = 8.9f;
        fArray[8] = 2.5f;
        fArray[10] = 9.3f;
        fArray[11] = 1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[9];
        fArray3[0] = 8.5f;
        fArray3[2] = 7.7f;
        fArray3[3] = 7.0f;
        fArray3[5] = 8.9f;
        fArray3[6] = 2.6f;
        fArray3[8] = 9.3f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 85, 0);
        this.enemy2 = new Enepc();
        this.enemy2.init(16396, 3, -9.5f, 0.0f, 7.5f, 180.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(3, 3, 3, 5);
        float[] fArray5 = new float[16];
        fArray5[0] = -9.5f;
        fArray5[2] = 7.5f;
        fArray5[3] = -1.0f;
        fArray5[4] = -9.5f;
        fArray5[6] = 2.5f;
        fArray5[8] = -9.5f;
        fArray5[10] = -3.5f;
        fArray5[12] = -9.5f;
        fArray5[14] = -5.5f;
        fArray5[15] = 1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 1, 2, 3, fArray6);
        float[] fArray7 = new float[18];
        fArray7[0] = -9.5f;
        fArray7[2] = 7.5f;
        fArray7[3] = -9.5f;
        fArray7[5] = 2.5f;
        fArray7[6] = -9.5f;
        fArray7[8] = -3.5f;
        fArray7[9] = -9.5f;
        fArray7[11] = -5.5f;
        fArray7[12] = -9.5f;
        fArray7[14] = -3.5f;
        fArray7[15] = -9.5f;
        fArray7[17] = 2.5f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy2.kickEnepc(10, 85, 0);
        this.enemy3 = new Enepc();
        this.enemy3.init(16396, 3, 2.5f, 0.0f, -10.0f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(5, 5, 5, 5);
        float[] fArray9 = new float[16];
        fArray9[0] = 2.5f;
        fArray9[2] = -10.0f;
        fArray9[3] = -1.0f;
        fArray9[4] = 3.5f;
        fArray9[6] = -9.0f;
        fArray9[8] = 6.5f;
        fArray9[10] = -9.0f;
        fArray9[11] = 1.0f;
        fArray9[12] = 7.8f;
        fArray9[14] = -7.9f;
        fArray9[15] = 2.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 1, 3, 3, fArray10);
        float[] fArray11 = new float[18];
        fArray11[0] = 2.5f;
        fArray11[2] = -10.0f;
        fArray11[3] = 3.5f;
        fArray11[5] = -9.0f;
        fArray11[6] = 6.5f;
        fArray11[8] = -9.0f;
        fArray11[9] = 7.8f;
        fArray11[11] = -7.9f;
        fArray11[12] = 6.5f;
        fArray11[14] = -9.0f;
        fArray11[15] = 3.5f;
        fArray11[17] = -9.0f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy3.kickEnepc(10, 85, 0);
        this.enemy1.kickEnepc(19, 1, 0, 500, 1);
        new Uwamono(2, 31);
        if (Runtime.getFlags(3087, 1) == 0) {
            this.BABABA = new Uwamono(3, 31);
            this.BABABA.SetCallNo(1);
        } else {
            Stage.setVisible(3, false);
        }
        new Uwamono(4, 33);
        if (Runtime.getFlags(3187, 1) == 0) {
            this.BIBIBI = new Uwamono(5, 33);
            this.BIBIBI.SetCallNo(2);
        } else {
            Stage.setVisible(5, false);
        }
        if (Runtime.getFlags(3286, 1) == 0) {
            this.doorA = new Uwamono(1, 40, '\u0004');
            this.doorA.SetDiffSize(0.0f, -0.01f, 0.0f);
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA = new Uwamono(1, 40, '\u0004');
            this.doorA.SetDiffSize(0.0f, -0.01f, 0.0f);
            this.doorA.SetDoorType('\u0004');
        }
        this.doorB = new Uwamono(16, 40, '\u0002');
        this.doorB.SetDoorRange(1.49f);
        this.doorB.SetDoorType('\u0004');
        if (Runtime.getFlags(3086, 1) == 0) {
            this.doorC = new Uwamono(17, 40, '\u0001');
            this.doorC.SetDoorRange(1.49f);
            this.doorC.SetDoorType('\u0002');
        } else {
            this.doorC = new Uwamono(17, 40, '\u0001');
            this.doorC.SetDoorRange(1.49f);
            this.doorC.SetDoorType('\u0004');
        }
        this.doorD = new Uwamono(107, 40, '\u0002');
        this.doorD.SetDoorRange(1.49f);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(124, 40, '\u0002');
        new Uwamono(123, 40, '\u0002', this.doorE);
        this.doorE.SetDoorType('\u0004');
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

        void KAITEN() {
            while (true) {
                this.setRotate(this.rx, this.ry - 0.01f, this.rz);
                System.sleep(1);
            }
        }
    }
}

