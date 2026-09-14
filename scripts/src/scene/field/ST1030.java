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
import xeno.map.MC_UTK03_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1030
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK03_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Uwamono doorA;
    Unit monitorA;
    Unit monitorB;
    boolean SCREEN_FLAG = false;
    boolean FIN_FLAG = false;
    boolean EnterCheck = false;
    boolean MARY_ARRIVAL = false;
    boolean MARY_STOP = false;
    boolean JR_FLAG_1 = false;
    boolean Boss_Flag = false;
    boolean Boss_Look = false;
    Effect Obj600_A;
    Effect Obj600_B;
    Effect Obj601;
    Effect B1;
    Effect B2;
    Effect Fade;
    Effect Fade_long;
    Effect fade;
    int talkFlag = 0;
    int n_count = 0;
    Enepc Boss;
    Enepc Mary;
    Enepc dummy;
    MAPUnit separator;
    MAPUnit sound;
    MAPUnit boss_oto;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int[] RedScreenParam1;
    int[] RedScreenParam2;
    int[] BlackScreenParam;
    int[] DefaultScreenParam;
    int MARY_IDLE;
    int MARY_WALK;
    int MARY_WALK_ST;
    int MARY_TURN;
    int MARY_YES;
    int MARY_TALK;
    int MARY_EXT1;
    int MARY_EXT2;
    int MARY_POINT;
    int BOSS_EXT;
    int EVENT_CALL;
    int CF_EVENT_FINISH;
    int page;
    String[] Q1;
    String[] TALK1;
    String[] TALK2;
    String[] TALK3;
    String[] J1;
    String[] J2;
    String[] J3;
    String[] J3_A;
    String[] J4;
    String[] J5;
    String[] J6;
    String[] J7;
    String[] J8;
    String[] M1;
    String[] M2;
    String[] M3;
    String[] M4;
    String[] M5;
    String[] M5_1;
    String[] M6;
    String[] M7;
    String[] D1;
    String[] D2;
    String[] D3;
    String[] SECURITY;

    ST1030() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 0x600000FF;
        this.RedScreenParam1 = nArray;
        int[] nArray2 = new int[8];
        nArray2[1] = 1;
        nArray2[2] = 0x100000;
        nArray2[3] = 0x60000088;
        this.RedScreenParam2 = nArray2;
        int[] nArray3 = new int[8];
        nArray3[1] = 1;
        nArray3[2] = 0x100000;
        nArray3[3] = 0x40000000;
        this.BlackScreenParam = nArray3;
        int[] nArray4 = new int[8];
        nArray4[2] = 0x100000;
        nArray4[3] = 0x40404040;
        this.DefaultScreenParam = nArray4;
        this.MARY_IDLE = 0;
        this.MARY_WALK = 1;
        this.MARY_WALK_ST = 2;
        this.MARY_TURN = 6;
        this.MARY_YES = 7;
        this.MARY_TALK = 8;
        this.MARY_EXT1 = 9;
        this.MARY_EXT2 = 10;
        this.MARY_POINT = 11;
        this.BOSS_EXT = 30;
        this.EVENT_CALL = Runtime.getFlags(6034, 1);
        this.CF_EVENT_FINISH = Runtime.getFlags(6036, 1);
        this.Q1 = new String[]{"The terminal is flashing. Operate it?", "/[waitkey(64)]/[close()]"};
        this.TALK1 = new String[]{"/[label(Mary)]", "Shall we take a look at the bridge? Help out too, Little Master.", "/[waitkey(64)]/[close()]"};
        this.TALK2 = new String[]{"/[label(Mary)]", "What is it, Little Master? Something the matter?", "/[waitkey(64)]/[close()]"};
        this.TALK3 = new String[]{"/[label(Mary)]", "...\n", "...\n", "...", "/[waitkey(1)]/[clear()]", "Hmm, this console is strange. It sure does stand out from the rest.", "/[waitkey(64)]/[close()]"};
        this.J1 = new String[]{"/[label(Jr.)]", "Looks like this is the bridge.", "/[waitkey(64)]/[close()]"};
        this.J2 = new String[]{"/[label(Jr.)]", "Of course. We came for the mainframe.", "/[waitkey(64)]/[close()]"};
        this.J3 = new String[]{"/[label(Jr.)]", "This thing?", "/[waitkey(64)]/[close()]"};
        this.J3_A = new String[]{"/[label(Jr.)]", "This should be okay, right?", "/[waitkey(64)]/[close()]"};
        this.J4 = new String[]{"/[label(Jr.)]", "So...you know what you need to do?", "/[waitkey(64)]/[close()]"};
        this.J5 = new String[]{"/[label(Jr.)]", "I see. It doesn't look like it's so hard that you'd have a tough time with it.", "/[waitkey(64)]/[close()]"};
        this.J6 = new String[]{"/[label(Jr.)]", "Mary!", "/[waitkey(64)]/[close()]"};
        this.J7 = new String[]{"/[label(Jr.)]", "Yeah. Before he finds us...", "/[waitkey(64)]/[close()]"};
        this.J8 = new String[]{"/[label(Jr.)]", "Hmm...?", "/[waitkey(64)]/[close()]"};
        this.M1 = new String[]{"/[label(Mary)]", "Little Master, do you remember our objective?", "/[waitkey(64)]/[close()]"};
        this.M2 = new String[]{"/[label(Mary)]", "Then let's take care of it quickly!", "/[waitkey(64)]/[close()]"};
        this.M3 = new String[]{"/[label(Mary)]", "Yes...probably, but don't touch it yet.", "/[waitkey(64)]/[close()]"};
        this.M4 = new String[]{"/[label(Mary)]", "Wait just a moment.", "/[waitkey(64)]/[close()]"};
        this.M5 = new String[]{"/[label(Mary)]", "Since this is that, so...", "/[waitkey(64)]/[close()]"};
        this.M5_1 = new String[]{"/[label(Mary)]", "Huh...? This thing is...?", "/[waitkey(64)]/[close()]"};
        this.M6 = new String[]{"/[label(Mary)]", "Well, it might take a little while, but I can handle it.", "/[waitkey(64)]/[close()]"};
        this.M7 = new String[]{"/[label(Mary)]", "Little Master, I told you not to touch it! This isn't the right terminal! What are we going to do now?!", "/[waitkey(64)]/[close()]"};
        this.D1 = new String[]{"/[label(Jr.)]", "No need to go back right now.", "/[waitkey(64)]/[close()]"};
        this.D2 = new String[]{"/[label(Mary)]", "Little Master, where are you going?", "/[waitkey(64)]/[close()]"};
        this.D3 = new String[]{"/[label(Jr.)]", "Hmm?\n", "...\n", "...", "/[waitkey(1)]/[clear()]", "I thought I felt something above me...", "/[waitkey(64)]/[close()]"};
        this.SECURITY = new String[]{"/[label(Warning)]", "Security system activated.\n", "Security system activated.\n", "Security system activated.\n", "All hands, please evacuate immediately.\n", "All hands, please evacuate immediately.\n", "All hands, please evacuate immediately.", "/[waitkey(64)]/[close()]"};
    }

    void CF_EVENT() {
        System.println("CF_EVENT START !!!");
        this.sound.start(1, "Sound_ReverseY");
        this.EV_Camera00_A();
        int n = 0;
        while (n <= 1080) {
            if (n == 6) {
                this.Mary.kickEnepc(2, this.MARY_EXT2, 0, 150, 1, 65);
            } else if (n == 50) {
                if (!this.JR_FLAG_1) {
                    this.player.mtn(6, 0, 19, 30, 1, 1.0f, true);
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                if (this.talkFlag != 2) {
                    this.win.print(this.J3, 0);
                } else {
                    this.win.print(this.J3_A, 0);
                }
            } else if (n == 100) {
                System.waitFor(this.win);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M3, 0);
            } else if (n == 250) {
                System.waitFor(this.win);
                this.player.mtn(11, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.J4, 0);
                this.EV_Camera00_BF();
                System.sleep(5);
                this.EV_Camera00_B();
                this.player.setShadow(3, 16);
            } else if (n == 265) {
                System.waitFor(this.win);
                this.Mary.kickEnepc(0, this.MARY_TALK);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M4, 0);
            } else if (n == 450) {
                this.Mary.moveEnepc(17, 20.0f, 1.0f, 50);
            } else if (n == 550) {
                this.Mary.moveEnepc(17, 10.0f, -1.0f, 45);
            } else if (n == 600) {
                System.waitFor(this.win);
                this.Mary.kickEnepc(2, this.MARY_POINT, 0, 95, 1, 65);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M5, 0);
            } else if (n == 700) {
                System.waitFor(this.win);
                this.Mary.kickEnepc(0, this.MARY_EXT2);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M5_1, 0);
            } else if (n == 760) {
                this.player.mtn(6, 1, 1.0f, true);
            } else if (n == 860) {
                System.waitFor(this.win);
                this.Mary.moveEnepc(17, 90.0f, 1.0f, 10);
                System.sleep(1);
                this.Mary.kickEnepc(9, 100);
                this.Mary.kickEnepc(0, this.MARY_TALK);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M6, 0);
            } else if (n == 950) {
                this.player.setRotate(0.0f, 0.0f, 0.0f);
                System.waitFor(this.win);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.J5, 0);
            } else if (n == 1020) {
                System.waitFor(this.win);
                this.player.mtn(12, 1, 1.0f, true);
            } else if (n == 1050) {
                this.Obj600_A.disp(false);
                this.Obj600_B.disp(true);
                Sound.effectPlay(196741);
                this.teiten7.setTranslate(0.0f, -1000.0f, 0.0f);
                this.Obj601.disp(false);
                this.monitorA.signal(0);
                this.monitorB.signal(1);
                Sound.effectPlay(196745);
            } else if (n == 1075) {
                System.waitFor(this.win);
                this.Boss.setTP(0);
                this.Boss.setTranslate(0.0f, 4.375f, 14.0f);
                this.EV_Camera4();
                this.player.setShadow(4, 16);
            }
            System.sleep(1);
            ++n;
        }
        Stage.setVisible(0, false);
        this.EV_Camera04();
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SECURITY, 0);
        int n2 = 0;
        while (n2 < 200) {
            this.Boss.setTP((int) ((float) n2 * 5.0f));
            if (n2 == 0) {
                this.separator.start(1, "Screen");
                this.Mary.setRotateY(0.0f);
                this.Mary.kickEnepc(9, -1);
                this.Mary.kickEnepc(1, this.MARY_EXT1);
            }
            System.sleep(1);
            ++n2;
        }
        this.Boss.setTP(999);
        System.waitFor(this.win);
        this.B1.disp(true);
        this.Boss.setTP(1000);
        System.sleep(4);
        this.B1.disp(false);
        this.EV_Camera05();
        this.Boss.kickEnepc(4, 0);
        this.Boss.kickEnepc(4, 1);
        this.Boss.kickEnepc(0, 23);
        Sound.effectPlay(196747);
        int n3 = 0;
        while (n3 <= 30) {
            if (n3 == 7) {
                this.Boss.setTranslate(0.0f, 8.0f, 8.0f);
            } else if (n3 == 12) {
                this.Boss.setTranslate(0.0f, 2.5f, 8.0f);
            } else if (n3 == 14) {
                this.Boss.setInvalidID(1);
            } else if (n3 == 15) {
                this.Boss.setTranslate(0.0f, 0.0f, 8.0f);
                this.B2.disp(true);
                this.EV_Camera06();
            }
            System.sleep(1);
            ++n3;
        }
        int n4 = 0;
        while (n4 < 30) {
            if (n4 == 0) {
                this.EV_Camera07();
            } else if (n4 == 10) {
                this.Boss.kickEnepc(3, 27, 0, 159, 1, 200);
                this.boss_oto.start(1, "Boss_Move");
            } else if (n4 == 15) {
                this.Mary.moveEnepc(17, 90.0f, 1.0f, 5);
            } else if (n4 == 20) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M7, 0);
                this.Mary.kickEnepc(9, 100);
                this.Mary.kickEnepc(0, this.MARY_TALK);
            }
            System.sleep(1);
            ++n4;
        }
        System.waitFor(this.win);
        this.Boss_Flag = false;
        while (!this.Boss_Look) {
            System.sleep(1);
        }
        this.Boss.kickEnepc(4, 0);
        this.Boss.kickEnepc(4, 1);
        this.Boss.kickEnepc(9, -1);
        this.Boss.kickEnepc(1, 3);
        System.sleep(5);
        int n5 = 0;
        while (n5 <= 40) {
            if (n5 == 0) {
                this.Boss_Flag = false;
                this.Boss.moveEnepc(15, 0.15f, -2.01f, 60);
                Sound.effectPlay(196749);
                this.EV_Camera08();
            } else if (n5 == 25) {
                this.player.mtn(27, 1, 1.0f, true);
            } else if (n5 == 30) {
                this.Mary.kickEnepc(9, -1);
                this.Mary.moveEnepc(17, 0.0f, -1.0f, 10);
            }
            System.sleep(1);
            ++n5;
        }
    }

    void CF_EVENT1() {
        System.println("CF_EVENT1 開始");
        Runtime.disable(524288);
        this.Light1();
        this.Mary.kickEnepc(0, this.MARY_IDLE);
        this.doorA.SetDoorType('\u0002');
        int n = 0;
        while (n <= 320) {
            if (n == 0) {
                this.EV_Camera02();
            } else if (n == 50) {
                this.player.mtn(2, 0, 25, 25, 8, 1.0f, true);
                this.player.move(68, 0.3f, 11.0f, true);
                this.Mary.moveEnepc(15, -0.3f, 11.0f, 82);
                this.Mary.kickEnepc(1, this.MARY_WALK);
            } else if (n == 121) {
                this.player.mtn(1, 8, 1.0f, true);
            } else if (n == 133) {
                this.Mary.kickEnepc(1, this.MARY_IDLE);
            } else if (n == 135) {
                this.doorA.DoorClose();
            } else if (n == 190) {
                this.Mary.kickEnepc(3, this.MARY_EXT2, 0, 150, 1, 60);
            } else if (n == 220) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.J1, 0);
            } else if (n == 230) {
                System.waitFor(this.win);
            }
            System.sleep(1);
            ++n;
        }
        this.Mary.setShadow(0, 0);
        int n2 = 0;
        while (n2 <= 230) {
            if (n2 == 0) {
                this.EV_Camera03();
                this.sound.start(1, "Sound_Move");
            } else if (n2 == 190) {
                this.Mary.moveEnepc(17, 140.0f, -1.0f, 32);
            } else if (n2 == 200) {
                this.Mary.kickEnepc(9, 100);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M1, 0);
            } else if (n2 == 220) {
                System.waitFor(this.win);
                this.DefaultTalk(this.J2);
                System.waitFor(this.win);
                this.Mary.moveEnepc(17, 180.0f, 1.0f, 32);
                System.sleep(10);
                this.Mary.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.M2, 0);
                System.waitFor(this.win);
            }
            System.sleep(1);
            ++n2;
        }
        System.sleep(10);
        this.Mary.kickEnepc(0, this.MARY_IDLE);
        this.Mary.kickEnepc(4, 0);
        System.sleep(40);
        this.doorA.SetDoorType('\u0004');
        this.cam0.setMode(0);
        this.player.mtn(1, 1, 1.0f, true);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
        Runtime.enable(524288);
        this.player.setID(1);
        this.DefaultLight();
        this.Mary.setShadow(4, 16);
        this.sound.start(1, "Sound_Reverse");
        Runtime.setFlags(6034, 1, 1);
        this.EVENT_CALL = Runtime.getFlags(6034, 1);
        System.println("CF_EVENT1 終了");
    }

    void DefaultLight() {
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(2, 30);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void EOB(int n) {
        System.println("EOB !!!!!!!!");
        switch (n) {
            case 1: {
                System.println("case 1 *************:");
                System.println("プレイヤーのスクリプト制御解除");
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                Runtime.setFlags(6036, 1, 1);
                this.CF_EVENT_FINISH = Runtime.getFlags(6036, 1);
                System.println("CFイベント終了");
                if (Runtime.getFlags(152, 1) != 0) break;
                System.println("Runtime.jumpEvent(SCE02038)が実行されます");
                Runtime.setFlags(152, 1, 1);
                Runtime.jumpEvent(2380);
                break;
            }
        }
    }

    void EV_Camera00_A() {
        System.println("A カメラ");
        float[] fArray = new float[]{1.0f, 6.8409142f, 6.1919312f, 11.704142f, 1000.0f, 3.514465f, 6.223903f, 11.478978f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -11.634224f;
        fArray2[2] = 24.659239f;
        fArray2[4] = 1000.0f;
        fArray2[5] = -14.054099f;
        fArray2[6] = 18.739162f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 1000);
        this.camEV.rotateSPL(fArray3, 1, 1, 1000);
        this.camEV.setFov(35.1997f);
        this.camEV.change();
    }

    void EV_Camera00_B() {
        System.println("B カメラ");
        float[] fArray = new float[]{1.0f, 1.4026738f, 2.7998428f, 0.9731244f, 3000.0f, 2.8404768f, 2.7998428f, 0.2710816f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -24.996845f;
        fArray2[2] = 27.093838f;
        fArray2[4] = 3000.0f;
        fArray2[5] = -23.61683f;
        fArray2[6] = 58.892944f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 4000);
        this.camEV.rotateSPL(fArray3, 1, 1, 4000);
        this.camEV.setFov(19.519936f);
        this.camEV.change();
    }

    void EV_Camera00_BF() {
        System.println("B カメラ フォロー");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.4026738f, 2.7998428f, 0.9731244f);
        this.camEV.setRotate(-24.996845f, 27.093838f, 0.0f);
        this.camEV.setFov(19.519936f);
        this.camEV.change();
    }

    void EV_Camera01() {
        System.println("イベント（回転）操作 カメラ");
        float[] fArray = new float[]{1.0f, -2.4625082f, 3.887941f, 1.9409868f, 100.0f, -4.483611f, 8.111832f, 11.174996f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -29.91834f;
        fArray2[2] = -40.89983f;
        fArray2[4] = 100.0f;
        fArray2[5] = -43.118046f;
        fArray2[6] = -21.95966f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 30);
        this.camEV.rotateSPL(fArray3, 1, 2, 30);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        System.println("イベント操作02 カメラ");
        float[] fArray = new float[]{1.0f, 2.2010486f, 2.1434062f, 8.564237f, 300.0f, 1.7464036f, 2.1434062f, 6.455716f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -16.753f;
        fArray2[2] = 139.95789f;
        fArray2[4] = 300.0f;
        fArray2[5] = -11.190601f;
        fArray2[6] = 160.80342f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 300);
        this.camEV.rotateSPL(fArray3, 1, 2, 300);
        this.camEV.setFov(18.57275f);
        this.camEV.change();
    }

    void EV_Camera03() {
        System.println("イベント操作02 カメラ");
        float[] fArray = new float[8];
        fArray[0] = 1.0f;
        fArray[2] = 2.8395534f;
        fArray[3] = 12.048007f;
        fArray[4] = 250.0f;
        fArray[6] = 1.5275279f;
        fArray[7] = 12.127861f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[0] = 1.0f;
        fArray3[1] = 16.63977f;
        fArray3[2] = -0.0426466f;
        fArray3[4] = 250.0f;
        fArray3[5] = -9.900067f;
        fArray3[6] = -0.0426466f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 2, 250);
        this.camEV.rotateSPL(fArray4, 1, 2, 250);
        this.camEV.setFov(43.519257f);
        this.camEV.change();
    }

    void EV_Camera04() {
        System.println("ボス出現 カメラ1");
        float[] fArray = new float[8];
        fArray[1] = -7.4173265f;
        fArray[2] = 6.447854f;
        fArray[3] = 5.2402964f;
        fArray[4] = 200.0f;
        fArray[5] = -3.8178523f;
        fArray[6] = 2.3519316f;
        fArray[7] = 3.7551014f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[1] = 3.2817392f;
        fArray3[2] = -141.7581f;
        fArray3[4] = 200.0f;
        fArray3[5] = 19.761395f;
        fArray3[6] = -158.01901f;
        float[] fArray4 = fArray3;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray2, 1, 2, 200);
        this.camEV.rotateSPL(fArray4, 1, 2, 200);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        System.println("ボス出現 カメラ2");
        float[] fArray = new float[]{1.0f, -3.8178523f, 2.3519316f, 3.7551014f, 15.0f, -3.8178523f, 2.3519316f, -3.0f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 19.761395f;
        fArray2[2] = -158.01901f;
        fArray2[4] = 15.0f;
        fArray2[5] = 1.0f;
        fArray2[6] = -158.01901f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 15);
        this.camEV.rotateSPL(fArray3, 1, 2, 15);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        System.println("ボス出現 カメラ3（着地)");
        this.camEV = Camera.create(1);
        float[] fArray = new float[]{1.0f, -3.8178523f, 2.3519316f, -3.0f, 2.0f, -3.8178523f, 2.0519314f, -3.0f, 3.0f, -3.8178523f, 1.7519315f, -3.0f, 4.0f, -3.8178523f, 2.0519314f, -3.0f, 5.0f, -3.8178523f, 2.3519316f, -3.0f, 6.0f, -3.8178523f, 2.0519314f, -3.0f, 7.0f, -3.8178523f, 1.7519315f, -3.0f, 8.0f, -3.8178523f, 2.0519314f, -3.0f, 9.0f, -3.8178523f, 2.3519316f, -3.0f, 10.0f, -3.8178523f, 2.0519314f, -3.0f, 11.0f, -3.8178523f, 1.7519315f, -3.0f, 12.0f, -3.8178523f, 2.0519314f, -3.0f, 13.0f, -3.8178523f, 2.3519316f, -3.0f};
        this.camEV.transSPL(fArray, 0, 2, 13);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        System.println("ボス出現 カメラ4（索敵)");
        this.camEV = Camera.create(1);
        float[] fArray = new float[8];
        fArray[1] = -3.8178523f;
        fArray[2] = 2.3519316f;
        fArray[3] = -3.0f;
        fArray[4] = 30.0f;
        fArray[5] = -0.5055733f;
        fArray[6] = 1.5161457f;
        fArray[7] = -4.588365f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[1] = 1.0f;
        fArray3[2] = -158.01901f;
        fArray3[4] = 30.0f;
        fArray3[5] = -0.7409238f;
        fArray3[6] = -177.11835f;
        float[] fArray4 = fArray3;
        this.camEV.transSPL(fArray2, 1, 1, 30);
        this.camEV.rotateSPL(fArray4, 1, 1, 30);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera08() {
        System.println("ボス出現 カメラ5（ダッシュ攻撃)");
        this.camEV = Camera.create(1);
        float[] fArray = new float[8];
        fArray[1] = -0.5055733f;
        fArray[2] = 1.5161457f;
        fArray[3] = -4.588365f;
        fArray[4] = 50.0f;
        fArray[5] = -2.1759145f;
        fArray[6] = 0.8799825f;
        fArray[7] = -8.724492f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[8];
        fArray3[1] = -0.7409238f;
        fArray3[2] = -177.11835f;
        fArray3[4] = 50.0f;
        fArray3[5] = 9.781712f;
        fArray3[6] = -167.8159f;
        float[] fArray4 = fArray3;
        this.camEV.transSPL(fArray2, 1, 2, 50);
        this.camEV.rotateSPL(fArray4, 1, 2, 50);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera1() {
        System.println("キーボード操作 カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.5653188f, 3.3439538f, 1.6908358f);
        this.camEV.setRotate(-23.098045f, -50.799145f, 0.0f);
        this.camEV.setFov(17.92001f);
        this.camEV.change();
    }

    void EV_Camera2() {
        System.println("戦闘前 カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.599758f, 2.3757617f, -9.276701f);
        this.camEV.setRotate(-4.3504815f, -169.28952f, 0.0f);
        this.camEV.setFov(30.399681f);
        this.camEV.change();
    }

    void EV_Camera3() {
        System.println("メインモニター前 カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.338374f, 3.3438845f, 1.630205f);
        this.camEV.setRotate(-30.597033f, -35.758896f, 0.0f);
        this.camEV.setFov(24.640013f);
        this.camEV.change();
    }

    void EV_Camera4() {
        System.println("フォロー・カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.4173265f, 6.447854f, 5.2402964f);
        this.camEV.setRotate(3.2817392f, -141.7581f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        System.println("Final_Init !!!!!");
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 11: {
                if (n2 == 1 && !this.MARY_STOP) {
                    System.println("Mary挙動");
                    this.MARY_STOP = true;
                    this.talkFlag = 1;
                    break;
                }
                if (n2 != 2 || this.MARY_ARRIVAL) break;
                System.println("Mary待機");
                this.MARY_ARRIVAL = true;
                this.talkFlag = 2;
                this.Mary.kickEnepc(7, 0);
                this.Mary.kickEnepc(4, 1);
                this.Mary.kickEnepc(9, 100);
                this.Mary.enableDTKFlag(4);
                break;
            }
            case 100: {
                if (n2 == 0 && this.CF_EVENT_FINISH == 0 && !this.EnterCheck) {
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    System.sleep(2);
                    System.println("Mary --- FREEZE_STOP_ALGO");
                    this.Mary.kickEnepc(4, 1);
                    if (this.MARY_ARRIVAL) {
                        this.Mary.kickEnepc(9, -1);
                    }
                    Runtime.enable(65536);
                    Runtime.disable(524288);
                    this.EV_Camera3();
                    if (this.MARY_ARRIVAL) {
                        this.Mary.getRotate();
                        this.Mary.ry += 3.6E7f;
                        if (this.Mary.ry % 360.0f > 180.0f) {
                            System.println("180 up");
                            this.Mary.moveEnepc(17, 10.0f, 1.0f, 12);
                        } else {
                            System.println("180 down");
                            this.Mary.moveEnepc(17, 10.0f, -1.0f, 12);
                        }
                        System.sleep(13);
                        this.Mary.kickEnepc(0, this.MARY_EXT2);
                    }
                    if (this.DefaultMenu(this.Q1) == 0) {
                        System.sleep(10);
                        if (!this.MARY_ARRIVAL) {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.J6, 0);
                            this.JR_FLAG_1 = true;
                            this.MARY_ARRIVAL = true;
                            this.player.mtn(6, 1, 1.0f, true);
                            System.waitFor(this.win);
                        } else {
                            this.player.mtn(11, 1, 1.0f, true);
                        }
                        int n3 = 0;
                        while (n3 <= 85) {
                            if (n3 == 0) {
                                this.Fade.call(0);
                                this.Teiten_Move(0.0f, -1000.0f, 0.0f);
                            } else if (n3 == 84) {
                                this.player.setTranslate(0.15f, 0.0f, -2.01f);
                                this.Mary.setTranslate(-0.5f, 0.0f, -2.01f);
                                this.player.setRotate(0.0f, 0.0f, 0.0f);
                                this.Mary.setRotate(0.0f, 0.0f, 0.0f);
                                this.Mary.kickEnepc(9, -1);
                                this.Mary.kickEnepc(7, 0);
                                this.Mary.setShadow(3, 16);
                            } else if (n3 == 85) {
                                int n4 = 0;
                                while (n4 < 40) {
                                    this.Fade_long.call(0);
                                    System.sleep(1);
                                    ++n4;
                                }
                            }
                            System.sleep(1);
                            ++n3;
                        }
                        this.CF_EVENT();
                        Runtime.enable(524288);
                        this.FIN_FLAG = true;
                        Runtime.setDefocus(0, 1, this.DefaultScreenParam);
                        this.EnterCheck = false;
                        Sound.effectStop(196745);
                        this.Boss.kickEnepc(4, 0);
                        this.Boss.kickEnepc(14, 1, 0);
                        break;
                    }
                    System.println("ボス戦には入らない");
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    Runtime.enable(524288);
                    System.println("Mary --- FREEZE_FREE");
                    this.Mary.kickEnepc(4, 0);
                    if (!this.MARY_ARRIVAL) break;
                    this.Mary.kickEnepc(9, 100);
                    break;
                }
                if (n2 != 3 || this.CF_EVENT_FINISH != 0 || this.EnterCheck) break;
                this.EnterCheck = true;
                Runtime.setPlayerControl(false);
                System.sleep(2);
                Runtime.enable(65536);
                this.player.getTranslate();
                if (this.n_count < 3) {
                    if (this.talkFlag != 2) {
                        this.DefaultTalk(this.D1);
                    } else {
                        this.DefaultTalk(this.D2);
                    }
                    ++this.n_count;
                } else {
                    this.DefaultTalk(this.D3);
                }
                this.player.rotY(5, 180.0f, true);
                System.sleep(5);
                this.player.mtn(2, 0, 10, 10, 8, 1.0f, true);
                this.player.move(10, this.player.px, this.player.pz - 0.5f, true);
                System.sleep(10);
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                break;
            }
        }
    }

    void Light1() {
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, -0.25f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        System.println("NPC1_TALK1");
        switch (this.talkFlag) {
            case 0: {
                window.print(this.TALK1, 0);
                ST1030.waitPage(window, 64);
                break;
            }
            case 1: {
                window.print(this.TALK2, 0);
                ST1030.waitPage(window, 64);
                break;
            }
            case 2: {
                window.print(this.TALK3, 0);
                ST1030.waitPage(window, 64);
                this.Mary.kickEnepc(9, 100);
                break;
            }
            default: {
                window.print(this.TALK3, 0);
                ST1030.waitPage(window, 64);
                this.Mary.kickEnepc(9, 100);
            }
        }
    }

    void Teiten_Move(float f, float f2, float f3) {
        this.teiten1.setTranslate(-4.5f + f, 0.0f + f2, 2.5f + f3);
        this.teiten2.setTranslate(4.5f + f, 0.0f + f2, 2.5f + f3);
        this.teiten3.setTranslate(-4.5f + f, 0.0f + f2, -4.5f + f3);
        this.teiten4.setTranslate(4.5f + f, 0.0f + f2, -4.5f + f3);
        this.teiten5.setTranslate(-4.0f + f, 0.0f + f2, -8.0f + f3);
        this.teiten6.setTranslate(4.0f + f, 0.0f + f2, -8.0f + f3);
        this.teiten7.setTranslate(0.0f + f, 0.0f + f2, -1.5f + f3);
        this.teiten8.setTranslate(0.0f + f, 1.0f + f2, -7.5f + f3);
        this.teiten9.setTranslate(0.0f + f, 1.5f + f2, -10.0f + f3);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1050, 7);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.Fade = new Effect(0);
        this.Fade.args[0] = -268435456;
        this.Fade.args[1] = 85;
        this.Fade.args[2] = 0;
        this.Fade_long = new Effect(0);
        this.Fade_long.args[0] = -268435456;
        this.Fade_long.args[1] = 1;
        this.Fade_long.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.monitorA = new Unit();
        this.monitorA.init(24613, 0.0f, 1.721f, -0.787f, 180.0f);
        this.monitorA.setArgs(0, 0.0f, 0.0f, 1.055f, 0.557f);
        this.monitorA.setArgs(1, 20051, 0, 106, 56);
        this.monitorA.setArgs(2, 55, 0, 15, -1);
        this.monitorA.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitorA.setVisible(true);
        this.monitorA.signal(1);
        this.monitorB = new Unit();
        this.monitorB.init(24613, 0.0f, 1.721f, -0.787f, 180.0f);
        this.monitorB.setArgs(0, 0.0f, 0.0f, 1.055f, 0.557f);
        this.monitorB.setArgs(1, 20041, 0, 106, 56);
        this.monitorB.setArgs(2, 55, 0, 15, -1);
        this.monitorB.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitorB.signal(0);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(3, 1, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(3, 2, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(3, 3, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        this.teiten1 = new Uwamono(28690, -4.5f, 0.0f, 2.5f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, 4.5f, 0.0f, 2.5f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -4.5f, 0.0f, -4.5f, 0.0f);
        this.teiten3.SetBgm(196609);
        this.teiten4 = new Uwamono(28690, 4.5f, 0.0f, -4.5f, 0.0f);
        this.teiten4.SetBgm(196609);
        this.teiten5 = new Uwamono(28690, -4.0f, 0.0f, -8.0f, 0.0f);
        this.teiten5.SetBgm(196609);
        this.teiten6 = new Uwamono(28690, 4.0f, 0.0f, -8.0f, 0.0f);
        this.teiten6.SetBgm(196609);
        this.teiten7 = new Uwamono(28690, 0.0f, 0.0f, -1.5f, 0.0f);
        this.teiten7.SetBgm(196610);
        this.teiten8 = new Uwamono(28690, 0.0f, 1.0f, -7.25f, 0.0f);
        this.teiten8.SetBgm(196611);
        this.teiten9 = new Uwamono(28690, 0.0f, 1.5f, -10.0f, 0.0f);
        this.teiten9.SetBgm(196611);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.Obj600_A = new Effect(1411, 0);
        this.Obj600_A.disp(true);
        this.doorA = new Uwamono(220, 42, '\u0002');
        new Uwamono(221, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetDoorRange(2.0f);
        this.doorA.SetDoorSpd(39);
        if (this.CF_EVENT_FINISH == 0) {
            this.player.setID(2);
            this.doorA.SetDoorSpd(1);
            this.doorA.DoorOpen();
            this.doorA.SetDoorSpd(39);
            this.Obj600_B = new Effect(1437, 0);
            this.Obj600_B.disp(false);
            this.Obj601 = new Effect(1441, 1);
            this.Obj601.disp(true);
            this.B1 = new Effect(1453, 0.0f, 8.05f, 13.55f, 0.0f);
            this.B1.disp(false);
            this.B2 = new Effect(1612, 0.5f, 0.0f, 8.0f, 0.0f);
            this.B2.setScale(1.25f, 1.25f, 1.25f);
            this.B2.disp(false);
            System.println("ボスの初期化");
            this.Boss = new Enepc();
            this.Boss.init(17413, 3, 0.0f, 4.375f, 14.0f, 180.0f);
            this.Boss.id = 1;
            this.Boss.setGroup(0, 0, 0, 0);
            this.Boss.setShadow(4, 16);
            this.Boss.setParams(0, 0, 1, 3);
            this.Boss.setInvalidID(0);
            this.Boss.kickEnepc(0, 0);
            this.Boss.kickEnepc(4, 2);
            this.Boss.setBatEvent(10);
            this.Boss.dispRadar(false);
            this.Boss.setTP(0);
            float[] fArray = new float[45];
            fArray[0] = -0.3f;
            fArray[2] = 11.0f;
            fArray[3] = -0.3f;
            fArray[5] = 11.0f;
            fArray[6] = -0.3f;
            fArray[8] = 8.0f;
            fArray[9] = -0.8f;
            fArray[11] = 6.0f;
            fArray[12] = -2.0f;
            fArray[14] = 4.5f;
            fArray[15] = -4.0f;
            fArray[17] = 4.0f;
            fArray[18] = -2.0f;
            fArray[20] = 3.75f;
            fArray[21] = -1.0f;
            fArray[23] = 3.0f;
            fArray[24] = -2.0f;
            fArray[26] = 1.5f;
            fArray[27] = -3.0f;
            fArray[29] = 1.35f;
            fArray[30] = -4.0f;
            fArray[32] = 1.15f;
            fArray[33] = -4.0f;
            fArray[35] = -3.0f;
            fArray[36] = -2.0f;
            fArray[38] = -3.0f;
            fArray[39] = -0.5f;
            fArray[41] = -3.0f;
            fArray[42] = -0.5f;
            fArray[44] = -2.01f;
            float[] fArray2 = fArray;
            float[] fArray3 = new float[8];
            fArray3[0] = -0.3f;
            fArray3[2] = 11.0f;
            fArray3[3] = 1.0f;
            fArray3[4] = -0.4f;
            fArray3[6] = 9.0f;
            fArray3[7] = -1.0f;
            float[] fArray4 = fArray3;
            System.println("メリィの初期化");
            this.Mary = new NPC_NORMAL(288, 11, 0, 1, 5, -0.3f, 0.0f, 11.0f, 180.0f, fArray4);
            this.Mary.setParams(fArray2);
            this.Mary.talkto("Talk_npc1");
            this.Mary.kickEnepc(4, 1);
            this.Mary.enableDTKFlag(262144);
            System.println("ダミーの初期化");
            this.dummy = new NPC_NORMAL(288, 12, 0, 1, 5, 0.0f, 0.0f, -2.0f, 180.0f);
            this.dummy.kickEnepc(4, 1);
            this.dummy.setVisible(false);
            this.dummy.setInvalidID(1);
            this.dummy.disableDTKFlag(65536);
        }
        if (this.EVENT_CALL == 0) {
            this.separator = new Mapunits();
            this.separator.mapUnit(58);
            this.sound = new Mapunits();
            this.sound.mapUnit(58);
            this.boss_oto = new Mapunits();
            this.boss_oto.mapUnit(58);
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            this.player.setTranslate(0.3f, 0.0f, 14.0f);
            this.Mary.setTranslate(-0.3f, 0.0f, 14.075f);
            System.println("EVENT_CALL");
            this.separator.start(1, "EVENT_CALL");
        }
        System.println("初期化終了");
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

        void Boss_Move() {
            System.println("Boss_Move");
            int n = 40;
            int n2 = 200;
            ST1030.this.Boss_Flag = true;
            block0:
            while (ST1030.this.Boss_Flag) {
                int n3 = 0;
                while (n3 < 65535) {
                    if (n3 % n == 0) {
                        Sound.effectPlay(196748);
                    }
                    if (n3 == n2) {
                        if (n3 == 200) {
                            System.println("t == 200");
                        }
                        if (!ST1030.this.Boss_Flag) {
                            Sound.effectStop(196748);
                            ST1030.this.Boss.kickEnepc(4, 0);
                            ST1030.this.Boss.kickEnepc(4, 2);
                            System.sleep(20);
                            ST1030.this.B1.setTranslate(0.0f, 4.0f, 7.55f);
                            ST1030.this.B1.disp(true);
                            System.sleep(3);
                            ST1030.this.B1.disp(false);
                            ST1030.this.Boss_Look = true;
                            continue block0;
                        }
                        n3 = 0;
                    }
                    System.sleep(1);
                    ++n3;
                }
            }
        }

        void EVENT_CALL() {
            System.println("EVENT_CALL_START");
            ST1030.this.CF_EVENT1();
            System.println("EVENT_CALL END");
            System.sleep(100);
        }

        void Screen() {
            System.println("画面の点滅開始");
            ST1030.this.SCREEN_FLAG = true;
            int n = 0;
            while (ST1030.this.SCREEN_FLAG) {
                if (ST1030.this.FIN_FLAG) {
                    Runtime.setDefocus(0, 1, ST1030.this.DefaultScreenParam);
                    ST1030.this.SCREEN_FLAG = false;
                    break;
                }
                if (n % 30 == 0) {
                    n = 0;
                    Runtime.setDefocus(0, 1, ST1030.this.RedScreenParam2);
                } else if (n % 15 == 0) {
                    Runtime.setDefocus(0, 1, ST1030.this.RedScreenParam1);
                }
                ++n;
                System.sleep(1);
            }
        }

        void Sound_Move() {
            System.println("Sound_Move");
            float f = 10.0f;
            float f2 = 0.25f;
            float f3 = 0.0f;
            while (f3 <= f) {
                ST1030.this.Teiten_Move(0.0f, 0.0f, f3);
                System.sleep(1);
                f3 += f2;
            }
        }

        void Sound_Reverse() {
            System.println("Sound_Reverse");
            float f = 10.0f;
            float f2 = 0.05f;
            float f3 = 0.0f;
            while (f3 <= f) {
                ST1030.this.Teiten_Move(0.0f, 0.0f, f - f3);
                System.sleep(1);
                f3 += f2;
            }
        }

        void Sound_ReverseY() {
            System.println("Sound_ReverseY");
            float f = 6.0f;
            float f2 = 0.025f;
            float f3 = 0.0f;
            while (f3 <= f) {
                ST1030.this.Teiten_Move(0.0f, -f + f3, 0.0f);
                System.sleep(1);
                f3 += f2;
            }
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(4, 16);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }
}

