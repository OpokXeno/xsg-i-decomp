import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2080
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK08_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    boolean WallFlag = false;
    Uwamono doorA;
    Uwamono col;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
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
    boolean EneterCheck = false;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
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
    Uwamono teiten1;
    MAPUnit Oto;
    int page;
    String[] NPC_TALK1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "What is it?\n", "/[waitkey(1)]/[clear()]", "I'm on break right now. Please don't bother me during my precious time off! I'm the type that just can't sit still when books aren't arranged neatly on bookshelves.\nI tidy up everyday.", "/[waitkey(64)]/[close()]"};
    String[] msg20002 = new String[]{"/[label()]", "These books were given to us by Little Master.", "/[waitkey(1)]/[clear()]", "Apparently, they're considered antiques on the market, and I hear you could easily buy a large cruiser with just the books we have here.", "/[waitkey(1)]/[clear()]", "Of course, these books are meant to be read, so I would never let them go.", "/[waitkey(64)]/[close()]"};
    String[] msg200021 = new String[]{"/[label()]", "Little Master! Thank you for all you do for us.", "/[waitkey(1)]/[clear()]", "Look! Thanks to you, this shelf is almost full.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "Man, the boss here uses people like slaves. And then, he says he's too shy and won't even set foot outside the shop. Good grief!", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "The boss here says that skill should be learned through observation, but meanwhile, he refuses to teach me anything. That's why I peek at the boss's work from here.", "/[waitkey(1)]/[clear()]", "Oh, please don't tell anyone about this.", "/[waitkey(64)]/[close()]"};

    ST2080() {
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.181f, 1.487f, -7.583f);
        this.camEV.setRotate(-2.594f, 108.575f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.694f, 3.439f, -3.94f);
        this.camEV.setRotate(-36.282f, 37.64f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                Runtime.enable(65536);
                this.player.setTranslate(15.64f, 0.0f, -6.97f);
                System.sleep(120);
                Runtime.disable(65536);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.msg200021, 0);
            ST2080.waitPage(window, 64);
            return;
        }
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg20001, 0);
                ST2080.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg20002, 0);
        ST2080.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2080.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2080.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("外概観８");
                Runtime.jumpCF(2070, 8);
                break;
            }
            case 1: {
                System.println("外概観１０");
                Runtime.jumpCF(2070, 10);
                break;
            }
            case 2: {
                System.println("外概観９");
                Runtime.jumpCF(2070, 9);
                break;
            }
            case 3: {
                System.println("外概観６");
                Runtime.jumpCF(2070, 6);
                break;
            }
            case 4: {
                System.println("外概観７");
                Runtime.jumpCF(2070, 7);
                break;
            }
            case 5: {
                System.println("外概観１６");
                Runtime.jumpCF(2070, 16);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
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
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, -0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(3, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(3, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, -0.075f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1405, 17.8f, 2.1f, -19.5f, 0.0f);
        this.light02 = new Effect(1405, 13.7f, 2.1f, -21.8f, 0.0f);
        this.light03 = new Effect(1405, 11.5f, 2.1f, -21.8f, 0.0f);
        this.light04 = new Effect(1405, 9.3f, 2.1f, -21.8f, 0.0f);
        this.light05 = new Effect(1405, 5.2f, 2.1f, -21.0f, 0.0f);
        this.light06 = new Effect(1405, 5.2f, 2.1f, -16.0f, 0.0f);
        this.light07 = new Effect(1405, 5.2f, 2.1f, -12.0f, 0.0f);
        this.light08 = new Effect(1405, 5.2f, 2.1f, -7.0f, 0.0f);
        this.light09 = new Effect(1405, 2.3f, 2.1f, -5.5f, 0.0f);
        this.light10 = new Effect(1405, 0.2f, 2.1f, -1.0f, 0.0f);
        this.light11 = new Effect(1405, 1.5f, 1.7f, -18.8f, 0.0f);
        this.light12 = new Effect(1405, 0.2f, 1.4f, -12.0f, 0.0f);
        this.light13 = new Effect(1405, 14.3f, 1.1f, -15.9f, 0.0f);
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
        this.light11.setScale(0.5f, 0.5f, 0.5f);
        this.light12.setScale(0.5f, 0.5f, 0.5f);
        this.light13.setScale(0.5f, 0.5f, 0.5f);
        this.fire = new Effect(1402, 9.5f, 0.7f, -10.8f, 0.0f);
        this.fire.setScale(0.3f, 0.3f, 0.3f);
        Runtime.progressEffect(60);
        this.teiten1 = new Uwamono(28690, 9.5f, 0.7f, -10.8f, 0.0f);
        this.teiten1.SetBgm(196609);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.015f, 0.015f);
        this.cam0.setCFLockX(9, 6.5f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 4.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.015f, 0.015f);
        this.cam0.setCFLockX(10, 6.5f);
        this.cam0.setCFAngle(11, -28.0f, 25.0f, 0.0f, 6.5f, 40.0f);
        this.cam0.setCFHokan(11, 0.015f, 0.015f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.015f, 0.015f);
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
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 237);
        new Uwamono(64, 77);
        new Uwamono(65, 77, this.item01);
        new Uwamono(66, 67);
        new Uwamono(69, 75);
        this.npc1 = new NPC_NORMAL(1597, 1, 0, 14, 5, 12.55f, 0.0f, -8.44f, 0.0f);
        this.npc2 = new NPC_NORMAL(1540, 2, 0, 14, 7, 3.18f, 0.0f, -17.79f, 180.0f);
        this.npc3 = new NPC_NORMAL(1541, 3, 0, 2, 6, 1.53f, 0.0f, -9.74f, 0.0f);
        this.npc4 = new NPC_NORMAL(1541, 4, 0, 14, 6, 17.17f, 0.0f, -6.82f, 310.0f);
        this.npc4.setMotion(0, 17);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.Oto = new Mapunits();
        this.Oto.mapUnit(42);
        this.Oto.start(1, "Oto_Move");
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

        void Oto_Move() {
            ST2080.this.player.getTranslate();
            if (ST2080.this.player.px < 8.0f || ST2080.this.player.px > 15.0f) {
                ST2080.this.teiten1.setTranslate(0.0f, -1000.0f, 0.0f);
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

