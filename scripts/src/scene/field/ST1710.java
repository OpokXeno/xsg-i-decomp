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
import xeno.map.MC_DYU01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1710
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU01_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc JUN;
    Enepc ZIG;
    Enepc CHA;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit ele0;
    Unit DonDon;
    Unit Star_1;
    Unit Star_2;
    Unit sphere1;
    Unit sphere2;
    Unit ring_1a;
    Unit ring_1b;
    Unit ring_1c;
    Unit ring_2a;
    Unit ring_2b;
    Unit ring_2c;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1_btalked = 0;
    int npc2talked = 0;
    int npc2_btalked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc4_btalked = 0;
    int npc5talked = 0;
    int npc5_btalked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc7_btalked = 0;
    int npc8talked = 0;
    int npc8_btalked = 0;
    int npc9talked = 0;
    int npc9_btalked = 0;
    int npc10talked = 0;
    int npc11talked = 0;
    int npc11_btalked = 0;
    int npc22talked = 0;
    int npc44talked = 0;
    int npc55talked = 0;
    int npc88talked = 0;
    int npc99talked = 0;
    int JUNtalked = 0;
    int CHAtalked = 0;
    int button_flg = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    MAPUnit space;
    MAPUnit kuk;
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
    String[] Info_00 = new String[]{"Go to the lower level?", "/[waitkey(64)]/[close()]"};
    String[] Ha_00 = new String[]{"We are the early mass production models that were tuned exclusively for the Durandal.", "/[waitkey(1)]/[clear()]", "Many of our sisters work here on the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] Ha_01 = new String[]{"Oh, Little Master!", "/[waitkey(1)]/[clear()]", "Little Master, did you see everything that I just did? I worked hard to provide backup so that you would praise me, Little Master.", "/[waitkey(64)]/[close()]"};
    String[] Ha_02 = new String[]{"Little Master, please designate me for the next operation as well.", "/[waitkey(64)]/[close()]"};
    String[] Hb_00 = new String[]{"Currently, the people in command of this ship are Little Master and Director Gaignun's secretaries, Shelley and Mary.", "/[waitkey(64)]/[close()]"};
    String[] Hb_01 = new String[]{"Little Master, you are technically the Captain of this ship, so please stay in the Captain's chair and look imposing. Leave the onerous tasks to us.", "/[waitkey(64)]/[close()]"};
    String[] Hd_00 = new String[]{"What on earth you doin'? Don't get in the way of my work!", "/[waitkey(64)]/[close()]"};
    String[] Hd_01 = new String[]{"Mary taught me all sorts of things.", "/[waitkey(1)]/[clear()]", "But while she taught me, I sorta picked up her speech pattern too!", "/[waitkey(64)]/[close()]"};
    String[] Hd_02 = new String[]{"Oh, it's you, Little Master. Don't scare me like that, ya hear?", "/[waitkey(64)]/[close()]"};
    String[] Hd_03 = new String[]{"Say, Little Master? You ever had a dish called gumbo? Mary told me that it's mighty delicious.", "/[waitkey(64)]/[close()]"};
    String[] Hd_04 = new String[]{"Gumbo, I wonder what it's like? Suppose it's make out of gum?", "/[waitkey(64)]/[close()]"};
    String[] He_00 = new String[]{"We are currently en route to the Miltian star system.", "/[waitkey(1)]/[clear()]", "It will take a short while to get within the column's field of effectiveness, so please get some rest.", "/[waitkey(64)]/[close()]"};
    String[] He_01 = new String[]{"Oh, Little Master. Please tell us more interesting stories again sometime.", "/[waitkey(64)]/[close()]"};
    String[] He_02 = new String[]{"I love Little Master's stories!", "/[waitkey(64)]/[close()]"};
    String[] Hh_00 = new String[]{"No, please don't talk to me.", "/[waitkey(1)]/[clear()]", "I'm behind in my work. So if I get caught slacking off, my elder sisters will scold me.", "/[waitkey(64)]/[close()]"};
    String[] Hh_01 = new String[]{"Little Master, I got scolded again!", "/[waitkey(1)]/[clear()]", "Big sis Mary called me a banana brain!", "/[waitkey(64)]/[close()]"};
    String[] Hh_02 = new String[]{"Little Master, what kind of brain does a banana have?", "/[waitkey(64)]/[close()]"};
    String[] Hi_00 = new String[]{"Welcome to the Durandal!\n", "/[waitkey(1)]/[clear()]", "Due to the special nature of this ship's missions, numerous cutting-edge technologies have been used here.", "/[waitkey(64)]/[close()]"};
    String[] Hi_01 = new String[]{"But much of it is top secret, so I can't really go into details.", "/[waitkey(64)]/[close()]"};
    String[] Hi_02 = new String[]{"Welcome to the Durandal! Oh, it's you, Little Master.", "/[waitkey(64)]/[close()]"};
    String[] shelley_00 = new String[]{"/[label(Shelley)]", "You look as though you cannot fathom how we obtained this many 100-Series Observational Units.", "/[waitkey(64)]/[close()]"};
    String[] shelley_01 = new String[]{"/[label(Shelley)]", "The CEO of Vector is among our principal investors.", "/[waitkey(1)]/[clear()]", "These 100-Series are assigned here by the military as prototypes. ", "They are put to work on this ship and at the same time, they are put through various tests.", "/[waitkey(64)]/[close()]"};
    String[] shelley_02 = new String[]{"/[label(Shelley)]", "Little Master, why not leave this to Mary and me, and go get some rest?", "/[waitkey(64)]/[close()]"};
    String[] shelley_03 = new String[]{"/[label(Shelley)]", "You must be tired from all the fighting.", "/[waitkey(64)]/[close()]"};
    String[] mary_01 = new String[]{"/[label(Mary)]", "Seems like you've all seen some tough times, too.", "/[waitkey(1)]/[clear()]", "It'll still be a while before we get to our destination, so you all may as well take your time and look around some.", "/[waitkey(64)]/[close()]"};
    String[] mary_02 = new String[]{"/[label(Mary)]", "What is the matter, Little Master? Are you that bored?", "/[waitkey(1)]/[clear()]", "Well, then if you organize these investigation reports, it'll be a huge help.", "/[waitkey(64)]/[close()]"};
    String[] mary_03 = new String[]{"/[label(Mary)]", "Just kidding.", "/[waitkey(1)]/[clear()]", "We'll organize the reports, so don't you worry about it.", "/[waitkey(64)]/[close()]"};
    String[] T01_00 = new String[]{"T-thank you very much!", "/[waitkey(1)]/[clear()]", "Thanks to you all, we have been safely freed.", "/[waitkey(1)]/[clear()]", "Everyone from the other departments are safe too, so please do not worry.", "/[waitkey(64)]/[close()]"};
    String[] T02_00 = new String[]{"I hear the U-TIC Organization has been behind these series of confrontations.", "/[waitkey(1)]/[clear()]", "If that's true, they will not be easy opponents. I'm worried about the position of Second Miltia in the Federation.", "/[waitkey(64)]/[close()]"};
    String[] T03_00 = new String[]{"Oh thank heavens you guys are all right too. I was mighty worried.", "/[waitkey(1)]/[clear()]", "Really, the guys from the U-TIC Organization sure do some nasty things.", "/[waitkey(64)]/[close()]"};
    String[] T04_00 = new String[]{"What? Allen? The engineer from Vector that you were with, correct?", "/[waitkey(1)]/[clear()]", "I'm sorry, but he hasn't come by here.", "/[waitkey(64)]/[close()]"};
    String[] T04_01 = new String[]{"Please wait a moment...", "/[waitkey(1)]/[clear()]", "It seems he boarded the shuttle just a while ago. It looks like he went down to the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] T05_00 = new String[]{"I'm so sorry, I'm so sorry. People from the Federation probably got mad because I kept messing up.", "/[waitkey(64)]/[close()]"};
    String[] T05_01 = new String[]{"What? That isn't it?\n", "/[waitkey(1)]/[clear()]", "My failures have nothing to do with it? Then my elder sisters won't scold me even if I make mistakes?!", "/[waitkey(64)]/[close()]"};
    String[] T06_00 = new String[]{"What is their problem?!\n", "/[waitkey(1)]/[clear()]", "I can't believe they'd dare to call a cute Realian like me a traitor!", "/[waitkey(1)]/[clear()]", "The Federation must be a collection of idiots!", "/[waitkey(64)]/[close()]"};
    String[] T06_01 = new String[]{"Oh, please excuse me. I'm not used to controlling my emotions yet.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "I wonder if MOMO is okay?", "/[waitkey(1)]/[clear()]", "It might not have been a good idea to talk about Mizrahi in front of her.", "/[waitkey(64)]/[close()]"};
    String[] JUN_01 = new String[]{"/[label(Jr.)]", "It'll be a while longer before we get to the Foundation.", "/[waitkey(1)]/[clear()]", "I had a room prepared in the Residential Area, so go get some rest.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "I see, so MOMO was at the park.", "/[waitkey(1)]/[clear()]", "She probably has something on her mind,", "/[waitkey(1)]/[clear()]", "but it is not for others to interfere in. Perhaps it wouldn't be a bad idea to leave her alone?", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "Oh...MOMO said that...?", "/[waitkey(1)]/[clear()]", "Her father must be more precious and more important to her than anything else.", "/[waitkey(1)]/[clear()]", "We better be careful too. Make sure we don't say something inadvertently that could hurt her.", "/[waitkey(64)]/[close()]"};
    String[] CHA_01 = new String[]{"/[label(chaos)]", "We'll be arriving at the Foundation soon.", "/[waitkey(1)]/[clear()]", "I guess I'm not going to get a chance to relax this time either.", "/[waitkey(64)]/[close()]"};

    ST1710() {
    }

    void EV_Camera00() {
        float[] fArray = new float[8];
        fArray[1] = 5.112f;
        fArray[2] = 7.193f;
        fArray[3] = 15.332f;
        fArray[4] = 240.0f;
        fArray[5] = 5.112f;
        fArray[6] = 7.193f;
        fArray[7] = 15.332f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[1] = -25.902f;
        fArray3[2] = 21.312f;
        fArray3[4] = 240.0f;
        fArray3[5] = -51.242f;
        fArray3[6] = 83.771f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 3, 240);
        this.camEV.rotateSPL(fArray4, 1, 3, 240);
        this.camEV.setFov(45.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 7.893f, 5.113f, 4.012f, 200.0f, 7.893f, 3.065f, 4.012f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -12.843f;
        fArray2[2] = -211.314f;
        fArray2[4] = 200.0f;
        fArray2[5] = -12.843f;
        fArray2[6] = -211.314f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 200);
        this.camEV.rotateSPL(fArray3, 1, 3, 200);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3068, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            if (Runtime.getFlags(310, 1) == 1) {
                                this.kuk.setTranslate(0.0f, 0.0f, -50.0f);
                            }
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(20, 0.0f, 16.5f, true);
                            System.sleep(25);
                            this.player.rotY(15, 180.0f, true);
                            System.sleep(20);
                            Runtime.disable(65536);
                            this.ele0.setArgs(12, -5.0f);
                            Sound.effectPlay(196745);
                            System.sleep(200);
                            Runtime.setFlags(3067, 1, 1);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.jumpCF(1820, 0);
                            return;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(60, 0.0f, 13.0f, true);
                    System.sleep(60);
                    Runtime.setFlags(3068, 1, 0);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 1) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(346, 1) == 0) {
                    if (Runtime.getLeader() != 5) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        this.npc8.kickEnepc(4, 1);
                        this.npc8.kickEnepc(9, 100);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Hh_00, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    }
                    if (this.npc8_btalked == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.setPlayerControl(false);
                        this.npc8.kickEnepc(4, 1);
                        this.npc8.kickEnepc(9, 100);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Hh_01, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        this.npc8_btalked = 1;
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    }
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.npc8.kickEnepc(4, 1);
                    this.npc8.kickEnepc(9, 100);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Hh_02, 0);
                    System.waitFor(this.win);
                    this.npc8.kickEnepc(4, 0);
                    this.npc8_btalked = 0;
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
                if (this.npc88talked == 0) {
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.npc8.kickEnepc(4, 1);
                    this.npc8.kickEnepc(9, 100);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.T05_00, 0);
                    System.waitFor(this.win);
                    this.npc88talked = 1;
                    this.npc8.kickEnepc(4, 0);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
                if (this.button_flg == 1) {
                    return;
                }
                this.button_flg = 1;
                Runtime.setPlayerControl(false);
                this.npc8.kickEnepc(4, 1);
                this.npc8.kickEnepc(9, 100);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.T05_01, 0);
                System.waitFor(this.win);
                this.npc88talked = 0;
                this.npc8.kickEnepc(4, 0);
                Runtime.setPlayerControl(true);
                this.button_flg = 0;
            }
        }
    }

    public void TalkCHA(Enepc enepc) {
        if (this.CHAtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.CHA_00, 0);
            System.waitFor(this.win);
            this.CHAtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.CHA_01, 0);
            System.waitFor(this.win);
            this.CHAtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkJUN(Enepc enepc) {
        if (this.JUNtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.JUN_00, 0);
            System.waitFor(this.win);
            this.JUNtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.JUN_01, 0);
            System.waitFor(this.win);
            this.JUNtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1_btalked == 0) {
                window.print(this.Ha_01, 0);
                System.waitFor(window);
                this.npc1_btalked = 1;
            } else {
                window.print(this.Ha_02, 0);
                System.waitFor(window);
                this.npc1_btalked = 0;
            }
        } else {
            window.print(this.Ha_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC11(Enepc enepc) {
        if (Runtime.getLeader() == 5) {
            if (this.npc11_btalked == 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.mary_02, 0);
                System.waitFor(this.win);
                this.npc11_btalked = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.mary_03, 0);
                System.waitFor(this.win);
                this.npc11_btalked = 0;
                Runtime.setPlayerControl(true);
            }
        } else if (this.npc11talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.mary_01, 0);
            System.waitFor(this.win);
            this.npc11talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.mary_01, 0);
            System.waitFor(this.win);
            this.npc11talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        window.print(this.T01_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.Hb_01, 0);
            System.waitFor(window);
        } else {
            window.print(this.Hb_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        window.print(this.T02_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc4_btalked == 0) {
                window.print(this.Hd_02, 0);
                System.waitFor(window);
                this.npc4_btalked = 1;
            } else if (this.npc4_btalked == 1) {
                window.print(this.Hd_03, 0);
                System.waitFor(window);
                this.npc4_btalked = 2;
            } else {
                window.print(this.Hd_04, 0);
                System.waitFor(window);
                this.npc4_btalked = 1;
            }
        } else if (this.npc4talked == 0) {
            window.print(this.Hd_00, 0);
            System.waitFor(window);
            this.npc4talked = 1;
        } else {
            window.print(this.Hd_01, 0);
            System.waitFor(window);
            this.npc4talked = 0;
        }
    }

    public void TalkNPC4a(Enepc enepc, Window window) {
        window.print(this.T03_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc5_btalked == 0) {
                window.print(this.He_01, 0);
                System.waitFor(window);
                this.npc5_btalked = 1;
            } else {
                window.print(this.He_02, 0);
                System.waitFor(window);
                this.npc5_btalked = 0;
            }
        } else {
            window.print(this.He_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5a(Enepc enepc, Window window) {
        if (this.npc55talked == 0) {
            window.print(this.T04_00, 0);
            System.waitFor(window);
            this.npc55talked = 1;
        } else {
            window.print(this.T04_01, 0);
            System.waitFor(window);
            this.npc55talked = 0;
        }
    }

    public void TalkNPC7(Enepc enepc) {
        if (Runtime.getLeader() == 5) {
            if (this.npc7_btalked == 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.shelley_02, 0);
                System.waitFor(this.win);
                this.npc7_btalked = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.shelley_03, 0);
                System.waitFor(this.win);
                this.npc7_btalked = 0;
                Runtime.setPlayerControl(true);
            }
        } else if (this.npc7talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.shelley_00, 0);
            System.waitFor(this.win);
            this.npc7talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.shelley_01, 0);
            System.waitFor(this.win);
            this.npc7talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC9(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.Hi_02, 0);
            System.waitFor(window);
        } else if (this.npc9talked == 0) {
            window.print(this.Hi_00, 0);
            this.npc9talked = 1;
            System.waitFor(window);
        } else {
            window.print(this.Hi_01, 0);
            this.npc9talked = 0;
            System.waitFor(window);
        }
    }

    public void TalkNPC9a(Enepc enepc, Window window) {
        if (this.npc99talked == 0) {
            window.print(this.T06_00, 0);
            System.waitFor(window);
            this.npc99talked = 1;
        } else {
            window.print(this.T06_01, 0);
            System.waitFor(window);
            this.npc99talked = 0;
        }
    }

    public void TalkZIG(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.ZIG_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            default:
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 5.0f, 0.0f, 15.5f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, -0.5f, 1.0f, 6.5f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -3.0f, -2.0f, 2.3f, 0.0f);
        this.teiten3.SetBgm(196609);
        this.teiten4 = new Uwamono(28690, 3.0f, -2.0f, 0.0f, 0.0f);
        this.teiten4.SetBgm(196609);
        this.teiten5 = new Uwamono(28690, -3.0f, -2.0f, -3.5f, 0.0f);
        this.teiten5.SetBgm(196609);
        Stage.setVisible(-1, true);
        this.ring_1a = new Mapunits();
        this.ring_1b = new Mapunits();
        this.ring_1c = new Mapunits();
        this.ring_1a.mapUnit(120);
        this.ring_1b.mapUnit(121);
        this.ring_1c.mapUnit(122);
        this.ring_1a.start(4, null);
        this.ring_1b.start(4, null);
        this.ring_1c.start(4, null);
        this.ring_1a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2a = new Mapunits();
        this.ring_2b = new Mapunits();
        this.ring_2c = new Mapunits();
        this.ring_2a.mapUnit(123);
        this.ring_2b.mapUnit(124);
        this.ring_2c.mapUnit(125);
        this.ring_2a.start(4, null);
        this.ring_2b.start(4, null);
        this.ring_2c.start(4, null);
        this.ring_2a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
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
        this.light.setColor(0, 0.05f, 0.05f, 0.05f);
        this.light.setColor(1, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(1, -1.0f, 1.0f, 3.0f);
        this.light.setColor(2, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(2, 2.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.1f, 0.1f, 0.1f);
        this.light.setDirection2(3, 1.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 10.0f, 0.0f, 7.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 350.0f, 0.0f, 7.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 6.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 0, 4, -2.609f, -2.213f, -3.609f, 225.0f);
        this.npc2 = new NPC_NORMAL(1028, 12, 0, 0, 4, 2.7f, -1.763f, -3.7f, 135.0f);
        this.npc4 = new NPC_NORMAL(1028, 14, 0, 0, 4, 2.609f, -1.763f, -0.609f, 135.0f);
        this.npc5 = new NPC_NORMAL(1028, 15, 0, 0, 4, -2.609f, -1.763f, 2.391f, 225.0f);
        this.npc8 = new NPC_NORMAL(1028, 18, 0, 3, 4, -5.625f, -1.5f, 5.219f, 180.0f);
        this.npc9 = new NPC_NORMAL(1028, 19, 0, 0, 4, 5.313f, 0.5f, 13.688f, 135.0f);
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
            this.npc4.talkto("TalkNPC4");
            this.npc5.talkto("TalkNPC5");
            this.npc8.talkto("TalkNPC8");
            this.npc9.talkto("TalkNPC9");
        } else {
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
            this.npc4.talkto("TalkNPC4a");
            this.npc5.talkto("TalkNPC5a");
            this.npc8.talkto("TalkNPC8a");
            this.npc9.talkto("TalkNPC9a");
        }
        this.npc1.disableDTKFlag(131075);
        this.npc2.disableDTKFlag(131075);
        this.npc4.disableDTKFlag(131075);
        this.npc5.disableDTKFlag(131075);
        this.npc8.disableDTKFlag(131075);
        this.npc9.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(12);
        this.npc2.enableDTKFlag(12);
        this.npc4.enableDTKFlag(12);
        this.npc5.enableDTKFlag(12);
        this.npc8.enableDTKFlag(12);
        this.npc9.enableDTKFlag(12);
        this.npc1.setMotion(0, 1);
        this.npc2.setMotion(0, 2);
        this.npc4.setMotion(0, 4);
        this.npc5.setMotion(0, 3);
        this.npc8.setMotion(0, 3);
        this.npc9.setMotion(0, 4);
        this.npc1.setInvalidID(1);
        this.npc2.setInvalidID(1);
        this.npc4.setInvalidID(1);
        this.npc5.setInvalidID(1);
        this.npc8.setInvalidID(1);
        this.npc9.setInvalidID(1);
        this.npc1.setShadow(0, 0);
        this.npc2.setShadow(0, 0);
        this.npc4.setShadow(0, 0);
        this.npc5.setShadow(0, 0);
        this.npc8.setShadow(0, 0);
        this.npc9.setShadow(0, 0);
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc7 = new NPC_NORMAL(289, 17, 0, 0, 4, -0.5f, 1.5f, 7.844f, 180.0f);
            this.npc11 = new NPC_NORMAL(288, 21, 0, 2, 7, -1.145f, 0.0f, 12.768f, 40.0f);
            this.npc7.talkto("TalkNPC7");
            this.npc11.talkto("TalkNPC11");
            this.npc7.disableDTKFlag(131083);
            this.npc11.disableDTKFlag(131082);
            this.npc7.enableDTKFlag(4);
            this.npc11.enableDTKFlag(4);
            this.npc7.setMotion(0, 2);
            this.npc7.setInvalidID(1);
            this.npc7.setShadow(0, 0);
        }
        if (Runtime.getFlags(304, 1) == 1 && Runtime.getFlags(305, 1) == 0) {
            this.JUN = new NPC_NORMAL(5, 21, 0, 0, 21, -3.645f, 0.0f, 16.268f, -30.0f);
            this.ZIG = new NPC_NORMAL(6, 22, 0, 0, 22, 3.9837f, 0.0f, 15.251f, -60.0f);
            this.CHA = new NPC_NORMAL(3, 23, 0, 0, 23, -4.428f, 0.0f, 17.129f, 160.0f);
            this.JUN.talkto("TalkJUN");
            this.ZIG.talkto("TalkZIG");
            this.CHA.talkto("TalkCHA");
            this.JUN.disableDTKFlag(131082);
            this.ZIG.disableDTKFlag(131082);
            this.CHA.disableDTKFlag(131082);
            this.JUN.enableDTKFlag(4);
            this.ZIG.enableDTKFlag(4);
            this.CHA.enableDTKFlag(4);
            this.JUN.setMotion(0, 9);
            this.CHA.setMotion(0, 10);
        }
        if (Runtime.getFlags(310, 1) == 1) {
            Stage.setVisible(2, false);
            Stage.setVisible(1, false);
            Stage.setVisible(0, false);
            this.space = new Mapunits();
            this.space.init(20656, 0.0f, 0.0f, -110.0f, 0.0f);
            this.space.setRotate(0.0f, -90.0f, 0.0f);
            this.space.setSortOffset(1.0E10f);
            this.kuk = new Mapunits();
            this.kuk.init(20654, 0.0f, 0.0f, -100.0f, 0.0f);
            this.kuk.setRotate(0.0f, -90.0f, 0.0f);
        } else {
            this.Star_1 = new Mapunits();
            this.Star_1.mapUnit(2);
            this.Star_1.start(4, null);
            this.Star_1.setTranslate(this.Star_1.px, this.Star_1.py + 400.0f, this.Star_1.pz);
            this.Star_2 = new Mapunits();
            this.Star_2.mapUnit(1);
            this.Star_2.start(4, null);
            this.Star_2.setTranslate(this.Star_2.px, this.Star_2.py + 400.0f, this.Star_2.pz);
        }
        if (Runtime.getFlags(3068, 1) == 1) {
            this.DonDon = new Mapunits();
            this.DonDon.mapUnit(133);
            this.DonDon.start(4, null);
            this.DonDon.start(1, "Evt0");
        }
        if (Runtime.getFlags(3068, 1) == 1) {
            this.ele0 = new Unit();
            this.ele0.initElevator(89, 0.025f, -5.0f);
            this.ele0.setArgs(1, 0, 1);
        } else {
            this.ele0 = new Unit();
            this.ele0.initElevator(89, 0.025f, 0.0f);
            this.ele0.setArgs(1, 0, 1);
            this.ele0.setArgs(12, 0.0f);
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

        void Evt0() {
            if (Runtime.getFlags(310, 1) == 1) {
                ST1710.this.kuk.setTranslate(0.0f, 0.0f, -50.0f);
            }
            Runtime.setPlayerControl(false);
            ST1710.this.cam0.setMode(-1);
            ST1710.this.EV_Camera00();
            ST1710.this.player.setTranslate(0.0f, -5.0f, 16.0f);
            ST1710.this.ele0.setArgs(12, 0.0f);
            Sound.effectPlay(196747);
            System.sleep(200);
            Sound.effectPlay(196746);
            System.sleep(40);
            ST1710.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1710.this.player.mtn(2, 9, 1.0f, true);
            ST1710.this.player.move(60, 0.0f, 13.0f, true);
            System.sleep(65);
            Runtime.setFlags(3068, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            if (Runtime.getFlags(310, 1) == 1) {
                ST1710.this.kuk.setTranslate(0.0f, 0.0f, -100.0f);
            }
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }
    }
}

