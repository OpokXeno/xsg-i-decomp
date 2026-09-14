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
import xeno.map.MC_ELS05B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0690
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS05B_PRJ {
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
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
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
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono Base01;
    Uwamono Base02;
    Uwamono Base03;
    Uwamono Base04;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int button_flg = 0;
    int button2_flg = 0;
    Unit Y1;
    Unit Y2;
    Unit X1;
    Unit X2;
    Unit kidou;
    Effect EF01;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect fade;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    Effect E08;
    Effect E09;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    int page;
    String[] SSS = new String[]{"/[label(Shion)]", "We have too many people to fight the enemy at one time. We should decide who's going to participate in the fighting.", "/[waitkey(64)]/[close()]"};
    String[] KKK = new String[]{"/[label(KOS-MOS)]", "According to calculations, it's optimal to have three people participate in the melee. Shion, please determine battle party.", "/[waitkey(64)]/[close()]"};
    String[] CCC = new String[]{"/[label(chaos)]", "There's nothing wrong with having lots of people to fight the enemy, but it gets harder to move around.", "/[waitkey(1)]/[clear()]", "So about three people would be ideal.", "/[waitkey(64)]/[close()]"};
    String[] ZZZ = new String[]{"/[label(Ziggy)]", "We shouldn't need that many people to annihilate the enemy.", "/[waitkey(1)]/[clear()]", "Now...what combination would be best?", "/[waitkey(64)]/[close()]"};
    String[] MMM = new String[]{"/[label(MOMO)]", "Is three people the best for battle? I would like to help.", "/[waitkey(64)]/[close()]"};
    String[] Loc0_1 = new String[]{"/[label(Warning)]", "Operating slide deck. Workers in the movement range, please retreat to a safe location immediately.", "/[waitkey(64)]/[close()]"};
    String[] Loc0_2 = new String[]{"/[label(Warning)]", "The slide deck is currently locked and cannot be operated.", "/[waitkey(1)]/[clear()]", "In order to operate the slide deck, you will need a /[color(0x329bbe)]Disarm Key/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] Loc0_3 = new String[]{"/[label(Warning)]", "Releasing slide deck lock.", "/[waitkey(64)]/[close()]"};
    String[] kakuheki_1 = new String[]{"/[label()]", "The slide deck switch is here.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 8.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 8.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 8, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0690() {
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
                Runtime.jumpCF(66216, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.checkItem(10, 64) != 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        if (Runtime.getFlags(3095, 1) == 0) {
                            this.enemy2.kickEnepc(4, 1);
                            this.enemy3.kickEnepc(4, 1);
                            Runtime.setPlayerControl(false);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.Loc0_3, 0);
                            System.waitFor(this.win);
                            Runtime.setFlags(3095, 1, 1);
                            Runtime.setPlayerControl(true);
                            this.enemy2.kickEnepc(4, 0);
                            this.enemy3.kickEnepc(4, 0);
                            this.button_flg = 0;
                            return;
                        } else if (Runtime.getFlags(3020, 1) == 0) {
                            this.off();
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
                                    Runtime.setPlayerControl(false);
                                    Runtime.enable(65536);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(30);
                                    Sound.effectPlay(196741);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 305);
                                    this.win.print(this.Loc0_1, 0);
                                    System.waitFor(this.win);
                                    this.Y1.start(1, "Move");
                                    Sound.effectPlay(196743);
                                    Runtime.setFlags(3020, 1, 1);
                                    this.button_flg = 0;
                                }
                            }
                            Runtime.setPlayerControl(true);
                            this.on();
                            this.button_flg = 0;
                            return;
                        } else {
                            this.off();
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
                                    Runtime.setPlayerControl(false);
                                    Runtime.enable(65536);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(30);
                                    Sound.effectPlay(196741);
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 305);
                                    this.win.print(this.Loc0_1, 0);
                                    System.waitFor(this.win);
                                    this.Y1.start(1, "Move2");
                                    Sound.effectPlay(196743);
                                    Runtime.setFlags(3020, 1, 0);
                                    this.button_flg = 0;
                                }
                            }
                            this.on();
                            Runtime.setPlayerControl(true);
                            this.button_flg = 0;
                        }
                        return;
                    } else {
                        this.enemy2.kickEnepc(4, 1);
                        this.enemy3.kickEnepc(4, 1);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Loc0_2, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.enemy2.kickEnepc(4, 0);
                        this.enemy3.kickEnepc(4, 0);
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3022, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    System.println("合流するってマジでマジで！！");
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.setFlags(3022, 1, 1);
                    Runtime.jumpCF(66227, 1);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3208, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3208, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    } else if (Runtime.getFlags(3228, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    } else {
                        if (Runtime.getFlags(3288, 1) != 0) return;
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(56);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_03, 0);
                        System.waitFor(this.win);
                        this.doorC.SetDoorType('\u0004');
                        Runtime.setFlags(3288, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 3) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3023, 1) != 0) return;
                Runtime.setPlayerControl(false);
                Runtime.setFlags(3023, 1, 1);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(66237, 4);
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66236, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(66236, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(66256, 1);
                break;
            }
            case 4: {
                System.println("55_ON");
                Runtime.setFlags(3055, 1, 1);
                Runtime.jumpCF(660, 1);
                break;
            }
            case 5: {
                System.println("56_ON");
                Runtime.setFlags(3056, 1, 1);
                Runtime.jumpCF(66196, 2);
                break;
            }
        }
    }

    void init() {
        this.EF01 = new Effect(1417, 0);
        this.EF01.disp(true);
        this.EF01.setClip(true);
        this.E01 = new Effect(1402, -5.311f, 0.0f, -1.101f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E02 = new Effect(1401, -2.639f, -0.5f, 2.49f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E03 = new Effect(1402, 10.332f, 0.0f, -3.114f, 0.0f);
        this.E03.disp(true);
        this.E03.setClip(true);
        this.E04 = new Effect(1401, 3.915f, -0.5f, 7.548f, 0.0f);
        this.E04.disp(true);
        this.E04.setClip(true);
        this.E05 = new Effect(1402, -0.781f, 0.0f, 16.003f, 0.0f);
        this.E05.disp(true);
        this.E05.setClip(true);
        this.E06 = new Effect(1401, 2.198f, -0.5f, 25.497f, 0.0f);
        this.E06.disp(true);
        this.E06.setClip(true);
        this.E07 = new Effect(1402, -4.477f, 0.0f, 22.261f, 0.0f);
        this.E07.disp(true);
        this.E07.setClip(true);
        this.E08 = new Effect(1401, 9.336f, -0.5f, 16.045f, 0.0f);
        this.E08.disp(true);
        this.E08.setClip(true);
        this.E09 = new Effect(1402, 4.761f, 0.0f, 19.092f, 0.0f);
        this.E09.disp(true);
        this.E09.setClip(true);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3023, 1) == 1 && Runtime.getFlags(3022, 1) == 1 && Runtime.getFlags(3094, 1) == 0) {
            this.kidou = new Mapunits();
            this.kidou.mapUnit(11);
            this.kidou.start(4, null);
            this.kidou.start(1, "GOGO");
            Runtime.setFlags(3094, 1, 1);
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
        this.teiten1 = new Uwamono(28690, -9.4f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten1.SetBgmType('\u0001');
        this.teiten2 = new Uwamono(28690, -5.311f, 0.0f, -1.101f, 0.0f);
        this.teiten2.SetBgm(196638);
        this.teiten3 = new Uwamono(28690, -2.639f, -0.5f, 2.49f, 0.0f);
        this.teiten3.SetBgm(196639);
        this.teiten4 = new Uwamono(28690, 10.332f, 0.0f, -3.114f, 0.0f);
        this.teiten4.SetBgm(196638);
        this.teiten5 = new Uwamono(28690, 3.915f, -0.5f, 7.548f, 0.0f);
        this.teiten5.SetBgm(196639);
        this.teiten6 = new Uwamono(28690, -0.781f, 0.0f, 16.003f, 0.0f);
        this.teiten6.SetBgm(196638);
        this.teiten7 = new Uwamono(28690, 2.198f, -0.5f, 25.497f, 0.0f);
        this.teiten7.SetBgm(196639);
        this.teiten8 = new Uwamono(28690, -4.477f, 0.0f, 22.261f, 0.0f);
        this.teiten8.SetBgm(196638);
        this.teiten9 = new Uwamono(28690, 9.336f, -0.5f, 16.045f, 0.0f);
        this.teiten9.SetBgm(196639);
        this.teiten10 = new Uwamono(28690, 4.761f, 0.0f, 19.092f, 0.0f);
        this.teiten10.SetBgm(196638);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setFog(1, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(8, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(9, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(10, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(11, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFLockX(1, 9.0f);
        this.cam0.setCFAngle(2, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 9.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, 0.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, 9.0f, 6.0f, -15.5f, 40.0f, -90.0f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.03f, 0.03f);
        this.cam0.setCFAngle(11, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.03f, 0.03f);
        this.cam0.setCFLockX(11, 9.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16641, 3, -4.5f, 0.0f, 2.0f, 270.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray = new float[40];
        fArray[0] = -4.5f;
        fArray[2] = 2.0f;
        fArray[3] = 1.0f;
        fArray[4] = -4.5f;
        fArray[6] = -5.0f;
        fArray[7] = 2.0f;
        fArray[8] = -9.0f;
        fArray[10] = -5.0f;
        fArray[11] = 3.0f;
        fArray[12] = -9.0f;
        fArray[14] = -1.0f;
        fArray[15] = 4.0f;
        fArray[16] = -4.5f;
        fArray[18] = 2.0f;
        fArray[19] = 5.0f;
        fArray[20] = 9.0f;
        fArray[22] = 1.0f;
        fArray[23] = 6.0f;
        fArray[24] = 9.0f;
        fArray[26] = -12.0f;
        fArray[27] = 7.0f;
        fArray[28] = 1.5f;
        fArray[30] = -12.0f;
        fArray[31] = 8.0f;
        fArray[32] = 1.5f;
        fArray[34] = -18.25f;
        fArray[35] = 9.0f;
        fArray[36] = -6.5f;
        fArray[38] = -18.25f;
        fArray[39] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        float[] fArray3 = new float[30];
        fArray3[0] = -4.5f;
        fArray3[2] = 2.0f;
        fArray3[3] = -7.0f;
        fArray3[5] = 2.0f;
        fArray3[6] = -9.0f;
        fArray3[8] = 2.0f;
        fArray3[9] = -9.0f;
        fArray3[11] = -1.0f;
        fArray3[12] = -9.0f;
        fArray3[14] = -3.0f;
        fArray3[15] = -9.0f;
        fArray3[17] = -6.0f;
        fArray3[18] = -7.0f;
        fArray3[20] = -6.0f;
        fArray3[21] = -4.5f;
        fArray3[23] = -6.0f;
        fArray3[24] = -4.5f;
        fArray3[26] = -3.0f;
        fArray3[27] = -4.5f;
        fArray3[29] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setShadow(0, 0);
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(16642, 5, 2.0f, 0.0f, 18.0f, 270.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(2, 2, 2, 2);
        float[] fArray5 = new float[40];
        fArray5[0] = 2.0f;
        fArray5[2] = 18.0f;
        fArray5[3] = 1.0f;
        fArray5[6] = 18.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = -3.0f;
        fArray5[10] = 18.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = -6.0f;
        fArray5[14] = 18.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = -6.0f;
        fArray5[18] = 21.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = -6.0f;
        fArray5[22] = 24.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = -3.0f;
        fArray5[26] = 24.0f;
        fArray5[27] = 7.0f;
        fArray5[30] = 24.0f;
        fArray5[31] = 8.0f;
        fArray5[32] = 2.0f;
        fArray5[34] = 24.0f;
        fArray5[35] = 9.0f;
        fArray5[36] = 2.0f;
        fArray5[38] = 21.0f;
        fArray5[39] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(2, 1, 2, 5, fArray6);
        float[] fArray7 = new float[30];
        fArray7[0] = 2.0f;
        fArray7[2] = 18.0f;
        fArray7[5] = 18.0f;
        fArray7[6] = -3.0f;
        fArray7[8] = 18.0f;
        fArray7[9] = -6.0f;
        fArray7[11] = 18.0f;
        fArray7[12] = -6.0f;
        fArray7[14] = 21.0f;
        fArray7[15] = -6.0f;
        fArray7[17] = 24.0f;
        fArray7[18] = -3.0f;
        fArray7[20] = 24.0f;
        fArray7[23] = 24.0f;
        fArray7[24] = 2.0f;
        fArray7[26] = 24.0f;
        fArray7[27] = 2.0f;
        fArray7[29] = 21.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setShadow(0, 0);
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(16642, 5, 4.0f, 0.0f, 23.0f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(2, 2, 3, 3);
        float[] fArray9 = new float[16];
        fArray9[0] = 4.0f;
        fArray9[2] = 23.0f;
        fArray9[3] = 1.0f;
        fArray9[4] = 4.0f;
        fArray9[6] = 18.0f;
        fArray9[7] = 2.0f;
        fArray9[8] = 8.0f;
        fArray9[10] = 18.0f;
        fArray9[11] = 3.0f;
        fArray9[12] = 8.0f;
        fArray9[14] = 23.0f;
        fArray9[15] = -1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(2, 1, 3, 5, fArray10);
        float[] fArray11 = new float[12];
        fArray11[0] = 4.0f;
        fArray11[2] = 23.0f;
        fArray11[3] = 4.0f;
        fArray11[5] = 18.0f;
        fArray11[6] = 8.0f;
        fArray11[8] = 18.0f;
        fArray11[9] = 8.0f;
        fArray11[11] = 23.0f;
        float[] fArray12 = fArray11;
        this.enemy3.setShadow(0, 0);
        this.enemy3.setParams(fArray12);
        this.enemy3.kickEnepc(19, 1, 0, 385, 1);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 102);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 103);
        this.item03 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 104);
        this.item04 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 105);
        new Uwamono(28, 4, this.item01);
        new Uwamono(29, 0, this.item02);
        new Uwamono(30, 0, this.item03);
        new Uwamono(31, 0, this.item04);
        new Uwamono(32, 4);
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
        this.trap1 = new Uwamono(28673, -7.5f, 0.0f, 0.5f, 0.0f);
        this.trap2 = new Uwamono(28673, 0.5f, 0.0f, 22.5f, 0.0f);
        this.itembox = new Uwamono(28680, -6.0f, 0.0f, 13.0f, 90.0f, 120);
        this.itembox.SetSymbol(28725);
        this.itembox.SetCallNo(1);
        this.doorA = new Uwamono(45, 40, '\u0001');
        new Uwamono(60, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetDoorRange(0.75f);
        if (Runtime.getFlags(3023, 1) == 0) {
            this.doorB = new Uwamono(48, 40, '\u0001');
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB = new Uwamono(48, 40, '\u0001');
            this.doorB.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(3288, 1) == 0) {
            this.doorC = new Uwamono(58, 40, '\u0004');
            this.doorC.SetDoorType('\u0002');
            this.doorC.SetSize(0.25f, 2.25f, 1.5f);
        } else {
            this.doorC = new Uwamono(58, 40, '\u0004');
            this.doorC.SetDoorType('\u0002');
            this.doorC.SetSize(0.25f, 2.25f, 1.5f);
        }
        this.doorD = new Uwamono(46, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorD.SetDoorRange(1.4f);
        this.doorE = new Uwamono(44, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
        if (Runtime.getFlags(3020, 1) == 0) {
            this.player.setID(2);
        } else {
            this.player.setID(1);
        }
        this.Y1.setRotateY(180.0f);
        this.X2.setRotateY(180.0f);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3248, 1, 1);
                break;
            }
        }
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
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

        void GOGO() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST0690.this.player.mtn(28, 1, 1.0f, true);
            ST0690.this.win = Window.create();
            ST0690.this.win.setSize(4, 45);
            ST0690.this.win.setLocation(15, 305);
            if (Runtime.getLeader() == 1) {
                ST0690.this.win.print(ST0690.this.SSS, 0);
            } else if (Runtime.getLeader() == 2) {
                ST0690.this.win.print(ST0690.this.KKK, 0);
            } else if (Runtime.getLeader() == 3) {
                ST0690.this.win.print(ST0690.this.CCC, 0);
            } else if (Runtime.getLeader() == 6) {
                ST0690.this.win.print(ST0690.this.ZZZ, 0);
            } else {
                ST0690.this.win.print(ST0690.this.MMM, 0);
            }
            System.waitFor(ST0690.this.win);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }

        void Move() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST0690.this.X1.getTranslate();
                    ST0690.this.X1.setTranslate(ST0690.this.X1.px + 0.115f, ST0690.this.X1.py, ST0690.this.X1.pz);
                    ST0690.this.X2.getTranslate();
                    ST0690.this.X2.setTranslate(ST0690.this.X2.px - 0.115f, ST0690.this.X2.py, ST0690.this.X2.pz);
                }
                if (n >= 30 && n < 90) {
                    ST0690.this.Y1.getTranslate();
                    ST0690.this.Y1.setTranslate(ST0690.this.Y1.px, ST0690.this.Y1.py + 0.016666668f, ST0690.this.Y1.pz);
                    ST0690.this.Y2.getTranslate();
                    ST0690.this.Y2.setTranslate(ST0690.this.Y2.px, ST0690.this.Y2.py + 0.016666668f, ST0690.this.Y2.pz);
                }
                if (n == 91) break;
                ++n;
                System.sleep(1);
            }
            ST0690.this.on();
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0690.this.player.setID(1);
        }

        void Move2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST0690.this.Y1.getTranslate();
                    ST0690.this.Y1.setTranslate(ST0690.this.Y1.px, ST0690.this.Y1.py - 0.016666668f, ST0690.this.Y1.pz);
                    ST0690.this.Y2.getTranslate();
                    ST0690.this.Y2.setTranslate(ST0690.this.Y2.px, ST0690.this.Y2.py - 0.016666668f, ST0690.this.Y2.pz);
                }
                if (n >= 60 && n < 90) {
                    ST0690.this.X1.getTranslate();
                    ST0690.this.X1.setTranslate(ST0690.this.X1.px - 0.115f, ST0690.this.X1.py, ST0690.this.X1.pz);
                    ST0690.this.X2.getTranslate();
                    ST0690.this.X2.setTranslate(ST0690.this.X2.px + 0.115f, ST0690.this.X2.py, ST0690.this.X2.pz);
                }
                if (n == 91) break;
                ++n;
                System.sleep(1);
            }
            ST0690.this.on();
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0690.this.player.setID(2);
        }
    }
}

