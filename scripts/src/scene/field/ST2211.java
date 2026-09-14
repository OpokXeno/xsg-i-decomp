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
import xeno.map.MC_KUK07B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2210
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK07B_PRJ {
    int LADDER = Runtime.getFlags(6044, 1);
    int STAIRS = Runtime.getFlags(6045, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Enepc HASIGO;
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
    Enepc npc20;
    Enepc npc21;
    Uwamono douzou;
    Unit ladder;
    Unit stairs;
    boolean EnterCheck = false;
    Light light = new Light(0);
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
    int gaku;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono Atari;
    Uwamono Atari2;
    Uwamono Atari3;
    Uwamono Atari4;
    Uwamono Atari5;
    Effect button1A;
    Effect button1B;
    Effect button2A;
    Effect button2B;
    Effect kemuri1;
    Effect kemuri2;
    Effect gaitou1;
    Effect gaitou2;
    Effect gaitou3;
    Effect gaitou4;
    Effect lamp_s1;
    Effect lamp_s2;
    Effect lamp_s3;
    Effect lamp_m;
    Effect lamp_l1;
    Effect lamp_l2;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    int page;
    String[] NPC_TALK1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Hmmm, this statue can be pretty useful. I've got renewed respect for it!", "/[waitkey(1)]/[clear()]", "It's a good example of how you can find a use for just about anything, even if at first glance it seems useless.", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Sheesh, why do I have to repair the statue of this old guy?!", "/[waitkey(1)]/[clear()]", "Mr. King said it's only natural that we should fix it since it was his city that got wrecked...but what a pain in the butt!", "/[waitkey(64)]/[close()]"};
    String[] msg20002 = new String[]{"/[label()]", "What are you doing?! I finally got it back to the way it was! Now I gotta start over again.", "/[waitkey(64)]/[close()]"};
    String[] msg20003 = new String[]{"/[label()]", "You're mean! Do you have something against me?!\n", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "Oh, did you hear? Johnny rescued Mina.", "/[waitkey(1)]/[clear()]", "She's saying that she's annoyed and that she never asked for his help.", "/[waitkey(1)]/[clear()]", "She should put her pride aside and just thank him.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label(Mina)]", "Oh, thank you for what you did earlier! You were a great help. My father is baking bread for everyone in town. Please stay and have some!", "/[waitkey(64)]/[clear()]"};
    String[] msg400012 = new String[]{"/[label(Mina)]", "Oh, Johnny! How long is it going to take you to fix that thing? When you're done there, please work on the inside too!", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label(Mina)]", "Johnny is somewhat unreliable, so I can't help but feel like I need to keep an eye on him.", "/[waitkey(1)]/[clear()]", "It was kind of cool what he did earlier though.", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label(Johnny)]", "Are you here to rebuild the city too? Then, let me ask you guys a question, okay?", "/[waitkey(1)]/[clear()]", "Do you think that Mina is in love with me?", "/[waitkey(64)]/[close()]"};
    String[] msg50002 = new String[]{"/[label(Johnny)]", "I see, so you think so too!", "/[waitkey(1)]/[clear()]", "Hmm, then I don't understand. If that's true, I wonder why she's always getting mad at me?\n", "/[waitkey(1)]/[clear()]", "I see, so she's using reverse psychology! Hmm, she's sooo cute.", "/[waitkey(64)]/[close()]"};
    String[] msg50003 = new String[]{"/[label(Johnny)]", "Hmm? I can't hear you very well.", "/[waitkey(1)]/[clear()]", "Ah, well, someday, I'll melt her heart with my charms.", "/[waitkey(64)]/[close()]"};
    String[] msg50004 = new String[]{"/[label(Johnny)]", "Don't bother me. Mina will get mad at me if I slack off.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "I like this place! You have to do your own things yourself!", "/[waitkey(1)]/[clear()]", "The thing I like best about this town is that it doesn't give up even at times like this!", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label()]", "This is a cleaners. The shop has been wrecked though, so we're in no condition to work.", "/[waitkey(1)]/[clear()]", "If you insist, though, I won't stop you. Come right in.", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "King may act tough, but at the end of the day it's only because he's worried about the people of this city.", "/[waitkey(1)]/[clear()]", "Really, he's not very straightforward...he's sort of twisted.", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "Hmm, there's a treasure box on the roof of this building. But how do I get to it?", "/[waitkey(1)]/[clear()]", "Some of the surroundings collapsed from the earlier fighting, so I may be able to find a way up there now. I wonder how it was placed there in the first place?", "/[waitkey(64)]/[close()]"};
    String[] msg120001 = new String[]{"/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST2210() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.005f, 11.835f, 20.439f);
        this.camEV.setRotate(-31.866f, -33.955f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        switch (n) {
            case 2: {
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                System.println("クリーニング屋５");
                Runtime.jumpCF(2091, 5);
                break;
            }
            default: {
                System.println("d");
            }
        }
    }

    public void KickEvent(int n, int n2) {
        block41:
        {
            block43:
            {
                block42:
                {
                    block40:
                    {
                        if (n != 100) {
                            return;
                        }
                        if (n2 != 0 || this.EnterCheck) break block40;
                        this.player.getTranslate();
                        if (this.player.py < 3.5f || this.player.py > 7.0f) {
                            return;
                        }
                        System.println("ハシゴの上げ下げ");
                        this.EnterCheck = true;
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Q1, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Press\nDon't press\n");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        if (this.selected == 0) {
                            Sound.effectPlay(58);
                            if (this.LADDER == 0 && this.STAIRS == 0) {
                                System.println("A");
                                this.button2A.disp(false);
                                this.button2B.disp(true);
                                Sound.effectPlay(196743);
                                float f = 3.2f;
                                while (f >= 0.0f) {
                                    this.ladder.setTranslate(8.25f, f, 12.15f);
                                    System.sleep(1);
                                    f -= 0.1f;
                                }
                                this.player.setID(2);
                                Runtime.setFlags(6044, 1, 1);
                                this.LADDER = Runtime.getFlags(6044, 1);
                            } else if (this.LADDER == 1 && this.STAIRS == 0) {
                                System.println("B");
                                this.button2A.disp(true);
                                this.button2B.disp(false);
                                Sound.effectPlay(196743);
                                float f = 0.0f;
                                while (f <= 3.2f) {
                                    this.ladder.setTranslate(8.25f, f, 12.15f);
                                    System.sleep(1);
                                    f += 0.1f;
                                }
                                this.player.setID(1);
                                Runtime.setFlags(6044, 1, 0);
                                this.LADDER = Runtime.getFlags(6044, 1);
                            } else if (this.LADDER == 0 && this.STAIRS == 1) {
                                System.println("C");
                                this.button2A.disp(false);
                                this.button2B.disp(true);
                                Sound.effectPlay(196743);
                                float f = 3.2f;
                                while (f >= 0.0f) {
                                    this.ladder.setTranslate(8.25f, f, 12.15f);
                                    System.sleep(1);
                                    f -= 0.1f;
                                }
                                this.player.setID(4);
                                Runtime.setFlags(6044, 1, 1);
                                this.LADDER = Runtime.getFlags(6044, 1);
                            } else if (this.LADDER == 1 && this.STAIRS == 1) {
                                System.println("D");
                                this.button2A.disp(true);
                                this.button2B.disp(false);
                                Sound.effectPlay(196743);
                                float f = 0.0f;
                                while (f <= 3.2f) {
                                    this.ladder.setTranslate(8.25f, f, 12.15f);
                                    System.sleep(1);
                                    f += 0.1f;
                                }
                                this.player.setID(3);
                                Runtime.setFlags(6044, 1, 0);
                                this.LADDER = Runtime.getFlags(6044, 1);
                            }
                        }
                        this.EnterCheck = false;
                        Runtime.setPlayerControl(true);
                        break block41;
                    }
                    if (n2 != 1 || this.EnterCheck) break block42;
                    this.player.getTranslate();
                    if (this.player.py < 3.5f || this.player.py > 7.0f) {
                        return;
                    }
                    System.println("回転床の上げ下げ");
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Q1, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Press\nDon't press\n");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    if (this.selected == 0) {
                        System.sleep(30);
                        Sound.effectPlay(58);
                        this.cam0.setMode(-1);
                        this.EV_Camera01();
                        System.sleep(30);
                        Sound.effectPlay(196744);
                        float f = 0.020799987f;
                        float f2 = 0.0035000006f;
                        if (this.LADDER == 0 && this.STAIRS == 0) {
                            System.println("E");
                            this.button1A.disp(false);
                            this.button1B.disp(true);
                            float f3 = 0.0f;
                            while (f3 <= 30.0f) {
                                this.stairs.setRotate(0.0f, 0.0f, -f3);
                                this.stairs.setTranslate(5.049f - f * f3, 5.795f + f2 * f3, 12.0f);
                                System.sleep(1);
                                f3 += 1.0f;
                            }
                            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
                            this.player.setID(3);
                            Runtime.setFlags(6045, 1, 1);
                            this.STAIRS = Runtime.getFlags(6045, 1);
                        } else if (this.LADDER == 1 && this.STAIRS == 0) {
                            System.println("F");
                            this.button1A.disp(false);
                            this.button1B.disp(true);
                            float f4 = 0.0f;
                            while (f4 <= 30.0f) {
                                this.stairs.setRotate(0.0f, 0.0f, -f4);
                                this.stairs.setTranslate(5.049f - f * f4, 5.795f + f2 * f4, 12.0f);
                                System.sleep(1);
                                f4 += 1.0f;
                            }
                            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
                            this.player.setID(4);
                            Runtime.setFlags(6045, 1, 1);
                            this.STAIRS = Runtime.getFlags(6045, 1);
                        } else if (this.LADDER == 0 && this.STAIRS == 1) {
                            System.println("G");
                            this.button1A.disp(true);
                            this.button1B.disp(false);
                            float f5 = 0.0f;
                            while (f5 <= 30.0f) {
                                this.stairs.setRotate(0.0f, 0.0f, -30.0f + f5);
                                this.stairs.setTranslate(4.425f + f * f5, 5.9f - f2 * f5, 12.0f);
                                System.sleep(1);
                                f5 += 1.0f;
                            }
                            this.stairs.setTranslate(5.049f, 5.795f, 12.0f);
                            this.player.setID(1);
                            Runtime.setFlags(6045, 1, 0);
                            this.STAIRS = Runtime.getFlags(6045, 1);
                        } else if (this.LADDER == 1 && this.STAIRS == 1) {
                            System.println("H");
                            this.button1A.disp(true);
                            this.button1B.disp(false);
                            float f6 = 0.0f;
                            while (f6 <= 30.0f) {
                                this.stairs.setRotate(0.0f, 0.0f, -30.0f + f6);
                                this.stairs.setTranslate(4.425f + f * f6, 5.9f - f2 * f6, 12.0f);
                                System.sleep(1);
                                f6 += 1.0f;
                            }
                            this.stairs.setTranslate(5.049f, 5.795f, 12.0f);
                            this.player.setID(2);
                            Runtime.setFlags(6045, 1, 0);
                            this.STAIRS = Runtime.getFlags(6045, 1);
                        }
                    }
                    this.EnterCheck = false;
                    System.sleep(30);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    break block41;
                }
                if (n2 != 2) break block43;
                System.println("serial = 2");
                this.player.getTranslate();
                if (!(this.player.py > 9.0f)) break block41;
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px - 0.35f, this.player.py, this.player.pz);
                System.sleep(10);
                Runtime.setPlayerControl(true);
                break block41;
            }
            if (n2 == 4 && !this.EnterCheck) {
                if (Runtime.getFlags(386, 1) == 0) {
                    return;
                }
                if (Runtime.getFlags(7057, 2) != 2) {
                    return;
                }
                if (Runtime.getFlags(7173, 1) == 0) {
                    Runtime.mailArriveSet(75);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.setFlags(7173, 1, 1);
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
            }
        }
        if (n2 == 5 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py > 3.5f) {
                return;
            }
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.player.look_char(this.npc4);
            ++this.talkFlag4;
            switch (this.talkFlag4) {
                case 1: {
                    this.npc4.kickEnepc(4, 1);
                    this.npc4.kickEnepc(1, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msg40001, 0);
                    ST2210.waitPage(this.win, 64);
                    this.npc4.look_char(this.npc5);
                    this.win.print(this.msg400012, 0);
                    ST2210.waitPage(this.win, 64);
                    this.EnterCheck = false;
                    this.player.look_default();
                    this.npc4.look_default();
                    this.npc4.kickEnepc(4, 0);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            this.npc4.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg40002, 0);
            ST2210.waitPage(this.win, 64);
            this.EnterCheck = false;
            this.player.look_default();
            this.npc4.kickEnepc(4, 0);
            Runtime.setPlayerControl(true);
            return;
        }
        if (n2 == 6 && !this.EnterCheck) {
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.player.look_char(this.npc8);
            this.npc8.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80001, 0);
            ST2210.waitPage(this.win, 64);
            this.npc8.kickEnepc(1, 10);
            this.EnterCheck = false;
            this.player.look_default();
            Runtime.setPlayerControl(true);
            return;
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        window.print(this.msg110001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg10001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        switch (this.gaku) {
            case 1: {
                window.print(this.msg20002, 0);
                ST2210.waitPage(window, 64);
                ++this.gaku;
                return;
            }
            case 2: {
                window.print(this.msg20003, 0);
                ST2210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg20001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg40001, 0);
                ST2210.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg40002, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc) {
        this.Talk_npc5_1();
    }

    void Talk_npc5_1() {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg50001, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("I think she loves you\nI don't think so");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg50002, 0);
                        ST2210.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg50003, 0);
                ST2210.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg50004, 0);
        ST2210.waitPage(this.win, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg60001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        window.print(this.msg80001, 0);
        ST2210.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg90001, 0);
        ST2210.waitPage(window, 64);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                ++this.gaku;
                this.npc2.setMotion(0, 6);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後・外観街１");
                Runtime.jumpCF(2171, 1);
                break;
            }
            case 1: {
                System.println("襲撃後・外観街７");
                Runtime.jumpCF(2171, 7);
                break;
            }
            case 2: {
                System.println("襲撃後倉庫・１");
                Runtime.jumpCF(2241, 1);
                break;
            }
            case 3: {
                System.println("襲撃後倉庫・２");
                Runtime.jumpCF(2241, 2);
                break;
            }
            case 4: {
                System.println("クリーニング屋１");
                Runtime.jumpCF(2091, 1);
                break;
            }
            case 5: {
                System.println("襲撃後・パン屋４");
                Runtime.jumpCF(2081, 4);
                break;
            }
            case 6: {
                System.println("襲撃後・パン屋５");
                Runtime.jumpCF(2081, 5);
                break;
            }
            case 7: {
                System.println("襲撃後・パン屋１");
                Runtime.jumpCF(2221, 1);
                break;
            }
            case 8: {
                System.println("襲撃後・パン屋３");
                Runtime.jumpCF(2221, 3);
                break;
            }
            case 9: {
                System.println("襲撃後・パン屋２");
                Runtime.jumpCF(2221, 2);
                break;
            }
            case 10: {
                System.println("クリーニング屋２");
                Runtime.jumpCF(2091, 2);
                break;
            }
            case 11: {
                System.println("襲撃後倉庫・４");
                Runtime.jumpCF(2241, 4);
                break;
            }
            case 12: {
                System.println("クリーニング屋３");
                Runtime.jumpCF(2091, 3);
                break;
            }
            case 13: {
                System.println("襲撃後倉庫・３");
                Runtime.jumpCF(2241, 3);
                break;
            }
            case 14: {
                System.println("ミニマップ");
                Runtime.jumpCF(2130, 2);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.15f, 1.15f, 1.15f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(1, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.37f, 0.37f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.37f, 0.37f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.37f, 0.37f, 0.3f);
        Runtime.setIdLightCol(2, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(4, 1, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(4, 2, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(4, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(5, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(6, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(6, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(7, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(7, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(7, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(7, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(8, 0, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(8, 1, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(8, 2, 0.345f, 0.345f, 0.3f);
        Runtime.setIdLightCol(8, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(8, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(8, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(8, 3, 0.0f, -1.0f, -3.0f);
        this.button1A = new Effect(1539, -6.5f, 4.86f, 12.18f, 0.0f);
        this.button1A.setScale(0.425f, 0.25f, 0.275f);
        this.button1A.setRotate(-35.0f, 0.0f, 0.0f);
        this.button1B = new Effect(1542, -6.5f, 4.86f, 12.18f, 0.0f);
        this.button1B.setScale(0.425f, 0.25f, 0.275f);
        this.button1B.setRotate(-35.0f, 0.0f, 0.0f);
        this.button2A = new Effect(1539, 5.996f, 4.84f, 12.18f, 0.0f);
        this.button2A.setScale(0.425f, 0.25f, 0.275f);
        this.button2A.setRotate(-50.0f, 0.0f, 0.0f);
        this.button2B = new Effect(1542, 5.996f, 4.84f, 12.18f, 0.0f);
        this.button2B.setScale(0.425f, 0.25f, 0.275f);
        this.button2B.setRotate(-55.0f, 0.0f, 0.0f);
        this.kemuri1 = new Effect(1515, -3.5f, 16.5f, 3.4f, 0.0f);
        this.kemuri1.setScale(0.8f, 1.0f, 0.8f);
        this.kemuri1.setClip(true);
        this.kemuri2 = new Effect(1515, 4.5f, 16.5f, 3.4f, 0.0f);
        this.kemuri2.setScale(0.8f, 1.0f, 0.8f);
        this.kemuri2.setClip(true);
        this.gaitou1 = new Effect(1405, -12.955f, 3.602f, 18.969f, 0.0f);
        this.gaitou1.disp(true);
        this.gaitou2 = new Effect(1405, -2.155f, 3.602f, 18.969f, 0.0f);
        this.gaitou2.disp(true);
        this.gaitou3 = new Effect(1405, 8.855f, 3.602f, 18.969f, 0.0f);
        this.gaitou3.disp(true);
        this.gaitou4 = new Effect(1405, 18.555f, 3.602f, 18.969f, 0.0f);
        this.gaitou4.disp(true);
        this.lamp_s1 = new Effect(1630, -12.924f, 2.953f, 7.647f, 0.0f);
        this.lamp_s1.setScale(0.4f, 0.4f, 0.4f);
        this.lamp_s1.setRotate(-25.0f, 0.0f, 0.0f);
        this.lamp_s1.disp(true);
        this.lamp_s2 = new Effect(1713, -11.673f, 9.476f, -3.7f, 0.0f);
        this.lamp_s2.setScale(0.35f, 0.35f, 0.35f);
        this.lamp_s2.disp(true);
        this.lamp_s3 = new Effect(1713, 18.881f, 9.638f, -1.703f, 0.0f);
        this.lamp_s3.setScale(0.45f, 0.45f, 0.45f);
        this.lamp_s3.disp(true);
        this.lamp_m = new Effect(1713, 17.365f, 6.734f, 4.373f, 0.0f);
        this.lamp_m.setScale(0.55f, 0.55f, 0.55f);
        this.lamp_m.disp(true);
        this.lamp_l1 = new Effect(1637, -7.714f, 2.127f, 14.282f, 0.0f);
        this.lamp_l1.setScale(1.0f, 0.75f, 1.0f);
        this.lamp_l1.disp(false);
        this.lamp_l2 = new Effect(1637, -1.294f, 2.127f, 14.282f, 0.0f);
        this.lamp_l2.setScale(1.0f, 0.75f, 1.0f);
        this.lamp_l2.setRotate(0.0f, 0.0f, 25.0f);
        this.lamp_l2.disp(true);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(151, false);
        Stage.setVisible(154, false);
        Stage.setVisible(156, false);
        Stage.setVisible(158, false);
        Stage.setVisible(153, false);
        Stage.setVisible(18, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, -12.5f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.02f, 0.02f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.02f, 0.02f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.015f, 0.015f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.02f, 0.02f);
        this.cam0.setCFPedestal(15, -12.487035f, 11.579102f, 12.472312f, 28.595207f, -26.044058f, 14.126161f, 0.0f, 2.0f);
        this.cam0.setCFHokan(15, 100.0f, 100.0f);
        this.cam0.setCFPedestal(16, 16.485668f, 13.60734f, 25.085058f, 29.759653f, -29.232653f, 34.92518f, 0.0f, 2.0f);
        this.cam0.setCFHokan(16, 100.0f, 100.0f);
        this.cam0.setCFPedestal(17, -8.388614f, 19.13557f, 6.524935f, 35.520004f, -42.413883f, 36.31963f, 0.0f, 2.0f);
        this.cam0.setCFHokan(17, 100.0f, 100.0f);
        this.cam0.setCFAngle(18, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(18, 0.015f, 0.015f);
        this.cam0.setCFAngle(19, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(19, 100.0f, 100.0f);
        this.cam0.setCFAngle(20, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(20, 100.0f, 100.0f);
        this.cam0.setCFAngle(21, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(21, 100.0f, 100.0f);
        this.cam0.setCFPedestal(22, 5.3421884f, 16.479866f, -3.0894277f, 43.19987f, -40.830788f, -32.078743f, 0.0f, 2.0f);
        this.cam0.setCFHokan(22, 100.0f, 100.0f);
        this.cam0.setCFAngle(23, -28.0f, -30.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(23, 0.015f, 0.015f);
        this.cam0.setCFAngle(24, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(24, 0.02f, 0.02f);
        this.cam0.setCFLockX(24, -18.5f);
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
        this.HASIGO = new NPC_NORMAL(1591, 20, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.HASIGO.setInvalidID(1);
        this.HASIGO.kickEnepc(4, 2);
        this.HASIGO.setVisible(false);
        this.HASIGO.dispRadar(false);
        this.HASIGO.kickEnepc(19, 1, 0, 420, 0);
        this.HASIGO.kickEnepc(19, 2, 0, 770, 1);
        this.HASIGO.kickEnepc(19, 3, 0, 420, 1);
        this.ladder = new Unit();
        this.ladder.mapUnit(169);
        this.ladder.start(4, null);
        this.stairs = new Unit();
        this.stairs.mapUnit(181);
        this.stairs.start(4, null);
        if (this.STAIRS == 0 && this.LADDER == 0) {
            System.println("STAIRS == 0 && LADDER == 0");
            this.button1A.disp(true);
            this.button1B.disp(false);
            this.button2A.disp(true);
            this.button2B.disp(false);
            this.ladder.setTranslate(8.25f, 3.2f, 12.15f);
            this.player.setID(1);
        } else if (this.STAIRS == 0 && this.LADDER == 1) {
            System.println("STAIRS == 0 && LADDER == 1");
            this.button1A.disp(true);
            this.button1B.disp(false);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 0.0f, 12.15f);
            this.player.setID(2);
        } else if (this.STAIRS == 1 && this.LADDER == 0) {
            System.println("STAIRS == 1 && LADDER == 0");
            this.button1A.disp(false);
            this.button1B.disp(true);
            this.button2A.disp(true);
            this.button2B.disp(false);
            this.ladder.setTranslate(8.25f, 3.2f, 12.15f);
            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(3);
        } else if (this.STAIRS == 1 && this.LADDER == 1) {
            System.println("STAIRS == 1 && LADDER == 1");
            this.button1A.disp(false);
            this.button1B.disp(true);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 0.0f, 12.15f);
            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(4);
        }
        this.itembox = new Uwamono(28677, -2.71f, 10.8f, 7.66f, 180.0f, 422);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        this.npc1 = new NPC_NORMAL(1552, 1, 0, 14, 15, 15.5f, 0.0f, 15.63f, 200.0f);
        this.npc2 = new NPC_NORMAL(1609, 2, 0, 14, 11, 14.98f, 0.0f, 12.31f, 180.0f);
        this.npc3 = new NPC_NORMAL(1591, 3, 0, 14, 12, -4.16f, 0.0f, 15.0f, 220.0f);
        this.npc4 = new NPC_NORMAL(1606, 4, 0, 14, 16, -5.14f, 0.0f, 13.8f, 0.0f);
        this.npc5 = new NPC_NORMAL(1613, 5, 0, 14, 11, -6.65f, 0.0f, 14.74f, 130.0f);
        this.npc6 = new NPC_NORMAL(1567, 6, 0, 14, 11, -15.77f, 0.04f, 16.63f, 45.0f);
        this.npc8 = new NPC_NORMAL(1570, 8, 0, 14, 14, -16.0f, 3.6f, -0.8f, 0.0f);
        this.npc9 = new NPC_NORMAL(1561, 9, 0, 14, 11, -13.94f, 3.59f, 1.93f, 45.0f);
        this.npc11 = new NPC_NORMAL(1540, 11, 0, 14, 13, 5.92f, 7.19f, 14.39f, 310.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 9);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 4);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 12);
        this.npc3.setInvalidID(1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 10);
        this.npc4.enableDTKFlag(262144);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 3);
        this.npc6.setInvalidID(1);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 10);
        this.npc8.setInvalidID(1);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 1);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 10);
        this.npc11.setInvalidID(1);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc11.talkto("Talk_npc11");
        new Uwamono(94, 39);
        new Uwamono(85, 39);
        new Uwamono(95, 40);
        this.douzou = new Uwamono(180, 5);
        this.douzou.SetCallNo(1);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3233, 1, 1);
                break;
            }
        }
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
    }
}

