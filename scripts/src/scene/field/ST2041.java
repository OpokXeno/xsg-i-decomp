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

class ST2041
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK04_PRJ {
    int SAKU = Runtime.getFlags(6041, 1);
    Player player;
    Camera cam0;
    Camera cam1;
    Camera camEV;
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
    Enepc npc21;
    Enepc npc25;
    Enepc boss;
    Effect obj600;
    Light light = new Light(0);
    Effect button_red;
    Effect button_blue;
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
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    int page;
    String[] NPC1_TALK1 = new String[]{"Which one would you like?\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"There's a switch.\n", "/[waitkey(64)]/[close()]"};
    String[] Q2 = new String[]{"Will you fight?", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Yo, you guys! If you train regularly, you'll be ready when trouble appears!", "/[waitkey(1)]/[clear()]", "You should learn from me and workout too!", "/[waitkey(1)]/[clear()]", "Gwa ha ha ha ha ha!", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "If everyone's fixing the shop, it would be great if they could fix the piano too while they're at it.", "/[waitkey(1)]/[clear()]", "I wonder how long they plan on leaving it broken like this.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "Good grief. Ever since the Gnosis attacked us, the bartender's stories seem to have gotten longer.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Hmm, what did I do wrong? I thought my body, pose, and spirit were all perfect.", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label()]", "Guess I still have a ways to go...", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Listen to this! My girlfriend was evacuated to the Durandal, and she hasn't come back. Don't you think\nit's horrible of her to run off and leave me behind?", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Wow! Awesome! I want to become strong like the bartender too!", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "I was worried when the Gnosis came and destroyed the city, ", "but nothing good will come from worrying about it.", "/[waitkey(1)]/[clear()]", "Smiles are really important at times like these!", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label()]", "The bartender's awesome! He said he defeated a Gnosis with his bare hands!", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Man, that was hell! But we can't just let something like that get us down!", "/[waitkey(64)]/[close()]"};
    String[] msg90002 = new String[]{"/[label()]", "We should have a drink, cheer up, and then put the town back together!", "/[waitkey(64)]/[close()]"};
    String[] msg100001 = new String[]{"/[label(Mayumi)]", "Welcome! The town's in shambles, but the inside is back to normal, thanks to our regulars!", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "*Sniff...sniff...*\n", "Oh, how is it that I'm so unlucky? I can't believe I lost the precious \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" that my fianc#28 gave me...", "/[waitkey(1)]/[clear()]", "If he finds out that I lost the \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" I'm sure he'll be so upset that he'll toss me out like garbage.", "/[waitkey(1)]/[clear()]", "*Sob...*\n", "If only I hadn't dropped the ring on the beach. If only the fish hadn't taken the ring I dropped in its mouth.\nIf only the fish with the ring hadn't escaped to Director Gaignun's private beach...", "/[waitkey(1)]/[clear()]", "I'd still be happy...\n", "*Sob...sniff...*", "/[waitkey(64)]/[close()]"};
    String[] msg110002 = new String[]{"/[label()]", "*Sniff...*\n", "*Sob... sob...*\n", "/[waitkey(1)]/[clear()]", "What should I do? The fish with the \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]\" should be in the waters of Director Gaignun's private beach.", "/[waitkey(1)]/[clear()]", "Someone...anyone...please help me...!", "/[waitkey(64)]/[close()]"};
    String[] msg110003 = new String[]{"/[label()]", "?!", "/[waitkey(1)]/[clear()]", "T-this is...my \"/[color(0x329bbe)]Engagement Ring/[color(0x808080)]!\" You went and found it for me?", "/[waitkey(1)]/[clear()]", "*S-sniff...*\n", "*Sob...* T-thank you...now, I can go to him...", "/[waitkey(1)]/[clear()]", "*Sniff...sob...* This is a just token of my appreciation. Please take it. I think it'll come in handy.", "/[waitkey(64)]/[close()]"};
    String[] msg110004 = new String[]{"/[label()]", "*Sniff...sniff...*\n", "I-I will find happiness!\n", "*Sob...sob...*\n", "Thank you.", "/[waitkey(64)]/[close()]"};
    String[] momo_1 = new String[]{"/[label(MOMO)]", "???", "/[waitkey(64)]/[close()]"};
    String[] majyo_1 = new String[]{"/[label(Mintia)]", "Hmm..."};
    String[] majyo_2 = new String[]{"/[label(Mintia)]", "Hmm..."};
    String[] majyo_3 = new String[]{"/[label(Mintia)]", "Good, good."};
    String[] majyo_11 = new String[]{"/[label(Mintia)]", "I-I hate to admit it, but I lost. All right, I'll give you this!", "/[waitkey(64)]/[close()]"};
    String[] majyo_12 = new String[]{"Learned Tech Attack, \"Dark Scepter.\"", "/[waitkey(64)]/[close()]"};
    String[] majyo_13 = new String[]{"/[label(Mintia)]", "That works pretty well against Gnosis if you know how to use it.", "/[waitkey(1)]/[clear()]", "Next time, I'll bring my number one pet. Let's play again, MOMO.", "/[waitkey(64)]/[close()]"};
    String[] msgROBO1 = new String[]{"/[label(Assistant Scott)]", "Damn it! Damn it! Why doesn't the professor understand? Why won't he listen to my opinions?", "/[waitkey(1)]/[clear()]", "I'm...I'm...I'm just worried about the professor, that's all...", "/[waitkey(1)]/[clear()]", "Damn, I need a drink! Damn eccentric, hardheaded\ngeezer! I don't care anymore. He'll regret it later when he's all alone!", "/[waitkey(64)]/[clear()]"};
    String[] msgROBO2 = new String[]{"/[label(Assistant Scott)]", "The professor can't even do anything by himself. He really can't do anything without me.", "/[waitkey(1)]/[clear()]", "He's probably having problems right about now. The professor might even be crying.", "/[waitkey(1)]/[clear()]", "Damn! I know he needs me! I can't just leave the Professor all by himself! Professor, just hang in there. I'm coming to help you!!", "/[waitkey(64)]/[close()]"};
    String[] msgROBO3 = new String[]{"/[label(Assistant Scott)]", "Professor, just hang in there. I promise I will save you!", "/[waitkey(64)]/[close()]"};

    ST2041() {
    }

    void EOB(int n) {
        if (n == 14) {
            Runtime.setFlags(8095, 1, 1);
            Runtime.etherTecSet(8);
            Runtime.setPlayerControl(false);
            System.sleep(1);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_11, 0);
            ST2041.waitPage(this.win, 64);
            Sound.effectPlay(6);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_12, 0);
            ST2041.waitPage(this.win, 64);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.majyo_13, 0);
            ST2041.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.184f, 2.327f, 4.246f);
        this.camEV.setRotate(-11.045f, 10.9f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.67f, 8.279f, 2.232f);
        this.camEV.setRotate(-15.465f, -41.899f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
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
                this.win.setLocation(15, 15);
                this.win.print(this.Q1, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Press\nDon't press\n");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        System.sleep(30);
                        Sound.effectPlay(58);
                        if (Runtime.getFlags(6041, 1) == 0) {
                            this.button_red.disp(false);
                            this.button_blue.disp(true);
                            this.player.setID(2);
                            Runtime.setFlags(6041, 1, 1);
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            System.sleep(30);
                            Sound.effectPlay(196745);
                            this.elv.setArgs(12, 6.0f);
                            System.sleep(90);
                            this.EV_Camera02();
                            System.sleep(90);
                            this.cam0.setMode(0);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            break block0;
                        }
                        this.button_blue.disp(false);
                        this.button_red.disp(true);
                        Runtime.setFlags(6041, 1, 0);
                        this.cam0.setMode(-1);
                        this.EV_Camera02();
                        System.sleep(30);
                        Sound.effectPlay(196745);
                        this.elv.setArgs(12, 0.097f);
                        System.sleep(90);
                        this.EV_Camera01();
                        System.sleep(90);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 1: {
                this.player.getTranslate();
                if (this.player.py < 6.0f) {
                    return;
                }
                if (Runtime.getLeader() != 4 || Runtime.getFlags(8095, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.EnterCheck = true;
                System.println("魔女登場");
                Runtime.enable(65536);
                Runtime.disable(524288);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.momo_1, 0);
                ST2041.waitPage(this.win, 64);
                this.player.move(60, 12.0f, 1.5f, true);
                this.player.mtn(2, 9, 1.0f, true);
                System.sleep(63);
                this.player.mtn(1, 9, 1.0f, true);
                this.player.rotY(10, 180.0f, true);
                System.sleep(10);
                this.boss.kickEnepc(4, 0);
                System.sleep(10);
                Stage.setVisible(137, true);
                this.boss.setVisible(true);
                this.cam0.setMode(-1);
                this.cam1 = Camera.create(1);
                this.cam1.setTranslate(10.6f, 7.4f, 3.3f);
                this.cam1.setRotate(20.0f, -29.3f, -180.0f);
                this.cam1.setFov(40.0f);
                this.cam1.change();
                this.boss.kickEnepc(4, 2);
                System.sleep(30);
                this.boss.kickEnepc(4, 0);
                Sound.effectPlay(196747);
                System.sleep(25);
                float[] fArray = new float[8];
                fArray[2] = 3.0f;
                fArray[3] = 10.0f;
                fArray[4] = 40.0f;
                fArray[5] = -7.9f;
                fArray[6] = 3.0f;
                fArray[7] = 6.1f;
                float[] fArray2 = fArray;
                this.cam1.setTranslate(10.6f, 7.4f, 3.3f);
                float[] fArray3 = new float[8];
                fArray3[1] = 20.0f;
                fArray3[2] = -29.3f;
                fArray3[3] = -180.0f;
                fArray3[4] = 40.0f;
                fArray3[5] = -0.5f;
                fArray3[6] = -29.3f;
                float[] fArray4 = fArray3;
                this.cam1.rotateSPL(fArray4, 0, 1, 40);
                System.sleep(200);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_1, 0);
                System.sleep(60);
                this.win.close();
                System.sleep(40);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_2, 0);
                System.sleep(60);
                this.win.close();
                System.sleep(80);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.majyo_3, 0);
                System.sleep(50);
                this.win.close();
                System.sleep(10);
                this.boss.kickEnepc(4, 2);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Q2, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo\n");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.boss.kickEnepc(14, 0);
                        Runtime.enable(524288);
                        this.cam0.setMode(0);
                        Stage.setVisible(137, false);
                        Runtime.disable(65536);
                        this.EnterCheck = false;
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.jumpCF(2171, 6);
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
        ST2041.waitPage(window, 64);
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
                    ST2041.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg110002, 0);
            ST2041.waitPage(window, 64);
            return;
        }
        if (Runtime.checkItem(10, 23) == 0) {
            ++this.talkFlag11;
            switch (this.talkFlag11) {
                case 1: {
                    window.print(this.msg110003, 0);
                    ST2041.waitPage(window, 64);
                    Runtime.removeItem(10, 72);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 23);
                    Runtime.setFlags(3228, 1, 1);
                    return;
                }
            }
            window.print(this.msg110004, 0);
            ST2041.waitPage(window, 64);
            return;
        }
        window.print(this.msg110004, 0);
        ST2041.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg10001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    public void Talk_npc21(Enepc enepc, Window window) {
        this.Talk_npc21_1(window);
    }

    void Talk_npc21_1(Window window) {
        ++this.talkFlag21;
        switch (this.talkFlag21) {
            case 1: {
                window.print(this.msgROBO1, 0);
                ST2041.waitPage(window, 64);
                Runtime.setFlags(3156, 1, 1);
                System.println("フラグオン！");
                window.print(this.msgROBO2, 0);
                ST2041.waitPage(window, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(2110, 1);
                return;
            }
        }
        window.print(this.msgROBO3, 0);
        ST2041.waitPage(window, 64);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg40001, 0);
                ST2041.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg40002, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg60001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        this.npc8.kickEnepc(1, 10);
        window.print(this.msg80001, 0);
        ST2041.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg90001, 0);
                ST2041.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg90002, 0);
        ST2041.waitPage(window, 64);
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
                Runtime.jumpCF(2171, 4);
                break;
            }
            case 1: {
                System.println("宿屋２Ｆ・４");
                Runtime.jumpCF(2201, 4);
                break;
            }
            case 2: {
                System.println("外観街１・６");
                Runtime.jumpCF(2171, 6);
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
        this.button_red = new Effect(1539, -11.483f, 7.15f, -4.122f, 0.0f);
        this.button_red.setScale(0.4f, 0.25f, 1.0f);
        this.button_red.setRotate(-55.0f, 0.0f, 0.0f);
        this.button_blue = new Effect(1542, -11.483f, 7.156f, -4.122f, 0.0f);
        this.button_blue.setScale(0.4f, 0.25f, 1.0f);
        this.button_blue.setRotate(-55.0f, 0.0f, 0.0f);
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
        this.obj600 = new Effect(1447, 0);
        this.obj600.disp(true);
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
        if (Runtime.getFlags(6041, 1) == 0) {
            this.button_red.disp(true);
            this.button_blue.disp(false);
            this.player.setID(1);
        } else {
            this.button_red.disp(false);
            this.button_blue.disp(true);
            this.player.setID(2);
        }
        this.npc1 = new NPC_NORMAL(1598, 1, 0, 14, 14, -0.07f, 0.0f, -3.1f, 0.0f);
        this.npc2 = new NPC_NORMAL(1571, 2, 0, 14, 17, -12.2f, 0.5f, 1.13f, 120.0f);
        this.npc3 = new NPC_NORMAL(1552, 3, 0, 14, 16, -1.27f, 0.0f, -1.67f, 180.0f);
        this.npc4 = new NPC_NORMAL(1599, 4, 0, 14, 13, 9.32f, 0.0f, 0.97f, 120.0f);
        this.npc5 = new NPC_NORMAL(1537, 5, 0, 14, 15, 7.33f, 0.0f, 4.51f, 45.0f);
        this.npc6 = new NPC_NORMAL(1585, 6, 0, 14, 32, 0.85f, 0.0f, -1.67f, 200.0f);
        this.npc7 = new NPC_NORMAL(1576, 7, 0, 14, 12, -8.38f, 0.0f, -2.36f, 0.0f);
        this.npc8 = new NPC_NORMAL(1555, 8, 0, 14, 31, -0.6f, 0.0f, -1.69f, 140.0f);
        this.npc9 = new NPC_NORMAL(527, 9, 0, 14, 15, -4.71f, 0.0f, 4.08f, 45.0f);
        this.npc10 = new NPC_NORMAL(1595, 10, 0, 7, 12, -2.36f, 0.0f, 1.85f, 45.0f);
        this.npc11 = new NPC_NORMAL(1593, 11, 0, 14, 25, -12.46f, 0.2f, 4.2f, 45.0f);
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
        this.npc6.setMotion(0, 9);
        this.npc6.setInvalidID(1);
        this.npc7.setMotion(0, 10);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 29);
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
        if (Runtime.getFlags(3156, 1) != 1 && Runtime.getFlags(3155, 1) == 1) {
            this.npc21 = new NPC_NORMAL(1615, 21, 0, 14, 34, 1.87f, 0.0f, -1.87f, 180.0f);
            this.npc21.disableDTKFlag(3);
            this.npc21.enableDTKFlag(4);
            this.npc21.setMotion(0, 28);
            this.npc21.setInvalidID(1);
            this.npc21.disableDTKFlag(8);
            this.npc21.talkto("Talk_npc21");
        }
        if (Runtime.getFlags(8095, 1) == 0) {
            this.boss = new NPC_NORMAL(20254, 14, 0, 14, 27, 0.0f, 0.0f, 0.0f, 0.0f);
            this.boss.kickEnepc(13, 0);
            this.boss.setVisible(1, false);
            this.boss.setVisible(2, false);
            this.boss.setVisible(3, false);
            this.boss.setVisible(4, false);
            this.boss.setInvalidID(1);
            this.boss.setMotion(0, 1);
            this.boss.kickEnepc(4, 2);
            this.boss.setVisible(false);
            this.boss.dispRadar(false);
            this.boss.setBatEvent(12);
            this.boss.setGroup(0, 0, 0, 0);
            this.boss.disableDTKFlag(65536);
        }
        this.fanL = new Mapunits();
        this.fanL.mapUnit(69);
        this.fanL.start(4, null);
        this.fanR = new Mapunits();
        this.fanR.mapUnit(18);
        this.fanR.start(4, null);
        this.fanR.start(1, "idle");
        if (this.SAKU == 0) {
            this.player.setID(1);
        } else {
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
                ST2041.this.fanR.setRotateY(f * 3.0f);
                ST2041.this.fanL.setRotateY(f * 3.0f);
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

