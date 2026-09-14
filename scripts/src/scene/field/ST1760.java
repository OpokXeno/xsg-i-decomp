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
import xeno.map.MC_DYU06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1760
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU06_PRJ {
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
    Enepc All;
    Enepc Zig;
    Enepc Cha;
    Enepc Jun;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Effect fade1;
    Effect fade2;
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;
    String[] ALLEN_00 = new String[]{"/[label(Allen)]", "Shelley called for me, so I'm heading over to Durandal's bridge.", "/[waitkey(64)]/[close()]"};
    String[] ALLEN_01 = new String[]{"/[label(Allen)]", "Shelley called for me, so I'm heading over to Durandal's bridge.", "/[waitkey(64)]/[close()]"};
    String[] ZIGGY_00 = new String[]{"/[label(Ziggy)]", "Joachim Mizrahi...", "/[waitkey(1)]/[clear()]", "He is a very mysterious man.", "/[waitkey(64)]/[close()]"};
    String[] CHAOS_00 = new String[]{"/[label(chaos)]", "I'm a little worried about MOMO.", "/[waitkey(1)]/[clear()]", "I wonder where she went?", "/[waitkey(64)]/[close()]"};
    String[] JUNIOUR_00 = new String[]{"/[label(Jr.)]", "I wonder if what I said bothered her?", "/[waitkey(1)]/[clear()]", "Perhaps the truth that exists here may have been a bit too much for her.", "/[waitkey(64)]/[close()]"};
    String[] SHION_00 = new String[]{"/[label(Shion)]", "I wonder where MOMO went?", "/[waitkey(64)]/[close()]"};
    String[] KOSMOS_00 = new String[]{"/[label(KOS-MOS)]", "I cannot confirm MOMO's presence.", "/[waitkey(1)]/[clear()]", "Shion, perhaps we should commence a search for her?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1760() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-12.124f, 0.815f, 4.183f);
        this.camEV.setRotate(8.238f, 30.952f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3158, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.fade1.call(0);
                    System.sleep(60);
                    this.cam0.setMode(-1);
                    this.EV_Camera00();
                    this.fade2.call(0);
                    System.sleep(60);
                    this.All.kickEnepc(4, 1);
                    this.All.kickEnepc(0, 9);
                    this.All.kickEnepc(9, 100);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ALLEN_00, 0);
                    System.waitFor(this.win);
                    this.All.kickEnepc(4, 0);
                    System.sleep(1);
                    this.All.kickEnepc(4, 3);
                    System.sleep(1);
                    this.All.kickEnepc(1, 3);
                    System.sleep(1);
                    this.All.move(60, -28.0f, 0.0f, true);
                    System.sleep(61);
                    this.All.kickEnepc(4, 2);
                    this.All.setTranslate(-100.0f, -100.0f, -100.0f);
                    this.fade1.call(0);
                    System.sleep(60);
                    this.cam0.setMode(0);
                    this.fade2.call(0);
                    System.sleep(60);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (Runtime.getLeader() == 1) {
                        this.win.print(this.SHION_00, 0);
                    } else {
                        this.win.print(this.KOSMOS_00, 0);
                    }
                    System.waitFor(this.win);
                    Runtime.setFlags(3158, 1, 1);
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 1) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3164, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAIL1, 0);
                Runtime.mailArriveSet(48);
                Runtime.setFlags(3164, 1, 1);
                System.waitFor(this.win);
                System.sleep(15);
                Runtime.setPlayerControl(true);
                Runtime.mailExec(1);
            }
        }
    }

    public void TalkALLEN(Enepc enepc) {
        if (this.npc1talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.ALLEN_00, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.ALLEN_01, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkCHAOS(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.CHAOS_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkJUNIOUR(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.JUNIOUR_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkZIGGY(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.ZIGGY_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(1832, 1);
                    break;
                }
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(1833, 1);
                    break;
                }
                Runtime.jumpCF(1830, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1720, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(3098, 1) == 0) {
            Runtime.setOutFriend(4);
            Runtime.setOutFriend(6);
            Runtime.setOutFriend(3);
            Runtime.setOutFriend(5);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 2);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
            System.println("パーティー情報・leader_shion,kosmos,chaos,*,*,*");
            Runtime.setFlags(3098, 1, 1);
        }
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
        Stage.setVisible(40, false);
        Stage.setVisible(58, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setColor(0.95f, 0.95f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.45f, 0.45f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.45f, 0.45f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 20.0f, 0.0f, 50.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 10.0f, 0.0f, 50.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 50.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, -10.0f);
        if (Runtime.getFlags(304, 1) == 0) {
            if (Runtime.getFlags(3158, 1) == 0) {
                this.All = new NPC_NORMAL(263, 11, 0, 2, 3, -16.023f, 0.0f, -0.462f, 90.0f);
                this.All.talkto("TalkALLEN");
                this.All.disableDTKFlag(131082);
                this.All.enableDTKFlag(4);
            }
            this.Zig = new NPC_NORMAL(6, 11, 0, 2, 6, -8.104f, -2.0f, -2.406f, 15.0f);
            this.Zig.talkto("TalkZIGGY");
            this.Zig.disableDTKFlag(131082);
            this.Zig.enableDTKFlag(4);
            this.Cha = new NPC_NORMAL(3, 11, 0, 2, 9, -6.067f, -2.0f, -0.811f, -30.0f);
            this.Cha.talkto("TalkCHAOS");
            this.Cha.disableDTKFlag(131082);
            this.Cha.enableDTKFlag(4);
            this.Jun = new NPC_NORMAL(5, 11, 0, 2, 12, -6.839f, -2.0f, 1.744f, 220.0f);
            this.Jun.talkto("TalkJUNIOUR");
            this.Jun.disableDTKFlag(131082);
            this.Jun.enableDTKFlag(4);
        }
        this.doorA = new Uwamono(0, 42, '\u0001');
        new Uwamono(1, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(2, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
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

