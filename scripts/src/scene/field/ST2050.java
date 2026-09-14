import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2050
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK05_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    boolean WallFlag = false;
    Uwamono doorA;
    Uwamono col;
    Uwamono Atari;
    Enepc enemy1;
    Enepc enemy2;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light10;
    Effect light09;
    Light light = new Light(0);
    Uwamono itembox;
    int page;
    String[] msg10001 = new String[]{"/[label()]", "What? A customer? There's nothing worthwhile being sold here. You still want to buy something?", "/[waitkey(64)]/[close()]"};
    String[] msg10002 = new String[]{"/[label()]", "See, didn't I tell you? Nothing worthwhile to buy, right?", "/[waitkey(64)]/[close()]"};
    String[] msg10003 = new String[]{"/[label()]", "Hey, don't you touch that safe!", "/[waitkey(1)]/[clear()]", "The contents inside are more important to my wife and me than life itself. It's not something for you guys to touch as you please!", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Welcome!", "/[waitkey(1)]/[clear()]", "Oh, you must be Director Gaignun's guests. I have already received payment from Director Gaignun, so please make yourselves at home.", "/[waitkey(1)]/[clear()]", "Would you like to turn in?", "/[waitkey(64)]/[close()]"};
    String[] msg200011 = new String[]{"/[label()]", "Welcome!", "/[waitkey(1)]/[clear()]", "Oh, Little Master, are you making rounds? If you are tired, please don't hesitate to rest here.", "/[waitkey(64)]/[close()]"};
    String[] msg20002 = new String[]{"/[label()]", "My dad is such a headache. That huge safe is so important to him.", "/[waitkey(1)]/[clear()]", "The safe itself may be big, but we don't have anything to put in it. He's stuck on appearances.", "/[waitkey(1)]/[clear()]", "Please forgive my father. He's been like that ever since my mother died.", "/[waitkey(1)]/[clear()]", "Oh, I'm sorry. That doesn't concern you. Would you like to stay the night?", "/[waitkey(64)]/[close()]"};
    String[] msg20003 = new String[]{"/[label()]", "Please make yourselves at home.", "/[waitkey(64)]/[close()]"};
    String[] msg20004 = new String[]{"/[label()]", "Really? Then please come again.", "/[waitkey(64)]/[close()]"};
    String[] msg20005 = new String[]{"/[label()]", "Welcome!", "/[waitkey(1)]/[clear()]", "Oh, you must be Director Gaignun's guests. I have already received payment from Director Gaignun, so please make yourselves at home.", "/[waitkey(64)]/[close()]"};
    String[] msg200051 = new String[]{"/[label()]", "Welcome!", "/[waitkey(1)]/[clear()]", "Oh, Little Master, are you making rounds? If you are tired, please don't hesitate to rest here.", "/[waitkey(64)]/[close()]"};
    String[] msg20006 = new String[]{"/[label()]", "Oh, I'm sorry, but please refrain from going back there without permission.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "My, my, a customer? Rare to see people in this remote area.", "/[waitkey(1)]/[clear()]", "There's nothing noteworthy to see here if you're sightseeing.", "/[waitkey(1)]/[clear()]", "Everyone's been moving to the newly built Sector 36.", "/[waitkey(64)]/[close()]"};
    String[] msg30002 = new String[]{"/[label()]", "Well, as for local specialties, this place is known for the family fights that occur. Just sit back and watch.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Oh, a customer?! Don't worry, nothing to be afraid of.", "/[waitkey(1)]/[clear()]", "That family's always like that. That's their way of bonding. Nothing to worry about.", "/[waitkey(64)]/[close()]"};
    String[] SHION_01 = new String[]{"/[label(Shion)]", "Ah, I feel much better!\n", "/[waitkey(1)]/[clear()]", "Oh, Allen's probably waiting. I better hurry back to the Elsa and start the maintenance on KOS-MOS.", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};

    ST2050() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.817f, 2.355f, 3.936f);
        this.camEV.setRotate(-7.0f, 41.199f, 0.0f);
        this.camEV.setFov(47.499f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.84f, 1.811f, 5.107f);
        this.camEV.setRotate(-8.701f, -40.359f, 0.0f);
        this.camEV.setFov(47.499f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.392f, 1.619f, 3.108f);
        this.camEV.setRotate(-8.876f, -34.96f, 0.0f);
        this.camEV.setFov(47.499f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                this.npc2.kickEnepc(4, 1);
                Runtime.setPlayerControl(false);
                this.npc2.moveEnepc(17, 60.0f, 0.1f, 10);
                System.sleep(10);
                this.player.look_char(this.npc2);
                this.npc2.look_char(this.player);
                System.sleep(1);
                this.cam0.setMode(-1);
                this.EV_Camera02();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg20006, 0);
                ST2050.waitPage(this.win, 64);
                System.sleep(15);
                this.cam0.setMode(0);
                this.npc2.moveEnepc(17, 0.0f, -0.1f, 10);
                System.sleep(10);
                this.player.look_default();
                this.npc2.look_default();
                Runtime.setPlayerControl(true);
                this.npc2.kickEnepc(4, 0);
                break;
            }
            case 1: {
                this.npc1.kickEnepc(4, 1);
                Runtime.setPlayerControl(false);
                this.npc1.moveEnepc(17, 300.0f, -0.1f, 10);
                System.sleep(10);
                this.npc1.kickEnepc(1, 9);
                this.player.look_char(this.npc1);
                this.npc1.look_char(this.player);
                System.sleep(1);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg10003, 0);
                ST2050.waitPage(this.win, 64);
                System.sleep(15);
                this.npc1.moveEnepc(17, 0.0f, 0.1f, 5);
                this.cam0.setMode(0);
                System.sleep(5);
                this.player.look_default();
                this.npc1.look_default();
                Runtime.setPlayerControl(true);
                this.npc1.kickEnepc(4, 0);
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Talk_npc1_1();
    }

    void Talk_npc1_1() {
        this.npc1.kickEnepc(4, 1);
        this.npc1.look_char(this.player);
        this.npc1.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10001, 0);
        ST2050.waitPage(this.win, 64);
        System.sleep(15);
        if (Runtime.getFlags(389, 1) == 1) {
            Runtime.enterShop(13);
        } else if (Runtime.getFlags(362, 1) == 1) {
            Runtime.enterShop(11);
        } else if (Runtime.getFlags(346, 1) == 1) {
            Runtime.enterShop(7);
        } else if (Runtime.getFlags(301, 1) == 1) {
            Runtime.enterShop(9);
        } else {
            Runtime.enterShop(1);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg10002, 0);
        ST2050.waitPage(this.win, 64);
        this.npc1.kickEnepc(4, 0);
    }

    public void Talk_npc2(Enepc enepc) {
        this.Talk_npc2_1();
    }

    public void Talk_npc21(Enepc enepc, Window window) {
        this.Talk_npc21_1(window);
    }

    void Talk_npc21_1(Window window) {
    }

    void Talk_npc2_1() {
        if (Runtime.getFlags(7123, 2) == 1) {
            if (Runtime.getLeader() == 5) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg200051, 0);
                ST2050.waitPage(this.win, 64);
            } else {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg20005, 0);
                ST2050.waitPage(this.win, 64);
            }
            System.sleep(30);
            this.fade1.call(0);
            Runtime.charAllRecovery();
            System.sleep(60);
            if (Runtime.getLeader() == 1) {
                this.npc21.setVisible(true);
                Runtime.enable(65536);
                this.player.setTranslate(100.0f, 0.0f, 100.0f);
                this.player.rotY(1, 300.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
                this.player.look_char(this.npc21);
            } else {
                this.npc21.setVisible(true);
                Runtime.enable(65536);
                this.player.setTranslate(2.73f, 0.0f, 1.07f);
                this.player.rotY(1, 300.0f, true);
                this.player.mtn(28, 9, 1.0f, true);
                this.player.look_char(this.npc21);
            }
            this.cam0.setMode(-1);
            this.EV_Camera03();
            this.fade2.call(0);
            System.sleep(60);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SHION_01, 0);
            ST2050.waitPage(this.win, 64);
            this.fade.call(0);
            System.sleep(28);
            this.npc21.setVisible(false);
            if (Runtime.getLeader() == 1) {
                this.player.setTranslate(1.83f, 0.0f, 1.42f);
                this.player.mtn(1, 9, 1.0f, true);
                this.player.rotY(1, 0.0f, true);
                this.player.look_default();
            } else {
                this.player.setTranslate(1.83f, 0.0f, 1.42f);
                this.player.mtn(1, 9, 1.0f, true);
                this.player.rotY(1, 0.0f, true);
                this.player.look_default();
            }
            System.sleep(2);
            this.npc21.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(65536);
            this.cam0.setMode(0);
            Runtime.disable(65536);
            Sound.effectPlay(26);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SYS_01, 0);
            ST2050.waitPage(this.win, 64);
            Runtime.setFlags(7123, 2, 2);
            return;
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg20002, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Rest\nDon't rest");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg20003, 0);
                ST2050.waitPage(this.win, 64);
                System.sleep(30);
                this.fade1.call(0);
                Runtime.charAllRecovery();
                System.sleep(60);
                this.fade2.call(0);
                System.sleep(65);
                Sound.effectPlay(26);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_01, 0);
                ST2050.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg20004, 0);
        ST2050.waitPage(this.win, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg30001, 0);
                ST2050.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg30002, 0);
        ST2050.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2050.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外概観１・３");
                Runtime.jumpCF(2030, 3);
                break;
            }
            case 1: {
                System.println("宿屋２Ｆ・１");
                Runtime.jumpCF(2060, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(2, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(2, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.15f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.425f, 0.425f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, -0.15f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1713, -1.0f, 3.0f, 0.6f, 0.0f);
        this.light02 = new Effect(1713, 6.1f, 2.2f, -4.4f, 0.0f);
        this.light03 = new Effect(1713, 8.9f, 2.2f, -4.4f, 0.0f);
        this.light04 = new Effect(1405, 9.3f, 1.5f, -3.7f, 0.0f);
        this.light05 = new Effect(1405, 9.3f, 1.5f, -1.7f, 0.0f);
        this.light06 = new Effect(1405, -3.0f, 1.5f, 6.8f, 0.0f);
        this.light07 = new Effect(1405, -8.9f, 1.2f, 6.2f, 0.0f);
        this.light08 = new Effect(1405, 3.0f, 1.5f, -0.5f, 0.0f);
        this.light09 = new Effect(1405, 10.6f, 1.5f, 0.6f, 0.0f);
        this.light10 = new Effect(1405, 6.8f, 1.5f, 6.7f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.5f, 0.5f, 0.5f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.2f, 0.2f, 0.2f);
        this.light05.setScale(0.2f, 0.2f, 0.2f);
        this.light06.setScale(0.2f, 0.2f, 0.2f);
        this.light07.setScale(0.2f, 0.2f, 0.2f);
        this.light08.setScale(0.2f, 0.2f, 0.2f);
        this.light09.setScale(0.2f, 0.2f, 0.2f);
        this.light10.setScale(0.2f, 0.2f, 0.2f);
        Runtime.progressEffect(60);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.col = new Uwamono(28672, 7.5f, 0.0f, -0.075f, 0.0f);
        this.col.SetHitKind('\u0002');
        this.col.SetSize(3.0f, 1.0f, 0.63f);
        this.Atari = new Uwamono(28672, -9.08f, 0.0f, -0.2f, 0.0f);
        this.Atari.SetSize(1.0f, 1.0f, 0.5f);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 47.5f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 47.5f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 7.5f, 47.5f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 5.5f, 47.5f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 7.0f, 47.5f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
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
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.itembox = new Uwamono(28677, -2.49f, 0.0f, -3.98f, 180.0f, 244);
        this.itembox.SetSymbol(28684);
        new Uwamono(28733, 9.0f, 0.0f, 3.9f);
        this.npc1 = new NPC_NORMAL(1614, 1, 0, 14, 9, -3.56f, 0.0f, -1.23f, 0.0f);
        this.npc2 = new NPC_NORMAL(1607, 2, 0, 14, 10, 0.8f, 0.0f, 0.63f, 210.0f);
        this.npc3 = new NPC_NORMAL(1582, 3, 0, 14, 8, 6.4f, 0.0f, 5.35f, 180.0f);
        this.npc4 = new NPC_NORMAL(1573, 4, 0, 7, 11, -5.51f, 0.0f, 4.0f, 270.0f);
        this.npc1.setInvalidID(1);
        this.npc2.setInvalidID(1);
        this.npc1.setMotion(0, 10);
        this.npc1.enableDTKFlag(262144);
        this.npc2.setMotion(0, 9);
        this.npc1.enableDTKFlag(262144);
        this.npc2.disableDTKFlag(1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 2);
        this.npc3.setInvalidID(1);
        this.npc4.setMotion(0, 9);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc20 = new NPC_NORMAL(1573, 20, 0, 14, 11, -3.59f, 0.0f, -0.1f, 270.0f);
        this.npc20.setInvalidID(1);
        this.npc20.setVisible(false);
        this.npc20.talkto("Talk_npc1");
        if (Runtime.getFlags(7123, 2) == 1) {
            this.npc21 = new NPC_NORMAL(1, 21, 0, 14, 17, 1.83f, 0.0f, 1.42f, 0.0f);
            this.npc21.enableDTKFlag(262144);
            this.npc21.setMotion(0, 27);
            this.npc21.setInvalidID(1);
            this.npc21.setVisible(false);
            this.npc21.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(65536);
            this.npc21.talkto("Talk_npc21");
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

