import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0510
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_ELS01_PRJ {
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
    Enepc npc20;
    Enepc npc21;
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
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2042;
    int S2043;
    int S2057;
    MAPUnit moni_00;
    MAPUnit doa20;
    MAPUnit doa21;
    Unit Hyper;
    Unit Star_1;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Unit curry;
    Effect EFcurry;
    int page;
    String[] msg0ED14DC9 = new String[]{"/[label(Tony)]", "Hey, you're a pretty good cook. How about it? Why don't you stay on this ship as the head chef?", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED14DCA = new String[]{"/[label(Shion)]", "Hmm...thanks for the offer, but I have all sorts of other work to do.", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED14DCB = new String[]{"/[label(Tony)]", "That's too bad.", "/[waitkey(1)]/[clear()]", "If you stay, I wouldn't have to put up with Hammer and chaos' bad leftovers anymore.", "/[waitkey(64)]/[close()]"};
    String[] msg0ED1AD77 = new String[]{"/[label(Tony)]", "By the way, you guys were on the warship that got wrecked, right? What the heck were you doing in that remote region anyway?", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED1AD78 = new String[]{"/[label(Shion)]", "Um, I believe they said they were investigating planetary extinction.", "/[waitkey(1)]/[clear()]", "But after we picked up some flotsam, the Gnosis attacked...and now we're here.", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED1AD79 = new String[]{"/[label(Tony)]", "Huh...? I don't get it at all.", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED1AD7A = new String[]{"/[label(Shion)]", "Even I don't know exactly what happened.", "/[waitkey(64)]/[close()]"};
    String[] msg0ED1AD7B = new String[]{"/[label(Tony)]", "Well, at least you're safe. Think about it this way, good things will happen eventually.", "/[waitkey(64)]/[close()]"};
    String[] msg0ED230E0 = new String[]{"/[label(Tony)]", "Say, have you ever played Cards?", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED230E1 = new String[]{"/[label(Shion)]", "Cards? As in playing with a deck of cards?", "/[waitkey(64)]/[clear()]"};
    String[] msg0ED230E2 = new String[]{"/[label(Tony)]", "No, no, no! It's no ordinary deck of cards. This is a really popular \"Card Game\" at the ports right now! It's always sold out, so it's hard to come by.", "/[waitkey(1)]/[clear()]", "How about it? I'll give you this Starter Set. So wanna play with me?", "/[waitkey(64)]/[close()]"};
    String[] msg00BA4128 = new String[]{"/[label(Tony)]", "What do you think of this ship? Impressive, isn't it?\n", "It's comparable to a military cruiser. And needless to say, ", "since I am the pilot of this ship, this is, without a doubt, ", "the most comfortable ship in all the Federation.", "/[waitkey(1)]/[clear()]", "But then again, it's close to being repossessed because of all the Captain's debts.", "/[waitkey(64)]/[close()]"};
    String[] msg00BAA0D4 = new String[]{"/[label(Tony)]", "By the way, wanna see my card collection? I don't mind showing you, but only if you beat me at the game. How about it? Wanna give it a try?", "/[waitkey(64)]/[close()]"};
    String[] msg0102DABF = new String[]{"/[label(Matthews)]", "The PA announcement?", "/[waitkey(1)]/[clear()]", "Oh, I just wanted to ask you to help us prepare for the gate jump. Great timing! Can you go check the catapult?", "/[waitkey(64)]/[close()]"};
    String[] msg0102DABF1 = new String[]{"/[label(Matthews)]", "Hey, Ms. Vector, you're still here? The console is right behind the catapult. Hurry up and check it out.", "/[waitkey(1)]/[clear()]", "Make it quick, okay? We'll be in the column area soon.", "/[waitkey(64)]/[close()]"};
    String[] msg04179A93 = new String[]{"/[label(Hammer)]", "Hmm, I think it'll probably be okay. The catapult on this ship is jerry-rigged.", "/[waitkey(1)]/[clear()]", "Well, I can't really get into what exactly is jerry-rigged, since it's the ship's greatest secret.", "/[waitkey(64)]/[clear()]"};
    String[] msg04179A94 = new String[]{"/[label(Shion)]", "Does that mean you have equipment that violates Federation laws?", "/[waitkey(64)]/[clear()]"};
    String[] msg04179A95 = new String[]{"/[label(Hammer)]", "Huh? Uh...looking at me like that won't work. I can't tell you any more than that!", "/[waitkey(64)]/[close()]"};
    String[] msg042FFE2F = new String[]{"/[label(Hammer)]", "Nope. This is just one of those things I just can't tell you. Gimme a break, okay?", "/[waitkey(64)]/[close()]"};
    String[] msgtonnyhelp1 = new String[]{"/[label(Matthews)]", "What? The /[color(0x329bbe)]Hazardous Area Map/[color(0x808080)]? What do you need that for?", "/[waitkey(1)]/[clear()]", "Tony's looking for it? What the hell is he doing? It's right here, of course!", "/[waitkey(64)]/[clear()]"};
    String[] msgtonnyhelp2 = new String[]{"/[label(Shion)]", "Huh?", "/[waitkey(64)]/[clear()]"};
    String[] msgtonnyhelp3 = new String[]{"/[label(Matthews)]", "I'm saying that I have it! See? This is it, right?", "/[waitkey(1)]/[clear()]", "Tell that moron to quit goofing off and get back here right now!", "/[waitkey(64)]/[close()]"};
    String[] msg0202DABF = new String[]{"/[label(Matthews)]", "There's a huge reading coming from Hangar 2. It might be an enemy boss. Be careful.", "/[waitkey(1)]/[clear()]", "And don't you wreck this ship! If you damage it, I'm gonna bill you for the repairs!!", "/[waitkey(64)]/[close()]"};
    String[] msg0202DABF1 = new String[]{"/[label(Matthews)]", "What are you doing? Hurry up and go fight them!", "/[waitkey(64)]/[close()]"};
    String[] msg042F9E82 = new String[]{"/[label(Hammer)]", "It's finally time for KOS-MOS to kick some butt, isn't it? I'm really looking forward to it! Go get 'em!", "/[waitkey(64)]/[close()]"};
    String[] msg01BA4128 = new String[]{"/[label(Tony)]", "The enemy's coming, right?", "/[waitkey(1)]/[clear()]", "Well? Aren't you gonna take that chick with you and take care of them?!", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D16 = new String[]{"/[label(Allen)]", "Chief, please don't do anything reckless.", "/[waitkey(1)]/[clear()]", "We're taking KOS-MOS into battle without permission from HQ. If something were to happen, we'd all be fired! Fired!", "/[waitkey(64)]/[close()]"};
    String[] msg00B81D161 = new String[]{"/[label(Allen)]", "Well, it wouldn't be so bad if getting fired was the worst of it. I don't care about that. But if something were to happen to you, Chief, I'd...", "/[waitkey(64)]/[close()]"};
    String[] msg0302DABF = new String[]{"/[label(Matthews)]", "Quit goofing off! Hurry up and go attack them!!", "/[waitkey(1)]/[clear()]", "Do you want them to take over the entire ship?!", "/[waitkey(64)]/[close()]"};
    String[] msg142F9E82 = new String[]{"/[label(Hammer)]", "Hmm? What's wrong? Forget something?", "/[waitkey(1)]/[clear()]", "Oh, are you scared?!", "/[waitkey(64)]/[close()]"};
    String[] msg02BA4128 = new String[]{"/[label(Tony)]", "Hey, we don't have time for games. The enemy is right there. Hurry up and show us what that weapon can do!", "/[waitkey(64)]/[close()]"};
    String[] msg0436E835 = new String[]{"/[label(Shion)]", "Let's see, this is the bridge.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E836 = new String[]{"/[label(Shion)]", "The frontmost part is the pilot console, and that's Tony's seat.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E837 = new String[]{"/[label(Shion)]", "The left front is the navigator seat. That's where Hammer sits.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E838 = new String[]{"/[label()]", "The right front is the engineer's control seat. The rear is the fire control seat.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E839 = new String[]{"/[label(Shion)]", "And the seat attached to the arm protruding from the left rear area is the Captain's seat. That's where Captain Matthews sits.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E83A = new String[]{"/[label(Shion)]", "I heard that all the controls can be automated for routine voyages.", "/[waitkey(64)]/[close()]"};
    String[] msg0402DABF = new String[]{"/[label(Matthews)]", "Hey, I don't want any trouble, so hurry up and go find him.", "/[waitkey(1)]/[clear()]", "Sheesh, going out dressed like a Marine...what is he, crazy?", "/[waitkey(64)]/[close()]"};
    String[] msg0402DABF1 = new String[]{"/[label(Matthews)]", "Huh? If you want to know, ask these guys. It's so pathetic, I can't bring myself to say it, especially as a former Marine.", "/[waitkey(64)]/[close()]"};
    String[] msg03BA4128 = new String[]{"/[label(Tony)]", "You have any idea of where the Commander might go? From the look on your face, it doesn't seem like you do.", "/[waitkey(1)]/[clear()]", "He's gonna stick out like a sore thumb in that outfit, so you shouldn't have any trouble finding him.", "/[waitkey(64)]/[close()]"};
    String[] msgziggy1 = new String[]{"/[label(Tony)]", "If you're looking for Ziggy, he was wandering around Cabin 1 just now. I think he was looking for something.", "/[waitkey(64)]/[close()]"};
    String[] msg05000001 = new String[]{"/[label(Matthews)]", "What do you want to do? Want to depart soon?\n", "Or do you want to wander around the colony still?", "/[waitkey(64)]/[close()]"};
    String[] msgikude = new String[]{"/[label(Matthews)]", "All right, Tony! We're leaving port, heading for the Miltian star system! Hurry up and get us released from the dock.", "/[waitkey(64)]/[close()]"};
    String[] msghayosei = new String[]{"/[label(Matthews)]", "What, you still gonna shop or something?", "/[waitkey(1)]/[clear()]", "Stop causing problems! If you don't give it a rest, I'm gonna leave you here!", "/[waitkey(64)]/[close()]"};
    String[] msg05000002 = new String[]{"/[label(Tony)]", "What? Forget to buy something?", "/[waitkey(1)]/[clear()]", "Hurry up, okay? The Captain looks like he's ready to go on a rampage at anytime.", "/[waitkey(64)]/[close()]"};
    String[] msg0502DABF = new String[]{"/[label(Matthews)]", "Yo, glad you're safe.", "/[waitkey(1)]/[clear()]", "Hmm? Where's the Commander?", "/[waitkey(64)]/[clear()]"};
    String[] msg0102DAC0 = new String[]{"/[label(Shion)]", "The thing is...", "/[waitkey(64)]/[clear()]"};
    String[] msg0102DAC2 = new String[]{"/[label(Matthews)]", "I see, don't let it bother you. Some guys just aren't lucky. And they say, you never know if it might be you tomorrow. Come on, no point in staying here any longer. Let's get out of here.", "/[waitkey(64)]/[close()]"};
    String[] msg04BA4128 = new String[]{"/[label(Tony)]", "Damn, how the heck did you survive being swallowed up by a Gnosis? You must have Lady Luck on your side or something.", "/[waitkey(64)]/[close()]"};
    String[] msg242F9E82 = new String[]{"/[label(Hammer)]", "I'm so glad! I thought I'd never see you again! When you suddenly disappeared, I didn't know what to do.\n", "/[waitkey(1)]/[clear()]", "Oh, I never thought of abandoning you and running. Honest!", "/[waitkey(64)]/[close()]"};
    String[] msg021C5348 = new String[]{"/[label(chaos)]", "Sorry, there's a lot going on.", "/[waitkey(64)]/[clear()]"};
    String[] msg021C5349 = new String[]{"/[label(Shion)]", "Oh, don't worry about me! I'm a mere guest here. This is nothing.", "/[waitkey(64)]/[clear()]"};
    String[] msg021C534A = new String[]{"/[label(chaos)]", "The Captain and Tony really abuse the catapult, so it's probably falling apart.", "/[waitkey(64)]/[close()]"};
    String[] msg011CB2F7 = new String[]{"/[label(chaos)]", "Want some company?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F8 = new String[]{"/[label(Shion)]", "No, I'll be fine. I may not look it, but I did develop KOS-MOS after all! A problem or two on a passenger-cargo ship should be a piece of cake, you know?", "/[waitkey(64)]/[clear()]"};
    String[] msg011CB2F9 = new String[]{"/[label(chaos)]", "Well, no worries then.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0510() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.061f, 3.277f, -11.353f);
        this.camEV.setRotate(-13.158f, -181.279f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.06f, 1.197f, 2.971f);
        this.camEV.setRotate(-1.488f, 208.458f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.838f, -0.394f, 1.255f);
        this.camEV.setRotate(21.417f, 106.175f, 0.0f);
        this.camEV.setFov(22.72f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 1: {
                if (Runtime.getFlags(158, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(159, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7149, 1) != 0) break;
                Runtime.mailArriveSet(41);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                ST0510.waitPage(this.win, 64);
                System.sleep(15);
                Runtime.setFlags(7149, 1, 1);
                Runtime.mailExec(1);
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0510.waitPage(window, 64);
    }

    public void Talk_npc1(Enepc enepc) {
        if (this.S2057 == 1) {
            this.Talk_npc1_9();
        } else if (this.S2042 == 1) {
            this.Talk_npc1_8();
        } else if (this.S2040C == 1) {
            this.Talk_npc1_7();
        } else if (this.S2030 == 1) {
            this.Talk_npc1_6();
        } else if (this.MOMO == 1) {
            this.Talk_npc1_5();
        } else if (this.S2028 == 1) {
            this.Talk_npc1_4();
        } else if (this.S2014 == 1) {
            this.Talk_npc1_3();
        } else if (this.S2013 == 1) {
            this.Talk_npc1_2();
        } else {
            this.Talk_npc1_1();
        }
    }

    void Talk_npc1_1() {
    }

    void Talk_npc1_2() {
    }

    void Talk_npc1_3() {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                if (Runtime.getFlags(7005, 1) == 1) {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msg0102DABF, 0);
                    ST0510.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgtonnyhelp1, 0);
                    ST0510.waitPage(this.win, 64);
                    this.win.print(this.msgtonnyhelp2, 0);
                    ST0510.waitPage(this.win, 64);
                    this.win.print(this.msgtonnyhelp3, 0);
                    ST0510.waitPage(this.win, 64);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 9);
                    Runtime.setFlags(7005, 1, 0);
                    return;
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0102DABF, 0);
                ST0510.waitPage(this.win, 64);
                return;
            }
        }
        if (Runtime.getFlags(7005, 1) == 1) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg0102DABF1, 0);
            ST0510.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgtonnyhelp1, 0);
            ST0510.waitPage(this.win, 64);
            this.win.print(this.msgtonnyhelp2, 0);
            ST0510.waitPage(this.win, 64);
            this.win.print(this.msgtonnyhelp3, 0);
            ST0510.waitPage(this.win, 64);
            Sound.effectPlay(6);
            Runtime.addItemWin(10, 9);
            Runtime.setFlags(7005, 1, 0);
            return;
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0102DABF1, 0);
        ST0510.waitPage(this.win, 64);
    }

    void Talk_npc1_4() {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                this.npc1.kickEnepc(9, 100);
                this.player.look_char(this.npc1);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0202DABF, 0);
                ST0510.waitPage(this.win, 64);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                this.player.look_default();
                return;
            }
        }
        this.npc1.kickEnepc(9, 100);
        this.player.look_char(this.npc1);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0202DABF1, 0);
        ST0510.waitPage(this.win, 64);
        this.cam0.setMode(0);
        this.npc1.kickEnepc(9, -1);
        this.player.look_default();
    }

    void Talk_npc1_5() {
        this.npc1.kickEnepc(9, 100);
        this.player.look_char(this.npc1);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0302DABF, 0);
        ST0510.waitPage(this.win, 64);
        this.cam0.setMode(0);
        this.npc1.kickEnepc(9, -1);
        this.player.look_default();
    }

    void Talk_npc1_6() {
    }

    void Talk_npc1_7() {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                this.npc1.kickEnepc(9, 100);
                this.player.look_char(this.npc1);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0402DABF, 0);
                ST0510.waitPage(this.win, 64);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                this.player.look_default();
                return;
            }
        }
        this.npc1.kickEnepc(9, 100);
        this.player.look_char(this.npc1);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0402DABF1, 0);
        ST0510.waitPage(this.win, 64);
        this.cam0.setMode(0);
        this.npc1.kickEnepc(9, -1);
        this.player.look_default();
    }

    void Talk_npc1_8() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg05000001, 0);
        ST0510.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, please depart\nI'm sorry but I'm not ready yet");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST0510.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(33);
                Runtime.setFlags(164, 1, 1);
                Runtime.jumpEvent(2430);
                System.println("フラグオン！");
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST0510.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_9() {
        this.npc1.kickEnepc(9, 100);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0502DABF, 0);
        ST0510.waitPage(this.win, 64);
        this.win.print(this.msg0102DAC0, 0);
        ST0510.waitPage(this.win, 64);
        this.win.print(this.msg0102DAC2, 0);
        ST0510.waitPage(this.win, 64);
        Runtime.setFlags(180, 1, 1);
        Runtime.jumpEvent(2580);
        System.println("フラグオン！");
        this.cam0.setMode(0);
        this.npc1.kickEnepc(9, -1);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc2_9(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc2_8(window);
        } else if (this.ZIGGY == 1) {
            this.Talk_npc2_71(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc2_7(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc2_6(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc2_5(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc2_4(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc2_3(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg0ED14DC9, 0);
                ST0510.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg0ED14DCA, 0);
                ST0510.waitPage(window, 64);
                window.print(this.msg0ED14DCB, 0);
                ST0510.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                window.print(this.msg0ED1AD77, 0);
                ST0510.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg0ED1AD78, 0);
                ST0510.waitPage(window, 64);
                window.print(this.msg0ED1AD79, 0);
                ST0510.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg0ED1AD7A, 0);
                ST0510.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg0ED1AD7B, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg00BA4128, 0);
                ST0510.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00BA4128, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
    }

    void Talk_npc2_4(Window window) {
        window.print(this.msg01BA4128, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_5(Window window) {
        window.print(this.msg02BA4128, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_6(Window window) {
    }

    void Talk_npc2_7(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msgziggy1, 0);
                ST0510.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg03BA4128, 0);
        ST0510.waitPage(window, 64);
        --this.talkFlag2;
        --this.talkFlag2;
    }

    void Talk_npc2_71(Window window) {
        window.print(this.msg03BA4128, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_8(Window window) {
        window.print(this.msg05000002, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc2_9(Window window) {
        window.print(this.msg04BA4128, 0);
        ST0510.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc3_9(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc3_8(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc3_7(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc3_6(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc3_5(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc3_4(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
    }

    void Talk_npc3_2(Window window) {
    }

    void Talk_npc3_3(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg04179A93, 0);
                ST0510.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg04179A94, 0);
                ST0510.waitPage(window, 64);
                window.print(this.msg04179A95, 0);
                ST0510.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg042FFE2F, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc3_4(Window window) {
        window.print(this.msg042F9E82, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc3_5(Window window) {
        window.print(this.msg142F9E82, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc3_6(Window window) {
    }

    void Talk_npc3_7(Window window) {
    }

    void Talk_npc3_8(Window window) {
    }

    void Talk_npc3_9(Window window) {
        window.print(this.msg242F9E82, 0);
        ST0510.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S2014 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg021C5348, 0);
                ST0510.waitPage(window, 64);
                window.print(this.msg021C5349, 0);
                ST0510.waitPage(window, 64);
                window.print(this.msg021C534A, 0);
                ST0510.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg011CB2F7, 0);
        ST0510.waitPage(window, 64);
        window.print(this.msg011CB2F8, 0);
        ST0510.waitPage(window, 64);
        window.print(this.msg011CB2F9, 0);
        ST0510.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_npc5_9(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc5_8(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc5_7(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc5_6(window);
        } else if (this.MOMO == 1) {
            this.Talk_npc5_5(window);
        } else if (this.S2028 == 1) {
            this.Talk_npc5_4(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S2013 == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
    }

    void Talk_npc5_2(Window window) {
    }

    void Talk_npc5_3(Window window) {
    }

    void Talk_npc5_4(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg00B81D16, 0);
                ST0510.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00B81D161, 0);
        ST0510.waitPage(window, 64);
    }

    void Talk_npc5_5(Window window) {
    }

    void Talk_npc5_6(Window window) {
    }

    void Talk_npc5_7(Window window) {
    }

    void Talk_npc5_8(Window window) {
    }

    void Talk_npc5_9(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S2030 == 1) {
                    Runtime.jumpCF(520, 1);
                    break;
                }
                if (this.S2028 == 1) {
                    Runtime.jumpCF(670, 1);
                    break;
                }
                Runtime.jumpCF(520, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(123, 1) == 0) {
            this.curry = new Obj();
            this.curry.init(24640, 0.0f, 0.0f, 0.0f, 0.0f);
            this.curry.start(4, null);
            this.curry.setTranslate(0.15f, 0.1f, -0.068f);
            this.curry.setRotate(260.0f, 90.0f, 0.0f);
            this.curry.setParent(this.player, 60);
            this.EFcurry = new Effect(1531, 0.0f, 0.0f, 0.0f, 0.0f);
            this.EFcurry.disp(true);
            this.EFcurry.setCaster(this.curry);
            this.EFcurry.noAttach(false);
        }
        this.S2009 = Runtime.getFlags(115, 1);
        this.S2013 = Runtime.getFlags(122, 1);
        this.S2013B = Runtime.getFlags(123, 1);
        this.S2014 = Runtime.getFlags(124, 1);
        this.S2014B = Runtime.getFlags(125, 1);
        this.S2028 = Runtime.getFlags(141, 1);
        this.MOMO = Runtime.getFlags(7014, 1);
        this.S2030 = Runtime.getFlags(143, 1);
        this.S2040C = Runtime.getFlags(158, 1);
        this.ZIGGY = Runtime.getFlags(7015, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S2057 = Runtime.getFlags(179, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, -1.35f, -2.6f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, 0.0f, -0.5f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -3.3f, -0.5f, 1.2f, 0.0f);
        this.teiten3.SetBgm(196610);
        this.teiten4 = new Uwamono(28690, 3.3f, -0.5f, 1.2f, 0.0f);
        this.teiten4.SetBgm(196610);
        this.teiten5 = new Uwamono(28690, 3.0f, 0.0f, 6.0f, 0.0f);
        this.teiten5.SetBgm(196610);
        this.teiten6 = new Uwamono(28690, -3.0f, 0.0f, 7.0f, 0.0f);
        this.teiten6.SetBgm(196611);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorA = new Uwamono(4, 40, '\u0001');
        new Uwamono(5, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(158, 1) == 1) {
            this.moni_00 = new Mapunit();
            this.moni_00.init(65);
            this.moni_00.start(4, null);
            this.moni_00.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa20 = new Mapunit();
            this.doa20.init(66);
            this.doa20.start(4, null);
            this.doa20.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa21 = new Mapunit();
            this.doa21.init(67);
            this.doa21.start(4, null);
            this.doa21.setTranslate(0.0f, -300.0f, 0.0f);
        } else if (Runtime.getFlags(143, 1) == 1) {
            this.Star_1 = new Mapunit();
            this.Star_1.mapUnit(68);
            this.Star_1.start(4, null);
            this.Star_1.setTranslate(this.Star_1.px, this.Star_1.py + 300.0f, this.Star_1.pz);
        } else if (Runtime.getFlags(141, 1) == 1) {
            this.Hyper = new Unit();
            this.Hyper.init(20490, 0.0f, 0.0f, 0.0f, 0.0f);
            this.Hyper.setScale(1.0f, 1.0f, 2.0f);
            Stage.setVisible(68, false);
            Stage.setVisible(6, false);
        } else {
            this.Star_1 = new Mapunit();
            this.Star_1.mapUnit(68);
            this.Star_1.start(4, null);
            this.Star_1.setTranslate(this.Star_1.px, this.Star_1.py + 300.0f, this.Star_1.pz);
        }
        if (Runtime.getFlags(158, 1) == 1 && Runtime.getFlags(7053, 1) == 0) {
            Runtime.setOutFriend(5);
            Runtime.setOutFriend(288);
            Runtime.setOutFriend(289);
            Runtime.resetOutFriend(1);
            Runtime.resetOutFriend(3);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 1);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
            Runtime.setTakeAgws(8193);
            Runtime.setTakeAgws(8450);
            Runtime.resetTakeAgws(8194);
            Runtime.setFlags(7053, 1, 1);
        }
        if (this.S2057 == 1) {
            this.npcset_9();
        } else if (this.S2042 == 1) {
            this.npcset_8();
        } else if (this.S2040C == 1) {
            this.npcset_7();
        } else if (this.S2030 == 1) {
            this.npcset_6();
        } else if (this.MOMO == 1) {
            this.npcset_5();
        } else if (this.S2028 == 1) {
            this.npcset_4();
        } else if (this.S2014B == 1) {
            this.npcset_0();
        } else if (this.S2014 == 1) {
            this.npcset_3();
        } else if (this.S2013B == 1) {
            this.npcset_0();
        } else if (this.S2013 == 1) {
            this.npcset_2();
        } else if (this.S2009 == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc2.talkto("Talk_npc2");
    }

    void npcset_2() {
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc2.talkto("Talk_npc2");
    }

    void npcset_3() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 3, -1.9f, -0.5f, 1.04f, 250.0f);
        this.npc3 = new NPC_NORMAL(278, 3, 0, 76, 17, -2.89f, -0.3f, 1.48f, 180.0f);
        this.npc4 = new NPC_NORMAL(3, 4, 0, 76, 17, 2.94f, -0.3f, 1.48f, 180.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 10);
        this.npc1.setInvalidID(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setInvalidID(1);
        this.npc3.setMotion(0, 4);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setInvalidID(1);
        this.npc4.setMotion(0, 4);
        this.npc1.talkto("Talk_npc1");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
    }

    void npcset_4() {
        if (Runtime.getFlags(3091, 1) == 0) {
            Runtime.setFriend(3);
            Runtime.resetOutFriend(2);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 2);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 1);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            System.println("パーティー情報・leader_shion,kosmos,chaos,*,*,*");
            Runtime.setFlags(3091, 1, 1);
        }
        Runtime.setFlags(141, 1, 1);
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 16, -3.0f, 1.8f, 2.85f, 180.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc3 = new NPC_NORMAL(278, 3, 0, 76, 17, -2.89f, -0.3f, 1.48f, 180.0f);
        this.npc5 = new NPC_NORMAL(263, 3, 0, 13, 9, -0.98f, 0.0f, 2.04f, 0.0f);
        this.npc20 = new NPC_NORMAL(275, 20, 0, 14, 3, -2.95f, -0.5f, 2.75f, 90.0f);
        this.npc21 = new NPC_NORMAL(275, 21, 0, 14, 3, -2.95f, -0.5f, 3.75f, 90.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 6);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setInvalidID(1);
        this.npc3.setMotion(0, 4);
        this.npc5.setMotion(0, 10);
        this.npc20.enableDTKFlag(393216);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc21.enableDTKFlag(393216);
        this.npc21.setInvalidID(1);
        this.npc21.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc20.talkto("Talk_npc1");
        this.npc21.talkto("Talk_npc1");
    }

    void npcset_5() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 16, -3.0f, 1.8f, 2.85f, 180.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc3 = new NPC_NORMAL(278, 3, 0, 2, 7, -1.79f, 0.0f, 2.02f, 0.0f);
        this.npc20 = new NPC_NORMAL(275, 20, 0, 14, 3, -2.95f, -0.5f, 2.75f, 90.0f);
        this.npc21 = new NPC_NORMAL(275, 21, 0, 14, 3, -2.95f, -0.5f, 3.75f, 90.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 6);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc20.enableDTKFlag(393216);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc21.enableDTKFlag(393216);
        this.npc21.setInvalidID(1);
        this.npc21.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc20.talkto("Talk_npc1");
        this.npc21.talkto("Talk_npc1");
    }

    void npcset_6() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 11, -0.65f, 0.0f, 7.9f, 180.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 13, 0.75f, 0.0f, 8.3f, 180.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 15, -0.04f, 0.0f, 7.41f, 180.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.kickEnepc(4, 1);
        this.npc6.disableDTKFlag(131072);
        this.npc6.disableDTKFlag(65536);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc7.disableDTKFlag(131072);
        this.npc7.disableDTKFlag(65536);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        if (Runtime.getFlags(7019, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 15, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.setInvalidID(1);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_7() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 16, -3.0f, 1.8f, 2.85f, 180.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc20 = new NPC_NORMAL(275, 20, 0, 14, 3, -2.95f, -0.5f, 2.75f, 90.0f);
        this.npc21 = new NPC_NORMAL(275, 21, 0, 14, 3, -2.95f, -0.5f, 3.75f, 90.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 6);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc20.enableDTKFlag(393216);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc21.enableDTKFlag(393216);
        this.npc21.setInvalidID(1);
        this.npc21.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc20.talkto("Talk_npc1");
        this.npc21.talkto("Talk_npc1");
    }

    void npcset_8() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 3, -2.02f, -0.5f, 1.66f, 180.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 10);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
    }

    void npcset_9() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 3, -0.86f, 0.0f, 6.0f, 90.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc3 = new NPC_NORMAL(278, 3, 0, 13, 7, -1.79f, 0.0f, 2.02f, 0.0f);
        this.npc20 = new NPC_NORMAL(275, 20, 0, 14, 3, -2.95f, -0.5f, 2.75f, 90.0f);
        this.npc21 = new NPC_NORMAL(275, 21, 0, 14, 3, -2.95f, -0.5f, 3.75f, 90.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 6);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc20.enableDTKFlag(393216);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc21.enableDTKFlag(393216);
        this.npc21.setInvalidID(1);
        this.npc21.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc20.talkto("Talk_npc1");
        this.npc21.talkto("Talk_npc1");
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

    class Mapunit
            extends MAPUnit {
        Mapunit() {
        }
    }

    class Object
            extends Unit {
        Object() {
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

        void ANNAI() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7019, 1, 1);
            Runtime.enable(65536);
            ST0510.this.cam0.setMode(-1);
            ST0510.this.EV_Camera01();
            ST0510.this.npc10.kickEnepc(0, 9);
            ST0510.this.npc6.kickEnepc(1, 10);
            ST0510.this.npc7.kickEnepc(0, 10);
            ST0510.this.npc6.setVisible(true);
            ST0510.this.npc7.setVisible(true);
            ST0510.this.npc10.setVisible(true);
            System.sleep(10);
            ST0510.this.win = Window.create();
            ST0510.this.win.setSize(4, 45);
            ST0510.this.win.setLocation(15, 305);
            ST0510.this.win.print(ST0510.this.msg0436E835, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.EV_Camera02();
            System.sleep(10);
            ST0510.this.npc10.kickEnepc(1, 9);
            ST0510.this.win.print(ST0510.this.msg0436E836, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.npc10.moveEnepc(17, 210.0f, 0.1f, 10);
            ST0510.this.npc7.kickEnepc(0, 10);
            ST0510.this.npc7.moveEnepc(17, 160.0f, -0.1f, 30);
            ST0510.this.npc6.kickEnepc(0, 7);
            ST0510.this.win.print(ST0510.this.msg0436E837, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.npc10.moveEnepc(17, 120.0f, -0.1f, 20);
            ST0510.this.npc10.kickEnepc(1, 10);
            ST0510.this.win.print(ST0510.this.msg0436E838, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.npc10.moveEnepc(17, 270.0f, 0.1f, 20);
            ST0510.this.npc7.kickEnepc(0, 10);
            ST0510.this.npc7.moveEnepc(17, 200.0f, 0.1f, 30);
            ST0510.this.npc6.kickEnepc(1, 9);
            ST0510.this.win.print(ST0510.this.msg0436E839, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.npc10.kickEnepc(0, 9);
            ST0510.this.win.print(ST0510.this.msg0436E83A, 0);
            ST0510.waitPage(ST0510.this.win, 64);
            ST0510.this.npc7.moveEnepc(17, 180.0f, -0.1f, 20);
            System.sleep(10);
            ST0510.this.fade.call(0);
            System.sleep(30);
            ST0510.this.npc6.setVisible(false);
            ST0510.this.npc7.setVisible(false);
            ST0510.this.npc10.setVisible(false);
            ST0510.this.npc11.setVisible(false);
            ST0510.this.player.setTranslate(-0.02f, 0.0f, 7.31f);
            ST0510.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

