import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KOU01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1211
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KOU01_PRJ {
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
    Enepc npc15;
    Enepc npc16;
    Enepc npc17;
    Enepc npc18;
    Enepc npc19;
    Enepc npc20;
    Enepc npc21;
    Enepc npc22;
    Enepc npc23;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int talkFlag14;
    int talkFlag15;
    int talkFlag16;
    int talkFlag17;
    int talkFlag18;
    int talkFlag19;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag6;
    int S2040C;
    int S2040D;
    int S2042;
    int S3;
    int angou2;
    int hamago;
    int hamahelp;
    Chr PLANE1;
    Chr PLANE2;
    Chr ELSA;
    Light light = new Light(0);
    boolean WALL_FLAG = false;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono door1;
    Uwamono door2;
    Uwamono door3;
    Uwamono saveA;
    Effect eve323_A;
    Effect eve323_B;
    Effect eve323_C;
    Effect eve323_D;
    Effect eve323_E;
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] msg0001 = new String[]{"/[label()]", "Haven't seen you guys around here before. Is this your first time here?", "/[waitkey(1)]/[clear()]", "Don't look so worried, we'll do a good job servicing your ship! Get some rest while you're here.", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label()]", "The government doesn't keep a strict watch on things in this area of space. You can get away with most things, but that doesn't mean you should get involved in too many shady things.", "/[waitkey(64)]/[close()]"};
    String[] msg0011 = new String[]{"/[label()]", "Who the heck are you guys? You guys are an odd bunch to come to this run-down dock.", "/[waitkey(1)]/[clear()]", "The army made a huge mess here during the Miltian Conflict. There's nothing of interest here.", "/[waitkey(64)]/[close()]"};
    String[] msg0012 = new String[]{"/[label()]", "Why was this facility targeted? What? You don't know?", "/[waitkey(1)]/[clear()]", "Fourteen years ago, a Realian in service went crazy in this star cluster. That's why.", "/[waitkey(64)]/[close()]"};
    String[] msg0013 = new String[]{"/[label()]", "Later, they reported that it was a Trojan horse sent in by the instigator of the conflict, but I have my doubts.", "/[waitkey(1)]/[clear()]", "Back then, there was a human rights issue for Realians, remember? I bet some in the government opposed the idea.", "/[waitkey(64)]/[close()]"};
    String[] msg0021 = new String[]{"/[label()]", "This colony was once a Realian nursery. After the war, the pier was remodeled into a dock.", "/[waitkey(1)]/[clear()]", "And many refugees came flooding in, probably because of its favorable location. But now, it's practically treated like a den of outlaws.", "/[waitkey(64)]/[close()]"};
    String[] msg0022 = new String[]{"/[label()]", "This colony was once a Realian nursery. After the war, the pier was remodeled into a dock.", "/[waitkey(1)]/[clear()]", "And many refugees came flooding in, probably because of its favorable location. But now, it's practically treated like a den of outlaws.", "/[waitkey(64)]/[close()]"};
    String[] msg0031 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right.", "/[waitkey(1)]/[clear()]", "There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here.", "/[waitkey(1)]/[clear()]", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg0032 = new String[]{"/[label()]", "What?! You're looking for a soldier?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably lying dead in some alleyway.", "/[waitkey(1)]/[clear()]", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg0041 = new String[]{"/[label()]", "Hey, you there! You better be more careful or you're gonna hit something.", "/[waitkey(1)]/[clear()]", "(Crash!)", "/[waitkey(1)]/[clear()]", "Oh man, I did it again...", "/[waitkey(1)]/[clear()]", "Oh well, it's got so many dents and scratches already, no one will notice another one.", "/[waitkey(64)]/[close()]"};
    String[] msg0042 = new String[]{"/[label()]", "Oh, you weren't watching, were ya?", "/[waitkey(1)]/[clear()]", "What?! That ship's yours?", "/[waitkey(1)]/[clear()]", "O-oh, don't worry. Everything will be fine, just leave it to us!", "/[waitkey(1)]/[clear()]", "(Crash!)", "\n", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0051 = new String[]{"/[label()]", "Sheesh, planets vanishing, Gnosis appearing, the universe is becoming a chaotic place. With all this commotion, I don't have any time to rest.", "/[waitkey(1)]/[clear()]", "But then again, it's thanks to that I can put food on the table.", "/[waitkey(64)]/[close()]"};
    String[] msg0052 = new String[]{"/[label()]", "The Federation sure is awful. As soon as the war was over, they didn't care about anything else. They didn't even give one thought to people like us, who went through terrible times.", "/[waitkey(1)]/[clear()]", "But I guess thanks to that, we get to do whatever we please.", "/[waitkey(64)]/[close()]"};
    String[] msg0061 = new String[]{"/[label()]", "What? Have I seen a Commander around? Hey, this is the wrong place to be talking about the military! Don't you know anything? They came and destroyed everything without discrimination, just because this was a Realian nursery.", "/[waitkey(64)]/[close()]"};
    String[] msg0062 = new String[]{"/[label()]", "Military and public officials are the scum of mankind. They started the war on their own and then dragged us into it. It's because of those good-for-nothings that I live like this now.", "/[waitkey(64)]/[close()]"};
    String[] msg0063 = new String[]{"/[label()]", "Don't you guys give up? I told you, I don't know! Your commander person is probably dead in some ditch. Soldiers don't die decent deaths around here!", "/[waitkey(64)]/[close()]"};
    String[] msg0071 = new String[]{"/[label()]", "Oh, I haven't seen you around before. Can I help you with something? Commander Cherenkov? So you're looking for a soldier? Are you an idiot or something?! Looking for a soldier on this colony? You must be insane!", "/[waitkey(64)]/[close()]"};
    String[] msg0072 = new String[]{"/[label()]", "After the conflict on Miltia, public officials and military people treated us like garbage and abandoned us in this remote region. Everyone here hates government officials. If you claim to be friends with a soldier, you better watch your back!", "/[waitkey(64)]/[close()]"};
    String[] msg0081 = new String[]{"/[label()]", "Hello, are you here on business? Not that we have anything worth seeing here, but please make yourselves at home.", "/[waitkey(64)]/[close()]"};
    String[] msg0082 = new String[]{"/[label()]", "Um, please don't let it get to you. Marines killed her family during the Miltian Conflict. ", "Everyone related to this facility was branded as traitors and got taken away by the Federation. After the war, we demanded compensation, but they said it was one\nrogue unit's fault, and that was the end of it. ", "That's why she gets like that when she hears about soldiers and public officials.", "/[waitkey(64)]/[close()]"};
    String[] msg0083 = new String[]{"/[label()]", "All the people here either hates or are dissatisfied with the Federation government to some extent.", "/[waitkey(64)]/[close()]"};
    String[] msg0091 = new String[]{"/[label()]", "What's the matter? Why the long face? Take a look at these people. ", "All the people here either escaped from the Miltian Conflict 14 years ago, or recently ran away from the Gnosis phenomenon. ", "But even after all they've been through, they're living as best they can.", "/[waitkey(64)]/[close()]"};
    String[] msg0092 = new String[]{"/[label()]", "I also come here when things start getting to me. You see, seeing everyone's faces makes me feel like I gotta keep going! ", "You people better do your best, too!", "/[waitkey(64)]/[close()]"};
    String[] msg00A1 = new String[]{"/[label()]", "Recently, more spaceships have been coming to this dock due to the battles against the Gnosis. ", "Nowadays, even civilian ships must equip weapons if they hope to get around at all. It's become an unpleasant world.", "/[waitkey(64)]/[close()]"};
    String[] msg00A2 = new String[]{"I don't really care, but how long do you suppose that lady's going to stay there? ", "She's in my way...it's really difficult to get my work done if she stays there.", "/[waitkey(64)]/[close()]"};
    String[] msg00A3 = new String[]{"/[label()]", "Um, could you also move out of my way please? You're in the way.", "/[waitkey(64)]/[close()]"};
    String[] msg00B1 = new String[]{"/[label()]", "Say, do you know about the planetary disappearance incident?", "/[waitkey(64)]/[close()]", "/[label()]", "A planet called Ariadne completely disappeared. The Federation government still doesn't know the cause of it, and the people who disappeared with the planet are still missing too. Some think it's the work of Gnosis. In any case, it's a terrible incident.", "/[waitkey(64)]/[close()]", "/[label()]", "The truth of that incident is still a mystery. I can't believe that a planet could disappear without a trace.", "/[waitkey(64)]/[close()]"};
    String[] msg00C1 = new String[]{"/[label()]", "That girl named Luty at the clinic refuses to talk at all! ", "I was being nice and said I'd play with her. She's such a brat.", "/[waitkey(64)]/[close()]"};
    String[] msg00D1 = new String[]{"/[label()]", "Luty used to live on Ariadne until just recently. But when the planet disappeared, she moved to the colony. Both her parents disappeared with the planet, so she's living with the doctor now.", "/[waitkey(64)]/[close()]"};
    String[] msg00E1 = new String[]{"/[label()]", "Guess what? He has a crush on Luty! That's why he's always messing with her.", "/[waitkey(64)]/[close()]"};
    String[] msg00E2 = new String[]{"/[label()]", "T-that's not true. It isn't like that! Why would I like her!!", "/[waitkey(64)]/[close()]"};
    String[] msg00E3 = new String[]{"/[label()]", "Heh heh, the way you get all worked up...it's awfully suspicious...nya-nya-nya-nya-nya!", "/[waitkey(64)]/[close()]"};
    String[] msg00F1 = new String[]{"/[label()]", "Hey, did you go to that suspicious-looking parts shop? There's something weird about that shopkeeper. You see, it's an embarrassing story, but I just asked the shopkeeper about \"○○○.\" ", "Then the shopkeeper suddenly got this fierce look and started asking who I heard it from and if I've got money. I got scared so I ran away.", "/[waitkey(64)]/[close()]"};
    String[] msg00F2 = new String[]{"/[label()]", "What's with that shopkeeper? I wonder if he hates \"○○○?\"", "/[waitkey(64)]/[close()]"};
    String[] msg00G1 = new String[]{"/[label()]", "What? You got a problem? Leave me alone. I want to be alone.", "/[waitkey(64)]/[close()]"};
    String[] msg00G2 = new String[]{"/[label()]", "You the one the shopkeeper told me about? I see...you do look like a bad person.", "/[waitkey(1)]/[clear()]", "You got the money ready? ○○○○G in cash. I won't take anything less.", "/[waitkey(64)]/[close()]"};
    String[] msg00G3 = new String[]{"/[label()]", "Man, that's sad. Get the hell outta here if you don't got the cash!", "/[waitkey(64)]/[close()]"};
    String[] msg00G4 = new String[]{"/[label()]", "Okay, it's all there. Here you go!", "/[waitkey(1)]/[clear()]", "Obtained \"Key to Hidden Corridor.\"", "/[waitkey(1)]/[clear()]", "You can open the corridor in the back of the shop with that key. The goods are in there.", "/[waitkey(64)]/[close()]"};
    String[] msg0111 = new String[]{"/[label()]", "Yo, having fun?", "/[waitkey(1)]/[clear()]", "The ship's repairs are done. You can launch anytime.", "/[waitkey(64)]/[close()]"};
    String[] msg0112 = new String[]{"/[label()]", "Oh, yeah, I hear someone was attacked over in that alleyway just now. You guys better be careful too.", "/[waitkey(64)]/[close()]"};
    String[] msg0121 = new String[]{"/[label()]", "Oh, did you hear? Apparently, there was a mugging in the alleyway nearby. Several men were attacked and they were taken to the clinic there.", " It's a dangerous world we live in...", "/[waitkey(64)]/[close()]"};
    String[] msg0122 = new String[]{"/[label()]", "What? The perpetrator? ", "Yes, I heard the thug was a soldier! It really makes me mad. In the end, soldiers are barbaric, good-for-nothing idiots!", "/[waitkey(64)]/[close()]"};
    String[] msg0131 = new String[]{"/[label()]", "Oh, we finished the repairs. Well? Looks perfect, doesn't it?!", "/[waitkey(1)]/[clear()]", "(Crash!)", "/[waitkey(1)]/[clear()]", "Whoops...", "/[waitkey(64)]/[close()]"};
    String[] msg0132 = new String[]{"/[label()]", "Ha ha ha, don't worry, don't worry. We'll fix it right away!", "/[waitkey(1)]/[clear()]", "Ha ha ha...", "/[waitkey(64)]/[close()]"};
    String[] msg0141 = new String[]{"/[label()]", "What? You guys are still here? ", "Some folks got attacked over there just now. You didn't do that, did you?", "/[waitkey(64)]/[close()]"};
    String[] msg0142 = new String[]{"/[label()]", "Let me give you a piece of advice. Nothing good will come of stirring up trouble here. If you've got no business here, you should get going.", "/[waitkey(64)]/[close()]"};
    String[] msg0151 = new String[]{"/[label()]", "Say, did you know?", "/[waitkey(64)]/[close()]", "/[label()]", "Some people were attacked in that alleyway just now. The perpetrator is supposedly a Federation soldier. It's such a dangerous world.", "/[waitkey(64)]/[close()]", "/[label()]", "Isn't it so dangerous? The people who were attacked seem to be in serious condition. There's also a rumor that there's a soldier wandering around...I don't feel safe walking around outside anymore.", "/[waitkey(64)]/[close()]"};
    String[] msg0211 = new String[]{"/[label()]", "Hey, more repairs? Well, relax and take a load off for a while.", "/[waitkey(64)]/[close()]"};
    String[] msg0221 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right. There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here. ", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg0222 = new String[]{"/[label()]", "What?! You're looking for a soldier? ", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably lying dead in some alleyway. ", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg0231 = new String[]{"/[label()]", "Darned soldiers do nothing but strut around. They don't know anything about what we've been through!", "/[waitkey(64)]/[close()]"};
    String[] msg0232 = new String[]{"/[label()]", "Military and public officials are the scum of mankind. They started the war on their own and then dragged us into it. ", "It's because of those good-for-nothings that I live like this now.", "/[waitkey(64)]/[close()]"};
    String[] msghama1 = new String[]{"/[label(Hammer)]", "Oh!! S-S-S-S-Shion! I-I-I-It's terrible! It's terrible!!", "/[waitkey(1)]/[clear()]", "In the thug, the alleyway is beat up all Commander!!", "/[waitkey(64)]/[clear()]"};
    String[] msghama2 = new String[]{"/[label(Shion)]", "Uh, um, Hammer? I can't understand you at all. Calm\ndown a little...", "/[waitkey(64)]/[clear()]"};
    String[] msghama3 = new String[]{"/[label(Hammer)]", "There's no time to calm down!", "/[waitkey(1)]/[clear()]", "Come on, hurry!", "/[waitkey(1)]/[clear()]", "Come this way, quickly!!", "/[waitkey(64)]/[close()]"};
    String[] msgHAMMER1 = new String[]{"/[label(Hammer)]", "Well, let's start looking for the Commander. We should split up to look for him. I'll let you know as soon as I find him!", "/[waitkey(64)]/[close()]"};

    ST1211() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(24.018f, 1.767f, -24.468f);
        this.camEV.setRotate(-8.123f, 157.259f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.16f, -0.511f, 17.626f);
        this.camEV.setRotate(-7.744f, 36.717f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                Stage.setVisible(12, this.WALL_FLAG);
                this.WALL_FLAG ^= true;
                break;
            }
            case 1: {
                Stage.setVisible(12, this.WALL_FLAG);
                this.WALL_FLAG ^= true;
                break;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print("（仮）錆び付いて開かない/[waitkey(64)]/[close()]");
                break;
            }
            case 3: {
                if (this.S2040D != 0) break;
                Runtime.setFlags(159, 1, 1);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpEvent(2403);
                System.println("フラグオン！");
                break;
            }
            case 4: {
                if (this.hamahelp == 1) {
                    return;
                }
                if (this.S2040D != 1) break;
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7036, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(1210, 1);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc1_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc1_1(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc10_1(window);
        } else {
            this.Talk_npc10_1(window);
        }
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg0091, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0092, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc10_2(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc11_1(window);
        } else {
            this.Talk_npc11_1(window);
        }
    }

    void Talk_npc11_1(Window window) {
        ++this.talkFlag11;
        switch (this.talkFlag11) {
            case 1: {
                window.print(this.msg00A1, 0);
                ST1211.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00A2, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00A3, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc11_2(Window window) {
        ++this.talkFlag11;
        switch (this.talkFlag11) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc12(Enepc enepc, Window window) {
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc13_1(window);
        } else {
            this.Talk_npc13_1(window);
        }
    }

    void Talk_npc13_1(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                window.print(this.msg00B1, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
    }

    void Talk_npc13_2(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc14(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc14_1(window);
        } else {
            this.Talk_npc14_1(window);
        }
    }

    void Talk_npc14_1(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                window.print(this.msg00C1, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
    }

    void Talk_npc14_2(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc15(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc15_1(window);
        } else {
            this.Talk_npc15_1(window);
        }
    }

    void Talk_npc15_1(Window window) {
        ++this.talkFlag15;
        switch (this.talkFlag15) {
            case 1: {
                window.print(this.msg00D1, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
    }

    void Talk_npc15_2(Window window) {
        ++this.talkFlag15;
        switch (this.talkFlag15) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc16(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc16_1(window);
        } else {
            this.Talk_npc16_1(window);
        }
    }

    void Talk_npc16_1(Window window) {
        ++this.talkFlag16;
        switch (this.talkFlag16) {
            case 1: {
                window.print(this.msg00E1, 0);
                ST1211.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00E2, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00E3, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc16_2(Window window) {
        ++this.talkFlag16;
        switch (this.talkFlag16) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc17(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc17_1(window);
        } else {
            this.Talk_npc17_1(window);
        }
    }

    void Talk_npc17_1(Window window) {
        ++this.talkFlag17;
        switch (this.talkFlag17) {
            case 1: {
                window.print(this.msg00F1, 0);
                ST1211.waitPage(window, 64);
                Runtime.setFlags(7016, 1, 1);
                return;
            }
        }
        window.print(this.msg00F2, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc17_2(Window window) {
        ++this.talkFlag17;
        switch (this.talkFlag17) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc18(Enepc enepc) {
        if (this.S3 == 1) {
            this.Talk_npc18_1();
        } else {
            this.Talk_npc18_1();
        }
    }

    void Talk_npc18_1() {
        switch (this.angou2) {
            case 1: {
                this.win = Window.create();
                this.win.print(this.msg00G2, 0);
                ST1211.waitPage(this.win, 64);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Pay\nDon't pay");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.print(this.msg00G4, 0);
                        ST1211.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.print(this.msg00G3, 0);
                ST1211.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.print(this.msg00G1, 0);
        ST1211.waitPage(this.win, 64);
    }

    void Talk_npc18_2() {
        ++this.talkFlag18;
        switch (this.talkFlag18) {
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void Talk_npc19(Enepc enepc) {
        this.fade.call(0);
        System.sleep(30);
        Runtime.setFlags(7036, 1, 1);
        System.println("フラグオン！");
        Runtime.jumpCF(1210, 1);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0001, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0002, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0111, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0112, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
        window.print(this.msg0211, 0);
        ST1211.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc2_1(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg0011, 0);
                ST1211.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0012, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0013, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc3_1(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0021, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0022, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg0031, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0032, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc5_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg0041, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0042, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg0131, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0132, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc6_1(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg0051, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0052, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc7_3(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc7_2(window);
        } else {
            this.Talk_npc7_1(window);
        }
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg0061, 0);
                ST1211.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0062, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0063, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg0141, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0142, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc7_3(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg0221, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0222, 0);
        ST1211.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc8_1(window);
        } else {
            this.Talk_npc8_1(window);
        }
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg0071, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0072, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc9_1(window);
        } else {
            this.Talk_npc9_1(window);
        }
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg0081, 0);
                ST1211.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0082, 0);
                ST1211.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0083, 0);
        ST1211.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                return;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(520, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1220, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(1240, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -2.437f, 0.0f, -24.864f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, -0.31f, 0.0f, -24.864f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, 13.5f, -2.0f, 15.95f, 0.0f);
        this.teiten3.SetBgm(196610);
        this.teiten4 = new Uwamono(28690, 15.5f, -2.0f, 16.0f, 0.0f);
        this.teiten4.SetBgm(196610);
        this.teiten5 = new Uwamono(28690, 10.5f, -2.0f, 7.0f, 0.0f);
        this.teiten5.SetBgm(196610);
        this.teiten6 = new Uwamono(28690, 17.0f, -2.0f, 7.85f, 0.0f);
        this.teiten6.SetBgm(196610);
        this.teiten7 = new Uwamono(28690, 17.0f, -2.0f, 7.85f, 0.0f);
        this.teiten7.SetBgm(196611);
        this.teiten7.SetBgmType('\u0001');
        this.S2040C = Runtime.getFlags(158, 1);
        this.S2040D = Runtime.getFlags(159, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S3 = Runtime.getFlags(179, 1);
        this.angou2 = Runtime.getFlags(7017, 1);
        this.hamago = Runtime.getFlags(7033, 1);
        this.hamahelp = Runtime.getFlags(7036, 1);
        Stage.setVisible(-1, true);
        this.saveA = new Uwamono(28733, 7.5f, 2.0f, -4.18f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 345.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 12.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 340.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.005f, 0.005f);
        this.cam0.setCFAngle(5, -28.0f, 375.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 375.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.02f, 0.02f);
        this.cam0.setCFAngle(8, -28.0f, 380.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.015f, 0.015f);
        this.PLANE1 = new Chr();
        this.PLANE1.init(20639, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PLANE1.setTranslate(16.5f, -1.0f, 4.6f);
        this.PLANE1.setRotate(0.0f, 90.0f, 0.0f);
        this.ELSA = new Chr();
        this.ELSA.init(20482, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ELSA.setTranslate(45.0f, -5.0f, 33.0f);
        this.ELSA.setRotate(0.0f, 180.0f, 0.0f);
        this.ELSA.setScale(167.39f, 167.39f, 167.39f);
        this.PLANE1.dispRadar(false);
        this.ELSA.dispRadar(false);
        this.eve323_A = new Effect(1722, -3.5f, -2.0f, 16.0f, 0.0f);
        this.eve323_A.setScale(1.0f, 0.75f, 1.0f);
        this.eve323_A.setClip(true);
        this.eve323_B = new Effect(1722, 5.4f, -2.0f, 9.0f, 0.0f);
        this.eve323_B.setScale(1.0f, 0.75f, 1.0f);
        this.eve323_B.setClip(true);
        this.eve323_C = new Effect(1722, 5.5f, 0.0f, -11.0f, 0.0f);
        this.eve323_C.setScale(1.0f, 0.75f, 1.0f);
        this.eve323_C.setClip(true);
        this.eve323_D = new Effect(1722, 5.5f, 0.0f, -15.0f, 0.0f);
        this.eve323_D.setScale(1.0f, 1.5f, 1.0f);
        this.eve323_D.setClip(true);
        this.eve323_E = new Effect(1722, -3.5f, -2.0f, 11.0f, 0.0f);
        this.eve323_E.setScale(1.0f, 0.75f, 1.0f);
        this.eve323_E.setClip(true);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.325f, 0.325f, 0.325f);
        this.light.setColor(1, 0.325f, 0.325f, 0.325f);
        this.light.setDirection2(1, -0.5f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.38f, 0.38f, 0.38f);
        Runtime.setIdLightCol(2, 1, 0.38f, 0.38f, 0.38f);
        Runtime.setIdLightCol(2, 2, 0.38f, 0.38f, 0.38f);
        Runtime.setIdLightCol(2, 3, 0.38f, 0.38f, 0.38f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.36f, 0.36f, 0.36f);
        Runtime.setIdLightCol(4, 1, 0.36f, 0.36f, 0.36f);
        Runtime.setIdLightCol(4, 2, 0.36f, 0.36f, 0.36f);
        Runtime.setIdLightCol(4, 3, 0.36f, 0.36f, 0.36f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.door2 = new Uwamono(212, 40, '\u0001');
        this.door2.SetDoorType('\u0004');
        this.door3 = new Uwamono(213, 40, '\u0001');
        this.door3.SetDoorType('\u0004');
        if (this.S3 == 1) {
            this.npcset_1();
        } else if (this.S2042 == 1) {
            this.npcset_1();
        } else if (this.hamahelp == 1) {
            this.npcset_3();
        } else if (this.S2040D == 1) {
            this.npcset_1();
        } else if (this.hamago == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
        this.npc20 = new NPC_NORMAL(1, 20, 0, 13, 5, 24.65f, 0.0f, -21.5f, 270.0f);
        this.npc21 = new NPC_NORMAL(278, 21, 0, 13, 3, 24.25f, 0.0f, -20.96f, 270.0f);
        this.npc22 = new NPC_NORMAL(3, 22, 0, 13, 7, 24.65f, 0.0f, -21.5f, 270.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setTranslate(24.65f, 0.25f, -21.5f);
        this.npc20.setMotion(0, 1);
        this.npc20.enableDTKFlag(262144);
        this.npc20.kickEnepc(4, 1);
        this.npc21.setInvalidID(1);
        this.npc21.setTranslate(24.25f, 0.25f, -20.96f);
        this.npc21.setMotion(0, 1);
        this.npc21.enableDTKFlag(262144);
        this.npc21.kickEnepc(4, 1);
        this.npc22.setInvalidID(1);
        this.npc22.setTranslate(24.665f, 0.25f, -21.5f);
        this.npc22.setMotion(0, 1);
        this.npc22.enableDTKFlag(262144);
        this.npc22.kickEnepc(4, 1);
        this.npc17 = new NPC_NORMAL(1, 17, 0, 13, 5, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc17.setInvalidID(1);
        this.npc17.disableDTKFlag(131072);
        this.npc17.disableDTKFlag(8);
        this.npc17.talkto("Talk_npc17");
        this.npc17.start(1, "SOUSAKU");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_1() {
    }

    void npcset_2() {
    }

    void npcset_3() {
        this.npc20 = new NPC_NORMAL(1, 20, 0, 13, 5, -1.22f, -2.0f, 14.01f, 270.0f);
        this.npc21 = new NPC_NORMAL(278, 21, 0, 13, 3, -11.61f, -2.0f, 13.87f, 90.0f);
        this.npc22 = new NPC_NORMAL(3, 22, 0, 13, 7, -0.34f, -2.0f, 14.67f, 270.0f);
        this.npc23 = new NPC_NORMAL(6, 23, 0, 13, 9, -0.57f, -2.0f, 13.01f, 270.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setMotion(0, 1);
        this.npc20.enableDTKFlag(262144);
        this.npc20.kickEnepc(4, 1);
        this.npc21.setInvalidID(1);
        this.npc21.setMotion(0, 1);
        this.npc21.enableDTKFlag(262144);
        this.npc21.kickEnepc(4, 1);
        this.npc22.setInvalidID(1);
        this.npc22.setMotion(0, 1);
        this.npc22.enableDTKFlag(262144);
        this.npc22.kickEnepc(4, 1);
        this.npc23.setInvalidID(1);
        this.npc23.setMotion(0, 1);
        this.npc23.enableDTKFlag(262144);
        this.npc23.kickEnepc(4, 1);
        this.npc17 = new NPC_NORMAL(1, 17, 0, 13, 5, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc17.setInvalidID(1);
        this.npc17.disableDTKFlag(131072);
        this.npc17.disableDTKFlag(8);
        this.npc17.talkto("Talk_npc17");
        this.npc17.start(1, "help");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
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

        void SOUSAKU() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7033, 1, 1);
            Runtime.enable(65536);
            ST1211.this.cam0.setMode(-1);
            ST1211.this.EV_Camera01();
            ST1211.this.npc20.kickEnepc(1, 1);
            ST1211.this.npc21.kickEnepc(1, 1);
            ST1211.this.npc22.kickEnepc(1, 1);
            ST1211.this.npc21.moveEnepc(15, 21.74f, -20.96f, 40);
            System.sleep(20);
            ST1211.this.npc20.moveEnepc(15, 23.23f, -22.15f, 40);
            System.sleep(5);
            ST1211.this.npc22.moveEnepc(15, 23.64f, -21.26f, 40);
            System.sleep(15);
            ST1211.this.npc21.moveEnepc(17, 90.0f, 0.0f, 20);
            ST1211.this.npc21.kickEnepc(1, 9);
            System.sleep(20);
            ST1211.this.npc20.kickEnepc(1, 10);
            System.sleep(5);
            ST1211.this.npc22.kickEnepc(1, 10);
            ST1211.this.win = Window.create();
            ST1211.this.win.setSize(4, 45);
            ST1211.this.win.setLocation(15, 305);
            ST1211.this.win.print(ST1211.this.msgHAMMER1, 0);
            ST1211.waitPage(ST1211.this.win, 64);
            ST1211.this.npc21.moveEnepc(17, 270.0f, -0.1f, 10);
            System.sleep(10);
            ST1211.this.npc21.kickEnepc(1, 3);
            ST1211.this.npc21.moveEnepc(15, 18.35f, -20.96f, 30);
            System.sleep(20);
            ST1211.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            Runtime.jumpCF(1210, 1);
        }

        void help() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST1211.this.cam0.setMode(-1);
            ST1211.this.EV_Camera02();
            ST1211.this.npc20.kickEnepc(1, 1);
            ST1211.this.npc21.kickEnepc(1, 3);
            ST1211.this.npc22.kickEnepc(1, 1);
            ST1211.this.npc23.kickEnepc(1, 1);
            ST1211.this.npc20.moveEnepc(15, -3.22f, 14.01f, 50);
            System.sleep(5);
            ST1211.this.npc22.moveEnepc(15, -2.34f, 14.678f, 50);
            ST1211.this.npc23.moveEnepc(15, -2.57f, 13.01f, 55);
            System.sleep(10);
            System.sleep(20);
            ST1211.this.npc21.moveEnepc(15, -4.25f, 14.02f, 20);
            System.sleep(15);
            ST1211.this.npc20.kickEnepc(1, 10);
            System.sleep(5);
            ST1211.this.npc22.kickEnepc(1, 10);
            System.sleep(5);
            ST1211.this.npc23.kickEnepc(1, 0);
            ST1211.this.npc21.kickEnepc(1, 9);
            ST1211.this.win = Window.create();
            ST1211.this.win.setSize(4, 45);
            ST1211.this.win.setLocation(15, 305);
            ST1211.this.win.print(ST1211.this.msghama1, 0);
            ST1211.waitPage(ST1211.this.win, 64);
            ST1211.this.npc20.kickEnepc(1, 9);
            ST1211.this.win.print(ST1211.this.msghama2, 0);
            ST1211.waitPage(ST1211.this.win, 64);
            ST1211.this.npc21.kickEnepc(1, 9);
            ST1211.this.win.print(ST1211.this.msghama3, 0);
            ST1211.waitPage(ST1211.this.win, 64);
            ST1211.this.npc21.kickEnepc(1, 3);
            ST1211.this.npc21.moveEnepc(17, 270.0f, 0.0f, 10);
            System.sleep(5);
            ST1211.this.npc21.moveEnepc(15, -11.61f, 13.87f, 30);
            System.sleep(20);
            ST1211.this.npc20.kickEnepc(1, 3);
            ST1211.this.npc20.moveEnepc(15, -8.22f, 14.01f, 40);
            System.sleep(10);
            ST1211.this.npc22.kickEnepc(1, 3);
            ST1211.this.npc22.moveEnepc(15, -7.34f, 14.678f, 40);
            System.sleep(5);
            ST1211.this.npc23.kickEnepc(1, 3);
            ST1211.this.npc23.moveEnepc(15, -7.57f, 13.01f, 30);
            ST1211.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            Runtime.setFlags(160, 1, 1);
            Runtime.setFlags(162, 1, 1);
            Runtime.jumpEvent(2404);
            System.println("フラグオン！");
        }
    }
}

