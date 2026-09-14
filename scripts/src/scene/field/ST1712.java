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

class ST1712
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
    Enepc gaignun;
    Enepc allen;
    Enepc jr;
    Enepc shion;
    Enepc z8;
    Enepc chaos;
    Enepc shelley;
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
    Effect fade1;
    Effect fade2;
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
    int Tuika_01 = 0;
    int Tuika_02 = 0;
    int Tuika_03 = 0;
    int Tuika_04 = 0;
    int Tuika_05 = 0;
    int Tuika_06 = 0;
    int Tuika_07 = 0;
    int Tuika_08 = 0;
    int Tuika_09 = 0;
    int Tuika_10 = 0;
    int kaburi = 0;
    int button_flg = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    MAPUnit uta;
    MAPUnit mil;
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
    String[] T01_00 = new String[]{"There are damage reports flooding in from every sector of the Foundation already.", "/[waitkey(1)]/[clear()]", "At this rate, the destruction will only spread. Please hurry!", "/[waitkey(64)]/[close()]"};
    String[] T02_00 = new String[]{"Currently, all launch decks have been opened. They have begun to receive evacuees!", "/[waitkey(1)]/[clear()]", "But it appears to be highly chaotic because there are so many of them.", "/[waitkey(64)]/[close()]"};
    String[] T04_00 = new String[]{"What's going on? The Gnosis keep transferring in!", "/[waitkey(64)]/[close()]"};
    String[] T04_01 = new String[]{"This sector of space will be overrun by the Gnosis if this continues! We have to stop them immediately!", "/[waitkey(64)]/[close()]"};
    String[] T05_00 = new String[]{"One of the residential sectors is sending out an SOS signal.", "/[waitkey(1)]/[clear()]", "It seems residents in the damaged areas are being left behind.", "/[waitkey(64)]/[close()]"};
    String[] T08_00 = new String[]{"G-Gnosis are still transferring in!", "/[waitkey(1)]/[clear()]", "It's terrible! There are Gnosis all around the ship too!", "/[waitkey(1)]/[clear()]", "Federation ships are going down one after another.", "/[waitkey(64)]/[close()]"};
    String[] T09_00 = new String[]{"The Federation ships and A.G.W.S. forces are engaging the Gnosis in the area.", "/[waitkey(1)]/[clear()]", "Everyone, please eliminate the enemy inside the Foundation!", "/[waitkey(64)]/[close()]"};
    String[] T07_00 = new String[]{"/[label(Mary)]", "Don't lose hope, y'all. It'll be fine. The Foundation won't go down that easily!", "/[waitkey(64)]/[close()]"};
    String[] T07_01 = new String[]{"/[label(Mary)]", "The A.G.W.S. forces have been launched. Little Master, please make it your priority to evacuate the residents!", "/[waitkey(64)]/[close()]"};
    String[] T11_00 = new String[]{"/[label(Shelley)]", "If we do not hurry, the enemy will get into this ship as well!", "/[waitkey(1)]/[clear()]", "Please evacuate the residents while we hold off the enemy!", "/[waitkey(64)]/[close()]"};
    String[] T31_00 = new String[]{"/[label(Allen)]", "I will help receive the residents here.", "/[waitkey(1)]/[clear()]", "Listen, please don't do anything reckless!", "/[waitkey(1)]/[clear()]", "If it gets dangerous, run, all right? Promise me!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CN = new String[]{"Um, please...save MOMO...my sister!", "/[waitkey(1)]/[clear()]", "I am counting on you!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CJ = new String[]{"Little Master! I know you can save MOMO...my sister!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CN = new String[]{"I've isolated the signal from the prototype...I mean, on MOMO. It's coming from the center of the warped area. I'm also sensing several very weak signals from the\nsame area.", "/[waitkey(1)]/[clear()]", "Could it be that some of our sisters have also been captured, besides MOMO?", "/[waitkey(64)]/[close()]"};
    String[] U02_00CJ = new String[]{"Little Master, that thing is saturated with signals from our sisters.", "/[waitkey(1)]/[clear()]", "But most of it is just noise and seem to be on the verge of fading out.", "/[waitkey(64)]/[close()]"};
    String[] U02_01CJ = new String[]{"Little Master, is this what it means to be sad?", "/[waitkey(64)]/[close()]"};
    String[] U04_00CN = new String[]{"I'm countin' on you! Our sisters' lives are at stake, so you better not slack off!", "/[waitkey(64)]/[close()]"};
    String[] U04_00CJ = new String[]{"That's our Little Master! Your spirit's totally in it.", "/[waitkey(64)]/[close()]"};
    String[] U04_01CJ = new String[]{"But don't get overenthusiastic. Master Gaignun's always scoldin' you about that.", "/[waitkey(64)]/[close()]"};
    String[] U05_00CN = new String[]{"D#23mmerung, Rhine Maiden, Multi-Transfer Cannon. Vector's R&D capabilities are amazing, giving birth to all these things!", "/[waitkey(1)]/[clear()]", "With their help, we'll definitely be able to rescue MOMO!", "/[waitkey(64)]/[close()]"};
    String[] U05_00CJ = new String[]{"Little Master, next chance we get, let's equip the Durandal with amazing weapons too!", "/[waitkey(1)]/[clear()]", "We can't lose to that D#23mmerung!", "/[waitkey(1)]/[clear()]", "Oh, how about I become like KOS-MOS? What do you think?", "/[waitkey(64)]/[close()]"};
    String[] U08_00CN = new String[]{"I still don't really understand what sisters or love are about,", " but she's the elder sister to all of us here.", "/[waitkey(1)]/[clear()]", "I think I will be very sad if she dies.", "/[waitkey(1)]/[clear()]", "So please help her!", "/[waitkey(64)]/[close()]"};
    String[] U08_00CJ = new String[]{"Little Master, MOMO will be saved, won't she?", "/[waitkey(1)]/[clear()]", "I'm really sad.", "/[waitkey(1)]/[clear()]", "At this rate, I'm bound to make more mistakes in my work.", "/[waitkey(64)]/[close()]"};
    String[] U09_00CN = new String[]{"With the entry of the D#23mmerung into the fight, the Gnosis' combat effectiveness is down 48%.", "/[waitkey(1)]/[clear()]", "The Federation fleet has also resumed their assault on the Gnosis.", "/[waitkey(64)]/[close()]"};
    String[] U09_01CN = new String[]{"Currently, no Gnosis are present along the route to the Song of Nephilim. Now is our chance. Please hurry.", "/[waitkey(64)]/[close()]"};
    String[] U09_01CJ = new String[]{"Little Master, now is our chance to use the Federation fleet as a shield and get to the Song of Nephilim!", "/[waitkey(1)]/[clear()]", "They're useless for anything else anyway, so let's at least use them to buy us some time!", "/[waitkey(64)]/[close()]"};
    String[] U07_00CJ = new String[]{"/[label(Mary)]", "Little Master, you ready?", "/[waitkey(1)]/[clear()]", "Don't worry about us. Go rescue MOMO, quickly!", "/[waitkey(64)]/[close()]"};
    String[] U07_01CJ = new String[]{"/[label(Mary)]", "Little Master. Don't worry about us. Go rescue MOMO, quickly!", "/[waitkey(64)]/[close()]"};
    String[] U07_00CN = new String[]{"/[label(Mary)]", "Damn it!", "/[waitkey(1)]/[clear()]", "If only MOMO wasn't in there.", "/[waitkey(1)]/[clear()]", "We could take out that thing with one shot from the Foundation's main cannon!", "/[waitkey(64)]/[close()]"};
    String[] U07_01CN = new String[]{"/[label(Mary)]", "This is it! Now or never, y'all ready? You gotta put your heart into it!", "/[waitkey(64)]/[close()]"};
    String[] s_1 = new String[]{"/[label(Shelley)]", "The Gnosis gate-outs are still continuing.", "/[waitkey(1)]/[clear()]", "We are currently holding our ground, but I have no idea how long we can last.", "/[waitkey(1)]/[clear()]", "We're certain that its source of power is the Song of Nephilim. Unless we can stop that thing...", "/[waitkey(64)]/[close()]"};
    String[] U11_00CN = new String[]{"/[label(Shelley)]", "The Elsa is preparing to takeoff on the deck.", "/[waitkey(1)]/[clear()]", "After the Elsa launches, the Durandal will provide cover with all the firepower she's got, so please head directly for the Song of Nephilim.", "/[waitkey(64)]/[close()]"};
    String[] U11_01CN = new String[]{"/[label(Shelley)]", "Captain Matthews is waiting anxiously.", "/[waitkey(1)]/[clear()]", "Good luck.", "/[waitkey(64)]/[close()]"};
    String[] g_1 = new String[]{"/[label(Gaignun)]", "Albedo has control over the Song of Nephilim, meaning he's definitely after Mizrahi's records that she possesses.", "/[waitkey(1)]/[clear()]", "We'll take care of the Gnosis outside, so you guys rescue her as fast as you can.", "/[waitkey(64)]/[close()]"};
    String[] g_2 = new String[]{"/[label(Gaignun)]", "The Song of Nephilim was said to have disappeared during the Miltian Conflict, but Albedo had it all along.", "/[waitkey(1)]/[clear()]", "How did that bastard...?", "/[waitkey(64)]/[close()]"};
    String[] U30_00CJ = new String[]{"/[label(Gaignun)]", "Knowing him, there's no telling what kind of traps he set.", "/[waitkey(1)]/[clear()]", "Be careful.", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "You are in front of Foundation. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] KARI_01 = new String[]{"/[label(Temp Person)]", "You are in front of the Song of Nephilim. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] ev_1 = new String[]{"/[label(Gaignun)]", "The Foundation has its hands full dealing with the waves of Gnosis. We cannot move from our current location.", "/[waitkey(1)]/[clear()]", "We'll need a ship with sufficient mobility in order to get past the Gnosis and dock with the Song of Nephilim.", "/[waitkey(64)]/[close()]"};
    String[] ev_2 = new String[]{"/[label(Jr.)]", "The Durandal's landing shuttle won't do. How about the Elsa? Is it operational?", "/[waitkey(64)]/[close()]"};
    String[] ev_3 = new String[]{"/[label(Shelley)]", "Yes. I checked its status just now, and fortunately, it seems to be unharmed.", "/[waitkey(1)]/[clear()]", "Shall we relay a message to Captain Matthews?", "/[waitkey(64)]/[close()]"};
    String[] ev_4 = new String[]{"/[label(Jr.)]", "Yeah, go ahead.", "/[waitkey(1)]/[clear()]", "Damn, that feeling I got from him...that bastard was trying to access MOMO's subconscious.", "/[waitkey(1)]/[clear()]", "We don't have time! Let's go old man! chaos!", "/[waitkey(64)]/[close()]"};
    String[] ev_5 = new String[]{"/[label(Shion)]", "Wait, Jr., let us help too!", "/[waitkey(64)]/[close()]"};
    String[] ev_6 = new String[]{"/[label(Jr.)]", "Don't be ridiculous!", "/[waitkey(1)]/[clear()]", "You have any idea what kind of place that is?! Do you know what that lunatic is like?!", "/[waitkey(64)]/[close()]"};
    String[] ev_7 = new String[]{"/[label(Shion)]", "Please! Let us go with you.", "/[waitkey(64)]/[close()]"};
    String[] ev_8 = new String[]{"/[label(Jr.)]", "Shion...", "/[waitkey(64)]/[close()]"};
    String[] ev_9 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ev_10 = new String[]{"/[label(Jr.)]", "All right.", "/[waitkey(1)]/[clear()]", "But you better not let your guard down, even for a second. There's no room for mistakes in there, got it?", "/[waitkey(64)]/[close()]"};
    String[] ev_11 = new String[]{"/[label(Shion)]", "Yes.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1712() {
    }

    void EV_Camera00() {
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[1] = -3.607f;
        fArray[2] = 8.345f;
        fArray[3] = -8.211f;
        fArray[4] = 240.0f;
        fArray[6] = 8.345f;
        fArray[7] = -8.211f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[0] = 1.0f;
        fArray3[1] = -27.583f;
        fArray3[2] = 202.139f;
        fArray3[4] = 240.0f;
        fArray3[5] = -27.583f;
        fArray3[6] = 180.0f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 3, 240);
        this.camEV.rotateSPL(fArray4, 1, 3, 240);
        this.camEV.setFov(40.0f);
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
                    if (this.kaburi == 1) {
                        return;
                    }
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
                            if (Runtime.getFlags(373, 1) == 0) {
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
                            Runtime.jumpCF(1822, 0);
                            return;
                        }
                        default: {
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
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(373, 1) == 0) {
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
                        this.win.print(this.T08_00, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    } else if (Runtime.getLeader() == 5) {
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
                        this.win.print(this.U08_00CJ, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                        return;
                    } else {
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
                        this.win.print(this.U08_00CN, 0);
                        System.waitFor(this.win);
                        this.npc8.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        this.button_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 2) return;
        switch (n) {
            default:
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 0) {
            window.print(this.T01_00, 0);
            System.waitFor(window);
        } else if (Runtime.getLeader() == 5) {
            window.print(this.U01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC11(Enepc enepc) {
        if (Runtime.getFlags(373, 1) == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.T11_00, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
        } else if (Runtime.getLeader() == 5) {
            Runtime.setPlayerControl(false);
            this.nwin(this.s_1);
            Runtime.setPlayerControl(true);
        } else if (this.Tuika_04 == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.U11_00CN, 0);
            System.waitFor(this.win);
            this.Tuika_04 = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.U11_01CN, 0);
            System.waitFor(this.win);
            this.Tuika_04 = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 0) {
            window.print(this.T02_00, 0);
            System.waitFor(window);
        } else if (Runtime.getLeader() == 5) {
            if (this.Tuika_07 == 0) {
                window.print(this.U02_00CJ, 0);
                System.waitFor(window);
                this.Tuika_07 = 1;
            } else {
                window.print(this.U02_01CJ, 0);
                System.waitFor(window);
                this.Tuika_07 = 0;
            }
        } else {
            window.print(this.U02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC30(Enepc enepc) {
        if (Runtime.getLeader() == 5) {
            if (this.Tuika_05 == 0) {
                Runtime.setPlayerControl(false);
                this.nwin(this.g_1);
                this.Tuika_05 = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.nwin(this.U30_00CJ);
                this.Tuika_05 = 0;
                Runtime.setPlayerControl(true);
            }
        } else {
            Runtime.setPlayerControl(false);
            this.nwin(this.g_2);
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC31(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.T31_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 0) {
            if (this.Tuika_01 == 0) {
                window.print(this.T04_00, 0);
                System.waitFor(window);
                this.Tuika_01 = 1;
            } else {
                window.print(this.T04_01, 0);
                System.waitFor(window);
                this.Tuika_01 = 0;
            }
        } else if (Runtime.getLeader() == 5) {
            if (this.Tuika_08 == 0) {
                window.print(this.U04_00CJ, 0);
                System.waitFor(window);
                this.Tuika_08 = 1;
            } else {
                window.print(this.U04_01CJ, 0);
                System.waitFor(window);
                this.Tuika_08 = 0;
            }
        } else {
            window.print(this.U04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 0) {
            window.print(this.T05_00, 0);
            System.waitFor(window);
        } else if (Runtime.getLeader() == 5) {
            window.print(this.U05_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U05_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC7(Enepc enepc) {
        if (Runtime.getFlags(373, 1) == 0) {
            if (this.Tuika_02 == 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.T07_00, 0);
                System.waitFor(this.win);
                this.Tuika_02 = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.T07_01, 0);
                System.waitFor(this.win);
                this.Tuika_02 = 0;
                Runtime.setPlayerControl(true);
            }
        } else if (Runtime.getLeader() == 5) {
            if (this.Tuika_09 == 0) {
                Runtime.setPlayerControl(false);
                this.nwin(this.U07_00CJ);
                this.Tuika_09 = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.nwin(this.U07_01CJ);
                this.Tuika_09 = 0;
                Runtime.setPlayerControl(true);
            }
        } else if (this.Tuika_09 == 0) {
            Runtime.setPlayerControl(false);
            this.nwin(this.U07_00CN);
            this.Tuika_09 = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.nwin(this.U07_01CN);
            this.Tuika_09 = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC9(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 0) {
            window.print(this.T09_00, 0);
            System.waitFor(window);
        } else if (Runtime.getLeader() == 5) {
            window.print(this.U09_01CJ, 0);
            System.waitFor(window);
        } else if (this.Tuika_03 == 0) {
            window.print(this.U09_00CN, 0);
            System.waitFor(window);
            this.Tuika_03 = 1;
        } else {
            window.print(this.U09_01CN, 0);
            System.waitFor(window);
            this.Tuika_03 = 0;
        }
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
        if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(3189, 1) == 0) {
            Runtime.resetOutFriend(3);
            Runtime.resetOutFriend(2);
            Runtime.resetOutFriend(6);
            Runtime.resetOutFriend(5);
            Runtime.resetOutFriend(4);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 7);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 1);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(3189, 1, 1);
        }
        if (Runtime.getFlags(373, 1) == 1 && Runtime.getFlags(3106, 1) == 0) {
            Runtime.resetOutFriend(3);
            Runtime.resetOutFriend(2);
            Runtime.resetOutFriend(6);
            Runtime.resetOutFriend(5);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 7);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 1);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(3106, 1, 1);
        }
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
        if (Runtime.getFlags(373, 1) == 0) {
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
        }
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
        this.npc7 = new NPC_NORMAL(289, 17, 0, 0, 4, -0.5f, 1.5f, 7.844f, 180.0f);
        this.npc8 = new NPC_NORMAL(1028, 18, 0, 3, 4, -5.625f, -1.5f, 5.219f, 180.0f);
        this.npc9 = new NPC_NORMAL(1028, 19, 0, 0, 4, 5.313f, 0.5f, 13.688f, 135.0f);
        this.npc11 = new NPC_NORMAL(288, 21, 0, 2, 7, -3.7f, 0.0f, 16.6f, 180.0f);
        this.npc1.renderCommand(530);
        this.npc2.renderCommand(530);
        this.npc4.renderCommand(530);
        this.npc5.renderCommand(530);
        this.npc7.renderCommand(530);
        this.npc9.renderCommand(530);
        this.npc1.talkto("TalkNPC1");
        this.npc2.talkto("TalkNPC2");
        this.npc4.talkto("TalkNPC4");
        this.npc5.talkto("TalkNPC5");
        this.npc8.talkto("TalkNPC8");
        this.npc7.talkto("TalkNPC11");
        this.npc9.talkto("TalkNPC9");
        this.npc11.talkto("TalkNPC7");
        this.npc1.disableDTKFlag(131075);
        this.npc2.disableDTKFlag(131075);
        this.npc4.disableDTKFlag(131075);
        this.npc5.disableDTKFlag(131075);
        this.npc8.disableDTKFlag(131075);
        this.npc9.disableDTKFlag(131075);
        this.npc7.disableDTKFlag(131083);
        this.npc11.disableDTKFlag(131082);
        this.npc1.enableDTKFlag(12);
        this.npc2.enableDTKFlag(12);
        this.npc4.enableDTKFlag(12);
        this.npc5.enableDTKFlag(12);
        this.npc8.enableDTKFlag(12);
        this.npc9.enableDTKFlag(12);
        this.npc7.enableDTKFlag(4);
        this.npc11.enableDTKFlag(4);
        this.npc1.setMotion(0, 1);
        this.npc2.setMotion(0, 2);
        this.npc4.setMotion(0, 4);
        this.npc5.setMotion(0, 3);
        this.npc7.setMotion(0, 2);
        this.npc8.setMotion(0, 3);
        this.npc9.setMotion(0, 4);
        this.npc1.setInvalidID(1);
        this.npc2.setInvalidID(1);
        this.npc4.setInvalidID(1);
        this.npc5.setInvalidID(1);
        this.npc7.setInvalidID(1);
        this.npc8.setInvalidID(1);
        this.npc9.setInvalidID(1);
        this.npc11.setInvalidID(1);
        this.npc1.setShadow(0, 0);
        this.npc2.setShadow(0, 0);
        this.npc4.setShadow(0, 0);
        this.npc5.setShadow(0, 0);
        this.npc8.setShadow(0, 0);
        this.npc9.setShadow(0, 0);
        this.npc7.setShadow(0, 0);
        if (Runtime.getFlags(373, 1) == 1) {
            this.gaignun = new NPC_NORMAL(259, 30, 0, 2, 14, -3.196f, 0.0f, 14.127f, 90.0f);
            this.gaignun.talkto("TalkNPC30");
            this.gaignun.disableDTKFlag(131083);
            this.gaignun.enableDTKFlag(4);
            this.gaignun.renderCommand(530);
            this.gaignun.setInvalidID(1);
            this.mil = new Mapunits();
            this.mil.init(20655, 0.0f, -10.0f, -7.0f, 0.0f);
            this.mil.setSortOffset(1.0E10f);
        }
        if (Runtime.getFlags(373, 1) == 0) {
            this.allen = new NPC_NORMAL(263, 31, 0, 2, 17, -3.196f, 0.0f, 14.127f, 90.0f);
            this.allen.talkto("TalkNPC31");
            this.allen.disableDTKFlag(131083);
            this.allen.enableDTKFlag(4);
            this.allen.setInvalidID(1);
        } else {
            this.uta = new Mapunits();
            this.uta.init(20562, 0.0f, 0.0f, -150.0f, 0.0f);
            this.uta.setRotate(0.0f, -45.0f, 0.0f);
            this.uta.setScale(15.0f, 15.0f, 15.0f);
            this.uta.renderCommand(530);
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
        if (Runtime.getFlags(352, 1) == 1 && Runtime.getFlags(361, 1) == 0 && Runtime.getFlags(3192, 1) == 0) {
            Runtime.setFlags(3192, 1, 1);
            this.player.setTranslate(3.0f, 0.0f, 13.5f);
        }
        if (Runtime.getFlags(373, 1) == 1 && Runtime.getFlags(8111, 1) == 0) {
            Runtime.setFlags(8111, 1, 1);
            this.gaignun.setTranslate(0.7f, 1.0f, 8.1f);
            this.gaignun.setRotateY(0.0f);
            this.gaignun.kickEnepc(4, 1);
            this.jr = new NPC_NORMAL(5, 41, 0, 2, 22, 1.7f, 0.0f, 14.1f, 180.0f);
            this.jr.kickEnepc(4, 1);
            this.jr.setInvalidID(1);
            this.jr.setInvalidID(1);
            this.z8 = new NPC_NORMAL(6, 42, 0, 2, 25, 4.2f, 0.0f, 13.0f, 270.0f);
            this.z8.kickEnepc(4, 1);
            this.z8.setInvalidID(1);
            this.z8.setInvalidID(1);
            this.shion = new NPC_NORMAL(1, 43, 0, 2, 28, -3.2f, 0.0f, 12.6f, 90.0f);
            this.shion.kickEnepc(4, 1);
            this.shion.setInvalidID(1);
            this.shion.setInvalidID(1);
            this.chaos = new NPC_NORMAL(3, 44, 0, 2, 31, 2.7f, 0.0f, 11.4f, 0.0f);
            this.chaos.setInvalidID(1);
            this.chaos.kickEnepc(4, 1);
            this.chaos.setInvalidID(1);
            this.shelley = new NPC_NORMAL(289, 45, 0, 0, 4, -0.5f, 1.5f, 7.844f, 180.0f);
            this.shelley.setMotion(0, 2);
            this.shelley.setInvalidID(1);
            this.shelley.setInvalidID(1);
            this.kaburi = 1;
            this.player.setLocation(1, 2);
            this.mil.start(1, "Utagoe");
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1712.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1712.waitPage(this.win, 64);
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
            if (Runtime.getFlags(373, 1) == 0) {
                ST1712.this.kuk.setTranslate(0.0f, 0.0f, -50.0f);
            }
            Runtime.setPlayerControl(false);
            ST1712.this.player.look_char(ST1712.this.npc1);
            ST1712.this.cam0.setMode(-1);
            ST1712.this.EV_Camera00();
            ST1712.this.player.setTranslate(0.0f, -5.0f, 16.0f);
            ST1712.this.ele0.setArgs(12, 0.0f);
            Sound.effectPlay(196747);
            System.sleep(200);
            Sound.effectPlay(196746);
            System.sleep(40);
            ST1712.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1712.this.player.mtn(2, 9, 1.0f, true);
            ST1712.this.player.move(60, 0.0f, 13.0f, true);
            System.sleep(65);
            Runtime.setFlags(3068, 1, 0);
            ST1712.this.player.look_default();
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            if (Runtime.getFlags(373, 1) == 0) {
                ST1712.this.kuk.setTranslate(0.0f, 0.0f, -100.0f);
            }
        }

        void Utagoe() {
            Runtime.setPlayerControl(false);
            ST1712.this.player.setVisible(false);
            ST1712.this.npc1.kickEnepc(4, 1);
            ST1712.this.npc1.setVisible(false);
            ST1712.this.npc2.kickEnepc(4, 1);
            ST1712.this.npc2.setVisible(false);
            ST1712.this.npc4.kickEnepc(4, 1);
            ST1712.this.npc4.setVisible(false);
            ST1712.this.npc5.kickEnepc(4, 1);
            ST1712.this.npc5.setVisible(false);
            ST1712.this.npc7.kickEnepc(4, 1);
            ST1712.this.npc7.setVisible(false);
            ST1712.this.npc8.kickEnepc(4, 1);
            ST1712.this.npc8.setVisible(false);
            ST1712.this.npc11.kickEnepc(4, 1);
            ST1712.this.npc11.setVisible(false);
            ST1712.this.cam0.setMode(-1);
            ST1712.this.camEV = Camera.create(1);
            ST1712.this.camEV.setRotate(-15.7f, 50.8f, 0.0f);
            ST1712.this.camEV.setTranslate(1.9f, 2.9f, 9.2f);
            ST1712.this.camEV.setFov(30.0f);
            ST1712.this.camEV.change();
            ST1712.this.jr.kickEnepc(0, 0);
            ST1712.this.gaignun.kickEnepc(0, 0);
            ST1712.this.z8.look_char(ST1712.this.jr);
            ST1712.this.z8.kickEnepc(0, 0);
            ST1712.this.shion.kickEnepc(0, 0);
            ST1712.this.chaos.look_char(ST1712.this.jr);
            ST1712.this.chaos.kickEnepc(0, 0);
            ST1712.this.gaignun.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_1);
            ST1712.this.camEV.setRotate(7.7f, -4.0f, 0.0f);
            ST1712.this.camEV.setTranslate(1.3f, 0.8f, 16.7f);
            ST1712.this.camEV.setFov(21.0f);
            ST1712.this.camEV.change();
            ST1712.this.jr.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_2);
            ST1712.this.camEV.setRotate(-15.7f, 50.8f, 0.0f);
            ST1712.this.camEV.setTranslate(1.9f, 2.9f, 9.2f);
            ST1712.this.camEV.setFov(30.0f);
            ST1712.this.camEV.change();
            ST1712.this.shelley.look_char(ST1712.this.jr);
            ST1712.this.nwin(ST1712.this.ev_3);
            ST1712.this.camEV.setRotate(-11.8f, 75.0f, 0.0f);
            ST1712.this.camEV.setTranslate(8.1f, 2.0f, 14.5f);
            ST1712.this.camEV.setFov(30.0f);
            ST1712.this.camEV.change();
            ST1712.this.jr.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_4);
            ST1712.this.chaos.kickEnepc(0, 7);
            ST1712.this.shion.look_char(ST1712.this.jr);
            ST1712.this.shion.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_5);
            ST1712.this.jr.look_char(ST1712.this.shion);
            ST1712.this.jr.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_6);
            ST1712.this.shion.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_7);
            ST1712.this.jr.kickEnepc(0, 9);
            ST1712.this.nwin(ST1712.this.ev_8);
            ST1712.this.nwin(ST1712.this.ev_9);
            ST1712.this.jr.kickEnepc(0, 7);
            ST1712.this.nwin(ST1712.this.ev_10);
            ST1712.this.shion.kickEnepc(0, 7);
            ST1712.this.nwin(ST1712.this.ev_11);
            ST1712.this.fade1.call(0);
            System.sleep(60);
            ST1712.this.cam0.setMode(0);
            ST1712.this.jr.kickEnepc(4, 2);
            ST1712.this.jr.setVisible(false);
            ST1712.this.jr.setTranslate(0.0f, 0.0f, 100.0f);
            ST1712.this.z8.kickEnepc(4, 2);
            ST1712.this.z8.setVisible(false);
            ST1712.this.z8.setTranslate(0.0f, 0.0f, 100.0f);
            ST1712.this.shion.kickEnepc(4, 2);
            ST1712.this.shion.setVisible(false);
            ST1712.this.shion.setTranslate(0.0f, 0.0f, 100.0f);
            ST1712.this.chaos.kickEnepc(4, 2);
            ST1712.this.chaos.setVisible(false);
            ST1712.this.chaos.setTranslate(0.0f, 0.0f, 100.0f);
            ST1712.this.shelley.kickEnepc(4, 2);
            ST1712.this.shelley.setVisible(false);
            ST1712.this.shelley.setTranslate(0.0f, 0.0f, 100.0f);
            ST1712.this.npc1.setVisible(true);
            ST1712.this.npc1.kickEnepc(4, 0);
            ST1712.this.npc2.setVisible(true);
            ST1712.this.npc2.kickEnepc(4, 0);
            ST1712.this.npc4.setVisible(true);
            ST1712.this.npc4.kickEnepc(4, 0);
            ST1712.this.npc5.setVisible(true);
            ST1712.this.npc5.kickEnepc(4, 0);
            ST1712.this.npc7.setVisible(true);
            ST1712.this.npc7.kickEnepc(4, 0);
            ST1712.this.npc8.setVisible(true);
            ST1712.this.npc8.kickEnepc(4, 0);
            ST1712.this.npc11.setVisible(true);
            ST1712.this.npc11.kickEnepc(4, 0);
            ST1712.this.gaignun.kickEnepc(4, 0);
            ST1712.this.player.setVisible(true);
            ST1712.this.fade2.call(0);
            System.sleep(60);
            ST1712.this.kaburi = 0;
            Runtime.setPlayerControl(true);
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

