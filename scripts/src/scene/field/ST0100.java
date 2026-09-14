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
import xeno.map.MC_VOK10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0100
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK10_PRJ {
    Player player;
    Camera cam1;
    Menu menu;
    Window win;
    Camera camEV;
    int entrance;
    Enepc hasigo;
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
    Enepc npc20;
    Enepc npc21;
    Enepc npc23;
    Enepc npc24;
    Enepc npc25;
    Enepc npc26;
    Enepc npc30;
    Unit[] unit;
    Unit takara;
    int count = 0;
    int selected = 0;
    int mapno;
    int enemy;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono itembox1;
    Uwamono itembox2;
    Uwamono itembox;
    Uwamono obj01;
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
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int selected0 = 0;
    int selected1 = 0;
    int pass1 = 0;
    int runaway = 0;
    int BUTTON_F = 0;
    int S014A;
    int S014B;
    int S015B;
    Unit monitor1;
    Unit monitor2;
    Effect fade;
    Effect EF01;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] msg11000001 = new String[]{"/[label(Sgt. Swaine)]", "Hey, missy. Anything you need today? I'll hook you up real cheap.", "/[waitkey(64)]/[clear()]"};
    String[] msg11000002 = new String[]{"/[label(Shion)]", "Oh, Sergeant Swaine.", "/[waitkey(1)]/[clear()]", "Come to think of it, how do you get ahold of all those things all the time?", "/[waitkey(64)]/[clear()]"};
    String[] msg11000003 = new String[]{"/[label(Sgt. Swaine)]", "As the old proverb goes, \"The wolf knows what the ill beast thinks.\" When you're a lifer in the military, you learn a lot of things, like how to skim stuff.", "/[waitkey(1)]/[clear()]", "Even on a warship, not every inch is controlled. There are hiding places that only I know of.", "/[waitkey(1)]/[clear()]", "So is there anything I can get you?", "/[waitkey(64)]/[close()]"};
    String[] msg11000004 = new String[]{"/[label(Sgt. Swaine)]", "Hmm...okay. Since you're such a good customer, this one's on me. It's yours for the taking.", "/[waitkey(64)]/[clear()]"};
    String[] msg11000005 = new String[]{"/[label(Shion)]", "Oh, thank you...", "/[waitkey(64)]/[close()]"};
    String[] msg11000006 = new String[]{"/[label(Sgt. Swaine)]", "You know how the nanomachine core is still a black box, right? It's a secret, but I heard it contains a bug that'll create useless doorways complete with locks when used for building and construction. Since they aren't dangerous, they've left them alone.", "/[waitkey(1)]/[clear()]", "I just happened to come across the secret document describing the bug. And this is a collection of all the hidden passages from all over the world based on that document. This is the secret to my personal hidden warehouse. There're still 17 or 18 that I haven't opened yet. I search for them every time we disembark, but\nit's tough.", "/[waitkey(1)]/[clear()]", "Well? Doesn't it seem fun? Almost like a treasure hunt, right?", "/[waitkey(1)]/[clear()]", "Oh, the ones on this ship are mine, so don't open them.", "/[waitkey(64)]/[clear()]"};
    String[] msg11000007 = new String[]{"/[label(Shion)]", "T-t...thank you...very much...", "/[waitkey(64)]/[close()]"};
    String[] msg11000008 = new String[]{"/[label(Sgt. Swaine)]", "Sure thing! See ya later!", "/[waitkey(64)]/[close()]"};
    String[] msg45316683 = new String[]{"/[label(Sgt. Swaine)]", "Thanks for your business. Come again.", "/[waitkey(64)]/[close()]"};
    String[] msg45472879 = new String[]{"/[label(Sgt. Swaine)]", "What? You're not buying? Well, if you ever need something, don't hesitate to ask.", "/[waitkey(64)]/[close()]"};
    String[] msg45475569 = new String[]{"/[label(Sgt. Swaine)]", "Hey, lady, feel like buying something now?", "/[waitkey(64)]/[close()]"};
    String[] msg11000009 = new String[]{"/[label(Sgt. Swaine)]", "If you think it's interesting, take your time searching. Even if you find the entrance, the decoder's sometimes in a totally different place.", "/[waitkey(1)]/[clear()]", "You know, I've got a bad feeling about this particular mission. Maybe your intuition improves over the years. If something happened to me, would you find all the remaining doors for me and open them? I just hate to leave things unfinshed.", "/[waitkey(64)]/[close()]"};
    String[] msg48370F8C = new String[]{"/[label()]", "Hey, you! Get away from there right now! Don't touch that console!", "/[waitkey(64)]/[close()]"};
    String[] msg58370F8C = new String[]{"/[label()]", "Who are you? State your name and rank.", "/[waitkey(1)]/[clear()]", "Oh, you're Vector personnel. You see, this is an emergency hatch, so there's no airlock. Everything will get sucked out if you open this in space, so please\n", "stay away from it, just to be safe.", "/[waitkey(64)]/[close()]"};
    String[] msg48376F39 = new String[]{"/[label()]", "You do understand, right? It's extremely hazardous, so you must never touch that switch.", "/[waitkey(64)]/[close()]"};
    String[] msg471F37FF = new String[]{"/[label()]", "Man, the nerve! Those strangers took over the hangar and filled it with their stuff, so there's no room for our equipment.", "/[waitkey(64)]/[close()]"};
    String[] msg471F97AC = new String[]{"/[label()]", "Oh, please don't touch that electromagnetic net.", "/[waitkey(1)]/[clear()]", "It's a barricade against Gnosis, but it'll electrocute people, too.", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"/[label()]", "There are no lines for the 2nd time yet.", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "Since we were investigating the disappearance of a planet, I thought we'd be out here for at least half a year. But now that we've picked up one little piece of flotsam, that's it? What a letdown.", "/[waitkey(64)]/[close()]"};
    String[] msg00000002 = new String[]{"/[label()]", "And to make things worse, when I called my wife, she had the nerve to tell me that she'd be out vacationing and won't be home when we return to port!", "/[waitkey(64)]/[close()]"};
    String[] msg00000003 = new String[]{"/[label()]", "It's just that...well I'm glad that the change in plans allows us to go home early, but if this is what being married is like in its third year, what's there to look forward to?", "/[waitkey(64)]/[close()]"};
    String[] msg00000004 = new String[]{"/[label()]", "Those things...they're called Gnosis, right? Do you think they're real?", "/[waitkey(1)]/[clear()]", "They're telling the public that they don't exist, much like the Loch Ness Monster or the Abominable Snowman.", "/[waitkey(64)]/[close()]"};
    String[] msg00000005 = new String[]{"/[label()]", "Or maybe it's just a ploy by Vector and the military brass to get funding for a new project.", "/[waitkey(64)]/[close()]"};
    String[] msg00000006 = new String[]{"/[label()]", "O-oh! You're a member of Vector?!", "/[waitkey(1)]/[clear()]", "That's not very nice. You should have said so. Hey, could you pretend you didn't hear what I said earlier?", "/[waitkey(64)]/[close()]"};
    String[] msg00000007 = new String[]{"/[label()]", "Hey, you didn't have to go that far. I can't believe you vandalized A.G.W.S.", "/[waitkey(64)]/[clear()]"};
    String[] msg00000008 = new String[]{"/[label()]", "I can't believe you're getting pissed over that!", "/[waitkey(64)]/[clear()]"};
    String[] msg00000009 = new String[]{"/[label()]", "But you didn't to have to write \"Only Designed to Go Straight\" so large.", "/[waitkey(64)]/[clear()]"};
    String[] msg00000010 = new String[]{"/[label()]", "What? It's the truth! The phrase fits a quack pilot like you perfectly!", "/[waitkey(64)]/[close()]"};
    String[] msg00000011 = new String[]{"/[label()]", "Found you! You bastard! Stay where you are!", "/[waitkey(64)]/[close()]"};
    String[] msg00000012 = new String[]{"/[label()]", "Oh, crap!", "/[wait(10)]/[close()]"};
    String[] msg00000013 = new String[]{"/[label()]", "Ha-ha!\n", "You can't catch me.\n", "/[wait(10)]/[close()]"};
    String[] msg00000014 = new String[]{"/[label(Shion)]", "Why did he bother chasing her? There's no way he'll catch her at that rate. He really is only designed for going straight.", "/[waitkey(64)]/[close()]"};
    String[] msg00000015 = new String[]{"/[label()]", "Sheesh, those two are always at it like that. Just watching them makes me tired.", "/[waitkey(64)]/[close()]"};
    String[] msg00000016 = new String[]{"/[label()]", "Really, why are so many pilots such simpletons?", "/[waitkey(64)]/[close()]"};
    String[] msg00000017 = new String[]{"/[label()]", "Say, did you hear? There's a rumor about Sergeant Swaine, and how he's got a fetish about doors.", "/[waitkey(64)]/[close()]"};
    String[] msg00000018 = new String[]{"/[label()]", "The other day, he peeked in my room, too.", "/[waitkey(1)]/[clear()]", "Then he suddenly shouted, \"Wrong!\" and got mad.", "/[waitkey(1)]/[clear()]", "Wrong? I mean, what's wrong with me? That was\nso rude!", "/[waitkey(64)]/[close()]"};
    String[] msg00000020 = new String[]{"/[label()]", "Man, how long is she going to make me wait? She said to wait in front of the information board, but she hasn't bothered to show up at all!", "/[waitkey(1)]/[clear()]", "Damn it, my break's almost over.", "/[waitkey(64)]/[close()]"};
    String[] msg00000021 = new String[]{"/[label()]", "Could it be that she doesn't like me?", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Corridor 4'", "/[waitkey(64)]/[close()]"};
    String[] msgtest1 = new String[]{"/[label()]", "Aw, no need to worry, the rules are simple. We're just gonna play tag.", "/[waitkey(64)]/[clear()]"};
    String[] msgtest2 = new String[]{"/[label()]", "We're gonna try to catch ya, all right?", "/[waitkey(64)]/[clear()]"};
    String[] msgtest3 = new String[]{"/[label()]", "All you gotta do is get that item behind us without getting caught, and ya win!", "/[waitkey(64)]/[clear()]"};
    String[] msgtest4 = new String[]{"/[label()]", "Don't try to charge straight through us, either. Ya need to watch your opponent's moves and use the features of the corridor to your advantage.", "/[waitkey(64)]/[clear()]"};
    String[] msgtest5 = new String[]{"/[label()]", "Ya ready? Here we go!", "/[waitkey(64)]/[close()]"};
    String[] msgtest10 = new String[]{"/[label()]", "I'll explain again, so listen closely.", "/[waitkey(64)]/[clear()]"};
    String[] msggoal = new String[]{"/[label()]", "Not bad. Not bad at all! With those moves, you'll be fine even when things get hot.", "/[waitkey(64)]/[close()]"};
    String[] msgzannen = new String[]{"/[label()]", "Ya gotta do better than that.", "/[waitkey(64)]/[close()]"};
    String[] msgzannen2 = new String[]{"/[label()]", "That was disappointing.", "/[waitkey(1)]/[clear()]", "At this rate, ya won't be able to outrun the Gnosis if they attack.", "/[waitkey(64)]/[close()]"};
    String[] msgtukiae1 = new String[]{"/[label()]", "Yo, Ms. Vector!", "/[waitkey(64)]/[close()]"};
    String[] msgtukiae2 = new String[]{"/[label()]", "Come join us for a friendly game!", "/[waitkey(64)]/[clear()]"};
    String[] msgtukiae3 = new String[]{"/[label(Shion)]", "Huh? Well, okay...", "/[waitkey(64)]/[close()]"};
    String[] msgtukiae4 = new String[]{"/[label()]", "What is it, Ms. Vector? Wanna play with us again?", "/[waitkey(64)]/[close()]"};
    String[] msgtukiae5 = new String[]{"/[label()]", "Man, you're no fun.", "/[waitkey(64)]/[close()]"};
    String[] msgtukiae6 = new String[]{"/[label()]", "What is it, Ms. Vector? Want to play with us again? We don't have any more prizes, though. Is that okay with you?", "/[waitkey(64)]/[close()]"};
    String[] msghehehe = new String[]{"/[label()]", "Heh heh, I never lose.", "/[waitkey(64)]/[close()]"};
    String[] msghehehe2 = new String[]{"/[label()]", "Heh heh, no one can beat me.", "/[waitkey(1)]/[clear()]", "Well, except maybe you, Ms. Vector...", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 7 has been decoded.", "/[waitkey(64)]/[close()]"};
    String[] msgsuwaga1 = new String[]{"/[label(Sgt. Swaine)]", "Hey, Shion!", "/[waitkey(64)]/[close()]"};
    String[] msgsuwaga2 = new String[]{"/[label(Sgt. Swaine)]", "Over here! Right here!", "/[waitkey(64)]/[close()]"};
    String[] msgsuwaga3 = new String[]{"/[label(Shion)]", "Oh, it's Sergeant Swaine.", "/[waitkey(64)]/[close()]"};
    String[] msgsuwaga4 = new String[]{"/[label(Shion)]", "Hello, Sergeant Swaine. Is something the matter?", "/[waitkey(64)]/[close()]"};
    String[] msgsf1 = new String[]{"/[label(Sgt. Swaine)]", "I've got something for you. I'm going to present you with the results of my research!", "/[waitkey(64)]/[close()]"};
    String[] msgsf2 = new String[]{"/[label(Shion)]", "Research? You mean your hobby of \"Investigating all the doors on the ship?\"", "/[waitkey(64)]/[close()]"};
    String[] msgsf3 = new String[]{"/[label(Sgt. Swaine)]", "Yup, that's it. But I'm not just investigating the doors.", "/[waitkey(1)]/[clear()]", "Well? Wanna know what I'm researching?", "/[waitkey(64)]/[close()]"};
    String[] msgsf4 = new String[]{"/[label(Sgt. Swaine)]", "You know how the nanomachine core is still a black box, just like when it was first developed? It's a secret, but I hear it has a bug. If used for building and\nconstruction, it has a tendency to create useless corridors with locks. This poses no real harm, so it's been kept under wraps.", "/[waitkey(1)]/[clear()]", "I just happened to find the secret documents describing the bug. And this is a collection of all the hidden passages from all over the world based on that document. This is the secret to my personal hidden warehouse. There're still 17 or 18 that I haven't opened yet. I search for them every time we disembark, but\nit's tough.", "/[waitkey(1)]/[clear()]", "Well? Doesn't it seem fun? Almost like a treasure hunt, right?", "/[waitkey(1)]/[clear()]", "Oh, the ones on this ship are mine, so don't open them.", "/[waitkey(64)]/[close()]"};
    String[] msgsf5 = new String[]{"/[label(Shion)]", "T-t...thank you...very much...", "/[waitkey(64)]/[close()]"};
    String[] msgsf6 = new String[]{"/[label(Sgt. Swaine)]", "Really? Then, I'll give you this /[color(0x329bbe)]Segment File/[color(0x808080)] for now. If you press the ○ Button in the Items List, you'll be able to look at the notebook.", "/[waitkey(1)]/[clear()]", "If you want to hear more about my research, just let me know!", "/[waitkey(64)]/[close()]"};
    String[] msgsf7 = new String[]{"/[label(Sgt. Swaine)]", "Hey, Shion! What's up?", "/[waitkey(64)]/[close()]"};
    String[] msgsf8 = new String[]{"/[label(Sgt. Swaine)]", "What do you think of the /[color(0x329bbe)]Segment File/[color(0x808080)]? If you think it's interesting, search patiently. Even if you find the entrance, the key's sometimes in a totally different place.", "/[waitkey(1)]/[clear()]", "You know, I've got a bad feeling about this particular mission. Maybe your intuition improves over the years. If something happened to me, would you find all the remaining doors for me and open them? I just hate to leave things unfinshed.", "/[waitkey(64)]/[close()]"};
    String[] msgsf9 = new String[]{"/[label(Sgt. Swaine)]", "Yeah? If you want to hear more about my research just let me know!", "/[waitkey(64)]/[close()]"};
    String[] msgsf10 = new String[]{"/[label(Sgt. Swaine)]", "Or rather, you already found a door?!", "/[waitkey(1)]/[clear()]", "That's my Shion! You're sharp as a tack!", "/[waitkey(64)]/[close()]"};
    String[] msgTUKARE = new String[]{"/[label(Shion)]", "Phew, I'm getting a little tired. I think I should go rest in my room for a while.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!", "/[waitkey(64)]/[close()]"};

    ST0100() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.888f, 4.598f, -4.851f);
        this.camEV.setRotate(-17.668f, 1.879f, 0.0f);
        this.camEV.setFov(38.199f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.686f, 9.654f, -16.175f);
        this.camEV.setRotate(-67.445f, -360.394f, 0.0f);
        this.camEV.setFov(69.556f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.135f, 4.063f, 22.239f);
        this.camEV.setRotate(-0.155f, 0.77f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-14.088f, 3.319f, -10.358f);
        this.camEV.setRotate(-15.408f, 0.0f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-25.798f, 1.727f, -26.899f);
        this.camEV.setRotate(-6.532f, -131.077f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-14.116f, 5.439f, 0.298f);
        this.camEV.setRotate(-16.652f, -178.092f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-16.506f, 3.639f, 9.172f);
        this.camEV.setRotate(-8.93f, -26.119f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera08() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-21.013f, 1.751f, -18.154f);
        this.camEV.setRotate(-11.128f, -32.979f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera09() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-15.931f, 3.945f, 27.667f);
        this.camEV.setRotate(-12.836f, -61.238f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void EV_Camera10() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-30.594f, 2.999f, -26.671f);
        this.camEV.setRotate(-18.503f, 53.367f, 0.0f);
        this.camEV.setFov(32.43f);
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
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Stage.setVisible(61, false);
                this.monitor1.signal(1);
                this.monitor2.signal(1);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.win = Window.create();
                this.win.print(this.msgMAP, 0);
                ST0100.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(20);
                this.cam0.setMode(0);
                Stage.setVisible(61, true);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 2: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(3207, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(55);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3207, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3227, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3287, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                this.doorF.SetDoorType('\u0004');
                Runtime.setFlags(3287, 1, 1);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 3: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.npc4.moveEnepc(17, 270.0f, -0.1f, 10);
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg48370F8C, 0);
                ST0100.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 4: {
                if (Runtime.getFlags(7039, 1) == 1) {
                    return;
                }
                Runtime.setFlags(7039, 1, 1);
                Runtime.setPlayerControl(false);
                System.sleep(5);
                this.player.look_char(this.npc24);
                Runtime.enable(65536);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgtukiae1, 0);
                ST0100.waitPage(this.win, 64);
                System.sleep(10);
                this.cam0.setMode(-1);
                this.EV_Camera05();
                this.player.setTranslate(-21.9f, 0.0f, -24.48f);
                this.player.setRotate(0.0f, 45.0f, 0.0f);
                this.npc24.moveEnepc(17, 200.0f, -0.1f, 0);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgtukiae2, 0);
                ST0100.waitPage(this.win, 64);
                this.player.mtn(10, 1, 1.0f, true);
                this.win.print(this.msgtukiae3, 0);
                ST0100.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(27);
                this.npc24.setVisible(false);
                this.npc25.setVisible(false);
                this.npc24.setTranslate(-16.18f, 2.0f, 7.0f);
                this.npc25.setTranslate(-15.64f, 2.0f, 8.2f);
                this.npc24.moveEnepc(17, 40.0f, 0.1f, 0);
                this.player.setTranslate(-13.96f, 2.0f, 6.1f);
                this.player.setRotate(0.0f, 0.0f, 0.0f);
                this.npc20.setVisible(true);
                this.npc21.setVisible(true);
                this.npc26.setVisible(true);
                this.takara.setVisible(true);
                this.npc20.kickEnepc(4, 1);
                this.npc21.kickEnepc(4, 1);
                this.npc20.kickEnepc(1, 9);
                this.npc21.kickEnepc(1, 10);
                this.player.look_char(this.npc20);
                this.cam0.setMode(0);
                System.sleep(3);
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgtest1, 0);
                ST0100.waitPage(this.win, 64);
                this.EV_Camera06();
                this.win.print(this.msgtest2, 0);
                ST0100.waitPage(this.win, 64);
                this.npc20.moveEnepc(17, 40.0f, -0.1f, 20);
                this.win.print(this.msgtest3, 0);
                ST0100.waitPage(this.win, 64);
                this.cam0.setMode(0);
                this.npc20.moveEnepc(17, 180.0f, 0.1f, 20);
                this.win.print(this.msgtest4, 0);
                ST0100.waitPage(this.win, 64);
                this.win.print(this.msgtest5, 0);
                ST0100.waitPage(this.win, 64);
                System.sleep(5);
                this.npc20.kickEnepc(4, 0);
                this.npc21.kickEnepc(4, 0);
                this.npc20.enableDTKFlag(131072);
                this.npc21.enableDTKFlag(131072);
                this.npc23.enableDTKFlag(131072);
                this.npc20.kickEnepc(7, 82);
                this.npc21.kickEnepc(7, 82);
                this.npc24.disableDTKFlag(65536);
                this.npc25.disableDTKFlag(65536);
                this.player.look_default();
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                break;
            }
            case 5: {
                if (Runtime.getFlags(23, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7066, 4) == 1) {
                    return;
                }
                if (Runtime.getFlags(22, 1) != 1 || Runtime.getFlags(7057, 2) != 2) break;
                Runtime.mailArriveSet(5);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7066, 4, 1);
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
                        break;
                    }
                    default: {
                        Runtime.setPlayerControl(true);
                        break;
                    }
                }
            }
            case 6: {
                if (Runtime.getFlags(7034, 1) == 1) {
                    return;
                }
                Runtime.setPlayerControl(false);
                Runtime.setFlags(7034, 1, 1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsuwaga1, 0);
                ST0100.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.EV_Camera09();
                Runtime.enable(65536);
                this.player.look_char(this.npc1);
                this.player.setTranslate(-13.96f, 2.0f, 26.01f);
                this.player.setRotate(0.0f, 110.0f, 0.0f);
                this.npc1.moveEnepc(17, 300.0f, -0.1f, 0);
                this.npc1.kickEnepc(9, 100);
                this.npc1.kickEnepc(0, 11);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsuwaga2, 0);
                ST0100.waitPage(this.win, 64);
                this.npc1.kickEnepc(0, 11);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsuwaga3, 0);
                ST0100.waitPage(this.win, 64);
                this.player.mtn(4, 9, 1.0f, true);
                this.player.move(29, -10.18f, 24.21f, true);
                this.fade.call(0);
                System.sleep(29);
                System.sleep(1);
                this.player.setTranslate(-3.56f, 2.0f, 20.68f);
                this.player.setRotate(0.0f, 220.0f, 0.0f);
                this.npc1.moveEnepc(17, 30.0f, -0.1f, 0);
                this.cam0.setMode(0);
                Runtime.disable(65536);
                System.sleep(30);
                this.npc1.kickEnepc(0, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsuwaga4, 0);
                ST0100.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf1, 0);
                ST0100.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf2, 0);
                ST0100.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf3, 0);
                ST0100.waitPage(this.win, 64);
                this.menu = Menu.create();
                this.menu.addItem("Yes, please tell me\nUm, not right now");
                Runtime.setPlayerControl(false);
                System.waitFor(this.menu);
                this.selected1 = this.menu.getSelected();
                switch (this.selected1) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgsf4, 0);
                        ST0100.waitPage(this.win, 64);
                        if (Runtime.getFlags(3210, 1) == 0) {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.msgsf5, 0);
                            ST0100.waitPage(this.win, 64);
                            Runtime.setFlags(7035, 1, 1);
                            Sound.effectPlay(6);
                            Runtime.addItemWin(10, 11);
                            this.player.look_default();
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgsf10, 0);
                        ST0100.waitPage(this.win, 64);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgsf5, 0);
                        ST0100.waitPage(this.win, 64);
                        Runtime.setFlags(7035, 1, 1);
                        Sound.effectPlay(6);
                        Runtime.addItemWin(10, 11);
                        this.player.look_default();
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf6, 0);
                ST0100.waitPage(this.win, 64);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 11);
                this.player.look_default();
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.S014A = Runtime.getFlags(21, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S015B = Runtime.getFlags(23, 1);
        if (Runtime.getFlags(7035, 1) == 0) {
            this.Talk_npc1_1();
        } else {
            this.Talk_npc1_2();
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        window.print(this.msg00000016, 0);
        ST0100.waitPage(window, 64);
    }

    void Talk_npc1_1() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgsf7, 0);
        ST0100.waitPage(this.win, 64);
        this.menu = Menu.create();
        this.menu.addItem("I guess I'll ask about research\nJust thought I'd say hello");
        System.waitFor(this.menu);
        this.selected0 = this.menu.getSelected();
        switch (this.selected0) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf4, 0);
                ST0100.waitPage(this.win, 64);
                if (Runtime.getFlags(3210, 1) == 0) {
                    Runtime.setFlags(7035, 1, 1);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgsf10, 0);
                ST0100.waitPage(this.win, 64);
                Runtime.setFlags(7035, 1, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgsf9, 0);
        ST0100.waitPage(this.win, 64);
    }

    void Talk_npc1_2() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgsf8, 0);
        ST0100.waitPage(this.win, 64);
    }

    void Talk_npc1_3() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNO2, 0);
        ST0100.waitPage(this.win, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    public void Talk_npc20(Enepc enepc, Window window) {
        this.npc20.enableDTKFlag(131072);
        this.npc21.enableDTKFlag(131072);
        this.npc23.enableDTKFlag(131072);
        window.print(this.msgtest1, 0);
        ST0100.waitPage(window, 64);
        this.npc24.setVisible(false);
        this.takara.setVisible(true);
        System.sleep(10);
        this.npc20.kickEnepc(7, 82);
        this.npc21.kickEnepc(7, 82);
    }

    public void Talk_npc21(Enepc enepc) {
    }

    public void Talk_npc23(Enepc enepc) {
    }

    public void Talk_npc24(Enepc enepc) {
        if (Runtime.getFlags(7039, 1) == 0) {
            this.fade.call(0);
            System.sleep(30);
            Runtime.setFlags(7039, 1, 1);
            this.cam0.setMode(-1);
            this.EV_Camera08();
            this.player.setTranslate(-19.71f, 0.0f, -19.6f);
            this.player.setRotate(0.0f, 270.0f, 0.0f);
            System.sleep(10);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtukiae1, 0);
            ST0100.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtukiae2, 0);
            ST0100.waitPage(this.win, 64);
            this.win.print(this.msgtukiae3, 0);
            ST0100.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(27);
            this.npc24.setVisible(false);
            this.npc25.setVisible(false);
            this.npc24.setTranslate(-16.18f, 2.0f, 7.0f);
            this.npc25.setTranslate(-15.64f, 2.0f, 8.2f);
            this.npc24.moveEnepc(17, 40.0f, 0.1f, 0);
            Runtime.enable(65536);
            this.player.setTranslate(-13.96f, 2.0f, 6.1f);
            this.player.setRotate(0.0f, 0.0f, 0.0f);
            this.npc20.setVisible(true);
            this.npc21.setVisible(true);
            this.npc26.setVisible(true);
            this.takara.setVisible(true);
            this.npc20.kickEnepc(4, 1);
            this.npc21.kickEnepc(4, 1);
            this.npc20.kickEnepc(1, 9);
            this.npc21.kickEnepc(1, 10);
            this.player.look_char(this.npc20);
            this.cam0.setMode(0);
            System.sleep(3);
            System.sleep(10);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtest1, 0);
            ST0100.waitPage(this.win, 64);
            this.EV_Camera06();
            this.win.print(this.msgtest2, 0);
            ST0100.waitPage(this.win, 64);
            this.npc20.moveEnepc(17, 40.0f, -0.1f, 20);
            this.win.print(this.msgtest3, 0);
            ST0100.waitPage(this.win, 64);
            this.cam0.setMode(0);
            this.npc20.moveEnepc(17, 180.0f, 0.1f, 20);
            this.win.print(this.msgtest4, 0);
            ST0100.waitPage(this.win, 64);
            this.win.print(this.msgtest5, 0);
            ST0100.waitPage(this.win, 64);
            System.sleep(5);
            this.npc20.kickEnepc(4, 0);
            this.npc21.kickEnepc(4, 0);
            this.npc20.enableDTKFlag(131072);
            this.npc21.enableDTKFlag(131072);
            this.npc23.enableDTKFlag(131072);
            this.npc20.kickEnepc(7, 82);
            this.npc21.kickEnepc(7, 82);
            this.npc24.disableDTKFlag(65536);
            this.npc25.disableDTKFlag(65536);
            this.player.look_default();
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            return;
        }
        this.fade.call(0);
        System.sleep(30);
        Stage.setVisible(4, true);
        Stage.setVisible(0, true);
        Stage.setVisible(1, true);
        Stage.setVisible(2, true);
        Stage.setVisible(3, true);
        Stage.setVisible(5, true);
        this.cam0.setMode(-1);
        this.EV_Camera07();
        this.player.setTranslate(-15.46f, 0.0f, 7.28f);
        this.player.setRotate(0.0f, 270.0f, 0.0f);
        System.sleep(10);
        if (Runtime.getFlags(7046, 2) == 3) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtukiae6, 0);
            ST0100.waitPage(this.win, 64);
        } else {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtukiae4, 0);
            ST0100.waitPage(this.win, 64);
        }
        this.menu = Menu.create();
        this.menu.addItem("Yes, let's play\nSorry, I haven't the time");
        System.waitFor(this.menu);
        this.selected1 = this.menu.getSelected();
        switch (this.selected1) {
            case 0: {
                this.fade.call(0);
                System.sleep(27);
                this.npc24.setVisible(false);
                this.npc25.setVisible(false);
                Stage.setVisible(4, false);
                Stage.setVisible(0, false);
                Stage.setVisible(1, false);
                Stage.setVisible(2, false);
                Stage.setVisible(3, false);
                Stage.setVisible(5, false);
                this.npc24.setTranslate(-16.18f, 2.0f, 7.0f);
                this.npc25.setTranslate(-15.64f, 2.0f, 8.2f);
                this.npc24.moveEnepc(17, 40.0f, 0.1f, 0);
                Runtime.enable(65536);
                this.player.setTranslate(-13.96f, 2.0f, 6.1f);
                this.player.setRotate(0.0f, 0.0f, 0.0f);
                this.npc20.setVisible(true);
                this.npc21.setVisible(true);
                this.npc26.setVisible(true);
                this.takara.setVisible(true);
                this.npc20.kickEnepc(4, 1);
                this.npc21.kickEnepc(4, 1);
                this.npc20.kickEnepc(1, 9);
                this.npc21.kickEnepc(1, 10);
                this.player.look_char(this.npc20);
                this.cam0.setMode(0);
                System.sleep(3);
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgtest10, 0);
                ST0100.waitPage(this.win, 64);
                this.EV_Camera06();
                this.win.print(this.msgtest2, 0);
                ST0100.waitPage(this.win, 64);
                this.npc20.moveEnepc(17, 40.0f, -0.1f, 20);
                this.win.print(this.msgtest3, 0);
                ST0100.waitPage(this.win, 64);
                this.cam0.setMode(0);
                this.npc20.moveEnepc(17, 180.0f, 0.1f, 20);
                this.win.print(this.msgtest4, 0);
                ST0100.waitPage(this.win, 64);
                this.win.print(this.msgtest5, 0);
                ST0100.waitPage(this.win, 64);
                System.sleep(5);
                this.npc20.kickEnepc(4, 0);
                this.npc21.kickEnepc(4, 0);
                this.npc20.enableDTKFlag(131072);
                this.npc21.enableDTKFlag(131072);
                this.npc23.enableDTKFlag(131072);
                this.npc20.kickEnepc(7, 82);
                this.npc21.kickEnepc(7, 82);
                this.npc24.disableDTKFlag(65536);
                this.npc25.disableDTKFlag(65536);
                this.player.look_default();
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgtukiae5, 0);
        ST0100.waitPage(this.win, 64);
        this.fade.call(0);
        System.sleep(20);
        System.sleep(10);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        this.cam0.setMode(0);
        Runtime.disable(65536);
    }

    public void Talk_npc25(Enepc enepc, Window window) {
        if (Runtime.getFlags(7040, 1) == 0) {
            window.print(this.msghehehe, 0);
            ST0100.waitPage(window, 64);
            return;
        }
        window.print(this.msghehehe2, 0);
        ST0100.waitPage(window, 64);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg471F37FF, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg471F97AC, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                this.npc4.kickEnepc(1, 3);
                window.print(this.msg58370F8C, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        this.npc4.kickEnepc(1, 3);
        window.print(this.msg48376F39, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0100.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00000002, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000003, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg00000017, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000018, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc) {
        this.Talk_npc7_1();
    }

    void Talk_npc7_1() {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg00000004, 0);
                ST0100.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00000005, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000006, 0);
        ST0100.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg00000020, 0);
                ST0100.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000021, 0);
        ST0100.waitPage(window, 64);
    }

    public void Touch_npc20(Enepc enepc) {
        this.npc20.disableDTKFlag(131072);
        this.npc21.disableDTKFlag(131072);
        this.npc23.disableDTKFlag(131072);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgzannen, 0);
        ST0100.waitPage(this.win, 64);
        Runtime.enable(65536);
        System.sleep(10);
        this.fade.call(0);
        System.sleep(25);
        this.takara.setVisible(false);
        this.npc24.setVisible(true);
        this.npc25.setVisible(true);
        Stage.setVisible(4, true);
        Stage.setVisible(0, true);
        Stage.setVisible(1, true);
        Stage.setVisible(2, true);
        Stage.setVisible(3, true);
        Stage.setVisible(5, true);
        this.npc20.setTranslate(-15.73f, 2.0f, 10.14f);
        this.npc21.setTranslate(-12.14f, 2.0f, 10.14f);
        this.npc20.moveEnepc(17, 180.0f, 0.0f, 0);
        this.npc21.moveEnepc(17, 180.0f, 0.0f, 0);
        this.npc20.kickEnepc(7, 14);
        this.npc21.kickEnepc(7, 14);
        this.npc20.setVisible(false);
        this.npc21.setVisible(false);
        this.npc26.setVisible(false);
        this.player.look_char(this.npc24);
        this.player.setTranslate(-15.46f, 2.0f, 7.28f);
        this.player.setRotate(0.0f, 270.0f, 0.0f);
        this.cam0.setMode(-1);
        this.EV_Camera07();
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgzannen2, 0);
        ST0100.waitPage(this.win, 64);
        System.sleep(20);
        this.fade.call(0);
        System.sleep(25);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        this.cam0.setMode(0);
        System.sleep(5);
        this.npc24.enableDTKFlag(65536);
        this.npc25.enableDTKFlag(65536);
        this.player.look_default();
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    public void Touch_npc23(Enepc enepc) {
        if (Runtime.getFlags(7046, 2) == 3) {
            Runtime.setFlags(7040, 1, 1);
            this.npc20.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(131072);
            this.npc23.disableDTKFlag(131072);
            this.fade.call(0);
            System.sleep(25);
            this.takara.setVisible(false);
            this.npc24.setVisible(true);
            this.npc25.setVisible(true);
            Stage.setVisible(4, true);
            Stage.setVisible(0, true);
            Stage.setVisible(1, true);
            Stage.setVisible(2, true);
            Stage.setVisible(3, true);
            Stage.setVisible(5, true);
            this.npc20.setTranslate(-15.73f, 2.0f, 10.14f);
            this.npc21.setTranslate(-12.14f, 2.0f, 10.14f);
            this.npc20.moveEnepc(17, 180.0f, 0.0f, 0);
            this.npc21.moveEnepc(17, 180.0f, 0.0f, 0);
            this.npc20.kickEnepc(7, 14);
            this.npc21.kickEnepc(7, 14);
            this.npc20.setVisible(false);
            this.npc21.setVisible(false);
            this.npc26.setVisible(false);
            Runtime.enable(65536);
            this.player.setTranslate(-15.46f, 2.0f, 7.28f);
            this.player.setRotate(0.0f, 270.0f, 0.0f);
            this.player.look_char(this.npc24);
            this.cam0.setMode(-1);
            this.EV_Camera07();
            System.sleep(10);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msggoal, 0);
            ST0100.waitPage(this.win, 64);
            System.sleep(20);
            this.fade.call(0);
            System.sleep(25);
            Stage.setVisible(4, false);
            Stage.setVisible(0, false);
            Stage.setVisible(1, false);
            Stage.setVisible(2, false);
            Stage.setVisible(3, false);
            Stage.setVisible(5, false);
            this.cam0.setMode(0);
            System.sleep(5);
            System.sleep(20);
            this.npc24.enableDTKFlag(65536);
            this.npc25.enableDTKFlag(65536);
            this.player.look_default();
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            return;
        }
        Runtime.setFlags(7040, 1, 1);
        this.npc20.disableDTKFlag(131072);
        this.npc21.disableDTKFlag(131072);
        this.npc23.disableDTKFlag(131072);
        this.fade.call(0);
        System.sleep(25);
        this.takara.setVisible(false);
        this.npc24.setVisible(true);
        this.npc25.setVisible(true);
        Stage.setVisible(4, true);
        Stage.setVisible(0, true);
        Stage.setVisible(1, true);
        Stage.setVisible(2, true);
        Stage.setVisible(3, true);
        Stage.setVisible(5, true);
        this.npc20.setTranslate(-15.73f, 2.0f, 10.14f);
        this.npc21.setTranslate(-12.14f, 2.0f, 10.14f);
        this.npc20.moveEnepc(17, 180.0f, 0.0f, 0);
        this.npc21.moveEnepc(17, 180.0f, 0.0f, 0);
        this.npc20.kickEnepc(7, 14);
        this.npc21.kickEnepc(7, 14);
        this.npc20.setVisible(false);
        this.npc21.setVisible(false);
        this.npc26.setVisible(false);
        Runtime.enable(65536);
        this.player.look_char(this.npc24);
        this.player.setTranslate(-15.46f, 2.0f, 7.28f);
        this.player.setRotate(0.0f, 270.0f, 0.0f);
        this.cam0.setMode(-1);
        this.EV_Camera07();
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msggoal, 0);
        ST0100.waitPage(this.win, 64);
        System.sleep(20);
        this.fade.call(0);
        System.sleep(25);
        Sound.effectPlay(6);
        Runtime.addItemWin(0, 1);
        ++this.count;
        Runtime.setFlags(7046, 2, this.count);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        this.cam0.setMode(0);
        System.sleep(5);
        System.sleep(20);
        this.npc24.enableDTKFlag(65536);
        this.npc25.enableDTKFlag(65536);
        this.player.look_default();
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(130, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(120, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(120, 3);
                break;
            }
            case 3: {
                Runtime.jumpCF(70, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 4.0f, 0.0f, 19.0f, 0.0f);
        this.teiten1.SetBgm(196628);
        this.teiten2 = new Uwamono(28690, -17.0f, 0.0f, 9.0f, 0.0f);
        this.teiten2.SetBgm(196629);
        this.teiten3 = new Uwamono(28690, -11.0f, 0.0f, 9.0f, 0.0f);
        this.teiten3.SetBgm(196629);
        this.teiten4 = new Uwamono(28690, -16.0f, 0.0f, -19.5f, 0.0f);
        this.teiten4.SetBgm(196630);
        this.teiten5 = new Uwamono(28690, -12.0f, 0.0f, -19.5f, 0.0f);
        this.teiten5.SetBgm(196630);
        this.teiten6 = new Uwamono(28690, -12.0f, 0.0f, -22.5f, 0.0f);
        this.teiten6.SetBgm(196630);
        this.teiten7 = new Uwamono(28690, -16.0f, 0.0f, -22.5f, 0.0f);
        this.teiten7.SetBgm(196630);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.hasigo = new Enepc();
        this.hasigo.init(16407, 9, -60.0f, 0.0f, 0.0f, 0.0f);
        this.hasigo.id = 1;
        this.hasigo.setGroup(5, 5, 5, 5);
        this.hasigo.setParams(0, 3, 1, 9);
        this.hasigo.setInvalidID(1);
        this.hasigo.kickEnepc(4, 2);
        this.count = Runtime.getFlags(7046, 2);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, -14.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 335.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, -36.8183f, 12.3756f, -13.09755f, 42.35f, -43.4036f, -13.3105f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        Stage.setVisible(171, false);
        this.itembox = new Uwamono(28677, -38.124f, 0.7f, -25.497f, 180.0f, 404);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 4.03f, 3.8f, 19.33f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18002, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Object();
        this.monitor2.init(24613, 4.03f, 3.8f, 19.33f, 0.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18007, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(31, 42, '\u0001');
        new Uwamono(32, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(35, 42, '\u0001');
        new Uwamono(36, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(39, 42, '\u0001');
        new Uwamono(40, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(43, 42, '\u0001');
        new Uwamono(44, 42, '\u0001', this.doorD);
        this.doorF = new Uwamono(26, 40, '\u0004');
        this.doorF.SetDoorType('\u0002');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        new Uwamono(23, 9);
        new Uwamono(24, 9);
        new Uwamono(25, 9);
        new Uwamono(76, 0);
        new Uwamono(77, 0);
        new Uwamono(78, 0);
        new Uwamono(83, 0);
        new Uwamono(171, 4);
        this.obj01 = new Uwamono(172, 6);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        new Uwamono(173, 1);
        new Uwamono(174, 1);
        this.hasigo.kickEnepc(19, 1, 0, 220, 1);
        this.hasigo.kickEnepc(19, 2, 0, 220, 1);
        this.EF01 = new Effect(1010, 0);
        this.EF01.disp(true);
        this.EF01.setLocation(6, 0);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S014A = Runtime.getFlags(21, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.npcset_1();
            if (Runtime.getFlags(7055, 1) == 1) {
                this.npc30 = new NPC_NORMAL(1, 30, 0, 13, 15, 100.0f, 100.0f, 100.0f, 0.0f);
                this.npc30.talkto("TalkNPC11");
                this.npc30.disableDTKFlag(131072);
                this.npc30.disableDTKFlag(8);
                this.npc30.setInvalidID(1);
                this.npc30.start(1, "TUKARETA");
            } else {
                this.npc6 = new NPC_NORMAL(524, 6, 0, 76, 7, -13.0f, 2.1f, 22.34f, 90.0f);
                this.npc6.disableDTKFlag(3);
                this.npc6.enableDTKFlag(4);
                this.npc6.setMotion(0, 7);
                this.npc6.talkto("Talk_npc6");
            }
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(522, 1, 0, 14, 5, -4.14f, 2.1f, 19.76f, 0.0f);
        this.npc1.enableDTKFlag(262144);
        this.npc1.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
        this.npc2 = new NPC_NORMAL(527, 2, 0, 14, 5, -34.061f, 0.0f, -24.357f, -60.0f);
        this.npc4 = new NPC_NORMAL(521, 4, 0, 13, 24, -3.32f, 2.1f, -31.37f, 0.0f);
        this.npc5 = new NPC_NORMAL(525, 5, 0, 5, 11, -16.27f, 0.0f, -27.14f, 60.0f);
        this.npc8 = new NPC_NORMAL(522, 8, 0, 5, 5, -18.28f, 2.1f, 16.4f, 90.0f);
        this.npc9 = new NPC_NORMAL(527, 9, 0, 14, 5, 5.09f, 2.1f, 20.11f, 0.0f);
        this.npc23 = new NPC_NORMAL(527, 23, 0, 14, 5, -13.966f, 2.1f, 14.09f, 0.0f);
        if (Runtime.getFlags(7039, 1) == 0) {
            this.npc24 = new NPC_NORMAL(525, 24, 0, 14, 5, -20.37f, 2.1f, -20.3f, 40.0f);
            this.npc25 = new NPC_NORMAL(525, 25, 0, 13, 11, -19.84f, 2.1f, -18.99f, 200.0f);
        } else {
            this.npc24 = new NPC_NORMAL(525, 24, 0, 14, 5, -16.18f, 2.1f, 7.0f, 40.0f);
            this.npc25 = new NPC_NORMAL(525, 25, 0, 13, 11, -15.64f, 2.1f, 8.2f, 200.0f);
        }
        this.npc2.disableDTKFlag(2);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 10);
        this.npc4.disableDTKFlag(1);
        this.npc4.enableDTKFlag(262144);
        this.npc4.setMotion(0, 1);
        this.npc9.setMotion(0, 17);
        this.npc23.enableDTKFlag(262144);
        this.npc23.setVisible(false);
        this.npc24.disableDTKFlag(3);
        this.npc24.enableDTKFlag(262144);
        this.npc24.setMotion(0, 9);
        this.npc25.disableDTKFlag(3);
        this.npc25.enableDTKFlag(4);
        this.npc25.enableDTKFlag(262144);
        this.npc25.setMotion(0, 10);
        this.npc2.talkto("Talk_npc2");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc23.talkto("Talk_npc23");
        this.npc24.talkto("Talk_npc24");
        this.npc25.talkto("Talk_npc25");
        this.npc23.touchto("Touch_npc23");
        this.npc20 = new Enepc();
        this.npc20.init(525, 11, -15.73f, 2.1f, 10.14f, 180.0f);
        this.npc20.talkto("Talk_npc21");
        this.npc20.id = 20;
        float[] fArray = new float[24];
        fArray[0] = -14.0f;
        fArray[1] = 2.0f;
        fArray[2] = 2.0f;
        fArray[3] = 1.0f;
        fArray[4] = -14.0f;
        fArray[6] = -16.0f;
        fArray[7] = 2.0f;
        fArray[8] = -19.0f;
        fArray[10] = -16.0f;
        fArray[11] = 3.0f;
        fArray[12] = -19.0f;
        fArray[14] = -26.0f;
        fArray[15] = 4.0f;
        fArray[16] = -9.0f;
        fArray[18] = -26.0f;
        fArray[19] = 5.0f;
        fArray[20] = -9.0f;
        fArray[22] = -16.0f;
        fArray[23] = -1.0f;
        float[] fArray2 = fArray;
        this.npc20.setParams(0, 14, 20, 11, fArray2);
        this.npc20.enableDTKFlag(262144);
        this.npc20.disableDTKFlag(65536);
        this.npc20.touchto("Touch_npc20");
        this.npc20.setShadow(3, 16);
        this.npc20.setVisible(false);
        this.npc21 = new Enepc();
        this.npc21.init(525, 11, -12.14f, 2.1f, 10.14f, 180.0f);
        this.npc21.talkto("Talk_npc21");
        this.npc21.id = 21;
        float[] fArray3 = new float[24];
        fArray3[0] = -14.0f;
        fArray3[1] = 2.0f;
        fArray3[2] = 4.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = -14.0f;
        fArray3[6] = -16.0f;
        fArray3[7] = 2.0f;
        fArray3[8] = -19.0f;
        fArray3[10] = -16.0f;
        fArray3[11] = 3.0f;
        fArray3[12] = -19.0f;
        fArray3[14] = -26.0f;
        fArray3[15] = 4.0f;
        fArray3[16] = -9.0f;
        fArray3[18] = -26.0f;
        fArray3[19] = 5.0f;
        fArray3[20] = -9.0f;
        fArray3[22] = -16.0f;
        fArray3[23] = -1.0f;
        float[] fArray4 = fArray3;
        this.npc21.setParams(0, 14, 21, 11, fArray4);
        this.npc21.enableDTKFlag(262144);
        this.npc21.disableDTKFlag(65536);
        this.npc21.touchto("Touch_npc20");
        this.npc21.setShadow(3, 16);
        this.npc21.setVisible(false);
        this.takara = new Unit();
        this.takara.init(24633, -13.96f, 2.0f, 14.09f, 0.0f);
        this.takara.setScale(1.5f, 1.5f, 1.5f);
        this.takara.setVisible(false);
        this.npc26 = new NPC_NORMAL(527, 26, 0, 14, 5, 4.71f, 0.0f, -30.08f, 0.0f);
        this.npc26.setMotion(0, 17);
        this.npc26.setVisible(false);
        this.npc26.disableDTKFlag(131072);
        this.npc26.disableDTKFlag(65536);
    }

    void npcset_2() {
    }

    void npcset_3() {
    }

    void npcset_4() {
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

    class Object
            extends Unit {
        Object() {
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

        void TUKARETA() {
            ST0100.this.cam0.setMode(-1);
            ST0100.this.EV_Camera10();
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7055, 1, 0);
            System.sleep(30);
            ST0100.this.win = Window.create();
            ST0100.this.win.setSize(4, 45);
            ST0100.this.win.setLocation(15, 305);
            ST0100.this.win.print(ST0100.this.msgTUKARE, 0);
            ST0100.waitPage(ST0100.this.win, 64);
            System.sleep(20);
            ST0100.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
        }
    }
}

