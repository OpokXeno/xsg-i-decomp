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
import xeno.map.MC_ELS08B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0720
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS08B_PRJ {
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
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int button_flg = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono itembox;
    Unit Bed;
    Unit SW02;
    Effect eve00;
    Effect EF01;
    Effect EF02;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] CASE_SHION = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there.", "/[waitkey(1)]/[clear()]", "Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] CASE_KOSMOS = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there.", "/[waitkey(1)]/[clear()]", "Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] CASE_ZIGGY = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there.", "/[waitkey(1)]/[clear()]", "Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] CASE_SHI = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there.", "/[waitkey(1)]/[clear()]", "Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] CASE_ZIG = new String[]{"/[label(Cherenkov)]", "Who the hell are you...?", "/[waitkey(1)]/[clear()]", "Ah, you must be a crewman from that cruiser the Captain just rescued.", "/[waitkey(1)]/[clear()]", "The enemy mother ship back there is your guest. Go give it a warm welcome.", "/[waitkey(64)]/[close()]"};
    String[] DENJI_01 = new String[]{"Deactivate electromagnetic floor?", "/[waitkey(64)]/[close()]"};
    String[] DENJI_02 = new String[]{"Deactivating electromagnetic floor.", "/[waitkey(64)]/[close()]"};
    String[] DENJI_03 = new String[]{"Activate electromagnetic floor?", "/[waitkey(64)]/[close()]"};
    String[] DENJI_04 = new String[]{"Activating electromagnetic floor.", "/[waitkey(64)]/[close()]"};

    ST0720() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            block0:
            switch (n) {
                case 100: {
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    if (Runtime.getFlags(3025, 1) == 1) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.DENJI_01, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                if (Runtime.getFlags(3040, 1) == 1 && Runtime.getFlags(3041, 1) == 1 && Runtime.getFlags(3042, 1) == 1 && Runtime.getFlags(3043, 1) == 1) {
                                    Runtime.enable(65536);
                                    this.player.mtn(16, 1, 1.0f, true);
                                    System.sleep(40);
                                    Sound.effectPlay(196741);
                                    System.sleep(20);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 305);
                                    this.win.print(this.DENJI_02, 0);
                                    System.waitFor(this.win);
                                    this.EF02.disp(false);
                                    this.SW02.start(1, "Chibi");
                                    System.sleep(60);
                                    Runtime.setFlags(3025, 1, 0);
                                    Runtime.disable(65536);
                                    Runtime.setPlayerControl(true);
                                    this.button_flg = 0;
                                    break block0;
                                }
                                Runtime.enable(65536);
                                this.player.mtn(16, 1, 1.0f, true);
                                System.sleep(40);
                                Sound.effectPlay(196741);
                                System.sleep(20);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 305);
                                this.win.print(this.DENJI_02, 0);
                                System.waitFor(this.win);
                                this.EF02.disp(false);
                                this.SW02.start(1, "Chibi");
                                System.sleep(60);
                                Runtime.setFlags(3025, 1, 0);
                                this.fade.call(0);
                                System.sleep(30);
                                Runtime.disable(65536);
                                Runtime.setPlayerControl(true);
                                Runtime.jumpCF(66218, 0);
                                this.button_flg = 0;
                                break block0;
                            }
                            case 1: {
                                Runtime.setPlayerControl(true);
                                this.button_flg = 0;
                                break block0;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        break;
                    }
                    if (Runtime.getFlags(3025, 1) != 0) break;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.DENJI_03, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            if (Runtime.getFlags(3040, 1) == 1 && Runtime.getFlags(3041, 1) == 1 && Runtime.getFlags(3042, 1) == 1 && Runtime.getFlags(3043, 1) == 1) {
                                Runtime.enable(65536);
                                this.player.mtn(16, 1, 1.0f, true);
                                System.sleep(40);
                                Sound.effectPlay(196741);
                                System.sleep(20);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 305);
                                this.win.print(this.DENJI_04, 0);
                                System.waitFor(this.win);
                                this.SW02.start(1, "Deka");
                                System.sleep(30);
                                Runtime.setFlags(3025, 1, 1);
                                Runtime.disable(65536);
                                Runtime.setPlayerControl(true);
                                this.button_flg = 0;
                                break block0;
                            }
                            Runtime.enable(65536);
                            this.player.mtn(16, 1, 1.0f, true);
                            System.sleep(40);
                            Sound.effectPlay(196741);
                            System.sleep(20);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.DENJI_04, 0);
                            System.waitFor(this.win);
                            this.SW02.start(1, "Deka");
                            System.sleep(60);
                            Runtime.setFlags(3025, 1, 1);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            Runtime.jumpCF(66218, 0);
                            this.button_flg = 0;
                            break block0;
                        }
                        case 1: {
                            Runtime.setPlayerControl(true);
                            this.button_flg = 0;
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    break;
                }
            }
        }
    }

    public void TalkNPC1(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        if (Runtime.getLeader() == 1) {
            this.win.print(this.CASE_SHI, 0);
        } else if (Runtime.getLeader() == 2) {
            this.win.print(this.CASE_SHI, 0);
        } else if (Runtime.getLeader() == 6) {
            this.win.print(this.CASE_ZIG, 0);
        } else if (Runtime.getLeader() == 4) {
            this.win.print(this.CASE_ZIG, 0);
        } else if (Runtime.getLeader() == 3) {
            this.win.print(this.CASE_SHI, 0);
        }
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66226, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(66216, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(66266, 2);
                break;
            }
        }
    }

    void init() {
        this.player.setID(1);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3025, 1) == 1) {
            this.SW02 = new Mapunits();
            this.SW02.mapUnit(25);
            this.SW02.setTranslate(2.822f, 1.626f, -6.85f);
            this.SW02.setRotate(0.0f, 180.0f, 0.0f);
            this.SW02.start(4, null);
            this.EF02 = new Effect(1437, 2.822f, 1.626f, -6.65f, 0.0f);
            this.EF02.setScale(0.8f, 0.5f, 1.0f);
            this.EF02.disp(true);
            this.EF02.setClip(true);
        } else {
            this.SW02 = new Mapunits();
            this.SW02.mapUnit(25);
            this.SW02.setTranslate(2.822f, 1.626f, -6.85f);
            this.SW02.setRotate(0.0f, 180.0f, 0.0f);
            this.SW02.start(4, null);
            this.SW02.setScale(0.0f, 0.0f, 0.0f);
            this.EF02 = new Effect(1437, 2.822f, 1.626f, -6.65f, 0.0f);
            this.EF02.setScale(0.8f, 0.5f, 1.0f);
            this.EF02.disp(false);
            this.EF02.setClip(true);
        }
        new Uwamono(28678, -2.0f, 0.0f, -4.5f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(0, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 2.8f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196625);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(279, 11, 0, 0, 3, 2.4294763f, 0.0f, 5.412483f, 225.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.disableDTKFlag(8);
        this.npc1.disableDTKFlag(131072);
        this.doorA = new Uwamono(11, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(21, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(4, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.eve00 = new Effect(1444, 0);
        this.eve00.disp(true);
        this.EF01 = new Effect(1011, 2.825f, 1.2f, -6.6f, 0.0f);
        this.EF01.setRotate(-130.0f, 180.0f, 0.0f);
        this.EF01.setScale(0.35f, 0.35f, 1.5f);
        this.EF01.disp(true);
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

    class Mapunits
            extends Unit {
        Mapunits() {
        }

        void Chibi() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST0720.this.SW02.getScale();
                    ST0720.this.SW02.setScale(0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n));
                }
                if (n == 31) break;
                ++n;
                System.sleep(1);
            }
        }

        void Deka() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST0720.this.SW02.getScale();
                    ST0720.this.SW02.setScale(0.033333335f * (float) n, 0.033333335f * (float) n, 0.033333335f * (float) n);
                }
                if (n == 31) break;
                ++n;
                System.sleep(1);
            }
            ST0720.this.EF02.disp(true);
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

