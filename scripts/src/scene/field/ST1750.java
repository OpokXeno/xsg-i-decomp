import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_DYU05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1750
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU05_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
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
    Enepc All;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit Kidou;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
    int npc3talked = 0;
    int npc3btalked = 0;
    int npc4talked = 0;
    int npc4btalked = 0;
    int npc5talked = 0;
    int npc5btalked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    int npc11talked = 0;
    int npc22talked = 0;
    int npc33talked = 0;
    int npc44talked = 0;
    int npc55talked = 0;
    int shishi = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono BASE01;
    Uwamono BASE02;
    Uwamono BASE03;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] POL_00 = new String[]{"This ship itself is about as large as a colony.", "/[waitkey(1)]/[clear()]", "The Foundation is unique in that it is far safer compared to your average city, but it still requires policemen like us.", "/[waitkey(64)]/[close()]"};
    String[] POL_01 = new String[]{"Well, our work most often consists of intervening in fights between drunks and such...", "/[waitkey(64)]/[close()]"};
    String[] POL_02 = new String[]{"Oh, Little Master, are you making rounds too?", "/[waitkey(64)]/[close()]"};
    String[] PX_00 = new String[]{"Oh, is this your companion, Little Master? Director Gaignun told me already.", "/[waitkey(1)]/[clear()]", "A room down the corridor has been prepared for them.", "/[waitkey(64)]/[close()]"};
    String[] PX_01 = new String[]{"It will be a little while before we arrive at the Foundation.", "/[waitkey(1)]/[clear()]", "Please relax and get some rest.", "/[waitkey(64)]/[close()]"};
    String[] CREW_00 = new String[]{"Shelley and Mary are great, you know.", "/[waitkey(1)]/[clear()]", "I like how dignified Shelley is, but I can't just dismiss how much I like Mary's enthusiasm either.", "/[waitkey(64)]/[close()]"};
    String[] CREW_01 = new String[]{"Maybe I should join the fan club after all.", "/[waitkey(64)]/[close()]"};
    String[] CREW_02 = new String[]{"But there are rumors that the both of them are Director Gaignun's girlfriends.", "/[waitkey(1)]/[clear()]", "It doesn't matter, I still love them!!", "/[waitkey(64)]/[close()]"};
    String[] CREW_03 = new String[]{"Oh, Little Master, please pardon me! I would like to ask you a question.", "/[waitkey(1)]/[clear()]", "Um, what is the relationship between Director Gaignun and Shelley and Mary?", "/[waitkey(64)]/[close()]"};
    String[] CREW_04 = new String[]{"Oh, if you can't tell me that, then what about their private email addresses?", "/[waitkey(64)]/[close()]"};
    String[] CREW_05 = new String[]{"At least tell me their measurements?", "/[waitkey(64)]/[close()]"};
    String[] PA_00 = new String[]{"The Durandal is a battleship, but many civilians also work here. It's not that dangerous since Shelley and Mary are in command -- they don't do too many reckless things.", "/[waitkey(1)]/[clear()]", "Oh, but when Little Master is with us, we all brace ourselves.", "/[waitkey(64)]/[close()]"};
    String[] PA_01 = new String[]{"Oh, Little Master! I'm prepared to give my life for you at any time!", "/[waitkey(1)]/[clear()]", "But...um, I do have a family that is waiting for me at home, so, please don't do anything too reckless.", "/[waitkey(64)]/[close()]"};
    String[] PB_00 = new String[]{"Considering the duties, it can't be helped, but a lot of the work here is tough for a normal person.", "/[waitkey(1)]/[clear()]", "The principle crew members all have special abilities.", "/[waitkey(1)]/[clear()]", "I think it's called psychokinesis or telepathy or something. Man, I want powers like that too!", "/[waitkey(64)]/[close()]"};
    String[] PB_01 = new String[]{"A normal human being doesn't stand a chance against the strength and concentration of Realians. Maybe I should get permission from the government and get\nan implant too.", "/[waitkey(64)]/[close()]"};
    String[] PB_02 = new String[]{"What? What's an implant?", "/[waitkey(1)]/[clear()]", "Let's see...", "/[waitkey(1)]/[clear()]", "Well, it's a general term that refers to a physical modification like connecting organic memory systems to cerebral nerves to aid your memory, or artificially control the production of dopamine and acetylcholine.", "/[waitkey(1)]/[clear()]", "It's also possible to implant nanomachine plants in the body to break down lactic acid or to purify chemical substances.", "/[waitkey(1)]/[clear()]", "You have to get permission unless you're a soldier or affiliated with the Federation police. It's really difficult to get permission.", "/[waitkey(64)]/[close()]"};
    String[] REAL_00 = new String[]{"Um, would you like to register for the Shelley & Mary Fan Club?", "/[waitkey(1)]/[clear()]", "If you register now, you'll get a real photograph of them!", "/[waitkey(64)]/[close()]"};
    String[] REAL_01 = new String[]{"Um, please, won't you register?", "/[waitkey(1)]/[clear()]", "If I don't persuade one more person this month, I won't hit my quota.", "/[waitkey(64)]/[close()]"};
    String[] REAL_02 = new String[]{"I wasn't able to fulfill my quota last month, either. And at this rate, Little Master is going to get mad at me.", "/[waitkey(64)]/[close()]"};
    String[] REAL_03 = new String[]{"Oh, Little Master?! Um, just a little more! I'll fulfill my quota in just a little bit!!", "/[waitkey(1)]/[clear()]", "...\n", "...\n", "So please don't yell at me...", "/[waitkey(64)]/[close()]"};
    String[] S01_00 = new String[]{"What? Looking for someone? Who are you looking for?", "/[waitkey(64)]/[close()]"};
    String[] S01_01 = new String[]{"His name is Allen. Uh-huh, and his characteristics are:", "/[waitkey(1)]/[clear()]", "a man in his 20s with an unimposing presence that you would find just about anywhere. On the thin side.", "/[waitkey(1)]/[clear()]", "He'll be tough to find, I think.", "/[waitkey(64)]/[close()]"};
    String[] S02_00 = new String[]{"Um, are Shelley and Mary all right?!", "/[waitkey(1)]/[clear()]", "They're not hurt or anything, are they?! Nobody treated them badly, did they?!", "/[waitkey(1)]/[clear()]", "Oh, this is no good! I have to join! I'm going to join the fan club! And I'll protect those two!!", "/[waitkey(64)]/[close()]"};
    String[] S03_00 = new String[]{"Really, why are soldiers so oppressive?", "/[waitkey(1)]/[clear()]", "Using violence against the innocent is despicable!", "/[waitkey(64)]/[close()]"};
    String[] S04_00 = new String[]{"Ugh...", "/[waitkey(1)]/[clear()]", "When these things happen, I get a renewed awareness of just how the Federation government sees us.", "/[waitkey(1)]/[clear()]", "Special abilities? Realians? That shouldn't matter.", "/[waitkey(1)]/[clear()]", "They're far more decent people than those snob council members in the government.", "/[waitkey(64)]/[close()]"};
    String[] S04_01 = new String[]{"Well, Second Miltia's Representative Helmer is different though.", "/[waitkey(1)]/[clear()]", "I wish everyone were like him.", "/[waitkey(1)]/[clear()]", "Man, things were better when the last chairman was around.", "/[waitkey(64)]/[close()]"};
    String[] S05_00 = new String[]{"How about it? Won't you join the Shelley & Mary Fan Club?\n", "/[waitkey(1)]/[clear()]", "If you become a member now, you'll get a charm as a gift from these two girls!", "/[waitkey(1)]/[clear()]", "As long as you have this charm, you'll be fine, even if you get caught by the Federation's soldiers...but uh, um, please listen to what I have to say.", "/[waitkey(64)]/[close()]"};
    String[] ALLEN_00 = new String[]{"/[label(Allen)]", "Well then, I'll be heading to the launch pad. So Chief, make sure to come too!!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1750() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.557f, 2.015f, 28.567f);
        this.camEV.setRotate(-11.751f, 45.819f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3167, 1) != 0) return;
                    if (Runtime.getFlags(346, 1) != 1) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(66);
                    Runtime.setFlags(3167, 1, 1);
                    System.waitFor(this.win);
                    System.sleep(15);
                    Runtime.setPlayerControl(true);
                    Runtime.mailExec(1);
                    return;
                }
            }
            return;
        }
        if (n2 != 2) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(310, 1) != 1) return;
                if (Runtime.mailArriveCheck(44) != 0) return;
                if (Runtime.mailReplyCheck(12) != 2 && Runtime.mailReplyCheck(25) == 1 && Runtime.mailReplyCheck(34) == 3) {
                    if (Runtime.mailReplyCheck(49) != 3) return;
                    if (Runtime.getFlags(3195, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(60);
                    Runtime.setFlags(3195, 1, 1);
                    System.waitFor(this.win);
                    System.sleep(15);
                    Runtime.setPlayerControl(true);
                    Runtime.mailExec(1);
                    return;
                }
                if (Runtime.mailReplyCheck(12) == 2 && Runtime.mailReplyCheck(25) != 1 && Runtime.mailReplyCheck(34) == 3) {
                    if (Runtime.mailReplyCheck(49) != 3) return;
                    if (Runtime.getFlags(3195, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(60);
                    Runtime.setFlags(3195, 1, 1);
                    System.waitFor(this.win);
                    System.sleep(15);
                    Runtime.setPlayerControl(true);
                    Runtime.mailExec(1);
                    return;
                }
                if (Runtime.mailReplyCheck(12) != 2) return;
                if (Runtime.mailReplyCheck(25) != 1) return;
                if (Runtime.mailReplyCheck(34) == 3) return;
                if (Runtime.mailReplyCheck(49) != 3) return;
                if (Runtime.getFlags(3195, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAIL1, 0);
                Runtime.mailArriveSet(60);
                Runtime.setFlags(3195, 1, 1);
                System.waitFor(this.win);
                System.sleep(15);
                Runtime.setPlayerControl(true);
                Runtime.mailExec(1);
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.POL_02, 0);
            System.waitFor(window);
        } else if (this.npc1talked == 0) {
            window.print(this.POL_00, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.POL_01, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC11(Enepc enepc, Window window) {
        if (this.npc11talked == 0) {
            window.print(this.S01_00, 0);
            System.waitFor(window);
            this.npc11talked = 1;
        } else {
            window.print(this.S01_01, 0);
            System.waitFor(window);
            this.npc11talked = 0;
        }
    }

    public void TalkNPC1x(Enepc enepc, Window window) {
        if (this.shishi == 0) {
            window.print(this.PX_00, 0);
            System.waitFor(window);
            this.shishi = 1;
        } else {
            window.print(this.PX_01, 0);
            System.waitFor(window);
            this.shishi = 0;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2btalked == 0) {
                window.print(this.CREW_03, 0);
                System.waitFor(window);
                this.npc2btalked = 1;
            } else if (this.npc2btalked == 1) {
                window.print(this.CREW_04, 0);
                System.waitFor(window);
                this.npc2btalked = 2;
            } else {
                window.print(this.CREW_05, 0);
                System.waitFor(window);
                this.npc2btalked = 0;
            }
        } else if (this.npc2talked == 0) {
            window.print(this.CREW_00, 0);
            System.waitFor(window);
            this.npc2talked = 1;
        } else if (this.npc2talked == 1) {
            window.print(this.CREW_01, 0);
            System.waitFor(window);
            this.npc2talked = 2;
        } else {
            window.print(this.CREW_02, 0);
            System.waitFor(window);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC22(Enepc enepc, Window window) {
        window.print(this.S02_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.PA_01, 0);
            System.waitFor(window);
        } else {
            window.print(this.PA_00, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC33(Enepc enepc, Window window) {
        window.print(this.S03_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (this.npc4talked == 0) {
            window.print(this.PB_00, 0);
            System.waitFor(window);
            this.npc4talked = 1;
        } else if (this.npc4talked == 1) {
            window.print(this.PB_01, 0);
            System.waitFor(window);
            this.npc4talked = 2;
        } else {
            window.print(this.PB_02, 0);
            System.waitFor(window);
            this.npc4talked = 0;
        }
    }

    public void TalkNPC44(Enepc enepc, Window window) {
        if (this.npc44talked == 0) {
            window.print(this.S04_00, 0);
            System.waitFor(window);
            this.npc44talked = 1;
        } else {
            window.print(this.S04_01, 0);
            System.waitFor(window);
            this.npc44talked = 0;
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.REAL_03, 0);
            System.waitFor(window);
        } else if (this.npc5talked == 0) {
            window.print(this.REAL_00, 0);
            System.waitFor(window);
            this.npc5talked = 1;
        } else if (this.npc5talked == 1) {
            window.print(this.REAL_01, 0);
            System.waitFor(window);
            this.npc5talked = 2;
        } else {
            window.print(this.REAL_02, 0);
            System.waitFor(window);
            this.npc5talked = 0;
        }
    }

    public void TalkNPC55(Enepc enepc, Window window) {
        window.print(this.S05_00, 0);
        System.waitFor(window);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1880, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1730, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(1740, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1870, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(1840, 1);
                break;
            }
        }
    }

    void init() {
        int n;
        this.teiten1 = new Uwamono(28690, 3.5f, 0.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, 3.5f, 0.0f, 2.0f, 0.0f);
        this.teiten2.SetBgm(196615);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -13.0f, 0.0f);
        this.teiten3.SetBgm(196616);
        this.teiten4 = new Uwamono(28690, -12.5f, 0.0f, -17.5f, 0.0f);
        this.teiten4.SetBgm(196617);
        this.teiten5 = new Uwamono(28690, 13.5f, 0.0f, 5.2f, 0.0f);
        this.teiten5.SetBgm(196616);
        if (Runtime.getFlags(310, 1) == 1 && Runtime.getFlags(3100, 1) == 0) {
            Runtime.setOutFriend(2);
            Runtime.setOutFriend(6);
            Runtime.setOutFriend(4);
            Runtime.setOutFriend(5);
            Runtime.setOutFriend(3);
            Runtime.setFlags(3100, 1, 1);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 2);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
            System.println("パーティー情報・leader_shion,*,*,*,*,*");
            Runtime.addItem(1, 47);
        }
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(310, 1) == 1 && Runtime.getFlags(3160, 1) == 0) {
            this.Kidou = new Mapunits();
            this.Kidou.mapUnit(66);
            this.Kidou.start(4, null);
            this.Kidou.start(1, "Evt");
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 13.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 0.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
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
        this.npc1 = new NPC_NORMAL(1289, 11, 0, 0, 16, 10.167f, 0.0f, 24.632f, 0.0f);
        this.npc2 = new NPC_NORMAL(1025, 12, 0, 0, 5, 15.418f, 0.0f, 15.0f, 90.0f);
        this.npc3 = new NPC_NORMAL(1540, 13, 0, 0, 7, -2.106f, 0.0f, 3.053f, 90.0f);
        this.npc4 = new NPC_NORMAL(1540, 14, 0, 0, 7, 2.186f, 0.0f, -11.413f, 0.0f);
        this.npc5 = new NPC_NORMAL(780, 15, 0, 0, 7, -15.148f, 0.0f, -11.419f, 45.0f);
        if (Runtime.getFlags(346, 1) == 0) {
            if (Runtime.getFlags(304, 1) == 1 && Runtime.getFlags(308, 1) == 0) {
                this.npc1.talkto("TalkNPC1x");
            } else {
                this.npc1.talkto("TalkNPC1");
            }
            this.npc2.talkto("TalkNPC2");
            this.npc3.talkto("TalkNPC3");
            this.npc4.talkto("TalkNPC4");
            this.npc5.talkto("TalkNPC5");
        } else {
            this.npc1.talkto("TalkNPC11");
            this.npc2.talkto("TalkNPC22");
            this.npc3.talkto("TalkNPC33");
            this.npc4.talkto("TalkNPC44");
            this.npc5.talkto("TalkNPC55");
        }
        this.npc1.disableDTKFlag(131075);
        this.npc2.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(131074);
        this.npc4.disableDTKFlag(131075);
        this.npc5.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(12);
        this.npc2.enableDTKFlag(12);
        this.npc3.enableDTKFlag(12);
        this.npc4.enableDTKFlag(12);
        this.npc5.enableDTKFlag(12);
        this.npc1.setInvalidID(1);
        this.npc4.setInvalidID(1);
        this.npc5.setInvalidID(1);
        this.npc1.setMotion(0, 27);
        if (Runtime.getFlags(310, 1) == 1 && Runtime.getFlags(3160, 1) == 0) {
            this.All = new NPC_NORMAL(263, 16, 0, 0, 15, -0.768f, 0.0f, 26.339f, -110.0f);
            this.All.setInvalidID(1);
        }
        this.BASE01 = new Uwamono(28672, -12.5f, -1.0f, -14.5f);
        this.BASE02 = new Uwamono(28672, -2.5f, -1.0f, 0.0f);
        this.BASE03 = new Uwamono(28672, 15.5f, -1.0f, 18.0f);
        this.BASE01.SetSize(15.0f, 1.0f, 8.0f);
        this.BASE02.SetSize(5.0f, 1.0f, 10.0f);
        this.BASE03.SetSize(5.0f, 1.0f, 10.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 209);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 210);
        this.item03 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 211);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 212);
        this.item05 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 213);
        new Uwamono(0, 20, this.item03);
        new Uwamono(1, 19);
        new Uwamono(2, 20);
        new Uwamono(43, 20, this.item01);
        new Uwamono(44, 20);
        new Uwamono(45, 19, this.item02);
        new Uwamono(17, 106);
        new Uwamono(16, 106, this.item04);
        new Uwamono(4, 32, this.item05);
        this.doorA = new Uwamono(21, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(22, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(23, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(24, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(25, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
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
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Evt() {
            Runtime.setPlayerControl(false);
            ST1750.this.cam0.setMode(-1);
            ST1750.this.EV_Camera00();
            ST1750.this.All.kickEnepc(4, 1);
            ST1750.this.All.kickEnepc(0, 9);
            ST1750.this.All.kickEnepc(9, 100);
            ST1750.this.win = Window.create();
            ST1750.this.win.setSize(4, 45);
            ST1750.this.win.setLocation(15, 305);
            ST1750.this.win.print(ST1750.this.ALLEN_00, 0);
            System.waitFor(ST1750.this.win);
            ST1750.this.All.kickEnepc(4, 0);
            System.sleep(1);
            ST1750.this.All.kickEnepc(4, 3);
            System.sleep(1);
            ST1750.this.All.kickEnepc(1, 3);
            System.sleep(1);
            ST1750.this.All.move(60, 4.611f, 25.855f, true);
            System.sleep(61);
            ST1750.this.fade1.call(0);
            System.sleep(60);
            ST1750.this.All.kickEnepc(4, 2);
            ST1750.this.All.setTranslate(-100.0f, -100.0f, -100.0f);
            ST1750.this.cam0.setMode(0);
            ST1750.this.fade2.call(0);
            System.sleep(60);
            Runtime.setFlags(3160, 1, 1);
            Runtime.setPlayerControl(true);
        }
    }
}

