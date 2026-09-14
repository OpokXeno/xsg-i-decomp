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
import xeno.map.MC_VOK12B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0413
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK12B_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
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
    int npc9talked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono Glass;
    Uwamono kow1;
    int passed1 = 0;
    int passed2 = 0;
    int passed3 = 0;
    int passed4 = 0;
    int button_flg = 0;
    int button2_flg = 0;
    int button3_flg = 0;
    Uwamono itembox;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono Base1;
    Uwamono SAVE;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono lc;
    int Loc_flg = 0;
    int test = 0;
    int guno4pass = 0;
    int stop = 0;
    Unit monitor1;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    int smd = 0;
    int finalflg;
    int page;
    int E4flg;
    String[] Npc_1_0 = new String[]{"Damn it!! I could have saved her if only I were stronger.", "/[waitkey(64)]/[close()]"};
    String[] Npc_2 = new String[]{"Dead.", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_0 = new String[]{"!!\n", "Oh, you're the one from Vector.", "/[waitkey(1)]/[clear()]", "By the way, do you know...?", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_1 = new String[]{"Damn!! Why do scientists act like know-it-alls when they actually don't know anything!", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_2 = new String[]{"Hey?! You're panicking aren't you?!", "/[waitkey(1)]/[clear()]", "You should take the time to listen to me BECAUSE it's an emergency!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_3 = new String[]{"Listen closely to what people have to say!! Everything will work out no matter what if you obey that rule!", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_3 = new String[]{"The ship monitor controls are broken.", "/[waitkey(64)]/[close()]"};
    String[] Sion_00 = new String[]{"/[label(Shion)]", "This is no time to be backtracking.", "/[waitkey(64)]/[close()]"};
    String[] Sion_01 = new String[]{"/[label(Shion)]", "I should be able to find someone if I keep going this way.", "/[waitkey(64)]/[close()]"};
    String[] Sion_13 = new String[]{"/[label(Shion)]", "No response. Was the bridge attacked too?!", "/[waitkey(64)]/[close()]"};
    String[] Sion_14 = new String[]{"/[label(Shion)]", "The bridge is sealed off?! ...This can't be good.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 16, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0413() {
    }

    void EOB(int n) {
        System.println("EOB****************************************************");
        if (n == 5) {
            System.println("5");
            Runtime.setFlags(3193, 1, 1);
        }
    }

    void EOB_Always(int n) {
        Runtime.progressEffect(120);
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 23.956f, 3.731f, -6.846f, 90.0f, 23.956f, 2.675f, -6.846f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -15.841f;
        fArray2[2] = 33.979f;
        fArray2[4] = 90.0f;
        fArray2[5] = -15.841f;
        fArray2[6] = 33.979f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 5: {
                if (this.finalflg == 1) {
                    return;
                }
                this.enemy5.kickEnepc(10, 50, 0);
                System.println("pass3");
                this.finalflg = 1;
                break;
            }
        }
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorD.DoorClose();
        System.sleep(n);
    }

    void Kakuheki_Open() {
        int n = 10;
        this.doorD.DoorOpen();
        System.sleep(n);
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3018, 1) != 0) return;
                    if (this.stop == 0) {
                        this.cam0.setMode(-1);
                        this.player.getTranslate();
                        this.player.setTranslate(this.player.px - 0.3f, this.player.py, this.player.pz);
                        this.enemy4.kickEnepc(4, 1);
                        this.enemy4.kickEnepc(1, 27);
                        this.EV_Camera00();
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.mtn(28, 1, 1.0f, true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_00, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.enemy4.kickEnepc(4, 0);
                        this.stop = 1;
                        this.cam0.setMode(0);
                        return;
                    }
                    this.cam0.setMode(-1);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px - 0.3f, this.player.py, this.player.pz);
                    this.enemy4.kickEnepc(4, 1);
                    this.enemy4.kickEnepc(1, 27);
                    this.EV_Camera00();
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_01, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.enemy4.kickEnepc(4, 0);
                    this.stop = 0;
                    this.cam0.setMode(0);
                    return;
                }
            }
            return;
        }
        if (n2 == 2) {
            return;
        }
        if (n2 == 3) {
            return;
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF03.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                    this.player.setRotate(0.0f, 180.0f, 0.0f);
                    this.player.mtn(12, 1, 1.0f, true);
                    System.sleep(50);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    this.EF03.disp(true);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.npc1talked = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    if (this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF02.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                    this.player.setRotate(0.0f, 180.0f, 0.0f);
                    this.player.mtn(12, 1, 1.0f, true);
                    System.sleep(50);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    this.EF02.disp(true);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.npc1talked = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 6) {
            switch (n) {
                case 100: {
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Loc_6_3, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 7) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3216, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3216, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3236, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3296, 1) != 0) return;
                    if (this.button2_flg == 1) {
                        return;
                    }
                    this.button2_flg = 1;
                    this.off();
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(56);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_03, 0);
                    System.waitFor(this.win);
                    this.doorF.SetDoorType('\u0004');
                    Runtime.setFlags(3296, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.on();
                    this.button2_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 != 16) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3018, 1) == 0) {
                    if (this.button3_flg == 1) {
                        return;
                    }
                    this.button3_flg = 1;
                    this.off();
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_14, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.on();
                    this.button3_flg = 0;
                    return;
                }
                if (this.button3_flg == 1) {
                    return;
                }
                this.button3_flg = 1;
                this.off();
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Sion_13, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
                this.on();
                this.button3_flg = 0;
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        this.off();
        window.print(this.Npc_1_0, 0);
        ST0413.waitPage(window, 64);
        this.on();
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        this.off();
        window.print(this.Npc_2, 0);
        ST0413.waitPage(window, 64);
        this.on();
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        this.off();
        window.print(this.Npc_2, 0);
        ST0413.waitPage(window, 64);
        this.on();
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("----------*-*-*-*-");
                Runtime.setFlags(3194, 1, 1);
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
                if (Runtime.getFlags(3007, 1) == 1) {
                    Runtime.jumpCF(65918, 2);
                    break;
                }
                Runtime.jumpCF(65917, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(65918, 3);
                break;
            }
            case 3: {
                if (Runtime.getFlags(3007, 1) == 1) {
                    Runtime.jumpCF(65861, 1);
                    break;
                }
                Runtime.jumpCF(65857, 1);
                break;
            }
            case 4: {
                Runtime.setFlags(3048, 1, 1);
                Runtime.jumpCF(65726, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.teiten1 = new Uwamono(28690, 24.0f, 0.0f, 22.0f, 0.0f);
        this.teiten1.SetBgm(196634);
        this.teiten2 = new Uwamono(28690, 22.0f, 0.0f, -20.0f, 0.0f);
        this.teiten2.SetBgm(196634);
        this.teiten3 = new Uwamono(28690, 30.0f, 0.0f, 1.0f, 0.0f);
        this.teiten3.SetBgm(196634);
        this.teiten4 = new Uwamono(28690, 25.0f, 0.0f, 1.0f, 0.0f);
        this.teiten4.SetBgm(196635);
        this.teiten5 = new Uwamono(28690, 20.5f, 0.0f, 13.0f, 0.0f);
        this.teiten5.SetBgm(196635);
        this.teiten6 = new Uwamono(28690, 20.5f, 0.0f, -9.0f, 0.0f);
        this.teiten6.SetBgm(196635);
        this.teiten7 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten7.SetBgm(196635);
        this.teiten8 = new Uwamono(28690, -14.0f, 0.0f, 16.0f, 0.0f);
        this.teiten8.SetBgm(196642);
        this.EF02 = new Effect(1011, 1);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1011, 2);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1528, 0.5f, 0.0f, -15.52f, 142.0f);
        this.EF04.disp(true);
        this.EF04.setClip(true);
        this.EF05 = new Effect(1528, 22.61f, 0.0f, 20.21f, -45.0f);
        this.EF05.disp(true);
        this.EF05.setClip(true);
        this.EF05.setForceLoop(true);
        Stage.setVisible(-1, true);
        Stage.renderCommand(4);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(23, false);
        Stage.setVisible(24, false);
        Stage.setVisible(25, false);
        Stage.setVisible(26, false);
        Stage.setVisible(27, false);
        Stage.setVisible(28, false);
        Stage.setVisible(31, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 21.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 3.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, 10.724f, 7.143f, -21.846f, 62.91f, -69.846f, -9.719f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, 27.6133f, 5.6059f, 4.9391f, 62.358f, -60.2295f, 0.2437f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFPedestal(9, -6.92025f, 4.6719f, 28.7121f, 40.0f, -20.5988f, -30.9395f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, 9.149f, 2.5f, -20.874f, 40.0f, -11.187f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFPedestal(13, 13.524f, 8.32379f, 15.4844f, 48.0f, -88.00234f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(13, 1);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16385, 6, -15.383f, 0.0f, 19.783f, 45.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray2 = new float[8];
            fArray2[0] = -17.776f;
            fArray2[2] = 18.568f;
            fArray2[3] = 1.0f;
            fArray2[4] = -16.0f;
            fArray2[6] = 21.5f;
            fArray2[7] = -1.0f;
            fArray = fArray2;
            this.enemy1.setParams(0, 0, 1, 6, fArray);
            this.enemy1.setBatEvent(13);
            this.enemy1.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16385, 6, -12.273f, 0.0f, 26.833f, 45.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(3, 3, 3, 6);
            float[] fArray3 = new float[8];
            fArray3[0] = -17.776f;
            fArray3[2] = 18.568f;
            fArray3[3] = 1.0f;
            fArray3[4] = -16.0f;
            fArray3[6] = 21.5f;
            fArray3[7] = -1.0f;
            fArray = fArray3;
            this.enemy1.setParams(0, 25, 1, 6, fArray);
            this.enemy1.renderCommand(22);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16385, 6, -11.0f, 0.0f, 19.27f, 45.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(0, 0, 0, 0);
            float[] fArray4 = new float[28];
            fArray4[0] = -11.0f;
            fArray4[2] = 19.27f;
            fArray4[3] = 1.0f;
            fArray4[4] = -13.16f;
            fArray4[6] = 20.23f;
            fArray4[7] = 2.0f;
            fArray4[8] = -11.09f;
            fArray4[10] = 18.23f;
            fArray4[11] = 3.0f;
            fArray4[12] = -9.56f;
            fArray4[14] = 20.57f;
            fArray4[15] = 4.0f;
            fArray4[16] = -6.13f;
            fArray4[18] = 20.93f;
            fArray4[19] = 5.0f;
            fArray4[20] = -2.44f;
            fArray4[22] = 21.02f;
            fArray4[23] = 6.0f;
            fArray4[24] = 0.46f;
            fArray4[26] = 21.38f;
            fArray4[27] = -1.0f;
            fArray = fArray4;
            this.enemy2.setParams(0, 10, 2, 6, fArray);
            this.enemy2.enableDTKFlag(262144);
            this.enemy2.setBatEvent(13);
            this.enemy2.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16385, 6, -11.0f, 0.0f, 19.27f, 45.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(3, 3, 3, 6);
            float[] fArray5 = new float[28];
            fArray5[0] = -11.0f;
            fArray5[2] = 19.27f;
            fArray5[3] = 1.0f;
            fArray5[4] = -13.16f;
            fArray5[6] = 20.23f;
            fArray5[7] = 2.0f;
            fArray5[8] = -11.09f;
            fArray5[10] = 18.23f;
            fArray5[11] = 3.0f;
            fArray5[12] = -9.56f;
            fArray5[14] = 20.57f;
            fArray5[15] = 4.0f;
            fArray5[16] = -6.13f;
            fArray5[18] = 20.93f;
            fArray5[19] = 5.0f;
            fArray5[20] = -2.44f;
            fArray5[22] = 21.02f;
            fArray5[23] = 6.0f;
            fArray5[24] = 0.46f;
            fArray5[26] = 21.38f;
            fArray5[27] = -1.0f;
            fArray = fArray5;
            this.enemy2.setParams(0, 25, 2, 6, fArray);
            this.enemy2.enableDTKFlag(262144);
            this.enemy2.renderCommand(22);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy3 = new Enepc();
            this.enemy3.init(20228, 10, 21.0f, 0.0f, 0.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(2, 2, 2, 2);
            float[] fArray6 = new float[8];
            fArray6[0] = 21.0f;
            fArray6[3] = 1.0f;
            fArray6[4] = 20.0f;
            fArray6[7] = -1.0f;
            fArray = fArray6;
            this.enemy3.setParams(0, 2, 3, 10, fArray);
            this.enemy3.setBatEvent(13);
            this.enemy3.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy3 = new Enepc();
            this.enemy3.init(20228, 10, 21.0f, 0.0f, 0.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(5, 5, 7, 7);
            float[] fArray7 = new float[8];
            fArray7[0] = 21.0f;
            fArray7[3] = 1.0f;
            fArray7[4] = 20.0f;
            fArray7[7] = -1.0f;
            fArray = fArray7;
            this.enemy3.setParams(0, 2, 3, 10, fArray);
            this.enemy3.renderCommand(22);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 6, 20.5f, 0.0f, -12.5f, 12.5f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(0, 0, 0, 0);
            float[] fArray8 = new float[8];
            fArray8[0] = 20.5f;
            fArray8[2] = -12.5f;
            fArray8[3] = 1.0f;
            fArray8[4] = 19.5f;
            fArray8[6] = -12.5f;
            fArray8[7] = -1.0f;
            fArray = fArray8;
            this.enemy4.setParams(0, 12, 4, 6, fArray);
            this.enemy4.enableDTKFlag(262144);
            this.enemy4.setBatEvent(13);
            this.enemy4.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 6, 20.5f, 0.0f, -12.5f, 192.5f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(3, 3, 3, 6);
            float[] fArray9 = new float[16];
            fArray9[0] = 20.5f;
            fArray9[2] = -12.5f;
            fArray9[3] = 1.0f;
            fArray9[4] = 20.888f;
            fArray9[6] = -17.132f;
            fArray9[7] = 2.0f;
            fArray9[8] = 3.043f;
            fArray9[10] = -17.509f;
            fArray9[11] = 3.0f;
            fArray9[12] = 2.783f;
            fArray9[14] = -26.988f;
            fArray9[15] = -1.0f;
            fArray = fArray9;
            this.enemy4.setParams(0, 8, 4, 6, fArray);
            this.enemy4.enableDTKFlag(262144);
            this.enemy4.renderCommand(22);
        }
        if (Runtime.getFlags(3001, 2) == 2 && Runtime.getFlags(3193, 1) == 0) {
            if (Runtime.getFlags(3018, 1) == 0) {
                this.enemy5 = new Enepc();
                this.enemy5.init(16386, 8, 12.48f, 0.0f, -29.144f, 0.0f);
                this.enemy5.id = 5;
                this.enemy5.setGroup(1, 1, 1, 1);
                this.enemy5.setParams(1, 4, 5, 8);
                this.enemy5.setBatEvent(13);
                this.enemy5.setTP(300);
            } else if (Runtime.getFlags(3018, 1) == 1) {
                this.enemy5 = new Enepc();
                this.enemy5.init(16386, 8, 12.48f, 0.0f, -29.144f, 0.0f);
                this.enemy5.id = 5;
                this.enemy5.setGroup(4, 4, 4, 4);
                this.enemy5.setParams(1, 4, 5, 8);
                this.enemy5.renderCommand(22);
            }
        }
        this.npc1 = new NPC_NORMAL(519, 11, 0, 22, 3, 8.12f, 0.0f, -26.51f, 45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.touchto("TouchNPC1");
        this.npc1.disableDTKFlag(131082);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc1.renderCommand(22);
        this.npc2 = new NPC_NORMAL(523, 12, 0, 11, 29, 0.5f, 0.0f, -15.52f, 142.0f);
        this.npc2.talkto("TalkNPC2");
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(2);
        this.npc2.disableDTKFlag(8);
        this.npc2.disableDTKFlag(1);
        this.npc2.setMotion(0, 1);
        this.npc2.setInvalidID(1);
        this.npc2.renderCommand(22);
        this.npc2.setShadow(0, 0);
        this.npc3 = new NPC_NORMAL(519, 13, 0, 11, 4, 22.61f, 0.0f, 20.21f, -45.0f);
        this.npc3.talkto("TalkNPC3");
        this.npc3.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(2);
        this.npc3.disableDTKFlag(8);
        this.npc3.disableDTKFlag(1);
        this.npc3.setMotion(0, 1);
        this.npc3.renderCommand(22);
        this.npc3.setShadow(0, 0);
        this.npc5 = new NPC_NORMAL(1592, 15, 0, 26, 13, 19.0f, 1.8f, 0.7f, 35.0f);
        this.npc5.kickEnepc(10, 70, 0);
        this.npc5.setTP(500);
        this.npc5.setInvalidID(1);
        this.npc5.setMotion(0, 10);
        this.npc5.setShadow(0, 0);
        this.npc5.dispRadar(false);
        this.npc5.renderCommand(22);
        this.npc6 = new NPC_NORMAL(1595, 16, 0, 26, 13, 19.0f, 1.8f, 1.3f, 110.0f);
        this.npc6.kickEnepc(10, 70, 0);
        this.npc6.setTP(500);
        this.npc6.setInvalidID(1);
        this.npc6.setMotion(0, 9);
        this.npc6.setShadow(0, 0);
        this.npc6.dispRadar(false);
        this.npc6.renderCommand(22);
        this.doorA = new Uwamono(13, 42, '\u0001');
        new Uwamono(14, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(17, 42, '\u0001');
        new Uwamono(18, 42, '\u0001', this.doorB);
        if (Runtime.getFlags(3007, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
        } else {
            this.doorB.SetDoorType('\u0002');
        }
        this.doorC = new Uwamono(19, 42, '\u0001');
        new Uwamono(20, 42, '\u0002', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(119, 42, '\u0001');
        new Uwamono(118, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0002');
        this.doorD.SetDoorRange(3.0f);
        this.doorD.DoorOpen();
        this.doorD.setVisible(false);
        this.doorE = new Uwamono(165, 40, '\u0001');
        new Uwamono(164, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0002');
        if (Runtime.getFlags(3296, 1) == 0) {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0004');
        }
        this.doorG = new Uwamono(0, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.Base1 = new Uwamono(28672, 27.5f, -1.0f, 0.0f, 0.0f);
        this.Base1.SetSize(8.0f, 1.0f, 10.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 20);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 21);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 22);
        new Uwamono(170, 84);
        this.lc = new Uwamono(168, 84, this.item1);
        this.lc.SetSize(0.75f, 1.0f, 0.755f);
        new Uwamono(169, 84);
        new Uwamono(129, 31);
        new Uwamono(130, 21);
        if (Runtime.getFlags(3194, 1) == 0) {
            this.Glass = new Uwamono(162, 126);
            this.Glass.SetParticle(1761);
            this.Glass.SetCallNo(1);
        } else {
            Stage.setVisible(162, false);
        }
        new Uwamono(166, 21, this.item3);
        this.kow1 = new Uwamono(167, 6, this.item2);
        this.kow1.SetBrokenEnemy(true);
        this.kow1.SetSize(2.5f, 0.5f, 2.5f);
        this.itembox = new Uwamono(28677, 10.0f, 0.0f, -31.0f, 180.0f, 31);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        this.SAVE = new Uwamono(28678, 14.5f, 0.0f, -24.0f);
        this.trap3 = new Uwamono(28673, -17.5f, 0.0f, 18.0f, 0.0f);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 18.1f, 2.0f, 1.0f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 2.38f);
        this.monitor1.setArgs(1, 18001, 0, 256, 128);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setScale(1.2f, 0.81f, 1.0f);
        this.monitor1.signal(1);
        Stage.setVisible(127, false);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3238, 1, 1);
                break;
            }
        }
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy3.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        if (Runtime.getFlags(3001, 2) == 2 && Runtime.getFlags(3193, 1) == 0) {
            this.enemy5.kickEnepc(4, 1);
        }
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy3.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        if (Runtime.getFlags(3001, 2) == 2 && Runtime.getFlags(3193, 1) == 0) {
            this.enemy5.kickEnepc(1, 27);
        }
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        if (Runtime.getFlags(3001, 2) == 2 && Runtime.getFlags(3193, 1) == 0) {
            this.enemy5.kickEnepc(4, 0);
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
            Runtime.setShootHeightCheck(false);
            this.setPlayer();
            this.setShadow(4, 16);
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class People
            extends Enepc {
        People() {
        }

        void init() {
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

    class Object
            extends Unit {
        Object() {
        }
    }
}

