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

class ST1742
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
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
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
    String[] K01_00CN = new String[]{"I hear the Foundation up there is in some trouble.", "/[waitkey(1)]/[clear()]", "After all, I am a policewoman, so I should save people in trouble,", "/[waitkey(1)]/[clear()]", "but this is a really good spot, so I can't just leave.", "/[waitkey(64)]/[close()]"};
    String[] K01_00CJ = new String[]{"Oh, Jr. It seems like something bad is going on, but this ship's all right, isn't it?", "/[waitkey(1)]/[clear()]", "You're the one in charge here, so act like it.", "/[waitkey(64)]/[close()]"};
    String[] K02_00CN = new String[]{"Um, how are things outside? Oh, things are under control here because I'm guarding it with my life!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CJ = new String[]{"Oh, everything's a-okay", "/[waitkey(1)]/[clear()]", "That soldier hasn't done anything suspicious so far!", "/[waitkey(1)]/[clear()]", "Little Master, please leave this place to me and go take care of the Gnosis!", "/[waitkey(64)]/[close()]"};
    String[] K03_00CN = new String[]{"Hey, this isn't good.", "/[waitkey(1)]/[clear()]", "I heard the Foundation is under attack by the Gnosis.", "/[waitkey(1)]/[clear()]", "At this rate, we will never be able to go home.", "/[waitkey(64)]/[close()]"};
    String[] K03_01CN = new String[]{"Well, I can't go home either way, since I've been doing nothing but losing.", "/[waitkey(1)]/[clear()]", "If I went home now, Ma would kill me. Forget the Gnosis!", "/[waitkey(64)]/[close()]"};
    String[] K03_00CJ = new String[]{"Little Master, the Foundation is safe, right? My home is still there, isn't it?", "/[waitkey(64)]/[close()]"};
    String[] K03_01CJ = new String[]{"You're right, everything should be fine because Little Master's handling things.", "/[waitkey(1)]/[clear()]", "Well then, I'm going to go play another game.", "/[waitkey(64)]/[close()]"};
    String[] K04_00CN = new String[]{"I hear things are terrible outside!", "/[waitkey(1)]/[clear()]", "Well, we've got nothing to worry about as long as we're on this ship, right?", "/[waitkey(1)]/[clear()]", "Then, it can't be helped. I guess I'll stay here and play for a while longer.", "/[waitkey(1)]/[clear()]", "Heh heh, heh heh heh!", "/[waitkey(64)]/[close()]"};
    String[] K04_00CJ = new String[]{"I hear things are terrible outside!", "/[waitkey(1)]/[clear()]", "Well, we've got nothing to worry about as long as we're on this ship, right?", "/[waitkey(1)]/[clear()]", "Then, it can't be helped. I guess I'll stay here and play for a while longer.", "/[waitkey(1)]/[clear()]", "Heh heh, heh heh heh!", "/[waitkey(64)]/[close()]"};
    String[] K05_00CN = new String[]{"Don't things seem chaotic outside?", "/[waitkey(1)]/[clear()]", "My shift ended a long time ago, but...", "/[waitkey(1)]/[clear()]", "No matter. Anyway, one thing at a time.", "/[waitkey(64)]/[close()]"};
    String[] K05_00CJ = new String[]{"Don't things seem chaotic outside?", "/[waitkey(1)]/[clear()]", "My shift ended a long time ago, but...", "/[waitkey(1)]/[clear()]", "No matter. Anyway, one thing at a time.", "/[waitkey(64)]/[close()]"};
    String[] U01_00CN = new String[]{"What? What's going on?! Is this ship in danger?", "/[waitkey(1)]/[clear()]", "Hey, if this ship goes down, I'm going to lose out big time!", "/[waitkey(1)]/[clear()]", "What's the Captain doing?!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CJ = new String[]{"Hey, Jr.! You're in charge of this ship, right?", "/[waitkey(1)]/[clear()]", "Everything's fine, right? Right? You're not going to get away with winning and running!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CN = new String[]{"Oh, everything is fine!", "/[waitkey(1)]/[clear()]", "I'll stay here and keep an eye on the soldier. Please don't worry!", "/[waitkey(64)]/[close()]"};
    String[] U02_01CN = new String[]{"But that soldier is amazing!", "/[waitkey(1)]/[clear()]", "I've never seen a move like that before!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CJ = new String[]{"Little Master!\n", "Please look at that Marine's technique!", "/[waitkey(1)]/[clear()]", "Isn't that amazing?! I'd be a full-fledged gambler too once I learn that move!", "/[waitkey(64)]/[close()]"};
    String[] U03_00CN = new String[]{"This ship sure is strange.", "/[waitkey(1)]/[clear()]", "You can still play here while other people are in the middle of a battle.", "/[waitkey(1)]/[clear()]", "You'd have to be pretty strange in the first place to build a recreation area like this on a battleship.", "/[waitkey(64)]/[close()]"};
    String[] U03_00CJ = new String[]{"Little Master? You're the one who built this recreation area, right?", "/[waitkey(1)]/[clear()]", "Don't forget your inner child, even when in battle. But most people wouldn't be able to do that.", "/[waitkey(64)]/[close()]"};
    String[] U04_00CN = new String[]{"Ha ha ha ha ha! Outside? Gnosis? It doesn't matter!", "/[waitkey(1)]/[clear()]", "I am truly invincible right now!", "/[waitkey(1)]/[clear()]", "Mere Gnosis can't stop my streak now! Ha ha ha ha ha!!", "/[waitkey(64)]/[close()]"};
    String[] U04_00CJ = new String[]{"Ha ha ha ha ha! Outside? Gnosis? It doesn't matter!", "/[waitkey(1)]/[clear()]", "I am truly invincible right now!", "/[waitkey(1)]/[clear()]", "Mere Gnosis can't stop my streak now! Ha ha ha ha ha!!", "/[waitkey(64)]/[close()]"};
    String[] U05_00CN = new String[]{"Hey! What the heck is going on?! Where did all my friends go?", "/[waitkey(1)]/[clear()]", "Shoot, did I get ditched?", "/[waitkey(64)]/[close()]"};
    String[] U05_01CN = new String[]{"This is bad. This is no time to be playing around here. I'll just play one more game and then go home.", "/[waitkey(64)]/[close()]"};
    String[] U05_00CJ = new String[]{"Hey! What the heck is going on?! Where did all my friends go?", "/[waitkey(1)]/[clear()]", "Shoot, did I get ditched?", "/[waitkey(64)]/[close()]"};
    String[] U05_01CJ = new String[]{"This is bad. This is no time to be playing around here. I'll just play one more game and then go home.", "/[waitkey(64)]/[close()]"};
    String[] O1_00CJ = new String[]{"Here I thought we finally got the military out of our hair, and now there are monsters?", "/[waitkey(1)]/[clear()]", "Man, what a damn busy ship this is.", "/[waitkey(64)]/[close()]"};
    String[] O1_01CJ = new String[]{"The Foundation? Of course, I'm concerned!!", "/[waitkey(1)]/[clear()]", "The people from the Foundation who came to the casino were easy prey.", "/[waitkey(64)]/[close()]"};
    String[] O1_00CN = new String[]{"Gamble until I run out of money! Gamble if I've got money!!", "/[waitkey(1)]/[clear()]", "I just love the way I am!!", "/[waitkey(64)]/[close()]"};
    String[] O1_01CN = new String[]{"The Foundation people are taking shelter here?!", "/[waitkey(1)]/[clear()]", "Well then, guess I'll make some money off them again.", "/[waitkey(64)]/[close()]"};
    String[] X1_1 = new String[]{"Here I thought the monsters finally went away, and now we have that weird triangular thing.", "/[waitkey(1)]/[clear()]", "Man, what a damn busy ship this is.", "/[waitkey(64)]/[close()]"};
    String[] X1_2 = new String[]{"Captain, since you're so busy you must not have any time to play, right?", "/[waitkey(1)]/[clear()]", "Man, it must be a stressful job.", "/[waitkey(64)]/[close()]"};
    String[] X1_3 = new String[]{"Gamble until I run out of money! Gamble if I've got money!!", "/[waitkey(1)]/[clear()]", "I just love the way I am!!", "/[waitkey(64)]/[close()]"};
    String[] X1_4 = new String[]{"The people from the Foundation are supposed to take shelter here, right?!", "/[waitkey(1)]/[clear()]", "I was just dreaming about taking on the best gamblers in the universe...", "/[waitkey(64)]/[close()]"};
    String[] J07_00CJ = new String[]{"Oh!! Damn it!! I mean, Little Master?!", "/[waitkey(1)]/[clear()]", "Like I said many times before, I'm not slacking!!", "/[waitkey(64)]/[close()]"};
    String[] J07_01CJ = new String[]{"Uh, ha ha ha...", "/[waitkey(1)]/[clear()]", "We certainly bump into each other a lot here...", "/[waitkey(64)]/[close()]"};
    String[] J07_00CN = new String[]{"Sheesh, why does Little Master keep coming here?", "/[waitkey(64)]/[close()]"};
    String[] J07_01CN = new String[]{"Could it be that he has a crush on me?!", "/[waitkey(1)]/[clear()]", "No way, couldn't be.", "/[waitkey(64)]/[close()]"};
    String[] X2_1 = new String[]{"Oh!! Little Master? We meet again!", "/[waitkey(1)]/[clear()]", "Are you interested in me?", "/[waitkey(64)]/[close()]"};
    String[] X2_2 = new String[]{"I know! Yes, I know.", "/[waitkey(1)]/[clear()]", "But I can't reciprocate the same feelings for you right now.", "/[waitkey(64)]/[close()]"};
    String[] X2_3 = new String[]{"Man, what a real bind!", "/[waitkey(1)]/[clear()]", "I never thought Little Master would have feelings for me.", "/[waitkey(64)]/[close()]"};
    String[] X2_4 = new String[]{"Well, I don't think gender matters at all when it comes to love!", "/[waitkey(1)]/[clear()]", "But I'm not like that!", "/[waitkey(64)]/[close()]"};
    String[] O2_00CJ = new String[]{"Little Master!!\n", "Is it running a casino good business?", "/[waitkey(1)]/[clear()]", "If you don't mind, could you share a secret to your success with me?", "/[waitkey(64)]/[close()]"};
    String[] O2_01CJ = new String[]{"It's more profitable to run a casino than to play at one, isn't it?", "/[waitkey(64)]/[close()]"};
    String[] O2_00CN = new String[]{"The other day, I saw an ad in a magazine for a rabbit necklace!!", "/[waitkey(64)]/[close()]"};
    String[] O2_01CN = new String[]{"It was a picture of a man in a bathtub full of wads of money and surrounded by beautiful women!!", "/[waitkey(1)]/[clear()]", "That rabbit necklace sure is amazing...", "/[waitkey(64)]/[close()]"};
    String[] X3_1 = new String[]{"What?! That girl with pink hair got taken away?", "/[waitkey(1)]/[clear()]", "Little Master! What are you doing hanging around here for?! You need to hurry and go rescue her!!", "/[waitkey(64)]/[close()]"};
    String[] X3_2 = new String[]{"If you want, I can help you rescue her too.", "/[waitkey(64)]/[close()]"};
    String[] X3_3 = new String[]{"You know, watching that girl with pink hair reminds me of my daughter when she was little...", "/[waitkey(64)]/[close()]"};
    String[] X3_4 = new String[]{"When I would win at the casinos, I'd often buy a doll for her.", "/[waitkey(64)]/[close()]"};
    String[] J09_00CJ = new String[]{"Does gambling addiction really exist?", "/[waitkey(64)]/[close()]"};
    String[] J09_01CJ = new String[]{"There must be something really wrong with people who get addicted to something like that, don't you think?", "/[waitkey(64)]/[close()]"};
    String[] J09_00CN = new String[]{"What? You're asking me if I'm worried about the people on the Foundation?", "/[waitkey(64)]/[close()]"};
    String[] J09_01CN = new String[]{"Of course I am! I'm very worried!!", "/[waitkey(1)]/[clear()]", "That's why I'm staying here with the intention of entertaining the people who come from the Foundation!!", "/[waitkey(1)]/[clear()]", "You guys ought to hurry up and evacuate the residents of the Foundation!!", "/[waitkey(64)]/[close()]"};
    String[] X4_1 = new String[]{"Some professor somewhere once said, \"God doesn't play dice.\"", "/[waitkey(64)]/[close()]"};
    String[] X4_2 = new String[]{"All I ever do is play with dice though.", "/[waitkey(64)]/[close()]"};
    String[] X4_3 = new String[]{"Yes, yes, I recently had a revelation.", "/[waitkey(1)]/[clear()]", "No matter how many people come from the Foundation, the crowds of the casino never change.", "/[waitkey(1)]/[clear()]", "To think that people who don't gamble will end their lives, never having known the beauty of gambling....", "/[waitkey(64)]/[close()]"};
    String[] X4_4 = new String[]{"In that sense, I'm really lucky!!", "/[waitkey(1)]/[clear()]", "I was introduced to gambling by knowing everything about the Durandal!", "/[waitkey(64)]/[close()]"};
    String[] J10_00CJ = new String[]{"Little Master, please close the casino,", "/[waitkey(64)]/[close()]"};
    String[] J10_01CJ = new String[]{"or else my family will be torn apart.", "/[waitkey(64)]/[close()]"};
    String[] J10_00CN = new String[]{"The people of the Foundation are coming here?", "/[waitkey(1)]/[clear()]", "Those people are probably going to be easy targets too.", "/[waitkey(64)]/[close()]"};
    String[] J10_01CN = new String[]{"The people of the Foundation are coming here?", "/[waitkey(1)]/[clear()]", "Those people are probably going to be easy targets too.", "/[waitkey(64)]/[close()]"};
    String[] X5_1 = new String[]{"Little Master! I decided to retract my petition to close down the casino!", "/[waitkey(64)]/[close()]"};
    String[] X5_2 = new String[]{"It sort of...just...became clear to me while I kept on hitting this spot!!", "/[waitkey(64)]/[close()]"};
    String[] X5_3 = new String[]{"A little more! Just a little bit longer, and I'll be on fire!", "/[waitkey(64)]/[close()]"};
    String[] X5_4 = new String[]{"Please stay and watch! I'm in the process of heating up.", "/[waitkey(1)]/[clear()]", "Of course, there are many who ran out of fuel even before they caught on fire.", "/[waitkey(64)]/[close()]"};

    ST1742() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void TalkNPC06(Enepc enepc, Window window) {
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

    public void TalkNPC06a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc06talked == 0) {
                window.print(this.X1_1, 0);
                System.waitFor(window);
                this.npc06talked = 1;
            } else {
                window.print(this.X1_2, 0);
                System.waitFor(window);
                this.npc06talked = 0;
            }
        } else if (this.npc0606talked == 0) {
            window.print(this.X1_3, 0);
            System.waitFor(window);
            this.npc0606talked = 1;
        } else {
            window.print(this.X1_4, 0);
            System.waitFor(window);
            this.npc0606talked = 0;
        }
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

    public void TalkNPC07a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc07talked == 0) {
                window.print(this.X2_1, 0);
                System.waitFor(window);
                this.npc07talked = 1;
            } else {
                window.print(this.X2_2, 0);
                System.waitFor(window);
                this.npc07talked = 0;
            }
        } else if (this.npc0707talked == 0) {
            window.print(this.X2_3, 0);
            System.waitFor(window);
            this.npc0707talked = 1;
        } else {
            window.print(this.X2_4, 0);
            System.waitFor(window);
            this.npc0707talked = 0;
        }
    }

    public void TalkNPC08(Enepc enepc, Window window) {
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

    public void TalkNPC08a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc08talked == 0) {
                window.print(this.X3_1, 0);
                System.waitFor(window);
                this.npc08talked = 1;
            } else {
                window.print(this.X3_2, 0);
                System.waitFor(window);
                this.npc08talked = 0;
            }
        } else if (this.npc0808talked == 0) {
            window.print(this.X3_3, 0);
            System.waitFor(window);
            this.npc0808talked = 1;
        } else {
            window.print(this.X3_4, 0);
            System.waitFor(window);
            this.npc0808talked = 0;
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

    public void TalkNPC09a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc09talked == 0) {
                window.print(this.X4_1, 0);
                System.waitFor(window);
                this.npc09talked = 1;
            } else {
                window.print(this.X4_2, 0);
                System.waitFor(window);
                this.npc09talked = 0;
            }
        } else if (this.npc0909talked == 0) {
            window.print(this.X4_3, 0);
            System.waitFor(window);
            this.npc0909talked = 1;
        } else {
            window.print(this.X4_4, 0);
            System.waitFor(window);
            this.npc0909talked = 0;
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K01_00CN, 0);
            System.waitFor(window);
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

    public void TalkNPC10a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc10talked == 0) {
                window.print(this.X5_1, 0);
                System.waitFor(window);
                this.npc10talked = 1;
            } else {
                window.print(this.X5_2, 0);
                System.waitFor(window);
                this.npc10talked = 0;
            }
        } else if (this.npc1010talked == 0) {
            window.print(this.X5_3, 0);
            System.waitFor(window);
            this.npc1010talked = 1;
        } else {
            window.print(this.X5_4, 0);
            System.waitFor(window);
            this.npc1010talked = 0;
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U01_00CN, 0);
            System.waitFor(window);
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

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U02_00CJ, 0);
            System.waitFor(window);
        } else if (this.npc22talked == 0) {
            window.print(this.U02_00CN, 0);
            System.waitFor(window);
            this.npc22talked = 1;
        } else {
            window.print(this.U02_01CN, 0);
            System.waitFor(window);
            this.npc22talked = 0;
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc3talked == 0) {
                window.print(this.K03_00CJ, 0);
                System.waitFor(window);
                this.npc3talked = 1;
            } else {
                window.print(this.K03_01CJ, 0);
                System.waitFor(window);
                this.npc3talked = 0;
            }
        } else if (this.npc33talked == 0) {
            window.print(this.K03_00CN, 0);
            System.waitFor(window);
            this.npc33talked = 1;
        } else {
            window.print(this.K03_01CN, 0);
            System.waitFor(window);
            this.npc33talked = 0;
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U03_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U03_00CN, 0);
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

    public void TalkNPC4a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K05_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K05_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc5talked == 0) {
                window.print(this.U05_00CJ, 0);
                System.waitFor(window);
                this.npc5talked = 1;
            } else {
                window.print(this.U05_01CJ, 0);
                System.waitFor(window);
                this.npc5talked = 0;
            }
        } else if (this.npc55talked == 0) {
            window.print(this.U05_00CN, 0);
            System.waitFor(window);
            this.npc55talked = 1;
        } else {
            window.print(this.U05_01CN, 0);
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
                Runtime.jumpCF(1752, 3);
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
        this.hyaku = new NPC_NORMAL(1028, 12, 0, 1, 3, 7.507f, 2.6f, -2.344f, 30.0f);
        this.crew = new NPC_NORMAL(1025, 13, 0, 3, 7, -2.236f, 0.0f, 0.918f, 0.0f);
        this.people = new NPC_NORMAL(1541, 14, 0, 4, 19, 8.482f, 2.6f, 1.6f, 185.0f);
        this.sold = new NPC_NORMAL(526, 15, 0, 1, 19, 8.5f, 2.6f, -1.5f, -15.0f);
        if (Runtime.getFlags(373, 1) == 0) {
            this.police.talkto("TalkNPC1");
            this.hyaku.talkto("TalkNPC2");
            this.crew.talkto("TalkNPC3");
            this.people.talkto("TalkNPC4");
            this.sold.talkto("TalkNPC5");
        } else {
            this.police.talkto("TalkNPC1a");
            this.hyaku.talkto("TalkNPC2a");
            this.crew.talkto("TalkNPC3a");
            this.people.talkto("TalkNPC4a");
            this.sold.talkto("TalkNPC5a");
        }
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
        this.hyaku.setInvalidID(1);
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
        if (Runtime.getFlags(373, 1) == 0) {
            this.npc06.talkto("TalkNPC06");
            this.npc07.talkto("TalkNPC07");
            this.npc08.talkto("TalkNPC08");
            this.npc09.talkto("TalkNPC09");
            this.npc10.talkto("TalkNPC10");
        } else {
            this.npc06.talkto("TalkNPC06a");
            this.npc07.talkto("TalkNPC07a");
            this.npc08.talkto("TalkNPC08a");
            this.npc09.talkto("TalkNPC09a");
            this.npc10.talkto("TalkNPC10a");
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

