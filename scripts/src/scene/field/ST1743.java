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

class ST1743
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
    String[] K01_00CN = new String[]{"I hear that something terrible is going to happen if they don't destroy that big thing outside!", "/[waitkey(1)]/[clear()]", "Well, something terrible is going on here for me too.", "/[waitkey(1)]/[clear()]", "I have to at least get back enough for my train ticket home.", "/[waitkey(64)]/[close()]"};
    String[] K01_00CJ = new String[]{"Oh, Jr., do you have a second?", "/[waitkey(1)]/[clear()]", "Oh, I know you're busy. But please hear what I have to say! Pretty please?", "/[waitkey(64)]/[close()]"};
    String[] K01_01CJ = new String[]{"Would you lend me some money?", "/[waitkey(64)]/[close()]"};
    String[] K01_02CJ = new String[]{"Oh, please! I don't even have enough money to get home!", "/[waitkey(1)]/[clear()]", "Pretty please?", "/[waitkey(64)]/[close()]"};
    String[] K02_00CN = new String[]{"I did it! I did it, look!", "/[waitkey(1)]/[clear()]", "I followed the soldier's instructions and I'm winning like crazy!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CJ = new String[]{"Little Master! I did it!", "/[waitkey(1)]/[clear()]", "I'm perfect at counting too! I can graduate from training now, right?", "/[waitkey(64)]/[close()]"};
    String[] K03_00CN = new String[]{"I've had enough. I just wasn't lucky today.", "/[waitkey(1)]/[clear()]", "Ah, well, I just have to come back again tomorrow! I think I'll go home soon.", "/[waitkey(1)]/[clear()]", "Well, maybe I'll just play one more game before I go!", "/[waitkey(64)]/[close()]"};
    String[] K03_00CJ = new String[]{"I've had enough. I just wasn't lucky today.", "/[waitkey(1)]/[clear()]", "Ah, well, I just have to come back again tomorrow! I think I'll go home soon.", "/[waitkey(1)]/[clear()]", "Well, maybe I'll just play one more game before I go!", "/[waitkey(64)]/[close()]"};
    String[] K04_00CN = new String[]{"Man, it was good. Today has been a really good day!", "/[waitkey(1)]/[clear()]", "I feel like I've used up a lifetime's worth of luck.", "/[waitkey(1)]/[clear()]", "It's so great to be alive!", "/[waitkey(64)]/[close()]"};
    String[] K04_00CJ = new String[]{"Ohhh, Little Master. Today was good. I was proud of myself today!", "/[waitkey(1)]/[clear()]", "Please tell Gaignun that I said thanks for his business.", "/[waitkey(64)]/[close()]"};
    String[] K05_00CN = new String[]{"That 100-Series has skill. Well, I was probably a good teacher too.", "/[waitkey(64)]/[close()]"};
    String[] K05_01CN = new String[]{"What? Aren't I going back to the Federation fleet? Naw, I quit being a Marine!", "/[waitkey(1)]/[clear()]", "I'd much rather work on this ship!", "/[waitkey(1)]/[clear()]", "I get to play all I want when I'm assigned as security to this place!", "/[waitkey(64)]/[close()]"};
    String[] K05_00CJ = new String[]{"That 100-Series has skill. Well, I was probably a good teacher too.", "/[waitkey(64)]/[close()]"};
    String[] K05_01CJ = new String[]{"What? Aren't I going back to the Federation fleet? Naw, I quit being a Marine!", "/[waitkey(1)]/[clear()]", "I'd much rather work on this ship!", "/[waitkey(1)]/[clear()]", "I get to play all I want when I'm assigned as security to this place!", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "Right before final stage. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] T1_1 = new String[]{"Just when I thought that triangular thing went away...what's that bowl-shaped thing?", "/[waitkey(1)]/[clear()]", "Man, this is a busy ship.", "/[waitkey(64)]/[close()]"};
    String[] T1_2 = new String[]{"Because it's so busy, the Captain probably doesn't have time to play, right?", "/[waitkey(1)]/[clear()]", "Man, it must be a stressful job.", "/[waitkey(64)]/[close()]"};
    String[] T1_3 = new String[]{"Gamble until I run out of money! Gamble if I've got money!!", "/[waitkey(1)]/[clear()]", "I just love the way I am!!", "/[waitkey(64)]/[close()]"};
    String[] T1_4 = new String[]{"The people from the Foundation are supposed to take shelter here, right?!", "/[waitkey(1)]/[clear()]", "I was just dreaming about taking on the best gamblers in the universe...", "/[waitkey(64)]/[close()]"};
    String[] T2_1 = new String[]{"Oh!! Little Master? We meet again!", "/[waitkey(1)]/[clear()]", "Are you interested in me?", "/[waitkey(64)]/[close()]"};
    String[] T2_2 = new String[]{"I know! Yes, I know.", "/[waitkey(1)]/[clear()]", "But I can't reciprocate the same feelings for you right now.", "/[waitkey(64)]/[close()]"};
    String[] T2_3 = new String[]{"Man, what a real bind!", "/[waitkey(1)]/[clear()]", "I never thought Little Master would have feelings for me.", "/[waitkey(64)]/[close()]"};
    String[] T2_4 = new String[]{"Well, I don't think gender matters at all when it comes to love!", "/[waitkey(1)]/[clear()]", "But I'm not like that!", "/[waitkey(64)]/[close()]"};
    String[] T3_1 = new String[]{"Oh, Little Master?! One problem after another, huh?", "/[waitkey(1)]/[clear()]", "Please hang in there. I look forward to hearing some good news!", "/[waitkey(64)]/[close()]"};
    String[] T3_2 = new String[]{"I really do want to lend you a hand, but I've never been good at fighting ever since I was a kid.", "/[waitkey(64)]/[close()]"};
    String[] T3_3 = new String[]{"Hey, it's you guys again! We're counting on you!", "/[waitkey(64)]/[close()]"};
    String[] T3_4 = new String[]{"Hey, it's you guys again! We're counting on you!", "/[waitkey(64)]/[close()]"};
    String[] T4_1 = new String[]{"Some professor somewhere once said, \"God doesn't play dice.\"", "/[waitkey(64)]/[close()]"};
    String[] T4_2 = new String[]{"All I ever do is play with dice though.", "/[waitkey(64)]/[close()]"};
    String[] T4_3 = new String[]{"Yes, yes, I recently had a revelation.", "/[waitkey(1)]/[clear()]", "No matter how many people come from the Foundation, the crowds of the casino never change.", "/[waitkey(1)]/[clear()]", "To think that people who don't gamble will end their lives, never having known the beauty of gambling....", "/[waitkey(64)]/[close()]"};
    String[] T4_4 = new String[]{"In that sense, I'm really lucky!!", "/[waitkey(1)]/[clear()]", "I was introduced to gambling by knowing everything about the Durandal!", "/[waitkey(64)]/[close()]"};
    String[] T5_1 = new String[]{"Little Master! I decided to retract my petition to close down the casino!", "/[waitkey(64)]/[close()]"};
    String[] T5_2 = new String[]{"It sort of...just...became clear to me while I kept on hitting this spot!!", "/[waitkey(64)]/[close()]"};
    String[] T5_3 = new String[]{"A little more! Just a little bit longer, and I'll be on fire!", "/[waitkey(64)]/[close()]"};
    String[] T5_4 = new String[]{"Please stay and watch! I'm in the process of heating up.", "/[waitkey(1)]/[clear()]", "Of course, there are many who ran out of fuel even before they caught on fire.", "/[waitkey(64)]/[close()]"};

    ST1743() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void TalkNPC06(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc06talked == 0) {
                window.print(this.T1_1, 0);
                System.waitFor(window);
                this.npc06talked = 1;
            } else {
                window.print(this.T1_2, 0);
                System.waitFor(window);
                this.npc06talked = 0;
            }
        } else if (this.npc0606talked == 0) {
            window.print(this.T1_3, 0);
            System.waitFor(window);
            this.npc0606talked = 1;
        } else {
            window.print(this.T1_4, 0);
            System.waitFor(window);
            this.npc0606talked = 0;
        }
    }

    public void TalkNPC07(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc07talked == 0) {
                window.print(this.T2_1, 0);
                System.waitFor(window);
                this.npc07talked = 1;
            } else {
                window.print(this.T2_2, 0);
                System.waitFor(window);
                this.npc07talked = 0;
            }
        } else if (this.npc0707talked == 0) {
            window.print(this.T2_3, 0);
            System.waitFor(window);
            this.npc0707talked = 1;
        } else {
            window.print(this.T2_4, 0);
            System.waitFor(window);
            this.npc0707talked = 0;
        }
    }

    public void TalkNPC08(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc08talked == 0) {
                window.print(this.T3_1, 0);
                System.waitFor(window);
                this.npc08talked = 1;
            } else {
                window.print(this.T3_2, 0);
                System.waitFor(window);
                this.npc08talked = 0;
            }
        } else if (this.npc0808talked == 0) {
            window.print(this.T3_3, 0);
            System.waitFor(window);
            this.npc0808talked = 1;
        } else {
            window.print(this.T3_4, 0);
            System.waitFor(window);
            this.npc0808talked = 0;
        }
    }

    public void TalkNPC09(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc09talked == 0) {
                window.print(this.T4_1, 0);
                System.waitFor(window);
                this.npc09talked = 1;
            } else {
                window.print(this.T4_2, 0);
                System.waitFor(window);
                this.npc09talked = 0;
            }
        } else if (this.npc0909talked == 0) {
            window.print(this.T4_3, 0);
            System.waitFor(window);
            this.npc0909talked = 1;
        } else {
            window.print(this.T4_4, 0);
            System.waitFor(window);
            this.npc0909talked = 0;
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1talked == 0) {
                window.print(this.K01_00CJ, 0);
                System.waitFor(window);
                this.npc1talked = 1;
            } else if (this.npc1talked == 1) {
                window.print(this.K01_01CJ, 0);
                System.waitFor(window);
                this.npc1talked = 2;
            } else {
                window.print(this.K01_02CJ, 0);
                System.waitFor(window);
                this.npc1talked = 0;
            }
        } else {
            window.print(this.K01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC10(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc10talked == 0) {
                window.print(this.T5_1, 0);
                System.waitFor(window);
                this.npc10talked = 1;
            } else {
                window.print(this.T5_2, 0);
                System.waitFor(window);
                this.npc10talked = 0;
            }
        } else if (this.npc1010talked == 0) {
            window.print(this.T5_3, 0);
            System.waitFor(window);
            this.npc1010talked = 1;
        } else {
            window.print(this.T5_4, 0);
            System.waitFor(window);
            this.npc1010talked = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K03_00CN, 0);
            System.waitFor(window);
        } else {
            window.print(this.K03_00CJ, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc5talked == 0) {
                window.print(this.K05_00CN, 0);
                System.waitFor(window);
                this.npc5talked = 1;
            } else {
                window.print(this.K05_01CN, 0);
                System.waitFor(window);
                this.npc5talked = 0;
            }
        } else if (this.npc55talked == 0) {
            window.print(this.K05_00CJ, 0);
            System.waitFor(window);
            this.npc55talked = 1;
        } else {
            window.print(this.K05_01CJ, 0);
            System.waitFor(window);
            this.npc55talked = 0;
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1753, 3);
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
        this.hyaku = new NPC_NORMAL(1028, 12, 0, 1, 11, 8.5f, 2.6f, -1.5f, -15.0f);
        this.crew = new NPC_NORMAL(1025, 13, 0, 3, 7, -2.236f, 0.0f, 0.918f, 0.0f);
        this.people = new NPC_NORMAL(1541, 14, 0, 4, 19, 8.482f, 2.6f, 1.6f, 185.0f);
        this.sold = new NPC_NORMAL(526, 15, 0, 3, 17, 11.144f, 2.6f, 1.023f, -15.0f);
        this.police.talkto("TalkNPC1");
        this.hyaku.talkto("TalkNPC2");
        this.crew.talkto("TalkNPC3");
        this.people.talkto("TalkNPC4");
        this.sold.talkto("TalkNPC5");
        this.police.disableDTKFlag(131075);
        this.hyaku.disableDTKFlag(131075);
        this.crew.disableDTKFlag(131074);
        this.people.disableDTKFlag(131075);
        this.sold.disableDTKFlag(131072);
        this.police.enableDTKFlag(12);
        this.hyaku.enableDTKFlag(12);
        this.crew.enableDTKFlag(12);
        this.people.enableDTKFlag(12);
        this.sold.enableDTKFlag(12);
        this.police.setInvalidID(1);
        this.hyaku.setInvalidID(1);
        this.people.setInvalidID(1);
        this.police.setMotion(0, 7);
        this.hyaku.setMotion(0, 7);
        this.people.setMotion(0, 5);
        this.npc06 = new NPC_NORMAL(1547, 16, 0, 1, 19, -5.72f, 0.1f, -4.5f, -45.0f);
        this.npc06.setMotion(0, 27);
        this.npc06.disableDTKFlag(131075);
        this.npc06.enableDTKFlag(12);
        this.npc06.setMotion(0, 4);
        this.npc06.setInvalidID(1);
        this.npc07 = new NPC_NORMAL(527, 17, 0, 1, 19, -15.0f, 0.0f, 2.75f, 90.0f);
        this.npc07.setMotion(0, 27);
        this.npc07.disableDTKFlag(131075);
        this.npc07.enableDTKFlag(12);
        this.npc07.setMotion(0, 3);
        this.npc07.setInvalidID(1);
        this.npc08 = new NPC_NORMAL(1543, 18, 0, 1, 19, -5.28f, 0.1f, -4.0f, -45.0f);
        this.npc08.setMotion(0, 10);
        this.npc08.disableDTKFlag(131075);
        this.npc08.enableDTKFlag(12);
        this.npc08.setMotion(0, 5);
        this.npc08.setInvalidID(1);
        this.npc09 = new NPC_NORMAL(780, 19, 0, 3, 7, -8.5f, 0.0f, 3.8f, -15.0f);
        this.npc09.enableDTKFlag(12);
        this.npc10 = new NPC_NORMAL(1540, 20, 0, 4, 10, -14.42f, 0.0f, -2.88f, -100.0f);
        this.npc10.setMotion(0, 4);
        this.npc10.disableDTKFlag(131075);
        this.npc10.enableDTKFlag(12);
        this.npc06.talkto("TalkNPC06");
        this.npc07.talkto("TalkNPC07");
        this.npc08.talkto("TalkNPC08");
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

