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
import xeno.map.MC_DYU04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1740
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU04_PRJ {
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
    Enepc police;
    Enepc hyaku;
    Enepc crew;
    Enepc people;
    Enepc sold;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit ring_A;
    Unit ring_B;
    Unit ring_C;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
    int npc3talked = 0;
    int npc3btalked = 0;
    int npc4talked = 0;
    int npc4btalked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    int T1 = 0;
    int T2 = 0;
    int T3 = 0;
    int T4 = 0;
    int T5 = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono saveA;
    Uwamono itembox;
    Light light = new Light(0);
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
    String[] PW_00 = new String[]{"Hmm? What?! You got a problem with a policewoman being here?", "/[waitkey(1)]/[clear()]", "I'm human too. You have to gamble a little in your life.", "/[waitkey(64)]/[close()]"};
    String[] PW_01 = new String[]{"Well, life's just a repetition of winning and losing. In my case, I just keep losing, though.", "/[waitkey(64)]/[close()]"};
    String[] PW_02 = new String[]{"Oh, it's you, Jr.", "/[waitkey(1)]/[clear()]", "What? Here to give me a piece of your mind? I'm free to do what I want during my breaks!", "/[waitkey(64)]/[close()]"};
    String[] PW_03 = new String[]{"You're probably just partying with the money you took from me, right?", "/[waitkey(1)]/[clear()]", "I'm increasing your allowance right now, so be quiet and watch.", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_00 = new String[]{"That's strange...I'm doing everything the way Little Master taught me, but I can't win at all.", "/[waitkey(1)]/[clear()]", "Am I doing something wrong?", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_01 = new String[]{"Hmm, I have no idea at all. Little Master said it would be good training, but I wonder if it's true.", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_02 = new String[]{"Oh, Little Master.", "/[waitkey(1)]/[clear()]", "I'm doing it just like you taught me, but I can't win at all. Is something wrong with me?", "/[waitkey(64)]/[close()]"};
    String[] CREW_00 = new String[]{"Damn, I can't win! That 100-Series's probably using some awesome powers to win like crazy!", "/[waitkey(64)]/[close()]", "She should share the wealth, damn it!", "/[waitkey(64)]/[close()]"};
    String[] CREW_PM_00 = new String[]{"You wanna hear the start of my life as a gambler?", "/[waitkey(64)]/[close()]"};
    String[] CREW_PM_01 = new String[]{"You should listen to me, it might come in handy someday.", "/[waitkey(64)]/[close()]"};
    String[] CREW_PM_02 = new String[]{"Ah, good of you to ask!!", "/[waitkey(1)]/[clear()]", "Well, actually, thinking back, I think it was about 14 years ago. I was still a kid back then! I was living in Miltia at the time and I used to go play at Miltia Park a lot.", "/[waitkey(1)]/[clear()]", "Then one day, I was playing at the pink dome in Miltia Park, when I lost a precious card!!", "/[waitkey(1)]/[clear()]", "And to this day, I'm in casinos trying to get that card back.", "/[waitkey(1)]/[clear()]", "Oh, back then, I was little, so I could get inside the dome.", "/[waitkey(64)]/[close()]"};
    String[] CREW_01 = new String[]{"Oh, Little Master?! I-I am currently making rounds around the ship, I was definitely not slacking off.", "/[waitkey(64)]/[close()]"};
    String[] CREW_02 = new String[]{"Umm...by the way, would you lend me a 100-Series next time?", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_00 = new String[]{"Heh heh, heh heh heh! Heh heh heh heh heh!", "/[waitkey(1)]/[clear()]", "Oops, are we starting a game? Luck is everything when it comes to these things. Good luck to the both of us.", "/[waitkey(1)]/[clear()]", "Heh heh, heh heh heh!", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_01 = new String[]{"Man, this is why I can't quit gambling!!", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_02 = new String[]{"Hehe, hehehehehe! Oh, Little Master?! Thank you so much for always letting me make money off of you!", "/[waitkey(64)]/[close()]"};
    String[] PEOPLE_03 = new String[]{"Well, a game is a game, no grudges now. Hehehehe, hehehehehe!", "/[waitkey(64)]/[close()]"};
    String[] S01_00 = new String[]{"I heard things got pretty sketchy there for a while.", "/[waitkey(1)]/[clear()]", "Well, this place was totally peaceful, so I didn't notice anything.", "/[waitkey(64)]/[close()]"};
    String[] S01_01 = new String[]{"The commotion's died down, so it's okay now, right? Now I can concentrate on the game again!", "/[waitkey(64)]/[close()]"};
    String[] S02_00 = new String[]{"It seems the military people are gone, but that soldier there won't leave.", "/[waitkey(64)]/[close()]"};
    String[] S02_01 = new String[]{"Oh, but it's okay. I'll take the responsibility of keeping an eye on him!", "/[waitkey(64)]/[close()]"};
    String[] S03_00 = new String[]{"Allen? Who's that? I don't know any Allen! Anyway, what's with that soldier?!", "/[waitkey(64)]/[close()]"};
    String[] S03_01 = new String[]{"If he's done with his work, he should just hurry up and go home!", "/[waitkey(1)]/[clear()]", "Just because he's had a good run, doesn't mean he should show off!!", "/[waitkey(1)]/[clear()]", "That spot was mine to begin with!!", "/[waitkey(64)]/[close()]"};
    String[] S04_00 = new String[]{"Heh, heh, heh! Not good, this is not good!", "/[waitkey(1)]/[clear()]", "At this rate, I'm going to have all the coins on this ship to myself.", "/[waitkey(1)]/[clear()]", "I better stop soon, or Gaignun will be very angry with me!", "/[waitkey(64)]/[close()]"};
    String[] S05_00 = new String[]{"Man, this is great! It just keeps coming and coming! This is no time for work!", "/[waitkey(64)]/[close()]"};
    String[] S05_01 = new String[]{"Hey! You guys are supposed to be confined, so don't wander around too much!!", "/[waitkey(64)]/[close()]"};

    ST1740() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1btalked == 0) {
                window.print(this.PW_02, 0);
                System.waitFor(window);
                this.npc1btalked = 1;
            } else {
                window.print(this.PW_03, 0);
                System.waitFor(window);
                this.npc1btalked = 0;
            }
        } else if (this.npc1talked == 0) {
            window.print(this.PW_00, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.PW_01, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (this.T1 == 0) {
            window.print(this.S01_00, 0);
            System.waitFor(window);
            this.T1 = 1;
        } else {
            window.print(this.S01_01, 0);
            System.waitFor(window);
            this.T1 = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.HYAKU_02, 0);
            System.waitFor(window);
        } else if (this.npc2talked == 0) {
            window.print(this.HYAKU_00, 0);
            System.waitFor(window);
            this.npc2talked = 1;
        } else {
            window.print(this.HYAKU_01, 0);
            System.waitFor(window);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (this.T2 == 0) {
            window.print(this.S02_00, 0);
            System.waitFor(window);
            this.T2 = 1;
        } else {
            window.print(this.S02_01, 0);
            System.waitFor(window);
            this.T2 = 0;
        }
    }

    public void TalkNPC3(Enepc enepc) {
        if (Runtime.getLeader() == 5) {
            if (this.npc3btalked == 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.CREW_01, 0);
                System.waitFor(this.win);
                this.npc3btalked = 1;
                Runtime.setPlayerControl(true);
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.CREW_02, 0);
                System.waitFor(this.win);
                this.npc3btalked = 0;
                Runtime.setPlayerControl(true);
            }
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.CREW_PM_00, 0);
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
                    this.win.print(this.CREW_PM_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    break;
                }
                default: {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.CREW_PM_01, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (this.T3 == 0) {
            window.print(this.S03_00, 0);
            System.waitFor(window);
            this.T3 = 1;
        } else {
            window.print(this.S03_01, 0);
            System.waitFor(window);
            this.T3 = 0;
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc4btalked == 0) {
                window.print(this.PEOPLE_02, 0);
                System.waitFor(window);
                this.npc4btalked = 1;
            } else {
                window.print(this.PEOPLE_03, 0);
                System.waitFor(window);
                this.npc4btalked = 0;
            }
        } else if (this.npc4talked == 0) {
            window.print(this.PEOPLE_00, 0);
            System.waitFor(window);
            this.npc4talked = 1;
        } else {
            window.print(this.PEOPLE_01, 0);
            System.waitFor(window);
            this.npc4talked = 0;
        }
    }

    public void TalkNPC4a(Enepc enepc, Window window) {
        window.print(this.S04_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (this.T5 == 0) {
            window.print(this.S05_00, 0);
            System.waitFor(window);
            this.T5 = 1;
        } else {
            window.print(this.S05_01, 0);
            System.waitFor(window);
            this.T5 = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1750, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -9.85f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196613);
        this.teiten2 = new Uwamono(28690, 2.5f, 0.0f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196614);
        Stage.setVisible(-1, true);
        this.ring_A = new Mapunits();
        this.ring_A.mapUnit(274);
        this.ring_A.start(4, null);
        this.ring_A.start(1, "L1");
        this.ring_B = new Mapunits();
        this.ring_B.mapUnit(272);
        this.ring_B.start(4, null);
        this.ring_B.start(1, "R1");
        this.ring_C = new Mapunits();
        this.ring_C.mapUnit(273);
        this.ring_C.start(4, null);
        this.ring_C.start(1, "L2");
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.55f, 0.6f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.55f, 0.6f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, -9.852f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 6.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.police = new NPC_NORMAL(1289, 11, 0, 1, 11, 6.303f, 2.6f, 1.583f, 195.0f);
        this.hyaku = new NPC_NORMAL(1028, 12, 0, 2, 3, 4.419f, 2.6f, -1.275f, 0.0f);
        this.crew = new NPC_NORMAL(1025, 13, 0, 3, 7, 0.537f, 0.0f, 0.624f, -15.0f);
        this.people = new NPC_NORMAL(1540, 14, 0, 4, 10, -14.42f, 0.0f, -2.88f, -100.0f);
        if (Runtime.getFlags(346, 1) == 0) {
            this.police.talkto("TalkNPC1");
            this.hyaku.talkto("TalkNPC2");
            this.crew.talkto("TalkNPC3");
            this.people.talkto("TalkNPC4");
        } else {
            this.police.talkto("TalkNPC1a");
            this.hyaku.talkto("TalkNPC2a");
            this.crew.talkto("TalkNPC3a");
            this.people.talkto("TalkNPC4a");
        }
        this.police.disableDTKFlag(131075);
        this.hyaku.disableDTKFlag(131074);
        this.crew.disableDTKFlag(131074);
        this.people.disableDTKFlag(131075);
        this.police.enableDTKFlag(12);
        this.hyaku.enableDTKFlag(12);
        this.crew.enableDTKFlag(12);
        this.people.enableDTKFlag(12);
        this.police.setInvalidID(1);
        this.people.setInvalidID(1);
        this.police.setMotion(0, 7);
        this.people.setMotion(0, 4);
        if (Runtime.getFlags(346, 1) == 1) {
            this.sold = new NPC_NORMAL(526, 15, 0, 3, 17, -6.811f, 0.0f, 1.74f, 0.0f);
            this.sold.talkto("TalkNPC5");
            this.sold.disableDTKFlag(131074);
            this.sold.enableDTKFlag(12);
        }
        this.doorA = new Uwamono(192, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.itembox = new Uwamono(28677, 11.5f, 2.6f, 0.0f, 90.0f, 408);
        this.itembox.SetSymbol(28683);
        this.saveA = new Uwamono(28733, -12.5f, 0.0f, -3.0f);
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

        void L1() {
            while (true) {
                this.setRotate(this.rx, this.ry - 0.5f, this.rz);
                System.sleep(1);
            }
        }

        void L2() {
            while (true) {
                this.setRotate(this.rx, this.ry - 1.5f, this.rz);
                System.sleep(1);
            }
        }

        void R1() {
            while (true) {
                this.setRotate(this.rx, this.ry + 1.0f, this.rz);
                System.sleep(1);
            }
        }
    }
}

