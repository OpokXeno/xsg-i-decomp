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
import xeno.map.MC_DYU10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1801
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU10_PRJ {
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
    Enepc enemy6;
    Enepc EXP0;
    Enepc EXP1;
    Unit unit1;
    Unit DenDen;
    Unit Pon;
    Unit Step;
    Unit monitor1;
    Unit monitor2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
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
    int npc6btalked = 0;
    int npc7talked = 0;
    int npc7btalked = 0;
    int npc8talked = 0;
    int button1_flg = 0;
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
    String[] GUIDE_00 = new String[]{"Please enter your destination.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_01 = new String[]{"Going to the Residential Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_02 = new String[]{"Going to the Bridge.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_03 = new String[]{"Going to the Hangar.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_04 = new String[]{"Going to the Park.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_05 = new String[]{"Going to the Isolation Area.", "/[waitkey(64)]/[close()]"};
    String[] GUIDE_06 = new String[]{"Cancelled destination input.", "/[waitkey(64)]/[close()]"};
    String[] ENE_00 = new String[]{"Hmm...? What do you want?", "/[waitkey(1)]/[clear()]", "This is no place for people like you to be wandering around!", "/[waitkey(64)]/[close()]"};
    String[] ENE_01 = new String[]{"According to communications with HQ, it seems some people on this ship have escaped.", "/[waitkey(64)]/[close()]"};
    String[] ENE_02 = new String[]{"!!", "/[waitkey(1)]/[clear()]", "You guys must be!!", "/[waitkey(64)]/[close()]"};
    String[] ENE_03 = new String[]{"Hmm?! Who the hell are you?!", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "It seems to be locked from the inside.", "/[waitkey(1)]/[clear()]", "I'm pretty sure the Elsa has an elevator for cargo.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "Hmm? It's locked from the inside? Where is that other place that can be used to get inside the Elsa...?", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "So it's locked from the inside.", "/[waitkey(1)]/[clear()]", "I'm pretty sure the Captain and I once lowered some cargo from the supplies transfer hangar on the roof.", "/[waitkey(1)]/[clear()]", "We might be able to get inside the Elsa if we use that!", "/[waitkey(64)]/[close()]"};
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "It seems to be locked from the inside.", "/[waitkey(1)]/[clear()]", "Oh? I can see a deck on the other side that connects to the Elsa's roof!!", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "It's locked from the inside?!", "/[waitkey(1)]/[clear()]", "There has to be some way to get inside the Elsa!", "/[waitkey(64)]/[close()]"};
    String[] SYS_02 = new String[]{"Looks like you can get to the roof of the Elsa from here. Jump over?", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Dock Area Station\n", "Boarding area for direct flights to the Foundation'", "/[waitkey(64)]/[close()]"};

    ST1801() {
    }

    void Departure(int n) {
        Runtime.disable(524288);
        this.cam0.setMode(-1);
        this.Step.start(1, "Nobi");
        Sound.effectPlay(196741);
        this.EXP0.kickEnepc(4, 1);
        this.EXP0.kickEnepc(0, 1);
        System.sleep(20);
        this.doorB.DoorOpen();
        System.sleep(25);
        this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
        Runtime.enable(65536);
        this.player.mtn(2, 9, 1.0f, true);
        this.player.move(60, -13.0f, -1.0f, true);
        System.sleep(35);
        this.doorB.DoorClose();
        this.EXP0.kickEnepc(0, 2);
        System.sleep(20);
        this.Step.start(1, "Chijimi");
        System.sleep(25);
        this.EXP0.kickEnepc(3, 2, 45, 45, 1, 100);
        this.player.setTranslate(-100.0f, -0.0f, -1.0f);
        Runtime.disable(65536);
        int n2 = 0;
        int n3 = 90;
        float f = 20.0f / (float) n3 / (float) n3;
        while (n2 < n3) {
            float f2 = 2.0f * f * (float) n2;
            this.EXP0.getTranslate();
            this.EXP0.setTranslate(this.EXP0.px, this.EXP0.py, this.EXP0.pz + f2);
            if (n2 == n3 - 30) {
                this.fade.call(0);
            }
            ++n2;
            System.sleep(1);
        }
        Runtime.setPlayerControl(true);
        Runtime.jumpCF(n, 0);
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-9.155f, 5.199f, 16.156f);
        this.camEV.setRotate(-17.549f, 17.735f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, -9.155f, 5.199f, 16.156f, 120.0f, 5.006f, 4.271f, 8.578f, 240.0f, 0.858f, 3.343f, -1.0f};
        float[] fArray2 = new float[12];
        fArray2[0] = 1.0f;
        fArray2[1] = -17.549f;
        fArray2[2] = 17.735f;
        fArray2[4] = 120.0f;
        fArray2[5] = -11.869f;
        fArray2[6] = 53.867f;
        fArray2[8] = 240.0f;
        fArray2[9] = -6.189f;
        fArray2[10] = 90.0f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 240);
        this.camEV.rotateSPL(fArray3, 1, 3, 240);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.035f, 2.0f, 0.835f);
        this.camEV.setRotate(-1.87f, 0.0f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (this.button1_flg == 1) {
                        return;
                    }
                    this.button1_flg = 1;
                    this.off();
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.GUIDE_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Residential Area\nBridge\nHangar\nPark\nIsolation Area\nCancel");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(3072, 1, 1);
                            this.Departure(1841);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1821);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1861);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1851);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1831);
                            return;
                        }
                        case 5: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.GUIDE_06, 0);
                            System.waitFor(this.win);
                            this.button1_flg = 0;
                            Runtime.setPlayerControl(true);
                            System.sleep(1);
                            this.on();
                            return;
                        }
                        default: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.GUIDE_06, 0);
                            System.waitFor(this.win);
                            this.button1_flg = 0;
                            Runtime.setPlayerControl(true);
                            System.sleep(1);
                            this.on();
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px - 0.3f, this.player.py, this.player.pz);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_02, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            System.println("1");
                            Runtime.setPlayerControl(false);
                            this.cam0.setMode(-1);
                            float f = 0.0f;
                            float f2 = 0.0f;
                            this.player.getTranslate();
                            float f3 = (23.293f - this.player.px) / 18.0f;
                            float f4 = (-9.204f - this.player.pz) / 18.0f;
                            float f5 = (28.489f - this.player.px) / 60.0f;
                            float f6 = (0.0634f - this.player.py) / 60.0f;
                            float f7 = (-9.204f - this.player.pz) / 60.0f;
                            Runtime.enable(65536);
                            this.player.mtn(23, 1, 1.0f, true);
                            System.sleep(10);
                            while (true) {
                                this.player.getTranslate();
                                f2 = (-9.8f * f / 23.0f + 5.0f) * f / 23.0f + 1.0f;
                                if (f < 18.0f) {
                                    this.player.setTranslate(this.player.px + f3, f2, this.player.pz + f4);
                                } else if (f2 >= -0.6f) {
                                    this.player.setTranslate(this.player.px, f2, this.player.pz);
                                } else {
                                    this.player.setTranslate(this.player.px, -0.6f, this.player.pz);
                                }
                                if (f == 9.0f) {
                                    this.player.mtn(24, 1, 1.0f, true);
                                }
                                if (f == 50.0f) {
                                    this.player.mtn(4, 9, 1.0f, true);
                                }
                                if (f >= 51.0f && f < 110.0f) {
                                    this.player.setTranslate(this.player.px + f5, this.player.py + f6, this.player.pz + f7);
                                }
                                if (f == 111.0f) break;
                                f += 1.0f;
                                System.sleep(1);
                            }
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.jumpCF(1891, 0);
                            Runtime.setPlayerControl(true);
                            return;
                        }
                        default: {
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2 || n2 == 3 || n2 == 4) return;
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    System.println("PPPPPPPPPPPPPPPPPPPPPPP");
                    Runtime.setPlayerControl(false);
                    this.keikoku_1();
                    System.sleep(32);
                    System.sleep(1);
                    this.cam0.setMode(-1);
                    this.EV_Camera03();
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.PLAYER_01, 0);
                    System.waitFor(this.win);
                    this.cam0.setMode(0);
                    System.sleep(1);
                    this.monitor2.signal(0);
                    System.sleep(30);
                    this.keikoku_2();
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    System.sleep(1);
                    Runtime.disable(262144);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 6) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3161, 1) != 0) return;
                System.sleep(1);
                Runtime.enable(262144);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.mtn(28, 1, 1.0f, true);
                System.sleep(20);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                if (Runtime.getLeader() == 1) {
                    this.win.print(this.SHI_00, 0);
                } else if (Runtime.getLeader() == 6) {
                    this.win.print(this.ZIG_00, 0);
                } else if (Runtime.getLeader() == 3) {
                    this.win.print(this.CHA_00, 0);
                } else if (Runtime.getLeader() == 4) {
                    this.win.print(this.MOM_00, 0);
                } else {
                    this.win.print(this.JUN_00, 0);
                }
                System.waitFor(this.win);
                Runtime.disable(65536);
                System.sleep(30);
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(60, 10.0f, 5.0f, true);
                System.sleep(65);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                System.sleep(1);
                Runtime.disable(262144);
            }
        }
    }

    public void TalkNPC2(Enepc enepc) {
        if (this.npc1talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.ENE_00, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else if (this.npc1talked == 1) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.ENE_01, 0);
            System.waitFor(this.win);
            this.npc1talked = 2;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.ENE_02, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            this.enemy2.disableDTKFlag(65536);
            this.enemy3.disableDTKFlag(65536);
            this.enemy2.kickEnepc(7, 7);
            this.enemy3.kickEnepc(7, 7);
        }
    }

    public void TalkNPC4(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.ENE_03, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
        this.enemy4.disableDTKFlag(65536);
        this.enemy4.kickEnepc(7, 7);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                if (Runtime.getFlags(3161, 1) == 1) {
                    this.cam0.setMode(-1);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.jumpCF(521, 2);
                    break;
                }
                return;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -2.3f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, -2.3f, 2.56f, -14.0f, 0.0f);
        this.teiten2.SetBgm(196621);
        this.teiten3 = new Uwamono(28690, -11.0f, 0.0f, -1.0f, 0.0f);
        this.teiten3.SetBgm(196622);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 0.0f, 2.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 2.0f, 0.0f, 0.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 15.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, -10.0f, 0.0f, 14.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, -6.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 10.0f, 0.0f, 20.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 10.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 10.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, -10.0f, 0.0f, 14.0f, 35.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFPedestal(13, 11.814281f, 8.0f, 11.174469f, 35.0f, -25.325975f, 350.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(14, -28.0f, -20.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy2 = new Enepc();
        this.enemy2.init(18179, 7, 6.0f, 0.0f, 3.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(2, 2, 3, 3);
        float[] fArray = new float[24];
        fArray[0] = 6.0f;
        fArray[2] = 3.0f;
        fArray[3] = 1.0f;
        fArray[4] = 6.0f;
        fArray[6] = 4.5f;
        fArray[7] = 2.0f;
        fArray[8] = 5.0f;
        fArray[10] = 4.7f;
        fArray[11] = 3.0f;
        fArray[12] = 4.0f;
        fArray[14] = 4.7f;
        fArray[15] = 4.0f;
        fArray[16] = 3.0f;
        fArray[18] = 4.7f;
        fArray[19] = 5.0f;
        fArray[20] = 2.0f;
        fArray[22] = 4.7f;
        fArray[23] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy2.setParams(1, 12, 2, 7, fArray2);
        this.enemy2.talkto("TalkNPC2");
        this.enemy2.enableDTKFlag(65536);
        this.enemy2.setTogetherWith(3, 3, 3, 3);
        this.enemy3 = new Enepc();
        this.enemy3.init(18179, 7, 6.0f, 0.0f, 7.0f, 180.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(2, 2, 3, 3);
        float[] fArray3 = new float[24];
        fArray3[0] = 6.0f;
        fArray3[2] = 7.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = 6.0f;
        fArray3[6] = 5.5f;
        fArray3[7] = 2.0f;
        fArray3[8] = 5.0f;
        fArray3[10] = 5.2f;
        fArray3[11] = 3.0f;
        fArray3[12] = 4.0f;
        fArray3[14] = 5.2f;
        fArray3[15] = 4.0f;
        fArray3[16] = 3.0f;
        fArray3[18] = 5.2f;
        fArray3[19] = 5.0f;
        fArray3[20] = 2.0f;
        fArray3[22] = 5.2f;
        fArray3[23] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy3.setParams(1, 12, 3, 7, fArray4);
        this.enemy3.talkto("TalkNPC2");
        this.enemy3.enableDTKFlag(65536);
        this.enemy3.setTogetherWith(2, 2, 2, 2);
        this.enemy4 = new Enepc();
        this.enemy4.init(18179, 7, -1.5f, 0.0f, 7.0f, -90.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(2, 2, 3, 3);
        float[] fArray5 = new float[76];
        fArray5[0] = -1.5f;
        fArray5[2] = 7.0f;
        fArray5[3] = 1.0f;
        fArray5[4] = -1.5f;
        fArray5[6] = 6.0f;
        fArray5[7] = 2.0f;
        fArray5[8] = -1.5f;
        fArray5[10] = 5.0f;
        fArray5[11] = 3.0f;
        fArray5[12] = -1.5f;
        fArray5[14] = 4.0f;
        fArray5[15] = 4.0f;
        fArray5[16] = -1.5f;
        fArray5[18] = 3.0f;
        fArray5[19] = 5.0f;
        fArray5[20] = -1.5f;
        fArray5[22] = 2.0f;
        fArray5[23] = 6.0f;
        fArray5[24] = -2.0f;
        fArray5[26] = 1.0f;
        fArray5[27] = 7.0f;
        fArray5[28] = -3.0f;
        fArray5[31] = 8.0f;
        fArray5[32] = -3.0f;
        fArray5[34] = -1.0f;
        fArray5[35] = 9.0f;
        fArray5[36] = -3.0f;
        fArray5[38] = -2.0f;
        fArray5[39] = 10.0f;
        fArray5[40] = -3.0f;
        fArray5[42] = -3.0f;
        fArray5[43] = 11.0f;
        fArray5[44] = -3.0f;
        fArray5[46] = -4.0f;
        fArray5[47] = 12.0f;
        fArray5[48] = -3.0f;
        fArray5[50] = -5.0f;
        fArray5[51] = 13.0f;
        fArray5[52] = -4.0f;
        fArray5[54] = -6.0f;
        fArray5[55] = 14.0f;
        fArray5[56] = -5.0f;
        fArray5[58] = -6.0f;
        fArray5[59] = 15.0f;
        fArray5[60] = -6.0f;
        fArray5[62] = -6.0f;
        fArray5[63] = 16.0f;
        fArray5[64] = -7.0f;
        fArray5[66] = -7.0f;
        fArray5[67] = 17.0f;
        fArray5[68] = -7.0f;
        fArray5[70] = -8.0f;
        fArray5[71] = 18.0f;
        fArray5[72] = -7.0f;
        fArray5[74] = -9.0f;
        fArray5[75] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy4.setParams(1, 12, 4, 7, fArray6);
        this.enemy4.enableDTKFlag(65536);
        this.enemy4.talkto("TalkNPC4");
        this.enemy5 = new Enepc();
        this.enemy5.init(17666, 9, -6.0f, 0.0f, 9.0f, 180.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[12];
        fArray7[0] = -6.0f;
        fArray7[2] = 9.0f;
        fArray7[3] = 1.0f;
        fArray7[4] = -5.0f;
        fArray7[6] = 9.0f;
        fArray7[7] = 2.0f;
        fArray7[8] = -3.0f;
        fArray7[10] = 9.0f;
        fArray7[11] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy5.setParams(1, 9, 5, 9, fArray8);
        this.enemy5.kickEnepc(19, 1, 0, 430, 1);
        this.enemy5.kickEnepc(19, 2, 0, 710, 1);
        this.EXP0 = new Enepc();
        this.EXP0.init(20492, 5, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.id = 31;
        this.EXP0.setParams(0, 0, 31, 5);
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.EXP1 = new Enepc();
        this.EXP1.init(20621, 12, 4.5f, 4.5f, -21.6f, 90.0f);
        this.EXP1.id = 32;
        this.EXP1.setParams(0, 0, 32, 12);
        this.EXP1.setInvalidID(1);
        this.EXP1.dispRadar(false);
        if (Runtime.getFlags(3108, 1) != 1) {
            this.EXP1.setMotion(0, 2);
        } else {
            this.EXP1.setMotion(0, 3);
        }
        if (Runtime.getFlags(3161, 1) == 0) {
            this.doorA = new Uwamono(109, 40, '\u0001');
            this.doorA.SetDoorType('\u0001');
        } else {
            this.doorA = new Uwamono(109, 40, '\u0001');
            this.doorA.SetDoorType('\u0002');
        }
        this.doorB = new Uwamono(105, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        this.doorC = new Uwamono(108, 40, '\u0001');
        this.doorC.SetDoorType('\u0002');
        if (Runtime.getFlags(3069, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt");
        } else {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "algo");
        }
        this.monitor1 = new Unit();
        this.monitor1.init(24613, -7.013f, 2.25f, -3.0f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18016, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, -7.013f, 2.25f, -3.01f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18019, 0, 256, 130);
        this.monitor2.setArgs(2, 100, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Step = new Mapunits();
        this.Step.mapUnit(326);
        this.Step.start(4, null);
        this.Step.setTranslate(1.0f, -0.01f, 0.0f);
    }

    void keikoku_1() {
        int n = 0;
        while (true) {
            if (n == 0) {
                this.monitor1.signal(1);
                this.monitor1.setScale(0.0f, 0.0f, 0.0f);
            }
            if (n >= 0 && n < 30) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.033333335f * (float) n, 0.033333335f * (float) n, 0.033333335f * (float) n);
            }
            if (n == 32) break;
            ++n;
            System.sleep(1);
        }
        this.monitor2.signal(1);
    }

    void keikoku_2() {
        int n = 0;
        while (true) {
            if (n >= 0 && n < 30) {
                this.monitor1.getScale();
                this.monitor1.setScale(0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n), 0.033333335f * (float) (30 - n));
            }
            if (n == 30) break;
            ++n;
            System.sleep(1);
        }
        this.monitor1.setScale(0.0f, 0.0f, 0.0f);
        this.monitor1.signal(0);
    }

    void off() {
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        this.enemy5.kickEnepc(4, 1);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        this.enemy5.kickEnepc(1, 27);
    }

    void on() {
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        this.enemy5.kickEnepc(4, 0);
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

        void Chijimi() {
            int n = 0;
            ST1801.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1801.this.Step.setTranslate(ST1801.this.Step.px + 0.016666668f, ST1801.this.Step.py, ST1801.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Evt() {
            ST1801.this.off();
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1801.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1801.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1801.this.EV_Camera01();
            System.sleep(1);
            ST1801.this.EXP0.kickEnepc(4, 1);
            ST1801.this.win = Window.create();
            ST1801.this.win.setSize(4, 45);
            ST1801.this.win.setLocation(15, 305);
            ST1801.this.win.print("Dock");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1801.this.EXP0.getTranslate();
                ST1801.this.EXP0.setTranslate(ST1801.this.EXP0.px, ST1801.this.EXP0.py, ST1801.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1801.this.win.close();
            ST1801.this.player.setLocation(1, 2);
            ST1801.this.EXP0.kickEnepc(0, 1);
            ST1801.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1801.this.doorB.DoorOpen();
            System.sleep(15);
            ST1801.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1801.this.player.mtn(2, 9, 1.0f, true);
            ST1801.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1801.this.Step.start(1, "Chijimi");
            ST1801.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1801.this.EXP0.kickEnepc(4, 0);
            ST1801.this.EXP0.kickEnepc(4, 2);
            ST1801.this.doorB.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1801.this.cam0.setMode(0);
            Runtime.setFlags(3069, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
            ST1801.this.on();
        }

        void Nobi() {
            int n = 0;
            ST1801.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1801.this.Step.setTranslate(ST1801.this.Step.px - 0.016666668f, ST1801.this.Step.py, ST1801.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void algo() {
            ST1801.this.EXP0.kickEnepc(0, 2);
            int n = 0;
            while (n < 44) {
                System.sleep(1);
                ++n;
            }
            ST1801.this.EXP0.kickEnepc(4, 2);
        }
    }
}

