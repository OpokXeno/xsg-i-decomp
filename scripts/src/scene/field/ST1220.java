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
import xeno.map.MC_KOU02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1220
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KOU02_PRJ {
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
    Enepc npc20;
    Enepc npc21;
    Enepc npc25;
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
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag6;
    int S2040C;
    int S2042;
    int S3;
    int Rouxtea = 0;
    int BUTTON_F = 0;
    Light light = new Light(0);
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    boolean WALL_FLAG = false;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Unit monitor1;
    Unit monitor2;
    Unit monitor3;
    Unit monitor4;
    Unit monitor5;
    Unit monitor6;
    Effect graph1;
    Effect graph2;
    Effect graph3;
    Effect graph4;
    Effect graph5;
    Effect graph6;
    Effect graph7;
    Effect heart;
    Effect lamp1;
    Effect lamp2;
    Effect red1;
    Effect red2;
    Effect blue1;
    Effect blue2;
    Effect blue3;
    Effect blue4;
    Effect blue5;
    Effect blue6;
    Effect moni_light;
    int page;
    String[] msg0001 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right.", "/[waitkey(1)]/[clear()]", "There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here.", "/[waitkey(1)]/[clear()]", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label()]", "What?! You're looking for a soldier?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably lying dead in some alleyway.", "/[waitkey(1)]/[clear()]", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg00011 = new String[]{"/[label()]", "Oh, hello. What do you think of this colony? Did you find any interesting places?", "/[waitkey(1)]/[clear()]", "Actually, just the other day there was a thug, and he injured several people. There isn't much to see in this colony, but there's plenty of excitement!", "/[waitkey(64)]/[close()]"};
    String[] msg0003 = new String[]{"/[label()]", "Who the heck are you guys? If you need treatment, take a seat and wait for your turn.", "/[waitkey(64)]/[close()]"};
    String[] msg0004 = new String[]{"/[label()]", "Sheesh, with the world the way it is, there are so many injured. I have no time to rest.", "/[waitkey(64)]/[close()]"};
    String[] msg0005 = new String[]{"/[label()]", "You mean Luty?", "/[waitkey(1)]/[clear()]", "Yes, what a poor child. But you know, her inability to speak is psychological. I can't do anything about it.", "/[waitkey(64)]/[clear()]"};
    String[] msg0006 = new String[]{"Well, you'd have to get her to open up first.", "/[waitkey(1)]/[clear()]", "If she has something to remind her of her home, family, and things of that nature, we might be able to get through to her.", "/[waitkey(64)]/[close()]"};
    String[] msg0007 = new String[]{"/[label()]", "Come to think of it, she takes really good care of the potted plant in the waiting room.", "/[waitkey(1)]/[clear()]", "Yes! I believe there is a beautiful flower that only grows in Ariadne! If we can get the /[color(0x329bbe)]Ariadne Flower/[color(0x808080)] to bloom, maybe that will give her some hope.", "/[waitkey(64)]/[clear()]"};
    String[] msg0008 = new String[]{"Unfortunately, Ariadne no longer exists in this world.", "/[waitkey(1)]/[clear()]", "Likewise, the Ariadne Flower also no longer exists in this world. Therefore, we cannot do anything.", "/[waitkey(64)]/[close()]"};
    String[] msg0009 = new String[]{"/[label()]", "It'd be impossible to get an /[color(0x329bbe)]Ariadne Flower/[color(0x808080)] and make it bloom.", "/[waitkey(64)]/[close()]"};
    String[] msg0010 = new String[]{"/[label()]", "Goodness, someone injured? Or someone with an acute illness? We don't need anyone here except the sick. Go on, get out of here!", "/[waitkey(64)]/[close()]"};
    String[] msg0011 = new String[]{"/[label()]", "What is it? If you pick on Luty, you won't get away with it!", "/[waitkey(1)]/[clear()]", "What? Oh, you're not? Sorry about that.", "/[waitkey(1)]/[clear()]", "Poor girl. As you can see, she's completely withdrawn.", "/[waitkey(64)]/[clear()]"};
    String[] msg0012 = new String[]{"Well, she lost her parents in that Ariadne incident. It's no wonder she's emotionally scarred.", "/[waitkey(64)]/[close()]"};
    String[] msg0013 = new String[]{"/[label()]", "She's so little and she's working so hard. I wish we could do something for her.", "/[waitkey(64)]/[close()]"};
    String[] msg0014 = new String[]{"/[label()]", "Hey, please don't come in here without permission.", "/[waitkey(64)]/[clear()]"};
    String[] msg0015 = new String[]{"Cherenkov?", "/[waitkey(1)]/[clear()]", "You're looking for a soldier? Then you'd better give up and leave the colony right away. If you don't, you won't get away scot-free either.", "/[waitkey(64)]/[close()]"};
    String[] msg00142 = new String[]{"/[label()]", "Hey, please don't come in here without permission. Also, wait your turn, and don't wander around!", "/[waitkey(64)]/[close()]"};
    String[] msg0016 = new String[]{"/[label()]", "Back when this place was a Realian nursery, the military treated everyone here horribly. That's why they're overly sensitive about the military and the\ngovernment.", "/[waitkey(64)]/[close()]"};
    String[] msg0017 = new String[]{"/[label()]", "Oh, but everyone here is very nice to Realians. We've had a long relationship with them, so we have a better understanding of them than any place in the\nFederation.", "/[waitkey(1)]/[clear()]", "When it comes down to it, the Realians were also victims of that war in Miltia.", "/[waitkey(64)]/[close()]"};
    String[] msg0018 = new String[]{"/[label()]", "Poor little thing...", "/[waitkey(1)]/[clear()]", "Such a darling girl...she went through such an awful tragedy, and now she can't talk. The doctor here is a quack, so he can't even cure her.", "/[waitkey(64)]/[close()]"};
    String[] msg00181 = new String[]{"What's the use of being a doctor, if you can't cure a little girl?!", "/[waitkey(64)]/[close()]"};
    String[] msg0019 = new String[]{"/[label()]", "What is it?", "/[waitkey(1)]/[clear()]", "Turn-'n-cough?!", "/[waitkey(1)]/[clear()]", "Hey, you're not the doctor. Not very nice of you, trying to play tricks on an old man.", "/[waitkey(64)]/[close()]"};
    String[] msg0020 = new String[]{"/[label()]", "What? No? Comb under? What, did I drop my comb somewhere?", "/[waitkey(1)]/[clear()]", "Ho ho ho, It's okay, I have lots of spares.", "/[waitkey(64)]/[close()]"};
    String[] msg0021 = new String[]{"/[label()]", "What? That's not it? Go under chair and cough?", "/[waitkey(1)]/[clear()]", "Now why the heck would I want to do such a thing? You people are really silly!", "/[waitkey(64)]/[close()]"};
    String[] msg0022 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0023 = new String[]{"/[label()]", "Luty used to live on Ariadne until just recently.\nBut when the planet disappeared, she moved to the colony.", "/[waitkey(1)]/[clear()]", "Her mother and father disappeared with the planet,\nso she lives with the doctor now.", "/[waitkey(64)]/[close()]"};
    String[] msg1001 = new String[]{"/[label()]", "Oh, did you hear? Apparently, there was a mugging in the alleyway nearby. Several men were attacked and they were taken to the clinic.", "/[waitkey(1)]/[clear()]", "It's a dangerous world we live in...", "/[waitkey(64)]/[close()]"};
    String[] msg1002 = new String[]{"/[label()]", "What? The perpetrator? ", "Yes, I heard the thug was a soldier! It really makes me mad. In the end, soldiers are barbaric, good-for-nothing idiots!", "/[waitkey(64)]/[close()]"};
    String[] msg1003 = new String[]{"/[label()]", "I'm busy with an emergency! If you want to chitchat, go somewhere else.", "/[waitkey(64)]/[close()]"};
    String[] msg1004 = new String[]{"/[label()]", "Come on, move it! You're getting in the way of the doctors! ", "Really, why do these troublesome things always happen?!", "/[waitkey(64)]/[close()]"};
    String[] msg1005 = new String[]{"/[label()]", "It seems that a soldier caused the commotion. Do you know something about this?", "/[waitkey(1)]/[clear()]", "If you're somehow related, you'd better leave the colony quickly.", "/[waitkey(64)]/[close()]"};
    String[] msg1006 = new String[]{"/[label()]", "If you stay any longer, don't come crying to me when something happens to you.", "/[waitkey(64)]/[close()]"};
    String[] msg1007 = new String[]{"/[label()]", "Ho ho ho. Luty works so hard. Such a big difference from my grandchild.", "/[waitkey(64)]/[close()]"};
    String[] msg1008 = new String[]{"/[label()]", "But for some reason, she won't talk to this old man. I wonder why? Did I do something to upset her?", "/[waitkey(64)]/[close()]"};
    String[] msg1009 = new String[]{"/[label()]", "Could it be that she hates me? Say, stranger, what do you think?", "/[waitkey(64)]/[close()]"};
    String[] msg1010 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg1011 = new String[]{"/[label()]", "Oh, hello! Tourists? Heh, yeah right.", "/[waitkey(1)]/[clear()]", "There's no way tourists would be visiting a place that was forsaken by the government. Besides, there aren't any tourist attractions here.", "/[waitkey(1)]/[clear()]", "But all the people here are good people. Of course, including me.", "/[waitkey(64)]/[close()]"};
    String[] msg1012 = new String[]{"/[label()]", "What?! You're looking for a soldier?", "/[waitkey(1)]/[clear()]", "You've got to be kidding! I don't know anything about a good for nothing soldier! Well, I wouldn't tell you, even if I did. He's probably lying dead in some alleyway.", "/[waitkey(1)]/[clear()]", "Public officials and military people are trash! Trash! No better than trash!!", "/[waitkey(64)]/[close()]"};
    String[] msg0037 = new String[]{"/[label()]", "//\tUse lines from 1.", "/[waitkey(64)]/[close()]"};
    String[] msg0038 = new String[]{"/[label()]", "//\tUse lines from 1.", "/[waitkey(64)]/[close()]"};
    String[] msg0039 = new String[]{"/[label()]", "//\tNo changes.", "/[waitkey(64)]/[close()]"};
    String[] msg0040 = new String[]{"/[label()]", "//\tNo changes.", "\n", "//Girl.", "/[waitkey(64)]/[close()]"};
    String[] msg0041 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg00421 = new String[]{"/[label(Shion)]", "Hello.", "/[waitkey(64)]/[clear()]"};
    String[] msg0042 = new String[]{"/[label(Luty)]", "...?!", "/[waitkey(64)]/[clear()]"};
    String[] msg0043 = new String[]{"/[label(Shion)]", "Here's a present. Will you take care of it for me? It should blossom into a pretty flower.", "/[waitkey(64)]/[close()]"};
    String[] msg00431 = new String[]{"/[label()]", "Gave her a /[color(0x329bbe)]Flower Seed/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] msg0044 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msg00441 = new String[]{"/[label(Shion)]", "Good luck.", "/[waitkey(64)]/[clear()]"};
    String[] msg00442 = new String[]{"/[label(Luty)]", "... *nod*", "/[waitkey(64)]/[close()]"};
    String[] msg00443 = new String[]{"/[label(Shion)]", "Do your best to get it to bloom pretty flowers, okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg0045 = new String[]{"/[label(Shion)]", "Oh, it's budding!", "/[waitkey(64)]/[clear()]"};
    String[] msg0050 = new String[]{"/[label(Luty)]", "... *nod*", "/[waitkey(64)]/[clear()]"};
    String[] msg00471 = new String[]{"/[label(Shion)]", "That's the spirit! Just a little more to go. Good luck.", "/[waitkey(64)]/[clear()]"};
    String[] msg0046 = new String[]{"/[label(Luty)]", "... *nod nod*", "/[waitkey(64)]/[close()]"};
    String[] msg00472 = new String[]{"/[label(Shion)]", "I wonder what kind of flower it will be? Isn't it exciting?", "/[waitkey(64)]/[clear()]"};
    String[] msg0047 = new String[]{"/[label(Shion)]", "But it doesn't look like it's doing too well. Maybe it doesn't have enough nutrients?", "/[waitkey(64)]/[close()]"};
    String[] msg0048 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0049 = new String[]{"/[label(Shion)]", "Don't look so sad. It'll be okay. Leave it to me! I'll go find some good fertilizer somewhere. Take good care of it until then!", "/[waitkey(64)]/[close()]"};
    String[] msg0051 = new String[]{"/[label(Shion)]", "Oh, the flower wilted...", "/[waitkey(64)]/[close()]"};
    String[] msg0052 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0053 = new String[]{"/[label(Shion)]", "I'm sorry, I guess we should have come to check on it more often.", "/[waitkey(64)]/[close()]"};
    String[] msg0054 = new String[]{"/[label(Luty)]", "...Humph!", "/[waitkey(64)]/[close()]"};
    String[] msg0055 = new String[]{"/[label(Shion)]", "Uh-oh...I think she hates me now.", "/[waitkey(64)]/[close()]"};
    String[] msg0056 = new String[]{"/[label(Shion)]", "The flower's still okay!", "/[waitkey(64)]/[close()]"};
    String[] msg0057 = new String[]{"/[label(Luty)]", "...! *nod*", "/[waitkey(64)]/[close()]"};
    String[] msg0058 = new String[]{"/[label(Shion)]", "Here, it's flower fertilizer. With this, I'm sure it'll bloom into a healthy flower.", "/[waitkey(64)]/[close()]"};
    String[] msg0059 = new String[]{"/[label(Luty)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0060 = new String[]{"/[label(Shion)]", "I'll come check again, so good luck taking care of it!", "/[waitkey(64)]/[clear()]"};
    String[] msg0061 = new String[]{"/[label(Luty)]", "...y... *nod*", "/[waitkey(64)]/[close()]"};
    String[] msg0062 = new String[]{"/[label(Shion)]", "Oh, it bloomed! What a cute and pretty flower. It's just like you, Luty.", "/[waitkey(64)]/[clear()]"};
    String[] msg0063 = new String[]{"/[label(Luty)]", "...yea.", "/[waitkey(64)]/[clear()]"};
    String[] msg0064 = new String[]{"/[label(Shion)]", "!", "/[waitkey(1)]/[clear()]", "You said something! You can talk now!", "/[waitkey(64)]/[clear()]"};
    String[] msg0065 = new String[]{"/[label(Luty)]", "Yeah...", "/[waitkey(64)]/[clear()]"};
    String[] msg0066 = new String[]{"/[label(Shion)]", "Oh, I'm so glad...! You'll be fine now!", "/[waitkey(64)]/[clear()]"};
    String[] msg0067 = new String[]{"/[label(Luty)]", "Yeah. Thank...you...Shion. Here...", "/[waitkey(64)]/[clear()]"};
    String[] msg0068 = new String[]{"/[label(Shion)]", "What? A present for me? Thank you, I'll treasure it!", "/[waitkey(64)]/[clear()]"};
    String[] msg0069 = new String[]{"/[label(Luty)]", "Okay. Let's...play together again sometime!", "/[waitkey(64)]/[close()]"};
    String[] msg00691 = new String[]{"/[label(Luty)]", "Shion, come...play again, okay?", "/[waitkey(64)]/[close()]"};
    String[] msg0070 = new String[]{"/[label(Shion)]", "Wow, this is amazing. So this is the Ariadne flower...", "/[waitkey(64)]/[close()]"};
    String[] msg0071 = new String[]{"/[label(Luty)]", "... *shake shake*", "/[waitkey(64)]/[close()]"};
    String[] msg0072 = new String[]{"/[label(Shion)]", "Maybe not. Well, of course not! Maybe the fertilizer wasn't such a good idea...", "/[waitkey(64)]/[close()]"};
    String[] msg0073 = new String[]{"/[label(Luty)]", "...I...hate...this...flo...wer...", "/[waitkey(64)]/[close()]"};
    String[] msg0074 = new String[]{"/[label(Shion)]", "Wha?!", "/[waitkey(1)]/[clear()]", "You can...you can talk now!", "/[waitkey(64)]/[close()]"};
    String[] msg0075 = new String[]{"/[label(Luty)]", "...I'm...scared...of...this...", "/[waitkey(64)]/[close()]"};
    String[] msg0076 = new String[]{"/[label(Shion)]", "Hmm...I don't know if I should be happy, but at least you can talk now!", "/[waitkey(64)]/[close()]"};
    String[] msg0077 = new String[]{"/[label(Luty)]", "Shion...is a...klutz...", "/[waitkey(64)]/[close()]"};
    String[] msg0078 = new String[]{"/[label(Shion)]", "Uh...sorry...", "/[waitkey(64)]/[close()]"};
    String[] msgdame = new String[]{"/[label()]", "Please stay in your seat until your name is called.", "/[waitkey(64)]/[close()]"};
    String[] msgHealth1 = new String[]{"/[label()]", "Height:         163cm\n", "Weight:          48kg\n", "Condition:      Good", "/[waitkey(64)]/[close()]"};
    String[] msgHealth11 = new String[]{"/[label(Shion)]", "Oh, no, I think I gained a little weight...", "/[waitkey(64)]/[close()]"};
    String[] msgHealth2 = new String[]{"/[label()]", "Height:         167cm\n", "Weight:          92kg\n", "Condition: Overweight", "/[waitkey(64)]/[close()]"};
    String[] msgHealth21 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgHealth3 = new String[]{"/[label()]", "Height:         169cm\n", "Weight:          53kg\n", "Condition:      Good", "/[waitkey(64)]/[close()]"};
    String[] msgHealth31 = new String[]{"/[label(chaos)]", "Well, I guess that's about right.", "/[waitkey(64)]/[close()]"};
    String[] msgHealth4 = new String[]{"/[label()]", "Height:         141cm\n", "Weight:          36kg\n", "Condition:      Good", "/[waitkey(64)]/[close()]"};
    String[] msgHealth41 = new String[]{"/[label(MOMO)]", "Oh, I'm healthy! I'm so happy!", "/[waitkey(64)]/[close()]"};
    String[] msgHealth5 = new String[]{"/[label()]", "Height:         197cm\n", "Weight:         Error\n", "Condition:    Unknown", "/[waitkey(64)]/[close()]"};
    String[] msgHealth51 = new String[]{"/[label(Ziggy)]", "Not surprising.", "/[waitkey(64)]/[close()]"};
    String[] msgHealth6 = new String[]{"/[label()]", "Height:         140cm\n", "Weight:          38kg\n", "Condition:      Good", "/[waitkey(64)]/[close()]"};
    String[] msgHealth61 = new String[]{"/[label(Jr.)]", "Man, this is stupid.", "/[waitkey(64)]/[close()]"};
    String[] msgHealth7 = new String[]{"/[label()]", "Height:         163cm\n", "Weight:          48kg\n", "Condition:      Good", "/[waitkey(64)]/[close()]"};
    String[] msgHealth71 = new String[]{"/[label()]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};
    String[] msgmed = new String[]{"/[label()]", "Oh, are you okay?", "/[waitkey(1)]/[clear()]", "You seem very tired. I could treat you if you'd like.\nNot to worry, this is a hospital after all.", "/[waitkey(64)]/[close()]"};
    String[] msgmed1 = new String[]{"/[label()]", "Okay, just hang on for a second. I'll have you feeling better in a jiffy.", "/[waitkey(64)]/[close()]"};
    String[] msgmed2 = new String[]{"/[label()]", "What? Are you really sure? Don't overdo it. It's really bad for you.", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};

    ST1220() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.102f, 1.671f, 2.631f);
        this.camEV.setRotate(-3.577f, 53.118f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.95f, 1.703f, -7.369f);
        this.camEV.setRotate(-22.338f, -150.114f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.788f, 1.511f, -4.554f);
        this.camEV.setRotate(-8.358f, 3.2f, 0.0f);
        this.camEV.setFov(44.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(196741);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                this.win = Window.create();
                this.win.setSize(2, 25);
                this.win.setLocation(15, 15);
                if (Runtime.getLeader() == 1) {
                    this.win.print(this.msgHealth1, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth11, 0);
                    ST1220.waitPage(this.win, 64);
                } else if (Runtime.getLeader() == 2) {
                    this.win.print(this.msgHealth2, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth21, 0);
                    ST1220.waitPage(this.win, 64);
                } else if (Runtime.getLeader() == 3) {
                    this.win.print(this.msgHealth3, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth31, 0);
                    ST1220.waitPage(this.win, 64);
                } else if (Runtime.getLeader() == 4) {
                    this.win.print(this.msgHealth4, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth41, 0);
                    ST1220.waitPage(this.win, 64);
                } else if (Runtime.getLeader() == 6) {
                    this.win.print(this.msgHealth5, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth51, 0);
                    ST1220.waitPage(this.win, 64);
                } else if (Runtime.getLeader() == 5) {
                    this.win.print(this.msgHealth6, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth61, 0);
                    ST1220.waitPage(this.win, 64);
                } else {
                    this.win.print(this.msgHealth7, 0);
                    ST1220.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgHealth71, 0);
                    ST1220.waitPage(this.win, 64);
                }
                System.sleep(20);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 1: {
                if (Runtime.getFlags(159, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(160, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7057, 2) != 2) {
                    return;
                }
                if (Runtime.getFlags(7150, 1) != 0) break;
                Runtime.mailArriveSet(42);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7150, 1, 1);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Read email\nDon't read email");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc1_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        window.print(this.msgdame, 0);
        ST1220.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc) {
        this.npc11.kickEnepc(1, 10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmed, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Treat\nDon't treat");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgmed1, 0);
                ST1220.waitPage(this.win, 64);
                System.sleep(15);
                Runtime.charAllRecovery();
                this.fade1.call(0);
                System.sleep(60);
                this.fade2.call(0);
                System.sleep(60);
                Sound.effectPlay(26);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_01, 0);
                ST1220.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgmed2, 0);
        ST1220.waitPage(this.win, 64);
    }

    void Talk_npc1_1(Window window) {
        switch (this.Rouxtea) {
            case 1: {
                ++this.talkFlag10;
                switch (this.talkFlag10) {
                    case 1: {
                        window.print(this.msg0005, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0006, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                    case 2: {
                        window.print(this.msg0007, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0008, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                }
                window.print(this.msg0009, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0003, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0004, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        switch (this.Rouxtea) {
            case 1: {
                ++this.talkFlag10;
                switch (this.talkFlag10) {
                    case 1: {
                        window.print(this.msg0005, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0006, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                    case 2: {
                        window.print(this.msg0007, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0008, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                }
                window.print(this.msg0009, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1003, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc2_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        switch (this.Rouxtea) {
            case 1: {
                ++this.talkFlag2;
                switch (this.talkFlag2) {
                    case 1: {
                        window.print(this.msg0011, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0012, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                }
                window.print(this.msg0013, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0010, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        switch (this.Rouxtea) {
            case 1: {
                ++this.talkFlag2;
                switch (this.talkFlag2) {
                    case 1: {
                        window.print(this.msg0011, 0);
                        ST1220.waitPage(window, 64);
                        window.print(this.msg0012, 0);
                        ST1220.waitPage(window, 64);
                        return;
                    }
                }
                window.print(this.msg0013, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1004, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0014, 0);
                ST1220.waitPage(window, 64);
                window.print(this.msg0015, 0);
                ST1220.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0016, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0017, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg1005, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1006, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg00142, 0);
                ST1220.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0016, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0017, 0);
        ST1220.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg0023, 0);
        ST1220.waitPage(window, 64);
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
            this.Talk_npc5_3(window);
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
                window.print(this.msg0001, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0002, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg1001, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1002, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg00011, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1002, 0);
        ST1220.waitPage(window, 64);
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
                window.print(this.msg0018, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00181, 0);
        ST1220.waitPage(window, 64);
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
            this.Talk_npc7_2(window);
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
                window.print(this.msg0019, 0);
                ST1220.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0020, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0021, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg1007, 0);
                ST1220.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg1008, 0);
                ST1220.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1009, 0);
        ST1220.waitPage(window, 64);
    }

    void Talk_npc7_3(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc8(Enepc enepc) {
        if (this.S3 == 1) {
            this.Talk_npc8_2();
        } else if (this.S2042 == 1) {
            this.Talk_npc8_1();
        } else {
            this.Talk_npc8_1();
        }
    }

    void Talk_npc8_1() {
        this.Rouxtea = 1;
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0022, 0);
        ST1220.waitPage(this.win, 64);
    }

    void Talk_npc8_2() {
        if (Runtime.getFlags(7131, 2) == 3) {
            if (Runtime.getFlags(7136, 1) == 0) {
                this.fade.call(0);
                System.sleep(30);
                Runtime.enable(65536);
                this.npc8.kickEnepc(9, 25);
                this.npc25.kickEnepc(9, 8);
                if (Runtime.getLeader() != 1) {
                    this.player.setTranslate(14.35f, 0.0f, -6.1f);
                    this.player.rotY(1, 210.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                } else {
                    this.player.setTranslate(100.0f, 0.0f, 100.0f);
                    this.player.rotY(1, 200.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                }
                this.npc8.moveEnepc(17, 270.0f, 0.1f, 0);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.npc25.setVisible(true);
                this.npc4.setVisible(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0062, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 8);
                this.win.print(this.msg0063, 0);
                ST1220.waitPage(this.win, 64);
                this.win.print(this.msg0064, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 7);
                this.win.print(this.msg0065, 0);
                ST1220.waitPage(this.win, 64);
                this.win.print(this.msg0066, 0);
                ST1220.waitPage(this.win, 64);
                this.win.print(this.msg0067, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 11);
                this.win.print(this.msg0068, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 7);
                this.win.print(this.msg0069, 0);
                ST1220.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                this.cam0.setMode(0);
                this.npc8.kickEnepc(9, -1);
                this.npc25.kickEnepc(9, -1);
                this.player.setTranslate(13.3f, 0.0f, -6.36f);
                this.npc25.setVisible(false);
                this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
                this.npc4.setVisible(true);
                Runtime.disable(65536);
                Runtime.setFlags(7136, 1, 1);
                Sound.effectPlay(6);
                Runtime.addItemWin(0, 44);
                return;
            }
            this.fade.call(0);
            System.sleep(30);
            Runtime.enable(65536);
            this.npc8.kickEnepc(9, 25);
            this.npc25.kickEnepc(9, 8);
            if (Runtime.getLeader() != 1) {
                this.player.setTranslate(14.43f, 0.0f, -5.33f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            } else {
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            }
            this.cam0.setMode(-1);
            this.EV_Camera02();
            this.npc25.setVisible(true);
            this.npc4.setVisible(false);
            this.npc8.kickEnepc(0, 7);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg00691, 0);
            ST1220.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(30);
            this.cam0.setMode(0);
            this.npc8.kickEnepc(9, -1);
            this.npc25.kickEnepc(9, -1);
            this.player.setTranslate(13.3f, 0.0f, -6.36f);
            this.npc25.setVisible(false);
            this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
            this.npc4.setVisible(true);
            Runtime.disable(65536);
            return;
        }
        if (Runtime.getFlags(7131, 2) == 2) {
            if (Runtime.getFlags(7135, 1) == 0) {
                this.fade.call(0);
                System.sleep(30);
                Runtime.enable(65536);
                this.npc8.kickEnepc(9, 25);
                this.npc25.kickEnepc(9, 8);
                if (Runtime.getLeader() != 1) {
                    this.player.setTranslate(14.35f, 0.0f, -6.1f);
                    this.player.rotY(1, 210.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                } else {
                    this.player.setTranslate(100.0f, 0.0f, 100.0f);
                    this.player.rotY(1, 200.0f, true);
                    this.player.mtn(28, 9, 1.0f, true);
                }
                this.npc8.moveEnepc(17, 270.0f, 0.1f, 0);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.npc25.setVisible(true);
                this.npc4.setVisible(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0045, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 7);
                this.win.print(this.msg0050, 0);
                ST1220.waitPage(this.win, 64);
                this.win.print(this.msg00471, 0);
                ST1220.waitPage(this.win, 64);
                this.npc8.kickEnepc(0, 7);
                this.win.print(this.msg0046, 0);
                ST1220.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                this.cam0.setMode(0);
                this.npc8.kickEnepc(9, -1);
                this.npc25.kickEnepc(9, -1);
                this.player.setTranslate(13.3f, 0.0f, -6.36f);
                this.npc25.setVisible(false);
                this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
                this.npc4.setVisible(true);
                Runtime.disable(65536);
                Runtime.setFlags(7135, 1, 1);
                return;
            }
            this.fade.call(0);
            System.sleep(30);
            Runtime.enable(65536);
            this.npc8.kickEnepc(9, 25);
            this.npc25.kickEnepc(9, 8);
            if (Runtime.getLeader() != 1) {
                this.player.setTranslate(14.43f, 0.0f, -5.33f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            } else {
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            }
            this.cam0.setMode(-1);
            this.EV_Camera02();
            this.npc25.setVisible(true);
            this.npc4.setVisible(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg0060, 0);
            ST1220.waitPage(this.win, 64);
            this.npc8.kickEnepc(0, 7);
            this.win.print(this.msg0061, 0);
            ST1220.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(30);
            this.cam0.setMode(0);
            this.npc8.kickEnepc(9, -1);
            this.npc25.kickEnepc(9, -1);
            this.player.setTranslate(13.3f, 0.0f, -6.36f);
            this.npc25.setVisible(false);
            this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
            this.npc4.setVisible(true);
            Runtime.disable(65536);
            return;
        }
        if (Runtime.getFlags(7131, 2) == 1) {
            this.fade.call(0);
            System.sleep(30);
            Runtime.enable(65536);
            this.npc8.kickEnepc(9, 25);
            this.npc25.kickEnepc(9, 8);
            if (Runtime.getLeader() != 1) {
                this.player.setTranslate(14.43f, 0.0f, -5.33f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            } else {
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            }
            this.cam0.setMode(-1);
            this.EV_Camera02();
            this.npc25.setVisible(true);
            this.npc4.setVisible(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg00443, 0);
            ST1220.waitPage(this.win, 64);
            this.npc8.kickEnepc(0, 7);
            this.win.print(this.msg00442, 0);
            ST1220.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(30);
            this.cam0.setMode(0);
            this.npc8.kickEnepc(9, -1);
            this.npc25.kickEnepc(9, -1);
            this.player.setTranslate(13.3f, 0.0f, -6.36f);
            this.npc25.setVisible(false);
            this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
            this.npc4.setVisible(true);
            Runtime.disable(65536);
            return;
        }
        if (Runtime.checkItem(10, 5) == 1) {
            this.fade.call(0);
            System.sleep(30);
            this.Rouxtea = 1;
            Runtime.removeItem(10, 5);
            Runtime.enable(65536);
            this.npc8.kickEnepc(9, 25);
            this.npc25.kickEnepc(9, 8);
            if (Runtime.getLeader() != 1) {
                this.player.setTranslate(14.43f, 0.0f, -5.33f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            } else {
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.player.rotY(1, 200.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
            }
            this.cam0.setMode(-1);
            this.EV_Camera02();
            this.npc25.setVisible(true);
            this.npc4.setVisible(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg00421, 0);
            ST1220.waitPage(this.win, 64);
            this.win.print(this.msg0042, 0);
            ST1220.waitPage(this.win, 64);
            this.win.print(this.msg0043, 0);
            ST1220.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg00431, 0);
            ST1220.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg0044, 0);
            ST1220.waitPage(this.win, 64);
            this.win.print(this.msg00441, 0);
            ST1220.waitPage(this.win, 64);
            this.npc8.kickEnepc(0, 7);
            this.win.print(this.msg00442, 0);
            ST1220.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(30);
            this.cam0.setMode(0);
            this.npc8.kickEnepc(9, -1);
            this.npc25.kickEnepc(9, -1);
            this.player.setTranslate(13.3f, 0.0f, -6.36f);
            this.npc25.setVisible(false);
            this.npc4.setTranslate(10.66f, 0.0f, 3.45f);
            this.npc4.setVisible(true);
            Runtime.disable(65536);
            Runtime.setFlags(7131, 2, 1);
            return;
        }
        this.Rouxtea = 1;
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0022, 0);
        ST1220.waitPage(this.win, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1210, 2);
                break;
            }
        }
    }

    void init() {
        this.S2040C = Runtime.getFlags(158, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S3 = Runtime.getFlags(301, 1);
        this.teiten1 = new Uwamono(28690, 8.0f, 0.0f, -5.0f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, -6.5f, 0.0f, -2.5f, 0.0f);
        this.teiten2.SetBgm(196612);
        this.teiten3 = new Uwamono(28690, -14.5f, 0.0f, 4.15f, 0.0f);
        this.teiten3.SetBgm(196613);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(129, false);
        Stage.setVisible(128, false);
        Stage.setVisible(130, false);
        if (Runtime.getFlags(7131, 2) == 3) {
            Stage.setVisible(162, false);
            Stage.setVisible(163, false);
            Stage.setVisible(164, false);
            this.light.setGlobalPointLightCol(0, 2.5f, 2.5f, 1.5f);
            this.light.setGlobalPointLightPos(0, 13.416f, 2.5f, -7.997f);
        } else if (Runtime.getFlags(7131, 2) == 2) {
            Stage.setVisible(162, false);
            Stage.setVisible(165, false);
            Stage.setVisible(164, false);
            this.light.setGlobalPointLightCol(0, 1.0f, 1.0f, 1.0f);
            this.light.setGlobalPointLightPos(0, 13.416f, 2.5f, -7.997f);
        } else {
            Stage.setVisible(162, false);
            Stage.setVisible(165, false);
            Stage.setVisible(163, false);
            Stage.setVisible(164, false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 9.25f, 45.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFLockX(5, 8.0f);
        this.monitor1 = new Unit();
        this.monitor1.init(24613, -8.361f, 2.301f, -2.206f, 45.0f);
        this.monitor1.setArgs(0, 0.0f, 0.0f, 1.224f, 1.374f);
        this.monitor1.setArgs(1, 20001, 0, 0, 0);
        this.monitor1.setArgs(2, 75, 0, 15, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        Stage.setVisible(41, false);
        this.monitor2 = new Unit();
        this.monitor2.init(24613, -7.181f, 1.219f, -2.591f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.0f, 1.0f, 0.545f);
        this.monitor2.setArgs(1, 20016, 0, 0, 0);
        this.monitor2.setArgs(2, 75, 0, 15, -1);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2.signal(1);
        Stage.setVisible(39, false);
        this.monitor3 = new Unit();
        this.monitor3.init(24613, -5.961f, 1.098f, -2.591f, 0.0f);
        this.monitor3.setArgs(0, 0.0f, 0.0f, 1.0f, 0.313f);
        this.monitor3.setArgs(1, 20003, 0, 0, 0);
        this.monitor3.setArgs(2, 75, 0, 15, -1);
        this.monitor3.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor3.signal(1);
        Stage.setVisible(38, false);
        this.monitor4 = new Unit();
        this.monitor4.init(24613, -4.923f, 1.663f, -1.921f, -63.5f);
        this.monitor4.setArgs(0, 0.0f, 0.0f, 1.258f, 1.0f);
        this.monitor4.setArgs(1, 20004, 0, 0, 0);
        this.monitor4.setArgs(2, 75, 0, 15, -1);
        this.monitor4.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor4.signal(1);
        Stage.setVisible(40, false);
        this.graph1 = new Effect(1618, 6.332f, 1.6f, 0.46f, 0.0f);
        this.graph1.setRotate(0.0f, 90.0f, 0.0f);
        this.heart = new Effect(1717, 6.332f, 1.65f, 0.15f, 0.0f);
        this.heart.setRotate(0.0f, 90.0f, 0.0f);
        this.heart.setScale(0.8f, 0.8f, 0.8f);
        this.lamp1 = new Effect(1405, 10.525f, 2.129f, -6.65f, 0.0f);
        this.lamp1.setScale(0.45f, 0.45f, 0.45f);
        this.lamp2 = new Effect(1637, 18.402f, 1.881f, 1.19f, 0.0f);
        this.lamp2.setScale(0.5f, 0.5f, 0.5f);
        this.red1 = new Effect(1539, 17.615f, 3.165f, -5.073f, 0.0f);
        this.red1.setRotate(0.0f, -45.0f, 0.0f);
        this.red1.setScale(0.4f, 1.35f, 1.0f);
        this.red2 = new Effect(1539, 17.415f, 3.0f, -5.273f, 0.0f);
        this.red2.setRotate(0.0f, -45.0f, 0.0f);
        this.red2.setScale(0.4f, 0.8f, 1.0f);
        this.blue1 = new Effect(1542, 17.415f, 3.425f, -5.273f, 0.0f);
        this.blue1.setRotate(0.0f, -45.0f, 0.0f);
        this.blue1.setScale(0.4f, 0.65f, 1.0f);
        this.blue2 = new Effect(1542, 8.667f, 2.921f, -3.339f, 0.0f);
        this.blue2.setRotate(0.0f, 45.0f, 0.0f);
        this.blue2.setScale(3.15f, 0.395f, 1.0f);
        this.blue3 = new Effect(1542, 18.504f, 3.118f, 3.0f, 0.0f);
        this.blue3.setRotate(0.0f, -90.0f, 0.0f);
        this.blue3.setScale(1.989f, 0.526f, 1.0f);
        this.blue4 = new Effect(1542, 6.346f, 2.616f, 3.23f, 0.0f);
        this.blue4.setRotate(0.0f, 90.0f, 0.0f);
        this.blue4.setScale(1.7f, 0.395f, 1.0f);
        this.blue5 = new Effect(1542, 4.932f, 2.616f, 3.23f, 0.0f);
        this.blue5.setRotate(0.0f, -90.0f, 0.0f);
        this.blue5.setScale(1.7f, 0.395f, 1.0f);
        this.blue6 = new Effect(1542, -10.017f, 2.64f, 3.23f, 0.0f);
        this.blue6.setRotate(0.0f, 90.0f, 0.0f);
        this.blue6.setScale(1.7f, 0.395f, 1.0f);
        this.moni_light = new Effect(1542, -14.506f, 1.396f, 4.82f, 0.0f);
        this.moni_light.setScale(2.0f, 1.0f, 1.0f);
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
        Stage.setColor(1.15f, 1.15f, 1.15f);
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        this.doorA = new Uwamono(122, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(123, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(124, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        if (this.S3 == 1) {
            this.npcset_3();
        } else if (this.S2042 == 1) {
            this.npcset_2();
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1600, 1, 0, 15, 5, -6.57f, 0.0f, -1.25f, 45.0f);
        this.npc2 = new NPC_NORMAL(1601, 2, 0, 9, 7, -4.4f, 0.0f, 3.42f, 0.0f);
        this.npc3 = new NPC_NORMAL(1601, 3, 0, 15, 7, 8.04f, 0.0f, -3.97f, 30.0f);
        this.npc4 = new NPC_NORMAL(1588, 4, 0, 77, 13, 10.66f, 0.0f, 3.45f, 180.0f);
        this.npc5 = new NPC_NORMAL(1570, 5, 0, 13, 10, 17.69f, 0.0f, -3.32f, 90.0f);
        this.npc6 = new NPC_NORMAL(1582, 6, 0, 16, 6, -5.22f, 0.2f, 0.21f, 250.0f);
        this.npc7 = new NPC_NORMAL(1552, 7, 0, 16, 5, 11.33f, 0.0f, -0.1f, 270.0f);
        this.npc8 = new NPC_NORMAL(1608, 8, 0, 12, 13, -15.86f, 0.0f, -0.98f, 180.0f);
        this.npc10 = new NPC_NORMAL(1601, 10, 0, 15, 7, 8.74f, 0.0f, -3.27f, 30.0f);
        this.npc11 = new NPC_NORMAL(1601, 11, 0, 15, 7, -7.53f, 0.0f, 1.91f, 90.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 7);
        this.npc1.setInvalidID(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.setMotion(1, 3);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 16);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 2);
        this.npc6.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 8);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 10);
        this.npc10.setInvalidID(1);
        this.npc10.setVisible(false);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 9);
        this.npc11.setInvalidID(1);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
    }

    void npcset_2() {
        this.npc1 = new NPC_NORMAL(1600, 1, 0, 15, 14, -15.89f, 0.0f, -0.47f, 120.0f);
        this.npc2 = new NPC_NORMAL(1601, 2, 0, 9, 7, -15.15f, 0.0f, 1.83f, 0.0f);
        this.npc3 = new NPC_NORMAL(1601, 3, 0, 15, 7, 8.04f, 0.0f, -3.97f, 30.0f);
        this.npc4 = new NPC_NORMAL(1588, 4, 0, 77, 13, 10.66f, 0.0f, 3.45f, 180.0f);
        this.npc5 = new NPC_NORMAL(1570, 5, 0, 13, 10, 17.69f, 0.0f, -3.32f, 90.0f);
        this.npc6 = new NPC_NORMAL(1582, 6, 0, 16, 6, 14.71f, 0.0f, -1.47f, 270.0f);
        this.npc7 = new NPC_NORMAL(1552, 7, 0, 16, 5, 11.33f, 0.0f, -0.1f, 270.0f);
        this.npc8 = new NPC_NORMAL(1608, 8, 0, 12, 13, 13.86f, 0.0f, -6.368f, 210.0f);
        this.npc10 = new NPC_NORMAL(1601, 10, 0, 15, 7, 8.74f, 0.0f, -3.27f, 30.0f);
        this.npc11 = new NPC_NORMAL(1601, 11, 0, 15, 7, -7.53f, 0.0f, 1.91f, 90.0f);
        this.npc20 = new NPC_NORMAL2(1609, 20, 0, 15, 17, -14.62f, 0.59f, -0.3f, 0.0f);
        this.npc21 = new NPC_NORMAL2(1543, 21, 0, 15, 17, -12.47f, 0.59f, -0.3f, 0.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 9);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.setMotion(1, 3);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 16);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 2);
        this.npc6.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 8);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 10);
        this.npc10.setInvalidID(1);
        this.npc10.setVisible(false);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 9);
        this.npc11.setInvalidID(1);
        this.npc20.disableDTKFlag(3);
        this.npc20.disableDTKFlag(8);
        this.npc20.setInvalidID(1);
        this.npc20.setMotion(0, 1);
        this.npc20.disableDTKFlag(65536);
        this.npc21.disableDTKFlag(3);
        this.npc21.disableDTKFlag(8);
        this.npc21.setInvalidID(1);
        this.npc21.setMotion(0, 0);
        this.npc21.disableDTKFlag(65536);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
    }

    void npcset_3() {
        this.npc1 = new NPC_NORMAL(1600, 1, 0, 15, 5, -6.57f, 0.0f, -1.25f, 45.0f);
        this.npc2 = new NPC_NORMAL(1601, 2, 0, 9, 7, -4.4f, 0.0f, 3.42f, 0.0f);
        this.npc3 = new NPC_NORMAL(1601, 3, 0, 15, 7, 8.04f, 0.0f, -3.97f, 30.0f);
        this.npc4 = new NPC_NORMAL(1588, 4, 0, 77, 13, 10.66f, 0.0f, 3.45f, 180.0f);
        this.npc5 = new NPC_NORMAL(1570, 5, 0, 13, 10, 17.69f, 0.0f, -3.32f, 90.0f);
        this.npc6 = new NPC_NORMAL(1582, 6, 0, 16, 6, -5.22f, 0.2f, 0.21f, 250.0f);
        this.npc7 = new NPC_NORMAL(1552, 7, 0, 16, 5, 11.33f, 0.0f, -0.1f, 270.0f);
        this.npc8 = new NPC_NORMAL(1608, 8, 0, 12, 13, 13.86f, 0.0f, -6.368f, 210.0f);
        this.npc10 = new NPC_NORMAL(1601, 10, 0, 15, 7, 8.74f, 0.0f, -3.27f, 30.0f);
        this.npc11 = new NPC_NORMAL(1601, 11, 0, 15, 7, -7.53f, 0.0f, 1.91f, 90.0f);
        this.npc25 = new NPC_NORMAL(1, 25, 0, 15, 19, 13.3f, 0.0f, -6.34f, 110.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 7);
        this.npc1.setInvalidID(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.setMotion(1, 3);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 16);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 2);
        this.npc6.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 8);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.enableDTKFlag(393216);
        this.npc8.setMotion(0, 10);
        this.npc10.setInvalidID(1);
        this.npc10.setVisible(false);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 9);
        this.npc11.setInvalidID(1);
        this.npc25.setInvalidID(1);
        this.npc25.disableDTKFlag(3);
        this.npc25.setMotion(0, 9);
        this.npc25.disableDTKFlag(131072);
        this.npc25.disableDTKFlag(65536);
        this.npc25.enableDTKFlag(262144);
        this.npc25.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
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

        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(0, 0);
        }
    }
}

