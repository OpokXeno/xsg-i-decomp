import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
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

class ST1210
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
    int guti = 0;
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
    Uwamono col1;
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
    String[] msg0022 = new String[]{"/[label()]", "The nursery? It's already been abandoned.", "/[waitkey(64)]/[close()]"};
    String[] msg0031 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right.", "/[waitkey(1)]/[clear()]", "There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here.", "/[waitkey(1)]/[clear()]", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg0032 = new String[]{"/[label()]", "What?! You're looking for a soldier?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably lying dead in some alleyway.", "/[waitkey(1)]/[clear()]", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg0041 = new String[]{"/[label()]", "Hey, you there! You better be more careful or you're gonna hit something.", "/[waitkey(64)]/[close()]"};
    String[] msg00412 = new String[]{"/[label()]", "(Crash!)", "/[waitkey(64)]/[close()]"};
    String[] msg00413 = new String[]{"/[label()]", "Oh man, I did it again...\n", "/[waitkey(1)]/[clear()]", "Oh well, it's got so many dents and scratches already, no one will notice another one.", "/[waitkey(64)]/[close()]"};
    String[] msg0042 = new String[]{"/[label()]", "Oh, you weren't watching, were ya?", "/[waitkey(1)]/[clear()]", "What?! That ship's yours?", "/[waitkey(1)]/[clear()]", "O-oh, don't worry. Everything will be fine, just leave it to us!", "/[waitkey(64)]/[close()]"};
    String[] msg00421 = new String[]{"/[label()]", "(Crash!)", "/[waitkey(64)]/[close()]"};
    String[] msg00422 = new String[]{"/[label()]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0051 = new String[]{"/[label()]", "Sheesh, planets vanishing, Gnosis appearing, the universe is becoming a chaotic place. With all this commotion, I don't have any time to rest.", "/[waitkey(1)]/[clear()]", "But then again, thanks to that, I can put food on the table.", "/[waitkey(64)]/[close()]"};
    String[] msg0052 = new String[]{"/[label()]", "The Federation sure is awful. As soon as the war was over, they didn't care about anything else. They didn't even give one thought to people like us, who went through terrible times.", "/[waitkey(1)]/[clear()]", "But I guess thanks to that, we get to do whatever we please.", "/[waitkey(64)]/[close()]"};
    String[] msg0061 = new String[]{"/[label()]", "What? Have I seen a Commander wandering around?", "/[waitkey(1)]/[clear()]", "Hey, this is the wrong place to be talking about the military!", "/[waitkey(1)]/[clear()]", "Don't you know anything? They came and destroyed everything without discrimination, just because this\nwas a Realian nursery.", "/[waitkey(64)]/[close()]"};
    String[] msg0062 = new String[]{"/[label()]", "Military and public officials are the scum of mankind. They started the war on their own and then dragged us into it. It's because of those good-for-nothings that I live like this now.", "/[waitkey(64)]/[close()]"};
    String[] msg0063 = new String[]{"/[label()]", "Don't you guys give up? I told you, I don't know!", "/[waitkey(1)]/[clear()]", "Your commander person is probably dead in some ditch. Soldiers don't die decent deaths around here!", "/[waitkey(64)]/[close()]"};
    String[] msg0071 = new String[]{"/[label()]", "Oh, I haven't seen you around before. Can I help you with something?", "/[waitkey(1)]/[clear()]", "Commander Cherenkov? So you're looking for a soldier? Are you an idiot or something?! Looking for a soldier on this colony? You must be insane!", "/[waitkey(64)]/[close()]"};
    String[] msg0072 = new String[]{"/[label()]", "After the conflict on Miltia, public officials and military people treated us like garbage and abandoned us in this remote region.", "/[waitkey(1)]/[clear()]", "Everyone here hates government officials. If you claim to be friends with a soldier, you better watch your back!", "/[waitkey(64)]/[close()]"};
    String[] msg0073 = new String[]{"/[label()]", "Oh, you're the one who was looking for that soldier.\nWhat do you want?", "/[waitkey(1)]/[clear()]", "You didn't find him anyway, right? Just give up already and leave this colony!", "/[waitkey(64)]/[close()]"};
    String[] msg0081 = new String[]{"/[label()]", "Hello, are you here on business? Not that we have anything worth seeing here, but please make yourselves at home.", "/[waitkey(64)]/[close()]"};
    String[] msg0082 = new String[]{"/[label()]", "Um, please don't let it get to you. Marines killed her family during the Miltian Conflict.", "/[waitkey(1)]/[clear()]", "Everyone related to this facility was branded as traitors and got taken away by the Federation. After the war, we demanded compensation, but they said it was one\nrogue unit's fault, and that was the end of it.", "/[waitkey(1)]/[clear()]", "That's why she gets like that when she hears about soldiers and public officials.", "/[waitkey(64)]/[close()]"};
    String[] msg0083 = new String[]{"/[label()]", "All the people here either hate or are dissatisfied with the Federation government to some extent.", "/[waitkey(64)]/[close()]"};
    String[] msg0091 = new String[]{"/[label()]", "What's the matter? Why the long face? Take a look at these people.", "/[waitkey(1)]/[clear()]", "All the people here either escaped from the Miltian Conflict 14 years ago, or recently ran away from the Gnosis phenomenon.", "/[waitkey(1)]/[clear()]", "But even after all they've been through, they're living as best they can.", "/[waitkey(64)]/[close()]"};
    String[] msg0092 = new String[]{"/[label()]", "I also come here when things start getting to me. You see, seeing everyone's faces makes me feel like I gotta keep going!", "/[waitkey(1)]/[clear()]", "You people better do your best, too!", "/[waitkey(64)]/[close()]"};
    String[] msg00A1 = new String[]{"/[label()]", "Recently, more spaceships have been coming to this dock due to the battles against the Gnosis.", "/[waitkey(1)]/[clear()]", "Nowadays, even civilian ships must equip weapons if they hope to get around at all. It's become an unpleasant world.", "/[waitkey(64)]/[close()]"};
    String[] msg00A2 = new String[]{"/[label()]", "I don't really care, but how long do you suppose that lady's going to stay there?", "/[waitkey(1)]/[clear()]", "She's in my way...it's really difficult to get my work done if she stays there.", "/[waitkey(64)]/[close()]"};
    String[] msg00A3 = new String[]{"/[label()]", "Um, could you also move out of my way please? You're in the way of my work.", "/[waitkey(64)]/[close()]"};
    String[] msg00B1 = new String[]{"/[label()]", "/[waitkey(64)]/[close()]", "Say, do you know about the planetary disappearance incident?", "/[waitkey(64)]/[close()]", "/[label()]", "A planet called Ariadne completely disappeared. The Federation government still doesn't know the cause of it, and the people who disappeared with the planet are still missing too. Some think it's the work of Gnosis. In any case, it's a terrible incident.", "/[waitkey(64)]/[close()]", "/[label()]", "The truth of that incident is still a mystery. I can't believe that a planet could disappear without a trace.", "/[waitkey(64)]/[close()]"};
    String[] msg00C1 = new String[]{"/[label()]", "That girl named Luty at the clinic refuses to talk at all!", "/[waitkey(1)]/[clear()]", "I was being nice and said I'd play with her. She's such a brat.", "/[waitkey(64)]/[close()]"};
    String[] msg00C2 = new String[]{"/[label()]", "That girl named Luty at the clinic refuses to talk at all!", "/[waitkey(1)]/[clear()]", "She just keeps messing with the vase. She's no fun at all.", "/[waitkey(64)]/[close()]"};
    String[] msg00D1 = new String[]{"/[label()]", "Luty used to live on Ariadne until just recently. But when the planet disappeared, she moved to the colony. Both her parents disappeared with the planet, so she's living with the doctor now.", "/[waitkey(64)]/[close()]"};
    String[] msg00E1 = new String[]{"/[label()]", "Guess what? He has a crush on Luty! That's why he's always messing with her.", "/[waitkey(64)]/[close()]"};
    String[] msg00E2 = new String[]{"/[label()]", "T-that's not true. It isn't like that! Why would I like her!!", "/[waitkey(64)]/[close()]"};
    String[] msg00E3 = new String[]{"/[label()]", "Heh heh, the way you get all worked up...it's awfully suspicious...nya-nya-nya-nya-nya!", "/[waitkey(64)]/[close()]"};
    String[] msg00F1 = new String[]{"/[label()]", "Sorry,\n", "the lines for the email are being modified.\n", "\n", "Nothing will happen if you talk to me now,\n", "so please wait a little longer.", "/[waitkey(64)]/[close()]"};
    String[] msg00F11 = new String[]{"/[label()]", "My husband's love of fishing is such a problem. He goes fishing every chance he gets...it's really an addiction.", "/[waitkey(1)]/[clear()]", "But the other day, he lost a tool called a /[color(0x329bbe)]Fish Detector/[color(0x808080)] or something. He was so depressed. Well, I'm thankful though, since I won't have to hear his fishing stories\nfor a while.", "/[waitkey(64)]/[close()]"};
    String[] msg00F12 = new String[]{"/[label()]", "To tell you the truth, I'm the one that lost that /[color(0x329bbe)]Fish Detector/[color(0x808080)]. I made a mistake and put it in the /[color(0x329bbe)]Tuxedo/[color(0x808080)] pocket, then sent it off to the cleaners on the Foundation.", "/[waitkey(1)]/[clear()]", "But don't tell my husband, okay? I don't know what he'd do if he found out.", "/[waitkey(64)]/[close()]"};
    String[] msg00F13 = new String[]{"/[label()]", "I'm sure that if I went to the cleaners on the Foundation right now, I'd find the /[color(0x329bbe)]Fish Detector/[color(0x808080)], but it's very far from here.", "/[waitkey(1)]/[clear()]", "I really don't have a chance to go get it unless I have some other errand to run there. Well, neither the /[color(0x329bbe)]Tuxedo/[color(0x808080)] nor the /[color(0x329bbe)]Fish Detector/[color(0x808080)] get used much, so it's okay.", "/[waitkey(64)]/[close()]"};
    String[] msg00F2 = new String[]{"/[label()]", "What's with that shopkeeper? I wonder if he hates \"○○○?\"", "/[waitkey(64)]/[close()]"};
    String[] msg00G1 = new String[]{"/[label()]", "What? You got a problem? The lines for the email are being modified. Leave me alone.", "/[waitkey(64)]/[close()]"};
    String[] msg00G2 = new String[]{"/[label()]", "You the one the shopkeeper told me about? I see...you do look like a bad person.", "/[waitkey(1)]/[clear()]", "You got the money ready? ○○○○G in cash. I won't take anything less.", "/[waitkey(64)]/[close()]"};
    String[] msg00G3 = new String[]{"/[label()]", "Man, that's sad. Get the hell outta here if you don't got the cash!", "/[waitkey(64)]/[close()]"};
    String[] msg00G4 = new String[]{"/[label()]", "Okay, it's all there. Here you go!", "/[waitkey(1)]/[clear()]", "Obtained \"Key to Hidden Corridor.\"", "/[waitkey(1)]/[clear()]", "You can open the corridor in the back of the shop with that key. The goods are in there.", "/[waitkey(64)]/[close()]"};
    String[] msg0111 = new String[]{"/[label()]", "Yo, having fun?", "/[waitkey(1)]/[clear()]", "The ship's repairs are done. You can launch anytime.", "/[waitkey(64)]/[close()]"};
    String[] msg0112 = new String[]{"/[label()]", "Oh, yeah, I hear someone was attacked over in that alleyway just now. You guys better be careful too.", "/[waitkey(64)]/[close()]"};
    String[] msg0121 = new String[]{"/[label()]", "Oh, did you hear? Apparently, there was a mugging in the alleyway nearby. Several men were attacked and they were taken to the clinic there.", "/[waitkey(1)]/[clear()]", "It's a dangerous world we live in...", "/[waitkey(64)]/[close()]"};
    String[] msg0122 = new String[]{"/[label()]", "What? The perpetrator?", "/[waitkey(1)]/[clear()]", "Yes, I heard the thug was a soldier! It really makes me mad. In the end, soldiers are barbaric, good-for-nothing idiots!", "/[waitkey(64)]/[close()]"};
    String[] msg0131 = new String[]{"/[label()]", "Oh, we finished the repairs. Well? Looks perfect, doesn't it?!", "/[waitkey(64)]/[close()]"};
    String[] msg01311 = new String[]{"/[label()]", "(Crash!)", "/[waitkey(64)]/[close()]"};
    String[] msg01312 = new String[]{"/[label()]", "Whoops...", "/[waitkey(64)]/[close()]"};
    String[] msg0132 = new String[]{"/[label()]", "Ha ha ha, don't worry, don't worry. We'll fix it right away!", "/[waitkey(1)]/[clear()]", "Ha ha ha...", "/[waitkey(64)]/[close()]"};
    String[] msg0133 = new String[]{"/[label()]", "Oh, more repairs? Your ship certainly breaks down a lot.", "/[waitkey(1)]/[clear()]", "Oh, don't worry, it'll be fine. We'll make it look as good as new!", "/[waitkey(64)]/[close()]"};
    String[] msg0134 = new String[]{"/[label()]", "Are you expecting something will happen? Too bad, but nothing's going to happen.", "/[waitkey(64)]/[close()]"};
    String[] msg0135 = new String[]{"/[label()]", "(Crash!)", "/[waitkey(64)]/[close()]"};
    String[] msg0136 = new String[]{"/[label()]", "Oh...", "/[waitkey(64)]/[close()]"};
    String[] msg0141 = new String[]{"/[label()]", "What? You guys are still here?", "/[waitkey(1)]/[clear()]", "Some folks got attacked over there just now. You didn't do that, did you?", "/[waitkey(64)]/[close()]"};
    String[] msg0142 = new String[]{"/[label()]", "Let me give you a piece of advice. Nothing good will come of stirring up trouble here. If you've got no business here, you should get going.", "/[waitkey(64)]/[close()]"};
    String[] msg0151 = new String[]{"/[label()]", "Say, did you know?", "/[waitkey(64)]/[close()]", "/[label()]", "Some people were attacked in that alleyway just now. The perpetrator is supposedly a Federation soldier. It's such a dangerous world.", "/[waitkey(64)]/[close()]", "/[label()]", "Isn't it so dangerous? The people who were attacked seem to be in serious condition. There's also a rumor that there's a soldier wandering around...I don't feel safe walking around outside.", "/[waitkey(64)]/[close()]"};
    String[] msg0211 = new String[]{"/[label()]", "Hey, more repairs? Well, relax and take a load off for a while.", "/[waitkey(64)]/[close()]"};
    String[] msg0221 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right.", "/[waitkey(1)]/[clear()]", "There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here.", "/[waitkey(1)]/[clear()]", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg0222 = new String[]{"/[label()]", "What?! You're looking for a soldier?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably laying dead in some alleyway.", "/[waitkey(1)]/[clear()]", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg0231 = new String[]{"/[label()]", "Darned soldiers do nothing but strut around. They don't know anything about what we've been through!", "/[waitkey(64)]/[close()]"};
    String[] msg0232 = new String[]{"/[label()]", "Military and public officials are the scum of mankind. They started the war on their own and then dragged us into it.", "/[waitkey(1)]/[clear()]", "It's because of those good-for-nothings that I live like this now.", "/[waitkey(64)]/[close()]"};
    String[] msghama1 = new String[]{"/[label(Hammer)]", "Oh!! S-S-S-S-Shion! I-I-I-It's terrible! It's terrible!!", "/[waitkey(1)]/[clear()]", "In the thug, the alleyway is beat up all Commander!!", "/[waitkey(64)]/[clear()]"};
    String[] msghama2 = new String[]{"/[label(Shion)]", "Uh, um, Hammer? I can't understand you at all. Calm\ndown a little...", "/[waitkey(64)]/[clear()]"};
    String[] msghama3 = new String[]{"/[label(Hammer)]", "There's no time to calm down!", "/[waitkey(1)]/[clear()]", "Come on, hurry!", "/[waitkey(1)]/[clear()]", "Come this way, quickly!!", "/[waitkey(64)]/[close()]"};
    String[] msgHAMMER1 = new String[]{"/[label(Hammer)]", "Well, let's start looking for the Commander. We should split up to look for him. I'll let you know as soon as I find him!", "/[waitkey(64)]/[close()]"};
    String[] msg05000001 = new String[]{"/[label(Matthews)]", "What? You want to go back to the Dock Colony? Well, I'll take you anywhere as long as I get paid.", "/[waitkey(64)]/[close()]"};
    String[] msgikude = new String[]{"/[label(Matthews)]", "All right, we're launching the Elsa. Hurry up and get onboard!", "/[waitkey(64)]/[close()]"};
    String[] msgikude2 = new String[]{"/[label(Matthews)]", "All right, we're launching the Elsa. Hurry up and get onboard!", "/[waitkey(64)]/[close()]"};
    String[] msghayosei = new String[]{"/[label(Matthews)]", "What? Not going?", "/[waitkey(1)]/[clear()]", "Man, don't come talk to me if you've got no business. We're getting irritated here without any decent work.", "/[waitkey(64)]/[close()]"};
    String[] msg05000002 = new String[]{"/[label(Tony)]", "What? Forget to buy something?", "/[waitkey(1)]/[clear()]", "Hurry up, okay? The Captain looks like he's ready to go on a rampage at anytime.", "/[waitkey(64)]/[close()]"};
    String[] msg001 = new String[]{"/[label(Matthews)]", "What? Something happen?", "/[waitkey(1)]/[clear()]", "MOMO? No, she hasn't come this way. Maybe she's\ntaking a walk around the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msg002 = new String[]{"/[label(Matthews)]", "Yo, it's rare to see you by yourself. Where's everyone else?", "/[waitkey(64)]/[close()]"};
    String[] msg003 = new String[]{"/[label(Matthews)]", "Hey, it's you! Glad you made it.", "/[font(KOS-MOS)]?\n", "Oh, the tuning room's safe for now.", "/[waitkey(64)]/[close()]"};
    String[] msg004 = new String[]{"/[label(Matthews)]", "Hey! Don't tell me you're going to go out?! It's full of Gnosis out there! This is no time for taking a leisurely cruise!", "/[waitkey(1)]/[clear()]", "All right, all right, I guess common sense doesn't work on you. So? Where do you want to go?", "/[waitkey(64)]/[close()]"};
    String[] msg006 = new String[]{"/[label(Matthews)]", "What's wrong? Did you rescue MOMO?", "/[waitkey(1)]/[clear()]", "I'll wait here for you, so hurry up and go rescue her!", "/[waitkey(64)]/[close()]"};
    String[] msg007 = new String[]{"/[label(Matthews)]", "What? You wanna go somewhere?\n", "Just let me know\n", "and I'll take you anywhere.", "/[waitkey(64)]/[close()]"};
    String[] msg008 = new String[]{"/[label(Matthews)]", "What is it? You want to go back to the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msgNOTGO = new String[]{"/[label(Matthews)]", "Hey, now, don't tell me you want to go somewhere else. Cause too bad for you, but we won't be able to go anywhere until we reach the Kukai Foundation.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msg0005 = new String[]{"/[label()]", "You're going to the Song, right?\n", "I'm ready!", "/[waitkey(64)]/[close()]"};
    String[] msg00051 = new String[]{"/[label(Matthews)]", "Man, I can't believe I'm stuck doing this. You better fork over a lot of hazard pay!", "/[waitkey(64)]/[close()]"};
    String[] msg00052 = new String[]{"/[label(Matthews)]", "What? Aren't you ready yet!? We don't have time. Hurry up!", "/[waitkey(64)]/[close()]"};
    String[] msg00721 = new String[]{"/[label(Matthews)]", "Hey, you just gonna leave that Albedo guy there?! You're certainly easygoing considering the critical situation we're in!", "/[waitkey(64)]/[close()]"};

    ST1210() {
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
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7036, 1, 1);
                System.println("フラグオン！");
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(1211, 1);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc1_3(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc1_2(window);
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
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0092, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00A2, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00A3, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
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
            this.Talk_npc14_2(window);
        } else {
            this.Talk_npc14_1(window);
        }
    }

    void Talk_npc14_1(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                window.print(this.msg00C1, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00C1, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc14_2(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                window.print(this.msg00C2, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00C2, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00E2, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00E3, 0);
        ST1210.waitPage(window, 64);
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
                window.print(this.msg00F11, 0);
                ST1210.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00F12, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00F13, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(this.win, 64);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Pay\nDon't pay");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.print(this.msg00G4, 0);
                        ST1210.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.print(this.msg00G3, 0);
                ST1210.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.print(this.msg00G1, 0);
        ST1210.waitPage(this.win, 64);
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

    void Talk_npc1_0() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNOTGO, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_1() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg05000001, 0);
        ST1210.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes please\nNo, I'm fine right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(33);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00721, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0001, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0002, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc1_2() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg001, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_2(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0111, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0112, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc1_3() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg002, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_3(Window window) {
        window.print(this.msg0211, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc1_4() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg003, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_5() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg004, 0);
        ST1210.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I want to go to the Dock Colony\nNo, I'm fine right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(33);
                Runtime.jumpCF(9051, 1);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_52() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg004, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_6() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0005, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go to the Song of Nephilim\nGo to the Durandal\nI'm staying put");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00051, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                Runtime.setFlags(7087, 1, 1);
                Runtime.setFlags(374, 1, 1);
                Runtime.jumpEvent(3461);
                return;
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00052, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00052, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_7() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg006, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_8() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg007, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go straight to Proto Merkabah\nGo to the Dock Colony\nNo, I'm okay for now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                if (Runtime.getFlags(390, 1) == 0) {
                    this.npc1.kickEnepc(9, -1);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgikude2, 0);
                    ST1210.waitPage(this.win, 64);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(7088, 1, 1);
                    Runtime.setFlags(390, 1, 1);
                    System.println("フラグオン！");
                    Runtime.jumpEvent(3543);
                    return;
                }
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9055, 1);
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_DOCK() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg008, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, please\nNo, not yet");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_DOCK2() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg008, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, please\nNo, not yet");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00721, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_TEN() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg007, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go to the Dock Colony\nGo to the Durandal\nNo, I'm okay for now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 0);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                return;
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST1210.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 0);
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST1210.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc2_1(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    public void Talk_npc20(Enepc enepc) {
        if (Runtime.getFlags(389, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK2();
            } else if (Runtime.getFlags(7088, 1) == 1) {
                this.Talk_npc1_TEN();
            } else {
                this.Talk_npc1_8();
            }
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc1_7();
        } else if (Runtime.getFlags(373, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_5();
            }
        } else if (Runtime.getFlags(362, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_5();
            }
        } else if (Runtime.getFlags(360, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_5();
            }
        } else if (Runtime.getFlags(346, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_1();
            }
        } else if (Runtime.getFlags(7162, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_1();
            }
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.Talk_npc1_4();
        } else if (Runtime.getFlags(314, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_1();
            }
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.Talk_npc1_3();
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.Talk_npc1_0();
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.Talk_npc1_2();
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_npc1_0();
        } else {
            this.Talk_npc1_0();
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg0011, 0);
                ST1210.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0012, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0013, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0022, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0032, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc5(Enepc enepc) {
        if (this.S3 == 1) {
            this.Talk_npc5_3();
        } else if (this.S2042 == 1) {
            this.Talk_npc5_2();
        } else {
            this.Talk_npc5_1();
        }
    }

    void Talk_npc5_1() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.npc5.kickEnepc(0, 11);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0041, 0);
                ST1210.waitPage(this.win, 64);
                Sound.effectPlay(65539);
                System.sleep(60);
                this.npc5.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg00413, 0);
                ST1210.waitPage(this.win, 64);
                this.npc5.enableDTKFlag(5);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0042, 0);
        ST1210.waitPage(this.win, 64);
        Sound.effectPlay(65539);
        System.sleep(60);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00422, 0);
        ST1210.waitPage(this.win, 64);
    }

    void Talk_npc5_2() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.npc5.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0131, 0);
                ST1210.waitPage(this.win, 64);
                Sound.effectPlay(65539);
                System.sleep(60);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg01312, 0);
                ST1210.waitPage(this.win, 64);
                return;
            }
        }
        this.npc5.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0132, 0);
        ST1210.waitPage(this.win, 64);
    }

    void Talk_npc5_3() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.npc5.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0133, 0);
                ST1210.waitPage(this.win, 64);
                return;
            }
        }
        this.npc5.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0134, 0);
        ST1210.waitPage(this.win, 64);
        Sound.effectPlay(65539);
        System.sleep(60);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0136, 0);
        ST1210.waitPage(this.win, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0052, 0);
        ST1210.waitPage(window, 64);
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
                ST1210.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0062, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0063, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg0141, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0142, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc7_3(Window window) {
        window.print(this.msg0062, 0);
        ST1210.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc8_2(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc8_2(window);
        } else {
            this.Talk_npc8_1(window);
        }
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg0071, 0);
                ST1210.waitPage(window, 64);
                ++this.guti;
                return;
            }
        }
        window.print(this.msg0072, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg0073, 0);
                ST1210.waitPage(window, 64);
                ++this.guti;
                return;
            }
        }
        window.print(this.msg0072, 0);
        ST1210.waitPage(window, 64);
    }

    void Talk_npc8_3(Window window) {
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc9_1(window);
        } else {
            this.Talk_npc9_1(window);
        }
    }

    void Talk_npc9_1(Window window) {
        if (this.guti == 0) {
            window.print(this.msg0081, 0);
            ST1210.waitPage(window, 64);
            return;
        }
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg0082, 0);
                ST1210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0083, 0);
        ST1210.waitPage(window, 64);
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
                if (Runtime.getFlags(301, 1) == 1) {
                    Runtime.jumpCF(521, 2);
                    break;
                }
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
        this.S3 = Runtime.getFlags(301, 1);
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
        Stage.setVisible(12, false);
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
        this.cam0.setFog(1, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(2, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(3, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(4, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(5, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(6, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(7, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        this.cam0.setFog(8, 9.75f, 15.0f, 0.0f, 0.08f, 255, 255, 255, 255);
        Runtime.setDefocusQuick(0, 1, 5000, 1);
        Runtime.setDefocusQuick(1, 1, 4000, 1);
        Runtime.setDefocusQuick(2, 1, 3000, 1);
        Runtime.setDefocusQuick(3, 1, 2000, 1);
        this.PLANE1 = new Chr();
        this.PLANE1.init(20639, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PLANE1.setTranslate(16.5f, -1.0f, 4.6f);
        this.PLANE1.setRotate(0.0f, 90.0f, 0.0f);
        this.PLANE2 = new Chr();
        this.PLANE2.init(20639, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PLANE2.setTranslate(16.5f, -1.0f, 17.21f);
        this.PLANE2.setRotate(0.0f, 90.0f, 0.0f);
        this.ELSA = new Chr();
        this.ELSA.init(20482, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ELSA.setTranslate(45.0f, -5.0f, 33.0f);
        this.ELSA.setRotate(0.0f, 180.0f, 0.0f);
        this.ELSA.setScale(167.39f, 167.39f, 167.39f);
        this.PLANE1.dispRadar(false);
        this.PLANE2.dispRadar(false);
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
            this.npc20 = new NPC_NORMAL(275, 20, 11, 14, 20, 23.71f, 0.25f, -21.55f, 270.0f);
            this.npc20.disableDTKFlag(2);
            this.npc20.enableDTKFlag(4);
            this.npc20.setInvalidID(1);
            this.npc20.talkto("Talk_npc20");
            this.col1 = new Uwamono(28672, 24.71f, 0.25f, -21.55f, 0.0f);
            this.npcset_1();
        } else if (this.S2042 == 1) {
            this.npcset_1();
        } else if (this.hamahelp == 1) {
            this.npcset_1();
        } else if (this.S2040D == 1) {
            this.npcset_1();
        } else if (this.hamago == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(527, 1, 11, 14, 6, 18.88f, 0.25f, -20.81f, 0.0f);
        this.npc2 = new NPC_NORMAL(1546, 2, 0, 15, 8, 3.22f, 0.0f, -23.65f, 45.0f);
        this.npc3 = new NPC_NORMAL(1591, 3, 0, 13, 13, 3.92f, 0.0f, -23.13f, 220.0f);
        this.npc5 = new NPC_NORMAL(527, 5, 0, 15, 6, 5.34f, 0.0f, -10.15f, 45.0f);
        this.npc6 = new NPC_NORMAL(527, 6, 0, 14, 15, 23.3f, 0.0f, 10.67f, 90.0f);
        this.npc7 = new NPC_NORMAL(1537, 7, 13, 3, 6, -1.5f, -1.4f, -1.5f, 90.0f);
        this.npc8 = new NPC_NORMAL(1594, 8, 0, 13, 13, -2.57f, 0.0f, 10.41f, 160.0f);
        this.npc9 = new NPC_NORMAL(1592, 9, 0, 13, 13, -1.98f, 0.0f, 10.0f, 340.0f);
        this.npc10 = new NPC_NORMAL(1571, 10, 0, 15, 14, 8.5f, 2.1f, -2.0f, 125.0f);
        this.npc14 = new NPC_NORMAL(1558, 14, 11, 77, 7, 14.1f, 0.25f, -21.5f, 270.0f);
        this.npc17 = new NPC_NORMAL(1576, 17, 12, 9, 14, -4.0f, 0.0f, -23.0f, 270.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 9);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 9);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc5.disableDTKFlag(3);
        this.npc5.setMotion(0, 11);
        if (this.S2042 == 1) {
            this.npc5.enableDTKFlag(4);
        }
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc7.setMotion(0, 9);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 9);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 10);
        this.npc10.setMotion(0, 9);
        this.npc14.setMotion(1, 3);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc14.talkto("Talk_npc14");
        this.npc17.talkto("Talk_npc17");
    }

    void npcset_3() {
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
            ST1210.this.cam0.setMode(-1);
            ST1210.this.EV_Camera01();
            ST1210.this.npc20.kickEnepc(1, 1);
            ST1210.this.npc21.kickEnepc(1, 1);
            ST1210.this.npc22.kickEnepc(1, 1);
            ST1210.this.npc21.moveEnepc(15, 21.74f, -20.96f, 40);
            System.sleep(20);
            ST1210.this.npc20.moveEnepc(15, 23.23f, -22.15f, 40);
            System.sleep(5);
            ST1210.this.npc22.moveEnepc(15, 23.64f, -21.26f, 40);
            System.sleep(15);
            ST1210.this.npc21.moveEnepc(17, 90.0f, 0.0f, 20);
            ST1210.this.npc21.kickEnepc(1, 9);
            System.sleep(20);
            ST1210.this.npc20.kickEnepc(1, 10);
            System.sleep(5);
            ST1210.this.npc22.kickEnepc(1, 10);
            ST1210.this.win = Window.create();
            ST1210.this.win.setSize(4, 45);
            ST1210.this.win.setLocation(15, 305);
            ST1210.this.win.print(ST1210.this.msgHAMMER1, 0);
            ST1210.waitPage(ST1210.this.win, 64);
            ST1210.this.npc21.moveEnepc(17, 270.0f, -0.1f, 10);
            System.sleep(10);
            ST1210.this.npc21.kickEnepc(1, 3);
            ST1210.this.npc21.moveEnepc(15, 18.35f, -20.96f, 30);
            System.sleep(20);
            ST1210.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            Runtime.jumpCF(1210, 1);
        }

        void help() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST1210.this.cam0.setMode(-1);
            ST1210.this.EV_Camera02();
            ST1210.this.npc20.kickEnepc(1, 1);
            ST1210.this.npc21.kickEnepc(1, 3);
            ST1210.this.npc22.kickEnepc(1, 1);
            ST1210.this.npc23.kickEnepc(1, 1);
            ST1210.this.npc20.moveEnepc(15, -3.22f, 14.01f, 50);
            System.sleep(5);
            ST1210.this.npc22.moveEnepc(15, -2.34f, 14.678f, 50);
            ST1210.this.npc23.moveEnepc(15, -2.57f, 13.01f, 55);
            System.sleep(10);
            System.sleep(20);
            ST1210.this.npc21.moveEnepc(15, -4.25f, 14.02f, 20);
            System.sleep(15);
            ST1210.this.npc20.kickEnepc(1, 10);
            System.sleep(5);
            ST1210.this.npc22.kickEnepc(1, 10);
            System.sleep(5);
            ST1210.this.npc23.kickEnepc(1, 0);
            ST1210.this.npc21.kickEnepc(1, 9);
            ST1210.this.win = Window.create();
            ST1210.this.win.setSize(4, 45);
            ST1210.this.win.setLocation(15, 305);
            ST1210.this.win.print(ST1210.this.msghama1, 0);
            ST1210.waitPage(ST1210.this.win, 64);
            ST1210.this.npc20.kickEnepc(1, 9);
            ST1210.this.win.print(ST1210.this.msghama2, 0);
            ST1210.waitPage(ST1210.this.win, 64);
            ST1210.this.npc21.kickEnepc(1, 9);
            ST1210.this.win.print(ST1210.this.msghama3, 0);
            ST1210.waitPage(ST1210.this.win, 64);
            ST1210.this.npc21.kickEnepc(1, 3);
            ST1210.this.npc21.moveEnepc(17, 270.0f, 0.0f, 10);
            System.sleep(5);
            ST1210.this.npc21.moveEnepc(15, -11.61f, 13.87f, 30);
            System.sleep(20);
            ST1210.this.npc20.kickEnepc(1, 3);
            ST1210.this.npc20.moveEnepc(15, -8.22f, 14.01f, 40);
            System.sleep(10);
            ST1210.this.npc22.kickEnepc(1, 3);
            ST1210.this.npc22.moveEnepc(15, -7.34f, 14.678f, 40);
            System.sleep(5);
            ST1210.this.npc23.kickEnepc(1, 3);
            ST1210.this.npc23.moveEnepc(15, -7.57f, 13.01f, 30);
            ST1210.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            Runtime.setFlags(160, 1, 1);
            Runtime.setFlags(162, 1, 1);
            Runtime.jumpEvent(2404);
            System.println("フラグオン！");
        }
    }

    class NPC_NORMAL2
            extends Enepc {
        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
        }
    }
}

