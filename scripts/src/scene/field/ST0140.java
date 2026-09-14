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
import xeno.map.MC_VOK14_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0140
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK14_PRJ {
    Player player;
    Camera cam1;
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
    Unit unit1;
    Menu menu;
    Window win;
    String moji;
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
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono drill;
    Uwamono kon0;
    Uwamono kon1;
    Uwamono kon2;
    Uwamono kon3;
    Uwamono kon4;
    Uwamono kon5;
    Uwamono kon6;
    Uwamono kon7;
    Uwamono kon8;
    Unit DRILLD;
    Uwamono teiten1;
    Uwamono teiten2;
    int eveflag_1;
    int eveflag_2;
    int eveflag_3;
    int eveflag_4;
    int eveflag_5;
    int eveflag_6;
    int S014A;
    int S014B;
    int S015B;
    Effect fade;
    Effect fade30;
    Effect fade60;
    Effect EF00;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Light light = new Light(0);
    int breakflg;
    int success;
    int DrillResult;
    int page;
    String[] msgD01 = new String[]{"/[label(Holgar)]", "Hi, Shion! You're looking cute as usual.", "/[waitkey(64)]/[clear()]"};
    String[] msgD02 = new String[]{"/[label(Shion)]", "Hello, Mr. Holgar.", "/[waitkey(64)]/[clear()]"};
    String[] msgD03 = new String[]{"/[label(Mr. Holgar)]", "Hey now, quit with the formalities. Call me \"Mr. Driller\"!", "/[waitkey(64)]/[clear()]"};
    String[] msgD04 = new String[]{"/[label(Shion)]", "D-Driller...did you say?", "/[waitkey(64)]/[clear()]"};
    String[] msgD05 = new String[]{"/[label(Mr. Holgar)]", "I'm really skilled at using the drill crane. So much so, I wish the Drill Council would acknowledge it! Of course, Susumu Hori and I are equals when it comes to drilling.", "/[waitkey(1)]/[clear()]", "Don't ever confuse me for a run-of-the-mill drill\nworker!", "/[waitkey(64)]/[clear()]"};
    String[] msgD06 = new String[]{"/[label(Shion)]", "Umm...Mr. Holgar?", "/[waitkey(64)]/[clear()]"};
    String[] msgD07 = new String[]{"/[label(Mr. Holgar)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msgD08 = new String[]{"/[label(Shion)]", "Mr. Hol...I mean, Mr. Driller?", "/[waitkey(64)]/[clear()]"};
    String[] msgD09 = new String[]{"/[label(Mr. Holgar)]", "What is it, Shion?", "/[waitkey(64)]/[clear()]"};
    String[] msgD10 = new String[]{"/[label(Shion)]", "I have to get going.", "/[waitkey(64)]/[clear()]"};
    String[] msgD11 = new String[]{"/[label(Mr. Holgar)]", "Bah, that doesn't matter at all right now.", "/[waitkey(1)]/[clear()]", "A clumsy worker caused an accident, and they can't work because the wreckage is in the way. That's where my drill crane and I come in.", "/[waitkey(64)]/[clear()]"};
    String[] msgD12 = new String[]{"/[label(Mr. Holgar)]", "But even with an awesome driller like me, this is one heckuva tough job for just one guy! It's tough! Real tough for just one guy! That's where you, my cute\nlittle Shion, come into the picture! You've answered my drill call!", "/[waitkey(1)]/[clear()]", "Am I right? I'm right, aren't I? You'll do it, won't you?! You'll help my noble cause, won't you?!", "/[waitkey(64)]/[close()]"};
    String[] msgD13 = new String[]{"/[label(Mr. Holgar)]", "Fine. So you're not going to help me and my drill, huh?\nI see how it is...", "/[waitkey(64)]/[clear()]"};
    String[] msgD14 = new String[]{"/[label(Shion)]", "What? No, that's not it. But...drilling? I don't know.", "/[waitkey(64)]/[clear()]"};
    String[] msgD15 = new String[]{"/[label(Mr. Holgar)]", "Oh that's okay, don't worry about it. Sigh...I've always thought of you like a daughter, but I guess you don't understand me at all.", "/[waitkey(64)]/[clear()]"};
    String[] msgD16 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgD45 = new String[]{"/[label(Mr. Holgar)]", "I see. No go, huh? Well, I guess that isn't much of a surprise. No reason why you'd want to keep company with a drill and a middle-aged guy like me.", "/[waitkey(1)]/[clear()]", "After all, it isn't fair to expect a girl to understand my passion for drills.", "/[waitkey(64)]/[clear()]"};
    String[] msgD46 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgD17 = new String[]{"/[label(Shion)]", "Um, Mr. Hol...Mr. Driller? I'll give it a try, but I may not be able to fully appreciate drilling like you do.", "/[waitkey(64)]/[clear()]"};
    String[] msgD18 = new String[]{"/[label(Mr. Holgar)]", "Yeah? You'll take over my drill then?! Okay. First, I'll teach you how to operate the drill crane.", "/[waitkey(1)]/[clear()]", "Listen closely, operating a drill is not about reflexes or clever techniques. It's all about heart and the soul within! Passion for your drill will allow you to operate\nthe drill as you wish!", "/[waitkey(64)]/[clear()]"};
    String[] msgD19 = new String[]{"/[label(Shion)]", "(Boy, he's really gone off the deep end...)", "/[waitkey(64)]/[clear()]"};
    String[] msgD20 = new String[]{"/[label(Mr. Holgar)]", "Now, I'll show you how to operate the drill. Listen carefully and get this all down, okay?", "/[waitkey(1)]/[clear()]", "First, I'll explain vertical movement. If you press the □ Button, the drill will start moving vertically. If you let go, the drill will stop right at that spot.", "/[waitkey(1)]/[clear()]", "Even if you keep pressing the button, the drill will stop once it's reached the edge of the rail.", "/[waitkey(1)]/[clear()]", "Before moving the drill, you can move the camera freely using the Left Analog Stick. You can also switch between cameras using the R2 Button.", "/[waitkey(1)]/[clear()]", "You'll need to switch cameras to determine the drill's precise location, so be sure to make good use of them.", "/[waitkey(1)]/[clear()]", "Moving the drill is a one shot deal. Operate the buttons carefully.", "/[waitkey(1)]/[clear()]", "When you're done moving vertically, you'll then move the crane horizontally. Switch cameras using the R2 Button and use the □ Button to move the drill, just like you\ndid when moving the crane vertically.", "/[waitkey(1)]/[clear()]", "Of course, this is also a one shot deal that you can't do over.", "/[waitkey(1)]/[clear()]", "When you're done moving vertically and horizontally, the drill will do the work on its own. The drill will come down and destroy anything under it. Obviously, nothing will happen if nothing's there.", "/[waitkey(1)]/[clear()]", "If you miss, you can always do it over again. And if you want to quit, the crane will stop if you press the × Button.", "/[waitkey(1)]/[clear()]", "That's everything you need to know about the drill. Well, what do you think? Simple, right?", "/[waitkey(64)]/[clear()]"};
    String[] msgD21 = new String[]{"/[label(Shion)]", "Hmm, I think I get it...sort of...", "/[waitkey(64)]/[close()]"};
    String[] msgD22 = new String[]{"/[label(Mr. Holgar)]", "Well, the best way to learn is by doing. If you're uneasy about it, I'll explain it to you again.", "/[waitkey(64)]/[close()]"};
    String[] msgD23 = new String[]{"/[label(Mr. Holgar)]", "At a girl, Shion! You really understand the soul of the drill!", "/[waitkey(64)]/[clear()]"};
    String[] msgD24 = new String[]{"/[label(Shion)]", "What? Um, you think so?", "/[waitkey(64)]/[clear()]"};
    String[] msgD25 = new String[]{"/[label(Mr. Holgar)]", "All on target! At this rate, you could take over my job any time!", "/[waitkey(1)]/[clear()]", "Oh, here's a present for you. It's a /[color(0x329bbe)]Drill Passport/[color(0x808080)].", "/[waitkey(1)]/[close()]"};
    String[] msgD26 = new String[]{"/[label(Shion)]", "T-thank you very much.", "/[waitkey(64)]/[clear()]"};
    String[] msggood = new String[]{"/[label(Mr. Holgar)]", "It's just a little thank you for showing me what a true driller is all about. Take this item with you, too.", "/[waitkey(64)]/[close()]"};
    String[] msgD27 = new String[]{"/[label(Shion)]", "Phew...", "/[waitkey(1)]/[clear()]", "Looks like I finally got it done.", "/[waitkey(64)]/[clear()]"};
    String[] msgD28 = new String[]{"/[label(Mr. Holgar)]", "Not too shabby! I knew you had a passion for drillin', Shion. Never giving up, even when you miss. That's the most important quality in a driller.", "/[waitkey(64)]/[clear()]"};
    String[] msgD29 = new String[]{"/[label(Shion)]", "Uh, umm...well, thanks.", "/[waitkey(64)]/[clear()]"};
    String[] msgD30 = new String[]{"/[label(Mr. Holgar)]", "Thanks to you, everything is nice and tidy again. Here, a token of my thanks. It's a /[color(0x329bbe)]Drill Passport/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] msgnormal = new String[]{"/[label(Mr. Holgar)]", "Here's a little something extra. Go ahead, it's yours.", "/[waitkey(64)]/[close()]"};
    String[] msgD31 = new String[]{"/[label(Mr. Holgar)]", "Hey, is that the best you can do? There are still some targets left. The drill's crying!", "/[waitkey(64)]/[clear()]"};
    String[] msgD32 = new String[]{"/[label(Shion)]", "What? Oh, I'm sorry...", "/[waitkey(64)]/[clear()]"};
    String[] msgD33 = new String[]{"/[label(Mr. Holgar)]", "Well, I guess it can't be helped if you're busy. But the drill isn't gonna wait for you. Destroy things while you can. That's the fundamentals of drillin'.", "/[waitkey(64)]/[clear()]"};
    String[] msgD34 = new String[]{"/[label(Shion)]", "...Ah.", "/[waitkey(64)]/[close()]"};
    String[] msgD47 = new String[]{"/[label(Mr. Holgar)]", "Yeah, good. You're quite a driller, Shion. It would have been even better if you hadn't given up in the middle.", "/[waitkey(64)]/[clear()]"};
    String[] msgD48 = new String[]{"/[label(Shion)]", "Oh, well, umm...I'm sorry.", "/[waitkey(64)]/[clear()]"};
    String[] msgD49 = new String[]{"/[label(Mr. Holgar)]", "No no no. That was rude of me to say that, especially after you helped me out. This is a token of my thanks. Take it.", "/[waitkey(64)]/[close()]"};
    String[] msgD35 = new String[]{"/[label(Mr. Holgar)]", "Hey, Shion!", "/[waitkey(64)]/[clear()]"};
    String[] msgD36 = new String[]{"/[label(Shion)]", "Hello, Mr. Hol...I mean Mr. Driller.", "/[waitkey(64)]/[clear()]"};
    String[] msgD37 = new String[]{"/[label(Mr. Holgar)]", "They say people who master the art of the drill look different. You know what? You look a little more mature now!", "/[waitkey(1)]/[clear()]", "Don't you ever let that passion for drilling die now, okay?!", "/[waitkey(64)]/[close()]"};
    String[] msgD38 = new String[]{"/[label(Mr. Holgar)]", "Hey, Shion!", "/[waitkey(64)]/[clear()]"};
    String[] msgD39 = new String[]{"/[label(Shion)]", "Hello, Mr. Hol...I mean Mr. Driller.", "/[waitkey(64)]/[clear()]"};
    String[] msgD40 = new String[]{"/[label(Mr. Holgar)]", "Leaving the work unfinished was bothering you, wasn't it? Yep, I knew it! A love for the drill won't let you quit halfway.", "/[waitkey(1)]/[clear()]", "You'll give it a go, of course, won't you?", "/[waitkey(64)]/[close()]"};
    String[] msgD41 = new String[]{"/[label(Mr. Holgar)]", "Hey, Shion!", "/[waitkey(64)]/[clear()]"};
    String[] msgD42 = new String[]{"/[label(Shion)]", "Hello, Mr. Hol...I mean Mr. Driller.", "/[waitkey(64)]/[clear()]"};
    String[] msgD43 = new String[]{"/[label(Mr. Holgar)]", "Heh heh, I thought you'd come back. You couldn't forget about this drill, could you?", "/[waitkey(1)]/[clear()]", "So how about it? Ready to fire up the ol' drill?", "/[waitkey(64)]/[close()]"};
    String[] msgD44 = new String[]{"/[label(Mr. Holgar)]", "All right, that's the spirit! Do you remember how to work the drill? Want me to explain it again?", "/[waitkey(64)]/[close()]"};
    String[] msgOrei = new String[]{"/[label(Mr. Holgar)]", "Yo, Shion! Thanks for what you did earlier. Don't forget your passion for drillin'!", "/[waitkey(64)]/[close()]"};
    String[] msg25C86B94 = new String[]{"/[label(Mr. Holgar)]", "Sorry, this crane isn't working right now.", "/[waitkey(64)]/[close()]"};
    String[] msg25C86BA0 = new String[]{"/[label()]", "Please be careful around here. It's a high traffic area for work carts, there's cargo being moved overhead by wires, and just now there was an accident over there.", "/[waitkey(64)]/[close()]"};
    String[] msg25C86BA1 = new String[]{"/[label()]", "Some of the soldiers were drafted unexpectedly, so the troop formation is not quite up to par. If the Gnosis attacked us now, I don't know if we could fight them head-on.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D50 = new String[]{"I'm sorry,\n", "I'll add it text in later!", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"There are no lines for the 2nd time yet.\n", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "Hey, that container doesn't belong there. Take it to Block 3!", "/[waitkey(1)]/[clear()]", "What are you doing?! Take all the ammo to the A.G.W.S. Hangar!", "/[waitkey(64)]/[close()]"};
    String[] msg00000002 = new String[]{"/[label()]", "Man, with the hangar taken over by that weird monolith, there's no place for other supplies. It's a complete mess.", "/[waitkey(64)]/[close()]"};
    String[] msg00000003 = new String[]{"/[label()]", "The most important thing in hand-to-hand combat is to know what your enemy looks like and have an understanding of its behavior.", "/[waitkey(64)]/[close()]"};
    String[] msg00000004 = new String[]{"/[label()]", "By correctly grasping battle situations, our troops can increase their survival rate by 30%.", "/[waitkey(64)]/[close()]"};
    String[] msg00000005 = new String[]{"/[label()]", "Please don't worry, we will protect all the personnel onboard this ship.", "/[waitkey(64)]/[close()]"};
    String[] msg00000006 = new String[]{"/[label()]", "Not even the Gnosis will be able to stand up to the teamwork of the A.G.W.S. and us.", "/[waitkey(64)]/[close()]"};
    String[] msg00000007 = new String[]{"/[label()]", "Vector employees are all such polite, friendly people.", "/[waitkey(1)]/[clear()]", "I was under the impression that researchers were all uptight, so I was a little surprised.", "/[waitkey(64)]/[close()]"};
    String[] msgkure = new String[]{"/[label(Mr. Holgar)]", "Wanna try a drilling game?", "/[waitkey(64)]/[close()]"};
    String[] msg0001 = new String[]{"Your first turn is over.", "/[wait(60)]/[close()]"};
    String[] msg0002 = new String[]{"The next one is the last one. Hang in there.", "/[wait(60)]/[close()]"};
    String[] msg0003 = new String[]{"All right. Good work.", "/[wait(60)]/[close()]"};
    String[] msg0004 = new String[]{"Are you getting used to it now?", "/[wait(60)]/[close()]"};
    String[] msgend = new String[]{"End Driller Game?", "/[waitkey(64)]/[close()]"};
    String[] msgsuccess = new String[]{"/[label(Mr. Holgar)]", "Not bad at all, Shion.", "/[wait(60)]/[close()]"};
    String[] msgfault = new String[]{"/[label(Mr. Holgar)]", "Ooh. That was close.", "/[wait(60)]/[close()]"};
    String[] msg_A = new String[]{"Acquired pattern A results.", "/[waitkey(64)]/[close()]"};
    String[] msg_B = new String[]{"Acquired pattern B results.", "/[waitkey(64)]/[close()]"};
    String[] msg_C = new String[]{"Acquired pattern C results.", "/[waitkey(64)]/[close()]"};
    String[] msg_D = new String[]{"Acquired pattern D results.\n", "This pattern was not anticipated.\n", "Please check the source.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0140() {
    }

    void After_Drill() {
        System.println("ドリルゲー終了");
        this.fade60.call(1);
        System.sleep(60);
        this.player.setTranslate(-4.0f, 0.0f, -9.8389f);
        this.player.rotY(1, 270.0f, true);
        this.drill.DrillCommand(3);
        this.npc1.kickEnepc(4, 0);
        this.npc2.kickEnepc(4, 0);
        this.npc3.kickEnepc(4, 0);
        this.npc4.kickEnepc(4, 0);
        this.npc5.kickEnepc(4, 0);
        this.npc6.kickEnepc(4, 0);
        this.npc7.kickEnepc(4, 0);
        this.npc1.setVisible(true);
        this.npc2.setVisible(true);
        this.npc3.setVisible(true);
        this.npc4.setVisible(true);
        this.npc5.setVisible(true);
        this.npc6.setVisible(true);
        this.npc7.setVisible(true);
        this.npc9.setVisible(false);
        this.npc10.setVisible(false);
        this.npc11.setVisible(false);
        this.npc12.setVisible(false);
        this.npc9.kickEnepc(4, 1);
        this.npc10.kickEnepc(4, 1);
        this.npc11.kickEnepc(4, 1);
        this.npc12.kickEnepc(4, 1);
        this.player.setVisible(true);
        if (Runtime.getFlags(4066, 1) == 1) {
            if (Runtime.getFlags(4004, 1) == 1 && Runtime.getFlags(4005, 1) == 1 && Runtime.getFlags(4006, 1) == 1 && Runtime.getFlags(4007, 1) == 1 && Runtime.getFlags(4008, 1) == 1) {
                this.Result_E();
            } else if (this.breakflg < 5) {
                this.Result_C();
            } else {
                this.Result_D();
            }
        } else if (Runtime.getFlags(4066, 1) == 0) {
            if (this.drill.DrillCommand(2) <= 6 && Runtime.getFlags(4004, 1) == 1 && Runtime.getFlags(4005, 1) == 1 && Runtime.getFlags(4006, 1) == 1 && Runtime.getFlags(4007, 1) == 1 && Runtime.getFlags(4008, 1) == 1) {
                this.Result_A();
            } else if (this.drill.DrillCommand(2) > 6 && Runtime.getFlags(4004, 1) == 1 && Runtime.getFlags(4005, 1) == 1 && Runtime.getFlags(4006, 1) == 1 && Runtime.getFlags(4007, 1) == 1 && Runtime.getFlags(4008, 1) == 1) {
                this.Result_B();
            } else if (this.breakflg < 5) {
                this.Result_C();
            } else {
                this.Result_D();
            }
        }
        System.sleep(30);
    }

    void Before_Drill() {
        this.breakflg = 0;
        Runtime.setPlayerControl(false);
        this.fade60.call(0);
        System.sleep(60);
        this.npc1.kickEnepc(4, 1);
        this.npc2.kickEnepc(4, 1);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc1.setVisible(false);
        this.npc2.setVisible(false);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.npc9.setVisible(true);
        this.npc10.setVisible(true);
        this.npc11.setVisible(true);
        this.npc12.setVisible(true);
        this.npc9.kickEnepc(4, 0);
        this.npc10.kickEnepc(4, 0);
        this.npc11.kickEnepc(4, 0);
        this.npc12.kickEnepc(4, 0);
        this.npc9.setMotion(0, 9);
        this.npc10.setMotion(0, 9);
        this.npc11.setMotion(0, 9);
        this.npc12.setMotion(0, 9);
        this.npc1.setVisible(false);
        this.player.setTranslate(-4.05f, 0.0f, -9.83f);
        this.player.rotY(1, -180.0f, true);
        this.drill.SendSignal();
        this.drill.DrillCommand(1);
    }

    void Final_init(int n) {
        switch (n) {
            case 9: {
                this.npc9.kickEnepc(4, 1);
                break;
            }
            case 10: {
                this.npc10.kickEnepc(4, 1);
                break;
            }
            case 11: {
                this.npc11.kickEnepc(4, 1);
                break;
            }
            case 12: {
                this.npc12.kickEnepc(4, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (Runtime.getFlags(23, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(22, 1) != 1 || Runtime.getFlags(7063, 1) != 0) break;
                Runtime.mailArriveSet(9);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                ST0140.waitPage(this.win, 64);
                System.sleep(30);
                Runtime.setFlags(7063, 1, 1);
                Runtime.mailExec(1);
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void NotOperat() {
        this.win = Window.create();
        this.win.print(this.msgD13, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD14, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD15, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD16, 0);
        ST0140.waitPage(this.win, 64);
    }

    void NotOperat2() {
        this.win = Window.create();
        this.win.print(this.msgD45, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD46, 0);
        ST0140.waitPage(this.win, 64);
    }

    void Operation1() {
        this.win = Window.create();
        if (Runtime.getFlags(4009, 1) == 0) {
            this.win.print(this.msgD17, 0);
            ST0140.waitPage(this.win, 64);
            this.win.print(this.msgD18, 0);
            ST0140.waitPage(this.win, 64);
            this.win.print(this.msgD19, 0);
            ST0140.waitPage(this.win, 64);
        }
        this.win.print(this.msgD20, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD21, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD22, 0);
        ST0140.waitPage(this.win, 64);
        Runtime.setFlags(4009, 1, 1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I got it already\nMaybe I should ask again");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.Before_Drill();
                Stage.setVisible(55, false);
                return;
            }
            case 1: {
                this.Operation1();
                return;
            }
        }
    }

    void Operation2() {
        this.win = Window.create();
        this.win.print(this.msgD44, 0);
        ST0140.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I remember\nHow did it work?");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.Before_Drill();
                Stage.setVisible(55, false);
                return;
            }
            case 1: {
                this.Operation1();
                return;
            }
        }
    }

    void Result_A() {
        System.println("パターンＡの結果を得ました");
        Stage.setVisible(55, true);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.msgD23, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD24, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD25, 0);
        ST0140.waitPage(this.win, 64);
        this.moji = Runtime.getItemName(10, 15);
        this.win = Window.create();
        Sound.effectPlay(6);
        this.win.print("/[color(0x329bbe)]Drill Passport/[color(0x808080)] obtained!/[waitkey(64)]/[close()]");
        Runtime.addItem(10, 15);
        ST0140.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.print(this.msgD26, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msggood, 0);
        ST0140.waitPage(this.win, 64);
        Sound.effectPlay(6);
        Runtime.addItemWin(0, 2);
        this.DrillResult = 1;
        Runtime.setFlags(4001, 1, 1);
        Runtime.setFlags(4002, 1, 1);
        Runtime.setPlayerControl(true);
    }

    void Result_B() {
        System.println("パターンＢの結果を得ました");
        Stage.setVisible(55, true);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.msgD27, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD28, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD29, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD30, 0);
        ST0140.waitPage(this.win, 64);
        this.moji = Runtime.getItemName(10, 15);
        this.win = Window.create();
        Sound.effectPlay(6);
        this.win.print("/[color(0x329bbe)]Drill Passport/[color(0x808080)] obtained!/[waitkey(64)]/[close()]");
        Runtime.addItem(10, 15);
        ST0140.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.print(this.msgnormal, 0);
        ST0140.waitPage(this.win, 64);
        Sound.effectPlay(6);
        Runtime.addItemWin(0, 1);
        this.DrillResult = 2;
        Runtime.setFlags(4001, 1, 1);
        Runtime.setFlags(4002, 1, 1);
        Runtime.setPlayerControl(true);
    }

    void Result_C() {
        Stage.setVisible(55, true);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.msgD31, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD32, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD33, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD34, 0);
        ST0140.waitPage(this.win, 64);
        this.DrillResult = 3;
        Runtime.setFlags(4003, 1, 1);
        Runtime.setPlayerControl(true);
    }

    void Result_D() {
        Stage.setVisible(55, true);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.msg_D, 0);
        ST0140.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void Result_E() {
        Stage.setVisible(55, true);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.msgD47, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD48, 0);
        ST0140.waitPage(this.win, 64);
        this.win.print(this.msgD49, 0);
        ST0140.waitPage(this.win, 64);
        this.moji = Runtime.getItemName(10, 15);
        this.win = Window.create();
        Sound.effectPlay(6);
        this.win.print("/[color(0x329bbe)]Drill Passport/[color(0x808080)] obtained!/[waitkey(64)]/[close()]");
        Runtime.addItem(10, 15);
        ST0140.waitPage(this.win, 64);
        this.DrillResult = 1;
        Runtime.setFlags(4001, 1, 1);
        Runtime.setFlags(4002, 1, 1);
        Runtime.setPlayerControl(true);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (Runtime.getFlags(4003, 1) == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
        if (Runtime.getFlags(4002, 1) == 0) {
            window.print(this.msgD01, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD02, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD03, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD04, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD05, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD06, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD07, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD08, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD09, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD10, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD11, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD12, 0);
            ST0140.waitPage(window, 64);
            System.waitFor(window);
            this.menu = Menu.create();
            this.menu.addItem("Maybe I should give it a try\nI have a bad feeling about this");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    if (Runtime.getFlags(4009, 1) == 0) {
                        this.Operation1();
                    } else if (Runtime.getFlags(4009, 1) == 1) {
                        this.Operation2();
                    }
                    return;
                }
                case 1: {
                    this.NotOperat();
                    Runtime.setFlags(4003, 1, 1);
                    return;
                }
            }
            return;
        }
        if (Runtime.getFlags(4002, 1) == 1) {
            switch (this.DrillResult) {
                case 1: {
                    window.print(this.msgD35, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD36, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD37, 0);
                    ST0140.waitPage(window, 64);
                    return;
                }
                case 2: {
                    window.print(this.msgD35, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD36, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD37, 0);
                    ST0140.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msgOrei, 0);
            ST0140.waitPage(window, 64);
            return;
        }
    }

    void Talk_npc1_2(Window window) {
        if (Runtime.getFlags(4002, 1) == 0) {
            window.print(this.msgD41, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD42, 0);
            ST0140.waitPage(window, 64);
            window.print(this.msgD43, 0);
            ST0140.waitPage(window, 64);
            System.waitFor(window);
            this.menu = Menu.create();
            this.menu.addItem("Maybe I should give it a try\nI have a bad feeling about this");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    if (Runtime.getFlags(4009, 1) == 0) {
                        this.Operation1();
                    } else if (Runtime.getFlags(4009, 1) == 1) {
                        this.Operation2();
                    }
                    return;
                }
                case 1: {
                    this.NotOperat2();
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (Runtime.getFlags(4002, 1) == 1) {
            switch (this.DrillResult) {
                case 1: {
                    window.print(this.msgD35, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD36, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD37, 0);
                    ST0140.waitPage(window, 64);
                    return;
                }
                case 2: {
                    window.print(this.msgD35, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD36, 0);
                    ST0140.waitPage(window, 64);
                    window.print(this.msgD37, 0);
                    ST0140.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msgOrei, 0);
            ST0140.waitPage(window, 64);
            return;
        }
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg25C86BA0, 0);
        ST0140.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg25C86BA1, 0);
        ST0140.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg00000003, 0);
                ST0140.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000004, 0);
        ST0140.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg00000005, 0);
                ST0140.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000006, 0);
        ST0140.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg00000007, 0);
        ST0140.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0140.waitPage(window, 64);
                this.npc7.enableDTKFlag(4);
                return;
            }
        }
        this.npc7.kickEnepc(0, 10);
        window.print(this.msg00000002, 0);
        ST0140.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    void broken(int n) {
        System.println("破壊チェック");
        this.success = 1;
        this.npc9.kickEnepc(1, 31);
        this.npc10.kickEnepc(1, 31);
        this.npc11.kickEnepc(1, 31);
        this.npc12.kickEnepc(1, 31);
        switch (n) {
            case 0: {
                ++this.breakflg;
                System.println("breakflg[0] = 1");
                this.EF05.disp(false);
                Runtime.setFlags(4004, 1, 1);
                break;
            }
            case 1: {
                ++this.breakflg;
                System.println("breakflg[0] = 1");
                Runtime.setFlags(4005, 1, 1);
                break;
            }
            case 2: {
                ++this.breakflg;
                System.println("breakflg[0] = 1");
                this.EF07.disp(false);
                Runtime.setFlags(4006, 1, 1);
                break;
            }
            case 3: {
                ++this.breakflg;
                System.println("breakflg[0] = 1");
                Runtime.setFlags(4007, 1, 1);
                break;
            }
            case 4: {
                ++this.breakflg;
                System.println("breakflg[0] = 1");
                this.EF06.disp(false);
                Runtime.setFlags(4008, 1, 1);
                break;
            }
        }
    }

    void drill_stanby() {
        System.println("ドリル落下終了");
        if (Runtime.getFlags(4004, 1) == 1 && Runtime.getFlags(4005, 1) == 1 && Runtime.getFlags(4006, 1) == 1 && Runtime.getFlags(4007, 1) == 1 && Runtime.getFlags(4008, 1) == 1) {
            this.After_Drill();
            return;
        }
        if (this.success == 1) {
            this.success = 0;
        } else {
            this.success = 0;
            this.npc9.kickEnepc(1, 8);
            this.npc10.kickEnepc(1, 8);
            this.npc11.kickEnepc(1, 8);
            this.npc12.kickEnepc(1, 8);
        }
    }

    void drill_stop() {
        if (Runtime.getFlags(4004, 1) != 1 || Runtime.getFlags(4005, 1) != 1 || Runtime.getFlags(4006, 1) != 1 || Runtime.getFlags(4007, 1) != 1 || Runtime.getFlags(4008, 1) != 1) {
            this.win = Window.create();
            this.win.print(this.msgend, 0);
            ST0140.waitPage(this.win, 64);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Yes\nNo");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    Runtime.setFlags(4066, 1, 1);
                    this.After_Drill();
                    return;
                }
            }
            this.drill.DrillCommand(5);
            return;
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(70, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(60, 1);
                break;
            }
        }
    }

    void init() {
        int n;
        this.teiten1 = new Uwamono(28690, -4.0f, 0.0f, -12.0f, 0.0f);
        this.teiten1.SetBgm(196639);
        this.teiten2 = new Uwamono(28690, 5.0f, 0.0f, -12.0f, 90.0f);
        this.teiten2.SetBgm(196638);
        this.teiten2.SetBgmType('\u0001');
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        this.drill = new Uwamono(28691, 60, 61, 63);
        this.drill.DrillGameCount(-1);
        this.drill.DrillSetContainerRandom(false);
        this.drill.DrillSetSpeed(0.15f, 0.1f, 0.15f);
        this.drill.DrillSetReturnSpeed(0.3f, 0.5f, 0.3f);
        if (Runtime.getFlags(4004, 1) == 0) {
            this.kon0 = new Uwamono(28712, 0.5f, 0.0f, -19.0f, 0.0f);
            this.kon0.SetParticle(601);
            this.kon0.SetCallNo(0);
            this.EF05 = new Effect(1401, 0.5f, 0.0f, -19.0f, 0.0f);
            this.EF05.disp(true);
            this.EF05.setScale(0.5f, 1.25f, 0.5f);
        }
        if (Runtime.getFlags(4005, 1) == 0) {
            this.kon1 = new Uwamono(28713, -1.5f, 0.0f, -22.0f, 0.0f);
            this.kon1.SetParticle(601);
            this.kon1.SetCallNo(1);
        }
        if (Runtime.getFlags(4006, 1) == 0) {
            this.kon2 = new Uwamono(28714, 2.0f, 0.0f, -16.0f, 330.0f);
            this.kon2.SetParticle(601);
            this.kon2.SetCallNo(2);
            this.EF07 = new Effect(1723, 3.45f, 0.0f, -16.87f, 0.0f);
            this.EF07.disp(true);
            this.EF07.setScale(1.5f, 0.4f, 1.5f);
        }
        if (Runtime.getFlags(4007, 1) == 0) {
            this.kon3 = new Uwamono(28714, -1.615f, 0.0f, -14.94f, 150.0f);
            this.kon3.SetParticle(601);
            this.kon3.SetCallNo(3);
        }
        if (Runtime.getFlags(4008, 1) == 0) {
            this.kon4 = new Uwamono(28712, 4.5f, 0.0f, -15.94f, 150.0f);
            this.kon4.SetParticle(601);
            this.kon4.SetCallNo(4);
            this.EF06 = new Effect(1401, 4.185f, 0.3f, -15.94f, 0.0f);
            this.EF06.disp(true);
            this.EF06.setScale(0.5f, 1.25f, 0.5f);
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 375.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 6.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 0.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 4.5f, 45.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.doorA = new Uwamono(58, 42, '\u0001');
        new Uwamono(59, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(57, 42, '\u0001');
        new Uwamono(56, 42, '\u0001', this.doorB);
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade60 = new Effect(0);
        this.fade60.args[0] = -268435456;
        this.fade60.args[1] = 60;
        this.fade60.args[2] = 0;
        this.fade30 = new Effect(0);
        this.fade30.args[0] = -268435456;
        this.fade30.args[1] = 30;
        this.fade30.args[2] = 1;
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1616, 1, 0, 68, 5, -4.7776f, 0.0f, -9.694f, 90.0f);
        this.npc2 = new NPC_NORMAL(527, 2, 0, 2, 3, -3.75f, 0.0f, -25.3f, -90.0f);
        this.npc3 = new NPC_NORMAL(519, 3, 13, 5, 3, -2.26f, 0.0f, 19.08f, -90.0f);
        this.npc4 = new NPC_NORMAL(542, 4, 0, 13, 8, 2.56f, 0.0f, -26.45f, 110.0f);
        this.npc5 = new NPC_NORMAL(537, 5, 0, 14, 5, 3.18f, 0.0f, -27.75f, 340.0f);
        this.npc6 = new NPC_NORMAL(527, 6, 0, 14, 3, 4.66f, 0.0f, 16.46f, 160.0f);
        this.npc7 = new NPC_NORMAL(527, 7, 0, 14, 3, 1.99f, 0.0f, 5.32f, 90.0f);
        this.npc1.setMotion(0, 9);
        this.npc4.setMotion(0, 9);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc5.setMotion(0, 9);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc6.setMotion(0, 9);
        this.npc7.disableDTKFlag(3);
        this.npc7.setMotion(0, 11);
        this.npc7.enableDTKFlag(262144);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc9 = new NPC_NORMAL(527, 9, 0, 17, 5, -2.29f, 0.0f, -24.5f, 0.0f);
        this.npc10 = new NPC_NORMAL(1616, 10, 0, 17, 5, 2.288f, 0.0f, -11.201f, 195.0f);
        this.npc11 = new NPC_NORMAL(542, 11, 0, 17, 5, 2.39f, 0.0f, -24.41f, 300.0f);
        this.npc12 = new NPC_NORMAL(537, 12, 0, 17, 8, 3.29f, 0.0f, -24.35f, 315.0f);
        this.npc9.setVisible(false);
        this.npc10.setVisible(false);
        this.npc11.setVisible(false);
        this.npc12.setVisible(false);
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

