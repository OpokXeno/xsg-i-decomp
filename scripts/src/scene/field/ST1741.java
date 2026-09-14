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

class ST1741
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
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc06;
    Enepc npc07;
    Enepc npc08;
    Enepc npc09;
    Enepc npc10;
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
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc11talked = 0;
    int npc22talked = 0;
    int npc33talked = 0;
    int npc44talked = 0;
    int npc55talked = 0;
    int npc06talked = 0;
    int npc07talked = 0;
    int npc08talked = 0;
    int npc09talked = 0;
    int npc10talked = 0;
    int npc0606talked = 0;
    int npc0707talked = 0;
    int npc0808talked = 0;
    int npc0909talked = 0;
    int npc1010talked = 0;
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
    String[] S01_00CN = new String[]{"Say, there's a lot of commotion going on outside. Did something happen? Well, not that I really care.", "/[waitkey(64)]/[close()]"};
    String[] S01_01CN = new String[]{"I'm concentrating right now, so don't talk to me.", "/[waitkey(64)]/[close()]"};
    String[] S01_00CJ = new String[]{"Say, there's a lot of commotion going on outside. Did something happen? Well, not that I really care.", "/[waitkey(64)]/[close()]"};
    String[] S01_01CJ = new String[]{"I'm concentrating right now, so don't talk to me.", "/[waitkey(64)]/[close()]"};
    String[] S02_00CN = new String[]{"Oh, I just saw soldiers take away some people outside.", "/[waitkey(1)]/[clear()]", "My scan results show that the majority of the people on the ship have been captured by soldiers.", "/[waitkey(64)]/[close()]"};
    String[] S02_01CN = new String[]{"Some people from the Federation army came in here just a while ago too.", "/[waitkey(1)]/[clear()]", "But they started a game eventually after wandering around. What did they come here for?", "/[waitkey(64)]/[close()]"};
    String[] S02_00CJ = new String[]{"Oh, Little Master!", "/[waitkey(1)]/[clear()]", "Has everyone been caught? Is everything all right?", "/[waitkey(64)]/[close()]"};
    String[] S02_01CJ = new String[]{"Wait, is that soldier a bad guy too?", "/[waitkey(64)]/[close()]"};
    String[] S03_00CN = new String[]{"Damn it!! That soldier took my spot!", "/[waitkey(1)]/[clear()]", "That was today's lucky spot!", "/[waitkey(1)]/[clear()]", "A newbie should play like a newbie and just stay in a corner!", "/[waitkey(64)]/[close()]"};
    String[] S03_00CJ = new String[]{"Oh, Little Master!", "/[waitkey(1)]/[clear()]", "Please say something to that soldier there! Why did you hire Federation soldiers anyway?", "/[waitkey(64)]/[close()]"};
    String[] S04_00CN = new String[]{"Heh heh, heh heh heh! Heh heh heh heh heh! ", "Man, I just can't stop.", "/[waitkey(1)]/[clear()]", "Looks like that soldier's doing good too. Gambling is\nsuch a wonderful thing! ", "/[waitkey(1)]/[clear()]", "Heh heh, heh heh heh! Heh heh heh heh heh!", "/[waitkey(64)]/[close()]"};
    String[] S04_00CJ = new String[]{"Heh heh, heh heh heh! Heh heh heh heh heh! ", "Man, I just can't stop.", "/[waitkey(1)]/[clear()]", "Well, hello there, Little Master! Just look at how I'm doing!", "/[waitkey(1)]/[clear()]", "No one can stop me now! ", "Heh heh, heh heh heh!", "/[waitkey(64)]/[close()]"};
    String[] S05_00CN = new String[]{"Hmm? Shoot, have I been spotted?", "/[waitkey(1)]/[clear()]", "I'll have you know, it's not like I'm playing.", "/[waitkey(1)]/[clear()]", "I'm keeping an eye on the people here!", "/[waitkey(64)]/[close()]"};
    String[] S05_01CN = new String[]{"Would you shut up for a second? I'm in the zone right now!", "/[waitkey(64)]/[close()]"};
    String[] S05_00CJ = new String[]{"Hmm? Shoot, have I been spotted?", "/[waitkey(1)]/[clear()]", "I'll have you know, it's not like I'm playing.", "/[waitkey(1)]/[clear()]", "I'm keeping an eye on the people here!", "/[waitkey(64)]/[close()]"};
    String[] S05_01CJ = new String[]{"Would you shut up for a second? I'm in the zone right now!", "/[waitkey(64)]/[close()]"};
    String[] O1_00CJ = new String[]{"Hey! This is no place for a kid like you to come to!!", "/[waitkey(1)]/[clear()]", "Oh, it's you, Little Master!", "/[waitkey(1)]/[clear()]", "Quit killing time here and do something about the military out front!!", "/[waitkey(64)]/[close()]"};
    String[] O1_01CJ = new String[]{"Taking refuge in a casino means all I do is lose money. It's not restful at all!!", "/[waitkey(1)]/[clear()]", "Can't you at least turn this casino into a place where you can win during emergencies?", "/[waitkey(64)]/[close()]"};
    String[] O1_00CN = new String[]{"That's why I should have thrown in more back then!", "/[waitkey(1)]/[clear()]", "I got scared and left my seat, so the guy who sat there later made off with everything that I put down!", "/[waitkey(64)]/[close()]"};
    String[] O1_01CN = new String[]{"It's a tough world.", "/[waitkey(1)]/[clear()]", "Some nobody makes off with my money from right under my nose.", "/[waitkey(64)]/[close()]"};
    String[] O2_00CJ = new String[]{"Little Master? Don't tell me you're here to earn a bit of cash too?!", "/[waitkey(1)]/[clear()]", "Ha ha ha, no way, right?", "/[waitkey(1)]/[clear()]", "If you did that, the Foundation's cash flow would come to a halt.", "/[waitkey(64)]/[close()]"};
    String[] O2_01CJ = new String[]{"By the way, Little Master, the proceeds of this place must be quite a bit, right? What in the world do you use it for?", "/[waitkey(64)]/[close()]"};
    String[] O2_00CN = new String[]{"You should buy a rabbit necklace too!!", "/[waitkey(1)]/[clear()]", "Luck is sure to come your way as long as you have that!!", "/[waitkey(64)]/[close()]"};
    String[] O2_01CN = new String[]{"My friend bought a rabbit necklace, and thanks to that, he's lucky like crazy!!", "/[waitkey(1)]/[clear()]", "Got promoted at work, got a girlfriend, and on top of all that, struck oil!!", "/[waitkey(64)]/[close()]"};
    String[] J10_00CJ = new String[]{"Little Master...I'm washing my hands of gambling after this month.", "/[waitkey(64)]/[close()]"};
    String[] J10_01CJ = new String[]{"Then again, I said that last month too, didn't I?", "/[waitkey(64)]/[close()]"};
    String[] J10_00CN = new String[]{"*Sniffle*\n", "I lost again today.", "/[waitkey(1)]/[clear()]", "With that, this month's total losses come out to more than my monthly salary.", "/[waitkey(64)]/[close()]"};
    String[] J10_01CN = new String[]{"Why is that?", "/[waitkey(1)]/[clear()]", "I guess my luck ended when I tried to gamble to win back what I lost by gambling in the first place.", "/[waitkey(64)]/[close()]"};
    String[] J09_00CJ = new String[]{"Well, in the beginning, I really thought of it as just a game.", "/[waitkey(64)]/[close()]"};
    String[] J09_01CJ = new String[]{"But then I realized, I was earning a lot more than my salary.", "/[waitkey(64)]/[close()]"};
    String[] J09_00CN = new String[]{"Oh? You love to gamble too?", "/[waitkey(64)]/[close()]"};
    String[] J09_01CN = new String[]{"As Little Master always says, \"There are no bad guys among gamblers!\"", "/[waitkey(64)]/[close()]"};
    String[] J07_00CJ = new String[]{"Oh!! Uh, um, uh, um!! You got it all wrong! I'm not slacking off or anything!!", "/[waitkey(64)]/[close()]"};
    String[] J07_01CJ = new String[]{"Oh, please, I live to maintain machines. I would never skip work to go gambling!", "/[waitkey(64)]/[close()]"};
    String[] J07_00CN = new String[]{"Don't forget to take things easy, even during an emergency! That's my motto!!", "/[waitkey(64)]/[close()]"};
    String[] J07_01CN = new String[]{"But I gotta be on the lookout for Little Master. He comes to play here sometimes too.", "/[waitkey(64)]/[close()]"};

    ST1741() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void TalkNPC07(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc07talked == 0) {
                window.print(this.J07_00CJ, 0);
                System.waitFor(window);
                this.npc07talked = 1;
            } else {
                window.print(this.J07_01CJ, 0);
                System.waitFor(window);
                this.npc07talked = 0;
            }
        } else if (this.npc0707talked == 0) {
            window.print(this.J07_00CN, 0);
            System.waitFor(window);
            this.npc0707talked = 1;
        } else {
            window.print(this.J07_01CN, 0);
            System.waitFor(window);
            this.npc0707talked = 0;
        }
    }

    public void TalkNPC09(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc09talked == 0) {
                window.print(this.J09_00CJ, 0);
                System.waitFor(window);
                this.npc09talked = 1;
            } else {
                window.print(this.J09_01CJ, 0);
                System.waitFor(window);
                this.npc09talked = 0;
            }
        } else if (this.npc0909talked == 0) {
            window.print(this.J09_00CN, 0);
            System.waitFor(window);
            this.npc0909talked = 1;
        } else {
            window.print(this.J09_01CN, 0);
            System.waitFor(window);
            this.npc0909talked = 0;
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1talked == 0) {
                window.print(this.S01_00CJ, 0);
                System.waitFor(window);
                this.npc1talked = 1;
            } else {
                window.print(this.S01_01CJ, 0);
                System.waitFor(window);
                this.npc1talked = 0;
            }
        } else if (this.npc11talked == 0) {
            window.print(this.S01_00CN, 0);
            System.waitFor(window);
            this.npc11talked = 1;
        } else {
            window.print(this.S01_01CN, 0);
            System.waitFor(window);
            this.npc11talked = 0;
        }
    }

    public void TalkNPC10(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc10talked == 0) {
                window.print(this.J10_00CJ, 0);
                System.waitFor(window);
                this.npc10talked = 1;
            } else {
                window.print(this.J10_01CJ, 0);
                System.waitFor(window);
                this.npc10talked = 0;
            }
        } else if (this.npc1010talked == 0) {
            window.print(this.J10_00CN, 0);
            System.waitFor(window);
            this.npc1010talked = 1;
        } else {
            window.print(this.J10_01CN, 0);
            System.waitFor(window);
            this.npc1010talked = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2talked == 0) {
                window.print(this.S02_00CJ, 0);
                System.waitFor(window);
                this.npc2talked = 1;
            } else {
                window.print(this.S02_01CJ, 0);
                System.waitFor(window);
                this.npc2talked = 0;
            }
        } else if (this.npc22talked == 0) {
            window.print(this.S02_00CN, 0);
            System.waitFor(window);
            this.npc22talked = 1;
        } else {
            window.print(this.S02_01CN, 0);
            System.waitFor(window);
            this.npc22talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.S03_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.S03_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.S04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.S04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc5talked == 0) {
                window.print(this.S05_00CJ, 0);
                System.waitFor(window);
                this.npc5talked = 1;
            } else {
                window.print(this.S05_01CJ, 0);
                System.waitFor(window);
                this.npc5talked = 0;
            }
        } else if (this.npc55talked == 0) {
            window.print(this.S05_00CN, 0);
            System.waitFor(window);
            this.npc55talked = 1;
        } else {
            window.print(this.S05_01CN, 0);
            System.waitFor(window);
            this.npc55talked = 0;
        }
    }

    public void TalkNPC6(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc06talked == 0) {
                window.print(this.O1_00CJ, 0);
                System.waitFor(window);
                this.npc06talked = 1;
            } else {
                window.print(this.O1_01CJ, 0);
                System.waitFor(window);
                this.npc06talked = 0;
            }
        } else if (this.npc0606talked == 0) {
            window.print(this.O1_00CN, 0);
            System.waitFor(window);
            this.npc0606talked = 1;
        } else {
            window.print(this.O1_01CN, 0);
            System.waitFor(window);
            this.npc0606talked = 0;
        }
    }

    public void TalkNPC8(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc08talked == 0) {
                window.print(this.O2_00CJ, 0);
                System.waitFor(window);
                this.npc08talked = 1;
            } else {
                window.print(this.O2_01CJ, 0);
                System.waitFor(window);
                this.npc08talked = 0;
            }
        } else if (this.npc0808talked == 0) {
            window.print(this.O2_00CN, 0);
            System.waitFor(window);
            this.npc0808talked = 1;
        } else {
            window.print(this.O2_01CN, 0);
            System.waitFor(window);
            this.npc0808talked = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1751, 3);
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
        this.hyaku = new NPC_NORMAL(1028, 12, 0, 2, 3, -5.416f, 0.0f, -0.937f, 0.0f);
        this.crew = new NPC_NORMAL(1025, 13, 0, 3, 7, -2.236f, 0.0f, 0.918f, 0.0f);
        this.people = new NPC_NORMAL(1541, 14, 0, 4, 19, 8.482f, 2.6f, 1.6f, 185.0f);
        this.sold = new NPC_NORMAL(526, 15, 0, 1, 19, 8.5f, 2.6f, -1.5f, -15.0f);
        this.police.talkto("TalkNPC1");
        this.hyaku.talkto("TalkNPC2");
        this.crew.talkto("TalkNPC3");
        this.people.talkto("TalkNPC4");
        this.sold.talkto("TalkNPC5");
        this.police.disableDTKFlag(131075);
        this.hyaku.disableDTKFlag(131074);
        this.crew.disableDTKFlag(131074);
        this.people.disableDTKFlag(131075);
        this.sold.disableDTKFlag(131075);
        this.police.enableDTKFlag(12);
        this.hyaku.enableDTKFlag(12);
        this.crew.enableDTKFlag(12);
        this.people.enableDTKFlag(12);
        this.sold.enableDTKFlag(12);
        this.police.setInvalidID(1);
        this.people.setInvalidID(1);
        this.sold.setInvalidID(1);
        this.police.setMotion(0, 7);
        this.people.setMotion(0, 5);
        this.sold.setMotion(0, 7);
        this.npc06 = new NPC_NORMAL(1547, 16, 0, 1, 26, -8.01f, 0.0f, -3.328f, 45.0f);
        this.npc06.setMotion(0, 27);
        this.npc06.disableDTKFlag(131074);
        this.npc06.enableDTKFlag(12);
        this.npc07 = new NPC_NORMAL(527, 17, 0, 3, 7, -12.2f, 0.0f, 2.75f, -15.0f);
        this.npc07.enableDTKFlag(12);
        this.npc08 = new NPC_NORMAL(1543, 18, 0, 1, 7, -7.45f, 0.0f, -2.51f, 225.0f);
        this.npc08.setMotion(0, 10);
        this.npc08.setMotion(3, 10);
        this.npc08.disableDTKFlag(131074);
        this.npc08.enableDTKFlag(12);
        this.npc09 = new NPC_NORMAL(780, 19, 0, 3, 7, -8.5f, 0.0f, 3.8f, -15.0f);
        this.npc09.enableDTKFlag(12);
        this.npc10 = new NPC_NORMAL(1540, 20, 0, 4, 10, -14.42f, 0.0f, -2.88f, -100.0f);
        this.npc10.setMotion(0, 4);
        this.npc10.disableDTKFlag(131075);
        this.npc10.enableDTKFlag(12);
        this.npc06.talkto("TalkNPC6");
        this.npc07.talkto("TalkNPC07");
        this.npc08.talkto("TalkNPC8");
        this.npc09.talkto("TalkNPC09");
        this.npc10.talkto("TalkNPC10");
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

