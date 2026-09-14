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
import xeno.map.MC_VOK10B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0381
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK10B_PRJ {
    Player player;
    Camera cam1;
    Camera camEV;
    Menu menu;
    Window win;
    int entrance;
    int count = 0;
    int selected = 0;
    int selected1 = 0;
    int selected2 = 0;
    int selected3 = 0;
    int selected4 = 0;
    int mapno;
    int enemy;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int beforeA = 0;
    int dummy1talked = 0;
    int dummy2talked = 0;
    int pass1 = 0;
    int button_flg = 0;
    int button1_flg = 0;
    int button2_flg = 0;
    int Aopen = 0;
    int AL01;
    int Okkake = 0;
    int Kemusi_1 = 0;
    int Kemusi_2 = 0;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Unit[] unit;
    Unit crane01;
    Unit monitor1;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono T1;
    Uwamono T2;
    Uwamono T3;
    Uwamono itembox;
    Uwamono itemsymbol;
    Uwamono pole1;
    Uwamono pole2;
    Uwamono Bcar;
    Uwamono Kow1;
    Uwamono Kow2;
    Uwamono obj01;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono Base1;
    Uwamono Base2;
    Uwamono Base3;
    Uwamono Base4;
    Uwamono Base5;
    Uwamono COL;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Chr dummy1;
    Chr dummy2;
    Effect FadeIn01;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect EF09;
    Effect EF10;
    Effect EF11;
    Effect EF12;
    Effect EF13;
    Effect EF14;
    Effect EF15;
    Effect EF16;
    Effect EF17;
    Effect EF18;
    Effect EF19;
    Effect fade;
    Light light = new Light(0);
    int page;
    String[] Dummy_3 = new String[]{"The airlock is operating in safety mode.", "/[waitkey(64)]/[close()]"};
    String[] npc_2_0 = new String[]{"/[label(Sgt. Swaine)]", "Y-yo...well if it isn't the lady from Vector. I r-really liked this line of work. But looks like...it's over for me...", "/[waitkey(1)]/[clear()]", "*Cough* Ugh...o-oh, yeah, take this key...the red d-door across from me...there's a storage room only I know about.", "/[waitkey(1)]/[clear()]", "I-inside you'll find something I've squirreled away. I don't think...I'm gonna need it where I'm going...so I want you to take it.", "/[waitkey(64)]/[close()]"};
    String[] npc_2_2 = new String[]{"/[label(Sgt. Swaine)]", "I-I liked...being a soldier...Even though...our job is...\n", "our job is...to die...", "/[waitkey(64)]/[close()]"};
    String[] npc_2_3 = new String[]{"/[label(Sgt. Swaine)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] npc_3_0 = new String[]{"O-oh, you're t-the one we played tag with! L-listen, there's a Connection Gear Plug-in in the A.G.W.S. hangar! You can fire small amounts of destructive agents with\nit using the Connection Gear's transmission function! If we destroy the car blocking our way, we can escape! C-can you get it for me?! H-hurry, I-I'll hold them off!", "/[waitkey(64)]/[close()]"};
    String[] npc_3_1 = new String[]{"H-hurry! I'll secure your retreat...", "/[waitkey(64)]/[close()]"};
    String[] npc_3_2 = new String[]{"He's dead.", "/[waitkey(64)]/[close()]"};
    String[] shion_0 = new String[]{"/[label(Shion)]", "S-Sergeant! Sergeant Swaine!", "/[waitkey(64)]/[close()]"};
    String[] shion_1 = new String[]{"/[label(Shion)]", "Sergeant! Hang in there, Sergeant Swaine!", "/[waitkey(64)]/[close()]"};
    String[] shion_2 = new String[]{"/[label(Shion)]", "Sergeant...Sergeant Swaine...", "/[waitkey(64)]/[close()]"};
    String[] shion_3 = new String[]{"/[label(Shion)]", "I can't believe they've infiltrated this far. I wonder if everyone is okay?", "/[waitkey(64)]/[close()]"};
    String[] SHION_04 = new String[]{"/[label(Shion)]", "Hmmm, this switch controls the airlock...", "/[waitkey(64)]/[close()]"};
    String[] SHION_05 = new String[]{"/[label(Shion)]", "Well, I might as well push it...", "/[waitkey(64)]/[close()]"};
    String[] SHION_06 = new String[]{"/[label(Shion)]", "I don't know what might happen, so I better leave it alone.", "/[waitkey(64)]/[close()]"};
    String[] SHION_07 = new String[]{"/[label(Shion)]", "If I charge straight in, I will definitely die...", "/[waitkey(64)]/[close()]"};
    String[] SHION_08 = new String[]{"/[label(Shion)]", "Isn't there another way...?", "/[waitkey(64)]/[close()]"};
    String[] KARI = new String[]{"/[label(Shion)]", "Oh how horrible...", "/[waitkey(1)]/[clear()]", "I've got to find another route.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 7.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 7, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_0 = new String[]{"...Hey, what the heck are those things? They managed with one blow, to wreck a container that even the Vaporizer Plug-in couldn't destroy.", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_1 = new String[]{"But you know, I saw them get caught in the electromagnetic net.", "/[waitkey(64)]/[close()]"};
    String[] Dennoji_2 = new String[]{"Just leave me alone. There's no one left to fight alongside me now.", "/[waitkey(64)]/[close()]"};
    String[] AIR_00 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};

    ST0381() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 12.353f, 3.999f, -24.416f, 90.0f, 12.353f, 1.631f, -24.416f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -12.666f;
        fArray2[2] = 77.099f;
        fArray2[4] = 90.0f;
        fArray2[5] = -12.666f;
        fArray2[6] = 77.099f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.353f, 1.631f, -24.416f);
        this.camEV.setRotate(-12.666f, 77.099f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.434f, 3.703f, -31.745f);
        this.camEV.setRotate(-16.854f, 115.278f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.598f, 1.823f, -23.162f);
        this.camEV.setRotate(-7.08f, 68.939f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.24f, 9.143f, -17.61f);
        this.camEV.setRotate(-35.514f, 23.677f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        float[] fArray = new float[]{1.0f, -14.723f, 2.776f, 14.241f, 120.0f, -11.511f, 2.776f, 13.601f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 0.814f;
        fArray2[2] = 11.259f;
        fArray2[4] = 120.0f;
        fArray2[5] = 0.814f;
        fArray2[6] = 11.259f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 120);
        this.camEV.rotateSPL(fArray3, 1, 2, 120);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.791f, 1.239f, -24.403f);
        this.camEV.setRotate(-0.874f, 388.377f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera_1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.24f, 4.21f, -30.82f);
        this.camEV.setRotate(-39.22f, 302.99f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera_2() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.09f, 0.759f, -25.097f);
        this.camEV.setRotate(2.36f, 360.0f, 0.0f);
        this.camEV.setFov(34.99f);
        this.camEV.change();
    }

    void EV_Camera_3() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-14.027f, 3.127f, 13.115f);
        this.camEV.setRotate(-14.377f, 0.0f, 0.0f);
        this.camEV.setFov(34.99f);
        this.camEV.change();
    }

    void EV_Camera_4() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.080346f, 4.7593136f, -23.510815f);
        this.camEV.setRotate(-20.020483f, 427.5341f, 0.0f);
        this.camEV.setFov(34.9996f);
        this.camEV.change();
    }

    void EV_Camera_5() {
        float[] fArray = new float[]{1.0f, 5.0f, 8.0f, 27.35f, 60.0f, 10.0f, 8.0f, 27.35f};
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 0, 60);
        this.camEV.setRotate(-43.427f, 0.0f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
        System.sleep(60);
    }

    void Final_init(int n) {
        switch (n) {
            case 2: {
                this.enemy2.kickEnepc(10, 50, 0);
                break;
            }
            case 3: {
                this.enemy3.kickEnepc(10, 50, 0);
                break;
            }
            case 4: {
                this.enemy4.kickEnepc(10, 75, 0);
                break;
            }
            case 5: {
                this.enemy5.kickEnepc(10, 75, 0);
                break;
            }
        }
    }

    void Kakuheki_Close() {
        int n = 40;
        this.doorE.DoorClose();
        System.sleep(n);
    }

    void Kakuheki_Open() {
        int n = 40;
        this.doorE.DoorOpen();
        System.sleep(n);
    }

    public void KickEvent(int n, int n2) {
        this.AL01 = Runtime.getFlags(3004, 1);
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (this.button_flg == 1) {
                        return;
                    }
                    this.button_flg = 1;
                    switch (this.AL01) {
                        case 0: {
                            Runtime.setPlayerControl(false);
                            this.cam0.setMode(-1);
                            Runtime.enable(65536);
                            this.player.move(1, -5.0f, -31.0f, true);
                            this.player.mtn(28, 9, 1.0f, true);
                            this.player.rotY(1, 180.0f, true);
                            this.EV_Camera02();
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.AIR_00, 0);
                            System.waitFor(this.win);
                            Runtime.setPlayerControl(false);
                            this.menu = Menu.create();
                            this.menu.addItem("Yes\nNo");
                            System.waitFor(this.menu);
                            this.selected = this.menu.getSelected();
                            switch (this.selected) {
                                case 0: {
                                    this.EV_Camera04();
                                    System.sleep(30);
                                    Runtime.enable(65536);
                                    this.player.mtn(12, 1, 1.0f, true);
                                    System.sleep(30);
                                    Sound.effectPlay(196743);
                                    Sound.streamPlay(196031, 48000);
                                    this.EF01.disp(false);
                                    System.sleep(414);
                                    Sound.effectPlay(196744);
                                    this.EV_Camera_4();
                                    this.Kakuheki_Open();
                                    System.sleep(30);
                                    this.EF19.disp(true);
                                    this.crane01.start(1, "ido0");
                                    this.enemy1.kickEnepc(4, 1);
                                    this.enemy1.kickEnepc(1, 3);
                                    System.sleep(60);
                                    Runtime.setFlags(3004, 1, 1);
                                    break;
                                }
                                case 1: {
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    this.cam0.setMode(0);
                                    break;
                                }
                                default: {
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    this.cam0.setMode(0);
                                }
                            }
                            this.button_flg = 0;
                            return;
                        }
                        case 1: {
                            this.cam0.setMode(-1);
                            this.EV_Camera_1();
                            Runtime.setPlayerControl(false);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.Dummy_3, 0);
                            System.waitFor(this.win);
                            Runtime.setPlayerControl(true);
                            this.cam0.setMode(0);
                            this.button_flg = 0;
                            return;
                        }
                        default: {
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3061, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    Runtime.enable(65536);
                    this.player.mtn(27, 1, 1.0f, true);
                    Stage.setVisible(4, true);
                    Stage.setVisible(0, true);
                    Stage.setVisible(1, true);
                    Stage.setVisible(2, true);
                    Stage.setVisible(3, true);
                    Stage.setVisible(5, true);
                    this.EV_Camera05();
                    this.enemy4.kickEnepc(4, 1);
                    this.enemy5.kickEnepc(4, 1);
                    this.enemy4.kickEnepc(1, 1);
                    this.enemy5.kickEnepc(1, 1);
                    this.enemy4.moveEnepc(15, -14.0f, 6.5f, 120);
                    this.enemy5.moveEnepc(15, -14.0f, 8.5f, 120);
                    System.sleep(120);
                    this.enemy4.kickEnepc(4, 0);
                    this.enemy5.kickEnepc(4, 0);
                    this.enemy4.kickEnepc(7, 9);
                    this.enemy5.kickEnepc(7, 9);
                    Runtime.setFlags(3061, 1, 1);
                    this.cam0.setMode(0);
                    Stage.setVisible(4, false);
                    Stage.setVisible(0, false);
                    Stage.setVisible(1, false);
                    Stage.setVisible(2, false);
                    Stage.setVisible(3, false);
                    Stage.setVisible(5, false);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3207, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3207, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                        return;
                    } else if (Runtime.getFlags(3227, 1) == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                        return;
                    } else {
                        if (Runtime.getFlags(3287, 1) != 0) return;
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(56);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_03, 0);
                        System.waitFor(this.win);
                        this.doorF.SetDoorType('\u0004');
                        Runtime.setFlags(3287, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button_flg = 0;
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.enemy6.kickEnepc(4, 1);
                    this.enemy8.kickEnepc(4, 1);
                    this.enemy6.kickEnepc(1, 27);
                    this.enemy8.kickEnepc(1, 27);
                    this.EF15.disp(true);
                    this.EF16.disp(true);
                    this.EF17.disp(true);
                    this.EF15.setForceLoop(true);
                    this.EF16.setForceLoop(true);
                    this.EF17.setForceLoop(true);
                    this.cam0.setMode(-1);
                    this.EV_Camera_5();
                    System.sleep(60);
                    Sound.effectPlay(196755);
                    System.sleep(60);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.KARI, 0);
                    System.waitFor(this.win);
                    this.cam0.setMode(0);
                    this.enemy6.kickEnepc(4, 0);
                    this.enemy6.kickEnepc(4, 0);
                    this.enemy6.kickEnepc(4, 0);
                    Runtime.enable(65536);
                    this.player.mtn(6, 1, 1.0f, true);
                    this.player.setTranslate(3.877f, 2.0f, 21.825f);
                    this.player.setRotate(0.0f, 270.0f, 0.0f);
                    System.sleep(10);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.EF15.setForceLoop(false);
                    this.EF16.setForceLoop(false);
                    this.EF17.setForceLoop(false);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 6) return;
        if (n2 == 7) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3064, 1) == 0) {
                        System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                        this.EF18.disp(false);
                        this.enemy2.kickEnepc(4, 2);
                        this.enemy3.kickEnepc(4, 2);
                        this.enemy4.setES_mGiveUp(0.0f);
                        this.enemy4.setES_cRange(0.0f, 0.0f, 0.0f);
                        this.enemy5.setES_mGiveUp(0.0f);
                        this.enemy5.setES_cRange(0.0f, 0.0f, 0.0f);
                        Runtime.setFlags(3064, 1, 1);
                        return;
                    } else {
                        System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                        this.EF18.disp(true);
                        this.enemy2.kickEnepc(4, 0);
                        this.enemy3.kickEnepc(4, 0);
                        this.enemy4.setES_mGiveUp(20.0f);
                        this.enemy4.setES_cRange(360.0f, 20.0f, 15.0f);
                        this.enemy5.setES_mGiveUp(20.0f);
                        this.enemy5.setES_cRange(360.0f, 20.0f, 15.0f);
                        Runtime.setFlags(3064, 1, 0);
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 8) return;
        if (n2 == 9) {
            switch (n) {
                case 100: {
                    if (this.Kemusi_1 != 0) return;
                    this.Kemusi_1 = 1;
                    System.println("gunoyose_lgunoyose_lgunoyose_lgunoyose_lgunoyose_l");
                    this.enemy2.kickEnepc(7, 17);
                    this.enemy2.kickEnepc(4, 3);
                    this.enemy2.mtn(2, 9, 1.0f, true);
                    this.enemy2.move(45, -36.0f, -25.5f, true);
                    System.sleep(50);
                    this.enemy2.mtn(28, 9, 1.0f, true);
                    System.sleep(5);
                    this.enemy2.getTranslate();
                    this.EF02.setTranslate(this.enemy2.px, this.enemy2.py, this.enemy2.pz);
                    this.EF02.disp(true);
                    this.teiten5 = new Uwamono(28690, -36.0f, 0.0f, -26.22f, 0.0f);
                    this.teiten5.SetBgm(196746);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 10) {
            switch (n) {
                case 100: {
                    if (this.Kemusi_2 != 0) return;
                    this.Kemusi_2 = 1;
                    System.println("gunoyose_rgunoyose_rgunoyose_rgunoyose_rgunoyose_r");
                    this.enemy3.kickEnepc(7, 17);
                    this.enemy3.kickEnepc(4, 3);
                    this.enemy3.mtn(2, 9, 1.0f, true);
                    this.enemy3.move(45, -32.5f, -22.5f, true);
                    System.sleep(50);
                    this.enemy3.mtn(28, 9, 1.0f, true);
                    System.sleep(5);
                    this.enemy3.getTranslate();
                    this.EF03.setTranslate(this.enemy3.px, this.enemy3.py, this.enemy3.pz);
                    this.EF03.disp(true);
                    this.teiten6 = new Uwamono(28690, -32.2f, 0.0f, -23.0f, 0.0f);
                    this.teiten6.SetBgm(196746);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 11) {
            switch (n) {
                case 4: {
                    if (Runtime.getFlags(3065, 1) == 0) {
                        System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy4.setES_mGiveUp(0.0f);
                        this.enemy4.setES_cRange(0.0f, 0.0f, 0.0f);
                        Runtime.setFlags(3065, 1, 1);
                        return;
                    } else {
                        System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy4.setES_mGiveUp(20.0f);
                        this.enemy4.setES_cRange(360.0f, 20.0f, 15.0f);
                        Runtime.setFlags(3065, 1, 0);
                    }
                    return;
                }
                case 5: {
                    if (Runtime.getFlags(3066, 1) == 0) {
                        System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy5.setES_mGiveUp(0.0f);
                        this.enemy5.setES_cRange(0.0f, 0.0f, 0.0f);
                        Runtime.setFlags(3066, 1, 1);
                        return;
                    } else {
                        System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy5.setES_mGiveUp(20.0f);
                        this.enemy5.setES_cRange(360.0f, 20.0f, 15.0f);
                        Runtime.setFlags(3066, 1, 0);
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 12) {
            switch (n) {
                case 4: {
                    if (Runtime.getFlags(3065, 1) == 0) {
                        System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy4.setES_mGiveUp(0.0f);
                        this.enemy4.setES_cRange(0.0f, 0.0f, 0.0f);
                        Runtime.setFlags(3065, 1, 1);
                        return;
                    } else {
                        System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy4.setES_mGiveUp(20.0f);
                        this.enemy4.setES_cRange(360.0f, 20.0f, 15.0f);
                        Runtime.setFlags(3065, 1, 0);
                    }
                    return;
                }
                case 5: {
                    if (Runtime.getFlags(3066, 1) == 0) {
                        System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy5.setES_mGiveUp(0.0f);
                        this.enemy5.setES_cRange(0.0f, 0.0f, 0.0f);
                        Runtime.setFlags(3066, 1, 1);
                        return;
                    } else {
                        System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                        this.enemy5.setES_mGiveUp(20.0f);
                        this.enemy5.setES_cRange(360.0f, 20.0f, 15.0f);
                        Runtime.setFlags(3066, 1, 0);
                    }
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 != 13) return;
        switch (n) {
            case 100: {
                System.println("###############################################");
                if (Runtime.getFlags(3014, 2) != 0) return;
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                System.sleep(5);
                this.player.mtn(4, 9, 1.0f, true);
                this.player.move(90, -6.313f, 21.962f, true);
                System.sleep(95);
                this.player.mtn(10, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.shion_0, 0);
                System.waitFor(this.win);
                Runtime.disable(65536);
                this.npc1.kickEnepc(4, 1);
                this.npc1.kickEnepc(9, 100);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.npc_2_0, 0);
                System.waitFor(this.win);
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.shion_1, 0);
                System.waitFor(this.win);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.npc_2_2, 0);
                System.waitFor(this.win);
                Runtime.setFlags(3014, 2, 2);
                this.npc1.kickEnepc(9, -1);
                this.EF11.disp(true);
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 22);
                Runtime.setFlags(3227, 1, 1);
                this.npc1.kickEnepc(4, 2);
                this.npc1.disableDTKFlag(4);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkNPC1(Enepc enepc) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.npc_2_3, 0);
        ST0381.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.shion_2, 0);
        ST0381.waitPage(this.win, 64);
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getFlags(3001, 2) == 0) {
            System.println("000");
            this.off();
            window.print(this.npc_3_0, 0);
            Runtime.setFlags(3001, 2, 1);
            ST0381.waitPage(window, 64);
            this.on();
        } else if (Runtime.getFlags(3001, 2) == 1) {
            System.println("111");
            this.off();
            window.print(this.npc_3_1, 0);
            ST0381.waitPage(window, 64);
            this.on();
        } else if (Runtime.getFlags(3001, 2) == 2) {
            System.println("222");
            this.off();
            window.print(this.npc_3_2, 0);
            ST0381.waitPage(window, 64);
            this.on();
        } else if (Runtime.getFlags(3001, 2) == 3) {
            System.println("333");
            this.off();
            window.print(this.npc_3_2, 0);
            ST0381.waitPage(window, 64);
            this.on();
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("----------*-*-*-*-");
                Runtime.setFlags(3044, 1, 1);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.setFlags(3061, 1, 0);
                Runtime.jumpCF(65926, 1);
                break;
            }
            case 1: {
                Runtime.setFlags(3061, 1, 0);
                Runtime.jumpCF(65949, 1);
                break;
            }
            case 2: {
                Runtime.setFlags(3061, 1, 0);
                Runtime.jumpCF(65949, 3);
                break;
            }
            case 3: {
                Runtime.setFlags(3061, 1, 0);
                if (Runtime.getFlags(41, 1) == 0) {
                    Runtime.setFlags(41, 1, 1);
                    Runtime.jumpEvent(1330);
                    break;
                }
                Runtime.jumpCF(65887, 1);
                break;
            }
            case 4: {
                Runtime.setFlags(3051, 1, 1);
                Runtime.jumpCF(65736, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        this.AL01 = Runtime.getFlags(3004, 1);
        if (this.AL01 == 0) {
            this.EF01 = new Effect(1010, 0);
            this.EF01.disp(true);
        } else {
            this.EF01 = new Effect(1010, 0);
            this.EF01.disp(false);
        }
        this.EF02 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF02.disp(false);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1401, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF03.disp(false);
        this.EF03.setClip(true);
        if (Runtime.getFlags(3014, 2) != 2) {
            this.EF11 = new Effect(1528, -5.447f, 2.0f, 23.337f, 180.0f);
            this.EF11.disp(false);
            this.EF11.setClip(true);
        }
        if (Runtime.getFlags(3014, 2) == 2) {
            this.EF19 = new Effect(1528, -5.447f, 2.0f, 23.337f, 180.0f);
            this.EF19.disp(true);
            this.EF19.setClip(true);
            Runtime.progressEffect(120);
        }
        this.EF12 = new Effect(1527, 9.55f, 2.0f, 21.182f, 0.0f);
        this.EF12.setScale(2.0f, 1.0f, 1.3f);
        this.EF12.disp(true);
        this.EF12.setClip(true);
        this.EF13 = new Effect(1528, 9.914f, 2.0f, 21.948f, 0.0f);
        this.EF13.setScale(1.6f, 1.3f, 2.5f);
        this.EF13.disp(true);
        this.EF13.setClip(true);
        this.EF15 = new Effect(1448, 8.793f, 2.3f, 21.948f, 0.0f);
        this.EF15.setScale(0.6f, 2.0f, 0.3f);
        this.EF15.disp(false);
        this.EF15.setForceLoop(false);
        this.EF15.setClip(true);
        this.EF16 = new Effect(1448, 9.9069f, 2.3f, 22.396f, 0.0f);
        this.EF16.setScale(0.3f, 1.7f, 1.3f);
        this.EF16.disp(false);
        this.EF16.setForceLoop(false);
        this.EF16.setClip(true);
        this.EF17 = new Effect(1448, 10.458f, 2.3f, 22.021f, 0.0f);
        this.EF17.setScale(1.0f, 1.3f, 0.7f);
        this.EF17.disp(false);
        this.EF17.setForceLoop(false);
        this.EF17.setClip(true);
        if (Runtime.getFlags(3001, 2) == 2) {
            this.EF18 = new Effect(1528, -20.5f, 0.0f, -13.5f, 135.0f);
            this.EF18.disp(true);
            this.EF18.setClip(true);
        } else {
            this.EF18 = new Effect(1528, -20.5f, 0.0f, -13.5f, 135.0f);
            this.EF18.disp(false);
            this.EF18.setClip(true);
        }
        this.EF19 = new Effect(1678, 0.0f, -1.0f, -29.5f, 0.0f);
        this.EF19.disp(false);
        this.EF19.setClip(true);
        this.FadeIn01 = new Effect(0);
        this.FadeIn01.args[0] = Integer.MIN_VALUE;
        this.FadeIn01.args[1] = 60;
        this.FadeIn01.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.teiten1 = new Uwamono(28690, 9.5f, 0.0f, 21.5f, 0.0f);
        this.teiten1.SetBgm(196747);
        this.teiten3 = new Uwamono(28690, -36.0f, 0.0f, -26.22f, 0.0f);
        this.teiten3.SetBgm(196745);
        this.teiten4 = new Uwamono(28690, -32.2f, 0.0f, -23.0f, 0.0f);
        this.teiten4.SetBgm(196745);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 1, 0.82f, 0.49f, 0.12f);
        Runtime.setIdLightCol(1, 2, 0.77f, 0.42f, 0.24f);
        Runtime.setIdLightCol(1, 3, 0.74f, 0.42f, 0.26f);
        Runtime.setIdLightCol(2, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(2, 1, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(2, 2, 0.27f, 0.92f, 0.49f);
        Runtime.setIdLightCol(2, 3, 0.33f, 0.85f, 0.59f);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, -14.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 335.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, -36.8183f, 12.3756f, -13.09755f, 42.35f, -43.4036f, -13.3105f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (this.AL01 == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16385, 9, -3.0f, 0.0f, -26.5f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 3, 3);
            float[] fArray3 = new float[8];
            fArray3[0] = -3.0f;
            fArray3[2] = -26.5f;
            fArray3[3] = 1.0f;
            fArray3[4] = -2.3f;
            fArray3[6] = -25.0f;
            fArray3[7] = -1.0f;
            fArray2 = fArray3;
            this.enemy1.setParams(1, 0, 1, 9, fArray2);
            this.enemy1.kickEnepc(10, 70, 0);
            this.enemy1.setBatEvent(13);
            this.enemy1.setTP(300);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16386, 11, -36.722f, 0.0f, -23.789f, 180.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(1, 1, 1, 1);
            float[] fArray4 = new float[8];
            fArray4[0] = -36.722f;
            fArray4[2] = -23.789f;
            fArray4[3] = 1.0f;
            fArray4[4] = -35.428f;
            fArray4[6] = -23.62f;
            fArray4[7] = -1.0f;
            fArray2 = fArray4;
            this.enemy2.setParams(2, 15, 2, 11, fArray2);
            float[] fArray5 = new float[12];
            fArray5[0] = -36.722f;
            fArray5[2] = -23.789f;
            fArray5[3] = -35.428f;
            fArray5[5] = -23.62f;
            fArray5[6] = -35.808f;
            fArray5[8] = -22.537f;
            fArray5[9] = -37.031f;
            fArray5[11] = -22.706f;
            fArray = fArray5;
            this.enemy2.setParams(fArray);
            this.enemy2.enableDTKFlag(262144);
            this.enemy2.setBatEvent(13);
            this.enemy2.setTP(300);
            this.enemy2.setShadow(0, 0);
            this.enemy2.setInvalidID(1);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16386, 11, -36.722f, 0.0f, -23.789f, 180.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(6, 6, 7, 8);
            float[] fArray6 = new float[8];
            fArray6[0] = -36.722f;
            fArray6[2] = -23.789f;
            fArray6[3] = 1.0f;
            fArray6[4] = -35.428f;
            fArray6[6] = -23.62f;
            fArray6[7] = -1.0f;
            fArray2 = fArray6;
            this.enemy2.setParams(2, 15, 2, 11, fArray2);
            float[] fArray7 = new float[12];
            fArray7[0] = -36.722f;
            fArray7[2] = -23.789f;
            fArray7[3] = -35.428f;
            fArray7[5] = -23.62f;
            fArray7[6] = -35.808f;
            fArray7[8] = -22.537f;
            fArray7[9] = -37.031f;
            fArray7[11] = -22.706f;
            fArray = fArray7;
            this.enemy2.setShadow(0, 0);
            this.enemy2.setParams(fArray);
            this.enemy2.enableDTKFlag(262144);
            this.enemy2.setInvalidID(1);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy3 = new Enepc();
            this.enemy3.init(16386, 11, -33.951f, 0.0f, -21.581f, 135.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(1, 1, 1, 1);
            float[] fArray8 = new float[8];
            fArray8[0] = -33.951f;
            fArray8[2] = -21.581f;
            fArray8[3] = 1.0f;
            fArray8[4] = -34.458f;
            fArray8[6] = -20.821f;
            fArray8[7] = -1.0f;
            fArray2 = fArray8;
            this.enemy3.setParams(2, 15, 3, 11, fArray2);
            float[] fArray9 = new float[12];
            fArray9[0] = -33.951f;
            fArray9[2] = -21.581f;
            fArray9[3] = -34.458f;
            fArray9[5] = -20.821f;
            fArray9[6] = -33.487f;
            fArray9[8] = -20.175f;
            fArray9[9] = -32.911f;
            fArray9[11] = -21.046f;
            fArray = fArray9;
            this.enemy3.setParams(fArray);
            this.enemy3.enableDTKFlag(262144);
            this.enemy3.setBatEvent(13);
            this.enemy3.setTP(300);
            this.enemy3.setShadow(0, 0);
            this.enemy3.setInvalidID(1);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy3 = new Enepc();
            this.enemy3.init(16386, 11, -33.951f, 0.0f, -21.581f, 135.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(6, 6, 7, 8);
            float[] fArray10 = new float[8];
            fArray10[0] = -33.951f;
            fArray10[2] = -21.581f;
            fArray10[3] = 1.0f;
            fArray10[4] = -34.458f;
            fArray10[6] = -20.821f;
            fArray10[7] = -1.0f;
            fArray2 = fArray10;
            this.enemy3.setParams(2, 15, 3, 11, fArray2);
            float[] fArray11 = new float[12];
            fArray11[0] = -33.951f;
            fArray11[2] = -21.581f;
            fArray11[3] = -34.458f;
            fArray11[5] = -20.821f;
            fArray11[6] = -33.487f;
            fArray11[8] = -20.175f;
            fArray11[9] = -32.911f;
            fArray11[11] = -21.046f;
            fArray = fArray11;
            this.enemy3.setShadow(0, 0);
            this.enemy3.setParams(fArray);
            this.enemy3.enableDTKFlag(262144);
            this.enemy3.setInvalidID(1);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 9, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(3, 3, 3, 0);
            float[] fArray12 = new float[24];
            fArray12[0] = -14.0f;
            fArray12[1] = 2.0f;
            fArray12[2] = 2.0f;
            fArray12[3] = 1.0f;
            fArray12[4] = -14.0f;
            fArray12[6] = -16.0f;
            fArray12[7] = 2.0f;
            fArray12[8] = -19.0f;
            fArray12[10] = -16.0f;
            fArray12[11] = 3.0f;
            fArray12[12] = -19.0f;
            fArray12[14] = -26.0f;
            fArray12[15] = 4.0f;
            fArray12[16] = -9.0f;
            fArray12[18] = -26.0f;
            fArray12[19] = 5.0f;
            fArray12[20] = -9.0f;
            fArray12[22] = -16.0f;
            fArray12[23] = -1.0f;
            fArray2 = fArray12;
            this.enemy4.setParams(0, 2, 4, 9, fArray2);
            this.enemy4.setLocation(5, 0);
            this.enemy4.setBatEvent(13);
            this.enemy4.setTP(300);
            this.enemy4.enableDTKFlag(262144);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 9, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(4, 4, 4, 5);
            float[] fArray13 = new float[24];
            fArray13[0] = -14.0f;
            fArray13[1] = 2.0f;
            fArray13[2] = 2.0f;
            fArray13[3] = 1.0f;
            fArray13[4] = -14.0f;
            fArray13[6] = -16.0f;
            fArray13[7] = 2.0f;
            fArray13[8] = -19.0f;
            fArray13[10] = -16.0f;
            fArray13[11] = 3.0f;
            fArray13[12] = -19.0f;
            fArray13[14] = -26.0f;
            fArray13[15] = 4.0f;
            fArray13[16] = -9.0f;
            fArray13[18] = -26.0f;
            fArray13[19] = 5.0f;
            fArray13[20] = -9.0f;
            fArray13[22] = -16.0f;
            fArray13[23] = -1.0f;
            fArray2 = fArray13;
            this.enemy4.setParams(0, 2, 4, 9, fArray2);
            this.enemy4.setLocation(5, 0);
            this.enemy4.enableDTKFlag(262144);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy5 = new Enepc();
            this.enemy5.init(16385, 9, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy5.id = 5;
            this.enemy5.setGroup(3, 3, 3, 0);
            float[] fArray14 = new float[24];
            fArray14[0] = -14.0f;
            fArray14[1] = 2.0f;
            fArray14[2] = 4.0f;
            fArray14[3] = 1.0f;
            fArray14[4] = -14.0f;
            fArray14[6] = -16.0f;
            fArray14[7] = 2.0f;
            fArray14[8] = -19.0f;
            fArray14[10] = -16.0f;
            fArray14[11] = 3.0f;
            fArray14[12] = -19.0f;
            fArray14[14] = -26.0f;
            fArray14[15] = 4.0f;
            fArray14[16] = -9.0f;
            fArray14[18] = -26.0f;
            fArray14[19] = 5.0f;
            fArray14[20] = -9.0f;
            fArray14[22] = -16.0f;
            fArray14[23] = -1.0f;
            fArray2 = fArray14;
            this.enemy5.setParams(0, 2, 5, 9, fArray2);
            this.enemy5.setLocation(5, 1);
            this.enemy5.setBatEvent(13);
            this.enemy5.setTP(300);
            this.enemy5.enableDTKFlag(262144);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy5 = new Enepc();
            this.enemy5.init(16385, 9, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy5.id = 5;
            this.enemy5.setGroup(4, 4, 4, 5);
            float[] fArray15 = new float[24];
            fArray15[0] = -14.0f;
            fArray15[1] = 2.0f;
            fArray15[2] = 4.0f;
            fArray15[3] = 1.0f;
            fArray15[4] = -14.0f;
            fArray15[6] = -16.0f;
            fArray15[7] = 2.0f;
            fArray15[8] = -19.0f;
            fArray15[10] = -16.0f;
            fArray15[11] = 3.0f;
            fArray15[12] = -19.0f;
            fArray15[14] = -26.0f;
            fArray15[15] = 4.0f;
            fArray15[16] = -9.0f;
            fArray15[18] = -26.0f;
            fArray15[19] = 5.0f;
            fArray15[20] = -9.0f;
            fArray15[22] = -16.0f;
            fArray15[23] = -1.0f;
            fArray2 = fArray15;
            this.enemy5.setParams(0, 2, 5, 9, fArray2);
            this.enemy5.setLocation(5, 1);
            this.enemy5.enableDTKFlag(262144);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy6 = new Enepc();
            this.enemy6.init(20227, 13, 8.172f, 2.0f, 21.217f, 52.0f);
            this.enemy6.id = 6;
            float[] fArray16 = new float[3];
            fArray16[0] = 8.172f;
            fArray16[2] = 21.217f;
            fArray2 = fArray16;
            this.enemy6.setParams(0, 14, 6, 13, fArray2);
            this.enemy6.setMotion(0, 3);
            this.enemy6.setBatEvent(13);
            this.enemy6.setTP(300);
            this.enemy6.setInvalidID(1);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy6 = new Enepc();
            this.enemy6.init(20227, 13, 8.172f, 2.0f, 21.217f, 52.0f);
            this.enemy6.id = 6;
            float[] fArray17 = new float[3];
            fArray17[0] = 8.172f;
            fArray17[2] = 21.217f;
            fArray2 = fArray17;
            this.enemy6.setParams(0, 14, 6, 13, fArray2);
            this.enemy6.setMotion(0, 3);
            this.enemy6.setInvalidID(1);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy7 = new Enepc();
            this.enemy7.init(20227, 13, 9.19f, 2.0f, 23.035f, 135.0f);
            this.enemy7.id = 7;
            float[] fArray18 = new float[3];
            fArray18[0] = 9.19f;
            fArray18[2] = 23.035f;
            fArray2 = fArray18;
            this.enemy7.setParams(0, 14, 7, 13, fArray2);
            this.enemy7.setMotion(0, 3);
            this.enemy7.setBatEvent(13);
            this.enemy7.setTP(300);
            this.enemy7.setInvalidID(1);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy7 = new Enepc();
            this.enemy7.init(20227, 13, 9.19f, 2.0f, 23.035f, 135.0f);
            this.enemy7.id = 7;
            float[] fArray19 = new float[3];
            fArray19[0] = 9.19f;
            fArray19[2] = 23.035f;
            fArray2 = fArray19;
            this.enemy7.setParams(0, 14, 7, 13, fArray2);
            this.enemy7.setMotion(0, 3);
            this.enemy7.setInvalidID(1);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy8 = new Enepc();
            this.enemy8.init(20227, 13, 11.087f, 2.0f, 22.7465f, 225.0f);
            this.enemy8.id = 8;
            float[] fArray20 = new float[3];
            fArray20[0] = 11.087f;
            fArray20[2] = 22.7465f;
            fArray2 = fArray20;
            this.enemy8.setParams(0, 14, 8, 13, fArray2);
            this.enemy8.setMotion(0, 3);
            this.enemy8.setBatEvent(13);
            this.enemy8.setTP(300);
            this.enemy8.setInvalidID(1);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy8 = new Enepc();
            this.enemy8.init(20227, 13, 11.087f, 2.0f, 22.7465f, 225.0f);
            this.enemy8.id = 8;
            float[] fArray21 = new float[3];
            fArray21[0] = 11.087f;
            fArray21[2] = 22.7465f;
            fArray2 = fArray21;
            this.enemy8.setParams(0, 14, 8, 13, fArray2);
            this.enemy8.setMotion(0, 3);
            this.enemy8.setInvalidID(1);
        }
        if (Runtime.getFlags(3014, 2) == 2) {
            this.npc1 = new NPC_NORMAL(519, 11, 0, 8, 7, -5.447f, 2.0f, 23.337f, 180.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.setMotion(0, 5);
            this.npc1.setShadow(0, 0);
        } else {
            this.npc1 = new NPC_NORMAL(519, 11, 0, 8, 7, -5.447f, 2.0f, 23.337f, 180.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(8);
            this.npc1.disableDTKFlag(131072);
            this.npc1.disableDTKFlag(1);
            this.npc1.disableDTKFlag(2);
            this.npc1.enableDTKFlag(4);
            this.npc1.setMotion(0, 5);
            this.npc1.setShadow(0, 0);
        }
        if (Runtime.getFlags(3001, 2) == 2) {
            System.println("読んだ？");
            this.npc2 = new NPC_NORMAL(525, 12, 0, 8, 7, -20.5f, 0.0f, -13.5f, 135.0f);
            this.npc2.talkto("TalkNPC2");
            this.npc2.disableDTKFlag(8);
            this.npc2.disableDTKFlag(131072);
            this.npc2.disableDTKFlag(1);
            this.npc2.disableDTKFlag(2);
            this.npc2.setMotion(0, 5);
            this.npc2.setInvalidID(1);
            this.npc2.setShadow(0, 0);
        } else {
            System.println("読んだ？");
            this.npc2 = new NPC_NORMAL(525, 12, 0, 8, 15, -20.5f, 0.0f, -13.5f, 135.0f);
            this.npc2.talkto("TalkNPC2");
            this.npc2.disableDTKFlag(8);
            this.npc2.disableDTKFlag(131072);
            this.npc2.disableDTKFlag(1);
            this.npc2.disableDTKFlag(2);
            this.npc2.enableDTKFlag(4);
            this.npc2.setMotion(0, 1);
            this.npc2.setInvalidID(1);
            this.npc2.setShadow(0, 0);
        }
        this.npc3 = new Enepc();
        this.npc3.init(519, 7, 9.55f, 2.0f, 21.182f, 40.0f);
        this.npc3.id = 13;
        fArray2 = new float[]{11.087f, 2.0f, 22.7465f};
        this.npc3.setParams(0, 3, 13, 7, fArray2);
        this.npc3.setMotion(0, 3);
        this.npc3.setShadow(0, 0);
        this.npc4 = new Enepc();
        this.npc4.init(523, 7, 9.914f, 2.0f, 21.948f, 200.0f);
        this.npc4.id = 14;
        fArray = new float[]{11.087f, 2.0f, 22.7465f};
        this.npc4.setParams(0, 3, 14, 7, fArray);
        this.npc4.setMotion(0, 3);
        this.npc4.setShadow(0, 0);
        if (Runtime.getFlags(3032, 1) == 0) {
            this.npc5 = new NPC_NORMAL(519, 15, 0, 8, 4, 100.0f, 100.0f, 100.0f, 0.0f);
            this.npc5.talkto("TalkNPC5");
            this.npc5.disableDTKFlag(131072);
            this.npc5.disableDTKFlag(8);
            this.npc5.start(1, "LookA");
            this.npc5.setInvalidID(1);
        }
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(44, 42, '\u0001');
        new Uwamono(45, 42, '\u0001', this.doorA);
        this.doorA.DoorClose();
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(48, 42, '\u0001');
        new Uwamono(49, 42, '\u0001', this.doorB);
        if (Runtime.getFlags(3007, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
        } else {
            this.doorB.SetDoorType('\u0002');
        }
        this.doorC = new Uwamono(52, 42, '\u0001');
        new Uwamono(53, 42, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(56, 42, '\u0001');
        new Uwamono(57, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(148, 40, '\u0001');
        new Uwamono(147, 40, '\u0002', this.doorE);
        this.doorE.SetDoorType('\u0002');
        this.doorE.SetDiffSize(0.0f, -2.0f, 0.0f);
        this.doorE.SetDoorRange(3.0f);
        this.doorE.DoorClose();
        if (Runtime.getFlags(3287, 1) == 0) {
            this.doorF = new Uwamono(39, 40, '\u0004');
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF = new Uwamono(39, 40, '\u0004');
            this.doorF.SetDoorType('\u0004');
        }
        this.Base1 = new Uwamono(28672, -20.0f, 1.0f, 18.5f, 0.0f);
        this.Base1.SetSize(3.0f, 1.0f, 3.0f);
        this.item1 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 15);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 16);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 17);
        this.item4 = new Uwamono(28672, -14.031f, 0.0f, -8.135f, 0.0f, 18);
        this.item5 = new Uwamono(28681, 0.0f, 0.0f, 0.0f, 0.0f, 19);
        new Uwamono(91, 0, this.item1);
        new Uwamono(158, 84, this.item2);
        this.obj01 = new Uwamono(157, 6, this.item3);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        new Uwamono(89, 0);
        new Uwamono(90, 0, this.item4);
        new Uwamono(159, 84);
        this.Kow2 = new Uwamono(97, 0, this.item5);
        this.Kow2.SetSize(1.0f, 1.0f, 3.0f);
        this.Kow2.SetBrokenEnemy(true);
        this.Kow2.SetBroken(false);
        this.Kow1 = new Uwamono(152, 4);
        this.Kow1.SetBrokenEnemy(true);
        if (Runtime.getFlags(3044, 1) == 0) {
            this.Bcar = new Uwamono(156, 18);
            this.Bcar.SetCallNo(1);
        } else {
            Stage.setVisible(156, false);
        }
        this.COL = new Uwamono(28672, -4.5f, 0.0f, -30.0f, 0.0f);
        this.COL.SetSize(17.0f, 2.0f, 2.0f);
        this.itembox = new Uwamono(28677, -34.098f, 0.0f, -23.771f, 135.0f, 30);
        this.itembox.SetSymbol(28683);
        this.crane01 = new Mapunits();
        this.crane01.mapUnit(75);
        this.crane01.start(4, null);
        if (this.AL01 == 0) {
            this.T1 = new Uwamono(153, 9);
            this.T1.SetBroken(false);
            this.T2 = new Uwamono(154, 9);
            this.T2.SetBroken(false);
            this.T3 = new Uwamono(155, 9);
            this.T3.SetBroken(false);
        } else {
            Stage.setVisible(153, false);
            Stage.setVisible(154, false);
            Stage.setVisible(155, false);
        }
        this.enemy2.kickEnepc(19, 1, 0, 220, 1);
        this.enemy2.kickEnepc(19, 2, 0, 220, 1);
        if (Runtime.getFlags(3001, 2) == 2) {
            Runtime.setShootFlag(true);
        } else {
            Runtime.setShootFlag(false);
        }
        this.monitor1 = new Object();
        this.monitor1.init(24613, 4.0f, 3.7f, 19.253f, 0.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18011, 0, 256, 130);
        this.monitor1.setArgs(2, 100, 0, 5, -6);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.signal(1);
        Stage.setVisible(74, false);
    }

    void off() {
        if (Runtime.getFlags(3004, 1) == 0) {
            this.enemy1.kickEnepc(4, 1);
        }
        this.enemy4.kickEnepc(4, 1);
        this.enemy5.kickEnepc(4, 1);
        this.enemy6.kickEnepc(4, 1);
        this.enemy7.kickEnepc(4, 1);
        this.enemy8.kickEnepc(4, 1);
        if (Runtime.getFlags(3004, 1) == 0) {
            this.enemy1.kickEnepc(1, 27);
        }
        this.enemy4.kickEnepc(1, 27);
        this.enemy5.kickEnepc(1, 27);
        this.enemy6.kickEnepc(1, 27);
        this.enemy7.kickEnepc(1, 27);
        this.enemy8.kickEnepc(1, 27);
    }

    void on() {
        if (Runtime.getFlags(3004, 1) == 0) {
            this.enemy1.kickEnepc(4, 0);
        }
        this.enemy4.kickEnepc(4, 0);
        this.enemy5.kickEnepc(4, 0);
        this.enemy6.kickEnepc(4, 0);
        this.enemy7.kickEnepc(4, 0);
        this.enemy8.kickEnepc(4, 0);
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

        void LookA() {
            Runtime.setPlayerControl(false);
            ST0381.this.cam0.setMode(-1);
            ST0381.this.EV_Camera00();
            System.sleep(90);
            ST0381.this.EV_Camera01();
            Runtime.enable(65536);
            ST0381.this.player.mtn(28, 1, 1.0f, true);
            ST0381.this.win = Window.create();
            ST0381.this.win.setSize(4, 45);
            ST0381.this.win.setLocation(15, 305);
            ST0381.this.win.print(ST0381.this.shion_3, 0);
            System.waitFor(ST0381.this.win);
            System.sleep(30);
            Runtime.setFlags(3032, 1, 1);
            ST0381.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void ido0() {
            int n = 0;
            ST0381.this.enemy1.getTranslate();
            float f = ST0381.this.enemy1.px;
            float f2 = (f - 0.0f) / 82.0f;
            float f3 = ST0381.this.enemy1.py;
            float f4 = (f3 - 0.0f) / 82.0f;
            float f5 = ST0381.this.enemy1.pz;
            float f6 = (f5 - -28.5f) / 82.0f;
            float f7 = (f5 - -35.0f) / 120.0f;
            ST0381.this.T1.getTranslate();
            float f8 = ST0381.this.T1.px;
            float f9 = (f8 - 0.0219939f) / 44.0f;
            float f10 = ST0381.this.T1.pz;
            float f11 = (f10 - -28.34985f) / 44.0f;
            ST0381.this.T2.getTranslate();
            float f12 = ST0381.this.T2.px;
            float f13 = (f12 - 0.0219939f) / 50.0f;
            float f14 = ST0381.this.T2.pz;
            float f15 = (f14 - -28.34985f) / 50.0f;
            ST0381.this.T3.getTranslate();
            float f16 = ST0381.this.T3.px;
            float f17 = (f16 - 0.0219939f) / 68.0f;
            float f18 = ST0381.this.T3.pz;
            float f19 = (f18 - -28.34985f) / 68.0f;
            while (true) {
                if (n >= 0 && n < 82) {
                    ST0381.this.enemy1.getTranslate();
                    ST0381.this.enemy1.setTranslate(ST0381.this.enemy1.px - f2, ST0381.this.enemy1.py, ST0381.this.enemy1.pz - f6);
                }
                if (n >= 8 && n < 52) {
                    ST0381.this.T1.getTranslate();
                    ST0381.this.T1.setTranslate(ST0381.this.T1.px - f9, ST0381.this.T1.py + 0.02f, ST0381.this.T1.pz - f11);
                    ST0381.this.T1.getRotate();
                    ST0381.this.T1.setRotate(ST0381.this.T1.rx, ST0381.this.T1.ry - 5.0f, ST0381.this.T1.rz);
                }
                if (n >= 10 && n < 60) {
                    ST0381.this.T2.getTranslate();
                    ST0381.this.T2.setTranslate(ST0381.this.T2.px - f13, ST0381.this.T2.py + 0.02f, ST0381.this.T2.pz - f15);
                    ST0381.this.T2.getRotate();
                    ST0381.this.T2.setRotate(ST0381.this.T2.rx - 5.0f, ST0381.this.T2.ry - 5.0f, ST0381.this.T2.rz - 5.0f);
                }
                if (n >= 14 && n < 82) {
                    ST0381.this.T3.getTranslate();
                    ST0381.this.T3.setTranslate(ST0381.this.T3.px - f17, ST0381.this.T3.py + 0.02f, ST0381.this.T3.pz - f19);
                    ST0381.this.T3.getRotate();
                    ST0381.this.T3.setRotate(ST0381.this.T3.rx - 5.0f, ST0381.this.T3.ry - 5.0f, ST0381.this.T3.rz - 5.0f);
                }
                if (n == 52) {
                    ST0381.this.T1.SendSignal();
                }
                if (n == 60) {
                    ST0381.this.T2.SendSignal();
                }
                if (n == 82) {
                    ST0381.this.T3.SendSignal();
                }
                if (n >= 82 && n < 111) {
                    ST0381.this.EV_Camera06();
                    ST0381.this.enemy1.kickEnepc(4, 3);
                    ST0381.this.enemy1.mtn(2, 9, 1.0f, true);
                    ST0381.this.enemy1.rotY(25, 0.0f, true);
                }
                if (n >= 112 && n < 231) {
                    ST0381.this.enemy1.mtn(28, 9, 1.0f, true);
                    ST0381.this.enemy1.setTP(300 - 2 * n);
                }
                if (n == 232) break;
                ++n;
                System.sleep(1);
            }
            ST0381.this.Kakuheki_Close();
            ST0381.this.enemy1.kickEnepc(7, 4);
            ST0381.this.enemy1.setVisible(false);
            ST0381.this.enemy1.dispRadar(false);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
            ST0381.this.cam0.setMode(0);
            ST0381.this.EF19.disp(false);
        }
    }

    class Object
            extends Unit {
        Object() {
        }
    }
}

