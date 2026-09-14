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
import xeno.map.MC_KUK03B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2171
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK03B_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Unit crank;
    Unit yane;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int discovery = 0;
    Uwamono doorA;
    Uwamono doorB;
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
    Effect kemuri1;
    Effect kemuri2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect singouki_red;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    MAPUnit komono;
    int CRANK = Runtime.getFlags(6042, 1);
    int A_OR_B = Runtime.getFlags(6048, 1);
    int page;
    String[] NPC_TALK1 = new String[]{"Where would be a good place?", "/[waitkey(64)]/[close()]"};
    String[] NPC_TALK2 = new String[]{"Please don't look for me.", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Turn the crank?", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "Man, I just finished packing. Now, I have to start all over again?", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Just what are those gruesome monsters?! What do they have against us?", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "What? Why is everyone so cheerful? Well, it's because there's no point in being depressed.", "/[waitkey(64)]/[close()]"};
    String[] msg30002 = new String[]{"/[label()]", "We were originally a group of jobless people anyway.", "/[waitkey(1)]/[clear()]", "So maybe we're used to tough times.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Hey, move your hands, not your mouth!", "/[waitkey(1)]/[clear()]", "Common sense dictates the car should be fixed before you load the cargo!", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "Sheesh, they sure did a number on us. We won't even be able to sleep unless we fix everything first.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Just watch. This city will be reborn again. It's sure to become a wonderful city because everyone is working so hard.", "/[waitkey(64)]/[close()]"};
    String[] msg90001 = new String[]{"/[label()]", "Oh, I'm so busy! You're in my way!", "/[waitkey(1)]/[clear()]", "Sightseeing at a time like this? You're so lucky to have so much free time.", "/[waitkey(64)]/[close()]"};
    String[] msg90002 = new String[]{"/[label()]", "I don't have time to play with you! I'm busy, so could you please get out of my way?", "/[waitkey(64)]/[close()]"};
    String[] msg100001 = new String[]{"/[label()]", "They're still duking it out outside, right? Well, even if this place got attacked again, we'd get it back to normal again right away.", "/[waitkey(64)]/[close()]"};
    String[] msg110001 = new String[]{"/[label()]", "I hear the neighboring precinct was damaged pretty badly too. But they are all working hard to help the injured.", "/[waitkey(64)]/[close()]"};
    String[] msg120001 = new String[]{"/[label()]", "Pitching in to clean up as a group is fun! Everyone is working really hard, so don't you slack off either.", "/[waitkey(64)]/[close()]"};
    String[] msg130001 = new String[]{"/[label()]", "Yo, thanks for what you did earlier. I can work without any worries thanks to you guys.", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 1.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 1.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 1, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST2171() {
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
        if (n != 100 || this.EnterCheck) {
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
                        float f6 = 3.83f - 0.49f / f4 * f;
                        float f7 = -6.15f + 1.85f / f4 * f;
                        this.crank.setRotateY(f5);
                        this.yane.setTranslate(13.75f, f6, f7);
                        System.sleep(1);
                        f += 1.0f;
                        f5 += f2;
                    }
                    this.yane.setTranslate(13.75f, 3.34f, -4.3f);
                    this.player.setID(2);
                    Runtime.setFlags(6042, 1, 1);
                    this.CRANK = Runtime.getFlags(6042, 1);
                } else {
                    int n3 = 0;
                    while ((float) n3 <= f3) {
                        Sound.effectPlay(196742);
                        float f8 = 3.34f + 0.49f / f4 * f;
                        float f9 = -4.3f - 1.85f / f4 * f;
                        this.crank.setRotateY(-n3);
                        this.yane.setTranslate(13.75f, f8, f9);
                        System.sleep(1);
                        f += 1.0f;
                        n3 = (int) ((float) n3 + f2);
                    }
                    this.yane.setTranslate(13.75f, 3.83f, -6.15f);
                    this.player.setID(1);
                    Runtime.setFlags(6042, 1, 0);
                    this.CRANK = Runtime.getFlags(6042, 1);
                }
            }
            this.EnterCheck = false;
            System.sleep(30);
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
        }
        switch (n2) {
            case 1: {
                if (this.discovery == 0) {
                    System.println("うえあげ");
                    ++this.discovery;
                    this.npc1.setTranslate(-16.3f, 0.7f, -4.65f);
                    break;
                }
                System.println("ぬけ");
                break;
            }
            case 2: {
                if (this.discovery == 1) {
                    System.println("したさげ");
                    --this.discovery;
                    this.npc1.setTranslate(-16.3f, 0.0f, -4.65f);
                    break;
                }
                System.println("ぬけぬけ");
                break;
            }
            case 3: {
                Runtime.setFlags(7122, 1, 1);
                this.doorB.SetDoorType('\u0004');
                Runtime.setFlags(7108, 1, 1);
                this.player.getTranslate();
                if (!(this.player.py > 7.0f)) break;
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px, this.player.py, this.player.pz + 0.3f);
                System.sleep(16);
                Runtime.setPlayerControl(true);
                break;
            }
            case 8: {
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
                return;
            }
            case 4: {
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
                    }
                }
                return;
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
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        window.print(this.msg110001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
        window.print(this.msg120001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        this.Talk_npc13_1(window);
    }

    void Talk_npc13_1(Window window) {
        window.print(this.msg130001, 0);
        ST2171.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg10001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg30001, 0);
                ST2171.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg30002, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msg50001, 0);
        ST2171.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        this.Talk_npc6_1(window);
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg60001, 0);
        ST2171.waitPage(window, 64);
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
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg90001, 0);
                ST2171.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg90002, 0);
        ST2171.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後外観街２・１");
                Runtime.jumpCF(2211, 1);
                break;
            }
            case 1: {
                System.println("外マップ");
                Runtime.jumpCF(2130, 2);
                break;
            }
            case 2: {
                System.println("襲撃後宿屋１Ｆ・１");
                Runtime.jumpCF(2051, 1);
                break;
            }
            case 3: {
                System.println("襲撃後酒場１Ｆ・１");
                Runtime.jumpCF(2041, 1);
                break;
            }
            case 4: {
                System.println("襲撃後酒場２Ｆ・２");
                Runtime.jumpCF(2201, 2);
                break;
            }
            case 5: {
                System.println("襲撃後酒場２Ｆ・３");
                Runtime.jumpCF(2041, 3);
                break;
            }
            case 6: {
                System.println("襲撃後外観街２・２");
                Runtime.jumpCF(2211, 2);
                break;
            }
            case 7: {
                System.println("サブルート MC_KUK14");
                Runtime.jumpCF(2140, 1);
                break;
            }
            case 8: {
                System.println("襲撃後宿屋３Ｆ・３");
                Runtime.jumpCF(2201, 3);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(7161, 1) == 0) {
            Runtime.setFlags(6044, 1, 1);
            Runtime.setFlags(6045, 1, 1);
            Runtime.setFlags(7161, 1, 1);
        }
        int n = Runtime.getFlags(6048, 1);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
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
        Runtime.setIdLightCol(3, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(4, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(53, false);
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.npc1 = new NPC_NORMAL(1561, 1, 0, 14, 18, 4.04f, 0.0f, 2.56f, 20.0f);
        this.npc2 = new NPC_NORMAL(1591, 2, 0, 14, 11, 11.85f, 0.0f, -1.39f, 40.0f);
        this.npc3 = new NPC_NORMAL(1592, 3, 0, 14, 14, 12.63f, 0.0f, -0.95f, 250.0f);
        this.npc4 = new NPC_NORMAL(1549, 4, 0, 14, 16, 6.78f, 0.0f, -0.46f, 340.0f);
        this.npc5 = new NPC_NORMAL(1579, 5, 0, 14, 17, -5.91f, 0.0f, -4.63f, 200.0f);
        this.npc6 = new NPC_NORMAL(1567, 6, 0, 14, 11, -11.2f, 0.0f, -2.33f, 180.0f);
        this.npc9 = new NPC_NORMAL(1567, 9, 0, 7, 11, 20.91f, 0.0f, 2.86f, 0.0f);
        this.npc10 = new NPC_NORMAL(1543, 10, 0, 14, 18, 6.78f, -0.1f, 6.59f, 70.0f);
        this.npc11 = new NPC_NORMAL(527, 11, 0, 14, 18, -11.48f, -0.1f, 7.3f, 0.0f);
        this.npc12 = new NPC_NORMAL(1585, 12, 0, 14, 18, 18.94f, 0.0f, 5.83f, 0.0f);
        this.npc13 = new NPC_NORMAL(1561, 13, 0, 14, 13, -17.22f, 0.0f, -2.63f, 180.0f);
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
        this.npc9.setMotion(0, 16);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc10.setInvalidID(1);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 1);
        this.npc11.setInvalidID(1);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.setMotion(0, 3);
        this.npc13.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        this.npc13.talkto("Talk_npc13");
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
        this.item01 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 252);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 253);
        this.item03 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 254);
        new Uwamono(68, 22, this.item01);
        new Uwamono(109, 33, this.item03);
        new Uwamono(110, 33, this.item02);
        if (Runtime.getFlags(3201, 1) == 0) {
            new Uwamono(81, 31);
        } else {
            Stage.setVisible(81, false);
        }
        this.komono = new MAPUnit();
        this.komono.mapUnit(29);
        this.komono.start(4, null);
        this.komono.getTranslate();
        this.komono.setTranslate(this.komono.px, this.komono.py, this.komono.pz - 0.8f);
        this.crank = new MAPUnit();
        this.crank.mapUnit(69);
        this.crank.start(4, null);
        this.yane = new MAPUnit();
        this.yane.mapUnit(113);
        this.yane.start(4, null);
        if (this.CRANK != 0) {
            this.yane.setTranslate(13.75f, 3.34f, -4.3f);
            this.player.setID(2);
        }
        this.doorA = new Uwamono(82, 40, '\u0004');
        if (Runtime.getFlags(3281, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(7108, 1) == 0) {
            this.doorB = new Uwamono(98, 40, '\u0002');
            this.doorB.SetDoorType('\u0002');
            this.doorB.SetSe0(196746);
            this.doorB.SetSe1(196746);
        } else {
            this.doorB = new Uwamono(98, 40, '\u0002');
            this.doorB.SetDoorType('\u0004');
            this.doorB.SetSe0(196746);
            this.doorB.SetSe1(196746);
        }
        this.kemuri1 = new Effect(1515, -20.5f, 11.5f, -5.6f, 0.0f);
        this.kemuri1.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri1.setClip(true);
        this.kemuri2 = new Effect(1515, -5.75f, 11.5f, -5.6f, 0.0f);
        this.kemuri2.setScale(0.8f, 1.25f, 0.8f);
        this.kemuri2.setClip(true);
        this.singouki_red = new Effect(1539, -0.2375f, 1.3825f, 3.85f, 270.0f);
        this.singouki_red.setScale(0.275f, 0.275f, 0.275f);
        this.singouki_red.setRotate(25.0f, 295.0f, -10.0f);
        this.light01 = new Effect(1405, -13.4f, 3.2f, 1.0f, 0.0f);
        this.light01.setScale(0.8f, 0.8f, 0.8f);
        this.light02 = new Effect(1405, -4.9f, 4.0f, 6.2f, 0.0f);
        this.light02.setScale(0.3f, 0.3f, 0.3f);
        this.light03 = new Effect(1405, 6.4f, 3.7f, -1.8f, 0.0f);
        this.light03.setScale(0.8f, 0.8f, 0.8f);
        Runtime.progressEffect(30);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2171.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2171.waitPage(this.win, 64);
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

