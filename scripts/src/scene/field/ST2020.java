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
import xeno.map.MC_KUK02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2020
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK02_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Unit unit;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc10;
    Enepc npc20;
    Enepc sakana1;
    Enepc sakana2;
    Enepc sakana3;
    Enepc sakana4;
    Enepc sakana5;
    Enepc sakana6;
    Enepc sakana7;
    Enepc sakana8;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
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
    int sakanapass = 0;
    int sakanapass2 = 0;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect fade2;
    Light light = new Light(0);
    int page;
    String[] msgALLEN1 = new String[]{"/[label(Allen)]", "Well, I'm going to go back to the Elsa to check on KOS-MOS.\n", "/[waitkey(1)]/[clear()]", "Are you sure you will be all right? Chief...", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION1 = new String[]{"/[label(Shion)]", "Yeah...sorry for losing it earlier.", "/[waitkey(1)]/[clear()]", "I don't...have very good memories of thunder...", "/[waitkey(64)]/[clear()]"};
    String[] msgALLEN12 = new String[]{"/[label(Allen)]", "Chief...", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION12 = new String[]{"/[label(Shion)]", "I'm a little tired, so maybe I'll rest at a hotel in City Sector 27. I'll go to the Elsa after I get a little rest. Please take care of KOS-MOS until then.", "/[waitkey(64)]/[clear()]"};
    String[] msgALLEN2 = new String[]{"/[label(Allen)]", "All right.", "/[waitkey(1)]/[clear()]", "Um, Chief?", "/[waitkey(1)]/[clear()]", "Please don't push yourself too hard.", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION2 = new String[]{"/[label(Shion)]", "Yes, I know.", "/[waitkey(64)]/[close()]"};
    String[] msgSAKANA = new String[]{"/[label()]", "Caught a small fish!", "/[waitkey(64)]/[close()]"};
    String[] msgSAKANA2 = new String[]{"/[label()]", "Caught a big fish!", "/[waitkey(64)]/[close()]"};
    String[] msgSAKANA3 = new String[]{"/[label()]", "Nothing...", "/[waitkey(64)]/[close()]"};
    String[] msgSAKANA4 = new String[]{"/[label()]", "Caught a fish that looks familiar.", "/[waitkey(64)]/[close()]"};
    String[] sakana_1 = new String[]{"/[label()]", "You can catch fish if you use the /[color(0x329bbe)]Fish Detector/[color(0x808080)].", "/[waitkey(1)]/[clear()]", "Go catch fish?", "/[waitkey(64)]/[close()]"};
    String[] ziggy_1 = new String[]{"/[label(MOMO)]", "Oh, Ziggy!", "/[wait(30)]/[close()]"};
    String[] ziggy_2 = new String[]{"/[label(Shion)]", "Is there something wrong?", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_3 = new String[]{"/[label(Ziggy)]", "No, I just came to see how you were doing...", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_4 = new String[]{"/[label(Jr.)]", "Sure that's what you say, but the real reason why you came here is because you wanted to play, right?", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_5 = new String[]{"/[label(Ziggy)]", "Are you all right? You don't look well.", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_6 = new String[]{"/[label(Shion)]", "Oh, it's nothing really. I'm completely fine.", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_7 = new String[]{"/[label(Ziggy)]", "Okay. If you say so, but it's better not to push yourself too much.", "/[waitkey(64)]/[clear()]"};
    String[] ziggy_8 = new String[]{"/[label(Shion)]", "Yes, I know.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST2020() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.395f, 3.487f, 1.058f);
        this.camEV.setRotate(-16.019f, -128.257f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(14.953f, 5.439f, 3.668f);
        this.camEV.setRotate(-29.784f, 0.079f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.628f, 2.623f, -6.146f);
        this.camEV.setRotate(-6.584f, 139.757f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 10: {
                this.sakana1.kickEnepc(10, 80, 0);
                break;
            }
            case 11: {
                this.sakana2.kickEnepc(10, 150, 0);
                break;
            }
            case 12: {
                this.sakana3.kickEnepc(10, 80, 0);
                break;
            }
            case 13: {
                this.sakana4.kickEnepc(10, 80, 0);
                break;
            }
            case 14: {
                this.sakana5.kickEnepc(10, 80, 0);
                break;
            }
            case 15: {
                this.sakana6.kickEnepc(10, 80, 0);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (Runtime.checkItem(10, 71) == 0) {
                    return;
                }
                if (this.sakanapass2 == 1) {
                    return;
                }
                System.println("sakana!!");
                if (this.sakanapass == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.sakana_1, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.player.setID(2);
                            this.sakana1.enableDTKFlag(65536);
                            this.sakana2.enableDTKFlag(65536);
                            this.sakana3.enableDTKFlag(65536);
                            this.sakana4.enableDTKFlag(65536);
                            this.sakana5.enableDTKFlag(65536);
                            this.sakana6.enableDTKFlag(65536);
                            this.sakana1.dispRadar(true);
                            this.sakana2.dispRadar(true);
                            this.sakana3.dispRadar(true);
                            this.sakana4.dispRadar(true);
                            this.sakana5.dispRadar(true);
                            this.sakana6.dispRadar(true);
                            this.sakanapass2 = 1;
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                    }
                    this.sakanapass = 1;
                    Runtime.setPlayerControl(true);
                    break;
                }
                this.sakanapass = 1;
                System.println("sakana2!!");
                break;
            }
            case 1: {
                if (this.sakanapass2 == 1) {
                    return;
                }
                if (this.sakanapass == 0) break;
                this.sakanapass = 0;
                System.println("on!!");
                break;
            }
            case 2: {
                if (Runtime.getFlags(7153, 1) == 1) {
                    return;
                }
                this.fade.call(0);
                Runtime.setPlayerControl(false);
                System.sleep(30);
                this.npc1.setVisible(true);
                this.npc2.setVisible(true);
                this.npc3.setVisible(true);
                this.npc4.setVisible(true);
                this.npc6.setVisible(true);
                this.cam0.setMode(-1);
                this.EV_Camera02();
                this.player.setTranslate(100.0f, 1.0f, 100.0f);
                this.npc4.setTranslate(14.94f, 1.0f, 1.833f);
                this.npc6.kickEnepc(1, 1);
                this.npc6.moveEnepc(15, 14.94f, -5.0f, 180);
                System.sleep(30);
                this.npc4.kickEnepc(1, 1);
                this.npc4.moveEnepc(15, 14.94f, -2.833f, 120);
                this.npc1.kickEnepc(1, 1);
                this.npc1.moveEnepc(15, 16.06f, -1.1f, 100);
                this.npc2.kickEnepc(1, 1);
                this.npc2.moveEnepc(15, 14.47f, -2.88f, 100);
                this.npc2.look_char(this.npc6);
                System.sleep(30);
                this.npc3.kickEnepc(1, 1);
                this.npc3.moveEnepc(15, 13.82f, -1.95f, 100);
                System.sleep(50);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.ziggy_1, 0);
                System.sleep(20);
                this.npc1.kickEnepc(1, 10);
                this.npc2.kickEnepc(1, 3);
                this.npc2.moveEnepc(15, 14.47f, -4.58f, 40);
                System.sleep(30);
                this.npc4.kickEnepc(1, 27);
                this.npc3.kickEnepc(1, 10);
                System.sleep(10);
                this.npc2.kickEnepc(1, 10);
                this.npc2.moveEnepc(17, 140.0f, -0.1f, 10);
                System.sleep(10);
                this.npc6.kickEnepc(1, 0);
                System.sleep(10);
                this.EV_Camera03();
                System.sleep(10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.ziggy_2, 0);
                ST2020.waitPage(this.win, 64);
                this.npc6.kickEnepc(1, 9);
                this.win.print(this.ziggy_3, 0);
                ST2020.waitPage(this.win, 64);
                this.npc6.kickEnepc(1, 0);
                this.npc3.kickEnepc(1, 9);
                this.win.print(this.ziggy_4, 0);
                ST2020.waitPage(this.win, 64);
                this.npc3.kickEnepc(1, 0);
                this.npc6.kickEnepc(0, 10);
                this.win.print(this.ziggy_5, 0);
                ST2020.waitPage(this.win, 64);
                this.npc6.kickEnepc(1, 0);
                this.npc4.kickEnepc(0, 9);
                this.win.print(this.ziggy_6, 0);
                ST2020.waitPage(this.win, 64);
                this.npc6.kickEnepc(1, 9);
                this.win.print(this.ziggy_7, 0);
                ST2020.waitPage(this.win, 64);
                this.npc6.kickEnepc(1, 0);
                this.npc4.kickEnepc(0, 7);
                this.win.print(this.ziggy_8, 0);
                ST2020.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.resetOutFriend(6);
                this.npc1.setVisible(false);
                this.npc2.setVisible(false);
                this.npc3.setVisible(false);
                this.npc4.setVisible(false);
                this.npc6.setVisible(false);
                this.player.setTranslate(14.94f, 1.0f, -2.833f);
                Runtime.setFlags(7153, 1, 1);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                break;
            }
            case 3: {
                if (Runtime.getFlags(315, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7155, 1) != 0) break;
                switch (Runtime.mailArriveCheck(59)) {
                    case 1: {
                        Runtime.mailArriveSet(61);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgMAIL1, 0);
                        ST2020.waitPage(this.win, 64);
                        System.sleep(30);
                        Runtime.setFlags(7155, 1, 1);
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        break block0;
                    }
                }
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc10(Enepc enepc) {
        this.Talk_npc10_1();
    }

    void Talk_npc10_1() {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                Sound.effectPlay(6);
                Runtime.addItemWin(0, 20);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc11(Enepc enepc) {
        this.Talk_npc11_1();
    }

    void Talk_npc11_1() {
        if (Runtime.getFlags(7165, 1) == 0) {
            ++this.talkFlag11;
            switch (this.talkFlag11) {
                case 1: {
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgSAKANA2, 0);
                    ST2020.waitPage(this.win, 64);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 72);
                    Runtime.setFlags(7165, 1, 1);
                    return;
                }
            }
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 15);
            this.win.print(this.msgSAKANA4, 0);
            ST2020.waitPage(this.win, 64);
            return;
        }
        ++this.talkFlag11;
        switch (this.talkFlag11) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSAKANA3, 0);
                ST2020.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc12(Enepc enepc) {
        this.Talk_npc12_1();
    }

    void Talk_npc12_1() {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSAKANA3, 0);
                ST2020.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc13(Enepc enepc) {
        this.Talk_npc13_1();
    }

    void Talk_npc13_1() {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSAKANA3, 0);
                ST2020.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc14(Enepc enepc) {
        this.Talk_npc14_1();
    }

    void Talk_npc14_1() {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                Sound.effectPlay(6);
                Runtime.addItemWin(0, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc15(Enepc enepc) {
        this.Talk_npc15_1();
    }

    void Talk_npc15_1() {
        ++this.talkFlag15;
        switch (this.talkFlag15) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSAKANA3, 0);
                ST2020.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    public void Talk_npc16(Enepc enepc) {
        this.Talk_npc16_1();
    }

    void Talk_npc16_1() {
        ++this.talkFlag16;
        switch (this.talkFlag16) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSAKANA, 0);
                ST2020.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSAKANA3, 0);
                ST2020.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msgSAKANA4, 0);
        ST2020.waitPage(this.win, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msgSHION2, 0);
                ST2020.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msgSHION2, 0);
        ST2020.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msgSHION2, 0);
        ST2020.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("ミニマップ・３");
                Runtime.jumpCF(2130, 3);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.05f, 1.05f, 1.05f);
        this.light.setColor(0, 0.425f, 0.425f, 0.425f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, 0.0f, 1.0f, 2.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(4, 1, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(4, 2, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(4, 3, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 12.5f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 10.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, 15.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.5f, 40.0f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
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
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 30;
        this.fade2.args[2] = 1;
        this.sakana1 = new NPC_NORMAL2(20594, 10, 0, 18, 6, 25.0f, -0.59f, 17.85f, 180.0f);
        this.sakana2 = new NPC_NORMAL2(20594, 11, 0, 18, 6, -12.96f, -1.0f, 30.99f, 0.0f);
        this.sakana3 = new NPC_NORMAL2(20594, 12, 0, 18, 6, 7.75f, -0.95f, 21.78f, 180.0f);
        this.sakana4 = new NPC_NORMAL2(20594, 13, 0, 18, 6, -1.67f, -1.0f, 24.33f, 180.0f);
        this.sakana5 = new NPC_NORMAL2(20594, 14, 0, 18, 6, -10.18f, -1.0f, 26.42f, 180.0f);
        this.sakana6 = new NPC_NORMAL2(20594, 15, 0, 18, 6, -3.53f, -1.0f, 29.87f, 180.0f);
        this.sakana1.disableDTKFlag(65536);
        this.sakana2.disableDTKFlag(65536);
        this.sakana3.disableDTKFlag(65536);
        this.sakana4.disableDTKFlag(65536);
        this.sakana5.disableDTKFlag(65536);
        this.sakana6.disableDTKFlag(65536);
        this.sakana1.dispRadar(false);
        this.sakana2.dispRadar(false);
        this.sakana3.dispRadar(false);
        this.sakana4.dispRadar(false);
        this.sakana5.dispRadar(false);
        this.sakana6.dispRadar(false);
        this.sakana1.talkto("Talk_npc10");
        this.sakana2.talkto("Talk_npc11");
        this.sakana3.talkto("Talk_npc12");
        this.sakana4.talkto("Talk_npc13");
        this.sakana5.talkto("Talk_npc14");
        this.sakana6.talkto("Talk_npc15");
        if (Runtime.getFlags(315, 1) == 0 && Runtime.getFlags(7123, 2) == 0) {
            this.npc6 = new NPC_NORMAL(6, 6, 0, 14, 8, 14.94f, 1.0f, -17.0f, 0.0f);
            this.npc1 = new NPC_NORMAL(3, 1, 0, 14, 10, 16.06f, 1.0f, 2.1f, 180.0f);
            this.npc2 = new NPC_NORMAL(4, 2, 0, 14, 12, 14.47f, 1.0f, 1.88f, 180.0f);
            this.npc3 = new NPC_NORMAL(5, 3, 0, 14, 14, 13.82f, 1.0f, 2.95f, 180.0f);
            this.npc4 = new NPC_NORMAL(1, 4, 0, 14, 16, 13.82f, 1.0f, 2.95f, 180.0f);
            this.npc1.enableDTKFlag(262144);
            this.npc1.kickEnepc(4, 1);
            this.npc1.setInvalidID(1);
            this.npc1.disableDTKFlag(196608);
            this.npc2.enableDTKFlag(262144);
            this.npc2.kickEnepc(4, 1);
            this.npc2.setInvalidID(1);
            this.npc2.disableDTKFlag(196608);
            this.npc3.enableDTKFlag(262144);
            this.npc3.kickEnepc(4, 1);
            this.npc3.setInvalidID(1);
            this.npc3.disableDTKFlag(196608);
            this.npc4.enableDTKFlag(262144);
            this.npc4.kickEnepc(4, 1);
            this.npc4.setInvalidID(1);
            this.npc4.disableDTKFlag(196608);
            this.npc6.enableDTKFlag(262144);
            this.npc6.kickEnepc(4, 1);
            this.npc6.setInvalidID(1);
            this.npc6.disableDTKFlag(196608);
            this.npc1.setVisible(false);
            this.npc2.setVisible(false);
            this.npc3.setVisible(false);
            this.npc4.setVisible(false);
            this.npc6.setVisible(false);
            this.npc5 = new NPC_NORMAL(263, 5, 0, 14, 3, 0.1f, 1.7f, 3.79f, 180.0f);
            this.npc7 = new NPC_NORMAL(1, 7, 0, 14, 16, 0.1f, 1.7f, 3.79f, 0.0f);
            this.npc20 = new NPC_NORMAL(263, 20, 0, 14, 3, 100.0f, 0.0f, 100.0f, 180.0f);
            this.npc5.enableDTKFlag(262144);
            this.npc5.setInvalidID(1);
            this.npc5.kickEnepc(4, 1);
            this.npc5.disableDTKFlag(196608);
            this.npc7.enableDTKFlag(262144);
            this.npc7.setInvalidID(1);
            this.npc7.kickEnepc(4, 1);
            this.npc7.disableDTKFlag(196608);
            this.npc20.setInvalidID(1);
            this.npc20.disableDTKFlag(196608);
            this.npc20.disableDTKFlag(8);
            this.npc20.kickEnepc(4, 1);
            this.npc5.talkto("Talk_npc2");
            this.npc20.talkto("Talk_npc2");
            this.npc20.start(1, "BAYBAY");
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

        void BAYBAY() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST2020.this.cam0.setMode(-1);
            ST2020.this.EV_Camera01();
            ST2020.this.player.setTranslate(100.0f, 1.6f, 100.0f);
            ST2020.this.npc7.setTranslate(-0.13f, 1.6f, 2.32f);
            ST2020.this.npc7.kickEnepc(1, 27);
            System.sleep(10);
            ST2020.this.npc5.kickEnepc(1, 9);
            ST2020.this.win = Window.create();
            ST2020.this.win.setSize(4, 45);
            ST2020.this.win.setLocation(15, 305);
            ST2020.this.win.print(ST2020.this.msgALLEN1, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            ST2020.this.npc5.kickEnepc(1, 10);
            ST2020.this.win.print(ST2020.this.msgSHION1, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            ST2020.this.win.print(ST2020.this.msgALLEN12, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            ST2020.this.win.print(ST2020.this.msgSHION12, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            ST2020.this.npc5.kickEnepc(0, 9);
            ST2020.this.win.print(ST2020.this.msgALLEN2, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            ST2020.this.win.print(ST2020.this.msgSHION2, 0);
            ST2020.waitPage(ST2020.this.win, 64);
            System.sleep(15);
            ST2020.this.fade.call(0);
            System.sleep(30);
            ST2020.this.player.setTranslate(-0.13f, 1.6f, 2.32f);
            ST2020.this.npc5.setVisible(false);
            ST2020.this.npc7.setVisible(false);
            ST2020.this.cam0.setMode(0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            Runtime.setFlags(7123, 2, 1);
        }
    }

    class NPC_NORMAL2
            extends Enepc {
        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
        }
    }
}

