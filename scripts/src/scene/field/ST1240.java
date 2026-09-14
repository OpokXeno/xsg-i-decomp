import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KOU04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1240
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KOU04_PRJ {
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
    Enepc npc13;
    Enepc npc14;
    Enepc npc15;
    Enepc npc16;
    Enepc npc17;
    Enepc npc18;
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
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int talkFlag14;
    int talkFlag15;
    int talkFlag16;
    int talkFlag17;
    int talkFlag18;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag6;
    int S2040C;
    int S2042;
    int S3;
    Light light = new Light(0);
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Effect blue1;
    Effect blue2;
    Effect green;
    Effect red_l1;
    Effect red_l2;
    Effect red_l3;
    Effect red_l4;
    Effect red_l5;
    Effect red_l6;
    Effect red_l7;
    Effect red_l8;
    Effect blue_lE;
    Effect red_r1;
    Effect red_r2;
    Effect red_r3;
    Effect red_r4;
    Effect red_r5;
    Effect red_r6;
    Effect red_r7;
    Effect red_r8;
    Effect blue_rE;
    Effect kanban;
    Effect light01;
    int page;
    String[] msg0001 = new String[]{"/[label()]", "Say, have you heard about the planet disappearance incident?", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label()]", "A planet called Ariadne completely disappeared. The Federation government still doesn't know the cause of it, ", "and the people who disappeared with the planet are still missing too. Some think it's the work of Gnosis. ", "In any case, it's a terrible incident.", "/[waitkey(64)]/[close()]"};
    String[] msg0003 = new String[]{"/[label()]", "The truth of that incident is still a mystery. I can't believe that a planet could disappear without a trace.", "/[waitkey(64)]/[close()]"};
    String[] msg0007 = new String[]{"/[label()]", "Welcome!", "/[waitkey(1)]/[clear()]", "We have everything from A.G.W.S. beam cannons to toilet paper. There's nothing you can't get here!", "/[waitkey(1)]/[clear()]", "What do you need?", "/[waitkey(64)]/[close()]"};
    String[] msg00071 = new String[]{"/[label()]", "Thank you very much! We look forward to your next visit.", "/[waitkey(64)]/[close()]"};
    String[] msg0008 = new String[]{"/[label()]", "The selection here is the best in the Dock Colony. I can even get my grandkids' diapers here. The world's certainly become a convenient place.", "/[waitkey(64)]/[close()]"};
    String[] msg0009 = new String[]{"/[label()]", "We went through some awful times during the war. The establishment of the Miltian Charter after the war was our saving grace. It would have been wrong to treat\nthe Realians like murderers.", "/[waitkey(64)]/[close()]"};
    String[] msg0010 = new String[]{"/[label()]", "Oh, yeah.", "/[waitkey(1)]/[clear()]", "I heard the Charter passed through Parliament because the Executive Committee Director of the Federation government at the time pushed it through.", "/[waitkey(64)]/[close()]"};
    String[] msg0011 = new String[]{"/[label()]", "The Executive Committee Director at the time?", "/[waitkey(1)]/[clear()]", "I don't know. I think it was some bigwig from a large corporation.", "/[waitkey(64)]/[close()]"};
    String[] msg0004 = new String[]{"/[label()]", "Hey, did you hear?", "/[waitkey(64)]/[close()]"};
    String[] msg0005 = new String[]{"/[label()]", "Some people were attacked in that alleyway just now. The perpetrator is supposedly a Federation soldier.", "/[waitkey(1)]/[clear()]", "It's such a dangerous world.", "/[waitkey(64)]/[close()]"};
    String[] msg0006 = new String[]{"/[label()]", "Isn't it so dangerous? The people who were attacked seem to be in serious condition.", "/[waitkey(1)]/[clear()]", "There's also a rumor that there's a soldier wandering around...I don't feel safe walking around outside anymore.", "/[waitkey(64)]/[close()]"};
    String[] msg029F2C0C = new String[0];
    String[] msg0012 = new String[]{"/[label()]", "Hey, did you hear? A soldier apparently started a fight in the alley there, and I heard his opponents are in critical condition. Isn't that terrible?", "/[waitkey(1)]/[clear()]", "Why do they allow such dangerous soldiers to run around doing as they please?", "/[waitkey(64)]/[close()]"};
    String[] msg0013 = new String[]{"/[label()]", "The Federation government pretends not to see this situation. They're completely hopeless.", "/[waitkey(64)]/[close()]"};
    String[] msgTOM1 = new String[]{"/[label(Tom)]", "That's really weird.\n", "/[waitkey(1)]/[clear()]", "There used to be a pink ghost doll at this shop. Where did it go? It broke really easily, so maybe it got thrown away.", "/[waitkey(64)]/[close()]"};
    String[] msgTOM2 = new String[]{"/[label(Tom)]", "Actually, that pink doll speaks.", "/[waitkey(1)]/[clear()]", "It broke easily, but if you spoke to it kindly, it would tell you all sorts of things!", "/[waitkey(1)]/[clear()]", "If you happen to find it somewhere, please be kind to it for me!", "/[waitkey(64)]/[close()]"};

    ST1240() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        if (this.S3 == 1) {
            this.Talk_npc1_1();
        } else {
            this.Talk_npc1_1();
        }
    }

    void Talk_npc1_1() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0007, 0);
        ST1240.waitPage(this.win, 64);
        System.sleep(20);
        if (Runtime.getFlags(389, 1) == 1) {
            Runtime.enterShop(18);
        } else if (Runtime.getFlags(362, 1) == 1) {
            Runtime.enterShop(17);
        } else if (Runtime.getFlags(346, 1) == 1) {
            Runtime.enterShop(16);
        } else if (Runtime.getFlags(301, 1) == 1) {
            Runtime.enterShop(15);
        } else {
            Runtime.enterShop(4);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg00071, 0);
        ST1240.waitPage(this.win, 64);
    }

    void Talk_npc1_2() {
    }

    public void Talk_npc2(Enepc enepc) {
        if (this.S3 == 1) {
            this.Talk_npc2_1();
        } else if (this.S2042 == 1) {
            this.Talk_npc2_2();
        } else {
            this.Talk_npc2_1();
        }
    }

    void Talk_npc2_1() {
        this.win = Window.create();
        this.win.print(this.msg0001, 0);
        ST1240.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, I have\nNo, I haven't");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.print(this.msg0003, 0);
                ST1240.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.print(this.msg0002, 0);
        ST1240.waitPage(this.win, 64);
    }

    void Talk_npc2_2() {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                this.win = Window.create();
                this.win.print(this.msg0004, 0);
                ST1240.waitPage(this.win, 64);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes, I did\nNo, I haven't");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.print(this.msg0006, 0);
                        ST1240.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.print(this.msg0005, 0);
                ST1240.waitPage(this.win, 64);
                return;
            }
        }
        --this.talkFlag3;
        --this.talkFlag3;
        this.win = Window.create();
        this.win.print(this.msg0001, 0);
        ST1240.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.print(this.msg0003, 0);
                ST1240.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.print(this.msg0002, 0);
        ST1240.waitPage(this.win, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc3_1(window);
        } else if (this.S2042 == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg0008, 0);
        ST1240.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0012, 0);
                ST1240.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0013, 0);
        ST1240.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S3 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg0009, 0);
                ST1240.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0010, 0);
                ST1240.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0011, 0);
        ST1240.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msgTOM1, 0);
                ST1240.waitPage(window, 64);
                Runtime.setFlags(7127, 1, 1);
                return;
            }
        }
        window.print(this.msgTOM2, 0);
        ST1240.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1210, 3);
                break;
            }
            case 1: {
                Runtime.jumpCF(1250, 1);
                break;
            }
        }
    }

    void init() {
        this.S2040C = Runtime.getFlags(158, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S3 = Runtime.getFlags(301, 1);
        this.teiten1 = new Uwamono(28690, -4.875f, 4.0f, -0.675f, 0.0f);
        this.teiten1.SetBgm(196614);
        this.teiten2 = new Uwamono(28690, 0.165f, 4.0f, -0.675f, 0.0f);
        this.teiten2.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, 4.925f, 4.0f, -0.675f, 0.0f);
        this.teiten3.SetBgm(196614);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(16, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 15.0f, 0.0f, 8.75f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, -15.0f, 0.0f, 8.75f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.blue1 = new Effect(1542, 0.0f, 5.5f, -7.325f, 0.0f);
        this.blue1.setScale(3.756f, 2.8f, 1.0f);
        this.blue2 = new Effect(1542, 5.0f, 5.5f, -7.3075f, 0.0f);
        this.blue2.setScale(2.0f, 3.0f, 1.0f);
        this.green = new Effect(1548, -5.0f, 5.5f, -7.315f, 0.0f);
        this.green.setScale(2.0f, 3.0f, 1.0f);
        this.red_l1 = new Effect(1539, -5.995f, 4.5f, -0.175f, 0.0f);
        this.red_l1.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l1.setScale(4.85f, 0.575f, 0.0f);
        this.red_l2 = new Effect(1539, -5.995f, 4.175f, -0.005f, 0.0f);
        this.red_l2.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l2.setScale(4.5f, 0.575f, 0.0f);
        this.red_l3 = new Effect(1539, -5.995f, 3.825f, 0.15f, 0.0f);
        this.red_l3.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l3.setScale(3.85f, 0.575f, 0.0f);
        this.red_l4 = new Effect(1539, -5.995f, 3.475f, 0.375f, 0.0f);
        this.red_l4.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l4.setScale(3.25f, 0.575f, 0.0f);
        this.red_l5 = new Effect(1539, -5.995f, 3.125f, 0.55f, 0.0f);
        this.red_l5.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l5.setScale(2.75f, 0.575f, 0.0f);
        this.red_l6 = new Effect(1539, -5.995f, 2.775f, 0.825f, 0.0f);
        this.red_l6.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l6.setScale(1.925f, 0.575f, 0.0f);
        this.red_l7 = new Effect(1539, -5.995f, 2.425f, 0.975f, 0.0f);
        this.red_l7.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l7.setScale(1.5f, 0.575f, 0.0f);
        this.red_l8 = new Effect(1539, -5.995f, 2.075f, 1.125f, 0.0f);
        this.red_l8.setRotate(0.0f, 90.0f, 0.0f);
        this.red_l8.setScale(1.15f, 0.575f, 0.0f);
        this.blue_lE = new Effect(1542, -5.995f, 1.75f, 1.25f, 0.0f);
        this.blue_lE.setRotate(0.0f, 90.0f, 0.0f);
        this.blue_lE.setScale(1.0f, 0.575f, 0.0f);
        this.red_r1 = new Effect(1539, 5.995f, 4.5f, -0.15f, 0.0f);
        this.red_r1.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r1.setScale(4.95f, 0.575f, 0.0f);
        this.red_r2 = new Effect(1539, 5.995f, 4.175f, 0.015f, 0.0f);
        this.red_r2.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r2.setScale(4.5f, 0.575f, 0.0f);
        this.red_r3 = new Effect(1539, 5.995f, 3.75f, 0.3f, 0.0f);
        this.red_r3.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r3.setScale(3.75f, 0.575f, 0.0f);
        this.red_r4 = new Effect(1539, 5.995f, 3.325f, 0.5f, 0.0f);
        this.red_r4.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r4.setScale(3.15f, 0.575f, 0.0f);
        this.red_r5 = new Effect(1539, 5.995f, 2.975f, 0.65f, 0.0f);
        this.red_r5.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r5.setScale(2.75f, 0.575f, 0.0f);
        this.red_r6 = new Effect(1539, 5.995f, 2.575f, 0.85f, 0.0f);
        this.red_r6.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r6.setScale(1.925f, 0.575f, 0.0f);
        this.red_r7 = new Effect(1539, 5.995f, 2.15f, 1.05f, 0.0f);
        this.red_r7.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r7.setScale(1.5f, 0.575f, 0.0f);
        this.red_r8 = new Effect(1539, 5.995f, 1.725f, 1.25f, 0.0f);
        this.red_r8.setRotate(0.0f, -90.0f, 0.0f);
        this.red_r8.setScale(1.15f, 0.575f, 0.0f);
        this.blue_rE = new Effect(1542, 5.995f, 1.375f, 1.3f, 0.0f);
        this.blue_rE.setRotate(0.0f, -90.0f, 0.0f);
        this.blue_rE.setScale(1.0f, 0.575f, 0.0f);
        this.kanban = new Effect(1542, -2.014f, 2.156f, 9.001f, 0.0f);
        this.kanban.setScale(3.25f, 1.25f, 1.0f);
        this.light01 = new Effect(1712, -7.15f, -1.5f, 7.0f, 0.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        this.doorA = new Uwamono(18, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.npcset_1();
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1567, 1, 0, 14, 9, -0.02f, 0.0f, -4.0f, 0.0f);
        this.npc2 = new NPC_NORMAL(1594, 2, 0, 7, 5, -1.7f, 4.1f, -3.97f, 45.0f);
        this.npc3 = new NPC_NORMAL(1576, 3, 0, 15, 7, -1.24f, 0.0f, -1.64f, 180.0f);
        this.npc4 = new NPC_NORMAL(1541, 4, 0, 2, 3, 2.26f, 0.0f, 3.05f, 100.0f);
        this.npc5 = new NPC_NORMAL(1558, 5, 0, 0, 11, -2.78f, 0.0f, 4.38f, 100.0f);
        this.npc10 = new NPC_NORMAL(1567, 10, 0, 14, 9, -0.02f, 0.0f, -3.0f, 0.0f);
        this.npc1.setMotion(0, 9);
        this.npc2.setMotion(0, 9);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 9);
        this.npc4.setMotion(0, 10);
        this.npc10.setInvalidID(1);
        this.npc10.setTranslate(-0.02f, 0.0f, -3.0f);
        this.npc10.setVisible(false);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc10.talkto("Talk_npc1");
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
    }
}

