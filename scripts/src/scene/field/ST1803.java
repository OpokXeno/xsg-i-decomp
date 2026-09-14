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

class ST1803
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
    Enepc jr;
    Enepc allen;
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
    Unit RunRun;
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
    int npc7talked = 0;
    int npc8talked = 0;
    int npc11talked = 0;
    int npc22talked = 0;
    int npc33talked = 0;
    int npc44talked = 0;
    int npc55talked = 0;
    int npc66talked = 0;
    int npc77talked = 0;
    int npc88talked = 0;
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
    String[] ANO_00 = new String[]{"This ship is exclusively for direct flights to the Kukai Foundation.", "/[waitkey(1)]/[clear()]", "Would you like to go to the Foundation?", "/[waitkey(64)]/[close()]"};
    String[] ANO_01 = new String[]{"Understood. We wish you a pleasant stay...", "/[waitkey(64)]/[close()]"};
    String[] ANO_02 = new String[]{"Understood. We look forward to serving you again soon.", "/[waitkey(64)]/[close()]"};
    String[] ANO_03 = new String[]{"This ship is currently traveling through space neighboring the Kukai Foundation.", "/[waitkey(1)]/[clear()]", "Please wait a few moments to use the shuttle to the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] L01_00CJ = new String[]{"They're trying to restore their city again, even though it was destroyed so badly.", "/[waitkey(1)]/[clear()]", "That's the kind of thing that I like about humans.", "/[waitkey(64)]/[close()]"};
    String[] L01_01CJ = new String[]{"Little Master? I feel like I have become more human through these experiences.", "/[waitkey(64)]/[close()]"};
    String[] L01_00CN = new String[]{"Oh, you must be Little Master's friends.", "/[waitkey(1)]/[clear()]", "You people are such a mystery to me. You offer your help to Little Master, even when you have nothing to gain from it.", "/[waitkey(64)]/[close()]"};
    String[] L01_01CN = new String[]{"Somehow, I feel like I should help when I see another 100-Series in trouble too.", "/[waitkey(1)]/[clear()]", "This isn't a wrong way to feel, is it?", "/[waitkey(64)]/[close()]"};
    String[] L02_00CJ = new String[]{"Move it, move it! ", "Oh, hey, well if it isn't Little Master!", "/[waitkey(1)]/[clear()]", "What are you doing here?!", "/[waitkey(64)]/[close()]"};
    String[] L02_01CJ = new String[]{"You thought I didn't want to go back to the Foundation?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I can't just simply abandon the place where my son was born, now can I?!", "/[waitkey(64)]/[close()]"};
    String[] L02_00CN = new String[]{"What is it? You look like you want to say something!", "/[waitkey(64)]/[close()]"};
    String[] L02_01CN = new String[]{"You saw me cowering in fear when those monsters attacked?", "/[waitkey(1)]/[clear()]", "Oh bull!!", "/[waitkey(1)]/[clear()]", "I've never been afraid of anything in my entire life!", "/[waitkey(1)]/[clear()]", "You probably mistook me for some coward!!", "/[waitkey(64)]/[close()]"};
    String[] L03_00CJ = new String[]{"Hello, Captain.", "/[waitkey(1)]/[clear()]", "I am truly amazed at the vitality of the people of the Foundation!", "/[waitkey(1)]/[clear()]", "They've already started restorative work on their city after going through all that.", "/[waitkey(64)]/[close()]"};
    String[] L03_01CJ = new String[]{"I bet the Captain's unyielding spirit has an effect on the people of the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] L03_00CN = new String[]{"Please take good care of the Captain. He can be rather rash at times.", "/[waitkey(64)]/[close()]"};
    String[] L03_01CN = new String[]{"Me? Yes, I like the Captain very much!", "/[waitkey(1)]/[clear()]", "I don't know why, but watching him gives me strength.", "/[waitkey(64)]/[close()]"};
    String[] L04_00CJ = new String[]{"Captain, do you think my old friends will accept me?", "/[waitkey(1)]/[clear()]", "I've been labeled as a complete traitor.", "/[waitkey(64)]/[close()]"};
    String[] L04_01CJ = new String[]{"Yes, I know how hard it is to regain trust.\n", "/[waitkey(64)]/[close()]"};
    String[] L04_00CN = new String[]{"Ooooooh...\n", "I abandoned my friends when I ran from the Foundation, and now I'm embarrassed to go back.", "/[waitkey(64)]/[close()]"};
    String[] L04_01CN = new String[]{"Maybe I'll just become a crew member of this ship...", "/[waitkey(64)]/[close()]"};
    String[] L05_00CJ = new String[]{"Thanks to you, I've been able to confirm the safety of my family.", "/[waitkey(1)]/[clear()]", "Right now, my days are filled working to restore the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] L05_01CJ = new String[]{"It is all thanks to you, Little Master, that I even found a new job. I cannot begin to express my gratitude.", "/[waitkey(64)]/[close()]"};
    String[] L05_00CN = new String[]{"It was believed that the Foundation's traffic situation was perfect, but...", "/[waitkey(1)]/[clear()]", "from this point on we will plan out the city taking emergency evacuation routes into consideration.", "/[waitkey(64)]/[close()]"};
    String[] L05_01CN = new String[]{"My family?\n", "...\n", "I actually live alone with my cat.", "/[waitkey(64)]/[close()]"};
    String[] L06_00CJ = new String[]{"You look pretty quick to fight too.", "/[waitkey(1)]/[clear()]", "How about it? Wanna pair up with me?", "/[waitkey(64)]/[close()]"};
    String[] L06_01CJ = new String[]{"What? What am I doing?", "/[waitkey(1)]/[clear()]", "I might not look it, but I was a well-known bodyguard on the Foundation!", "/[waitkey(64)]/[close()]"};
    String[] L06_00CN = new String[]{"Guess I'll go back soon!", "/[waitkey(1)]/[clear()]", "Sure, I was shocked when those monsters showed up.", "/[waitkey(1)]/[clear()]", "But I can't let that keep me down forever!!", "/[waitkey(64)]/[close()]"};
    String[] L06_01CN = new String[]{"I'm really embarrassed at what you saw.", "/[waitkey(1)]/[clear()]", "Ah, well...at least I learned a little humility through that incident.", "/[waitkey(64)]/[close()]"};
    String[] L07_00CJ = new String[]{"Little Master, please do not push yourself too much.", "/[waitkey(64)]/[close()]"};
    String[] L07_01CJ = new String[]{"If something were to happen to you, Little Master, I...I...", "/[waitkey(64)]/[close()]"};
    String[] L07_00CN = new String[]{"Oh! You are Little Master's friends, aren't you?", "/[waitkey(1)]/[clear()]", "Please take good care of him.", "/[waitkey(64)]/[close()]"};
    String[] L07_01CN = new String[]{"If you let something happen to Little Master, I'm coming after you!", "/[waitkey(64)]/[close()]"};
    String[] L08_00CJ = new String[]{"Oh!! Hi Jr.! When are you going to play with me again?", "/[waitkey(64)]/[close()]"};
    String[] L08_01CJ = new String[]{"I wanna pilot a huge ship like this too!!", "/[waitkey(1)]/[clear()]", "But before that, I gotta learn how to ride a bicycle without training wheels...", "/[waitkey(64)]/[close()]"};
    String[] L08_00CN = new String[]{"This ship is fun, but I wanna get back to the Foundation.", "/[waitkey(1)]/[clear()]", "I wish Jr. would play with me a little more.", "/[waitkey(64)]/[close()]"};
    String[] L08_01CN = new String[]{"My house on the Foundation got all messed up.", "/[waitkey(1)]/[clear()]", "So my father told me to stay on this ship for a while!", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Dock Area Station\n", "Boarding area for direct flights to the Foundation'", "/[waitkey(64)]/[close()]"};

    ST1803() {
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

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.705f, 3.623f, -6.575f);
        this.camEV.setRotate(3.771f, -26.879f, 0.0f);
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
                            this.Departure(1843);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1823);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1863);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1853);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1833);
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
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1 || n2 == 2) return;
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(310, 1) == 1) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ANO_00, 0);
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
                                this.win.print(this.ANO_01, 0);
                                System.waitFor(this.win);
                                Sound.streamPlay(1195008, 48000);
                                this.RunRun.start(1, "Evt3");
                                return;
                            }
                            default: {
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 305);
                                this.win.print(this.ANO_02, 0);
                                System.waitFor(this.win);
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                    } else {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.ANO_03, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 4 || n2 != 5) return;
        switch (n) {
            case 100: {
                System.println("PPPPPPPPPPPPPPPPPPPPPPP");
                Runtime.setPlayerControl(false);
                this.keikoku_1();
                System.sleep(32);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.PLAYER_01, 0);
                System.waitFor(this.win);
                this.cam0.setMode(0);
                this.monitor2.signal(0);
                System.sleep(30);
                this.keikoku_2();
                System.sleep(30);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1talked == 0) {
                window.print(this.L01_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc1talked = 1;
            } else {
                window.print(this.L01_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc1talked = 0;
            }
        } else if (this.npc11talked == 0) {
            window.print(this.L01_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc11talked = 1;
        } else {
            window.print(this.L01_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc11talked = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2talked == 0) {
                window.print(this.L02_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc2talked = 1;
            } else {
                window.print(this.L02_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc2talked = 0;
            }
        } else if (this.npc22talked == 0) {
            window.print(this.L02_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc22talked = 1;
        } else {
            window.print(this.L02_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc22talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc3talked == 0) {
                window.print(this.L03_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc3talked = 1;
            } else {
                window.print(this.L03_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc3talked = 0;
            }
        } else if (this.npc33talked == 0) {
            window.print(this.L03_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc33talked = 1;
        } else {
            window.print(this.L03_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc33talked = 0;
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc4talked == 0) {
                window.print(this.L04_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc4talked = 1;
            } else {
                window.print(this.L04_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc4talked = 0;
            }
        } else if (this.npc44talked == 0) {
            window.print(this.L04_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc44talked = 1;
        } else {
            window.print(this.L04_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc44talked = 0;
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc5talked == 0) {
                window.print(this.L05_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc5talked = 1;
            } else {
                window.print(this.L05_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc5talked = 0;
            }
        } else if (this.npc55talked == 0) {
            window.print(this.L05_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc55talked = 1;
        } else {
            window.print(this.L05_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc55talked = 0;
        }
    }

    public void TalkNPC6(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc6talked == 0) {
                window.print(this.L06_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc6talked = 1;
            } else {
                window.print(this.L06_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc6talked = 0;
            }
        } else if (this.npc66talked == 0) {
            window.print(this.L06_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc66talked = 1;
        } else {
            window.print(this.L06_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc66talked = 0;
        }
    }

    public void TalkNPC7(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc7talked == 0) {
                window.print(this.L07_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc7talked = 1;
            } else {
                window.print(this.L07_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc7talked = 0;
            }
        } else if (this.npc77talked == 0) {
            window.print(this.L07_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc77talked = 1;
        } else {
            window.print(this.L07_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc77talked = 0;
        }
    }

    public void TalkNPC8(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc8talked == 0) {
                window.print(this.L08_00CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc8talked = 1;
            } else {
                window.print(this.L08_01CJ, 0);
                ST1803.waitPage(window, 64);
                this.npc8talked = 0;
            }
        } else if (this.npc88talked == 0) {
            window.print(this.L08_00CN, 0);
            ST1803.waitPage(window, 64);
            this.npc88talked = 1;
        } else {
            window.print(this.L08_01CN, 0);
            ST1803.waitPage(window, 64);
            this.npc88talked = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(521, 2);
                break;
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
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 3, 4, 0.9415f, 0.0f, -3.049f, 90.0f);
        this.npc2 = new NPC_NORMAL(1546, 12, 0, 10, 15, -3.14f, 0.0f, -4.769f, 0.0f);
        this.npc3 = new NPC_NORMAL(1025, 13, 0, 3, 4, -1.282f, 0.0f, -5.483f, -45.0f);
        this.npc4 = new NPC_NORMAL(1561, 14, 0, 3, 22, -5.039f, 0.0f, -6.385f, 225.0f);
        this.npc5 = new NPC_NORMAL(1540, 15, 0, 3, 15, -1.83f, 0.0f, -4.975f, 135.0f);
        this.npc6 = new NPC_NORMAL(1609, 16, 0, 3, 31, -3.567f, 2.5499f, -17.386f, 45.0f);
        this.npc7 = new NPC_NORMAL(783, 17, 0, 3, 17, 1.049f, 0.0f, 4.108f, -45.0f);
        this.npc8 = new NPC_NORMAL(1558, 18, 0, 10, 18, -7.05f, 0.0f, 7.448f, 0.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc3.talkto("TalkNPC3");
        this.npc4.talkto("TalkNPC4");
        this.npc5.talkto("TalkNPC5");
        this.npc6.talkto("TalkNPC6");
        this.npc7.talkto("TalkNPC7");
        this.npc8.talkto("TalkNPC8");
        this.npc1.disableDTKFlag(131074);
        this.npc2.disableDTKFlag(131074);
        this.npc3.disableDTKFlag(131074);
        this.npc3.setMotion(0, 9);
        this.npc4.disableDTKFlag(131075);
        this.npc4.setMotion(0, 2);
        this.npc5.disableDTKFlag(131074);
        this.npc5.setMotion(0, 7);
        this.npc6.disableDTKFlag(131074);
        this.npc7.disableDTKFlag(131074);
        this.npc8.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(8);
        this.npc2.enableDTKFlag(8);
        this.npc3.enableDTKFlag(8);
        this.npc4.enableDTKFlag(8);
        this.npc5.enableDTKFlag(8);
        this.npc6.enableDTKFlag(8);
        this.npc7.enableDTKFlag(8);
        this.npc8.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc2.enableDTKFlag(4);
        this.npc3.enableDTKFlag(4);
        this.npc4.enableDTKFlag(4);
        this.npc5.enableDTKFlag(4);
        this.npc6.enableDTKFlag(4);
        this.npc7.enableDTKFlag(4);
        this.npc8.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc4.setInvalidID(1);
        this.npc6.setInvalidID(1);
        this.npc8.kickEnepc(19, 1, 0, 420, 1);
        this.npc8.kickEnepc(19, 2, 0, 700, 1);
        this.EXP0 = new Enepc();
        this.EXP0.init(20492, 5, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.id = 31;
        this.EXP0.setParams(0, 0, 31, 5);
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.EXP1 = new Enepc();
        this.EXP1.init(20621, 30, 4.5f, 4.5f, -21.6f, 90.0f);
        this.EXP1.id = 32;
        this.EXP1.setParams(0, 0, 32, 30);
        this.EXP1.setInvalidID(1);
        this.EXP1.dispRadar(false);
        if (Runtime.getFlags(3108, 1) != 1) {
            this.EXP1.setMotion(0, 2);
        } else {
            this.EXP1.setMotion(0, 3);
        }
        this.doorA = new Uwamono(109, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(105, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        this.doorC = new Uwamono(108, 40, '\u0001');
        this.doorC.SetDoorType('\u0001');
        if (Runtime.getFlags(3069, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt");
        }
        if (Runtime.getFlags(3108, 1) == 1) {
            this.DenDen = new Mapunits();
            this.DenDen.mapUnit(1);
            this.DenDen.start(4, null);
            this.DenDen.start(1, "Evt2");
        }
        this.RunRun = new Mapunits();
        this.RunRun.mapUnit(11);
        this.RunRun.start(4, null);
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
            ST1803.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1803.this.Step.setTranslate(ST1803.this.Step.px + 0.016666668f, ST1803.this.Step.py, ST1803.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Down() {
            int n = 0;
            ST1803.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 120) {
                    ST1803.this.EXP1.setTranslate(ST1803.this.EXP1.px, ST1803.this.EXP1.py - 0.045833334f, ST1803.this.EXP1.pz);
                }
                if (n == 149) {
                    ST1803.this.player.setTranslate(6.0f, 4.95f, -22.0f);
                }
                if (n == 150) {
                    ST1803.this.EXP1.kickEnepc(0, 1);
                }
                if (n == 195) break;
                ++n;
                System.sleep(1);
            }
            ST1803.this.EXP1.kickEnepc(1, 2);
        }

        void Evt() {
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1803.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1803.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1803.this.EV_Camera01();
            System.sleep(1);
            ST1803.this.EXP0.kickEnepc(4, 1);
            ST1803.this.win = Window.create();
            ST1803.this.win.setSize(4, 45);
            ST1803.this.win.setLocation(15, 305);
            ST1803.this.win.print("Dock");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1803.this.EXP0.getTranslate();
                ST1803.this.EXP0.setTranslate(ST1803.this.EXP0.px, ST1803.this.EXP0.py, ST1803.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1803.this.win.close();
            ST1803.this.player.setLocation(1, 2);
            ST1803.this.EXP0.kickEnepc(0, 1);
            ST1803.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1803.this.doorB.DoorOpen();
            System.sleep(15);
            ST1803.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1803.this.player.mtn(2, 9, 1.0f, true);
            ST1803.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1803.this.Step.start(1, "Chijimi");
            ST1803.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1803.this.EXP0.kickEnepc(4, 0);
            ST1803.this.EXP0.kickEnepc(4, 2);
            ST1803.this.doorB.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1803.this.cam0.setMode(0);
            Runtime.setFlags(3069, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt2() {
            Runtime.disable(524288);
            ST1803.this.player.setTranslate(100.0f, 100.0f, 100.0f);
            Runtime.setPlayerControl(false);
            ST1803.this.EXP1.setTranslate(4.5f, 10.0f, -21.6f);
            ST1803.this.cam0.setMode(-1);
            ST1803.this.EV_Camera02();
            Sound.streamPlay(1195009, 48000);
            System.sleep(30);
            ST1803.this.EXP1.kickEnepc(4, 1);
            ST1803.this.EXP1.getTranslate();
            this.Down();
            ST1803.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1803.this.player.mtn(2, 9, 1.0f, true);
            ST1803.this.player.move(60, 6.0f, -20.0f, true);
            System.sleep(60);
            Runtime.disable(65536);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt3() {
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1803.this.cam0.setMode(-1);
            ST1803.this.EV_Camera02();
            System.sleep(30);
            ST1803.this.EXP1.kickEnepc(4, 1);
            ST1803.this.EXP1.getTranslate();
            this.Up();
            System.sleep(120);
            Runtime.disable(65536);
            ST1803.this.cam0.setMode(0);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1803.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST1803.this.Step.setTranslate(ST1803.this.Step.px - 0.016666668f, ST1803.this.Step.py, ST1803.this.Step.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void Up() {
            int n = 0;
            ST1803.this.Step.getTranslate();
            while (true) {
                if (n == 0) {
                    ST1803.this.EXP1.kickEnepc(0, 0);
                }
                if (n == 45) {
                    ST1803.this.EXP1.kickEnepc(1, 3);
                }
                if (n == 75) {
                    ST1803.this.player.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n >= 75 && n < 195) {
                    ST1803.this.EXP1.setTranslate(ST1803.this.EXP1.px, ST1803.this.EXP1.py + 0.083333336f, ST1803.this.EXP1.pz);
                }
                if (n == 195) break;
                ++n;
                System.sleep(1);
            }
            ST1803.this.EXP1.kickEnepc(4, 0);
            Runtime.setPlayerControl(true);
            ST1803.this.fade.call(0);
            System.sleep(30);
            Runtime.setFlags(7138, 1, 1);
            Runtime.jumpCF(2160, 2);
        }
    }
}

