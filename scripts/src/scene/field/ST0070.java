import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0070
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK07_PRJ {
    int button_flg;
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
    Unit[] unit;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
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
    int touchFlag6;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    int S014A;
    int S014B;
    int S015B;
    int eveflag_1;
    int eveflag_2;
    int eveflag_3;
    int eveflag_4;
    int eveflag_5;
    Effect fade;
    Unit monitor1;
    Unit monitor2;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] msg5774FF3B = new String[]{"/[label()]", "Man, I'm getting sick of this. Investigating planetary disappearances and recovering flotsam! There's nothing for us Marines to do!", "/[waitkey(64)]/[close()]"};
    String[] msg5774FF3C = new String[]{"/[label()]", "What is a Marine worth without battles to fight? I'm so bored, and I'm getting out of shape!", "/[waitkey(64)]/[close()]"};
    String[] msg578316BC = new String[]{"/[label()]", "Y-you're Vector personnel, right? Then you must know what that thing we recovered is.", "/[waitkey(64)]/[clear()]"};
    String[] msg578316BD = new String[]{"/[label(Shion)]", "The thing we recovered? I'm sorry, I don't know anything about it.", "/[waitkey(64)]/[clear()]"};
    String[] msg578316BE = new String[]{"/[label()]", "Whoa, not even Vector knows about it? I'd thought the Investigations Unit came from Vector.", "/[waitkey(64)]/[close()]"};
    String[] msg5783766B = new String[]{"/[label()]", "It's supposedly something really dangerous. I heard that some of the recovery workers disappeared the instant they touched that thing.", "/[waitkey(64)]/[clear()]"};
    String[] msg5783766D = new String[]{"/[label()]", "Maybe it's the Gnosis' secret weapon?", "/[waitkey(64)]/[close()]"};
    String[] msg57912E3D = new String[]{"/[label()]", "What are these Gnosis things anyway? In this era of scientific omnipotence, I never thought we'd be fighting ghosts and monsters.", "/[waitkey(1)]/[clear()]", "They creep me out.", "/[waitkey(64)]/[close()]"};
    String[] msg5635CC33 = new String[]{"/[label()]", "H-hey, you're Vector personnel, right? Is it true that this ship has the ultimate anti-Gnosis weapon onboard?", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC34 = new String[]{"/[label(Shion)]", "Yes, that's true.", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC35 = new String[]{"/[label()]", "Hmm...Since it's called the ultimate weapon, it must be really amazing! I bet it's massive and capable of zapping entire hordes of Gnosis, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC36 = new String[]{"/[label(Shion)]", "Oh, no. It's not that large. And about the zapping part...", "/[waitkey(64)]/[close()]"};
    String[] msg5635CC37 = new String[]{"/[label()]", "I bet it can fire huge beams from its chest area and vaporize the enemy in a single shot.", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC38 = new String[]{"/[label(Shion)]", "What? No, I didn't design it to be able to do things like that.", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC39 = new String[]{"/[label()]", "So who's gonna pilot the secret weapon?", "/[waitkey(64)]/[clear()]"};
    String[] msg5635CC3A = new String[]{"/[label(Shion)]", "Huh? Well, uh, umm...excuse me, but I have to go!", "/[waitkey(64)]/[close()]"};
    String[] msg56362BE7 = new String[]{"/[label()]", "Hey, have they decided on a pilot for the secret weapon?", "/[waitkey(1)]/[clear()]", "If you haven't decided yet, let me pilot it for ya! I bet I can utilize the secret weapon's functions to the fullest!", "/[waitkey(64)]/[clear()]"};
    String[] msg56362BE8 = new String[]{"/[label(Shion)]", "W-what?! Uh, no, pilots aren't an issue...", "/[waitkey(1)]/[clear()]", "Umm...I'm sorry. Goodbye!", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D3F = new String[]{"/[label()]", "Damn it, now where could it be? I thought I left it in this pocket.", "/[waitkey(64)]/[clear()]"};
    String[] msg57AD5D40 = new String[]{"/[label(Shion)]", "Is something the matter?", "/[waitkey(64)]/[clear()]"};
    String[] msg57AD5D41 = new String[]{"/[label()]", "Huh? Oh, I think I misplaced the key to my quarters. Where did it go...?", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D42 = new String[]{"/[label()]", "Damn, I can't find it. I left my gear in there so I better find that key soon, or else Lieutenant Virgil's gonna kick my butt.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D45 = new String[]{"/[label()]", "It's locked.", "/[waitkey(64)]/[clear()]"};
    String[] msg57AD5D47 = new String[]{"/[label()]", "Hey! Hey! Those are my quarters! Don't be snooping around!", "/[waitkey(64)]/[clear()]"};
    String[] msg57AD5D48 = new String[]{"/[label(Shion)]", "Oh, sorry!", "/[waitkey(64)]/[clear()]"};
    String[] msg57AD5D49 = new String[]{"/[label()]", "Well, it's not like you can get in without the key anyway.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D50 = new String[]{"I'm sorry.\n", "I'll add text in later!", "/[waitkey(64)]/[close()]"};
    String[] msg5775739A = new String[]{"/[label()]", "Man, day in, day out, all we do is train. Where's the motivation in that? Show me real combat soon, or I'm gonna get rusty.", "/[waitkey(64)]/[close()]"};
    String[] msg5775739E = new String[]{"/[label()]", "Hey, I bet you want to see me in action too, right?!", "/[waitkey(64)]/[clear()]"};
    String[] msg5775739B = new String[]{"/[label(Shion)]", "M-me? It's the same to me, either way...", "/[waitkey(64)]/[clear()]"};
    String[] msg5775739C = new String[]{"/[label()]", "Yep, I knew it! You want to see this chiseled body in a wild rage, don't you?!", "/[waitkey(64)]/[clear()]"};
    String[] msg5775739D = new String[]{"/[label(Shion)]", "No. Umm...were you listening at all to what I just said?", "/[waitkey(64)]/[close()]"};
    String[] msg5775D34A = new String[]{"/[label()]", "Man, I'm itching for a good fight!", "/[waitkey(64)]/[close()]"};
    String[] msg5725D023 = new String[]{"/[label()]", "The recovery of a mysterious object, a top-secret Investigations Unit, and the sudden disappearance of workers...", "/[waitkey(1)]/[clear()]", "It all stinks of a conspiracy.", "/[waitkey(64)]/[clear()]"};
    String[] msg5725D024 = new String[]{"/[label(Shion)]", "Really? Do I smell?!", "/[waitkey(64)]/[clear()]"};
    String[] msg5725D025 = new String[]{"/[label()]", "Oh yeah, it reeks. Someone must be pulling some strings behind the scenes.", "/[waitkey(64)]/[clear()]"};
    String[] msg5725D026 = new String[]{"/[label(Shion)]", "What? I smell that badly?!", "/[waitkey(1)]/[clear()]", "...Pulling strings? No one's been pulling my finger or anything.", "/[waitkey(64)]/[clear()]"};
    String[] msg5725D027 = new String[]{"/[label()]", "That's right...it definitely stinks.", "/[waitkey(1)]/[clear()]", "This incident must have something to do with the Gnosis. I WILL get to the bottom of this conspiracy!", "/[waitkey(64)]/[clear()]"};
    String[] msg5725D028 = new String[]{"/[label(Shion)]", "What?! The smell of Gnosis! I must be really smelly...", "/[waitkey(1)]/[clear()]", "How embarrassing. I think I've been cooped up in the lab for too long. I'd better take a shower later.", "/[waitkey(64)]/[close()]"};
    String[] msg5603D6A4 = new String[]{"/[label()]", "Say, have you ever seen a real Gnosis?", "/[waitkey(64)]/[clear()]"};
    String[] msg5603D6A5 = new String[]{"/[label(Shion)]", "No, I've only seen simulated images of them.", "/[waitkey(64)]/[clear()]"};
    String[] msg5603D6A6 = new String[]{"/[label()]", "I see. I've never seen a real one either. But I hear they have no real substance.", "/[waitkey(64)]/[close()]"};
    String[] msg56043653 = new String[]{"/[label()]", "If they attack you, you'll die instantly, almost as if your soul got sucked out or something.", "/[waitkey(1)]/[clear()]", "Sounds like something straight out of a horror story.", "/[waitkey(64)]/[close()]"};
    String[] msg5737BB69 = new String[]{"/[label()]", "Well, if it isn't Ms. Vector! I hear the ultimate weapon hasn't been field tested yet! I bet you haven't been able to find a pilot yet, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6A = new String[]{"/[label(Shion)]", "Um, like I said, pilots aren't the issue...", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6B = new String[]{"/[label()]", "Oh, come on, no need to be shy! Ask me, and I'm yours!", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6C = new String[]{"/[label(Shion)]", "Hmm, the lights are on, but no one's home, huh?", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6D = new String[]{"/[label()]", "Hey, can't you see it? Me, valiantly piloting the ultimate weapon.", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6E = new String[]{"/[label(Shion)]", "Hello? Anybody home?", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB6F = new String[]{"/[label()]", "I can see it already. They'll refer to me as \"Red Star\" or \"Black Star\" or something like that.", "/[waitkey(64)]/[clear()]"};
    String[] msg5737BB70 = new String[]{"/[label(Shion)]", "Definitely a space cadet...", "/[waitkey(64)]/[close()]"};
    String[] msg5737BB71 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg57ADD19E = new String[]{"/[label()]", "Damn, can't find it...Where the heck did it go?", "/[waitkey(1)]/[clear()]", "Where's my gear? Did I forget it?", "/[waitkey(64)]/[close()]"};
    String[] msg57ADD1A1 = new String[]{"/[label()]", "Damn! Can't find it. At this rate, I'm gonna be late for training.", "/[waitkey(64)]/[clear()]"};
    String[] msg57ADD19F = new String[]{"/[label(Shion)]", "Why don't you look for it after your training?", "/[waitkey(64)]/[clear()]"};
    String[] msg57ADD1A0 = new String[]{"/[label()]", "I can't. Lieutenant Virgil will fry me on a spit if I'm not ready to go. Then I'll never live down the commentary he's sure to spout...", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"Lieutenant Grable, your excessive use of ammo was noted during today's hostage rescue training.", "/[waitkey(64)]/[clear()]"};
    String[] msg00000002 = new String[]{"/[label(Lt. Grable)]", "Sorry. I got caught up...", "/[waitkey(64)]/[clear()]"};
    String[] msg00000003 = new String[]{"/[label()]", "As a field commander, you must maintain your composure at all times.", "/[waitkey(1)]/[clear()]", "Otherwise your actions will result in the deaths of those whom you command.", "/[waitkey(64)]/[clear()]"};
    String[] msg00000004 = new String[]{"/[label(Lt. Grable)]", "I know. I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg00000005 = new String[]{"/[label()]", "Keep in mind that you're an officer and try to act accordingly.", "/[waitkey(64)]/[close()]"};
    String[] msg00000006 = new String[]{"/[label(Lt. Grable)]", "I'm embarassed to say I messed up during training.", "/[waitkey(1)]/[clear()]", "When I found out that the hostage was a kid, my emotions took over.", "/[waitkey(64)]/[close()]"};
    String[] msg00000007 = new String[]{"/[label(Lt. Grable)]", "Guess I have a ways to go...", "/[waitkey(64)]/[close()]"};
    String[] msg00000008 = new String[]{"/[label()]", "The government's Investigations Unit is keeping a close grip on that object that was recovered the other day.", "/[waitkey(64)]/[close()]"};
    String[] msg00000009 = new String[]{"/[label()]", "And they're referring to it by the code name \"Zohar.\"", "/[waitkey(64)]/[close()]"};
    String[] msg00000010 = new String[]{"/[label()]", "The Woglinde is truly a wonderful ship! But I am unable to express in words how wonderful I think this ship really is.", "/[waitkey(1)]/[clear()]", "Please go have a walk around the ship and enjoy the greatness of this craft for yourself.", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Corridor 3'", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0070() {
    }

    void EV_Camera1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.673f, 1.931f, 12.381f);
        this.camEV.setRotate(0.0519f, -89.759f, 0.0f);
        this.camEV.setFov(39.999f);
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
                this.npc5.kickEnepc(4, 1);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.npc5.kickEnepc(1, 4);
                this.win.print(this.msg57AD5D47, 0);
                ST0070.waitPage(this.win, 64);
                this.win.print(this.msg57AD5D48, 0);
                ST0070.waitPage(this.win, 64);
                this.win.print(this.msg57AD5D49, 0);
                ST0070.waitPage(this.win, 64);
                this.npc5.kickEnepc(4, 0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Stage.setVisible(93, false);
                this.monitor1.signal(1);
                this.monitor2.signal(1);
                this.cam0.setMode(-1);
                this.EV_Camera1();
                this.win = Window.create();
                this.win.print(this.msgMAP, 0);
                ST0070.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(20);
                this.cam0.setMode(0);
                Stage.setVisible(93, true);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 4: {
                if (Runtime.getFlags(23, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(22, 1) != 1 || Runtime.getFlags(7062, 1) != 0) break;
                Runtime.mailArriveSet(2);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7062, 1, 1);
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
        if (this.S015B == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        this.Talk_npc10_1(window);
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg00000008, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000009, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc10_2(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        window.print(this.msg00000010, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc11_2(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg5774FF3B, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg5774FF3C, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg5775739A, 0);
                ST0070.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg5775739E, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5775739B, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5775739C, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5775739D, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg5775D34A, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg578316BC, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg578316BD, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg578316BE, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg5783766B, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5783766D, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msg5725D023, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5725D024, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5725D025, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5725D026, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5725D027, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5725D028, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg57912E3D, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg5603D6A4, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5603D6A5, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5603D6A6, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg56043653, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg5635CC33, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC34, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC35, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC36, 0);
                ST0070.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg5635CC37, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC38, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC39, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5635CC3A, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg56362BE7, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg56362BE8, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg5737BB69, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6A, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6B, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6C, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6D, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6E, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB6F, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg5737BB70, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg5737BB6D, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg5737BB71, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg57AD5D3F, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg57AD5D40, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg57AD5D41, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57AD5D42, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg57ADD19E, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57ADD1A1, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg57ADD19F, 0);
        ST0070.waitPage(window, 64);
        window.print(this.msg57ADD1A0, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
    }

    void Talk_npc6_2(Window window) {
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg00000006, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000007, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg00000002, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg00000003, 0);
                ST0070.waitPage(window, 64);
                window.print(this.msg00000004, 0);
                ST0070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000005, 0);
        ST0070.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0070.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc9_2(window);
        } else {
            this.Talk_npc9_1(window);
        }
    }

    void Talk_npc9_1(Window window) {
    }

    void Talk_npc9_2(Window window) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(100, 4);
                break;
            }
            case 2: {
                Runtime.jumpCF(90, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(140, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -18.0f, 0.0f, 12.0f, 0.0f);
        this.teiten1.SetBgm(196624);
        this.teiten2 = new Uwamono(28690, -18.0f, 0.0f, 14.0f, 0.0f);
        this.teiten2.SetBgm(196624);
        this.teiten3 = new Uwamono(28690, -4.0f, 0.0f, -4.5f, 0.0f);
        this.teiten3.SetBgm(196625);
        this.teiten4 = new Uwamono(28690, -4.0f, 0.0f, -0.5f, 0.0f);
        this.teiten4.SetBgm(196625);
        this.teiten5 = new Uwamono(28690, -4.0f, 0.0f, 3.5f, 0.0f);
        this.teiten5.SetBgm(196625);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(10, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(11, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(12, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(13, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(14, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(15, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(16, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -9.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestal(4, -18.872f, 4.022f, 16.727f, 65.0f, -44.62f, -21.666f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(4, 1);
        this.cam0.setCFAngle(5, -28.0f, 335.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(6, false);
        Stage.setVisible(7, false);
        Stage.setVisible(8, false);
        Stage.setVisible(9, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.monitor1 = new Object();
        this.monitor1.init(24613, -5.1f, 1.7f, 12.33f, 270.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18002, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Object();
        this.monitor2.init(24613, -5.1f, 1.7f, 12.33f, 270.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18006, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.doorA = new Uwamono(30, 40, '\u0001');
        this.doorB = new Uwamono(28, 40, '\u0001');
        this.doorC = new Uwamono(75, 42, '\u0001');
        new Uwamono(76, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(77, 42, '\u0001');
        new Uwamono(78, 42, '\u0001', this.doorD);
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
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
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(519, 1, 0, 14, 6, -5.0f, 0.0f, -5.0f, 90.0f);
        this.npc2 = new NPC_NORMAL(523, 2, 0, 13, 13, -19.8f, 0.0f, 11.1f, 50.0f);
        this.npc3 = new NPC_NORMAL(520, 3, 0, 14, 12, -17.29f, 0.2f, 16.4f, 180.0f);
        this.npc4 = new NPC_NORMAL(525, 4, 0, 5, 4, -11.7f, 0.0f, 10.1f, 0.0f);
        this.npc5 = new NPC_NORMAL(526, 5, 0, 17, 25, -10.2f, 0.0f, 26.2f, -45.0f);
        this.npc7 = new NPC_NORMAL(525, 7, 0, 14, 6, -11.08f, 0.0f, 2.8f, 320.0f);
        this.npc8 = new NPC_NORMAL(542, 8, 0, 13, 11, -11.61f, 0.0f, 3.47f, 140.0f);
        this.npc10 = new NPC_NORMAL(542, 10, 0, 13, 11, -18.26f, 0.0f, 11.97f, 270.0f);
        this.npc11 = new NPC_NORMAL(537, 11, 0, 2, 4, -7.65f, 0.0f, 34.9f, 180.0f);
        this.npc1.setMotion(0, 10);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 7);
        this.npc2.setInvalidID(1);
        this.npc3.setMotion(0, 9);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setInvalidID(1);
        this.npc4.setMotion(0, 10);
        this.npc5.setMotion(0, 6);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc7.setMotion(0, 9);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc8.setMotion(0, 9);
        this.npc8.disableDTKFlag(3);
        this.npc10.setMotion(0, 10);
        this.npc10.disableDTKFlag(2);
        this.npc10.enableDTKFlag(4);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
    }

    void npcset_2() {
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
    }
}

