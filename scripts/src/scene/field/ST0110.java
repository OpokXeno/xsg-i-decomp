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
import xeno.map.MC_VOK11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0110
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK11_PRJ {
    int entrance;
    Player player;
    Camera cam1;
    Camera camEV;
    Menu menu;
    Window win;
    Unit sphere1;
    Unit sphere2;
    Unit ring_1a;
    Unit ring_1b;
    Unit ring_1c;
    Unit ring_2a;
    Unit ring_2b;
    Unit ring_2c;
    Unit sonar_ring_0;
    Unit sonar_ring_1;
    Unit sonar_ring_2;
    Unit sonar_ring_3;
    Unit Star_1;
    Unit Star_2;
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
    int talkFlag14;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int S014A;
    int S014B;
    int S015B;
    int SHION_TALK_3;
    Unit elv;
    int epass = 1;
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
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    Uwamono teiten11;
    Uwamono teiten12;
    Uwamono teiten13;
    Uwamono teiten14;
    int page;
    String[] msgMORIYAMA_TALK1 = new String[]{"/[label(Shion)]", "Please excuse me for being late.", "/[waitkey(64)]/[clear()]"};
    String[] msgMORIYAMA_TALK2 = new String[]{"/[label(Moriyama)]", "Don't worry about it.", "/[waitkey(1)]/[clear()]", "Now that you're here, can you please show us KOS-MOS' data?", "/[waitkey(64)]/[clear()]"};
    String[] msgMORIYAMA_TALK3 = new String[]{"/[label(Shion)]", "Yes, Sir. Right away.", "/[waitkey(64)]/[close()]"};
    String[] msgANDREW_TALK1 = new String[]{"/[label(Cherenkov)]", "You're 10 minutes late. It's no wonder that your research is behind schedule.", "/[waitkey(64)]/[clear()]"};
    String[] msgANDREW_TALK2 = new String[]{"/[label(Shion)]", "I'm sorry.", "/[waitkey(64)]/[clear()]"};
    String[] msgANDREW_TALK3 = new String[]{"/[label(Cherenkov)]", "Instead of taking up more time with your apologies, you should hurry up and present your report to the Captain.", "/[waitkey(64)]/[close()]"};
    String[] msg378BCB0A = new String[]{"/[label()]", "Oh, umm...it was K-KOS-MOS, right? The battle android?", "/[waitkey(1)]/[clear()]", "I-I hope it does well...", "/[waitkey(64)]/[close()]"};
    String[] msg378C2AB7 = new String[]{"/[label()]", "What? Umm...I'm sorry. Android you said? It's an android that looks like a human? I-I've only read about them in classical literature. Why are we building something like that now?", "/[waitkey(64)]/[close()]"};
    String[] msg39C93009 = new String[]{"/[label()]", "Oh, you're from Vector aren't you? How are you enjoying the Woglinde so far? She's a great ship isn't she?", "/[waitkey(64)]/[close()]"};
    String[] msg39C98FB5 = new String[]{"/[label()]", "You know...you'd be better off working on something more practical, like naval engineering.", "/[waitkey(64)]/[close()]"};
    String[] msg36EE42D3 = new String[]{"/[label()]", "It sure must be nice...you guys get to conduct your research while getting red carpet treatment on a military ship. On top of that, you'll get credit for\nactive duty as well.", "/[waitkey(64)]/[close()]"};
    String[] msg36EEA280 = new String[]{"/[label()]", "By the way, we aren't counting on some prototype that was built as a hobby. Leave it to the pros if the enemy shows up.", "/[waitkey(64)]/[close()]"};
    String[] msg3708C222 = new String[]{"/[label()]", "Those floating objects are console pods built exclusively for the new 100-Series Observational Unit Realians.", "/[waitkey(1)]/[clear()]", "Unfortunately, the 100-Series Observational Units haven't been issued yet. It'd be helpful if they were, but we're just fine on our own for now.", "/[waitkey(64)]/[close()]"};
    String[] msg380A808B = new String[]{"/[label()]", "The officer on the starboard side is our Chief Navigator, a veteran of the Miltian Conflict.", "/[waitkey(64)]/[close()]"};
    String[] msg380AE038 = new String[]{"/[label()]", "Terms like starboard side don't really apply when you're in the middle of a star-filled galaxy, but apparently, it's customarily been called that since back in the time when ships used to float in water.", "/[waitkey(64)]/[close()]"};
    String[] msg36E7AD52 = new String[]{"/[label()]", "Did you talk to the person sitting across from me?", "/[waitkey(64)]/[clear()]"};
    String[] msg36E7AD54 = new String[]{"/[label()]", "I'm so sorry! The Chief Engineer is a nice person and very talented. But I wish he wouldn't crack those stupid jokes.", "/[waitkey(64)]/[close()]"};
    String[] msg36E7AD56 = new String[]{"/[label()]", "It might be better not to talk to him. He'll drain you of all your energy.", "/[waitkey(64)]/[close()]"};
    String[] msg369FE9B4 = new String[]{"/[label()]", "Oh, hi there. You're quite young for a Chief Engineer.", "/[waitkey(64)]/[close()]"};
    String[] msg36A04961 = new String[]{"/[label()]", "They sure do things differently in large corporations like Vector.", "/[waitkey(1)]/[clear()]", "For such a young person to be given the opportunity to develop an android weapon...Private companies sure have a lot of money to throw around.", "/[waitkey(64)]/[close()]"};
    String[] msg369D49C1 = new String[]{"/[label()]", "I'm sorry. I hope you'll forgive their rude remarks. Everyone's feeling a bit uneasy about the information blackout surrounding this mission.", "/[waitkey(64)]/[close()]"};
    String[] msg369DA96D = new String[]{"/[label()]", "It's just that your department is an easy target for people to vent their frustrations against.", "/[waitkey(1)]/[clear()]", "As military personnel, they really ought to maintain their decorum regardless of their personal concerns over the mission.", "/[waitkey(64)]/[close()]"};
    String[] msg371C0303 = new String[]{"/[label()]", "Looks like we came in contact with something in hyperspace. The outer coating on the port side was damaged.", "/[waitkey(1)]/[clear()]", "Hmm, I wonder if it'll hold out? I suppose we can call in a picket or AWACS for escort and support if things get hairy.", "/[waitkey(64)]/[close()]"};
    String[] msg371C62AF = new String[]{"/[label()]", "I hear that Gnosis are extremely mobile. I'm not so certain that this ship's FCS is even capable of pursuing them...", "/[waitkey(64)]/[close()]"};
    String[] msg362E6A78 = new String[]{"/[label()]", "T-this isn't good! It looks like they used low-grade coal and the exhaust is too thick -- at this rate, they'll see us coming from beyond the horizon.", "/[waitkey(1)]/[clear()]", "Ha ha ha, just kidding. The main engine on this ship is the Zuisei Model 2, a cutting-edge logical drive. It doesn't use coal!", "/[waitkey(64)]/[close()]"};
    String[] msg362ECA26 = new String[]{"/[label()]", "Shoot, we're low on hypergol. Hey, you got any good oxidizer?", "/[waitkey(1)]/[clear()]", "I'm just kidding. This ship doesn't use chemical propulsion either, it's completely free from the limitations of Newtonian dynamics.", "/[waitkey(64)]/[close()]"};
    String[] msg389D01D2 = new String[]{"/[label()]", "A modern warship cannot exist without a network. A fleet...no, the military itself is like a giant living entity.", "/[waitkey(1)]/[clear()]", "A mere link with this ship can mobilize several thousand troops. And that's just through one synapse.", "/[waitkey(64)]/[close()]"};
    String[] msg389D617E = new String[]{"/[label()]", "Our Communications Department is quite knowledgable in the use of hardware, and the entire staff is fluent in several languages.", "/[waitkey(1)]/[clear()]", "The work itself though, is not very fulfilling at times.", "/[waitkey(64)]/[close()]"};
    String[] msg39A1D84E = new String[]{"/[label(Moriyama)]", "Good work. Now go and get some rest.", "/[waitkey(64)]/[close()]"};
    String[] msg39A237FA = new String[]{"/[label(Moriyama)]", "Don't worry about the Commander. He's one of the best, but he's a bit on the anxious side.", "/[waitkey(1)]/[clear()]", "He tends to take out his worries on others.", "/[waitkey(64)]/[close()]"};
    String[] msg39A1D84E1 = new String[]{"/[label(Moriyama)]", "What's the matter? You've been pushing yourself too hard lately, haven't you?", "/[waitkey(1)]/[clear()]", "Go back to your quarters and get some rest.", "/[waitkey(64)]/[close()]"};
    String[] msg389D617E1 = new String[]{"/[label()]", "No need to worry. Just one more gate jump and this operation will pretty much be over.", "/[waitkey(1)]/[clear()]", "We won't have to resort to that unfinished weapon either.", "/[waitkey(64)]/[close()]"};
    String[] msg389D617E2 = new String[]{"/[label()]", "Vector's Headquarters is located on the \"D#23mmerung,\" right?", "/[waitkey(1)]/[clear()]", "You can take your time completing your weapon, once you get there.", "/[waitkey(64)]/[close()]"};
    String[] msg377053F9 = new String[]{"Whoa, I'm surprised you found me. I'll give you a present.", "/[waitkey(64)]/[close()]"};
    String[] msg3770B3A6 = new String[]{"I only have one present. ", "I don't have anymore.", "/[waitkey(64)]/[close()]"};
    String[] elev_1 = new String[]{"/[label()]", "Would you like to go to the lower level?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0110() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.671f, 4.505f, 8.874f);
        this.camEV.setRotate(-20.6f, 216.896f, 0.0f);
        this.camEV.setFov(29.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.076f, 6.0735f, 6.618f);
        this.camEV.setRotate(-28.048f, 539.462f, 0.0f);
        this.camEV.setFov(24.879f);
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
                if (this.epass == 0) break;
                this.epass = 0;
                System.println("on!!");
                break;
            }
            case 1: {
                System.println("ele!!");
                if (this.epass == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.elev_1, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.setFlags(7042, 1, 1);
                            System.println("よね１");
                            System.sleep(10);
                            this.cam0.setMode(-1);
                            this.EV_Camera02();
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(20, 0.0f, 16.0f, true);
                            System.sleep(25);
                            this.player.rotY(15, 180.0f, true);
                            System.sleep(20);
                            Runtime.disable(65536);
                            System.sleep(10);
                            this.elv.setArgs(12, -5.0f);
                            Sound.effectPlay(196716);
                            System.sleep(30);
                            System.sleep(30);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.setPlayerControl(true);
                            Runtime.jumpCF(120, 2);
                            break block0;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(40, 0.0f, 13.78f, true);
                    System.sleep(40);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.epass = 1;
                    break;
                }
                this.epass = 1;
                System.println("ele2!!");
                break;
            }
            case 2: {
                if (Runtime.getFlags(25, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7064, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(23, 1) != 1) break;
                if (Runtime.mailReplyCheck(2) == 1) {
                    Runtime.mailArriveSet(11);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.setFlags(7064, 1, 1);
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
                if (Runtime.mailReplyCheck(2) == 2) {
                    Runtime.mailArriveSet(16);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.setFlags(7064, 1, 1);
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
                System.println("それ以外");
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        if (this.S015B == 1) {
            this.Talk_npc1_2();
        } else {
            this.Talk_npc1_1();
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        this.Talk_npc10_1(window);
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg371C0303, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg371C62AF, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg39C93009, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg39C98FB5, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg369FE9B4, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg36A04961, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        this.Talk_npc13_1(window);
    }

    void Talk_npc13_1(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                window.print(this.msg369D49C1, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg369DA96D, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc14(Enepc enepc, Window window) {
        this.Talk_npc14_1(window);
    }

    void Talk_npc14_1(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                window.print(this.msgANDREW_TALK1, 0);
                ST0110.waitPage(window, 64);
                window.print(this.msgANDREW_TALK2, 0);
                ST0110.waitPage(window, 64);
                window.print(this.msgANDREW_TALK3, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msgANDREW_TALK3, 0);
        ST0110.waitPage(window, 64);
    }

    void Talk_npc1_1() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgMORIYAMA_TALK1, 0);
        ST0110.waitPage(this.win, 64);
        this.win.print(this.msgMORIYAMA_TALK2, 0);
        ST0110.waitPage(this.win, 64);
        this.win.print(this.msgMORIYAMA_TALK3, 0);
        ST0110.waitPage(this.win, 64);
        System.sleep(7);
        this.fade.call(0);
        System.sleep(30);
        Runtime.setFlags(23, 1, 1);
        Runtime.setFlags(23, 1, 1);
        Runtime.setFlags(7011, 1, 1);
        Runtime.jumpEvent(1150);
    }

    void Talk_npc1_2() {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg39A1D84E, 0);
                ST0110.waitPage(this.win, 64);
                return;
            }
            case 2: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg39A237FA, 0);
                ST0110.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg39A1D84E1, 0);
        ST0110.waitPage(this.win, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg378BCB0A, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg378C2AB7, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg36EE42D3, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg36EEA280, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg389D617E1, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg389D617E2, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg39C93009, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg39C98FB5, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg3708C222, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg36EE42D3, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg36EEA280, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg389D01D2, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg389D617E, 0);
        ST0110.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg362E6A78, 0);
                ST0110.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg362ECA26, 0);
        ST0110.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            default:
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 6.0f, 0.0f, 14.0f, 0.0f);
        this.teiten1.SetBgm(196646);
        this.teiten4 = new Uwamono(28690, -6.0f, 0.0f, 14.0f, 0.0f);
        this.teiten4.SetBgm(196646);
        this.teiten10 = new Uwamono(28690, -2.0f, -2.0f, 0.0f, 0.0f);
        this.teiten10.SetBgm(196646);
        this.teiten11 = new Uwamono(28690, 2.0f, -2.0f, 0.0f, 0.0f);
        this.teiten11.SetBgm(196646);
        this.teiten14 = new Uwamono(28690, 0.0f, -1.0f, -7.0f, 0.0f);
        this.teiten14.SetBgm(196632);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        Stage.setVisible(-1, true);
        this.ring_1a = new Mapunits();
        this.ring_1b = new Mapunits();
        this.ring_1c = new Mapunits();
        this.ring_1a.mapUnit(126);
        this.ring_1b.mapUnit(127);
        this.ring_1c.mapUnit(128);
        this.ring_1a.start(4, null);
        this.ring_1b.start(4, null);
        this.ring_1c.start(4, null);
        this.ring_1a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_1a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2a = new Mapunits();
        this.ring_2b = new Mapunits();
        this.ring_2c = new Mapunits();
        this.ring_2a.mapUnit(129);
        this.ring_2b.mapUnit(130);
        this.ring_2c.mapUnit(131);
        this.ring_2a.start(4, null);
        this.ring_2b.start(4, null);
        this.ring_2c.start(4, null);
        this.ring_2a.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setTranslate(0.0f, 0.0f, 0.0f);
        this.ring_2a.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2b.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_2c.setRotate(0.0f, 0.0f, 0.0f);
        this.ring_1a.start(1, "ring_L");
        this.ring_1b.start(1, "ring_R");
        this.ring_1c.start(1, "ring_L");
        this.ring_2a.start(1, "ring_R");
        this.ring_2b.start(1, "ring_L");
        this.ring_2c.start(1, "ring_R");
        this.sonar_ring_0 = new Mapunits();
        this.sonar_ring_1 = new Mapunits();
        this.sonar_ring_2 = new Mapunits();
        this.sonar_ring_3 = new Mapunits();
        this.sonar_ring_0.mapUnit(133);
        this.sonar_ring_1.mapUnit(134);
        this.sonar_ring_2.mapUnit(135);
        this.sonar_ring_3.mapUnit(136);
        this.sonar_ring_0.start(4, null);
        this.sonar_ring_1.start(4, null);
        this.sonar_ring_2.start(4, null);
        this.sonar_ring_3.start(4, null);
        this.sonar_ring_0.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_1.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_2.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_3.setTranslate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_0.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_1.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_2.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_3.setRotate(0.0f, 0.0f, 0.0f);
        this.sonar_ring_0.start(1, "sonar_ring_Large");
        this.sonar_ring_1.start(1, "sonar_ring_L");
        this.sonar_ring_2.start(1, "sonar_ring_R");
        this.sonar_ring_3.start(1, "sonar_ring_L");
        this.Star_1 = new Mapunits();
        this.Star_1.mapUnit(2);
        this.Star_1.start(4, null);
        this.Star_1.setTranslate(this.Star_1.px, this.Star_1.py + 400.0f, this.Star_1.pz);
        this.Star_2 = new Mapunits();
        this.Star_2.mapUnit(1);
        this.Star_2.start(4, null);
        this.Star_2.setTranslate(this.Star_2.px, this.Star_2.py + 400.0f, this.Star_2.pz);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 10.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 350.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 6.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 5.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(7042, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(88, 0.041666668f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 0.0f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(88, 0.041666668f, -5.0f);
            this.elv.setArgs(1, 0, 1);
        }
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
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S015B = Runtime.getFlags(23, 1);
        this.SHION_TALK_3 = Runtime.getFlags(7008, 1);
        this.npcset_0();
        if (this.S015B == 1) {
            this.npcset_2();
        } else {
            this.npcset_1();
        }
        if (Runtime.getFlags(7042, 1) == 1) {
            this.npc21 = new NPC_NORMAL2(523, 21, 0, 13, 9, -4.8f, 0.0f, 5.23f, -135.0f);
            this.npc21.setInvalidID(1);
            this.npc21.setTranslate(-4.8f, -2.0f, 5.23f);
            this.npc21.setVisible(false);
            this.npc21.talkto("Talk_npc7");
            System.println("えれべイベント！");
            this.npc21.start(1, "elev");
        } else {
            this.npc21 = new NPC_NORMAL2(523, 21, 0, 13, 9, -4.8f, 0.0f, 5.23f, -135.0f);
            this.npc21.setInvalidID(1);
            this.npc21.setTranslate(-4.8f, -2.0f, 5.23f);
            this.npc21.setVisible(false);
            this.npc21.talkto("Talk_npc7");
        }
    }

    void npcset_0() {
        this.npc2 = new NPC_NORMAL2(524, 2, 0, 76, 9, 5.3f, 0.6f, 13.8f, 135.0f);
        this.npc4 = new NPC_NORMAL2(522, 4, 0, 76, 8, -5.2f, 0.0f, 13.7f, -135.0f);
        this.npc6 = new NPC_NORMAL2(520, 6, 0, 76, 8, 5.6f, -2.0f, 5.3f, 180.0f);
        this.npc7 = new NPC_NORMAL2(523, 7, 0, 76, 9, -5.6f, -1.5f, 5.3f, 180.0f);
        this.npc8 = new NPC_NORMAL2(524, 8, 0, 76, 9, 2.5f, -2.2f, 2.41f, 135.0f);
        this.npc9 = new NPC_NORMAL2(519, 9, 0, 76, 8, 2.5f, -2.2f, -0.5f, 135.0f);
        this.npc10 = new NPC_NORMAL2(520, 10, 0, 76, 8, 2.5f, -2.2f, -3.65f, 135.0f);
        this.npc11 = new NPC_NORMAL2(522, 11, 0, 76, 8, -2.5f, -2.4f, 2.31f, -135.0f);
        this.npc12 = new NPC_NORMAL2(521, 12, 0, 76, 8, -2.5f, -2.2f, -0.6f, -135.0f);
        this.npc13 = new NPC_NORMAL2(523, 13, 0, 76, 9, -2.5f, -1.8f, -3.6f, -135.0f);
        this.npc20 = new NPC_NORMAL2(523, 20, 0, 13, 9, 4.8f, 0.0f, 5.23f, -135.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 3);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 4);
        this.npc7.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 2);
        this.npc8.setInvalidID(1);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc9.setInvalidID(1);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 5);
        this.npc10.setInvalidID(1);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 7);
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 6);
        this.npc12.setInvalidID(1);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.setMotion(0, 4);
        this.npc13.setInvalidID(1);
        this.npc13.disableDTKFlag(3);
        this.npc13.enableDTKFlag(4);
        this.npc13.setMotion(0, 2);
        this.npc20.setInvalidID(1);
        this.npc20.setTranslate(4.8f, -2.0f, 5.23f);
        this.npc20.setVisible(false);
        this.npc2.talkto("Talk_npc2");
        this.npc4.talkto("Talk_npc4");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        this.npc13.talkto("Talk_npc13");
        this.npc20.talkto("Talk_npc6");
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL2(302, 1, 0, 76, 8, -0.5f, 2.1f, 7.9f, 160.0f);
        this.npc14 = new NPC_NORMAL(279, 14, 0, 57, 11, 2.34f, 0.0f, 13.3f, 0.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 5);
        this.npc1.disableDTKFlag(8);
        this.npc14.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
        this.npc14.talkto("Talk_npc14");
    }

    void npcset_2() {
        this.npc1 = new NPC_NORMAL(302, 1, 0, 14, 12, 0.91f, -2.0f, 2.21f, 180.0f);
        this.npc1.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
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
            this.setElevatorMode(1);
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

        void elev() {
            Runtime.setPlayerControl(false);
            Sound.effectPlay(196716);
            ST0110.this.player.setTranslate(0.0f, -5.0f, 16.12f);
            ST0110.this.cam0.setMode(-1);
            ST0110.this.EV_Camera01();
            System.sleep(10);
            System.sleep(90);
            ST0110.this.cam0.setMode(0);
            System.sleep(20);
            Sound.effectStop(196716);
            Sound.effectPlay(196717);
            System.sleep(15);
            Runtime.enable(65536);
            ST0110.this.player.mtn(2, 9, 1.0f, true);
            ST0110.this.player.move(40, 0.0f, 13.78f, true);
            System.sleep(40);
            Runtime.setFlags(7042, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void ring_L() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz - 0.1f);
                System.sleep(1);
            }
        }

        void ring_R() {
            while (true) {
                this.setRotate(this.rx, this.ry, this.rz + 0.1f);
                System.sleep(1);
            }
        }

        void sonar_ring_L() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void sonar_ring_Large() {
            float f = 0.5f;
            this.setTranslate(0.0f, 0.0f, -9.0f);
            while (true) {
                this.setTranslate(Math.cos(Math.toRadians(f)) * 9.0f, this.py, -9.0f + Math.sin(Math.toRadians(f)) * 9.0f);
                this.setRotateY(f);
                f += 0.5f;
                System.sleep(1);
            }
        }

        void sonar_ring_R() {
            float f = 0.5f;
            this.setTranslate(0.0f, 3.125f, 0.0f);
            while (true) {
                this.setTranslate(-Math.cos(Math.toRadians(f)) * 3.125f, 3.125f - Math.sin(Math.toRadians(f)) * 3.125f, this.pz);
                this.setRotate(this.rx, this.ry, -f);
                f += 0.5f;
                System.sleep(1);
            }
        }
    }
}

