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
import xeno.map.MC_KUK07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2070
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK07_PRJ {
    int LADDER = Runtime.getFlags(6044, 1);
    int STAIRS = Runtime.getFlags(6045, 1);
    int VERSION = Runtime.getFlags(6046, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Uwamono col;
    Enepc HASIGO;
    Enepc debug1;
    Enepc debug2;
    Enepc debug3;
    Enepc debug4;
    Enepc debug5;
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
    Effect Obj600;
    Effect Obj601;
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
    Unit ladder;
    Unit stairs;
    boolean EnterCheck = false;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
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
    MAPUnit Clip;
    int page;
    String[] Q1 = new String[]{"Press the switch?\n", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Do you know about this statue?\n", "/[waitkey(1)]/[clear()]", "This statue is a self-professed acquaintance of a classmate that lived next door to the second cousin of Director Gaignun, who built this town. So basically,\nit's a statue of someone who has nothing to do with\nthis town.", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "Why is something like that here, you ask? If I knew that, it would make my life easier. It's such a nuisance. One day, I'm gonna knock that thing down, just you watch!", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Oh, it's dangerous to stand here. Everyone hits their head on the ladder that's lowered from above.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "This bakery is one of the best on the Foundation. The bread's tasty, and the cute girl at the front draws in the customers. One thing about this bakery is that\nthe baker is really odd.", "/[waitkey(1)]/[clear()]", "He's never shown himself in public, and there are very few people who have ever seen the baker, even among those living in this town!", "/[waitkey(64)]/[close()]"};
    String[] msg30002 = new String[]{"/[label()]", "Oh, yeah, there's one more local specialty here. That's Johnny there over by the sign! He comes by everyday to court Mina, the girl at the counter.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label(Mina)]", "Welcome! Would you like some freshly baked bread? It's very good! My father is extremely shy of strangers, but he's a great baker.", "/[waitkey(64)]/[close()]"};
    String[] msg40002 = new String[]{"/[label(Mina)]", "My father is a tremendously shy person, so he never shows himself in public. People find that intriguing and there's all sorts of rumors going around.", "/[waitkey(1)]/[clear()]", "That somehow led to our popularity. It's really hard to figure this world out.", "/[waitkey(64)]/[close()]"};
    String[] msg40003 = new String[]{"/[label()]", "Meow...\n", "/[waitkey(64)]/[close()]"};
    String[] msg40004 = new String[]{"/[label(Mina)]", "Hey! You thieving cat! You ate our bread again!", "/[waitkey(64)]/[close()]"};
    String[] msg40005 = new String[]{"/[label(Mina)]", "Hey, Johnny, are you listening?", "/[waitkey(1)]/[clear()]", "Your stray cat came by and stole bread from us again! Now, enough is enough!", "/[waitkey(1)]/[clear()]", "Please let Mr. King know that I'll be coming by to collect payment!", "/[waitkey(64)]/[clear()]"};
    String[] msg40006 = new String[]{"/[label(Johnny)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label(Johnny)]", "What? You got a problem with me? If not, then hurry up and get lost! I'm not on exhibit, you know. Can't you even understand something as simple as that?", "/[waitkey(1)]/[clear()]", "If you're around, Mina will see me. Hurry up and get lost!", "/[waitkey(64)]/[close()]"};
    String[] msg50002 = new String[]{"/[label(Johnny)]", "Oh, Mina. You are so beautiful. Just watching you makes me...weak in the knees.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Oh, is that man bothering you? The man sneaking around by the bakery is one of King's men. His name is Johnny. He thinks he's King's right hand man. I wonder if\nthat's really true?", "/[waitkey(1)]/[clear()]", "He seems to have a crush on the baker's daughter, but having to look at that pathetic display everyday will make any love fade.", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "King? He's the leader of the roughnecks that control this town. He never lifts a finger to work and just plays with his cat all day. He's a complete softy\naround that cat.", "/[waitkey(64)]/[close()]"};
    String[] msg60003 = new String[]{"/[label()]", "Oh, yeah, there was a time when he was going around saying that he gave Gaignun his directorship position.", "/[waitkey(64)]/[close()]"};
    String[] msg70001 = new String[]{"/[label()]", "You're a tourist, right? There're a lot of things to see on the Foundation.", "/[waitkey(1)]/[clear()]", "First, there's the colony skyscraper called the Durandal! There's also the beach and the large A.G.W.S. maintenance facility.", "/[waitkey(1)]/[clear()]", "This has got to be the most interesting colony around!", "/[waitkey(64)]/[close()]"};
    String[] msg80001 = new String[]{"/[label()]", "This is a cleaners. It's not the sort of shop tourists visit.", "/[waitkey(1)]/[clear()]", "I won't stop you though if you insist on seeing it. Be my guest.", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Hmm? This place? This is King's warehouse.", "/[waitkey(1)]/[clear()]", "There's a swarm of thugs inside.", "/[waitkey(1)]/[clear()]", "If you've got no business in there, you're better off staying out.", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "Hmm, there's a treasure box on the roof of this building. But how to get to it?", "/[waitkey(1)]/[clear()]", "I've lived in this town for years, but I still haven't been able to get to the roof of that building. I wonder who put it up there?", "/[waitkey(64)]/[close()]"};
    String[] msg120001 = new String[]{"/[label()]", "You guys outsiders? Seems like it's your first time to the Foundation.", "/[waitkey(1)]/[clear()]", "Then let this knowledgeable old woman tell you something interesting.", "/[waitkey(1)]/[clear()]", "The Foundation, you see...can actually move around.", "/[waitkey(64)]/[close()]"};
    String[] msg120002 = new String[]{"/[label()]", "That's right, when the Durandal docks with the colony, the Foundation can go anywhere it pleases.", "/[waitkey(1)]/[clear()]", "Ever since the Miltian Conflict, the Foundation has been all over space, taking care of the aftermath of the conflict.", "/[waitkey(1)]/[clear()]", "Of course, the Durandal did most of the fighting, so the colony itself was never in harm's way.", "/[waitkey(64)]/[close()]"};
    String[] msg120003 = new String[]{"/[label()]", "Well, even this old woman doesn't know any more details than that.", "/[waitkey(64)]/[close()]"};

    ST2070() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.872f, 3.745f, 19.395f);
        this.camEV.setRotate(-27.855f, -28.539f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        float[] fArray = new float[]{1.0f, -5.872f, 3.745f, 19.395f, 20.0f, -5.872f, 3.745f, 19.395f, 40.0f, -5.872f, 3.745f, 19.395f};
        float[] fArray2 = new float[12];
        fArray2[0] = 1.0f;
        fArray2[1] = -27.855f;
        fArray2[2] = -28.539f;
        fArray2[4] = 20.0f;
        fArray2[5] = -27.855f;
        fArray2[6] = -8.818f;
        fArray2[8] = 40.0f;
        fArray2[9] = -27.855f;
        fArray2[10] = 15.181f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.rotateSPL(fArray3, 1, 3, 40);
        this.camEV.setFov(40.0f);
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
                Runtime.jumpCF(2090, 5);
                break;
            }
            default: {
                System.println("d");
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck) {
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
        } else if (n2 == 1 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py < 3.5f || this.player.py > 7.0f) {
                return;
            }
            System.println("階段の上げ下げ");
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
                float f = 0.099999905f;
                if (this.LADDER == 0 && this.STAIRS == 0) {
                    System.println("E");
                    this.button1A.disp(false);
                    this.button1B.disp(true);
                    float f2 = 0.0f;
                    while (f2 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -f2);
                        System.sleep(1);
                        f2 += 1.0f;
                    }
                    Sound.effectPlay(196744);
                    this.player.setID(3);
                    Runtime.setFlags(6045, 1, 1);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 0) {
                    System.println("F");
                    this.button1A.disp(false);
                    this.button1B.disp(true);
                    float f3 = 0.0f;
                    while (f3 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -f3);
                        System.sleep(1);
                        f3 += 1.0f;
                    }
                    Sound.effectPlay(196744);
                    this.player.setID(4);
                    Runtime.setFlags(6045, 1, 1);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 0 && this.STAIRS == 1) {
                    System.println("G");
                    this.button1A.disp(true);
                    this.button1B.disp(false);
                    float f4 = 0.0f;
                    while (f4 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -30.0f + f4);
                        System.sleep(1);
                        f4 += 1.0f;
                    }
                    Sound.effectPlay(196744);
                    this.player.setID(1);
                    Runtime.setFlags(6045, 1, 0);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 1) {
                    System.println("H");
                    this.button1A.disp(true);
                    this.button1B.disp(false);
                    float f5 = 0.0f;
                    while (f5 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -30.0f + f5);
                        System.sleep(1);
                        f5 += 1.0f;
                    }
                    Sound.effectPlay(196744);
                    this.player.setID(2);
                    Runtime.setFlags(6045, 1, 0);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
        } else if (n2 == 2) {
            System.println("serial = 2");
            this.player.getTranslate();
            if (this.player.py > 9.0f) {
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px - 0.35f, this.player.py, this.player.pz);
                System.sleep(10);
                Runtime.setPlayerControl(true);
            }
        }
        if (n2 == 4 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py > 3.5f) {
                return;
            }
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.player.look_char(this.npc4);
            if (Runtime.getFlags(7158, 1) == 0) {
                ++this.talkFlag4;
                switch (this.talkFlag4) {
                    case 1: {
                        this.npc4.kickEnepc(1, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg40001, 0);
                        ST2070.waitPage(this.win, 64);
                        this.EnterCheck = false;
                        this.player.look_default();
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    case 2: {
                        this.npc4.kickEnepc(1, 9);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg40002, 0);
                        ST2070.waitPage(this.win, 64);
                        this.EnterCheck = false;
                        this.player.look_default();
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    case 3: {
                        Runtime.disable(524288);
                        this.npc5.kickEnepc(4, 1);
                        this.npc4.kickEnepc(1, 9);
                        this.npc13 = new NPC_NORMAL(1611, 13, 0, 14, 33, -0.43f, 0.0f, 14.5f, 340.0f);
                        this.npc13.setInvalidID(1);
                        this.npc13.enableDTKFlag(262144);
                        this.npc13.kickEnepc(4, 1);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msg40003, 0);
                        ST2070.waitPage(this.win, 64);
                        this.npc13.kickEnepc(1, 1);
                        this.player.look_char(this.npc13);
                        this.npc3.look_char(this.npc13);
                        this.npc4.look_char(this.npc13);
                        System.sleep(15);
                        this.fade.call(0);
                        System.sleep(30);
                        this.npc4.setTranslate(-5.14f, 0.0f, 13.6f);
                        this.cam0.setMode(-1);
                        this.EV_Camera01();
                        this.npc13.moveEnepc(17, 290.0f, -0.1f, 90);
                        this.npc13.moveEnepc(15, -0.94f, 15.76f, 60);
                        System.sleep(60);
                        this.npc13.kickEnepc(1, 0);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msg40003, 0);
                        ST2070.waitPage(this.win, 64);
                        this.npc13.kickEnepc(1, 1);
                        System.sleep(15);
                        this.npc13.moveEnepc(15, -7.75f, 16.98f, 180);
                        System.sleep(240);
                        this.player.look_char(this.npc4);
                        this.npc3.look_char(this.npc4);
                        this.npc4.look_default();
                        this.npc5.kickEnepc(1, 6);
                        this.npc4.kickEnepc(0, 8);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg40004, 0);
                        ST2070.waitPage(this.win, 64);
                        this.npc4.kickEnepc(1, 9);
                        this.EV_Camera02();
                        System.sleep(30);
                        this.player.look_char(this.npc5);
                        this.npc4.look_char(this.npc5);
                        System.sleep(30);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg40005, 0);
                        ST2070.waitPage(this.win, 64);
                        this.win.print(this.msg40006, 0);
                        ST2070.waitPage(this.win, 64);
                        this.npc13.setTranslate(100.0f, 0.0f, 100.0f);
                        System.sleep(15);
                        this.fade.call(0);
                        System.sleep(10);
                        this.npc5.kickEnepc(4, 0);
                        System.sleep(20);
                        this.npc4.setTranslate(-5.14f, 0.0f, 13.8f);
                        this.player.look_default();
                        this.npc4.look_default();
                        this.cam0.setMode(0);
                        Runtime.setFlags(7158, 1, 1);
                        this.EnterCheck = false;
                        Runtime.enable(524288);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                }
                this.npc4.kickEnepc(1, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg40002, 0);
                ST2070.waitPage(this.win, 64);
                this.EnterCheck = false;
                this.player.look_default();
                Runtime.setPlayerControl(true);
                return;
            }
            ++this.talkFlag4;
            switch (this.talkFlag4) {
                case 1: {
                    this.npc4.kickEnepc(1, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msg40001, 0);
                    ST2070.waitPage(this.win, 64);
                    this.EnterCheck = false;
                    this.player.look_default();
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            this.npc4.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg40002, 0);
            ST2070.waitPage(this.win, 64);
            this.EnterCheck = false;
            this.player.look_default();
            Runtime.setPlayerControl(true);
            return;
        }
        if (n2 == 5 && !this.EnterCheck) {
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg80001, 0);
            ST2070.waitPage(this.win, 64);
            this.EnterCheck = false;
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
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg120001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg120002, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg120003, 0);
        ST2070.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg10001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg10002, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg30001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg30002, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg40001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg40002, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg50001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg50002, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg60001, 0);
                ST2070.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg60002, 0);
                ST2070.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg60003, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg70001, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        window.print(this.msg80001, 0);
        ST2070.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg90001, 0);
        ST2070.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外観街１");
                Runtime.jumpCF(2030, 1);
                break;
            }
            case 1: {
                System.println("外観街７");
                Runtime.jumpCF(2030, 7);
                break;
            }
            case 2: {
                System.println("倉庫・１");
                Runtime.jumpCF(2100, 1);
                break;
            }
            case 3: {
                System.println("倉庫・２");
                Runtime.jumpCF(2100, 2);
                break;
            }
            case 4: {
                System.println("クリーニング屋１");
                Runtime.jumpCF(2090, 1);
                break;
            }
            case 5: {
                System.println("パン屋４");
                Runtime.jumpCF(2080, 4);
                break;
            }
            case 6: {
                System.println("パン屋５");
                Runtime.jumpCF(2080, 5);
                break;
            }
            case 7: {
                System.println("パン屋１");
                Runtime.jumpCF(2080, 1);
                break;
            }
            case 8: {
                System.println("パン屋３");
                Runtime.jumpCF(2080, 3);
                break;
            }
            case 9: {
                System.println("パン屋２");
                Runtime.jumpCF(2080, 2);
                break;
            }
            case 10: {
                System.println("クリーニング屋２");
                Runtime.jumpCF(2090, 2);
                break;
            }
            case 11: {
                System.println("倉庫・４");
                Runtime.jumpCF(2100, 4);
                break;
            }
            case 12: {
                System.println("クリーニング屋３");
                Runtime.jumpCF(2090, 3);
                break;
            }
            case 13: {
                System.println("倉庫・３");
                Runtime.jumpCF(2100, 3);
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
        this.lamp_s1 = new Effect(1630, -12.924f, 2.853f, 7.897f, 0.0f);
        this.lamp_s1.setScale(0.4f, 0.4f, 0.4f);
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
        this.lamp_l1.disp(true);
        this.lamp_l2 = new Effect(1637, -1.294f, 2.127f, 14.282f, 0.0f);
        this.lamp_l2.setScale(1.0f, 0.75f, 1.0f);
        this.lamp_l2.disp(true);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
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
        this.cam0.setCFAngle(23, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(23, 0.02f, 0.02f);
        this.cam0.setCFLockX(23, -18.5f);
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
        this.HASIGO = new NPC_NORMAL(1591, 11, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.HASIGO.kickEnepc(4, 2);
        this.HASIGO.setInvalidID(1);
        this.HASIGO.setVisible(false);
        this.HASIGO.dispRadar(false);
        this.HASIGO.kickEnepc(19, 1, 0, 420, 0);
        this.HASIGO.kickEnepc(19, 2, 0, 770, 1);
        this.HASIGO.kickEnepc(19, 3, 0, 420, 1);
        this.ladder = new MAPUnit();
        this.ladder.mapUnit(161);
        this.ladder.start(4, null);
        this.stairs = new MAPUnit();
        this.stairs.mapUnit(195);
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
            this.button1A.disp(true);
            this.button1B.disp(false);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 3.2f, 12.15f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(3);
        } else if (this.STAIRS == 1 && this.LADDER == 1) {
            System.println("STAIRS == 1 && LADDER == 1");
            this.button1A.disp(false);
            this.button1B.disp(true);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 0.0f, 12.15f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(4);
        }
        this.itembox = new Uwamono(28677, -2.71f, 10.8f, 7.66f, 180.0f, 422);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        this.npc1 = new NPC_NORMAL(1552, 1, 0, 14, 18, 15.5f, 0.0f, 15.63f, 200.0f);
        this.npc2 = new NPC_NORMAL(1588, 2, 0, 14, 19, 6.7f, 0.0f, 12.8f, 120.0f);
        this.npc3 = new NPC_NORMAL(1591, 3, 0, 14, 15, -4.16f, 0.0f, 15.0f, 220.0f);
        this.npc4 = new NPC_NORMAL(1606, 4, 0, 14, 17, -5.14f, 0.0f, 13.8f, 0.0f);
        this.npc5 = new NPC_NORMAL(1613, 5, 0, 14, 14, -8.25f, 0.0f, 14.74f, 90.0f);
        this.npc6 = new NPC_NORMAL(1567, 6, 0, 14, 20, -15.77f, 0.04f, 16.63f, 45.0f);
        this.npc7 = new NPC_NORMAL(1558, 7, 11, 7, 19, -17.8f, 3.6f, 3.0f, 0.0f);
        this.npc8 = new NPC_NORMAL(1570, 8, 0, 14, 15, -16.0f, 3.6f, -0.8f, 0.0f);
        this.npc9 = new NPC_NORMAL(1561, 9, 0, 14, 16, 12.36f, 3.6f, 7.38f, 20.0f);
        this.npc11 = new NPC_NORMAL(1540, 11, 0, 14, 16, 5.92f, 7.19f, 14.39f, 310.0f);
        this.npc12 = new NPC_NORMAL(1582, 12, 12, 7, 18, 17.5f, 7.2f, 1.5f, 0.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 9);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 10);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 12);
        this.npc3.enableDTKFlag(262144);
        this.npc3.setInvalidID(1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 10);
        this.npc4.enableDTKFlag(262144);
        this.npc4.setInvalidID(1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.enableDTKFlag(262144);
        this.npc5.setMotion(0, 5);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 4);
        this.npc6.setInvalidID(1);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 16);
        this.npc8.setInvalidID(1);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 12);
        this.npc9.setInvalidID(1);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 10);
        this.npc11.setInvalidID(1);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        new Uwamono(115, 39);
        new Uwamono(93, 39);
        new Uwamono(122, 40);
        new Uwamono(196, 5);
        this.Clip = new Mapunits();
        this.Clip.mapUnit(11);
        this.Clip.start(1, "Clipping");
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Clipping() {
            boolean bl = false;
            boolean bl2 = false;
            boolean bl3 = false;
            boolean bl4 = false;
            boolean bl5 = false;
            boolean bl6 = false;
            boolean bl7 = false;
            boolean bl8 = false;
            boolean bl9 = false;
            boolean bl10 = false;
            boolean bl11 = false;
            ST2070.this.player.getTranslate();
            if (ST2070.this.player.px > 9.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC1_TRUE_A");
                bl = false;
            } else if (ST2070.this.player.px <= 9.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC1_FALSE_A");
                ST2070.this.npc1.setVisible(false);
                ST2070.this.npc1.kickEnepc(4, 2);
                bl = true;
            } else if (ST2070.this.player.py > 3.5f) {
                System.println("NPC1_FALSE_B");
                ST2070.this.npc1.setVisible(false);
                ST2070.this.npc1.kickEnepc(4, 2);
                bl = true;
            } else if (ST2070.this.player.py <= 3.5f && ST2070.this.player.px > 9.0f) {
                System.println("NPC1_TRUE_B");
                bl = false;
            }
            if (ST2070.this.player.px < 9.25f && ST2070.this.player.px > 7.2f && ST2070.this.player.py > 3.5f && ST2070.this.player.pz > 11.75f && ST2070.this.player.pz < 13.8f) {
                System.println("NPC2_TRUE_A");
                bl2 = false;
            } else if (ST2070.this.player.py > 7.0f) {
                System.println("NPC2_FALSE_A");
                ST2070.this.npc2.setVisible(false);
                ST2070.this.npc2.kickEnepc(4, 2);
                bl2 = true;
            } else if (ST2070.this.player.px < 13.5f && ST2070.this.player.px > -0.5f && ST2070.this.player.py < 0.1f) {
                System.println("NPC2_TRUE_B");
                bl2 = false;
            } else if ((ST2070.this.player.px >= 13.0f || ST2070.this.player.px <= -0.5f) && ST2070.this.player.py < 0.1f) {
                System.println("NPC2_FALSE_B");
                ST2070.this.npc2.setVisible(false);
                ST2070.this.npc2.kickEnepc(4, 2);
                bl2 = true;
            } else {
                System.println("NPC2_TRUE_C");
                bl2 = false;
            }
            if (ST2070.this.player.px < 2.0f && ST2070.this.player.px > -10.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC3_TRUE");
                bl4 = false;
            } else if ((ST2070.this.player.px >= 2.0f || ST2070.this.player.px <= -10.0f) && ST2070.this.player.py < 0.1f) {
                System.println("NPC3_FALSE_A");
                bl4 = true;
                ST2070.this.npc3.setVisible(false);
                ST2070.this.npc3.kickEnepc(4, 2);
                bl4 = true;
            } else if (ST2070.this.player.py > 3.5f) {
                System.println("NPC3_FALSE_B");
                bl4 = true;
                ST2070.this.npc3.setVisible(false);
                ST2070.this.npc3.kickEnepc(4, 2);
                bl4 = true;
            }
            if (ST2070.this.player.px < 1.5f && ST2070.this.player.px > -11.5f && ST2070.this.player.py < 0.1f) {
                System.println("NPC4_TRUE");
                bl5 = false;
            } else if ((ST2070.this.player.px >= 1.5f || ST2070.this.player.px <= -11.5f) && ST2070.this.player.py < 0.1f) {
                System.println("NPC4_FALSE_A");
                bl5 = true;
                ST2070.this.npc4.setVisible(false);
                ST2070.this.npc4.kickEnepc(4, 0);
                ST2070.this.npc4.kickEnepc(4, 2);
                bl5 = true;
            } else if (ST2070.this.player.py > 3.5f) {
                System.println("NPC4_FALSE_B");
                bl5 = true;
                ST2070.this.npc4.setVisible(false);
                ST2070.this.npc4.kickEnepc(4, 0);
                ST2070.this.npc4.kickEnepc(4, 2);
                bl5 = true;
            }
            if (ST2070.this.player.px < -1.75f && ST2070.this.player.px > -14.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC5_TRUE");
                bl6 = false;
            } else if ((ST2070.this.player.px >= -1.75f || ST2070.this.player.px <= -14.0f) && ST2070.this.player.py < 0.1f) {
                System.println("NPC5_FALSE_A");
                bl6 = true;
                ST2070.this.npc5.setVisible(false);
                ST2070.this.npc5.kickEnepc(4, 2);
                bl6 = true;
            } else if (ST2070.this.player.py > 3.5f) {
                System.println("NPC5_FALSE_B");
                bl6 = true;
                ST2070.this.npc5.setVisible(false);
                ST2070.this.npc5.kickEnepc(4, 2);
                bl6 = true;
            }
            if (ST2070.this.player.px < -11.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC6_TRUE_A");
                bl7 = false;
            } else if (ST2070.this.player.px >= -11.0f && ST2070.this.player.py < 0.1f) {
                System.println("NPC6_FALSE_A");
                ST2070.this.npc6.setVisible(false);
                ST2070.this.npc6.kickEnepc(4, 2);
                bl7 = true;
            } else if (ST2070.this.player.py > 3.5f) {
                System.println("NPC6_FALSE_B");
                ST2070.this.npc6.setVisible(false);
                ST2070.this.npc6.kickEnepc(4, 2);
                bl7 = true;
            } else if (ST2070.this.player.py <= 3.5f && ST2070.this.player.px < -11.0f) {
                System.println("NPC6_TRUE_B");
                bl7 = false;
            }
            if (ST2070.this.player.px < -4.5f && ST2070.this.player.py > 0.1f) {
                System.println("NPC7_TRUE");
                System.println("NPC8_TRUE");
                bl8 = false;
            } else if (ST2070.this.player.px >= -4.5f && ST2070.this.player.py > 0.1f) {
                System.println("NPC7_FALSE_A");
                ST2070.this.npc7.setVisible(false);
                ST2070.this.npc7.kickEnepc(4, 2);
                System.println("NPC8_FALSE_A");
                ST2070.this.npc8.setVisible(false);
                ST2070.this.npc8.kickEnepc(4, 2);
                bl8 = true;
            } else if (ST2070.this.player.py < 0.1f || ST2070.this.player.py > 7.2f) {
                System.println("NPC7_FALSE_B");
                ST2070.this.npc7.setVisible(false);
                ST2070.this.npc7.kickEnepc(4, 2);
                System.println("NPC8_FALSE_B");
                ST2070.this.npc8.setVisible(false);
                ST2070.this.npc8.kickEnepc(4, 2);
                bl8 = true;
            }
            if (ST2070.this.player.px > 3.8f && ST2070.this.player.py > 0.1f) {
                System.println("NPC9_TRUE");
                bl9 = false;
            } else if (ST2070.this.player.px <= 3.8f && ST2070.this.player.py > 0.1f) {
                System.println("NPC9_FALSE_A");
                bl9 = true;
                ST2070.this.npc9.setVisible(false);
                ST2070.this.npc9.kickEnepc(4, 2);
                bl9 = true;
            } else if (ST2070.this.player.py < 0.1f) {
                System.println("NPC9_FALSE_B");
                bl9 = true;
                ST2070.this.npc9.setVisible(false);
                ST2070.this.npc9.kickEnepc(4, 2);
                bl9 = true;
            }
            if (ST2070.this.player.px > 3.0f && ST2070.this.player.py > 3.5f) {
                System.println("NPC12_TRUE");
                bl11 = false;
            } else if (ST2070.this.player.px <= 3.0f && ST2070.this.player.py > 3.5f) {
                System.println("NPC12_FALSE_A");
                bl11 = true;
                ST2070.this.npc12.setVisible(false);
                ST2070.this.npc12.kickEnepc(4, 2);
                bl11 = true;
            } else if (ST2070.this.player.py <= 3.5f) {
                System.println("NPC12_FALSE_B");
                bl11 = true;
                ST2070.this.npc12.setVisible(false);
                ST2070.this.npc12.kickEnepc(4, 2);
                bl11 = true;
            }
            while (true) {
                ST2070.this.player.getTranslate();
                if (ST2070.this.player.px > 9.0f && ST2070.this.player.py < 0.1f) {
                    if (bl) {
                        System.println("NPC1++++ A");
                        ST2070.this.npc1.setVisible(true);
                        ST2070.this.npc1.kickEnepc(4, 0);
                        bl = false;
                    }
                } else if (ST2070.this.player.px <= 9.0f && ST2070.this.player.py < 0.1f) {
                    if (!bl) {
                        System.println("NPC1---- A");
                        ST2070.this.npc1.setVisible(false);
                        ST2070.this.npc1.kickEnepc(4, 2);
                        bl = true;
                    }
                } else if (ST2070.this.player.py > 3.5f) {
                    if (!bl) {
                        System.println("NPC1---- B");
                        ST2070.this.npc1.setVisible(false);
                        ST2070.this.npc1.kickEnepc(4, 2);
                        bl = true;
                    }
                } else if (ST2070.this.player.py <= 3.5f && ST2070.this.player.px > 9.0f && bl) {
                    System.println("NPC1++++ B");
                    ST2070.this.npc1.setVisible(true);
                    ST2070.this.npc1.kickEnepc(4, 0);
                    bl = false;
                }
                if (ST2070.this.player.px < 9.25f && ST2070.this.player.px > 7.2f && ST2070.this.player.py > 3.5f && ST2070.this.player.pz > 11.75f && ST2070.this.player.pz < 13.8f) {
                    if (bl2) {
                        System.println("NPC2*****");
                        ST2070.this.npc2.setVisible(true);
                        ST2070.this.npc2.kickEnepc(4, 0);
                        bl2 = false;
                    }
                } else if (ST2070.this.player.py > 7.0f) {
                    if (!bl2) {
                        System.println("NPC2!!!!!");
                        ST2070.this.npc2.setVisible(false);
                        ST2070.this.npc2.kickEnepc(4, 2);
                        bl2 = true;
                    }
                } else if (ST2070.this.player.px < 13.5f && ST2070.this.player.px > -0.5f && ST2070.this.player.py < 0.1f) {
                    if (bl2) {
                        System.println("NPC2+++++");
                        ST2070.this.npc2.setVisible(true);
                        ST2070.this.npc2.kickEnepc(4, 0);
                        bl2 = false;
                    }
                } else if ((ST2070.this.player.px >= 13.0f || ST2070.this.player.px <= -0.5f) && ST2070.this.player.py < 0.1f && !bl2) {
                    System.println("NPC2-----");
                    ST2070.this.npc2.setVisible(false);
                    ST2070.this.npc2.kickEnepc(4, 2);
                    bl2 = true;
                }
                if (ST2070.this.player.px < 2.0f && ST2070.this.player.px > -10.0f && ST2070.this.player.py < 0.1f) {
                    if (bl4) {
                        System.println("NPC3+++++");
                        ST2070.this.npc3.setVisible(true);
                        ST2070.this.npc3.kickEnepc(4, 0);
                        bl4 = false;
                    }
                } else if ((ST2070.this.player.px >= 2.0f || ST2070.this.player.px <= -10.0f) && ST2070.this.player.py < 0.1f) {
                    if (!bl4) {
                        System.println("NPC3---- A");
                        ST2070.this.npc3.setVisible(false);
                        ST2070.this.npc3.kickEnepc(4, 2);
                        bl4 = true;
                    }
                } else if (ST2070.this.player.py > 3.5f && !bl4) {
                    System.println("NPC3---- B");
                    ST2070.this.npc3.setVisible(false);
                    ST2070.this.npc3.kickEnepc(4, 2);
                    bl4 = true;
                }
                if (ST2070.this.player.px < 1.5f && ST2070.this.player.px > -11.5f && ST2070.this.player.py < 0.1f) {
                    if (bl5) {
                        System.println("NPC4+++++");
                        ST2070.this.npc4.setVisible(true);
                        ST2070.this.npc4.kickEnepc(4, 0);
                        bl5 = false;
                    }
                } else if ((ST2070.this.player.px >= 1.5f || ST2070.this.player.px <= -11.5f) && ST2070.this.player.py < 0.1f) {
                    if (!bl5) {
                        System.println("NPC4---- A");
                        ST2070.this.npc4.setVisible(false);
                        ST2070.this.npc4.kickEnepc(4, 2);
                        bl5 = true;
                    }
                } else if (ST2070.this.player.py > 3.5f && !bl5) {
                    System.println("NPC4---- B");
                    ST2070.this.npc4.setVisible(false);
                    ST2070.this.npc4.kickEnepc(4, 2);
                    bl5 = true;
                }
                if (ST2070.this.player.px < -1.75f && ST2070.this.player.px > -14.0f && ST2070.this.player.py < 0.1f) {
                    if (bl6) {
                        System.println("NPC5+++++");
                        ST2070.this.npc5.setVisible(true);
                        ST2070.this.npc5.kickEnepc(4, 0);
                        bl6 = false;
                    }
                } else if ((ST2070.this.player.px >= -1.75f || ST2070.this.player.px <= -14.0f) && ST2070.this.player.py < 0.1f) {
                    if (!bl6) {
                        System.println("NPC5---- A");
                        ST2070.this.npc5.setVisible(false);
                        ST2070.this.npc5.kickEnepc(4, 2);
                        bl6 = true;
                    }
                } else if (ST2070.this.player.py > 3.5f && !bl6) {
                    System.println("NPC5---- B");
                    ST2070.this.npc5.setVisible(false);
                    ST2070.this.npc5.kickEnepc(4, 2);
                    bl6 = true;
                }
                if (ST2070.this.player.px < -11.0f && ST2070.this.player.py < 0.1f) {
                    if (bl7) {
                        System.println("NPC6++++ A");
                        ST2070.this.npc6.setVisible(true);
                        ST2070.this.npc6.kickEnepc(4, 0);
                        bl7 = false;
                    }
                } else if (ST2070.this.player.px >= -11.0f && ST2070.this.player.py < 0.1f) {
                    if (!bl7) {
                        System.println("NPC6---- A");
                        ST2070.this.npc6.setVisible(false);
                        ST2070.this.npc6.kickEnepc(4, 2);
                        bl7 = true;
                    }
                } else if (ST2070.this.player.py > 3.5f) {
                    if (!bl7) {
                        System.println("NPC6---- B");
                        ST2070.this.npc6.setVisible(false);
                        ST2070.this.npc6.kickEnepc(4, 2);
                        bl7 = true;
                    }
                } else if (ST2070.this.player.py <= 3.5f && ST2070.this.player.px < -11.0f && bl7) {
                    System.println("NPC6++++ B");
                    ST2070.this.npc6.setVisible(true);
                    ST2070.this.npc6.kickEnepc(4, 0);
                    bl7 = false;
                }
                if (ST2070.this.player.px < -4.5f && ST2070.this.player.py > 0.1f) {
                    if (bl8) {
                        System.println("NPC7+++++");
                        ST2070.this.npc7.setVisible(true);
                        ST2070.this.npc7.kickEnepc(4, 0);
                        System.println("NPC8+++++");
                        ST2070.this.npc8.setVisible(true);
                        ST2070.this.npc8.kickEnepc(4, 0);
                        bl8 = false;
                    }
                } else if (ST2070.this.player.px >= -4.5f && ST2070.this.player.py > 0.1f) {
                    if (!bl8) {
                        System.println("NPC7---- A");
                        ST2070.this.npc7.setVisible(false);
                        ST2070.this.npc7.kickEnepc(4, 2);
                        System.println("NPC8---- A");
                        ST2070.this.npc8.setVisible(false);
                        ST2070.this.npc8.kickEnepc(4, 2);
                        bl8 = true;
                    }
                } else if ((ST2070.this.player.py < 0.1f || ST2070.this.player.py > 7.2f) && !bl8) {
                    System.println("NPC7---- B");
                    ST2070.this.npc7.setVisible(false);
                    ST2070.this.npc7.kickEnepc(4, 2);
                    System.println("NPC8---- B");
                    ST2070.this.npc8.setVisible(false);
                    ST2070.this.npc8.kickEnepc(4, 2);
                    bl8 = true;
                }
                if (ST2070.this.player.px > 3.8f && ST2070.this.player.py > 0.1f) {
                    if (bl9) {
                        System.println("NPC9+++++");
                        ST2070.this.npc9.setVisible(true);
                        ST2070.this.npc9.kickEnepc(4, 0);
                        bl9 = false;
                    }
                } else if (ST2070.this.player.px <= 3.8f && ST2070.this.player.py > 0.1f) {
                    if (!bl9) {
                        System.println("NPC9---- A");
                        ST2070.this.npc9.setVisible(false);
                        ST2070.this.npc9.kickEnepc(4, 2);
                        bl9 = true;
                    }
                } else if (ST2070.this.player.py < 0.1f && !bl9) {
                    System.println("NPC9---- B");
                    ST2070.this.npc9.setVisible(false);
                    ST2070.this.npc9.kickEnepc(4, 2);
                    bl9 = true;
                }
                if (ST2070.this.player.px > 3.0f && ST2070.this.player.py > 3.5f) {
                    if (bl11) {
                        System.println("NPC12+++++");
                        ST2070.this.npc12.setVisible(true);
                        ST2070.this.npc12.kickEnepc(4, 0);
                        bl11 = false;
                    }
                } else if (ST2070.this.player.px <= 3.0f && ST2070.this.player.py > 3.5f) {
                    if (!bl11) {
                        System.println("NPC12---- A");
                        ST2070.this.npc12.setVisible(false);
                        ST2070.this.npc12.kickEnepc(4, 2);
                        bl11 = true;
                    }
                } else if (ST2070.this.player.py <= 3.5f && !bl11) {
                    System.println("NPC12---- B");
                    ST2070.this.npc12.setVisible(false);
                    ST2070.this.npc12.kickEnepc(4, 2);
                    bl11 = true;
                }
                System.sleep(1);
            }
        }
    }
}

