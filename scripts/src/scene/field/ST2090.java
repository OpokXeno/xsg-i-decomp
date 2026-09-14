import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2090
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK09_PRJ {
    int VERSION = Runtime.getFlags(6046, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Enepc HASIGO;
    int BUTTON_F = 0;
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
    Uwamono doorA;
    Uwamono col;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect EF01;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect light10;
    Effect light11;
    Effect light12;
    Effect light13;
    Effect fire;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono teiten1;
    int page;
    String[] NPC_TALK1 = new String[]{"Make before and after attack changes?\n", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "You know, I'm just a part-timer here, so I don't know the details. But this family sure seems to love arguing.", "/[waitkey(1)]/[clear()]", "Inherit the shop, not inherit the shop. Then it's \"I'm leaving,\" \"No, I'm throwing you out.\" I really don't know how they never get tired of it.", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "Oh, I'm just a part-timer here, so please don't tell people what I just said.", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Oh, that prodigal son? He's a headache. He makes such a racket saying he'll run away from home and never come back whenever his mother scolds him.", "/[waitkey(1)]/[clear()]", "And then, the very next day, he's right back home. He's such a troublemaker.", "/[waitkey(64)]/[close()]"};
    String[] msg20002 = new String[]{"/[label()]", "When he runs away from home, he always leaves using that ladder. He's probably got a good hiding place somewhere.", "/[waitkey(64)]/[close()]"};
    String[] msg20003 = new String[]{"/[label()]", "Um, you're in my way and I can't work. Would you mind moving?", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "Really, that little troublemaker of mine...he never helps with work and just wanders off.", "/[waitkey(1)]/[clear()]", "I wish he'd be a good son and take over the cleaners, but then he started talking about becoming a soldier. That boy's got no respect!", "/[waitkey(64)]/[close()]"};
    String[] msg30002 = new String[]{"/[label()]", "All soldiers die horrible deaths! I'll never let him become one! It's out of the question! I'm going to make him take over this shop!", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Leave me alone!", "/[waitkey(1)]/[clear()]", "I'm going to become a soldier, no matter what it takes! Why are we running a cleaners in this day and age anyway?", "/[waitkey(1)]/[clear()]", "You can just leave that job to a laundry machine.", "/[waitkey(1)]/[clear()]", "A cleaners is so not cool!", "/[waitkey(1)]/[clear()]", "Mom doesn't understand how a man feels! I don't care about that stubborn fool! I'm leaving this house!", "/[waitkey(64)]/[close()]"};
    String[] PM_GET = new String[]{"/[label()]", "There's something in the dryer.", "/[waitkey(64)]/[close()]"};
    String[] FISH_GET = new String[]{"/[label()]", "There's something inside the pocket.", "/[waitkey(64)]/[close()]"};
    String[] sub_01 = new String[]{"Discovered Segment Address No. 3.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 3.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No. 3, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST2090() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.604f, 3.886f, -12.364f);
        this.camEV.setRotate(-62.973f, -91.898f, 0.0f);
        this.camEV.setFov(42.499f);
        this.camEV.change();
    }

    public void HashigoTop(int n) {
        if (n == 0) {
            System.println("外観街１・１５");
            Runtime.setPlayerControl(false);
            this.fade.call(0);
            System.sleep(30);
            Runtime.jumpCF(2070, 15);
        }
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
                if (Runtime.checkItem(10, 71) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.FISH_GET, 0);
                    ST2090.waitPage(this.win, 64);
                    this.menu = Menu.create();
                    this.menu.addItem("Take it\nDon't take it");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Sound.effectPlay(6);
                            Runtime.addItemWin(10, 71);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                this.BUTTON_F = 0;
                break;
            }
            case 1: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.checkItem(10, 37) == 0) {
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_Camera01();
                    System.sleep(30);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.PM_GET, 0);
                    ST2090.waitPage(this.win, 64);
                    this.menu = Menu.create();
                    this.menu.addItem("Take it\nDon't take it");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            System.sleep(15);
                            this.cam0.setMode(0);
                            this.EF01.disp(false);
                            Sound.effectPlay(6);
                            Runtime.addItemWin(10, 37);
                            Runtime.setPlayerControl(true);
                            this.BUTTON_F = 0;
                            break block0;
                        }
                    }
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                this.BUTTON_F = 0;
                break;
            }
            case 2: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3203, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3203, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3223, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3283, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorA.SetDoorType('\u0004');
                    Runtime.setFlags(3283, 1, 1);
                }
                this.BUTTON_F = 0;
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg10001, 0);
                ST2090.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg10002, 0);
        ST2090.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg20001, 0);
                ST2090.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg20002, 0);
                ST2090.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg20003, 0);
        ST2090.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg30001, 0);
                ST2090.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg30002, 0);
        ST2090.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2090.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外概観２・５");
                Runtime.jumpCF(2070, 5);
                break;
            }
            case 1: {
                System.println("外概観２・１１");
                Runtime.jumpCF(2070, 11);
                break;
            }
            case 2: {
                System.println("外概観２・１３");
                Runtime.jumpCF(2070, 13);
                break;
            }
            case 3: {
                System.println("サブルート MC_KUK15");
                Runtime.jumpCF(2150, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
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
        Runtime.setIdLightVec(1, 2, 0.0f, 0.5f, 1.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(3, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(4, 1, -0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(5, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(6, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(6, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(6, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(6, 1, 0.0f, 1.0f, -0.075f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1405, 3.5f, 2.1f, -0.8f, 0.0f);
        this.light02 = new Effect(1405, 0.7f, 2.1f, -2.0f, 0.0f);
        this.light03 = new Effect(1405, 2.8f, 2.6f, -15.7f, 0.0f);
        this.light04 = new Effect(1405, 10.0f, 4.6f, -15.3f, 0.0f);
        this.light05 = new Effect(1405, 14.7f, 6.0f, -13.7f, 0.0f);
        this.light06 = new Effect(1405, 0.3f, 2.8f, -18.4f, 0.0f);
        this.light07 = new Effect(1405, 7.6f, 3.2f, -23.4f, 0.0f);
        this.light08 = new Effect(1405, 11.7f, 3.1f, -20.0f, 0.0f);
        this.light09 = new Effect(1405, 11.6f, 5.2f, -23.8f, 0.0f);
        this.light10 = new Effect(1405, 15.6f, 5.6f, -23.8f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.5f, 0.5f, 0.5f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.5f, 0.5f, 0.5f);
        this.light05.setScale(0.5f, 0.5f, 0.5f);
        this.light06.setScale(0.5f, 0.5f, 0.5f);
        this.light07.setScale(0.5f, 0.5f, 0.5f);
        this.light08.setScale(0.5f, 0.5f, 0.5f);
        this.light09.setScale(0.5f, 0.5f, 0.5f);
        this.light10.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(60);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        new Uwamono(28672, 2.8f, -1.0f, -23.4f, 0.0f);
        new Uwamono(28672, 4.9f, -1.0f, -23.4f, 0.0f);
        this.item01 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 239);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 240);
        this.item03 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 248);
        if (Runtime.getFlags(3203, 1) == 0) {
            new Uwamono(3, 31);
        } else {
            Stage.setVisible(3, false);
        }
        new Uwamono(2, 31, this.item01);
        new Uwamono(1, 31, this.item03);
        new Uwamono(65, 32, this.item02);
        this.doorA = new Uwamono(83, 40, '\u0004');
        if (Runtime.getFlags(3283, 1) == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
        this.doorA.SetDiffSize(0.0f, -0.27f, 0.0f);
        if (Runtime.checkItem(10, 37) == 0) {
            this.EF01 = new Effect(1018, 0);
            this.EF01.disp(true);
        }
        this.col = new Uwamono(28672, 10.0f, 0.0f, -1.5f, 0.0f);
        this.col.SetSize(2.0f, 1.0f, 0.63f);
        this.HASIGO = new NPC_NORMAL(1570, 11, 0, 0, 5, 8.5f, 0.0f, 0.0f, 0.0f);
        this.HASIGO.kickEnepc(4, 2);
        this.HASIGO.disableDTKFlag(131072);
        this.HASIGO.disableDTKFlag(65536);
        this.HASIGO.setInvalidID(1);
        this.HASIGO.setVisible(false);
        this.HASIGO.dispRadar(false);
        this.HASIGO.kickEnepc(19, 1, 0, 280, 1);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, -25.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFPedestal(4, 3.095878f, 4.7519803f, -8.383345f, 51.079285f, -12.538941f, 329.77567f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(7, 0.02f, 0.02f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFAngle(12, -28.0f, -20.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(12, 0.015f, 0.015f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(13, 0.02f, 0.02f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(14, 100.0f, 100.0f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 3.0f, 45.0f);
        this.cam0.setCFHokan(15, 0.02f, 0.02f);
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
        this.npc1 = new NPC_NORMAL(1571, 1, 0, 14, 8, 9.11f, 0.0f, -10.12f, 45.0f);
        this.npc2 = new NPC_NORMAL(1570, 2, 0, 14, 9, 6.91f, 0.0f, -3.36f, 100.0f);
        this.npc3 = new NPC_NORMAL(1605, 3, 0, 7, 6, 6.34f, 0.0f, 2.35f, 0.0f);
        this.npc4 = new NPC_NORMAL(1602, 4, 0, 7, 7, 10.36f, 0.0f, -20.02f, 0.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 10);
        this.npc1.setInvalidID(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 9);
        this.npc2.setInvalidID(1);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.teiten1 = new Uwamono(28690, 10.5f, 2.5f, -8.0f, 0.0f);
        this.teiten1.SetBgm(196610);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2090.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2090.waitPage(this.win, 64);
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

