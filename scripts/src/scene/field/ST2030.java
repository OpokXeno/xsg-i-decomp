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
import xeno.map.MC_KUK03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2030
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK03_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Unit crank;
    Unit yane;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int trap_msg = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono trap;
    boolean EnterCheck = false;
    Light light = new Light(0);
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect singouki_blue;
    Effect singouki_red;
    Effect kemuri1;
    Effect kemuri2;
    Effect lamp1;
    Effect lamp2;
    Effect lamp3;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten1_2;
    int CRANK = Runtime.getFlags(6042, 1);
    int A_OR_B = Runtime.getFlags(6048, 1);
    int page;
    String[] NPC_TALK1 = new String[]{"Where would be good?\n", "/[waitkey(64)]/[close()]"};
    String[] NPC_TALK2 = new String[]{"Please don't look for me.\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Turn the crank?\n", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Man, this is exhausting. That old man's a slave driver. He could let me rest a little!", "/[waitkey(1)]/[clear()]", "You an outsider?", "/[waitkey(1)]/[clear()]", "This Foundation is comprised of people like us with special powers and physical abilities. I hear people call us the victims of the Life Recycling Act, but we've never thought of ourselves as victims.", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "Everyone has a place in society.", "/[waitkey(1)]/[clear()]", "If you think of it that way, these powers aren't all that bad to have.", "/[waitkey(1)]/[clear()]", "Everyone already acknowledges the contributions Jr.'s crew on the Durandal have made to the Federation since the war.", "/[waitkey(1)]/[clear()]", "They've exterminated a lot of Gnosis too.", "/[waitkey(64)]/[close()]"};
    String[] msg10003 = new String[]{"/[label()]", "Huh? What's my special power?", "/[waitkey(1)]/[clear()]", "I can tell an object's mass instantly. Well? Sounds perfect for my line of work, right?", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "This bar's been done in poor taste. Everything is so tacky...", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "Hey, I heard that the bartender is actually a member of Director Gaignun's fan club. I wonder if his tastes run along those lines?", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Hey! Quit chitchatting and get to work! Who are you people? You're from the Durandal, aren't you?", "/[waitkey(1)]/[clear()]", "Do you work on the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label()]", "You sure are lucky. We better try harder so we can work there too. Come on! Work! Work harder!!", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Oh, are you going to stay at this hotel?", "/[waitkey(1)]/[clear()]", "This hotel has been losing customers to the new super high-rise hotel that was just built in Sector 36.\nIt used to be run by the happiest couple on the\nFoundation, and the hotel had the best service.", "/[waitkey(64)]/[close()]"};
    String[] msg50002 = new String[]{"/[label()]", "But a couple years ago, the wife died. And ever since then, the old man's totally lost his spirit.", "/[waitkey(1)]/[clear()]", "They say he sits and cries in front of a family photo all the time.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Oh, a customer? This hotel isn't very popular.", "/[waitkey(1)]/[clear()]", "Well, I guess it can't be helped that most of the tourists go to the other side of the city.", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "You know, they have a huge safe in the middle of a shop even though they probably aren't making any money. And rumor has it that there's an enormous fortune inside!", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "Hey, did you hear? They say Gaignun has a love child! That's what my mom told me yesterday. ", "And I heard his kid doesn't age at all! Mom was saying that maybe it's\na cyborg!", "/[waitkey(1)]/[clear()]", "I bet Gaignun's kid is cool!", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Hello! What do you think of City Sector 27? Pretty different atmosphere, right? It was created in the image of the good old days back on planet Earth, which no longer exists.", "/[waitkey(64)]/[close()]"};
    String[] msg90002 = new String[]{"/[label()]", "Apparently, there are authentic building materials used in this block that Directors Gaignun and Jr. collected from real buildings of that time.", "/[waitkey(1)]/[clear()]", "But I wonder if that's true?", "/[waitkey(64)]/[close()]"};
    String[] msgHOTEL1 = new String[]{"/[label()]", "What? A hotel? There's one right over there. The building with the sign that says, \"OUR TREASURE\" is the hotel.", "/[waitkey(64)]/[close()]"};
    String[] msgHOTEL2 = new String[]{"/[label()]", "Did you find the hotel? It has a sign on it that says, \"OUR TREASURE,\" so it's hard to miss.", "/[waitkey(64)]/[close()]"};
    String[] msg100001 = new String[]{"/[label()]", "You guys tourists?\n", "/[waitkey(1)]/[clear()]", "There are a lot of thugs in this sector, and the leader's name is King. He claims to be diabolical, so you better be careful.", "/[waitkey(64)]/[close()]"};
    String[] msg100002 = new String[]{"/[label()]", "This King guy rages about and calls himself, \"The Real Man Behind the Foundation.\" But he can't even tame his own pet, so I wouldn't give it much thought.", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "The bakery in the next block over is really popular. But sometimes King's men are watching the place.", "/[waitkey(1)]/[clear()]", "I wonder if they're doing something bad?", "/[waitkey(64)]/[close()]"};
    String[] msg110002 = new String[]{"/[label()]", "Oh, don't worry. There's nothing strange in it. I guarantee it'll taste good.", "/[waitkey(64)]/[close()]"};
    String[] msg120001 = new String[]{"/[label()]", "Oh, the bartender will get angry if you mess with that crank. The bartender's really strong. He'd probably beat you down in one punch.", "/[waitkey(64)]/[close()]"};
    String[] msg130001 = new String[]{"/[label()]", "What? I'm not scrounging for trash. I'm recycling, okay? I'm working!", "/[waitkey(64)]/[close()]"};
    String[] msg130002 = new String[]{"/[label()]", "Hey, there's work going on in here! Don't come in, it's dangerous.", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 1.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 1.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 1, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};
    String[] msg210001 = new String[]{"/[label()]", "Hey, don't just come up to me like that! I'm doing dangerous work here! If someone like you touched it, something terrible could happen!", "/[waitkey(1)]/[clear()]", "This is a trap to stop the enemy's skills. This type is especially effective against poison skills. Got it?! Even a nitwit like you can understand what I'm saying, right?", "/[waitkey(1)]/[clear()]", "Well if you really do understand, then hurry up and move out of the way!", "/[waitkey(64)]/[close()]"};
    String[] msg210002 = new String[]{"/[label()]", "Oh, now you've done it. I warned you not to touch it.\nOh, but it's strange...I feel really happy.", "/[waitkey(1)]/[clear()]", "I feel like...we could become really good friends.", "/[waitkey(64)]/[close()]"};

    ST2030() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.914f, 4.374f, 0.771f);
        this.camEV.setRotate(-7.642f, -50.279f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.EnterCheck) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py > 0.5f) {
                return;
            }
            System.println("クランク");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Q1, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Turn\nDon't turn\n");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            if (this.selected == 0) {
                this.cam0.setMode(-1);
                this.EV_Camera01();
                System.sleep(30);
                float f = 1.0f;
                float f2 = 15.0f;
                float f3 = 720.0f;
                float f4 = f3 / f2;
                if (this.CRANK == 0) {
                    float f5 = 0.0f;
                    while (f5 <= f3) {
                        Sound.effectPlay(196742);
                        float f6 = 3.975f - 0.495f / f4 * f;
                        float f7 = -6.15f + 1.85f / f4 * f;
                        this.crank.setRotateY(f5);
                        this.yane.setTranslate(13.75f, f6, f7);
                        System.sleep(1);
                        f += 1.0f;
                        f5 += f2;
                    }
                    this.yane.setTranslate(13.75f, 3.48f, -4.3f);
                    this.player.setID(2);
                    Runtime.setFlags(6042, 1, 1);
                    this.CRANK = Runtime.getFlags(6042, 1);
                } else {
                    int n3 = 0;
                    while ((float) n3 <= f3) {
                        Sound.effectPlay(196742);
                        float f8 = 3.48f + 0.495f / f4 * f;
                        float f9 = -4.3f - 1.85f / f4 * f;
                        this.crank.setRotateY(-n3);
                        this.yane.setTranslate(13.75f, f8, f9);
                        System.sleep(1);
                        f += 1.0f;
                        n3 = (int) ((float) n3 + f2);
                    }
                    this.yane.setTranslate(13.75f, 3.975f, -6.15f);
                    this.player.setID(1);
                    Runtime.setFlags(6042, 1, 0);
                    this.CRANK = Runtime.getFlags(6042, 1);
                }
            }
            this.EnterCheck = false;
            System.sleep(30);
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
        } else if (n2 == 1) {
            System.println("serial = 1");
            this.player.getTranslate();
            if (this.player.py > 7.0f) {
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
                System.sleep(16);
                Runtime.setPlayerControl(true);
            }
        } else if (n2 == 2) {
            Runtime.setPlayerControl(false);
            this.EnterCheck = true;
            System.println("サブルート扉見つけた");
            if (Runtime.getFlags(3201, 1) == 0) {
                Runtime.setFlags(3201, 1, 1);
                Sound.effectPlay(55);
                this.nwin(this.sub_01);
            } else if (Runtime.getFlags(3221, 1) == 0) {
                this.nwin(this.sub_02);
            } else if (Runtime.getFlags(3281, 1) == 0) {
                Sound.effectPlay(56);
                this.nwin(this.sub_03);
                this.doorA.SetDoorType('\u0004');
                Runtime.setFlags(3281, 1, 1);
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
        } else if (n2 == 3) {
            if (Runtime.getFlags(7156, 1) == 0) {
                return;
            }
            Runtime.mailArriveSet(76);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 15);
            this.win.print(this.msgMAIL1, 0);
            Runtime.setFlags(7157, 1, 1);
            Runtime.setFlags(7156, 1, 0);
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
        } else if (n2 == 4 && !this.EnterCheck) {
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            ++this.talkFlag13;
            switch (this.talkFlag13) {
                case 1: {
                    this.npc13.kickEnepc(9, 100);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msg130001, 0);
                    ST2030.waitPage(this.win, 64);
                    this.npc13.kickEnepc(9, -1);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            this.npc13.kickEnepc(9, 100);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg130002, 0);
            ST2030.waitPage(this.win, 64);
            this.npc13.kickEnepc(9, -1);
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            return;
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        this.Talk_npc10_1(window);
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg100001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg100002, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        ++this.talkFlag11;
        switch (this.talkFlag11) {
            case 1: {
                window.print(this.msg110001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg110002, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
        window.print(this.msg120001, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        this.Talk_npc13_1(window);
    }

    void Talk_npc13_1(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                this.npc13.kickEnepc(9, 100);
                window.print(this.msg130001, 0);
                ST2030.waitPage(window, 64);
                this.npc13.kickEnepc(9, -1);
                return;
            }
        }
        this.npc13.kickEnepc(9, 100);
        window.print(this.msg130002, 0);
        ST2030.waitPage(window, 64);
        this.npc13.kickEnepc(9, -1);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                this.npc1.kickEnepc(9, 100);
                window.print(this.msg10001, 0);
                ST2030.waitPage(window, 64);
                this.npc1.kickEnepc(9, -1);
                return;
            }
            case 2: {
                this.npc1.kickEnepc(9, 100);
                window.print(this.msg10002, 0);
                ST2030.waitPage(window, 64);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.npc1.kickEnepc(9, 100);
        window.print(this.msg10003, 0);
        ST2030.waitPage(window, 64);
        this.npc1.kickEnepc(9, -1);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    public void Talk_npc21(Enepc enepc, Window window) {
        this.Talk_npc21_1(window);
    }

    void Talk_npc21_1(Window window) {
        switch (this.trap_msg) {
            case 1: {
                window.print(this.msg210002, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg210001, 0);
        ST2030.waitPage(window, 64);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg40001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg40002, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg50001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg50002, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg60001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg60002, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2030.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        if (Runtime.getFlags(7123, 2) == 1) {
            ++this.talkFlag9;
            switch (this.talkFlag9) {
                case 1: {
                    window.print(this.msgHOTEL1, 0);
                    ST2030.waitPage(window, 64);
                    return;
                }
                case 2: {
                    window.print(this.msg90002, 0);
                    ST2030.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msgHOTEL2, 0);
            ST2030.waitPage(window, 64);
            return;
        }
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg90001, 0);
                ST2030.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg90002, 0);
        ST2030.waitPage(window, 64);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                ++this.trap_msg;
                this.npc21.setMotion(0, 3);
                break;
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
                System.println("外観街２・１");
                Runtime.jumpCF(2070, 1);
                break;
            }
            case 1: {
                System.println("外マップ");
                Runtime.jumpCF(2130, 2);
                break;
            }
            case 2: {
                System.println("宿屋１Ｆ・１");
                Runtime.jumpCF(2050, 1);
                break;
            }
            case 3: {
                System.println("酒場１Ｆ・１");
                Runtime.jumpCF(2040, 1);
                break;
            }
            case 4: {
                System.println("酒場２Ｆ・２");
                Runtime.jumpCF(2060, 2);
                break;
            }
            case 5: {
                System.println("酒場２Ｆ・３");
                Runtime.jumpCF(2040, 3);
                break;
            }
            case 6: {
                System.println("外観街２・２");
                Runtime.jumpCF(2070, 2);
                break;
            }
            case 7: {
                System.println("サブルート MC_KUK14");
                Runtime.jumpCF(2140, 1);
                break;
            }
            case 8: {
                System.println("宿屋３Ｆ・３");
                Runtime.jumpCF(2060, 3);
                break;
            }
        }
    }

    void init() {
        this.singouki_blue = new Effect(1542, 0.1795f, 3.725f, 5.005f, 270.0f);
        this.singouki_blue.setScale(0.275f, 0.275f, 0.275f);
        this.singouki_blue.setRotate(0.0f, 270.0f, 0.0f);
        this.singouki_blue.setClip(true);
        this.singouki_red = new Effect(1539, 0.1795f, 3.425f, 5.005f, 270.0f);
        this.singouki_red.setScale(0.275f, 0.275f, 0.275f);
        this.singouki_red.setRotate(0.0f, 270.0f, 0.0f);
        this.singouki_red.setClip(true);
        this.kemuri1 = new Effect(1515, -20.5f, 11.5f, -5.6f, 0.0f);
        this.kemuri1.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri1.setClip(true);
        this.kemuri2 = new Effect(1515, -5.75f, 11.5f, -5.6f, 0.0f);
        this.kemuri2.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri2.setClip(true);
        this.lamp1 = new Effect(1405, -14.689f, 4.375f, -1.344f, 0.0f);
        this.lamp1.setScale(0.8f, 0.8f, 0.8f);
        this.lamp1.setClip(true);
        this.lamp2 = new Effect(1405, -5.539f, 4.375f, 7.356f, 0.0f);
        this.lamp2.setScale(0.8f, 1.0f, 0.8f);
        this.lamp2.setClip(true);
        this.lamp3 = new Effect(1405, 4.261f, 4.375f, -1.344f, 0.0f);
        this.lamp3.setScale(0.8f, 1.0f, 0.8f);
        this.lamp3.setClip(true);
        Stage.setColor(1.15f, 1.15f, 1.15f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(1, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 2, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 3, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
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
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(5, false);
        Stage.setVisible(100, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.npc1 = new NPC_NORMAL(1561, 1, 0, 14, 18, 7.24f, 1.7f, 0.48f, 20.0f);
        this.npc2 = new NPC_NORMAL(1591, 2, 0, 14, 11, 11.85f, 0.0f, -1.39f, 40.0f);
        this.npc3 = new NPC_NORMAL(1592, 3, 0, 14, 14, 12.63f, 0.0f, -0.95f, 250.0f);
        this.npc4 = new NPC_NORMAL(1549, 4, 0, 14, 16, 6.3f, 0.0f, 1.71f, 160.0f);
        this.npc5 = new NPC_NORMAL(1579, 5, 0, 14, 17, -5.91f, 0.0f, -4.63f, 200.0f);
        this.npc6 = new NPC_NORMAL(1567, 6, 0, 14, 11, -11.39f, 0.0f, -3.86f, 20.0f);
        this.npc7 = new NPC_NORMAL(1585, 7, 0, 7, 15, -12.74f, 0.0f, 2.29f, 0.0f);
        this.npc9 = new NPC_NORMAL(1567, 9, 0, 7, 11, 20.91f, 0.0f, 2.86f, 0.0f);
        this.npc10 = new NPC_NORMAL(1543, 10, 0, 14, 18, 16.43f, -0.1f, 7.5f, 270.0f);
        this.npc11 = new NPC_NORMAL(527, 11, 0, 14, 18, -13.4f, -0.1f, 7.3f, 0.0f);
        this.npc12 = new NPC_NORMAL(1585, 12, 0, 14, 15, 8.87f, 0.0f, -4.36f, 0.0f);
        this.npc13 = new NPC_NORMAL(1561, 13, 0, 14, 13, -17.22f, 0.0f, -2.63f, 180.0f);
        this.npc10.setInvalidID(1);
        this.npc11.setInvalidID(1);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.enableDTKFlag(262144);
        this.npc1.setMotion(0, 6);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 13);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 9);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 10);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 9);
        this.npc7.setMotion(0, 10);
        this.npc9.setMotion(0, 16);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 4);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.setMotion(0, 10);
        this.npc13.disableDTKFlag(3);
        this.npc13.enableDTKFlag(4);
        this.npc13.enableDTKFlag(262144);
        this.npc13.setMotion(0, 10);
        this.npc21 = new NPC_NORMAL(1591, 1, 0, 14, 18, -2.47f, 0.0f, 0.17f, 200.0f);
        this.npc21.disableDTKFlag(3);
        this.npc21.enableDTKFlag(4);
        this.npc21.enableDTKFlag(262144);
        this.npc21.setMotion(0, 1);
        this.npc21.talkto("Talk_npc21");
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        this.npc13.talkto("Talk_npc13");
        this.npc20 = new NPC_NORMAL(1561, 1, 0, 14, 18, 7.24f, 0.0f, 1.18f, 0.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.talkto("Talk_npc1");
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.5f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFPedestal(5, -0.7949745f, 8.467842f, 12.857608f, 40.0f, -12.077396f, 40.858994f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFPedestal(6, 4.723863f, 8.307665f, 12.93971f, 37.11962f, -10.071769f, -34.093002f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.015f, 0.015f);
        this.cam0.setCFAngle(9, -28.0f, 25.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, -21.341139f, 5.334243f, 4.9919443f, 46.565556f, -23.526102f, -30.57156f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        new Uwamono(178, 33);
        new Uwamono(179, 33);
        new Uwamono(180, 22);
        if (Runtime.getFlags(3201, 1) == 0) {
            new Uwamono(110, 31);
        } else {
            Stage.setVisible(110, false);
        }
        this.trap = new Uwamono(28675, -2.7f, 0.0f, -0.5f, 180.0f);
        this.trap.SetCallNo(1);
        this.crank = new Unit();
        this.crank.mapUnit(10);
        this.crank.start(4, null);
        this.yane = new Unit();
        this.yane.mapUnit(86);
        this.yane.start(4, null);
        this.yane.setRotate(15.0f, 0.0f, 0.0f);
        if (this.CRANK != 0) {
            this.yane.setTranslate(13.75f, 3.48f, -4.3f);
            this.player.setID(2);
        }
        this.doorA = new Uwamono(115, 40, '\u0004');
        if (Runtime.getFlags(3281, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(7108, 1) == 0) {
            this.doorB = new Uwamono(164, 40, '\u0002');
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB = new Uwamono(164, 40, '\u0002');
            this.doorB.SetDoorType('\u0004');
        }
        this.teiten1_2 = new Uwamono(28690, 9.5f, 0.0f, 0.5f, 0.0f);
        this.teiten1_2.SetBgm(196611);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2030.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2030.waitPage(this.win, 64);
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    void yesno() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
    }

    class Player
            extends Chr {
        Player() {
        }

        void init() {
            this.setPlayer();
            this.setShadow(4, 16);
            this.setEdgeFall(1);
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

