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

class ST1800
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
    Enepc ziggy;
    Enepc chaos;
    Enepc momo;
    Enepc kosmos;
    Enepc mat;
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
    int npc6btalked = 0;
    int npc7talked = 0;
    int npc7btalked = 0;
    int npc8talked = 0;
    int button1_flg = 0;
    int npc2xxtalked = 0;
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
    String[] Real_00 = new String[]{"We apologize, but the launch to the Foundation is currently closed.", "/[waitkey(1)]/[clear()]", "We look forward to serving you again in the future.", "/[waitkey(64)]/[close()]"};
    String[] Real_01 = new String[]{"The Foundation is a fun place with lots of shops and restaurants.", "/[waitkey(1)]/[clear()]", "I hope you enjoy your stay.", "/[waitkey(64)]/[close()]"};
    String[] PEO_1_00 = new String[]{"A ship this size must be an ordeal to move.", "/[waitkey(64)]/[close()]"};
    String[] PEO_1_XX = new String[]{"Hmm? A little girl? Haven't seen one in this area.", "/[waitkey(1)]/[clear()]", "Come to think of it, I think someone said he saw a girl he'd never seen before in the Park.", "/[waitkey(1)]/[clear()]", "You know what this ship's like. A little girl is pretty rare.", "/[waitkey(64)]/[close()]"};
    String[] PEO_1_XXX = new String[]{"When you're on a ship as big as this, it's easy to lose track of where you are! When you're not sure where you are, you should use these Information Boards!! ", "If you look at them, you'll know where you are right away! There's a board set up at every station in the Durandal. They're really handy!", "/[waitkey(64)]/[close()]"};
    String[] PEO_2_00 = new String[]{"Oh!!\n", "Well if it isn't Little Master!", "/[waitkey(1)]/[clear()]", "How do you stay so young like that? I wish you'd share your secret with me.", "/[waitkey(64)]/[close()]"};
    String[] PEO_2_01 = new String[]{"Did you know?", "/[waitkey(1)]/[clear()]", "The Durandal also acts as the Foundation's administrative government.", "/[waitkey(1)]/[clear()]", "You don't understand what I'm saying?", "/[waitkey(1)]/[clear()]", "Well, you'll see right away what I mean when this ship arrives in port at the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] ENG_00 = new String[]{"Little Master?! There is a work corridor past this point.", "/[waitkey(1)]/[clear()]", "It's dangerous, so please leave the work up to us.", "/[waitkey(64)]/[close()]"};
    String[] ENG_01 = new String[]{"Oh, hey! It's a work corridor past this point. It's dangerous, so you can't go any further!", "/[waitkey(64)]/[close()]"};
    String[] REAL_M_00 = new String[]{"O-oh, Little Master! I can't do it. Not anymore!", "/[waitkey(64)]/[close()]"};
    String[] REAL_M_01 = new String[]{"No matter how hard I try, I just can't hit on girls...", "/[waitkey(64)]/[close()]"};
    String[] REAL_M_02 = new String[]{"Uh, um, would you like to go out for some coffee?", "/[waitkey(1)]/[clear()]", "Hmm, that's not right.", "/[waitkey(64)]/[close()]"};
    String[] REAL_M_03 = new String[]{"Hey, baby, how's about you and me go for some tea?", "/[waitkey(1)]/[clear()]", "That's sort of weird.", "/[waitkey(64)]/[close()]"};
    String[] REAL_M_04 = new String[]{"Hmm, it just doesn't sound right! I'm not smooth like Little Master.", "/[waitkey(64)]/[close()]"};
    String[] REAL_W_00 = new String[]{"Oh, Little Master, I don't think he has what it takes. Spending any more time teaching him is just a waste.", "/[waitkey(64)]/[close()]"};
    String[] REAL_W_01 = new String[]{"Little Master, what's the point of teaching Realians how to hit on girls?", "/[waitkey(64)]/[close()]"};
    String[] REAL_W_02 = new String[]{"Oh, it's not like he's broken or anything.", "/[waitkey(1)]/[clear()]", "He seems to be taking lessons from Little Master on how to hit on girls, and he's practicing.", "/[waitkey(64)]/[close()]"};
    String[] ANO_00 = new String[]{"This shuttle is exclusively for direct flights to the Kukai Foundation.", "/[waitkey(1)]/[clear()]", "Would you like to go to the Foundation?", "/[waitkey(64)]/[close()]"};
    String[] ANO_01 = new String[]{"Understood. We wish you a pleasant stay...", "/[waitkey(64)]/[close()]"};
    String[] ANO_02 = new String[]{"Understood. We look forward to serving you again soon.", "/[waitkey(64)]/[close()]"};
    String[] ANO_03 = new String[]{"This ship is currently traveling through Miltian space.", "/[waitkey(1)]/[clear()]", "Please wait a few moments to use the launch to the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] JR_00 = new String[]{"/[label(Jr.)]", "Shion, why do you look so down? Let's hurry and go to the beach! I'll show you around!", "/[waitkey(64)]/[close()]"};
    String[] ALLEN_00 = new String[]{"/[label(Allen)]", "A lot of things have happened since the Woglinde, so I can understand your brooding.", "/[waitkey(1)]/[clear()]", "But let's listen to Jr. here and forget our worries for a while.", "/[waitkey(64)]/[close()]"};
    String[] ZIGGY_00 = new String[]{"/[label(Ziggy)]", "I appreciate your invitation, but I need to report our current situation to the committee.", "/[waitkey(1)]/[clear()]", "It's not a chance you get often. Go on and enjoy yourselves.", "/[waitkey(64)]/[close()]"};
    String[] CHAOS_00 = new String[]{"/[label(chaos)]", "Gaignun's private beach...", "/[waitkey(1)]/[clear()]", "It was still under construction last time I was here, so it'll be the first time I actually get to see it.", "/[waitkey(64)]/[close()]"};
    String[] MOMO_00 = new String[]{"/[label(MOMO)]", "Gaignun's private beach?", "/[waitkey(1)]/[clear()]", "I can't wait to see what it's like!!", "/[waitkey(64)]/[close()]"};
    String[] KOSMOS_00 = new String[]{"/[label(KOS-MOS)]", "Shion, I will return to the service module on the Elsa for my regular data update.", "/[waitkey(64)]/[close()]"};
    String[] KOSMOS_01 = new String[]{"/[label(KOS-MOS)]", "I will return to the service module on the Elsa for my regular data update.", "/[waitkey(64)]/[close()]"};
    String[] MAT_00 = new String[]{"/[label(Matthews)]", "Hey! You look awfully bored!", "/[waitkey(1)]/[clear()]", "Want me to take you to the Dock Colony or something?!", "/[waitkey(64)]/[close()]"};
    String[] MAT_00_1 = new String[]{"/[label(Matthews)]", "All right!!", "/[waitkey(1)]/[clear()]", "Let's make a quick run to the Dock Colony!", "/[waitkey(1)]/[clear()]", "I'm gonna charge you for it, though!", "/[waitkey(64)]/[close()]"};
    String[] MAT_00_2 = new String[]{"/[label(Matthews)]", "I suppose.", "/[waitkey(1)]/[clear()]", "I'll personally guarantee the comfort of the Foundation's hotel!!", "/[waitkey(64)]/[close()]"};
    String[] MAT_00_3 = new String[]{"/[label(Matthews)]", "Hey! If you're going to the Dock Colony, let me take you!", "/[waitkey(64)]/[close()]"};
    String[] MAT_01 = new String[]{"/[label(Matthews)]", "Yo! How was the Foundation's hotel?", "/[waitkey(1)]/[clear()]", "Wasn't it comfortable, just like I said?", "/[waitkey(1)]/[clear()]", "I'm still keeping the Elsa parked here! Or is there someplace else you wanna go?", "/[waitkey(64)]/[close()]"};
    String[] MAT_01_1 = new String[]{"/[label(Matthews)]", "I see...it might be a good idea to just wander around for a while.", "/[waitkey(64)]/[close()]"};
    String[] SHI_X0 = new String[]{"/[label(Shion)]", "Maybe I'll try staying at the Foundation's hotel after all.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_X0 = new String[]{"/[label(Ziggy)]", "The Foundation's hotel? Shall we head out there and see it?", "/[waitkey(64)]/[close()]"};
    String[] CHA_X0 = new String[]{"/[label(chaos)]", "The Foundation's hotel? Since I'm here and all, maybe I should stay at the hotel?", "/[waitkey(64)]/[close()]"};
    String[] MOM_X0 = new String[]{"/[label(MOMO)]", "We are here at the Foundation, and I'd like to go to the hotel since we're here already.", "/[waitkey(64)]/[close()]"};
    String[] JUN_X0 = new String[]{"/[label(Jr.)]", "Durandal's bedrooms, huh? They're not bad. But since it's been a while since I've stayed there, wanna go to the Foundation's hotel?", "/[waitkey(64)]/[close()]"};
    String[] PLAYER_01 = new String[]{"'Dock Area Station\n", "Boarding area for direct flights to the Foundation'", "/[waitkey(64)]/[close()]"};
    String[] K02_00 = new String[]{"Looking at this information board doesn't tell me much.", "/[waitkey(1)]/[clear()]", "Residential area? Is that some kind of code?", "/[waitkey(64)]/[close()]"};
    String[] K03_00 = new String[]{"What did you think of the Durandal transforming into a giant skyscraper?", "/[waitkey(1)]/[clear()]", "Wasn't it a spectacular sight?", "/[waitkey(1)]/[clear()]", "You'll never see scenery like this anywhere else.", "/[waitkey(64)]/[close()]"};
    String[] K03_01 = new String[]{"Oh! Little Master.", "/[waitkey(1)]/[clear()]", "The sight of this ship entering port never ceases to amaze me, no matter how many times I've seen it!", "/[waitkey(64)]/[close()]"};
    String[] K06_00 = new String[]{"Uh, um, do you think I will be able to hit on girls too?", "/[waitkey(1)]/[clear()]", "I've been practicing a lot, but I haven't improved at all.", "/[waitkey(1)]/[clear()]", "Do you suppose it's impossible for Realians to hit on girls?", "/[waitkey(64)]/[close()]"};
    String[] K07_00 = new String[]{"What? Allen?", "/[waitkey(1)]/[clear()]", "Come to think of it, I saw someone from Vector walking unsteadily onto a shuttle.", "/[waitkey(1)]/[clear()]", "He looked like he was ready to die,", "/[waitkey(1)]/[clear()]", "so I think you'd better go after him quickly.", "/[waitkey(64)]/[close()]"};

    ST1800() {
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
                this.fade1.call(0);
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
                            this.Departure(1840);
                            return;
                        }
                        case 1: {
                            Runtime.setFlags(3070, 1, 1);
                            this.Departure(1820);
                            return;
                        }
                        case 2: {
                            Runtime.setFlags(3074, 1, 1);
                            this.Departure(1860);
                            return;
                        }
                        case 3: {
                            Runtime.setFlags(3073, 1, 1);
                            this.Departure(1850);
                            return;
                        }
                        case 4: {
                            Runtime.setFlags(3071, 1, 1);
                            this.Departure(1830);
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
        if (n2 == 1) return;
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(314, 1) != 1 || Runtime.getFlags(315, 1) != 0 || Runtime.getFlags(7123, 2) != 1)
                        return;
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px - 0.3f, this.player.py, this.player.pz);
                    Runtime.enable(65536);
                    this.player.mtn(28, 9, 1.0f, true);
                    this.player.look_char(this.mat);
                    this.mat.kickEnepc(4, 1);
                    this.mat.kickEnepc(9, 100);
                    this.mat.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.MAT_00, 0);
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
                            this.win.print(this.MAT_00_1, 0);
                            System.waitFor(this.win);
                            this.mat.kickEnepc(9, -1);
                            this.mat.kickEnepc(4, 0);
                            this.fade1.call(0);
                            System.sleep(30);
                            Runtime.setFlags(7086, 1, 1);
                            this.player.look_default();
                            Runtime.setPlayerControl(true);
                            Runtime.disable(65536);
                            Runtime.jumpCF(9051, 0);
                            return;
                        }
                        default: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.MAT_01_1, 0);
                            System.waitFor(this.win);
                            this.player.look_default();
                            Runtime.setPlayerControl(true);
                            Runtime.disable(65536);
                            this.mat.kickEnepc(9, -1);
                            this.mat.kickEnepc(4, 0);
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    this.player.getTranslate();
                    if (this.player.py <= 4.0f) {
                        return;
                    }
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
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(314, 1) != 1 || Runtime.getFlags(315, 1) != 0 || Runtime.getFlags(7123, 2) != 1)
                        return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (Runtime.getLeader() == 1) {
                        this.win.print(this.SHI_X0, 0);
                    } else if (Runtime.getLeader() == 6) {
                        this.win.print(this.ZIG_X0, 0);
                    } else if (Runtime.getLeader() == 3) {
                        this.win.print(this.CHA_X0, 0);
                    } else if (Runtime.getLeader() == 4) {
                        this.win.print(this.MOM_X0, 0);
                    } else {
                        this.win.print(this.JUN_X0, 0);
                    }
                    System.waitFor(this.win);
                    Runtime.disable(65536);
                    System.sleep(1);
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(60, -6.0f, -1.0f, true);
                    System.sleep(65);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 5) return;
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

    public void TalkALLEN(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.ALLEN_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
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

    public void TalkJR(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.JR_00, 0);
        System.waitFor(this.win);
        this.fade.call(0);
        System.sleep(60);
        this.jr.setTranslate(-100.0f, -100.0f, -100.0f);
        this.allen.setTranslate(-100.0f, -100.0f, -100.0f);
        this.chaos.setTranslate(-100.0f, -100.0f, -100.0f);
        this.momo.setTranslate(-100.0f, -100.0f, -100.0f);
        Runtime.setTakeAgws(8194);
        Runtime.resetOutFriend(4);
        Runtime.resetOutFriend(3);
        Runtime.resetOutFriend(5);
        Runtime.setPartyData(0x1010000, 3);
        Runtime.setPartyData(65538, 1);
        Runtime.setPartyData(0x1010004, 7);
        Runtime.setPartyData(65542, 2);
        Runtime.setPartyData(0x1010008, 1);
        Runtime.setPartyData(65546, 3);
        Runtime.setPartyData(16777260, 1);
        System.println("パーティー情報・leader_shion,kosmos,chaos,*,*,*");
        Runtime.setFlags(3098, 1, 1);
        this.fade2.call(0);
        System.sleep(60);
        Runtime.setFlags(3134, 1, 1);
        Runtime.setFlags(7162, 1, 1);
        this.doorC.SetDoorType('\u0001');
        Runtime.setPlayerControl(true);
    }

    public void TalkKOSMOS(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        if (Runtime.getLeader() == 1) {
            this.win.print(this.KOSMOS_00, 0);
        } else {
            this.win.print(this.KOSMOS_01, 0);
        }
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkMATa(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.MAT_00, 0);
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
                this.win.print(this.MAT_00_1, 0);
                System.waitFor(this.win);
                this.fade1.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 1);
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(9051, 0);
                break;
            }
            default: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.MAT_01_1, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkMOMO(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.MOMO_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getFlags(310, 1) == 1) {
            window.print(this.Real_01, 0);
            ST1800.waitPage(window, 64);
        } else {
            window.print(this.Real_00, 0);
            ST1800.waitPage(window, 64);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        window.print(this.PEO_1_00, 0);
        ST1800.waitPage(window, 64);
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        window.print(this.K02_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC2xx(Enepc enepc, Window window) {
        if (this.npc2xxtalked == 0) {
            window.print(this.PEO_1_XX, 0);
            ST1800.waitPage(window, 64);
            this.npc2xxtalked = 1;
        } else if (this.npc2xxtalked == 1) {
            window.print(this.PEO_1_00, 0);
            ST1800.waitPage(window, 64);
            this.npc2xxtalked = 2;
        } else {
            window.print(this.PEO_1_XXX, 0);
            ST1800.waitPage(window, 64);
            this.npc2xxtalked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.PEO_2_00, 0);
            ST1800.waitPage(window, 64);
        } else {
            window.print(this.PEO_2_01, 0);
            ST1800.waitPage(window, 64);
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K03_01, 0);
            System.waitFor(window);
        } else {
            window.print(this.K03_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.ENG_00, 0);
            ST1800.waitPage(window, 64);
        } else {
            window.print(this.ENG_01, 0);
            ST1800.waitPage(window, 64);
        }
    }

    public void TalkNPC6(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc6btalked == 0) {
                window.print(this.REAL_M_00, 0);
                ST1800.waitPage(window, 64);
                this.npc6btalked = 1;
            } else {
                window.print(this.REAL_M_01, 0);
                ST1800.waitPage(window, 64);
                this.npc6btalked = 0;
            }
        } else if (this.npc6talked == 0) {
            window.print(this.REAL_M_02, 0);
            ST1800.waitPage(window, 64);
            this.npc6talked = 1;
        } else if (this.npc6talked == 1) {
            window.print(this.REAL_M_03, 0);
            ST1800.waitPage(window, 64);
            this.npc6talked = 2;
        } else {
            window.print(this.REAL_M_04, 0);
            ST1800.waitPage(window, 64);
            this.npc6talked = 0;
        }
    }

    public void TalkNPC6a(Enepc enepc, Window window) {
        window.print(this.K06_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC7(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc7btalked == 0) {
                window.print(this.REAL_W_00, 0);
                ST1800.waitPage(window, 64);
                this.npc7btalked = 1;
            } else {
                window.print(this.REAL_W_01, 0);
                ST1800.waitPage(window, 64);
                this.npc7btalked = 0;
            }
        } else {
            window.print(this.REAL_W_02, 0);
            ST1800.waitPage(window, 64);
        }
    }

    public void TalkNPC7a(Enepc enepc, Window window) {
        window.print(this.K07_00, 0);
        System.waitFor(window);
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
        this.fade1.call(0);
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
        this.fade.args[1] = 60;
        this.fade.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 30;
        this.fade1.args[2] = 0;
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
        this.npc1 = new NPC_NORMAL(783, 11, 0, 3, 9, 1.0f, 2.55f, -14.7f, -90.0f);
        this.npc2 = new NPC_NORMAL(1543, 12, 0, 3, 32, -6.324f, 0.0f, -2.282f, -135.0f);
        this.npc3 = new NPC_NORMAL(1540, 13, 0, 3, 12, -8.7f, 0.0f, 6.62f, 90.0f);
        this.npc5 = new NPC_NORMAL(527, 15, 0, 3, 16, -6.568f, 0.0f, 10.833f, 62.859f);
        this.npc6 = new NPC_NORMAL(780, 16, 0, 3, 14, 5.316f, 0.0f, 3.487f, 164.5f);
        this.npc7 = new NPC_NORMAL(783, 17, 0, 3, 9, 6.514f, 0.0f, 3.544f, 192.8f);
        this.npc1.talkto("TalkNPC1");
        this.npc5.talkto("TalkNPC5");
        if (Runtime.getFlags(346, 1) == 0) {
            if (Runtime.getFlags(304, 1) == 0) {
                this.npc2.talkto("TalkNPC2xx");
            } else {
                this.npc2.talkto("TalkNPC2");
            }
            this.npc6.talkto("TalkNPC6");
            this.npc7.talkto("TalkNPC7");
        } else {
            this.npc2.talkto("TalkNPC2a");
            this.npc3.talkto("TalkNPC3a");
            this.npc6.talkto("TalkNPC6a");
            this.npc7.talkto("TalkNPC7a");
        }
        if (Runtime.getFlags(310, 1) == 0) {
            this.npc3.talkto("TalkNPC3");
        } else {
            this.npc3.talkto("TalkNPC3a");
        }
        this.npc1.disableDTKFlag(131074);
        this.npc2.disableDTKFlag(131075);
        this.npc3.disableDTKFlag(131075);
        this.npc5.disableDTKFlag(131075);
        this.npc6.disableDTKFlag(131078);
        this.npc7.disableDTKFlag(131074);
        this.npc1.enableDTKFlag(8);
        this.npc2.enableDTKFlag(8);
        this.npc3.enableDTKFlag(8);
        this.npc5.enableDTKFlag(8);
        this.npc6.enableDTKFlag(8);
        this.npc7.enableDTKFlag(8);
        this.npc1.enableDTKFlag(4);
        this.npc2.enableDTKFlag(4);
        this.npc3.enableDTKFlag(4);
        this.npc5.enableDTKFlag(4);
        this.npc7.enableDTKFlag(4);
        this.npc3.setInvalidID(1);
        this.npc5.setInvalidID(1);
        this.npc6.setInvalidID(1);
        this.npc7.setInvalidID(1);
        this.npc2.setMotion(0, 27);
        this.npc3.setMotion(0, 2);
        this.npc5.setMotion(0, 1);
        this.npc6.setMotion(0, 10);
        this.npc7.setMotion(0, 10);
        this.npc6.look_char(this.npc7);
        this.npc7.look_char(this.npc6);
        if (Runtime.getFlags(310, 1) == 1 && Runtime.getFlags(3134, 1) == 0) {
            this.jr = new NPC_NORMAL(5, 20, 0, 3, 27, -2.992f, 2.549f, -16.7f, 15.0f);
            this.jr.talkto("TalkJR");
            this.jr.disableDTKFlag(131082);
            this.jr.enableDTKFlag(4);
            this.jr.setInvalidID(1);
            this.allen = new NPC_NORMAL(263, 21, 0, 3, 28, -0.9f, 2.549f, -17.373f, -15.0f);
            this.allen.talkto("TalkALLEN");
            this.allen.disableDTKFlag(131082);
            this.allen.enableDTKFlag(4);
            this.allen.setInvalidID(1);
            this.ziggy = new NPC_NORMAL(6, 22, 0, 3, 32, -0.881f, 0.0f, -2.826f, -30.0f);
            this.ziggy.talkto("TalkZIGGY");
            this.ziggy.disableDTKFlag(131082);
            this.ziggy.enableDTKFlag(4);
            this.ziggy.setInvalidID(1);
            this.chaos = new NPC_NORMAL(3, 23, 0, 3, 33, -3.367f, 2.549f, -14.591f, 160.0f);
            this.chaos.talkto("TalkCHAOS");
            this.chaos.disableDTKFlag(131082);
            this.chaos.enableDTKFlag(4);
            this.chaos.setInvalidID(1);
            this.momo = new NPC_NORMAL(4, 24, 0, 3, 34, 0.457f, 2.549f, -17.051f, -60.0f);
            this.momo.talkto("TalkMOMO");
            this.momo.disableDTKFlag(131082);
            this.momo.enableDTKFlag(4);
            this.momo.setInvalidID(1);
            this.kosmos = new NPC_NORMAL(2, 25, 0, 3, 35, -1.648f, 0.0f, 0.668f, -97.0f);
            this.kosmos.talkto("TalkKOSMOS");
            this.kosmos.disableDTKFlag(131082);
            this.kosmos.enableDTKFlag(4);
            this.kosmos.setMotion(0, 27);
            this.kosmos.setInvalidID(1);
        }
        if (Runtime.getFlags(314, 1) == 1 && Runtime.getFlags(315, 1) == 0) {
            if (Runtime.getFlags(7123, 2) == 1) {
                this.mat = new NPC_NORMAL(275, 26, 0, 3, 46, 11.3f, 0.0f, 5.0f, -90.0f);
                this.mat.talkto("TalkMATa");
                this.mat.disableDTKFlag(131082);
                this.mat.enableDTKFlag(4);
                this.mat.setMotion(0, 27);
                this.mat.setInvalidID(1);
            } else {
                this.mat = new NPC_NORMAL(275, 26, 0, 3, 46, 8.5f, 0.0f, 4.5f, 0.0f);
                this.mat.talkto("TalkMATa");
                this.mat.disableDTKFlag(131082);
                this.mat.enableDTKFlag(4);
                this.mat.setMotion(0, 27);
                this.mat.setInvalidID(1);
            }
        }
        this.npc1.kickEnepc(19, 1, 0, 420, 1);
        this.npc1.kickEnepc(19, 2, 0, 700, 1);
        this.EXP0 = new Enepc();
        this.EXP0.init(20492, 5, -13.0f, -1.0f, -1.0f, 0.0f);
        this.EXP0.id = 31;
        this.EXP0.setParams(0, 0, 31, 5);
        this.EXP0.setInvalidID(1);
        this.EXP0.dispRadar(false);
        this.EXP1 = new Enepc();
        this.EXP1.init(20621, 31, 4.5f, 4.5f, -21.6f, 90.0f);
        this.EXP1.id = 32;
        this.EXP1.setParams(0, 0, 32, 31);
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
        if (Runtime.getFlags(310, 1) == 1) {
            if (Runtime.getFlags(3134, 1) == 0) {
                this.doorC = new Uwamono(108, 40, '\u0001');
                this.doorC.SetDoorType('\u0002');
            } else {
                this.doorC = new Uwamono(108, 40, '\u0001');
                this.doorC.SetDoorType('\u0001');
            }
        } else {
            this.doorC = new Uwamono(108, 40, '\u0001');
            this.doorC.SetDoorType('\u0001');
        }
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
            ST1800.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 30) {
                    ST1800.this.Step.setTranslate(ST1800.this.Step.px + 0.033333335f, ST1800.this.Step.py, ST1800.this.Step.pz);
                }
                if (n == 30) break;
                ++n;
                System.sleep(1);
            }
        }

        void Down() {
            int n = 0;
            ST1800.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 120) {
                    ST1800.this.EXP1.setTranslate(ST1800.this.EXP1.px, ST1800.this.EXP1.py - 0.045833334f, ST1800.this.EXP1.pz);
                }
                if (n == 149) {
                    ST1800.this.player.setTranslate(6.0f, 4.95f, -22.0f);
                }
                if (n == 150) {
                    ST1800.this.EXP1.kickEnepc(0, 1);
                }
                if (n == 195) break;
                ++n;
                System.sleep(1);
            }
            ST1800.this.EXP1.kickEnepc(1, 2);
        }

        void Evt() {
            System.println("22222222222222222222222222222222222222222222222222222");
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1800.this.EXP0.setTranslate(-13.0f, -1.0f, -45.0f);
            ST1800.this.cam0.setMode(-1);
            Sound.effectPlay(196742);
            ST1800.this.EV_Camera01();
            System.sleep(1);
            ST1800.this.EXP0.kickEnepc(4, 1);
            ST1800.this.win = Window.create();
            ST1800.this.win.setSize(4, 45);
            ST1800.this.win.setLocation(15, 305);
            ST1800.this.win.print("Dock");
            int n = 0;
            int n2 = 150;
            float f = -45.0f;
            float f2 = (-1.4f - f) / (float) n2 / (float) n2;
            while (n < n2) {
                float f3 = 2.0f * f2 * (float) (n2 - n);
                ST1800.this.EXP0.getTranslate();
                ST1800.this.EXP0.setTranslate(ST1800.this.EXP0.px, ST1800.this.EXP0.py, ST1800.this.EXP0.pz + f3);
                ++n;
                System.sleep(1);
            }
            ST1800.this.win.close();
            ST1800.this.player.setLocation(1, 2);
            ST1800.this.EXP0.kickEnepc(0, 1);
            ST1800.this.Step.start(1, "Nobi");
            System.sleep(30);
            ST1800.this.doorB.DoorOpen();
            System.sleep(15);
            ST1800.this.EXP0.kickEnepc(3, 1, 45, 45, 1, 100);
            Runtime.enable(65536);
            ST1800.this.player.mtn(2, 9, 1.0f, true);
            ST1800.this.player.move(90, -7.0f, -1.0f, true);
            System.sleep(30);
            ST1800.this.Step.start(1, "Chijimi");
            ST1800.this.EXP0.kickEnepc(0, 2);
            System.sleep(45);
            ST1800.this.EXP0.kickEnepc(4, 0);
            ST1800.this.EXP0.kickEnepc(4, 2);
            ST1800.this.doorB.DoorClose();
            System.sleep(15);
            Runtime.disable(65536);
            System.sleep(15);
            ST1800.this.cam0.setMode(0);
            Runtime.setFlags(3069, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt2() {
            Runtime.disable(524288);
            ST1800.this.player.setTranslate(100.0f, 100.0f, 100.0f);
            Runtime.setPlayerControl(false);
            ST1800.this.EXP1.setTranslate(4.5f, 10.0f, -21.6f);
            ST1800.this.cam0.setMode(-1);
            ST1800.this.EV_Camera02();
            Sound.streamPlay(1195009, 48000);
            System.sleep(30);
            ST1800.this.EXP1.kickEnepc(4, 1);
            ST1800.this.EXP1.getTranslate();
            this.Down();
            ST1800.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST1800.this.player.mtn(2, 9, 1.0f, true);
            ST1800.this.player.move(60, 6.0f, -20.0f, true);
            System.sleep(60);
            Runtime.disable(65536);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Evt3() {
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST1800.this.cam0.setMode(-1);
            ST1800.this.EV_Camera02();
            System.sleep(30);
            ST1800.this.EXP1.kickEnepc(4, 1);
            ST1800.this.EXP1.getTranslate();
            this.Up();
            System.sleep(120);
            Runtime.disable(65536);
            ST1800.this.cam0.setMode(0);
            Runtime.setFlags(3108, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }

        void Nobi() {
            int n = 0;
            ST1800.this.Step.getTranslate();
            while (true) {
                if (n >= 0 && n < 30) {
                    ST1800.this.Step.setTranslate(ST1800.this.Step.px - 0.033333335f, ST1800.this.Step.py, ST1800.this.Step.pz);
                }
                if (n == 30) break;
                ++n;
                System.sleep(1);
            }
        }

        void Up() {
            int n = 0;
            ST1800.this.Step.getTranslate();
            while (true) {
                if (n == 0) {
                    ST1800.this.EXP1.kickEnepc(0, 0);
                }
                if (n == 45) {
                    ST1800.this.EXP1.kickEnepc(1, 3);
                }
                if (n == 75) {
                    ST1800.this.player.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n >= 75 && n < 195) {
                    ST1800.this.EXP1.setTranslate(ST1800.this.EXP1.px, ST1800.this.EXP1.py + 0.083333336f, ST1800.this.EXP1.pz);
                }
                if (n == 195) {
                    ST1800.this.EXP1.kickEnepc(4, 0);
                    Runtime.setPlayerControl(true);
                    if (Runtime.getFlags(310, 1) == 1 && Runtime.getFlags(311, 1) == 0) {
                        ST1800.this.fade1.call(0);
                        System.sleep(30);
                        Runtime.setFlags(311, 1, 1);
                        Runtime.jumpEvent(3060);
                        break;
                    }
                    ST1800.this.fade1.call(0);
                    System.sleep(30);
                    Runtime.setFlags(7138, 1, 1);
                    Runtime.jumpCF(2160, 2);
                    break;
                }
                ++n;
                System.sleep(1);
            }
        }
    }
}

