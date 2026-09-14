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
import xeno.map.MC_ELS05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0551
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS05_PRJ {
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
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    int button_flg = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2042;
    int S2057;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono Base01;
    Uwamono Base02;
    Uwamono Base03;
    Uwamono Base04;
    Unit Y1;
    Unit Y2;
    Unit X1;
    Unit X2;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve00;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] msg0436E833 = new String[]{"/[label(Shion)]", "Good morning, Commander!\n", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E834 = new String[]{"/[label(Cherenkov)]", "Yes.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E835 = new String[]{"/[label(Shion)]", "Really, Commander!\n", "\n", "Don't you think even a soldier could give a decent morning greeting?", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E836 = new String[]{"/[label(Cherenkov)]", "Ah, yes.\n", "You're right.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E837 = new String[]{"/[label(Shion)]", "Go-od mor-ning.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E838 = new String[]{"/[label(Cherenkov)]", "G-good...morning.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E839 = new String[]{"/[label(Shion)]", "Good morning!", "/[waitkey(64)]/[close()]"};
    String[] msg043747E6 = new String[]{"/[label(Shion)]", "Commander, I know what happened on the Woglinde was truly horrific. But this is a civilian passenger freighter, and we are simply here as guests. I don't think you\nneed to be such a stickler to the ways of the military.", "/[waitkey(64)]/[clear()]"};
    String[] msg043747E7 = new String[]{"/[label(Cherenkov)]", "I see...That's true. I'll try to be more careful.", "/[waitkey(64)]/[close()]"};
    String[] msg1436E833 = new String[]{"/[label(Cherenkov)]", "What's wrong?", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E834 = new String[]{"/[label(Shion)]", "Oh, I'm going to see what's wrong with the catapult.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E835 = new String[]{"/[label(Cherenkov)]", "There's something wrong? How bad is it?", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E836 = new String[]{"/[label(Shion)]", "No, I don't think it's anything serious. But I'm going to check it out just to be safe.", "/[waitkey(64)]/[clear()]"};
    String[] msg1436E837 = new String[]{"/[label(Cherenkov)]", "I see...", "/[waitkey(64)]/[close()]"};
    String[] msg143747E4 = new String[]{"/[label(Cherenkov)]", "I'm sure you know this, but seemingly insignificant things can lead to huge accidents. Be very careful.", "/[waitkey(64)]/[close()]"};
    String[] Loc0_1 = new String[]{"/[label(Warning)]", "Operating slide deck. Workers within its moving range should retreat to a safe area immediately.", "/[waitkey(64)]/[close()]"};
    String[] kakuheki_1 = new String[]{"/[label()]", "There is a slide deck switch.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 8.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 8.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 8, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] Loc0_2 = new String[]{"/[label(Warning)]", "The slide deck is currently locked and cannot be operated.", "/[waitkey(1)]/[clear()]", "In order to operate the slide deck, you will need a /[color(0x329bbe)]Disarm Key/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] Loc0_3 = new String[]{"/[label(Warning)]", "Releasing the slide deck lock.", "/[waitkey(64)]/[close()]"};

    ST0551() {
    }

    void Final_init(int n) {
    }

    public void HashigoTop(int n) {
        System.println("top***********************");
        switch (n) {
            case 0: {
                System.println("top");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(541, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (this.button_flg == 1) {
                    return;
                }
                this.button_flg = 1;
                if (Runtime.checkItem(10, 64) == 1) {
                    if (Runtime.getFlags(3095, 1) == 1) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.kakuheki_1, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Press\nDon't press");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                if (Runtime.getFlags(3020, 1) == 0) {
                                    Runtime.enable(65536);
                                    this.player.rotY(10, 180.0f, true);
                                    System.sleep(11);
                                    System.sleep(4);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(20);
                                    Sound.effectPlay(196741);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 15);
                                    this.win.print(this.Loc0_1, 0);
                                    System.waitFor(this.win);
                                    Sound.effectPlay(196743);
                                    this.Y1.start(1, "Move");
                                    Runtime.setFlags(3020, 1, 1);
                                    this.button_flg = 0;
                                    break block0;
                                }
                                Runtime.setPlayerControl(false);
                                Runtime.enable(65536);
                                this.player.rotY(10, 180.0f, true);
                                System.sleep(11);
                                System.sleep(4);
                                this.player.mtn(12, 1, 1.0f, true);
                                System.sleep(20);
                                Sound.effectPlay(196741);
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.Loc0_1, 0);
                                System.waitFor(this.win);
                                Sound.effectPlay(196743);
                                this.Y1.start(1, "Move2");
                                Runtime.setFlags(3020, 1, 0);
                                this.button_flg = 0;
                                break block0;
                            }
                        }
                        this.button_flg = 0;
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(196741);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.Loc0_3, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3095, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    break;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.Loc0_2, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                this.button_flg = 0;
                break;
            }
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(3208, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(55);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3208, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3228, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3288, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                this.doorC.SetDoorType('\u0004');
                Runtime.setFlags(3288, 1, 1);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0551.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
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
                Runtime.jumpCF(561, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(561, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(581, 1);
                break;
            }
            case 4: {
                System.println("57_ON");
                Runtime.setFlags(3057, 1, 0);
                Runtime.jumpCF(660, 1);
                break;
            }
            case 5: {
                System.println("58_ON");
                Runtime.setFlags(3058, 1, 0);
                Runtime.jumpCF(660, 2);
                break;
            }
        }
    }

    void init() {
        this.S2009 = Runtime.getFlags(115, 1);
        this.S2013 = Runtime.getFlags(122, 1);
        this.S2013B = Runtime.getFlags(123, 1);
        this.S2014 = Runtime.getFlags(124, 1);
        this.S2014B = Runtime.getFlags(125, 1);
        this.S2028 = Runtime.getFlags(141, 1);
        this.MOMO = Runtime.getFlags(7014, 1);
        this.S2030 = Runtime.getFlags(143, 1);
        this.S2040C = Runtime.getFlags(158, 1);
        this.ZIGGY = Runtime.getFlags(7015, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S2057 = Runtime.getFlags(179, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(50, false);
        Stage.setVisible(51, false);
        Stage.setVisible(53, false);
        Stage.setVisible(52, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, 9.5f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten1.SetBgmType('\u0001');
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFLockX(1, 9.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 9.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, 9.0f, 6.0f, -15.5f, 40.0f, -90.0f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.03f, 0.03f);
        this.cam0.setCFAngle(11, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.03f, 0.03f);
        this.cam0.setCFLockX(11, 9.0f);
        this.enemy1 = new Enepc();
        this.enemy1.init(279, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy1.setVisible(false);
        this.enemy1.setInvalidID(1);
        this.enemy1.disableDTKFlag(131072);
        this.enemy1.disableDTKFlag(65536);
        this.enemy1.kickEnepc(4, 2);
        this.enemy1.kickEnepc(19, 1, 0, 350, 1);
        this.itembox = new Uwamono(28680, -6.0f, 0.0f, 13.0f, 90.0f, 120);
        this.itembox.SetSymbol(28725);
        this.itembox.SetCallNo(1);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 76);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 77);
        this.item03 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 78);
        this.item04 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 79);
        new Uwamono(28, 4);
        new Uwamono(32, 4, this.item01);
        new Uwamono(31, 0, this.item02);
        new Uwamono(30, 0, this.item03);
        new Uwamono(29, 0, this.item04);
        if (Runtime.getFlags(3208, 1) == 0) {
            new Uwamono(4, 24);
            new Uwamono(39, 24);
        } else {
            Stage.setVisible(4, false);
            Stage.setVisible(39, false);
        }
        this.Base01 = new Uwamono(28672, -7.0f, -1.0f, 0.0f, 0.0f);
        this.Base01.SetSize(5.0f, 1.0f, 5.0f);
        this.Base02 = new Uwamono(28672, -7.5f, -1.0f, 17.5f, 0.0f);
        this.Base02.SetSize(5.0f, 1.0f, 5.0f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.eve00 = new Effect(1417, 0);
        this.eve00.disp(true);
        if (Runtime.getFlags(3020, 1) == 0) {
            this.player.setID(2);
        } else if (Runtime.getFlags(3020, 1) == 0) {
            this.player.setID(1);
        }
        if (Runtime.getFlags(3020, 1) == 1) {
            this.Y1 = new Mapunits();
            this.Y1.mapUnit(38);
            this.Y1.start(4, null);
            this.Y1.getTranslate();
            this.Y1.setTranslate(this.Y1.px, this.Y1.py + 1.0f, this.Y1.pz);
            this.X1 = new Mapunits();
            this.X1.mapUnit(37);
            this.X1.start(4, null);
            this.X1.getTranslate();
            this.X1.setTranslate(this.X1.px + 3.45f, this.X1.py, this.X1.pz);
            this.Y2 = new Mapunits();
            this.Y2.mapUnit(7);
            this.Y2.start(4, null);
            this.Y2.getTranslate();
            this.Y2.setTranslate(this.Y2.px, this.Y2.py + 1.0f, this.Y2.pz);
            this.X2 = new Mapunits();
            this.X2.mapUnit(59);
            this.X2.start(4, null);
            this.X2.getTranslate();
            this.X2.setTranslate(this.X2.px - 3.45f, this.X2.py, this.X2.pz);
        } else {
            this.Y1 = new Mapunits();
            this.Y1.mapUnit(38);
            this.Y1.start(4, null);
            this.X1 = new Mapunits();
            this.X1.mapUnit(37);
            this.X1.start(4, null);
            this.Y2 = new Mapunits();
            this.Y2.mapUnit(7);
            this.Y2.start(4, null);
            this.X2 = new Mapunits();
            this.X2.mapUnit(59);
            this.X2.start(4, null);
        }
        this.doorA = new Uwamono(45, 40, '\u0001');
        new Uwamono(60, 40, '\u0001', this.doorA);
        this.doorA.SetDoorRange(0.75f);
        this.doorB = new Uwamono(48, 40, '\u0001');
        this.doorD = new Uwamono(46, 40, '\u0001');
        this.doorE = new Uwamono(44, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        this.doorE.SetDoorType('\u0004');
        if (Runtime.getFlags(3288, 1) == 0) {
            this.doorC = new Uwamono(58, 40, '\u0004');
            this.doorC.SetSize(0.25f, 2.25f, 1.5f);
            this.doorC.SetDoorType('\u0002');
        } else {
            this.doorC = new Uwamono(58, 40, '\u0004');
            this.doorC.SetSize(0.25f, 2.25f, 1.5f);
            this.doorC.SetDoorType('\u0004');
        }
        this.Y1.setRotateY(180.0f);
        this.X2.setRotateY(180.0f);
        if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_0();
        } else {
            this.npcset_0();
        }
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3248, 1, 1);
                break;
            }
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }

    class Mapunits
            extends Unit {
        Mapunits() {
        }

        void Move() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST0551.this.X1.getTranslate();
                    ST0551.this.X1.setTranslate(ST0551.this.X1.px + 0.115f, ST0551.this.X1.py, ST0551.this.X1.pz);
                    ST0551.this.X2.getTranslate();
                    ST0551.this.X2.setTranslate(ST0551.this.X2.px - 0.115f, ST0551.this.X2.py, ST0551.this.X2.pz);
                }
                if (n >= 30 && n < 90) {
                    ST0551.this.Y1.getTranslate();
                    ST0551.this.Y1.setTranslate(ST0551.this.Y1.px, ST0551.this.Y1.py + 0.016666668f, ST0551.this.Y1.pz);
                    ST0551.this.Y2.getTranslate();
                    ST0551.this.Y2.setTranslate(ST0551.this.Y2.px, ST0551.this.Y2.py + 0.016666668f, ST0551.this.Y2.pz);
                }
                if (n == 91) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0551.this.player.setID(1);
        }

        void Move2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0551.this.Y1.getTranslate();
                    ST0551.this.Y1.setTranslate(ST0551.this.Y1.px, ST0551.this.Y1.py - 0.016666668f, ST0551.this.Y1.pz);
                    ST0551.this.Y2.getTranslate();
                    ST0551.this.Y2.setTranslate(ST0551.this.Y2.px, ST0551.this.Y2.py - 0.016666668f, ST0551.this.Y2.pz);
                }
                if (n >= 60 && n < 90) {
                    ST0551.this.X1.getTranslate();
                    ST0551.this.X1.setTranslate(ST0551.this.X1.px - 0.115f, ST0551.this.X1.py, ST0551.this.X1.pz);
                    ST0551.this.X2.getTranslate();
                    ST0551.this.X2.setTranslate(ST0551.this.X2.px + 0.115f, ST0551.this.X2.py, ST0551.this.X2.pz);
                }
                if (n == 91) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0551.this.player.setID(2);
        }
    }
}

