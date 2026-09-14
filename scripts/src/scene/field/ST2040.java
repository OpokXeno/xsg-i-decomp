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
import xeno.map.MC_KUK04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2040
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK04_PRJ {
    int SAKU = Runtime.getFlags(6041, 1);
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    Unit elv;
    boolean EnterCheck = false;
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
    Enepc npc20;
    Enepc npc21;
    Enepc npc25;
    Effect obj600;
    Light light = new Light(0);
    Uwamono K1;
    Uwamono K2;
    Uwamono K3;
    Uwamono K4;
    Uwamono K5;
    Uwamono K6;
    Uwamono K7;
    Uwamono K8;
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
    int talkFlag20;
    int talkFlag21;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    MAPUnit fanR;
    MAPUnit fanL;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect button_red;
    Effect button_blue;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    int page;
    String[] NPC1_TALK1 = new String[]{"Which one would you like?\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"What will you do?\n", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Hey, I haven't seen you around before. Have you been working out regularly? You won't make it in this town looking like that!", "/[waitkey(1)]/[clear()]", "Listen, men have to be strong. You know what I mean? Or else, you can't protect what's important.", "/[waitkey(1)]/[clear()]", "Gwa ha ha ha ha ha!", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "I'm always telling the kids in this city, \"If you're a man, become a strong man like me.\"", "/[waitkey(1)]/[clear()]", "Gwa ha ha ha ha ha!", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "What did you think of my performance?\n", "/[waitkey(1)]/[clear()]", "Just kidding, this piano is broken, so it doesn't make a sound.", "/[waitkey(64)]/[close()]"};
    String[] msg20002 = new String[]{"/[label()]", "Gives it pretty nice atmosphere, doesn't it?", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "The bartender's long-winded stories are annoying. He boasts about himself every chance he gets.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Man, I love this shop's style!", "/[waitkey(1)]/[clear()]", "Look at that, the Iron 3! I've always admired them ever since I was a kid. I am who I am today thanks to that comic!", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label()]", "What do you think of this body?! A respectable body, comparable to the Iron 3, right? If I joined up with them, it will be the beginning of the Iron 4! Well, it's just a comic book story though.", "/[waitkey(64)]/[close()]"};
    String[] msg40003 = new String[]{"/[label()]", "Hey mister, you've got some muscle there. But you don't have your fundamentals right. The fundamental of the Iron 3 is lots of muscle! It's not cool to have a\nscrawny body.", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "That customer's been going on and on about comics.\nI don't care if it's an iron or an ironing board, I just wish he'd be a little quieter.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Why did we come to a bar like this? We should have picked a bar with better atmosphere!", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "I heard the Durandal entered port. I should have picked the observation room instead. But then again, the waitress here, Mayumi, is really nice.", "/[waitkey(1)]/[clear()]", "She's like a sweet older sister.", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "W-watch out!", "/[waitkey(1)]/[clear()]", "This is a cargo elevator. You can operate it with the switch on the 2nd floor.", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label()]", "Shelley and Mary are really great. They're so beautiful.\nI envy Director Gaignun for being able to be with those sisters.", "/[waitkey(64)]/[close()]"};
    String[] msg80002 = new String[]{"/[label()]", "I'm in their fan club, and my fan club membership number is in the single digits. Of course, I've met the president, Jr., too.", "/[waitkey(1)]/[clear()]", "Jr. is so lucky. He gets to be with those beautiful sisters on the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msg800021 = new String[]{"/[label()]", "Oh, Mr. Jr.?! N-Nice to meet you! I-I'm member #93 143 in the Shelley & Mary Fan Club!", "/[waitkey(1)]/[clear()]", "This month's fan club bulletin was really interesting. I particularly liked the specially made hologram of Shelley. It was the best! You certainly have good taste.!", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Did you go to the hotel next door?", "/[waitkey(1)]/[clear()]", "That's one really amazing girl. Her father was really family oriented and hard working before his wife died.", "/[waitkey(1)]/[clear()]", "Well, they do say that people who care about each other fight more. He's probably just worried sick about his daughter.", "/[waitkey(64)]/[close()]"};
    String[] msg100001 = new String[]{"/[label(Mayumi)]", "Welcome!", "/[waitkey(1)]/[clear()]", "Welcome to the Iron Man! Please take any available seat!", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "*Sniff...sniff...*\n", "Oh, how is it that I'm so unlucky? I can't believe I lost the precious \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" that my fianc#28 gave me...", "/[waitkey(1)]/[clear()]", "If he finds out that I lost the \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" I'm sure he'll be so upset that he'll toss me out like garbage.", "/[waitkey(1)]/[clear()]", "*Sob...*\n", "If only I hadn't dropped the ring on the beach. If only the fish hadn't taken the ring I dropped in its mouth.\nIf only the fish with the ring hadn't escaped to Director Gaignun's private beach...", "/[waitkey(1)]/[clear()]", "I'd still be happy...\n", "*Sob...sniff...*", "/[waitkey(64)]/[close()]"};
    String[] msg110002 = new String[]{"/[label()]", "*Sniff...*\n", "*Sob... sob...*\n", "/[waitkey(1)]/[clear()]", "What should I do? The fish with the \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" should be in the waters of Director Gaignun's private beach.", "/[waitkey(1)]/[clear()]", "Someone...anyone...please help me...!", "/[waitkey(64)]/[close()]"};
    String[] msg110003 = new String[]{"/[label()]", "?!", "/[waitkey(1)]/[clear()]", "T-this is...my \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]!\" You went and found it for me?", "/[waitkey(1)]/[clear()]", "*S-sniff...*\n", "*Sob...* T-thank you...now, I can go to him...", "/[waitkey(1)]/[clear()]", "*Sniff...sob...* This is a just token of my appreciation. Please take it. I think it'll come in handy.", "/[waitkey(64)]/[close()]"};
    String[] msg110004 = new String[]{"/[label()]", "*Sniff...sniff...*\n", "I-I will find happiness!\n", "*Sob...sob...*\n", "Thank you.", "/[waitkey(64)]/[close()]"};
    String[] msgSHION1 = new String[]{"/[label(Shion)]", "Oh, Allen? What in the world are you doing here?", "/[waitkey(64)]/[clear()]"};
    String[] msgALLEN1 = new String[]{"/[label(Allen)]", "Oh, Chief! What's the matter? Weren't you resting in your room?", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION2 = new String[]{"/[label(Shion)]", "Yeah, I was going to but...", "/[waitkey(64)]/[clear()]"};
    String[] msgALLEN2 = new String[]{"/[label(Allen)]", "Oh, yeah, take a look. What do you think of this pose?! I look pretty cool, don't I?", "/[waitkey(1)]/[clear()]", "I really hit it off with this person! He's teaching me right now.", "/[waitkey(1)]/[clear()]", "Umm, what's the matter?", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION3 = new String[]{"/[label(Shion)]", "...Oh no, it's nothing. As long as you're all right.", "/[waitkey(1)]/[clear()]", "I'm going to go back to the Durandal. See ya!", "/[waitkey(64)]/[clear()]"};
    String[] msgALLEN3 = new String[]{"/[label(Allen)]", "What? Chief?! What's the matter? What are you so mad about?!", "/[waitkey(64)]/[close()]"};
    String[] msgROBO1 = new String[]{"/[label(Assistant Scott)]", "Damn it! Damn it! Why doesn't the professor understand? Why won't he listen to my opinions?", "/[waitkey(1)]/[clear()]", "I'm...I'm...I'm just worried about the professor, that's all...", "/[waitkey(1)]/[clear()]", "Damn, I need a drink! Damn eccentric, hardheaded\ngeezer! I don't care anymore. He'll regret it later when he's all alone!", "/[waitkey(64)]/[clear()]"};
    String[] msgROBO2 = new String[]{"/[label(Assistant Scott)]", "The professor can't even do anything by himself. He really can't do anything without me.", "/[waitkey(1)]/[clear()]", "He's probably having problems right about now. The professor might even be crying.", "/[waitkey(1)]/[clear()]", "Damn! I know he needs me! I can't just leave the Professor all by himself! Professor, just hang in there. I'm coming to help you!!", "/[waitkey(64)]/[close()]"};
    String[] msgROBO3 = new String[]{"/[label(Assistant Scott)]", "Professor, just hang in there. I promise I will save you!", "/[waitkey(64)]/[close()]"};

    ST2040() {
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
                System.println("aaaaaa");
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
                switch (this.selected) {
                    case 0: {
                        if (Runtime.getFlags(6041, 1) == 0) {
                            this.button_red.disp(false);
                            this.button_blue.disp(true);
                            this.player.setID(2);
                            Runtime.setFlags(6041, 1, 1);
                            Sound.effectPlay(196745);
                            this.elv.setArgs(12, 6.0f);
                            System.sleep(60);
                            System.sleep(60);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            break block0;
                        }
                        this.button_blue.disp(false);
                        this.button_red.disp(true);
                        this.player.setID(1);
                        Runtime.setFlags(6041, 1, 0);
                        Sound.effectPlay(196745);
                        this.elv.setArgs(12, 0.0f);
                        System.sleep(60);
                        System.sleep(60);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        this.Talk_npc10_1(window);
    }

    void Talk_npc10_1(Window window) {
        window.print(this.msg100001, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        if (Runtime.getFlags(7165, 1) == 0) {
            ++this.talkFlag11;
            switch (this.talkFlag11) {
                case 1: {
                    window.print(this.msg110001, 0);
                    ST2040.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg110002, 0);
            ST2040.waitPage(window, 64);
            return;
        }
        if (Runtime.checkItem(10, 23) == 0) {
            ++this.talkFlag11;
            switch (this.talkFlag11) {
                case 1: {
                    window.print(this.msg110003, 0);
                    ST2040.waitPage(window, 64);
                    Runtime.setFlags(3228, 1, 1);
                    Runtime.removeItem(10, 72);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 23);
                    Runtime.setFlags(3228, 1, 1);
                    return;
                }
            }
            window.print(this.msg110004, 0);
            ST2040.waitPage(window, 64);
            return;
        }
        window.print(this.msg110004, 0);
        ST2040.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                this.npc1.kickEnepc(4, 1);
                this.npc1.look_char(this.player);
                this.npc1.kickEnepc(1, 10);
                window.print(this.msg10001, 0);
                ST2040.waitPage(window, 64);
                this.npc1.look_default();
                this.npc1.kickEnepc(4, 0);
                return;
            }
        }
        this.npc1.kickEnepc(4, 1);
        this.npc1.look_char(this.player);
        this.npc1.kickEnepc(1, 10);
        window.print(this.msg10002, 0);
        ST2040.waitPage(window, 64);
        this.npc1.look_default();
        this.npc1.kickEnepc(4, 0);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    public void Talk_npc20(Enepc enepc, Window window) {
        this.Talk_npc20_1(window);
    }

    void Talk_npc20_1(Window window) {
        window.print(this.msgSHION1, 0);
        ST2040.waitPage(window, 64);
        this.npc20.kickEnepc(1, 9);
        window.print(this.msgALLEN1, 0);
        ST2040.waitPage(window, 64);
        window.print(this.msgSHION2, 0);
        ST2040.waitPage(window, 64);
        this.npc20.kickEnepc(1, 28);
        window.print(this.msgALLEN2, 0);
        ST2040.waitPage(window, 64);
        window.print(this.msgSHION3, 0);
        ST2040.waitPage(window, 64);
        this.npc20.kickEnepc(1, 9);
        window.print(this.msgALLEN3, 0);
        ST2040.waitPage(window, 64);
        System.sleep(15);
        this.fade.call(0);
        System.sleep(30);
        Runtime.setFlags(3138, 1, 1);
        Runtime.setFlags(347, 1, 1);
        System.println("フラグオン！");
        Runtime.jumpEvent(3320);
    }

    public void Talk_npc21(Enepc enepc, Window window) {
        this.Talk_npc21_1(window);
    }

    void Talk_npc21_1(Window window) {
        ++this.talkFlag21;
        switch (this.talkFlag21) {
            case 1: {
                window.print(this.msgROBO1, 0);
                ST2040.waitPage(window, 64);
                Runtime.setFlags(3156, 1, 1);
                System.println("フラグオン！");
                window.print(this.msgROBO2, 0);
                ST2040.waitPage(window, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(2110, 1);
                return;
            }
        }
        window.print(this.msgROBO3, 0);
        ST2040.waitPage(window, 64);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg20001, 0);
                ST2040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg20002, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(347, 1) == 1) {
            this.Talk_npc4_1(window);
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        switch (this.talkFlag4) {
            case 0: {
                this.npc4.kickEnepc(1, 28);
                window.print(this.msg40001, 0);
                ST2040.waitPage(window, 64);
                ++this.talkFlag4;
                return;
            }
        }
        this.npc4.kickEnepc(1, 28);
        window.print(this.msg40002, 0);
        ST2040.waitPage(window, 64);
        --this.talkFlag4;
    }

    void Talk_npc4_2(Window window) {
        this.npc4.kickEnepc(1, 28);
        window.print(this.msg40003, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg60001, 0);
                ST2040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg60002, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.msg800021, 0);
            ST2040.waitPage(window, 64);
            return;
        }
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg80001, 0);
                ST2040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg80002, 0);
        ST2040.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg90001, 0);
        ST2040.waitPage(window, 64);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                break;
            }
            case 2: {
                System.println("case2");
                this.K1.getTranslate();
                System.sleep(13);
                this.K1.SetGravity(false);
                this.K1.setTranslate(this.K1.px, 0.0f, this.K1.pz);
                break;
            }
            case 3: {
                System.println("case3");
                this.K2.getTranslate();
                System.sleep(13);
                this.K2.SetGravity(false);
                this.K2.setTranslate(this.K2.px, 0.0f, this.K2.pz);
                break;
            }
            case 4: {
                System.println("case4");
                this.K3.getTranslate();
                System.sleep(13);
                this.K3.SetGravity(false);
                this.K3.setTranslate(this.K3.px, 0.0f, this.K3.pz);
                break;
            }
            case 5: {
                System.println("case5");
                break;
            }
            case 6: {
                System.println("case6");
                this.K5.getTranslate();
                System.sleep(13);
                this.K5.SetGravity(false);
                this.K5.setTranslate(this.K5.px, 0.0f, this.K5.pz);
                break;
            }
            case 7: {
                System.println("case7");
                this.K6.getTranslate();
                System.sleep(13);
                this.K6.SetGravity(false);
                this.K6.setTranslate(this.K6.px, 0.0f, this.K6.pz);
                break;
            }
            case 8: {
                System.println("case8");
                this.K7.getTranslate();
                System.sleep(13);
                this.K7.SetGravity(false);
                this.K7.setTranslate(this.K7.px, 0.0f, this.K7.pz);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外観街１・４");
                Runtime.jumpCF(2030, 4);
                break;
            }
            case 1: {
                System.println("宿屋２Ｆ・４");
                Runtime.jumpCF(2060, 4);
                break;
            }
            case 2: {
                System.println("外観街１・６");
                Runtime.jumpCF(2030, 6);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(1, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(4, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightCol(5, 1, 0.4f, 0.4f, 0.355f);
        Runtime.setIdLightCol(5, 2, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightCol(5, 3, 0.4f, 0.4f, 0.35f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, -0.25f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1713, 15.8f, 9.2f, -3.5f, 0.0f);
        this.light02 = new Effect(1713, 15.6f, 9.2f, 1.5f, 0.0f);
        this.light03 = new Effect(1713, 13.6f, 2.7f, 1.5f, 0.0f);
        this.light04 = new Effect(1713, 10.5f, 2.7f, -3.7f, 0.0f);
        this.light05 = new Effect(1713, 3.5f, 2.7f, -2.3f, 0.0f);
        this.light06 = new Effect(1713, -3.5f, 2.7f, -2.3f, 0.0f);
        this.light07 = new Effect(1713, -10.4f, 9.7f, -3.9f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.5f, 0.5f, 0.5f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.5f, 0.5f, 0.5f);
        this.light05.setScale(0.5f, 0.5f, 0.5f);
        this.light06.setScale(0.5f, 0.5f, 0.5f);
        this.light07.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(60);
        this.light08 = new Effect(1579, -2.25f, 3.55f, -2.25f, 0.0f);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(6041, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(202, 0.041666668f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 0.097f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(202, 0.041666668f, 6.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 6.0f);
        }
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(137, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(4, 0.03f, 0.03f);
        this.cam0.setCFAngle(5, -28.0f, -12.5f, 0.0f, 5.0f, 42.5f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.0f, 42.5f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
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
        this.button_red = new Effect(1539, -11.483f, 7.15f, -4.122f, 0.0f);
        this.button_red.setScale(0.4f, 0.25f, 1.0f);
        this.button_red.setRotate(-55.0f, 0.0f, 0.0f);
        this.button_blue = new Effect(1542, -11.483f, 7.156f, -4.122f, 0.0f);
        this.button_blue.setScale(0.4f, 0.25f, 1.0f);
        this.button_blue.setRotate(-55.0f, 0.0f, 0.0f);
        new Uwamono(28672, -8.2f, -1.0f, -8.6f, 0.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 228);
        this.K1 = new Uwamono(58, 48, this.item01);
        this.K1.SetGravity(true);
        this.K1.SetCallNo(1);
        this.K2 = new Uwamono(48, 48);
        this.K2.SetGravity(true);
        this.K2.SetCallNo(2);
        this.K3 = new Uwamono(45, 48);
        this.K3.SetGravity(true);
        this.K3.SetCallNo(3);
        this.K4 = new Uwamono(44, 48);
        this.K4.SetCallNo(4);
        this.K5 = new Uwamono(59, 48);
        this.K5.SetGravity(true);
        this.K5.SetCallNo(5);
        this.K6 = new Uwamono(47, 48);
        this.K6.SetGravity(true);
        this.K6.SetCallNo(6);
        this.K7 = new Uwamono(46, 48);
        this.K7.SetGravity(true);
        this.K7.SetCallNo(7);
        this.K8 = new Uwamono(7, 48);
        this.K8.SetCallNo(8);
        this.itembox = new Uwamono(28677, 7.49f, 0.0f, -6.36f, 180.0f, 243);
        this.itembox.SetSymbol(28684);
        this.npc1 = new NPC_NORMAL(1598, 1, 0, 14, 14, -0.07f, 0.0f, -3.1f, 0.0f);
        this.npc2 = new NPC_NORMAL(1571, 2, 0, 14, 17, -12.2f, 0.5f, 1.13f, 120.0f);
        this.npc3 = new NPC_NORMAL(1552, 3, 0, 14, 16, -1.27f, 0.0f, -1.67f, 180.0f);
        this.npc4 = new NPC_NORMAL(1599, 4, 0, 14, 13, 9.32f, 0.0f, 0.97f, 120.0f);
        this.npc5 = new NPC_NORMAL(1537, 5, 0, 14, 15, 7.33f, 0.0f, 4.51f, 45.0f);
        this.npc6 = new NPC_NORMAL(1591, 6, 0, 14, 15, 8.45f, 0.0f, 4.44f, 310.0f);
        this.npc7 = new NPC_NORMAL(1576, 7, 0, 14, 12, -8.38f, 0.0f, -2.36f, 0.0f);
        this.npc8 = new NPC_NORMAL(1543, 8, 0, 14, 15, -5.38f, 0.0f, 2.48f, 240.0f);
        this.npc9 = new NPC_NORMAL(527, 9, 0, 14, 15, -4.71f, 0.0f, 4.08f, 45.0f);
        this.npc10 = new NPC_NORMAL(1595, 10, 0, 7, 12, -2.36f, 0.0f, 1.85f, 45.0f);
        this.npc11 = new NPC_NORMAL(1593, 11, 0, 14, 27, -12.46f, 0.2f, 4.2f, 45.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.enableDTKFlag(262144);
        this.npc1.setMotion(0, 9);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 4);
        this.npc2.setInvalidID(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 29);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 30);
        this.npc5.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 28);
        this.npc6.setInvalidID(1);
        this.npc7.setMotion(0, 10);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 28);
        this.npc8.setInvalidID(1);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 30);
        this.npc9.setInvalidID(1);
        this.npc10.setMotion(0, 10);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 14);
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(8);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc25 = new NPC_NORMAL(1593, 25, 0, 14, 27, -0.06f, 0.0f, -2.5f, 0.0f);
        this.npc25.setInvalidID(1);
        this.npc25.setVisible(false);
        this.npc25.talkto("Talk_npc1");
        if (Runtime.getFlags(347, 1) != 1 && Runtime.getFlags(346, 1) == 1) {
            this.npc20 = new NPC_NORMAL(263, 20, 0, 14, 13, 9.87f, 0.0f, 1.71f, 180.0f);
            this.npc20.disableDTKFlag(1);
            this.npc20.enableDTKFlag(262144);
            this.npc20.setMotion(0, 29);
            this.npc20.setInvalidID(1);
            this.npc20.disableDTKFlag(8);
            this.npc20.talkto("Talk_npc20");
        }
        if (Runtime.getFlags(3156, 1) != 1 && Runtime.getFlags(3155, 1) == 1) {
            this.npc21 = new NPC_NORMAL(1615, 21, 0, 14, 29, 1.87f, 0.0f, -1.87f, 180.0f);
            this.npc21.disableDTKFlag(3);
            this.npc21.enableDTKFlag(4);
            this.npc21.setMotion(0, 28);
            this.npc21.setInvalidID(1);
            this.npc21.disableDTKFlag(8);
            this.npc21.talkto("Talk_npc21");
        }
        this.fanL = new Mapunits();
        this.fanL.mapUnit(69);
        this.fanL.start(4, null);
        this.fanR = new Mapunits();
        this.fanR.mapUnit(18);
        this.fanR.start(4, null);
        this.fanR.start(1, "idle");
        if (this.SAKU == 0) {
            this.button_red.disp(true);
            this.button_blue.disp(false);
            this.player.setID(1);
        } else {
            this.button_red.disp(false);
            this.button_blue.disp(true);
            this.player.setID(2);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            while (true) {
                ST2040.this.fanR.setRotateY(f * 3.0f);
                ST2040.this.fanL.setRotateY(f * 3.0f);
                if ((f += 1.0f) == 360.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
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

